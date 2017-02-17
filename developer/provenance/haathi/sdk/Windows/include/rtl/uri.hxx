/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef _RTL_URI_HXX_
#define _RTL_URI_HXX_

#include "rtl/malformeduriexception.hxx"
#include "rtl/uri.h"
#include "rtl/textenc.h"
#include "rtl/ustring.hxx"
#include "sal/types.h"

namespace rtl {

/** A wrapper around the C functions from <rtl/uri.h>.
 */
class Uri
{
public:
    /** A wrapper around rtl_uriEncode() from <rtl/uri.h> (see there), using
        an array of 128 booleans as char class.
     */
    static inline rtl::OUString encode(rtl::OUString const & rText,
                                       sal_Bool const * pCharClass,
                                       rtl_UriEncodeMechanism eMechanism,
                                       rtl_TextEncoding eCharset)
        SAL_THROW(());

    /** A wrapper around rtl_uriEncode() from <rtl/uri.h> (see there), using
        a predefined rtl_UriCharClass enumeration member.
     */
    static inline rtl::OUString encode(rtl::OUString const & rText,
                                       rtl_UriCharClass eCharClass,
                                       rtl_UriEncodeMechanism eMechanism,
                                       rtl_TextEncoding eCharset)
        SAL_THROW(());

    /** A wrapper around rtl_uriDecode() from <rtl/uri.h> (see there).
     */
    static inline rtl::OUString decode(rtl::OUString const & rText,
                                       rtl_UriDecodeMechanism eMechanism,
                                       rtl_TextEncoding eCharset)
        SAL_THROW(());

    /** A wrapper around rtl_uriConvertRelToAbs() from <rtl/uri.h> (see there).

        @exception MalformedUriException
        Thrown in case rtl_uriConvertRelToAbs() signals an exception due to a
        malformed base URI.
     */
    static inline rtl::OUString convertRelToAbs(
        rtl::OUString const & rBaseUriRef, rtl::OUString const & rRelUriRef);

private:
    /** not implemented
        @internal */
    Uri();

    /** not implemented
        @internal */
    Uri(Uri &);

    /** not implemented
        @internal */
    ~Uri();

    /** not implemented
        @internal */
    void operator =(Uri);
};

inline rtl::OUString Uri::encode(rtl::OUString const & rText,
                                 sal_Bool const * pCharClass,
                                 rtl_UriEncodeMechanism eMechanism,
                                 rtl_TextEncoding eCharset)
    SAL_THROW(())
{
    rtl::OUString aResult;
    rtl_uriEncode(const_cast< rtl::OUString & >(rText).pData,
                  pCharClass,
                  eMechanism,
                  eCharset,
                  &aResult.pData);
    return aResult;
}

inline rtl::OUString Uri::encode(rtl::OUString const & rText,
                                 rtl_UriCharClass eCharClass,
                                 rtl_UriEncodeMechanism eMechanism,
                                 rtl_TextEncoding eCharset)
    SAL_THROW(())
{
    rtl::OUString aResult;
    rtl_uriEncode(const_cast< rtl::OUString & >(rText).pData,
                  rtl_getUriCharClass(eCharClass),
                  eMechanism,
                  eCharset,
                  &aResult.pData);
    return aResult;
}

inline rtl::OUString Uri::decode(rtl::OUString const & rText,
                                 rtl_UriDecodeMechanism eMechanism,
                                 rtl_TextEncoding eCharset)
    SAL_THROW(())
{
    rtl::OUString aResult;
    rtl_uriDecode(const_cast< rtl::OUString & >(rText).pData,
                  eMechanism,
                  eCharset,
                  &aResult.pData);
    return aResult;
}

inline rtl::OUString Uri::convertRelToAbs(rtl::OUString const & rBaseUriRef,
                                          rtl::OUString const & rRelUriRef)
{
    rtl::OUString aResult;
    rtl::OUString aException;
    if (!rtl_uriConvertRelToAbs(
            const_cast< rtl::OUString & >(rBaseUriRef).pData,
            const_cast< rtl::OUString & >(rRelUriRef).pData, &aResult.pData,
            &aException.pData))
        throw MalformedUriException(aException);
    return aResult;
}

}

#endif // _RTL_URI_HXX_
