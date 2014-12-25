/*
 * lsparsefile.c
 *
 *  Created on: 30/09/2014
 *      Author: glenn
 */

#include "lightserver.h"
#include "lsparsefile.h"
#include "lscolors.h"
#include "lsspiconfig.h"
#include "lsutils.h"
#include "lsmdns.h"
#include "lsrandomeffect.h"

/* Standard Header files */
#include <stdio.h>
#include <stdlib.h>

#include "Board.h"

//*****************************************************************************
//! setProtocolType
//!
//! Sets the protocol type based on the chip type
//!
//! Returns:
//*****************************************************************************
void setProtocolType(uint8_t icType)
{
	if (icType == 4)
		lightConfig.protocolType = MANCHESTER;
	else
		lightConfig.protocolType = STANDARD;
}

//*****************************************************************************
//! setBitCount
//!
//! Sets the bit count value based on the chip type
//!
//! Returns:
//*****************************************************************************
void setBitCount(uint8_t icType)
{
	if (icType == 4)
		lightConfig.bitCount = 12;
	else
		lightConfig.bitCount = 8;
}

//*****************************************************************************
//! createConfigJSON
//!
//! Create the config JSON data from current device and global values
//!
//! Returns: The json config char *
//*****************************************************************************
char * createConfigJSON()
{
	int i;
	char ssid[32] = "";

	// Obtain the AP SSID Name
	unsigned short len = 32;
	unsigned short  config_opt = WLAN_AP_OPT_SSID;
	sl_WlanGet(SL_WLAN_CFG_AP_ID, &config_opt , &len, (unsigned char*)ssid);

	// Obtain the device name
	//sl_NetAppGet (SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, strlen((const char *)lightConfig.deviceName), lightConfig.deviceName);
	sl_NetAppGet(SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, (unsigned char *)strlen((const char *)lightConfig.deviceName), lightConfig.deviceName);

	// Convert lightCount to ASCII
	  char  lightCountChar[5];
	  sprintf(lightCountChar, "%d", lightConfig.lightCount);
	  char  icTypeChar[1];
	  sprintf(icTypeChar, "%d", lightConfig.icType);
	  char  protocolTypeChar[1];
	  sprintf(protocolTypeChar, "%d", lightConfig.protocolType);
	  char  colorTypeChar[1];
	  sprintf(colorTypeChar, "%d", lightConfig.colorOrder);
	  char  bitCountChar[2];
	  sprintf(bitCountChar, "%d", lightConfig.bitCount);

	// Create DDP response packet
	static char configPacket[140] = "";

	for (i = 0; i < 140; i++)
		configPacket[i] = 0x00;

	strcat(configPacket, "{\"config\":{\"SSID\":\"");
	strcat(configPacket, ssid);
	strcat(configPacket, "\",\"name\":\"");
	strcat(configPacket, (const char *)lightConfig.deviceName);
	strcat(configPacket, "\",\"ports\":[{\"l\":");
	strcat(configPacket, (const char *)lightCountChar);
	strcat(configPacket, ",\"it\":");
	strcat(configPacket, (const char *)icTypeChar);
	strcat(configPacket, ",\"pt\":");
	strcat(configPacket, (const char *)protocolTypeChar);
	strcat(configPacket, ",\"ct\":");
	strcat(configPacket, (const char *)colorTypeChar);
	strcat(configPacket, ",\"bc\":");
	strcat(configPacket, (const char *)bitCountChar);
	strcat(configPacket, "}]}}");
	strcat(configPacket, "\0");

	return configPacket;
}


//*****************************************************************************
//! createStatusJSON
//!
//! Create the status JSON data
//!
//! Returns: The json status char *
//*****************************************************************************
char * createStatusJSON()
{
	int i;

	static char statusPacket[120] = "";

	for (i = 0; i < 120; i++)
		statusPacket[i] = 0x00;

	char * macAddress = getMacAddress();

	//{\"status\":{\"mac\":\"001ec01a8b6\",\"man\":\"GV\",\"mod\":\"LightServer\",\"push\":1,\"ver\":\"1.0.0\"}}";

	strcat(statusPacket, "{\"status\":{\"mac\":\"");
	strcat(statusPacket, macAddress);
	strcat(statusPacket, "\",\"man\":\"");
	strcat(statusPacket, DEVICE_MANUFACTURE);
	strcat(statusPacket, "\",\"mod\":\"");
	strcat(statusPacket, DEVICE_MODEL);
	strcat(statusPacket, "\",\"ver\":\"");
	strcat(statusPacket, DEVICE_VERSION);
	strcat(statusPacket, "\"}}");
	strcat(statusPacket, "\0");

	return statusPacket;
}


