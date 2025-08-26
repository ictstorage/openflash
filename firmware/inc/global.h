#ifndef _GLOBAL_H_
#define _GLOBAL_H_


#include <string.h>


//#define NVME_TEST
//#define CDDR_TEST
//#define BEND_TEST
//#define DRAM_TEST
//#define SRAM_TEST
//#define GDMA_TEST
//#define TRQF_TEST //Tbl Range Queue Fifo

//function/feature enable
//#define GC_SIM
//#define ASIC_FAST_SIM //set 1ce to speedup simulation
//#define FPGA_FAST_SIM //set 1ce to speedup simulation
//#define FTL_FAST_SIM  //reduce gc table initial entry
//#define GC_FAST_TEST  //reduce blk number to speedup gc test, only for FPGA_MODE
//#define NAND_FAST_SIM //reduce tERASE, tPROG time, only for asic post sim
#define FPGA_MODE
//#define M2_ONFI_PHY

//#define NPHS_CALI
// #define FPGA_U2
//#define FPGA_SORTING
//#define DLNK_EN    //enable die link function
//#define CPUA_PROC  //use cup agent processor
//#define RDEC_TEST  //enable raid decoder test
//#define HWCMD_CHK  //check the bmu handle cmd
//#define RCAH_FUNC  //enable nand cache read
//#define WCAH_FUNC  //enable nand cache write
// #define AIPR_FUNC  //enable aipr function
//#define SUSP_FUNC  //enable suspend function
//#define LDPC_MS    //use LDPC min-sum engine to decode
//#define ERR_INJECT //error injection
// #define HTWR_DBUF  //enable host write data to dram buffer
//#define WAIT_AXIW  //wait AXI bvalid for cpu write dram

//#define FTL_DBG    //DRAM space only 1GB for P2L table, so if(capacity > 128GB), can't define FTL_DBG
//#define UART_DBG
#define NQUE_CHK 0 //check whether nand queue full

#define LGET_EN //enable HW auto get l2p function
#define LUPD_EN //enable HW update "gc write info" into "BMU buffer table", need enable LGET_EN
//#define N4KA_EN //enable non 4KB align, set LBA unit as 512B
#define HTMT_EN //enable host meta
//#define HTMT_CHK //check host meta error
#define ONFI_MODE
#ifdef M2_ONFI_PHY
  #define SETF_LDQS //use low dqs mode for set feature
  #define RSTS_LDQS //use low dqs mode for read status
#endif

#ifdef HTMT_EN
 #ifdef N4KA_EN
  #define HMT_MODE 3 //host meta: 0:8bytes, 1:16bytes, 3:64bytes, 5:0byte
  #define FMT_MODE 1 //fw meta: 0:4bytes, 1:8bytes,....7:32bytes
 #else
  #define HMT_MODE 0
  #define FMT_MODE 5
 #endif
#else
  #define HMT_MODE 5
  #define FMT_MODE 7
#endif


//set NAND vender
#ifdef FPGA_MODE
  //#define NAND_BICS5 //use BiCS4/5 NAND
  #define NAND_X39070 //use YMTC X3 9070 NAND
#else
  //#define NAND_BICS5
  #define NAND_X39070 //use YMTC X3 9070 NAND
  //#define B47R_SLC
  //#define NAND_B47R
#endif

//set NAND interface timing register
#ifdef FPGA_MODE
  #define NAND_500M
#else
#ifdef NAND_BICS5
  #define NAND_1600M
#else
  #define NAND_2400M
#endif
#endif 

//for fpga, can't use register to change single_end/differential mode, so skip status check
#ifdef FPGA_MODE
  #define SKIP_STS_CHK //after powe on reset, only add latency
  #define MANUAL_SETF  //manual set feature, only for fpga high speed nphy mode
  #define NAND_DIFF    //differential mode
  #define ZQ_CALI      //ZQ calibration
