#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <string>
#include <vector>

namespace utils {

/**
 * @brief 加解密工具类
 * 使用 AES-256-CBC 加密
 */
class Encryptor {
public:
    /**
     * @brief 使用密钥加密字符串
     * @param plaintext 明文
     * @param key 密钥 (32 bytes for AES-256)
     * @return 加密后的 Base64 字符串 (IV + 密文)
     */
    static std::string encrypt(const std::string& plaintext, const std::string& key);
    
    /**
     * @brief 使用密钥解密字符串
     * @param ciphertext 加密后的 Base64 字符串
     * @param key 密钥
     * @return 解密后的明文，失败返回空
     */
    static std::string decrypt(const std::string& ciphertext, const std::string& key);
    
    /**
     * @brief 生成随机密钥
     * @param length 密钥长度 (默认 32 bytes)
     * @return 随机密钥的 Base64 字符串
     */
    static std::string generateKey(int length = 32);
    
    /**
     * @brief 从 Base64 密钥获取原始字节
     */
    static std::vector<unsigned char> keyFromBase64(const std::string& base64Key);
    
    /**
     * @brief 将字节转换为 Base64
     */
    static std::string toBase64(const std::vector<unsigned char>& data);
};

} // namespace utils

#endif // ENCRYPTOR_H
