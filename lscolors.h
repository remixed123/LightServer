//*****************************************************************************
//
// Application Name        - LightServer
// File Name			   - lscolors.h
// Application Version     - 2.5.0
// Application Modify Date - 26th of December 2014
// Application Developer   - Glenn Vassallo
// Application Contact	   - contact@swiftsoftware.com.au
// Application Repository  - https://github.com/remixed123/LightServer
//
// Further Details:		   - Please view the details in lscolors.c
//
//*****************************************************************************

#ifndef LSCOLORS_H_
#define LSCOLORS_H_

lsresult_t createRandomColorsRGB();
lsresult_t createRandomColorHSV(uint8_t colorNumber, uint8_t mix);
lsresult_t createRandomColorsHSV();

#endif /* LSCOLORS_H_ */
