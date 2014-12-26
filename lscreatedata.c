//*****************************************************************************
//
// Application Name        - LightServer
// File Name			   - lscreatedata.c
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
// File Overview		   - lscreatedata.c contains functions to manipulate and
// 							 create lighting data for various types of LED chips
//
// Application Details     - https://github.com/remixed123/LightServer/readme.txt
//
//*****************************************************************************

#include "lightserver.h"
#include "lscreatedata.h"
#include "lsutils.h"

#include <stdbool.h>

//*****************************************************************************
// Gamma Correction Lookup Table
//*****************************************************************************
static const uint8_t gamma[] = {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
      1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,
      3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,
      7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12,
     13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
     20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
     30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
     42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
     58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
     76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
     97, 99,100,102,103,105,106,108,109,111,112,114,115,117,119,120,
    122,124,125,127,129,130,132,134,136,137,139,141,143,145,146,148,
    150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,
    182,184,186,188,191,193,195,197,199,202,204,206,209,211,213,215,
    218,220,223,225,227,230,232,235,237,240,242,245,247,250,252,255
};


//*****************************************************************************
//! performGammaCorrection
//!
//! This function performs a gamma correction the get the right color balance
//!
//! params: type: stripeCounter
//*****************************************************************************
lsresult_t performGammaCorrection(int stripeCounter)
{
	if (lightConfig.gammaCorrection == GAMMA_ON)
	{
		// This is the slow manual way to calculate gamma
//		colorData.red[stripeCounter] = (pow(((double)colorData.red[stripeCounter]/255),(double)(0.5)))*255;
//		colorData.green[stripeCounter] = (pow(((double)colorData.green[stripeCounter]/255),(double)(0.5)))*255;
//		colorData.blue[stripeCounter] = (pow(((double)colorData.blue[stripeCounter]/255),(double)(0.5)))*255;

		// This is the fast lookup table method to calculate gamma
		colorData.red[stripeCounter] = gamma[colorData.red[stripeCounter]];
		colorData.green[stripeCounter] = gamma[colorData.green[stripeCounter]];
		colorData.blue[stripeCounter] = gamma[colorData.blue[stripeCounter]];
	}

	//System_printf("8 Bit (Gamma): Stripe: %i redPart: %i greenPart: %d bluePart: %i\n",stripeCounter, colorData.red[stripeCounter], colorData.green[stripeCounter], colorData.blue[stripeCounter]);
	//System_flush();

	return LS_SUCCESS;
}

//*****************************************************************************
//! convertTo12Bit
//!
//! This function converts data to the correct bitcount for the LED chip
//!
//! params: type: stripeCounter
//*****************************************************************************
lsresult_t convertTo12Bit(int stripeCounter)
{
	uint8_t bitShift = lightConfig.bitCount - 8;
	// convert the 8 bit number into its 12 bit scaled equivalent
	colorData.red[stripeCounter] = colorData.red[stripeCounter] << bitShift | colorData.red[stripeCounter] >> bitShift;
	colorData.green[stripeCounter] = colorData.green[stripeCounter] << bitShift | colorData.green[stripeCounter] >> bitShift;
	colorData.blue[stripeCounter] = colorData.blue[stripeCounter] << bitShift | colorData.blue[stripeCounter] >> bitShift;

	//System_printf("12 Bit: Stripe: %i redPart: %i greenPart: %i bluePart: %i\n",stripeCounter, colorData.red[stripeCounter], colorData.green[stripeCounter], colorData.blue[stripeCounter]);
	//System_flush();

	return LS_SUCCESS;
}

//*****************************************************************************
//! convertToBitCount
//!
//! Calls the correct bit count conversion function
//!
//! params: type: stripeCounter
//*****************************************************************************
lsresult_t convertToBitCount(int bitCount, int stripeCounter)
{
	// First perform a gamma correction
	performGammaCorrection(stripeCounter);

	if (bitCount == 12)
	{
		convertTo12Bit(stripeCounter);
		return LS_SUCCESS;
	}
	else if (bitCount == 8)
	{
		// no conversion require, just return
		return LS_SUCCESS; //convertTo8Bit(stripeCounter);
	}

	return LS_BIT_COUNT_NOT_SUPPORTED;
}

