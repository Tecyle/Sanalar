#pragma once
#include <TlHelp32.h>

namespace Sanalar
{
	// �������̵�״̬
	typedef enum ProcessState
	{
		ProcessState_init,		// ��û�д򿪽��̻��߽��̻�û�п�ʼ����
		ProcessState_running,	// ���̵�ǰ����������
		ProcessState_suspend,	// ���̵�ǰ�Ѿ�������
		ProcessState_finished	// ���̵�ǰ�Ѿ�ֹͣ����
	} ProcessState;

	// �������̵�����ʱ�����
	typedef enum ProcessRunningTimeType
	{
		ProcessRunningTimeType_all,		// �������ĵ���ʱ��
		ProcessRunningTimeType_core,	// �������ĵ��ں�ʱ��
		ProcessRunningTimeType_user		// �������ĵ��û�̬ʱ��
	} ProcessRunningTimeType;

	// �������̵����ȼ�
	typedef enum ProcessPriority
	{
		ProcessPriority_normal,		// ����
		ProcessPriority_realTime,	// ʵʱ
		ProcessPriority_above,		// ��������
		ProcessPriority_below,		// ��������
		ProcessPriority_high,		// ��
		ProcessPriority_low,		// ��
		ProcessPriority_undefined	// Ԥ��֮���ֵ��һ���Ƿ�������
	} ProcessPriority;

	//////////////////////////////////////////////////////////////////////////
	// ������̵��࣬�����޷�ֱ�Ӵ�������Ҫͨ�� Process::Shell �� Process::Open
	// ������ȡ
	class Process
	{
	public:
		virtual bool Suspend();		// ����ý���
		virtual bool Resume();		// �ָ��ý���
		virtual bool Terminate();	// ��ֹ�ý���

		// �����ȴ�ֱ�����̽������У����ߵȴ������� timeLimited (ms) ʱ��
		// �� timeLimited ����Ϊ 0 ʱ��������ֹ���صȴ����޳�ʱʱ��
		// ���� true �������̽������У����� false �����ȴ���ʱ
		virtual bool Wait(unsigned long timeLimited = 0);

		// �жϵ�ǰ�����Ƿ����
		// �����̴�������״̬���߹���״̬���򷵻� true�����򷵻� false
		virtual bool IsActive();
		// ���ص�ǰ�����Ƿ���������
		virtual bool IsRunning();
		// ���ص�ǰ�����Ƿ��ѱ�����
		virtual bool IsSuspended();
		// ���ص�ǰ�����Ƿ��Ѿ���������
		virtual bool IsFinished();
		// ���ص�ǰ�������й������Ƿ����
		virtual bool ExceptionHappened();

		// ��ȡ��ǰ���̵�״̬
		virtual ProcessState GetProcessState();

		// ��ȡ���̵ķ���ֵ���������û���˳�����û�д������򷵻� STILL_ACTIVE
		virtual int GetReturnCode();

		// ��ȡ���̵�����ʱ�䣬��λ����
		virtual UINT64 GetRunningTime(ProcessRunningTimeType timeType = ProcessRunningTimeType_all) const;
		// ��õ�ǰ���̵��� CPU ռ���ʣ����� 0 ~ 100 ֮���һ����
		// interval ָ��ͳ��ʹ���ʵ�һ��ʱ����
		virtual UINT8 GetCpuUsage(size_t interval = 100);

		// ��ȡ�����ڴ�����ʹ��������λ�ֽ�
		virtual size_t GetMaxMemoryUsed() const;
		// ��ý��̵ĵ�ǰ�ڴ�ʹ��������λ�ֽ�
		virtual size_t GetMemoryUsed() const;

		// ��ȡ��ǰ���̵��ļ����ƣ�����·��
		// ��� fileName Ϊ NULL����������ļ����ĳ��ȣ�����ĩβ�� \0
		virtual size_t GetProcessName(wchar_t* fileName, size_t fileNameBufferSize);
		// ��ȡ��ǰ���̵��ļ�λ�ã������ļ�����ĩβ����б��
		// ��� filePath Ϊ NULL����������ļ�·���ĳ��ȣ�����ĩβ�� \0
		virtual size_t GetProcessLocation(wchar_t* filePath, size_t filePathBufferSize);

		// ��ȡ���̵� ID�����������δ���л����Ѿ��˳����򷵻� 0
		virtual DWORD GetId();

		// ��ȡ���̵����ȼ�
		virtual ProcessPriority GetPriority() const;
		// ���ý��̵����ȼ�
		virtual bool SetPriority(ProcessPriority priority) const;

		// ��ȡ���̵�����ʱ��
		virtual bool GetStartTime(LPSYSTEMTIME startTime) const;
		// ��ȡ���̵Ľ���ʱ��
		virtual bool GetFinishedTime(LPSYSTEMTIME finishedTime) const;

	public:
		// ͨ������һ������ķ�ʽ��ȡһ������
		static Process* Shell(const wchar_t* cmdLine, bool suspend = false);
		// ��һ���Ѿ������еĽ���
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