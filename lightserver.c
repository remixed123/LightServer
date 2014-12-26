//*****************************************************************************
//
// Application Name        - LightServer
// File Name			   - lightserver.c
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
// File Overview		   - lightserver.c contains the main program loop that sends
//							 and receives packets. It also includes the control aspects,
//							 the clock and the processing of the internal HTTP web
//							 server requests.
//
// Application Details     - https://github.com/remixed123/LightServer/readme.txt
//
//*****************************************************************************

/* Standard Header files */
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

/* JSON Library Headers */
#include "jsmn.h"
#include "json.h"
#include "buf.h"

/* LightServer Library Headers */
#include "lightserver.h"
#include "lseffects.h"
#include "lscolors.h"
#include "lsspiconfig.h"
#include "lsspisend.h"
#include "lsspiencode.h" // SPI Word Encoding Header
#include "lsutils.h" // Utilities
#include "lsrandomeffect.h"
#include "lsmdns.h"
#include "lsparsefile.h"

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>

/* SYS/BIOS Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/gates/GateHwi.h>

/* Peripheral Headers */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/WiFi.h>
#include <ti/drivers/SPI.h>
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"

/* SimpleLink Wi-Fi Host Driver Header files */
#include <simplelink.h>
#include <netapp.h>
#include <netcfg.h>
#include <osi.h>
#include <fs.h>
//#include <protocol.h>

/* Board Config Header */
#include "Board.h"


char slconfigfilename [] = "config.txt";
char slcontrol1filename [] = "control1.txt";
char slcontrol2filename [] = "control2.txt";
char slcontrol3filename [] = "control3.txt";
char slcontrol4filename [] = "control4.txt";
char slcontrol5filename [] = "control5.txt";
char slcontrol6filename [] = "control6.txt";
char slcontrolstoredfilename [] = "controls.txt";

 //*****************************************************************************
 // Globals used by SPI
 //*****************************************************************************
 uint32_t masterRxBuffer[SPI_MSG_LENGTH];
 UChar masterTxBuffer[SPI_MSG_LENGTH] = "";

//*****************************************************************************
// Globals used by sockets and simplelink
//*****************************************************************************
uint8_t buffer[UDPPACKETSIZE];
bool deviceConnected = false;
bool ipAcquired = false;
uint32_t currButton;
uint32_t prevButton;

char returnPacket[128];

typedef struct
{
    unsigned long  ipV4;
    unsigned long  ipV4Mask;
    unsigned long  ipV4Gateway;
    unsigned long  ipV4DnsServer;
}_NetCfgIpV4Args_t;

//*****************************************************************************
// Globals used by http web server
//*****************************************************************************
uint8_t POST_token[] = "__SL_P_L.D";
uint8_t GET_Color_token[]  = "__SL_G_COL";
uint8_t GET_Light_token[]  = "__SL_G_LCF";

//*****************************************************************************
// Globals used by the timer interrupt handler.
//*****************************************************************************
static volatile unsigned long g_ulSysTickValue;
static volatile unsigned long g_ulBase;  // Base address for first timer
static volatile unsigned long g_ulRefBase;
static volatile unsigned long g_ulRefTimerInts = 0;
static volatile unsigned long g_ulIntClearVector;
unsigned long g_ulTimerInts;

//*****************************************************************************
// Globals used in JSON Data Creation and Storage
//*****************************************************************************
UChar controlJsonData[250];
UChar configJsonData[200];

//*****************************************************************************
// Globals used in Raw Data Light Effect Creation
//*****************************************************************************
uint8_t  rawDataSize;
UChar * rawData;

//*****************************************************************************
// Globals used in Light Configuration
//*****************************************************************************
LightConfiguration lightConfig =  {"", "", 100, 490, 8, STANDARD, WS8212, GBR, GAMMA_ON};

//*****************************************************************************
// Globals used in Light Color Data Creation and Manipulation
//*****************************************************************************
UChar lightData[5366];
LightColorData colorData; // Color data used to create lightData
LightColorsSelected colorSelected; // Color data which has been selected by the user
LightColorsSelected colorSelectedStored;
LightColorEffects colorEffects; // Color data used is effects
// Color data that has been either selected or randomally generated and will
// be stored for later, used when randomally mixing colors about
LightColorSelectMix colorMix;

//*****************************************************************************
// Globals used in Lighting Effect Control and Creation
//*****************************************************************************
LightEffectControl effectControl = {"Raw",100,0,0,20,500,500,1,1,1,1,0,0};

//*****************************************************************************
// Globals used in Lighting Effect Calculations
//*****************************************************************************
LightEffectCalcs effectCalc = {0,0,0,0,{50,100,150,175,200,250,200,175,150,100,50},{1,1,1,1,1,0,0,0,0,0,0}};

//static uint8_t lightBitCountShift = 12; // Currently only 12 bit needs shifting

//*****************************************************************************
// Globals used as Triggers
//*****************************************************************************
// Triggers the saving of config or control data when initiated from within the HTTP Callback
static uint8_t saveConfigTrigger = 0;
static uint8_t saveControlTrigger = 0;

//*****************************************************************************
// Globals used by the Clock
//*****************************************************************************
static Clock_Handle effectClock =  NULL;
static Clock_Params clkParams;


///////////////////////////////////////////////////////////////////////////////
// Effect Selectors and Configuration
///////////////////////////////////////////////////////////////////////////////


