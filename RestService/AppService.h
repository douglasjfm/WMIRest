#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "AppUsersHandler.h"

using namespace utility;
using namespace web::http::experimental::listener;

class AppService
{
	http_listener listener;
	AppUsersHandler users_handler;

public:

	bool on;
	AppService(utility::string_t url);
	void open();
	void close();

private:
	void AppService::AppHandlePost(http_request message);

	// Return "Server Ok" to the client when no url is provided
	// Turns off the server when hit localhost:12345/off
	// Set http error 404 when any other url is provided
	void AppService::AppHandleGet(http_request message);

	void AppService::AppHandleJSON(json::value obj, http_request message);

	bool AppService::AppHandleUserCreds(utility::string_t user_name, utility::string_t user_pass);
};