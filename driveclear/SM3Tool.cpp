#include "stdafx.h"
#include "SM3Tool.h"

SM3Tool::SM3Tool()
{
}

SM3Tool::~SM3Tool()
{
}

void SM3Tool::sm3_process( sm3_context *ctx, unsigned char data[64] )
{
	unsigned int SS1, SS2, TT1, TT2, W[68],W1[64];
	unsigned int A, B, C, D, E, F, G, H;
	unsigned int T[64];
	unsigned int Temp1,Temp2,Temp3,Temp4,Temp5;
	int j;

	for(j = 0; j < 16; j++)
		T[j] = 0x79CC4519;
	for(j =16; j < 64; j++)
		T[j] = 0x7A879D8A;

	GET_ULONG_BE( W[ 0], data,  0 );
	GET_ULONG_BE( W[ 1], data,  4 );
	GET_ULONG_BE( W[ 2], data,  8 );
	GET_ULONG_BE( W[ 3], data, 12 );
	GET_ULONG_BE( W[ 4], data, 16 );
	GET_ULONG_BE( W[ 5], data, 20 );
	GET_ULONG_BE( W[ 6], data, 24 );
	GET_ULONG_BE( W[ 7], data, 28 );
	GET_ULONG_BE( W[ 8], data, 32 );
	GET_ULONG_BE( W[ 9], data, 36 );
	GET_ULONG_BE( W[10], data, 40 );
	GET_ULONG_BE( W[11], data, 44 );
	GET_ULONG_BE( W[12], data, 48 );
	GET_ULONG_BE( W[13], data, 52 );
	GET_ULONG_BE( W[14], data, 56 );
	GET_ULONG_BE( W[15], data, 60 );

#define FF0(x,y,z) ( (x) ^ (y) ^ (z))
#define FF1(x,y,z) (((x) & (y)) | ( (x) & (z)) | ( (y) & (z)))

#define GG0(x,y,z) ( (x) ^ (y) ^ (z))
#define GG1(x,y,z) (((x) & (y)) | ( (~(x)) & (z)) )


#define  SHL(x,n) (((x) & 0xFFFFFFFF) << n)
#define ROTL(x,n) (SHL((x),n) | ((x) >> (32 - n)))

#define P0(x) ((x) ^  ROTL((x),9) ^ ROTL((x),17))
#define P1(x) ((x) ^  ROTL((x),15) ^ ROTL((x),23))

	for(j = 16; j < 68; j++ )
	{
		//W[j] = P1( W[j-16] ^ W[j-9] ^ ROTL(W[j-3],15)) ^ ROTL(W[j - 13],7 ) ^ W[j-6];
		//Why thd release's result is different with the debug's ?
		//Below is okay. Interesting, Perhaps VC6 has a bug of Optimizaiton.

		Temp1 = W[j-16] ^ W[j-9];
		Temp2 = ROTL(W[j-3],15);
		Temp3 = Temp1 ^ Temp2;
		Temp4 = P1(Temp3);
		Temp5 =  ROTL(W[j - 13],7 ) ^ W[j-6];
		W[j] = Temp4 ^ Temp5;
	}

	for(j =  0; j < 64; j++)
	{
		W1[j] = W[j] ^ W[j+4];
	}

	A = ctx->state[0];
	B = ctx->state[1];
	C = ctx->state[2];
	D = ctx->state[3];
	E = ctx->state[4];
	F = ctx->state[5];
	G = ctx->state[6];
	H = ctx->state[7];

	for(j =0; j < 16; j++)
	{
		SS1 = ROTL((ROTL(A,12) + E + ROTL(T[j],j)), 7);
		SS2 = SS1 ^ ROTL(A,12);
		TT1 = FF0(A,B,C) + D + SS2 + W1[j];
		TT2 = GG0(E,F,G) + H + SS1 + W[j];
		D = C;
		C = ROTL(B,9);
		B = A;
		A = TT1;
		H = G;
		G = ROTL(F,19);
		F = E;
		E = P0(TT2);
	}

	for(j =16; j < 64; j++)
	{
		SS1 = ROTL((ROTL(A,12) + E + ROTL(T[j],j)), 7);
		SS2 = SS1 ^ ROTL(A,12);
		TT1 = FF1(A,B,C) + D + SS2 + W1[j];
		TT2 = GG1(E,F,G) + H + SS1 + W[j];
		D = C;
		C = ROTL(B,9);
		B = A;
		A = TT1;
		H = G;
		G = ROTL(F,19);
		F = E;
		E = P0(TT2);
	}

	ctx->state[0] ^= A;
	ctx->state[1] ^= B;
	ctx->state[2] ^= C;
	ctx->state[3] ^= D;
	ctx->state[4] ^= E;
	ctx->state[5] ^= F;
	ctx->state[6] ^= G;
	ctx->state[7] ^= H;
}

