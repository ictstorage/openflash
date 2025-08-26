#define IDTF_NS_LEN  40
#define IDTF_CTL_LEN 136
#define IDTF_PS_LEN  8
#define LOGP_SH_LEN  64

const dwrd gd_idtf_ns[IDTF_NS_LEN] =
{
    L2PE_QNTY,                                             //offset 0, NSZE
    0x0,                                                   //offset 4, high 32bits
    L2PE_QNTY,                                             //offset 0, NCAP
    0x0,                                                   //offset 12, high 32bits
    L2PE_QNTY,                                             //offset 0, NUSE
    0x0,                                                   //offset 20, high 32bits
    0x0, 0x0,                                              //offset 24~31,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                //offset 32~63,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                //offset 64~95,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                //offset 96~127,
#ifdef N4KA_EN
    ((0x2 << 24) + (0x9 << 16) + (0x0 << 0)),              //offset 128, LBA format, {RP, LBADS, MS[2]}
#else
    ((0x2 << 24) + (0xC << 16) + (0x0 << 0)),
#endif
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0                      //offset 132~159, other LBA format
};

const dwrd gd_idtf_ctl[IDTF_CTL_LEN] =
{
    0x000016c3,                                            //offset 0, PCI sub-system vendor ID, PCI vendor ID

    (('0' << 24) + ('D' << 16) + ('S' << 8) + ('S' << 0)), //offset 4, serial number: SSD00000
    (('0' << 24) + ('0' << 16) + ('0' << 8) + ('0' << 0)),
    ((' ' << 24) + (' ' << 16) + (' ' << 8) + (' ' << 0)),
    ((' ' << 24) + (' ' << 16) + (' ' << 8) + (' ' << 0)),
    ((' ' << 24) + (' ' << 16) + (' ' << 8) + (' ' << 0)),

    (('e' << 24) + ('I' << 16) + ('C' << 8) + ('P' << 0)), //offset 24, model number: PCIe FPGA SSD
    (('G' << 24) + ('P' << 16) + ('F' << 8) + (' ' << 0)),
    (('S' << 24) + ('S' << 16) + (' ' << 8) + ('A' << 0)),
    ((' ' << 24) + (' ' << 16) + (' ' << 8) + ('D' << 0)),
    ((' ' << 24) + (' ' << 16) + (' ' << 8) + (' ' << 0)),
    ((' ' << 24) + (' ' << 16) + (' ' << 8) + (' ' << 0)),
    ((' ' << 24) + (' ' << 16) + (' ' << 8) + (' ' << 0)),
    ((' ' << 24) + (' ' << 16) + (' ' << 8) + (' ' << 0)),
    ((' ' << 24) + (' ' << 16) + (' ' << 8) + (' ' << 0)),
    ((' ' << 24) + (' ' << 16) + (' ' << 8) + (' ' << 0)),

    (('2' << 24) + ('2' << 16) + ('0' << 8) + ('2' << 0)), //offset 64, firmware revision: 20220831
    (('1' << 24) + ('3' << 16) + ('8' << 8) + ('0' << 0)),

    ((0x5cd2e4 << 8) + (0x0 << 0)),                        //offset 72, {IEEE[3], RAB}
    ((0x9 << 16) + (0x6 << 8) + (0x0 << 0)),               //offset 76, {CNTLID[2], MDTS, CMIC}

    0x0,                                                   //offset 80, version, v1.1 report 0
    0x0,                                                   //offset 84, RTD3R, resume time
    0x0,                                                   //offset 88, RTD3E, entry time
    0x0,                                                   //offset 92, OAES
    0x0, 0x0, 0x0,                                         //offset 96~107,
    ((0x1 << 24) + (0x0 << 0)),                            //offset 108,
    0x0, 0x0, 0x0, 0x0,                                    //offset 112~127,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                //offset 128~159,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                //offset 160~191,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                //offset 192~223,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                //offset 224~255,

    ((0x4 << 24) + (0x4 << 16) + (0x0 << 0)),              //offset 256, {AERL, ACL, OACS[2]}
    ((0x0 << 8) + (0x1 << 1) + (0x1 << 0)),                //offset 260, {NPSS, ELPE, LPA, FRMW}
    0x01570000,                                            //offset 264,
    0x0, 0x0, 0x0, 0x0, 0x0,                               //offset 268~287,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                //offset 288~319,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                //offset 320~351,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                //offset 352~383,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                //offset 384~415,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                //offset 416~447,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                //offset 448~479,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                //offset 480~511,

    ((0x0 << 16) + (0x44 << 8) + (0x66 << 0)),             //offset 512, {MAXCMD, CQES, SQES}
    0x1,                                                   //offset 516, NN(number of namespace)
    0x0,                                                   //offset 520, {FUSES, ONCS}
#ifdef N4KA_EN
    ((256 << 16) + (0x1 << 8) + (0x0 << 0)),               //offset 524, {AWUN, VWC, FNA}
#else
    ((32 << 16) + (0x1 << 8) + (0x0 << 0)),
#endif
    0x0, 0x0, 0x0, 0x0                                     //offset 528~543,
};

const dwrd gd_idtf_ps[IDTF_PS_LEN] =
{
    0x9C4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0               //offset 2048~2079, power state0
};

const dwrd gd_logp_sh[LOGP_SH_LEN] =
{
    ((100 << 24) + (300 << 8) + (0x0 << 0)),               //offset 0, {available space, temperature[2], warning}
    ((0x0 << 16) + (0 << 8) + (10 << 0)),                  //offset 4, {endurance warning, % used, available space thr}
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                          //offset 8~31, reserved
    0x0, 0x0, 0x0, 0x0,                                    //offset 32~47, data units read
    0x0, 0x0, 0x0, 0x0,                                    //offset 48~63, data units written
    0x0, 0x0, 0x0, 0x0,                                    //offset 64~79, host read cmd
    0x0, 0x0, 0x0, 0x0,                                    //offset 80~95, host write cmd
    0x0, 0x0, 0x0, 0x0,                                    //offset 96~111, ctrl busy time
    0x0, 0x0, 0x0, 0x0,                                    //offset 112~127, power cycle
    0x0, 0x0, 0x0, 0x0,                                    //offset 128~143, power on hours
    0x0, 0x0, 0x0, 0x0,                                    //offset 144~159, unsafe shutdowns
    0x0, 0x0, 0x0, 0x0,                                    //offset 160~175, data integrity error
    0x0, 0x0, 0x0, 0x0,                                    //offset 176~191, error log number
    0x0,                                                   //offset 192, warning temperature time
    0x0,                                                   //offset 196, critical temperature time
    ((0 << 16) + (100 << 0)),                              //offset 200, temperature sensor 1, 2
    0x0, 0x0, 0x0, 0x0, 0x0,                               //offset 204~223,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0                 //offset 224~255,
};
