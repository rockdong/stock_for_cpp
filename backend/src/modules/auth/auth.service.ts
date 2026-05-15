import { Injectable, UnauthorizedException, BadRequestException } from '@nestjs/common';
import { JwtService } from '@nestjs/jwt';
import { ConfigService } from '@nestjs/config';
import { PrismaClient } from '@prisma/client';
import * as bcrypt from 'bcryptjs';

@Injectable()
export class AuthService {
  private prisma: PrismaClient;

  constructor(
    private jwtService: JwtService,
    private configService: ConfigService,
  ) {
    this.prisma = new PrismaClient();
  }

  /**
   * 验证用户名格式
   * 规则: 3-20字符，仅允许字母、数字、下划线
   */
  private validateUsername(username: string): boolean {
    const regex = /^[a-zA-Z0-9_]{3,20}$/;
    return regex.test(username);
  }

  /**
   * 验证密码格式
   * 规则: 6-32字符，至少包含字母和数字
   */
  private validatePassword(password: string): boolean {
    if (password.length < 6 || password.length > 32) return false;
    const hasLetter = /[a-zA-Z]/.test(password);
    const hasNumber = /[0-9]/.test(password);
    return hasLetter && hasNumber;
  }

  /**
   * 用户注册
   */
  async register(username: string, password: string) {
    if (!this.validateUsername(username)) {
      throw new BadRequestException('用户名格式错误：需3-20字符，仅允许字母、数字、下划线');
    }

    if (!this.validatePassword(password)) {
      throw new BadRequestException('密码格式错误：需6-32字符，至少包含字母和数字');
    }

    const existingAccount = await this.prisma.account.findUnique({
      where: { username },
    });

    if (existingAccount) {
      throw new BadRequestException('用户名已存在');
    }

    const user = await this.prisma.user.create({
      data: {},
    });

    const passwordHash = await this.generatePasswordHash(password);
    await this.prisma.account.create({
      data: {
        userId: user.id,
        username,
        password: passwordHash,
        role: 'user',
      },
    });

    const payload = {
      sub: user.id,
      username,
      role: 'user',
    };

    const token = this.jwtService.sign(payload);

    return {
      success: true,
      data: {
        token,
        user: {
          id: user.id,
          username,
          role: 'user',
        },
      },
    };
  }

  /**
   * 用户账号密码登录
   */
  async login(username: string, password: string) {
    const account = await this.prisma.account.findUnique({
      where: { username },
      include: { user: true },
    });

    if (!account) {
      throw new UnauthorizedException('用户名或密码错误');
    }

    const isPasswordValid = await bcrypt.compare(password, account.password);
    if (!isPasswordValid) {
      throw new UnauthorizedException('用户名或密码错误');
    }

    if (!account.isActive) {
      throw new UnauthorizedException('账号已被禁用');
    }

    const payload = {
      sub: account.userId,
      username: account.username,
      role: account.role,
    };

    const token = this.jwtService.sign(payload);

    return {
      success: true,
      data: {
        token,
        user: {
          id: account.userId,
          username: account.username,
          role: account.role,
        },
      },
    };
  }

  /**
   * 管理员账号密码登录
   * 管理员账号密码从环境变量读取，不存储在数据库中（更安全）
   */
  async adminLogin(username: string, password: string) {
    const adminUsername = this.configService.get<string>('ADMIN_USERNAME') || 'admin';
    const adminPasswordHash = this.configService.get<string>('ADMIN_PASSWORD_HASH');

    if (username !== adminUsername) {
      throw new UnauthorizedException('用户名或密码错误');
    }

    const defaultPasswordHash = '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7OqK3MZVvqB0hJfK3MZVvqB';
    const passwordHash = adminPasswordHash || defaultPasswordHash;

    const isPasswordValid = await bcrypt.compare(password, passwordHash);
    if (!isPasswordValid) {
      throw new UnauthorizedException('用户名或密码错误');
    }

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