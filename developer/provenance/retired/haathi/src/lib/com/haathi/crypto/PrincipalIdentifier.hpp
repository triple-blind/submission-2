/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/crypto/PrincipalIdentifier.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__CRYPTO__PRINCIPAL_IDENTIFIER_HPP
#	define COM__HAATHI__CRYPTO__PRINCIPAL_IDENTIFIER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "cppJSON.hpp"

#include <string>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace crypto {
/******************************************************************************
Class
******************************************************************************/
class PrincipalIdentifier {
	private:
		PrincipalIdentifier(const PrincipalIdentifier& other);
		PrincipalIdentifier& operator=(const PrincipalIdentifier& other);

		std::string _username;
		cJSON* _identifier;

		void copy(const PrincipalIdentifier& other);

	public:
		PrincipalIdentifier();
		PrincipalIdentifier(std::string username);
		virtual ~PrincipalIdentifier();

		std::string getJSONString();
		void parseJSONString(char* jsonString);
		std::string getUsername();
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
