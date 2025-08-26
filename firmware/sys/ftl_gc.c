#include <global.h>
#include <register.h>

extern void fv_nand_erase(word iw_head_blk);
extern void fv_nand_gcprg(byte ib_cmd_end, dwrd id_cmd_bptr, dwrd id_cmd_padr);

extern dwrd *gd_l2p_tbl;
extern byte *gb_crl_tbl;
extern dwrd gd_vcnt_tbl[RBLK_QNTY];
extern dwrd gd_ecnt_tbl[RBLK_QNTY];
extern DBL_TBL gs_dbl_tbl[RBLK_QNTY];
extern byte gb_blk_type[RBLK_QNTY];
extern PHY_ADR gs_head_padr[HEAD_QNTY];

extern word gw_fblk_str;
extern word gw_fblk_end;
extern word gw_fblk_num;

dwrd gd_min_vcnt;
byte gb_gcblk_vld = 0;
word gw_gcblk_num = 0;
word gw_gcrd_cnt = 0;
word gw_chkz_ptr = 0;

dwrd gd_ht_token = 0;
dwrd gd_gc_token = 0;
dwrd gd_ht_weight = 1;
dwrd gd_gc_weight = 1;
dwrd gd_ht_tmp = 1;
dwrd gd_gc_tmp = 1;
byte gb_gcblk_que = 0;
byte gb_gcblk_fin = 0;
word gw_gcblk_cnt = 0;
word gw_gcblk_tmp = 0;

//p2l table message
dwrd gd_pmsg_pcf[PMSG_NUM]; //page, cech, frag
byte gb_pmsg_npg[PMSG_NUM]; //next page unit
byte gb_pmsg_epg[PMSG_NUM]; //end page unit
word gw_pmsg_rdp = 0;       //read ptr
word gw_pmsg_gap = 0;       //gcad ptr

byte gb_p2lp_rds = 0; //read select
byte gb_p2lp_cps = 0; //compare select
byte gb_p2lp_fns = 0; //finish select
byte gb_p2lp_gas = 0; //gcad blk select
byte gb_p2lp_nrd[2];
word gw_p2lp_blk[2];
byte gb_p2lp_npg[2];
byte gb_p2lp_cnt[2];
byte gb_gcad_wat[2];
word gw_gcad_blk[2];
byte gb_gcad_exe = 0;
byte gb_ochk_sel = 0;
word gw_ochk_ptr = 0;
word gw_chkz_cnt[8];
byte gb_chkz_fin[8];

word gw_gcp2l_bfp = 10;
word gw_gcp2l_ofs = 0;
dwrd gd_gcp2l_ladr[4];

byte gb_gcwl_sel = 0;

#ifdef FTL_DBG
extern dwrd *gd_p2l_tbl;
extern dwrd *gd_vmap_tbl;

dwrd gd_p2lc_ptr = 0;
word gw_gcad_cnt = 0;
dwrd gd_gcp2l_ptr;
#endif



