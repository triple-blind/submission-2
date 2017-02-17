/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/network/Server.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/network/Server.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/network/Packet.hpp"

#include "cyasslBeg.hpp"
#include <cyassl/test.h> // StartTCP
#include "cyasslEnd.hpp"

#include <cstdlib>
#include <iostream>
#include <memory.h> // memset
#include <sstream>

#if OSYS == OSYS_WIN
#	include <ws2tcpip.h> // inet_pton
#endif
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::crypto::ProvenanceHelper;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace network {
/******************************************************************************
Class
******************************************************************************/
static const int MAXLINE = 4096;
#if !defined(SA)
#	define	SA	struct sockaddr
#endif

//-----------------------------------------------------
// Server Constuctors and Destructor
//-----------------------------------------------------
Server::Server(NetworkInfo networkInfo) throw (SSLInitializationException,
		SSLCertificateLoadException, SSLKeyLoadException, SSLSocketException,
		AddressConversionException, SSLEndpointBindException,
		SSLSendDataException, SSLReceiveDataException):
		ctx(NULL), ssl(NULL), privBuff(networkInfo.privateKey.key),
		certBuff(networkInfo.publicCertificate.key), refCount(1), networkInfo(networkInfo) {
	initialize();
	setupConnection();
}

Server& Server::operator=(Server& other) {
	// Assign everything over.
	ctx = other.ctx;
	ssl = other.ssl;
	sockfd = other.sockfd;
	servaddr = other.servaddr;
	privBuff = other.privBuff;
	certBuff = other.certBuff;
	serverAddress = other.serverAddress;
	port = other.port;
	privBuffSize = other.privBuffSize;
	certBuffSize = other.certBuffSize;
	networkInfo = other.networkInfo;

	// Increment the reference count by one, since now an additional object has a reference.
	refCount++;
	return *this;
}

Server::~Server() throw () {
	// Check the reference count and only free stuff if there are no more references.
	// Of course, calling the destructor reduces the reference by one as it is.
	if (--refCount == 0) {
		if (ssl)
			CyaSSL_free(ssl);
		if (ctx)
			CyaSSL_CTX_free(ctx);
		CyaSSL_Cleanup();
	}
}

const char *Server::getServerAddress() {
	return serverAddress.c_str();
}
//-----------------------------------------------------
// Private Helper Functions
//-----------------------------------------------------

void errorLoading(int error, std::string path) {
	if (error == SSL_SUCCESS)
		return;

	std::string message = "Error loading \"";
	message.append(path);
	message.append("\", please check the file.\n");
	std::cerr << message << std::endl;
	throw network::SSLCertificateLoadException(message);
}

// kwa: With an argument of NetworkInfo networkInfo, a copy of networkInfo is created
// when this function is called.  It disappears upon return, which
// means that serverAddress will disappear.  The arguments has been
// changed to be a reference instead.
// NetworkInfo has now been made a member variable and doesn't need to be passed.
void Server::initialize() {
	serverAddress = networkInfo.host;
	port = networkInfo.portNumber;

	privBuffSize = networkInfo.privateKey.keySize;
	certBuffSize = networkInfo.publicCertificate.keySize;

	StartTCP();
	CyaSSL_Init();

	// We're the client and using TLS 1.2, previously just 1
	if ((ctx = CyaSSL_CTX_new(CyaTLSv1_2_client_method())) == NULL) {
		std::cerr << "SSL_CTX_new error." << std::endl;
		throw network::NetworkException(
				"SSL_CTX_new error. Error initializing TLS protocol");
	}

	// So that client can verify server's identity.  Shoud be CA certificates in PEM format.
	// Is there a localhost option?
	if (networkInfo.caCertificatePath.length() > 0) {
		int error = CyaSSL_CTX_load_verify_locations(ctx,
				networkInfo.caCertificatePath.c_str(), 0);
		errorLoading(error, networkInfo.caCertificatePath);
	}

	// So that server (Provenance) can verify client's identity.  Shoud be in PEM format.
	if (networkInfo.publicKeyPath.length() > 0) {
		int error = CyaSSL_CTX_use_certificate_chain_file(ctx,
				networkInfo.publicKeyPath.c_str());
		errorLoading(error, networkInfo.publicKeyPath);
	}

	// So that server (Provenance) can verify client's identity.  Shoud be in PEM format.
	if (networkInfo.privateKeyPath.length() > 0) {
		int error = CyaSSL_CTX_use_PrivateKey_file(ctx,
				networkInfo.privateKeyPath.c_str(), SSL_FILETYPE_PEM);
		errorLoading(error, networkInfo.privateKeyPath);
	}
}

void initSockAddr(struct sockaddr_in *sockaddr, const char *address, int port) {
	// Zero out possibly unused memory
	memset(sockaddr, sizeof(*sockaddr), 0);
	// IPv4
	sockaddr->sin_family = AF_INET;
	// Convert port from host to network byte order
	sockaddr->sin_port = htons(port);
	// Converts an IPv4 or IPv6 Internet network address in its standard text
	// presentation form into its numeric binary form
	if (inet_pton(AF_INET, address, &sockaddr->sin_addr) < 0) {
		throw network::AddressConversionException(
				"Error during address conversion.");
	}
}

void Server::setupConnection() {
	// In case this must be done first!
	CyaSSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);

	if (certBuff.length() > 0) {
		std::string decodedCertificate = ProvenanceHelper::base64Decode(certBuff);

		if (CyaSSL_CTX_use_certificate_buffer(ctx,
				(unsigned char*) decodedCertificate.c_str(), certBuffSize,
				SSL_FILETYPE_PEM) != SSL_SUCCESS) {
			throw network::SSLCertificateLoadException(
					"Error loading certificate.");
		}
	}

	if (privBuff.length() > 0) {
		std::string decodedKey = ProvenanceHelper::base64Decode(privBuff);

		if (CyaSSL_CTX_use_PrivateKey_buffer(ctx,
				(unsigned char*) decodedKey.c_str(), privBuffSize,
				SSL_FILETYPE_PEM) != SSL_SUCCESS) {
//		if (CyaSSL_CTX_use_PrivateKey_buffer(ctx,
//				(unsigned char*) privBuff, strlen((char *)privBuff),
//				SSL_FILETYPE_PEM) != SSL_SUCCESS) {
			throw network::SSLKeyLoadException("Error loading private key.");
		}
	}

	/*
		This function sets the verification method for remote peers and also allows a verify
		callback to be registered with the SSL context. The verify callback will be called only
		when a verification failure has occurred. If no verify callback is desired, the NULL
		pointer can be used for verify_callback.

		SSL_VERIFY_PEER
			Client mode: the client will verify the certificate received from the server during
			the handshake. This is turned on by default in CyaSSL, therefore, using this
			option has no effect.
			Server mode: the server will send a certificate request to the client and verify the
			client certificate received.
		SSL_VERIFY_FAIL_IF_NO_PEER_CERT
			Client mode: no effect when used on the client side.
			Server mode: the verification will fail on the server side if the client fails to send
			a certificate when requested to do so (when using SSL_VERIFY_PEER on the
			SSL server).
	*/
	// If there are certificate problems, try this version for testing
	// CyaSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
	// If it's working, this is the intented version.
	CyaSSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		throw network::SSLSocketException("Error with sockets.");
	}

	initSockAddr(&servaddr, serverAddress.c_str(), port);

	// What is difference between connect and Connect?
	// This was lowercase and not working for Windows.
	// CyaSSL handbook uses uppercase, which doesn't even compile.
	// And it is not used in the examples they cite.
	//if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) {
	//if (tcp_connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) {
	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) {
		throw network::SSLEndpointBindException(
				"Error: SSLEndpointBindException.\nIs the trusted server running at this address?");
	}

	if ((ssl = CyaSSL_new(ctx)) == NULL) {
		throw network::NetworkException(
				"SSL_CTX_new error. Error during SSL handshake.");
	}
	CyaSSL_set_fd(ssl, sockfd);
}

