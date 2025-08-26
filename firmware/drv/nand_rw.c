#include <global.h>
#include <register.h>


extern word gw_null_ptr;

extern byte gb_htwl_sel;
extern byte gb_gcwl_sel;

byte gb_gccmd_wcnt = 0;
dwrd gd_gccah_bptr[FRAG_QNTY*WLPG_QNTY];
dwrd gd_gccah_padr[FRAG_QNTY*WLPG_QNTY];

byte gb_rdec_wpt = 0x0;
byte gb_rdec_rpt = 0x0;
dwrd gd_rdec_cid[RDEC_NUM][4];

#ifdef RDEC_TEST
extern PHY_ADR gs_head_padr[HEAD_QNTY];
byte gb_rerr_cnt = 0x0;
#endif


//send erase cmd to nand ctl
void fv_nand_erase(word iw_head_blk)
{
    byte lb_ce, lb_chan, lb_cech;

    for(lb_ce = 0; lb_ce < BANK_QNTY; lb_ce = lb_ce + 1)
    {
        for(lb_chan = 0; lb_chan < CHAN_QNTY; lb_chan = lb_chan + 1)
        {
            lb_cech = (lb_ce << CHAN_SHIFT) | lb_chan;
            rd_cmd_padr = ((dwrd)iw_head_blk << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT)) | ((dwrd)lb_cech << FRAG_SHIFT);
            fv_nque_chk(0);
            rw_cmd_type = ERS_CPTR | LOW_PRI | CMD_END;
        }
    }

    return;
}

//send read cmd to nand ctl
void fv_nand_read(void)
{
    if(rs_host_cmd.sd_cmd_padr == L2P_NULL)
    {
        while(rd_cawf_set & CAWF_ACT);
        *(volatile dwrd *)(SRAM_META_BASE + ((gw_null_ptr & BPTR_MSK) << 12) + 4) = rs_host_cmd.sd_cmd_ladr; //write L4K to host meta, for internal PI check
        rd_cawf_cid(0) = rs_host_cmd.sd_cmd_cid;
        rd_cawf_set = CAWF_ACT | CAWF_KEP | CAWF_RFN | CAWF_WAT | CAWF_MSZ | ((dwrd)gw_null_ptr << 16);
    }
#ifdef LUPD_EN
    else if(rs_host_cmd.sw_cmd_bfp != BPTR_INVLD)
    {

        if((rs_host_cmd.sb_cmd_crd == 0) && (rs_host_cmd.sb_cmd_crl != 0))
        {
            fv_uart_print("gc error\r\n");
            fv_dbg_loop(0x19);
        }

        while(rd_cawf_set & CAWF_ACT);
        rd_cawf_cid(rs_host_cmd.sb_cmd_crl) = rs_host_cmd.sd_cmd_cid;
        rd_cawf_set = CAWF_ACT | ((dwrd)rs_host_cmd.sb_cmd_crd << 1) | CAWF_RLS | CAWF_RFB | CAWF_MSZ | ((dwrd)rs_host_cmd.sw_cmd_bfp << 16);
    }
#endif
    else
    {
#ifdef RDEC_TEST
        word lw_cmd_blk;
        lw_cmd_blk = (word)(rs_host_cmd.sd_cmd_padr >> (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT));
        if((gb_rerr_cnt < RERR_THR) &&
           ((((rs_host_cmd.sd_cmd_padr >> (BKCH_SHIFT + FRAG_SHIFT)) & PAGE_MASK) + RDEC_GAP) < ((lw_cmd_blk == gs_head_padr[0].sw_head_blk) ? gs_head_padr[0].sw_head_page :
                                                                                                 ((lw_cmd_blk == gs_head_padr[GC_HLOC].sw_head_blk) ? gs_head_padr[GC_HLOC].sw_head_page : PAGE_QNTY))))
        {
            rb_cmd_lidx = LIDX_AERR;
            gb_rerr_cnt++;
        }
#endif
#ifdef LDPC_MS
        rb_cmd_ltyp = ((rs_host_cmd.sd_cmd_ladr & 0x7) == 0) ? 0 : 1;
#endif
#ifdef ERR_INJECT
        rb_cmd_misc = 0x02;
#endif
        rd_cmd_cid(0) = rs_host_cmd.sd_cmd_cid;
        rd_cmd_padr = rs_host_cmd.sd_cmd_padr;
        fv_nque_chk(1);
        rw_cmd_bptr = BPTR_INVLD;
#ifdef AIPR_FUNC
        rw_cmd_type = AIPR_CPTR | AIPR_PRI;
#else
        rw_cmd_type = READ_CPTR | LOW_PRI;
#endif
    }

    return;
}