//*****************************************************************************
//! clockEffectSelector
//!
//! The function is triggered by a clock and will select the appropriate effect
//! the run
//!
//! \param  Effect Name
//!
//! \return none
//*****************************************************************************
Void clockEffectSelector(UArg arg0)
{

	if (strcmp((const char *)effectControl.fx, "Raw") == 0)
	{
		rawDataConvert();
	}
	else if ((strcmp((const char *)effectControl.fx, "Fixed") == 0) || (strcmp((const char *)effectControl.fx, "Auto Fixed") == 0))
	{
		stationaryEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Candle") == 0) || (strcmp((const char *)effectControl.fx, "Auto Candle") == 0))
	{
		candleEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Chase") == 0) || (strcmp((const char *)effectControl.fx, "Auto Chase") == 0))
	{
		chaseEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Strobe") == 0) || (strcmp((const char *)effectControl.fx, "Auto Strobe") == 0))
	{
		strobeEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Waves") == 0) || (strcmp((const char *)effectControl.fx, "Auto Waves") == 0))
	{
		waveEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Comets") == 0) || (strcmp((const char *)effectControl.fx, "Auto Comets") == 0))
	{
		cometEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Snakes") == 0) || (strcmp((const char *)effectControl.fx, "Auto Snakes") == 0))
	{
		snakeEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Ants") == 0) || (strcmp((const char *)effectControl.fx, "Auto Ants") == 0))
	{
		antsEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Paint") == 0) || (strcmp((const char *)effectControl.fx, "Auto Paint") == 0))
	{
		paintEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Twinkle") == 0) || (strcmp((const char *)effectControl.fx, "Auto Twinkle") == 0) || (strcmp((const char *)effectControl.fx, "Auto Twinkle Multi") == 0))
	{
		twinkleEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Glow") == 0) || (strcmp((const char *)effectControl.fx, "Auto Glow") == 0))
	{
		glowEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Glow Mix") == 0) || (strcmp((const char *)effectControl.fx, "Auto Glow Mix") == 0) || (strcmp((const char *)effectControl.fx, "Auto Glow Multi") == 0))
	{
		glowMixEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Blend") == 0) || (strcmp((const char *)effectControl.fx, "Auto Blend") == 0) || (strcmp((const char *)effectControl.fx, "Auto Blend Multi") == 0))
	{
		blendEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Pulse") == 0) || (strcmp((const char *)effectControl.fx, "Auto Pulse") == 0))
	{
		pulseEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Glow Wave") == 0) || (strcmp((const char *)effectControl.fx, "Auto Glow Wave") == 0))
	{
		glowWaveEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Blend Mix") == 0) || (strcmp((const char *)effectControl.fx, "Auto Blend Mix") == 0))
	{
		blendMixEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Auto Blend Mix Multi") == 0))
	{
		blendMixMultiEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Rainbow") == 0))
	{
		rainbowEffect();
	}
	else if ((strcmp((const char *)effectControl.fx, "Color Wheel") == 0))
	{
		colorWheelEffect();
	}
	else
	{
		stationaryEffect();
	}

}

//*****************************************************************************
//! settingsEffectSelector
//!
//! Sets up the various values/config to be used in the creaton of effects
//!
//! \param  None
//!
//! \return none
//*****************************************************************************
Void settingsEffectSelector()
{
	int i;

	effectControl.randomColor = 0;

	if (strcmp((const char *)effectControl.fx, "Fixed") == 0)
	{
		effectControl.effectSpeed = 1000;
	}
	else if (strcmp((const char *)effectControl.fx, "Candle") == 0)
	{
		effectControl.effectSpeed = 20;
		effectControl.tempSpeed = effectControl.speed;
		effectControl.tempColorCount = effectControl.actualColorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Chase") == 0)
	{
		effectControl.effectSpeed = 105 - effectControl.speed;
		if (effectControl.direction == 1)
		{
			effectControl.tempDirection = effectControl.actualColorCount - 1;
		}
		else
		{
			effectControl.tempDirection = 0;
		}
	}
	else if (strcmp((const char *)effectControl.fx, "Strobe") == 0)
	{
		effectControl.effectSpeed = 105 - effectControl.speed;
		effectControl.tempColorCount = effectControl.actualColorCount;
		effectControl.colorCount = 1;
		if (effectControl.direction == 0)
		{
			effectControl.tempDirection = 0;
		}
		else
		{
			effectControl.tempDirection = (effectControl.actualColorCount - 1);
		}
	}
	else if (strcmp((const char *)effectControl.fx, "Waves") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Comets") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Glow") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 10;
		effectControl.tempColorCount = effectControl.colorCount;

		for (i = 0; i < 24; i++)
		{
			effectCalc.glow[i] = randRange(50,250);
			effectCalc.glowDirection[i] = (rand() % 100) % 2;
		}
	}
	else if (strcmp((const char *)effectControl.fx, "Glow Mix") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 25;
		effectControl.tempColorCount = effectControl.colorCount;

		for (i = 0; i < 25; i++)
		{
			effectCalc.glow[i] = randRange(0,250);
			effectCalc.glowDirection[i] = (rand() % 100) % 2;
		}
	}
	else if (strcmp((const char *)effectControl.fx, "Glow Wave") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 11;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Blend") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = effectControl.actualColorCount;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Snakes") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Ants") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Paint") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Pulse") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.tempColorCount = effectControl.colorCount;

		effectCalc.glow[0] = 100;
		effectCalc.glowDirection[0] = 1;
	}
	else if (strcmp((const char *)effectControl.fx, "Blend Mix") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 25;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Twinkle") == 0)
	{
		effectControl.effectSpeed = 15;
		effectControl.tempSpeed = effectControl.speed;
		effectControl.tempColorCount = effectControl.actualColorCount;
	}
	if (strcmp((const char *)effectControl.fx, "Auto Fixed") == 0)
	{
		effectControl.effectSpeed = 1000;
		effectControl.colorCount = 1;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Candle") == 0)
	{
		effectControl.effectSpeed = 20;
		effectControl.tempSpeed = effectControl.speed;
		effectControl.colorCount = 1;
		effectControl.tempColorCount = 1;
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Chase") == 0)
	{
		effectControl.colorCount = 3;
		effectControl.effectSpeed = 105 - effectControl.speed;
		if (effectControl.direction == 1)
		{
			effectControl.tempDirection = effectControl.actualColorCount - 1;
		}
		else
		{
			effectControl.tempDirection = 0;
		}
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Glow") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 10;
		effectControl.tempColorCount = 1;

		for (i = 0; i < 10; i++)
		{
			effectCalc.glow[i] = randRange(50,250);
			effectCalc.glowDirection[i] = (rand() % 100) % 2;
		}
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Pulse") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 1;
		effectControl.tempColorCount = 1;

		effectCalc.glow[0] = 100;
		effectCalc.glowDirection[0] = 1;
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Glow Wave") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 11;
		effectControl.tempColorCount = 1;
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Glow Mix") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 25;
		effectControl.tempColorCount = 5;

		for (i = 0; i < 25; i++)
		{
			effectCalc.glow[i] = randRange(0,250);
			effectCalc.glowDirection[i] = (rand() % 100) % 2;
		}
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Blend") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 5;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Blend Mix") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 5;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Strobe") == 0)
	{
		effectControl.effectSpeed = 105 - effectControl.speed;
		effectControl.tempColorCount = 3;
		effectControl.colorCount = 1;
		if (effectControl.direction == 0)
		{
			effectControl.tempDirection = 0;
		}
		else
		{
			effectControl.tempDirection = (effectControl.actualColorCount - 1);
		}
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Waves") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 3;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Comets") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 2;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Snakes") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 2;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Ants") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 5;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Paint") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 5;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Twinkle") == 0)
	{
		effectControl.effectSpeed = 15;
		effectControl.tempSpeed = effectControl.speed;
		effectControl.colorCount = 5;
		effectControl.tempColorCount = effectControl.colorCount;
	}
	else if (strcmp((const char *)effectControl.fx, "Rainbow") == 0)
	{
		effectControl.effectSpeed = abs((104 - effectControl.speed)/2);
		effectControl.tempSpeed = effectControl.speed;
		effectControl.colorCount = 0;
	}
	else if  (strcmp((const char *)effectControl.fx, "Color Wheel") == 0)
	{
		effectControl.effectSpeed = abs((104 - effectControl.speed)/2);
		effectControl.tempSpeed = effectControl.speed;
		effectControl.colorCount = 0;
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Glow Multi") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 15;
		effectControl.tempColorCount = 5;
		effectControl.randomColor = 1;

		for (i = 0; i < 15; i++)
		{
			effectCalc.glow[i] = randRange(0,250);
			effectCalc.glowDirection[i] = (rand() % 100) % 2;
		}
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Blend Multi") == 0)
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 5;
		effectControl.tempColorCount = effectControl.colorCount;
		effectControl.randomColor = 1;
	}
	else if (strcmp((const char *)effectControl.fx, "Auto Twinkle Multi") == 0)
	{
		effectControl.effectSpeed = 15;
		effectControl.tempSpeed = effectControl.speed;
		effectControl.colorCount = 4;
		effectControl.tempColorCount = effectControl.colorCount;
		effectControl.randomColor = 1;
	}
	else if ((strcmp((const char *)effectControl.fx, "Auto Blend Mix Multi") == 0))
	{
		double speedCalc = abs((104 - effectControl.speed)/2);
		effectControl.effectSpeed = (int)speedCalc;
		effectControl.colorCount = 25;
		effectControl.tempColorCount = effectControl.colorCount;
		effectControl.randomColor = 1;
	}
	else
	{
		//stationaryEffect();
	}

	// This helps ensure we do not trigger the clock before the processing of lightData has completed
	// Triggering the clock too early will leave no time for the network to process new packets
	// This leads to an effect running but no access to any other feature
	// The more lights the slower we need to trigger lights.
	if (lightConfig.protocolType == 0)
	{
		effectControl.effectSpeed = effectControl.effectSpeed * 7;

		if ((effectControl.effectSpeed * 10) < lightConfig.lightCount)
			effectControl.effectSpeed = lightConfig.lightCount/7;

		//if (effectControl.effectSpeed < 35)
		//		effectControl.effectSpeed = 35;
	}
	else
	{
		if (effectControl.effectSpeed < 35)
				effectControl.effectSpeed = 35;
	}

 }

///////////////////////////////////////////////////////////////////////////////
// Reset and Addressing Clock Triggered - NOT IN USE
///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//! The interrupt handler for the addressing timer interrupt. It resend addressing packet
//!
//! \param  None
//!
//! \return none
//*****************************************************************************
Void sendResetPacket(UArg arg0)
{
    resetLights();
}

//*****************************************************************************
//! The interrupt handler for the addressing timer interrupt. It resend addressing packet
//!
//! \param  None
//!
//! \return none
//*****************************************************************************
Void sendAddressingPacket(UArg arg0)
{
	addressLights();
}


///////////////////////////////////////////////////////////////////////////////
// Packet Processing and Clock Management functions
///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//! This stops and deletes the current clock, making it ready to recreate
//!
//! \param  None
//!
//! \return 1 when successful
//*****************************************************************************
int stopDeleteClock()
{
	// Stop the Clock and delete it, if it is running
    if (effectClock != NULL) {
    	Clock_stop(effectClock);
    	Clock_delete(&effectClock);
    }
    return 1;
}

//*****************************************************************************
//! beginEffectClock
//!
//! This creates a clock and sets up the effect to be driven by the clock
//!
//! \param  None
//!
//! \return whether successful or not; 0:unsuccesful ; 1:successful
//*****************************************************************************
int beginEffectClock()
{
	stopDeleteClock();
	settingsEffectSelector(); // Make sure the appropriate settings are established for the selected effect
	clockEffectSelector(NULL); // called once to ensure instant change of effect

    Clock_Params_init(&clkParams);
    clkParams.period = effectControl.effectSpeed;
    clkParams.startFlag = TRUE;
    effectClock = Clock_create(clockEffectSelector, 5, &clkParams, NULL);

    if (effectClock == NULL)
    {
    	System_abort("Clock create failed");
    	return 0;
    }

    return 1;
}

//*****************************************************************************
//! processStatus
//!
//! This function determines what type of packet has been received and then processes
//! the packet
//!
//! \return whether successful or not.
//*****************************************************************************
//int processPacket(uint8_t* buffer, int nbytes)
int processPacket(int nbytes)
{
//	GPIO_toggle(Board_LED_RED);

//	unsigned char *timeSeperationPtr = (unsigned char*)&buffer[14];
//  unsigned char *speedPtr = (unsigned char*)&buffer[2];
//	speedHex = hexStringToBytes(speedPtr, 1);
//	speedInt = (unsigned int)*speedHex;

	uint8_t i;
	uint8_t packetIdentifier = ((uint8_t*)buffer)[0];
	uint8_t packetType = ((uint8_t*)buffer)[3];

	// Data packet
	if (packetIdentifier == 0x41)
	{
		rawDataSize = (((uint8_t*)buffer)[9]);

		// Raw data packet
		if (packetType == 0x01)
		{
			stopDeleteClock();

			rawData = (unsigned char *)&buffer[10];
			lightConfig.byteCount = abs(((19+(39*lightConfig.lightCount))/8)+1);

			memcpy(effectControl.fx, "Raw\0", 4);

			rawDataConvert();

		}
		// Control packet
		else if (packetType == 0xF6)
		{
			// Store the json string into a global array for safe keeping
			for (i = 0; i < rawDataSize; i++)
			{
				controlJsonData[i] = (unsigned char)buffer[10+i];
			}

			extractControlJSON();
			beginEffectClock();

		}
		// Config packet
		else if (packetType == 0xFA)
		{
			// Store the json string into a global array for safe keeping
			for (i = 0; i < rawDataSize; i++)
			{
				configJsonData[i] = (unsigned char)buffer[10+i];
			}
			configJsonData[i++] = 0x00;

			extractConfigJSON();
			saveConfig();
			loadConfig();
		}

	}
	// Query packet
	else if (packetIdentifier == 0x42)
	{
		// Query config packet
		if (packetType == 0xFA)
		{

		}
		// Query status packet
		else if (packetType == 0xFB)
		{

		}
	}

	return(1);
}

///////////////////////////////////////////////////////////////////////////////
// SimpleLink Functions
///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//! SimpleLinkHttpServerCallback
//!
//! \brief This function handles callback for the HTTP server events
//!
//! \param[in]     	pServerEvent - Contains the relevant event information
//! \param[in]      pServerResponse - Should be filled by the user with the
//!					relevant response information
//!
//! \return 		None
//*****************************************************************************
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pEvent, SlHttpServerResponse_t *pResponse)
{
	uint8_t i;

    switch (pEvent->Event)
    {
        case SL_NETAPP_HTTPGETTOKENVALUE:
        {
            uint8_t *ptr = 0;

            ptr = pResponse->ResponseData.token_value.data;
            pResponse->ResponseData.token_value.len = 0;

            if(memcmp(pEvent->EventData.httpTokenName.data, GET_Color_token, strlen((const char *)GET_Color_token)) == 0) // Send the current color settings to the web page
            {
            	char redString[3];
            	char greenString[3];
            	char blueString[3];

                for (i=0; i < effectControl.actualColorCount; i++) // Color is on
                {
                	memcpy(ptr, "1,", 2);
                	ptr+=2;
                }

                for (i=effectControl.actualColorCount; i < 5; i++) // Color is off
                {
                	memcpy(ptr, "0,", 2);
                	ptr+=2;
                }

                for (i=0; i < 5; i++)  // Colors
                {
                	sprintf(redString,"%02X",colorSelectedStored.red[i]);
                	memcpy(ptr, redString, 2);
                	ptr+=2;
                	sprintf(greenString,"%02X",colorSelectedStored.green[i]);
                	memcpy(ptr, greenString, 2);
                	ptr+=2;
                	sprintf(blueString,"%02X",colorSelectedStored.blue[i]);
                	memcpy(ptr, blueString, 2);
                	ptr+=2;
                	if (i != 4) // do not add comma for last color
                	{
                		memcpy(ptr, ",", 1);
                		ptr+=1;
                	}
                }

                //memcpy(ptr, "1,1,1,0,0,FF0000,0000FF,FFFF00,000000,FFFFFF", 44);

                *ptr = '\0';
                pResponse->ResponseData.token_value.len = 44;
            }
            else if(memcmp(pEvent->EventData.httpTokenName.data, GET_Light_token, strlen((const char *)GET_Light_token)) == 0) // Sends the current light config to the webpage
            {
				// Convert to ASCII
				char  lightCountChar[5];
				sprintf(lightCountChar, "%d", lightConfig.lightCount);
				char  icTypeChar[1];
				sprintf(icTypeChar, "%d", lightConfig.icType);
				char  colorTypeChar[1];
				sprintf(colorTypeChar, "%d", lightConfig.colorOrder);

				memcpy(ptr, icTypeChar, 1);
				ptr++;
				memcpy(ptr, ",", 1);
				ptr++;
				memcpy(ptr, colorTypeChar, 1);
				ptr++;
				memcpy(ptr, ",", 1);
				ptr++;
				uint8_t lightCountLength = strlen(lightCountChar);
				memcpy(ptr, lightCountChar, lightCountLength);
				ptr+=lightCountLength;

				// Test data - data will look like the below
            	//memcpy(ptr, "0,5,125", 7);
            	//ptr+=7;

            	*ptr = '\0';

				uint8_t dataLength = 4 + lightCountLength;
				pResponse->ResponseData.token_value.len = dataLength;
            }
        }
        break;

        case SL_NETAPP_HTTPPOSTTOKENVALUE:
        {
        	uint8_t i, j;
            uint8_t *ptr = pEvent->EventData.httpPostData.token_name.data;
            uint8_t dataLength = pEvent->EventData.httpPostData.token_value.len;
            uint8_t dataValue[45];
        	uint8_t intColorValue;
        	uint8_t intChipTypeValue;
        	uint8_t intColorOrderValue;
        	uint16_t intLightCountValue;
            uint8_t colorValue[3];
            uint8_t chipTypeValue[2];
            uint8_t colorOrderValue[2];
            uint8_t lightCountValue[5];

            if(memcmp(ptr, POST_token, strlen((const char *)POST_token)) == 0)
            {
                ptr = pEvent->EventData.httpPostData.token_value.data;
                memcpy(dataValue, ptr+4, dataLength - 4);
                dataValue[dataLength - 4] = '\0';

                if (memcmp(ptr, "SPD", 3) == 0) // Speed of the effect
                {
                	int speedValue = atoi((const char *)dataValue);
    				effectControl.speed = speedValue;
                }
                else if (memcmp(ptr, "INT", 3) == 0) // Brightness of the effect
                {
    				int intensityValue = atoi((const char *)dataValue);
    				effectControl.intensity = intensityValue;
                }
                else if (memcmp(ptr, "DIR", 3) == 0) // Direction of the effect
                {
    				int directionValue = atoi((const char *)dataValue);
    				effectControl.direction = directionValue;
                }
                else if (memcmp(ptr, "FXX", 3) == 0) // The effect to use
                {
    				memcpy(effectControl.fx, dataValue, dataLength - 4);
    				effectControl.fx[dataLength - 4] = '\0';

    				if (memcmp(effectControl.fx, "Auto", 4) != 0) // Used to store the selected colors, not the random created colors
    				{
    					effectControl.colorCount = effectControl.storedColorCount;
    					effectControl.actualColorCount = effectControl.storedColorCount;

    					for (i = 0; i < 5; i++)
    					{
        					colorSelected.red[i] = colorSelectedStored.red[i];
        					colorSelected.green[i] = colorSelectedStored.green[i];
        					colorSelected.blue[i] = colorSelectedStored.blue[i];
    					}
    				}
    				else if(memcmp(effectControl.fx, "Auto", 4) == 0) // We need to create some random colors
    				{
    					createRandomColorsHSV();
    				}
                }
                else if (memcmp(ptr, "SAV", 3) == 0) // Save the current effect
                {
    				int savValue = atoi((const char *)dataValue);
    				if (savValue == 1)
    				{
    					saveControlTrigger = 1;
    				}
                }
                else if (memcmp(ptr, "OFF", 3) == 0) // Turn off the lights
                {
    				int offValue = atoi((const char *)dataValue);
    				if (offValue == 1)
    				{
    					effectControl.colorCount = 1;
    					colorSelected.red[0] = 0;
    					colorSelected.green[0] = 0;
    					colorSelected.blue[0] = 0;
    					memcpy(effectControl.fx, "Fixed\0", 6);
    				}
                }
                else if (memcmp(ptr, "COL", 3) == 0) // Colors
                {
                	effectControl.colorCount = (dataLength - 4)/6; // Calculate how many colors we have been sent
                	effectControl.actualColorCount = effectControl.colorCount;
                	effectControl.storedColorCount = effectControl.actualColorCount;

                	if (effectControl.actualColorCount < 1) // No colors have been included, turn off all colors (black)
                	{
                		effectControl.colorCount = 1;
                		effectControl.actualColorCount = 1;
                		colorSelected.red[0] = 0;
                		colorSelected.green[0] = 0;
                		colorSelected.blue[0] = 0;
                	}
                	else // We have colors, use these in effects
                	{
						for (i = 0; i < effectControl.colorCount; i++)
						{
							memcpy(colorValue, ptr + 4 + (i*6), 2);
							colorValue[2] = '\0';
							intColorValue = hex2int(colorValue);
							colorSelected.red[i] = intColorValue;
							colorSelectedStored.red[i] = intColorValue;

							memcpy(colorValue, ptr + 6 + (i*6), 2);
							colorValue[2] = '\0';
							intColorValue = hex2int(colorValue);
							colorSelected.green[i] = intColorValue;
							colorSelectedStored.green[i] = intColorValue;

							memcpy(colorValue, ptr + 8 + (i*6), 2);
							colorValue[2] = '\0';
							intColorValue = hex2int(colorValue);
							colorSelected.blue[i] = intColorValue;
							colorSelectedStored.blue[i] = intColorValue;
						}

						for (i=0; i < 25; i++)
						{
							j = randRange(0, (effectControl.actualColorCount - 1));
							// Store colors for later use, this is used when randomally mixing colors in different positions
							colorMix.red[i] = colorSelected.red[j];
							colorMix.green[i] = colorSelected.green[j];
							colorMix.blue[i] = colorSelected.blue[j];
						}

						// Make sure we are not using the Random effect otherwise this just triggers random colors
						// once beginEffectClock is called, which we do not want to do when setting colors
						if (memcmp(effectControl.fx, "Auto", 4) == 0)
						{
							//memcpy(effectControl.fx, "Fixed", 5);
							//effectControl.fx[5] = '\0';
							uint8_t dataLength = strlen((const char *)effectControl.fx);
							memcpy(effectControl.fx, effectControl.fx+5, dataLength - 5);
							effectControl.fx[dataLength - 5] = '\0';
						}
                	}
                }
                else if (memcmp(ptr, "LCF", 3) == 0) // Light configuration details
                {
					// Chip type for the lights
                	memcpy(chipTypeValue, ptr+4, 1);
					chipTypeValue[1] = '\0';
    				intChipTypeValue = atoi((const char *)chipTypeValue);
    				lightConfig.icType = (ictype_t)intChipTypeValue;

    				// Color order for the lights
					memcpy(colorOrderValue, ptr+6, 1);
					colorOrderValue[1] = '\0';
    				intColorOrderValue = atoi((const char *)colorOrderValue);
    				lightConfig.colorOrder = (colororder_t)intColorOrderValue;

    				// The light count
					memcpy(lightCountValue, ptr+8, 4);
					lightCountValue[4] = '\0';
					intLightCountValue = atoi((const char *)lightCountValue);
					lightConfig.lightCount = intLightCountValue;

					setProtocolType(lightConfig.icType);
					setBitCount(lightConfig.icType);

					saveConfigTrigger = 1;
                }
            }

            beginEffectClock(); // Begin the effect
        }
        break;

        default:
        break;
    }
}

//****************************************************************************
//
//!	\brief OOB Application Main Task - Initializes SimpleLink Driver and
//!                                              Handles HTTP Requests
//! \param[in]              	pvParameters is the data passed to the Task
//!
//! \return	                	None
//
//****************************************************************************
//static void HTTPServerTask(void *pvParameters)
//{
//    //memset(g_ucSSID,'\0',AP_SSID_LEN_MAX);
//
//    //Read Device Mode Configuration
//    //ReadDeviceConfiguration();
//
//    //Connect to Network
//    //ConnectToNetwork();
//
//    //Handle Async Events
//    while(1)
//    {
//
//    }
//}

/*
 *  ======== SimpleLinkWlanEventHandler ========
 *  SimpleLink Host Driver callback for handling WLAN connection or
 *  disconnection events.
 */
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pArgs)
{
    switch (pArgs->Event) {
        case SL_WLAN_CONNECT_EVENT:
            deviceConnected = true;
            break;

        case SL_WLAN_DISCONNECT_EVENT:
            deviceConnected = false;
            break;

        default:
            break;
    }
}

/*
 *  ======== SimpleLinkNetAppEventHandler ========
 *  SimpleLink Host Driver callback for asynchoronous IP address events.
 */
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pArgs)
{
    switch (pArgs->Event) {
        case SL_NETAPP_IPV4_ACQUIRED:
            ipAcquired = true;
            break;

        default:
            break;
    }
}

/*
 *  ======== smartConfigFxn ========
 *  Starts the Smart Config process which allows the user to tell the CC3100
 *  which AP to connect to, using a smart phone app. Downloads available here:
 *  http://www.ti.com/tool/smartconfig
 */
void smartConfigFxn()
{
  uint8_t policyVal;

  // Turn LED to Yellow to indicate that device is in smartconfig mode
  GPIO_write(Board_LED0, Board_LED_OFF); //Blue
  GPIO_write(Board_LED1, Board_LED_ON); //Green
  GPIO_write(Board_LED2, Board_LED_ON); //Red

  /* Set auto connect policy */
  sl_WlanPolicySet(SL_POLICY_CONNECTION,
                   SL_CONNECTION_POLICY(1, NULL, NULL, NULL, NULL),
                   &policyVal,
                   sizeof(policyVal));

  /* Start SmartConfig using unsecured method. */
  sl_WlanSmartConfigStart(NULL, SMART_CONFIG_CIPHER_NONE, NULL, NULL,
                          NULL, NULL, NULL, NULL);
}

//*****************************************************************************
//! lightServer
//!
//! This is the main program that establishes connectivity and then runs a program
//! loop wait to receive packets
//!
//! Task for this function is created statically. See the project's .cfg file.
//*****************************************************************************
Void lightServer(UArg arg0, UArg arg1)
{
	int i;
    WiFi_Handle       handle;
    WiFi_Params       wifiParams;
    _NetCfgIpV4Args_t ipV4;
    //NetCfgIpV4Args_t ipV4;
    uint8_t           len = sizeof(ipV4);
    uint8_t           dhcpIsOn;
    int               nbytes;
    int               status;
    int               selectRes;
    int               slSocket;
    fd_set            readSet;
    timeval           timeout;
    sockaddr_in       localAddr;
    sockaddr_in       client_addr;
    socklen_t         addrlen = sizeof(client_addr);
    int 		      processStatus;
    int			      bytesSent;
    int				  configValue;

    ULong       currButton;
    ULong		deleteButton;
    ULong       prevButton = 0;

    /* Turn LED OFF. It will be used as a connection indicator */
    GPIO_write(Board_LED0, Board_LED_OFF); //Blue
    GPIO_write(Board_LED1, Board_LED_OFF); //Green
    GPIO_write(Board_LED2, Board_LED_OFF); //Red

    /* Open WiFi driver */
    WiFi_Params_init(&wifiParams);
    wifiParams.bitRate = SPI_BIT_RATE;
    handle = WiFi_open(Board_WIFI, Board_SPI_CC3100, NULL, &wifiParams);
    if (handle == NULL) {
        System_abort("WiFi driver failed to open.");
    }

    /*
     * Host driver starts the network processor.
     *
     * sl_Start returns the network processor operating mode:
     *      ROLE_STA (0x00): configured as a station
     *      ROLE_AP  (0x02): configured as an access point
     */

    configValue = sl_Start(NULL, NULL, NULL);
    if (configValue < 0) {
        System_abort("Could not initialize WiFi");
    }

    // Set the AP domain name
    unsigned char strDomain[32] = "lightserver.net";
    unsigned char lenDomain = strlen((const char *)strDomain);
    sl_NetAppSet(SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DOMAIN_NAME, lenDomain, (unsigned char*)strDomain);


    // Holding down button switches the connection mode and deletes the config and control settings
    // It also sets the AP mode as open security and gives it the standard AP name LightServerAP
    deleteButton = GPIO_read(Board_BUTTON1);
    if(deleteButton == 0) //&& (prevButton != 0))
    {
        deleteConfig();

        for (i=1; i < 7; i++) // Delete all scene effects
        	deleteControl(i);

        deleteControl(127); // Delete the stored effect

        // delete profiles
        for (i = 0; i < 8; i++)
        	sl_WlanProfileDel(i);

		if (configValue == ROLE_STA) {

			/* Change mode to access point */
			configValue = sl_WlanSetMode(ROLE_AP);

			// Set SSID name for AP mode
			unsigned char  str[33] = "LightServerAP";
			unsigned short  length = strlen((const char *)str);
			sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SSID, length, str);

			// Set security type for AP mode
			//Security options are:
			//Open security: SL_SEC_TYPE_OPEN
			//WEP security:  SL_SEC_TYPE_WEP
			//WPA security:  SL_SEC_TYPE_WPA
			unsigned char  val = SL_SEC_TYPE_OPEN;
			sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SECURITY_TYPE, 1, (unsigned char *)&val);

			//Set Password for for AP mode (for WEP or for WPA) example:
			//Password - for WPA: 8 - 63 characters
			//           for WEP: 5 / 13 characters (ascii)
//			unsigned char  strpw[65] = "passWORD";
//			unsigned short  len = strlen((const char *)strpw);
//			memset(strpw, 0, 65);
//			memcpy(strpw, "passWORD", len);
//			sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_PASSWORD, len, (unsigned char *)strpw);

			/* Restart the network processor */
			configValue = sl_Stop(0);
			configValue = sl_Start(NULL, NULL, NULL);
		}
		else if  (configValue == ROLE_AP) {
			/* Change mode to wireless station */
			configValue = sl_WlanSetMode(ROLE_STA);

			/* Restart the network processor */
			configValue = sl_Stop(0);
			configValue = sl_Start(NULL, NULL, NULL);
		}
    }

    // We are in Station Mode, so need to connect to Wifi Router/Access Point
    if (configValue == ROLE_STA)
    {
        // Turn LED to Red to indicate that device is attempting to connected to Wifi Router
        GPIO_write(Board_LED0, Board_LED_OFF); //Blue
        GPIO_write(Board_LED1, Board_LED_OFF); //Green
        GPIO_write(Board_LED2, Board_LED_ON); //Red

		/*
		 * Wait for the WiFi to connect to an AP. If a profile for the AP in
		 * use has not been stored yet, press Board_BUTTON0 to start SmartConfig.
		 */
		while ((deviceConnected != true) || (ipAcquired != true)) {
			/*
			 *  Start SmartConfig if a button is pressed. This could be done with
			 *  GPIO interrupts, but for simplicity polling is used to check the
			 *  button.
			 */

			currButton = GPIO_read(Board_BUTTON1);
			if((currButton == 0) && (prevButton != 0))
			{
				smartConfigFxn();
			}
			prevButton = currButton;
			Task_sleep(50);
		}
    }

    // Set the color of LED to indicate which mode we are in
    if (configValue == ROLE_STA)
    {
        // Turn LED to Blue to indicate that device is connected in Station Mode
        GPIO_write(Board_LED0, Board_LED_ON); //Blue
        GPIO_write(Board_LED1, Board_LED_OFF); //Green
        GPIO_write(Board_LED2, Board_LED_OFF); //Red
    }
    else
    {
        // Turn LED to Green to indicate that device is in AP mode
        GPIO_write(Board_LED0, Board_LED_OFF); //Blue
        GPIO_write(Board_LED1, Board_LED_ON); //Green
        GPIO_write(Board_LED2, Board_LED_OFF); //Red
    }

    /* Print IP address */
    sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO, &dhcpIsOn, &len, (unsigned char *)&ipV4);
    System_printf("CC3100 has connected to an AP and acquired an IP address.\n");
    System_printf("IP Address: %d.", SL_IPV4_BYTE(ipV4.ipV4, 3));
    System_printf("%d.", SL_IPV4_BYTE(ipV4.ipV4, 2));
    System_printf("%d.", SL_IPV4_BYTE(ipV4.ipV4, 1));
    System_printf("%d", SL_IPV4_BYTE(ipV4.ipV4, 0));
    System_printf("\n");
    System_flush();

    /* Create a UDP socket */
    slSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (slSocket < 0)
    {
        System_printf("Error: socket not created.");
        WiFi_close(handle);
        Task_exit();
    }

    memset((char *)&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(0);
    localAddr.sin_port = htons(UDPPORT);

    status = bind(slSocket, (const sockaddr *)&localAddr, sizeof(localAddr));
    if (status < 0)
    {
        System_printf("Error: bind failed.");
        close(slSocket);
        WiFi_close(handle);
        Task_exit();
    }

    // Stop and restarts the webserver - not needed
    sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
    sl_NetAppStart(SL_NET_APP_HTTP_SERVER_ID);

    // Uncomment when required during development and testing
    //deleteConfig();
    //deleteControl();
    //saveConfig();
    //createDefaultConfig();

    loadConfig(); // load the Configuration - protocolType, lightCount etc.

    // Setup SPI based on configuration settings.
    configureSPI();

    loadControl(127); // load the last saved control json data (which loads an effect)

    // Obtain the device name
    //sl_NetAppGet (SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, (unsigned char *)strlen((const char *)lightConfig.deviceName), lightConfig.deviceName);

    registermDNSService();

    /* Loop in case remote application terminates the connection */
    while (1)
    {
        FD_ZERO(&readSet);
        FD_SET(slSocket, &readSet);

        /* Set up 2.5sec timeout for select() - May want to reduce to 1/2sec */
        //memset(&timeout, 0, sizeof(timeval));
        timeout.tv_sec = 2; //TIMEOUT;
        timeout.tv_usec = 500000;

        /* Wait for the reply. If timeout, assume UDP packet dropped */
        selectRes = select(slSocket + 1, &readSet, NULL, NULL, &timeout);

        if ((selectRes > 0) && (selectRes != -1))
        {
            if(FD_ISSET(slSocket, &readSet))
            {
                nbytes = recvfrom(slSocket, buffer, UDPPACKETSIZE, 0,(sockaddr *)&client_addr, &addrlen);
                if (nbytes > 0)
                {
                	if (((uint8_t*)buffer)[0] == 0x41) // Raw or Control  or Config Packet
                	{
                		// Process the packet received
                		processStatus = processPacket(nbytes);
                	}
                	else if (((uint8_t*)buffer)[0] == 0x42) // Query Packet
                	{
                		 if (((uint8_t*)buffer)[3] == 0xFB) // Status Query Packet Response
                		 {
                			 char * statusJsonPacket = createStatusJSON();

							char statusPacket[120] = "";
							strcat(statusPacket, "E000000000");
							strcat(statusPacket, statusJsonPacket);

							const char sp[120];
							int spLen = strlen(statusPacket);
							memcpy((void *)sp, (const char *)statusPacket, spLen);

							bytesSent = sendto(slSocket, sp, spLen, 0, (sockaddr *)&client_addr, sizeof(client_addr));

							//statusPacket = asciiToHex(statusPacket, sizeof(statusPacket));
							//returnPacket = "450001FB000000000063\0"; //strcat("450001FB000000000063\0", asciiToHex(statusPacket, sizeof(statusPacket)));
							//sendto(slSocket, statusPacket, sizeof(statusPacket)-1, 0, (sockaddr *)&client_addr,sizeof(client_addr));
                		 }
                		 else if (((uint8_t*)buffer)[3] == 0xFA) // Config Query Packet Response
                		 {
								char * configJsonPacket = createConfigJSON(); // create the config query response packet

								char configPacket[140] = ""; // = "E000000000";
								strcat(configPacket, "E000000000");
								strcat(configPacket, configJsonPacket);

								const char cp[200];
								int cpLen = strlen(configPacket);
								memcpy((void *)cp, (const char *)configPacket, cpLen);

								bytesSent = sendto(slSocket, cp, cpLen, 0, (sockaddr *)&client_addr,sizeof(client_addr));
                		 }
                	}

                    if (processStatus == 1 || bytesSent >= 1)
                    {
                    	 //System_printf("Successfully processed the packet\n");
                    	 //System_flush();
                    }
                    else
                    {
                    	System_printf("Unknown packet received. Packet not processed\n");
                    	System_flush();
                    }
                }
            }
        }
        else if (selectRes == -1)
        {
            System_printf("Closing socket 0x%x.\n", slSocket);
            close(slSocket);
            WiFi_close(handle);
            Task_exit();
        }

        // Saves control data when requested from a HTTP POST from within the HTTP Callback
        if (saveControlTrigger)
        {
        	saveControl(1);
        	saveControlTrigger = 0;
        }

        // Saves config data when requested from a HTTP POST from within the HTTP Callback
        if (saveConfigTrigger)
        {
    		saveConfig();
    		loadConfig();
    	    //registermDNSService();
    		saveConfigTrigger = 0;
        }
    }
}


