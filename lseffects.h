//*****************************************************************************
//
// Application Name        - LightServer
// File Name			   - lseffects.h
// Application Version     - 2.5.0
// Application Modify Date - 26th of December 2014
// Application Developer   - Glenn Vassallo
// Application Contact	   - contact@swiftsoftware.com.au
// Application Repository  - https://github.com/remixed123/LightServer
//
// Further Details:		   - Please view the details in lseffects.c
//
//*****************************************************************************

#ifndef LSEFFECTS_H_
#define LSEFFECTS_H_

void stationaryEffect();
void candleEffect();
void chaseEffect();
void snakeEffect();
void cometEffect();
void waveEffect();
void antsEffect();
void paintEffect();
void strobeEffect();
void twinkleEffect();
void pulseEffect();
void glowEffect();
void glowMixEffect();
void blendMixEffect();
void blendMixMultiEffect();
void blendEffect();
void glowWaveEffect();
void colorWheelCalc(uint8_t position, int stripeCounter);
void colorWheelEffect();
void rainbowEffect();
void rawDataConvert();

#endif /* LSEFFECTS_H_ */