#else
  //#define SKIP_STS_CHK
  //#define MANUAL_SETF  //manual set feature, only for fpga high speed nphy mode
  #define NAND_DIFF
  #define ZQ_CALI      //ZQ calibration
#endif

//set nand clock rate
#ifdef FPGA_MODE
  //#define DMY_READ  //dummy read, only for fpga high speed nphy mode
  #define NAND_HSPD //only Micron/YMTC NAND need be set timing mode by set_feature // compress board
 // #define NAND_CLK CLK31M // compress board
  //#define NAND_CLK CLK62M
  //#define NAND_CLK CLK125M
  #define NAND_CLK CLK250M  // compress board
#else
  //#define DMY_READ  //dummy read, only for fpga high speed nphy mode
  #define NAND_HSPD
  #define NAND_CLK CLKFAST //switch to high speed nand clk
#endif

//micro processor timing delay
#ifdef FPGA_MODE
  #define PROC_TDLY
#endif

#ifdef FPGA_SORTING
  #define MIRR_LCE 0x00
#else
  #define MIRR_LCE 0xAA
#endif
  #define CMPR_SHIFT  0

//parameter setting
#if defined(FPGA_MODE)

  #if defined(NAND_BICS5)

    #define FRAG_SHIFT  3
    #define CHAN_SHIFT  3
    #define PCE_SHIFT   0
    #define BANK_SHIFT  0
    #define PAGE_SHIFT  11
    #define BLK_SHIFT   11
    #define PLANE_SHIFT 1
    #define FRAG_QNTY   8
    #define CHAN_QNTY   8
    #define PCE_QNTY    1
    #define BANK_QNTY   1
    #define PAGE_QNTY   1152 //BiCS5:1344
  #ifdef GC_FAST_TEST
    #define BLK_QNTY    100
  #else
    #define BLK_QNTY    1958 //BiCS5:1662
  #endif
    #define PLANE_QNTY  2
  #ifdef GC_FAST_TEST
    #define DIE_MAPU    781250
  #else
    #define DIE_MAPU    15625000           //64GB = 64000000000Bytes = 15625000x4096Bytes
  #endif
#if (defined(HTMT_EN) && defined(N4KA_EN))
    #define LDPC_MODE   (0 << 5) | 8       //spare bytes: 1952Byte/page = 488Byte/mapu, 488B-72B-4B = 412B
#else
    #define LDPC_MODE   (1 << 5) | 6       //spare bytes: 1952Byte/page = 488Byte/mapu, 488B-32B-4B = 452B
#endif

  #elif defined(NAND_X39070)

    #define FRAG_SHIFT  5
    #define CHAN_SHIFT  3
  #ifdef FPGA_FAST_SIM
    #define PCE_SHIFT   1
    #define BANK_SHIFT  1
  #else
#ifdef FPGA_SORTING
    #define PCE_SHIFT   1
    #define BANK_SHIFT  1
#else
    #define PCE_SHIFT   2
    #define BANK_SHIFT  2
#endif
  #endif
    #define PAGE_SHIFT  13
    #define BLK_SHIFT   9
    #define PLANE_SHIFT 3
    #define FRAG_QNTY   24
    #define CHAN_QNTY   8
  #ifdef FPGA_FAST_SIM
    #define PCE_QNTY    2
    #define BANK_QNTY   2
  #else
#ifdef FPGA_SORTING
    #define PCE_QNTY    2
    #define BANK_QNTY   2
#else
    #define PCE_QNTY    4
    #define BANK_QNTY   4
#endif
  #endif
    #define PAGE_QNTY   4176
  #ifdef GC_FAST_TEST
    #define BLK_QNTY    50
  #else
    #define BLK_QNTY    363
  #endif
    #define PLANE_QNTY  6
  #ifdef GC_FAST_TEST
    #define DIE_MAPU    4000000
  #else
    #define DIE_MAPU    12207031           //128GB = 128000000000Bytes = 31250000x4096Bytes
  #endif
