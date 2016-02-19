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

	};
}