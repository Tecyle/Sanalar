// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Headers for CppUnitTest
#include "CppUnitTest.h"

// TODO: reference additional headers your program requires here
#include <windows.h>
#include "sanalar_subprocess.h"
using namespace Sanalar;

#pragma warning(disable : 4505)

namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			template<> static std::wstring ToString<ProcessState>(const ProcessState& t)
			{
				switch (t)
				{
				case ProcessState_init:
					return L"ProcessState_init";
				case ProcessState_running:
					return L"ProcessState_running";
				case ProcessState_suspend:
					return L"ProcessState_suspend";
				case ProcessState_finished:
					return L"ProcessState_finished";
				default:
					return L"Unknown SubProcessState";
				}
			}
		}
	}
}