#if (defined(HTMT_EN) && defined(N4KA_EN))
    #define LDPC_MODE   (0 << 5) | 12      //spare bytes: 1984Byte/page = 496Byte/mapu, 496B-72B-4B = 420B
#else
    #define LDPC_MODE   (1 << 5) | 10      //spare bytes: 1984Byte/page = 496Byte/mapu, 496B-32B-4B = 460B
#endif

  #else //NAND_B47R

    #define FRAG_SHIFT  4
    #define CHAN_SHIFT  3
    #define PCE_SHIFT   0
    #define BANK_SHIFT  0
    #define PAGE_SHIFT  12
    #define BLK_SHIFT   10
    #define PLANE_SHIFT 2
    #define FRAG_QNTY   16
    #define CHAN_QNTY   8
    #define PCE_QNTY    1
    #define BANK_QNTY   1
  #ifdef B47R_SLC
    #define PAGE_QNTY   704
  #else
    #define PAGE_QNTY   2112
  #endif
    #define BLK_QNTY    556
    #define PLANE_QNTY  4
  #ifdef B47R_SLC
    #define DIE_MAPU    5000000            //21GB = 21000000000Bytes = 5000000x4096Bytes
  #else
    #define DIE_MAPU    15625000           //64GB = 64000000000Bytes = 15625000x4096Bytes
  #endif
#if (defined(HTMT_EN) && defined(N4KA_EN))
    #define LDPC_MODE   (0 << 5) | 10      //spare bytes: 1968Byte/page = 492Byte/mapu, 492B-72B-4B = 416B
#else
    #define LDPC_MODE   (1 << 5) | 8       //spare bytes: 1968Byte/page = 492Byte/mapu, 492B-32B-4B = 456B
#endif

  #endif

#else

  #if defined(NAND_BICS5)

    #define FRAG_SHIFT  3
    #define CHAN_SHIFT  3
  #ifdef ASIC_FAST_SIM
    #define PCE_SHIFT   0
    #define BANK_SHIFT  0
  #else
    #define PCE_SHIFT   3
    #define BANK_SHIFT  3
  #endif
    #define PAGE_SHIFT  11
    #define BLK_SHIFT   11
    #define PLANE_SHIFT 1
    #define FRAG_QNTY   8
    #define CHAN_QNTY   8
  #ifdef ASIC_FAST_SIM
    #define PCE_QNTY    1
    #define BANK_QNTY   1
  #else
    #define PCE_QNTY    8
    #define BANK_QNTY   8
  #endif
    #define PAGE_QNTY   1152 //BiCS5:1344
    #define BLK_QNTY    1958 //BiCS5:1662
    #define PLANE_QNTY  2
    #define DIE_MAPU    15625000           //64GB = 64000000000Bytes = 15625000x4096Bytes
#if (defined(HTMT_EN) && defined(N4KA_EN))
    #define LDPC_MODE   (0 << 5) | 8       //spare bytes: 1952Byte/page = 488Byte/mapu, 488B-72B-4B = 412B
#else
    #define LDPC_MODE   (1 << 5) | 6       //spare bytes: 1952Byte/page = 488Byte/mapu, 488B-32B-4B = 452B
#endif

  #elif defined(NAND_X39070)

    #define FRAG_SHIFT  5
    #define CHAN_SHIFT  3
  #ifdef ASIC_FAST_SIM
    #define PCE_SHIFT   0
    #define BANK_SHIFT  0
  #else
    #define PCE_SHIFT   3
    #define BANK_SHIFT  3
  #endif
    #define PAGE_SHIFT  13
    #define BLK_SHIFT   9
    #define PLANE_SHIFT 3
    #define FRAG_QNTY   24
    #define CHAN_QNTY   8
  #ifdef ASIC_FAST_SIM
    #define PCE_QNTY    1
    #define BANK_QNTY   1
  #else
    #define PCE_QNTY    8
    #define BANK_QNTY   8
  #endif
    #define PAGE_QNTY   4176
    #define BLK_QNTY    363
    #define PLANE_QNTY  6
    #define DIE_MAPU    31250000           //128GB = 128000000000Bytes = 31250000x4096Bytes
