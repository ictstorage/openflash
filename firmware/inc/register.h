#ifndef _REGISTER_H_
#define _REGISTER_H_


/*******************************************************************************************************/
/********************                 global definition                       **************************/
/*******************************************************************************************************/
#define DTCM_REG_BASE    0x00400000
#define CPUA_REG_BASE    0x00600000

#define NVME_REG_BASE    0x10000000
#define PCIE_REG_BASE    0x10800000
#define DRAM_REG_BASE    0x11000000
#define DRAM_PHY_BASE    0x11800000
#define  BMU_REG_BASE    0x12000000
#define  ECC_REG_BASE    0x13000000
#define NAND_REG_BASE    0x14000000
#define NAND_PHY_BASE    0x14800000

#define BOOT_ROM_BASE    0x15000000
#define  SYS_REG_BASE    0x15100000
#define INTC_REG_BASE    0x15200000
#define UART_REG_BASE    0x15300000
#define GPIO_REG_BASE    0x15400000
#define  TMR_REG_BASE    0x15500000
#define  WDT_REG_BASE    0x15600000
#define  SPI_REG_BASE    0x15700000
#define  I2C_REG_BASE    0x15800000

#define SRAM_MEM_BASE    0x16000000
#define CPUA_EXT_BASE    0x17000000
#define ITCM_MEM_BASE    0x20000000
#define DTCM_MEM_BASE    0x20400000
#define DRAM_MEM_BASE    0x80000000


/*******************************************************************************************************/
/********************           cpu_agent DTCM register definition            **************************/
/*******************************************************************************************************/
//host cmd data
#define rs_host_cmd      (*(volatile HST_CMD *)(CPUA_REG_BASE+0x000))
  #define HCMD_WR          0x01
  #define HCMD_RD          0x02
  #define HCMD_OTH         0x03 //flush, trim.....
  #define LCMD_INV         0xFE
  #define HCMD_INV         0xFF
#define rb_host_get      (*(volatile byte    *)(CPUA_REG_BASE+0x000))
#define rb_init_fin      (*(volatile byte    *)(CPUA_REG_BASE+0x020))
//#define rb_pclk_set      (*(volatile byte    *)(CPUA_REG_BASE+0x024))
//#define rb_pclk_rdy      (*(volatile byte    *)(CPUA_REG_BASE+0x025))
//#define rb_pcie_fsm      (*(volatile byte    *)(CPUA_REG_BASE+0x026))
//#define rb_wait_wok      (*(volatile byte    *)(CPUA_REG_BASE+0x029))
#define rb_wait_wok      (*(volatile byte    *)(CPUA_REG_BASE+0x02A))
#define rw_cdir_wct      (*(volatile word    *)(CPUA_REG_BASE+0x02A))
#define rb_cdir_bas      (*(volatile byte    *)(CPUA_REG_BASE+0x02C))
#define rd_inda_msg      (*(volatile dwrd    *)(CPUA_REG_BASE+0x030))
#define rb_inda_exe      (*(volatile byte    *)(CPUA_REG_BASE+0x034))
#define rb_inda_rdy      (*(volatile byte    *)(CPUA_REG_BASE+0x035))
#define rq_inda_rdt      (*(volatile qwrd    *)(CPUA_REG_BASE+0x038))
#define rd_inda_rdt0     (*(volatile dwrd    *)(CPUA_REG_BASE+0x038))
#define rd_inda_rdt1     (*(volatile dwrd    *)(CPUA_REG_BASE+0x03C))

#define rb_lget_set      (*(volatile byte    *)(CPUA_REG_BASE+0x040)) //enable HW auto get l2p function
  #define LGET_ACT         (1 << 0)
#define rb_cawt_set      (*(volatile byte    *)(CPUA_REG_BASE+0x042)) //send wait messag to bmu
  #define CAWT_ACT         (1 << 0)
#define rb_cawt_rdy      (*(volatile byte    *)(CPUA_REG_BASE+0x043))
#define rw_lupd_set      (*(volatile word    *)(CPUA_REG_BASE+0x048)) //FW update l2p
  #define LUPD_ACT         (1 << 0)
  #define LUPD_GCQ         (3 << 1)
  #define LUPD_CRL         (0 << 8)
#define rw_lupd_cri      (*(volatile word    *)(CPUA_REG_BASE+0x04A)) //compression info
  #define LUPD_SDM         (1 << 0) //set CRD & CRM
  #define LUPD_CRD         (0 << 1) //Compression Data
  #define LUPD_CRM         (1 << 8) //Compression Map
#define rd_lupd_l4k      (*(volatile dwrd    *)(CPUA_REG_BASE+0x050))
#define rd_lupd_p4k      (*(volatile dwrd    *)(CPUA_REG_BASE+0x058))
#define rw_lupd_bfp      (*(volatile word    *)(CPUA_REG_BASE+0x060))

#define rb_gcad_vld      (*(volatile byte    *)(CPUA_REG_BASE+0x080))
#define rb_gcad_nul      (*(volatile byte    *)(CPUA_REG_BASE+0x081))
#define rb_gcad_fin      (*(volatile byte    *)(CPUA_REG_BASE+0x082))
#define rb_gcad_map      (*(volatile byte    *)(CPUA_REG_BASE+0x083)) //compression valid bitmap
#define rd_gcad_pcf      (*(volatile dwrd    *)(CPUA_REG_BASE+0x084))

#define rb_gcad_set      (*(volatile byte    *)(CPUA_REG_BASE+0x088))
  #define GCAD_ACT         (1 << 0)
  #define GCAD_EXT         (1 << 1)
#define rb_gcad_tfl      (*(volatile byte    *)(CPUA_REG_BASE+0x089)) //gc address ptr table full
#define rw_gcad_bsy      (*(volatile word    *)(CPUA_REG_BASE+0x088))
#define rw_gcad_bfp(n)   (*(volatile word    *)(CPUA_REG_BASE+0x090+((n)<<1)))
#define rd_gpcf_str      (*(volatile dwrd    *)(CPUA_REG_BASE+0x098))
#define rb_gwln_str      (*(volatile byte    *)(CPUA_REG_BASE+0x09C))
#define rb_pgrp_str      (*(volatile byte    *)(CPUA_REG_BASE+0x09D))
#define rw_gcad_blk      (*(volatile word    *)(CPUA_REG_BASE+0x0A2))

#define rb_mapu_sft_h    (*(volatile byte    *)(CPUA_REG_BASE+0x0B0))
#define rb_plan_sft_h    (*(volatile byte    *)(CPUA_REG_BASE+0x0B1))
#define rb_chan_sft_h    (*(volatile byte    *)(CPUA_REG_BASE+0x0B2))
#define rb_lnce_sft_h    (*(volatile byte    *)(CPUA_REG_BASE+0x0B3))
#define rb_page_sft_h    (*(volatile byte    *)(CPUA_REG_BASE+0x0B4))
#define rb_cmpr_sft_h    (*(volatile byte    *)(CPUA_REG_BASE+0x0B5))
#define rb_p4kj_che_h    (*(volatile byte    *)(CPUA_REG_BASE+0x0B6))
#define rb_gcad_che_h    (*(volatile byte    *)(CPUA_REG_BASE+0x0B7))
// #define rb_gcp4kj_che_h    (*(volatile byte    *)(CPUA_REG_BASE+0x0B7))
#define rb_mapu_end_h    (*(volatile byte    *)(CPUA_REG_BASE+0x0B8))
#define rb_plan_end_h    (*(volatile byte    *)(CPUA_REG_BASE+0x0B9))
#define rb_chan_end_h    (*(volatile byte    *)(CPUA_REG_BASE+0x0BA))
#define rb_lnce_end_h    (*(volatile byte    *)(CPUA_REG_BASE+0x0BB))
#define rw_page_end_h    (*(volatile word    *)(CPUA_REG_BASE+0x0BC))

#define rw_pgrp_tbl(n)   (*(volatile word    *)(CPUA_REG_BASE+0x0C0+((n)<<1)))

#define rw_pext_num      (*(volatile word    *)(CPUA_REG_BASE+0x0E0))
#define rd_gpcf_ptr      (*(volatile dwrd    *)(CPUA_REG_BASE+0x0E8))
#define rb_gwln_ptr      (*(volatile byte    *)(CPUA_REG_BASE+0x0EC))
#define rb_pgrp_ptr      (*(volatile byte    *)(CPUA_REG_BASE+0x0ED))