//send non-4KB align read cmd to nand ctl
void fv_nand_n4k(byte ib_n4rd_idx, byte ib_cmd_hit)
{
#ifdef LDPC_MS
    rb_cmd_ltyp = 0;
#endif
#ifdef ERR_INJECT
    rb_cmd_misc = 0x02;
#endif
    rb_cmd_qidx = QLNK_N4K;
    rd_cmd_cid(0) = (dwrd)ib_n4rd_idx;
    rd_cmd_padr = rs_host_cmd.sd_cmd_padr;
    fv_nque_chk(2);
    rw_cmd_bptr = rs_host_cmd.sw_cmd_bfp;
#ifdef AIPR_FUNC
    if(ib_cmd_hit == 0)
    {
        rw_cmd_type = AIPR_CPTR | AIPR_PRI | CMD_END;
    }
    else
#endif
    {
        rw_cmd_type = READ_CPTR | LOW_PRI | CMD_END;
    }

    return;
}


#ifndef CPUA_PROC

//send host program cmd to nand ctl
void fv_nand_htprg(byte ib_cmd_end, dwrd id_cmd_bptr, dwrd id_cmd_padr)
{
    rb_nblk_idx = rb_head_idx;
    if((id_cmd_bptr >> 12) != 0x0) //program p2l
    {
        rb_cmd_qidx = QLNK_TRQ;
        rw_cmd_othr = (DBUF_KEEP << 8) | (word)(id_cmd_padr & 0x3);
        rw_cmd_dmpt = (word)(id_cmd_bptr >> 12);
        rw_cmd_bptr = (word)(id_cmd_bptr & 0xfff);
    }
    else
    {
        rb_cmd_qidx = QLNK_HTQ;
        rw_cmd_bptr = (word)id_cmd_bptr;
    }

    rd_cmd_padr = id_cmd_padr;
    fv_nque_chk(3);
    rw_cmd_type = PROG_CPTR | LOW_PRI | CMD_BUF | (ib_cmd_end ? CMD_END : 0);

    return;
}

#endif

//send gc program cmd to nand ctl
void fv_nand_gcprg(byte ib_cmd_end, dwrd id_cmd_bptr, dwrd id_cmd_padr)
{
    rb_nblk_idx = rb_head_idx;
    if((id_cmd_bptr >> 12) != 0x0) //program p2l
    {
        rb_cmd_qidx = QLNK_TRQ;
        rw_cmd_othr = (DBUF_KEEP << 8) | 0x4 | (word)(id_cmd_padr & 0x3);
        rw_cmd_dmpt = (word)(id_cmd_bptr >> 12);
        rw_cmd_bptr = (word)(id_cmd_bptr & 0xfff);
    }
    else
    {
        rb_cmd_qidx = QLNK_GCQ;
        rw_cmd_bptr = (word)id_cmd_bptr;
    }

    rd_cmd_padr = id_cmd_padr;
    fv_nque_chk(4);
    rw_cmd_type = PROG_CPTR | LOW_PRI | CMD_BUF | (ib_cmd_end ? CMD_END : 0);

    return;
}


