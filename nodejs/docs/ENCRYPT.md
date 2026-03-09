# 环境变量加密

Node.js 项目现在支持对敏感环境变量进行加密存储，与 CPP 项目保持一致的加密方式。

## 加密方式

- 使用 AES-256-CBC 加密算法
- 每次加密生成随机 IV
- 密文格式：IV + 加密数据，Base64 编码
- 需要一个 32 字节的密钥（Base64 编码）

## 使用说明

### 1. 生成密钥

```bash
node encrypt.js generate-key
```

这会生成一个 `secret.key` 文件，包含加密密钥。

### 2. 加密环境变量

编辑 `.env` 文件，设置未加密的值：

```env
APP_ID=your_app_id_value
APP_SECRET=your_app_secret_value
ENCRYPT_KEY=your_encrypt_key_value
VERIFICATION_TOKEN=your_verification_token_value
```

然后运行加密命令：

```bash
node encrypt.js encrypt
```

这会在 `.env` 文件中添加加密后的值：

```env
APP_ID=your_app_id_value
# APP_ID_ENC=encrypted_value_here
APP_SECRET=your_app_secret_value
# APP_SECRET_ENC=encrypted_value_here
ENCRYPT_KEY=your_encrypt_key_value
# ENCRYPT_KEY_ENC=encrypted_value_here
VERIFICATION_TOKEN=your_verification_token_value
# VERIFICATION_TOKEN_ENC=encrypted_value_here
```

### 3. 自动解密

应用启动时会自动查找 `secret.key` 文件，如果存在，会自动解密 `_ENC` 后缀的环境变量，并覆盖原始环境变量。

### 4. 测试解密

验证加密配置是否正确：

```bash
node encrypt.js decrypt
```

## 安全注意事项

1. **secret.key 文件必须保密**
   - 不要提交到版本控制系统
   - 不要在日志或错误信息中暴露
   - 在生产环境中通过安全的方式分发

2. **备份管理**
   - 加密时会自动备份 `.env` 文件为 `.env.backup`
   - 定期清理备份文件

3. **CI/CD 集成**
   - 在 CI/CD 环境中，可以通过环境变量或安全文件管理 `secret.key`
   - 建议使用密钥管理服务（如 AWS Secrets Manager、HashiCorp Vault 等）

## 文件说明

- `src/utils/encryptor.js` - 加密工具类，实现与 CPP 版本兼容的加密算法
- `encrypt.js` - 加密管理脚本
- `secret.key` - 加密密钥文件（需要自行生成）
- `.env` - 环境变量文件
- `.gitignore` - 已配置忽略敏感文件

## 与 CPP 项目兼容

Node.js 版本的加密实现与 CPP 项目的 `Encryptor` 类完全兼容，可以使用相同的密钥加解密数据。