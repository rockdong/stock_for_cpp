import { PrismaService } from '../../prisma/prisma.service';
import { GetStocksDto, SearchStocksDto } from './stocks.dto';
export declare class StocksService {
    private prisma;
    constructor(prisma: PrismaService);
    findAll(dto: GetStocksDto): Promise<{
        tsCode: string;
        name: string | null;
        industry: string | null;
        market: string | null;
    }[]>;
    search(dto: SearchStocksDto): Promise<{
        tsCode: string;
        name: string | null;
        industry: string | null;
        market: string | null;
    }[]>;
    findByCode(code: string): Promise<{
        tsCode: string;
        name: string | null;
        industry: string | null;
        market: string | null;
    } | null>;
}
