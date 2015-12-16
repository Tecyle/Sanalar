#include "sanalar_string.h"
#include <string.h>
namespace Sanalar
{
	typedef struct StringData
	{
		int refCount;
		size_t stringLength;
		size_t allocLength;
		bool isUnicode;

		inline byte* Data()
		{
			return (byte*)(this + 1);
		}
		// ERROR : 以下需要考虑线程安全
		void AddReff()
		{
			refCount++;
		}
		// ERROR : 以下需要考虑线程安全
		void Release()
		{
			refCount--;
			if (refCount <= 0)
			{
				delete this;
			}
		}
	} StringData;

	// 构造函数
	String::String()
	{
		stringDataW = (StringData*)new byte[sizeof(StringData)+sizeof(byte)* STRING_DATA_RESERVED_SIZE];
		stringDataW->allocLength = STRING_DATA_RESERVED_SIZE;
		stringDataW->isUnicode = true;
		stringDataW->refCount = 1;
		stringDataW->stringLength = 0;
		stringDataW->Data()[0] = 0;

		stringDataA = NULL;
	}

	String::String(const wchar_t* str)
	{
		stringDataW = (StringData*)new byte[sizeof(StringData)+sizeof(byte)* STRING_DATA_RESERVED_SIZE];
		stringDataW->allocLength = STRING_DATA_RESERVED_SIZE;
		stringDataW->isUnicode = true;
		stringDataW->refCount = 1;
		memcpy_s(stringDataW->Data(), STRING_DATA_RESERVED_SIZE, str, (wcslen(str) + 1) * sizeof(wchar_t)); // 拷贝包括末尾的 0
		stringDataW->stringLength = wcslen(str);
	}

}