///////////////////////////////////////////////////////
//  Creating final light data before sending out to SPI //
///////////////////////////////////////////////////////

//*****************************************************************************
//! createCyt305
//!
//! This function create the light packet for Minleon lights
//!
//! params: type: raw = 0 ; ddp control = 1
//*****************************************************************************

lsresult_t createCyt305(int packetType)
{
	int i, k;

	int shiftCounter;
	int arrayCounter;
	int stripeCounter;

	lightConfig.byteCount = abs(((19+(39*lightConfig.lightCount))/8)+1);

	//Initialise bits 0
	for (i=0; i < lightConfig.byteCount; i++)
	{
		lightData[i] = 0x0;
	}

	shiftCounter = 1;
	arrayCounter = 0;
	stripeCounter = 0;

	// Create the header portion of the packet
	for (i=0; i<15; i++)
	{
		lightData[arrayPosition(arrayCounter++)] |= one << bitPosition(shiftCounter++);
	}
	lightData[arrayPosition(arrayCounter++)] |= zero << bitPosition(shiftCounter++);
	lightData[arrayPosition(arrayCounter++)] |= zero << bitPosition(shiftCounter++);
	lightData[arrayPosition(arrayCounter++)] |= one << bitPosition(shiftCounter++);
	lightData[arrayPosition(arrayCounter++)] |= zero << bitPosition(shiftCounter++);

	// Create the data for the light portion of the the packet
	for (k = 0; k < lightConfig.lightCount; k++)
	{
		lightData[arrayPosition(arrayCounter++)] |= zero << bitPosition(shiftCounter++);
		for (i=11; i >= 0; i--)
		{
				lightData[arrayPosition(arrayCounter++)] |= (CHECK_BIT(colorData.red[stripeCounter], i)) << bitPosition(shiftCounter++);
				//System_printf("CHECK_BIT: lightNumber: %i colorData.red[%i]: bitNumber: %i bitValue: %i\n", k , stripeCounter, i, (CHECK_BIT(colorData.red[stripeCounter], i)) );
				//System_flush();
		}
		lightData[arrayPosition(arrayCounter++)] |= zero << bitPosition(shiftCounter++);
		for (i=11; i >= 0; i--)
		{
			lightData[arrayPosition(arrayCounter++)] |= (CHECK_BIT(colorData.blue[stripeCounter], i)) << bitPosition(shiftCounter++);
		}
		lightData[arrayPosition(arrayCounter++)] |= zero << bitPosition(shiftCounter++);
		for (i=11; i >= 0; i--)
		{
			lightData[arrayPosition(arrayCounter++)] |= (CHECK_BIT(colorData.green[stripeCounter], i)) << bitPosition(shiftCounter++);
		}

		// increment the stripe counter and reset to 0 if we have no more stripes
		stripeCounter++;

		if (stripeCounter == effectControl.colorCount && packetType == 1)
		{
			stripeCounter = 0;
		}
	}

	//Invert the bits so Manchester Encoding works correctly.
//	for (i=0; i < lightConfig.byteCount; i++)
//	{
//		lightData[i] = ~lightData[i];
//	}

	return LS_SUCCESS;
}

