/*
 * lseffects.c
 *
 *  Created on: 30/09/2014
 *      Author: glenn
 */

#include "lightserver.h"
#include "lsutils.h"
#include "lscolors.h"
#include "lsspisend.h"
#include "lscreatedata.h"

/* Standard Header files */
#include <stdlib.h>

//*****************************************************************************
//! stationaryEffect
//!
//! This function creates a stationary effect (just a solid color)
//!
//! \return None.
//*****************************************************************************
void stationaryEffect()
{
	int i;
	int stripeCounter = 0;

	//Prepare the input light data for sending out to the lights.
	for (i = 0; i < effectControl.colorCount; i++)
	{
		colorData.red[stripeCounter] = (unsigned int)(colorSelected.red[i] * (double)effectControl.intensity/100);
		colorData.green[stripeCounter] = (unsigned int)(colorSelected.green[i] * (double)effectControl.intensity/100);
		colorData.blue[stripeCounter] = (unsigned int)(colorSelected.blue[i]  * (double)effectControl.intensity/100);

		//performGammaCorrection(stripeCounter);
		convertToBitCount(lightConfig.bitCount, stripeCounter);

		stripeCounter += 1;
	}

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

//*****************************************************************************
//! candleEffect
//!
//! This function creates a chase effect
//!
//! \return None.
//*****************************************************************************
void candleEffect()
{
	int i, k;
	int r;
	double strength;
	int stripeCounter;
	//int stripeCountInt;
	int uniqueCandles;

	k = 0;
	stripeCounter = 0;
	uniqueCandles = effectControl.tempColorCount; // assign the current colorCount to unique candles, so we can store the number of stripes

	// this ensures we do not have overlap of candle colors
	if (effectControl.tempColorCount % 2 == 0)
	{
		effectControl.colorCount = 8; // 2 and 4 divide into 8
	}
	else
	{
		effectControl.colorCount = 15; // 1 and 3 and 5 divide into 15
	}
	//stripeCountInt = effectControl.colorCount;

	//Prepare the input light data for sending out to the lights.
	for (i = 0; i < effectControl.colorCount; i++)
	{
		r = rand() % effectControl.tempSpeed; // The strength of the flicker is controlled via the speed slider

//		if (effectControl.direction == 0)
//		{
			strength = 1;
//		}
//		else
//		{
//			strength = 0.1;
//		}

		//Extract the individual colors from each candy cane and cast as an integer
		//colorData.red[stripeCounter] =  (unsigned int)(colorSelected.red[k] * (double)effectControl.intensity/100); //colorSelected.red[k];
		colorData.red[stripeCounter] =  (unsigned int)(colorSelected.red[k] * (double)effectControl.intensity/100 * (double)(100 - r)/100 * strength); //colorSelected.red[k];
		colorData.green[stripeCounter] = (unsigned int)(colorSelected.green[k] * (double)effectControl.intensity/100  * (double)(100 - r)/100  * strength); //colorSelected.green[k];
		colorData.blue[stripeCounter] =  (unsigned int)(colorSelected.blue[k]  * (double)effectControl.intensity/100  * (double)(100 - r)/100 * strength); //colorSelected.blue[k];

		//System_printf("8 Bit: Stripe: %i redPart: %i greenPart: %i bluePart: %i\n",stripeCounter, colorData.red[stripeCounter], colorData.green[stripeCounter], colorData.blue[stripeCounter]);
		//System_flush();

		//performGammaCorrection(stripeCounter);
		convertToBitCount(lightConfig.bitCount, stripeCounter);
		//convertTo12Bit(stripeCounter);

		stripeCounter += 1;
		k +=1;

		//reset the k counter to make sure we only use the colors/stripes that have been set
		if (k >= uniqueCandles)
		{
			k = 0;
		}
	}

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

//*****************************************************************************
//! chaseEffect
//!
//! This function creates a chase effect
//!
//! \return None.
//*****************************************************************************
void chaseEffect()
{
	int i, k;
	int stripeCounter;
	unsigned int stripeCountInt;

	stripeCounter = 0;
	stripeCountInt = effectControl.colorCount;
	k = 0;

	colorEffects.red[0] = colorSelected.red[effectControl.tempDirection];
	colorEffects.green[0] = colorSelected.green[effectControl.tempDirection];
	colorEffects.blue[0] = colorSelected.blue[effectControl.tempDirection];

	// Rotate the lights
	if (effectControl.direction == 0)
	{
		for (i = 0; i < effectControl.colorCount; i++)
		{
			if (i < (effectControl.colorCount - 1))
			{
				colorSelected.red[i] = colorSelected.red[i+1];
				colorSelected.green[i] = colorSelected.green[i+1];
				colorSelected.blue[i] = colorSelected.blue[i+1];
			}
			else
			{
				colorSelected.red[i] = colorEffects.red[0];
				colorSelected.green[i] = colorEffects.green[0];
				colorSelected.blue[i] = colorEffects.blue[0];
			}
		}
	}
	else
	{
		for (i = effectControl.tempDirection; i >= 0; i--)
		{
			if (i > 0)
			{
				colorSelected.red[i] = colorSelected.red[i-1];
				colorSelected.green[i] = colorSelected.green[i-1];
				colorSelected.blue[i] = colorSelected.blue[i-1];
			}
			else
			{
				colorSelected.red[i] = colorEffects.red[0];
				colorSelected.green[i] = colorEffects.green[0];
				colorSelected.blue[i] = colorEffects.blue[0];
			}
		}
	}
	//effectCounter++;

	//Prepare the input light data for sending out to the lights.
	for (i = 0; i < (stripeCountInt * 3); i+=3)
	{
		//Extract the individual colors from each candy cane and cast as an integer
		colorData.red[stripeCounter] =  (unsigned int)(colorSelected.red[k] * (double)effectControl.intensity/100); //colorSelected.red[k];
		colorData.green[stripeCounter] = (unsigned int)(colorSelected.green[k] * (double)effectControl.intensity/100); //colorSelected.green[k];
		colorData.blue[stripeCounter] =  (unsigned int)(colorSelected.blue[k]  * (double)effectControl.intensity/100); //colorSelected.blue[k];

		//System_printf("8 Bit: Stripe: %i redPart: %i greenPart: %i bluePart: %i\n",stripeCounter, colorData.red[stripeCounter], colorData.green[stripeCounter], colorData.blue[stripeCounter]);
		//System_flush();

		//performGammaCorrection(stripeCounter);
		convertToBitCount(lightConfig.bitCount, stripeCounter);

		stripeCounter += 1;
		k +=1;
	}

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

//*****************************************************************************
//! snakeEffect
//!
//! This function creates a snakes effect
//!
//! \return None.
//*****************************************************************************
void snakeEffect()
{
	int i, j, k;
	int stripeCounter;
	int reset;

	int cometTrail[12] = {100, 70, 55, 40, 30, 25, 20, 15, 13, 10, 10, 0};

	reset = 0;
	stripeCounter = 0;
	effectControl.colorCount = 12 * effectControl.actualColorCount;
	k = 0;

	//Prepare the input light data for sending out to the lights.
	for (j = 0; j < 12; j++)
	{
		for (i = 0; i < effectControl.actualColorCount; i++)
		{
			//Extract the individual colors from each candy cane and cast as an integer
			colorData.red[stripeCounter] =  (unsigned int)(colorSelected.red[i] * (double)effectControl.intensity/100 * (double)cometTrail[j]/100);
			colorData.green[stripeCounter] = (unsigned int)(colorSelected.green[i] * (double)effectControl.intensity/100 * (double)cometTrail[j]/100);
			colorData.blue[stripeCounter] =  (unsigned int)(colorSelected.blue[i]  * (double)effectControl.intensity/100 * (double)cometTrail[j]/100);

			convertToBitCount(lightConfig.bitCount, stripeCounter);
			stripeCounter++;
		}
	}

	for (i = 0; i < effectControl.colorCount; i++)
	{
		colorEffects.red[i] = colorData.red[i];
		colorEffects.green[i] = colorData.green[i];
		colorEffects.blue[i] = colorData.blue[i];
	}

	// Rotate the lights
	if (effectControl.direction == 0)
	{
		k = effectCalc.rotation;

		for (i = 0; i < effectControl.colorCount; i++)
		{
			j = i + effectCalc.rotation;

			if (j >= (effectControl.colorCount - 1) && reset == 0)
			{
				k = 0;
				reset = 1;
			}

			colorData.red[i] = colorEffects.red[k];
			colorData.green[i] = colorEffects.green[k];
			colorData.blue[i] = colorEffects.blue[k];

			k++;
		}
	}
	else
	{
		k = effectCalc.rotation;
		int l = 0;

		for (i = effectControl.colorCount; i >= 0; i--)
		{
			j = l + effectCalc.rotation;

			if (j >= (effectControl.colorCount - 1) && reset == 0)
			{
				k = 0;
				reset = 1;
			}

			colorData.red[i] = colorEffects.red[k];
			colorData.green[i] = colorEffects.green[k];
			colorData.blue[i] = colorEffects.blue[k];

			k++;
			l++;
		}
	}

	effectCalc.rotation++;
	if (effectCalc.rotation >= (effectControl.colorCount - 1))
	{
		effectCalc.rotation = 0;
	}

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

//*****************************************************************************
//! cometEffect
//!
//! This function creates a comet effect
//!
//! \return None.
//*****************************************************************************
void cometEffect()
{
	int i, j, k;
	int stripeCounter;
	int reset;

	int cometTrail[12] = {100, 55, 45, 35, 25, 20, 17, 15, 13, 11, 10, 0};

	reset = 0;
	stripeCounter = 0;
	effectControl.colorCount = 12 * effectControl.actualColorCount;
	k = 0;

	//Prepare the input light data for sending out to the lights.
	for (i = 0; i < effectControl.actualColorCount; i++)
	{
		for (j = 0; j < 12; j++)
		{
			//Extract the individual colors from each candy cane and cast as an integer
			colorData.red[stripeCounter] =  (unsigned int)(colorSelected.red[i] * (double)effectControl.intensity/100 * (double)cometTrail[j]/100);
			colorData.green[stripeCounter] = (unsigned int)(colorSelected.green[i] * (double)effectControl.intensity/100 * (double)cometTrail[j]/100);
			colorData.blue[stripeCounter] =  (unsigned int)(colorSelected.blue[i]  * (double)effectControl.intensity/100 * (double)cometTrail[j]/100);

			convertToBitCount(lightConfig.bitCount, stripeCounter);
			stripeCounter++;
		}
	}

	for (i = 0; i < effectControl.colorCount; i++)
	{
		colorEffects.red[i] = colorData.red[i];
		colorEffects.green[i] = colorData.green[i];
		colorEffects.blue[i] = colorData.blue[i];
	}

	// Rotate the lights
	if (effectControl.direction == 0)
	{
		k = effectCalc.rotation;

		for (i = 0; i < effectControl.colorCount; i++)
		{
			j = i + effectCalc.rotation;

			if (j >= (effectControl.colorCount - 1) && reset == 0)
			{
				k = 0;
				reset = 1;
			}

			colorData.red[i] = colorEffects.red[k];
			colorData.green[i] = colorEffects.green[k];
			colorData.blue[i] = colorEffects.blue[k];

			k++;
		}
	}
	else
	{
		k = effectCalc.rotation;
		int l = 0;

		for (i = effectControl.colorCount; i >= 0; i--)
		{
			j = l + effectCalc.rotation;

			if (j >= (effectControl.colorCount - 1) && reset == 0)
			{
				k = 0;
				reset = 1;
			}

			colorData.red[i] = colorEffects.red[k];
			colorData.green[i] = colorEffects.green[k];
			colorData.blue[i] = colorEffects.blue[k];

			k++;
			l++;
		}
	}

	effectCalc.rotation++;
	if (effectCalc.rotation >= (effectControl.colorCount - 1))
	{
		effectCalc.rotation = 0;
	}

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

//*****************************************************************************
//! wavesEffect
//!
//! This function creates a wave effect
//!
//! \return None.
//*****************************************************************************
void waveEffect()
{
	int i, j, k;
	int stripeCounter;
	int reset;

	int waveIntensity[8] = {10,20,40,100,100,40,20,10};

	reset = 0;
	stripeCounter = 0;
	effectControl.colorCount = 8 * effectControl.actualColorCount;
	k = 0;

	//Prepare the input light data for sending out to the lights.
	for (i = 0; i < effectControl.actualColorCount; i++)
	{
		for (j = 0; j < 8; j++)
		{
			//Extract the individual colors from each candy cane and cast as an integer
			colorData.red[stripeCounter] =  (unsigned int)(colorSelected.red[i] * (double)effectControl.intensity/100 * (double)waveIntensity[j]/100);
			colorData.green[stripeCounter] = (unsigned int)(colorSelected.green[i] * (double)effectControl.intensity/100 * (double)waveIntensity[j]/100);
			colorData.blue[stripeCounter] =  (unsigned int)(colorSelected.blue[i]  * (double)effectControl.intensity/100 * (double)waveIntensity[j]/100);

			convertToBitCount(lightConfig.bitCount, stripeCounter);
			stripeCounter++;
		}
	}


	for (i = 0; i < effectControl.colorCount; i++)
	{
		colorEffects.red[i] = colorData.red[i];
		colorEffects.green[i] = colorData.green[i];
		colorEffects.blue[i] = colorData.blue[i];
	}

	// Rotate the lights
	if (effectControl.direction == 0)
	{
		k = effectCalc.rotation;

		for (i = 0; i < effectControl.colorCount; i++)
		{
			j = i + effectCalc.rotation;

			if (j >= (effectControl.colorCount - 1) && reset == 0)
			{
				k = 0;
				reset = 1;
			}

			colorData.red[i] = colorEffects.red[k];
			colorData.green[i] = colorEffects.green[k];
			colorData.blue[i] = colorEffects.blue[k];

			k++;
		}
	}
	else
	{
		k = effectCalc.rotation;
		int l = 0;

		for (i = effectControl.colorCount; i >= 0; i--)
		{
			j = l + effectCalc.rotation;

			if (j >= (effectControl.colorCount - 1) && reset == 0)
			{
				k = 0;
				reset = 1;
			}

			colorData.red[i] = colorEffects.red[k];
			colorData.green[i] = colorEffects.green[k];
			colorData.blue[i] = colorEffects.blue[k];

			k++;
			l++;
		}
	}

	effectCalc.rotation++;
	if (effectCalc.rotation >= (effectControl.colorCount - 1))
	{
		effectCalc.rotation = 0;
	}

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

//*****************************************************************************
//! antsEffect
//!
//! This function creates a ant effect
//!
//! \return None.
//*****************************************************************************
void antsEffect()
{
	int i, j, k;
	int stripeCounter;
	int reset;

	int raceIntensity[3] = {100,50,10};

	reset = 0;
	stripeCounter = 0;
	effectControl.colorCount = 20 * effectControl.actualColorCount;
	k = 0;

	//Prepare the input light data for sending out to the lights.
	for (i = 0; i < effectControl.actualColorCount; i++)
	{
		for (j = 0; j < 20; j++)
		{
			if (j < 3)
			{
			//Extract the individual colors from each candy cane and cast as an integer
			colorData.red[stripeCounter] =  (unsigned int)(colorSelected.red[i] * (double)effectControl.intensity/100 * (double)raceIntensity[j]/100);
			colorData.green[stripeCounter] = (unsigned int)(colorSelected.green[i] * (double)effectControl.intensity/100 * (double)raceIntensity[j]/100);
			colorData.blue[stripeCounter] =  (unsigned int)(colorSelected.blue[i]  * (double)effectControl.intensity/100 * (double)raceIntensity[j]/100);
			}
			else
			{
				colorData.red[stripeCounter] =  0x00;
				colorData.green[stripeCounter] = 0x00;
				colorData.blue[stripeCounter] =  0x00;
			}

			convertToBitCount(lightConfig.bitCount, stripeCounter);
			stripeCounter++;
		}
	}


	for (i = 0; i < effectControl.colorCount; i++)
	{
		colorEffects.red[i] = colorData.red[i];
		colorEffects.green[i] = colorData.green[i];
		colorEffects.blue[i] = colorData.blue[i];
	}

	// Rotate the lights
	if (effectControl.direction == 0)
	{
		k = effectCalc.rotation;

		for (i = 0; i < effectControl.colorCount; i++)
		{
			j = i + effectCalc.rotation;

			if (j >= (effectControl.colorCount - 1) && reset == 0)
			{
				k = 0;
				reset = 1;
			}

			colorData.red[i] = colorEffects.red[k];
			colorData.green[i] = colorEffects.green[k];
			colorData.blue[i] = colorEffects.blue[k];

			k++;
		}
	}
	else
	{
		k = effectCalc.rotation;
		int l = 0;

		for (i = effectControl.colorCount; i >= 0; i--)
		{
			j = l + effectCalc.rotation;

			if (j >= (effectControl.colorCount - 1) && reset == 0)
			{
				k = 0;
				reset = 1;
			}

			colorData.red[i] = colorEffects.red[k];
			colorData.green[i] = colorEffects.green[k];
			colorData.blue[i] = colorEffects.blue[k];

			k++;
			l++;
		}
	}

	effectCalc.rotation++;
	if (effectCalc.rotation >= (effectControl.colorCount - 1))
	{
		effectCalc.rotation = 0;
	}

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

//*****************************************************************************
//! paintEffect
//!
//! This function creates a paint effect
//!
//! \return None.
//*****************************************************************************
void paintEffect()
{
	int j;
	int colorIndex;

	effectControl.colorCount = lightConfig.lightCount;

	//Prepare the input light data for sending out to the lights.
	if (effectControl.direction == 0)
	{
		for (j = 0; j < lightConfig.lightCount; j++)
		{
			if (j < effectCalc.light)
			{
				//Extract the individual colors from each candy cane and cast as an integer
				colorData.red[j] =  (unsigned int)(colorSelected.red[effectCalc.rotation] * (double)effectControl.intensity/100);
				colorData.green[j] = (unsigned int)(colorSelected.green[effectCalc.rotation] * (double)effectControl.intensity/100);
				colorData.blue[j] =  (unsigned int)(colorSelected.blue[effectCalc.rotation]  * (double)effectControl.intensity/100);
			}
			else
			{
				if (effectCalc.rotation == 0)
				{
					colorIndex = effectControl.actualColorCount - 1;
				}
				else
				{
					colorIndex = effectCalc.rotation - 1;
				}

				colorData.red[j] =  (unsigned int)(colorSelected.red[colorIndex] * (double)effectControl.intensity/100);
				colorData.green[j] = (unsigned int)(colorSelected.green[colorIndex] * (double)effectControl.intensity/100);
				colorData.blue[j] =  (unsigned int)(colorSelected.blue[colorIndex]  * (double)effectControl.intensity/100);
			}

			convertToBitCount(lightConfig.bitCount, j);
		}
	}
	else
	{
		for (j = lightConfig.lightCount - 1; j >= 0; j--)
		{
			if (j > (lightConfig.lightCount - effectCalc.light - 1))
			{
				//Extract the individual colors from each candy cane and cast as an integer
				colorData.red[j] =  (unsigned int)(colorSelected.red[effectCalc.rotation] * (double)effectControl.intensity/100);
				colorData.green[j] = (unsigned int)(colorSelected.green[effectCalc.rotation] * (double)effectControl.intensity/100);
				colorData.blue[j] =  (unsigned int)(colorSelected.blue[effectCalc.rotation]  * (double)effectControl.intensity/100);
			}
			else
			{
				if (effectCalc.rotation == 0)
				{
					colorIndex = effectControl.actualColorCount - 1;
				}
				else
				{
					colorIndex = effectCalc.rotation - 1;
				}

				colorData.red[j] =  (unsigned int)(colorSelected.red[colorIndex] * (double)effectControl.intensity/100);
				colorData.green[j] = (unsigned int)(colorSelected.green[colorIndex] * (double)effectControl.intensity/100);
				colorData.blue[j] =  (unsigned int)(colorSelected.blue[colorIndex]  * (double)effectControl.intensity/100);
			}

			convertToBitCount(lightConfig.bitCount, j);
		}
	}


	effectCalc.light++;
	if (effectCalc.light >= lightConfig.lightCount)
	{
		effectCalc.light = 0;

		effectCalc.rotation++;
		if (effectCalc.rotation >= effectControl.actualColorCount)
		{
			effectCalc.rotation = 0;
		}
	}

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}


//*****************************************************************************
//! strobeEffect
//!
//! This function creates a strobe effect
//!
//! \return None.
//*****************************************************************************
void strobeEffect()
{
	int stripeCounter;

	stripeCounter = 0;

	//Prepare the input light data for sending out to the lights.
	colorData.red[stripeCounter] =  (unsigned int)(colorSelected.red[effectControl.tempDirection] * (double)effectControl.intensity/100);
	colorData.green[stripeCounter] = (unsigned int)(colorSelected.green[effectControl.tempDirection] * (double)effectControl.intensity/100);
	colorData.blue[stripeCounter] =  (unsigned int)(colorSelected.blue[effectControl.tempDirection]  * (double)effectControl.intensity/100);

	//performGammaCorrection(stripeCounter);
	convertToBitCount(lightConfig.bitCount, stripeCounter);

	if (effectControl.direction == 0)
	{
		effectControl.tempDirection += 1;

		//reset the counter to make sure we only use the colors/stripes that have been set
		if (effectControl.tempDirection >= effectControl.tempColorCount)
		{
			effectControl.tempDirection = 0;
		}
	}
	else
	{
		//reset the counter to make sure we only use the colors/stripes that have been set
		if ((effectControl.tempDirection - 1) < 0)
		{
			effectControl.tempDirection = effectControl.actualColorCount;
		}

		effectControl.tempDirection -= 1;
	}

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}


//*****************************************************************************
//! twinkleEffect
//!
//! This function creates a twinkle effect
//!
//! \return None.
//*****************************************************************************
void twinkleEffect()
{
	int i, k;
	int r;
	int twinkleAmount;
	int stripeCounter;
	int uniqueCandles;

	k = 0;
	stripeCounter = 0;
	uniqueCandles = effectControl.tempColorCount; // assign the current colorCount to unique candles, so we can store the number of stripes

	// this ensures we do not have overlap of candle colors
	effectControl.colorCount = 25; // 2 and 4 divide into 16

	//Prepare the input light data for sending out to the lights.
	for (i = 0; i < effectControl.colorCount; i++)
	{
		if (effectControl.direction == 0)
		{
			r = rand() % 105;
			twinkleAmount = effectControl.tempSpeed - r; // The amount of twinkling is controlled via the speed slider

			if (twinkleAmount < 0)
			{
				twinkleAmount = 0;
			}
			else
			{
				twinkleAmount = 1;
			}
		}
		else
		{
			r = rand() % 350;
			twinkleAmount = effectControl.tempSpeed - r; // The amount of twinkling is controlled via the speed slider

			if (twinkleAmount < 0)
			{
				twinkleAmount = 0;
			}
			else
			{
				twinkleAmount = 1;
			}
		}

		//Extract the individual colors from each candy cane and cast as an integer
		colorData.red[stripeCounter] =  (unsigned int)(colorMix.red[k] * (double)effectControl.intensity/100 * (double)(twinkleAmount)); //colorSelected.red[k];
		colorData.green[stripeCounter] = (unsigned int)(colorMix.green[k] * (double)effectControl.intensity/100  * (double)(twinkleAmount)); //colorSelected.green[k];
		colorData.blue[stripeCounter] =  (unsigned int)(colorMix.blue[k]  * (double)effectControl.intensity/100  * (double)(twinkleAmount)); //colorSelected.blue[k];

		//System_printf("8 Bit: Stripe: %i redPart: %i greenPart: %i bluePart: %i\n",stripeCounter, colorData.red[stripeCounter], colorData.green[stripeCounter], colorData.blue[stripeCounter]);
		//System_flush();

		//performGammaCorrection(stripeCounter);
		convertToBitCount(lightConfig.bitCount, stripeCounter);

		stripeCounter += 1;
		k +=1;

		//reset the k counter to make sure we only use the colors/stripes that have been set
		if (k >= uniqueCandles)
		{
			k = 0;
		}
	}


	if (effectControl.randomColor == 0)
	{
		for (i = 0; i < effectControl.colorCount; i++)
		{
			k =  randRange(0, (effectControl.actualColorCount - 1));
			colorMix.red[i] = colorSelected.red[k];
			colorMix.green[i] = colorSelected.green[k];
			colorMix.blue[i] = colorSelected.blue[k];
		}
	}
	else // generate new random colors
	{
		for (i = 0; i < effectControl.colorCount; i++)
		{
			createRandomColorHSV(i,1);
		}
	}

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

//*****************************************************************************
//! wavesEffect
//!
//! This function creates a wave effect
//!
//! \return None.
//*****************************************************************************
void pulseEffect()
{
	int i, j;
	int stripeCounter;

	stripeCounter = 0;
	effectControl.colorCount = effectControl.actualColorCount;
	i = 0;

	// Change direction of wave
	if (effectControl.tempDirection != effectControl.direction)
	{
		if (effectCalc.glowDirection[0] == 1)
			effectCalc.glowDirection[0] = 0;
		else
			effectCalc.glowDirection[0] = 1;
	}

	//Prepare the input light data for sending out to the lights.
	for (j = 0; j < 5; j++)
	{
		//Extract the individual colors from each candy cane and cast as an integer
		colorData.red[stripeCounter] =  (unsigned int)(colorSelected.red[i] * (double)effectControl.intensity/100 * (double)effectCalc.glow[0]/250);
		colorData.green[stripeCounter] = (unsigned int)(colorSelected.green[i] * (double)effectControl.intensity/100 * (double)effectCalc.glow[0]/250);
		colorData.blue[stripeCounter] =  (unsigned int)(colorSelected.blue[i]  * (double)effectControl.intensity/100 * (double)effectCalc.glow[0]/250);

		convertToBitCount(lightConfig.bitCount, stripeCounter);
		stripeCounter++;

		// Do we have another color to select from
		if (i >= (effectControl.actualColorCount - 1))
		{
			i = 0;
		}
		else
		{
			i++;
		}

		// Change the direction of the glow if needed
		if (effectCalc.glow[0] >= 250)
		{
			effectCalc.glowDirection[0] = 0;
		}
		else if (effectCalc.glow[0] <= 100)
		{
			effectCalc.glowDirection[0] = 1;
		}

		// Increase of decrease the intensity of the light
		if (effectCalc.glowDirection[0] == 1)
		{
			effectCalc.glow[0] = effectCalc.glow[0] + 1;
		}
		else
		{
			effectCalc.glow[0] = effectCalc.glow[0] - 1;
		}
	}

	effectControl.tempDirection = effectControl.direction;

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

//*****************************************************************************
//! glowWaveEffect
//!
//! This function creates a wave effect
//!
//! \return None.
//*****************************************************************************
void glowEffect()
{
	int i, j;
	int stripeCounter;

	stripeCounter = 0;
	effectControl.colorCount = 24;
	i = 0;

	// Change direction of wave
	if (effectControl.tempDirection != effectControl.direction)
	{
		for (i = 0; i < 10; i++)
		{
			if (effectCalc.glowDirection[i] == 1)
				effectCalc.glowDirection[i] = 0;
			else
				effectCalc.glowDirection[i] = 1;
		}
	}

	//Prepare the input light data for sending out to the lights.
	for (j = 0; j < 24; j++)
	{
		//Extract the individual colors from each candy cane and cast as an integer
		colorData.red[stripeCounter] =  (unsigned int)(colorSelected.red[i] * (double)effectControl.intensity/100 * (double)effectCalc.glow[j]/250);
		colorData.green[stripeCounter] = (unsigned int)(colorSelected.green[i] * (double)effectControl.intensity/100 * (double)effectCalc.glow[j]/250);
		colorData.blue[stripeCounter] =  (unsigned int)(colorSelected.blue[i]  * (double)effectControl.intensity/100 * (double)effectCalc.glow[j]/250);

		convertToBitCount(lightConfig.bitCount, stripeCounter);
		stripeCounter++;

		// Do we have another color to select from
		if (i >= (effectControl.actualColorCount - 1))
		{
			i = 0;
		}
		else
		{
			i++;
		}

		// Change the direction of the glow if needed
		if (effectCalc.glow[j] >= 249)
		{
			effectCalc.glowDirection[j] = 0;
		}
		else if (effectCalc.glow[j] <= 50)
		{
			effectCalc.glowDirection[j] = 1;
		}

		// Increase or decrease the intensity of the light
		if (effectCalc.glowDirection[j] == 1)
		{
			effectCalc.glow[j] = effectCalc.glow[j] + 2;
		}
		else
		{
			effectCalc.glow[j] = effectCalc.glow[j] - 2;
		}
	}

	effectControl.tempDirection = effectControl.direction;

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

//*****************************************************************************
//! glowMixEffect
//!
//! This function creates a wave effect
//!
//! \return None.
//*****************************************************************************
void glowMixEffect()
{
	int i, j, k;
	int stripeCounter;

	stripeCounter = 0;
	effectControl.colorCount = 25;
	i = 0;

	// Change direction of wave
	if (effectControl.tempDirection != effectControl.direction)
	{
		for (i = 0; i < 10; i++)
		{
			if (effectCalc.glowDirection[i] == 1)
				effectCalc.glowDirection[i] = 0;
			else
				effectCalc.glowDirection[i] = 1;
		}
	}

	//Prepare the input light data for sending out to the lights.
	for (j = 0; j < 25; j++)
	{
		//Extract the individual colors from each candy cane and cast as an integer
		colorData.red[stripeCounter] =  (unsigned int)(colorMix.red[j] * (double)effectControl.intensity/100 * (double)effectCalc.glow[j]/250);
		colorData.green[stripeCounter] = (unsigned int)(colorMix.green[j] * (double)effectControl.intensity/100 * (double)effectCalc.glow[j]/250);
		colorData.blue[stripeCounter] =  (unsigned int)(colorMix.blue[j]  * (double)effectControl.intensity/100 * (double)effectCalc.glow[j]/250);

		convertToBitCount(lightConfig.bitCount, stripeCounter);
		stripeCounter++;

		// Change the direction of the glow if needed
		if (effectCalc.glow[j] >= 248)
		{
			effectCalc.glowDirection[j] = 0;
		}
		else if (effectCalc.glow[j] <= 2)
		{
			effectCalc.glowDirection[j] = 1;
			if (effectControl.randomColor == 0)
			{
				k =  randRange(0, (effectControl.actualColorCount - 1));
				colorMix.red[j] = colorSelected.red[k];
				colorMix.green[j] = colorSelected.green[k];
				colorMix.blue[j] = colorSelected.blue[k];
			}
			else // generate a new random color
			{
				createRandomColorHSV(j,1);
			}
		}

		// Increase or decrease the intensity of the light
		if (effectCalc.glowDirection[j] == 1)
		{
			effectCalc.glow[j] = effectCalc.glow[j] + 2;
		}
		else
		{
			effectCalc.glow[j] = effectCalc.glow[j] - 2;
		}
	}

	effectControl.tempDirection = effectControl.direction;

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

//*****************************************************************************
//! blendMixEffect
//!
//! This function creates a wave effect
//!
//! \return None.
//*****************************************************************************
void blendMixEffect()
{
	int j;
	int stripeCounter;
	int colorIndex;

	stripeCounter = 0;
	effectControl.colorCount = effectControl.actualColorCount;

	// Change direction of blend
	if (effectControl.tempDirection != effectControl.direction)
	{
		if (effectCalc.blendDirection == 1)
			effectCalc.blendDirection = 0;
		else
			effectCalc.blendDirection = 1;
	}

	if (effectControl.actualColorCount == 1)
	{
		colorSelected.red[1] = colorSelected.red[0];
		colorSelected.green[1] = colorSelected.green[0];
		colorSelected.blue[1] = colorSelected.blue[0];
	}

	//Prepare the input light data for sending out to the lights.
	for (j = 0; j < effectControl.actualColorCount; j++)
	{
		//Determine the next color, loop to the first color if we are on the last color
		if (j == (effectControl.actualColorCount - 1))
		{
			colorIndex = 0;
		}
		else
		{
			colorIndex = j + 1;
		}

		//Extract the individual colors from each candy cane and cast as an integer
		colorData.red[stripeCounter] =  (unsigned int)(((colorSelected.red[j] * (double)effectCalc.blend/250) + (colorSelected.red[colorIndex] * (double)(250 - effectCalc.blend)/250)) * (double)effectControl.intensity/100);
		colorData.green[stripeCounter] = (unsigned int)(((colorSelected.green[j] * (double)effectCalc.blend/250) + (colorSelected.green[colorIndex] * (double)(250 - effectCalc.blend)/250)) * (double)effectControl.intensity/100);
		colorData.blue[stripeCounter] =  (unsigned int)(((colorSelected.blue[j]  * (double)effectCalc.blend/250) + (colorSelected.blue[colorIndex] * (double)(250 - effectCalc.blend)/250)) * (double)effectControl.intensity/100);

		convertToBitCount(lightConfig.bitCount, stripeCounter);
		stripeCounter++;

//		// Do we have another color to select from
//		if (i >= (effectControl.actualColorCount - 1))
//		{
//			i = 0;
//		}
//		else
//		{
//			i++;
//		}

		// Change the direction of the blend if needed
		if (effectCalc.blend >= 248)
		{
			effectCalc.blendDirection = 0;
		}
		else if (effectCalc.blend <= 2)
		{
			effectCalc.blendDirection = 1;
		}

		// Increase or decrease the intensity of the light
		if (effectCalc.blendDirection == 1)
		{
			effectCalc.blend = effectCalc.blend + 2;
		}
		else
		{
			effectCalc.blend = effectCalc.blend - 2;
		}
	}

	effectControl.tempDirection = effectControl.direction;

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

//*****************************************************************************
//! blendMixMultiEffect
//!
//! This function creates a wave effect
//!
//! \return None.
//*****************************************************************************
void blendMixMultiEffect()
{
	int j;
	int stripeCounter;
	int colorIndex;

	stripeCounter = 0;
	effectControl.colorCount = 13;

	// Change direction of the blend
	if (effectControl.tempDirection != effectControl.direction)
	{
		if (effectCalc.blendDirection == 1)
			effectCalc.blendDirection = 0;
		else
			effectCalc.blendDirection = 1;
	}

//	if (effectControl.actualColorCount == 1)
//	{
//		colorSelected.red[1] = colorSelected.red[0];
//		colorSelected.green[1] = colorSelected.green[0];
//		colorSelected.blue[1] = colorSelected.blue[0];
//	}

	//Prepare the input light data for sending out to the lights.
	for (j = 0; j < 25; j+=2)
	{
		//Determine the next color, loop to the first color if we are on the last color
		if (j == 24)
		{
			colorIndex = 0;
		}
		else
		{
			colorIndex = j + 1;
		}

		//Extract the individual colors from each candy cane and cast as an integer
		colorData.red[stripeCounter] =  (unsigned int)(((colorMix.red[j] * (double)effectCalc.blend/250) + (colorMix.red[colorIndex] * (double)(250 - effectCalc.blend)/250)) * (double)effectControl.intensity/100);
		colorData.green[stripeCounter] = (unsigned int)(((colorMix.green[j] * (double)effectCalc.blend/250) + (colorMix.green[colorIndex] * (double)(250 - effectCalc.blend)/250)) * (double)effectControl.intensity/100);
		colorData.blue[stripeCounter] =  (unsigned int)(((colorMix.blue[j]  * (double)effectCalc.blend/250) + (colorMix.blue[colorIndex] * (double)(250 - effectCalc.blend)/250)) * (double)effectControl.intensity/100);

		convertToBitCount(lightConfig.bitCount, stripeCounter);
		stripeCounter++;

//		// Do we have another color to select from
//		if (i >= (effectControl.actualColorCount - 1))
//		{
//			i = 0;
//		}
//		else
//		{
//			i++;
//		}

		// Change the direction of the blend if needed
		if (effectCalc.blend >= 248)
		{
			effectCalc.blendDirection = 0;
			createRandomColorHSV(colorIndex,1);
		}
		else if (effectCalc.blend <= 2)
		{
			effectCalc.blendDirection = 1;
			createRandomColorHSV(j,1);
		}

		// Increase or decrease the intensity of the light
		if (effectCalc.blendDirection == 1)
		{
			effectCalc.blend = effectCalc.blend + 2;
		}
		else
		{
			effectCalc.blend = effectCalc.blend - 2;
		}
	}

	effectControl.tempDirection = effectControl.direction;

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

//*****************************************************************************
//! colorBlendEffect
//!
//! This function creates a wave effect
//!
//! \return None.
//*****************************************************************************
void blendEffect()
{
	int stripeCounter;
	int colorIndex;

	stripeCounter = 0;
	effectControl.colorCount = 1;

	// Change direction of wave
	if (effectControl.tempDirection != effectControl.direction)
	{
		if (effectCalc.blendDirection == 1)
			effectCalc.blendDirection = 0;
		else
			effectCalc.blendDirection = 1;
	}

	if (effectControl.actualColorCount == 1)
	{
		colorSelected.red[1] = colorSelected.red[0];
		colorSelected.green[1] = colorSelected.green[0];
		colorSelected.blue[1] = colorSelected.blue[0];
	}

	if (effectCalc.blendDirection == 1)
	{
		//Determine the next color, loop to the first color if we are on the last color
		if (effectCalc.rotation == (effectControl.actualColorCount - 1))
		{
			colorIndex = 0;
		}
		else
		{
			colorIndex = effectCalc.rotation + 1;
		}
	}
	else
	{
		//Determine the next color, loop to the first color if we are on the last color
		if (effectCalc.rotation == 0)
		{
			colorIndex = effectControl.actualColorCount - 1;
		}
		else
		{
			colorIndex = effectCalc.rotation - 1;
		}
	}

	// If random, generate a new color
	if (effectControl.randomColor == 1 && effectCalc.blend <= 2)
	{
		createRandomColorHSV(colorIndex,0);
	}

	//Extract the individual colors from each candy cane and cast as an integer
	colorData.red[stripeCounter] =  (unsigned int)(((colorSelected.red[effectCalc.rotation] * (double)(250 - effectCalc.blend)/250) + (colorSelected.red[colorIndex] * (double)(effectCalc.blend)/250)) * (double)effectControl.intensity/100);
	colorData.green[stripeCounter] = (unsigned int)(((colorSelected.green[effectCalc.rotation] * (double)(250 - effectCalc.blend)/250) + (colorSelected.green[colorIndex] * (double)(effectCalc.blend)/250)) * (double)effectControl.intensity/100);
	colorData.blue[stripeCounter] =  (unsigned int)(((colorSelected.blue[effectCalc.rotation]  * (double)(250 - effectCalc.blend)/250) + (colorSelected.blue[colorIndex] * (double)(effectCalc.blend)/250)) * (double)effectControl.intensity/100);

	convertToBitCount(lightConfig.bitCount, stripeCounter);
	stripeCounter++;

	// Change the direction of the blend if needed
	if (effectCalc.blend >= 248)
	{
		//effectCalc.blendDirection = 0;
		effectCalc.blend = 0;

		if (effectCalc.blendDirection == 1)
		{
			effectCalc.rotation++;
			if (effectCalc.rotation >= effectControl.actualColorCount)
			{
				effectCalc.rotation = 0;
			}
		}
		else
		{
			effectCalc.rotation--;
			if (effectCalc.rotation < 0)
			{
				effectCalc.rotation = effectControl.actualColorCount - 1;
			}
		}
	}

	effectCalc.blend = effectCalc.blend + 2;

	effectControl.tempDirection = effectControl.direction;

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}


//*****************************************************************************
//! glowWaveEffect
//!
//! This function creates a wave effect
//!
//! \return None.
//*****************************************************************************
void glowWaveEffect()
{
	int i, j;
	int stripeCounter;

	stripeCounter = 0;
	effectControl.colorCount = 11;
	i = 0;

	// Change direction of wave
	if (effectControl.tempDirection != effectControl.direction)
	{
		for (i = 0; i < 10; i++)
		{
			if (effectCalc.intensityDirection[i] == 1)
				effectCalc.intensityDirection[i] = 0;
			else
				effectCalc.intensityDirection[i] = 1;
		}
	}

	//Prepare the input light data for sending out to the lights.
	for (j = 0; j < 11; j++)
	{
		//Extract the individual colors from each candy cane and cast as an integer
		colorData.red[stripeCounter] =  (unsigned int)(colorSelected.red[i] * (double)effectControl.intensity/100 * (double)effectCalc.intensity[j]/250);
		colorData.green[stripeCounter] = (unsigned int)(colorSelected.green[i] * (double)effectControl.intensity/100 * (double)effectCalc.intensity[j]/250);
		colorData.blue[stripeCounter] =  (unsigned int)(colorSelected.blue[i]  * (double)effectControl.intensity/100 * (double)effectCalc.intensity[j]/250);

		convertToBitCount(lightConfig.bitCount, stripeCounter);
		stripeCounter++;

		// Do we have another color to select from
		if (i >= (effectControl.actualColorCount - 1))
		{
			i = 0;
		}
		else
		{
			i++;
		}

		// Change the direction of the glow if needed
		if (effectCalc.intensity[j] >= 246)
		{
			effectCalc.intensityDirection[j] = 0;
		}
		else if (effectCalc.intensity[j] <= 50)
		{
			effectCalc.intensityDirection[j] = 1;
		}

		// Increase of decrease the intensity of the light
		if (effectCalc.intensityDirection[j] == 1)
		{
			effectCalc.intensity[j] = effectCalc.intensity[j] + 5;
		}
		else
		{
			effectCalc.intensity[j] = effectCalc.intensity[j] - 5;
		}
	}

	effectControl.tempDirection = effectControl.direction;

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}



///////////////////////////////////////////////////////////////////////////////
// Effect Algorithms - With No Color Setting Options
///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//! colorWheelCalc
//!
//! This function calculates the correct color from the color wheel
//!
//! \return None.
//*****************************************************************************
void colorWheelCalc(uint8_t position, int stripeCounter)
{
	//uint16_t returnColor;
	if (position < 85)
	{
		colorData.red[stripeCounter] = (unsigned int)((position * 3) * (double)effectControl.intensity/100);
		colorData.green[stripeCounter] = (unsigned int)((255 - position * 3) * (double)effectControl.intensity/100);
		colorData.blue[stripeCounter] = 0;
	}
	else if (position < 170)
	{
		position -=85;
		colorData.red[stripeCounter] =  (unsigned int)((255 - position * 3) * (double)effectControl.intensity/100);
		colorData.green[stripeCounter] = 0;
		colorData.blue[stripeCounter] = (unsigned int)((position * 3) * (double)effectControl.intensity/100);
	}
	else
	{
		position -= 170;
		colorData.red[stripeCounter] =  0;
		colorData.green[stripeCounter] = (unsigned int)((position * 3) * (double)effectControl.intensity/100);
		colorData.blue[stripeCounter] = (unsigned int)((255 - position * 3) * (double)effectControl.intensity/100);
	}
}

//*****************************************************************************
//! rainbowEffect
//!
//! This function creates a rainbow effect
//!
//! \return None.
//*****************************************************************************
void colorWheelEffect()
{
	int j;
	int stripeCounter;

	stripeCounter = 0;
	for (j = 0; j < lightConfig.lightCount; j++)
	{
		colorWheelCalc(effectCalc.rotation, stripeCounter);
		convertToBitCount(lightConfig.bitCount, stripeCounter);
		stripeCounter++;
	}

	effectCalc.rotation = effectCalc.rotation + 2;
	if (effectCalc.rotation >= 255)
	{
		effectCalc.rotation = 0;
	}

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

//*****************************************************************************
//! rainbowEffect
//!
//! This function creates a rainbow effect
//!
//! \return None.
//*****************************************************************************
void rainbowEffect()
{
	int j;
	int stripeCounter;

	//Prepare the input light data for sending out to the lights.
	if (effectControl.direction == 0)
	{
		stripeCounter = 0;
	}
	else
	{
		stripeCounter = lightConfig.lightCount;
	}

	for (j = 0; j < lightConfig.lightCount; j++)
	{
		//colorWheelCalc((effectCalc.rotation+j)%255, stripeCounter);
		colorWheelCalc((effectCalc.rotation+j)%255, stripeCounter);
		convertToBitCount(lightConfig.bitCount, stripeCounter);

		if (effectControl.direction == 0)
		{
			stripeCounter++;
		}
		else
		{
			stripeCounter--;
		}
	}


	effectCalc.rotation = effectCalc.rotation + 3;
	if (effectCalc.rotation >= 255)
	{
		effectCalc.rotation = 0;
	}

	createLightData(lightConfig.protocolType, CONTROL);
	sendSpi(lightConfig.icType);
}

///////////////////////////////////////////////////////////////////////////////
// Raw Data Conversion
///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//! rawDataConvert
//!
//! This function converts raw DDP into light data
//!
//! \return None.
//*****************************************************************************
void rawDataConvert()
{
	int i;
	int stripeCounter = 0;
	unsigned char * eightBitColorsHex;

	eightBitColorsHex = rawData; //hexStringToBytes(rawData, (3 * lightConfig.lightCount));

	//Prepare the input light data for sending out to the lights.
	for (i = 0; i < (lightConfig.lightCount * 3); i+=3)
	{
		//Extract the individual colors from each candy cane and cast as an integer
		colorData.red[stripeCounter] = (unsigned int)eightBitColorsHex[i];
		colorData.green[stripeCounter] = (unsigned int)eightBitColorsHex[i+1];
		colorData.blue[stripeCounter] = (unsigned int)eightBitColorsHex[i+2];

		convertToBitCount(lightConfig.bitCount, stripeCounter);

		stripeCounter += 1;
	}

	createLightData(lightConfig.protocolType, RAW);
	sendSpi(lightConfig.icType);
}
