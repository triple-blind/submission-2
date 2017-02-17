/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/StringStream.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/utils/StringStream.hpp"
/******************************************************************************
Include others
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::sun::star::io::BufferSizeExceededException;
using com::sun::star::io::IOException;
using com::sun::star::io::NotConnectedException;
using com::sun::star::io::XSeekable;
using com::sun::star::io::XInputStream;
using com::sun::star::io::XOutputStream;
using com::sun::star::uno::Sequence;
using com::sun::star::uno::RuntimeException;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace utils {
/******************************************************************************
Class
******************************************************************************/
StringStream::StringStream() {
}

StringStream::StringStream(const std::string& str) {
	stream.str(str);
}

StringStream::~StringStream() {
}

sal_Int32 SAL_CALL StringStream::readBytes(
		Sequence<sal_Int8>& aData, sal_Int32 nBytesToRead)
		throw (NotConnectedException, BufferSizeExceededException,
		IOException, RuntimeException) {
	osl::MutexGuard guard(lock);
	if (!stream)
		throw IOException();
	aData.realloc(nBytesToRead);

	char* buffer = new char[nBytesToRead];
	stream.read(buffer, nBytesToRead);
	sal_Int32 bytesActuallyRead = stream.gcount();
	for (sal_Int32 i = 0; i < bytesActuallyRead; ++i)
		aData[i] = buffer[i];
	if (bytesActuallyRead != nBytesToRead)
		aData.realloc(bytesActuallyRead);
	delete[] buffer;
	return bytesActuallyRead;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
