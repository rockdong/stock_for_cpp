## No Specification Changes

This change is a bug fix that corrects the implementation to match the existing expected behavior. No specification changes are required.

### Existing Behavior (Correct)

The `trade_date` in `AnalysisResult` SHOULD reflect the actual trading date of the data used for analysis.

- For daily data: the last trading day in the dataset
- For weekly data: the Friday of the last complete week
- For monthly data: the last day of the last complete month

### Bug Fixed

The code was incorrectly overriding `trade_date` with `analysisDate` (the date analysis was run), causing all frequency data to have the same incorrect date.