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



#ifndef _RTL_USTRBUF_HXX_
#define _RTL_USTRBUF_HXX_

#include <osl/diagnose.h>
#include <rtl/ustrbuf.h>
#ifndef _RTL_USTRING_HXX
#include <rtl/ustring.hxx>
#endif

#ifdef __cplusplus

namespace rtl
{

/** @HTML
    A string buffer implements a mutable sequence of characters.
    <p>
    String buffers are safe for use by multiple threads. The methods
    are synchronized where necessary so that all the operations on any
    particular instance behave as if they occur in some serial order.
    <p>
    String buffers are used by the compiler to implement the binary
    string concatenation operator <code>+</code>. For example, the code:
    <p><blockquote><pre>
        x = "a" + 4 + "c"
    </pre></blockquote><p>
    is compiled to the equivalent of:
    <p><blockquote><pre>
        x = new OUStringBuffer().append("a").append(4).append("c")
                              .toString()
    </pre></blockquote><p>
    The principal operations on a <code>OUStringBuffer</code> are the
    <code>append</code> and <code>insert</code> methods, which are
    overloaded so as to accept data of any type. Each effectively
    converts a given datum to a string and then appends or inserts the
    characters of that string to the string buffer. The
    <code>append</code> method always adds these characters at the end
    of the buffer; the <code>insert</code> method adds the characters at
    a specified point.
    <p>
    For example, if <code>z</code> refers to a string buffer object
    whose current contents are "<code>start</code>", then
    the method call <code>z.append("le")</code> would cause the string
    buffer to contain "<code>startle</code>", whereas
    <code>z.insert(4, "le")</code> would alter the string buffer to
    contain "<code>starlet</code>".
    <p>
    Every string buffer has a capacity. As long as the length of the
    character sequence contained in the string buffer does not exceed
    the capacity, it is not necessary to allocate a new internal
    buffer array. If the internal buffer overflows, it is
    automatically made larger.
 */
class OUStringBuffer
{
public:
    /**
        Constructs a string buffer with no characters in it and an
        initial capacity of 16 characters.
     */
    OUStringBuffer()
        : pData(NULL)
        , nCapacity( 16 )
    {
        rtl_uString_new_WithLength( &pData, nCapacity );
    }

    /**
        Allocates a new string buffer that contains the same sequence of
        characters as the string buffer argument.

        @param   value   a <code>OStringBuffer</code>.
     */
    OUStringBuffer( const OUStringBuffer & value )
        : pData(NULL)
        , nCapacity( value.nCapacity )
    {
        rtl_uStringbuffer_newFromStringBuffer( &pData, value.nCapacity, value.pData );
    }

    /**
        Constructs a string buffer with no characters in it and an
        initial capacity specified by the <code>length</code> argument.

        @param      length   the initial capacity.
     */
    OUStringBuffer(sal_Int32 length)
        : pData(NULL)
        , nCapacity( length )
    {
        rtl_uString_new_WithLength( &pData, length );
    }

    /**
        Constructs a string buffer so that it represents the same
        sequence of characters as the string argument.

        The initial
        capacity of the string buffer is <code>16</code> plus the length
        of the string argument.

        @param   str   the initial contents of the buffer.
     */
    OUStringBuffer(OUString value)
        : pData(NULL)
        , nCapacity( value.getLength() + 16 )
    {
        rtl_uStringbuffer_newFromStr_WithLength( &pData, value.getStr(), value.getLength() );
    }

    /** Assign to this a copy of value.
     */
    OUStringBuffer& operator = ( const OUStringBuffer& value )
    {
        if (this != &value)
        {
            rtl_uStringbuffer_newFromStringBuffer(&pData,
                                                  value.nCapacity,
                                                  value.pData);
            nCapacity = value.nCapacity;
        }
        return *this;
    }

    /**
        Release the string data.
     */
    ~OUStringBuffer()
    {
        rtl_uString_release( pData );
    }

