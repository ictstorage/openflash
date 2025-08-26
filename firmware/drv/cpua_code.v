`define CPUA_CODE_NUM   3
`define DATA_PTR        4'd0 //program data
`define P2LP_PTR        4'd1 //p2l page
`define RAPT_PTR        4'd2 //raid parity

//instruction
`define INST_PBUS 16'h0000
`define INST_READ 16'h1000
`define INST_JPFW 16'h2000
`define INST_JPBK 16'h3000
  `define JUMP_JPEQ 16'h4  //jump for euqal
  `define JUMP_JPNE 16'h5  //jump for non euqal
  `define JUMP_JPNC 16'h6  //jump for no condition
`define INST_SFTL 16'h4000 //shift left
`define INST_SFTR 16'h6000 //shift right
`define INST_ADD  16'h8000 //add
`define INST_SBT  16'h9000 //subtract
`define INST_AND  16'ha000 //and
`define INST_OR   16'hb000 //or
`define INST_OTHR 16'hf000
  `define OTHR_ADD4 16'h1
  `define OTHR_SBT4 16'h3
`define INST_WDAT 16'hF800
  `define WDAT_BYT0 16'h0  //byte0
  `define WDAT_BYT1 16'h1  //byte1
  `define WDAT_BYTA 16'h6  //byte all
`define INST_PCLR 16'hfffd
`define INST_PFIN 16'hfffe
`define INST_PNOP 16'hffff

//cpua proc register
`define PROC_WDAT 16'h0  //write data reg
`define PROC_GREG 16'h1  //general reg
`define PROC_ZERO 16'h2  //all zero
`define PROC_BFP  16'h3  //bfp
`define PROC_P2L0 16'h4  //p2l 0
`define PROC_P2L1 16'h5  //p2l 1
`define PROC_P2L2 16'h6  //p2l 2
`define PROC_P2L3 16'h7  //p2l 3
`define PROC_L4K  16'h8  //l4k
`define PROC_P4K  16'h9  //p4k
`define PROC_P2LA 16'hA  //p2l adr
`define PROC_QLRA 16'hB  //{que_idx, raid idx}
`define PROC_CMDT 16'hC  //cmd type
`define PROC_NCMD 16'hD  //NCTL_CMD_BASE(32'h14000000)
`define PROC_SMMT 16'hE  //SRAM_META_BASE
`define PROC_DMMT 16'hF  //DRAM_META_BASE

`define SMDT_BASE 16'h2


