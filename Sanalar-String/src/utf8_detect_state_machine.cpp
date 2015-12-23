#include "utf8_detect_state_machine.h"

namespace Sanalar
{
#define UTF8_ACCEPTED_MULTIBYTES_NUM 100

	typedef enum Utf8DetectState
	{
		Utf8DetectState_waitingInput,
		Utf8DetectState_acceptedChar,
		Utf8DetectState_refused,
		Utf8DetectState_closed,
		Utf8DetectState_expectByte1,
		Utf8DetectState_expectByte2,
		Utf8DetectState_expectByte3,
		Utf8DetectState_expectByte4,
		Utf8DetectState_expectByte5
	} Utf8DetectState;

	bool Utf8DetectStateMachine::InputByte(byte b)
	{
		ChangeState(b);
		// 处理是否出现过被接受的字符
		if (m_state == Utf8DetectState_acceptedChar)
		{
			m_totalCharNum++;
			if (m_isCheckingMultiBytes)
				m_multiBytesCharNum++;
			m_isCheckingMultiBytes = false;
			m_state = Utf8DetectState_waitingInput;
		}
		else if (m_state == Utf8DetectState_refused)
		{
			return false;
		}
		return true;
	}

	Utf8DetectState _DecideWaitingInputNextState(byte b)
	{
		if (!b)
			return Utf8DetectState_closed;
		else if (!(b & 0x80))			// 起始位为 0
			return Utf8DetectState_acceptedChar;
		else if ((b >> 5) == 0x06)		// 起始位为 110
			return Utf8DetectState_expectByte1;
		else if ((b >> 4) == 0x0E)		// 起始位为 1110
			return Utf8DetectState_expectByte2;
		else if ((b >> 3) == 0x1E)		// 起始位为 1 1110
			return Utf8DetectState_expectByte3;
		else if ((b >> 4) == 0x3E)		// 起始位为 11 1110
			return Utf8DetectState_expectByte4;
		else if ((b >> 5) == 0x7E)		// 起始位为 111 1110
			return Utf8DetectState_expectByte5;
		else
			return Utf8DetectState_refused;
	}

#define _IsByteStartWith10(b) (((b) >> 6) == 0x02)

	void Utf8DetectStateMachine::ChangeState(byte b)
	{
		switch (m_state)
		{
		case Utf8DetectState_waitingInput:
			m_state = _DecideWaitingInputNextState(b);
			break;
		case Utf8DetectState_refused:
			// 已经被 UTF-8 编码拒绝，即该文本不可能为 UTF-8，无需继续检测
			return;
		case Utf8DetectState_expectByte1:
			m_isCheckingMultiBytes = true;
			if (_IsByteStartWith10(b))
				m_state = Utf8DetectState_acceptedChar;
			else
				m_state = Utf8DetectState_refused;
			break;
		case Utf8DetectState_expectByte2:
			m_isCheckingMultiBytes = true;
			if (_IsByteStartWith10(b))
				m_state = Utf8DetectState_expectByte1;
			else
				m_state = Utf8DetectState_refused;
			break;
		case Utf8DetectState_expectByte3:
			m_isCheckingMultiBytes = true;
			if (_IsByteStartWith10(b))
				m_state = Utf8DetectState_expectByte2;
			else
				m_state = Utf8DetectState_refused;
			break;
		case Utf8DetectState_expectByte4:
			m_isCheckingMultiBytes = true;
			if (_IsByteStartWith10(b))
				m_state = Utf8DetectState_expectByte3;
			else
				m_state = Utf8DetectState_refused;
			break;
		case Utf8DetectState_expectByte5:
			m_isCheckingMultiBytes = true;
			if (_IsByteStartWith10(b))
				m_state = Utf8DetectState_expectByte4;
			else
				m_state = Utf8DetectState_refused;
			break;
		case Utf8DetectState_closed:
			m_state = Utf8DetectState_refused;
			break;
		default:
			m_state = Utf8DetectState_refused;
			break;
		}
	}

	bool Utf8DetectStateMachine::Initialize()
	{
		m_state = Utf8DetectState_waitingInput;
		m_confidenceLevel = 0.0;

		m_totalCharNum = 0;
		m_multiBytesCharNum = 0;
		m_isCheckingMultiBytes = false;
	}

	Sanalar::EncodeDetectState Utf8DetectStateMachine::GetDetectState() const
	{
		switch (m_state)
		{
		case Utf8DetectState_waitingInput:
			return EncodeDetectState_waitingInput;
		case Utf8DetectState_refused:
			return EncodeDetectState_refused;
		case Utf8DetectState_expectByte1:
		case Utf8DetectState_expectByte2:
		case Utf8DetectState_expectByte3:
		case Utf8DetectState_expectByte4:
		case Utf8DetectState_expectByte5:
			return EncodeDetectState_processing;
		case Utf8DetectState_closed:
			return EncodeDetectState_waitingInput;
		default:
			return EncodeDetectState_error;
		}
	}

	double Utf8DetectStateMachine::GetConfidenceLevel()
	{
		if (m_multiBytesCharNum > m_acceptMultiBytesNum)
			m_confidenceLevel = 1.0;
		return m_confidenceLevel;
	}

}