#if (defined(HTMT_EN) && defined(N4KA_EN))
    #define LDPC_MODE   (0 << 5) | 12      //spare bytes: 1984Byte/page = 496Byte/mapu, 496B-72B-4B = 420B
#else
    #define LDPC_MODE   (1 << 5) | 10      //spare bytes: 1984Byte/page = 496Byte/mapu, 496B-32B-4B = 460B
#endif

  #else //NAND_B47R

    #define FRAG_SHIFT  4
    #define CHAN_SHIFT  3
    #define PCE_SHIFT   3
    #define BANK_SHIFT  3
    #define PAGE_SHIFT  12
    #define BLK_SHIFT   10
    #define PLANE_SHIFT 2
    #define FRAG_QNTY   16
    #define CHAN_QNTY   8
    #define PCE_QNTY    8
    #define BANK_QNTY   8
    #define PAGE_QNTY   2112
    #define BLK_QNTY    556
    #define PLANE_QNTY  4
    #define DIE_MAPU    15625000           //64GB = 64000000000Bytes = 15625000x4096Bytes
#if (defined(HTMT_EN) && defined(N4KA_EN))
    #define LDPC_MODE   (0 << 5) | 10      //spare bytes: 1968Byte/page = 492Byte/mapu, 492B-72B-4B = 416B
#else
    #define LDPC_MODE   (1 << 5) | 8       //spare bytes: 1968Byte/page = 492Byte/mapu, 492B-32B-4B = 456B
#endif

  #endif

#endif


#define WLPG_QNTY   3 //word line page quantity, TLC NAND:3, QLC NAND:4
#define INST_SHARE  3
#if defined(NAND_BICS5)
#define MAPU_SIZE   4584
#define PG_SIZE     18336
#elif defined(NAND_X39070)
#define MAPU_SIZE   4592
#define PG_SIZE     18368
#elif defined(NAND_B47R)
#define MAPU_SIZE   4588
#define PG_SIZE     18352
#else
#define MAPU_SIZE   4640
#define PG_SIZE     18560
#endif

#define PSLC_BNDY   40
#define HEAD_QNTY    2
#define GC_HLOC      1 //gc head location
#define RWCMD_THR   16
#define HMETA_QSZ   ((HMT_MODE == 0) ? 1 : ((HMT_MODE == 1) ? 2 : ((HMT_MODE == 3) ? 8 : 0))) //qword size, 8byte unit
#define HMETA_DSZ   (HMETA_QSZ*2) //dword size, 4byte unit

#define HCMD_MASK   0x3f

#define GC_SIM_THR  6   //p2l page number
#define GC_BLK_THR  3   //free blk number
#define GC_RD_THR   1500


#define CTAG_SFT    11
#ifdef FPGA_MODE
#define COFS_SFT    7
#else
#define COFS_SFT    9
#endif
#define CTAG_NUM    (1 << CTAG_SFT)
#define CTAG_MSK    (CTAG_NUM - 1)
#define COFS_NUM    (1 << COFS_SFT)
#define COFS_MSK    (COFS_NUM - 1)


//base address
//#define SRAM_META_BASE  (SRAM_MEM_BASE + 0x00200000)
#define SRAM_META_BASE  (SRAM_MEM_BASE + 0x00400000)

#define FTL_L2P_BASE    (DRAM_MEM_BASE + 0x00000000) //512MB: 0x00000000 ~ 0x1FFFFFFF
#define FTL_P2L_BASE    (DRAM_MEM_BASE + 0x20000000) //4GB:   0x20000000 ~ 0x5FFFFFFF (only 1GB, not enough)
#define FTL_VMAP_BASE   (DRAM_MEM_BASE + 0x60000000) //128MB: 0x60000000 ~ 0x67FFFFFF
#define FTL_CRL_BASE    (DRAM_MEM_BASE + 0x68000000) //128MB: 0x68000000 ~ 0x6FFFFFFF
//TBD??
#define DRAM_DATA_BASE  (DRAM_MEM_BASE + 0x7F000000) //10MB:  0x7F000000 ~ 0x7F9FFFFF
#define DRAM_META_BASE  (DRAM_MEM_BASE + 0x7FE00000) //640KB: 0x7FE00000 ~ 0x7FE9FFFF

