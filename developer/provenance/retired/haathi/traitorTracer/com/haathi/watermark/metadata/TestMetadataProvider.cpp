/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/retired/haathi/traitorTracer/com/haathi/watermark/metadata/TestMetadataProvider.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/metadata/TestMetadataProvider.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/network/TrustedServer.hpp"

#include <fstream>
#include <sstream>
/******************************************************************************
Using
******************************************************************************/
using com::haathi::crypto::CryptoInfo;
using com::haathi::network::GetAllUsersResponse;
using com::haathi::network::GetAllDocumentsResponse;
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
static std::string loadResponse(std::string filename) {
	std::ifstream myfile(filename.c_str());
	std::stringstream buffer;
	std::string response;

	buffer << myfile.rdbuf();
	myfile.close();
	response = buffer.str();
	return response;
}

TestMetadataProvider::TestMetadataProvider(const char* users, const char* documents, const char* documentKeys) {
	this->users = users;
	this->documents = documents;
	this->documentKeys = documentKeys;
}

std::map<int, std::string> TestMetadataProvider::getDocuments() {
	GetAllDocumentsResponse response;

	response.setPacket(loadResponse(documents));
	return response.getDocuments();
}

std::map<int, CryptoInfo> TestMetadataProvider::getDocumentKeys() {
	GetAllDocumentsResponse response;

	response.setPacket(loadResponse(documentKeys));
	return response.getDocumentKeys();
}

std::map<int, CryptoInfo> TestMetadataProvider::getUsers() {
	GetAllUsersResponse response;

	response.setPacket(loadResponse(users));
	return response.getUsers();
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