//raid decoder read
void fv_rdec_read(void)
{
    byte lb_bkch;
    word lw_rdec_rid;

    //get free bfp
    do
    {
        rb_cafp_set = CAFP_ACT | PRTY_BUF | CPUA_GET;
        while(rb_cafp_set & CAFP_ACT);
    } while(rb_cafp_rlt == 0);
    rw_rtbl_r6p = rw_cafp_ptr;

    //write raid table
    lw_rdec_rid = 0x1C0 | (word)gb_rdec_wpt;
    rw_rtbl_r5p = rw_errd_ptr;
    rw_rtbl_set = RTBL_ACT | RTBL_WR | lw_rdec_rid;

    //send nand read cmd
    for(lb_bkch = 0; lb_bkch < RAID_LST1; lb_bkch = lb_bkch + 1)
    {
        if(lb_bkch != (byte)((rd_errd_pad >> FRAG_SHIFT) & BKCH_MASK))
        {
            rw_cmd_ridx = lw_rdec_rid;
            rb_cmd_qidx = QLNK_RAQ;
            rb_cmd_rtp = (lb_bkch == RAID_LST2) ? 0x1 : ((lb_bkch == RAID_LST1) ? 0x2 : 0x3);
            rd_cmd_padr = (rd_errd_pad & ~(BKCH_MASK << FRAG_SHIFT)) | ((dwrd)lb_bkch << FRAG_SHIFT);
            rw_cmd_bptr = BPTR_INVLD;
            rw_cmd_type = READ_CPTR | LOW_PRI | CMD_END;
        }
    }

    //record raid decoder l4k
    gd_rdec_cid[gb_rdec_wpt][0] = rd_errd_cid(0);
    gd_rdec_cid[gb_rdec_wpt][1] = rd_errd_cid(1);
    gd_rdec_cid[gb_rdec_wpt][2] = rd_errd_cid(2);
    gd_rdec_cid[gb_rdec_wpt][3] = rd_errd_cid(3);

    //next rdec wr ptr
    gb_rdec_wpt = (gb_rdec_wpt + 0x1) & ~(BYTE_MASK << RDEC_SFT);

    return;
}

void fv_rdec_send(void)
{
    word lw_rdec_rid, lw_rdec_r5p, lw_rdec_r6p;

    while(gb_rdec_rpt != gb_rdec_wpt)
    {
        //check whether raid decode read finish
        lw_rdec_rid = 0x1C0 | (word)gb_rdec_rpt;
        rw_rsts_ptr = lw_rdec_rid;
        if(rb_rbuf_rdy)
        {
            //read raid table
            rw_rtbl_set = RTBL_ACT | RTBL_RD | lw_rdec_rid;
            while(rw_rtbl_set & RTBL_ACT);
            lw_rdec_r5p = (SMDT_BASE << BPTR_SFT) | rw_rtbl_r5p;
            lw_rdec_r6p = (SMDT_BASE << BPTR_SFT) | rw_rtbl_r6p;

            //wait last cawf done
            while(rd_cawf_set & CAWF_ACT);

            //send decoder result
            rd_cawf_cid(0) = gd_rdec_cid[gb_rdec_rpt][0];
            rd_cawf_set = CAWF_ACT | CAWF_RLS | CAWF_RFN | CAWF_MSZ | ((dwrd)lw_rdec_r5p << 16);

            //release raid6 bfp
            rd_carl_set = ((dwrd)lw_rdec_r6p << 16) | CARL_ACT;

            //next rdec rd ptr
            gb_rdec_rpt = (gb_rdec_rpt + 0x1) & ~(BYTE_MASK << RDEC_SFT);

#ifdef RDEC_TEST
            //set error injection cnt
            gb_rerr_cnt--;
#endif
        }
        else if(gb_rdec_rpt != ((gb_rdec_wpt + 0x1) & ~(BYTE_MASK << RDEC_SFT)))
        {
            break;
        }
    }

    return;
}