void fv_ftl_gc(void)
{
    byte lb_p2lp_act = 0;
    byte lb_p2lp_ptr = 0;
    byte lb_raid_act = 0;
    byte lb_cmd_end;
    byte lb_mapu_ptr;
    byte lb_chkz_sel;
    byte lb_olkp_act;
    word lw_cmd_blk;
    dwrd ld_buf_ptr;
    dwrd ld_cah_ladr[CMPR_NUM];
    dwrd ld_cmd_ladr, ld_cmd_padr;

    //get gc source blk
    if((gb_gcblk_vld == 0) && (gb_gcblk_que < 2))
    {
        //search gc source blk(change HW accelerator in the future)
#ifdef GC_SIM
        gd_min_vcnt = (FRAG_QNTY * RDIE_QNTY * PAGE_QNTY) / 8;
        gb_gcblk_vld = 1;
        gw_gcblk_num += 1;
#else
        gd_min_vcnt = 0xffffffff;
        for(lw_cmd_blk = 1; lw_cmd_blk < RBLK_QNTY; lw_cmd_blk = lw_cmd_blk + 1)
        {
            if((gb_blk_type[lw_cmd_blk] == VCNT_BLK) && (gd_vcnt_tbl[lw_cmd_blk] < gd_min_vcnt))
            {
                gd_min_vcnt = gd_vcnt_tbl[lw_cmd_blk];
                gb_gcblk_vld = 1;
                gw_gcblk_num = lw_cmd_blk;
            }
        }

        if(gb_gcblk_vld == 0)
        {
            fv_uart_print("gc source blk search error\r\n");
            fv_dbg_loop(0x10);
        }
#endif
        gb_blk_type[gw_gcblk_num] = VCNT_BLK | GC_BLK;

        //calculate token/weight
        gd_ht_tmp = gd_min_vcnt / (FRAG_QNTY * RDIE_QNTY) + 1;
        gd_gc_tmp = ((PAGE_QNTY - P2LP_NUM) > gd_ht_tmp) ? (PAGE_QNTY - P2LP_NUM - gd_ht_tmp) : 1;
        gb_gcblk_que++;
        if(gb_gcblk_que == 1)
        {
            gd_ht_weight = gd_ht_tmp;
            gd_gc_weight = gd_gc_tmp;
        }
        fv_uart_print("get gc source blk: blk_num:%x, vld_cnt:%d, gcblk_que:%d, ht_weight:%d, gc_weight:%d\r\n", gw_gcblk_num, gd_min_vcnt, gb_gcblk_que, gd_ht_tmp, gd_gc_tmp);
    }


    //read p2l page
    while(gb_gcblk_vld && !gb_p2lp_nrd[gb_p2lp_rds])
    {
        fv_uart_debug("rd p2l: blk_num:%x, gw_pmsg_rdp:%d, gb_p2lp_rds:%d, gd_pmsg_pcf:%x\r\n", gw_gcblk_num, gw_pmsg_rdp, gb_p2lp_rds, gd_pmsg_pcf[gw_pmsg_rdp]);

        while(rw_cpua_set & CPUA_ACT);
        for(lb_mapu_ptr = 0; lb_mapu_ptr < 4; lb_mapu_ptr = lb_mapu_ptr + 1)
        {
            rb_cmd_qidx = QLNK_SMQ;
            rd_cmd_cid(0) = 0x0;
            rd_cmd_padr = (gw_gcblk_num << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT)) | gd_pmsg_pcf[gw_pmsg_rdp] | (dwrd)lb_mapu_ptr;
            fv_nque_chk(10);
            rw_cmd_bptr = (SMDT_BASE << BPTR_SFT) | ((word)gb_p2lp_rds << 2) | (word)lb_mapu_ptr;
            rw_cmd_type = READ_CPTR | HIGH_PRI;
        }
        gb_p2lp_nrd[gb_p2lp_rds] = 1;
        gw_p2lp_blk[gb_p2lp_rds] = gw_gcblk_num;
        gb_p2lp_npg[gb_p2lp_rds] = gb_pmsg_npg[gw_pmsg_rdp];
        gb_p2lp_rds = !gb_p2lp_rds;

        if(gw_pmsg_rdp == PMSG_END)
        {
            gw_pmsg_rdp = 0;
            gb_gcblk_vld = 0;
            fv_uart_print("gc blk finish(read p2l): blk_num:%x, vld_cnt:%d\r\n", gw_gcblk_num, gd_vcnt_tbl[gw_gcblk_num]);
        }
        else
        {
            gw_pmsg_rdp++;
        }
    }

    //check whether p2l table ready
    while(rb_smrd_vld)
    {
        if(rw_smrd_bfp & 0x4)
        {
            gb_p2lp_cnt[1]++;
        }
        else
        {
            gb_p2lp_cnt[0]++;
        }
        rb_smrd_vld = SMRD_CLR; //write 1 clear
        ARM_NOP(); //wait 3 cycle for HW handle
    }

    //clear nrd for next p2l read
    if((rb_gcad_set == 0x0) && gb_gcad_exe)
    {
        fv_uart_debug("clear p2l rd flag: gb_p2lp_fns:%d, gb_p2lp_nrd:%d\r\n", gb_p2lp_fns, gb_p2lp_nrd[gb_p2lp_fns]);

        gb_gcad_exe = 0;
        gb_p2lp_nrd[gb_p2lp_fns] = 0;
        gb_p2lp_fns = !gb_p2lp_fns;
    }

    //enable gc p4k compare
    //if((rw_gcad_bsy == 0x0) && !gb_gcad_wat[gb_p2lp_cps] && !gb_gcad_exe && (gb_p2lp_cnt[gb_p2lp_cps] == 4) && (!gb_p2lp_npg[gb_p2lp_cps] || (((rb_ochk_map | rb_oclr_map) & (1 << gb_ochk_sel)) == 0)))
    if(!gb_gcad_wat[gb_p2lp_cps] && !gb_gcad_exe && (gb_p2lp_cnt[gb_p2lp_cps] == 4) && (!gb_p2lp_npg[gb_p2lp_cps] || (((rb_ochk_map | rb_oclr_map) & (1 << gb_ochk_sel)) == 0)))
    {
        fv_uart_debug("en gc cmp: gb_p2lp_npg:%d, gw_ochk_ptr:%d, rd_gpcf_ptr:%x, rb_gwln_ptr:%d, rb_pgrp_ptr:%d\r\n", gb_p2lp_npg[gb_p2lp_cps], gw_ochk_ptr, rd_gpcf_ptr, rb_gwln_ptr, rb_pgrp_ptr);

#ifdef FTL_DBG
        if(gd_p2lc_ptr == 0)
        {
            gd_p2lc_ptr = (dwrd)gw_gcblk_num << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT + CMPR_SHIFT);
        }

        do
        {
            if(gd_p2l_tbl[gd_p2lc_ptr] != rd_smdt_dat(((gd_p2lc_ptr >> 10) & 0x7), (gd_p2lc_ptr & 0x3ff)))
            {
                fv_uart_print("p2l table compare fail: gd_p2lc_ptr:%x, gd_p2l_tbl:%x, rd_smdt_dat:%x\r\n", gd_p2lc_ptr, gd_p2l_tbl[gd_p2lc_ptr], rd_smdt_dat(((gd_p2lc_ptr >> 10) & 0x7), (gd_p2lc_ptr & 0x3ff)));
                fv_dbg_loop(0x12);
            }
            gd_p2lc_ptr++;
        } while((gd_p2lc_ptr & 0xfff) != 0);

        if(((gd_p2lc_ptr >> 12) & PMSG_MSK) == PMSG_NUM)
        {
            gd_p2lc_ptr = 0;
        }
