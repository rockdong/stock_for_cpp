#ifndef CORE_CORE_H
#define CORE_CORE_H

#include "Stock.h"
#include "AnalysisResult.h"
#include "FundamentalData.h"

#include "Strategy.h"
#include "StrategyFactory.h"
#include "StrategyManager.h"

#include "PredictionHistory.h"
#include "AccuracyAnalyzer.h"
#include "ParameterOptimizer.h"

#include "strategies/MACrossStrategy.h"
#include "strategies/MACDStrategy.h"
#include "strategies/RSIStrategy.h"
#include "strategies/BOLLStrategy.h"
#include "strategies/GridStrategy.h"
#include "strategies/EMA17TO25Strategy.h"
#include "strategies/EMA17BreakoutStrategy.h"
#include "strategies/EMAConvergenceStrategy.h"

#endif // CORE_CORE_H
