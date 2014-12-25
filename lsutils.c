/*
 * lsutils.c
 *
 *  Created on: 29/09/2014
 *      Author: glenn
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "lsutils.h"

//*****************************************************************************
// Lookup Tables used by Utilises
//*****************************************************************************
static const long hextable[] = {
   [0 ... 255] = -1,                     // bit aligned access into this table is considerably
   ['0'] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, // faster for most modern processors,
   ['A'] = 10, 11, 12, 13, 14, 15,       // for the space conscious, reduce to
   ['a'] = 10, 11, 12, 13, 14, 15        // signed char.
};

uint8_t lookup[16] = {
   0x0, 0x8, 0x4, 0xC,
   0x2, 0xA, 0x6, 0xE,
   0x1, 0x9, 0x5, 0xD,
   0x3, 0xB, 0x7, 0xF };


//*****************************************************************************
//! randRange
//!
//! Generates a random between a range
//!
//! returns: integer
//*****************************************************************************
int randRange(int min_n, int max_n)
{
    return rand() % (max_n - min_n + 1) + min_n;
}

//*****************************************************************************
//! flip
//!
//! reverses the bits
//!
//! returns: reversed bits
//*****************************************************************************
uint8_t flip( uint8_t n )
{
   //This should be just as fast and it is easier to understand.
   //return (lookup[n%16] << 4) | lookup[n/16];
   return (lookup[n&0x0F] << 4) | lookup[n>>4];
}

//*****************************************************************************
//! arrayPosition
//!
//! Returns the position in an array .....
//!
//!
//*****************************************************************************
int arrayPosition(int arrayCounter)
{
	int arrayPos = abs(arrayCounter/8);
	return arrayPos;
}

//*****************************************************************************
//! bitPosition
//!
//! Returns the bit position .....
//!
//!
//*****************************************************************************
int bitPosition(int shiftCounter)
{
	int bitPos = 8-(shiftCounter++%8);

	if (bitPos == 8)
	{
		return 0;  // This is for the last bit in the byte, position 0
	}
	else
	{
		return bitPos;
	}
}

//*****************************************************************************
//! hex2int
//!
//! @brief Convert a hexidecimal string to a signed long. will not produce or process
//! negative numbers except to signal error.
//!
//! @param hex without decoration, case insensative.
//!
//! @return -1 on error, or result (max sizeof(long)-1 bits)
//*****************************************************************************
unsigned int hex2int(unsigned const char *hex)
{
   long ret = 0;
   while (*hex && ret >= 0) {
      ret = (ret << 4) | hextable[*hex++];
   }
   return ret;
}

//unsigned int hex2intMultiByte(unsigned const char * s)
//{
// unsigned int result = 0;
// int c ;
// if ('0' == *s && 'x' == *(s+1)) { s+=2;
//  while (*s) {
//   result = result << 4;
//   if (c=(*s-'0'),(c>=0 && c <=9)) result|=c;
//   else if (c=(*s-'A'),(c>=0 && c <=5)) result|=(c+10);
//   else if (c=(*s-'a'),(c>=0 && c <=5)) result|=(c+10);
//   else break;
//   ++s;
//  }
// }
// return result;
//}

//*****************************************************************************
//! hex2intMultiByte
//!
//! Converts a hex values to ....
//!
//!
//*****************************************************************************
unsigned int hex2intMultiByte(char const* hexstring)  //(char const* hexstring)
{
    unsigned int result = 0;
    char const *c = hexstring;
    char thisC;

    while( (thisC = *c) != NULL )
    {
        unsigned int add;
        //thisC = toupper(thisC);

        result <<= 4;

        if( thisC >= ASCII_0_VALU &&  thisC <= ASCII_9_VALU )
            add = thisC - ASCII_0_VALU;
        else if( thisC >= ASCII_A_VALU && thisC <= ASCII_F_VALU)
            add = thisC - ASCII_A_VALU + 10;
        else
        {
        	//System_printf("Unrecognised hex character \"%c\"\n", thisC);
            exit(-1);
        }

        result += add;
        ++c;
    }
    return result;
}

//*****************************************************************************
//! hexStringToBytes
//!
//! Converts a string of hexadecimal numbers to an array of bytes.s
//!
//!
//*****************************************************************************
unsigned char * hexStringToBytes(unsigned char * hexstring, int byteCount)
{
   unsigned char *b;
   char work[2]; // = {0x00, 0x00, 0x00};
   unsigned char *bytes = NULL;
   int i;

   //bytes = calloc(strlen(hexstring) / 2, sizeof *bytes);
   bytes = calloc(byteCount, sizeof *bytes);
   b = bytes;

   //for (i = 0; i < (int)strlen(hexstring); i += 2)
   for (i = 0; i < (byteCount * 2); i += 2)
   {
      work[0] = (char)hexstring[i];
      work[1] = (char)hexstring[i + 1];
      *(b++) = (unsigned char)strtoul(work, (char **)NULL, 16);
   }

   return bytes;
}

//*****************************************************************************
//! asciiToHex
//!
//! This function converts an ASCII string into its HEX Values, represented as a string
//!
//!
//*****************************************************************************
char * asciiToHex(char * asciiString, int maxLength) {
    int i = 0;
    char * hex = (char *) malloc(sizeof (char) * ((maxLength*2)+1));

    for (i = 0; i < maxLength; i++)
    {
        //sprintf((hex+(i*2)), "&#37;%02x", (unsigned char)(*(asciiString+i)));
        sprintf(hex+i*2, "%02X", asciiString[i]);
    }

    hex[(maxLength*2)] = '\0';

    return hex;
}
