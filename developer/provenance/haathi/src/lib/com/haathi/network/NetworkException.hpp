/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/network/NetworkException.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__NETWORK__NETWORK_EXCEPTION_HPP
#	define COM__HAATHI__NETWORK__NETWORK_EXCEPTION_HPP
/******************************************************************************
Include
******************************************************************************/
#include <string>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace network {
/******************************************************************************
Class
******************************************************************************/
class NetworkException {
	private:
		std::string message;

	public:
		NetworkException(std::string message): message(message) {
		}

		~NetworkException() {
		}

		std::string getMessage() {
			return message;
		}
	};

class SSLInitializationException: public NetworkException {
	public:
		SSLInitializationException(std::string message): NetworkException(message) {
		}
};

class SSLCertificateLoadException: public NetworkException {
	public:
		SSLCertificateLoadException(std::string message): NetworkException(message) {
	}
};

class SSLKeyLoadException: public NetworkException {
	public:
		SSLKeyLoadException(std::string message): NetworkException(message) {
		}
};

class SSLSocketException: public NetworkException {
	public:
		SSLSocketException(std::string message): NetworkException(message) {
		}
};

class AddressConversionException: public NetworkException {
	public:
		AddressConversionException(std::string message): NetworkException(message) {
		}
};

class SSLEndpointBindException: public NetworkException {
	public:
		SSLEndpointBindException(std::string message): NetworkException(message) {
		}
};

class SSLSendDataException: public NetworkException {
	public:
		SSLSendDataException(std::string message): NetworkException(message) {
		}
};

class SSLReceiveDataException: public NetworkException {
	public:
		SSLReceiveDataException(std::string message): NetworkException(message) {
		}
};

class AuthenticationCredentialsMissingException: public NetworkException {
	public:
		AuthenticationCredentialsMissingException(std::string message): NetworkException(message) {
		}
};

class InvalidAuthenticationCredentialsException: public NetworkException {
	public:
		InvalidAuthenticationCredentialsException(std::string message): NetworkException(message) {
		}
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
