/*
 * main.c
 *
 *  Created on: 29/09/2014
 *      Author: glenn
 */

/* XDCtools Header files */
#include <xdc/runtime/System.h>

/* SYS/BIOS Headers */
#include <ti/sysbios/BIOS.h>

/* Board Config Header */
#include "Board.h"

/* Spawn Task Priority */
extern const int SPAWN_TASK_PRI;

/* SimpleLink Wi-Fi Host Driver Header files */
#include <osi.h>

/*
 *  ======== main ========
 */
main(void)
{
    /* Call board init functions. */
    Board_initGeneral();
    Board_initGPIO();
    Board_initWiFi();
    Board_initSPI();

    // Turn LED to White to indicate that program entered main.
    GPIO_write(Board_LED0, Board_LED_ON); //Blue
    GPIO_write(Board_LED1, Board_LED_ON); //Green
    GPIO_write(Board_LED2, Board_LED_ON); //Red

    System_printf("Starting LightServer (Tiva C + CC3100 Version)\n"
                  "System provider is set to SysMin. Halt the target to view"
                  " any SysMin content in ROV.\n");

    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /*
     * The SimpleLink Host Driver requires a mechanism to allow functions to
     * execute in temporary context.  The SpawnTask is created to handle such
     * situations.  This task will remain blocked until the host driver
     * posts a function.  If the SpawnTask priority is higher than other tasks,
     * it will immediately execute the function and return to a blocked state.
     * Otherwise, it will remain ready until it is scheduled.
     */
    VStartSimpleLinkSpawnTask(SPAWN_TASK_PRI);

    /* Start BIOS */
    BIOS_start();

    return (0);
}
