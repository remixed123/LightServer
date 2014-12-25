/*
 * lsspiconfig.c
 *
 *  Created on: 30/09/2014
 *      Author: glenn
 */

#include "lightserver.h"
#include "lsspisend.h"
#include "lsspiconfig.h"

/* SYS/BIOS Headers */
#include <ti/sysbios/knl/Task.h>

/* Peripheral Headers */
#include <ti/drivers/SPI.h>
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"

//*****************************************************************************
//! configureSPI
//!
//! Configures SPI Clock for the particular protocolType and icType
//!
//!
//****************************************************************************
lsresult_t configureSPI()
{
	if (lightConfig.protocolType == 0) //WS8211 or TM1809
	{
		if (lightConfig.icType == 0 || lightConfig.icType == 1) //WS8211 or WS8212
		{
			SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN); //80 MHz
			SSIConfigSetExpClk(SSI0_BASE, 80000000, SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, 8000000, 10);

		}
		else//TM1809 or other
		{
			SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN); //80
			SSIConfigSetExpClk(SSI0_BASE, 80000000, SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, 8000000, 10);
		}
	 }
	 else if (lightConfig.protocolType == 1) //CYT3005 Lights
	 {
		SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
		SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_1, SSI_MODE_MASTER, 1000000, 16);
	 }

	// Enable the SPI SSI0 module.
	SSIEnable(SSI0_BASE);

	if (lightConfig.protocolType == 1) // Send Reset and Addressing details for Minleon lights
	{
		resetLights();
		Task_sleep(1);
		addressLights();
		Task_sleep(10);
		resetLights();
		Task_sleep(1);
		addressLights();
		Task_sleep(10);
	}

	return LS_SUCCESS;
}