#define rd_ochk_pat      (*(volatile dwrd    *)(CPUA_REG_BASE+0x100))
#define rq_ochk_pat      (*(volatile qwrd    *)(CPUA_REG_BASE+0x100))
#define rd_olkp_pat      (*(volatile dwrd    *)(CPUA_REG_BASE+0x110))
#define rq_olkp_pat      (*(volatile qwrd    *)(CPUA_REG_BASE+0x110))
#define rb_olkp_exe      (*(volatile byte    *)(CPUA_REG_BASE+0x118))
#define rb_olkp_hit      (*(volatile byte    *)(CPUA_REG_BASE+0x119))
#define rb_olkp_err      (*(volatile byte    *)(CPUA_REG_BASE+0x11A))

#define rd_ochk_bpu(n)   (*(volatile dwrd    *)(CPUA_REG_BASE+0x120+((n)<<2)))
#define rb_ochk_map      (*(volatile byte    *)(CPUA_REG_BASE+0x140))
#define rb_oclr_map      (*(volatile byte    *)(CPUA_REG_BASE+0x144))

#define rb_gdma_exe      (*(volatile byte    *)(CPUA_REG_BASE+0x160))
#define rw_gdma_set      (*(volatile word    *)(CPUA_REG_BASE+0x160))
  #define GDMA_ACT         (1 << 0)
  #define GDMA_WR          (1 << 8)
#define rd_gdma_dat      (*(volatile dwrd    *)(CPUA_REG_BASE+0x164))
#define rq_gdma_len      (*(volatile qwrd    *)(CPUA_REG_BASE+0x168))
#define rq_gdma_sad      (*(volatile qwrd    *)(CPUA_REG_BASE+0x170))
#define rq_gdma_dad      (*(volatile qwrd    *)(CPUA_REG_BASE+0x178))

#define rb_hp4k_set      (*(volatile byte    *)(CPUA_REG_BASE+0x180))
  #define JUMP_ACT         (1 << 0)
#define rb_head_idx      (*(volatile byte    *)(CPUA_REG_BASE+0x181))
#define rb_mapu_loc      (*(volatile byte    *)(CPUA_REG_BASE+0x188))
#define rb_plan_loc      (*(volatile byte    *)(CPUA_REG_BASE+0x189))
#define rb_chan_loc      (*(volatile byte    *)(CPUA_REG_BASE+0x18A))
#define rb_lnce_loc      (*(volatile byte    *)(CPUA_REG_BASE+0x18B))
#define rw_page_loc      (*(volatile word    *)(CPUA_REG_BASE+0x18C))
#define rw_hblk_loc      (*(volatile word    *)(CPUA_REG_BASE+0x18E))
#define rb_dgrp_loc      (*(volatile byte    *)(CPUA_REG_BASE+0x190)) //die group
#define rb_gwln_loc      (*(volatile byte    *)(CPUA_REG_BASE+0x191)) //word line
#define rb_pgrp_loc      (*(volatile byte    *)(CPUA_REG_BASE+0x192)) //page group
#define rb_gwln_end      (*(volatile byte    *)(CPUA_REG_BASE+0x193)) //word line end
#define rw_pgrp_end      (*(volatile word    *)(CPUA_REG_BASE+0x194)) //page group end
#define rb_frag_loc      (*(volatile byte    *)(CPUA_REG_BASE+0x196))
#define rb_cech_loc      (*(volatile byte    *)(CPUA_REG_BASE+0x197))
#define rd_hp4k_loc      (*(volatile dwrd    *)(CPUA_REG_BASE+0x198))
#define rq_hp4k_loc      (*(volatile qwrd    *)(CPUA_REG_BASE+0x198))
#define rb_head_stg      (*(volatile byte    *)(CPUA_REG_BASE+0x1A0)) //head p4k stage

#define rd_cpua_reg0     (*(volatile dwrd    *)(CPUA_REG_BASE+0x200))
#define rd_cpua_reg1     (*(volatile dwrd    *)(CPUA_REG_BASE+0x208))
#define rd_cpua_reg2     (*(volatile dwrd    *)(CPUA_REG_BASE+0x210))
#define rd_cpua_reg3     (*(volatile dwrd    *)(CPUA_REG_BASE+0x218))
#define rd_cpua_reg4     (*(volatile dwrd    *)(CPUA_REG_BASE+0x220))
#define rd_cpua_reg5     (*(volatile dwrd    *)(CPUA_REG_BASE+0x228))
#define rd_cpua_reg6     (*(volatile dwrd    *)(CPUA_REG_BASE+0x230))
#define rd_cpua_reg7     (*(volatile dwrd    *)(CPUA_REG_BASE+0x238))
#define rd_cpua_reg8     (*(volatile dwrd    *)(CPUA_REG_BASE+0x240))
#define rd_cpua_reg9     (*(volatile dwrd    *)(CPUA_REG_BASE+0x248))
#define rd_cpua_reg10    (*(volatile dwrd    *)(CPUA_REG_BASE+0x250))
#define rd_cpua_reg11    (*(volatile dwrd    *)(CPUA_REG_BASE+0x258))
#define rd_cpua_reg12    (*(volatile dwrd    *)(CPUA_REG_BASE+0x260))
#define rd_cpua_reg13    (*(volatile dwrd    *)(CPUA_REG_BASE+0x268))
#define rd_cpua_reg14    (*(volatile dwrd    *)(CPUA_REG_BASE+0x270))
#define rd_cpua_reg15    (*(volatile dwrd    *)(CPUA_REG_BASE+0x278))
#define rq_cpua_reg15    (*(volatile qwrd    *)(CPUA_REG_BASE+0x278))

#define rw_cpua_set      (*(volatile word    *)(CPUA_REG_BASE+0x280))
  #define CPUA_ACT         (1 << 0)
  #define DATA_CMD         (0 << 8) //program data
  #define P2LP_CMD         (1 << 8) //p2l page
  #define RAPT_CMD         (2 << 8) //raid parity
#define rd_imem_set      (*(volatile dwrd    *)(CPUA_REG_BASE+0x288)) //cpua proc instruction memory setting
  #define IMEM_ACT         (1 << 0)
  #define IMEM_WEN         (1 << 8)
#define rw_imem_dat      (*(volatile word    *)(CPUA_REG_BASE+0x28C))



/*******************************************************************************************************/
/********************               HOST register definition                  **************************/
/*******************************************************************************************************/
#define NVME_STD_BASE    0x10000000 //NVMe Standard register
#define NVME_QUE_BASE    0x10001000 //NVMe SQ/CQ register
#define NVME_ADM_BASE    0x10200000 //NVMe Admin cmd register
#define NVME_LOC_BASE    0x10400000 //NVMe Local register
#define NVME_MEM_BASE    0x10500000 //NVMe cmem register

//NVMe Standard register

#define rd_stdr_sts(n)      (*(volatile dwrd *)(NVME_STD_BASE + ((n) << 12) + 0x1C))
    #define NVME_RDY                     (1 << 0)
    #define NVME_SHN_STS_NORMAL          (0 << 2)
    #define NVME_SHN_STS_PROC_OCR        (1 << 2)
    #define NVME_SHN_STS_PROC_CMP        (2 << 2)
    #define NVME_SHN_STS_MASK            (3 << 2)
    #define NVME_SHN_STS_CTL             (1 << 6)

#define rd_stdr_cfg(n)      (*(volatile dwrd *)(NVME_STD_BASE + ((n) << 12) + 0x14))
    #define NVME_ENA          (1 << 0)
    #define NVME_SHN_NORMAL   (1 << 14) //normal shutdown notify
    #define NVME_SHN_ABRUPT   (2 << 14) //NVME_SHN_ABRUPT shutdown notify  
#define rd_pcie_cfg_sel  (*(volatile dwrd *)(NVME_LOC_BASE + 0x30C))
    #define NVME_MRRS_BY_PCIE (0)
    #define NVME_MRRS_BY_FW   (1)
    #define NVME_MRRS_MIN     (2)
    #define NVME_MPS_BY_PCIE  (0)
    #define NVME_MPS_BY_FW    (1)
    #define NVME_MPS_MIN      (2)
#define rd_sbmq_adrl(n)   (*(volatile dwrd *)(NVME_ADM_BASE + 0x00 + ((n) << 6)))
    #define SQ_CREATED     (1 << 0)
    #define SQ_ADMIN       (1 << 1)
    #define SQ_IO          (0 << 1)
    #define SQ_SUS          (1 << 2)
