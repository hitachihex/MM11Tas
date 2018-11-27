#include "stdafx.h"
#include "BossBase.h"

Boss::BossBase::BossBase(unsigned long long orig, Boss::BossDecisionHandler Fun)
{
	this->m_OriginalAddress = orig;
	this->m_DecisionHandler = Fun;
	this->m_HookHandle = { NULL };

	*(unsigned long long*)(&this->m_Original) = m_OriginalAddress;


	DebugOutput("Boss::BossBase::BossBase: orig = %llx, fun=%llx", orig, (unsigned long long)Fun);
}

bool Boss::BossBase::Hook()
{
	// If the default ctor is used, these variables will be defaulted. 
	// No one supplied an original or a decision handler, hook can't happen.
	if (this->m_OriginalAddress == NULL || this->m_DecisionHandler == nullptr)
	{
		DebugOutput("Boss::BossBase::Hook - this->m_OriginalAddress == NULL || this->m_DecisionHandler == nullptr");
		return false;
	}

	// NOTE: Add code!

	NTSTATUS hookResult = AddHook((void*)this->m_OriginalAddress, this->m_DecisionHandler, NULL, &this->m_HookHandle);

	if (FAILED(hookResult))
	{
		auto  err = RtlGetLastErrorString();
		DebugOutputW(L"Boss::BossBase::Hook - we encountered an error.. %s", err);
		return false;
	}
	ExclusiveHook(&this->m_HookHandle);

	return true;

}

