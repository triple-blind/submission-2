/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/network/NetworkInfo.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__NETWORK__NETWORK_INFO_HPP
#	define COM__HAATHI__NETWORK__NETWORK_INFO_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/Serialization.hpp"

#include <map>
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
struct NetworkKey {
	std::string key;
	long keySize;

	NetworkKey() {
		key = "";
		keySize = 0;
	}
};
			
struct NetworkInfo {
	std::string host;
	int portNumber;
	std::string caCertificatePath;
	std::string publicKeyPath;
	std::string privateKeyPath;
	struct NetworkKey privateKey;
	struct NetworkKey publicCertificate;

	NetworkInfo() {
		host = "";
		portNumber = 0;
		caCertificatePath = "";
		publicKeyPath = "";
		privateKeyPath = "";
	}

	NetworkInfo(std::string host, int portNumber, std::string caCertificatePath) {
		this->host = host;
		this->portNumber = portNumber;
		this->caCertificatePath = caCertificatePath;
		publicKeyPath = "";
		privateKeyPath = "";
	}

	NetworkInfo(std::string host, int portNumber, std::string caCertificatePath,
		std::string publicKeyPath, std::string privateKeyPath) {
		this->host = host;
		this->portNumber = portNumber;
		this->caCertificatePath = caCertificatePath;
		this->publicKeyPath = publicKeyPath;
		this->privateKeyPath = privateKeyPath;
	}

	template<typename Archive> void serialize(Archive & ar, const unsigned int version) {

		ar & BOOST_SERIALIZATION_NVP(host);
		ar & BOOST_SERIALIZATION_NVP(portNumber);
		ar & BOOST_SERIALIZATION_NVP(caCertificatePath);
		ar & BOOST_SERIALIZATION_NVP(publicKeyPath);
		ar & BOOST_SERIALIZATION_NVP(privateKeyPath);

	}

	friend std::ostream& operator<<(std::ostream & stream, const NetworkInfo& info);
};

std::map<std::string, NetworkInfo> generateDefaultNetworkInfos();
std::map<std::string, NetworkInfo> loadNetworkInfos(std::string filename);
void saveNetworkInfos(std::string filename, std::map<std::string, NetworkInfo>& networkInfos);
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
