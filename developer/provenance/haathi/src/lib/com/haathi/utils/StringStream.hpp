/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/StringStream.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__UTILS__STRING_STREAM_HPP
#	define COM__HAATHI__UTILS__STRING_STREAM_HPP
/******************************************************************************
Include
******************************************************************************/
#include "sdkBeg.hpp"
#include <com/sun/star/io/XSeekable.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <cppuhelper/compbase3.hxx>
#include "sdkEnd.hpp"

#include <sstream>
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
/*!
 @class StringStream
 
 @author Aaron Gibson -- based on online help.
 
 This class implements an OpenOffice stream with the standard library's stringstream class.
 Certain interfaces must be implemented for OpenOffice to handle the streams. This class
 handles these streams and can be used as a template for other types of stream classes that
 you may want to implement.
 
 I don't really explain this in detail because the XInputStream, XSeekable, and XOutputStream
 speak for themselves... sort of. Ask any questions if you need help, though I doubt that 
 you will have to use this.
 */
class StringStream: public cppu::WeakImplHelper3<com::sun::star::io::XInputStream, com::sun::star::io::XSeekable,
		com::sun::star::io::XOutputStream> {
	private:
		std::stringstream stream;
		osl::Mutex lock;

		// Don't implement these.
		StringStream& operator=(const StringStream& other);
		StringStream(const StringStream& other);

	public:
		StringStream();
		StringStream(const std::string& str);
		virtual ~StringStream();

		std::string str() const {
			return stream.str();
		}

		virtual sal_Int32 SAL_CALL readBytes(
				com::sun::star::uno::Sequence<sal_Int8>& aData,
				sal_Int32 nBytesToRead)
				throw(com::sun::star::io::NotConnectedException,
				com::sun::star::io::BufferSizeExceededException,
				com::sun::star::io::IOException,
				com::sun::star::uno::RuntimeException);

		virtual sal_Int32 SAL_CALL readSomeBytes(
				com::sun::star::uno::Sequence< sal_Int8 >& aData,
				sal_Int32 nMaxBytesToRead )
				throw(com::sun::star::io::NotConnectedException,
				com::sun::star::io::BufferSizeExceededException,
				com::sun::star::io::IOException,
				com::sun::star::uno::RuntimeException) {
			osl::MutexGuard guard(lock);
			return readBytes(aData, nMaxBytesToRead);
		}

		virtual void SAL_CALL skipBytes(
				sal_Int32 nBytesToSkip )
				throw( com::sun::star::io::NotConnectedException,
				com::sun::star::io::BufferSizeExceededException,
				com::sun::star::io::IOException,
				com::sun::star::uno::RuntimeException) {
			osl::MutexGuard guard(lock);
			stream.seekg(nBytesToSkip, std::ios_base::cur);
		}

		virtual sal_Int32 SAL_CALL available()
				throw( com::sun::star::io::NotConnectedException,
				com::sun::star::io::IOException,
				com::sun::star::uno::RuntimeException ) {
			osl::MutexGuard guard(lock);
			return 0;
		}

		virtual void SAL_CALL closeInput()
				throw( com::sun::star::io::NotConnectedException,
				com::sun::star::io::IOException,
				com::sun::star::uno::RuntimeException ) {
			// Don't do anything right now.
		}

		virtual void SAL_CALL seek(sal_Int64 location )
				throw( com::sun::star::lang::IllegalArgumentException,
				com::sun::star::io::IOException,
				com::sun::star::uno::RuntimeException ) {
			osl::MutexGuard guard(lock);
			// kwa: todo: This should instead be repeated until the sal_Int64
			// value has been reached
			stream.seekg((std::streamoff) location);
		}

		virtual sal_Int64 SAL_CALL getPosition()
				throw( com::sun::star::io::IOException,
				com::sun::star::uno::RuntimeException ) {
			osl::MutexGuard guard(lock);
			return sal_Int64(stream.tellg());
		}

		virtual sal_Int64 SAL_CALL getLength()
			throw( com::sun::star::io::IOException,
				com::sun::star::uno::RuntimeException ) {
			osl::MutexGuard guard(lock);
			return sal_Int64(stream.str().size());
		}

		virtual void SAL_CALL writeBytes(const com::sun::star::uno::Sequence< sal_Int8 > & bytes)
				throw (com::sun::star::io::NotConnectedException,
				com::sun::star::io::IOException,
				com::sun::star::uno::RuntimeException ) {
			osl::MutexGuard guard(lock);
			for(sal_Int32 i = 0; i < bytes.getLength(); ++i)
				stream.put(bytes[i]);
		}

		virtual void SAL_CALL flush()
				throw (com::sun::star::io::NotConnectedException,
				com::sun::star::io::IOException,
				com::sun::star::uno::RuntimeException ) {
			// Nothing to do...?
		}

		virtual void SAL_CALL closeOutput()
				throw (com::sun::star::io::NotConnectedException,
				com::sun::star::io::IOException,
				com::sun::star::uno::RuntimeException ) {
			// Nothing to do...?
		}
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
