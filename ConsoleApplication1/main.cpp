#include "app.h"

// System values -
//array for the processes info
AppProcessInfo_t *aInfoProcesses;
DWORD cProcesses; //counter of processes

//Number of cores
int numProcessors;

// System values -
//overall mem and CPU values
SIZE_T ulTotalMemConsumed, ulTotalMemFree;
double overallCPUUsage;

void AppInitProcessCPUCalc(AppProcessInfo_t *process)
{
	FILETIME ftime, fsys, fuser;
	HANDLE hProcess;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&(process->lastCPU), &ftime, sizeof(FILETIME));

	hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION |
		PROCESS_VM_READ,
		FALSE, process->dwPid);
	
	if (!hProcess)
	{
		process->percentCPU = 0;
		wcscpy(process->szProcessName,TEXT("<AccessDenied>"));
		process->ulMemSet = 0;
		process->lastCPU.QuadPart = process->lastSysCPU.QuadPart = process->lastUserCPU.QuadPart = 0;
		return;
	}

	GetProcessTimes(hProcess, &ftime, &ftime, &fsys, &fuser);
	memcpy(&(process->lastSysCPU), &fsys, sizeof(FILETIME));
	memcpy(&(process->lastUserCPU), &fuser, sizeof(FILETIME));

	CloseHandle(hProcess);
}


void AppCPUCalcInit(AppProcessInfo_t *aInfoProcesses, DWORD c)
{
	SYSTEM_INFO sysInfo;
	unsigned i = 0;

	GetSystemInfo(&sysInfo);
	numProcessors = sysInfo.dwNumberOfProcessors;

	for (i = 0; i < c; i++)
	{
		AppInitProcessCPUCalc(&(aInfoProcesses[i]));
	}
}

double AppGetCPUPercentage(AppProcessInfo_t *process)
{
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user, ullPercent;
	double percent;
	HANDLE hProcess;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION |
		PROCESS_VM_READ,
		FALSE, process->dwPid);
	GetProcessTimes(hProcess, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));

	ullPercent.QuadPart = (sys.QuadPart - process->lastSysCPU.QuadPart) +
		(user.QuadPart - process->lastUserCPU.QuadPart);
	percent = (double) ullPercent.QuadPart /(now.QuadPart - process->lastCPU.QuadPart);
	percent /= numProcessors;

	return percent * 100;
}

// Get the process info for the process referred by processInfo->dwPid and stores in processInfo struct
void AppGetProcessInfo(AppProcessInfo_t *processInfo)
{
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processInfo->dwPid);
	if (NULL == hProcess)
		return;

	// Get the process name.
	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;
	
		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
			&cbNeeded))
		{
			GetModuleBaseName(hProcess, hMod, processInfo->szProcessName,
				sizeof(processInfo->szProcessName) / sizeof(TCHAR));
		}
	}

	// Get the process mem working set.
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		processInfo->ulMemSet = pmc.WorkingSetSize;
	}

	processInfo->percentCPU = AppGetCPUPercentage(processInfo);

	CloseHandle(hProcess);
}

void AppWait()
{
	clock_t t0 = clock();
	double dif = 0;
	printf("Waiting..\n");
	fflush(stdout);
	while (dif < 3.0)
		dif = (double)((clock() - t0) / CLOCKS_PER_SEC);
}

void AppGetOverallValues()
{
	PERFORMACE_INFORMATION perfInfo;
	unsigned i;

	GetPerformanceInfo(&perfInfo, sizeof(perfInfo));
	ulTotalMemFree = perfInfo.PhysicalAvailable;

	ulTotalMemConsumed = 0;
	overallCPUUsage = 0.0;
	for (i = 0; i < cProcesses; i++)
	{
		ulTotalMemConsumed += aInfoProcesses[i].ulMemSet;
		overallCPUUsage += aInfoProcesses[i].percentCPU;
	}
}

int main(void)
{
	// Get the list of process identifiers.
	DWORD aProcessesIds[1024], cbNeeded;
	unsigned int i, j;

	if (!EnumProcesses(aProcessesIds, sizeof(aProcessesIds), &cbNeeded))
	{
		return 1;
	}

	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	aInfoProcesses = (AppProcessInfo_t *) malloc(cProcesses*sizeof(AppProcessInfo_t));
	
	for (i = 0; i < cProcesses; i++)
		aInfoProcesses[i].dwPid = aProcessesIds[i];

	//Init only the readable process
	AppCPUCalcInit(aInfoProcesses, cProcesses);

	AppWait();

	for (i = 0; i < cProcesses; i++)
	{
		aInfoProcesses[i].dwPid = aProcessesIds[i];
		if (wcscmp(aInfoProcesses[i].szProcessName, TEXT("<AccessDenied>")) != 0)
			AppGetProcessInfo(&aInfoProcesses[i]);
	}

	AppGetOverallValues();
	AppSendJsonEncoded();

	free(aInfoProcesses);

	printf("Done..\n");
	fflush(stdout);

	return 0;
}