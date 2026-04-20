import { Injectable } from '@nestjs/common';
import { PrismaService } from '../../prisma/prisma.service';

@Injectable()
export class ChartsService {
  constructor(private prisma: PrismaService) {}

  async getChartData(tsCode: string, freq: string = 'd', limit: number = 100) {
    const records = await this.prisma.chartData.findMany({
      where: { tsCode, freq },
      orderBy: { updatedAt: 'desc' },
      take: limit,
    });
    
    const candles = records
      .map(r => r.data as any[])
      .flat()
      .slice(-limit);
    return candles.reverse();
  }
}