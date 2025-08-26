#include <global.h>
#include <register.h>

extern dwrd *gd_l2p_tbl;
#ifdef FTL_DBG
extern dwrd *gd_p2l_tbl;
extern dwrd *gd_vmap_tbl;
extern byte *gb_crl_tbl;
#endif

extern dwrd gd_vcnt_tbl[RBLK_QNTY];
extern dwrd gd_ecnt_tbl[RBLK_QNTY];
extern DBL_TBL gs_dbl_tbl[RBLK_QNTY];
extern byte gb_blk_type[RBLK_QNTY];

extern word gw_fblk_str;
extern word gw_fblk_end;
extern word gw_fblk_num;

extern PHY_ADR gs_head_padr[HEAD_QNTY];

#ifdef N4KA_EN
extern byte gb_n4rd_idxq [255];
#endif

//p2l table message
extern dwrd gd_pmsg_pcf[PMSG_NUM]; //page, cech, frag
extern byte gb_pmsg_npg[PMSG_NUM]; //next page unit
extern byte gb_pmsg_epg[PMSG_NUM]; //end page unit

extern byte gb_p2lp_nrd[2];
extern byte gb_p2lp_cnt[2];
extern byte gb_gcad_wat[2];

extern word gw_chkz_cnt[8];
extern byte gb_chkz_fin[8];

word gw_bblk_str;
word gw_bblk_end;

word gw_null_ptr;