//*****************************************************************************
//! createControlJSON
//!
//! Create the control JSON data from current global values
//!
//! Returns: The json control char *
//*****************************************************************************
char * createControlJSON()
{
	int i;

	// Convert globals to ASCII
//	char  fxChar[15];
//	sprintf(fxChar, "%d", fx);
	char  speedChar[3];
	sprintf(speedChar, "%d", effectControl.speed);
	char  intensityChar[3];
	sprintf(intensityChar, "%d", effectControl.intensity);
	char  directionChar[1];
	sprintf(directionChar, "%d", effectControl.direction);

	static char controlPacket[250] = "";

	for (i = 0; i < 250; i++)
		controlPacket[i] = 0x00;

	//"{\"control\":{\"fx\":\"Waves\",\"int\":100,\"dir\":0,\"spd\":50,\"colors\":[{\"r\":255,\"g\":0,\"b\":0},{\"r\":0,\"g\":255,\"b\":0},{\"r\":0,\"g\":0,\"b\":255}]}}\0";

	strcat(controlPacket, "{\"control\":{\"fx\":\"");
	strcat(controlPacket, (char *)effectControl.fx);
	strcat(controlPacket, "\",\"int\":\"");
	strcat(controlPacket, intensityChar);
	strcat(controlPacket, "\",\"dir\":\"");
	strcat(controlPacket, directionChar);
	strcat(controlPacket, "\",\"spd\":\"");
	strcat(controlPacket, speedChar);
	strcat(controlPacket, "\",\"colors\":[");

	for (i = 0; i < effectControl.actualColorCount; i++)
	{
		char  redPartChar[15];
		sprintf(redPartChar, "%d", colorSelected.red[i]);
		char  greenPartChar[15];
		sprintf(greenPartChar, "%d", colorSelected.green[i]);
		char  bluePartChar[15];
		sprintf(bluePartChar, "%d", colorSelected.blue[i]);

		strcat(controlPacket, "{\"r\":");
		strcat(controlPacket, (const char *)redPartChar);
		strcat(controlPacket, ",\"g\":");
		strcat(controlPacket, (const char *)greenPartChar);
		strcat(controlPacket, ",\"b\":");
		strcat(controlPacket, (const char *)bluePartChar);
		strcat(controlPacket, "}");

		if (i < (effectControl.actualColorCount - 1))
		{
			strcat(controlPacket, ",");
		}
	}
	strcat(controlPacket, "]}}");
	strcat(controlPacket, "\0");

	return controlPacket;
}


