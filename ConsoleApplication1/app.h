#ifndef APP_H_
#define APP_H_

#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <psapi.h>
#include <tchar.h>

#include <cpprest/http_client.h>
#include <cpprest/json.h>

// Structture to retrieve process info
typedef struct AppProcessInfo_t
{
	DWORD dwPid;
	TCHAR szProcessName[30];
	double percentCPU;
	SIZE_T ulMemSet;

	//Used for calc the cpu percentage
	ULARGE_INTEGER lastCPU;
	ULARGE_INTEGER lastSysCPU;
	ULARGE_INTEGER lastUserCPU;

}AppProcessInfo_t;

//function for sending tha date to the server
web::http::http_response AppSendJsonEncoded();

#endif