/*
 * TestTemplateClass.h
 *
 *  Created on: Jul 28, 2016
 *      Author: parallels
 */

#ifndef TESTTEMPLATECLASS_H_
#define TESTTEMPLATECLASS_H_

#include <iostream>
#include <stdint.h>
#include <thread>
#include <chrono>

class TESTCLASS
{
public:
	explicit TESTCLASS(int32_t nValue) : m_nValue(nValue)
	{};
	int32_t TestFunction(int32_t nArg)
	{
		using namespace std;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		cout << "[TestFunction] member variable: "  << m_nValue <<endl;
		cout << "[TestFunction] Argument: " << nArg << endl;
		return nArg;
	};
	int32_t StartTask();
private:
	TESTCLASS(const TESTCLASS&);
	TESTCLASS& operator=(const TESTCLASS&);
	int32_t m_nValue;
};

#endif /* TESTTEMPLATECLASS_H_ */
