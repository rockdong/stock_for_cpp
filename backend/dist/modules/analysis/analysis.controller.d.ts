import { AnalysisService } from './analysis.service';
export declare class AnalysisController {
    private analysisService;
    constructor(analysisService: AnalysisService);
    getSignals(label?: string, limit?: string): Promise<{
        success: boolean;
        data: {
            ts_code: string;
            strategy_name: string;
            label: string;
            freq: string | null;
            trade_date: string;
        }[];
    }>;
    getProgress(): Promise<{
        success: boolean;
        data: {
            id: number;
            phase1: {
                status: string;
                total: number;
                completed: number;
                qualified: number;
            };
            phase2: {
                status: string;
                total: number;
                completed: number;
                failed: number;
            };
            started_at: Date | null;
            phase1_completed_at: Date | null;
            updated_at: string;
            execute_time: any;
        };
    }>;
    getProcess(ts_code?: string, start_date?: string, end_date?: string, signal?: string, strategy?: string, freq?: string, limit?: string): Promise<{
        success: boolean;
        data: {
            id: number;
            ts_code: string;
            stock_name: string | null;
            trade_date: string;
            data: import("@prisma/client/runtime/library").JsonValue;
            created_at: string;
        }[];
    }>;
    getStrategies(): Promise<{
        success: boolean;
        data: string[];
    }>;
    getProcessChart(tsCode: string, strategy?: string, freq?: string): Promise<{
        success: boolean;
        data: any;
    }>;
    getProcessById(id: string): Promise<{
        success: boolean;
        error: string;
        data?: undefined;
    } | {
        success: boolean;
        data: {
            id: number;
            ts_code: string;
            stock_name: string | null;
            trade_date: string;
            data: import("@prisma/client/runtime/library").JsonValue;
            created_at: string;
        };
        error?: undefined;
    }>;
    getByCode(code: string): Promise<{
        success: boolean;
        error: string;
        data?: undefined;
    } | {
        success: boolean;
        data: {
            ts_code: string;
            strategy_name: string;
            label: string;
            freq: string | null;
            trade_date: string;
        };
        error?: undefined;
    }>;
}
