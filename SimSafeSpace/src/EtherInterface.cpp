#include "EtherInterface.h"
#include <mutex>
#include <iostream>

using namespace std;

extern std::mutex g_mutex_Printout;

ETHER_INTERFACE::ETHER_INTERFACE(const std::string& strIpAddr,  uint16_t nPortNum)	:
	m_strIpAddr(strIpAddr),
	m_nListenPortNum(nPortNum),
	m_nStatus(ETHER_STATUS::STATUS_FAIL)
{
	
	sockaddr_in AddrClient;
	m_nSocket = ::socket(PF_INET, SOCK_DGRAM, 0);
	if(INVALID_SOCKET == m_nSocket)
	{
		std::lock_guard<std::mutex> Lock(g_mutex_Printout);
		cout<<"[ETHER_INTERFACE] socket() error"<<endl;
		m_nStatus = ETHER_STATUS::STATUS_FAIL;
		return;
	}

	AddrClient.sin_family = AF_INET;
	AddrClient.sin_addr.S_un.S_addr = inet_addr(m_strIpAddr.c_str());
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
		closesocket(m_nSocket);

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
