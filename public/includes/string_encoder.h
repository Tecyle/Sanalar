#pragma once

namespace Sanalar{
	typedef enum StringEncode
	{
		StringEncode_utf8,
		StringEncode_gbk,
		StringEncode_ucs2_le,
		StringEncode_ucs2_be,
		StringEncode_base64,
		StringEncode_auto
	} StringEncode;
#define StringEncode_unicode StringEncode_ucs2_le
#define ToUnicode ToUcs2Le
#define ToMultiByte ToGbk
#define UnicodeToMultiByte Ucs2LeToGbk
#define UnicodeToUtf8 Ucs2LeToUtf8

	class StringEncoder
	{
	public:
		StringEncoder();
		StringEncoder(byte* buffer, StringEncode encode = StringEncode_auto, bool copyData = false);
		~StringEncoder();

	public:
		size_t ToUcs2Le(byte** buffer, size_t maxBufferSize);
		size_t ToGbk(byte** buffer, size_t maxBufferSize);
		size_t ToUtf8(byte** buffer, size_t maxBufferSize);
		size_t ToUcs2Be(byte** buffer, size_t maxBufferSize);
		size_t ToBase64(byte** buffer, size_t maxBufferSize);

		static StringEncode DetectStringEncode(const byte* buffer, size_t bufferLength);

		static size_t Ucs2LeToGbk(const wchar_t* szSrc, size_t srcLen, char* szOut, size_t outLen);
		static size_t Ucs2LeToUtf8(const wchar_t* szSrc, size_t srcLen, char* szOut, size_t outLen);

		static size_t Ucs2LeToUcs2Be(const wchar_t* szSrc, size_t srcLen, wchar_t* szOut, size_t outLen);
		static size_t Ucs2BeToUcs2Le(const wchar_t* szSrc, size_t srcLen, wchar_t* szOut, size_t outLen);

		static size_t AnyToBase64(const byte* szSrc, size_t srcLen, char* szOut, size_t outLen);
		static size_t Base64ToAny(const char* szSrc, size_t srcLen, byte* szOut, size_t outLen);
	private:
		byte* dataBuffer;
		StringEncode dataEncode;
		bool dataNeedRelease;
	};
}