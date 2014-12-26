//*****************************************************************************
//
// Application Name        - LightServer
// File Name			   - lsutils.h
// Application Version     - 2.5.0
// Application Modify Date - 26th of December 2014
// Application Developer   - Glenn Vassallo
// Application Contact	   - contact@swiftsoftware.com.au
// Application Repository  - https://github.com/remixed123/LightServer
//
// Further Details:		   - Please view the details in lsutils.c
//
//*****************************************************************************

#include <stdint.h>

#ifndef LSUTILS_H_
#define LSUTILS_H_

 //*****************************************************************************
 // Defines used by hex2intMultiByte function
 //*****************************************************************************
 #define ASCII_0_VALU 48
 #define ASCII_9_VALU 57
 #define ASCII_A_VALU 65
 #define ASCII_F_VALU 70

//******************************************************************************
// APIs
//******************************************************************************
int randRange(int min_n, int max_n);
uint8_t flip( uint8_t n );
int arrayPosition(int arrayCounter);
int bitPosition(int shiftCounter);
unsigned int hex2int(unsigned const char *hex);
//unsigned int hex2intMultiByte(unsigned const char * s);
unsigned int hex2intMultiByte(char const* hexstring);
unsigned char * hexStringToBytes(unsigned char * hexstring, int byteCount);
char * asciiToHex(char * asciiString, int maxLength);

#endif /* LSUTILS_H_ */