//*****************************************************************************
//! extractConfigJSON
//!
//! This function extracts the JSON Config data and assigns the values to global variables
//!
//!
//*****************************************************************************
int extractConfigJSON()
{
	int i;
	const char *js;
	UChar keyString[200];
	UChar keyStringValue[200];

	js = (const char *)configJsonData;

    jsmn_parser p;
    jsmntok_t tokens[50];
    memset(&tokens, 0, sizeof(tokens));
    jsmn_init(&p);
    int err = jsmn_parse(&p, js, strlen(js), tokens, 50);

	for (i = 1; tokens[i].end != 0 && tokens[i].end < tokens[0].end; i++)
	{
		jsmntok_t key = tokens[i];
		jsmntok_t keyValue = tokens[i+1];

		unsigned int length = key.end - key.start;
		unsigned int lengthValue = keyValue.end - keyValue.start;

		memcpy(keyString, &js[key.start], length);
		keyString[length] = '\0';

		memcpy(keyStringValue, &js[keyValue.start], lengthValue);
		keyStringValue[lengthValue] = '\0';

//		System_printf("%i\n", i);
//		System_printf("Key: %s\n", keyString);
//		System_printf("tokens[0].end: %i\n", tokens[0].end);
//		System_printf("tokens[%i].end: %i\n",i, tokens[i].end);
//		System_flush();

		if (tokens[i].type == JSMN_STRING || tokens[i].type == JSMN_PRIMITIVE)
		{
			//Make sure the temporary values are not over written due to only sending one setting change
			effectControl.colorCount = effectControl.actualColorCount; // added to ensure use the actual color count

			if (strcmp((const char *)keyString, "name") == 0)
			{
				memcpy(lightConfig.deviceName, &js[keyValue.start], lengthValue);
				lightConfig.deviceName[lengthValue] = '\0';

				// Code to change the device name

				// Retrieve the current devicename and stored in a temp, so we can unregister it
				//sl_NetAppGet(SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, strlen((const char *)lightConfig.oldDeviceName), lightConfig.oldDeviceName);
				sl_NetAppGet(SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, (unsigned char *)strlen((const char *)lightConfig.deviceName), lightConfig.deviceName);

				// Change the name and remove old name if we have a new name
//				if (strcmp((const char *)lightConfig.deviceName, (const char *)lightConfig.oldDeviceName) != 0)
//				{
//					//Change the device name
//					sl_NetAppSet (SL_NET_APP_DEVICE_CONFIG_ID,
//					NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN,
//					strlen((const char *)lightConfig.deviceName),
//					(unsigned char *) lightConfig.deviceName);
//
//					// Create mDNS Service Name
//					for (i = 0; i < 40; i++)
//						mdnsServiceName[i] = 0x00;
//
//					strcat(mdnsServiceName, (const char *)lightConfig.oldDeviceName);
//					strcat(mdnsServiceName, MDNS_SERVICE);
//
//					//Unregistering for the mDNS service.
//					sl_NetAppMDNSUnRegisterService((const signed char *)mdnsServiceName,strlen(mdnsServiceName));
//					//sl_NetAppMDNSUnRegisterService(NULL, NULL); // Delete all
//				}
			}
			if (strcmp((const char *)keyString, "l") == 0)
			{
				lightConfig.lightCount = atoi((const char *)keyStringValue);
			}
			if (strcmp((const char *)keyString, "it") == 0)
			{
				lightConfig.icType = (ictype_t)atoi((const char *)keyStringValue);
				configureSPI();

				if (lightConfig.icType <= 3)
					lightConfig.bitCount = 8;
				else
					lightConfig.bitCount = 12;
			}
			if (strcmp((const char *)keyString, "pt") == 0)
			{
				lightConfig.protocolType = (protocoltype_t)atoi((const char *)keyStringValue);
			}
			if (strcmp((const char *)keyString, "ct") == 0)
			{
				lightConfig.colorOrder = (colororder_t)atoi((const char *)keyStringValue);
			}
			if (strcmp((const char *)keyString, "bc") == 0)
			{
				// See icType - this has been done to get around bug in release code of LightKit
				//lightConfig.bitCount = atoi((const char *)keyStringValue);
			}
			//System_printf("%.*s\n", tokens[i].end - tokens[i].start, js + tokens[i].start);
			//System_flush();
		}
		else if (tokens[i].type == JSMN_ARRAY)
		{
//			System_printf("[%d elems]\n", tokens[i].size);
//			System_flush();
		}
		else if (tokens[i].type == JSMN_OBJECT)
		{
//			System_printf("{%d elems}\n", tokens[i].size);
//			System_flush();
		}
		else
		{
			//TOKEN_PRINT(tokens[i]);
		}
	}

	// Register the updated values in mDNS
	registermDNSService();

	return 1;
}

//*****************************************************************************
//! extractControlJSON
//!
//! This function extracts the JSON Control data and assigns the values to global variables
//!
//!
//*****************************************************************************

