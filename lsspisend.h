//*****************************************************************************
//
// Application Name        - LightServer
// File Name			   - lsspisend.h
// Application Version     - 2.5.0
// Application Modify Date - 26th of December 2014
// Application Developer   - Glenn Vassallo
// Application Contact	   - contact@swiftsoftware.com.au
// Application Repository  - https://github.com/remixed123/LightServer
//
// Further Details:		   - Please view the details in lsspisend.c
//
//*****************************************************************************

#ifndef LSSPISEND_H_
#define LSSPISEND_H_

lsresult_t sendSpi(int icType);
void resetLights();
void addressLights();

#endif /* LSSPISEND_H_ */