/*
* SM3 context setup
*/
void SM3Tool::sm3_starts( sm3_context *ctx )
{
	ctx->total[0] = 0;
	ctx->total[1] = 0;

	ctx->state[0] = 0x7380166F;
	ctx->state[1] = 0x4914B2B9;
	ctx->state[2] = 0x172442D7;
	ctx->state[3] = 0xDA8A0600;
	ctx->state[4] = 0xA96F30BC;
	ctx->state[5] = 0x163138AA;
	ctx->state[6] = 0xE38DEE4D;
	ctx->state[7] = 0xB0FB0E4E;

}

/*
* SM3 process buffer
*/
void SM3Tool::sm3_update( sm3_context *ctx, unsigned char *input, int ilen )
{
	int fill;
	unsigned int left;

	if( ilen <= 0 )
		return;

	left = ctx->total[0] & 0x3F;
	fill = 64 - left;

	ctx->total[0] += ilen;
	ctx->total[0] &= 0xFFFFFFFF;

	if( ctx->total[0] < (unsigned int) ilen )
		ctx->total[1]++;

	if( left && ilen >= fill )
	{
		memcpy( (void *) (ctx->buffer + left),
			(void *) input, fill );
		sm3_process( ctx, ctx->buffer );
		input += fill;
		ilen  -= fill;
		left = 0;
	}

	while( ilen >= 64 )
	{
		sm3_process( ctx, input );
		input += 64;
		ilen  -= 64;
	}

	if( ilen > 0 )
	{
		memcpy( (void *) (ctx->buffer + left),
			(void *) input, ilen );
	}
}

/*
* SM3 final digest
*/
void SM3Tool::sm3_finish( sm3_context *ctx, unsigned char output[32] )
{
	unsigned int last, padn;
	unsigned int high, low;
	unsigned char msglen[8];

	high = ( ctx->total[0] >> 29 )
		| ( ctx->total[1] <<  3 );
	low  = ( ctx->total[0] <<  3 );

	PUT_ULONG_BE( high, msglen, 0 );
	PUT_ULONG_BE( low,  msglen, 4 );

	last = ctx->total[0] & 0x3F;
	padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

	sm3_update( ctx, (unsigned char *) sm3_padding, padn );
	sm3_update( ctx, msglen, 8 );

	PUT_ULONG_BE( ctx->state[0], output,  0 );
	PUT_ULONG_BE( ctx->state[1], output,  4 );
	PUT_ULONG_BE( ctx->state[2], output,  8 );
	PUT_ULONG_BE( ctx->state[3], output, 12 );
	PUT_ULONG_BE( ctx->state[4], output, 16 );
	PUT_ULONG_BE( ctx->state[5], output, 20 );
	PUT_ULONG_BE( ctx->state[6], output, 24 );
	PUT_ULONG_BE( ctx->state[7], output, 28 );
}

/*
* output = SM3( input buffer )
*/
void SM3Tool::sm3( unsigned char *input, int ilen,unsigned char output[32] )
{
	sm3_context ctx;

	sm3_starts( &ctx );
	sm3_update( &ctx, input, ilen );
	sm3_finish( &ctx, output );

	memset( &ctx, 0, sizeof( sm3_context ) );
}

