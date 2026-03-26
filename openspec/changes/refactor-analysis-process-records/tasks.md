# Tasks: Refactor Analysis Process Records

## 1. Database Migration

- [x] 1.1 Create migration file `002_refactor_analysis_process_records.sql`
- [x] 1.2 Define new table structure with `data` JSON field
- [x] 1.3 Add unique constraint `(ts_code, strategy_name, trade_date, freq)`
- [x] 1.4 Create indexes for fields (`expires_at`)
- [x] 1.5 Write data migration script

## 2. Node.js API Updates

- [x] 2.1 Update `GET /api/analysis/process` to parse `data` JSON field
- [x] 2.2 Update `POST /api/analysis/process` to accept new data format
- [x] 2.3 Update `GET /api/analysis/process/chart/:ts_code` to work with JSON array
- [x] 2.4 Add `data` field to response objects

## 3. Web Frontend Updates

- [x] 3.1 Update `types/analysis.ts` with new `AnalysisProcessRecord` interface
- [x] 3.2 Add `DataPoint` interface for JSON array items
- [x] 3.3 Update `RecordTable.tsx` to display data
- [x] 3.4 Update `AnalysisFilter.tsx`
- [x] 3.5 Update `CandlestickChart.tsx` to work with new data format
- [x] 3.6 Add `ProgressBadge` component for progress display
- [x] 3.7 Add `getProgress` API method

## 4. C++ DAO Updates (Optional)

- [x] 4.1 Skipped - using Node.js API for writes

## 5. Testing & Verification

- [x] 5.1 Verify migration script
- [x] 5.2 Verify API returns correct JSON structure
- [x] 5.3 Test frontend displays correctly
- [x] 5.4 Test chart renders with new data format
- [x] 5.5 Test progress badge displays correctly

## 6. Documentation

- [x] 6.1 Update API documentation
- [x] 6.2 Update database schema documentation
- [x] 6.3 Add migration guide for existing deployments