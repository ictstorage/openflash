`define MICRO_CODE_NUM   8
`define NAND_ERASE_PTR   6'd1
`define NAND_WRITE_PTR   6'd2
`define NAND_READ_PTR    6'd3

//instruction
`define INST_WRITE 16'h0000
`define INST_MOVE  16'h8000
`define INST_JUMP  16'hC000
  `define JUMP_EQFW  5'h10
  `define JUMP_EQBK  5'h11
  `define JUMP_NEBK  5'h15
  `define JUMP_NCFW  5'h18
  `define JUMP_NCBK  5'h19
`define INST_SHIFT 16'hE000
  `define SHIFT_RL4  5'h14 //REG_P0 << 4
`define INST_OPRT  16'hF000
  `define OPRT_ADD   5'h01
  `define OPRT_AND   5'h04
`define INST_NOP   16'hFFFF
`define INST_FIN   16'hFFFE

//register address
`define NAND_READ  7'h00
`define NAND_WRITE 7'h04
`define NAND_RDMA  7'h08
`define NAND_WDMA  7'h0C
`define NAND_CMD   7'h10
`define NAND_ADR   7'h14
`define NAND_CEN   7'h18
`define NAND_WAIT  7'h1C

`define PROC_REG0  7'h20
`define PROC_REG1  7'h21

`define ECTI_STS   7'h48
  `define RCAH_SET   8'h01
  `define AIPR_SET   8'h04
`define CEN_NUM    7'h4C
`define SINGLE_PL  7'h4E
`define COL_ADR1   7'h52
`define COL_ADR2   7'h53
`define ROW_ADR1   7'h54
`define ROW_ADR2   7'h55
`define ROW_ADR3   7'h56
`define ROW_ADR4   7'h57
`define NAND_RDT   7'h58
`define SET_BUSY   7'h60
`ifdef RCAH_EN
  `define RCMD_RSTS  8'h40
`else
  `define RCMD_RSTS  8'h00
`endif
  `define AIPR_RSTS  8'h20 //bit5: aipr type
  `define ERAS_RSTS  8'h11 //bit4: force read status
  `define PROG_RSTS  8'h12
  `define PMED_RSTS  8'h13
`define STS_DATA   7'h61
`define SET_PLANE  7'h62
  `define AUTO_PL    8'h10
`define DMA_STS    7'h64
`define OSPG_NUM   7'h6C
`define LOWDQS_EN  7'h7A
`define AIPR_STS   7'h7F

//NAND command
`define READ_PAGE  8'h00
`define READ_MTPL  8'h32
`define READ_END   8'h30
`define RCAH_CMD   8'h31
`define RCAH_END   8'h3F
`ifdef NAND_BICS5
`define RCOL_CMD   8'h05
`else
`define RCOL_CMD   8'h06
`endif
`define RCOL_END   8'hE0

`define PROG_CMD   8'h80
`define PROG_MTPL  8'h11
`define PROG_END   8'h10
`define PROG_MED   8'h1A //for Kioxia/YMTC NAND only

`define ERAS_CMD   8'h60
`define ERAS_MTPL  8'hD1 //for Micron NAND only
`define ERAS_END   8'hD0

`define READ_STS   8'h70
`define READ_ASTS  8'h77
`define READ_ESTS  8'h78
  `define STS_ARDY   8'h60 //bit5: array ready, bit[6]: register ready
`ifdef RCAH_EN
  `define STS_RDY    8'h40
`else
  `define STS_RDY    8'h60
