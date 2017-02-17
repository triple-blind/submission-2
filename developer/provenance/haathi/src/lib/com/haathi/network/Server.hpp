/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/network/Server.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__NETWORK__SERVER_HPP
#	define COM__HAATHI__NETWORK__SERVER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "Config.hpp"

// The cryptopp files here must come before crypto files
// included elsewhere by the server
#include "com/haathi/crypto/ProvenanceHelper.hpp"
#include "com/haathi/network/NetworkException.hpp"
#include "com/haathi/network/NetworkInfo.hpp"
#include "com/haathi/network/Packet.hpp"

#include "cppJSON.hpp"

#include "sdkBeg.hpp"
#include <osl/socket.h>
#include "sdkEnd.hpp"

#include "cyasslBeg.hpp"
#include <cyassl/ssl.h>
#include <cyassl/ctaocrypt/rsa.h>
#include "cyasslEnd.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#if OSYS != OSYS_WIN
#	include	<unistd.h>
#	include	<sys/socket.h>
#	include	<netinet/in.h>
#	include	<arpa/inet.h>
#else
#	if defined(MessageBox)
#		include <winsock2.h> // sockaddr_in
#	else
#		include <winsock2.h> // sockaddr_in
#		if defined(MessageBox) // It wasn't defined before, so it shouldn't be now!
#			undef MessageBox
#		endif
#	endif
#endif
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace network {
/******************************************************************************
Class
******************************************************************************/
class Server {
	private:
		CYASSL_CTX* ctx;
		CYASSL* ssl;
		int sockfd;
		struct sockaddr_in servaddr;
		std::string& privBuff;
		std::string& certBuff;
		std::string serverAddress;
		int port;
		long privBuffSize;
		long certBuffSize;

		// I added this to implement reference-counting. This prevents us from freeing pointers
		// to CyaSSL stuff at the wrong time. -- Aaron
		int refCount;

		void initialize();
		void setupConnection();

	protected:
		NetworkInfo networkInfo;
		bool execute(RequestPacket& request, ResponsePacket& response)
				throw (SSLSendDataException, SSLReceiveDataException);

	public:
		const char *getServerAddress();
		/*!
		 Default Constructor
		 
		 This constructor will initialize a connection to the address specified in 
		 the passed NetworkInfo object. The constructor will throw exceptions for
		 all types of problems that can occur when setting up the connection.
		 
		 @param networkInfo -- a NetworkInfo object specifying all connection details.
		 @throw SSLInitializationException --
		 @throw SSLCertificateLoadException -- thrown when the certificate could not be loaded (check filepath)
		 @throw SSLKeyLoadException -- thrown when the key could not be loaded.
		 @throw SSLSocketException -- thrown when the Sockets could not be initialized or connected.
		 @throw AddressConversionException -- thrown when the IP address could not be found...?
		 @throw SSLEndpointBindException -- I don't know.
		 @throw SSLSendDataException -- thrown when there was an error sending data.
		 @throw SSLReceiveException -- thrown when there was an error receiving data.
		 */
		Server(NetworkInfo networkInfo) throw (SSLInitializationException,
				SSLCertificateLoadException, SSLKeyLoadException,
				SSLSocketException, AddressConversionException,
				SSLEndpointBindException, SSLSendDataException,
				SSLReceiveDataException);
/*
		Server(NetworkInfo networkInfo, bool connect) throw (SSLInitializationException,
				SSLCertificateLoadException, SSLKeyLoadException,
				SSLSocketException, AddressConversionException,
				SSLEndpointBindException, SSLSendDataException,
				SSLReceiveDataException);
*/
		/*!
		 Assignment Operator
		 
		 -- Should this be protected?
		 
		 This copies all of the information from one instance of Server to another.
		 
		 NOTE -- it is extremely important to note that this implementation uses a 
		 reference counting system!
		 
		 @param other -- the instance to assign from.
		 @result -- this instance of Server.
		 */
		Server& operator=(Server& other);

		/*!
		 Standard Destructor.
		 */
		virtual ~Server() throw ();

		bool ping() throw (SSLSendDataException, SSLReceiveDataException);

		bool sendData(cJSON* packet) throw (SSLSendDataException);

		std::string receiveData() throw (SSLReceiveDataException);
};
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
