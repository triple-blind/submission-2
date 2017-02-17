/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/AuthenticationCredentials.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/utils/AuthenticationCredentials.hpp"
/******************************************************************************
Include others
******************************************************************************/

/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace utils {
/******************************************************************************
Class
******************************************************************************/
AuthenticationCredentials::AuthenticationCredentials() {
}

void AuthenticationCredentials::setEmailAddress(OUString emailAddress) {
	this->emailAddress = emailAddress;
}

OUString AuthenticationCredentials::getEmailAddress() {
	return emailAddress;
}

void AuthenticationCredentials::setPassword(std::string password) {
	this->password = password;
}

std::string AuthenticationCredentials::getPassword() {
	return password;
}

AuthenticationCredentials authenticationCredentials;
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
