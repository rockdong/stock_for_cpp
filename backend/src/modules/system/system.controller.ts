import { Controller, Get } from '@nestjs/common';

@Controller('api/system')
export class SystemController {
  
  @Get('version')
  getVersion() {
    return {
      success: true,
      data: {
        version: process.env.APP_VERSION || '2.4.1',
        buildTime: process.env.BUILD_TIME || '',
        env: process.env.NODE_ENV || 'production',
      },
    };
  }
  
  @Get('info')
  getInfo() {
    return {
      success: true,
      data: {
        name: 'StockLens API Server',
        version: process.env.APP_VERSION || '2.4.1',
        buildTime: process.env.BUILD_TIME || '',
        env: process.env.NODE_ENV || 'production',
      },
    };
  }
}
