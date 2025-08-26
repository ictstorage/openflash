#include <global.h>
#include <register.h>

#include "./micro_code"
#ifdef CPUA_PROC
#include "./cpua_code"
#endif

void fv_nand_init(void)
{
    word lw_cnt0, lw_cnt1, lw_cnt2;
    word lw_addr, lw_data;

/**************************        controller initial        **************************/
#ifdef RTL_SIM
    //for FPGA, change PHY sampling phase
    rb_nand_phs = FPGA_PHS;
#else
//nphy phase calibration
#ifdef NAND_HSPD
    //------------ ch0  ch1  ch2  ch3  ch4  ch5  ch6  ch7
#ifdef FPGA_SORTING    
    word np[] = {0x1, 0x1, 0x1, 0x1, 0x5, 0x2, 0x6, 0x1};// socket 250M
#else
    word np[] = {0x2, 0x1, 0x1, 0x1, 0x3, 0x2, 0x1, 0x1};// AIC 4M sram 1 Die
#endif
#else
#ifdef NAND_BICS5
    word np[] = {0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4};//low speed all 0 or 4
#else
    word np[] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};//low speed all 0 or 4
#endif
#endif

    for(lw_cnt0 = 0; lw_cnt0 < CHAN_QNTY; lw_cnt0++)
    {
        rb_index = lw_cnt0;                               //set chan
        rd_nphy_phs = (lw_cnt0 == 0) ? ((np[0] << 21) | (np[0] << 18) | (np[0] << 15) | (np[0] << 12) | (np[0] << 9) | (np[0] << 6) | (np[0] << 3) | (np[0] << 0)) :
                      (lw_cnt0 == 1) ? ((np[1] << 21) | (np[1] << 18) | (np[1] << 15) | (np[1] << 12) | (np[1] << 9) | (np[1] << 6) | (np[1] << 3) | (np[1] << 0)) :
                      (lw_cnt0 == 2) ? ((np[2] << 21) | (np[2] << 18) | (np[2] << 15) | (np[2] << 12) | (np[2] << 9) | (np[2] << 6) | (np[2] << 3) | (np[2] << 0)) :
                      (lw_cnt0 == 3) ? ((np[3] << 21) | (np[3] << 18) | (np[3] << 15) | (np[3] << 12) | (np[3] << 9) | (np[3] << 6) | (np[3] << 3) | (np[3] << 0)) :
                      (lw_cnt0 == 4) ? ((np[4] << 21) | (np[4] << 18) | (np[4] << 15) | (np[4] << 12) | (np[4] << 9) | (np[4] << 6) | (np[4] << 3) | (np[4] << 0)) :
                      (lw_cnt0 == 5) ? ((np[5] << 21) | (np[5] << 18) | (np[5] << 15) | (np[5] << 12) | (np[5] << 9) | (np[5] << 6) | (np[5] << 3) | (np[5] << 0)) :
                      (lw_cnt0 == 6) ? ((np[6] << 21) | (np[6] << 18) | (np[6] << 15) | (np[6] << 12) | (np[6] << 9) | (np[6] << 6) | (np[6] << 3) | (np[6] << 0)) :
                      ((np[7] << 21) | (np[7] << 18) | (np[7] << 15) | (np[7] << 12) | (np[7] << 9) | (np[7] << 6) | (np[7] << 3) | (np[7] << 0));
    }
#endif

    //meta/parity mode
    rb_cmd_mtsz = (FMT_MODE << 3) | HMT_MODE;
    rb_cmd_ptsz = LDPC_MODE;

#ifdef NAND_X39070
    rb_frag_end = (1 << 6) | (FRAG_QNTY - 1);
#endif

    //forc FPGA, change cmd_add timeout threshold
    rd_tout_thr = 0x1000;

    //set NIC initial register
