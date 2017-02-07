/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "device.h"
#include "serial_api.h"
#include "hal_crypto.h"
#include "main.h"
#include "diag.h"
#include <polarssl/aes.h>

#define STACKSIZE                   2048

void test_sha1(void)
{
    const unsigned char *sha1_text[3] = {
        "",
        "abc",
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
    };
    const uint8_t sha1_test_digest[3][20] = {
        { 0xda,0x39,0xa3,0xee, 0x5e,0x6b,0x4b,0x0d, 0x32,0x55,0xbf,0xef, 0x95,0x60,0x18,0x90, 0xaf,0xd8,0x07,0x09 },
        { 0xa9,0x99,0x3e,0x36, 0x47,0x06,0x81,0x6a, 0xba,0x3e,0x25,0x71, 0x78,0x50,0xc2,0x6c, 0x9c,0xd0,0xd8,0x9d },
        { 0x84,0x98,0x3e,0x44, 0x1c,0x3b,0xd2,0x6e, 0xba,0xae,0x4a,0xa1, 0xf9,0x51,0x29,0xe5, 0xe5,0x46,0x70,0xf1 }
    };
    uint32_t i;
    uint8_t digest[20];
    int ret;
    for (i=0; i<3; i++)
    {
        memset((void*)digest, 0, sizeof(digest));
        ret = rtl_crypto_sha1(sha1_text[i], strlen(sha1_text[i]), digest);
        if ( rtl_memcmpb((void*)digest, (void*)&sha1_test_digest[i][0], 20) == 0 )
            DiagPrintf("SHA1 test result is correct, ret=%d\r\n", ret);
        else
            DiagPrintf("SHA test result is WRONG!!, ret=%d\r\n", ret);
    }
}
//static const u8 plaintext[] 	= "The quick brown fox jumps over the lazy dog";
//static const u8 md5_digest[] 	=  "\x9e\x10\x7d\x9d\x37\x2b\xb6\x82"
//					 			   "\x6b\xd8\x1d\x35\x42\xa4\x19\xd6";
//static const u8 md5_key[] 		= "key";

static const char plaintext[] = "12345678901234567890123456789012345678901234567890123456789012" \
      "345678901234567890";
static const char md5_digest[] = { 0x57, 0xED, 0xF4, 0xA2, 0x2B, 0xE3, 0xC9, 0x55,
      0xAC, 0x49, 0xDA, 0x2E, 0x21, 0x07, 0xB6, 0x7A };
static const u8 md5_key[] 		= "key";

