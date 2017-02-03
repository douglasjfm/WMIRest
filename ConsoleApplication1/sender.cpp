#include "app.h"
#include "config.h"

using namespace web;
using namespace web::http;
using namespace web::http::client;

// System values -
//array for the processes info
extern AppProcessInfo_t *aInfoProcesses;
extern DWORD cProcesses; //counter of processes

//Number of cores
extern int numProcessors;

// System values -
//overall mem and CPU values
extern SIZE_T ulTotalMemConsumed, ulTotalMemFree;
extern double overallCPUUsage;

http_response AppSendJsonEncoded()
{
	json::value jsonData;
	unsigned i;
	http_response serverResponse;

	AppLoadConfig();

	jsonData[L"ulTotalMemConsumed"] = json::value::number((uint64_t)ulTotalMemConsumed);
	jsonData[L"ulTotalMemFree"] = json::value::number((uint64_t)ulTotalMemFree);
	jsonData[L"overallCPUUsage"] = json::value::number(overallCPUUsage);
	jsonData[L"sAppUserName"] = json::value::string(utility::conversions::to_string_t(sAppUserName));
	jsonData[L"sAppUserPass"] = json::value::string(utility::conversions::to_string_t(sAppUserPass));
	
	for (i = 0; i < cProcesses; i++)
	{
		jsonData[L"array"][i][L"dwPid"] = json::value::number((unsigned)aInfoProcesses[i].dwPid);
		jsonData[L"array"][i][L"szProcessName"] = json::value::string(aInfoProcesses[i].szProcessName);
		jsonData[L"array"][i][L"percentCPU"] = json::value::number(aInfoProcesses[i].percentCPU);
		jsonData[L"array"][i][L"ulMemSet"] = json::value::number((uint64_t)aInfoProcesses[i].ulMemSet);
	}

	uri urlAppService;

	try
	{
		const size_t cSize = strlen(sAppServiceURL) + 1;
		wchar_t* wsAppServiceURL = new wchar_t[cSize];
		mbstowcs(wsAppServiceURL, sAppServiceURL, cSize);

		web::uri_builder validateUri(U("http://localhost"));
		validateUri.set_host((wchar_t*)wsAppServiceURL);
		validateUri.set_port(12345);
		urlAppService = validateUri.to_uri();
	}
	catch (std::exception& e)
	{
		exit(3);
	}

	http_client client(urlAppService);
	client.request(methods::POST,L"",jsonData.to_string().c_str(),L"application/json").then([&serverResponse](http_response response)
	{
		std::wcout << response.status_code() << std::endl;
		serverResponse = response;
	}).wait();

	return serverResponse;
}