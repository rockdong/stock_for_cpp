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
exports.StocksController = void 0;
const common_1 = require("@nestjs/common");
const stocks_service_1 = require("./stocks.service");
const stocks_dto_1 = require("./stocks.dto");
let StocksController = class StocksController {
    constructor(stocksService) {
        this.stocksService = stocksService;
    }
    async findAll(dto) {
        const data = await this.stocksService.findAll(dto);
        return { success: true, data };
    }
    async search(dto) {
        const data = await this.stocksService.search(dto);
        return { success: true, data };
    }
    async findByCode(code) {
        const data = await this.stocksService.findByCode(code);
        if (!data)
            return { success: false, error: '股票不存在' };
        return { success: true, data };
    }
};
exports.StocksController = StocksController;
__decorate([
    (0, common_1.Get)(),
    __param(0, (0, common_1.Query)()),
    __metadata("design:type", Function),
    __metadata("design:paramtypes", [stocks_dto_1.GetStocksDto]),
    __metadata("design:returntype", Promise)
], StocksController.prototype, "findAll", null);
__decorate([
    (0, common_1.Get)('search'),
    __param(0, (0, common_1.Query)()),
    __metadata("design:type", Function),
    __metadata("design:paramtypes", [stocks_dto_1.SearchStocksDto]),
    __metadata("design:returntype", Promise)
], StocksController.prototype, "search", null);
__decorate([
    (0, common_1.Get)(':code'),
    __param(0, (0, common_1.Param)('code')),
    __metadata("design:type", Function),
    __metadata("design:paramtypes", [String]),
    __metadata("design:returntype", Promise)
], StocksController.prototype, "findByCode", null);
exports.StocksController = StocksController = __decorate([
    (0, common_1.Controller)('api/stocks'),
    __metadata("design:paramtypes", [stocks_service_1.StocksService])
], StocksController);
//# sourceMappingURL=stocks.controller.js.map