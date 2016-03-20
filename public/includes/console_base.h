#pragma once

namespace Sanalar
{
	typedef enum ConsoleFontColor
	{

	} ConsoleFontColor;

	typedef enum ConsoleBackgroundColor
	{

	} ConsoleBackgroundColor;

	typedef struct CursorPoint
	{
		int x;
		int y;
	} CursorPoint;

	class Console
	{
	public:
		// 基本输出功能
		int printf(const char* format, ...);
		int printf(const wchar_t* format, ...);
		
		void setStdHandle();

		int printWithColor(ConsoleFontColor fontColor, ConsoleBackgroundColor backgroundColor, const char* format, ...);
		int printWithColor(ConsoleFontColor fontColor, ConsoleBackgroundColor backgroundColor, const wchar_t* format, ...);

		// 基本输入功能

		// 控制台属性（对于非标准屏幕输出无效）
		void hideCursor();
		void showCursor();
		bool isCursorVisiable() const;

		CursorPoint getCursorPosition() const;
		CursorPoint setCursorPosition(const CursorPoint* newPosition);
	};
}