int extractControlJSON()
{
	int i;
	const char *js;
	UChar keyString[200];
	UChar keyStringValue[200];

	js = (const char *)controlJsonData;

//	js = "{\"control\":{\"fx\":\"Waves\",\"int\":100,\"spd\":50,\"colors\":[{\"r\":255,\"g\":67,\"b\":133}]}}";

    jsmn_parser p;
    jsmntok_t tokens[50];
    memset(&tokens, 0, sizeof(tokens));
    jsmn_init(&p);
    int err = jsmn_parse(&p, js, strlen(js), tokens, 50);

	for (i = 1; tokens[i].end != 0 && tokens[i].end < tokens[0].end; i++)
	{
		jsmntok_t key = tokens[i];
		jsmntok_t keyValue = tokens[i+1];

		unsigned int length = key.end - key.start;
		unsigned int lengthValue = keyValue.end - keyValue.start;

		memcpy(keyString, &js[key.start], length);
		keyString[length] = '\0';

		memcpy(keyStringValue, &js[keyValue.start], lengthValue);
		keyStringValue[lengthValue] = '\0';

//		System_printf("%i\n", i);
//		System_printf("Key: %s\n", keyString);
//		System_printf("tokens[0].end: %i\n", tokens[0].end);
//		System_printf("tokens[%i].end: %i\n",i, tokens[i].end);
//		System_flush();

		if (tokens[i].type == JSMN_STRING || tokens[i].type == JSMN_PRIMITIVE)
		{
			//Make sure the temporary values are not over written due to only sending one setting change
			effectControl.colorCount = effectControl.actualColorCount; // added to ensure use the actual color count

			if (strcmp((const char *)keyString, "fx") == 0)
			{
				effectControl.randEffect = 0; // we are not selecting a random effect, so set to 0

				memcpy(effectControl.fx, &js[keyValue.start], lengthValue);
				effectControl.fx[lengthValue] = '\0';

				if(memcmp(effectControl.fx, "Auto", 4) == 0) // We need to create some random colors
				{
				    createRandomColorsHSV();
				}
			}
			else if (strcmp((const char *)keyString, "int") == 0)
			{
				int intensityValue = atoi((const char *)keyStringValue); //hex2intMultiByte((const char *)keyStringValue);
				effectControl.intensity = intensityValue;
			}
			else if (strcmp((const char *)keyString, "dir") == 0)
			{
				int directionValue = atoi((const char *)keyStringValue);
				effectControl.direction = directionValue;
			}
			else if (strcmp((const char *)keyString, "spd") == 0)
			{
				int speedValue = atoi((const char *)keyStringValue);
				effectControl.speed = speedValue;
			}
			else if (strcmp((const char *)keyString, "sav") == 0)
			{
				int savValue = atoi((const char *)keyStringValue);
				if (savValue > 0)
				{
					saveControl(savValue); // Save the control data (scene) to flash
					savValue = 0;
					break; // We only send save by its self, so we may as well break here
				}
			}
			else if (strcmp((const char *)keyString, "run") == 0)
			{
				int runValue = atoi((const char *)keyStringValue);
				if (runValue > 0)
				{
					loadControl(runValue); // Load and run the last saved effect
					runValue = 0;
					break; // So as to avoid problems caused after the recursive call to extractControlJSON from within loadControl
				}
			}
			else if (strcmp((const char *)keyString, "rc") == 0)
			{
				int runValue = atoi((const char *)keyStringValue);
				if (runValue > 0)
				{
					createRandomColorsHSV(); // Create random colors
				}
			}
			else if (strcmp((const char *)keyString, "re") == 0)
			{
				int runValue = atoi((const char *)keyStringValue);
				if (runValue > 0)
				{
					selectRandomEffect(); // Choose a random effect
				}
			}

			else if (strcmp((const char *)keyString, "colors") == 0)
			{
				int colorElementCount = tokens[i+1].size;
				effectControl.colorCount = colorElementCount;
				if (effectControl.colorCount > 0)
				{
					effectControl.actualColorCount = effectControl.colorCount;
					effectControl.storedColorCount = effectControl.actualColorCount;
				}
				int j,k;
				j = 0;
				int lastElement = (colorElementCount*7)+i;
				for (k = i; k < lastElement; k+=7)
				{
					//int testRed = atoi(js + tokens[k+4].start);
					colorSelected.red[j] = atoi(js + tokens[k+4].start);
					colorSelectedStored.red[j] = colorSelected.red[j];
					colorSelected.green[j] = atoi(js + tokens[k+6].start);
					colorSelectedStored.green[j] = colorSelected.green[j];
					colorSelected.blue[j] = atoi(js + tokens[k+8].start);
					colorSelectedStored.blue[j] = colorSelected.blue[j];

					j+=1;
				}
				for (k = 0; k < 25; k++)
				{
					j = randRange(0, (effectControl.actualColorCount - 1));
					// Store colors for later use, this is used when randomally mixing colors in different positions
					colorMix.red[k] = colorSelected.red[j];
					colorMix.green[k] = colorSelected.green[j];
					colorMix.blue[k] = colorSelected.blue[j];
				}
			}
			//System_printf("%.*s\n", tokens[i].end - tokens[i].start, js + tokens[i].start);
			//System_flush();
		}
		else if (tokens[i].type == JSMN_ARRAY)
		{
//			System_printf("[%d elems]\n", tokens[i].size);
//			System_flush();
		}
		else if (tokens[i].type == JSMN_OBJECT)
		{
//			System_printf("{%d elems}\n", tokens[i].size);
//			System_flush();
		}
		else
		{
			//TOKEN_PRINT(tokens[i]);
		}
	}

	return 1;
}

