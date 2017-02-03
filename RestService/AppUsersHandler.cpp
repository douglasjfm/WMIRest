#include "AppUsersHandler.h"
#include <time.h>
#include <curl/curl.h>

AppUsersHandler::AppUsersHandler()
{
	load();
}


AppUsersHandler::~AppUsersHandler()
{
	user_pass.clear();
	user_mem_lim.clear();
	user_cpu_lim.clear();
}

void AppUsersHandler::load()
{
	string pass, email;
	double cpu_lim, mem_lim;
	ifstream infile("users.txt");

	while (infile >> email >> pass >> cpu_lim >> mem_lim)
	{
		user_pass[email] = pass;
		user_cpu_lim[email] = cpu_lim;
		user_mem_lim[email] = mem_lim;
	}
	infile.close();
}

string AppUsersHandler::getUserPass(string email)
{
	std::map<string, string>::iterator it;
	it = user_pass.find(email);
	if (it != user_pass.end())
		return user_pass[email];
	return "n/a";
}

double AppUsersHandler::getUserCPULim(string email)
{
	std::map<string, double>::iterator it;
	it = user_cpu_lim.find(email);
	if (it != user_cpu_lim.end())
		return user_cpu_lim[email];
	return -1;
}

double AppUsersHandler::getUserMemLim(string email)
{
	std::map<string, double>::iterator it;
	it = user_mem_lim.find(email);
	if (it != user_mem_lim.end())
		return user_mem_lim[email];
	return -1;
}

void AppUsersHandler::userLogAppend(string email, string log)
{
	fstream flog;
	time_t systime;
	flog.open(email + ".log", fstream::out | fstream::app);

	time(&systime);
	string strtime(ctime(&systime));
	flog << strtime << endl;
	flog << log + "\n\n";
	
	flog.close();
}

void AppUsersHandler::sendUserAlert(string msg, string email)
{
	//TODO
}