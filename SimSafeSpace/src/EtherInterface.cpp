#include "EtherInterface.h"
#include <mutex>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <thread>
#include <iostream>
#include <chrono>
#include <future>


#include <netdb.h>
#include "CommTest.h"
#include <condition_variable>

using namespace std;

extern std::mutex g_mutex_UserTerminate;
extern std::mutex g_mutex_Printout;
extern bool g_bTerminate;
extern std::condition_variable g_condition_UserTerminate;


int ThreadSimSpaceReceiver(int nListenSocket)
{
	if(INVALID_SOCKET == nListenSocket)
		return -1;
#if 0
	fd_set readFd, tempFdList;
	struct timeval sTimeout;
	FD_ZERO(&readFd);
	FD_SET(nListenSocket,&readFd);

	std::shared_ptr<char> RecvBuffer(new char[1024], [](char* ptr){ delete [] ptr;});
	uint32_t nPrevCounter = 0;

	while(1)
	{
		sTimeout.tv_sec = 0;
		sTimeout.tv_usec = 1000;
		tempFdList = readFd;
		int nRetValue = ::select(nListenSocket+1, &tempFdList, NULL, NULL, &sTimeout);
		if(0 < nRetValue)
		{
			//for(unsigned int nIndex = 0; nIndex < readFd.fd_count; nIndex++)
			{
				if(FD_ISSET(readFd, &tempFdList))
				{
					if(readFd == nListenSocket)	// Some data has been received
					{
						sockaddr_in AddrSender;
						int nSockAddrSize = sizeof(sockaddr_in);
						int nRecvByte = recvfrom(nListenSocket, RecvBuffer.get(), 1024, 0, (struct sockaddr*)&AddrSender, &nSockAddrSize);
						cout<<"[ThreadSimSafeSpace] RecvByte: "<<nRecvByte<<endl;

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
#endif
	return 0;
}
int ThreadSimSafeSpace(int nListenChanl)
{
#if 0
	sockaddr_in AddrMainBd;
	nListenChanl = ::socket(PF_INET, SOCK_DGRAM, 0);
	if(INVALID_SOCKET == nListenChanl)
	{
		std::lock_guard<std::mutex> Lock(g_mutex_Printout);
		cout<<"socket() error"<<endl;
		return -1;
	}

	AddrMainBd.sin_family = AF_INET;
	AddrMainBd.sin_addr = inet_addr("192.168.1.128");
	AddrMainBd.sin_port = htons(5001);

	CommTestFrame DataToSend;
	memset(&DataToSend, 0x00, sizeof(CommTestFrame));

	unsigned int nLoopCount = 0;

	uint32_t nPrevCounter = 0;
	while(1)
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
#endif
	return 0;
}

ETHER_INTERFACE::ETHER_INTERFACE(const std::string& strIpAddr,  uint16_t nPortNum)	:
	m_strIpAddr(strIpAddr),
	m_nListenPortNum(nPortNum),
	m_nStatus(ETHER_STATUS::STATUS_FAIL)
{
	
	sockaddr_in AddrClient;
	m_nSocket = socket(PF_INET, SOCK_DGRAM, 0);
	if(INVALID_SOCKET == m_nSocket)
	{
		std::lock_guard<std::mutex> Lock(g_mutex_Printout);
		cout<<"[ETHER_INTERFACE] socket() error"<<endl;
		m_nStatus = ETHER_STATUS::STATUS_FAIL;
		return;
	}

	AddrClient.sin_family = AF_INET;
	AddrClient.sin_addr.s_addr = inet_addr(m_strIpAddr.c_str());
	AddrClient.sin_port = htons(m_nListenPortNum);

	if(SOCKET_ERROR == ::bind(m_nSocket, (sockaddr*)&AddrClient, sizeof(sockaddr_in)))
	{
		std::lock_guard<std::mutex> Lock(g_mutex_Printout);
		cout<<"[ETHER_INTERFACE] bind() error"<<endl;
		m_nStatus = ETHER_STATUS::STATUS_FAIL;
		m_nSocket = INVALID_SOCKET;
		return;
	}
	else
	{
		cout<<"[ETHER_INTERFACE] binding socket is successful, IP:"<<inet_ntoa(AddrClient.sin_addr)<<", Port:"<<m_nListenPortNum<<endl;
	}
	m_nStatus = ETHER_STATUS::STATUS_OK;

}

ETHER_INTERFACE::~ETHER_INTERFACE()
{
	if(INVALID_SOCKET!=m_nSocket)
	{
		//closesocket(m_nSocket);
		close(m_nSocket);

	}

	cout<<"[ETHER_INTERFACE] terminated"<<endl;
}

std::shared_ptr<ETHER_INTERFACE> ETHER_INTERFACE::CreateEtherInterface(const std::string& strIpAddr, uint16_t nPortNum)
{
	return std::shared_ptr<ETHER_INTERFACE>(new ETHER_INTERFACE(strIpAddr.c_str(), nPortNum));
}

int32_t ETHER_INTERFACE::GetStatus()
{
	return static_cast<int32_t>(m_nStatus);
}

int32_t ETHER_INTERFACE::GetSocketHandle()
{
	return m_nSocket;
}
