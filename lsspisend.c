//*****************************************************************************
//
// Application Name        - LightServer
// File Name			   - lsspisend.c
// Application Version     - 2.5.0
// Application Modify Date - 26th of December 2014
// Application Developer   - Glenn Vassallo
// Application Contact	   - contact@swiftsoftware.com.au
// Application Repository  - https://github.com/remixed123/LightServer
//
// Application Overview    - LightServer is a Wifi enabled embedded device which
//						     controls RGB Intelligent lighting in various ways.
//							 You can control the lights via iOS Apps or via a
//							 webpages that are on the device.This example project
//							 provides a starting
//
// File Overview		   - lsspisend.c contains functions to send SPI out after
//							 correct encoding
//
// Application Details     - https://github.com/remixed123/LightServer/readme.txt
//
//*****************************************************************************

#include "lsspiencode.h"
#include "lightserver.h"
#include "lsspiencode.h"
#include "lscreatedata.h"

#include <stdbool.h>
#include <stdint.h>

/* Peripheral Headers */
//#include <ti/drivers/SPI.h>
#include "driverlib/ssi.h"

//*****************************************************************************
//! sendSPI
//!
//! Sends bits out the MOSI SPI pin, uses the SPI Clock and a XOR IC to create
//! Manchester encoded signal.
//!
//! icType: 1 = Manchester ; 2 = WS8211; 3 = TM1809; 4 = WS8212
//*****************************************************************************
lsresult_t sendSpi(int icType)
{
	int j;
    uint16_t lightDataOutInt;

    lightDataOutInt = 0b0000000000000000;; //  000011110000;

    // Read any residual data from the SSI port.
    while(SSIDataGetNonBlocking(SSI0_BASE, &masterRxBuffer[0]))
    {
    }

    if (icType == WS8212)  // WS8212/NeoPixels Type
    {
		// Send the bits out using SPI
		for (j = 0; j < (lightConfig.lightCount * 3); j+=3)
		{
			// Yes I know this look clumbsy, but it ensures SPI moves quickly for 24bit color
  			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j], 0)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j], 1)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j], 2)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j], 3)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j], 4)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j], 5)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j], 6)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j], 7)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+1], 0)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+1], 1)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+1], 2)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+1], 3)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+1], 4)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+1], 5)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+1], 6)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+1], 7)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+2], 0)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+2], 1)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+2], 2)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+2], 3)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+2], 4)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+2], 5)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+2], 6)]);
			SSIDataPut(SSI0_BASE, WS8212_encode[CHECK_BIT(lightData[j+2], 7)]);
		}
    }
    else if (icType == WS8211) // WS8211 or WS8212 Type
    {
		// Send the bits out using SPI
		for (j = 0; j < (lightConfig.lightCount * 3); j+=3)
		{
			// Yes I know this look clumbsy, but it ensures SPI moves quickly for a 24bit color
  			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j], 0)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j], 1)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j], 2)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j], 3)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j], 4)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j], 5)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j], 6)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j], 7)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+1], 0)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+1], 1)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+1], 2)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+1], 3)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+1], 4)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+1], 5)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+1], 6)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+1], 7)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+2], 0)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+2], 1)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+2], 2)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+2], 3)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+2], 4)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+2], 5)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+2], 6)]);
			SSIDataPut(SSI0_BASE, WS8211_encode[CHECK_BIT(lightData[j+2], 7)]);
			//SSIDataPut(SSI0_BASE, me_WS8211_encode[2]);
			//SSIDataPut(SSI0_BASE, me_WS8211_encode[2]);
		}
    }
    else if (icType == TM1809)  // TM1809 Type
    {
		// Send the bits out using SPI
		for (j = 0; j < (lightConfig.lightCount * 3); j+=3)
		{
			// Yes I know this look clumbsy, but it ensures SPI moves quickly for 24bit color
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j], 0)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j], 1)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j], 2)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j], 3)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j], 4)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j], 5)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j], 6)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j], 7)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+1], 0)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+1], 1)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+1], 2)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+1], 3)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+1], 4)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+1], 5)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+1], 6)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+1], 7)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+2], 0)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+2], 1)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+2], 2)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+2], 3)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+2], 4)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+2], 5)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+2], 6)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[CHECK_BIT(lightData[j+2], 7)]);
			SSIDataPut(SSI0_BASE, TM1809_encode[2]);
		}
    }
    else if (icType == SPARE)  // SPARE Type
    {
		// Send the bits out using SPI
//		for (j = 0; j < (lightConfig.lightCount * 3); j+=3)
//		{
//
//		}
    }
    else if ( icType == CYT3005) // Mancester Encoded Type - CYT3005/Minleon
    {
    	lightConfig.byteCount = abs(((19+(39*lightConfig.lightCount))/8)+1);

		// Send the bits out using SPI
		for (j = 0; j < lightConfig.byteCount; j++)
		{
			// Convert 8bit word into 16bit manchester encoded word
			lightDataOutInt = manchester_encode[lightData[j]];
			SSIDataPut(SSI0_BASE, lightDataOutInt);
		}
    }

	return LS_SUCCESS;
}

//*****************************************************************************
//
//! resetLights
//!
//! This function sends reset signal to the lights
//!
//! \return None.
//
//*****************************************************************************
void resetLights()
{
	uint32_t i;

	uint16_t resetData[3];

    resetData[0] = 0b1010101010101010;
    resetData[1] = 0b1010101010101001;
    resetData[2] = 0b1001010000000000;

	// Invert the bits so Manchester Encoding works correctly.
//	for (i=0; i < 3; i++)
//	{
//		resetData[i] = ~resetData[i];
//	}

    // Read any residual data from the SSI port.
    while(SSIDataGetNonBlocking(SSI0_BASE, &masterRxBuffer[0]))
    {
    }

	// Send the bits out using SPI
	for (i = 0; i < 3; i++)
	{
		SSIDataPut(SSI0_BASE, resetData[i]);
	}
}


//*****************************************************************************
//
//! addressLights
//!
//! This function sends addressing signal to the lights
//!
//! \return None.
//
//*****************************************************************************
void addressLights()
{
	uint32_t i;

	uint16_t addressData[4];

    addressData[0] = 0b1010101010101010;
    addressData[1] = 0b1010101010101001;
    addressData[2] = 0b0101100101010101;
    addressData[3] = 0b0101010101010000;

    // Read any residual data from the SSI port.
    while(SSIDataGetNonBlocking(SSI0_BASE, &masterRxBuffer[0]))
    {
    }

	// Send the bits out using SPI
	for (i = 0; i < 4; i++)
	{
		SSIDataPut(SSI0_BASE, addressData[i]);
	}
}