static unsigned char md5_test_buf[16][128] =
{
    { "" },
    { "a" },
    { "abc" },
    { "message digest" },
    { "abcdefghijklmnopqrstuvwxyz" },
    { "The quick brown fox jumps over the lazy dog" },
    { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" },
    { "12345678901234567890123456789012345678901234567890123456789012" \
      "345678901234567890" },
      { "" },
    { "a" },
    { "abc" },
    { "message digest" },
    { "abcdefghijklmnopqrstuvwxyz" },
    { "The quick brown fox jumps over the lazy dog" },
    { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" },
    { "12345678901234567890123456789012345678901234567890123456789012" \
      "345678901234567890" }
};

static const int md5_test_buflen[16] =
{
    0, 1, 3, 14, 26, 43, 62, 80, 0, 1, 3, 14, 26, 43, 62, 80
};

static const unsigned char md5_test_sum[16][16] =
{
    { 0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
      0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E },
    { 0x0C, 0xC1, 0x75, 0xB9, 0xC0, 0xF1, 0xB6, 0xA8,
      0x31, 0xC3, 0x99, 0xE2, 0x69, 0x77, 0x26, 0x61 },
    { 0x90, 0x01, 0x50, 0x98, 0x3C, 0xD2, 0x4F, 0xB0,
      0xD6, 0x96, 0x3F, 0x7D, 0x28, 0xE1, 0x7F, 0x72 },
    { 0xF9, 0x6B, 0x69, 0x7D, 0x7C, 0xB7, 0x93, 0x8D,
      0x52, 0x5A, 0x2F, 0x31, 0xAA, 0xF1, 0x61, 0xD0 },
    { 0xC3, 0xFC, 0xD3, 0xD7, 0x61, 0x92, 0xE4, 0x00,
      0x7D, 0xFB, 0x49, 0x6C, 0xCA, 0x67, 0xE1, 0x3B },
      {"\x9e\x10\x7d\x9d\x37\x2b\xb6\x82"
        "\x6b\xd8\x1d\x35\x42\xa4\x19\xd6"},
    { 0xD1, 0x74, 0xAB, 0x98, 0xD2, 0x77, 0xD9, 0xF5,
      0xA5, 0x61, 0x1C, 0x2C, 0x9F, 0x41, 0x9D, 0x9F },
    { 0x57, 0xED, 0xF4, 0xA2, 0x2B, 0xE3, 0xC9, 0x55,
      0xAC, 0x49, 0xDA, 0x2E, 0x21, 0x07, 0xB6, 0x7A },
      { 0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
      0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E },
    { 0x0C, 0xC1, 0x75, 0xB9, 0xC0, 0xF1, 0xB6, 0xA8,
      0x31, 0xC3, 0x99, 0xE2, 0x69, 0x77, 0x26, 0x61 },
    { 0x90, 0x01, 0x50, 0x98, 0x3C, 0xD2, 0x4F, 0xB0,
      0xD6, 0x96, 0x3F, 0x7D, 0x28, 0xE1, 0x7F, 0x72 },
    { 0xF9, 0x6B, 0x69, 0x7D, 0x7C, 0xB7, 0x93, 0x8D,
      0x52, 0x5A, 0x2F, 0x31, 0xAA, 0xF1, 0x61, 0xD0 },
    { 0xC3, 0xFC, 0xD3, 0xD7, 0x61, 0x92, 0xE4, 0x00,
      0x7D, 0xFB, 0x49, 0x6C, 0xCA, 0x67, 0xE1, 0x3B },
      {"\x9e\x10\x7d\x9d\x37\x2b\xb6\x82"
        "\x6b\xd8\x1d\x35\x42\xa4\x19\xd6"},
    { 0xD1, 0x74, 0xAB, 0x98, 0xD2, 0x77, 0xD9, 0xF5,
      0xA5, 0x61, 0x1C, 0x2C, 0x9F, 0x41, 0x9D, 0x9F },
    { 0x57, 0xED, 0xF4, 0xA2, 0x2B, 0xE3, 0xC9, 0x55,
      0xAC, 0x49, 0xDA, 0x2E, 0x21, 0x07, 0xB6, 0x7A },

};




u8 digest[64];
u8 cipher_result[2048];
u8 test_result[1024];

serial_t	sobj;

/*
 *
 *
 *  This test_md5 function is used to test hardware md5 functoinality
 */
void test_md5(void)
{
	int i;
	int ret;
	u8 md5sum[16];

	DiagPrintf("MD5 test\r\n"); 
	
	ret = rtl_crypto_md5(plaintext, strlen(plaintext), (unsigned char *)&digest); // the length of MD5's digest is 16 bytes. 

	if ( rtl_memcmpb(digest, md5_digest, 16) == 0 ) {
		DiagPrintf("MD5 test result is correct, ret=%d\r\n", ret);
	} else {
		DiagPrintf("MD5 test result is WRONG!!, ret=%d\r\n", ret);
	}

	for( i = 0; i < 16; i++ )
	{	
		DiagPrintf( "  MD5 test #%d: ", i + 1 );
		ret = rtl_crypto_md5(md5_test_buf[i], md5_test_buflen[i], md5sum); // the length of MD5's digest is 16 bytes.
		DiagPrintf(" MD5 ret=%d\n", ret);
		if( rtl_memcmpb( md5sum, md5_test_sum[i], 16 ) != 0 )
		{
			DiagPrintf( "failed\n" );
			 memset(md5sum,0,16);
		}
		else{
			DiagPrintf( "passed\n" );
			memset(md5sum,0,16);}
	}
	

}


//
// vector : AES CBC 128 bit : 
// http://www.inconteam.com/software-development/41-encryption/55-aes-test-vectors#aes-cbc-128 
//

//#ifdef __ICCARM__
//#pragma data_alignment = 4
//#elif defined (__GNUC__)
//__attribute__ ((aligned (4)))
//#endif
static const unsigned char aes_test_key[16] =
{
	0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
	0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
} ;


//#ifdef __ICCARM__
//#pragma data_alignment = 4
//#elif defined (__GNUC__)
//__attribute__ ((aligned (4)))
//#endif
static const unsigned char aes_test_iv_1[16] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

static const unsigned char aes_test_buf[16] =
{
	0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
};
static const unsigned char aes_test_ecb_buf[160] =
{
	0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};



static const unsigned char aes_test_res_128[16] =
{
	0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
	0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d
};

static const unsigned char aes_test_ecb_res_128[160] =
{
	0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
	0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
	0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
	0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
	0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
	0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
	0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
	0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
	0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
	0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
	0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97
};

/*
 *
 *  THis test_aes_cbc function is use to directly test hardware aes cbc crypto functionality
 *  
 */
int test_aes_cbc(void)
{
    const u8 *key, *pIv;
	u32 keylen= 0;
	u32 ivlen = 0;
    u8 *message;
	u32 msglen; 
    u8 *pResult;

	int ret;

	DiagPrintf("AES CBC test\r\n"); 

	key = aes_test_key;
	keylen = 16;
	pIv = aes_test_iv_1;
	ivlen = 16;

	pResult = cipher_result;		  
        
	message = (unsigned char *)aes_test_buf;	
	msglen = sizeof(aes_test_buf);
	ret = rtl_crypto_aes_cbc_init(key,keylen);
	if ( ret != 0 ) {
		DiagPrintf("AES CBC init failed\r\n");
		return ret;
	}
        
	ret = rtl_crypto_aes_cbc_encrypt(message, msglen, pIv, ivlen, pResult);
	if ( ret != 0 ) {
		DiagPrintf("AES CBC encrypt failed\r\n");
		return ret;
	}
	if ( rtl_memcmpb(aes_test_res_128, pResult, msglen) == 0 ) {
		DiagPrintf("AES CBC encrypt result success\r\n");	
	} else {
		DiagPrintf("AES CBC encrypt result failed\r\n");	
	}

	message = pResult;
	
	ret = rtl_crypto_aes_cbc_decrypt(message, msglen, pIv, ivlen, pResult);
	if ( ret != 0 ) {
		DiagPrintf("AES CBC decrypt failed, ret=%d\r\n", ret);
		return ret;
	}

	if ( rtl_memcmpb(aes_test_buf, pResult, msglen) == 0 ) {
		DiagPrintf("AES CBC decrypt result success\r\n");	
	} else {
		DiagPrintf("AES CBC decrypt result failed\r\n");	
	}

	return 0;
}

/*
 *
 *  THis test_aes_ecb function is use to directly test hardware ecb cbc crypto functionality
 *  
 *  The input parameter for ecb need to confirm iv is null and ivlen is 0
 */
int test_aes_ecb(void)
{
    
  const u8 *key, *pIv;
        u32 keylen= 0;
        u32 ivlen = 0;
    u8 *message;
        u32 msglen; 
    u8 *pResult;
 
        int ret;
 
        DiagPrintf("AES ECB test\r\n"); 
 
        key = aes_test_key;
        keylen = 16;
        pIv = NULL;
        ivlen = 0;
 
        pResult = cipher_result;            
        message = (unsigned char *)aes_test_ecb_buf;      
        msglen = sizeof(aes_test_buf);
        //for(int i=0;i<msglen;i++)
        //printf("\r\n first message[%d] = %p,",i,message[i]);
        ret = rtl_crypto_aes_ecb_init(key,keylen);
        if ( ret != 0 ) {
                DiagPrintf("AES ECB init failed\r\n");
                return ret;
        }
        
        ret = rtl_crypto_aes_ecb_encrypt(message, msglen, pIv, ivlen, pResult);
        if ( ret != 0 ) {
                DiagPrintf("AES ECB encrypt failed\r\n");
                return ret;
        }
 
        if ( rtl_memcmpb(aes_test_ecb_res_128, pResult, msglen) == 0 ) 
        {
                DiagPrintf("AES ECB encrypt result success\r\n");  
        } 
        else {
                DiagPrintf("AES ECB encrypt result failed\r\n");       
        }
 
        message = pResult;
        //for(int i=0;i<msglen;i++)
        //printf("\r\n second message[%d] = %p,",i,message[i]);
        ret = rtl_crypto_aes_ecb_decrypt(message, msglen, pIv, ivlen, pResult);
        if ( ret != 0 ) {
                DiagPrintf("AES ECB decrypt failed, ret=%d\r\n", ret);
                return ret;
        }
 
        if ( rtl_memcmpb(aes_test_ecb_buf, pResult, msglen) == 0 ) 
        {
                DiagPrintf("AES ECB decrypt result success\r\n");
        } 
        else {
                DiagPrintf("AES ECB decrypt result failed\r\n");       
        }
 
        //for(int i=0;i<msglen;i++)
        //printf("\r\n last message[%d] = %p,",i,message[i]);
        return 0;
}




void main(void)
{
     // sample text
    char rc;
	//
	int ret;
	int loop=0;
        u32 keylen= 0;
	u32 ivlen = 0;
    u8 *pResult;
    u8 *message;
    u32 *ResultLen;
    u32 msglen = 0;
    const u8 *key, *pIv;
    key = aes_test_key;
	keylen = 16;
	pIv = aes_test_iv_1;
	ivlen = 16;
    //
    message = (unsigned char *)aes_test_buf;	
    msglen = sizeof(aes_test_buf);

	
    DiagPrintf("CRYPTO API Demo...\r\n");

	if ( rtl_cryptoEngine_init() != 0 ) {
		DiagPrintf("crypto engine init failed\r\n");            
	}	
        else
          printf("init success\n");
       
        
        pResult = test_result;	
	test_md5();   
	test_aes_cbc();
        test_aes_ecb();
        //aes_test();  //added api combined aes_cbc setkey and cryption into one function
    
}

