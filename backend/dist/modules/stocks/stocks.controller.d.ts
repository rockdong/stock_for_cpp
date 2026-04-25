import { StocksService } from './stocks.service';
import { GetStocksDto, SearchStocksDto } from './stocks.dto';
export declare class StocksController {
    private stocksService;
    constructor(stocksService: StocksService);
    findAll(dto: GetStocksDto): Promise<{
        success: boolean;
        data: {
            tsCode: string;
            name: string | null;
            industry: string | null;
            market: string | null;
        }[];
    }>;
    search(dto: SearchStocksDto): Promise<{
        success: boolean;
        data: {
            tsCode: string;
            name: string | null;
            industry: string | null;
            market: string | null;
        }[];
    }>;
    findByCode(code: string): Promise<{
        success: boolean;
        error: string;
        data?: undefined;
    } | {
        success: boolean;
        data: {
            tsCode: string;
            name: string | null;
            industry: string | null;
            market: string | null;
        };
        error?: undefined;
    }>;
}
