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
			template<> static std::wstring ToString<SubProcessState>(const SubProcessState& t)
			{
				switch (t)
				{
				case SubProcessState_init:
					return L"SubProcessState_init";
				case SubProcessState_running:
					return L"SubProcessState_running";
				case SubProcessState_suspend:
					return L"SubProcessState_suspend";
				case SubProcessState_finished:
					return L"SubProcessState_finished";
				default:
					return L"Unknown SubProcessState";
				}
			}
		}
	}
}