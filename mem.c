#include "os.h"
#include "armv7.h"
#include "pages.h"

#define SCTLR_TE (1<<30) //No
#define SCTLR_AFE (1<<29) //Yes
#define SCTLR_TRE (1<<28) //Yes
#define SCTLR_NMFI (1<<27) //No
#define SCTLR_EE (1<<25) //No
#define SCTLR_VE (1<<24) //Yes
#define SCTLR_DZ (1<<19) //Yes
#define SCTLR_BR (1<<17) //No?
#define SCTLR_RR (1<<14) //No
#define SCTLR_V (1<13) //No
#define SCTLR_I (1<<12) // instruction cache
#define SCTLR_Z (1<<11) // branch prediction
#define SCTLR_SW (1<<10) //No
#define SCTLR_CP15BEN (1<<5) //Yes
#define SCTLR_C (1<<2) // cache enable
#define SCTLR_A (1<<1) //Yes
#define SCTLR_M 1 //MMU enable

#define LVL1_SEC_ADDR_MASK 0xfff00000
#define LVL1_SEC_NS (1<<19)
#define LVL1_SEC_NB (1<<17)
#define LVL1_SEC_S (1<<16)
#define LVL1_SEC_AP (1<<15)
#define LVL1_SEC_TEX_SHIFT 12
#define LVL1_SEC_AP_SHIFT 10
//TODO: bit 9
#define LVL1_SEC_DOMAIN_SHIFT 5
#define LVL1_SEC_XN (1<<4)
#define LVL1_SEC_C (1<<3)
#define LVL1_SEC_B (1<<2)
#define LVL1_SEC (1<<1)
#define LVL1_SEC_PXN 1

void init_mem(void) {
    /* Set the SCTLR. We are executing from DRAM with the MMU
     * enabled.  We can't disable the MMU because the default
     * memory map does not map DRAM as executable. */
    u32 sctlr;
    sctlr = get_sctlr();
    sctlr |= SCTLR_AFE | SCTLR_TRE | SCTLR_VE | SCTLR_DZ |
                SCTLR_I | SCTLR_Z | SCTLR_CP15BEN |
                SCTLR_A | SCTLR_M;
    sctlr &= ~( SCTLR_TE | SCTLR_NMFI | SCTLR_EE | SCTLR_BR |
                SCTLR_RR | SCTLR_V | SCTLR_SW );
    set_sctlr(sctlr);
}


