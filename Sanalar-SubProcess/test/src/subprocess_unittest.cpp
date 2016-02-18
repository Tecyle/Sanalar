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
			MemoryStdStream* stdStream = MemoryStdStream::CreateStdStream(false);
			subProcess.SetCommand(L"ping 127.0.0.1");
			subProcess.SetStdOutput(stdStream);
			Assert::IsTrue(subProcess.Start());
			Assert::IsTrue(subProcess.Wait());
			Assert::AreEqual(SubProcessState_finished, subProcess.GetSubProcessState());

			char output[1024] = { 0 };
			Assert::IsTrue(stdStream->Read((unsigned char*)output, 1024) > 0);
			Logger::WriteMessage(output);
		}

	};
}