//const word gw_cpua_code[][64] = {
wire [0:64*16-1] cpua_code [`CPUA_CODE_NUM];

//host write command, pcmd_ptr = 6'd0
assign cpua_code[0] =
{
  //(`INST_PBUS | (`PROC_L4K  << 8) | (`PROC_P4K  << 4) | 16'hf     ), //write l2p table
  (`INST_SFTR | (16'd9      << 8) | (`PROC_BFP  << 4) | `PROC_GREG), //bfp shift right 10bits to greg
  (`INST_WDAT | (`WDAT_BYT0 << 8) |                     `SMDT_BASE), //write SMDT_BASE to wdat reg byte0
  (`INST_JPFW | (`PROC_GREG << 8) | (`JUMP_JPNE << 5) | 16'd7     ), //check whether bfp[11:10] != SRAM_DATA_BASE
  (`INST_WDAT | (`WDAT_BYT0 << 8) |                     16'hff    ), //write 16'hff to wdat reg byte0
  (`INST_WDAT | (`WDAT_BYT1 << 8) |                     16'h03    ), //write 16'h3 to wdat reg byte1
  (`INST_AND  | (`PROC_WDAT << 8) | (`PROC_BFP  << 4) | `PROC_GREG), //bfp & 16'h3ff
  (`INST_SFTL | (16'd11     << 8) | (`PROC_GREG << 4) | `PROC_GREG), //bfp shift left 12bits
  (`INST_OR   | (`PROC_SMMT << 8) | (`PROC_GREG << 4) | `PROC_GREG), //bfp | SMMT_BASE
  (`INST_JPFW |                     (`JUMP_JPNC << 5) | 16'd3     ), //jump
  (`INST_SFTL | (16'd7      << 8) | (`PROC_BFP  << 4) | `PROC_GREG), //bfp shift left 8bits
  (`INST_OR   | (`PROC_DMMT << 8) | (`PROC_GREG << 4) | `PROC_GREG), //bfp | DRAM_META_BASE
  (`INST_PBUS | (`PROC_GREG << 8) | (`PROC_L4K  << 4) | 16'hf     ), //write fw meta 0
  (`INST_OTHR | (`OTHR_ADD4 << 8) | (`PROC_GREG << 4) | `PROC_GREG), //greg+4
  (`INST_PBUS | (`PROC_GREG << 8) | (`PROC_P4K  << 4) | 16'hf     ), //write fw meta 1
  (`INST_OTHR | (`OTHR_ADD4 << 8) | (`PROC_GREG << 4) | `PROC_GREG), //greg+4
  (`INST_PBUS | (`PROC_GREG << 8) | (`PROC_ZERO << 4) | 16'hf     ), //write fw meta 2
  (`INST_OTHR | (`OTHR_ADD4 << 8) | (`PROC_GREG << 4) | `PROC_GREG), //greg+4
  (`INST_PBUS | (`PROC_GREG << 8) | (`PROC_ZERO << 4) | 16'hf     ), //write fw meta 3
  (`INST_OTHR | (`OTHR_ADD4 << 8) | (`PROC_GREG << 4) | `PROC_GREG), //greg+4
  (`INST_PBUS | (`PROC_GREG << 8) | (`PROC_ZERO << 4) | 16'hf     ), //write fw meta 4
  (`INST_OTHR | (`OTHR_ADD4 << 8) | (`PROC_GREG << 4) | `PROC_GREG), //greg+4
  (`INST_PBUS | (`PROC_GREG << 8) | (`PROC_ZERO << 4) | 16'hf     ), //write fw meta 5
  (`INST_OTHR | (`OTHR_ADD4 << 8) | (`PROC_GREG << 4) | `PROC_GREG), //greg+4
  (`INST_PBUS | (`PROC_GREG << 8) | (`PROC_ZERO << 4) | 16'hf     ), //write fw meta 6
  (`INST_OTHR | (`OTHR_ADD4 << 8) | (`PROC_GREG << 4) | `PROC_GREG), //greg+4
  (`INST_PBUS | (`PROC_GREG << 8) | (`PROC_ZERO << 4) | 16'hf     ), //write fw meta 7

  (`INST_WDAT | (`WDAT_BYT0 << 8) |                     16'hc     ), //write 16'hc to wdat reg byte0
  (`INST_WDAT | (`WDAT_BYT1 << 8) |                     16'h0     ), //write 16'h0 to wdat reg byte1
  (`INST_AND  | (`PROC_WDAT << 8) | (`PROC_P2LA << 4) | `PROC_GREG), //write (p2l_adr & 16'hc) to greg
  (`INST_JPFW | (`PROC_GREG << 8) | (`JUMP_JPNE << 5) | 16'd11    ), //check whether (p2l_adr & 16'hc) != 16'hc
  (`INST_WDAT | (`WDAT_BYTA << 8) |                     16'hff    ), //write 16'hff to wdat reg all byte
  (`INST_WDAT | (`WDAT_BYT0 << 8) |                     16'hf0    ), //write 16'hf0 to wdat reg byte0
  (`INST_AND  | (`PROC_WDAT << 8) | (`PROC_P2LA << 4) | `PROC_GREG), //p2l_adr & 32'hfffffff0
  (`INST_PBUS | (`PROC_GREG << 8) | (`PROC_P2L0 << 4) | 16'hf     ), //write p2l 0
  (`INST_OTHR | (`OTHR_ADD4 << 8) | (`PROC_GREG << 4) | `PROC_GREG), //greg+4
  (`INST_PBUS | (`PROC_GREG << 8) | (`PROC_P2L1 << 4) | 16'hf     ), //write p2l 1
  (`INST_OTHR | (`OTHR_ADD4 << 8) | (`PROC_GREG << 4) | `PROC_GREG), //greg+4
  (`INST_PBUS | (`PROC_GREG << 8) | (`PROC_P2L2 << 4) | 16'hf     ), //write p2l 2
  (`INST_OTHR | (`OTHR_ADD4 << 8) | (`PROC_GREG << 4) | `PROC_GREG), //greg+4
  (`INST_PBUS | (`PROC_GREG << 8) | (`PROC_P2L3 << 4) | 16'hf     ), //write p2l 3

  (`INST_OTHR | (`OTHR_ADD4 << 8) | (`PROC_NCMD << 4) | `PROC_NCMD), //`PROC_NCMD+4
  (`INST_OTHR | (`OTHR_ADD4 << 8) | (`PROC_NCMD << 4) | `PROC_NCMD), //`PROC_NCMD+4
  (`INST_PBUS | (`PROC_NCMD << 8) | (`PROC_QLRA << 4) | 16'hb     ), //write {qidx, ridx} to nctl
  (`INST_OTHR | (`OTHR_SBT4 << 8) | (`PROC_NCMD << 4) | `PROC_NCMD), //`PROC_NCMD-4
  (`INST_PBUS | (`PROC_NCMD << 8) | (`PROC_P4K  << 4) | 16'hf     ), //write p4k to nctl
  (`INST_SFTL | (16'd15     << 8) | (`PROC_BFP  << 4) | `PROC_BFP ), //bfp shift left 16bits
  (`INST_OR   | (`PROC_BFP  << 8) | (`PROC_CMDT << 4) | `PROC_CMDT), //cmd_type | bfp
  (`INST_OTHR | (`OTHR_SBT4 << 8) | (`PROC_NCMD << 4) | `PROC_NCMD), //`PROC_NCMD-4
  (`INST_PBUS | (`PROC_NCMD << 8) | (`PROC_CMDT << 4) | 16'hf     ), //write (cmd_type | bfp) to nctl
  (`INST_PFIN                                                     )  //finish
};

//p2l page command, pcmd_ptr = 6'd1
assign cpua_code[1] =
{
  (`INST_WDAT | (`WDAT_BYTA << 8) |                     16'h00    ), //write 16'h00 to wdat reg all byte
  (`INST_WDAT | (`WDAT_BYT0 << 8) |                     16'h28    ), //write 16'h28 to wdat reg byte0
  (`INST_OR   | (`PROC_WDAT << 8) | (`PROC_NCMD << 4) | `PROC_NCMD), //`PROC_NCMD | 16'h28
  (`INST_PBUS | (`PROC_NCMD << 8) | (`PROC_P2LA << 4) | 16'h3     ), //write p2l base addr to nctl dmpt
  (`INST_WDAT | (`WDAT_BYT0 << 8) |                     16'h1c    ), //write 16'h1c to wdat reg byte0
  (`INST_SBT  | (`PROC_WDAT << 8) | (`PROC_NCMD << 4) | `PROC_NCMD), //`PROC_NCMD-16'h1c
  (`INST_SFTL | (16'd15     << 8) | (`PROC_GREG << 4) | `PROC_GREG), //greg shift left 16bits
  (`INST_PBUS | (`PROC_NCMD << 8) | (`PROC_GREG << 4) | 16'hc     ), //write dram loc to nctl
  (`INST_OTHR | (`OTHR_SBT4 << 8) | (`PROC_NCMD << 4) | `PROC_NCMD), //`PROC_NCMD-4
  (`INST_PBUS | (`PROC_NCMD << 8) | (`PROC_QLRA << 4) | 16'hb     ), //write {qidx, ridx} to nctl
  (`INST_OTHR | (`OTHR_SBT4 << 8) | (`PROC_NCMD << 4) | `PROC_NCMD), //`PROC_NCMD-4
  (`INST_PBUS | (`PROC_NCMD << 8) | (`PROC_P4K  << 4) | 16'hf     ), //write p4k to nctl
  (`INST_SFTL | (16'd15     << 8) | (`PROC_BFP  << 4) | `PROC_BFP ), //bfp shift left 16bits
  (`INST_OR   | (`PROC_BFP  << 8) | (`PROC_CMDT << 4) | `PROC_CMDT), //cmd_type | bfp
  (`INST_OTHR | (`OTHR_SBT4 << 8) | (`PROC_NCMD << 4) | `PROC_NCMD), //`PROC_NCMD-4
  (`INST_PBUS | (`PROC_NCMD << 8) | (`PROC_CMDT << 4) | 16'hf     ), //write (cmd_type | bfp) to nctl
  (`INST_PFIN                                                     )  //finish
};

//raid parity command, pcmd_ptr = 6'd2
assign cpua_code[2] =
{
  (`INST_WDAT | (`WDAT_BYT0 << 8) |                     16'h10    ), //write 16'h10 to wdat reg byte0
  (`INST_OR   | (`PROC_WDAT << 8) | (`PROC_NCMD << 4) | `PROC_NCMD), //`PROC_NCMD | 16'h10
  (`INST_PBUS | (`PROC_NCMD << 8) | (`PROC_ZERO << 4) | 16'h1     ), //write p2l base addr to nctl dmpt
  (`INST_OTHR | (`OTHR_SBT4 << 8) | (`PROC_NCMD << 4) | `PROC_NCMD), //`PROC_NCMD-4
  (`INST_OTHR | (`OTHR_SBT4 << 8) | (`PROC_NCMD << 4) | `PROC_NCMD), //`PROC_NCMD-4
  (`INST_PBUS | (`PROC_NCMD << 8) | (`PROC_QLRA << 4) | 16'hb     ), //write {qidx, ridx} to nctl
  (`INST_OTHR | (`OTHR_SBT4 << 8) | (`PROC_NCMD << 4) | `PROC_NCMD), //`PROC_NCMD-4
  (`INST_PBUS | (`PROC_NCMD << 8) | (`PROC_P4K  << 4) | 16'hf     ), //write p4k to nctl
  (`INST_SFTL | (16'd15     << 8) | (`PROC_BFP  << 4) | `PROC_BFP ), //bfp shift left 16bits
  (`INST_OR   | (`PROC_BFP  << 8) | (`PROC_CMDT << 4) | `PROC_CMDT), //cmd_type | bfp
  (`INST_OTHR | (`OTHR_SBT4 << 8) | (`PROC_NCMD << 4) | `PROC_NCMD), //`PROC_NCMD-4
  (`INST_PBUS | (`PROC_NCMD << 8) | (`PROC_CMDT << 4) | 16'hf     ), //write (cmd_type | bfp) to nctl
  (`INST_PFIN                                                     )  //finish
}; //end
