#include <global.h>
#include <register.h>

#include <stdio.h>
#include <stdlib.h>

extern void fv_uart_init(uint32_t baudrate);
extern void fv_nand_init(void);
extern void fv_ftl_init(void);

extern void fv_pcie_chk(void);
extern void fv_uart_chk(void);
extern void fv_nerr_chk(void);
extern void fv_ram_chk(void);
extern void fv_rcnt_chk(void);
extern void fv_nand_scan(void);
extern void fv_pwr_chk(void);
extern void fv_rdec_read(void);
extern void fv_rdec_send(void);

extern void fv_hst_hdl(void);
extern void fv_ftl_hdl(void);
extern void fv_ftl_gc(void);

extern word gw_fblk_num;
extern dwrd gd_ht_token;
extern dwrd gd_gc_token;
#ifdef GC_SIM
extern byte gb_htp2l_num;
#endif

dwrd gd_sys_tmr = 0;
//dwrd gd_sys_lst = 0;


//??
//ECC fail
//read cmd put into high pri queue, write cmd put into low pri queue

int main(void)
{
#ifdef NPHS_CALI

#ifdef RTL_SIM
  #define TEST_BLK 3
#else
  #define TEST_BLK 1
#endif
#ifdef NAND_BICS5
  #define MAPU_NUM 8
  #define WLPG_NUM 3
  #define META_NUM 4 //dword unit
  #define RBUF_OFS 32
#else //YMTC X3-9070
  #define MAPU_NUM 24
  #define WLPG_NUM 3
  #define META_NUM 8
  #define RBUF_OFS 128
#endif

    byte lb_wlpg_cnt, lb_mapu_cnt, lb_bptr_cnt, lb_cech_ptr, lb_nphy_phs, lb_nddr_ptr, lb_fail_phs, lb_dbit_loc;
    dwrd ld_bmu_adr, ld_rdm_seed, ld_wait_cnt, ld_read_dat, ld_expt_dat, ld_xor_dat;
    word lw_buff_ptr [WLPG_NUM][MAPU_NUM];
    dwrd ld_err_cnt [8];

#ifndef RTL_SIM
    fv_uart_init(115200);
    fv_uart_print("ssd power on\r\n");
#endif

    fv_nand_init();

    //set buffer ready
    rb_dbuf_rdy = 1;
    rb_sbuf_rdy = 1;
    rb_pbuf_rdy = 1;

    //disable randomizer
    rb_rdmz_en = 0;


    //set pattern into BMU
#ifdef RTL_SIM
    {
        {
            lb_bptr_cnt = 0x0;
#else
    for(lb_wlpg_cnt=0;lb_wlpg_cnt<WLPG_NUM;lb_wlpg_cnt=lb_wlpg_cnt+1)
    {
        for(lb_mapu_cnt=0;lb_mapu_cnt<MAPU_NUM;lb_mapu_cnt=lb_mapu_cnt+1)
        {
            lb_bptr_cnt = ((dwrd)lb_wlpg_cnt * MAPU_NUM) + (dwrd)lb_mapu_cnt;
#endif

            for(ld_bmu_adr=0;ld_bmu_adr<1024;ld_bmu_adr=ld_bmu_adr+1)
            {
                ld_rdm_seed = ((dwrd)lb_bptr_cnt << 10) | ld_bmu_adr;
                srand(ld_rdm_seed);
                rd_smdt_dat(lb_bptr_cnt, ld_bmu_adr) = rand();
            }

            for(ld_bmu_adr=0;ld_bmu_adr<META_NUM;ld_bmu_adr=ld_bmu_adr+1)
            {
                rd_smmt_dat(lb_bptr_cnt, ld_bmu_adr) = ((0x6789 + (dwrd)lb_bptr_cnt) << 16) + ld_bmu_adr;
            }
        }
    }


    //erase/program data
    for(lb_cech_ptr=0;lb_cech_ptr<BKCH_QNTY;lb_cech_ptr=lb_cech_ptr+1)
    {
        rb_index = lb_cech_ptr & CHAN_MASK;

        //send erase cmd to nand ctl
        rd_cmd_padr = (TEST_BLK << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT)) | ((dwrd)lb_cech_ptr << FRAG_SHIFT);
        rw_cmd_type = ERS_CPTR | LOW_PRI | CMD_END;

        //send program cmd to nand ctl
        for(lb_wlpg_cnt=0;lb_wlpg_cnt<WLPG_NUM;lb_wlpg_cnt=lb_wlpg_cnt+1)
        {
            for(lb_mapu_cnt=0;lb_mapu_cnt<MAPU_NUM;lb_mapu_cnt=lb_mapu_cnt+1)
            {
#ifdef RTL_SIM
                lb_bptr_cnt = 0x0;
#else
                lb_bptr_cnt = ((dwrd)lb_wlpg_cnt * MAPU_NUM) + (dwrd)lb_mapu_cnt;
#endif
                rb_cmd_qidx = QLNK_SMQ;
                rd_cmd_padr = (TEST_BLK << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT)) | ((dwrd)lb_wlpg_cnt << (BKCH_SHIFT + FRAG_SHIFT)) | ((dwrd)lb_cech_ptr << FRAG_SHIFT) | (dwrd)lb_mapu_cnt;
                rw_cmd_bptr = (SMDT_BASE << BPTR_SFT) | (word)lb_bptr_cnt;
                rw_cmd_type = PROG_CPTR | LOW_PRI | (((lb_wlpg_cnt == (WLPG_NUM-1)) && (lb_mapu_cnt == (MAPU_NUM-1))) ? CMD_END : 0);
            }
        }
    } //for(lb_cech_ptr=0;lb_cech_ptr<CHAN_QNTY;lb_cech_ptr=lb_cech_ptr+1)


    //read data & check
    for(lb_cech_ptr=0;lb_cech_ptr<BKCH_QNTY;lb_cech_ptr=lb_cech_ptr+1)
    {
        fv_uart_print("nand phy phase calibration: cech:%x\r\n", lb_cech_ptr);
        rb_index = lb_cech_ptr & CHAN_MASK;

        for(lb_nphy_phs=0;lb_nphy_phs<8;lb_nphy_phs=lb_nphy_phs+1)
        {
            fv_uart_print("phase:%x\r\n", lb_nphy_phs);
            rd_nphy_phs = ((dwrd)lb_nphy_phs << 21) | ((dwrd)lb_nphy_phs << 18) | ((dwrd)lb_nphy_phs << 15) | ((dwrd)lb_nphy_phs << 12) |
                          ((dwrd)lb_nphy_phs <<  9) | ((dwrd)lb_nphy_phs <<  6) | ((dwrd)lb_nphy_phs <<  3) | ((dwrd)lb_nphy_phs <<  0);

            //send read cmd to nand ctl
            for(lb_wlpg_cnt=0;lb_wlpg_cnt<WLPG_NUM;lb_wlpg_cnt=lb_wlpg_cnt+1)
            {
                for(lb_mapu_cnt=0;lb_mapu_cnt<MAPU_NUM;lb_mapu_cnt=lb_mapu_cnt+1)
                {
                    lb_bptr_cnt = ((dwrd)lb_wlpg_cnt * MAPU_NUM) + (dwrd)lb_mapu_cnt;
                    rb_cmd_misc = 0x1; //don't through LDPC decoder
                    rb_cmd_qidx = QLNK_SMQ;
                    rd_cmd_padr = (TEST_BLK << (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT)) | ((dwrd)lb_wlpg_cnt << (BKCH_SHIFT + FRAG_SHIFT)) | ((dwrd)lb_cech_ptr << FRAG_SHIFT) | (dwrd)lb_mapu_cnt;
                    rw_cmd_bptr = (SMDT_BASE << BPTR_SFT) | RBUF_OFS | (word)lb_bptr_cnt;
                    rw_cmd_type = READ_CPTR | LOW_PRI | ((lb_mapu_cnt == (MAPU_NUM-1)) ? CMD_END : 0);
                }
            }

            //check whether data ready
            lb_fail_phs = 0;
            lb_bptr_cnt = 0;
            ld_wait_cnt = 0;
            while(lb_bptr_cnt < (WLPG_NUM * MAPU_NUM))
            {
                if(rb_nddr_vld)
                {
                    //fv_uart_print("p4k:%x\r\n", rd_nddr_pad);
                    lb_nddr_ptr = rb_nddr_ptr;
                    lw_buff_ptr[((rd_nddr_pad >> (BKCH_SHIFT + FRAG_SHIFT)) & 0x3)][(rd_nddr_pad & FRAG_MASK)] = (SMPT_BASE << 8) | (word)lb_nddr_ptr;
                    rb_nddr_vld = NDDR_CLR;
                    rd_carl_set = (((SMDT_BASE << BPTR_SFT) | (SMPT_BASE << 8) | (dwrd)lb_nddr_ptr) << 16) | CARL_ACT;
                    lb_bptr_cnt++;
                }

                ld_wait_cnt++;
                if(ld_wait_cnt == 1000000)
                {
                    fv_uart_print("read cmd timeout:cech:%x\r\n", lb_cech_ptr);
                    lb_fail_phs = 1;
                    rb_chip_rst = BMU_RST | NAND_RST;

                    //initial again
                    fv_nand_init();
                    rb_dbuf_rdy = 1;
                    rb_sbuf_rdy = 1;
                    rb_pbuf_rdy = 1;
                    rb_rdmz_en = 0;

                    break;
                }
            }

            //this phase fail, go to next phase
            if(lb_fail_phs)
            {
                continue;
            }

            //initial error cnt
            for(lb_dbit_loc=0;lb_dbit_loc<8;lb_dbit_loc=lb_dbit_loc+1)
            {
                ld_err_cnt[lb_dbit_loc] = 0;
            }

            //check data from NDDR(nand to decoder read)
            for(lb_wlpg_cnt=0;lb_wlpg_cnt<WLPG_NUM;lb_wlpg_cnt=lb_wlpg_cnt+1)
            {
                for(lb_mapu_cnt=0;lb_mapu_cnt<MAPU_NUM;lb_mapu_cnt=lb_mapu_cnt+1)
                {
#ifdef RTL_SIM
                    lb_bptr_cnt = 0x0;
#else
                    lb_bptr_cnt = ((dwrd)lb_wlpg_cnt * MAPU_NUM) + (dwrd)lb_mapu_cnt;
#endif

                    for(ld_bmu_adr=0;ld_bmu_adr<1024;ld_bmu_adr=ld_bmu_adr+1)
                    {
                        ld_read_dat = rd_smdt_dat(lw_buff_ptr[lb_wlpg_cnt][lb_mapu_cnt], ld_bmu_adr);
                        ld_rdm_seed = ((dwrd)lb_bptr_cnt << 10) | ld_bmu_adr;
                        srand(ld_rdm_seed);
                        ld_expt_dat = rand();
                        ld_xor_dat = ld_read_dat ^ ld_expt_dat;

                        for(lb_dbit_loc=0;lb_dbit_loc<32;lb_dbit_loc=lb_dbit_loc+1)
                        {
                            if(ld_xor_dat & 0x1)
                            {
                                ld_err_cnt[(lb_dbit_loc & 0x7)]++;
                            }
                            ld_xor_dat = ld_xor_dat >> 1;
                        }
                    }
                }
            }

            //display result
            for(lb_dbit_loc=0;lb_dbit_loc<8;lb_dbit_loc=lb_dbit_loc+1)
            {
                fv_uart_print("loc:%d, err_cnt:%d\r\n", lb_dbit_loc, ld_err_cnt[lb_dbit_loc]);
            }

        } //for(lb_nphy_phs=0;lb_nphy_phs<8;lb_nphy_phs=lb_nphy_phs+1)
    } //for(lb_cech_ptr=0;lb_cech_ptr<CHAN_QNTY;lb_cech_ptr=lb_cech_ptr+1)


    //enable randomizer
    rb_rdmz_en = 1;

#endif


/******************************************************************************************************************************/
/*******************************                        main system FW                          *******************************/
/******************************************************************************************************************************/
    //initial
#ifndef FPGA_MODE
    rb_hcpu_set = 1;
    ARM_NOP(); //wait 3 cycle for HW handle
    ARM_NOP(); //wait 3 cycle for HW handle
    ARM_NOP(); //wait 3 cycle for HW handle
    ARM_NOP(); //wait 3 cycle for HW handle
    ARM_NOP(); //wait 3 cycle for HW handle
#endif

    fv_uart_init(115200);
    fv_uart_print("ssd power on\r\n");

#ifdef DLNK_EN
    if(rb_dlnk_ena && rb_dual_prt) //dual pcie port, use ctag msb bit to identify port number
    {
        rw_sqio_thr = CTAG_NUM/2;
        rw_ctag_end = CTAG_NUM/2 - 1;
    }
#endif
    rb_nvme_hmt = HMT_MODE;

#ifndef NVME_TEST
    fv_nand_init();
    fv_ftl_init();
#endif

    rb_init_fin = 1;

    //task loop
    while(1)
    {
        //check whether time tick comes
        //if(gd_sys_tmr != gd_sys_lst)
        if((gd_sys_tmr & 0xff) == 0x0)
        {
            //check pcie link
            fv_pcie_chk();

            //check uart
            fv_uart_chk();

            //check ecc fail
            if(rb_errd_vld)
            {
                rb_elsm_ptr = rb_errd_lid;
                //if(((rw_elsm_dat & 0x3) != 0x0) || (rb_errd_que == QLNK_ERQ))
                if((rw_elsm_dat & 0x3) != 0x0)
                {
                    fv_uart_print("ecc fail: queue id:%x, pad:%x, chan:%x, err cnt:%d\r\n", rb_errd_que, rd_errd_pad, ((rd_errd_pad >> FRAG_SHIFT) & CHAN_MASK), ((rw_elsm_dat >> 2) & 0xfff));
                    //rb_nand_phs = rb_nand_phs ^ (1 << ((rd_errd_pad >> FRAG_SHIFT) & CHAN_MASK));
                    if(rb_errd_que != QLNK_RAQ)
                    {
                        //if send to dram buffer, need release temp buffer
                        if((rw_errd_bfp >> BPTR_SFT) < SMDT_BASE)
                        {
                            rd_carl_set = (((SMDT_BASE << BPTR_SFT) | (dwrd)rw_errd_ptr) << 16) | CARL_ACT;
                        }

//                        fv_rdec_read();

                        rb_cmd_qidx = rb_errd_que;
                        rd_cmd_cid(0) = rd_errd_cid(0);
                        rd_cmd_padr = rd_errd_pad;
                        rw_cmd_bptr = rw_errd_bfp;
                        rb_errd_vld = ERRD_CLR; //must clear old queue first, and send read cmd to nctl
                        rw_cmd_type = READ_CPTR | HIGH_PRI | CMD_END;
                    }
                    else
                    {
                        fv_uart_print("need use raid6 to decode\r\n");
                        fv_dbg_loop(0x20);
                    }
                }
                else
                {
                    fv_uart_print("error: queue id:%x, bfp:%x, rid:%x, pad:%x. cid:%x\r\n", rb_errd_que, rw_errd_bfp, rw_errd_rid, rd_errd_pad, rd_errd_cid(0));
                    fv_dbg_loop(0x21);
                }
            }

            //check ecc decode result
//            fv_rdec_send();

            //check error flag
            if(rd_lctl_dbg != 0x0)
            {
                fv_uart_print("error2: lctl:%x\r\n", rd_lctl_dbg);
                fv_dbg_loop(0x22);
            }

#ifdef HTMT_CHK
            //check host meta error
            if(rb_htmt_err)
            {
                fv_uart_print("error3: host meta error\r\n");
                fv_dbg_loop(0x23);
            }
#endif

 
//            //check ram error
//            fv_ram_chk();
//  
//            //check read disturbance cnt
//            fv_rcnt_chk();
//  
//            //scan nand
//            fv_nand_scan();
//  
//            //power mode
//            fv_pwr_chk();
//
//            //update gd_sys_lst
//            gd_sys_lst = gd_sys_tmr;
        }
        gd_sys_tmr++;

        //handle nvme admin/io task, pcie/nvme error handling
        if((gd_sys_tmr & 0xf) == 0x0)
        {
            fv_hst_hdl();
        }

#ifdef GC_SIM
        if(!(gb_htp2l_num > GC_SIM_THR) || (gd_ht_token < gd_gc_token))
#else
        if(!(gw_fblk_num < GC_BLK_THR) || (gd_ht_token < gd_gc_token))
#endif
        {
            fv_ftl_hdl(); //handle ftl task
        }

#ifdef GC_SIM
        if((gb_htp2l_num > GC_SIM_THR) && !(gd_ht_token < gd_gc_token))
#else
        if((gw_fblk_num < GC_BLK_THR) && !(gd_ht_token < gd_gc_token))
#endif
        {
            fv_ftl_gc(); //handle gc task
        }
    }
}
