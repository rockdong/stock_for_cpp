import { ChartsService } from './charts.service';
export declare class ChartsController {
    private chartsService;
    constructor(chartsService: ChartsService);
    getChartData(code: string, freq?: string, limit?: string): Promise<{
        success: boolean;
        data: any[];
    }>;
}
