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



#ifndef _RTL_CIPHER_H_
#define _RTL_CIPHER_H_ "$Revision: 1.7 $"

#include <sal/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*========================================================================
 *
 * rtlCipher interface.
 *
 *======================================================================*/
/** Cipher Handle opaque type.
 */
typedef void* rtlCipher;


/** Cipher Algorithm enumeration.
	@see rtl_cipher_create()
 */
enum __rtl_CipherAlgorithm
{
	rtl_Cipher_AlgorithmBF,
	rtl_Cipher_AlgorithmARCFOUR,
	rtl_Cipher_AlgorithmInvalid,
	rtl_Cipher_Algorithm_FORCE_EQUAL_SIZE = SAL_MAX_ENUM
};

/** Cipher Algorithm type.
 */
typedef enum __rtl_CipherAlgorithm rtlCipherAlgorithm;


/** Cipher Mode enumeration.
	@see rtl_cipher_create()
 */
enum __rtl_CipherMode
{
	rtl_Cipher_ModeECB,
	rtl_Cipher_ModeCBC,
	rtl_Cipher_ModeStream,
	rtl_Cipher_ModeInvalid,
	rtl_Cipher_Mode_FORCE_EQUAL_SIZE = SAL_MAX_ENUM
};

/** Cipher Mode type.
 */
typedef enum __rtl_CipherMode rtlCipherMode;


/** Cipher Direction enumeration.
	@see rtl_cipher_init()
 */
enum __rtl_CipherDirection
{
	rtl_Cipher_DirectionBoth,
	rtl_Cipher_DirectionDecode,
	rtl_Cipher_DirectionEncode,
	rtl_Cipher_DirectionInvalid,
	rtl_Cipher_Direction_FORCE_EQUAL_SIZE = SAL_MAX_ENUM
};

/** Cipher Direction type.
 */
typedef enum __rtl_CipherDirection rtlCipherDirection;


/** Error Code enumeration.
 */
enum __rtl_CipherError
{
	rtl_Cipher_E_None,
	rtl_Cipher_E_Argument,
	rtl_Cipher_E_Algorithm,
	rtl_Cipher_E_Direction,
	rtl_Cipher_E_Mode,
	rtl_Cipher_E_BufferSize,
	rtl_Cipher_E_Memory,
	rtl_Cipher_E_Unknown,
	rtl_Cipher_E_FORCE_EQUAL_SIZE = SAL_MAX_ENUM
};

/** Error Code type.
 */
typedef enum __rtl_CipherError rtlCipherError;


/** Create a cipher handle for the given algorithm and mode.
    @see rtlCipherAlgorithm
    @see rtlCipherMode

	@param  Algorithm [in] cipher algorithm.
	@param  Mode      [in] cipher mode.
	@return Cipher handle, or 0 upon failure.
 */
rtlCipher SAL_CALL rtl_cipher_create (
	rtlCipherAlgorithm Algorithm,
	rtlCipherMode      Mode
) SAL_THROW_EXTERN_C();


/** Inititialize a cipher for the given direction.
    @see rtlCipherDirection

	@param  Cipher    [in] cipher handle.
	@param  Direction [in] cipher direction.
	@param  pKeyData  [in] key material buffer.
	@param  nKeyLen   [in] key material length in bytes.
	@param  pArgData  [in] initialization vector buffer.
	@param  nArgLen   [in] initialization vector length in bytes.
	@return rtl_Cipher_E_None upon success.
 */
rtlCipherError SAL_CALL rtl_cipher_init (
	rtlCipher           Cipher,
	rtlCipherDirection  Direction,
	const sal_uInt8    *pKeyData, sal_Size nKeyLen,
	const sal_uInt8    *pArgData, sal_Size nArgLen
) SAL_THROW_EXTERN_C();


/** Encode a buffer under a given cipher algorithm.
    @precond Initialized for a compatible cipher direction.
	@see     rtl_cipher_init()

	@param  Cipher  [in]  cipher handle.
	@param  pData   [in]  plaintext buffer.
	@param  nDatLen [in]  plaintext length in bytes.
	@param  pBuffer [out] ciphertext buffer.
	@param  nBufLen [in]  ciphertext length in bytes.
	@return rtl_Cipher_E_None upon success.
 */
rtlCipherError SAL_CALL rtl_cipher_encode (
	rtlCipher   Cipher,
	const void *pData,   sal_Size nDatLen,
	sal_uInt8  *pBuffer, sal_Size nBufLen
) SAL_THROW_EXTERN_C();


/** Decode a buffer under a given cipher algorithm.
    @precond Initialized for a compatible cipher direction.
	@see     rtl_cipher_init()

	@param  Cipher  [in]  cipher handle.
	@param  pData   [in]  ciphertext buffer.
	@param  nDatLen [in]  ciphertext length in bytes.
	@param  pBuffer [out] plaintext buffer.
	@param  nBufLen [in]  plaintext length in bytes.
	@return rtl_Cipher_E_None upon success.
 */
