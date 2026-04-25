import { PrismaService } from '../../prisma/prisma.service';
export declare class ChartsService {
    private prisma;
    constructor(prisma: PrismaService);
    getChartData(tsCode: string, freq?: string, limit?: number): Promise<any[]>;
}
