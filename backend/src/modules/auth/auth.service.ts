import { Injectable, UnauthorizedException } from '@nestjs/common';
import { JwtService } from '@nestjs/jwt';
import { ConfigService } from '@nestjs/config';
import * as bcrypt from 'bcryptjs';

@Injectable()
export class AuthService {
  constructor(
    private jwtService: JwtService,
    private configService: ConfigService,
  ) {}

  /**
   * 管理员账号密码登录
   * 管理员账号密码从环境变量读取，不存储在数据库中（更安全）
   */
  async adminLogin(username: string, password: string) {
    // 从环境变量获取管理员账号配置
    const adminUsername = this.configService.get<string>('ADMIN_USERNAME') || 'admin';
    const adminPasswordHash = this.configService.get<string>('ADMIN_PASSWORD_HASH');

    // 验证用户名
    if (username !== adminUsername) {
      throw new UnauthorizedException('用户名或密码错误');
    }

    // 验证密码
    // 如果没有配置密码 hash，使用默认密码 'admin123' 的 hash
    const defaultPasswordHash = '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7OqK3MZVvqB0hJfK3MZVvqB';
    const passwordHash = adminPasswordHash || defaultPasswordHash;

    const isPasswordValid = await bcrypt.compare(password, passwordHash);
    if (!isPasswordValid) {
      throw new UnauthorizedException('用户名或密码错误');
    }

    // 生成 JWT token
    const payload = {
      sub: 'admin',
      username: adminUsername,
      role: 'admin',
    };

    const token = this.jwtService.sign(payload);

    return {
      success: true,
      data: {
        token,
        user: {
          id: 'admin',
          username: adminUsername,
          role: 'admin',
        },
      },
    };
  }

  /**
   * 验证 JWT token
   */
  async verifyToken(token: string) {
    try {
      const payload = this.jwtService.verify(token);
      return {
        valid: true,
        user: payload,
      };
    } catch (error) {
      return {
        valid: false,
        error: error.message,
      };
    }
  }

  /**
   * 生成密码 hash（用于配置环境变量）
   * 使用方法：调用此方法获取 hash，然后设置到 ADMIN_PASSWORD_HASH 环境变量
   */
  async generatePasswordHash(password: string): Promise<string> {
    const salt = await bcrypt.genSalt(10);
    return await bcrypt.hash(password, salt);
  }
}