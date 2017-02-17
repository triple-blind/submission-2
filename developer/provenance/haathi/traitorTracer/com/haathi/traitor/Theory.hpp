/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/haathi/traitorTracer/com/haathi/traitor/Theory.hpp#3 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__TRAITOR__THEORY_HPP
#	define COM__HAATHI__TRAITOR__THEORY_HPP
/******************************************************************************
Include
******************************************************************************/
#include <string>
#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace traitor {
/******************************************************************************
Class
******************************************************************************/
struct Theory {
	public:
		std::string userId;
		std::string userPrivateKey;
		std::string documentId;
		std::string versionNo;
		std::string versionId;

		Theory(std::string userId, std::string userPrivateKey, std::string documentId, std::string versionNo, std::string versionId):
				userId(userId), userPrivateKey(userPrivateKey),
				documentId(documentId), versionNo(versionNo), versionId(versionId) {
		};

		~Theory() {
		};
};

typedef std::vector<Theory*> Theories;
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
