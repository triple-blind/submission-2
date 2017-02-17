/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/AuthenticationCredentials.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__UTILS__AUTHENTICATION_CREDENTIALS_HPP
#	define COM__HAATHI__UTILS__AUTHENTICATION_CREDENTIALS_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"

#include <string>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace utils {
/******************************************************************************
Class
******************************************************************************/
class AuthenticationCredentials {
	private:
		OUString emailAddress;
		std::string password;

	public:
		AuthenticationCredentials();
		
		void setEmailAddress(OUString emailAddress);
		OUString getEmailAddress();

		void setPassword(std::string password);
		std::string getPassword();
};

extern AuthenticationCredentials authenticationCredentials;
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
