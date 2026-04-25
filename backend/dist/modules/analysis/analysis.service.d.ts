import { ConfigService } from '@nestjs/config';
import { PrismaService } from '../../prisma/prisma.service';
export declare class AnalysisService {
    private prisma;
    private config;
    constructor(prisma: PrismaService, config: ConfigService);
    getSignals(label?: string, limit?: number): Promise<{
        ts_code: string;
        strategy_name: string;
        label: string;
        freq: string | null;
        trade_date: string;
    }[]>;
    getProgress(): Promise<{
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
    }>;
    getProcessRecords(params: {
        ts_code?: string;
        start_date?: string;
        end_date?: string;
        signal?: string;
        strategy?: string[];
        freq?: string[];
        limit?: number;
    }): Promise<{
        id: number;
        ts_code: string;
        stock_name: string | null;
        trade_date: string;
        data: import("@prisma/client/runtime/library").JsonValue;
        created_at: string;
    }[]>;
    getProcessById(id: number): Promise<{
        id: number;
        ts_code: string;
        stock_name: string | null;
        trade_date: string;
        data: import("@prisma/client/runtime/library").JsonValue;
        created_at: string;
    } | null>;
    getStrategies(): Promise<string[]>;
    getProcessChart(tsCode: string, strategy?: string, freq?: string): Promise<any>;
    getByCode(code: string): Promise<{
        ts_code: string;
        strategy_name: string;
        label: string;
        freq: string | null;
        trade_date: string;
    } | null>;
}
