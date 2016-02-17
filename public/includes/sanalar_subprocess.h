#pragma once

namespace Sanalar
{
	__interface StdStream;

	typedef enum SubProcessState
	{
		SubProcessState_init,
		SubProcessState_running,
		SubProcessState_suspend,
		SubProcessState_finished
	} SubProcessState;

	class SubProcess
	{
	public:
		SubProcess();
		SubProcess(const wchar_t* command);

		~SubProcess();

	public:
		void SetCommand(const wchar_t* command);

		bool Start(bool suspend = false);
		bool Suspend();
		bool Resume();
		bool Terminate();
		void Reset();

		void SetStdInput(StdStream* stdInput);
		void SetStdOutput(StdStream* stdOutput);
		void SetStdError(StdStream* stdError);

		bool Wait(unsigned long timeLimited = 0);
		bool IsRunning();
		SubProcessState GetSubProcessState();

		int GetReturnCode() const;
		unsigned long GetRunningTime() const;
		unsigned long GetMaxMemorySize() const;

		void SendMessage();

	private:
		wchar_t* m_commandLine;
		SubProcessState m_state;

		HANDLE m_hProcess;
		HANDLE m_hThread;

		HANDLE m_hStdin;
		HANDLE m_hStdout;
		HANDLE m_hStderr;

		DWORD m_returnCode;
	};

	__interface StdStream
	{
		size_t Read(unsigned char* buffer, size_t maxSize);
		bool Write(unsigned char* buffer, size_t bufferSize);

		HANDLE GetInputHandle() const;
		HANDLE GetOutputHandle() const;

		HANDLE GetRedirectInputHandle() const;
		HANDLE GetRedirectOutputHandle() const;
	};

	class FileStdStream : public StdStream
	{
	public:
		static FileStdStream* CreateStdStream(const wchar_t* fileName, bool isInputStream);
	};

	class MemoryStdStream : public StdStream
	{
	public:
		static MemoryStdStream* CreateStdStream(bool isInputStream);

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
		MemoryStdStream();
		~MemoryStdStream();
	};
}