//*****************************************************************************
//! getControlFileName
//!
//! Returns the filename for the control file for the selected scene
//!
//*****************************************************************************
char * getControlFileName(int sceneNumber)
{
	if (sceneNumber == 1)
		return slcontrol1filename;
	else if (sceneNumber == 2)
		return slcontrol2filename;
	else if (sceneNumber == 3)
		return slcontrol3filename;
	else if (sceneNumber == 4)
		return slcontrol4filename;
	else if (sceneNumber == 5)
		return slcontrol5filename;
	else if (sceneNumber == 6)
		return slcontrol6filename;
	else if (sceneNumber == 127)
		return slcontrolstoredfilename;
	else
		return slcontrol1filename;
}

//*****************************************************************************
//! createDefaultConfig
//!
//! Creates the default config and saves to serial flash.
//! This should only be run once at first start up.
//!
//*****************************************************************************
int createDefaultConfig()
{
    int32_t         fileHandle = -1;
    uint32_t        Token = 0;
    int32_t         retVal = 0;

    int i;
	char ssid[32] = "";

	// Obtain the AP SSID Name
	unsigned short len = 32;
	unsigned short  config_opt = WLAN_AP_OPT_SSID;
	sl_WlanGet(SL_WLAN_CFG_AP_ID, &config_opt , &len, (unsigned char*)ssid);

	// Obtain the device name
	//sl_NetAppGet (SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, strlen((const char *)lightConfig.deviceName), lightConfig.deviceName);
	sl_NetAppGet(SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, (unsigned char *)strlen((const char *)lightConfig.deviceName), lightConfig.deviceName);

	// l = lightCount ; it = icType ; pt = protocolType ; ct = colorType ; bc = bitCount
	static char configJsonDataDefault[200] = "";

	for (i = 0; i < 200; i++)
		configJsonDataDefault[i] = 0x00;

	strcat(configJsonDataDefault, "{\"config\":{\"SSID\":\"");
	strcat(configJsonDataDefault, ssid);
	strcat(configJsonDataDefault, "\",\"name\":\"");
	strcat(configJsonDataDefault, (const char *)lightConfig.deviceName);
	strcat(configJsonDataDefault, "\",\"ports\":[{\"l\":50,\"it\":0,\"pt\":0,\"ct\":5,\"bc\":8}]}}\0");

	/* File Doesn't exit create a new of 200 byte file */
	retVal = sl_FsOpen((unsigned char *)&slconfigfilename,
					   FS_MODE_OPEN_CREATE(SIZE_200bytes,_FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE),
					   (unsigned long *)&Token, (signed long *)&fileHandle);
	if(retVal < 0)
	{
		// Turn LED to Magenta to indicate an error has occured
		GPIO_write(Board_LED0, Board_LED_ON); //Blue
		GPIO_write(Board_LED1, Board_LED_OFF); //Green
		GPIO_write(Board_LED2, Board_LED_ON); //Red
		return -1;
	}

    /* write config file to serial flash */
	retVal = sl_FsWrite(fileHandle, 0, (uint8_t *)configJsonDataDefault, sizeof(configJsonDataDefault));
	if (retVal < 0)
	{
		retVal = sl_FsClose(fileHandle, 0, 0, 0);
		//retVal = sl_FsDel((unsigned char *)&slconfigfilename, Token);

		// Turn LED to Magenta to indicate an error has occured
		GPIO_write(Board_LED0, Board_LED_ON); //Blue
		GPIO_write(Board_LED1, Board_LED_OFF); //Green
		GPIO_write(Board_LED2, Board_LED_ON); //Red;
		return -1;
	}

    /* close the user file */
    retVal = sl_FsClose(fileHandle, 0, 0, 0);
    if (retVal < 0)
    {
        //retVal = sl_FsDel((unsigned char *)&slconfigfilename, Token);
        return -1;
    }

    // Read the config data into the global variable to now use
    for (i = 0; i < 200; i++)
    {
    	configJsonData[i] = configJsonDataDefault[i];
    }

    return 0;
}

