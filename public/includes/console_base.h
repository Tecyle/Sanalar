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
		int Printf(const char* format, ...);
		int Printf(const wchar_t* format, ...);
		
		void SetStdHandle();

		int PrintWithColor(ConsoleFontColor fontColor, ConsoleBackgroundColor backgroundColor, const char* format, ...);
		int PrintWithColor(ConsoleFontColor fontColor, ConsoleBackgroundColor backgroundColor, const wchar_t* format, ...);

		void ClearScreen();

		// 基本输入功能

		// 控制台属性（对于非标准屏幕输出无效）
		void HideCursor();
		void ShowCursor();
		bool IsCursorVisiable() const;

		const wchar_t* GetTitle() const;
		void SetTitle(const wchar_t* title) const;

		CursorPoint GetCursorPosition() const;
		CursorPoint SetCursorPosition(const CursorPoint* newPosition);
	};
}