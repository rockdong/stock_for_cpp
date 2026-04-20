import { Module } from '@nestjs/common';
import { ConfigModule } from '@nestjs/config';
import { PrismaModule } from './prisma/prisma.module';
import { StocksModule } from './modules/stocks/stocks.module';
import { AnalysisModule } from './modules/analysis/analysis.module';
import { ChartsModule } from './modules/charts/charts.module';

@Module({
  imports: [
    ConfigModule.forRoot({ isGlobal: true }),
    PrismaModule,
    StocksModule,
    AnalysisModule,
    ChartsModule,
  ],
})
export class AppModule {}