//*****************************************************************************
//! createDefaultControl
//!
//! Creates the default control json and saves to serial flash.
//! This should only be run once at first start up.
//!
//*****************************************************************************
int createDefaultControl(int sceneNumber)
{
    int32_t         fileHandle = -1;
    uint32_t        Token = 0;
    int32_t         retVal = 0;

     char controlJsonDataDefault[250] = "{\"control\":{\"fx\":\"Glow Mix\",\"int\":100,\"dir\":0,\"spd\":80,\"colors\":[{\"r\":255,\"g\":255,\"b\":0},{\"r\":0,\"g\":255,\"b\":255},{\"r\":255,\"g\":0,\"b\":255}]}}\0";

 	char * slcontrolfilename = getControlFileName(sceneNumber);

	/* File Doesn't exit create a new of 200 byte file */
	retVal = sl_FsOpen((unsigned char *)&slcontrolfilename,
					   FS_MODE_OPEN_CREATE(SIZE_250bytes,_FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE),
					   (unsigned long *)&Token, (signed long *)&fileHandle);
	if(retVal < 0)
	{
		// Turn LED to Magenta to indicate an error has occured
		GPIO_write(Board_LED0, Board_LED_ON); //Blue
		GPIO_write(Board_LED1, Board_LED_OFF); //Green
		GPIO_write(Board_LED2, Board_LED_ON); //Red
		return -1;
	}

    /* write config file to serial flash */
	retVal = sl_FsWrite(fileHandle, 0, (uint8_t *)controlJsonDataDefault, sizeof(controlJsonDataDefault));
	if (retVal < 0)
	{
		retVal = sl_FsClose(fileHandle, 0, 0, 0);
		//retVal = sl_FsDel((unsigned char *)&slconfigfilename, Token);

		// Turn LED to Magenta to indicate an error has occured
		GPIO_write(Board_LED0, Board_LED_ON); //Blue
		GPIO_write(Board_LED1, Board_LED_OFF); //Green
		GPIO_write(Board_LED2, Board_LED_ON); //Red;
		return -1;
	}

    /* close the user file */
    retVal = sl_FsClose(fileHandle, 0, 0, 0);
    if (retVal < 0)
    {
        //retVal = sl_FsDel((unsigned char *)&slconfigfilename, Token);
        return -1;
    }

    return 0;
}


//*****************************************************************************
//! deleteConfig
//!
//! Deletes the config stored on the serial flash.
//! Used in testing, should never run in live application
//! May use for resetting settings
//!
//*****************************************************************************
int deleteConfig()
{
    int32_t         retVal = 0;
    uint32_t       Token = 0;

	/* Delete the user file */
    retVal = sl_FsDel((unsigned char *)&slconfigfilename, Token);
    if(retVal < 0)
    {
		// Turn LED to Red to indicate an error has occured
		GPIO_write(Board_LED0, Board_LED_ON); //Blue
		GPIO_write(Board_LED1, Board_LED_ON); //Green
		GPIO_write(Board_LED2, Board_LED_ON); //Red
		return -1;
    }

    return 0;
}

