/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/retired/haathi/traitorTracer/com/haathi/watermark/metadata/TrustedMetadataProvider.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__METADATA__TRUSTED_METADATA_PROVIDER_HPP
#	define COM__HAATHI__WATERMARK__METADATA__TRUSTED_METADATA_PROVIDER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/CryptoInfo.hpp"
#include "com/haathi/network/NetworkInfo.hpp"
#include "com/haathi/network/Packet.hpp"
#include "com/haathi/watermark/metadata/MetadataProvider.hpp"

#include <string>
#include <map>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace metadata {
/******************************************************************************
Class
******************************************************************************/
class TrustedMetadataProvider: public MetadataProvider {
	private:
		std::string username;
		std::string password;
		std::string certificatePath;
		com::haathi::network::NetworkInfo networkInfo;
	protected:
		bool log;
		void saveResponse(com::haathi::network::ResponsePacket& responsePacket, char* filename);
	public:
		TrustedMetadataProvider(std::string username, std::string password, std::string certificatePath, bool log);
		std::map<int, std::string> getDocuments();
		std::map<int, com::haathi::crypto::CryptoInfo> getDocumentKeys();
		std::map<int, com::haathi::crypto::CryptoInfo> getUsers();
};
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
