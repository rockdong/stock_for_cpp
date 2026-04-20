import { Controller, Get, Param, Query } from '@nestjs/common';
import { ChartsService } from './charts.service';

@Controller('api/charts')
export class ChartsController {
  constructor(private chartsService: ChartsService) {}

  @Get(':code')
  async getChartData(
    @Param('code') code: string,
    @Query('freq') freq: string = 'd',
    @Query('limit') limit: string = '100',
  ) {
    const data = await this.chartsService.getChartData(code, freq, parseInt(limit));
    return { success: true, data };
  }
}