#pragma once

#include "stdafx.h"

#include "mutex.h"


// --------------------------------------------------------
/// @brief	constructor
// --------------------------------------------------------
Mutex::Mutex(void)
	: dwOwnerThreadId_(0)
{
	InitializeCriticalSection(&crit_sec_);
}


// --------------------------------------------------------
/// @brief	destructor
// --------------------------------------------------------
Mutex::~Mutex(void)
{
	unlock();
	DeleteCriticalSection(&crit_sec_);
}


// --------------------------------------------------------
/// @brief	critical section 진입
///			여러번 호출해도 lock 1번 호출과 같다.
// --------------------------------------------------------
void Mutex::lock()
{
	DWORD dwCurThreadId = GetCurrentThreadId();
	
	// 주의: CRITICAL_SECTION과는 달리 같은 thread에서의 nesting 금지
	if(dwCurThreadId == dwOwnerThreadId_) return;

	EnterCriticalSection(&crit_sec_);

	// trace 분석한 결과, lock이 걸렸는데도 다른 thread에서 들어와 lock을 2이상으로 증가시키는 경우가 있는 것 같음.
	// 일단 아래와 같이 임시 보완했음. 원인 분석을 위해 EnterCriticalSection(), LeaveCriticalSection() 소스코드 필요.
	while(crit_sec_.LockCount>1) {
		TRACE(_T("critical section fault : LockCount==%d \r\n"), crit_sec_.LockCount);
		LeaveCriticalSection(&crit_sec_);
	}

	dwOwnerThreadId_ = dwCurThreadId;
}


// --------------------------------------------------------
/// @brief	critical section 종료
///			여러번 호출해도 unlock 1번 호출과 같다.
// --------------------------------------------------------
void Mutex::unlock()
{
	if(is_locked()) {
		dwOwnerThreadId_ = 0;
		LeaveCriticalSection(&crit_sec_);
	}
}
