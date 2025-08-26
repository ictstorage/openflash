#include <global.h>
#include <register.h>


void fv_ftl_oth(void)
{
    word lw_cqsd_ctg;

    lw_cqsd_ctg = (rs_host_cmd.sd_cmd_cid >> 7);

    //send CQ cmd
    rw_cqsd_sts = 0x0;
    rq_cqsd_dat = 0x0;
    rd_cqsd_set = ((dwrd)lw_cqsd_ctg << 16) | CQSD_SIR | CQSD_SDN | CQSD_ACT;

    while(rd_cqsd_set & CQSD_ACT);

  return;
}