void fv_ers_build(void)
{
    byte lb_good_blk, lb_ce, lb_chan, lb_cech;
    word lw_blk_ptr;

    //L2P Table init
    //dwrd ld_tmp_ptr;
    //for(ld_tmp_ptr = 0; ld_tmp_ptr < L2PE_QNTY; ld_tmp_ptr = ld_tmp_ptr + 1)
    //{
    //    gd_l2p_tbl[ld_tmp_ptr] = L2P_NULL;
    //}

    //memset((byte *)(FTL_L2P_BASE), 0x0, L2PE_QNTY*4);

    //use gdma to init l2p table
    rd_gdma_dat = 0x0;
    rq_gdma_len = (qwrd)L2PE_QNTY*4;
    rq_gdma_dad = ((qwrd)FTL_L2P_BASE << 4);
    rw_gdma_set = GDMA_ACT | GDMA_WR;
    while(rb_gdma_exe);

  #ifdef FTL_DBG
    //compression location table init
    memset((byte *)(FTL_CRL_BASE), 0x0, L2PE_QNTY);

    //valid bitmap table init
    for(lw_blk_ptr = 0; lw_blk_ptr < RBLK_QNTY; lw_blk_ptr = lw_blk_ptr + 1)
    {
        memset((byte *)(&gd_vmap_tbl[((dwrd)lw_blk_ptr << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT + CMPR_SHIFT - 5))]), 0x0, (1 << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT + CMPR_SHIFT - 3)));
    }
  #endif

    ARM_NOP();
    ARM_NOP();

    //Valid Cnt Table init
    for(lw_blk_ptr = 0; lw_blk_ptr < RBLK_QNTY; lw_blk_ptr = lw_blk_ptr + 1)
    {
        gd_vcnt_tbl[lw_blk_ptr] = 0x0;
    }
    //memset((byte *)(&gd_vcnt_tbl[0]), 0x0, RBLK_QNTY*4);

    //Erase Cnt Table init
    for(lw_blk_ptr = 0; lw_blk_ptr < RBLK_QNTY; lw_blk_ptr = lw_blk_ptr + 1)
    {
        gd_ecnt_tbl[lw_blk_ptr] = 0x0;
    }
    //memset((byte *)(&gd_ecnt_tbl[0]), 0x0, RBLK_QNTY*4);


    //erase build table
    gw_fblk_str = DBLK_INVLD;
    gw_bblk_str = DBLK_INVLD;

    for(lw_blk_ptr = 1; lw_blk_ptr < RBLK_QNTY; lw_blk_ptr = lw_blk_ptr + 1)
    {
        lb_good_blk = 1;

        for(lb_ce = 0; lb_ce < BANK_QNTY; lb_ce = lb_ce + 1)
        {
            for(lb_chan = 0; lb_chan < CHAN_QNTY; lb_chan = lb_chan + 1)
            {
                lb_cech = (lb_ce << CHAN_SHIFT) | lb_chan;
                rd_cmd_padr = ((dwrd)lw_blk_ptr << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT)) | ((dwrd)lb_cech << FRAG_SHIFT);
                fv_nque_chk(20);
                rw_cmd_type = ERS_CPTR | LOW_PRI | CMD_END;
            }
        }

        for(lb_ce = 0; lb_ce < BANK_QNTY; lb_ce = lb_ce + 1)
        {
            for(lb_chan = 0; lb_chan < CHAN_QNTY; lb_chan = lb_chan + 1)
            {
                //check erase status
                rb_index = phy_chan_map(lb_chan);
                while(rb_cmd_sts != ALL_FIN);
                while(rw_lowq_vld != ALL_INVLD);
                while(rw_que_rdy != ALL_READY);
                while(rb_qabt_fsm != IDLE_ST);
                while((rw_sts_rdy & (1 << lb_ce)) == 0);

                if(rw_sts_fail & (1 << lb_ce)) //bad block
                {
                    fv_uart_print("bad block: ce:%x, chan:%x, blk_ptr:%x, sts:%x\r\n", lb_ce, lb_chan, lw_blk_ptr, rd_nrd_dat);

                    //change H/W, write 1 clear
                    //rw_sts_fail = rw_sts_fail & ~(1 << lb_ce); //clear erase fail status
                    rw_sts_fail = rw_sts_fail & (1 << lb_ce); //clear erase fail status
                    lb_good_blk = 0;
                }

                if((lb_ce == (BANK_QNTY - 1)) && (lb_chan == (CHAN_QNTY - 1)))
                {
                    if(lb_good_blk == 0)
                    {
                        if(gw_bblk_str == DBLK_INVLD)
                            gw_bblk_str = lw_blk_ptr;
                        else
                            gs_dbl_tbl[gw_bblk_end].wd.w_next_ptr = lw_blk_ptr;

                        gw_bblk_end = lw_blk_ptr;
                        gs_dbl_tbl[lw_blk_ptr].wd.w_next_ptr = DBLK_INVLD;
                        gb_blk_type[lw_blk_ptr] = BAD_BLK;
                    }
                    else
                    {
                        //if(gw_fblk_num[MQ_TABLE] != TBL_BLOCK_QNTY)
                        //{
                        //    gw_fblk_num[MQ_TABLE] = gw_fblk_num[MQ_TABLE] + 0x1;
                        //}
                        //else
                        //{
                        //    if(gw_fblk_num[MQ_NDATA] != FREE_NSPD_THR)
                        //        gw_fblk_num[MQ_NDATA] = gw_fblk_num[MQ_NDATA] + 0x1;
                        //    else if(gwHSpdBlkNum != FREE_HSPD_THR)
                        //        gw_fblk_num[MQ_HDATA] = gw_fblk_num[MQ_HDATA] + 0x1;
                        //    else
                        //        gw_fblk_num[MQ_NDATA] = gw_fblk_num[MQ_NDATA] + 0x1;
                        //}
                        gw_fblk_num = gw_fblk_num + 0x1;

                        //if(gw_fblk_str == DBLK_INVLD)
                        //    gw_fblk_end = lw_blk_ptr;
                        //else
                        //    gs_dbl_tbl[lw_blk_ptr].wd.w_next_ptr = gw_fblk_str;

                        //gw_fblk_str = lw_blk_ptr;


                        if(gw_fblk_str == DBLK_INVLD)
                            gw_fblk_str = lw_blk_ptr;
                        else
                            gs_dbl_tbl[gw_fblk_end].wd.w_next_ptr = lw_blk_ptr;

                        gw_fblk_end = lw_blk_ptr;
                        gb_blk_type[lw_blk_ptr] = FREE_BLK;
                    }
                }
            }
        }


        if(gw_fblk_num == VBLK_QNTY)
        {
            fv_uart_print("free blk num reach the reserved rate, stop\r\n");

            lw_blk_ptr = lw_blk_ptr + 1;
            while(lw_blk_ptr < RBLK_QNTY)
            {
                if(gw_bblk_str == DBLK_INVLD)
                    gw_bblk_str = lw_blk_ptr;
                else
                    gs_dbl_tbl[gw_bblk_end].wd.w_next_ptr = lw_blk_ptr;

                gw_bblk_end = lw_blk_ptr;
                gs_dbl_tbl[lw_blk_ptr].wd.w_next_ptr = DBLK_INVLD;
                lw_blk_ptr = lw_blk_ptr + 1;
            }

            break;
        }
    }

    fv_uart_print("free blk num:%d\r\n",gw_fblk_num);

    return;
}


