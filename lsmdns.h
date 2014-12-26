//*****************************************************************************
//
// Application Name        - LightServer
// File Name			   - lsmdns.h
// Application Version     - 2.5.0
// Application Modify Date - 26th of December 2014
// Application Developer   - Glenn Vassallo
// Application Contact	   - contact@swiftsoftware.com.au
// Application Repository  - https://github.com/remixed123/LightServer
//
// Further Details:		   - Please view the details in lsmdns.c
//
//*****************************************************************************

#ifndef LSMDNS_H_
#define LSMDNS_H_

 //*****************************************************************************
 // mDNS Defines
 //*****************************************************************************
#define MDNS_SERVICE  "._ddpr._udp.local"
#define TTL             120
#define UNIQUE_SERVICE  1       /* Set 1 for unique services, 0 otherwise */

char * getMacAddress();
int registermDNSService();

#endif /* LSMDNS_H_ */