    /**
        Fill the string data in the new string and clear the buffer.

        This method is more efficient than the contructor of the string. It does
        not copy the buffer.

        @return the string previously contained in the buffer.
     */
    OUString makeStringAndClear()
    {
        OUString aRet( pData );
        rtl_uString_new(&pData);
        nCapacity = 0;
        return aRet;
    }

    /**
        Returns the length (character count) of this string buffer.

        @return  the number of characters in this string buffer.
     */
    sal_Int32 getLength() const
    {
        return pData->length;
    }

    /**
        Returns the current capacity of the String buffer.

        The capacity
        is the amount of storage available for newly inserted
        characters. The real buffer size is 2 bytes longer, because
        all strings are 0 terminated.

        @return  the current capacity of this string buffer.
     */
    sal_Int32 getCapacity() const
    {
        return nCapacity;
    }

    /**
        Ensures that the capacity of the buffer is at least equal to the
        specified minimum.

        The new capacity will be at least as large as the maximum of the current
        length (so that no contents of the buffer is destroyed) and the given
        minimumCapacity.  If the given minimumCapacity is negative, nothing is
        changed.

        @param   minimumCapacity   the minimum desired capacity.
     */
    void ensureCapacity(sal_Int32 minimumCapacity)
    {
        rtl_uStringbuffer_ensureCapacity( &pData, &nCapacity, minimumCapacity );
    }

    /**
        Sets the length of this String buffer.

        If the <code>newLength</code> argument is less than the current
        length of the string buffer, the string buffer is truncated to
        contain exactly the number of characters given by the
        <code>newLength</code> argument.
        <p>
        If the <code>newLength</code> argument is greater than or equal
        to the current length, sufficient null characters
        (<code>'&#92;u0000'</code>) are appended to the string buffer so that
        length becomes the <code>newLength</code> argument.
        <p>
        The <code>newLength</code> argument must be greater than or equal
        to <code>0</code>.

        @param      newLength   the new length of the buffer.
     */
    void setLength(sal_Int32 newLength)
    {
        OSL_ASSERT(newLength >= 0);
        // Avoid modifications if pData points to const empty string:
        if( newLength != pData->length )
        {
            if( newLength > nCapacity )
                rtl_uStringbuffer_ensureCapacity(&pData, &nCapacity, newLength);
            else
                pData->buffer[newLength] = 0;
            pData->length = newLength;
        }
    }

    /**
        Returns the character at a specific index in this string buffer.

        The first character of a string buffer is at index
        <code>0</code>, the next at index <code>1</code>, and so on, for
        array indexing.
        <p>
        The index argument must be greater than or equal to
        <code>0</code>, and less than the length of this string buffer.

        @param      index   the index of the desired character.
        @return     the character at the specified index of this string buffer.
     */
    sal_Unicode charAt( sal_Int32 index ) const
    {
        OSL_ASSERT(index >= 0 && index < pData->length);
        return pData->buffer[ index ];
    }

    /**
        Return a null terminated unicode character array.
     */
    operator        const sal_Unicode *() const { return pData->buffer; }

    /**
        Return a null terminated unicode character array.
     */
    const sal_Unicode*  getStr() const { return pData->buffer; }


    /**
        The character at the specified index of this string buffer is set
        to <code>ch</code>.

        The index argument must be greater than or equal to
        <code>0</code>, and less than the length of this string buffer.

        @param      index   the index of the character to modify.
        @param      ch      the new character.
     */
    OUStringBuffer & setCharAt(sal_Int32 index, sal_Unicode ch)
    {
        OSL_ASSERT(index >= 0 && index < pData->length);
        pData->buffer[ index ] = ch;
        return *this;
    }

    /**
        Appends the string to this string buffer.

        The characters of the <code>String</code> argument are appended, in
        order, to the contents of this string buffer, increasing the
        length of this string buffer by the length of the argument.

        @param   str   a string.
        @return  this string buffer.
     */
    OUStringBuffer & append(const OUString &str)
    {
        return append( str.getStr(), str.getLength() );
    }