//*****************************************************************************
//! deleteControl
//!
//! Deletes the control json stored on the serial flash.
//! Used in testing, should never run in live application
//! May use for resetting settings
//!
//*****************************************************************************
int deleteControl(int sceneNumber)
{
    int32_t         retVal = 0;
    uint32_t       Token = 0;

	char * slcontrolfilename = getControlFileName(sceneNumber);

	/* Delete the user file */
    retVal = sl_FsDel((unsigned char *)&slcontrolfilename, Token);
    if(retVal < 0)
    {
		// Turn LED to Red to indicate an error has occured
		//GPIO_write(Board_LED0, Board_LED_ON); //Blue
		//GPIO_write(Board_LED1, Board_LED_ON); //Green
		//GPIO_write(Board_LED2, Board_LED_ON); //Red

    	// Most likely the file does not exist
		return -1;
    }

    return 0;
}


//*****************************************************************************
//! loadConfig
//!
//! Loads the config stored on the serial flash
//!
//!
//*****************************************************************************
int loadConfig()
{
    int32_t         fileHandle = -1;
    uint32_t        Token = 0;
    int32_t         retVal = 0;

    uint8_t i;

	long readSize = SIZE_200bytes;
	unsigned char readBuff[200];

    /* open a user file for reading */
    retVal = sl_FsOpen((unsigned char *)&slconfigfilename,
                       FS_MODE_OPEN_READ, (unsigned long *)&Token, (signed long *)&fileHandle);
    if (retVal < 0)
    {
        /* File Doesn't exit create a new 200 byte file */
    	createDefaultConfig();
    	extractConfigJSON();
        return 2;
    }

    /* read the data from serial flash */
	retVal = sl_FsRead(fileHandle, (uint32_t)0, &readBuff[0], readSize);

	if ( (retVal < 0) ||(retVal != sizeof(configJsonData)) )
	{
		retVal = sl_FsClose(fileHandle, 0, 0, 0);
		//retVal = sl_FsDel((unsigned char *)&slconfigfilename, Token);

		return -1;
	}

	for (i = 0; i < readSize; i++)
	{
		configJsonData[i] = readBuff[i];
	}

    /* close the user file */
    retVal = sl_FsClose(fileHandle, 0, 0, 0);
    if (retVal < 0)
    {
        //retVal = sl_FsDel((unsigned char *)&slconfigfilename, Token);

        // Turn LED to Magenta to indicate an error has occured
        GPIO_write(Board_LED0, Board_LED_ON); //Blue
        GPIO_write(Board_LED1, Board_LED_OFF); //Green
        GPIO_write(Board_LED2, Board_LED_ON); //Red
        return -1;
    }

    extractConfigJSON();

    return 0;
}

//*****************************************************************************
//! loadControl
//!
//! Loads the config stored on the serial flash
//!
//!
//*****************************************************************************
int loadControl(int sceneNumber)
{
    int32_t         fileHandle = -1;
    uint32_t        Token = 0;
    int32_t         retVal = 0;

    uint8_t i;

	long readSize = SIZE_250bytes;
	unsigned char readBuff[250];

	char * slcontrolfilename = getControlFileName(sceneNumber);

    /* open a user file for reading */
    retVal = sl_FsOpen((unsigned char *)&slcontrolfilename,
                       FS_MODE_OPEN_READ, (unsigned long *)&Token, (signed long *)&fileHandle);
    if (retVal < 0)
    {
        /* File Doesn't exit create a new 200 byte file */
    	createDefaultControl(sceneNumber);
        retVal = sl_FsOpen((unsigned char *)&slcontrolfilename,
                           FS_MODE_OPEN_READ, (unsigned long *)&Token, (signed long *)&fileHandle);
        //return -1;
    }

    /* read the data from serial flash */
	retVal = sl_FsRead(fileHandle, (uint32_t)0, &readBuff[0], readSize);

	if ( (retVal < 0) ||(retVal != sizeof(controlJsonData)) )
	{
		retVal = sl_FsClose(fileHandle, 0, 0, 0);
		//retVal = sl_FsDel((unsigned char *)&slconfigfilename, Token);

		return -1;
	}

	for (i = 0; i < readSize; i++)
	{
		controlJsonData[i] = readBuff[i];
	}

    /* close the user file */
    retVal = sl_FsClose(fileHandle, 0, 0, 0);
    if (retVal < 0)
    {
        //retVal = sl_FsDel((unsigned char *)&slconfigfilename, Token);

        // Turn LED to Magenta to indicate an error has occured
        GPIO_write(Board_LED0, Board_LED_ON); //Blue
        GPIO_write(Board_LED1, Board_LED_OFF); //Green
        GPIO_write(Board_LED2, Board_LED_ON); //Red
        return -1;
    }

    extractControlJSON();
    beginEffectClock(); // Starts the effect

    return 0;
}


