#include <global.h>


extern int $Super$$main(void);

void $Sub$$main(void)
{
    /* COLD reset; program is reloaded */

    /* change to sys mode and enable CPU interrupts */
    sysCPUSysModeEnaIntr();

    $Super$$main();
}
