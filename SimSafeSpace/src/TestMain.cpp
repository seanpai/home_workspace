
#if defined(_WINDOWS_)
#include "stdafx.h"
#endif

#include <stdio.h>
#include <string.h>
#include <string>
#include <thread>
#include <iostream>
#include <chrono>
#include <future>
#include <istream>
#include <ostream>
#include <mutex>
#include <condition_variable>
#include "TestTemplateClass.h"
#if defined(_WINDOWS_)
#include <WinSock2.h>
#endif

#define TRUE (1)

//#include <boost/thread/thread.hpp>
//#include <boost/shared_ptr.hpp>

#include "CommTest.h"
#include "EtherInterface.h"

using namespace std;

//using FUNC_TYPE = int(const std::string&);

std::mutex g_mutex_Printout;
std::mutex g_mutex_UserTerminate;
bool g_bTerminate = false;

const static std::string strMainBdIpAddr("192.168.1.128");

std::condition_variable g_condition_UserTerminate;

void ThreadFunc(int& Aug)
{
	do
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		Aug++;
		cout << "[ThreadFunc] count: " << Aug <<endl;
	} 
	while (10 >= Aug);
}

int ThreadCount(const std::string& strAug)
{
	std::cout << strAug.c_str() << std::endl;
	uint32_t nLoopCount = 0;

	while(true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		nLoopCount++;
		cout<<"[ThreadCount] loop count :"<<nLoopCount<<endl;
		if(5 == nLoopCount)
		{
			cout<<"[ThreadCount] this loop ends"<<endl;
			break;
		}
	}

	g_bTerminate = true;
	g_condition_UserTerminate.notify_all();

	return 1;
}

#include "SmartPtr.h"
int main()
{
#if defined(_WINDOWS_)
	WSADATA wsaData;

	if(0 != WSAStartup(MAKEWORD(2,2), &wsaData))
	{
		int nRetValue = 0;
		nRetValue = ::WSAGetLastError();
		return nRetValue;
	}
#endif

	{
		SmartPtr<int> spLambda(new int[1024], [](int* pData)
		{
			cout << "spLambda, deallocate" << endl;
			delete[] pData;
		});

		ArrayDeleter<int> deleter;
		SmartPtr<int> spInt(new int[1024], deleter);
		SmartPtr<char> spChar(new char[1024], (unsigned int)ARRAY_PTR);
		//SmartPtr<char> spChar(new char[1024], static_cast<unsigned int>(ARRAY_PTR));
		//SmartPtr<char> spChar(new char[1024], static_cast<unsigned int>(2));
	}

	//// sender thread
	//std::packaged_task<int(int)> TaskSender(ThreadSimSafeSpace);
	//std::future<int> futureThreadSender = TaskSender.get_future();
	//std::thread ThreadSender(std::move(TaskSender), EthInterface->GetSocketHandle());
	//ThreadSender.detach();

	//// receiver thread
	//std::packaged_task<int(int)> TaskReceiver(ThreadSimSpaceReceiver);
	//std::future<int> futureThreadReceiver = TaskReceiver.get_future();
	//std::thread ThreadReceiver(std::move(TaskReceiver), EthInterface->GetSocketHandle());
	//ThreadReceiver.detach();

	// Timer thread
	//std::string strMessage("ThreadCount is running");
	//std::packaged_task<int(const std::string&)> TaskCount(ThreadCount);
	//std::future<int> futureTaskCount = TaskCount.get_future();
	//std::thread ThreadCount(std::move(TaskCount),  std::ref(strMessage));
	//ThreadCount.detach();

	//boost::thread testThread([]() { cout << "hello, boost" << endl; });

	TESTCLASS TestClass(255);

	TestClass.StartTask();

	//int nValue = 0;
	//nValue = futureTaskCount.get();
	//nValue = futureThreadSender.get();
	//nValue = futureThreadReceiver.get();

	std::cout << "[main] terminated" << std::endl;
#if defined(_WINDOWS_)
	WSACleanup();
#endif

    return 0;
}

