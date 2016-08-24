#pragma once

#ifdef _WINDOWS
#include <WinSock2.h>
#elif defined(__GNUC__)
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#else

#endif



#include <memory>
#include <string>

enum class ETHER_STATUS : int32_t
{
	STATUS_FAIL,
	STATUS_OK
};

class ETHER_INTERFACE
{
private:
	explicit ETHER_INTERFACE(const std::string& strIpAddr, uint16_t nPortNum);
	
	ETHER_INTERFACE(const ETHER_INTERFACE&);
	const ETHER_INTERFACE& operator=(const ETHER_INTERFACE&);

	std::string m_strIpAddr;
	uint16_t m_nListenPortNum;
	int32_t m_nSocket;
	ETHER_STATUS m_nStatus;

public:
	
	static std::shared_ptr<ETHER_INTERFACE> CreateEtherInterface(const std::string& strIpAddr, uint16_t nPortNum);
	virtual ~ETHER_INTERFACE();
	int32_t GetStatus();
	int32_t GetSocketHandle();

};