`endif

`define AIPR_ENTR   8'h0B
`define AIPR_EXIT   8'h0C

//wait timer
`ifdef NAND_2400M //use X3-9070 timing
  `define WAIT_TWB   8'd59  //wait tWB(100ns)
  `define WAIT_TADL  8'd179 //wait tADL(300ns)
  `define WAIT_TCCS  8'd179 //wait tCCS(300ns)
`elsif NAND_1600M //use B47R timing
  `define WAIT_TWB   8'd39  //wait tWB(B58R:80ns, B47R:100ns)
  `define WAIT_TADL  8'd59  //wait tADL(150ns)
  `define WAIT_TCCS  8'd159 //wait tCCS(B58R:275ns, B47R:400ns)
`else //FPGA mode, NAND_500M, use B47R timing
  `define WAIT_TWB   8'd12  //wait tWB(B58R:80ns, B47R:100ns)
  `define WAIT_TADL  8'd18  //wait tADL(150ns)
  `define WAIT_TCCS  8'd49  //wait tCCS(B58R:275ns, B47R:400ns)
`endif

//other
`define CEN_OFF    8'h80
//`define WAIT_TRPH  8'd8
`define PLANE_END  (8'd`NAND_PLANE_NUM - 8'd1)
`define OSPG_END   8'd3


//const word gw_micro_code[][64] = {
wire [0:64*16-1] micro_code [`MICRO_CODE_NUM];

