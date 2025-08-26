#include <global.h>
#include <register.h>

extern void fv_nand_erase(word iw_head_blk);
extern void fv_nand_read(void);
extern void fv_nand_n4k(byte ib_n4rd_idx, byte ib_cmd_hit);
#ifndef CPUA_PROC
extern void fv_nand_htprg(byte ib_cmd_end, dwrd id_cmd_bptr, dwrd id_cmd_padr);
#endif
extern void fv_ftl_oth(void);

extern dwrd gd_ht_token;
extern dwrd gd_gc_token;
extern dwrd gd_ht_weight;

dwrd *gd_l2p_tbl;
#ifdef FTL_DBG
dwrd *gd_p2l_tbl;
dwrd *gd_vmap_tbl;
byte *gb_crl_tbl;
#endif

dwrd gd_vcnt_tbl[RBLK_QNTY];
dwrd gd_ecnt_tbl[RBLK_QNTY];
//dwrd gd_rcnt_tbl[RBLK_QNTY];
//qwrd gq_sn_tbl[RBLK_QNTY];
DBL_TBL gs_dbl_tbl[RBLK_QNTY];
byte gb_blk_type[RBLK_QNTY];

word gw_fblk_str;
word gw_fblk_end;
word gw_fblk_num;

#ifdef GC_SIM
byte gb_htp2l_num = 0;
#endif

PHY_ADR gs_head_padr[HEAD_QNTY];

word gw_htp2l_bfp = 8;
word gw_htp2l_ofs = 0;
dwrd gd_htp2l_ladr[4];

byte gb_htwl_sel = 0;

#ifdef FTL_DBG
dwrd gd_htp2l_ptr;
#endif

#ifdef N4KA_EN
byte gb_n4rd_wptr = 254;
byte gb_n4rd_rptr = 0;
byte gb_n4rd_idxq[255];
byte gb_n4rd_strm[255];
dwrd gd_n4rd_ladr[255];
dwrd gd_n4rd_padr[255];
#endif



#ifdef N4KA_EN
byte fb_ploc_chk(dwrd id_cmd_padr)
{
    byte lb_cmd_cech;
    word lw_cmd_hblk, lw_cmd_page;

    rb_head_idx = GC_HLOC;
    ARM_NOP(); //wait 3 cycle for HW handle

    //check blk
    lw_cmd_hblk = (word)(id_cmd_padr >> (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT));
    if(lw_cmd_hblk != rw_hblk_loc)
    {
        return 0;
    }

    //check page
    lw_cmd_page = (word)(id_cmd_padr >> (BKCH_SHIFT + FRAG_SHIFT)) & PAGE_MASK;
    if(lw_cmd_page/3 != rw_page_loc/3) //if micron b47r/b58r, need change this condition
    {
        return 0;
    }

    //check cech
    lb_cmd_cech = (byte)(id_cmd_padr >> FRAG_SHIFT) & BKCH_MASK;
    if(lb_cmd_cech != rb_cech_loc)
    {
        return 0;
    }

    return 1;
}
#endif