#ifdef ONFI_MODE
  #if (defined(NAND_2400M))
    rb_tcr     = 5;
    rb_tcs     = 11;
    rb_tcs1    = 17;
    rb_tcd     = 59;
    rb_tcsd    = 5;
    rb_tceh    = 11;
    rb_tclr    = 5;
    rb_tar     = 5;
    rb_tcals2  = 14;
    rb_twhr    = 47;
    rb_twc     = 5;
    rb_twh     = 2;
    rb_tch     = 2;
    rb_tadl    = 179;
    rb_trhw    = 59;
    rb_tcdqss  = 17;
    //rb_trpre   = 8;
    rb_trpre   = 14;
    rb_twpre   = 8;
    //rb_tcals   = 1;
    rb_tcals   = 8;
    //rb_tcalh   = 1;
    rb_tcalh   = 2;
    //rb_twp     = 2;
    rb_twp     = 6;
    //rb_tcas    = 1;
    rb_tcas    = 2;
    rb_tdqsrh  = 2;
    rb_trpst   = 15;
    rb_trpsth  = 8;
    rb_tchz    = 17;
    rb_tclhz   = 17;
    rb_twpst   = 3;
    rb_twpsth  = 14;
    rb_tcdqsh  = 59;
    rb_tcres   = 5;
    rb_tdbs    = 5;
  #elif (defined(NAND_1600M))
    rb_tcr     = 7;
    rb_tcs     = 15;
    rb_tcs1    = 23;
    rb_tcd     = 79;
    rb_tcsd    = 7;
    rb_tceh    = 15;
    rb_tclr    = 7;
    rb_tar     = 7;
    rb_tcals2  = 19;
    rb_twhr    = 63;
    rb_twc     = 19;
    rb_twh     = 8;
    rb_tch     = 3;
    rb_tadl    = 119;
    rb_trhw    = 79;
    rb_tcdqss  = 23;
    rb_trpre   = 14;
    rb_twpre   = 11;
    rb_tcals   = 11;
    rb_tcalh   = 3;
    rb_twp     = 8;
    rb_tcas    = 3;
    rb_tdqsrh  = 3;
    rb_trpst   = 20;
    rb_trpsth  = 11;
    rb_tchz    = 23;
    rb_tclhz   = 23;
    rb_twpst   = 4;
    rb_twpsth  = 19;
    rb_tcdqsh  = 79;
    rb_tcres   = 3;
    rb_tdbs    = 3;
  #elif (defined(NAND_500M)) //500M for ku15p
    rb_tcr     = 2;
    rb_tcs     = 4;
    rb_tcs1    = 7;
    rb_tcd     = 24;
    rb_tcsd    = 2;
    rb_tceh    = 4;
    rb_tclr    = 2;
    rb_tar     = 2;
    rb_tcals2  = 6;
    rb_twhr    = 19;
    rb_twc     = 6;
    rb_twh     = 2;
    rb_tch     = 1;
    rb_tadl    = 30;
    rb_trhw    = 20;
    rb_tcdqss  = 6;
    rb_trpre   = 4;
    rb_twpre   = 3;
    rb_tcals   = 3;
    rb_tcalh   = 1;
    rb_twp     = 2;
    rb_tcas    = 1;
    rb_tdqsrh  = 1;
    rb_trpst   = 6;
    rb_trpsth  = 3;
    rb_tchz    = 7;
    rb_tclhz   = 7;
    rb_twpst   = 1;
    rb_twpsth  = 6;
    rb_tcdqsh  = 20;
    rb_tcres   = 1;
    rb_tdbs    = 1;
  #endif
    rb_tchk_en = 0xf;

    rb_nand_md = 0x0;                    //set nand mode as ddr mode
#else
    rb_nand_md = LEGACY_MD;              //set nand mode as legacy mode
#endif

    rb_inst_share = INST_SHARE;          //share instruction SRAM

    rb_frag_sft  = FRAG_SHIFT;
    rb_chan_sft  = CHAN_SHIFT;
    rb_ce_sft    = PCE_SHIFT;
    rb_lunce_sft = BANK_SHIFT;
    rb_page_sft  = PAGE_SHIFT;
    rb_blk_sft   = BLK_SHIFT;
    rb_plane_sft = PLANE_SHIFT;
    rb_plane_num = PLANE_QNTY;
    rw_mapu_size = MAPU_SIZE;
    rw_pg_size   = PG_SIZE;
    rb_nrd_ten   = 1;
    rw_nrd_thr   = 500;
    rd_phy_chan  = (0x7 << 21) | (0x6 << 18) | (0x5 << 15) | (0x4 << 12) | (0x3 << 9) | (0x2 << 6) | (0x1 << 3) | (0x0 << 0);
    //rd_phy_chan  = (0x6 << 21) | (0x3 << 18) | (0x7 << 15) | (0x5 << 12) | (0x4 << 9) | (0x2 << 6) | (0x1 << 3) | (0x0 << 0);
    rd_phy_ce0   = 0x76543210;
    //rd_phy_ce0   = 0x76503214;
    rd_phy_ce1   = 0xfedcba98;
    rb_sts_cfg   = 0x0;
