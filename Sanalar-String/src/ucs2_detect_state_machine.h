#pragma once
#include "encode_detect_state_machine.h"

namespace Sanalar
{
	typedef enum Ucs2DetectState Ucs2DetectState;

	class Ucs2DetectStateMachine : EncodeDetectStateMachine
	{
	public:
		virtual bool Initialize();
		virtual bool InputByte(byte b);
		virtual EncodeDetectState GetDetectState() const;
		virtual double GetConfidenceLevel();
		virtual void Uninitialize();
		virtual bool CheckBOM(const byte* bs, size_t bLen) const;

	protected:
		void ChangeState(byte b);

	private:
		Ucs2DetectState m_state;
		double m_confidenceLevel;

		size_t m_totalCharNum;
	};
}