void fv_ftl_hdl(void)
{
    byte lb_rw_cnt = RWCMD_THR;
    byte lb_p2lp_act = 0;
    byte lb_p2lp_ptr = 0;
    byte lb_raid_act = 0;
    byte lb_cmd_end;
#ifdef N4KA_EN
    byte lb_n4rd_vld;
    byte lb_sect_cnt;
    byte lb_sect_map;
    byte lb_n4rd_idx;
#endif
    word lw_cmd_blk;
    dwrd ld_buf_ptr;
    dwrd ld_cmd_padr;
#ifdef N4KA_EN
    dwrd ld_data_base;
    dwrd ld_meta_base;
    dwrd ld_data_src;
    dwrd ld_meta_src;
#endif

    //access host cmd, and handle raid_parity, bt_page, p2l_page, tbl_unit_flush
#ifdef N4KA_EN
    while((rs_host_cmd.sb_cmd_type != HCMD_INV) || rb_n4rd_vld)
#else
    while(rs_host_cmd.sb_cmd_type != HCMD_INV)
#endif
    {
#ifndef LGET_EN
        //rb_cdir_bas = (byte)rs_host_cmd.sd_cmd_lext;
        rs_host_cmd.sd_cmd_padr = gd_l2p_tbl[rs_host_cmd.sd_cmd_ladr];
#endif

#ifdef N4KA_EN
        //check non 4KB align read queue
        lb_n4rd_vld = rb_n4rd_vld;
#endif

        //nand read cmd handle
        if(rs_host_cmd.sb_cmd_type == HCMD_RD)
        {
#ifdef HWCMD_CHK
            fv_uart_print("r%x,%x,%x,%x,%x\r\n", (rs_host_cmd.sd_cmd_cid >> 7), rs_host_cmd.sd_cmd_ladr, rs_host_cmd.sb_cmd_ftl, rs_host_cmd.sb_cmd_crd, rs_host_cmd.sb_cmd_crl);
            if(rs_host_cmd.sb_cmd_ftl == 0)
            {
                rb_host_get = HCMD_INV;
                continue;
            }
#endif
            while(rw_cpua_set & CPUA_ACT);
            fv_nand_read();

            //get host cmd
            rb_host_get = HCMD_INV;
        }

        //nand write cmd handle
#ifdef N4KA_EN
        else if(lb_n4rd_vld || ((rs_host_cmd.sb_cmd_type == HCMD_WR) && ((rs_host_cmd.sb_cmd_smap == BYTE_MASK) ||
                                                                         (rs_host_cmd.sd_cmd_padr == L2P_NULL) ||
                                                                         (rs_host_cmd.sb_cmd_hit && fb_ploc_chk(rs_host_cmd.sd_cmd_padr)))))
        {
            if(lb_n4rd_vld)
            {
                rs_host_cmd.sb_cmd_mued = 1;
                rs_host_cmd.sb_cmd_strm = gb_n4rd_strm[rb_n4rd_idx];
                rs_host_cmd.sw_cmd_bfp  = rw_n4rd_bfp;
                rs_host_cmd.sd_cmd_ladr = gd_n4rd_ladr[rb_n4rd_idx];
                rs_host_cmd.sd_cmd_padr = gd_n4rd_padr[rb_n4rd_idx];
            }
            else if(rs_host_cmd.sb_cmd_smap != BYTE_MASK) //1. L2P_NULL, add dummy into data buffer, 2. gcwr hit, copy gcp data to bfp
            {
                rb_cawt_set = CAWT_ACT;

                if((rs_host_cmd.sw_cmd_bfp >> BPTR_SFT) == SMDT_BASE)
                {   
                    ld_data_base = SRAM_MEM_BASE  + ((dwrd)(rs_host_cmd.sw_cmd_bfp & BPTR_MSK) << 12);
                    ld_meta_base = SRAM_META_BASE + ((dwrd)(rs_host_cmd.sw_cmd_bfp & BPTR_MSK) << 12);
                }
                else
                {
                    ld_data_base = DRAM_DATA_BASE + ((dwrd)rs_host_cmd.sw_cmd_bfp << 12);
                    ld_meta_base = DRAM_META_BASE + ((dwrd)rs_host_cmd.sw_cmd_bfp << 8);
                }

                if((rs_host_cmd.sw_cmd_gcp >> BPTR_SFT) == SMDT_BASE)
                {   
                    ld_data_src = SRAM_MEM_BASE  + ((dwrd)(rs_host_cmd.sw_cmd_gcp & BPTR_MSK) << 12);
                    ld_meta_src = SRAM_META_BASE + ((dwrd)(rs_host_cmd.sw_cmd_gcp & BPTR_MSK) << 12);
                }
                else
                {
                    ld_data_src = DRAM_DATA_BASE + ((dwrd)rs_host_cmd.sw_cmd_gcp << 12);
                    ld_meta_src = DRAM_META_BASE + ((dwrd)rs_host_cmd.sw_cmd_gcp << 8);
                }

                while(rb_cawt_rdy == 0);
                lb_sect_map = (byte)(rd_stbl_dat(rs_host_cmd.sw_cmd_bfp) >> 1);

                for(lb_sect_cnt = 0; lb_sect_cnt < 8; lb_sect_cnt = lb_sect_cnt + 1)
                {
                    if((lb_sect_map & (1 << lb_sect_cnt)) == 0x0)
                    {
                        if(rs_host_cmd.sd_cmd_padr == L2P_NULL)
                        {
                            memset((byte *)(ld_data_base + (lb_sect_cnt << 9)), 0x0, 512);
                            *(volatile dwrd *)(ld_meta_base + (lb_sect_cnt << 3) + 0) = 0x4d558a87; //zero payload crc result
#ifdef DLNK_EN
                            *(volatile dwrd *)(ld_meta_base + (lb_sect_cnt << 3) + 4) = (rs_host_cmd.sd_cmd_ladr << (rb_dlnk_ena ? 4 : 3)) | ((rb_dlnk_ena && !rb_dlnk_mst) ? 0x8 : 0x0) | (dwrd)lb_sect_cnt;
#else
                            *(volatile dwrd *)(ld_meta_base + (lb_sect_cnt << 3) + 4) = (rs_host_cmd.sd_cmd_ladr << 3) | (dwrd)lb_sect_cnt;
#endif
                        }
                        else
                        {
                            memcpy((byte *)(ld_data_base + (lb_sect_cnt << 9)), (byte *)(ld_data_src + (lb_sect_cnt << 9)), 512);
                            memcpy((byte *)(ld_meta_base + (lb_sect_cnt << 3)), (byte *)(ld_meta_src + (lb_sect_cnt << 3)), 8);
                        }
                    }
                }

                rd_stbl_dat(rs_host_cmd.sw_cmd_bfp) = (BYTE_MASK << 1);
                rb_cawt_set = 0;
            }
#else
        else if(rs_host_cmd.sb_cmd_type == HCMD_WR)
        {
#endif

#ifdef HWCMD_CHK
#ifdef N4KA_EN
            if(lb_n4rd_vld)
            {
                fv_uart_print("nw%x,%x\r\n", rw_n4rd_bfp, rd_n4rd_idx);
            }
            else
#endif
            {
                fv_uart_print("w%x,%x,%x\r\n", (rs_host_cmd.sd_cmd_cid >> 7), rs_host_cmd.sd_cmd_ladr, rs_host_cmd.sb_cmd_mued);
                if(rs_host_cmd.sb_cmd_ftl == 0)
                {
                    rb_host_get = HCMD_INV;
                    continue;
                }
            }
#endif

            //set head blk index for p4k_jump
            rb_head_idx = rs_host_cmd.sb_cmd_strm;
            ARM_NOP(); //wait 3 cycle for HW handle

            do
            {
                lb_cmd_end = (rb_frag_loc == ((FRAG_QNTY - 1)) && (rb_gwln_loc == rb_gwln_end)) ? 1 : 0;
                ld_cmd_padr = rd_hp4k_loc;

                if(lb_p2lp_act) //p2l page
                {
                    ld_buf_ptr = (FTL_HTP2L_BASE >> 12) | (dwrd)lb_p2lp_ptr;
                    lb_p2lp_act = (lb_p2lp_ptr == 3) ? 0 : 1;
                    lb_p2lp_ptr = (lb_p2lp_ptr == 3) ? 0 : (lb_p2lp_ptr + 1);

                    rb_nddn_bit = 0xf; //clear transfer done flag
                    while(rb_ddma_act);

#ifdef CPUA_PROC
                    //enable cpua_proc
                    while(rw_cpua_set & CPUA_ACT);
                    rb_nblk_idx = rb_head_idx;
                    rd_cpua_reg0  = 0x0;
                    rd_cpua_reg1  = ld_cmd_padr & 0x3;
                    rd_cpua_reg3  = ld_buf_ptr & 0xfff;
                    rd_cpua_reg9  = ld_cmd_padr;
                    rd_cpua_reg10 = (ld_buf_ptr >> 12);
                    rd_cpua_reg11 = (QLNK_TRQ << 24) | ((dwrd)gb_htwl_sel << FRAG_SHIFT) | (dwrd)rb_frag_loc;
                    rd_cpua_reg12 = PROG_CPTR | LOW_PRI | CMD_BUF | (lb_cmd_end ? CMD_END : 0);
                    rw_cpua_set = CPUA_ACT | P2LP_CMD;
#else
                    //send program cmd to HW
                    fv_nand_htprg(lb_cmd_end, ld_buf_ptr, ld_cmd_padr);
#endif
                }
                else //host write cmd
                {
                    ld_buf_ptr = (dwrd)rs_host_cmd.sw_cmd_bfp;

                    //original vcnt
                    if(rs_host_cmd.sd_cmd_padr != L2P_NULL)
                    {
#ifdef FTL_DBG
                        byte lb_vmap_ofs;
                        lb_vmap_ofs = (((byte)rs_host_cmd.sd_cmd_padr << CMPR_SHIFT) & 0x1f) | (gb_crl_tbl[rs_host_cmd.sd_cmd_ladr] & CMPR_MASK);
                        if((gd_vmap_tbl[(rs_host_cmd.sd_cmd_padr >> (5 - CMPR_SHIFT))] & (1 << lb_vmap_ofs)) == 0x0)
                        {
                            fv_uart_print("ht vld map table clr err: p4k:%x, vmap_ofs:%x\r\n", rs_host_cmd.sd_cmd_padr, lb_vmap_ofs);
                            fv_dbg_loop(0x2);
                        }
                        gd_vmap_tbl[(rs_host_cmd.sd_cmd_padr >> (5 - CMPR_SHIFT))] &= ~(1 << lb_vmap_ofs);
#endif
                        lw_cmd_blk = (word)(rs_host_cmd.sd_cmd_padr >> (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT));
                        if(gd_vcnt_tbl[lw_cmd_blk] == 0)
                        {
                            fv_uart_print("host vld cnt table err: block:%x\r\n", lw_cmd_blk);
                            fv_dbg_loop(0x8);
                        }
                        gd_vcnt_tbl[lw_cmd_blk]--;

                        if((gd_vcnt_tbl[lw_cmd_blk] == 0) && ((gb_blk_type[lw_cmd_blk] & HEAD_BLK) == 0x0))
                        {
                            if((gb_blk_type[lw_cmd_blk] & VCNT_BLK) == 0x0)
                            {
                                fv_uart_print("host_wr release block err: block: %d\r\n", lw_cmd_blk);
                                fv_dbg_loop(0x7);
                            }

                            gb_blk_type[lw_cmd_blk] &= ~VCNT_BLK;
                            if((gb_blk_type[lw_cmd_blk] & FREE_MASK) == FREE_BLK)
                            {
                                //update blk info
                                if(gw_fblk_str == DBLK_INVLD)
                                {
                                    gw_fblk_str = lw_cmd_blk;
                                }
                                else
                                {
                                    gs_dbl_tbl[gw_fblk_end].wd.w_next_ptr = lw_cmd_blk;
                                }
                                gw_fblk_end = lw_cmd_blk;
                                gw_fblk_num++;

                                fv_uart_print("host_wr release blk: blk_num:%x, free blk cnt:%d\r\n", lw_cmd_blk, gw_fblk_num);
                                if(gw_fblk_num == GC_BLK_THR)
                                {
                                    fv_uart_print("gc task finish: host\r\n");
                                }
                            }
                        }
                    }

                    //get free block
                    if((rb_head_stg == 0x0) && ((gw_htp2l_ofs & CMPR_MASK) == 0x0))
                    {
                        //check free block
                        if(gw_fblk_num == 0x0)
                        {
                            fv_uart_print("ht free blk zero\r\n");
                            fv_dbg_loop(0x1);
                        }

                        //update blk info
                        lw_cmd_blk = gw_fblk_str;
                        gw_fblk_str = (lw_cmd_blk == gw_fblk_end) ? DBLK_INVLD : gs_dbl_tbl[lw_cmd_blk].wd.w_next_ptr;
                        gb_blk_type[lw_cmd_blk] = HEAD_BLK;
                        gw_fblk_num--;
                        fv_uart_print("host_wr get new blk: blk_num:%x, free blk cnt:%d\r\n", lw_cmd_blk, gw_fblk_num);
                        if(gw_fblk_num == (GC_BLK_THR - 1))
                        {
                            fv_uart_print("gc task start: host\r\n");
                        }

                        rw_hblk_loc = lw_cmd_blk;
                        ld_cmd_padr = (dwrd)lw_cmd_blk << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT);
#ifdef FTL_DBG
                        gd_htp2l_ptr = (dwrd)lw_cmd_blk << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT + CMPR_SHIFT);
#endif

                        //send erase cmd to HW
                        while(rw_cpua_set & CPUA_ACT);
                        fv_nand_erase(lw_cmd_blk);
                        gd_ecnt_tbl[lw_cmd_blk]++;
                    }

                    //write l2p & vcnt table
                    gd_l2p_tbl[rs_host_cmd.sd_cmd_ladr] = ld_cmd_padr;
                    gd_vcnt_tbl[rw_hblk_loc]++;
#ifdef FTL_DBG
                    gb_crl_tbl[rs_host_cmd.sd_cmd_ladr] = (byte)gw_htp2l_ofs & CMPR_MASK;

                    {
                        byte lb_vmap_ofs;
                        lb_vmap_ofs = (((byte)ld_cmd_padr << CMPR_SHIFT) & 0x1f) | ((byte)gw_htp2l_ofs & CMPR_MASK);
                        if((gd_vmap_tbl[(ld_cmd_padr >> (5 - CMPR_SHIFT))] & (1 << lb_vmap_ofs)) != 0x0)
                        {
                            fv_uart_print("ht vld map table set err: p4k:%x, vmap_ofs:%x\r\n", ld_cmd_padr, lb_vmap_ofs);
                            fv_dbg_loop(0x3);
                        }
                        gd_vmap_tbl[(ld_cmd_padr >> (5 - CMPR_SHIFT))] |= (1 << lb_vmap_ofs);
                    }
#endif

#ifdef LGET_EN
                    rd_lupd_l4k = rs_host_cmd.sd_cmd_ladr;
                    rd_lupd_p4k = ld_cmd_padr;
                    rw_lupd_set = LUPD_ACT;
#endif

                    //set check zone
#ifdef N4KA_EN
                    if(lb_n4rd_vld == 0)
#endif
                    {
                        //rq_ochk_pat = (qwrd)rs_host_cmd.sd_cmd_padr;
                        rd_ochk_pat = rs_host_cmd.sd_cmd_padr;
                    }

#ifdef FTL_DBG
                    if((gd_htp2l_ptr & 0xfff) != gw_htp2l_ofs)
                    {
                        fv_uart_print("ht p2l ptr error1: gd_htp2l_ptr:%x, gw_htp2l_ofs:%x\r\n", gd_htp2l_ptr, gw_htp2l_ofs);
                        fv_dbg_loop(0x4);
                    }

                    //write p2l table of dram for debug
                    gd_p2l_tbl[gd_htp2l_ptr] = rs_host_cmd.sd_cmd_ladr;
                    gd_htp2l_ptr++;
#endif

                    //write p2l tmp table
                    gd_htp2l_ladr[gw_htp2l_ofs & 0x3] = rs_host_cmd.sd_cmd_ladr;

#ifdef CPUA_PROC
                    //enable cpua_proc
                    while(rw_cpua_set & CPUA_ACT);
                    rb_nblk_idx = rb_head_idx;
                    rd_cpua_reg0  = 0x0;
                    rd_cpua_reg3  = ld_buf_ptr;
                    rd_cpua_reg4  = gd_htp2l_ladr[0];
                    rd_cpua_reg5  = gd_htp2l_ladr[1];
                    rd_cpua_reg6  = gd_htp2l_ladr[2];
                    rd_cpua_reg7  = gd_htp2l_ladr[3];
                    rd_cpua_reg8  = rs_host_cmd.sd_cmd_ladr;
                    rd_cpua_reg9  = ld_cmd_padr;
                    rd_cpua_reg10 = SRAM_MEM_BASE | ((dwrd)gw_htp2l_bfp << 12) | ((dwrd)gw_htp2l_ofs << 2);
                    rd_cpua_reg11 = (QLNK_HTQ << 24) | ((dwrd)gb_htwl_sel << FRAG_SHIFT) | rb_frag_loc;
                    rd_cpua_reg12 = PROG_CPTR | LOW_PRI | CMD_BUF | (lb_cmd_end ? CMD_END : 0);
                    rw_cpua_set = CPUA_ACT | DATA_CMD;
#else
                    //write l4k/p4k to fw meta
                    if((ld_buf_ptr >> BPTR_SFT) == SMDT_BASE)
                    {
                        rq_smmt_dat(ld_buf_ptr, (HMETA_QSZ+0)) = ((qwrd)ld_cmd_padr << DWRD_SHIFT) | (qwrd)rs_host_cmd.sd_cmd_ladr;
                        memset((byte *)(&rq_smmt_dat(ld_buf_ptr, (HMETA_QSZ+1))), 0, (4*6));
                    }
                    else
                    {
                        rq_dmmt_dat(ld_buf_ptr, (HMETA_QSZ+0)) = ((qwrd)ld_cmd_padr << DWRD_SHIFT) | (qwrd)rs_host_cmd.sd_cmd_ladr;
                        memset((byte *)(&rq_dmmt_dat(ld_buf_ptr, (HMETA_QSZ+1))), 0, (4*6));
                    }

                    //write p2l page to bmu sram
                    if((gw_htp2l_ofs & 0x3) == 0x3)
                    {
                        memcpy((byte *)(&rd_smdt_dat(gw_htp2l_bfp, (gw_htp2l_ofs & 0x3fc))), (byte *)(gd_htp2l_ladr), (4*4));
                    }

                    //send program cmd to HW
                    fv_nand_htprg(lb_cmd_end, ld_buf_ptr, ld_cmd_padr);
#endif

                    //move p2l 4KB data from sram to dram
                    if((gw_htp2l_ofs & 0x3ff) == 0x3ff)
                    {
                        while(rw_cpua_set & CPUA_ACT);
                        while((rb_nddn_bit & 0xf) != 0xf); //check whether last p2l prgram done
                        while(rb_ddma_act);
                        rd_ddma_dad = ((FTL_HTP2L_BASE | ((gw_htp2l_ofs << 2) & 0x3000)) >> 4);
                        rd_ddma_sad = ((dwrd)gw_htp2l_bfp << (13 - 4));
                        rb_ddma_wen = 1;
                        rb_ddma_act = 1;
                        gw_htp2l_bfp = (gw_htp2l_bfp == 8) ? 9 : 8;

                        //move p2l page from dram to nand
                        if(gw_htp2l_ofs == 0xfff)
                        {
                            gw_htp2l_ofs = 0xffff; //set as -1
                            lb_p2lp_act = 1;
#ifdef GC_SIM
                            gb_htp2l_num++;
#endif
                        }
                    }

                    gw_htp2l_ofs++;
                } //if(lb_p2lp_act) else


                //set next padr
                lw_cmd_blk = rw_hblk_loc;
                rb_hp4k_set = JUMP_ACT;
                while(rb_hp4k_set & JUMP_ACT);
                if(rb_head_stg == 0x0)
                {
                    if(gd_vcnt_tbl[lw_cmd_blk] != 0)
                    {
                        gb_blk_type[lw_cmd_blk] = VCNT_BLK;
                    }
                    else
                    {
                        //update blk info
                        if(gw_fblk_str == DBLK_INVLD)
                        {
                            gw_fblk_str = lw_cmd_blk;
                        }
                        else
                        {
                            gs_dbl_tbl[gw_fblk_end].wd.w_next_ptr = lw_cmd_blk;
                        }
                        gw_fblk_end = lw_cmd_blk;
                        gw_fblk_num++;

                        fv_uart_print("host_wr release blk 2: blk_num:%x, free blk cnt:%d\r\n", lw_cmd_blk, gw_fblk_num);
                        if(gw_fblk_num == GC_BLK_THR)
                        {
                            fv_uart_print("gc task finish: host2\r\n");
                        }
                    }

                    gb_htwl_sel = 0;
                }
                else
                {
                    gb_htwl_sel = (rb_head_stg == 0x1) ? ((gb_htwl_sel < 4) ? 4 : 0) : ((gb_htwl_sel & 0x4) | rb_gwln_loc);
                }

                //update token
                if(rb_head_stg != 0x7)
                {
#ifdef GC_SIM
                    if((gb_htp2l_num > GC_SIM_THR) || (gd_ht_token < gd_gc_token))
#else
                    if((gw_fblk_num < GC_BLK_THR) || (gd_ht_token < gd_gc_token))
#endif
                    {
                        gd_ht_token = gd_ht_token + gd_ht_weight;
                    }
                }

                //move last p2l page from dram to nand
                if((rw_page_loc == (PAGE_QNTY - 1)) && (rb_cech_loc == (RDIE_QNTY - 1)) && (rb_frag_loc == (FRAG_QNTY - 4)))
                {
                    while(rw_cpua_set & CPUA_ACT);

                    //add dummy
                    while(gw_htp2l_ofs < 0x1000)
                    {
#ifdef FTL_DBG
                        if((gd_htp2l_ptr & 0xfff) != gw_htp2l_ofs)
                        {
                            fv_uart_print("ht p2l ptr error2: gd_htp2l_ptr:%x, gw_htp2l_ofs:%x\r\n", gd_htp2l_ptr, gw_htp2l_ofs);
                            fv_dbg_loop(0x5);
                        }

                        //write p2l table of dram for debug
                        gd_p2l_tbl[gd_htp2l_ptr] = P2L_NULL;
                        gd_htp2l_ptr++;
#endif

                        rd_smdt_dat(gw_htp2l_bfp, (gw_htp2l_ofs & 0x3ff)) = P2L_NULL;

                        //move p2l 4KB data from sram to dram
                        if((gw_htp2l_ofs & 0x3ff) == 0x3ff)
                        {
                            while((rb_nddn_bit & 0xf) != 0xf); //check whether last p2l program done
                            while(rb_ddma_act);
                            rd_ddma_dad = ((FTL_HTP2L_BASE | ((gw_htp2l_ofs << 2) & 0x3000)) >> 4);
                            rd_ddma_sad = ((dwrd)gw_htp2l_bfp << (13 - 4));
                            rb_ddma_wen = 1;
                            rb_ddma_act = 1;
                            gw_htp2l_bfp = (gw_htp2l_bfp == 8) ? 9 : 8;
                        }
                        gw_htp2l_ofs++;
                    }

                    //move p2l page from dram to nand
                    gw_htp2l_ofs = 0;
                    lb_p2lp_act = 1;
                }
            } while(lb_p2lp_act || lb_raid_act);

            //get host cmd
#ifdef N4KA_EN
            if(lb_n4rd_vld)
            {
                gb_n4rd_idxq[gb_n4rd_wptr] = rb_n4rd_idx;
                gb_n4rd_wptr = (gb_n4rd_wptr == 254) ? 0 : (gb_n4rd_wptr + 1);
                rb_n4rd_vld = N4RD_CLR;
                rb_host_get = LCMD_INV;
            }
            else
#endif
            {
                rb_host_get = HCMD_INV;
            }

            //check read unit cnt
            if(rs_host_cmd.sb_cmd_mued && (rb_frag_loc == 0x0))
            {
                break;
            }
        } //else if(rs_host_cmd.sb_cmd_type == HCMD_WR)

#ifdef N4KA_EN
        //nand write non 4KB cmd handle
        else if((rs_host_cmd.sb_cmd_type == HCMD_WR) && (gb_n4rd_rptr != gb_n4rd_wptr))
        {
#ifdef HWCMD_CHK
            fv_uart_print("nr%x,%x,%x\r\n", (rs_host_cmd.sd_cmd_cid >> 7), rs_host_cmd.sd_cmd_ladr);
            if(rs_host_cmd.sb_cmd_ftl == 0)
            {
                rb_host_get = HCMD_INV;
                continue;
            }
#endif
            lb_n4rd_idx = gb_n4rd_idxq[gb_n4rd_rptr];
            gb_n4rd_rptr = (gb_n4rd_rptr == 254) ? 0 : (gb_n4rd_rptr + 1);
            gb_n4rd_strm[lb_n4rd_idx] = rs_host_cmd.sb_cmd_strm;
            gd_n4rd_ladr[lb_n4rd_idx] = rs_host_cmd.sd_cmd_ladr;
            gd_n4rd_padr[lb_n4rd_idx] = rs_host_cmd.sd_cmd_padr;

            //set check zone
            //rq_ochk_pat = (qwrd)rs_host_cmd.sd_cmd_padr;
            rd_ochk_pat = rs_host_cmd.sd_cmd_padr;

            while(rw_cpua_set & CPUA_ACT);
            fv_nand_n4k(lb_n4rd_idx, rs_host_cmd.sb_cmd_hit);

            //get host cmd
            rb_host_get = HCMD_INV;
        }
#endif

        //other cmd handle, trim, flush, power mode, security erase, download FW, system info
        else
        {
            fv_ftl_oth();

            //get host cmd
            rb_host_get = HCMD_INV;

            break;
        }

        //update Read/Write command Cnt
        lb_rw_cnt--;
        if(lb_rw_cnt == 0x0)
        {
            break;
        }
    } //while(rs_host_cmd.sb_cmd_type != HCMD_INV)

//    //pre read check
//    if(rb_bmu_hold)
//    {
//        //if pre_read tbl has this LBA, enable nau and send to host
//        //else if pre read end, skip this data
//        //else record to pre_read tbl
//    }

    return;
}