void fv_gctbl_init(void)
{
    byte lb_head_wcnt = 0x0;
#if (defined(NAND_BICS5) || defined(NAND_X39070))
    byte lb_head_wend = 0x2;
#else
    byte lb_head_wend = 0x0;
#endif
    byte lb_head_frag = 0x0;
    byte lb_head_cech = 0x0;
    word lw_head_page = 0x0;
    word lw_mapu_cnt = 0;
    word lw_pmsg_ptr = 0;
    word lw_next_page;
    word lw_end_page;
    word lw_min_page;
    word lw_max_page;
    word lw_tmp_page;

    //calculate gc table
    lw_next_page = 0;
    lw_end_page = (1 << (15 - BKCH_SHIFT - FRAG_SHIFT));
    lw_min_page = 0;
    lw_max_page = 0;
    lw_tmp_page = 0;
    while(1)
    {
        if(lb_head_frag != (FRAG_QNTY - 1))
        {
            lb_head_frag++;
        }
        else
        {
            lb_head_frag = 0x0;
            if(lb_head_wcnt != lb_head_wend)
            {
                lw_head_page++;
                lb_head_wcnt++;
                if(lw_head_page > lw_tmp_page)
                {
                    lw_tmp_page = lw_head_page;
                }
            }
            else
            {
                lb_head_wcnt = 0x0;
                if(lb_head_cech != (RDIE_QNTY - 1))
                {
                    lb_head_cech++;
                    lw_head_page = lw_head_page - (word)lb_head_wend;
                }
                else
                {
                    lb_head_cech = 0x0;
                    if(lw_head_page != (PAGE_QNTY - 1))
                    {
                        lw_head_page++;
                    }
                    else
                    {
                        if(lw_pmsg_ptr != PMSG_END)
                        {
                            fv_uart_print("p2l page location error\r\n");
                            fv_dbg_loop(0x11);
                        }
                        gd_pmsg_pcf[lw_pmsg_ptr] = ((((PAGE_QNTY - 1) << BKCH_SHIFT) | (RDIE_QNTY - 1)) << FRAG_SHIFT) | (FRAG_QNTY - 4);
                        gb_pmsg_npg[lw_pmsg_ptr] = 0;
                        gb_pmsg_epg[lw_pmsg_ptr] = 1;

                        break;
                    }

#if (!defined(SLC_MODE) && defined(NAND_B47R))
                    if((lw_head_page < 4) || (lw_head_page >= 2108))
                        lb_head_wend = 0x0;
                    else if((lw_head_page >= 1048) && (lw_head_page < 1064))
                        lb_head_wend = 0x1;
                    else
                        lb_head_wend = 0x2;
#endif

                    lw_min_page = lw_head_page;
                    lw_tmp_page = lw_head_page;
                }
            }
        }

        lw_mapu_cnt = (lw_mapu_cnt < 4096) ? (lw_mapu_cnt + CMPR_NUM) : (lw_mapu_cnt + 1);
        //fv_uart_print("%d, %x, %x, %d\r\n", lw_mapu_cnt, lb_head_frag, lb_head_cech, lw_head_page);
        if(lw_mapu_cnt == 4096)
        {
            gd_pmsg_pcf[lw_pmsg_ptr] = ((((dwrd)lw_head_page << BKCH_SHIFT) | (dwrd)lb_head_cech) << FRAG_SHIFT) | (dwrd)lb_head_frag;

            if(lw_max_page < lw_next_page)
            {
                gb_pmsg_npg[lw_pmsg_ptr] = 0;
            }
            else
            {
                gb_pmsg_npg[lw_pmsg_ptr] = 1;
                lw_next_page = lw_next_page + (1 << (15 - BKCH_SHIFT - FRAG_SHIFT));
            }
        }
        else if(lw_mapu_cnt == (4096 + 4))
        {
            lw_mapu_cnt = 0x0;

	    if(lw_min_page < lw_end_page)
            {
                gb_pmsg_epg[lw_pmsg_ptr] = 0;
            }
            else
            {
                gb_pmsg_epg[lw_pmsg_ptr] = 1;
                lw_end_page = lw_end_page + (1 << (15 - BKCH_SHIFT - FRAG_SHIFT));
            }

            lw_pmsg_ptr++;
        }

        lw_max_page = lw_tmp_page;

    } //while(1)

    return;
} 

