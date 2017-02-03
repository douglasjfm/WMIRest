#include "stdafx.h"
#include "CppUnitTest.h"
#include "app.h"
#include "config.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
void AppGetOverallValues();
void AppGetProcessInfo(AppProcessInfo_t *processInfo);
void AppCPUCalcInit(AppProcessInfo_t *aInfoProcesses, DWORD c);
void AppWait();

//array for the processes info from main.c
extern AppProcessInfo_t *aInfoProcesses;
extern DWORD cProcesses; //counter of processes

//Extern int from main.c
extern int numProcessors;

//overall mem and CPU values from main.c
extern SIZE_T ulTotalMemConsumed, ulTotalMemFree;
extern double overallCPUUsage;

namespace ConsoleApplication1Test
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		/*
			Performs a test by retrieving the process info for the current process.
			Note that for a successfull test the process must be a user process,
			and the current process is (probably) a user process
		*/
		TEST_METHOD(PositiveAppGetProcessInfoTest)
		{
			AppProcessInfo_t p;

			p.dwPid = GetCurrentProcessId();
			AppCPUCalcInit(&p,1);

			Assert::IsTrue(numProcessors > 0);
			Assert::IsTrue(p.lastCPU.QuadPart > 0);
			Assert::IsTrue(p.lastSysCPU.QuadPart > 0);
			Assert::IsTrue(p.lastUserCPU.QuadPart > 0);

			AppWait();
			AppGetProcessInfo(&p);

			Assert::AreNotEqual(TEXT("<AccessDenied>"), p.szProcessName);
			Assert::AreNotEqual(p.percentCPU, 0, 0.00001);
			Assert::IsTrue(p.ulMemSet > 0);
		}

		/*
		Performs a test by (in this case, trying to) retrieving the process info for the Windows Idle process.
		Note that for a successfull test the process must be a user process (code level privileges access),
		and the Idle process is system process
		*/
		TEST_METHOD(NegativeAppGetProcessInfoTest)
		{
			AppProcessInfo_t p;

			p.dwPid = 0;
			AppCPUCalcInit(&p, 1);

			Assert::IsTrue(p.lastCPU.QuadPart == 0);
			Assert::IsTrue(p.lastSysCPU.QuadPart == 0);
			Assert::IsTrue(p.lastUserCPU.QuadPart == 0);

			AppGetProcessInfo(&p);

			Assert::AreEqual(TEXT("<AccessDenied>"), p.szProcessName);
			Assert::IsTrue(p.ulMemSet == 0);
		}

		/*
		Test get overall values in a situation with only a user processes
		*/
		TEST_METHOD(PositiveAppGetOverallValuesTest)
		{
			AppProcessInfo_t p;

			p.dwPid = GetCurrentProcessId();
			AppCPUCalcInit(&p, 1);
			AppWait();
			AppGetProcessInfo(&p);

			aInfoProcesses = &p;
			cProcesses = 1;

			AppGetOverallValues();

			Assert::AreEqual(p.ulMemSet, ulTotalMemConsumed);
			Assert::IsTrue(ulTotalMemFree > 0);
			Assert::AreEqual(p.percentCPU, overallCPUUsage, 0.000001);
		}

		/*
		Test get overall values in a situation with only a system processes
		*/
		TEST_METHOD(NegativeAppGetOverallValuesTest)
		{
			AppProcessInfo_t p;

			p.dwPid = 0;
			AppCPUCalcInit(&p, 1);
			AppWait();
			AppGetProcessInfo(&p);

			aInfoProcesses = &p;
			cProcesses = 1;

			AppGetOverallValues();

			Assert::AreEqual(p.ulMemSet, (SIZE_T) 0);
			Assert::IsTrue(ulTotalMemFree > 0);
			Assert::AreEqual(p.percentCPU, overallCPUUsage, 0.000001);
		}
	};
}