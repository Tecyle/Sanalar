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
		// ERROR : ������Ҫ�����̰߳�ȫ
		void AddReff()
		{
			refCount++;
		}
		// ERROR : ������Ҫ�����̰߳�ȫ
		void Release()
		{
			refCount--;
			if (refCount <= 0)
			{
				delete this;
			}
		}
	} StringData;

	// ���캯��
	String::String()
	{
		m_stringDataW = (StringData*)new byte[sizeof(StringData)+sizeof(byte)* STRING_DATA_RESERVED_SIZE];
		m_stringDataW->allocLength = STRING_DATA_RESERVED_SIZE;
		m_stringDataW->isUnicode = true;
		m_stringDataW->refCount = 1;
		m_stringDataW->stringLength = 0;
		m_stringDataW->Data()[0] = 0;

		m_stringDataA = NULL;
	}

	String::String(const wchar_t* str)
	{
		m_stringDataW = (StringData*)new byte[sizeof(StringData)+sizeof(byte)* STRING_DATA_RESERVED_SIZE];
		m_stringDataW->allocLength = STRING_DATA_RESERVED_SIZE;
		m_stringDataW->isUnicode = true;
		m_stringDataW->refCount = 1;
		memcpy_s(m_stringDataW->Data(), STRING_DATA_RESERVED_SIZE, str, (wcslen(str) + 1) * sizeof(wchar_t)); // ��������ĩβ�� 0
		m_stringDataW->stringLength = wcslen(str);
	}

}
