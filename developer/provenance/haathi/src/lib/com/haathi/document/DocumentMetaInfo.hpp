/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/DocumentMetaInfo.hpp#1 $

/*
 *  Created by Aaron Gibson on 2/24/12.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__DOCUMENT_META_INFO_HPP
#	define COM__HAATHI__DOCUMENT__DOCUMENT_META_INFO_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/CryptoInfo.hpp"
#include "com/haathi/crypto/Key.hpp"
#include "com/haathi/network/NetworkInfo.hpp"
#include "com/haathi/network/UserInfo.hpp"

#include <map>
#include <string>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
Class
******************************************************************************/
struct DocumentMetaInfo {
	int documentId;
	std::string versionId;
	int versionNo;
	std::string documentTitle;
	com::haathi::crypto::Key documentEncryptionKey;
	com::haathi::crypto::CryptoInfo provenanceEncryption;
	com::haathi::network::NetworkInfo cloudInfo;
	com::haathi::network::UserInfo userInfo;
	std::map<int, com::haathi::crypto::CryptoInfo> auditorList;
	std::string previousSavedEvent;
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
