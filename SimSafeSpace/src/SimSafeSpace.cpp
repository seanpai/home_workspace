// SimSafeSpace.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <string>
#include <thread>
#include <iostream>
#include <chrono>
#include <future>
#include <istream>
#include <ostream>
#include <mutex>
#include <condition_variable>
#include <WinSock2.h>

using namespace std;

//using FUNC_TYPE = int(const std::string&);

static std::mutex g_mutex_Printout;

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

int ThreadGetUserInput(const std::string& strAug)
{
	std::cout << strAug.c_str() << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "starting getting the input from user..." << std::endl;

	std::string strInput;
	int nValue = 0;
	do
	{
		std::getline(cin, strInput);
		nValue = strInput.compare("end");
		std::cout << "echo: " << strInput.c_str() << std::endl;
		std::cout << "compare result: " << nValue << std::endl;
	} while (0 != nValue);

	return 1;
}

int ThreadSimSafeSpace()
{
	WSADATA wsaData;

	if(0 != WSAStartup(MAKEWORD(2,2), &wsaData))
	{
		int nRetValue = 0;
		nRetValue = ::WSAGetLastError();
		return nRetValue;
	}

	int nListenChanl = 0;
	sockaddr_in AddrClient;

	nListenChanl = ::socket(PF_INET, SOCK_DGRAM, 0);
	if(INVALID_SOCKET == nListenChanl)
	{
		std::lock_guard<std::mutex> Lock(g_mutex_Printout);
		cout<<"socket() error"<<endl;
		return -1;
	}

	AddrClient.sin_family = AF_INET;
	AddrClient.sin_addr.S_un.S_addr = inet_addr("192.168.1.48");
	AddrClient.sin_port = htons(5001);

	if(SOCKET_ERROR == ::bind(nListenChanl, (sockaddr*)&AddrClient, sizeof(sockaddr_in)))
	{
		std::lock_guard<std::mutex> Lock(g_mutex_Printout);
		cout<<"socket() error"<<endl;
		return -1;
	}

	fd_set readFd;
	struct timeval sTimeout;
	FD_ZERO(&readFd);
	FD_SET(nListenChanl,&readFd);

	sTimeout.tv_sec		= 1;

	int nRetValue = ::select(1, &readFd, NULL, NULL, &sTimeout);


	WSACleanup();

	return 0;
}

int main()
{
	int a = 0;
	//std::thread ThreadInstance(ThreadFunc, std::ref(a));
	std::string strMessage("thread is running");

	std::packaged_task<int(const std::string&)> task(ThreadGetUserInput);
	
	std::future<int> RetValue = task.get_future();

	std::thread ThreadInstance2(std::move(task), std::ref(strMessage));
	ThreadInstance2.detach();
	int nValue = RetValue.get();
	std::cout << "[main] terminated" << std::endl;

    return 0;
}