#define rd_sbmq_cfg1(n)   (*(volatile dwrd *)(NVME_ADM_BASE + 0x0C + ((n) << 6)))
    #define SBMQ_QPC         (1 << 0)
    #define SBMQ_PRI(n)      (n << 1)
#define rd_cplq_tal(n)    (*(volatile dwrd *)(NVME_ADM_BASE + 0x30 + ((n) << 6)))
#define rd_cplq_phas(n)   (*(volatile dwrd *)(NVME_ADM_BASE + 0x34 + ((n) << 6)))
#define rd_sbmq_adrh(n)   (*(volatile dwrd *)(NVME_ADM_BASE + 0x04 + ((n) << 6)))
#define rd_cplq_adrl(n)   (*(volatile dwrd *)(NVME_ADM_BASE + 0x20 + ((n) << 6)))
    #define CQ_CREATED     (1 << 0)
    #define CQ_ADMIN       (1 << 1)
    #define CQ_IO          (0 << 1)

#define rd_sbmq_hdr(n)    (*(volatile dwrd *)(NVME_ADM_BASE + 0x10 + ((n) << 6)))
#define rd_sbmq_loc(n)    (*(volatile dwrd *)(NVME_ADM_BASE + 0x14 + ((n) << 6)))
#define rd_cplq_adrh(n)   (*(volatile dwrd *)(NVME_ADM_BASE + 0x24 + ((n) << 6)))
#define rd_sbcp_qdbl(n)   (*(volatile dwrd *)(NVME_QUE_BASE + 0x0 + ((n) << 3)))
#define rw_stdr_asqs(n)      (*(volatile word *)(NVME_STD_BASE + ((n) << 12) + 0x24)) // ACQS: Admin Completion Queue Size
#define rw_stdr_acqs(n)      (*(volatile word *)(NVME_STD_BASE + ((n) << 12) + 0x26)) // ASQS: Admin Submit Queue Size
#define rd_func_qloc(n)     (*(volatile dwrd *)(0x10600000 + ((n) << 8) + 0x10))
#define rd_func_qnum(n)     (*(volatile dwrd *)(0x10600000 + ((n) << 8) + 0x14))
#define rd_msi_addr_l      (*(volatile dwrd *)(NVME_LOC_BASE + 0x450))
#define rd_msi_addr_h      (*(volatile dwrd *)(NVME_LOC_BASE + 0x454))
#define rd_msi_addr_data   (*(volatile dwrd *)(NVME_LOC_BASE + 0x458))
#define rd_ns_tbl_unit   (*(volatile dwrd *)(NVME_LOC_BASE + 0x2C0))
#define rw_cqcs_sts      (*(volatile word *)(NVME_LOC_BASE + 0x24))
    #define rd_cqcs_dat(n)   (*(volatile dwrd *)(NVME_LOC_BASE + 0x28 + ((n) << 2)))
    #define rq_cqcs_dat      (*(volatile qwrd *)(NVME_LOC_BASE + 0x28))
    #define rw_cqdn_set      (*(volatile word *)(NVME_LOC_BASE + 0x30))
        #define CQDN_ACT         (1 << 0)
    #define rd_cqcs_set      (*(volatile dwrd *)(NVME_LOC_BASE + 0x20))
        #define CQSD_ACT         (1 << 0)
        #define CQSD_SDN         (1 << 1) //send cqdn
        #define CQSD_NDN         (0 << 1) //no send cqdn
        #define CQSD_SIR         (1 << 2) //send interrupt
        #define CQSD_NIR         (0 << 2) //no send interrupt

//NVMe SQ/CQ register
#define rw_sbmq_tal(n)   (*(volatile word *)(NVME_QUE_BASE+0x0+((n)<<3)))
#define rw_cplq_hdr(n)   (*(volatile word *)(NVME_QUE_BASE+0x4+((n)<<3)))

//NVMe Admin register
#define rq_sbmq_prp(n)   (*(volatile qwrd *)(NVME_ADM_BASE+0x00+((n)<<5)))
#define rw_sbmq_qsz(n)   (*(volatile word *)(NVME_ADM_BASE+0x08+((n)<<5)))
#define rw_sbmq_qid(n)   (*(volatile word *)(NVME_ADM_BASE+0x0A+((n)<<5)))
#define rw_sbmq_cpl(n)   (*(volatile word *)(NVME_ADM_BASE+0x0C+((n)<<5)))
#define rb_sbmq_cfg(n)   (*(volatile byte *)(NVME_ADM_BASE+0x0E+((n)<<5)))
  #define SBMQ_QPC         (1 << 0)
  #define SBMQ_PRI(n)      (n << 1)
#define rb_sbmq_fun(n)   (*(volatile byte *)(NVME_ADM_BASE+0x0F+((n)<<5)))
#define rq_cplq_prp(n)   (*(volatile qwrd *)(NVME_ADM_BASE+0x10+((n)<<5)))
#define rw_cplq_qsz(n)   (*(volatile word *)(NVME_ADM_BASE+0x18+((n)<<5)))
#define rw_cplq_qid(n)   (*(volatile word *)(NVME_ADM_BASE+0x1A+((n)<<5)))
#define rw_cplq_vct(n)   (*(volatile word *)(NVME_ADM_BASE+0x1C+((n)<<5)))
#define rb_cplq_cfg(n)   (*(volatile byte *)(NVME_ADM_BASE+0x1E+((n)<<5)))
  #define CPLQ_QPC         (1 << 0)
  #define CPLQ_IEN         (1 << 1)

//NVMe Local register
#define rw_sqio_thr      (*(volatile word *)(NVME_LOC_BASE+0x1A))
#define rw_ctag_end      (*(volatile word *)(NVME_LOC_BASE+0x1C))
#define rd_cqsd_set      (*(volatile dwrd *)(NVME_LOC_BASE+0x20))
  #define CQSD_ACT         (1 << 0)
  #define CQSD_SDN         (1 << 1) //send cqdn
  #define CQSD_NDN         (0 << 1) //no send cqdn
  #define CQSD_SIR         (1 << 2) //send interrupt
  #define CQSD_NIR         (0 << 2) //no send interrupt
#define rw_cqsd_ctg      (*(volatile word *)(NVME_LOC_BASE+0x22))
#define rw_cqsd_sts      (*(volatile word *)(NVME_LOC_BASE+0x24))
#define rd_cqsd_dat(n)   (*(volatile dwrd *)(NVME_LOC_BASE+0x28+((n)<<2)))
#define rq_cqsd_dat      (*(volatile qwrd *)(NVME_LOC_BASE+0x28))

#define rd_cqdn_set      (*(volatile dwrd *)(NVME_LOC_BASE+0x30))
  #define CQDN_ACT         (1 << 0)
#define rw_cqdn_ctg      (*(volatile word *)(NVME_LOC_BASE+0x32))
#define rw_cqdn_ptr      (*(volatile word *)(NVME_LOC_BASE+0x34))

#define rd_pdma_set      (*(volatile dwrd *)(NVME_LOC_BASE+0x40))
  #define PDMA_ACT         (1 << 0)
  #define PDMA_MWR         (1 << 1)
  #define PDMA_MRD         (0 << 1)
  #define PDMA_FUN(n)      (n << 2)
#define rb_pdma_ben      (*(volatile byte *)(NVME_LOC_BASE+0x41))
#define rw_pdma_dsz      (*(volatile word *)(NVME_LOC_BASE+0x42))
#define rq_pdma_ba0      (*(volatile qwrd *)(NVME_LOC_BASE+0x48))
#define rq_pdma_ba1      (*(volatile qwrd *)(NVME_LOC_BASE+0x50))

#define rb_unit_lba      (*(volatile byte *)(NVME_LOC_BASE+0x60))
#define rb_nvme_hmt      (*(volatile byte *)(NVME_LOC_BASE+0x64))
#define rb_htmt_err      (*(volatile byte *)(NVME_LOC_BASE+0x65))

