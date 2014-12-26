//*****************************************************************************
//
// Application Name        - LightServer
// File Name			   - lscreatedata.h
// Application Version     - 2.5.0
// Application Modify Date - 26th of December 2014
// Application Developer   - Glenn Vassallo
// Application Contact	   - contact@swiftsoftware.com.au
// Application Repository  - https://github.com/remixed123/LightServer
//
// Further Details:		   - Please view the details in lscreatedata.c
//
//*****************************************************************************

#ifndef LSCREATEDATA_H_
#define LSCREATEDATA_H_


 //*****************************************************************************
 // LightData Defines
 //*****************************************************************************
#define zero false //0b0
#define one  true //0b1

#define CHECK_BIT(var, pos) !!((var) & (1 << (pos)))

//*****************************************************************************
// function declarations
//*****************************************************************************
lsresult_t convertToBitCount(int bitCount, int stripeCounter);
lsresult_t createLightData(int protocolType, int packetType);

#endif /* LSCREATEDATA_H_ */
