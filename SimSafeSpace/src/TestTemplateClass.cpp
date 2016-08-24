/*
 * TestTemplateClass.cpp
 *
 *  Created on: Jul 28, 2016
 *      Author: parallels
 */

#include "TestTemplateClass.h"
#include <thread>
#include <future>
#include <stdint.h>

using namespace std;

int32_t TESTCLASS::StartTask()
{
	int32_t nValue = 1024;
#ifdef _VS2012
	std::future<int32_t> fut = std::async(std::launch::async, &TESTCLASS::TestFunction, this, nValue);
	int32_t nRet = fut.get();
#else
	std::packaged_task<int32_t(TESTCLASS*, int32_t)> task(&TESTCLASS::TestFunction);
	std::future<int32_t> fut = task.get_future();
	std::thread workerThead(std::move(task), this, nValue);
	workerThead.detach();
	int32_t nRet = fut.get();
#endif
	cout << "[TESTCLASS::StartTask] ret :" << nRet << endl;

	return 1;

}