/*
* output = SM3( file contents )
*/
int SM3Tool::sm3_file( char *path, unsigned char output[32] )
{
	FILE *f;
	size_t n;
	sm3_context ctx;
	unsigned char buf[1024];
	f=fopen(path, "rb");
	if( f == NULL )
		return( 1 );

	sm3_starts( &ctx );

	while( ( n = fread( buf, 1, sizeof( buf ), f ) ) > 0 )
		sm3_update( &ctx, buf, (int) n );

	sm3_finish( &ctx, output );

	memset( &ctx, 0, sizeof( sm3_context ) );

	if( ferror( f ) != 0 )
	{
		fclose( f );
		return( 2 );
	}

	fclose( f );
	return( 0 );
}

/*
* SM3 HMAC context setup
*/
void SM3Tool::sm3_hmac_starts( sm3_context *ctx, unsigned char *key, int keylen )
{
	int i;
	unsigned char sum[32];

	if( keylen > 64 )
	{
		sm3( key, keylen, sum );
		keylen = 32;
		//keylen = ( is224 ) ? 28 : 32;
		key = sum;
	}

	memset( ctx->ipad, 0x36, 64 );
	memset( ctx->opad, 0x5C, 64 );

	for( i = 0; i < keylen; i++ )
	{
		ctx->ipad[i] = (unsigned char)( ctx->ipad[i] ^ key[i] );
		ctx->opad[i] = (unsigned char)( ctx->opad[i] ^ key[i] );
	}

	sm3_starts( ctx);
	sm3_update( ctx, ctx->ipad, 64 );

	memset( sum, 0, sizeof( sum ) );
}

/*
* SM3 HMAC process buffer
*/
void SM3Tool::sm3_hmac_update( sm3_context *ctx, unsigned char *input, int ilen )
{
	sm3_update( ctx, input, ilen );
}

/*
* SM3 HMAC final digest
*/
void SM3Tool::sm3_hmac_finish( sm3_context *ctx, unsigned char output[32] )
{
	int hlen;
	unsigned char tmpbuf[32];

	//is224 = ctx->is224;
	hlen =  32;

	sm3_finish( ctx, tmpbuf );
	sm3_starts( ctx );
	sm3_update( ctx, ctx->opad, 64 );
	sm3_update( ctx, tmpbuf, hlen );
	sm3_finish( ctx, output );

	memset( tmpbuf, 0, sizeof( tmpbuf ) );
}

/*
* output = HMAC-SM#( hmac key, input buffer )
*/
void SM3Tool::sm3_hmac( unsigned char *key, int keylen,unsigned char *input, int ilen,unsigned char output[32] )
{
	sm3_context ctx;

	sm3_hmac_starts( &ctx, key, keylen);
	sm3_hmac_update( &ctx, input, ilen );
	sm3_hmac_finish( &ctx, output );

	memset( &ctx, 0, sizeof( sm3_context ) );
}