#define rb_admd_clr      (*(volatile byte *)(NVME_LOC_BASE+0x78))
#define rb_admd_vld      (*(volatile byte *)(NVME_LOC_BASE+0x78))
#define rw_admd_ctg      (*(volatile word *)(NVME_LOC_BASE+0x7A))
//#define rb_fluc_clr      (*(volatile byte *)(NVME_LOC_BASE+0x7C))
//#define rw_fluc_cnt      (*(volatile word *)(NVME_LOC_BASE+0x7C))
//#define rw_fluc_ctg      (*(volatile word *)(NVME_LOC_BASE+0x7E))
#define rw_admd_dat(n)   (*(volatile word *)(NVME_LOC_BASE+0x80+((n)<<1)))
#define rd_admd_dat(n)   (*(volatile dwrd *)(NVME_LOC_BASE+0x80+((n)<<2)))
#define rq_admd_dat(n)   (*(volatile qwrd *)(NVME_LOC_BASE+0x80+((n)<<3)))

//NVMe cmem register
#define rd_cmem_dat(n)   (*(volatile dwrd *)(NVME_MEM_BASE+((n)<<2)))


//PCIe Ctl register



/*******************************************************************************************************/
/********************               DRAM register definition                  **************************/
/*******************************************************************************************************/



/*******************************************************************************************************/
/********************                BMU register definition                  **************************/
/*******************************************************************************************************/
//cpu related
#define rb_nddr_vld      (*(volatile byte *)(BMU_REG_BASE+0x00))
  #define NDDR_CLR         (1 << 0)
#define rw_nddr_bfp      (*(volatile word *)(BMU_REG_BASE+0x02))
#define rd_nddr_pad      (*(volatile dwrd *)(BMU_REG_BASE+0x08))
#define rb_nddr_ptr      (*(volatile byte *)(BMU_REG_BASE+0x0D))

#define rb_gcrd_vld      (*(volatile byte *)(BMU_REG_BASE+0x20))
  #define GCRD_CLR         (1 << 0)
#define rb_gcrd_msz      (*(volatile byte *)(BMU_REG_BASE+0x21))
#define rw_gcrd_bfp      (*(volatile word *)(BMU_REG_BASE+0x22))
#define rb_gcrd_crd      (*(volatile byte *)(BMU_REG_BASE+0x24))
#define rd_gcrd_pad      (*(volatile dwrd *)(BMU_REG_BASE+0x28))
#define rq_gcrd_pad      (*(volatile qwrd *)(BMU_REG_BASE+0x28))
#define rd_gcrd_cid(n)   (*(volatile dwrd *)(BMU_REG_BASE+0x30+((n)<<2)))

#define rb_errd_vld      (*(volatile byte *)(BMU_REG_BASE+0x40))
  #define ERRD_CLR         (1 << 0)
#define rb_errd_msz      (*(volatile byte *)(BMU_REG_BASE+0x41))
#define rw_errd_bfp      (*(volatile word *)(BMU_REG_BASE+0x42))
#define rw_errd_rid      (*(volatile word *)(BMU_REG_BASE+0x44))
#define rb_errd_lid      (*(volatile byte *)(BMU_REG_BASE+0x46))
#define rb_errd_que      (*(volatile byte *)(BMU_REG_BASE+0x47))
#define rd_errd_pad      (*(volatile dwrd *)(BMU_REG_BASE+0x48))
#define rq_errd_pad      (*(volatile qwrd *)(BMU_REG_BASE+0x48))
#define rw_errd_ptr      (*(volatile word *)(BMU_REG_BASE+0x4E))
#define rd_errd_cid(n)   (*(volatile dwrd *)(BMU_REG_BASE+0x50+((n)<<2)))

#define rb_smrd_vld      (*(volatile byte *)(BMU_REG_BASE+0x60))
  #define SMRD_CLR         (1 << 0)
#define rb_smrd_msz      (*(volatile byte *)(BMU_REG_BASE+0x61))
#define rw_smrd_bfp      (*(volatile word *)(BMU_REG_BASE+0x62))
#define rw_smrd_rid      (*(volatile word *)(BMU_REG_BASE+0x64))
#define rb_smrd_lid      (*(volatile byte *)(BMU_REG_BASE+0x66))
#define rd_smrd_pad      (*(volatile dwrd *)(BMU_REG_BASE+0x68))
#define rq_smrd_pad      (*(volatile qwrd *)(BMU_REG_BASE+0x68))
#define rd_smrd_cid      (*(volatile dwrd *)(BMU_REG_BASE+0x70))

#define rb_trrd_vld      (*(volatile byte *)(BMU_REG_BASE+0x80))
  #define TRRD_CLR         (1 << 0)
#define rw_trrd_bfp      (*(volatile word *)(BMU_REG_BASE+0x82))
#define rw_trrd_dmp      (*(volatile word *)(BMU_REG_BASE+0x84)) //dram ptr

#define rb_n4rd_vld      (*(volatile byte *)(BMU_REG_BASE+0x90))
  #define N4RD_CLR         (1 << 0)
#define rb_n4rd_msz      (*(volatile byte *)(BMU_REG_BASE+0x91))
#define rw_n4rd_bfp      (*(volatile word *)(BMU_REG_BASE+0x92))
#define rd_n4rd_idx      (*(volatile dwrd *)(BMU_REG_BASE+0x94))
#define rb_n4rd_idx      (*(volatile byte *)(BMU_REG_BASE+0x94))

#define rb_cafp_set      (*(volatile byte *)(BMU_REG_BASE+0x98))
  #define CAFP_ACT         (1 << 0)
  #define DRAM_BUF         (0 << 1)
  #define SRAM_BUF         (1 << 1)
  #define PRTY_BUF         (2 << 1)
  #define HOST_GET         (0 << 3)
  #define NAND_GET         (1 << 3)
  #define DWDM_GET         (2 << 3)                              //DECW/DRAM
  #define CPUA_GET         (3 << 3)
#define rb_cafp_rlt      (*(volatile byte *)(BMU_REG_BASE+0x99))
  #define CAFP_BOK         (1 << 0)
#define rw_cafp_ptr      (*(volatile word *)(BMU_REG_BASE+0x9A))

#define rd_carl_set      (*(volatile dwrd *)(BMU_REG_BASE+0x9C))
  #define CARL_ACT         (1 << 0)
#define rw_carl_bfp      (*(volatile word *)(BMU_REG_BASE+0x9E))

#define rb_cawf_set      (*(volatile byte *)(BMU_REG_BASE+0xA0))
#define rd_cawf_set      (*(volatile dwrd *)(BMU_REG_BASE+0xA0))
  #define CAWF_ACT         (1 << 0)
  #define CAWF_CRD         (1 << 1)                              //compression data
  #define CAWF_RLS         (1 << 2)
  #define CAWF_KEP         (0 << 2)
  #define CAWF_RFN         (1 << 3)
  #define CAWF_RFB         (0 << 3)
  #define CAWF_UNC         (1 << 4)
  #define CAWF_WAT         (1 << 5)
  #define CAWF_MSZ         (((FMT_MODE << 3) | HMT_MODE) << 8)   //host meta size, 0:8B,1:16B,2:32B,3:64B,4:128B,5:0B
#define rb_cawf_msz      (*(volatile byte *)(BMU_REG_BASE+0xA1))
#define rw_cawf_bfp      (*(volatile word *)(BMU_REG_BASE+0xA2))
#define rw_cawf_rid      (*(volatile word *)(BMU_REG_BASE+0xA4))
#define rb_cawf_lid      (*(volatile byte *)(BMU_REG_BASE+0xA6))
#define rb_cawf_que      (*(volatile byte *)(BMU_REG_BASE+0xA7))
#define rd_cawf_pad      (*(volatile dwrd *)(BMU_REG_BASE+0xA8))
#define rq_cawf_pad      (*(volatile qwrd *)(BMU_REG_BASE+0xA8))
#define rw_cawf_ptr      (*(volatile word *)(BMU_REG_BASE+0xAE))
#define rd_cawf_cid(n)   (*(volatile dwrd *)(BMU_REG_BASE+0xB0+((n)<<2)))


//H/W related
#define rb_pstr_bmu      (*(volatile byte *)(BMU_REG_BASE+0xC0)) //page bit start
#define rb_psft_bmu      (*(volatile byte *)(BMU_REG_BASE+0xC1)) //page bit shift
#define rw_nerr_num      (*(volatile word *)(BMU_REG_BASE+0xC2))
#define rb_rdmz_en       (*(volatile byte *)(BMU_REG_BASE+0xC4))

#define rb_itbl_set      (*(volatile byte *)(BMU_REG_BASE+0xD4))
  #define ITBL_ACT         (1 << 0)
#define rw_itbl_dat      (*(volatile word *)(BMU_REG_BASE+0xD6))

