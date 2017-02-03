#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define sAppConfigFileName "app.cfg"

void AppLoadConfig();

//Config variables
extern char sAppServiceURL[100], sAppUserName[50], sAppUserPass[50];

#endif