    /**
        Appends the string representation of the <code>char</code> array
        argument to this string buffer.

        The characters of the array argument are appended, in order, to
        the contents of this string buffer. The length of this string
        buffer increases by the length of the argument.

        @param   str   the characters to be appended.
        @return  this string buffer.
     */
    OUStringBuffer & append( const sal_Unicode * str )
    {
        return append( str, rtl_ustr_getLength( str ) );
    }

    /**
        Appends the string representation of the <code>char</code> array
        argument to this string buffer.

        Characters of the character array <code>str</code> are appended,
        in order, to the contents of this string buffer. The length of this
        string buffer increases by the value of <code>len</code>.

        @param str the characters to be appended; must be non-null, and must
        point to at least len characters
        @param len the number of characters to append; must be non-negative
        @return  this string buffer.
     */
    OUStringBuffer & append( const sal_Unicode * str, sal_Int32 len)
    {
        // insert behind the last character
        rtl_uStringbuffer_insert( &pData, &nCapacity, getLength(), str, len );
        return *this;
    }

    /**
        Appends a 8-Bit ASCII character string to this string buffer.
 
       Since this method is optimized for performance. the ASCII
        character values are not converted in any way. The caller
        has to make sure that all ASCII characters are in the
        allowed range between 0 and 127. The ASCII string must be
        NULL-terminated.
        <p>
        The characters of the array argument are appended, in order, to
        the contents of this string buffer. The length of this string
        buffer increases by the length of the argument.

        @param   str   the 8-Bit ASCII characters to be appended.
        @return  this string buffer.
     */
    OUStringBuffer & appendAscii( const sal_Char * str )
    {
        return appendAscii( str, rtl_str_getLength( str ) );
    }

    /**
        Appends a 8-Bit ASCII character string to this string buffer.

        Since this method is optimized for performance. the ASCII
        character values are not converted in any way. The caller
        has to make sure that all ASCII characters are in the
        allowed range between 0 and 127. The ASCII string must be
        NULL-terminated.
        <p>
        Characters of the character array <code>str</code> are appended,
        in order, to the contents of this string buffer. The length of this
        string buffer increases by the value of <code>len</code>.

        @param str the 8-Bit ASCII characters to be appended; must be non-null,
        and must point to at least len characters
        @param len the number of characters to append; must be non-negative
        @return  this string buffer.
     */
    OUStringBuffer & appendAscii( const sal_Char * str, sal_Int32 len)
    {
        rtl_uStringbuffer_insert_ascii( &pData, &nCapacity, getLength(), str, len );
        return *this;
    }