#define rw_fcfg_set      (*(volatile word *)(BMU_REG_BASE+0xD8)) //function config
  #define CHAN_TYP         (0 <<  0)
  #define RAID_ENA         (3 <<  2)
  #define RMAP_HID         (1 <<  4)
  #define CACQ_ALL         (1 <<  5)
  #define HFRC_ADD         (1 <<  6)
  #define GCWR_ADD         (1 <<  7)
  #define RAID_TYP         (3 << 14)

#define rw_dmdt_bas      (*(volatile word *)(BMU_REG_BASE+0xDC)) //dram data buffer base address
#define rw_dmmt_bas      (*(volatile word *)(BMU_REG_BASE+0xDE)) //dram meta buffer(128+32Bytes) base address

#define rb_ddma_act      (*(volatile byte *)(BMU_REG_BASE+0xE0))
#define rb_ddma_wen      (*(volatile byte *)(BMU_REG_BASE+0xE1))
#define rw_ddma_len      (*(volatile word *)(BMU_REG_BASE+0xE2)) //dma length, max 8KB length, 8byte unit
#define rd_ddma_dad      (*(volatile dwrd *)(BMU_REG_BASE+0xE4))
#define rd_ddma_sad      (*(volatile dwrd *)(BMU_REG_BASE+0xE8))

#define rb_dbuf_rdy      (*(volatile byte *)(BMU_REG_BASE+0xEC))
#define rb_sbuf_rdy      (*(volatile byte *)(BMU_REG_BASE+0xED))
#define rb_pbuf_rdy      (*(volatile byte *)(BMU_REG_BASE+0xEE))
#define rb_abuf_set      (*(volatile byte *)(BMU_REG_BASE+0xEF))
  #define ABUF_ACT         (1 << 0)
  #define HTRD_BUF         (0 << 1)
  #define GCRD_BUF         (1 << 1)
  #define RAID_BUF         (2 << 1)

#define rd_rgrp_cfg      (*(volatile dwrd *)(BMU_REG_BASE+0xF0))
#define rw_rtbl_set      (*(volatile word *)(BMU_REG_BASE+0xF4))
  #define RTBL_ACT         (1 << 12)
  #define RTBL_WR          (1 << 13)
  #define RTBL_RD          (0 << 13)
  #define RTBL_MSG         (1 << 14)
#define rb_rtbl_num      (*(volatile byte *)(BMU_REG_BASE+0xF6))
#define rb_rtbl_idx      (*(volatile byte *)(BMU_REG_BASE+0xF7))
#define rw_rtbl_r5p      (*(volatile word *)(BMU_REG_BASE+0xF8))
#define rw_rtbl_r6p      (*(volatile word *)(BMU_REG_BASE+0xFA))
#define rw_rsts_ptr      (*(volatile word *)(BMU_REG_BASE+0xFC))
#define rb_rbuf_rdy      (*(volatile byte *)(BMU_REG_BASE+0xFE))
#define rb_rbuf_prg      (*(volatile byte *)(BMU_REG_BASE+0xFF))

#define rw_htdl_thr      (*(volatile word *)(BMU_REG_BASE+0x100))
#define rw_htdb_thr      (*(volatile word *)(BMU_REG_BASE+0x102))
#define rw_htsl_thr      (*(volatile word *)(BMU_REG_BASE+0x104))
#define rw_htsb_thr      (*(volatile word *)(BMU_REG_BASE+0x106))
#define rw_htpl_thr      (*(volatile word *)(BMU_REG_BASE+0x108))
#define rw_htpb_thr      (*(volatile word *)(BMU_REG_BASE+0x10A))
#define rw_hrsl_thr      (*(volatile word *)(BMU_REG_BASE+0x114))
#define rw_hrsb_thr      (*(volatile word *)(BMU_REG_BASE+0x116))
#define rw_hrpl_thr      (*(volatile word *)(BMU_REG_BASE+0x118))
#define rw_hrpb_thr      (*(volatile word *)(BMU_REG_BASE+0x11A))
#define rw_gcdl_thr      (*(volatile word *)(BMU_REG_BASE+0x11C))
#define rw_gcdb_thr      (*(volatile word *)(BMU_REG_BASE+0x11E))
#define rw_gcsl_thr      (*(volatile word *)(BMU_REG_BASE+0x120))
#define rw_gcsb_thr      (*(volatile word *)(BMU_REG_BASE+0x122))
#define rw_gcpl_thr      (*(volatile word *)(BMU_REG_BASE+0x124))
#define rw_gcpb_thr      (*(volatile word *)(BMU_REG_BASE+0x126))

#define rd_lctl_dbg      (*(volatile dwrd *)(BMU_REG_BASE+0x240))

#define rw_hrbf_num      (*(volatile word *)(BMU_REG_BASE+0x1A8))
#define rw_gcbf_num      (*(volatile word *)(BMU_REG_BASE+0x1AA))
#define rw_rabf_num      (*(volatile word *)(BMU_REG_BASE+0x1AC))

#define rw_htrd_num      (*(volatile word *)(BMU_REG_BASE+0x1B0))
#define rw_gcrd_num      (*(volatile word *)(BMU_REG_BASE+0x1B2))
#define rw_errd_num      (*(volatile word *)(BMU_REG_BASE+0x1B4))
#define rw_dmrd_num      (*(volatile word *)(BMU_REG_BASE+0x1B6))
#define rw_smrd_num      (*(volatile word *)(BMU_REG_BASE+0x1B8))
#define rw_rard_num      (*(volatile word *)(BMU_REG_BASE+0x1BA))
#define rw_trrd_num      (*(volatile word *)(BMU_REG_BASE+0x1BC))
#define rw_n4rd_num      (*(volatile word *)(BMU_REG_BASE+0x1BE))

#define rb_nddn_bit      (*(volatile byte *)(BMU_REG_BASE+0x1C0))

#define rq_tmem_ben      (*(volatile qwrd *)(BMU_REG_BASE+0x200))
#define rb_tmem_dat      (*(volatile byte *)(BMU_REG_BASE+0x208))

#define rd_ltbl_set      (*(volatile dwrd *)(BMU_REG_BASE+0x210))
  #define LTBL_ACT         (1 << 0)
  #define LTBL_WR          (1 << 8)
  #define LTBL_RD          (0 << 8)
#define rd_ltbl_l4k      (*(volatile dwrd *)(BMU_REG_BASE+0x218))

//status table
#define rd_stbl_dat(n)   (*(volatile dwrd *)(BMU_REG_BASE+0x80000+((n)<<2)))

//ram data buffer access
#define rd_smdt_dat(n,m) (*(volatile dwrd *)(SRAM_MEM_BASE+((n)<<12)+((m)<<2)))

//ram meta data access
#define rd_smmt_dat(n,m) (*(volatile dwrd *)(SRAM_META_BASE+(((n)&BPTR_MSK)<<12)+((m)<<2))) //meta, m:0~39 
#define rd_dmmt_dat(n,m) (*(volatile dwrd *)(DRAM_META_BASE+((n)<<8)+((m)<<2)))
#define rq_smmt_dat(n,m) (*(volatile qwrd *)(SRAM_META_BASE+(((n)&BPTR_MSK)<<12)+((m)<<3))) //meta, m:0~19 
#define rq_dmmt_dat(n,m) (*(volatile qwrd *)(DRAM_META_BASE+((n)<<8)+((m)<<3)))



/*******************************************************************************************************/
/********************                ECC register definition                  **************************/
/*******************************************************************************************************/
#define rb_elsm_ptr      (*(volatile byte *)(ECC_REG_BASE+0xC4)) //ecc status message pointer
#define rw_elsm_dat      (*(volatile word *)(ECC_REG_BASE+0xD8)) //ecc status


/*******************************************************************************************************/
/********************               NAND register definition                  **************************/
/*******************************************************************************************************/
//NAND cmd
#define rw_cmd_type      (*(volatile word *)(NAND_REG_BASE+0x00))
  #define ERS_CPTR         (1 << 0)
  #define PROG_CPTR        (2 << 0)
  #define READ_CPTR        (3 << 0)
  #define AIPR_CPTR        (7 << 0)
  #define URG_PRI          (0 << 8)
  #define HIGH_PRI         (1 << 8)
  #define LOW_PRI          (2 << 8)
  #define AIPR_PRI         (3 << 8)
  #define CMD_BUF          (1 << 10)
  #define CMD_END          (1 << 11)
  #define CMD_DMAP         (3 << 12)
