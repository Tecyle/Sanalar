#pragma once

namespace Sanalar
{
	typedef ConsoleFontColor
	{

	} ConsoleFontColor;

	typedef ConsoleBackgroundColor
	{

	} ConsoleBackgroundColor;

	class Console
	{
	public:
		// �����������
		int printf(const char* format, ...);
		int printf(const wchar_t* format, ...);
		
		void setStdHandle();

		int printWithColor(ConsoleFontColor fontColor, ConsoleBackgroundColor backgroundColor, const char* format, ...);
		int printWithColor(ConsoleFontColor fontColor, ConsoleBackgroundColor backgroundColor, const wchar_t* format, ...);

		// �������빦��

		// ����̨���ԣ����ڷǱ�׼��Ļ�����Ч��
		void hideCursor();
		void showCursor();
		bool isCursorVisiable() const;
	};
}