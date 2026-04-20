import { Injectable } from '@nestjs/common';
import { ConfigService } from '@nestjs/config';
import { PrismaService } from '../../prisma/prisma.service';

@Injectable()
export class AnalysisService {
  constructor(
    private prisma: PrismaService,
    private config: ConfigService,
  ) {}

  async getSignals(label?: string, limit: number = 50) {
    const where = label ? { label } : {};
    const results = await this.prisma.analysisResult.findMany({
      where,
      orderBy: { tradeDate: 'desc' },
      take: limit,
      select: {
        tsCode: true,
        strategyName: true,
        label: true,
        freq: true,
        tradeDate: true,
      },
    });
    return results.map(r => ({
      ts_code: r.tsCode,
      strategy_name: r.strategyName,
      label: r.label,
      freq: r.freq,
      trade_date: r.tradeDate.toISOString().split('T')[0],
    }));
  }

  async getProgress() {
    const progress = await this.prisma.analysisProgress.findUnique({
      where: { id: 1 },
    });
    return {
      id: progress?.id || 1,
      phase1: {
        status: progress?.phase1Status || 'idle',
        total: progress?.phase1Total || 0,
        completed: progress?.phase1Completed || 0,
        qualified: progress?.phase1Qualified || 0,
      },
      phase2: {
        status: progress?.phase2Status || 'idle',
        total: progress?.phase2Total || 0,
        completed: progress?.phase2Completed || 0,
        failed: progress?.phase2Failed || 0,
      },
      started_at: progress?.startedAt || null,
      phase1_completed_at: progress?.phase1CompletedAt || null,
      updated_at: progress?.updatedAt?.toISOString() || '',
      execute_time: this.config.get('SCHEDULER_EXECUTE_TIME', '20:00'),
    };
  }

  async getProcessRecords(params: {
    ts_code?: string;
    start_date?: string;
    end_date?: string;
    limit?: number;
  }) {
    const where: any = {};
    if (params.ts_code) where.tsCode = params.ts_code;
    if (params.start_date)
      where.tradeDate = { ...where.tradeDate, gte: new Date(params.start_date) };
    if (params.end_date)
      where.tradeDate = { ...where.tradeDate, lte: new Date(params.end_date) };

    const records = await this.prisma.analysisProcessRecord.findMany({
      where,
      orderBy: [{ tradeDate: 'desc' }, { createdAt: 'desc' }],
      take: params.limit || 100,
    });

    return records.map(r => ({
      id: r.id,
      ts_code: r.tsCode,
      stock_name: r.stockName,
      trade_date: r.tradeDate.toISOString().split('T')[0],
      data: r.data,
      created_at: r.createdAt.toISOString(),
    }));
  }

  async getProcessById(id: number) {
    const record = await this.prisma.analysisProcessRecord.findUnique({
      where: { id },
    });
    if (!record) return null;
    return {
      id: record.id,
      ts_code: record.tsCode,
      stock_name: record.stockName,
      trade_date: record.tradeDate.toISOString().split('T')[0],
      data: record.data,
      created_at: record.createdAt.toISOString(),
    };
  }

  async getStrategies() {
    const strategiesEnv = this.config.get('STRATEGIES', '');
    if (!strategiesEnv) return [];
    return strategiesEnv.split(';').filter(s => s.trim());
  }

  async getProcessChart(tsCode: string, strategy?: string, freq: string = 'd') {
    const record = await this.prisma.analysisProcessRecord.findFirst({
      where: { tsCode },
      orderBy: { tradeDate: 'desc' },
    });

    if (!record) return [];

    const data = record.data as any;
    if (!data.strategies) return [];

    for (const s of data.strategies) {
      if (strategy && !s.name.includes(strategy)) continue;
      for (const f of s.freqs || []) {
        if (f.freq === freq) {
          return f.candles || [];
        }
      }
    }
    return [];
  }

  async getByCode(code: string) {
    const result = await this.prisma.analysisResult.findFirst({
      where: { tsCode: code },
      orderBy: { tradeDate: 'desc' },
    });
    if (!result) return null;
    return {
      ts_code: result.tsCode,
      strategy_name: result.strategyName,
      label: result.label,
      freq: result.freq,
      trade_date: result.tradeDate.toISOString().split('T')[0],
    };
  }
}