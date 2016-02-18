#pragma once
#include "sanalar_process.h"

namespace Sanalar
{
	class StdStream;

	class SubProcess : public Process
	{
	public:
		SubProcess();
		SubProcess(const wchar_t* command);

		~SubProcess();

	public:
		void SetCommand(const wchar_t* command);

		bool Start(bool suspend = false);
		void Reset();

		void SetStdInput(StdStream* stdInput);
		void SetStdOutput(StdStream* stdOutput);
		void SetStdError(StdStream* stdError);

		unsigned long GetRunningTime() const;
		unsigned long GetMaxMemorySize() const;

	private:
		wchar_t* m_commandLine;

		HANDLE m_hStdin;
		HANDLE m_hStdout;
		HANDLE m_hStderr;
	};

	class StdStream
	{
	public:
		static StdStream* CreateMemoryStream(bool isInputStream);
		static StdStream* CreateFileStream(bool isInputStream);

	public:
		virtual size_t Read(unsigned char* buffer, size_t maxSize);
		virtual bool Write(unsigned char* buffer, size_t bufferSize);

		virtual HANDLE GetInputHandle() const;
		virtual HANDLE GetOutputHandle() const;

	private:
		HANDLE GetRedirectInputHandle() const;
		HANDLE GetRedirectOutputHandle() const;
		friend void SubProcess::SetStdInput(StdStream* stdInput);
		friend void SubProcess::SetStdOutput(StdStream* stdOutput);
		friend void SubProcess::SetStdError(StdStream* stdError);

	protected:
		HANDLE m_hInput;
		HANDLE m_hOutput;
		bool m_isInputStream;

	protected:
		StdStream();
		~StdStream();
	};
}