#include "stdafx.h"
#include <stdlib.h>
#include "sanalar_process.h"
#include <ntdef.h>

typedef NTSTATUS (WINAPI *FP_NtSuspendProcess)(HANDLE hProcess);
typedef NTSTATUS (WINAPI *FP_NtResumeProcess)(HANDLE hProcess);

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

static NTSTATUS NtSuspendProcess(HANDLE hProcess)
{
	if (!_fnNtSuspendProcess)
		_InitExternalFunc();
	return _fnNtSuspendProcess(hProcess);
}

static NTSTATUS NtResumeProcess(HANDLE hProcess)
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
		m_pid(0)
	{
	}

	Process::~Process()
	{
		CloseHandle(m_hProcess);
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
			if (NT_SUCCESS(NtSuspendProcess(m_hProcess)))
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
		if (NT_SUCCESS(NtResumeProcess(m_hProcess)))
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

	UINT64 Process::GetRunningTime(ProcessRunningTimeType timeType /*= ProcessRunningTimeType_all*/)
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

}