//*****************************************************************************
//! createStandard
//!
//! This function creates the light packet for WS8211/WS8212/TM1809/NeoPixels
//! This is the most common protocol/data frame that is used
//!
//! params: packetType: raw = 0 ; ddp control = 1
//! params: colorType: rgb = 0 ; rbg = 1 ; brg = 2 ; bgr = 3 ; gbr = 4 ; grb = 5
//*****************************************************************************
lsresult_t createStandard(int packetType, int colorType)
{
	int i, k;

	int stripeCounter;

	// Initialise bits 0
	for (i=0; i < (lightConfig.lightCount * 3); i++)
	{
		lightData[i] = 0x0;
	}

	stripeCounter = 0;

	// RGB
	if (colorType == RGB)
	{
		for (k = 0; k < (lightConfig.lightCount * 3); k+=3)
		{
			lightData[k] = colorData.red[stripeCounter];
			lightData[k+1] = colorData.green[stripeCounter];
			lightData[k+2] = colorData.blue[stripeCounter];

			stripeCounter++;

			if (stripeCounter == effectControl.colorCount && packetType == 1)
			{
				stripeCounter = 0;
			}
		}
	}
	// RBG
	else if (colorType == RBG)
	{
		for (k = 0; k < (lightConfig.lightCount * 3); k+=3)
		{
			lightData[k] = colorData.red[stripeCounter];
			lightData[k+1] = colorData.blue[stripeCounter];
			lightData[k+2] = colorData.green[stripeCounter];

			stripeCounter++;

			if (stripeCounter == effectControl.colorCount && packetType == 1)
			{
				stripeCounter = 0;
			}
		}
	}
	// BRG
	else if (colorType == BRG)
	{
		for (k = 0; k < (lightConfig.lightCount * 3); k+=3)
		{
			lightData[k] = colorData.blue[stripeCounter];
			lightData[k+1] = colorData.red[stripeCounter];
			lightData[k+2] = colorData.green[stripeCounter];

			stripeCounter++;

			if (stripeCounter == effectControl.colorCount && packetType == 1)
			{
				stripeCounter = 0;
			}
		}
	}
	// BGR
	else if (colorType == BGR)
	{
		for (k = 0; k < (lightConfig.lightCount * 3); k+=3)
		{
			lightData[k] = colorData.blue[stripeCounter];
			lightData[k+1] = colorData.green[stripeCounter];
			lightData[k+2] = colorData.red[stripeCounter];

			stripeCounter++;

			if (stripeCounter == effectControl.colorCount && packetType == 1)
			{
				stripeCounter = 0;
			}
		}
	}
	// GBR
	else if (colorType == GBR)
	{
		for (k = 0; k < (lightConfig.lightCount * 3); k+=3)
		{
			lightData[k] = colorData.green[stripeCounter];
			lightData[k+1] = colorData.blue[stripeCounter];
			lightData[k+2] = colorData.red[stripeCounter];

			stripeCounter++;

			if (stripeCounter == effectControl.colorCount && packetType == 1)
			{
				stripeCounter = 0;
			}
		}
	}
	// GRB
	else if (colorType == GRB)
	{
		for (k = 0; k < (lightConfig.lightCount * 3); k+=3)
		{
			lightData[k] = colorData.green[stripeCounter];
			lightData[k+1] = colorData.red[stripeCounter];
			lightData[k+2] = colorData.blue[stripeCounter];

			stripeCounter++;

			if (stripeCounter == effectControl.colorCount && packetType == 1)
			{
				stripeCounter = 0;
			}
		}
	}

	//reverse the bits LSB
	for (i=0; i < (lightConfig.lightCount * 3); i++)
	{
		lightData[i] = flip(lightData[i]);
	}

	return LS_SUCCESS;
}

//*****************************************************************************
//! createLightData
//!
//! This function selects the correct function to create the lightData for a specific type of light
//!
//! params: type: 0 = WS8211/WS8212/TM1809/NeoPixels ; 1 = CYT3005
//*****************************************************************************
lsresult_t createLightData(int protocolType, int packetType)
{

	if (protocolType == STANDARD)
	{
		createStandard(packetType, lightConfig.colorOrder);
	}
	else if (protocolType == MANCHESTER)
	{
		createCyt305(packetType);
	}

	return LS_SUCCESS;
}
