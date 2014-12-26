//*****************************************************************************
//
// Application Name        - LightServer
// File Name			   - lsparsefile.h
// Application Version     - 2.5.0
// Application Modify Date - 26th of December 2014
// Application Developer   - Glenn Vassallo
// Application Contact	   - contact@swiftsoftware.com.au
// Application Repository  - https://github.com/remixed123/LightServer
//
// Further Details:		   - Please view the details in lsparsefile.c
//
//*****************************************************************************

#ifndef LSPARSEFILE_H_
#define LSPARSEFILE_H_

void setProtocolType(uint8_t icType);
void setBitCount(uint8_t icType);
char * createConfigJSON();
char * createStatusJSON();
char * createControlJSON();
int extractConfigJSON();
int extractControlJSON();
char * getControlFileName(int sceneNumber);
int createDefaultConfig();
int createDefaultControl(int sceneNumber);
int deleteConfig();
int deleteControl(int sceneNumber);
int loadConfig();
int loadControl(int sceneNumber);
int saveConfig();
int saveControl(int sceneNumber);

#endif /* LSPARSEFILE_H_ */
