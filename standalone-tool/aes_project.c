#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

static const char magic[] = "Salted__";


unsigned char *data = NULL;
long size = 0;
int loadFile(const char *filename){
    FILE *fp = fopen(filename, "rb");

    if (!fp) return -1;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    data = malloc(size + 1);

    if (!data){
        fclose(fp);
        return -1;
    }

    fread(data, 1, size, fp);
    fclose(fp);
    return 0;
}


int saveFile(const char *filename, unsigned char *data, long len){
    FILE *fp = fopen(filename, "wb");

    if (!fp) return -1;
    fwrite(data, 1, len, fp);
    fclose(fp);
    return 0;
}


int encrypt_func(const char *password, int useSalt, unsigned char **outBuf, long *outLen){
    unsigned char salt[8];
    unsigned char key[32];
    unsigned char iv[32];
    const unsigned char *saltPtr = NULL;

    if (useSalt) {
        RAND_bytes(salt, 8);
        saltPtr = salt;
    }


    EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), saltPtr,
                   (unsigned char*)password, strlen(password), 1, key, iv);
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    int bufSize = size + 64 + (useSalt ? 16 : 0);
    unsigned char *ciphertext = malloc(bufSize);
    int pos = 0;
    int tmpLen;

    if (useSalt){
        memcpy(ciphertext, magic, 8);
        memcpy(ciphertext + 8, salt, 8);
        pos = 16;
    }


    EVP_EncryptUpdate(ctx, ciphertext + pos, &tmpLen, data, size);
    pos += tmpLen;
    EVP_EncryptFinal_ex(ctx, ciphertext + pos, &tmpLen);
    pos += tmpLen;
    EVP_CIPHER_CTX_free(ctx);
    *outBuf = ciphertext;
    *outLen = pos;
    return 0;
}


int decrypt_func(const char *password, int expectSalt, unsigned char **outBuf, long *outLen){
    unsigned char key[32];
    unsigned char iv[32];
    unsigned char salt[8];
    const unsigned char *saltPtr = NULL;
    unsigned char *inPtr = data;
    long inLen = size;
    if (expectSalt) {
        if (size < 16 || memcmp(data, magic, 8))
            return -1;
        memcpy(salt, data + 8, 8);
        saltPtr = salt;
        inPtr += 16;
        inLen -= 16;
    }


    EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), saltPtr,
                   (unsigned char*)password, strlen(password), 1, key, iv);
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    unsigned char *plaintext = malloc(inLen + 64);
    int len1, len2;
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_DecryptUpdate(ctx, plaintext, &len1, inPtr, inLen);


    int pass = EVP_DecryptFinal_ex(ctx, plaintext + len1, &len2);
    if (pass <= 0){
        len2 = 0;
    }



    EVP_CIPHER_CTX_free(ctx);
    *outBuf = plaintext;
    *outLen = len1 + len2;
    return 0;
}





int main(int argc, char *argv[]){
    if(argc < 5) return 1;
    int encrypt = (argv[4][0] == 'e' );

    int useSalt = 1;

    if (argc > 5 && strcmp(argv[5], "nosalt")==0){
        useSalt =0;
    }

    if(loadFile(argv[1])) return 1;
    unsigned char *result = NULL;
    long resultLen = 0;

    if (encrypt)
        encrypt_func(argv[3], useSalt, &result, &resultLen);
    else
        decrypt_func(argv[3], useSalt, &result, &resultLen);
    saveFile(argv[2], result, resultLen);

    free(data);
    free(result);

    return 0;

}