//    rb_fail_mask = 0x0;

    rw_tmr_base = 16;
    rw_tmr_data0 = 48;
    rw_tmr_data1 = 5500;
    rw_tmr_data2 = 1800;
    rw_tmr_data3 = 4;

    rb_mapu_sft_h = 2;
    rb_plan_sft_h = PLANE_SHIFT;
    rb_chan_sft_h = CHAN_SHIFT;
    rb_lnce_sft_h = BANK_SHIFT;
    rb_page_sft_h = PAGE_SHIFT;
    rb_cmpr_sft_h = (CMPR_SHIFT != 0) ? 1 : 0;
    rb_p4kj_che_h = (1 << 3) | (CHAN_QNTY - 1);
    rb_gcad_che_h = (1 << 3) | (CHAN_QNTY - RAID_PNUM - 1);
    // rb_gcp4kj_che_h = (1 << 3) | (CHAN_QNTY - RAID_PNUM - 1);
    rb_mapu_end_h = 3;
    rb_plan_end_h = PLANE_QNTY - 1;
    rb_chan_end_h = CHAN_QNTY - 1;
    rb_lnce_end_h = BANK_QNTY - 1;
    rw_page_end_h = PAGE_QNTY - 1;

#if (!defined(SLC_MODE) && defined(NAND_B47R))
    rw_pgrp_tbl(0) = (0 << 14) | 3;
    rw_pgrp_tbl(1) = (2 << 14) | 1047;
    rw_pgrp_tbl(2) = (1 << 14) | 1063;
    rw_pgrp_tbl(3) = (2 << 14) | 2107;
    rw_pgrp_tbl(4) = (0 << 14) | PAGE_QNTY;
#else
    rw_pgrp_tbl(0) = (2 << 14) | PAGE_QNTY;
#endif

    rw_pext_num  = 4 * CMPR_NUM;
    rb_pstr_bmu  = FRAG_SHIFT + CHAN_SHIFT + BANK_SHIFT;
    rb_psft_bmu  = PAGE_SHIFT;
    rw_nerr_num  = 110;

    rb_index     = 0x08;        //set reg_index as all channel
#ifdef FPGA_U2
    rb_bus_mirr = 0x30;
#elif (defined(FPGA_SORTING))
    rb_bus_mirr = 0x00;
#else
    rb_bus_mirr = 0xaa;//set ce1/3/5/7 bus mirror
#endif
    rb_lunce_end = BANK_QNTY-1; //set reg_lence_end for sequence nand program
#ifdef NAND_BICS5
    rb_wlpg_set  = WLPG_SEL | WLPG_SFT; //word line page mode
#endif

#ifdef CPUA_PROC
    //set cpua proc reg default value
    rd_cpua_reg13 = NAND_REG_BASE;
    rd_cpua_reg14 = SRAM_META_BASE + (HMETA_QSZ << 3);
    //rd_cpua_reg15 = DRAM_META_BASE;
    rq_cpua_reg15 = ((qwrd)DRAM_MEM_BASE << 4) | ((qwrd)(DRAM_META_BASE + (HMETA_QSZ << 3)) & 0x7fffffff);
#endif


    //set micro code SRAM
    for(lw_cnt0 = 0; lw_cnt0 < MICRO_CODE_NUM; lw_cnt0++)
    {
        lw_addr = (lw_cnt0 << 9);
        lw_cnt1 = 0;
        do
        {
            lw_data = gw_micro_code[lw_cnt0][lw_cnt1];
            lw_cnt1++;
            for(lw_cnt2 = 0; lw_cnt2 < (8 >> INST_SHARE); lw_cnt2++)
            {
                rw_inst_mem(lw_addr) = lw_data;
                lw_addr++;
            }
        } while(lw_data != INST_FIN);
    }

#ifdef CPUA_PROC
    //set cpua proc code SRAM
    for(lw_cnt0 = 0; lw_cnt0 < CPUA_CODE_NUM; lw_cnt0++)
    {
        lw_addr = (lw_cnt0 << 6);
        do
        {
            lw_data = gw_cpua_code[lw_cnt0][(lw_addr & 0x3f)];
            rw_imem_dat = lw_data;
            rd_imem_set = IMEM_ACT | IMEM_WEN | ((dwrd)lw_addr << 16);
            lw_addr++;
        } while(lw_data != INST_FIN);
    }
