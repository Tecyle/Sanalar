#include "utf8_detect_state_machine.h"

namespace Sanalar
{
	typedef enum Utf8DetectState
	{
		Utf8DetectState_waitingInput,
		Utf8DetectState_acceptedChar,
		Utf8DetectState_refused,
		Utf8DetectState_expectByte1,
		Utf8DetectState_expectByte2,
		Utf8DetectState_expectByte3,
		Utf8DetectState_expectByte4,
		Utf8DetectState_expectByte5
	} Utf8DetectState;

	bool Utf8DetectStateMachine::InputByte(byte b)
	{
		ChangeState(b);
		// �����Ƿ���ֹ������ܵ��ַ�
		if (m_state == Utf8DetectState_acceptedChar)
		{
			// TODO : ���ַ�ͳ��
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
		if (!(b & 0x80))				// ��ʼλΪ 0
			return Utf8DetectState_acceptedChar;
		else if ((b >> 5) == 0x06)		// ��ʼλΪ 110
			return Utf8DetectState_expectByte1;
		else if ((b >> 4) == 0x0E)		// ��ʼλΪ 1110
			return Utf8DetectState_expectByte2;
		else if ((b >> 3) == 0x1E)		// ��ʼλΪ 1 1110
			return Utf8DetectState_expectByte3;
		else if ((b >> 4) == 0x3E)		// ��ʼλΪ 11 1110
			return Utf8DetectState_expectByte4;
		else if ((b >> 5) == 0x7E)		// ��ʼλΪ 111 1110
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
			// �Ѿ��� UTF-8 ����ܾ��������ı�������Ϊ UTF-8������������
			return;
		case Utf8DetectState_expectByte1:
			if (_IsByteStartWith10(b))
				m_state = Utf8DetectState_acceptedChar;
			else
				m_state = Utf8DetectState_refused;
			break;
		case Utf8DetectState_expectByte2:
			if (_IsByteStartWith10(b))
				m_state = Utf8DetectState_expectByte1;
			else
				m_state = Utf8DetectState_refused;
			break;
		case Utf8DetectState_expectByte3:
			if (_IsByteStartWith10(b))
				m_state = Utf8DetectState_expectByte2;
			else
				m_state = Utf8DetectState_refused;
			break;
		case Utf8DetectState_expectByte4:
			if (_IsByteStartWith10(b))
				m_state = Utf8DetectState_expectByte3;
			else
				m_state = Utf8DetectState_refused;
			break;
		case Utf8DetectState_expectByte5:
			if (_IsByteStartWith10(b))
				m_state = Utf8DetectState_expectByte4;
			else
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
	}

	Sanalar::EncodeDetectState Utf8DetectStateMachine::GetDetectState() const
	{
		switch (m_state)
		{
		case Sanalar::Utf8DetectState_waitingInput:
			return EncodeDetectState_waitingInput;
		case Sanalar::Utf8DetectState_refused:
			return EncodeDetectState_refused;
		case Sanalar::Utf8DetectState_expectByte1:
		case Sanalar::Utf8DetectState_expectByte2:
		case Sanalar::Utf8DetectState_expectByte3:
		case Sanalar::Utf8DetectState_expectByte4:
		case Sanalar::Utf8DetectState_expectByte5:
			return EncodeDetectState_processing;
		default:
			return EncodeDetectState_error;
		}
	}

	double Utf8DetectStateMachine::GetConfidenceLevel() const
	{
		return m_confidenceLevel;
	}

}