#define rw_cmd_bptr      (*(volatile word *)(NAND_REG_BASE+0x02))
#define rd_cmd_padr      (*(volatile dwrd *)(NAND_REG_BASE+0x04))

#define rw_cmd_ridx      (*(volatile word *)(NAND_REG_BASE+0x08))
#define rb_cmd_misc      (*(volatile byte *)(NAND_REG_BASE+0x0A))
#define rb_cmd_qidx      (*(volatile byte *)(NAND_REG_BASE+0x0B))
#define rb_cmd_mtsz      (*(volatile byte *)(NAND_REG_BASE+0x0C)) //meta size
#define rb_cmd_ptsz      (*(volatile byte *)(NAND_REG_BASE+0x0D)) //parity size
#define rb_cmd_lidx      (*(volatile byte *)(NAND_REG_BASE+0x0E)) //ldpc index
#define rb_cmd_ltyp      (*(volatile byte *)(NAND_REG_BASE+0x0F)) //ldpc type: 0:ms, 1:bf
#define rw_cmd_othr      (*(volatile word *)(NAND_REG_BASE+0x0E))
#define rb_cmd_rtp       (*(volatile byte *)(NAND_REG_BASE+0x10))
#define rd_cmd_cid(n)    (*(volatile dwrd *)(NAND_REG_BASE+0x10+((n)<<2)))
#define rd_cmd_cid1      (*(volatile dwrd *)(NAND_REG_BASE+0x14))
#define rd_cmd_cid2      (*(volatile dwrd *)(NAND_REG_BASE+0x18))
#define rd_cmd_cid3      (*(volatile dwrd *)(NAND_REG_BASE+0x1C))
#define rq_cmd_tmt       (*(volatile qwrd *)(NAND_REG_BASE+0x20)) //table range meta
#define rw_cmd_dmpt      (*(volatile word *)(NAND_REG_BASE+0x28)) //table range base address
#define rw_cmd_proc      (*(volatile word *)(NAND_REG_BASE+0x2C))
#define rb_cmd_busy      (*(volatile byte *)(NAND_REG_BASE+0x31))
#define rb_nblk_idx      (*(volatile byte *)(NAND_REG_BASE+0x32))

#define rb_hacc_fun      (*(volatile byte *)(NAND_REG_BASE+0x40))
  #define QSWH_EN          (1 << 0)
  #define RCAH_EN          (1 << 1)
  #define WCAH_EN          (1 << 2)
  #define AIPR_EN          (1 << 3)
  #define SUSP_EN          (1 << 4)
  #define WSEQ_EN          (1 << 5)
  #define RPTY_GET         (1 << 6)
  #define BSTS_CHK         (1 << 7)

#define rb_frag_end      (*(volatile byte *)(NAND_REG_BASE+0x44))
#define rb_auto_fun      (*(volatile byte *)(NAND_REG_BASE+0x45))
  #define AUTO_WEND        (1 << 0)
  #define AUTO_REND        (1 << 1)
  #define CMPR_MERGE       (1 << 2)
#define rd_tout_thr      (*(volatile dwrd *)(NAND_REG_BASE+0x4C))

#define rb_ladr_chk      (*(volatile byte *)(NAND_REG_BASE+0x64))
#define rd_cmd_ladr      (*(volatile dwrd *)(NAND_REG_BASE+0x68))


//NAND setting
#define rb_index         (*(volatile byte *)(NAND_REG_BASE+0x100))
#define rb_nand_md       (*(volatile byte *)(NAND_REG_BASE+0x102))
  #define LEGACY_MD        (1 << 0)
  #define ASIC_MD          (1 << 1)
#define rb_inst_share    (*(volatile byte *)(NAND_REG_BASE+0x103))

#define rb_frag_sft      (*(volatile byte *)(NAND_REG_BASE+0x104))
#define rb_chan_sft      (*(volatile byte *)(NAND_REG_BASE+0x105))
#define rb_ce_sft        (*(volatile byte *)(NAND_REG_BASE+0x106))
#define rb_lunce_sft     (*(volatile byte *)(NAND_REG_BASE+0x107))
#define rb_page_sft      (*(volatile byte *)(NAND_REG_BASE+0x108))
#define rb_blk_sft       (*(volatile byte *)(NAND_REG_BASE+0x109))
#define rb_plane_sft     (*(volatile byte *)(NAND_REG_BASE+0x10A))
#define rb_plane_num     (*(volatile byte *)(NAND_REG_BASE+0x10B))
#define rw_mapu_size     (*(volatile word *)(NAND_REG_BASE+0x10C))
#define rw_pg_size       (*(volatile word *)(NAND_REG_BASE+0x10E))
#define rb_nrd_ten       (*(volatile byte *)(NAND_REG_BASE+0x111))
#define rw_nrd_thr       (*(volatile word *)(NAND_REG_BASE+0x112))
#define rd_phy_chan      (*(volatile dwrd *)(NAND_REG_BASE+0x114))
#define rd_phy_ce0       (*(volatile dwrd *)(NAND_REG_BASE+0x118))
#define rd_phy_ce1       (*(volatile dwrd *)(NAND_REG_BASE+0x11C))

#define rb_sts_cfg       (*(volatile byte *)(NAND_REG_BASE+0x140))
#define rb_rds_mask      (*(volatile byte *)(NAND_REG_BASE+0x144))
#define rb_rds_info      (*(volatile byte *)(NAND_REG_BASE+0x145))
#define rb_fail_mask     (*(volatile byte *)(NAND_REG_BASE+0x146))
#define rb_fail_info     (*(volatile byte *)(NAND_REG_BASE+0x147))
#define rw_tmr_base      (*(volatile word *)(NAND_REG_BASE+0x148))

#define rw_tmr_data0     (*(volatile word *)(NAND_REG_BASE+0x160))
#define rw_tmr_data1     (*(volatile word *)(NAND_REG_BASE+0x162))
#define rw_tmr_data2     (*(volatile word *)(NAND_REG_BASE+0x164))
#define rw_tmr_data3     (*(volatile word *)(NAND_REG_BASE+0x166))
#define rw_tmr_data4     (*(volatile word *)(NAND_REG_BASE+0x168))
#define rw_tmr_data5     (*(volatile word *)(NAND_REG_BASE+0x16A))
#define rw_tmr_data6     (*(volatile word *)(NAND_REG_BASE+0x16C))
#define rw_tmr_data7     (*(volatile word *)(NAND_REG_BASE+0x16E))

#define rb_tcr           (*(volatile byte *)(NAND_REG_BASE+0x1C0))
#define rb_tcs           (*(volatile byte *)(NAND_REG_BASE+0x1C1))
#define rb_tcs1          (*(volatile byte *)(NAND_REG_BASE+0x1C2))
#define rb_tcd           (*(volatile byte *)(NAND_REG_BASE+0x1C3))
#define rb_tcsd          (*(volatile byte *)(NAND_REG_BASE+0x1C4))
#define rb_tceh          (*(volatile byte *)(NAND_REG_BASE+0x1C5))
#define rb_tclr          (*(volatile byte *)(NAND_REG_BASE+0x1C6))
#define rb_tar           (*(volatile byte *)(NAND_REG_BASE+0x1C7))
#define rb_tcals2        (*(volatile byte *)(NAND_REG_BASE+0x1C8))
#define rb_twhr          (*(volatile byte *)(NAND_REG_BASE+0x1C9))
#define rb_twc           (*(volatile byte *)(NAND_REG_BASE+0x1CA))
#define rb_twh           (*(volatile byte *)(NAND_REG_BASE+0x1CB))
#define rb_tch           (*(volatile byte *)(NAND_REG_BASE+0x1CC))
#define rb_tadl          (*(volatile byte *)(NAND_REG_BASE+0x1CD))
#define rb_trhw          (*(volatile byte *)(NAND_REG_BASE+0x1CE))
#define rb_tcdqss        (*(volatile byte *)(NAND_REG_BASE+0x1CF))
#define rb_trpre         (*(volatile byte *)(NAND_REG_BASE+0x1D0))
#define rb_twpre         (*(volatile byte *)(NAND_REG_BASE+0x1D1))
#define rb_tcals         (*(volatile byte *)(NAND_REG_BASE+0x1D2))
#define rb_tcalh         (*(volatile byte *)(NAND_REG_BASE+0x1D3))
#define rb_twp           (*(volatile byte *)(NAND_REG_BASE+0x1D4))
#define rb_tcas          (*(volatile byte *)(NAND_REG_BASE+0x1D5))
#define rb_tdqsrh        (*(volatile byte *)(NAND_REG_BASE+0x1D6))
#define rb_trpst         (*(volatile byte *)(NAND_REG_BASE+0x1D7))
#define rb_trpsth        (*(volatile byte *)(NAND_REG_BASE+0x1D8))
#define rb_tchz          (*(volatile byte *)(NAND_REG_BASE+0x1D9))
#define rb_tclhz         (*(volatile byte *)(NAND_REG_BASE+0x1DA))
#define rb_twpst         (*(volatile byte *)(NAND_REG_BASE+0x1DB))
#define rb_twpsth        (*(volatile byte *)(NAND_REG_BASE+0x1DC))
#define rb_tcdqsh        (*(volatile byte *)(NAND_REG_BASE+0x1DD))
#define rb_tcres         (*(volatile byte *)(NAND_REG_BASE+0x1DE))
#define rb_tdbs          (*(volatile byte *)(NAND_REG_BASE+0x1DF))
#define rb_tchk_en       (*(volatile byte *)(NAND_REG_BASE+0x1E0))

