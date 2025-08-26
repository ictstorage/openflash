#include <global.h>
#include <register.h>

#include "./identify.h"

#define ADMD_QID 0
#define SLOC_OPC 0
#define SLOC_CID 1
#define SLOC_PRP 12
#define SLOC_PR2 16
#define SLOC_QID 20
#define SLOC_QSZ 21
#define SLOC_CFG 22 //create SQ: {pri[1:0], qpc}
#define SLOC_CPL 23 //create SQ: mapping to CQ ID
#define SLOC_VCT 23 //create CQ: interrupt vector
#define SLOC_LID 20 //log page ID
#define SLOC_CNS 20 //indentify: Ctrl/NS structure
#define SLOC_FID 20 //set feature ID
#define SLOC_FMG 22 //set feature message

//byte gb_uart_ptr = 0;
//dwrd gd_uart_msg[256];


void fv_pcie_chk(void)
{
//    dwrd ld_dly_tmr;

    if(rb_pclk_rdy == 0)
    {
        while(rb_pclk_rdy == 0);
//??
//        //extend pcie reset, reset again
//        for(ld_dly_tmr = 0; ld_dly_tmr < (PRST_DLY*10); ld_dly_tmr = ld_dly_tmr + 1);
//        rb_pcie_rst = 1;
//        for(ld_dly_tmr = 0; ld_dly_tmr < PRST_DLY; ld_dly_tmr = ld_dly_tmr + 1);
//        rb_pcie_rst = 0;
//        while(rb_pclk_rdy == 0);
//        rb_pclk_set = 0; //clear
//        for(ld_dly_tmr = 0; ld_dly_tmr < (PRST_DLY*3); ld_dly_tmr = ld_dly_tmr + 1);

        //asyn fifo auto clear
        //  //clear async FIFO
        //  rb_asyn_clr = HCLR;
        //  for(ld_dly_tmr = 0; ld_dly_tmr < 10; ld_dly_tmr = ld_dly_tmr + 1);
        //  rb_asyn_clr = 0;

        //set nvme ready
        while((rd_stdr_cfg(0) & NVME_ENA) == 0);
        rd_stdr_sts(0) = rd_stdr_sts(0) | NVME_RDY;
    }
    else
    {
        if(rd_stdr_cfg(0) & NVME_ENA)
        {
            if(!(rd_stdr_sts(0) & NVME_RDY))
            {
                rd_stdr_sts(0) = rd_stdr_sts(0) | NVME_RDY;
            }
            else
            {
                if(((rd_stdr_sts(0) & NVME_SHN_STS_MASK) == NVME_SHN_STS_NORMAL) && ((rd_stdr_cfg(0) & NVME_SHN_NORMAL) || (rd_stdr_cfg(0) & NVME_SHN_ABRUPT)))
                {
                    fv_uart_print("NVMe Controller %s Shutdown...\r\n", (rd_stdr_cfg(0) & NVME_SHN_NORMAL) ? "normal" : "abrupt");
                    rd_stdr_sts(0) = rd_stdr_sts(0) & (~NVME_SHN_STS_CTL);
                    rd_stdr_sts(0) = (rd_stdr_sts(0) & (~NVME_SHN_STS_MASK)) | NVME_SHN_STS_PROC_OCR;
                    rd_stdr_sts(0) = (rd_stdr_sts(0) & (~NVME_SHN_STS_MASK)) | NVME_SHN_STS_PROC_CMP;
                    fv_uart_print("NVMe Shutdown Done!\r\n");
                }
            }
        }
        else
            rd_stdr_sts(0) = rd_stdr_sts(0) & ~NVME_RDY;
    }

    return;
}

