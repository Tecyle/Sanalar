#include "stdafx.h"
#include "sanalar_subprocess.h"
#include <string.h>
#include <stdlib.h>

namespace Sanalar
{


	void SubProcess::SetCommand(const wchar_t* command)
	{
		if (m_state != ProcessState_init)
		{
			// ERROR: 
			return;
		}
		free(m_commandLine);
		if (!command)
			return;
		m_commandLine = _wcsdup(command);
	}

	bool SubProcess::Start(bool suspend /*= false*/)
	{
		DWORD flag = 0;
		if (suspend)
			flag |= CREATE_SUSPENDED;

		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		if (m_hStdin || m_hStdout || m_hStderr)
		{
			si.dwFlags |= STARTF_USESTDHANDLES;
			si.dwFlags |= STARTF_USESHOWWINDOW;
			si.hStdInput = m_hStdin;
			si.hStdOutput = m_hStdout;
			si.hStdError = m_hStderr;
		}

		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));
		
		if (!CreateProcess(NULL, m_commandLine, NULL, NULL, TRUE, flag, NULL, NULL, &si, &pi))
			return false;

		CloseHandle(m_hStdout);
		m_state = suspend ? ProcessState_suspend : ProcessState_running;
		m_hProcess = pi.hProcess;
		m_hThread = pi.hThread;
		
		return true;
	}

	void SubProcess::SetStdInput(StdStream* stdInput)
	{
		if (stdInput)
			m_hStdin = stdInput->GetRedirectOutputHandle();
		else
			m_hStdin = NULL;
	}

	void SubProcess::SetStdOutput(StdStream* stdOutput)
	{
		if (stdOutput)
			m_hStdout = stdOutput->GetRedirectInputHandle();
		else
			m_hStdout = NULL;
	}

	void SubProcess::SetStdError(StdStream* stdError)
	{
		if (stdError)
			m_hStderr = stdError->GetRedirectInputHandle();
		else
			m_hStderr = NULL;
	}

	bool SubProcess::IsRunning()
	{
		ProcessState state = GetProcessState();
		if (state != ProcessState_running && state != ProcessState_suspend)
			return false;
		return true;
	}

	ProcessState SubProcess::GetProcessState()
	{
		if (m_state != ProcessState_running && m_state != ProcessState_suspend)
			return m_state;

		// get the information whether process had finished.
		if (WaitForSingleObject(m_hProcess, 0) == WAIT_OBJECT_0)
		{
			m_state = ProcessState_finished;
			if (!GetExitCodeProcess(m_hProcess, &m_returnCode))
			{
				// ERROR:
			}
		}
		return m_state;
	}

	bool SubProcess::Suspend()
	{
		if (GetProcessState() == ProcessState_running)
		{
			NtSuspendProcess(m_hProcess);
			return true;
		}
		return false;
	}

	bool SubProcess::Resume()
	{
		if (GetProcessState() == ProcessState_suspend)
		{
			NtResumeProcess(m_hProcess);
			return true;
		}
		return false;
	}

	bool SubProcess::Terminate()
	{
		if (IsRunning())
		{
			if (TerminateProcess(m_hProcess, 4))
				return true;
		}
		return false;
	}

	void SubProcess::Reset()
	{
		// Terminate sub process if it is running
		Terminate();
		CloseHandle(m_hProcess);
		CloseHandle(m_hThread);
		m_hProcess = NULL;
		m_hThread = NULL;
		m_returnCode = 0;
		m_hStdin = NULL;
		m_hStdout = NULL;
		m_hStderr = NULL;
		m_state = ProcessState_init;
	}

	int SubProcess::GetReturnCode() const
	{
		return (int)m_returnCode;
	}

	bool SubProcess::Wait(unsigned long timeLimited /*= 0*/)
	{
		DWORD timeout = timeLimited ? timeLimited : INFINITE;
		if (WaitForSingleObject(m_hProcess, timeout) == WAIT_OBJECT_0)
			return true;
		return false;
	}

	SubProcess::SubProcess() :
		m_hProcess(NULL),
		m_hThread(NULL),
		m_commandLine(NULL),
		m_hStdin(NULL),
		m_hStdout(NULL),
		m_hStderr(NULL),
		m_returnCode(0),
		m_state(ProcessState_init)
	{
	}

	SubProcess::~SubProcess()
	{
		free(m_commandLine);
		CloseHandle(m_hProcess);
		CloseHandle(m_hThread);
	}


	MemoryStdStream* MemoryStdStream::CreateStdStream(bool isInputStream)
	{
		HANDLE hRead, hWrite;
		SECURITY_ATTRIBUTES sa;
		ZeroMemory(&sa, sizeof(sa));
		sa.nLength = sizeof(sa);
		sa.bInheritHandle = TRUE;
		if (!CreatePipe(&hRead, &hWrite, &sa, 0))
			return NULL;

		MemoryStdStream* ssMemo = new MemoryStdStream;
		ssMemo->m_isInputStream = isInputStream;
		ssMemo->m_hInput = hWrite;
		ssMemo->m_hOutput = hRead;
		
		return ssMemo;
	}

	HANDLE MemoryStdStream::GetInputHandle() const
	{
		if (m_isInputStream)
			return m_hInput;
		return (HANDLE)NULL;
	}

	HANDLE MemoryStdStream::GetOutputHandle() const
	{
		if (!m_isInputStream)
			return m_hOutput;
		return (HANDLE)NULL;
	}

	bool MemoryStdStream::Write(unsigned char* buffer, size_t bufferSize)
	{
		if (m_isInputStream)
		{
			if (WriteFile(m_hInput, buffer, bufferSize, NULL, NULL))
				return true;
		}
		return false;
	}

	size_t MemoryStdStream::Read(unsigned char* buffer, size_t maxSize)
	{
		DWORD numOfBytesRed = 0;
		if (!m_isInputStream)
		{
			if (ReadFile(m_hOutput, buffer, maxSize, &numOfBytesRed, NULL))
				return (size_t)numOfBytesRed;
		}
		return 0u;
	}

	MemoryStdStream::MemoryStdStream() : m_hInput(NULL), m_hOutput(NULL)
	{
	}

	MemoryStdStream::~MemoryStdStream()
	{
		if (m_hInput)
			CloseHandle(m_hInput);
		if (m_hOutput)
			CloseHandle(m_hOutput);
	}

	HANDLE MemoryStdStream::GetRedirectInputHandle() const
	{
		return m_hInput;
	}

	HANDLE MemoryStdStream::GetRedirectOutputHandle() const
	{
		return m_hOutput;
	}

}