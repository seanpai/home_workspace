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
#include "EtherInterface.h"

using namespace std;

//using FUNC_TYPE = int(const std::string&);

std::mutex g_mutex_Printout;
static std::mutex g_mutex_UserTerminate;
static bool g_bTerminate = false;

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
		if(10 == nLoopCount)
		{
			cout<<"[ThreadCount] this loop ends"<<endl;
			break;
		}
	}

	g_bTerminate = true;
	g_condition_UserTerminate.notify_all();

	return 1;
}

int ThreadSimSafeSpace(int nListenChanl)
{

	sockaddr_in AddrMainBd;
	nListenChanl = ::socket(PF_INET, SOCK_DGRAM, 0);
	if(INVALID_SOCKET == nListenChanl)
	{
		std::lock_guard<std::mutex> Lock(g_mutex_Printout);
		cout<<"socket() error"<<endl;
		return -1;
	}

	AddrMainBd.sin_family = AF_INET;
	AddrMainBd.sin_addr.S_un.S_addr = inet_addr("192.168.1.128");
	AddrMainBd.sin_port = htons(5001);

	CommTestFrame DataToSend;
	::memset(&DataToSend, 0x00, sizeof(CommTestFrame));
	
	unsigned int nLoopCount = 0;

	uint32_t nPrevCounter = 0;
	while(TRUE)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		
		nLoopCount++;
		
		DataToSend.StartFrame = htonl(STARTFRAME);
		DataToSend.EndFrame = htonl(ENDFRAME);
		DataToSend.nCounter = htonl(nLoopCount);

		// Send test-data to main B/D
		int nSendByte = ::sendto(nListenChanl, reinterpret_cast<char*>(&DataToSend), sizeof(CommTestFrame), 0, reinterpret_cast<struct sockaddr*>(&AddrMainBd), sizeof(struct sockaddr));
		if(SOCKET_ERROR == nSendByte)
		{
			cout<<"[ThreadSimSafeSpace] fail to sendto"<<endl;
		}

		{
			std::unique_lock<std::mutex> lock(g_mutex_UserTerminate);
			bool bRetValue = g_condition_UserTerminate.wait_for(lock, std::chrono::microseconds(0), [](){ return (true==g_bTerminate);});
			if(true == bRetValue)
			{
				cout<<"[ThreadSimSpaceReceiver] this loop ends"<<endl;
				break;
			}
		}
		
	}

	return 0;
}

int ThreadSimSpaceReceiver(int nListenSocket)
{
	if(INVALID_SOCKET == nListenSocket)
		return -1;

	fd_set readFd, tempFdList;
	struct timeval sTimeout;
	FD_ZERO(&readFd);
	FD_SET(nListenSocket,&readFd);

	std::shared_ptr<char> RecvBuffer(new char[1024], [](char* ptr){ delete [] ptr;});
	uint32_t nPrevCounter = 0;

	while(TRUE)
	{
		sTimeout.tv_sec = 0;
		sTimeout.tv_usec = 1000;
		tempFdList = readFd;
		int nRetValue = ::select(nListenSocket+1, &tempFdList, NULL, NULL, &sTimeout);
		if(0 < nRetValue)
		{
			for(unsigned int nIndex = 0; nIndex < readFd.fd_count; nIndex++)
			{
				if(FD_ISSET(readFd.fd_array[nIndex], &tempFdList))
				{
					if(readFd.fd_array[nIndex] == nListenSocket)	// Some data has been received
					{
						sockaddr_in AddrSender;
						int nSockAddrSize = sizeof(sockaddr_in);
						int nRecvByte = ::recvfrom(nListenSocket, RecvBuffer.get(), 1024, 0, (struct sockaddr*)&AddrSender, &nSockAddrSize);	
						//cout<<"[ThreadSimSafeSpace] RecvByte: "<<nRecvByte<<endl;

						CommTestFrame RecvDataFrame;
						memcpy(reinterpret_cast<char*>(&RecvDataFrame), RecvBuffer.get(), nRecvByte);
						uint32_t nCurrCounter = ntohl(RecvDataFrame.nCounter);
						if(nCurrCounter != nPrevCounter + 1)
						{
							cout<<"[ThreadSimSpaceReceiver] Counter didn't match, CurrCounter: "<<nCurrCounter<<", PrevCounter: "<<nPrevCounter<<endl;
						}

						nPrevCounter = nCurrCounter;
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

		{
			std::unique_lock<std::mutex> lock(g_mutex_UserTerminate);
			bool bRetValue = g_condition_UserTerminate.wait_for(lock, std::chrono::microseconds(0), [](){ return (true==g_bTerminate);});
			if(true == bRetValue)
			{
				cout<<"[ThreadSimSpaceReceiver] this loop ends"<<endl;
				break;
			}
		}
	}
	return 0;
}

int main()
{
	WSADATA wsaData;

	if(0 != WSAStartup(MAKEWORD(2,2), &wsaData))
	{
		int nRetValue = 0;
		nRetValue = ::WSAGetLastError();
		return nRetValue;
	}

	int nClientSocket = 0;

	string strIpAddr("192.168.1.48");
	std::shared_ptr<ETHER_INTERFACE> EthInterface = ETHER_INTERFACE::CreateEtherInterface(std::ref(strIpAddr), 5001);

	// sender thread
	std::packaged_task<int(int)> TaskSender(ThreadSimSafeSpace);
	std::future<int> futureThreadSender = TaskSender.get_future();
	std::thread ThreadSender(std::move(TaskSender), EthInterface->GetSocketHandle());
	ThreadSender.detach();

	// receiver thread
	std::packaged_task<int(int)> TaskReceiver(ThreadSimSpaceReceiver);
	std::future<int> futureThreadReceiver = TaskReceiver.get_future();
	std::thread ThreadReceiver(std::move(TaskReceiver), EthInterface->GetSocketHandle());
	ThreadReceiver.detach();

	// Timer thread
	std::string strMessage("ThreadCount is running");
	std::packaged_task<int(const std::string&)> TaskCount(ThreadCount);
	std::future<int> futureTaskCount = TaskCount.get_future();
	std::thread ThreadCount(std::move(TaskCount),  std::ref(strMessage));
	ThreadCount.detach();

	int nValue = 0;
	nValue = futureTaskCount.get();
	nValue = futureThreadSender.get();
	nValue = futureThreadReceiver.get();

	std::cout << "[main] terminated" << std::endl;
	WSACleanup();

    return 0;
}

