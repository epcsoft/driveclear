#ifndef __SM3Tool_H__
#define __SM3Tool_H__

#include <stdio.h>
#include <string.h>

/*
* 32-bit integer manipulation macros (big endian)
*/
#ifndef GET_ULONG_BE
#define GET_ULONG_BE(n,b,i)                         \
{                                                   \
	(n) = ( (unsigned int) (b)[(i)    ] << 24 )    \
	| ( (unsigned int) (b)[(i) + 1] << 16 )        \
	| ( (unsigned int) (b)[(i) + 2] <<  8 )        \
	| ( (unsigned int) (b)[(i) + 3]       );       \
}
#endif

#ifndef PUT_ULONG_BE
#define PUT_ULONG_BE(n,b,i)                             \
{                                                       \
	(b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
	(b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
	(b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
	(b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

typedef struct
{
	unsigned int total[2];     /*!< number of bytes processed  */
	unsigned int state[8];     /*!< intermediate digest state  */
	unsigned char buffer[64];   /*!< data block being processed */
	unsigned char ipad[64];     /*!< HMAC: inner padding        */
	unsigned char opad[64];     /*!< HMAC: outer padding        */
}
sm3_context;

static const unsigned char sm3_padding[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

class SM3Tool
{
public:
	SM3Tool();
	~SM3Tool();

	/**
	* \brief          Output = SM3( input buffer )
	*
	* \param input    buffer holding the  data
	* \param ilen     length of the input data
	* \param output   SM3 checksum result
	*/
	void sm3( unsigned char *input, int ilen,unsigned char output[32]);

	//SM2签名用到的预处理
	bool SM3HashWithPreprocess(unsigned char *input,unsigned int input_byte_len,unsigned char *public_key,unsigned int public_key_byte_len,unsigned char *signer_ID,unsigned int signer_ID_byte_len,unsigned char *hash_value,unsigned int *hash_value_byte_len_pointer);

private:
	void sm3_process( sm3_context *ctx, unsigned char data[64] );

	/**
	* \brief          SM3 context setup
	*
	* \param ctx      context to be initialized
	*/
	void sm3_starts( sm3_context *ctx );

	/**
	* \brief          SM3 process buffer
	*
	* \param ctx      SM3 context
	* \param input    buffer holding the  data
	* \param ilen     length of the input data
	*/
	void sm3_update( sm3_context *ctx, unsigned char *input, int ilen );

	/**
	* \brief          SM3 final digest
	*
	* \param ctx      SM3 context
	*/
	void sm3_finish( sm3_context *ctx, unsigned char output[32] );

	/**
	* \brief          Output = SM3( file contents )
	*
	* \param path     input file name
	* \param output   SM3 checksum result
	*
	* \return         0 if successful, 1 if fopen failed,
	*                 or 2 if fread failed
	*/
	int sm3_file( char *path, unsigned char output[32] );

	/**
	* \brief          SM3 HMAC context setup
	*
	* \param ctx      HMAC context to be initialized
	* \param key      HMAC secret key
	* \param keylen   length of the HMAC key
	*/
	void sm3_hmac_starts( sm3_context *ctx, unsigned char *key, int keylen);

	/**
	* \brief          SM3 HMAC process buffer
	*
	* \param ctx      HMAC context
	* \param input    buffer holding the  data
	* \param ilen     length of the input data
	*/
	void sm3_hmac_update( sm3_context *ctx, unsigned char *input, int ilen );

	/**
	* \brief          SM3 HMAC final digest
	*
	* \param ctx      HMAC context
	* \param output   SM3 HMAC checksum result
	*/
	void sm3_hmac_finish( sm3_context *ctx, unsigned char output[32] );

	/**
	* \brief          Output = HMAC-SM3( hmac key, input buffer )
	*
	* \param key      HMAC secret key
	* \param keylen   length of the HMAC key
	* \param input    buffer holding the  data
	* \param ilen     length of the input data
	* \param output   HMAC-SM3 result
	*/
	void sm3_hmac( unsigned char *key, int keylen,unsigned char *input, int ilen,unsigned char output[32] );
};

#endif