//set ddma & trq, don't need DRAM_MEM_BASE
//#define FTL_HTP2L_BASE  (DRAM_MEM_BASE + 0x7FFF0000) //16KB:  0x7FFF0000 ~ 0x7FFF3FFF
//#define FTL_GCP2L_BASE  (DRAM_MEM_BASE + 0x7FFF4000) //16KB:  0x7FFF4000 ~ 0x7FFF7FFF
#define FTL_HTP2L_BASE  (                0x7FFF0000) //16KB:  0x7FFF0000 ~ 0x7FFF3FFF
#define FTL_GCP2L_BASE  (                0x7FFF4000) //16KB:  0x7FFF4000 ~ 0x7FFF7FFF


//fix value
#define CMPR_NUM    (1 << CMPR_SHIFT)
#define CMPR_MASK   (CMPR_NUM - 1)

#define FRAG_NUM    (1 << FRAG_SHIFT)
#define FRAG_MASK   (FRAG_NUM - 1)
#define CHAN_NUM    (1 << CHAN_SHIFT)
#define CHAN_MASK   (CHAN_NUM - 1)

#define BKCH_SHIFT  (BANK_SHIFT + CHAN_SHIFT)
#define BKCH_QNTY   (BANK_QNTY * CHAN_QNTY)
#define BKCH_NUM    (1 << BKCH_SHIFT)
#define BKCH_MASK   (BKCH_NUM - 1)

#define PAGE_NUM    (1 << PAGE_SHIFT)
#define PAGE_MASK   (PAGE_NUM - 1)

#define CHAN_MAP    ((1 << CHAN_QNTY) - 1)

#define BYTE_SHIFT  8
#define WORD_SHIFT  16
#define DWRD_SHIFT  32

#define BYTE_MASK   0xff
#define WORD_MASK   0xffff
#define DWRD_MASK   0xffffffff

#define PSLC_BLK    0
#define TQLC_BLK    1

#define FREE_MASK   0x7
#define FREE_BLK    0x0
#define VCNT_BLK    (1 << 0)
#define HEAD_BLK    (1 << 1)
#define GC_BLK      (1 << 2)
#define GCFN_BLK    (1 << 3)
#define BAD_BLK     (1 << 4)

#define DBLK_INVLD  0xffff
#define L2P_NULL    0x0
#define P2L_NULL    0xffffffff

//#define SMDT_BASE   0x5  //sram data buffer base address
//#define BPTR_SFT    9
#define SMDT_BASE   0x2  //sram data buffer base address
#define SMPT_BASE   0x3  //sram parity buffer basee address
#define BPTR_SFT    10
#define BPTR_MSK    ((1 << BPTR_SFT) - 1)

#define RPTR_INVLD  0x1ff
#define BPTR_INVLD  0xfff
#define RIDX_INVLD  0x7f
#define LIDX_INVLD  0xff
#define LIDX_AERR   0xfe //add error bit
#define LIDX_TYPE   0x0
#define FLAG_INVLD  0xff
#define DBUF_RLS    0x1
#define DBUF_KEEP   0x0
#define MCID_INVLD  0xfffff

#define QLNK_HTQ    0x0 //send to host read queue
#define QLNK_GCQ    0x1 //send to cpua gc queue (hid)
#define QLNK_ERQ    0x2 //send to cpua ecc fail queue
#define QLNK_DMQ    0x3 //send to dram queue
#define QLNK_SMQ    0x4 //send to cpua sram/dram queue
#define QLNK_RAQ    0x5 //send to raid decoder queue
#define QLNK_TRQ    0x6 //send to cpua table range queue (dmp)
#define QLNK_N4K    0x7 //for non 4KB align


