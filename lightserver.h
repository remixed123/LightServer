//*****************************************************************************
//
// Application Name        - LightServer
// File Name			   - lightserver.h
// Application Version     - 2.5.0
// Application Modify Date - 26th of December 2014
// Application Developer   - Glenn Vassallo
// Application Contact	   - contact@swiftsoftware.com.au
// Application Repository  - https://github.com/remixed123/LightServer
//
// Further Details:		   - Please view the details in lightserver.c
//
//*****************************************************************************

#ifndef LIGHTSERVER_H_
#define LIGHTSERVER_H_

/* XDCtools Header files */
#include <xdc/std.h>

/* JSON Library Headers */
#include "jsmn.h"
//#include "json.h"
//#include "buf.h"

/* SimpleLink Wi-Fi Host Driver Header files */
#include <simplelink.h>
#include <netapp.h>
//#include <netcfg.h>
//#include <osi.h>
//#include <fs.h>

 //*****************************************************************************
 // Version Define
 //*****************************************************************************
#define DEVICE_VERSION	"1.0.0"
#define DEVICE_MANUFACTURE	"GV"
#define DEVICE_MODEL	"LS"

 //*****************************************************************************
 // jsmn defines for JSON parsing
 //*****************************************************************************
#define fail()	return __LINE__ /* Terminate current test with error */
#define done() return 0 /* Successfull end of the test case */
#define check(cond) do { if (!(cond)) fail(); } while (0) /* Check single condition */

#define TOKEN_EQ(t, tok_start, tok_end, tok_type) \
	((t).start == tok_start \
	 && (t).end == tok_end  \
	 && (t).type == (tok_type))

#define TOKEN_STRING(js, t, s) \
	(strncmp(js+(t).start, s, (t).end - (t).start) == 0 \
	 && strlen(s) == (t).end - (t).start)

//#define TOKEN_PRINT(t) \
//	printf("start: %d, end: %d, type: %d, size: %d\n", \
//			(t).start, (t).end, (t).type, (t).size)

 //*****************************************************************************
 // SimpleLink/WiFi Defines
 //*****************************************************************************
#define UDPPORT         4048 /* Port number to which the connection is bound */
#define UDPPACKETSIZE   1024
#define SPI_BIT_RATE    14000000

//#define SL_MAC_ADDR_LEN      6

#define TIMEOUT 5

#define OOB_TASK_PRIORITY				(1)
#define OSI_STACK_SIZE					(1024)

#define ASSERT_ON_ERROR(line_number, error_code) \
            {\
                /* Handling the error-codes is specific to the application */ \
                if (error_code < 0) return error_code; \
                /* else, continue w/ execution */ \
            }

// //*****************************************************************************
// // mDNS Defines
// //*****************************************************************************
//#define MDNS_SERVICE  "._ddpr._udp.local"
//
//#define TTL             120
//#define UNIQUE_SERVICE  1       /* Set 1 for unique services, 0 otherwise */

// //*****************************************************************************
// // LightData Defines
// //*****************************************************************************
//#define zero false //0b0
//#define one  true //0b1
//
//#define CHECK_BIT(var, pos) !!((var) & (1 << (pos)))


 //*****************************************************************************
 // SPI Defines
 //*****************************************************************************
#define SSI0_BASE         0x40008000  // SSI0
#define SPI_MSG_LENGTH    10

 //*****************************************************************************
 // Server Event Defines
 //*****************************************************************************
/* Server Events */
#define SL_NETAPP_HTTPGETTOKENVALUE        1
#define SL_NETAPP_HTTPPOSTTOKENVALUE    2

#define SL_NETAPP_IPV4_ACQUIRED    1

 //*****************************************************************************
 // File operation defines
 //*****************************************************************************
#define BUF_SIZE        	1024
#define SIZE_200bytes       200
#define SIZE_250bytes       250

//union
//{
//    uint8_t g_Buf[BUF_SIZE];
//    uint32_t demobuf[BUF_SIZE/4];
//} uNvmemBuf;

extern char slconfigfilename []; // = "config.txt";
extern char slcontrol1filename []; // = "control1.txt";
extern char slcontrol2filename []; // = "control2.txt";
extern char slcontrol3filename []; // = "control3.txt";
extern char slcontrol4filename []; // = "control4.txt";
extern char slcontrol5filename []; // = "control5.txt";
extern char slcontrol6filename []; // = "control6.txt";
extern char slcontrolstoredfilename []; // = "controls.txt";


//*****************************************************************************
// LightServer Result/Error Type
//*****************************************************************************
typedef enum {
	LS_SUCCESS = 0,
	LS_ERROR = -1,
	LS_BIT_COUNT_NOT_SUPPORTED = -2,
	LS_UNKNOWN_ERROR = -100
} lsresult_t;

//*****************************************************************************
// LightServer Packet Type - either RAW data or CONTROL data
//*****************************************************************************
typedef enum {
	RAW = 0,
	CONTROL = 1
} packettype_t;

//*****************************************************************************
// Globals used by SPI
//*****************************************************************************
extern uint32_t masterRxBuffer[SPI_MSG_LENGTH];
extern UChar masterTxBuffer[SPI_MSG_LENGTH];

