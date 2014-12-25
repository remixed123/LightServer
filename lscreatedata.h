/*
 * lscreatedata.h
 *
 *  Created on: 30/09/2014
 *      Author: glenn
 */

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
