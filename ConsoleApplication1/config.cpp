#include "config.h"

char sAppServiceURL[100], sAppUserName[50], sAppUserPass[50];

void AppLoadConfig()
{
	FILE *fconfig = fopen(sAppConfigFileName,"r");
	char b[100];
	int i = 0;

	if (!fconfig)
		exit(2);

	b[0] = '\0';
	while ((1 == fscanf(fconfig, "%c", b + i)) && (b[i] != ','))
		i++;
	if (b[i] != ',')// Exit if fisrt comma separated value was not found
		exit(2);
	b[i] = '\0';
	strcpy(sAppServiceURL, b);

	i = 0;
	while ((1 == fscanf(fconfig, "%c", b + i)) && (b[i] != ','))
		i++;
	if (b[i] != ',')// Exit if second comma separated value was not found
		exit(2);
	b[i] = '\0';
	strcpy(sAppUserName, b);

	i = 0;
	while ((1 == fscanf(fconfig, "%c", b + i)) && (b[i] != ',')) i++;
	b[i] = '\0';
	strcpy(sAppUserPass, b);

	fclose(fconfig);
}