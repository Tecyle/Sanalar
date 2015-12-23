#pragma once
#include "sanalar_base.h"

namespace Sanalar
{
	typedef enum EncodeDetectState
	{
		EncodeDetectState_waitingInput,
		EncodeDetectState_accepted,
		EncodeDetectState_refused,
		EncodeDetectState_processing,
		EncodeDetectState_error
	} EncodeDetectState;

	class EncodeDetectStateMachine
	{
	public:
		virtual bool Initialize() = 0;
		virtual bool InputByte(byte b) = 0;
		virtual EncodeDetectState GetDetectState() const = 0;
		virtual double GetConfidenceLevel() = 0;
		virtual void Uninitialize() = 0;

		virtual bool CheckBOM(const byte* bs, size_t bLen) const = 0;
	};
}