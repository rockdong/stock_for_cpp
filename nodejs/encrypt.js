#!/usr/bin/env node

const fs = require('fs');
const path = require('path');
const Encryptor = require('./src/utils/encryptor');
require('dotenv').config();

// 需要加密的环境变量
const ENV_VARS_TO_ENCRYPT = [
    'APP_SECRET',
    'ENCRYPT_KEY',
    'VERIFICATION_TOKEN',
    'APP_ID'
];

function main() {
    const args = process.argv.slice(2);
    const command = args[0];

    switch(command) {
        case 'generate-key':
            generateSecretKey();
            break;
        case 'encrypt':
            encryptEnvVars();
            break;
        case 'decrypt':
            decryptEnvVars();
            break;
        default:
            console.log('使用方法:');
            console.log('  node encrypt.js generate-key  # 生成加密密钥');
            console.log('  node encrypt.js encrypt      # 加密环境变量');
            console.log('  node encrypt.js decrypt      # 解密环境变量（测试用）');
            process.exit(1);
    }
}

function generateSecretKey() {
    const key = Encryptor.generateKey(32);
    const keyFile = path.join(__dirname, 'secret.key');
    
    fs.writeFileSync(keyFile, key, 'utf8');
    console.log('✅ 密钥已生成并保存到 secret.key');
    console.log(`密钥: ${key}`);
    console.log('⚠️  请妥善保管此密钥文件，不要提交到版本控制系统！');
}

function encryptEnvVars() {
    // 读取密钥
    const keyFile = path.join(__dirname, 'secret.key');
    if (!fs.existsSync(keyFile)) {
        console.error('❌ 未找到 secret.key 文件，请先运行: node encrypt.js generate-key');
        process.exit(1);
    }
    
    const secretKey = fs.readFileSync(keyFile, 'utf8').trim();
    if (!secretKey) {
        console.error('❌ secret.key 文件为空');
        process.exit(1);
    }

    // 读取 .env 文件
    const envFile = path.join(__dirname, '.env');
    if (!fs.existsSync(envFile)) {
        console.error('❌ 未找到 .env 文件');
        process.exit(1);
    }

    const envContent = fs.readFileSync(envFile, 'utf8');
    const lines = envContent.split('\n');
    const newLines = [];
    let hasChanges = false;

    for (const line of lines) {
        const trimmedLine = line.trim();
        
        // 跳过空行和注释
        if (!trimmedLine || trimmedLine.startsWith('#')) {
            newLines.push(line);
            continue;
        }

        // 检查是否是目标环境变量
        const eqIndex = trimmedLine.indexOf('=');
        if (eqIndex === -1) {
            newLines.push(line);
            continue;
        }

        const varName = trimmedLine.substring(0, eqIndex);
        const varValue = trimmedLine.substring(eqIndex + 1);
        
        if (ENV_VARS_TO_ENCRYPT.includes(varName) && varValue) {
            try {
                // 加密值
                const encryptedValue = Encryptor.encrypt(varValue, secretKey);
                // 添加加密后的变量，保留原值作为注释
                newLines.push(line); // 保留原行
                newLines.push(`# ${varName}_ENC=${encryptedValue}`);
                hasChanges = true;
                console.log(`✅ 已加密: ${varName}`);
            } catch (error) {
                console.error(`❌ 加密失败 ${varName}:`, error.message);
                newLines.push(line);
            }
        } else {
            newLines.push(line);
        }
    }

    if (hasChanges) {
        // 备份原文件
        const backupFile = path.join(__dirname, '.env.backup');
        fs.copyFileSync(envFile, backupFile);
        
        // 写入新内容
        fs.writeFileSync(envFile, newLines.join('\n'), 'utf8');
        console.log('✅ .env 文件已更新，备份保存为 .env.backup');
    } else {
        console.log('ℹ️  没有找到需要加密的变量');
    }
}

function decryptEnvVars() {
    // 读取密钥
    const keyFile = path.join(__dirname, 'secret.key');
    if (!fs.existsSync(keyFile)) {
        console.error('❌ 未找到 secret.key 文件');
        process.exit(1);
    }
    
    const secretKey = fs.readFileSync(keyFile, 'utf8').trim();

    // 读取 .env 文件
    const envFile = path.join(__dirname, '.env');
    const envContent = fs.readFileSync(envFile, 'utf8');
    const lines = envContent.split('\n');

    console.log('解密测试结果:');
    
    for (const line of lines) {
        const trimmedLine = line.trim();
        if (trimmedLine.startsWith('#') && trimmedLine.includes('_ENC=')) {
            const match = trimmedLine.match(/# (\w+)_ENC=(.+)/);
            if (match) {
                const varName = match[1];
                const encryptedValue = match[2];
                try {
                    const decryptedValue = Encryptor.decrypt(encryptedValue, secretKey);
                    console.log(`${varName}: ${decryptedValue}`);
                } catch (error) {
                    console.error(`${varName}: 解密失败 - ${error.message}`);
                }
            }
        }
    }
}

if (require.main === module) {
    main();
}

module.exports = { generateSecretKey, encryptEnvVars, decryptEnvVars };