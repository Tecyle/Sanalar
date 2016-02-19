#include "stdafx.h"
#include <stdlib.h>
#include "sanalar_process.h"
#include <Psapi.h>

#pragma comment(lib, "psapi.lib")

typedef LONG (WINAPI *FP_NtSuspendProcess)(HANDLE hProcess);
typedef LONG (WINAPI *FP_NtResumeProcess)(HANDLE hProcess);

FP_NtSuspendProcess _fnNtSuspendProcess = NULL;
FP_NtResumeProcess _fnNtResumeProcess = NULL;
HMODULE hNtDll = NULL;

static void _InitExternalFunc()
{
	hNtDll = LoadLibrary(L"ntdll.dll");
	_fnNtSuspendProcess = (FP_NtSuspendProcess)GetProcAddress(hNtDll, "NtSuspendProcess");
	_fnNtResumeProcess = (FP_NtResumeProcess)GetProcAddress(hNtDll, "NtResumeProcess");
}

void _ReleaseExternalFunc()
{
	FreeLibrary(hNtDll);
}

static LONG NtSuspendProcess(HANDLE hProcess)
{
	if (!_fnNtSuspendProcess)
		_InitExternalFunc();
	return _fnNtSuspendProcess(hProcess);
}

static LONG NtResumeProcess(HANDLE hProcess)
{
	if (!_fnNtResumeProcess)
		_InitExternalFunc();
	return _fnNtResumeProcess(hProcess);
}


namespace Sanalar
{


	Process::Process() :
		m_state(ProcessState_init),
		m_hProcess(NULL),
		m_returnCode(0),
		m_pid(0),
		m_pProcessEntry(NULL)
	{
	}

	Process::~Process()
	{
		CloseHandle(m_hProcess);
		delete m_pProcessEntry;
	}

