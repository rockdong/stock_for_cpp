"use strict";
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.AnalysisService = void 0;
const common_1 = require("@nestjs/common");
const config_1 = require("@nestjs/config");
const prisma_service_1 = require("../../prisma/prisma.service");
let AnalysisService = class AnalysisService {
    constructor(prisma, config) {
        this.prisma = prisma;
        this.config = config;
    }
    async getSignals(label, limit = 50) {
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
    async getProcessRecords(params) {
        const where = {};
        if (params.ts_code)
            where.tsCode = params.ts_code;
        if (params.start_date)
            where.tradeDate = { ...where.tradeDate, gte: new Date(params.start_date) };
        if (params.end_date)
            where.tradeDate = { ...where.tradeDate, lte: new Date(params.end_date) };
        const fetchLimit = (params.signal || params.strategy)
            ? Math.max((params.limit || 100) * 5, 500)
            : params.limit || 100;
        const records = await this.prisma.analysisProcessRecord.findMany({
            where,
            orderBy: [{ tradeDate: 'desc' }, { createdAt: 'desc' }],
            take: fetchLimit,
        });
        let parsedRecords = records.map(r => ({
            id: r.id,
            ts_code: r.tsCode,
            stock_name: r.stockName,
            trade_date: r.tradeDate.toISOString().split('T')[0],
            data: r.data,
            created_at: r.createdAt.toISOString(),
        }));
        if (params.signal) {
            parsedRecords = parsedRecords.filter(record => {
                const data = record.data;
                const strategies = data?.strategies || [];
                return strategies.some((s) => (s.freqs || []).some((f) => f.signal === params.signal));
            });
        }
        if (params.strategy && params.strategy.length > 0) {
            parsedRecords = parsedRecords.filter(record => {
                const data = record.data;
                const strategies = data?.strategies || [];
                return strategies.some((s) => params.strategy.includes(s.name));
            });
        }
        if (params.freq && params.freq.length > 0) {
            parsedRecords = parsedRecords.filter(record => {
                const data = record.data;
                const strategies = data?.strategies || [];
                return strategies.some((s) => (s.freqs || []).some((f) => params.freq.includes(f.freq)));
            });
        }
        return parsedRecords.slice(0, params.limit || 100);
    }
    async getProcessById(id) {
        const record = await this.prisma.analysisProcessRecord.findUnique({
            where: { id },
        });
        if (!record)
            return null;
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
        const records = await this.prisma.analysisProcessRecord.findMany({
            take: 100,
            select: { data: true },
        });
        const strategyNames = new Set();
        for (const record of records) {
            const data = record.data;
            if (data?.strategies) {
                for (const s of data.strategies) {
                    if (s.name)
                        strategyNames.add(s.name);
                }
            }
        }
        return Array.from(strategyNames);
    }
    async getProcessChart(tsCode, strategy, freq = 'd') {
        const record = await this.prisma.analysisProcessRecord.findFirst({
            where: { tsCode },
            orderBy: { tradeDate: 'desc' },
        });
        if (!record)
            return [];
        const data = record.data;
        if (!data.strategies)
            return [];
        for (const s of data.strategies) {
            if (strategy && !s.name.includes(strategy))
                continue;
            for (const f of s.freqs || []) {
                if (f.freq === freq) {
                    return f.candles || [];
                }
            }
        }
        return [];
    }
    async getByCode(code) {
        const result = await this.prisma.analysisResult.findFirst({
            where: { tsCode: code },
            orderBy: { tradeDate: 'desc' },
        });
        if (!result)
            return null;
        return {
            ts_code: result.tsCode,
            strategy_name: result.strategyName,
            label: result.label,
            freq: result.freq,
            trade_date: result.tradeDate.toISOString().split('T')[0],
        };
    }
};
exports.AnalysisService = AnalysisService;
exports.AnalysisService = AnalysisService = __decorate([
    (0, common_1.Injectable)(),
    __metadata("design:paramtypes", [prisma_service_1.PrismaService,
        config_1.ConfigService])
], AnalysisService);
//# sourceMappingURL=analysis.service.js.map