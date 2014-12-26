//*****************************************************************************
//
// Application Name        - LightServer
// File Name			   - lscolors.c
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
// File Overview		   - Includes functions to create random colors.
//
// Application Details     - https://github.com/remixed123/LightServer/readme.txt
//
//*****************************************************************************

#include "lightserver.h"
#include "lsutils.h"

/* Standard Header files */
#include <stdlib.h>
#include <math.h>

//*****************************************************************************
//! createRandomColors
//!
//! Creates random colors to store in the Select part of the color
//! This uses a naive random number generator on RGB which does not produce best results
//! Leaving here in case it will be useful in something
//!
//****************************************************************************
lsresult_t createRandomColorsRGB()
{
	int i, j;
	for (i=0; i < 5; i++)
	{
		colorSelected.red[i] = rand() % 255;
		colorSelected.green[i] = rand() % 200;
		colorSelected.blue[i] = rand() % 255;
	}

	for (i=0; i < 25; i++)
	{
		j = randRange(0, (effectControl.actualColorCount - 1));
		// Store colors for later use, this is used when randomally mixing colors in different positions
		colorMix.red[i] = colorSelected.red[j];
		colorMix.green[i] = colorSelected.green[j];
		colorMix.blue[i] = colorSelected.blue[j];
	}

	return LS_SUCCESS;
}

//*****************************************************************************
//! createRandomColorHSV
//!
//! Creates random colors to store in the Select part of the color
//! This uses the HSV color scheme to produce the best results
//!
//! Details on algorithm used: http://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically
//!
//****************************************************************************
lsresult_t  createRandomColorHSV(uint8_t colorNumber, uint8_t mix)
{
	double golden_ratio_conjugate = 0.618033988749895;
	double s = 0.99;
	double v = 0.99;
	double h;

	h = rand(); // # use random start value
	h = h / (float) RAND_MAX;
	h = h + golden_ratio_conjugate;
	if (h > 1)
		h = h - 1;

	double	hi	= floor(h*6);
	double f = h*6 - hi;
	double p	= v * (1-s);
	double q	= v * (1-f*s);
	double t	= v * (1-(1-f)*s);
	double r	= 255;
	double g	= 255;
	double b	= 255;

	switch((int)hi) {
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		case 5:
			r = v;
			g = p;
			b = q;
			break;
		default:
			r = v;
			g = t;
			b = p;
			break;
	}

	if (mix == 0)
	{
		colorSelected.red[colorNumber] = floor(r*256);
		colorSelected.green[colorNumber] = floor(g*256);
		colorSelected.blue[colorNumber] = floor(b*256);
	}
	else // used to generate random color in mix effects
	{
		colorMix.red[colorNumber] = floor(r*256);
		colorMix.green[colorNumber] = floor(g*256);
		colorMix.blue[colorNumber] = floor(b*256);
	}


	return LS_SUCCESS;
}

//*****************************************************************************
//! createRandomColors
//!
//! Creates random colors to store in the Select part of the color
//! This uses the HSV color scheme to produce the best results
//!
//! Details on algorithm used: http://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
//!
//****************************************************************************
lsresult_t createRandomColorsHSV()
{
	uint8_t i, j;

	for (i = 0; i < 5; i++)
	{
		createRandomColorHSV(i,0);
	}

	for (i=0; i < 25; i++)
	{
		j = randRange(0, (effectControl.actualColorCount - 1));
		// Store colors for later use, this is used when randomally mixing colors in different positions
		colorMix.red[i] = colorSelected.red[j];
		colorMix.green[i] = colorSelected.green[j];
		colorMix.blue[i] = colorSelected.blue[j];
	}

	return LS_SUCCESS;
}
