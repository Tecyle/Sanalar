#pragma once

namespace Sanalar
{
	__interface StdStream;

	class SubProcess
	{
	public:
		void SetCommand(const wchar_t* command);

		bool Start();
		bool Suspend();
		void Terminate();
		void Reset();

		void SetStdInput(StdStream* stdInput);
		void SetStdOutput(StdStream* stdOutput);
		void SetStdError(StdStream* stdError);

		bool Wait(unsigned long timeLimited = 0);
		bool IsRunning();

		int GetReturnCode();
		unsigned long GetRunningTime();
		unsigned long GetMaxMemorySize();

		void SendMessage();
	};

	__interface StdStream
	{
		virtual size_t Read(unsigned char* buffer, size_t maxSize);
		virtual bool Write(unsigned char* buffer, size_t bufferSize);
	};

	class FileStdStream : public StdStream
	{
	public:
		FileStdStream* CreateStdStream(const wchar_t* fileName, bool isInputStream);
	};

	class MemoryStdStream : public StdStream
	{
	public:
		MemoryStdStream* CreateStdStream(bool isInputStream);
	};
}