void fv_ftl_pre(void)
{
    byte lb_cnt;

    gd_l2p_tbl = (dwrd *)(FTL_L2P_BASE);
#ifdef FTL_DBG
    gd_p2l_tbl = (dwrd *)(FTL_P2L_BASE);
    gd_vmap_tbl = (dwrd *)(FTL_VMAP_BASE);
    gb_crl_tbl = (byte *)(FTL_CRL_BASE);
#endif

    for(lb_cnt = 0; lb_cnt < 2; lb_cnt = lb_cnt + 1)
    {
        gb_p2lp_nrd[lb_cnt] = 0;
        gb_p2lp_cnt[lb_cnt] = 0;
        gb_gcad_wat[lb_cnt] = 0;
    }

    for(lb_cnt = 0; lb_cnt < 8; lb_cnt = lb_cnt + 1)
    {
        gw_chkz_cnt[lb_cnt] = 0;
        gb_chkz_fin[lb_cnt] = 0;
    }

    for(lb_cnt = 0; lb_cnt < HEAD_QNTY; lb_cnt = lb_cnt + 1)
    {
        gs_head_padr[lb_cnt].sb_head_wcnt = 0x0;
#if (defined(NAND_BICS5) || defined(NAND_X39070))
        gs_head_padr[lb_cnt].sb_head_wend = 0x2;
#else
        gs_head_padr[lb_cnt].sb_head_wend = 0x0;
#endif
        gs_head_padr[lb_cnt].sb_head_frag = 0x0;
        gs_head_padr[lb_cnt].sb_head_cech = 0x0;
        gs_head_padr[lb_cnt].sw_head_page = 0x0;
        //gs_head_padr[lb_cnt].sw_head_blk  = 0x0;
    }

#ifdef N4KA_EN
    for(lb_cnt = 0; lb_cnt < 255; lb_cnt = lb_cnt + 1)
    {
        gb_n4rd_idxq[lb_cnt] = lb_cnt;
    }
#endif

    //initial dram base address
    rw_dmdt_bas = ((DRAM_DATA_BASE & 0x7fffffff) >> (12 + 12));
    rw_dmmt_bas = ((DRAM_META_BASE & 0x7fffffff) >> (12 + 8));

    //initial nptr table
    rb_itbl_set = ITBL_ACT;
    while(rb_itbl_set & ITBL_ACT);

#ifdef WAIT_AXIW
    rb_wait_wok = 1;
#else
    rb_wait_wok = 0;
#endif

#ifdef LGET_EN
    rb_lget_set = LGET_ACT;  //enable HW auto get l2p function
#endif

#ifdef LUPD_EN
    rw_fcfg_set |= GCWR_ADD; //enable HW update "gc write info" into "BMU buffer table"
    rb_hacc_fun |= BSTS_CHK; //gc write would check buffer status
#endif

#ifdef HWCMD_CHK
    rw_fcfg_set |= CACQ_ALL; //check the bmu handle cmd
#endif

#ifdef RCAH_FUNC
    rb_hacc_fun |= QSWH_EN | RCAH_EN;  //enable read cache function
#endif

#ifdef AIPR_FUNC
    rb_hacc_fun |= AIPR_EN;  //enable aipr function
#endif

    //set bmu status table bit write mask
    rq_tmem_ben = ((qwrd)BYTE_MASK << 1);

    //clear overwrite bimap
    rb_oclr_map = 0xff;
    while(rb_oclr_map);

    //get free sram buffer ptr
    for(lb_cnt = 0; lb_cnt < 12; lb_cnt = lb_cnt + 1)
    {
        rb_cafp_set = CAFP_ACT | SRAM_BUF | CPUA_GET;
        while(rb_cafp_set & CAFP_ACT);
        if(rb_cafp_rlt == 0)
        {
            fv_uart_print("free buffer error\r\n");
            fv_dbg_loop(0x05);
        }
    }

    //buffer allocate threshold
#ifdef HTWR_DBUF
    rw_htdl_thr = 200;
    rw_htdb_thr = 300;
    rw_htsl_thr = 100;
    rw_htsb_thr = 150;
    rw_htpl_thr = 0;
    rw_htpb_thr = 100;
#endif
    // rw_hrsl_thr = 40;
    // rw_hrsb_thr = 140;
    // rw_hrpb_thr = 100;

    //set buffer ready
    rb_dbuf_rdy = 1;
    rb_sbuf_rdy = 1;
    rb_pbuf_rdy = 1;

    //gc table initial
    fv_gctbl_init();

    return;
}

