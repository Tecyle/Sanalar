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

	private:
		byte* dataBuffer;
		StringEncode dataEncode;
		bool dataNeedRelease;
	};
}