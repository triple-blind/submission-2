//
// network.cxx
//
// Author: Aaron Gibson
//
// This is a convenience file to compile everything pertaining to networking.
// I suppose instead of including the source files here might not be the best idea,
// but the alternative involves editing the Makefile everytime I decide to add
// something. 
// Yes, I'm lazy.

#include "com/haathi/network/Server.cpp"
#include "com/haathi/network/TrustedServer.cpp"
#include "com/haathi/network/CloudServer.cpp"
#include "com/haathi/network/AuthenticationResponse.cxx"

/*
 std::ostream& operator<<(std::ostream & stream, const com::haathi::network::NetworkInfo& info)
 {
 stream << "NetworkInfo -- host: " << info.host;
 stream << "\tcaCertificatePath: " << info.caCertificatePath;
 return stream;
 }

 std::ostream& operator<<(std::ostream & stream, const com::haathi::network::UserInfo& info)
 {
 stream << "UserInfo -- username: " << info.username << " ID: " << info.userID;
 return stream;
 }
 
 */