//-----------------------------------------------------
// Member Functions
//-----------------------------------------------------
bool Server::sendData(cJSON* jsonObject) throw (SSLSendDataException) {
	char* data = cJSON_Print(jsonObject);
	json_free_ptr json(data);
	// There can't be any \0 in the data for this to work
	int length = strlen(data);
	int n = CyaSSL_write(ssl, data, length);

	if (n != length)
		throw network::SSLSendDataException("Error Sending Data.");
	return n > 0;
}

bool Server::ping() throw (SSLSendDataException, SSLReceiveDataException) {
	PingRequest request;
	PingResponse response;

	sendData(request.getPacket());
	return response.setPacket(receiveData());
}

std::string Server::receiveData() throw (SSLReceiveDataException) {
	char buf[MAXLINE];
	std::stringstream data;
	int n;

	while ((n = CyaSSL_read(ssl, buf, MAXLINE)) > 0) {
		data.write(buf, n);
		if (n != MAXLINE)
			break;
	}
	if (!data.tellp())
		throw network::SSLReceiveDataException("No data received on end point");
	return data.str();
}

bool Server::execute(RequestPacket& request, ResponsePacket& response)
		throw (SSLSendDataException, SSLReceiveDataException){
	bool success = sendData(request.getPacket());

	if (success)
		success = response.setPacket(receiveData());
	return success;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/

