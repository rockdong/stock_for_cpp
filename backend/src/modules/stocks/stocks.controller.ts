import { Controller, Get, Query, Param, Logger } from '@nestjs/common';
import { StocksService } from './stocks.service';
import { GetStocksDto, SearchStocksDto } from './stocks.dto';

@Controller('api/stocks')
export class StocksController {
  private readonly logger = new Logger(StocksController.name);
  
  constructor(private stocksService: StocksService) {}

  @Get()
  async findAll(@Query() dto: GetStocksDto) {
    try {
      const data = await this.stocksService.findAll(dto);
      return { success: true, data };
    } catch (err: any) {
      this.logger.error(`findAll failed: ${err.message}`, err.stack);
      return { success: false, error: err.message || 'Internal error', data: [] };
    }
  }

  @Get('search')
  async search(@Query() dto: SearchStocksDto) {
    try {
      const data = await this.stocksService.search(dto);
      return { success: true, data };
    } catch (err: any) {
      this.logger.error(`search failed: ${err.message}`, err.stack);
      return { success: false, error: err.message || 'Internal error', data: [] };
    }
  }

  @Get(':code')
  async findByCode(@Param('code') code: string) {
    try {
      const data = await this.stocksService.findByCode(code);
      if (!data) return { success: false, error: '股票不存在' };
      return { success: true, data };
    } catch (err: any) {
      this.logger.error(`findByCode failed: ${err.message}`, err.stack);
      return { success: false, error: err.message || 'Internal error' };
    }
  }
}