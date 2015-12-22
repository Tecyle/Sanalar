#pragma once
#include "sanalar_base.h"
#include "string_encoder.h"
#include "regular_expression.h"

namespace Sanalar
{
#define REPLACE_TIME_MAX ((size_t)-1)
#define STRING_DATA_RESERVED_SIZE 1024

	typedef struct StringData StringData;

	class String
	{
	public:
		// 默认构造，产生一个空串
		String();
		// 通过一个 Unicode 字串进行构造（UCS-16 LE）
		String(const wchar_t* str);
		// 通过一个多字节字串进行构造（GBK）
		String(const char* str);
		// 通过指定编码进行构造
		String(const unsigned char* buffer, size_t length, StringEncode encode);
		// 拷贝构造
		String(const String& str);
		String(String& str);

		~String();

	public:
		// 返回子串
		String SubString(int start, int end, bool endIsLength = false);
		String SubString(const RegularExpression* regular);
		String Left(int length);
		String Right(int length);

		String Replace(String strMatch, String strReplace, size_t replaceTime = REPLACE_TIME_MAX);
		String Replace(const RegularExpression* regularMatch, String strReplace, size_t replaceTime = REPLACE_TIME_MAX);

		int Find(String strToFind, int startPos = 0, bool caseSensitive = true, bool increaseDirection = true);
		int Find(const RegularExpression* regularMatch, int startPos = 0, bool increaseDirection = true);

		bool Match(const RegularExpression* regularMatch);

		inline size_t GetLength() const;

		double ToDouble() const;
		int ToInt(int radix = 10) const;
		long ToLong(int radix = 10) const;
		long long ToInt64(int radix = 10) const;
		char ToInt8(int radix = 10) const;
		unsigned char ToUInt8(int radix = 10) const;
		unsigned int ToUInt(int radix = 10) const;
		unsigned long ToULong(int radix = 10) const;
		unsigned long long ToUInt64(int radix = 10) const;

		static String FromNumber(unsigned long long number, int radix = 10, bool hasPrefix = true, bool hasSuffix = false);
		static String FromNumber(long long number, int radix = 10, bool hasPrefix = true, bool hasSuffix = false);
		static String FromNumber(double number, String format);
		static String FromNumber(unsigned long long number, String format);
		static String FromNumber(long long number, String format);
		static String FromBoolean(bool expression, String truePart, String falsePart);
		static String FromFormat(const String format, ...);

		String Format(const String format, ...);

		String Arg(unsigned long long number, int radix = 10, bool hasPrefix = true, bool hasSuffix = false);
		String Arg(long long number, int radix = 10, bool hasPrefix = true, bool hasSuffix = false);
		String Arg(double number, String format);
		String Arg(unsigned long long number, String format);
		String Arg(long long number, String format);
		String Arg(bool expression, String truePart, String falsePart);

		String ToUpper();
		String ToLower();
		String ToFirstCharUpper();

		double CalculateFourArithmeticOperation();
		bool CalculateBooleanExpression(int languageStyle);

		size_t Split(String delimiter, String* resultArray, size_t splitTimes = (size_t)-1, bool caseSensitive = true);

		String Trim(String charList = _T("\b\t\r\n\p"));
		String TrimLeft(String charList = _T("\b\t\r\n\p"));
		String TrimRight(String charList = _T("\b\t\r\n\p"));

		bool StartWith(String startString);
		bool EndWith(String endString);

		String Reverse();

		void Empty();
		bool IsEmpty();

		char& CharAt(int index);
		wchar_t& WCharAt(int index);

		char* GetAnsiBuffer();
		wchar_t* GetBuffer();
		unsigned char* AllocEncodedBuffer(StringEncode dstEncode);

		wchar_t* Duplicate();
		char* DuplicateAnsi();

		// 取字符，只返回 Unicode 的字符
		wchar_t& operator[](int index);
		const wchar_t& operator[](int index) const;
		// 大小比较，可以和自身比也可以和常字串比
		bool operator>(const String& cmpStr);
		bool operator>=(const String& cmpStr);
		bool operator>(const wchar_t* cmpStr);
		bool operator>=(const wchar_t* cmpStr);
		bool operator>(const char* cmpStr);
		bool operator>=(const char* cmpStr);
		bool operator<(const String& cmpStr);
		bool operator<=(const String& cmpStr);
		bool operator<(const wchar_t* cmpStr);
		bool operator<=(const wchar_t* cmpStr);
		bool operator<(const char* cmpStr);
		bool operator<=(const char* cmpStr);
		bool operator==(const String& cmpStr);
		bool operator==(const wchar_t* cmpStr);
		bool operator==(const char* cmpStr);
		// 赋值操作
		String& operator=(const String& str);
		String& operator=(const wchar_t* str);
		String& operator=(const char* str);
		// 连接操作
		String& operator+(const String& r);
		String& operator+(const wchar_t* r);
		String& operator+(const char* r);
		String& operator+(long long r);
		String& operator+(unsigned long long r);
		String& operator+(double r);
		friend String& operator+(const wchar_t* l, const String& r);
		friend String& operator+(const char* l, const String& r);
		friend String& operator+(long long l, const String& r);
		friend String& operator+(unsigned long long l, const String& r);
		friend String& operator+(double l, const String& r);
		// 倍数操作
		String& operator*(unsigned int multiple);
		// 连加操作
		String& operator+=(const String& r);
		String& operator+=(const wchar_t* r);
		String& operator+=(const char* r);
		String& operator+=(long long r);
		String& operator+=(unsigned long long r);
		String& operator+=(double r);
		// 连乘操作
		String& operator*=(unsigned int multiple);

		operator const char*();
		operator const wchar_t*();

		private:
			StringData* m_stringDataW;
			StringData* m_stringDataA;
	};
}