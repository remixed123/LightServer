/*
 * lsmdns.h
 *
 *  Created on: 30/09/2014
 *      Author: glenn
 */

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
