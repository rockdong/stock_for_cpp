import { Injectable } from '@nestjs/common';
import { PrismaService } from '../../prisma/prisma.service';
import { GetStocksDto, SearchStocksDto } from './stocks.dto';

@Injectable()
export class StocksService {
  constructor(private prisma: PrismaService) {}

  async findAll(dto: GetStocksDto) {
    const stocks = await this.prisma.stock.findMany({
      take: dto.limit,
      select: {
        tsCode: true,
        name: true,
        industry: true,
        market: true,
      },
    });
    return stocks;
  }

  async search(dto: SearchStocksDto) {
    if (!dto.keyword) return [];
    const stocks = await this.prisma.stock.findMany({
      where: {
        OR: [
          { name: { contains: dto.keyword } },
          { tsCode: { contains: dto.keyword } },
        ],
      },
      take: 20,
      select: {
        tsCode: true,
        name: true,
        industry: true,
        market: true,
      },
    });
    return stocks;
  }

  async findByCode(code: string) {
    const stock = await this.prisma.stock.findUnique({
      where: { tsCode: code },
      select: {
        tsCode: true,
        name: true,
        industry: true,
        market: true,
      },
    });
    return stock;
  }
}