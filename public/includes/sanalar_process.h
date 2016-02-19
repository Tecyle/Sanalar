#pragma once
#include <TlHelp32.h>

namespace Sanalar
{
	// 表明进程的状态
	typedef enum ProcessState
	{
		ProcessState_init,		// 还没有打开进程或者进程还没有开始运行
		ProcessState_running,	// 进程当前正在运行中
		ProcessState_suspend,	// 进程当前已经被挂起
		ProcessState_finished	// 进程当前已经停止运行
	} ProcessState;

	// 表明进程的运行时间参数
	typedef enum ProcessRunningTimeType
	{
		ProcessRunningTimeType_all,		// 进程消耗的总时间
		ProcessRunningTimeType_core,	// 进程消耗的内核时间
		ProcessRunningTimeType_user		// 进程消耗的用户态时间
	} ProcessRunningTimeType;

	// 表明进程的优先级
	typedef enum ProcessPriority
	{
		ProcessPriority_normal,		// 正常
		ProcessPriority_realTime,	// 实时
		ProcessPriority_above,		// 高于正常
		ProcessPriority_below,		// 低于正常
		ProcessPriority_high,		// 高
		ProcessPriority_low,		// 低
		ProcessPriority_undefined	// 预期之外的值，一般是发生错误
	} ProcessPriority;

	//////////////////////////////////////////////////////////////////////////
	// 管理进程的类，该类无法直接创建，需要通过 Process::Shell 或 Process::Open
	// 方法获取
	class Process
	{
	public:
		virtual bool Suspend();		// 挂起该进程
		virtual bool Resume();		// 恢复该进程
		virtual bool Terminate();	// 终止该进程

		// 阻塞等待直到进程结束运行，或者等待超过了 timeLimited (ms) 时间
		// 当 timeLimited 设置为 0 时，表明无止尽地等待而无超时时间
		// 返回 true 表明进程结束运行，返回 false 表明等待超时
		virtual bool Wait(unsigned long timeLimited = 0);

		// 判断当前进程是否存在
		// 当进程处于运行状态或者挂起状态，则返回 true，否则返回 false
		virtual bool IsActive();
		// 返回当前进程是否正在运行
		virtual bool IsRunning();
		// 返回当前进程是否已被挂起
		virtual bool IsSuspended();
		// 返回当前进程是否已经结束运行
		virtual bool IsFinished();
		// 返回当前进程运行过程中是否崩溃
		virtual bool ExceptionHappened();

		// 获取当前进程的状态
		virtual ProcessState GetProcessState();

		// 获取进程的返回值，如果进程没有退出或者没有创建，则返回 STILL_ACTIVE
		virtual int GetReturnCode();

		// 获取进程的运行时间，单位毫秒
		virtual UINT64 GetRunningTime(ProcessRunningTimeType timeType = ProcessRunningTimeType_all) const;
		// 获得当前进程的总 CPU 占用率，返回 0 ~ 100 之间的一个数
		// interval 指定统计使用率的一次时间间隔
		virtual UINT8 GetCpuUsage(size_t interval = 100);

		// 获取进程内存的最大使用数，单位字节
		virtual size_t GetMaxMemoryUsed() const;
		// 获得进程的当前内存使用数，单位字节
		virtual size_t GetMemoryUsed() const;

		// 获取当前进程的文件名称，不带路径
		// 如果 fileName 为 NULL，则仅返回文件名的长度，包括末尾的 \0
		virtual size_t GetProcessName(wchar_t* fileName, size_t fileNameBufferSize);
		// 获取当前进程的文件位置，不带文件名，末尾带反斜杠
		// 如果 filePath 为 NULL，则仅返回文件路径的长度，包括末尾的 \0
		virtual size_t GetProcessLocation(wchar_t* filePath, size_t filePathBufferSize);

		// 获取进程的 ID，如果进程尚未运行或者已经退出，则返回 0
		virtual DWORD GetId();

		// 获取进程的优先级
		virtual ProcessPriority GetPriority() const;
		// 设置进程的优先级
		virtual bool SetPriority(ProcessPriority priority) const;

		// 获取进程的启动时间
		virtual bool GetStartTime(LPSYSTEMTIME startTime) const;
		// 获取进程的结束时间
		virtual bool GetFinishedTime(LPSYSTEMTIME finishedTime) const;

	public:
		// 通过启动一个命令的方式获取一个进程
		static Process* Shell(const wchar_t* cmdLine, bool suspend = false);
		// 打开一个已经在运行的进程
		static Process* Open(DWORD pid);

		~Process();
		
	protected:
		Process();

		virtual bool CreateProcessEntry();

	protected:
		ProcessState m_state;

		HANDLE m_hProcess;
		DWORD m_pid;

		DWORD m_returnCode;

		LPPROCESSENTRY32W m_pProcessEntry;
	};
}