bool SM3Tool::SM3HashWithPreprocess(unsigned char *input,unsigned int input_byte_len,unsigned char *public_key,unsigned int public_key_byte_len,unsigned char *signer_ID,unsigned int signer_ID_byte_len,unsigned char *hash_value,unsigned int *hash_value_byte_len_pointer)
{
	unsigned short ID_bit_len;
	unsigned char *step1_input;
	unsigned int step1_input_byte_len;
	unsigned char step1_output[32];
	unsigned char *step2_input;
	unsigned int step2_input_byte_len;

	unsigned char a[32]={0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC};

	unsigned char b[32]={0x28, 0xE9, 0xFA, 0x9E, 0x9D, 0x9F, 0x5E, 0x34,
		0x4D, 0x5A, 0x9E, 0x4B, 0xCF, 0x65, 0x09, 0xA7,
		0xF3, 0x97, 0x89, 0xF5, 0x15, 0xAB, 0x8F, 0x92,
		0xDD, 0xBC, 0xBD, 0x41, 0x4D, 0x94, 0xE,  0x93};

	unsigned char x_G[32]={0x32, 0xC4, 0xAE, 0x2C, 0x1F, 0x19, 0x81, 0x19,
		0x5F, 0x99, 0x4,  0x46, 0x6A, 0x39, 0xC9, 0x94,
		0x8F, 0xE3, 0xB,  0xBF, 0xF2, 0x66, 0xB,  0xE1,
		0x71, 0x5A, 0x45, 0x89, 0x33, 0x4C, 0x74, 0xC7};

	unsigned char y_G[32]={0xBC, 0x37, 0x36, 0xA2, 0xF4, 0xF6, 0x77, 0x9C,
		0x59, 0xBD, 0xCE, 0xE3, 0x6B, 0x69, 0x21, 0x53,
		0xD0, 0xA9, 0x87, 0x7C, 0xC6, 0x2A, 0x47, 0x40,
		0x2,  0xDF, 0x32, 0xE5, 0x21, 0x39, 0xF0, 0xA0};

	// 下面定义的结构体 x 用于判断当前环境是 big-endian 还是 little-endian
	union { int i;
	char c[sizeof(int)];
	} x;

	if ( (!public_key) || (!public_key_byte_len) || (!signer_ID) || (!signer_ID_byte_len) )
	{
		sm3(input, input_byte_len, hash_value);
		*hash_value_byte_len_pointer = 32U;
		return true;
	}

	// 下面为满足 SM2 签名的要求，做预处理操作
	step1_input_byte_len = (2 + signer_ID_byte_len + 32 * 6);
	if ( !(step1_input = new unsigned char [step1_input_byte_len]) )
	{
		return false;
	}

	/* 预处理1 */
	ID_bit_len = (unsigned short)(signer_ID_byte_len*8);

	/* 判断当前环境是 big-endian 还是 little-endian。
	国密规范中要求把 ENTL(用 2 个字节表示的 ID 的比特长度)
	以 big-endian 方式作为预处理 1 输入的前两个字节  */
	x.i = 1;
	if(x.c[0] == 1)  /* little-endian */
	{
		memcpy(step1_input, (unsigned char *)(&ID_bit_len) + 1, 1);
		memcpy((step1_input + 1), (unsigned char *)(&ID_bit_len), 1);
	}
	else  /* big-endian */
	{
		memcpy(step1_input, (unsigned char *)(&ID_bit_len), 1);
		memcpy((step1_input + 1), (unsigned char *)(&ID_bit_len) + 1, 1);
	}

	memcpy((step1_input + 1 + 1), signer_ID, signer_ID_byte_len);
	memcpy((step1_input + 1 + 1 + signer_ID_byte_len), a, 32);
	memcpy((step1_input + 1 + 1 + signer_ID_byte_len + 32), b, 32);
	memcpy((step1_input + 1 + 1 + signer_ID_byte_len + 32 + 32), x_G, 32);
	memcpy((step1_input + 1 + 1 + signer_ID_byte_len + 32 + 32 + 32), y_G, 32);
	memcpy((step1_input + 1 + 1 + signer_ID_byte_len + 32 + 32 + 32 + 32), public_key, 32);
	memcpy((step1_input + 1 + 1 + signer_ID_byte_len + 32 + 32 + 32 + 32 + 32), public_key + 32, 32);
	sm3(step1_input, step1_input_byte_len, step1_output);

	/* 预处理2 */
	step2_input_byte_len = (32 + input_byte_len);
	if ( !(step2_input = new unsigned char [step2_input_byte_len]) )
	{
		delete[] step1_input;
		return false;
	}
	memcpy(step2_input, step1_output, 32);
	memcpy((step2_input + 32), input, input_byte_len);
	sm3(step2_input, step2_input_byte_len, hash_value);
	*hash_value_byte_len_pointer = 32U;

	delete[] step1_input;
	delete[] step2_input;
	return true;
}
