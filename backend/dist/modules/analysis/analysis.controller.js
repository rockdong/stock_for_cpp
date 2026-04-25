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
var __param = (this && this.__param) || function (paramIndex, decorator) {
    return function (target, key) { decorator(target, key, paramIndex); }
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.AnalysisController = void 0;
const common_1 = require("@nestjs/common");
const analysis_service_1 = require("./analysis.service");
let AnalysisController = class AnalysisController {
    constructor(analysisService) {
        this.analysisService = analysisService;
    }
    async getSignals(label, limit) {
        const data = await this.analysisService.getSignals(label, parseInt(limit || '50'));
        return { success: true, data };
    }
    async getProgress() {
        const data = await this.analysisService.getProgress();
        return { success: true, data };
    }
    async getProcess(ts_code, start_date, end_date, signal, strategy, freq, limit) {
        const freqArray = freq ? freq.split(',') : undefined;
        const strategyArray = strategy ? strategy.split(',') : undefined;
        const data = await this.analysisService.getProcessRecords({
            ts_code,
            start_date,
            end_date,
            signal,
            strategy: strategyArray,
            freq: freqArray,
            limit: parseInt(limit || '100'),
        });
        return { success: true, data };
    }
    async getStrategies() {
        const data = await this.analysisService.getStrategies();
        return { success: true, data };
    }
    async getProcessChart(tsCode, strategy, freq) {
        const data = await this.analysisService.getProcessChart(tsCode, strategy, freq || 'd');
        return { success: true, data };
    }
    async getProcessById(id) {
        const data = await this.analysisService.getProcessById(parseInt(id));
        if (!data)
            return { success: false, error: '记录不存在' };
        return { success: true, data };
    }
    async getByCode(code) {
        const data = await this.analysisService.getByCode(code);
        if (!data)
            return { success: false, error: '无分析结果' };
        return { success: true, data };
    }
};
exports.AnalysisController = AnalysisController;
__decorate([
    (0, common_1.Get)('signals'),
    __param(0, (0, common_1.Query)('label')),
    __param(1, (0, common_1.Query)('limit')),
    __metadata("design:type", Function),
    __metadata("design:paramtypes", [String, String]),
    __metadata("design:returntype", Promise)
], AnalysisController.prototype, "getSignals", null);
__decorate([
    (0, common_1.Get)('progress'),
    __metadata("design:type", Function),
    __metadata("design:paramtypes", []),
    __metadata("design:returntype", Promise)
], AnalysisController.prototype, "getProgress", null);
__decorate([
    (0, common_1.Get)('process'),
    __param(0, (0, common_1.Query)('ts_code')),
    __param(1, (0, common_1.Query)('start_date')),
    __param(2, (0, common_1.Query)('end_date')),
    __param(3, (0, common_1.Query)('signal')),
    __param(4, (0, common_1.Query)('strategy')),
    __param(5, (0, common_1.Query)('freq')),
    __param(6, (0, common_1.Query)('limit')),
    __metadata("design:type", Function),
    __metadata("design:paramtypes", [String, String, String, String, String, String, String]),
    __metadata("design:returntype", Promise)
], AnalysisController.prototype, "getProcess", null);
__decorate([
    (0, common_1.Get)('process/strategies'),
    __metadata("design:type", Function),
    __metadata("design:paramtypes", []),
    __metadata("design:returntype", Promise)
], AnalysisController.prototype, "getStrategies", null);
__decorate([
    (0, common_1.Get)('process/chart/:tsCode'),
    __param(0, (0, common_1.Param)('tsCode')),
    __param(1, (0, common_1.Query)('strategy')),
    __param(2, (0, common_1.Query)('freq')),
    __metadata("design:type", Function),
    __metadata("design:paramtypes", [String, String, String]),
    __metadata("design:returntype", Promise)
], AnalysisController.prototype, "getProcessChart", null);
__decorate([
    (0, common_1.Get)('process/:id'),
    __param(0, (0, common_1.Param)('id')),
    __metadata("design:type", Function),
    __metadata("design:paramtypes", [String]),
    __metadata("design:returntype", Promise)
], AnalysisController.prototype, "getProcessById", null);
__decorate([
    (0, common_1.Get)(':code'),
    __param(0, (0, common_1.Param)('code')),
    __metadata("design:type", Function),
    __metadata("design:paramtypes", [String]),
    __metadata("design:returntype", Promise)
], AnalysisController.prototype, "getByCode", null);
exports.AnalysisController = AnalysisController = __decorate([
    (0, common_1.Controller)('api/analysis'),
    __metadata("design:paramtypes", [analysis_service_1.AnalysisService])
], AnalysisController);
//# sourceMappingURL=analysis.controller.js.map