#endif


/**************************          NAND initial          **************************/
    //release ptc_hdl reset
    rb_ptch_rst = 0x0;

#ifdef FPGA_MODE
  #ifdef NAND_HSPD
    //switch to high speed clk
    rb_nand_clk = NAND_CLK;                               //select fast nand clk
  #endif
    for(lw_cnt0 = 0; lw_cnt0 < CHAN_QNTY; lw_cnt0++)
    {
        rb_index = phy_chan_map(lw_cnt0);                 //set chan
        while(rb_nphy_rdy == 0);                          //wait high speed select IO IP ready
    }
#endif


    //reset NAND
    for(lw_cnt0 = 0; lw_cnt0 < CHAN_QNTY; lw_cnt0++)
    {
        rb_index = phy_chan_map(lw_cnt0);                 //set chan
    #ifndef SKIP_STS_CHK
        rb_rds_cfg |= RDS_HOLD;                           //set read sts hold
        rb_set_cfg = CMD_HOLD;                            //set micro proc hold
    #endif
        for(lw_cnt1 = 0; lw_cnt1 < PCE_QNTY; lw_cnt1++)
        {
            rb_cmd_lunce = lw_cnt1;
            rb_nand_cen = rb_phy_ce;                      //set CEn
            //rb_nand_cmd = 0xFD;
            rb_nand_cmd = 0xFF;                           //reset cmd
    #ifndef SKIP_STS_CHK
            rb_set_busy = FRC_RDS;                        //check ready/busy
    #endif
        }
    }

    //wait reset ready
    #ifdef SKIP_STS_CHK
    for(lw_cnt0 = 0; lw_cnt0 < 1000; lw_cnt0++)
    {
        for(lw_cnt1 = 0; lw_cnt1 < 1000; lw_cnt1++);
    }
    #else
    for(lw_cnt0 = 0; lw_cnt0 < CHAN_QNTY; lw_cnt0++)
    {
        rb_index = phy_chan_map(lw_cnt0);                 //set chan
        rb_rds_cfg &= ~RDS_HOLD;
        rb_set_cfg = 0;
        for(lw_cnt1 = 0; lw_cnt1 < PCE_QNTY; lw_cnt1++)
        {
            while((rw_sts_rdy & (1 << lw_cnt1)) == 0);
            if(rw_sts_fail & (1 << lw_cnt1))              //bad die
                fv_uart_print("reset fail: chan:%x, physical ce:%x\r\n", lw_cnt0, lw_cnt1);
        }
    }
    #endif


