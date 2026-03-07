#include "Encryptor.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <stdexcept>
#include <cstring>

namespace utils {

std::vector<unsigned char> Encryptor::keyFromBase64(const std::string& base64Key) {
    BIO* bio = BIO_new_mem_buf(base64Key.c_str(), static_cast<int>(base64Key.length()));
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);
    
    std::vector<unsigned char> buffer(base64Key.length());
    int decodedLen = BIO_read(bio, buffer.data(), static_cast<int>(buffer.size()));
    BIO_free_all(bio);
    
    buffer.resize(decodedLen > 0 ? decodedLen : 0);
    return buffer;
}

std::string Encryptor::toBase64(const std::vector<unsigned char>& data) {
    BIO* bio = NULL;
    BIO* b64 = NULL;
    BUF_MEM* bufferPtr = NULL;
    
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    
    BIO_write(bio, data.data(), static_cast<int>(data.size()));
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    
    std::string result(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);
    
    return result;
}

std::string Encryptor::generateKey(int length) {
    std::vector<unsigned char> key(length);
    if (RAND_bytes(key.data(), length) != 1) {
        throw std::runtime_error("Failed to generate random key");
    }
    return toBase64(key);
}

std::string Encryptor::encrypt(const std::string& plaintext, const std::string& keyBase64) {
    std::vector<unsigned char> key = keyFromBase64(keyBase64);
    
    if (key.size() != 32) {
        throw std::runtime_error("Key must be 32 bytes for AES-256");
    }
    
    unsigned char iv[16];
    if (RAND_bytes(iv, 16) != 1) {
        throw std::runtime_error("Failed to generate IV");
    }
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }
    
    std::vector<unsigned char> ciphertext(plaintext.size() + AES_BLOCK_SIZE);
    int outLen1 = 0, outLen2 = 0;
    
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize encryption");
    }
    
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &outLen1, 
                          reinterpret_cast<const unsigned char*>(plaintext.c_str()), 
                          static_cast<int>(plaintext.length())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to encrypt");
    }
    
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + outLen1, &outLen2) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize encryption");
    }
    
    EVP_CIPHER_CTX_free(ctx);
    
    ciphertext.resize(outLen1 + outLen2);
    
    std::vector<unsigned char> result;
    result.insert(result.end(), iv, iv + 16);
    result.insert(result.end(), ciphertext.begin(), ciphertext.end());
    
    return toBase64(result);
}

std::string Encryptor::decrypt(const std::string& ciphertextBase64, const std::string& keyBase64) {
    std::vector<unsigned char> key = keyFromBase64(keyBase64);
    std::vector<unsigned char> data = keyFromBase64(ciphertextBase64);
    
    if (key.size() != 32) {
        throw std::runtime_error("Key must be 32 bytes for AES-256");
    }
    
    if (data.size() < 16) {
        throw std::runtime_error("Invalid ciphertext");
    }
    
    unsigned char iv[16];
    std::memcpy(iv, data.data(), 16);
    
    std::vector<unsigned char> ciphertext(data.begin() + 16, data.end());
    std::vector<unsigned char> plaintext(ciphertext.size());
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }
    
    int outLen1 = 0, outLen2 = 0;
    
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize decryption");
    }
    
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &outLen1, ciphertext.data(), 
                         static_cast<int>(ciphertext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to decrypt");
    }
    
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + outLen1, &outLen2) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize decryption - wrong key or corrupted data");
    }
    
    EVP_CIPHER_CTX_free(ctx);
    
    plaintext.resize(outLen1 + outLen2);
    return std::string(plaintext.begin(), plaintext.end());
}

} // namespace utils
