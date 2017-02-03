#include "stdafx.h"
#include "CppUnitTest.h"
#include <cpprest/http_listener.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace web;
using namespace web::http;
using namespace web::http::client;

#include <AppUsersHandler.h>
#include <AppService.h>

namespace RestServiceTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		/*AppService class tests*/

		/* Positive test for http GET Method.
		Service should response htttp OK when hit the address localhost:12345,
		and should turn off when hit the address localhost:12345/off */
		TEST_METHOD(PositiveAppHandleGetTest)
		{
			AppService server(L"http://localhost:12345");
			server.open();

			Assert::AreEqual(server.on,true);

			http_client client(L"http://localhost:12345");
			client.request(methods::GET).then([](http_response response)
			{
				Assert::IsTrue(response.status_code() == http::status_codes::OK);
			}).wait();

			client.request(methods::GET, L"off").then([&server](http_response response)
			{
				Assert::IsFalse(server.on);
			}).wait();

			server.close();
		}

		/* Negative test for http GET Method.
		Service should response htttp NotFound when hit the address localhost:12345/<whatever>,
		and should turn off when hit the address localhost:12345/off */
		TEST_METHOD(NegativeAppHandleGetTest)
		{
			AppService server(L"http://localhost:12345");
			server.open();

			Assert::AreEqual(server.on, true);

			http_client client(L"http://localhost:12345");
			client.request(methods::GET, L"error_url").then([](http_response response)
			{
				Assert::IsTrue(response.status_code() == http::status_codes::NotFound);
			}).wait();

			client.request(methods::GET, L"off").then([&server](http_response response)
			{
				Assert::IsFalse(server.on);
			}).wait();

			server.close();
		}

		TEST_METHOD(PositiveAppHandleUserAccessTest)
		{
			//Creating test users.txt file
			ofstream testfile("users.txt");
			//Adding 1 user: <email> <pass> <cpu_threshold> <mem_threshold>
			testfile << "user1@server.com qwerty 15 70\n";
			testfile.close();

			//Valid User JSON (fake process info)
			json::value jobj;
			jobj[L"sAppUserName"] = json::value::string(L"user1@server.com");
			jobj[L"sAppUserPass"] = json::value::string(L"qwerty");
			jobj[L"ulTotalMemConsumed"] = json::value::number(1234);
			jobj[L"ulTotalMemFree"] = json::value::number(1);
			jobj[L"overallCPUUsage"] = json::value::number(1);
			jobj[L"array"][0][L"dwPid"] = json::value::number(1);
			jobj[L"array"][0][L"szProcessName"] = json::value::string(L"AnyProcessName");
			jobj[L"array"][0][L"percentCPU"] = json::value::number(1);
			jobj[L"array"][0][L"ulMemSet"] = json::value::number(1234);

			AppService server(L"http://localhost:12345");
			server.open();

			Assert::AreEqual(server.on, true);

			http_client client(L"http://localhost:12345");
			client.request(methods::POST, L"", jobj.to_string().c_str(), L"application/json").then([&](http_response response)
			{
				//Test server response
				Assert::IsTrue(response.status_code() == http::status_codes::OK);
				//Test user log created
				FILE *testuserlog = fopen("user1@server.com.log", "r");
				Assert::IsTrue(testuserlog != NULL);
				fclose(testuserlog);
			}).wait();

			server.close();
		}

		TEST_METHOD(NegativeAppHandleUserAccessTest)
		{
			//Creating test users.txt file
			ofstream testfile("users.txt");
			//Adding 1 user: <email> <pass> <cpu_threshold> <mem_threshold>
			testfile << "user1@server.com qwerty 15 70\n";
			testfile.close();

			//Invalid User JSON (and fake process info)
			json::value jobj1;
			
			
			//Invalid pass user JSON (and fake process info)
			json::value jobj;
			jobj[L"sAppUserName"] = json::value::string(L"user1@server.com");
			jobj[L"sAppUserPass"] = json::value::string(L"wxyz");
			jobj[L"ulTotalMemConsumed"] = json::value::number(1234);
			jobj[L"ulTotalMemFree"] = json::value::number(1);
			jobj[L"overallCPUUsage"] = json::value::number(1);
			jobj[L"array"][0][L"dwPid"] = json::value::number(1);
			jobj[L"array"][0][L"szProcessName"] = json::value::string(L"AnyProcessName");
			jobj[L"array"][0][L"percentCPU"] = json::value::number(1);
			jobj[L"array"][0][L"ulMemSet"] = json::value::number(1234);

			AppService server(L"http://localhost:12345");
			server.open();

			Assert::AreEqual(server.on, true);

			http_client client(L"http://localhost:12345");
			client.request(methods::POST, L"", jobj.to_string().c_str(), L"application/json").then([&](http_response response)
			{
				//Test server response
				Assert::IsTrue(response.status_code() == http::status_codes::Unauthorized);
			}).wait();

			//Now testing a invalid user email
			jobj[L"sAppUserName"] = json::value::string(L"user2@server.com");
			jobj[L"sAppUserPass"] = json::value::string(L"qwerty");
			client.request(methods::POST, L"", jobj.to_string().c_str(), L"application/json").then([&](http_response response)
			{
				//Test server response
				Assert::IsTrue(response.status_code() == http::status_codes::Unauthorized);
			}).wait();

			server.close();
		}
		/*-------------------End tests------------------------------------------*?/

		/* 
			AppUserHandler class tests
		*/

		/*Positive test for getting users info.
		AppUsersHandler object should return the pass, cpu and mem limites,
		according to the data written in users.txt file*/
		TEST_METHOD(PositiveAppUsersHandlerGetUserInfo)
		{
			//Creating test users.txt file
			ofstream testfile("users.txt");
			//Adding 2 users: <email> <pass> <cpu_threshold> <mem_threshold>
			testfile << "user1@server.com qwerty 15 70\n";
			testfile << "user2@server.com asdfgh 60 50\n";
			testfile.close();

			AppUsersHandler *uhandler = new AppUsersHandler();
			string pass = uhandler->getUserPass("user1@server.com");
			double cpul = uhandler->getUserCPULim("user1@server.com");
			double meml = uhandler->getUserMemLim("user1@server.com");
			Assert::IsTrue(pass == "qwerty");
			Assert::IsTrue(cpul == 15);
			Assert::IsTrue(meml == 70);


			pass = uhandler->getUserPass("user2@server.com");
			cpul = uhandler->getUserCPULim("user2@server.com");
			meml = uhandler->getUserMemLim("user2@server.com");
			Assert::IsTrue(pass == "asdfgh");
			Assert::IsTrue(cpul == 60);
			Assert::IsTrue(meml == 50);
			uhandler->~AppUsersHandler();
		}

		/*Negative test for getting users info.
		AppUsersHandler object should return the 'n/a' for not found user email,
		-1 for cpu and mem limites of a not found user, according to the data written in users.txt file*/
		TEST_METHOD(NegativeAppUsersHandlerGetUserInfo)
		{
			//Creating test users.txt file
			ofstream testfile("users.txt");
			//Adding 2 users: <email> <pass> <cpu_threshold> <mem_threshold>
			testfile << "user1@server.com qwerty 15 70\n";
			testfile << "user2@server.com asdfgh 60 50\n";
			testfile.close();

			AppUsersHandler *uhandler = new AppUsersHandler();
			string pass = uhandler->getUserPass("user3@server.com");
			double cpul = uhandler->getUserCPULim("user3@server.com");
			double meml = uhandler->getUserMemLim("user3@server.com");
			Assert::IsTrue(pass == "n/a");
			Assert::IsTrue(cpul < 0);
			Assert::IsTrue(meml < 0);
			uhandler->~AppUsersHandler();
		}
		/*-------------------End tests------------------------------------------*/

	};
}