#define RAID_PNUM   0
#define RAID_GCRID  (1 << (FRAG_SHIFT + 3))
#define RAID_LST2   (BKCH_QNTY - 2) //last 2
#define RAID_LST1   (BKCH_QNTY - 1) //last 1
#define RDEC_GAP    30
#define RERR_THR    16
#define RDEC_SFT    4
#define RDEC_NUM    (1 << RDEC_SFT)
#define RDIE_QNTY   (BKCH_QNTY - RAID_PNUM)

#define PRST_DLY    4000000
#define L2PE_QNTY   (DIE_MAPU*RDIE_QNTY) //L2P entry
#define RBLK_QNTY   BLK_QNTY
#define VBLK_QNTY   RBLK_QNTY

#define PMSG_END    ((FRAG_QNTY*RDIE_QNTY*PAGE_QNTY-1)/((4096/CMPR_NUM)+4))
#define PMSG_NUM    (PMSG_END + 1)
#define PMSG_SFT    (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT + CMPR_SHIFT - 12)
#define PMSG_MSK    ((1 << PMSG_SFT) - 1)
#define CHKZ_END    ((PAGE_QNTY - 1) >> (15 - BKCH_SHIFT - FRAG_SHIFT))
#define CHKZ_SFT    (((PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT) > 15) ? (PAGE_SHIFT + BKCH_SHIFT + FRAG_SHIFT - 15) : 0)
#define CHKZ_MSK    ((CHKZ_SFT < 3) ? ((1 << CHKZ_SFT) - 1) : 0x7)
#define P2LP_NUM    (((PMSG_NUM*4)/(FRAG_QNTY*RDIE_QNTY)) + 1)

//#define ARM_NOP()   __asm{nop nop nop}
#define ARM_NOP()   __asm{nop} __asm{nop} __asm{nop}


typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned char      byte;
typedef unsigned short     word;
typedef unsigned int       dwrd;
typedef unsigned long long qwrd;

typedef union _DBL_TBL
{
  struct
  {
    dwrd d_dbl_ptr;
  } dw;

  struct
  {
    word w_next_ptr;
    word w_last_ptr;
  } wd;
} DBL_TBL, *PDBL_TBL;

typedef struct _HST_CMD
{
  byte sb_cmd_type;
  byte sb_cmd_smap;
  byte sb_cmd_mued; //mapu end
  byte sb_cmd_strm; //stream id
  word sw_cmd_bfp;  //buffer ptr
  byte sb_cmd_crd;  //compression data
  byte sb_cmd_crl;  //compression location
  dwrd sd_cmd_cid;  //host cmd id
  byte sb_cmd_ftl;  //need handle by ftl
  byte sb_cmd_hit;  //gc write l4k hit
  word sw_cmd_gcp;  //gc write bfp
  dwrd sd_cmd_ladr;
  dwrd sd_cmd_lext;
  dwrd sd_cmd_padr;
  dwrd sd_cmd_pext;
} HST_CMD, *PHST_CMD;

typedef struct _PHY_ADR
{
  byte sb_head_wcnt;
  byte sb_head_wend;
  byte sb_head_frag;
  byte sb_head_cech;
  word sw_head_page;
  word sw_head_blk;
} PHY_ADR, *PPHY_ADR;



void sysCPUSysModeEnaIntr(void);
void sysCPUSysModeDisIntr(void);

extern void fv_uart_debug(const char *fmt, ...);
extern void fv_uart_print(const char *fmt, ...);
extern void fv_dbg_loop(byte b_flag);

#define fv_nque_chk(n) while(NQUE_CHK && (rb_cmd_busy != 0x0)) { fv_uart_print("nq:%d\r\n", n); }
//#define phy_chan_map(n) ((rd_phy_chan >> ((n)*3)) & 0x7);
#define phy_chan_map(n) (n)

#endif
