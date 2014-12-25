/*
 * lsparsefile.h
 *
 *  Created on: 30/09/2014
 *      Author: glenn
 */

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
