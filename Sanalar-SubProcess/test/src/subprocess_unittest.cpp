#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SanalarSubProcessTest
{		
	TEST_CLASS(SubProcessTest)
	{
	public:
		
		TEST_METHOD(RedirectOutputTest)
		{
			SubProcess subProcess;
			StdStream* stdStream = StdStream::CreateMemoryStream(false);
			subProcess.SetCommand(L"ping 127.0.0.1");
			subProcess.SetStdOutput(stdStream);
			Assert::IsTrue(subProcess.Start());
			Assert::IsTrue(subProcess.Wait());
			Assert::AreEqual(ProcessState_finished, subProcess.GetProcessState());

			char output[1024] = { 0 };
			Assert::IsTrue(stdStream->Read((unsigned char*)output, 1024) > 0);
			Logger::WriteMessage(output);
		}

		TEST_METHOD(ProcessBasic)
		{
			Process* process = Process::Shell(L"ping 127.0.0.1 -t");
			Assert::IsTrue(process->GetId() > 0);
			Assert::IsTrue(process->GetPriority() == ProcessPriority_normal);
			Assert::IsTrue(process->IsActive());
			Assert::IsTrue(process->Suspend());
			Sleep(200);
			Assert::IsTrue(process->Resume());
			Sleep(200);
			Assert::IsTrue(process->Terminate());
			Sleep(200);
			Assert::IsTrue(process->IsFinished());
			delete process;
		}

	};
}