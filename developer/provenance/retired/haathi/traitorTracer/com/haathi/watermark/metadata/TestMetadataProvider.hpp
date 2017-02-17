/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/retired/haathi/traitorTracer/com/haathi/watermark/metadata/TestMetadataProvider.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__METADATA__TEST_METADATA_PROVIDER_HPP
#	define COM__HAATHI__WATERMARK__METADATA__TEST_METADATA_PROVIDER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/CryptoInfo.hpp"
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
class TestMetadataProvider: public MetadataProvider {
	protected:
		std::string users;
		std::string documents;
		std::string documentKeys;

	public:
		TestMetadataProvider(const char* _documents, const char* _documentKeys, const char* _users);
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
