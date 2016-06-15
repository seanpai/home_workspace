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

#include "CommTest.h"

using namespace std;

//using FUNC_TYPE = int(const std::string&);

static std::mutex g_mutex_Printout;
const static std::string strMainBdIpAddr("192.168.1.128");

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

	fd_set readFd, tempFdList;
	struct timeval sTimeout;
	FD_ZERO(&readFd);
	FD_SET(nListenChanl,&readFd);

	std::shared_ptr<char> RecvBuffer(new char[1024], [](char* ptr){ delete [] ptr;});
	std::shared_ptr<char> RefBuffer(new char[1024], [](char* ptr){ delete [] ptr;});

	CommTestFrame DataToSend;
	::memset(&DataToSend, 0x00, sizeof(CommTestFrame));
	
	unsigned int nLoopCount = 0;

	while(TRUE)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		
		DataToSend.StartFrame = htonl(STARTFRAME);
		DataToSend.EndFrame = htonl(ENDFRAME);
		DataToSend.nCounter = htonl(nLoopCount);

		// Send test-data to main B/D


		sTimeout.tv_sec = 0;
		sTimeout.tv_usec = 0;
		tempFdList = readFd;
		int nRetValue = ::select(nListenChanl+1, &tempFdList, NULL, NULL, &sTimeout);
		if(0 < nRetValue)
		{
			for(unsigned int nIndex = 0; nIndex < readFd.fd_count; nIndex++)
			{
				if(FD_ISSET(readFd.fd_array[nIndex], &tempFdList))
				{
					if(readFd.fd_array[nIndex] == nListenChanl)	// Some data has been received
					{
						sockaddr_in AddrSender;
						int nSockAddrSize = sizeof(sockaddr_in);
						::recvfrom(nListenChanl, RecvBuffer.get(), 1024, 0, (struct sockaddr*)&AddrSender, &nSockAddrSize);
					
					}
				}
			}

		}
		else if(0 == nRetValue)
		{
			; // timeout
		}
		else
		{
			;
		}


	}

	closesocket(nListenChanl);
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

