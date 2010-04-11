#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <string>

#ifdef _MSC_VER
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
#ifdef __SUNPRO_CC
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
#else
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <string.h>
#endif
#endif

#ifndef NI_MAXHOST
	#define NI_MAXHOST  1025
	#define NI_MAXSERV    32
	#ifndef NI_NUMERICHOST
	#define NI_NUMERICHOST  2
	#endif
	#ifndef NI_NUMERICSERV
	#define NI_NUMERICSERV  8
	#endif
	#define socklen_t int
#endif

const int MAX_SEND_LENGTH = 1024;
const int MAX_RECV_LENGTH = 4096;

class Socket
{
public:
	Socket();
	Socket(int port);
	Socket(std::string host, int port);
	
	virtual ~Socket();
	
	bool create();
	bool bind(const int port);
	bool listen(const int connections) const;
	bool accept(Socket & sock) const;
	bool accept(Socket *sock) const;
	
	bool connect();
	bool connect(const std::string host, const int port);
	void close();
	
	bool send(const std::string data) const;
	int recv(std::string &data) const;
	
	bool isValid() const { return m_sock != -1; }
	
	std::string getClientHost();
	std::string getClientAddress();
	int getClientPort() { return m_clientPort; }
	
	static bool initWinsocks();
	static bool cleanupWinsocks();

protected:
	sockaddr_in m_addr;
	int m_sock;
	
	int m_port;
	std::string m_host;
	
	std::string m_clientHost;
	std::string m_clientAddress;
	int m_clientPort;	
};

#endif