//read status command, cmd_ptr = 6'd0
assign micro_code[0] =
{
(`INST_MOVE  | (`CEN_NUM   << 7) | `NAND_CEN ), //enable CEn
(`INST_WRITE | (`READ_STS  << 7) | `NAND_CMD ), //send read status cmd(8'h70) to NAND
`ifdef RSTS_LDQS
(`INST_WRITE | (8'h1       << 7) | `LOWDQS_EN), //enable low dqs mode
`endif
(`INST_WRITE | (8'h0       << 7) | `NAND_READ), //read status
`ifdef PROC_TDLY
(`INST_MOVE  | (`NAND_RDT  << 7) | `PROC_REG0), //dummy read for FPGA timing issue
`endif
(`INST_MOVE  | (`NAND_RDT  << 7) | `STS_DATA ), //move NAND read data to status check reg
`ifdef RSTS_LDQS
(`INST_WRITE | (8'h0       << 7) | `LOWDQS_EN), //disable low dqs mode
`endif
`ifndef NAND_B47R
(`INST_WRITE | (`CEN_OFF   << 7) | `NAND_CEN ), //disable CEn
//(`INST_WRITE | (`WAIT_TRPH << 7) | `NAND_WAIT), //need wait tRPSTH(15/25ns)
`endif
(`INST_FIN                                   )  //finish
};

//multi-plane erase command, cmd_ptr = 6'd1
assign micro_code[1] =
{
(`INST_MOVE  | (`CEN_NUM   << 7) | `NAND_CEN ), //enable CEn
(`INST_WRITE | (8'h0       << 7) | `SET_PLANE), //init plane number
(`INST_MOVE  | (`AIPR_STS  << 7) | `PROC_REG0), //copy "aipr status" to processor reg0
(`INST_WRITE | (8'h0       << 7) | `PROC_REG1), //set "8'h0" to processor reg1
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd7      ), //compare aipr status, if aipr no set, jump to "erase cmd"
(`INST_WRITE | (`READ_ESTS << 7) | `NAND_CMD ), //send read enhance status cmd(8'h78) to NAND
(`INST_MOVE  | (`ROW_ADR1  << 7) | `NAND_ADR ), //send row adr1 to NAND
(`INST_MOVE  | (`ROW_ADR2  << 7) | `NAND_ADR ), //send row adr2 to NAND
(`INST_MOVE  | (`ROW_ADR3  << 7) | `NAND_ADR ), //send row adr3 to NAND
(`INST_MOVE  | (`ROW_ADR4  << 7) | `NAND_ADR ), //send row adr4 to NAND
(`INST_WRITE | (`AIPR_EXIT << 7) | `NAND_CMD ), //send aipr disable cmd(8'h0C) to NAND
`ifdef B47R_SLC
(`INST_WRITE | (8'h3B      << 7) | `NAND_CMD ), //send SLC mode cmd(8'h3B) to NAND
`endif
(`INST_WRITE | (`ERAS_CMD  << 7) | `NAND_CMD ), //send erase cmd(8'h60) to NAND
(`INST_MOVE  | (`ROW_ADR1  << 7) | `NAND_ADR ), //send row adr1 to NAND
(`INST_MOVE  | (`ROW_ADR2  << 7) | `NAND_ADR ), //send row adr2 to NAND
(`INST_MOVE  | (`ROW_ADR3  << 7) | `NAND_ADR ), //send row adr3 to NAND
`ifndef NAND_BICS5
(`INST_MOVE  | (`ROW_ADR4  << 7) | `NAND_ADR ), //send row adr4 to NAND
`endif
(`INST_MOVE  | (`SET_PLANE << 7) | `PROC_REG0), //copy "current plane" to processor reg0
(`INST_WRITE | (`PLANE_END << 7) | `PROC_REG1), //set "plane end number" to processor reg1
`ifdef NAND_BICS5
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd3      ), //compare plane number, if plane fin, jump to "erase end cmd"
`elsif PROC_TDLY
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd12     ), //compare plane number, if plane fin, jump to "erase end cmd"
`else
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd11     ), //compare plane number, if plane fin, jump to "erase end cmd"
`endif
(`INST_OPRT  | (`OPRT_ADD  << 7) | `SET_PLANE), //add plane number
`ifdef NAND_BICS5
(`INST_JUMP  | (`JUMP_NCBK << 8) | 8'd8      ), //jump back to "erase cmd"
`else
(`INST_WRITE | (`ERAS_MTPL << 7) | `NAND_CMD ), //send erase multi-plane cmd(8'hD1) to NAND
(`INST_WRITE | (`WAIT_TWB  << 7) | `NAND_WAIT), //need wait tWB(80/90/100ns)
(`INST_WRITE | (`READ_STS  << 7) | `NAND_CMD ), //send read status cmd(8'h70) to NAND
(`INST_WRITE | (8'h0       << 7) | `NAND_READ), //read status
`ifdef PROC_TDLY
(`INST_MOVE  | (`NAND_RDT  << 7) | `PROC_REG0), //dummy read for FPGA timing issue
`endif
(`INST_MOVE  | (`NAND_RDT  << 7) | `PROC_REG0), //copy NAND read data to processor reg0
(`INST_WRITE | (`STS_ARDY  << 7) | `PROC_REG1), //set nand_ready_bitmap(8'h60) to processor reg1
(`INST_OPRT  | (`OPRT_AND  << 7) | `PROC_REG0), //sts_data & nand_ready_bitmap(8'h60)
`ifdef PROC_TDLY
`ifdef NAND_BICS5
(`INST_JUMP  | (`JUMP_EQBK << 8) | 8'd16     ), //check whether NAND ready, if ready, jump back to "erase cmd"
`else
(`INST_JUMP  | (`JUMP_EQBK << 8) | 8'd17     ), //check whether NAND ready, if ready, jump back to "erase cmd"
`endif
(`INST_JUMP  | (`JUMP_NCBK << 8) | 8'd7      ), //jump to "read status cmd"
`else
`ifdef NAND_BICS5
(`INST_JUMP  | (`JUMP_EQBK << 8) | 8'd15     ), //check whether NAND ready, if ready, jump back to "erase cmd"
`else
(`INST_JUMP  | (`JUMP_EQBK << 8) | 8'd16     ), //check whether NAND ready, if ready, jump back to "erase cmd"
`endif
(`INST_JUMP  | (`JUMP_NCBK << 8) | 8'd6      ), //jump to "read status cmd"
`endif
`endif //NAND_BICS5
(`INST_WRITE | (`ERAS_END  << 7) | `NAND_CMD ), //send erase end(8'hD0) to NAND
(`INST_WRITE | (`ERAS_RSTS << 7) | `SET_BUSY ), //set nand status as busy and force read status
(`INST_FIN                                   )  //finish
};

//multi-plane program command, cmd_ptr = 6'd2
assign micro_code[2] =
{
(`INST_MOVE  | (`CEN_NUM   << 7) | `NAND_CEN ), //enable CEn
(`INST_WRITE | (8'h0       << 7) | `SET_PLANE), //init plane number
(`INST_MOVE  | (`AIPR_STS  << 7) | `PROC_REG0), //copy "aipr status" to processor reg0
(`INST_WRITE | (8'h0       << 7) | `PROC_REG1), //set "8'h0" to processor reg1
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd7      ), //compare aipr status, if aipr no set, jump to "program cmd"
(`INST_WRITE | (`READ_ESTS << 7) | `NAND_CMD ), //send read enhance status cmd(8'h78) to NAND
(`INST_MOVE  | (`ROW_ADR1  << 7) | `NAND_ADR ), //send row adr1 to NAND
(`INST_MOVE  | (`ROW_ADR2  << 7) | `NAND_ADR ), //send row adr2 to NAND
(`INST_MOVE  | (`ROW_ADR3  << 7) | `NAND_ADR ), //send row adr3 to NAND
(`INST_MOVE  | (`ROW_ADR4  << 7) | `NAND_ADR ), //send row adr4 to NAND
(`INST_WRITE | (`AIPR_EXIT << 7) | `NAND_CMD ), //send aipr disable cmd(8'h0C) to NAND
`ifdef NAND_BICS5
(`INST_MOVE  | (`OSPG_NUM  << 7) | `NAND_CMD ), //send "one shot page number(8'h01/8'h02/8'h03)" to NAND
`endif
`ifdef B47R_SLC
(`INST_WRITE | (8'h3B      << 7) | `NAND_CMD ), //send SLC mode cmd(8'h3B) to NAND
`endif
(`INST_WRITE | (`PROG_CMD  << 7) | `NAND_CMD ), //send program cmd(8'h80) to NAND
(`INST_WRITE | (8'h0       << 7) | `NAND_ADR ), //send column adr1 to NAND
(`INST_WRITE | (8'h0       << 7) | `NAND_ADR ), //send column adr2 to NAND
(`INST_MOVE  | (`ROW_ADR1  << 7) | `NAND_ADR ), //send row adr1 to NAND
(`INST_MOVE  | (`ROW_ADR2  << 7) | `NAND_ADR ), //send row adr2 to NAND
(`INST_MOVE  | (`ROW_ADR3  << 7) | `NAND_ADR ), //send row adr3 to NAND
`ifndef NAND_BICS5
(`INST_MOVE  | (`ROW_ADR4  << 7) | `NAND_ADR ), //send row adr4 to NAND
`endif
(`INST_WRITE | (`WAIT_TADL << 7) | `NAND_WAIT), //need wait tADL(150/300ns)
(`INST_WRITE | (8'h0       << 7) | `NAND_WDMA), //enable DMA write engine
(`INST_MOVE  | (`SET_PLANE << 7) | `PROC_REG0), //copy "current plane" to processor reg0
(`INST_WRITE | (`PLANE_END << 7) | `PROC_REG1), //set "plane end number" to processor reg1
`ifdef PROC_TDLY
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd12     ), //compare plane number, if plane fin, jump to "program end cmd"
`else
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd11     ), //compare plane number, if plane fin, jump to "program end cmd"
`endif
(`INST_OPRT  | (`OPRT_ADD  << 7) | `SET_PLANE), //add plane number
(`INST_WRITE | (`PROG_MTPL << 7) | `NAND_CMD ), //send program multi-plane cmd(8'h11) to NAND
(`INST_WRITE | (`WAIT_TWB  << 7) | `NAND_WAIT), //need wait tWB(80/90/100ns)
(`INST_WRITE | (`READ_STS  << 7) | `NAND_CMD ), //send read status cmd(8'h70) to NAND
(`INST_WRITE | (8'h0       << 7) | `NAND_READ), //read status
`ifdef PROC_TDLY
(`INST_MOVE  | (`NAND_RDT  << 7) | `PROC_REG0), //dummy read for FPGA timing issue
`endif
(`INST_MOVE  | (`NAND_RDT  << 7) | `PROC_REG0), //copy NAND read data to processor reg0
(`INST_WRITE | (`STS_ARDY  << 7) | `PROC_REG1), //set nand_ready_bitmap(8'h60) to processor reg1
(`INST_OPRT  | (`OPRT_AND  << 7) | `PROC_REG0), //sts_data & nand_ready_bitmap(8'h60)
`ifdef PROC_TDLY
(`INST_JUMP  | (`JUMP_EQBK << 8) | 8'd21     ), //check whether NAND ready, if ready, jump back to "program cmd"
(`INST_JUMP  | (`JUMP_NCBK << 8) | 8'd7      ), //jump to "read status cmd"
`else
(`INST_JUMP  | (`JUMP_EQBK << 8) | 8'd20     ), //check whether NAND ready, if ready, jump back to "program cmd"
(`INST_JUMP  | (`JUMP_NCBK << 8) | 8'd6      ), //jump to "read status cmd"
`endif
`ifndef NAND_B47R
(`INST_MOVE  | (`OSPG_NUM  << 7) | `PROC_REG0), //copy "one shot page number" to processor reg0
(`INST_WRITE | (`OSPG_END  << 7) | `PROC_REG1), //set "one shot page end(3)" to processor reg1
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd4      ), //compare one shot page number, if is 3, jump to "program end cmd"
(`INST_WRITE | (`PROG_MED  << 7) | `NAND_CMD ), //send program medium(8'h1A) to NAND
(`INST_WRITE | (`PMED_RSTS << 7) | `SET_BUSY ), //set nand status as busy and force read status
(`INST_JUMP  | (`JUMP_NCFW << 8) | 8'd3      ), //jump to "set nand status"
`endif
(`INST_WRITE | (`PROG_END  << 7) | `NAND_CMD ), //send program end(8'h10) to NAND
(`INST_WRITE | (`PROG_RSTS << 7) | `SET_BUSY ), //set nand status as busy and force read status
(`INST_FIN                                   )  //finish
};

//multi-plane read command, cmd_ptr = 6'd3
assign micro_code[3] =
{
(`INST_MOVE  | (`CEN_NUM   << 7) | `NAND_CEN ), //enable CEn
(`INST_MOVE  | (`SINGLE_PL << 7) | `PROC_REG0), //move single_plane to processor reg0
(`INST_SHIFT | (`SHIFT_RL4 << 7) | `SET_PLANE), //init plane number, 8'h10:single plane, 8'h0:multi-plane
(`INST_MOVE  | (`AIPR_STS  << 7) | `PROC_REG0), //copy "aipr status" to processor reg0
(`INST_WRITE | (8'h0       << 7) | `PROC_REG1), //set "8'h0" to processor reg1
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd7      ), //compare aipr status, if aipr no set, jump to "read cmd"
(`INST_WRITE | (`READ_ESTS << 7) | `NAND_CMD ), //send read enhance status cmd(8'h78) to NAND
(`INST_MOVE  | (`ROW_ADR1  << 7) | `NAND_ADR ), //send row adr1 to NAND
(`INST_MOVE  | (`ROW_ADR2  << 7) | `NAND_ADR ), //send row adr2 to NAND
(`INST_MOVE  | (`ROW_ADR3  << 7) | `NAND_ADR ), //send row adr3 to NAND
(`INST_MOVE  | (`ROW_ADR4  << 7) | `NAND_ADR ), //send row adr4 to NAND
(`INST_WRITE | (`AIPR_EXIT << 7) | `NAND_CMD ), //send aipr disable cmd(8'h0C) to NAND
`ifdef NAND_BICS5
(`INST_MOVE  | (`OSPG_NUM  << 7) | `NAND_CMD ), //send "one shot page number(8'h01/8'h02/8'h03)" to NAND
`endif
`ifdef B47R_SLC
(`INST_WRITE | (8'h3B      << 7) | `NAND_CMD ), //send SLC mode cmd(8'h3B) to NAND
`endif
(`INST_WRITE | (`READ_PAGE << 7) | `NAND_CMD ), //send read cmd(8'h00) to NAND
(`INST_WRITE | (8'h0       << 7) | `NAND_ADR ), //send column adr1 to NAND
(`INST_WRITE | (8'h0       << 7) | `NAND_ADR ), //send column adr2 to NAND
(`INST_MOVE  | (`ROW_ADR1  << 7) | `NAND_ADR ), //send row adr1 to NAND
(`INST_MOVE  | (`ROW_ADR2  << 7) | `NAND_ADR ), //send row adr2 to NAND
(`INST_MOVE  | (`ROW_ADR3  << 7) | `NAND_ADR ), //send row adr3 to NAND
`ifndef NAND_BICS5
(`INST_MOVE  | (`ROW_ADR4  << 7) | `NAND_ADR ), //send row adr4 to NAND
`endif
(`INST_MOVE  | (`SET_PLANE << 7) | `PROC_REG0), //copy "current plane" to processor reg0
(`INST_WRITE | (8'h10      << 7) | `PROC_REG1), //set 8'h10(imply single plane) to processor reg1
`ifdef PROC_TDLY
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd14     ), //compare plane, if single plane, jump to "read end cmd"
`else
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd13     ), //compare plane, if single plane, jump to "read end cmd"
`endif
(`INST_WRITE | (`PLANE_END << 7) | `PROC_REG1), //set "plane end number" to processor reg1
`ifdef PROC_TDLY
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd12     ), //compare plane number, if plane fin, jump to "read end cmd"
`else
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd11     ), //compare plane number, if plane fin, jump to "read end cmd"
`endif
(`INST_OPRT  | (`OPRT_ADD  << 7) | `SET_PLANE), //add plane number
(`INST_WRITE | (`READ_MTPL << 7) | `NAND_CMD ), //send read multi-plane cmd(8'h32) to NAND
(`INST_WRITE | (`WAIT_TWB  << 7) | `NAND_WAIT), //need wait tWB(80/90/100ns)
(`INST_WRITE | (`READ_STS  << 7) | `NAND_CMD ), //send read status cmd(8'h70) to NAND
(`INST_WRITE | (8'h0       << 7) | `NAND_READ), //read status
`ifdef PROC_TDLY
(`INST_MOVE  | (`NAND_RDT  << 7) | `PROC_REG0), //dummy read for FPGA timing issue
`endif
(`INST_MOVE  | (`NAND_RDT  << 7) | `PROC_REG0), //copy NAND read data to processor reg0
(`INST_WRITE | (`STS_RDY   << 7) | `PROC_REG1), //set nand_ready_bitmap(8'h60) to processor reg1
(`INST_OPRT  | (`OPRT_AND  << 7) | `PROC_REG0), //sts_data & nand_ready_bitmap(8'h60)
`ifdef PROC_TDLY
(`INST_JUMP  | (`JUMP_EQBK << 8) | 8'd21     ), //check whether NAND ready, if ready, jump back to "read cmd"
(`INST_JUMP  | (`JUMP_NCBK << 8) | 8'd7      ), //jump to "read status cmd"
`else
(`INST_JUMP  | (`JUMP_EQBK << 8) | 8'd20     ), //check whether NAND ready, if ready, jump back to "read cmd"
(`INST_JUMP  | (`JUMP_NCBK << 8) | 8'd6      ), //jump to "read status cmd"
`endif
(`INST_MOVE  | (`ECTI_STS  << 7) | `PROC_REG0), //copy "cache status" to processor reg0
(`INST_WRITE | (`RCAH_SET  << 7) | `PROC_REG1), //set "cache read set" to processor reg1
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd3      ), //compare cache status, if cache read set, jump to "cache read cmd"
(`INST_WRITE | (`READ_END  << 7) | `NAND_CMD ), //send read end(8'h30) to NAND
(`INST_JUMP  | (`JUMP_NCFW << 8) | 8'd2      ), //jump to "set nand status"
(`INST_WRITE | (`RCAH_CMD  << 7) | `NAND_CMD ), //send cache read(8'h31) to NAND
(`INST_WRITE | (`RCMD_RSTS << 7) | `SET_BUSY ), //set nand status as busy
(`INST_FIN                                   )  //finish
};

//multi-plane read data, cmd_ptr = 6'd4
assign micro_code[4] =
{
(`INST_MOVE  | (`CEN_NUM   << 7) | `NAND_CEN ), //enable CEn
(`INST_WRITE | (`AUTO_PL   << 7) | `SET_PLANE), //init plane number
(`INST_WRITE | (`RCOL_CMD  << 7) | `NAND_CMD ), //send "change read column cmd(8'h05/8'h06)" to NAND
(`INST_MOVE  | (`COL_ADR1  << 7) | `NAND_ADR ), //send column adr1 to NAND
(`INST_MOVE  | (`COL_ADR2  << 7) | `NAND_ADR ), //send column adr2 to NAND
(`INST_MOVE  | (`ROW_ADR1  << 7) | `NAND_ADR ), //send row adr1 to NAND
(`INST_MOVE  | (`ROW_ADR2  << 7) | `NAND_ADR ), //send row adr2 to NAND
(`INST_MOVE  | (`ROW_ADR3  << 7) | `NAND_ADR ), //send row adr3 to NAND
`ifndef NAND_BICS5
(`INST_MOVE  | (`ROW_ADR4  << 7) | `NAND_ADR ), //send row adr4 to NAND
`endif
(`INST_WRITE | (`RCOL_END  << 7) | `NAND_CMD ), //send change read column end(8'hE0)" to NAND
(`INST_WRITE | (`WAIT_TCCS << 7) | `NAND_WAIT), //need wait tCCS(275/300/400ns)
(`INST_WRITE | (8'h0       << 7) | `NAND_RDMA), //enable DMA read engine
(`INST_WRITE | (8'h0       << 7) | `PROC_REG0), //set 8'h0 to processor reg0
(`INST_MOVE  | (`DMA_STS   << 7) | `PROC_REG1), //copy "dma status" to processor reg1
`ifdef NAND_BICS5
(`INST_JUMP  | (`JUMP_NEBK << 8) | 8'd11     ), //jump to "change read column cmd"
`else
(`INST_JUMP  | (`JUMP_NEBK << 8) | 8'd12     ), //jump to "change read column cmd"
`endif
`ifndef NAND_B47R
(`INST_WRITE | (`CEN_OFF   << 7) | `NAND_CEN ), //disable CEn
//(`INST_WRITE | (`WAIT_TRPH << 7) | `NAND_WAIT), //need wait tRPSTH(15/25ns)
`endif
(`INST_FIN                                   )  //finish
};

//cache read end, cmd_ptr = 6'd5
assign micro_code[5] =
{
(`INST_MOVE  | (`CEN_NUM   << 7) | `NAND_CEN ), //enable CEn
(`INST_WRITE | (`RCAH_END  << 7) | `NAND_CMD ), //send "cache read end(8'h3F)" to NAND
(`INST_WRITE | (`RCMD_RSTS << 7) | `SET_BUSY ), //set nand status as busy
(`INST_FIN                                   )  //finish
};

//read aipr status command, cmd_ptr = 6'd6
assign micro_code[6] =
{
(`INST_MOVE  | (`CEN_NUM   << 7) | `NAND_CEN ), //enable CEn
(`INST_WRITE | (`AUTO_PL   << 7) | `SET_PLANE), //init plane number, set auto plane cnt(8'h10)
(`INST_WRITE | (`READ_ASTS << 7) | `NAND_CMD ), //send read aipr status cmd(8'h77) to NAND
(`INST_MOVE  | (`ROW_ADR1  << 7) | `NAND_ADR ), //send row adr1 to NAND
(`INST_MOVE  | (`ROW_ADR2  << 7) | `NAND_ADR ), //send row adr2 to NAND
(`INST_MOVE  | (`ROW_ADR3  << 7) | `NAND_ADR ), //send row adr3 to NAND
`ifndef NAND_BICS5
(`INST_MOVE  | (`ROW_ADR4  << 7) | `NAND_ADR ), //send row adr4 to NAND
`endif
(`INST_WRITE | (8'h0       << 7) | `NAND_READ), //read status
`ifdef PROC_TDLY
(`INST_MOVE  | (`NAND_RDT  << 7) | `PROC_REG0), //dummy read for FPGA timing issue
`endif
(`INST_MOVE  | (`NAND_RDT  << 7) | `STS_DATA ), //move NAND read data to status check reg
`ifndef NAND_B47R
(`INST_WRITE | (`CEN_OFF   << 7) | `NAND_CEN ), //disable CEn
//(`INST_WRITE | (`WAIT_TRPH << 7) | `NAND_WAIT), //need wait tRPSTH(15/25ns)
`endif
(`INST_FIN                                   )  //finish
};

//aipr read command, cmd_ptr = 6'd7
assign micro_code[7] =
{
(`INST_MOVE  | (`CEN_NUM   << 7) | `NAND_CEN ), //enable CEn
(`INST_WRITE | (`AUTO_PL   << 7) | `SET_PLANE), //init plane number, set auto plane cnt(8'h10)
(`INST_MOVE  | (`AIPR_STS  << 7) | `PROC_REG0), //copy "aipr status" to processor reg0
(`INST_WRITE | (8'h1       << 7) | `PROC_REG1), //set "8'h1" to processor reg1
(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd7      ), //compare aipr status, if aipr set, jump to "read cmd"
(`INST_WRITE | (`READ_ESTS << 7) | `NAND_CMD ), //send read enhance status cmd(8'h78) to NAND
(`INST_MOVE  | (`ROW_ADR1  << 7) | `NAND_ADR ), //send row adr1 to NAND
(`INST_MOVE  | (`ROW_ADR2  << 7) | `NAND_ADR ), //send row adr2 to NAND
(`INST_MOVE  | (`ROW_ADR3  << 7) | `NAND_ADR ), //send row adr3 to NAND
(`INST_MOVE  | (`ROW_ADR4  << 7) | `NAND_ADR ), //send row adr4 to NAND
(`INST_WRITE | (`AIPR_ENTR << 7) | `NAND_CMD ), //send aipr enable cmd(8'h0B) to NAND
(`INST_WRITE | (`READ_PAGE << 7) | `NAND_CMD ), //send read cmd(8'h00) to NAND
(`INST_WRITE | (8'h0       << 7) | `NAND_ADR ), //send column adr1 to NAND
(`INST_WRITE | (8'h0       << 7) | `NAND_ADR ), //send column adr2 to NAND
(`INST_MOVE  | (`ROW_ADR1  << 7) | `NAND_ADR ), //send row adr1 to NAND
(`INST_MOVE  | (`ROW_ADR2  << 7) | `NAND_ADR ), //send row adr2 to NAND
(`INST_MOVE  | (`ROW_ADR3  << 7) | `NAND_ADR ), //send row adr3 to NAND
(`INST_MOVE  | (`ROW_ADR4  << 7) | `NAND_ADR ), //send row adr4 to NAND
//(`INST_MOVE  | (`ECTI_STS  << 7) | `PROC_REG0), //copy "cache status" to processor reg0
//(`INST_WRITE | (`RCAH_SET  << 7) | `PROC_REG1), //set "cache read set" to processor reg1
//(`INST_JUMP  | (`JUMP_EQFW << 8) | 8'd3      ), //compare cache status, if cache read set, jump to "cache read cmd"
(`INST_WRITE | (`READ_END  << 7) | `NAND_CMD ), //send read end(8'h30) to NAND
//(`INST_JUMP  | (`JUMP_NCFW << 8) | 8'd2      ), //jump to "set nand status"
//(`INST_WRITE | (`RCAH_CMD  << 7) | `NAND_CMD ), //send cache read(8'h31) to NAND
(`INST_WRITE | (`AIPR_RSTS << 7) | `SET_BUSY ), //set nand status as busy
(`INST_FIN                                   )  //finish
}; //end
