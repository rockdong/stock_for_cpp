import { Controller, Get, Query, Param } from '@nestjs/common';
import { AnalysisService } from './analysis.service';

@Controller('api/analysis')
export class AnalysisController {
  constructor(private analysisService: AnalysisService) {}

  @Get('signals')
  async getSignals(
    @Query('label') label?: string,
    @Query('limit') limit?: string,
  ) {
    const data = await this.analysisService.getSignals(
      label,
      parseInt(limit || '50'),
    );
    return { success: true, data };
  }

  @Get('progress')
  async getProgress() {
    const data = await this.analysisService.getProgress();
    return { success: true, data };
  }

  @Get('process')
  async getProcess(
    @Query('ts_code') ts_code?: string,
    @Query('start_date') start_date?: string,
    @Query('end_date') end_date?: string,
    @Query('signal') signal?: string,
    @Query('strategy') strategy?: string,
    @Query('freq') freq?: string,
    @Query('limit') limit?: string,
  ) {
    // freq 可能是数组或单值
    const freqArray = freq ? freq.split(',') : undefined;
    const data = await this.analysisService.getProcessRecords({
      ts_code,
      start_date,
      end_date,
      signal,
      strategy,
      freq: freqArray,
      limit: parseInt(limit || '100'),
    });
    return { success: true, data };
  }

  @Get('process/strategies')
  async getStrategies() {
    const data = await this.analysisService.getStrategies();
    return { success: true, data };
  }

  @Get('process/chart/:tsCode')
  async getProcessChart(
    @Param('tsCode') tsCode: string,
    @Query('strategy') strategy?: string,
    @Query('freq') freq?: string,
  ) {
    const data = await this.analysisService.getProcessChart(
      tsCode,
      strategy,
      freq || 'd',
    );
    return { success: true, data };
  }

  @Get('process/:id')
  async getProcessById(@Param('id') id: string) {
    const data = await this.analysisService.getProcessById(parseInt(id));
    if (!data) return { success: false, error: '记录不存在' };
    return { success: true, data };
  }

  @Get(':code')
  async getByCode(@Param('code') code: string) {
    const data = await this.analysisService.getByCode(code);
    if (!data) return { success: false, error: '无分析结果' };
    return { success: true, data };
  }
}