#define rb_ldqs_act      (*(volatile byte *)(NAND_REG_BASE+0x1F5))
//#define rb_opr_mode      (*(volatile byte *)(NAND_REG_BASE+0x1F1))
//#define rw_dll_rstn      (*(volatile word *)(NAND_REG_BASE+0x1F4))
//#define rw_init_cpl      (*(volatile word *)(NAND_REG_BASE+0x1F6))


//channel register
#define rw_nand_rdt      (*(volatile word *)(NAND_REG_BASE+0x1000))
#define rw_nand_wdt      (*(volatile word *)(NAND_REG_BASE+0x1004))
#define rb_nand_rdma     (*(volatile byte *)(NAND_REG_BASE+0x1008))
#define rb_nand_wdma     (*(volatile byte *)(NAND_REG_BASE+0x100C))
#define rb_nand_cmd      (*(volatile byte *)(NAND_REG_BASE+0x1010))
#define rb_nand_adr      (*(volatile byte *)(NAND_REG_BASE+0x1014))
#define rb_nand_cen      (*(volatile byte *)(NAND_REG_BASE+0x1018))
#define rb_nand_wait     (*(volatile byte *)(NAND_REG_BASE+0x101C))

#define rb_cmd_lunce     (*(volatile byte *)(NAND_REG_BASE+0x104A))
#define rb_phy_ce        (*(volatile byte *)(NAND_REG_BASE+0x104C))
#define rb_phy_lun       (*(volatile byte *)(NAND_REG_BASE + 0x104D))
#define rb_row_adr1      (*(volatile byte *)(NAND_REG_BASE + 0x1054))
#define rb_row_adr2      (*(volatile byte *)(NAND_REG_BASE + 0x1055))
#define rb_row_adr3      (*(volatile byte *)(NAND_REG_BASE + 0x1056))
#define rb_row_adr4      (*(volatile byte *)(NAND_REG_BASE + 0x1057))
#define rd_nrd_dat       (*(volatile dwrd *)(NAND_REG_BASE+0x1058))
#define rw_sts_rdy       (*(volatile word *)(NAND_REG_BASE+0x105C))
#define rw_sts_fail      (*(volatile word *)(NAND_REG_BASE+0x105E))
#define rb_set_busy      (*(volatile byte *)(NAND_REG_BASE+0x1060))
  #define FRC_RDS          (1 << 4)
  #define AIPR_TYP         (1 << 5)

#define rb_rds_cfg       (*(volatile byte *)(NAND_REG_BASE+0x1065))
  #define DMA_HOLD         (1 << 0)
  #define RDS_HOLD         (1 << 1)
  #define RDS_FRC          (1 << 2)
  #define RDS_PRI          (1 << 3)

#define rb_set_cfg       (*(volatile byte *)(NAND_REG_BASE+0x1069))
  #define PROC_RST         (1 << 0)
  #define PTC_RST          (1 << 1)
  #define CMD_HOLD         (1 << 2)
#define rb_bus_mirr      (*(volatile byte *)(NAND_REG_BASE+0x106B))

#define rb_lowdqs_en     (*(volatile byte *)(NAND_REG_BASE+0x107A))

#define rb_cmd_sts       (*(volatile byte *)(NAND_REG_BASE+0x107C))
  #define ALL_FIN          0x08
#define rb_lunce_end     (*(volatile byte *)(NAND_REG_BASE+0x107D))
#define rb_wlpg_set      (*(volatile byte *)(NAND_REG_BASE+0x107E))
  #define WLPG_SEL         (1 << 0)
  #define WLPG_GAP         (1 << 1)
  #define WLPG_SFT         (2 << 2)

#define rw_urgq_vld      (*(volatile word *)(NAND_REG_BASE+0x10A0))
#define rw_higq_vld      (*(volatile word *)(NAND_REG_BASE+0x10A2))
#define rw_lowq_vld      (*(volatile word *)(NAND_REG_BASE+0x10A4))
  #define ALL_INVLD        0x0
#define rw_que_rptr      (*(volatile word *)(NAND_REG_BASE+0x1084))
#define rw_que_wptr      (*(volatile word *)(NAND_REG_BASE+0x1086))
  #define ALL_EMPTY        0xfff
#define rw_que_rdy       (*(volatile word *)(NAND_REG_BASE+0x1088))
  #define ALL_READY        0xffff
#define rb_qabt_fsm      (*(volatile byte *)(NAND_REG_BASE+0x108C))
  #define IDLE_ST          0x0

#define rb_nphy_rdy      (*(volatile byte *)(NAND_REG_BASE+0x10B1))
#define rb_nphy_frc      (*(volatile byte *)(NAND_REG_BASE+0x10B2))
  #define DQS_SEQ          (1 << 0)
  #define DQS_FRC          (1 << 1)
  #define DQE_FRC          (1 << 2)
  #define DQO_FRC          (1 << 3)
#define rb_dqs_data      (*(volatile byte *)(NAND_REG_BASE+0x10B3))
#define rb_dqo_data      (*(volatile byte *)(NAND_REG_BASE+0x10B4))

#define rd_nphy_phs      (*(volatile dwrd *)(NAND_REG_BASE+0x10C0))


//SRAM access
#define rd_cmdq_mem(n)   (*(volatile dwrd *)(NAND_REG_BASE+0x40000+((n)<<2)))
#define rw_inst_mem(n)   (*(volatile word *)(NAND_REG_BASE+0x80000+((n)<<1)))
#define rd_bbrm_mem(n)   (*(volatile dwrd *)(NAND_REG_BASE+0xC0000+((n)<<2)))


//NAND PHY register
#define rd_nphy_dqt(n)   (*(volatile dwrd *)(NAND_PHY_BASE+0x2000+((n)<<16)))
#define rd_nphy_dqst(n)  (*(volatile dwrd *)(NAND_PHY_BASE+0x2004+((n)<<16)))
#define rd_nphy_lpbk(n)  (*(volatile dwrd *)(NAND_PHY_BASE+0x2008+((n)<<16)))
#define rd_nphy_dllm(n)  (*(volatile dwrd *)(NAND_PHY_BASE+0x200C+((n)<<16)))
#define rd_nphy_dlls(n)  (*(volatile dwrd *)(NAND_PHY_BASE+0x2010+((n)<<16)))

#define rd_nphy_ctrl(n)  (*(volatile dwrd *)(NAND_PHY_BASE+0x2080+((n)<<16)))
#define rd_nphy_tsel(n)  (*(volatile dwrd *)(NAND_PHY_BASE+0x2084+((n)<<16)))
#define rd_nphy_gpio0(n) (*(volatile dwrd *)(NAND_PHY_BASE+0x2088+((n)<<16)))
#define rd_nphy_gpio1(n) (*(volatile dwrd *)(NAND_PHY_BASE+0x208C+((n)<<16)))
#define rd_nphy_gsts0(n) (*(volatile dwrd *)(NAND_PHY_BASE+0x2090+((n)<<16)))
#define rd_nphy_gsts1(n) (*(volatile dwrd *)(NAND_PHY_BASE+0x2094+((n)<<16)))



