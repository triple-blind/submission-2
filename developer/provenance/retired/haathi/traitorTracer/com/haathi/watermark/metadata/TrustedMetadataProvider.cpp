/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/retired/haathi/traitorTracer/com/haathi/watermark/metadata/TrustedMetadataProvider.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/metadata/TrustedMetadataProvider.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/network/TrustedServer.hpp"

#include <fstream>
/******************************************************************************
Using
******************************************************************************/
using com::haathi::crypto::CryptoInfo;
using com::haathi::network::GetAllUsersRequest;
using com::haathi::network::GetAllUsersResponse;
using com::haathi::network::GetAllDocumentsRequest;
using com::haathi::network::GetAllDocumentsResponse;
using com::haathi::network::ResponsePacket;
using com::haathi::network::TrustedServer;
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
TrustedMetadataProvider::TrustedMetadataProvider(std::string username, std::string password, std::string certPath, bool log):
		username(username), password(password), certificatePath(certPath), log(log) {
	networkInfo.host = "127.0.0.1"; // localhost
//	networkInfo.host = "128.196.203.26"; // escrow.cs.arizona.edu
	networkInfo.portNumber = 9987;
	networkInfo.caCertificatePath = certificatePath;
}

void TrustedMetadataProvider::saveResponse(ResponsePacket& responsePacket, char* filename) {
	if (!log)
		return;

	std::ofstream myfile(filename);

	// This must be done a different way for debugging, possibly bo turning on logging
//	myfile << responsePacket.getJSONString();
	myfile.close();
}

std::map<int, std::string> TrustedMetadataProvider::getDocuments() {
	TrustedServer server(networkInfo);
	GetAllDocumentsRequest request(username, password);
	GetAllDocumentsResponse response;

	server.getAllDocuments(request, response);
//	saveResponse(response, "documents.json");
	return response.getDocuments();
}

std::map<int, CryptoInfo> TrustedMetadataProvider::getDocumentKeys() {
	TrustedServer server(networkInfo);
	GetAllDocumentsRequest request(username, password);
	GetAllDocumentsResponse response;

	server.getAllDocuments(request, response);
//	saveResponse(response, "documentKeys.json");
	return response.getDocumentKeys();
}

std::map<int, CryptoInfo> TrustedMetadataProvider::getUsers() {
	TrustedServer server(networkInfo);
	GetAllUsersRequest request(username, password);
	GetAllUsersResponse response;

	server.getAllUsers(request, response);
//	saveResponse(response, "users.json");
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
