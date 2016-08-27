#pragma once

#include <Windows.h>


///////////////////////////////////////////////////////////
/**
	@class	Mutex
	@author Choi, Won-Hyuk, Hyundai Heavy Industries Co., Ltd.
	@brief	동기화 객체 mutex. (주의: process간 공유 mutex 아님.)
			일단은 Windows API 활용. (추후 C++11의 std::mutex로 대체하자.)
	@date 2016-01-04
*/
class Mutex
{
public:
	Mutex(void);	///< constructor
	~Mutex(void);	///< destructor

	void lock();
	void unlock();
		
	/// @brief	locked 여부 리턴
	bool is_locked() const { return (dwOwnerThreadId_ != 0); }
		// (주의: CRITICAL_SECTION::LockCount 등은 OS dependant하므로 사용하지 말자.)

private:
	DWORD dwOwnerThreadId_;			///> mutex를 소유한 thread의 ID
	CRITICAL_SECTION crit_sec_;		///> buffer 접근 제어 동기화
};