#ifdef ONFI_MODE
    //select slow nand clk
    //rb_nand_clk = CLK31M;


  #ifdef NAND_DIFF

    //set NAND NV-DDR3 config
    for(lw_cnt0 = 0; lw_cnt0 < CHAN_QNTY; lw_cnt0++)
    {
        rb_index = phy_chan_map(lw_cnt0);                 //set chan
    #ifndef SKIP_STS_CHK
        rb_rds_cfg |= RDS_HOLD;                           //set read sts hold
        rb_set_cfg = CMD_HOLD;                            //set micro proc hold
    #endif
        for(lw_cnt1 = 0; lw_cnt1 < PCE_QNTY; lw_cnt1++)
        {
            rb_cmd_lunce = lw_cnt1;
            rb_nand_cen = rb_phy_ce;                      //set CEn
            rb_nand_cmd = 0xEF;                           //set feature cmd
            rb_nand_adr = 0x02;                           //set NV-DDR3 config reg
            lw_cnt2 = 150;
            while(--lw_cnt2!=0);                          //wait tADL of set feature
    #ifdef MANUAL_SETF
            rb_nphy_frc |= DQS_FRC;
            rb_dqo_data = (MIRR_LCE & (1 << lw_cnt1)) ? 0xe4 : 0x27;
            rb_dqs_data = 0x03;
            rb_nphy_frc |= DQO_FRC;
            rb_nphy_frc |= DQE_FRC;
            rb_nphy_frc |= DQS_SEQ;
            rb_nphy_frc &= ~DQS_SEQ;
            rb_nphy_frc &= ~DQO_FRC;
            rb_nphy_frc |= DQS_SEQ;
            rb_nphy_frc &= ~DQS_SEQ;
            rb_nphy_frc |= DQS_SEQ;
            rb_nphy_frc &= ~DQS_SEQ;
            rb_nphy_frc |= DQS_SEQ;
            rb_nphy_frc &= ~DQS_SEQ;
            rb_nphy_frc &= ~DQE_FRC;
            rb_nphy_frc &= ~DQS_FRC;
    #else
      #ifdef SETF_LDQS
            rb_lowdqs_en = 0x1;
      #endif
            rw_nand_wdt = 0x2727;                         //set P1 data
            //rw_nand_wdt = 0x0707;                         //set P1 data
            rw_nand_wdt = 0x0;                            //set P2 data
            rw_nand_wdt = 0x0;                            //set P3 data
            rw_nand_wdt = 0x0;                            //set P4 data
      #ifdef SETF_LDQS
            rb_lowdqs_en = 0x0;
      #endif
    #endif
    #ifndef SKIP_STS_CHK
            rb_set_busy = FRC_RDS;                        //check ready/busy
    #endif
        }
        rb_nand_cen = 0xff;                               //turn off CEn
    }

    //wait "set feature" ready
    #ifdef SKIP_STS_CHK
    for(lw_cnt0 = 0; lw_cnt0 < 1000; lw_cnt0++)
    {
        for(lw_cnt1 = 0; lw_cnt1 < 1000; lw_cnt1++);
    }
    #else
    for(lw_cnt0 = 0; lw_cnt0 < CHAN_QNTY; lw_cnt0++)
    {
        rb_index = phy_chan_map(lw_cnt0);                 //set chan
        rb_rds_cfg &= ~RDS_HOLD;
        rb_set_cfg = 0;
        while(rw_sts_rdy != 0xffff);
    }
    #endif

  #endif


  #if (defined(NAND_HSPD) && !defined(NAND_BICS5)) //BICS5 don't need to set timing mode

    //set NAND timing mode
    for(lw_cnt0 = 0; lw_cnt0 < CHAN_QNTY; lw_cnt0++)
    {
        rb_index = phy_chan_map(lw_cnt0);                 //set chan
    //#ifndef SKIP_STS_CHK
    //    rb_rds_cfg |= RDS_HOLD;                           //set read sts hold
    //    rb_set_cfg = CMD_HOLD;                            //set micro proc hold
    //#endif
        for(lw_cnt1 = 0; lw_cnt1 < PCE_QNTY; lw_cnt1++)
        {
            rb_cmd_lunce = lw_cnt1;
            rb_nand_cen = rb_phy_ce;                      //set CEn
            rb_nand_cmd = 0xEF;                           //set feature cmd
            rb_nand_adr = 0x01;                           //set timing mode reg
            lw_cnt2 = 150;
            while(--lw_cnt2!=0);                          //wait tADL of set feature
    #ifdef MANUAL_SETF
            rb_nphy_frc |= DQS_FRC;
#if (!defined(RTL_SIM) && defined(FPGA_MODE))
            rb_dqo_data = (MIRR_LCE & (1 << lw_cnt1)) ? ((NAND_CLK == CLK62M) ? 0x42 : ((NAND_CLK == CLK125M) ? 0xa2 : 0x12)) :
                          ((NAND_CLK == CLK62M) ? 0x42 : ((NAND_CLK == CLK125M) ? 0x45 : 0x48));
#else
            rb_dqo_data = (NAND_CLK == CLK62M) ? 0x42 : ((NAND_CLK == CLK125M) ? 0x45 : 0x48);
#endif
            rb_dqs_data = 0x03;
            rb_nphy_frc |= DQO_FRC;
            rb_nphy_frc |= DQE_FRC;
            rb_nphy_frc |= DQS_SEQ;
            rb_nphy_frc &= ~DQS_SEQ;
            rb_nphy_frc &= ~DQO_FRC;
            rb_nphy_frc |= DQS_SEQ;
            rb_nphy_frc &= ~DQS_SEQ;
            rb_nphy_frc |= DQS_SEQ;
            rb_nphy_frc &= ~DQS_SEQ;
            rb_nphy_frc |= DQS_SEQ;
            rb_nphy_frc &= ~DQS_SEQ;
            rb_nphy_frc &= ~DQE_FRC;
            rb_nphy_frc &= ~DQS_FRC;
    #else
      #ifdef SETF_LDQS
            rb_lowdqs_en = 0x1;
      #endif
      #ifdef NAND_B47R
            rw_nand_wdt = 0x7f7f;                         //set P1 data
      #else //NAND_X39070
            rw_nand_wdt = 0xc3c3;                         //set P1 data
      #endif
            rw_nand_wdt = 0x0;                            //set P2 data
            rw_nand_wdt = 0x0;                            //set P3 data
            rw_nand_wdt = 0x0;                            //set P4 data
      #ifdef SETF_LDQS
            rb_lowdqs_en = 0x0;
      #endif
    #endif
    //#ifndef SKIP_STS_CHK
    //        rb_set_busy = FRC_RDS;                        //check ready/busy
    //#endif
        }
        rb_nand_cen = 0xff;                               //turn off CEn
    }

    //wait "set feature" ready
    //#ifdef SKIP_STS_CHK
    for(lw_cnt0 = 0; lw_cnt0 < 1000; lw_cnt0++)
    {
        for(lw_cnt1 = 0; lw_cnt1 < 1000; lw_cnt1++);
    }
    //#else
    //for(lw_cnt0 = 0; lw_cnt0 < CHAN_QNTY; lw_cnt0++)
    //{
    //    rb_index = phy_chan_map(lw_cnt0);                 //set chan
    //    rb_rds_cfg &= ~RDS_HOLD;
    //    rb_set_cfg = 0;
    //    while(rw_sts_rdy != 0xffff);
    //}
    //#endif

  #endif


  #ifndef FPGA_MODE
    //switch to high speed clk & turn off low dqs mode
    rb_nand_clk = NAND_CLK;                               //select fast nand clk
    #ifdef RSTS_LDQS
    rb_ldqs_act = 0;                                      //turn off low dqs function
    #endif
  #endif

  #ifdef NAND_2400M
    rb_tcals   = 1;
    rb_tcalh   = 1;
    rb_twp     = 2;
    rb_tcas    = 1;
  #endif

  #ifdef FPGA_MODE
    if(NAND_CLK == CLK62M)
      rw_tmr_base = 32;
    else if(NAND_CLK == CLK125M)
      rw_tmr_base = 64;
    else if(NAND_CLK == CLK250M)
      rw_tmr_base = 100;
  #else
    rw_tmr_base = 600;
  #endif