    /**
        Appends the string representation of the <code>sal_Bool</code>
        argument to the string buffer.

        The argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then appended to this string buffer.

        @param   b   a <code>sal_Bool</code>.
        @return  this string buffer.
     */
    OUStringBuffer & append(sal_Bool b)
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFBOOLEAN];
        return append( sz, rtl_ustr_valueOfBoolean( sz, b ) );
    }

    /**
        Appends the string representation of the <code>char</code>
        argument to this string buffer.

        The argument is appended to the contents of this string buffer.
        The length of this string buffer increases by <code>1</code>.

        @param   ch   a <code>char</code>.
        @return  this string buffer.
     */
    OUStringBuffer & append(sal_Unicode c)
    {
        return append( &c, 1 );
    }

    /**
        Appends the string representation of the <code>sal_Int32</code>
        argument to this string buffer.

        The argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then appended to this string buffer.

        @param   i   an <code>sal_Int32</code>.
        @return  this string buffer.
     */
    OUStringBuffer & append(sal_Int32 i, sal_Int16 radix = 10 )
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFINT32];
        return append( sz, rtl_ustr_valueOfInt32( sz, i, radix ) );
    }

    /**
        Appends the string representation of the <code>long</code>
        argument to this string buffer.

        The argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then appended to this string buffer.

        @param   l   a <code>long</code>.
        @return  this string buffer.
     */
    OUStringBuffer & append(sal_Int64 l, sal_Int16 radix = 10 )
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFINT64];
        return append( sz, rtl_ustr_valueOfInt64( sz, l, radix ) );
    }

    /**
        Appends the string representation of the <code>float</code>
        argument to this string buffer.

        The argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then appended to this string buffer.

        @param   f   a <code>float</code>.
        @return  this string buffer.
     */
    OUStringBuffer & append(float f)
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFFLOAT];
        return append( sz, rtl_ustr_valueOfFloat( sz, f ) );
    }

    /**
        Appends the string representation of the <code>double</code>
        argument to this string buffer.

        The argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then appended to this string buffer.

        @param   d   a <code>double</code>.
        @return  this string buffer.
     */
    OUStringBuffer & append(double d)
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFDOUBLE];
        return append( sz, rtl_ustr_valueOfDouble( sz, d ) );
    }

    /**
       Appends a single UTF-32 character to this string buffer.

       <p>The single UTF-32 character will be represented within the string
       buffer as either one or two UTF-16 code units.</p>

       @param c a well-formed UTF-32 code unit (that is, a value in the range
       <code>0</code>&ndash;<code>0x10FFFF</code>, but excluding
       <code>0xD800</code>&ndash;<code>0xDFFF</code>)

       @return
       this string buffer
     */
    OUStringBuffer & appendUtf32(sal_uInt32 c) {
        return insertUtf32(getLength(), c);
    }

    /**
        Inserts the string into this string buffer.

        The characters of the <code>String</code> argument are inserted, in
        order, into this string buffer at the indicated offset. The length
        of this string buffer is increased by the length of the argument.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      str      a string.
        @return     this string buffer.
     */
    OUStringBuffer & insert(sal_Int32 offset, const OUString & str)
    {
        return insert( offset, str.getStr(), str.getLength() );
    }

    /**
        Inserts the string representation of the <code>char</code> array
        argument into this string buffer.

        The characters of the array argument are inserted into the
        contents of this string buffer at the position indicated by
        <code>offset</code>. The length of this string buffer increases by
        the length of the argument.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      ch       a character array.
        @return     this string buffer.
     */
    OUStringBuffer & insert( sal_Int32 offset, const sal_Unicode * str )
    {
        return insert( offset, str, rtl_ustr_getLength( str ) );
    }

    /**
        Inserts the string representation of the <code>char</code> array
        argument into this string buffer.

        The characters of the array argument are inserted into the
        contents of this string buffer at the position indicated by
        <code>offset</code>. The length of this string buffer increases by
        the length of the argument.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      ch       a character array.
        @param      len     the number of characters to append.
        @return     this string buffer.
     */
    OUStringBuffer & insert( sal_Int32 offset, const sal_Unicode * str, sal_Int32 len)
    {
        // insert behind the last character
        rtl_uStringbuffer_insert( &pData, &nCapacity, offset, str, len );
        return *this;
    }

    /**
        Inserts the string representation of the <code>sal_Bool</code>
        argument into this string buffer.

        The second argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then inserted into this string buffer at the indicated
        offset.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      b        a <code>sal_Bool</code>.
        @return     this string buffer.
     */
    OUStringBuffer & insert(sal_Int32 offset, sal_Bool b)
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFBOOLEAN];
        return insert( offset, sz, rtl_ustr_valueOfBoolean( sz, b ) );
    }

    /**
        Inserts the string representation of the <code>char</code>
        argument into this string buffer.

        The second argument is inserted into the contents of this string
        buffer at the position indicated by <code>offset</code>. The length
        of this string buffer increases by one.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      ch       a <code>char</code>.
        @return     this string buffer.
     */
    OUStringBuffer & insert(sal_Int32 offset, sal_Unicode c)
    {
        return insert( offset, &c, 1 );
    }

    /**
        Inserts the string representation of the second <code>sal_Int32</code>
        argument into this string buffer.

        The second argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then inserted into this string buffer at the indicated
        offset.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      b        an <code>sal_Int32</code>.
        @return     this string buffer.
        @exception  StringIndexOutOfBoundsException  if the offset is invalid.
     */
    OUStringBuffer & insert(sal_Int32 offset, sal_Int32 i, sal_Int16 radix = 10 )
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFINT32];
        return insert( offset, sz, rtl_ustr_valueOfInt32( sz, i, radix ) );
    }

    /**
        Inserts the string representation of the <code>long</code>
        argument into this string buffer.

        The second argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then inserted into this string buffer at the indicated
        offset.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      b        a <code>long</code>.
        @return     this string buffer.
        @exception  StringIndexOutOfBoundsException  if the offset is invalid.
     */
    OUStringBuffer & insert(sal_Int32 offset, sal_Int64 l, sal_Int16 radix = 10 )
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFINT64];
        return insert( offset, sz, rtl_ustr_valueOfInt64( sz, l, radix ) );
    }

    /**
        Inserts the string representation of the <code>float</code>
        argument into this string buffer.

        The second argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then inserted into this string buffer at the indicated
        offset.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      b        a <code>float</code>.
        @return     this string buffer.
        @exception  StringIndexOutOfBoundsException  if the offset is invalid.
     */
    OUStringBuffer insert(sal_Int32 offset, float f)
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFFLOAT];
        return insert( offset, sz, rtl_ustr_valueOfFloat( sz, f ) );
    }

    /**
        Inserts the string representation of the <code>double</code>
        argument into this string buffer.

        The second argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then inserted into this string buffer at the indicated
        offset.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      b        a <code>double</code>.
        @return     this string buffer.
        @exception  StringIndexOutOfBoundsException  if the offset is invalid.
     */
    OUStringBuffer & insert(sal_Int32 offset, double d)
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFDOUBLE];
        return insert( offset, sz, rtl_ustr_valueOfDouble( sz, d ) );
    }

    /**
       Inserts a single UTF-32 character into this string buffer.

       <p>The single UTF-32 character will be represented within the string
       buffer as either one or two UTF-16 code units.</p>

       @param offset the offset into this string buffer (from zero to the length
       of this string buffer, inclusive)

       @param c a well-formed UTF-32 code unit (that is, a value in the range
       <code>0</code>&ndash;<code>0x10FFFF</code>, but excluding
       <code>0xD800</code>&ndash;<code>0xDFFF</code>)

       @return this string buffer
     */
    OUStringBuffer & insertUtf32(sal_Int32 offset, sal_uInt32 c) {
        rtl_uStringbuffer_insertUtf32(&pData, &nCapacity, offset, c);
        return *this;
    }

    /** Allows access to the internal data of this OUStringBuffer, for effective
        manipulation.

        This method should be used with care.  After you have called this
        method, you may use the returned pInternalData or pInternalCapacity only
        as long as you make no other method call on this OUStringBuffer.

        @param pInternalData
        This output parameter receives a pointer to the internal data
        (rtl_uString pointer).  pInternalData itself must not be null.

        @param pInternalCapacity
        This output parameter receives a pointer to the internal capacity.
        pInternalCapacity itself must not be null.
     */
    inline void accessInternals(rtl_uString *** pInternalData,
                                sal_Int32 ** pInternalCapacity)
    {
        *pInternalData = &pData;
        *pInternalCapacity = &nCapacity;
    }

private:
    /**
        A pointer to the data structur which contains the data.
     */
    rtl_uString * pData;

    /**
        The len of the pData->buffer.
     */
    sal_Int32       nCapacity;
};

}

#endif  /* __cplusplus */
#endif  /* _RTL_USTRBUF_HXX_ */