//*****************************************************************************
// Globals used in Raw Data Light Effect Creation
//*****************************************************************************
extern uint8_t  rawDataSize;
extern UChar * rawData;

//*****************************************************************************
// Globals used in JSON Data Creation and Storage
//*****************************************************************************
extern UChar controlJsonData[250];
extern UChar configJsonData[200];

//*****************************************************************************
// Globals used in Light Configuration
//*****************************************************************************

typedef enum {
	STANDARD = 0,
	MANCHESTER = 1,
} protocoltype_t;

typedef enum {
	WS8212 = 0,
	WS8211 = 1,
	TM1809 = 2,
	SPARE = 3, // To be used later
	CYT3005 = 4
} ictype_t;

typedef enum {
	RGB = 0,
	RBG = 1,
	BRG = 2,
	BGR = 3, // To be used later
	GBR = 4,
	GRB = 5,
} colororder_t;

typedef enum {
	GAMMA_ON = 0,
	GAMMA_OFF = 1,
} gammacorrection_t;

typedef struct {
	UChar deviceName[40]; // The device name given to the lightserver
	UChar oldDeviceName[40]; // The old device name, used when changing device name
	uint16_t lightCount; // Number of lights attached to the LightServer
	uint16_t byteCount; // byteCount = abs(((19+(39*lightCount))/8)+1);
	uint8_t bitCount; // The bit count for the lights
	protocoltype_t protocolType; // Type of protocol the chip uses
	ictype_t icType; // The Type of chip the lights use
	colororder_t colorOrder; // The order in which the colors receive data
	gammacorrection_t gammaCorrection; // Gamma correction
} LightConfiguration;

extern LightConfiguration lightConfig; // =  {"", "", 100, 490, 8, STANDARD, WS8212, GBR, GAMMA_ON};

//*****************************************************************************
// Globals used in Light Color Data Creation and Manipulation
//*****************************************************************************

// The light data that will be sent out by SPI
// 4878 = enough for 1000 CYT3005 ; 7804 - 1600 Lights ;
// 6828 - 1400 Lights ; 5853 - 1200 Lights ; 5366 - 1100 Lights
extern UChar lightData[5366];

typedef struct {
	uint16_t red[1100];
	uint16_t green[1100];
	uint16_t blue[1100];
} LightColorData;

extern LightColorData colorData; // Color data used to create lightData

typedef struct {
	uint8_t red[5];
	uint8_t green[5];
	uint8_t blue[5];
} LightColorsSelected;

extern LightColorsSelected colorSelected; // Color data which has been selected by the user

// Color data which has been selected by the user, and will be stored for later use.
// This may occur when random colors have been set, so we can still retrieve/return
// to the previously seleted data by the user.
extern LightColorsSelected colorSelectedStored;

typedef struct {
	uint16_t red[100];
	uint16_t green[100];
	uint16_t blue[100];
} LightColorEffects;

extern LightColorEffects colorEffects; // Color data used is effects

typedef struct {
	uint8_t red[25];
	uint8_t green[25];
	uint8_t blue[25];
} LightColorSelectMix;

// Color data that has been either selected or randomally generated and will
// be stored for later, used when randomally mixing colors about
extern LightColorSelectMix colorMix;

//*****************************************************************************
// Globals used in Lighting Effect Control and Creation
//*****************************************************************************

typedef struct {
	UChar fx[25]; // The effect name
	uint8_t intensity; // Brightness percentage
	uint8_t direction; // Direction; 0: normal ; 1:reversed
	int8_t tempDirection; //
	uint16_t effectSpeed;
	uint16_t speed; // Speed of the effect
	uint16_t tempSpeed;
	uint8_t colorCount; // The number of colors set to be used in an effect
	uint8_t tempColorCount; // Used to store various temporary values, usually associated with colorCount
	uint8_t actualColorCount; // Used to store the actual color count for long term storage
	uint8_t storedColorCount; // Used to store the actual color count for long term storage, even when randon colors create their own color count
	uint8_t randEffect; // Used when selecting a random effect
	uint8_t randomColor; //Used when wanted to create a new random color in the mix
} LightEffectControl;

extern LightEffectControl effectControl; // = {"Raw",100,0,0,20,500,500,1,1,1,1,0,0};

//*****************************************************************************
// Globals used in Lighting Effect Calculations
//*****************************************************************************

typedef struct {
	int8_t rotation; // Used to assist with effect creation, usually to loop an effect
	uint16_t light; // Used to assist with effect creation, used in the paintEffect and ...
	uint8_t blend;
	uint8_t blendDirection;
	uint8_t intensity[11]; // Used to assist with effect creation, usually creating a glow or other intensity related effects
	uint8_t intensityDirection[11]; // Used to assist with effect creation, usually creating a glow or other intensity related effects
	uint8_t glow[25]; // Used to assist with effect creation, usually creating a glow or other intensity related effects
	uint8_t glowDirection[25]; // Used to assist with effect creation, usually creating a glow or other intensity related effects
} LightEffectCalcs;

extern LightEffectCalcs effectCalc; // = {0,0,0,0,{50,100,150,175,200,250,200,175,150,100,50},{1,1,1,1,1,0,0,0,0,0,0}};


//*****************************************************************************
// lightServer.c function declarations
//*****************************************************************************
int beginEffectClock();

#endif /* LIGHTSERVER_H_ */
