//*****************************************************************************
//
// Application Name        - LightServer
// File Name			   - lsmdns.c
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
// File Overview		   - lsmdns.c contains functions to create and register
//							 a mDNS service
//
// Application Details     - https://github.com/remixed123/LightServer/readme.txt
//
//*****************************************************************************

#include "lightserver.h"
#include "lsmdns.h"

#include <stdio.h>

/* XDCtools Header files */
#include <xdc/runtime/System.h>

//*****************************************************************************
// Globals used by mDNS
//*****************************************************************************
static char mdnsServiceName[40] = "";
static char mdnsText[70] = "";

//*****************************************************************************
//! getMacAddress
//!
//! Returns the MAC Address as string
//!
//****************************************************************************
char * getMacAddress()
{
	int i;

	unsigned char macAddressVal[SL_MAC_ADDR_LEN];
	unsigned char macAddressLen = SL_MAC_ADDR_LEN;

	sl_NetCfgGet(SL_MAC_ADDRESS_GET,NULL,&macAddressLen,(unsigned char *)macAddressVal);

	char macAddressPart[2];
	static char macAddressFull[18]; //18

	for (i = 0 ; i < 6 ; i++)
	{
		sprintf(macAddressPart, "%02X", macAddressVal[i]);
		strcat(macAddressFull, (char *)macAddressPart);
		strcat(macAddressFull, ":");
	}

	macAddressFull[17] = '\0'; // Replace the the last : with a zero termination

	return macAddressFull;
}



//*****************************************************************************
//! registermDNSService
//!
//! Registers the mDNS Service
//!
//!
//****************************************************************************
int registermDNSService()
{
	uint8_t iretvalmDNS;
	uint8_t i;

	// Obtain the device name
	sl_NetAppGet(SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, (unsigned char *)strlen((const char *)lightConfig.deviceName), lightConfig.deviceName);
	//sl_NetAppGet(SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, strlen((const char *)lightConfig.deviceName), lightConfig.deviceName);


	// Convert  to ASCII
	char  lightCountChar[4];
	sprintf(lightCountChar, "%d", lightConfig.lightCount);
	char  icTypeChar[1];
	sprintf(icTypeChar, "%d", lightConfig.icType);
	char  protocolTypeChar[1];
	sprintf(protocolTypeChar, "%d", lightConfig.protocolType);
	char  colorTypeChar[1];
	sprintf(colorTypeChar, "%d", lightConfig.colorOrder);
	char  bitCountChar[2];
	sprintf(bitCountChar, "%d", lightConfig.bitCount);

	// Create mDNS Service Name
	for (i = 0; i < 40; i++)
		mdnsServiceName[i] = 0x00;

	strcat(mdnsServiceName, (const char *)lightConfig.deviceName);
	strcat(mdnsServiceName, MDNS_SERVICE);

	// Create mDNS Text
	for (i = 0; i < 70; i++)
		mdnsText[i] = 0x00;

	char * macAddress = getMacAddress();

	strcat(mdnsText, "mac=");
	strcat(mdnsText, macAddress);
	strcat(mdnsText, ";ver=");
	strcat(mdnsText, DEVICE_VERSION);
	strcat(mdnsText, ";man=");
	strcat(mdnsText, DEVICE_MANUFACTURE);
	strcat(mdnsText, ";mod=");
	strcat(mdnsText, DEVICE_MODEL);
	strcat(mdnsText, ";l=");
	strcat(mdnsText, (const char *)lightCountChar);
	strcat(mdnsText, ";it=");
	strcat(mdnsText, (const char *)icTypeChar);
	strcat(mdnsText, ";pt=");
	strcat(mdnsText, (const char *)protocolTypeChar);
	strcat(mdnsText, ";ct=");
	strcat(mdnsText, (const char *)colorTypeChar);
	strcat(mdnsText, ";bc=");
	strcat(mdnsText, (const char *)bitCountChar);
	strcat(mdnsText, "\0");

	int strSvrLength = strlen(mdnsServiceName);
	int strTxtLength = strlen(mdnsText);

	//Registering for the mDNS service.
	iretvalmDNS = sl_NetAppMDNSUnRegisterService((const signed char *)mdnsServiceName,strlen(mdnsServiceName));

	iretvalmDNS = sl_NetAppMDNSRegisterService((const signed char *)mdnsServiceName,strlen(mdnsServiceName),
			(const signed char *)mdnsText,strlen(mdnsText)+1, UDPPORT, TTL, UNIQUE_SERVICE);

	if(iretvalmDNS == 0)
	{
		System_printf("MDNS Registration successful\n");
	}
	else
	{
		System_printf("MDNS Registered failed\n");

	}
	System_flush();

	return iretvalmDNS;
}
