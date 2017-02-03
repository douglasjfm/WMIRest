#include <iostream>
#include <thread>
#include <chrono>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "AppUsersHandler.h"

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

class AppService
{
	http_listener listener;
	AppUsersHandler users_handler;

	public:
		
		bool on;

		AppService(utility::string_t url) : listener(url), users_handler()
		{
			on = true;
			listener.support(methods::POST, std::bind(&AppService::AppHandlePost, this, std::placeholders::_1));
			listener.support(methods::GET, std::bind(&AppService::AppHandleGet, this, std::placeholders::_1));
		}

		void open() { listener.open().wait(); }
		void close() { listener.close(); }

	private:
		void AppService::AppHandlePost(http_request message)
		{
			message.extract_json().then([&](pplx::task<json::value> previousTask)
			{
				try
				{
					json::value const & json = previousTask.get();
					AppHandleJSON(json, message);
				}
				catch (http_exception const & e)
				{
					std::wcout << e.what() << std::endl;
				}
			}).wait();
		}

		// Return "Server Ok" to the client when no url is provided
		// Turns off the server when hit localhost:12345/off
		// Set http error 404 when any other url is provided
		void AppService::AppHandleGet(http_request message)
		{
			auto get_uri = message.request_uri().to_string();
			std::wcout << L"GET: " + get_uri << std::endl;

			if (get_uri == L"/off")
			{
				message.reply(web::http::status_codes::OK, L"Server turning off...");
				on = false;
			}
			else if (get_uri == L"/")
				message.reply(web::http::status_codes::OK, L"Server Ok");
			else
				message.reply(web::http::status_codes::NotFound);
		}

		void AppService::AppHandleJSON(json::value obj, http_request message)
		{
			utility::string_t sAppUserName = obj[L"sAppUserName"].as_string();
			utility::string_t sAppUserPass = obj[L"sAppUserPass"].as_string();

			if (!AppService::AppHandleUserCreds(sAppUserName, sAppUserPass))
			{
				message.reply(web::http::status_codes::Unauthorized, L"Invalid user credentials for: " + sAppUserName);
			}
			else
			{
				std::cout << "ok1" << std::endl;
				double userCurrentCPU = obj[L"overallCPUUsage"].as_double();
				uint64_t ulUserMemConsumed = obj[L"ulTotalMemConsumed"].as_integer();
				uint64_t ulUserMemFree = obj[L"ulTotalMemFree"].as_integer();

				double cpuUserLimit = users_handler.getUserCPULim(utility::conversions::to_utf8string(sAppUserName));
				double memUserLimit = users_handler.getUserMemLim(utility::conversions::to_utf8string(sAppUserName));

				if (userCurrentCPU > cpuUserLimit)
					users_handler.sendUserAlert("CPU Limit Alert", utility::conversions::to_utf8string(sAppUserName));
				if (ulUserMemConsumed / (ulUserMemConsumed + ulUserMemFree) > memUserLimit)
					users_handler.sendUserAlert("Memory Limit Alert", utility::conversions::to_utf8string(sAppUserName));
				std::cout << "ok2" << std::endl;
				std::string log = "CPU Usage: " + std::to_string(userCurrentCPU)+ "\n";
				log += "Memory consumed: " + std::to_string(ulUserMemConsumed) + "\n";
				log += "Memory available: " + std::to_string(ulUserMemFree) + "\n";
				users_handler.userLogAppend(utility::conversions::to_utf8string(sAppUserName), log);
				std::cout << "ok3\n" << std::endl;
				message.reply(web::http::status_codes::OK, L"OK " + sAppUserName);
			}
		}

		bool AppService::AppHandleUserCreds(utility::string_t user_name, utility::string_t user_pass)
		{
			utility::string_t pass = utility::conversions::to_string_t(
				users_handler.getUserPass(utility::conversions::to_utf8string(user_name)));
			return (user_pass == pass);
		}
};

int wmain()
{
	AppService server(L"http://localhost:12345");
	server.open();
	
	while (server.on) {}

	server.close();

	system("pause");
}