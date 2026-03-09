const crypto = require('crypto');

/**
 * 加解密工具类
 * 使用 AES-256-CBC 加密，与 CPP 项目的 Encryptor 保持一致
 */
class Encryptor {
    /**
     * 从 Base64 密钥获取原始字节
     * @param {string} base64Key - Base64 编码的密钥
     * @returns {Buffer} 原始字节
     */
    static keyFromBase64(base64Key) {
        return Buffer.from(base64Key, 'base64');
    }

    /**
     * 将字节转换为 Base64
     * @param {Buffer} data - 字节数据
     * @returns {string} Base64 字符串
     */
    static toBase64(data) {
        return data.toString('base64');
    }

    /**
     * 生成随机密钥
     * @param {number} length - 密钥长度（默认 32 bytes）
     * @returns {string} 随机密钥的 Base64 字符串
     */
    static generateKey(length = 32) {
        const key = crypto.randomBytes(length);
        return this.toBase64(key);
    }

    /**
     * 使用密钥加密字符串
     * @param {string} plaintext - 明文
     * @param {string} keyBase64 - Base64 编码的密钥 (32 bytes for AES-256)
     * @returns {string} 加密后的 Base64 字符串 (IV + 密文)
     */
    static encrypt(plaintext, keyBase64) {
        const key = this.keyFromBase64(keyBase64);
        
        if (key.length !== 32) {
            throw new Error('Key must be 32 bytes for AES-256');
        }
        
        // 生成随机 IV (16 bytes)
        const iv = crypto.randomBytes(16);
        
        // 创建加密器
        const cipher = crypto.createCipheriv('aes-256-cbc', key, iv);
        
        // 加密数据
        let ciphertext = cipher.update(plaintext, 'utf8');
        ciphertext = Buffer.concat([ciphertext, cipher.final()]);
        
        // 将 IV 和密文合并
        const result = Buffer.concat([iv, ciphertext]);
        
        return this.toBase64(result);
    }

    /**
     * 使用密钥解密字符串
     * @param {string} ciphertextBase64 - 加密后的 Base64 字符串
     * @param {string} keyBase64 - Base64 编码的密钥
     * @returns {string} 解密后的明文
     */
    static decrypt(ciphertextBase64, keyBase64) {
        const key = this.keyFromBase64(keyBase64);
        const data = this.keyFromBase64(ciphertextBase64);
        
        if (key.length !== 32) {
            throw new Error('Key must be 32 bytes for AES-256');
        }
        
        if (data.length < 16) {
            throw new Error('Invalid ciphertext');
        }
        
        // 提取 IV 和密文
        const iv = data.slice(0, 16);
        const ciphertext = data.slice(16);
        
        // 创建解密器
        const decipher = crypto.createDecipheriv('aes-256-cbc', key, iv);
        
        // 解密数据
        let plaintext = decipher.update(ciphertext, null, 'utf8');
        plaintext += decipher.final('utf8');
        
        return plaintext;
    }
}

module.exports = Encryptor;