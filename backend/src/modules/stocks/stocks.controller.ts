import { Controller, Get, Query, Param } from '@nestjs/common';
import { StocksService } from './stocks.service';
import { GetStocksDto, SearchStocksDto } from './stocks.dto';

@Controller('api/stocks')
export class StocksController {
  constructor(private stocksService: StocksService) {}

  @Get()
  async findAll(@Query() dto: GetStocksDto) {
    const data = await this.stocksService.findAll(dto);
    return { success: true, data };
  }

  @Get('search')
  async search(@Query() dto: SearchStocksDto) {
    const data = await this.stocksService.search(dto);
    return { success: true, data };
  }

  @Get(':code')
  async findByCode(@Param('code') code: string) {
    const data = await this.stocksService.findByCode(code);
    if (!data) return { success: false, error: '股票不存在' };
    return { success: true, data };
  }
}