#endif

        //enable check zone
        if(gb_p2lp_npg[gb_p2lp_cps])
        {
            rb_ochk_map |= (1 << gb_ochk_sel); //set enable
            rd_ochk_bpu(gb_ochk_sel) = ((dwrd)gw_p2lp_blk[gb_p2lp_cps] << CHKZ_SFT) | (dwrd)gw_ochk_ptr;
            gw_ochk_ptr = (gw_ochk_ptr == CHKZ_END) ? 0 : (gw_ochk_ptr + 1);
            //gb_ochk_sel = (gb_ochk_sel == 0x7) ? 0 : (gb_ochk_sel + 1);
            gb_ochk_sel = gw_ochk_ptr & 0x7;
        }

        //enable gc cmp
        rw_gcad_bfp(0) = gb_p2lp_cps ? 0x4 : 0x0;
        rw_gcad_bfp(1) = gb_p2lp_cps ? 0x5 : 0x1;
        rw_gcad_bfp(2) = gb_p2lp_cps ? 0x6 : 0x2;
        rw_gcad_bfp(3) = gb_p2lp_cps ? 0x7 : 0x3;
        if(rw_gcad_blk == gw_p2lp_blk[gb_p2lp_cps])
        {
            rd_gpcf_str = rd_gpcf_ptr;
            rb_gwln_str = rb_gwln_ptr;
            rb_pgrp_str = rb_pgrp_ptr;
        }
        else
        {
            rd_gpcf_str = 0;
            rb_gwln_str = 0;
            rb_pgrp_str = 0;
            rw_gcad_blk = gw_p2lp_blk[gb_p2lp_cps];
        }
        rb_gcad_set = GCAD_ACT;

        gb_gcad_exe = 1;
        gb_gcad_wat[gb_p2lp_cps] = 1;
        gw_gcad_blk[gb_p2lp_cps] = gw_p2lp_blk[gb_p2lp_cps];
        gb_p2lp_cnt[gb_p2lp_cps] = 0;
        gb_p2lp_cps = !gb_p2lp_cps;
    }


    //gc read
    while(rb_gcad_vld && (rw_gcbf_num < GC_RD_THR))
    {
        if(!rb_gcad_nul)
        {
            while(rw_cpua_set & CPUA_ACT);
            rb_cmd_qidx = QLNK_GCQ;
            rd_cmd_cid(0) = (dwrd)rb_gcad_map; //record valid map
            rd_cmd_padr = (gw_gcad_blk[gb_p2lp_gas] << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT)) | rd_gcad_pcf;
            fv_nque_chk(11);
            rw_cmd_bptr = BPTR_INVLD;
            rw_cmd_type = READ_CPTR | LOW_PRI;

            lb_chkz_sel = (rd_gcad_pcf >> 15) & CHKZ_MSK;
            gw_chkz_cnt[lb_chkz_sel]++;
            gw_gcrd_cnt++;
            rb_abuf_set = ABUF_ACT | GCRD_BUF;
#ifdef FTL_DBG
            gw_gcad_cnt++;

            ld_cmd_padr = rd_cmd_padr;
            for(lb_mapu_ptr = 0; lb_mapu_ptr < CMPR_NUM; lb_mapu_ptr = lb_mapu_ptr + 1)
            {
                byte lb_vmap_ofs;
                lb_vmap_ofs = (((byte)ld_cmd_padr << CMPR_SHIFT) & 0x1f) | lb_mapu_ptr;
                if((rb_gcad_map & (1 << lb_mapu_ptr)) && ((gd_vmap_tbl[(ld_cmd_padr >> (5 - CMPR_SHIFT))] & (1 << lb_vmap_ofs)) == 0x0))
                {
                    //rq_olkp_pat = (qwrd)ld_cmd_padr;
                    rd_olkp_pat = ld_cmd_padr;
                    while(rb_olkp_exe);
                    if(!rb_olkp_hit || rb_olkp_err)
                    {
                        fv_uart_print("gcad result err: p4k:%x, cmpr_loc:%x, vldmap:%x\r\n", ld_cmd_padr, lb_mapu_ptr, rb_gcad_map);
                        fv_dbg_loop(0x13);
                    }
                }
            }
#endif
        }

        if(rb_gcad_fin)
        {
#ifdef FTL_DBG
            lb_chkz_sel = (byte)(gw_chkz_ptr & 0x7);
            fv_uart_debug("gcad rd fin: gw_gcad_cnt:%d, gw_pmsg_gap:%d, gb_pmsg_epg:%d, gw_gcrd_cnt:%d, gw_chkz_cnt:%d\r\n", gw_gcad_cnt, gw_pmsg_gap, gb_pmsg_epg[gw_pmsg_gap], gw_gcrd_cnt, gw_chkz_cnt[lb_chkz_sel]);
            gw_gcad_cnt = 0;
#endif
            if(gb_pmsg_epg[gw_pmsg_gap])
            {
                lb_chkz_sel = (byte)(gw_chkz_ptr & 0x7);
                if(gw_chkz_cnt[lb_chkz_sel] == 0)
                {
                    fv_uart_debug("chk zone fin2: lb_chkz_sel:%d, blk_num:%x, vld cnt: %d\r\n", lb_chkz_sel, gw_gcad_blk[gb_p2lp_gas], gd_vcnt_tbl[gw_gcad_blk[gb_p2lp_gas]]);
#ifdef FTL_DBG
                    {
                        dwrd ld_vmap_ptr;
                        ld_vmap_ptr = ((dwrd)gw_gcad_blk[gb_p2lp_gas] << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT + CMPR_SHIFT - 5)) | ((dwrd)gw_chkz_ptr << 10);
                        do
                        {
                            if(gd_vmap_tbl[ld_vmap_ptr] != 0x0)
                            {
                                fv_uart_print("gc vld map sts2: ld_vmap_ptr:%x, vldmap:%x\r\n", ld_vmap_ptr, gd_vmap_tbl[ld_vmap_ptr]);
                                //fv_dbg_loop(0x14); //need check gd_gccah_sadr in nand_rw.c
                            }
                            ld_vmap_ptr = ld_vmap_ptr + 1;
                        } while((ld_vmap_ptr & 0x3ff) != 0);
                    }
#endif

                    rb_oclr_map |=  (1 << lb_chkz_sel);
                    rb_ochk_map &= ~(1 << lb_chkz_sel);
                    rd_ochk_bpu(lb_chkz_sel) = 0x0;
                }
                else
                {
                    gb_chkz_fin[lb_chkz_sel] = 1;
                }
                gw_chkz_ptr = (gw_chkz_ptr == CHKZ_END) ? 0 : (gw_chkz_ptr + 1);
            } //if(gb_pmsg_epg[gw_pmsg_gap])

            if(gw_pmsg_gap == PMSG_END)
            {
                gw_pmsg_gap = 0;
                gb_blk_type[gw_gcad_blk[gb_p2lp_gas]] &= ~GC_BLK;
                gb_blk_type[gw_gcad_blk[gb_p2lp_gas]] |= GCFN_BLK;
                fv_uart_print("gc blk finish(read data): blk_num:%x, vld_cnt:%d\r\n", gw_gcad_blk[gb_p2lp_gas], gd_vcnt_tbl[gw_gcad_blk[gb_p2lp_gas]]);
                if((gb_blk_type[gw_gcad_blk[gb_p2lp_gas]] & FREE_MASK) == FREE_BLK)
                {
                    //update blk info
                    if(gw_fblk_str == DBLK_INVLD)
                    {
                        gw_fblk_str = gw_gcad_blk[gb_p2lp_gas];
                    }
                    else
                    {
                        gs_dbl_tbl[gw_fblk_end].wd.w_next_ptr = gw_gcad_blk[gb_p2lp_gas];
                    }
                    gw_fblk_end = gw_gcad_blk[gb_p2lp_gas];
                    gw_fblk_num++;

                    fv_uart_print("gc_wr release blk 3: blk_num:%x, free blk cnt:%d\r\n", gw_gcad_blk[gb_p2lp_gas], gw_fblk_num);
                    if(gw_fblk_num == GC_BLK_THR)
                    {
                        fv_uart_print("gc task finish: gc3\r\n");
                    }
                }

                //set token & weight
                if(gw_gcrd_cnt == 0x0)
                {
                    gb_gcblk_que--;
                    if((gb_gcblk_que == 1) && (gb_gcblk_fin == 0))
                    {
                        if(gd_ht_token < gd_gc_token)
                        {
                            gd_gc_token = gd_gc_token - gd_ht_token;
                            gd_gc_token = gd_gc_token / gd_ht_weight;
                            gd_gc_token = gd_gc_token * gd_ht_tmp;
                            gd_ht_token = 0;
                        }
                        else
                        {
                            gd_ht_token = gd_ht_token - gd_gc_token;
                            gd_ht_token = gd_ht_token / gd_ht_weight + 1;
                            gd_ht_token = gd_ht_token * gd_ht_tmp;
                            gd_gc_token = 0;
                        }
                        gd_ht_weight = gd_ht_tmp;
                        gd_gc_weight = gd_gc_tmp;
                        fv_uart_debug("weight update1: ht_weight:%d, gc_weight:%d\r\n", gd_ht_weight, gd_gc_weight);
                    }
                }
                else
                {
                    gb_gcblk_fin++;
                    if(gb_gcblk_fin == 1)
                    {
                        gw_gcblk_cnt = gw_gcrd_cnt;
                    }
                    else
                    {
                        gw_gcblk_tmp = gw_gcrd_cnt;
                    }
                }
            }
            else
            {
                gw_pmsg_gap++;
            }

            gb_gcad_wat[gb_p2lp_gas] = 0;
            gb_p2lp_gas = !gb_p2lp_gas;
        }

        rb_gcad_vld = 1; //clear
    }


    //gc write
    while(rb_gcrd_vld)
    {
#if (defined(FTL_DBG))
        //check P4K address
        ld_cmd_padr = ((rw_gcrd_bfp >> BPTR_SFT) == SMDT_BASE) ? rd_smmt_dat(rw_gcrd_bfp, (HMETA_DSZ+1)) : rd_dmmt_dat(rw_gcrd_bfp, (HMETA_DSZ+1));
        if(ld_cmd_padr != rd_gcrd_pad)
        {
            fv_uart_print("P4K error: rw_gcrd_bfp:%x, ld_cmd_padr:%x, rd_gcrd_pad:%x\r\n", rw_gcrd_bfp, ld_cmd_padr, rd_gcrd_pad);
            fv_dbg_loop(0x15);
        }
#endif
        //copy gcrd message
        ld_buf_ptr = (dwrd)rw_gcrd_bfp;
        if((ld_buf_ptr >> BPTR_SFT) == SMDT_BASE)
        {
            memcpy((byte *)(ld_cah_ladr), (byte *)(&rq_smmt_dat(ld_buf_ptr, (HMETA_QSZ+0))), (CMPR_NUM*4));
        }
        else
        {
            memcpy((byte *)(ld_cah_ladr), (byte *)(&rq_dmmt_dat(ld_buf_ptr, (HMETA_QSZ+0))), (CMPR_NUM*4));
        }

        //check whether host overwrite
        lb_olkp_act = (byte)rd_gcrd_cid(0);
        //rq_olkp_pat = (qwrd)rd_gcrd_pad;
        rd_olkp_pat = rd_gcrd_pad;
        while(rb_olkp_exe);
        if(rb_olkp_err)
        {
            fv_uart_print("olkp setting err: p4k:%x\r\n", rd_gcrd_pad);
            fv_dbg_loop(0x1C);
        }

        if(rb_olkp_hit)
        {
            lb_olkp_act = 0x0;
        }

        if(lb_olkp_act == 0x0)
        {
            //fv_uart_print("o");
            fv_uart_debug("gc wr overwite: rd_gcrd_pad:%x, rw_gcrd_bfp:%x\r\n", rd_gcrd_pad, rw_gcrd_bfp);

            //host overwrite, release gc read buffer
            rd_carl_set = ((dwrd)rw_gcrd_bfp << 16) | CARL_ACT;
        }
        else //gc write
        {
#ifdef LUPD_EN
            rw_lupd_cri = LUPD_SDM | LUPD_CRD | LUPD_CRM;
#endif

            //set head blk index for p4k_jump
            rb_head_idx = GC_HLOC;
            ARM_NOP(); //wait 3 cycle for HW handle

            do
            {
                lb_cmd_end = (rb_frag_loc == ((FRAG_QNTY - 1)) && (rb_gwln_loc == rb_gwln_end)) ? 1 : 0;
                ld_cmd_padr = rd_hp4k_loc;

                if(lb_p2lp_act) //p2l page
                {
                    ld_buf_ptr = (FTL_GCP2L_BASE >> 12) | (dwrd)lb_p2lp_ptr;
                    lb_p2lp_act = (lb_p2lp_ptr == 3) ? 0 : 1;
                    lb_p2lp_ptr = (lb_p2lp_ptr == 3) ? 0 : (lb_p2lp_ptr + 1);

                    rb_nddn_bit = 0xf0; //clear transfer done flag
                    while(rb_ddma_act);

#ifdef CPUA_PROC
                    //enable cpua_proc
                    while(rw_cpua_set & CPUA_ACT);
                    rb_nblk_idx = rb_head_idx;
                    rd_cpua_reg0  = 0x0;
                    rd_cpua_reg1  = (ld_cmd_padr & 0x3) | 0x4;
                    rd_cpua_reg3  = ld_buf_ptr & 0xfff;
                    rd_cpua_reg9  = ld_cmd_padr;
                    rd_cpua_reg10 = (ld_buf_ptr >> 12);
                    rd_cpua_reg11 = (QLNK_TRQ << 24) | RAID_GCRID | ((dwrd)gb_gcwl_sel << FRAG_SHIFT) | (dwrd)rb_frag_loc;
                    rd_cpua_reg12 = PROG_CPTR | LOW_PRI | CMD_BUF | (lb_cmd_end ? CMD_END : 0);
                    rw_cpua_set = CPUA_ACT | P2LP_CMD;
#else
                    //send program cmd to HW
                    fv_nand_gcprg(lb_cmd_end, ld_buf_ptr, ld_cmd_padr);
#endif
                }
                else //gc write
                {
                    ld_buf_ptr = (dwrd)rw_gcrd_bfp;

                    while(1) //for compression function, need to loop 4 times
                    {
                        //check valid mapu
                        if(lb_olkp_act & (1 << (gw_gcp2l_ofs & CMPR_MASK)))
                        {
                            //original vcnt
#ifdef FTL_DBG
                            byte lb_vmap_ofs;
                            lb_vmap_ofs = (((byte)rd_gcrd_pad << CMPR_SHIFT) & 0x1f) | ((byte)gw_gcp2l_ofs & CMPR_MASK);
                            if((gd_vmap_tbl[(rd_gcrd_pad >> (5 - CMPR_SHIFT))] & (1 << lb_vmap_ofs)) == 0x0)
                            {
                                fv_uart_print("gc vld map table clr err: p4k:%x, vmap_ofs:%x\r\n", rd_gcrd_pad, lb_vmap_ofs);
                                fv_dbg_loop(0x16);
                            }
                            gd_vmap_tbl[(rd_gcrd_pad >> (5 - CMPR_SHIFT))] &= ~(1 << lb_vmap_ofs);
#endif
                            lw_cmd_blk = (word)(rd_gcrd_pad >> (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT));
                            if(gd_vcnt_tbl[lw_cmd_blk] == 0)
                            {
                                fv_uart_print("gc vld cnt table err: block:%x\r\n", lw_cmd_blk);
                                fv_dbg_loop(0x1b);
                            }
                            gd_vcnt_tbl[lw_cmd_blk]--;

                            if((gd_vcnt_tbl[lw_cmd_blk] == 0) && ((gb_blk_type[lw_cmd_blk] & HEAD_BLK) == 0x0))
                            {
                                if((gb_blk_type[lw_cmd_blk] & VCNT_BLK) == 0x0)
                                {
                                    fv_uart_print("gc_wr release block err: block: %x\r\n", lw_cmd_blk);
                                    fv_dbg_loop(0x1a);
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

                                    fv_uart_print("gc_wr release blk: blk_num:%x, free blk cnt:%d\r\n", lw_cmd_blk, gw_fblk_num);
                                    if(gw_fblk_num == GC_BLK_THR)
                                    {
                                        fv_uart_print("gc task finish: gc\r\n");
                                    }
                                }
                            }

                            //get free block
                            if((rb_head_stg == 0x0) && ((gw_gcp2l_ofs & CMPR_MASK) == 0x0))
                            {
                                //check free block
                                if(gw_fblk_num == 0x0)
                                {
                                    fv_uart_print("gc free blk zero\r\n");
                                    fv_dbg_loop(0x11);
                                }

                                //update blk info
                                lw_cmd_blk = gw_fblk_str;
                                gw_fblk_str = (lw_cmd_blk == gw_fblk_end) ? DBLK_INVLD : gs_dbl_tbl[lw_cmd_blk].wd.w_next_ptr;
                                gb_blk_type[lw_cmd_blk] = HEAD_BLK;
                                gw_fblk_num--;
                                fv_uart_print("gc_wr get new blk: blk_num:%x, free blk cnt:%d\r\n", lw_cmd_blk, gw_fblk_num);
                                if(gw_fblk_num == (GC_BLK_THR - 1))
                                {
                                    fv_uart_print("gc task start: gc\r\n");
                                }

                                rw_hblk_loc = lw_cmd_blk;
                                ld_cmd_padr = (dwrd)lw_cmd_blk << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT);
#ifdef FTL_DBG
                                gd_gcp2l_ptr = (dwrd)lw_cmd_blk << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT + CMPR_SHIFT);
#endif

                                //send erase cmd to HW
                                while(rw_cpua_set & CPUA_ACT);
                                fv_nand_erase(lw_cmd_blk);
                                gd_ecnt_tbl[lw_cmd_blk]++;
                            }

                            //get L4K address
                            lb_mapu_ptr = (byte)gw_gcp2l_ofs & CMPR_MASK;
                            ld_cmd_ladr = ld_cah_ladr[lb_mapu_ptr];

#ifdef FTL_DBG
                            //check dram buffer ptr & L4K address
                            if((ld_buf_ptr >= 0xc00) && (ld_cmd_ladr >= L2PE_QNTY))
                            {
                                fv_uart_print("bfp or L4K error: ld_buf_ptr:%x, ld_cmd_ladr:%x", ld_buf_ptr, ld_cmd_ladr);
                                fv_dbg_loop(0x17);
                            }
#endif

                            //write l2p & vcnt table
                            gd_l2p_tbl[ld_cmd_ladr] = ld_cmd_padr;
                            gd_vcnt_tbl[rw_hblk_loc]++;
#ifdef FTL_DBG
                            gb_crl_tbl[ld_cmd_ladr] = (byte)gw_gcp2l_ofs & CMPR_MASK;

                            {
                                byte lb_vmap_ofs;
                                lb_vmap_ofs = (((byte)ld_cmd_padr << CMPR_SHIFT) & 0x1f) | ((byte)gw_gcp2l_ofs & CMPR_MASK);
                                if((gd_vmap_tbl[(ld_cmd_padr >> (5 - CMPR_SHIFT))] & (1 << lb_vmap_ofs)) != 0x0)
                                {
                                    fv_uart_print("gc vld map table set err: p4k:%x, vmap_ofs:%x\r\n", ld_cmd_padr, lb_vmap_ofs);
                                    fv_dbg_loop(0x18);
                                }
                                gd_vmap_tbl[(ld_cmd_padr >> (5 - CMPR_SHIFT))] |= (1 << lb_vmap_ofs);
                            }
#endif

#if (defined(LUPD_EN))
                            rd_lupd_l4k = ld_cmd_ladr;
                            rd_lupd_p4k = ld_cmd_padr;
                            rw_lupd_bfp = (word)ld_buf_ptr;
                            rw_lupd_set = LUPD_ACT | LUPD_GCQ | ((word)lb_mapu_ptr << 8);
                            while(rw_lupd_set & LUPD_ACT);
                            rw_lupd_cri &= ~LUPD_SDM;
#elif (defined(LGET_EN))
                            rd_lupd_l4k = ld_cmd_ladr;
                            rd_lupd_p4k = ld_cmd_padr;
                            rw_lupd_set = LUPD_ACT;
#endif

#ifdef FTL_DBG
                            if((gd_gcp2l_ptr & 0xfff) != gw_gcp2l_ofs)
                            {
                                fv_uart_print("gc p2l ptr error1: gd_gcp2l_ptr:%x, gw_gcp2l_ofs:%x\r\n", gd_gcp2l_ptr, gw_gcp2l_ofs);
                                fv_dbg_loop(0x19);
                            }

                            //write p2l table to dram for debug
                            gd_p2l_tbl[gd_gcp2l_ptr] = ld_cmd_ladr;
                            gd_gcp2l_ptr++;
#endif

                            //write p2l tmp table
                            gd_gcp2l_ladr[gw_gcp2l_ofs & 0x3] = ld_cmd_ladr;
                        }
                        else //invalid mapu
                        {
#ifdef FTL_DBG
                            //write l4k null to p2l table of dram for debug
                            gd_p2l_tbl[gd_gcp2l_ptr] = P2L_NULL;
                            gd_gcp2l_ptr++;
#endif

                            //write l4k null to p2l table
                            gd_gcp2l_ladr[gw_gcp2l_ofs & 0x3] = P2L_NULL;
                        }

                        if((gw_gcp2l_ofs & CMPR_MASK) == CMPR_MASK)
                        {
                            break;
                        }
                        else
                        {
                            gw_gcp2l_ofs++;
                        }
                    } //while(1)

#ifdef CPUA_PROC
                    //enable cpua_proc
                    while(rw_cpua_set & CPUA_ACT);
                    rb_nblk_idx = rb_head_idx;
                    rd_cpua_reg0  = 0x0;
                    rd_cpua_reg3  = ld_buf_ptr;
                    rd_cpua_reg4  = gd_gcp2l_ladr[0];
                    rd_cpua_reg5  = gd_gcp2l_ladr[1];
                    rd_cpua_reg6  = gd_gcp2l_ladr[2];
                    rd_cpua_reg7  = gd_gcp2l_ladr[3];
                    rd_cpua_reg8  = ld_cah_ladr[0];
                    rd_cpua_reg9  = ld_cmd_padr;
                    rd_cpua_reg10 = SRAM_MEM_BASE | ((dwrd)gw_gcp2l_bfp << 12) | ((dwrd)(gw_gcp2l_ofs & 0x3ff) << 2);
                    rd_cpua_reg11 = (QLNK_GCQ << 24) | RAID_GCRID | ((dwrd)gb_gcwl_sel << FRAG_SHIFT) | (dwrd)rb_frag_loc;
                    rd_cpua_reg12 = PROG_CPTR | LOW_PRI | CMD_BUF | (lb_cmd_end ? CMD_END : 0);
                    rw_cpua_set = CPUA_ACT | DATA_CMD;
#else

                    //write l4k & p4k to fw meta
                    if((ld_buf_ptr >> BPTR_SFT) == SMDT_BASE)
                    {
                        rq_smmt_dat(ld_buf_ptr, (HMETA_QSZ+0)) = ((qwrd)ld_cmd_padr << DWRD_SHIFT) | (qwrd)ld_cah_ladr[0];
                        memset((byte *)(&rq_smmt_dat(ld_buf_ptr, (HMETA_QSZ+1))), 0, (4*6));
                    }
                    else
                    {
                        rq_dmmt_dat(ld_buf_ptr, (HMETA_QSZ+0)) = ((qwrd)ld_cmd_padr << DWRD_SHIFT) | (qwrd)ld_cah_ladr[0];
                        memset((byte *)(&rq_dmmt_dat(ld_buf_ptr, (HMETA_QSZ+1))), 0, (4*6));
                    }

                    if((gw_gcp2l_ofs & 0x3) == 0x3)
                    {
                        memcpy((byte *)(&rd_smdt_dat(gw_gcp2l_bfp, (gw_gcp2l_ofs & 0x3fc))), (byte *)(gd_gcp2l_ladr), (4*4));
                    }

                    //send program cmd to HW
                    fv_nand_gcprg(lb_cmd_end, ld_buf_ptr, ld_cmd_padr);
#endif

                    //move p2l 4KB data from sram to dram
                    if((gw_gcp2l_ofs & 0x3ff) == 0x3ff)
                    {
                        while(rw_cpua_set & CPUA_ACT);
                        while((rb_nddn_bit & 0xf0) != 0xf0); //check whether last p2l prgram done
                        while(rb_ddma_act);
                        rd_ddma_dad = ((FTL_GCP2L_BASE | ((gw_gcp2l_ofs << 2) & 0x3000)) >> 4);
                        rd_ddma_sad = ((dwrd)gw_gcp2l_bfp << (13 - 4));
                        rb_ddma_wen = 1;
                        rb_ddma_act = 1;
                        gw_gcp2l_bfp = (gw_gcp2l_bfp == 10) ? 11 : 10;

                        //move p2l page from dram to nand
                        if(gw_gcp2l_ofs == 0xfff)
                        {
                            gw_gcp2l_ofs = 0xffff; //set as -1
                            lb_p2lp_act = 1;
                        }
                    }

                    gw_gcp2l_ofs++;
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

                        fv_uart_print("gc_wr release blk 2: blk_num:%x, free blk cnt:%d\r\n", lw_cmd_blk, gw_fblk_num);
                        if(gw_fblk_num == GC_BLK_THR)
                        {
                            fv_uart_print("gc task finish: gc2\r\n");
                        }
                    }

                    gb_gcwl_sel = 0;
                }
                else
                {
                    gb_gcwl_sel = (rb_head_stg == 0x1) ? ((gb_gcwl_sel < 4) ? 4 : 0) : ((gb_gcwl_sel & 0x4) | rb_gwln_loc);
                }

                //update token
                if(rb_head_stg != 0x7)
                {
                    gd_gc_token = gd_gc_token + gd_gc_weight;
                }
 
                //move last p2l page from dram to nand
                if((rw_page_loc == (PAGE_QNTY - 1)) && (rb_cech_loc == (RDIE_QNTY - 1)) && (rb_frag_loc == (FRAG_QNTY - 4)))
                {
                    while(rw_cpua_set & CPUA_ACT);

                    //add dummy
                    while(gw_gcp2l_ofs < 0x1000)
                    {
#ifdef FTL_DBG
                        if((gd_gcp2l_ptr & 0xfff) != gw_gcp2l_ofs)
                        {
                            fv_uart_print("gc p2l ptr error2: gd_gcp2l_ptr:%x, gw_gcp2l_ofs:%x\r\n", gd_gcp2l_ptr, gw_gcp2l_ofs);
                            fv_dbg_loop(0x1A);
                        }

                        //write p2l table to dram for debug
                        gd_p2l_tbl[gd_gcp2l_ptr] = P2L_NULL;
                        gd_gcp2l_ptr++;
#endif

                        rd_smdt_dat(gw_gcp2l_bfp, (gw_gcp2l_ofs & 0x3ff)) = P2L_NULL;

                        //move p2l 4KB data from sram to dram
                        if((gw_gcp2l_ofs & 0x3ff) == 0x3ff)
                        {
                            while((rb_nddn_bit & 0xf0) != 0xf0); //check whether last p2l program done
                            while(rb_ddma_act);
                            rd_ddma_dad = ((FTL_GCP2L_BASE | ((gw_gcp2l_ofs << 2) & 0x3000)) >> 4);
                            rd_ddma_sad = ((dwrd)gw_gcp2l_bfp << (13 - 4));
                            rb_ddma_wen = 1;
                            rb_ddma_act = 1;
                            gw_gcp2l_bfp = (gw_gcp2l_bfp == 10) ? 11 : 10;
                        }
                        gw_gcp2l_ofs++;
                    }

                    //move p2l page from dram to nand
                    gw_gcp2l_ofs = 0;
                    lb_p2lp_act = 1;
                }
            } while(lb_p2lp_act || lb_raid_act);
        } //if(lb_olkp_act == 0x0) else

        //clear chkz table
        lb_chkz_sel = (rd_gcrd_pad >> 15) & CHKZ_MSK;
        gw_chkz_cnt[lb_chkz_sel]--;
        gw_gcrd_cnt--;
        if(gb_chkz_fin[lb_chkz_sel] && (gw_chkz_cnt[lb_chkz_sel] == 0))
        {
            lw_cmd_blk = (word)(rd_gcrd_pad >> (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT));
            fv_uart_debug("chk zone fin1: lb_chkz_sel:%d, blk_num:%x, vld cnt: %d\r\n", lb_chkz_sel, lw_cmd_blk, gd_vcnt_tbl[lw_cmd_blk]);
#ifdef FTL_DBG
            {
                dwrd ld_vmap_ptr;
                ld_vmap_ptr = (rd_gcrd_pad >> (5 - CMPR_SHIFT)) & 0xfffffc00;
                do
                {
                    if(gd_vmap_tbl[ld_vmap_ptr] != 0x0)
                    {
                        fv_uart_print("gc vld map sts1: rd_gcrd_pad:%x, ld_vmap_ptr:%x, vldmap:%x\r\n", rd_gcrd_pad, ld_vmap_ptr, gd_vmap_tbl[ld_vmap_ptr]);
                        //fv_dbg_loop(0x1B); //need check gd_gccah_sadr in nand_rw.c
                    }
                    ld_vmap_ptr = ld_vmap_ptr + 1;
                } while((ld_vmap_ptr & 0x3ff) != 0);
            }
#endif

            gb_chkz_fin[lb_chkz_sel] = 0;
            rb_oclr_map |=  (1 << lb_chkz_sel);
            rb_ochk_map &= ~(1 << lb_chkz_sel);
            rd_ochk_bpu(lb_chkz_sel) = 0x0;
        }

        //set token & weight
        if(gb_gcblk_fin)
        {
            gw_gcblk_cnt--;
            if(gw_gcblk_cnt == 0x0)
            {
                gb_gcblk_fin--;
                gw_gcblk_cnt = (gb_gcblk_fin == 1) ? gw_gcblk_tmp : 0x0;
                gb_gcblk_que--;
                if(gb_gcblk_que == 1)
                {
                    if(gd_ht_token < gd_gc_token)
                    {
                        gd_gc_token = gd_gc_token - gd_ht_token;
                        gd_gc_token = gd_gc_token / gd_ht_weight;
                        gd_gc_token = gd_gc_token * gd_ht_tmp;
                        gd_ht_token = 0;
                    }
                    else
                    {
                        gd_ht_token = gd_ht_token - gd_gc_token;
                        gd_ht_token = gd_ht_token / gd_ht_weight + 1;
                        gd_ht_token = gd_ht_token * gd_ht_tmp;
                        gd_gc_token = 0;
                    }
                    gd_ht_weight = gd_ht_tmp;
                    gd_gc_weight = gd_gc_tmp;
                    fv_uart_debug("weight update2: ht_weight:%d, gc_weight:%d\r\n", gd_ht_weight, gd_gc_weight);
                }
            }
        }

        //get gc write cmd
        rb_gcrd_vld = GCRD_CLR; //clear

        //check read unit cnt
        if((lb_olkp_act != 0x0) && (rb_frag_loc == 0x0))
        {
            break;
        }
    } //while(rb_gcrd_vld)

    return;
}
