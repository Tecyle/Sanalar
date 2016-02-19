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
		CloseHandle(m_hStdin);
		CloseHandle(m_hStderr);
		m_state = suspend ? ProcessState_suspend : ProcessState_running;
		m_hProcess = pi.hProcess;
		CloseHandle(pi.hThread);
		
		return true;
	}

	void SubProcess::SetStdInput(StdStream* stdInput)
	{
		if (stdInput)
			m_hStdin = stdInput->m_hOutput;
		else
			m_hStdin = NULL;
	}

	void SubProcess::SetStdOutput(StdStream* stdOutput)
	{
		if (stdOutput)
			m_hStdout = stdOutput->m_hInput;
		else
			m_hStdout = NULL;
	}

	void SubProcess::SetStdError(StdStream* stdError)
	{
		if (stdError)
			m_hStderr = stdError->m_hInput;
		else
			m_hStderr = NULL;
	}

	void SubProcess::Reset()
	{
		// Terminate sub process if it is running
		Terminate();
		CloseHandle(m_hProcess);
		m_hProcess = NULL;
		m_returnCode = 0;
		m_hStdin = NULL;
		m_hStdout = NULL;
		m_hStderr = NULL;
		m_state = ProcessState_init;
		m_pid = 0;
		delete m_pProcessEntry;
		m_pProcessEntry = NULL;
	}

	SubProcess::SubProcess() :
		m_commandLine(NULL),
		m_hStdin(NULL),
		m_hStdout(NULL),
		m_hStderr(NULL)
	{
	}

	SubProcess::~SubProcess()
	{
		free(m_commandLine);
	}


	StdStream* StdStream::CreateMemoryStream(bool isInputStream)
	{
		HANDLE hRead, hWrite;
		SECURITY_ATTRIBUTES sa;
		ZeroMemory(&sa, sizeof(sa));
		sa.nLength = sizeof(sa);
		sa.bInheritHandle = TRUE;
		if (!CreatePipe(&hRead, &hWrite, &sa, 0))
			return NULL;

		StdStream* ssMemo = new StdStream;
		ssMemo->m_isInputStream = isInputStream;
		ssMemo->m_hInput = hWrite;
		ssMemo->m_hOutput = hRead;
		
		return ssMemo;
	}

	HANDLE StdStream::GetInputHandle() const
	{
		if (m_isInputStream)
			return m_hInput;
		return (HANDLE)NULL;
	}

	HANDLE StdStream::GetOutputHandle() const
	{
		if (!m_isInputStream)
			return m_hOutput;
		return (HANDLE)NULL;
	}

	bool StdStream::Write(unsigned char* buffer, size_t bufferSize)
	{
		if (m_isInputStream)
		{
			if (WriteFile(m_hInput, buffer, bufferSize, NULL, NULL))
				return true;
		}
		return false;
	}

	size_t StdStream::Read(unsigned char* buffer, size_t maxSize)
	{
		DWORD numOfBytesRed = 0;
		if (!m_isInputStream)
		{
			if (ReadFile(m_hOutput, buffer, maxSize, &numOfBytesRed, NULL))
				return (size_t)numOfBytesRed;
		}
		return 0u;
	}

	StdStream::StdStream() : m_hInput(NULL), m_hOutput(NULL)
	{
	}

	StdStream::~StdStream()
	{
		if (m_hInput)
			CloseHandle(m_hInput);
		if (m_hOutput)
			CloseHandle(m_hOutput);
	}

}