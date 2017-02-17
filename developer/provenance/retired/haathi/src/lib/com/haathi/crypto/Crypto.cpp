/*!
 This file will compile the source of the cryptography stuff.
 
 @author Aaron Gibson
 */

#ifndef __CRYPTO_CXX__
#define __CRYPTO_CXX__

// All source that must be built
//#include "crypto/provenance_crypto.cpp"

#include "com/haathi/crypto/CryptoStructures.hpp"

#include "com/haathi/crypto/PrincipalIdentifier.cpp"
#include "com/haathi/crypto/ProvenanceEntry.cpp"

#include "com/haathi/crypto/ProvenanceHelper.cpp"
#include "com/haathi/crypto/KeyingMaterial.cpp"
#include "com/haathi/crypto/ProvenanceChainEvolutionVerifier.cpp"
#include "com/haathi/crypto/ProvenanceChainIntegrityVerifier.cpp"

/*
 // This is for debugging. Apparently, this has to be defined in some source file.
 std::ostream& operator<<(std::ostream& stream, const com::haathi::crypto::CryptoInfo& info)
 {
 stream << "PrivateKey: " << info.privateKey;
 stream << "PublicKey: " << info.publicKey;
 stream << "sessionKey: " << info.sessionKey; 
 
 return stream;
 }
 */
#endif