rtlCipherError SAL_CALL rtl_cipher_decode (
	rtlCipher   Cipher,
	const void *pData,   sal_Size nDatLen,
	sal_uInt8  *pBuffer, sal_Size nBufLen
) SAL_THROW_EXTERN_C();


/** Destroy a cipher handle.
	@param  Cipher [in] cipher handle to be destroyed.
	@return None. Cipher handle destroyed and invalid.
 */
void SAL_CALL rtl_cipher_destroy (
	rtlCipher Cipher
) SAL_THROW_EXTERN_C();


/*========================================================================
 *
 * rtl_cipherBF (Blowfish) interface.
 *
 *======================================================================*/
/** Create a Blowfish cipher handle for the given mode.
    @descr The Blowfish block cipher algorithm is specified in
	Bruce Schneier: Applied Cryptography, 2nd edition, ch. 14.3

	@see rtl_cipher_create()
 */
rtlCipher SAL_CALL rtl_cipher_createBF (
	rtlCipherMode Mode
) SAL_THROW_EXTERN_C();


/** Inititialize a Blowfish cipher for the given direction.
	@see rtl_cipher_init()
 */
rtlCipherError SAL_CALL rtl_cipher_initBF (
	rtlCipher          Cipher,
	rtlCipherDirection Direction,
	const sal_uInt8 *pKeyData, sal_Size nKeyLen,
	const sal_uInt8 *pArgData, sal_Size nArgLen
) SAL_THROW_EXTERN_C();


/** Encode a buffer under the Blowfish cipher algorithm.
	@see rtl_cipher_encode()
 */
rtlCipherError SAL_CALL rtl_cipher_encodeBF (
	rtlCipher   Cipher,
	const void *pData,   sal_Size nDatLen,
	sal_uInt8  *pBuffer, sal_Size nBufLen
) SAL_THROW_EXTERN_C();


/** Decode a buffer under the Blowfish cipher algorithm.
	@see rtl_cipher_decode()
 */
rtlCipherError SAL_CALL rtl_cipher_decodeBF (
	rtlCipher   Cipher,
	const void *pData,   sal_Size nDatLen,
	sal_uInt8  *pBuffer, sal_Size nBufLen
) SAL_THROW_EXTERN_C();


/** Destroy a Blowfish cipher handle.
	@see rtl_cipher_destroy()
 */
void SAL_CALL rtl_cipher_destroyBF (
	rtlCipher Cipher
) SAL_THROW_EXTERN_C();


/*========================================================================
 *
 * rtl_cipherARCFOUR (RC4) interface.
 *
 *======================================================================*/
/** Create a RC4 cipher handle for the given mode.
    @descr The RC4 symmetric stream cipher algorithm is specified in
	Bruce Schneier: Applied Cryptography, 2nd edition, ch. 17.1

	@see rtl_cipher_create()

	@param  Mode [in] cipher mode. Must be rtl_Cipher_ModeStream.
	@return Cipher handle, or 0 upon failure.
 */
rtlCipher SAL_CALL rtl_cipher_createARCFOUR (
	rtlCipherMode Mode
) SAL_THROW_EXTERN_C();


/** Inititialize a RC4 cipher for the given direction.
	@see rtl_cipher_init()
 */
rtlCipherError SAL_CALL rtl_cipher_initARCFOUR (
	rtlCipher          Cipher,
	rtlCipherDirection Direction,
	const sal_uInt8 *pKeyData, sal_Size nKeyLen,
	const sal_uInt8 *pArgData, sal_Size nArgLen
) SAL_THROW_EXTERN_C();


/** Encode a buffer under the RC4 cipher algorithm.
	@see rtl_cipher_encode()
 */
rtlCipherError SAL_CALL rtl_cipher_encodeARCFOUR (
	rtlCipher   Cipher,
	const void *pData,   sal_Size nDatLen,
	sal_uInt8  *pBuffer, sal_Size nBufLen
) SAL_THROW_EXTERN_C();


/** Decode a buffer under the RC4 cipher algorithm.
	@see rtl_cipher_decode()
 */
rtlCipherError SAL_CALL rtl_cipher_decodeARCFOUR (
	rtlCipher   Cipher,
	const void *pData,   sal_Size nDatLen,
	sal_uInt8  *pBuffer, sal_Size nBufLen
) SAL_THROW_EXTERN_C();


/** Destroy a RC4 cipher handle.
	@see rtl_cipher_destroy()
 */
void SAL_CALL rtl_cipher_destroyARCFOUR (
	rtlCipher Cipher
) SAL_THROW_EXTERN_C();


/*========================================================================
 *
 * The End.
 *
 *======================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* !_RTL_CIPHER_H_ */

