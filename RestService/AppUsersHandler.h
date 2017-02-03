#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

class AppUsersHandler
{
private:
	map<string, string> user_pass;
	map<string, double> user_cpu_lim;
	map<string, double> user_mem_lim;

	void load();

public:
	string getUserPass(string email);
	double getUserMemLim(string email);
	double getUserCPULim(string email);
	void userLogAppend(string email, string log);
	void sendUserAlert(string msg, string email);

	AppUsersHandler();
	~AppUsersHandler();
};