#endif //#ifdef ONFI_MODE


#ifdef ZQ_CALI

    for(lw_cnt0 = 0; lw_cnt0 < CHAN_QNTY; lw_cnt0++)
    {
        rb_index = phy_chan_map(lw_cnt0);                 //set chan
        for(lw_cnt1 = 0; lw_cnt1 < BANK_QNTY; lw_cnt1++)
        {
            rb_cmd_lunce = lw_cnt1;
            rb_nand_cen = rb_phy_ce;                      //set CEn
            rb_nand_cmd = 0xF9;                           //set ZQ Calibration cmd
            rb_nand_adr = rb_phy_lun;                            //set row addr1
            //rb_nand_adr = 0x0;                            //set row addr2
            //rb_nand_adr = 0x0;                            //set row addr3
            //rb_nand_adr = 0x0;                            //set row addr4
            rb_nand_cen = 0xff;                           //turn off CEn
        }
    }
#endif

    //read status
    for(lw_cnt0 = 0; lw_cnt0 < CHAN_QNTY; lw_cnt0++)
    {
        rb_index = phy_chan_map(lw_cnt0);                 //set chan
        for(lw_cnt1 = 0; lw_cnt1 < PCE_QNTY; lw_cnt1++)
        {
            rb_cmd_lunce = lw_cnt1;
            rb_nand_cen = rb_phy_ce;                      //set CEn

            do
            {
                rb_nand_cmd = 0x78;                       //read enhance status cmd
                rb_nand_adr = rb_row_adr1;                //set row addr1
                rb_nand_adr = rb_row_adr2;                //set row addr2
                rb_nand_adr = rb_row_adr3;                //set row addr3
                rb_nand_adr = rb_row_adr4;                //set row addr4

#ifdef DMY_READ
                //dummy read
                rw_nand_rdt = 0x1;                        //dummy read
                lw_cnt2 = 5;
                while(--lw_cnt2!=0);                      //wait 5 clk cycle for dummy read
#endif

                //real read
                rw_nand_rdt = 0x0;                        //read status
            } while((rd_nrd_dat & 0x60) != 0x60);

            //print read data
            fv_uart_print("read status: ce:%x, ch:%x, sts:%x\r\n", lw_cnt1, lw_cnt0, rd_nrd_dat);

            rb_nand_cen = 0xff;                           //turn off CEn
        }
    }


    return;
}