void fv_set_null(void)
{
    dwrd ld_bmu_base;

    //get free buffer ptr
    do
    {
        rb_cafp_set = CAFP_ACT | SRAM_BUF | CPUA_GET;
        while(rb_cafp_set & CAFP_ACT);
    } while(rb_cafp_rlt == 0);

    gw_null_ptr = rw_cafp_ptr;

    //set null pattern
    ld_bmu_base = SRAM_MEM_BASE  + ((dwrd)(gw_null_ptr & BPTR_MSK) << 12);
    memset((byte *)(ld_bmu_base), 0x0, 4096);
    *(volatile dwrd *)(SRAM_META_BASE + ((gw_null_ptr & BPTR_MSK) << 12)) = 0x38e3ffee; //CRC of zero payload

    return;
}

void fv_ftl_init(void)
{
    //pre-initial table variable
    fv_ftl_pre();

    //set null data pattern
    fv_set_null();

//#if (defined(QUICK_TABLE_FUN) || defined(PARTIAL_TABLE_FUN))
//
//#ifdef BAD_REMAP_FUN
//
//    //remap table
//    vFtlRemapTable();
//
//#endif
//
//    //build bad block bit map table
//    vFtlBadBlockBuild();
//
//#endif


//#ifdef QUICK_TABLE_FUN
//
//    //read table from NAND directly and check QBT_LABEL
//    if(bFtlQuickBuild(1))
//    {
//        fv_uart_print("quick build table sucessful\r\n");
//        return;
//    }
//    else
//    {
//        fv_uart_print("quick build table fail\r\n");
//    }
//
//#endif


#ifdef PARTIAL_TABLE_FUN

//    fv_uart_print("=======> %d.%d secs\r\n", rdwSystemTimer/CONFIG_TMR_TICK_HZ, 10*(rdwSystemTimer%CONFIG_TMR_TICK_HZ)/CONFIG_TMR_TICK_HZ);
//
//    //Serial Number Sorting
//    vFtlSNSort();
//
//    fv_uart_print("=======> %d.%d secs\r\n", rdwSystemTimer/CONFIG_TMR_TICK_HZ, 10*(rdwSystemTimer%CONFIG_TMR_TICK_HZ)/CONFIG_TMR_TICK_HZ);
//
//    //read spare byte to build Index table
//    vFtlIndexBuild();
//
//    fv_uart_print("=======> %d.%d secs\r\n", rdwSystemTimer/CONFIG_TMR_TICK_HZ, 10*(rdwSystemTimer%CONFIG_TMR_TICK_HZ)/CONFIG_TMR_TICK_HZ);
//
//    //read all table block to build VldCnt table
//    vFtlVldCntBuild();
//
//    fv_uart_print("=======> %d.%d secs\r\n", rdwSystemTimer/CONFIG_TMR_TICK_HZ, 10*(rdwSystemTimer%CONFIG_TMR_TICK_HZ)/CONFIG_TMR_TICK_HZ);
//
//    //read data page to build Cache table
//    vFtlCacheBuild();
//
//    fv_uart_print("=======> %d.%d secs\r\n", rdwSystemTimer/CONFIG_TMR_TICK_HZ, 10*(rdwSystemTimer%CONFIG_TMR_TICK_HZ)/CONFIG_TMR_TICK_HZ);

#else

    //erase build table
    fv_ers_build();

#endif


    //check the relationship of all table
    //vFtlTableCheck();

    return;
}

