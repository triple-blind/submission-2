/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/cyasslSocketTest/cyasslSocketTest.cpp#1 $
/******************************************************************************
Include others
******************************************************************************/
#include <iostream> // cout

#include <cyassl/ssl.h> // CyaSSL_Init()
#include <cyassl/test.h> // StartTCP()

#if defined(WIN32)
#	include <ws2tcpip.h> // socket(), htons()
#else
#	include	<sys/socket.h> // socket, htons()
#endif
/******************************************************************************
Define
******************************************************************************/
#define PORT 9987
#define SERVER_ADDRESS "127.0.0.1"
#define	SA struct sockaddr
/******************************************************************************
Content
******************************************************************************/
bool cyasslSocketTest() {
	StartTCP();
	CyaSSL_Init();

	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return true; // Error with sockets.

	struct sockaddr_in servaddr;
	memset(&servaddr, sizeof(servaddr),0);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, SERVER_ADDRESS, &servaddr.sin_addr) < 0)
		return true; // Error during address conversion.

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		return true; // Is the trusted server running at this address?
	return false;
}

int passfail(bool testResult) {
	std::cout << (testResult ? "Failed" : "Passed") << std::endl;
	return testResult ? 1 : 0;
}

int main(int argc, char* argv[]) {
	return passfail(cyasslSocketTest());
}
/*****************************************************************************/
