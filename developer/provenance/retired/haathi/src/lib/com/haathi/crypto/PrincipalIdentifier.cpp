/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/crypto/PrincipalIdentifier.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/crypto/PrincipalIdentifier.hpp"
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
		namespace crypto {
/******************************************************************************
Class
******************************************************************************/
PrincipalIdentifier::PrincipalIdentifier(): _username(), _identifier() {
}

PrincipalIdentifier::PrincipalIdentifier(std::string username) {
	_username = username;
	_identifier = cJSON_CreateObject();
	cJSON_AddStringToObject(_identifier, "username", _username.c_str());
}

PrincipalIdentifier::~PrincipalIdentifier() {
	cJSON_Delete(_identifier);
}

PrincipalIdentifier::PrincipalIdentifier(const PrincipalIdentifier& other) {
	copy(other);
}

PrincipalIdentifier& PrincipalIdentifier::operator=(const PrincipalIdentifier& other) {
	copy(other);
	return *this;
}

void PrincipalIdentifier::copy(const PrincipalIdentifier& other) {
	_username = other._username;
	_identifier = cppJSON_Duplicate(other._identifier);
}

std::string PrincipalIdentifier::getJSONString() {
	char* output;
	output = cJSON_Print(_identifier);
	std::string result(output);
	cppJSON_FreeString(output);
	return result;
}

std::string PrincipalIdentifier::getUsername() {
	return _username;
}

void PrincipalIdentifier::parseJSONString(char* jsonString) {
	cJSON_Delete(_identifier);
	_identifier = cJSON_Parse(jsonString);
	_username = cJSON_GetObjectItem(_identifier, "username")->valuestring;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