//*****************************************************************************
//! saveConfig
//!
//! Saves the latest json config  to the serial flash
//!
//!
//*****************************************************************************
int saveConfig()
{
    int32_t         fileHandle = -1;
    uint32_t        Token = 0;
    int32_t         retVal = 0;
    int i;

    char * configJson = createConfigJSON(); // create the config json

    int jsonSize = strlen((const char *)configJson);

	for (i = 0; i < jsonSize; i++)
	{
		configJsonData[i] = configJson[i];
	}
	configJsonData[i++] = 0x00;

    /* open a user file for writing */
    retVal = sl_FsOpen((unsigned char *)&slconfigfilename, FS_MODE_OPEN_WRITE, (unsigned long *)&Token, (signed long *)&fileHandle);

    if(retVal < 0)
    {
        /* File Doesn't exist create a new 200 byte file */
    	createDefaultConfig();
    	return -1;
    }

    /* write config file to serial flash */
	retVal = sl_FsWrite(fileHandle, 0, (uint8_t *)configJsonData, sizeof(configJsonData));

	if (retVal < 0)
	{
		retVal = sl_FsClose(fileHandle, 0, 0, 0);
		retVal = sl_FsDel((unsigned char *)&slconfigfilename, Token);

		// Turn LED to Magenta to indicate an error has occured
		GPIO_write(Board_LED0, Board_LED_ON); //Blue
		GPIO_write(Board_LED1, Board_LED_OFF); //Green
		GPIO_write(Board_LED2, Board_LED_ON); //Red;
		return -1;
	}

    /* close the user file */
    retVal = sl_FsClose(fileHandle, 0, 0, 0);
    if (retVal < 0)
    {
        retVal = sl_FsDel((unsigned char *)&slconfigfilename, Token);
        //turnLedOn(LED1);
        return -1;
    }

    return 0;
}

//*****************************************************************************
//! saveControl
//!
//! Saves the latest json control data to the serial flash
//!
//!
//*****************************************************************************
int saveControl(int sceneNumber)
{
    int32_t         fileHandle = -1;
    uint32_t        Token = 0;
    int32_t         retVal = 0;
    int i;

    char * controlJson = createControlJSON(); // create the config json

    int jsonSize = strlen((const char *)controlJson);

	for (i = 0; i < jsonSize; i++)
	{
		controlJsonData[i] = controlJson[i];
	}
	controlJsonData[i++] = 0x00;

	char * slcontrolfilename = getControlFileName(sceneNumber);

    /* open a user file for writing */
    retVal = sl_FsOpen((unsigned char *)&slcontrolfilename, FS_MODE_OPEN_WRITE, (unsigned long *)&Token, (signed long *)&fileHandle);

    if(retVal < 0)
    {
        /* File Doesn't exist create a new 250 byte file */
    	createDefaultControl(sceneNumber);
    	return -1;
    }

    /* write config file to serial flash */
	retVal = sl_FsWrite(fileHandle, 0, (uint8_t *)controlJsonData, sizeof(controlJsonData));

	if (retVal < 0)
	{
		retVal = sl_FsClose(fileHandle, 0, 0, 0);
		retVal = sl_FsDel((unsigned char *)&slcontrolfilename, Token);

		// Turn LED to Magenta to indicate an error has occured
		GPIO_write(Board_LED0, Board_LED_ON); //Blue
		GPIO_write(Board_LED1, Board_LED_OFF); //Green
		GPIO_write(Board_LED2, Board_LED_ON); //Red;
		return -1;
	}

    /* close the user file */
    retVal = sl_FsClose(fileHandle, 0, 0, 0);
    if (retVal < 0)
    {
        retVal = sl_FsDel((unsigned char *)&slcontrolfilename, Token);
        //turnLedOn(LED1);
        return -1;
    }

    return 0;
}

