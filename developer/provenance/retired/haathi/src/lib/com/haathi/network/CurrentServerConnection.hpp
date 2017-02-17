/*!
 
 @header CurrentServerConnection.hpp
 
 @author Aaron Gibsonm
 @date February 22, 2012
 
 This file implements a "wrapper" around a dialog that acquires the information for connection to
 the trusted server. This wrapper will also store public information about the server, notably the
 IP Address, username, and the port number. The password must be handled with more caution, but
 might be considered later.
 */

#ifndef CURRENT_SERVER_CONNECTION_HPP
#define CURRENT_SERVER_CONNECTION_HPP

#include "com/haathi/network/NetworkInfo.hpp"

namespace com {
namespace haathi {
namespace menu {

class CurrentServerConnection {
private:
	// Don't implement these... We should have a singleton anyway!
	CurrentServerConnection(const CurrentServerConnection& other);
	CurrentServerConnection& operator=(const CurrentServerConnection& other);

	// Instance Variables
	com::haathi::network::NetworkInfo server_info;
	sal_Bool server_set;
public:
	// Constructors and such
	CurrentServerConnection(sal_Bool loadWithDialog = sal_False );
	virtual ~CurrentServerConnection();

	// Member functions		
	com::haathi::network::NetworkInfo getCurrentServerInfo() const;

	void changeCurrentServer(std::string _serverIP, std::string _username,
			int _portNumber);

	sal_Bool changeCurrentServerWithDialog();
};

}
;
}
;
}
;
// namespace com::haathi::menu

#endif

