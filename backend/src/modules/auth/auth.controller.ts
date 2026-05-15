import { Controller, Post, Body, UnauthorizedException, BadRequestException } from '@nestjs/common';
import { AuthService } from './auth.service';

@Controller('auth')
export class AuthController {
  constructor(private authService: AuthService) {}

  @Post('register')
  async register(@Body() body: { username: string; password: string }) {
    if (!body.username || !body.password) {
      throw new BadRequestException('请提供用户名和密码');
    }

    return await this.authService.register(body.username, body.password);
  }

  @Post('login')
  async login(@Body() body: { username: string; password: string }) {
    if (!body.username || !body.password) {
      throw new UnauthorizedException('请提供用户名和密码');
    }

    return await this.authService.login(body.username, body.password);
  }

  /**
   * 管理员账号密码登录
   * POST /api/auth/admin/login
   * Body: { username: string, password: string }
   */
  @Post('admin/login')
  async adminLogin(@Body() body: { username: string; password: string }) {
    if (!body.username || !body.password) {
      throw new UnauthorizedException('请提供用户名和密码');
    }

    return await this.authService.adminLogin(body.username, body.password);
  }

  /**
   * 生成密码 hash（仅供管理员配置使用）
   * POST /api/auth/admin/generate-hash
   * Body: { password: string }
   * 
   * 注意：生产环境建议禁用此接口，或添加额外的安全验证
   */
  @Post('admin/generate-hash')
  async generateHash(@Body() body: { password: string }) {
    if (!body.password) {
      return {
        success: false,
        message: '请提供密码',
      };
    }

    const hash = await this.authService.generatePasswordHash(body.password);
    
    return {
      success: true,
      data: {
        hash,
        message: '请将此 hash 设置到环境变量 ADMIN_PASSWORD_HASH 中',
      },
    };
  }

  /**
   * 验证 token 是否有效
   * POST /api/auth/admin/verify
   * Body: { token: string }
   */
  @Post('admin/verify')
  async verifyToken(@Body() body: { token: string }) {
    if (!body.token) {
      return {
        valid: false,
        error: '请提供 token',
      };
    }

    return await this.authService.verifyToken(body.token);
  }
}