	Process* Process::Shell(const wchar_t* cmdLine, bool suspend /*= false*/)
	{
		if (!cmdLine || !*cmdLine)
			return NULL;

		DWORD flag = suspend ? CREATE_SUSPENDED : 0;
		wchar_t* cmd = _wcsdup(cmdLine);
		STARTUPINFO si = { 0 };
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi;

		if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, flag, NULL, NULL, &si, &pi))
		{
			free(cmd);
			return NULL;
		}

		Process* process = new Process;
		process->m_hProcess = pi.hProcess;
		process->m_pid = pi.dwProcessId;
		process->m_state = suspend ? ProcessState_suspend : ProcessState_running;
		CloseHandle(pi.hThread);

		return process;
	}

	Process* Process::Open(DWORD pid)
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		if (hProcess)
		{
			Process* process = new Process;
			process->m_hProcess = hProcess;
			process->m_pid = pid;
			process->m_state = ProcessState_running;
			
			return process;
		}
		return NULL;
	}

	bool Process::Suspend()
	{
		if (m_hProcess)
		{
			if (NtSuspendProcess(m_hProcess) >= 0)
			{
				m_state = ProcessState_suspend;
				return true;
			}
		}
		return false;
	}

	bool Process::Resume()
	{
		if (m_hProcess)
		if (NtResumeProcess(m_hProcess) >= 0)
		{
			m_state = ProcessState_running;
			return true;
		}
		return false;
	}

	bool Process::Terminate()
	{
		if (m_hProcess)
		{
			if (TerminateProcess(m_hProcess, 4))
			{
				m_state = ProcessState_finished;
				return true;
			}
		}
		return false;
	}

	bool Process::Wait(unsigned long timeLimited /*= 0*/)
	{
		timeLimited = timeLimited ? timeLimited : INFINITE;
		if (GetProcessState() == ProcessState_running || GetProcessState() == ProcessState_suspend)
		{
			if (WaitForSingleObject(m_hProcess, timeLimited) == WAIT_OBJECT_0)
			{
				m_state = ProcessState_finished;
				return true;
			}
		}
		return false;
	}

	bool Process::IsActive()
	{
		ProcessState state = GetProcessState();
		return state == ProcessState_running || state == ProcessState_suspend;
	}

	bool Process::IsRunning()
	{
		return GetProcessState() == ProcessState_running;
	}

	bool Process::IsSuspended()
	{
		return GetProcessState() == ProcessState_suspend;
	}

	bool Process::IsFinished()
	{
		return GetProcessState() == ProcessState_finished;
	}

	bool Process::ExceptionHappened()
	{
		return GetProcessState() == ProcessState_finished && GetReturnCode() != 0;
	}

	// TODO 现有公开的 Windows API 中，暂无方法获取一个进程是否处于挂起状态
	ProcessState Process::GetProcessState()
	{
		if (!m_hProcess)
		{
			m_state = ProcessState_init;
		}
		else
		{
			if (WaitForSingleObject(m_hProcess, 0) == WAIT_OBJECT_0)
			{
				m_state = ProcessState_finished;
			}
		}
		return m_state;
	}

	int Process::GetReturnCode()
	{
		if (m_hProcess && GetExitCodeProcess(m_hProcess, &m_returnCode))
			return (int)m_returnCode;
		return (int)STILL_ACTIVE;
	}

	UINT64 Process::GetRunningTime(ProcessRunningTimeType timeType /*= ProcessRunningTimeType_all*/) const
	{
		if (!m_hProcess)
			return 0u;

		FILETIME startTime, endTime, kernelTime, userTime;
		UINT64 kRunTime, uRunTime;

		if (!GetProcessTimes(m_hProcess, &startTime, &endTime, &kernelTime, &userTime))
			return 0u;

		kRunTime = (UINT64)kernelTime.dwHighDateTime << 32 | (UINT64)kernelTime.dwLowDateTime;
		uRunTime = (UINT64)userTime.dwHighDateTime << 32 | (UINT64)userTime.dwLowDateTime;
		kRunTime /= 10000;
		uRunTime /= 10000;
		
		switch (timeType)
		{
		case ProcessRunningTimeType_all:
			return kRunTime + uRunTime;
		case ProcessRunningTimeType_core:
			return kRunTime;
		case ProcessRunningTimeType_user:
			return uRunTime;
		default:
			return 0u;
		}
	}

	UINT8 Process::GetCpuUsage(size_t interval /*= 100*/)
	{
		static DWORD cpuNum = 0;
		if (!cpuNum)
		{
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			cpuNum = sysInfo.dwNumberOfProcessors;
		}

		if (!m_hProcess || GetProcessState() == ProcessState_finished)
			return 0u;


		FILETIME ftLastTime, ftNowTime;
		UINT64 lastProcessTime, nowProcessTime;
		UINT64 lastTime, nowTime;
		lastProcessTime = GetRunningTime();
		GetSystemTimeAsFileTime(&ftLastTime);

		Sleep(interval);

		nowProcessTime = GetRunningTime();
		GetSystemTimeAsFileTime(&ftNowTime);

		lastTime = (UINT64)ftLastTime.dwHighDateTime << 32 | (UINT64)ftLastTime.dwLowDateTime;
		nowTime = (UINT64)ftNowTime.dwHighDateTime << 32 | (UINT64)ftNowTime.dwLowDateTime;

		return (UINT8)((nowProcessTime - lastProcessTime) * 100 / ((nowTime - lastTime) / 10000));
	}

	size_t Process::GetMaxMemoryUsed() const
	{
		if (!m_hProcess)
			return 0u;

		PROCESS_MEMORY_COUNTERS pmc = { 0 };
		if (!GetProcessMemoryInfo(m_hProcess, &pmc, sizeof(pmc)))
			return 0u;
		return pmc.PeakWorkingSetSize;
	}

	size_t Process::GetMemoryUsed() const
	{
		if (!m_hProcess)
			return 0u;

		PROCESS_MEMORY_COUNTERS pmc = { 0 };
		if (!GetProcessMemoryInfo(m_hProcess, &pmc, sizeof(pmc)))
			return 0u;
		return pmc.WorkingSetSize;
	}

	size_t Process::GetProcessName(wchar_t* fileName, size_t fileNameBufferSize)
	{
		if (!CreateProcessEntry())
			return 0;

		size_t nameLen = 0;
		wchar_t* str = NULL;
		for (int i = (int)wcslen(m_pProcessEntry->szExeFile) - 1; i >= 0; --i)
		{
			str = &m_pProcessEntry->szExeFile[i];
			nameLen++;
			if (*str == L'\\' || *str == L'/')
			{
				break;
			}
		}
		if (fileName && nameLen && nameLen <= fileNameBufferSize)
			wcscpy_s(fileName, fileNameBufferSize, str + 1);
		return nameLen;
	}

	bool Process::CreateProcessEntry()
	{
		if (!m_hProcess)
			return false;

		if (!m_pProcessEntry)
		{
			HANDLE hTHSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hTHSnapshot == INVALID_HANDLE_VALUE)
				return false;

			PROCESSENTRY32W pe32 = { 0 };
			pe32.dwSize = sizeof(pe32);
			if (!Process32FirstW(hTHSnapshot, &pe32))
			{
				CloseHandle(hTHSnapshot);
				return false;
			}
			do
			{
				if (pe32.th32ProcessID == GetId())
				{
					m_pProcessEntry = new PROCESSENTRY32W;
					memcpy_s(m_pProcessEntry, sizeof(PROCESSENTRY32W), &pe32, sizeof(pe32));
					break;
				}
			} while (Process32NextW(hTHSnapshot, &pe32));
			CloseHandle(hTHSnapshot);
			if (!m_pProcessEntry)
				return false;
		}
		return true;
	}

	size_t Process::GetProcessLocation(wchar_t* filePath, size_t filePathBufferSize)
	{
		if (!CreateProcessEntry())
			return 0;

		size_t nameLen = 0;
		wchar_t* str = NULL;
		int i;
		for (i = (int)wcslen(m_pProcessEntry->szExeFile) - 1; i >= 0; --i)
		{
			str = &m_pProcessEntry->szExeFile[i];
			if (*str == L'\\' || *str == L'/')
			{
				break;
			}
		}
		nameLen = i + 2;
		if (filePath && nameLen && nameLen <= filePathBufferSize)
		{
			wcsncpy_s(filePath, filePathBufferSize, m_pProcessEntry->szExeFile, nameLen - 1);
			filePath[nameLen - 1] = 0;
		}
		return nameLen;
	}

	DWORD Process::GetId()
	{
		if (!m_hProcess)
			return 0;

		if (!m_pid)
			m_pid = GetProcessId(m_hProcess);
		
		return m_pid;
	}

	ProcessPriority Process::GetPriority() const
	{
		if (!m_hProcess)
			return ProcessPriority_undefined;
		switch (GetPriorityClass(m_hProcess))
		{
		case ABOVE_NORMAL_PRIORITY_CLASS:
			return ProcessPriority_above;
		case BELOW_NORMAL_PRIORITY_CLASS:
			return ProcessPriority_below;
		case NORMAL_PRIORITY_CLASS:
			return ProcessPriority_normal;
		case REALTIME_PRIORITY_CLASS:
			return ProcessPriority_realTime;
		case HIGH_PRIORITY_CLASS:
			return ProcessPriority_high;
		case IDLE_PRIORITY_CLASS:
			return ProcessPriority_low;
		default:
			return ProcessPriority_undefined;
		}
	}

	bool Process::SetPriority(ProcessPriority priority) const
	{
		if (!m_hProcess)
			return false;

		switch (priority)
		{
		case ProcessPriority_normal:
			return SetPriorityClass(m_hProcess, NORMAL_PRIORITY_CLASS) != FALSE;
		case ProcessPriority_realTime:
			return SetPriorityClass(m_hProcess, REALTIME_PRIORITY_CLASS) != FALSE;
		case ProcessPriority_above:
			return SetPriorityClass(m_hProcess, ABOVE_NORMAL_PRIORITY_CLASS) != FALSE;
		case ProcessPriority_below:
			return SetPriorityClass(m_hProcess, BELOW_NORMAL_PRIORITY_CLASS) != FALSE;
		case ProcessPriority_high:
			return SetPriorityClass(m_hProcess, HIGH_PRIORITY_CLASS) != FALSE;
		case ProcessPriority_low:
			return SetPriorityClass(m_hProcess, IDLE_PRIORITY_CLASS) != FALSE;
		case ProcessPriority_undefined:
		default:
			return false;
		}
	}

	bool Process::GetStartTime(LPSYSTEMTIME startTime) const
	{
		if (!m_hProcess)
			return false;

		FILETIME createTime, endTime, kernelTime, userTime;
		if (!GetProcessTimes(m_hProcess, &createTime, &endTime, &kernelTime, &userTime))
			return false;

		return FileTimeToSystemTime(&createTime, startTime) != FALSE;
	}

	bool Process::GetFinishedTime(LPSYSTEMTIME finishedTime) const
	{
		if (!m_hProcess)
			return false;

		FILETIME createTime, endTime, kernelTime, userTime;
		if (!GetProcessTimes(m_hProcess, &createTime, &endTime, &kernelTime, &userTime))
			return false;

		return FileTimeToSystemTime(&endTime, finishedTime) != FALSE;
	}

}
