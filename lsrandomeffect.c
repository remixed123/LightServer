//*****************************************************************************
//
// Application Name        - LightServer
// File Name			   - lsrandomeffect.c
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
// File Overview		   - lsrandomeffect.c contains functions to randomly
//							 select effects.
//
// Application Details     - https://github.com/remixed123/LightServer/readme.txt
//
//*****************************************************************************

#include "lightserver.h"
#include "lsrandomeffect.h"

#include <stdlib.h>


//*****************************************************************************
//! randomEffectSelector
//!
//! The function selects the random effect
//!
//! \param  randEffect
//!
//! \return 0 on success
//*****************************************************************************
lsresult_t randomEffectSelector(int randEffect)
{
	if (randEffect == 1)
	{
		memcpy(effectControl.fx, "Fixed", 5);
		effectControl.fx[5] = '\0';
	}
	else if (randEffect == 2)
	{
		memcpy(effectControl.fx, "Candle", 6);
		effectControl.fx[6] = '\0';
	}
	else if (randEffect == 3)
	{
		memcpy(effectControl.fx, "Chase", 5);
		effectControl.fx[5] = '\0';
	}
	else if (randEffect == 4)
	{
		memcpy(effectControl.fx, "Strobe", 6);
		effectControl.fx[6] = '\0';
	}
	else if (randEffect == 5)
	{
		memcpy(effectControl.fx, "Waves", 5);
		effectControl.fx[5] = '\0';
	}
	else if (randEffect == 6)
	{
		memcpy(effectControl.fx, "Comets", 6);
		effectControl.fx[6] = '\0';
	}
	else if (randEffect == 7)
	{
		memcpy(effectControl.fx, "Snakes", 6);
		effectControl.fx[6] = '\0';
	}
	else if (randEffect == 8)
	{
		memcpy(effectControl.fx, "Twinkle", 7);
		effectControl.fx[7] = '\0';
	}
	else if (randEffect == 9)
	{
		memcpy(effectControl.fx, "Rainbow", 7);
		effectControl.fx[7] = '\0';
	}
	else if (randEffect == 10)
	{
		memcpy(effectControl.fx, "Color Wheel", 11);
		effectControl.fx[11] = '\0';
	}
	else if (randEffect == 11)
	{
		memcpy(effectControl.fx, "Glow", 4);
		effectControl.fx[4] = '\0';
	}
	else if (randEffect == 12)
	{
		memcpy(effectControl.fx, "Pulse", 5);
		effectControl.fx[5] = '\0';
	}
	else if (randEffect == 13)
	{
		memcpy(effectControl.fx, "Glow Mix", 8);
		effectControl.fx[8] = '\0';
	}
	else if (randEffect == 14)
	{
		memcpy(effectControl.fx, "Glow Wave", 9);
		effectControl.fx[9] = '\0';
	}
	else if (randEffect == 15)
	{
		memcpy(effectControl.fx, "Ants", 4);
		effectControl.fx[4] = '\0';
	}
	else if (randEffect == 16)
	{
		memcpy(effectControl.fx, "Paint", 5);
		effectControl.fx[5] = '\0';
	}
	else if (randEffect == 17)
	{
		memcpy(effectControl.fx, "Blend", 5);
		effectControl.fx[5] = '\0';
	}
	else if (randEffect == 18)
	{
		memcpy(effectControl.fx, "Blend Mix", 9);
		effectControl.fx[9] = '\0';
	}
	else
	{
		memcpy(effectControl.fx, "Fixed", 5);
		effectControl.fx[5] = '\0';
	}

	return LS_SUCCESS;
}


//*****************************************************************************
//! selectRandomEffect()
//!
//! Selects a random effect to use
//!
//!
//****************************************************************************
lsresult_t selectRandomEffect()
{
	effectControl.randEffect = rand() % 19;
	randomEffectSelector(effectControl.randEffect);

	effectControl.speed = (rand() % 100) + 1;
	effectControl.direction = (rand() % 100) % 2;
	effectControl.colorCount = (rand() % 5) + 1;
	effectControl.actualColorCount = effectControl.colorCount;
	effectControl.storedColorCount = effectControl.actualColorCount;

	beginEffectClock();

	return LS_SUCCESS;
}
