/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/retired/haathi/traitorTracer/com/haathi/watermark/metadata/MetadataProvider.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__METADATA__METADATA_PROVIDER_HPP
#	define COM__HAATHI__WATERMARK__METADATA__METADATA_PROVIDER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/CryptoInfo.hpp"

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
class MetadataProvider {
	public:
		virtual std::map<int, std::string> getDocuments() = 0;
		virtual std::map<int, com::haathi::crypto::CryptoInfo> getDocumentKeys() = 0;
		virtual std::map<int, com::haathi::crypto::CryptoInfo> getUsers() = 0;
		virtual ~MetadataProvider() { };
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