void fv_hst_hdl(void)
{
    byte lb_cqsd_dis = 0;
    byte lb_admd_opc, lb_logp_lid, lb_idtf_cns, lb_feat_fid;
    word lw_sqcq_ptr, lw_cqsd_ctg/*, lw_fluc_cnt*/;
    //dwrd ld_sram_ptr;

    if(rb_pclk_rdy == 0)
    {
        return;
    }
//??
//    //dummy access
//    lb_idtf_cns = rb_admd_vld;

    //check admin cmd
    while(rb_admd_vld)
    {
        lb_admd_opc = (byte)rw_admd_dat(SLOC_OPC);

        switch(lb_admd_opc)
        {
            case 0x0: //ADMIN_DELETE_IO_SQ
            {
                fv_uart_print("ADMIN_DELETE_IO_SQ: ctag:%x\r\n", rw_admd_ctg);
                lw_sqcq_ptr = rw_admd_dat(SLOC_QID); //if qid > HW support q_num, need remapping
                rq_sbmq_prp(lw_sqcq_ptr) = 0;
                rw_sbmq_qsz(lw_sqcq_ptr) = 0;
                rw_sbmq_qid(lw_sqcq_ptr) = lw_sqcq_ptr;
                rw_sbmq_cpl(lw_sqcq_ptr) = lw_sqcq_ptr;
                rb_sbmq_cfg(lw_sqcq_ptr) = 0x1;
                rb_sbmq_fun(lw_sqcq_ptr) = 0x0;
                rw_cqsd_sts = 0x0;
                rq_cqsd_dat = 0x0;

                break;
            }

            case 0x1: //ADMIN_CREATE_IO_SQ
            {
                fv_uart_print("ADMIN_CREATE_IO_SQ: ctag:%x\r\n", rw_admd_ctg);
                lw_sqcq_ptr = rw_admd_dat(SLOC_QID); //if qid > HW support q_num, need remapping
                rq_sbmq_prp(lw_sqcq_ptr) = rq_admd_dat(SLOC_PRP >> 2);
                rw_sbmq_qsz(lw_sqcq_ptr) = rw_admd_dat(SLOC_QSZ);
                rw_sbmq_qid(lw_sqcq_ptr) = rw_admd_dat(SLOC_QID);
                rw_sbmq_cpl(lw_sqcq_ptr) = rw_admd_dat(SLOC_CPL);
                rb_sbmq_cfg(lw_sqcq_ptr) = (byte)rw_admd_dat(SLOC_CFG);
                rb_sbmq_fun(lw_sqcq_ptr) = 0x0;
                rw_cqsd_sts = 0x0;
                rq_cqsd_dat = 0x0;

                break;
            }

            case 0x2: //ADMIN_GET_LOG_PAGE
            {
                lb_logp_lid = (byte)rw_admd_dat(SLOC_LID);
                fv_uart_print("ADMIN_GET_LOG_PAGE: ctag:%x, lid:%x\r\n", rw_admd_ctg, lb_logp_lid);
                if(lb_logp_lid == 0x2) //smart & health info
                {
                    memcpy((byte *)(NVME_MEM_BASE), (byte *)(gd_logp_sh), (LOGP_SH_LEN*4));
                    memset((byte *)(NVME_MEM_BASE+LOGP_SH_LEN*4), 0x0, (512 - LOGP_SH_LEN*4));
                    rq_pdma_ba0 = rq_admd_dat(SLOC_PRP >> 2);
                    rq_pdma_ba1 = rq_admd_dat(SLOC_PR2 >> 2);
                    rd_pdma_set = (128 << 16) | (0xff << 8) | PDMA_FUN(0) | PDMA_MWR | PDMA_ACT;
                    while(rd_pdma_set & PDMA_ACT);
                    rw_cqsd_sts = 0x0;
                }
                else
                {
                    rw_cqsd_sts = 0x109;
                }
                rq_cqsd_dat = 0x0;

                break;
            }

            case 0x4: //ADMIN_DELETE_IO_CQ
            {
                fv_uart_print("ADMIN_DELETE_IO_CQ: ctag:%x\r\n", rw_admd_ctg);
                lw_sqcq_ptr = rw_admd_dat(SLOC_QID);
                rq_cplq_prp(lw_sqcq_ptr) = 0x0;
                rw_cplq_qsz(lw_sqcq_ptr) = 0x0;
                rw_cplq_qid(lw_sqcq_ptr) = lw_sqcq_ptr;
                rw_cplq_vct(lw_sqcq_ptr) = 0x0;
                rb_cplq_cfg(lw_sqcq_ptr) = 0x3;
                rw_cqsd_sts = 0x0;
                rq_cqsd_dat = 0x0;

                break;
            }

            case 0x5: //ADMIN_CREATE_IO_CQ
            {
                fv_uart_print("ADMIN_CREATE_IO_CQ: ctag:%x\r\n", rw_admd_ctg);
                lw_sqcq_ptr = rw_admd_dat(SLOC_QID);
                rq_cplq_prp(lw_sqcq_ptr) = rq_admd_dat(SLOC_PRP >> 2);
                rw_cplq_qsz(lw_sqcq_ptr) = rw_admd_dat(SLOC_QSZ);
                rw_cplq_qid(lw_sqcq_ptr) = rw_admd_dat(SLOC_QID);
                rw_cplq_vct(lw_sqcq_ptr) = rw_admd_dat(SLOC_VCT);
                rb_cplq_cfg(lw_sqcq_ptr) = (byte)rw_admd_dat(SLOC_CFG);
                rw_cqsd_sts = 0x0;
                rq_cqsd_dat = 0x0;

                break;
            }

            case 0x6: //ADMIN_IDENTIFY
            {
                lb_idtf_cns = (byte)rw_admd_dat(SLOC_CNS);
                fv_uart_print("ADMIN_IDENTIFY: ctag:%x, cns:%x\r\n", rw_admd_ctg, lb_idtf_cns);
                if(lb_idtf_cns == 0x0) //namespace
                {
                    memcpy((byte *)(NVME_MEM_BASE), (byte *)(gd_idtf_ns), (IDTF_NS_LEN*4));
                    memset((byte *)(NVME_MEM_BASE+IDTF_NS_LEN*4), 0x0, (4096 - IDTF_NS_LEN*4));

                    //initial LBA unit
#ifdef N4KA_EN
                    rb_unit_lba = 0x0; //set LBA unit as 512B
#else
                    rb_unit_lba = 0x3; //set LBA unit as 4KB
#endif
                }
                else if(lb_idtf_cns == 0x1) //controller
                {
                    memcpy((byte *)(NVME_MEM_BASE), (byte *)(gd_idtf_ctl), (IDTF_CTL_LEN*4));
                    memset((byte *)(NVME_MEM_BASE+IDTF_CTL_LEN*4), 0x0, (4096 - IDTF_CTL_LEN*4));
                    memcpy((byte *)(NVME_MEM_BASE+2048), (byte *)(gd_idtf_ps), (IDTF_PS_LEN*4));
                }
                else if(lb_idtf_cns == 0x2) //namespace ID list
                {
                    *((dwrd *)NVME_MEM_BASE) = 1;
                    memset((byte *)(NVME_MEM_BASE+4), 0x0, 4092);
                }
                else if(lb_idtf_cns == 0x6) // I/O Command Set specific Identify Controller data structure
                {
                    memset((byte *)(NVME_MEM_BASE), 0x0, 4096);
                }
                else
                {
                    while(1);
                }

                rq_pdma_ba0 = rq_admd_dat(SLOC_PRP >> 2);
                rq_pdma_ba1 = rq_admd_dat(SLOC_PR2 >> 2);
                rd_pdma_set = (1024 << 16) | (0xff << 8) | PDMA_FUN(0) | PDMA_MWR | PDMA_ACT;
                while(rd_pdma_set & PDMA_ACT);
                rw_cqsd_sts = 0x0;
                rq_cqsd_dat = 0x0;

                break;
            }

            case 0x9: //ADMIN_SET_FEATURES
            {
                lb_feat_fid = (byte)rw_admd_dat(SLOC_FID);
                fv_uart_print("ADMIN_SET_FEATURES: ctag:%x, fid:%x\r\n", rw_admd_ctg, lb_feat_fid);
                rw_cqsd_sts = 0x0;
                if(lb_feat_fid == 0x7)
                {
                    rd_cqsd_dat(0) = rd_admd_dat(SLOC_FMG >> 1);
                    rd_cqsd_dat(1) = 0x0;
                }
                else
                {
                    rq_cqsd_dat = 0x0;
                }

                break;
            }

            case 0xA: //ADMIN_GET_FEATURES
            {
                fv_uart_print("ADMIN_GET_FEATURES: ctag:%x\r\n", rw_admd_ctg);
                rw_cqsd_sts = 0x0;
                rq_cqsd_dat = 0x0;

                break;
            }

            case 0xC: //ADMIN_ASYNCHRONOUS_EVENT_REQUEST
            {
                fv_uart_print("ADMIN_ASYNCHRONOUS_EVENT_REQUEST: ctag:%x\r\n", rw_admd_ctg);
                lb_cqsd_dis = 1;

                break;
            }

            default:
            {
                while(1);
            }
        }

        lw_cqsd_ctg = rw_admd_ctg;
        rb_admd_clr = 0x1; //clear this admin cmd

        if(lb_cqsd_dis)
        {
            //release ctag
            rw_cqdn_ptr = ADMD_QID;
            rd_cqdn_set = ((dwrd)lw_cqsd_ctg << 16) | CQDN_ACT;
            while(rd_cqdn_set & CQDN_ACT);
        }
        else
        {
            //send CQ cmd
            rd_cqsd_set = ((dwrd)lw_cqsd_ctg << 16) | CQSD_SIR | CQSD_SDN | CQSD_ACT;
            while(rd_cqsd_set & CQSD_ACT);
        }

//        //debug
//        gd_uart_msg[gb_uart_ptr] = ((dwrd)lb_feat_fid << 8) | (dwrd)lb_admd_opc;
//        gb_uart_ptr++;
//        gd_uart_msg[gb_uart_ptr] = rd_cqsd_dat(0);
//        gb_uart_ptr++;
    }

//    //check flush cmd
//    while(1)
//    {
//        lw_fluc_cnt = rw_fluc_cnt;
//       
//        if(lw_fluc_cnt == 0x0)
//        {
//            break;
//        } 
//        else if(lw_fluc_cnt != 0x1) //check multi flush cmd
//        {
//            while(1);
//        }
//
//        lw_cqsd_ctg = rw_fluc_ctg;
//        rb_fluc_clr = 0x1; //clear this flush cmd
//
//        //send CQ cmd
//        rw_cqsd_sts = 0x0;
//        rq_cqsd_dat = 0x0;
//        rd_cqsd_set = ((dwrd)lw_cqsd_ctg << 16) | CQSD_SIR | CQSD_SDN | CQSD_ACT;
//    }

    return;
}