/*******************************************************************************************************/
/********************            Peripheral register definition               **************************/
/*******************************************************************************************************/
#define rb_pcie_rst      (*(volatile byte *)( SYS_REG_BASE+0x00))
#define rb_nand_clk      (*(volatile byte *)( SYS_REG_BASE+0x01))
  #define CLK31M           (0) //{2'b00, 1'b0}, default setting, nctl clk: 15.625MHz(nphy clk: 31.25MHz)
  #define CLK62M           (1) //{2'b00, 1'b1}, for fpga, set nctl clk: 31.25MHz(nphy clk: 62.5MHz)
  #define CLK125M          (3) //{2'b01, 1'b1}, for fpga, set nctl clk: 62.5MHz (nphy clk: 125MHz)
  #define CLK250M          (5) //{2'b10, 1'b1}, for fpga, set nctl clk: 125MHz  (nphy clk: 250MHz)
  #define CLKFAST          (1) //{2'b00, 1'b1}, for asic, set nctl clk: 600MHz  (nphy clk: 1200MHz)
#define rb_nand_phs      (*(volatile byte *)( SYS_REG_BASE+0x02))
  #define FPGA_PHS         (0x00 << 0)
  #define POS              (0x00 << 0)
  #define NEG              (0xff << 0)
#define rb_asyn_clr      (*(volatile byte *)( SYS_REG_BASE+0x03))
  #define HCLR             (1 << 0)
  #define DCLR             (1 << 1)
  #define NCLR             (1 << 2)
#define rb_cdbg_flag     (*(volatile byte *)( SYS_REG_BASE+0x04))
#define rb_hcpu_set      (*(volatile byte *)( SYS_REG_BASE+0x05))
#define rb_dlnk_ena      (*(volatile byte *)( SYS_REG_BASE+0x06))
#define rb_cmpr_ena      (*(volatile byte *)( SYS_REG_BASE+0x07))
#define rb_pclk_set      (*(volatile byte *)( SYS_REG_BASE+0x08))
#define rb_pclk_rdy      (*(volatile byte *)( SYS_REG_BASE+0x09))
#define rb_pcie_fsm      (*(volatile byte *)( SYS_REG_BASE+0x0a))
#define rb_dlnk_mst      (*(volatile byte *)( SYS_REG_BASE+0x0c))
#define rb_dual_prt      (*(volatile byte *)( SYS_REG_BASE+0x0d))
#define rb_ptch_rst      (*(volatile byte *)( SYS_REG_BASE+0x10))
#define rb_chip_rst      (*(volatile byte *)( SYS_REG_BASE+0x11))
  #define SYS_RST          (1 << 0)
  #define HACC_RST         (1 << 1)
  #define BMU_RST          (1 << 2)
  #define LDPC_RST         (1 << 3)
  #define NAND_RST         (1 << 4)
  
#define rd_patt_dbg      (*(volatile dwrd *)( SYS_REG_BASE+0x20))


//uart
/* Receive control Registers    */
#define rb_rbr (*(volatile byte    *)(UART_REG_BASE + 0x000))	/* Receive holding register   */
#define rb_iir (*(volatile byte    *)(UART_REG_BASE + 0x008))	/* Interrupt identification register  */
#define rb_mcr (*(volatile byte    *)(UART_REG_BASE + 0x010))	/* Modem Control Register  */
#define rb_scr (*(volatile byte    *)(UART_REG_BASE + 0x01c))	/* Scratch register   */
#define rb_rfl (*(volatile byte    *)(UART_REG_BASE + 0x084))	/* Receive FIFO Level   */

/* Transmit control Registers   */
#define rb_thr (*(volatile byte    *)(UART_REG_BASE + 0x000))	/* Transmit holding register  */
#define rb_ier (*(volatile byte    *)(UART_REG_BASE + 0x004))	/* Interrupt enable register  */
#define rb_fcr (*(volatile byte    *)(UART_REG_BASE + 0x008))	/* FIFO control register  */
#define rb_lcr (*(volatile byte    *)(UART_REG_BASE + 0x00c))	/* Line control register  */
#define rb_dll (*(volatile byte    *)(UART_REG_BASE + 0x000))	/* LSB of baud rate   */
#define rb_dlh (*(volatile byte    *)(UART_REG_BASE + 0x004))	/* MSB of baud rate   */
#define rb_tfl (*(volatile byte    *)(UART_REG_BASE + 0x080))	/* Transmit FIFO Level   */

/* Status Registers*/
#define rb_lsr (*(volatile byte    *)(UART_REG_BASE + 0x014))	/* Line status register   */
#define rb_usr (*(volatile byte    *)(UART_REG_BASE + 0x07c))	/* UART Status register   */
#define rb_cpr (*(volatile byte    *)(UART_REG_BASE + 0x0f4))	/* Component parameter register   */
#define rb_iir (*(volatile byte    *)(UART_REG_BASE + 0x008))	/*  Interrupt Identification Register   */

/* Interrupt enable Register */
#define IER_RDA     0x01    /* Enable received data available interrupt*/
#define IER_THE     0x02    /* Enable transmitter holding register empty interrupt*/
#define IER_RLS     0x04    /* Enable received data available interrupt*/

/* Interrupt identification Register */
#define IIR_MS      0x00
#define IIR_nIP     0x01
#define IIR_IBT0    0x02
#define IIR_IBT1    0x04
#define IIR_IBT2    0x08

/* FIFO Control Register */
#define FCR_CRF     0x02    /* Clear receive FIFO  */
#define FCR_CTF     0x04    /* Clear transmit FIFO */
//TODO
#define FCR_RTF2    0xC0    /* Set Rx trigger at 2 less than full    */
#define FCR_RTH     0x80    /* Set Rx trigger at 1/2 full */
#define FCR_RTQ     0x40    /* Set Rx trigger at 1/4 full */
#define FCR_RT1     0x00    /* Set Rx trigger at 1 */

/* Line Control Register */
#define LCR_WL5     0x00    /* Word length  */
#define LCR_WL6     0x01
#define LCR_WL7     0x02
#define LCR_WL8     0x03
#define LCR_SB1     0x00    /* Number of stop bits  */
#define LCR_SB1_5   0x04    /* 1.5 -> only valid with 5 bit words   */
#define LCR_SB2     0x04
#define LCR_PN      0x00    /* Parity mode - none   */
#define LCR_PO      0x08    /* Parity mode - odd    */
#define LCR_PE      0x18    /* Parity mode - even   */
#define LCR_PM      0x28    /* Forced "mark" parity */
#define LCR_PS      0x38    /* Forced "space" parity    */
#define LCR_DL      0x80    /* Enable baud rate latch   */

/* Line Status Register */
#define LSR_DR      0x01    /* Data Ready indicator */
#define LSR_OE      0x02    /* Overrun Error indicator */
#define LSR_PE      0x04    /* Parity Error indicator */
#define LSR_FE      0x08    /* Framing error indicator */
#define LSR_BI      0x10    /* Break condition indicator */
#define LSR_THE     0x20    /* Transmitter FIFO empty */
#define LSR_TEMT    0x40    /* Transmitter empty indicator */
#define LSR_FIE     0x80    /* Error condition */

/*  UART Status register */
#define USR_BUSY    0x01    /* UART Busy */
#define USR_TFNF    0x02    /* Transmit FIFO Not Full */
#define USR_TFE     0x04    /* Transmit FIFO Empty */
#define USR_RFNE    0x08    /* Receive FIFO Not Empty */
#define USR_RFF     0x10    /* Receive FIFO Full */


//intc
#define REG_FIQ          (*(volatile dwrd *)(INTC_REG_BASE+0x00))
#define REG_IRQ          (*(volatile dwrd *)(INTC_REG_BASE+0x04))
#define REG_FIQE         (*(volatile dwrd *)(INTC_REG_BASE+0x08))
#define REG_IRQE         (*(volatile dwrd *)(INTC_REG_BASE+0x0C))
#define REG_IMR          (*(volatile dwrd *)(INTC_REG_BASE+0x10))
#define REG_MECR         (*(volatile dwrd *)(INTC_REG_BASE+0x14))
#define REG_EBA          (*(volatile dwrd *)(INTC_REG_BASE+0x18))
#define REG_TEST         (*(volatile dwrd *)(INTC_REG_BASE+0x1C))
#define REG_SCR(n)       (*(volatile dwrd *)(INTC_REG_BASE+0x40+((n)<<2)))


#endif
