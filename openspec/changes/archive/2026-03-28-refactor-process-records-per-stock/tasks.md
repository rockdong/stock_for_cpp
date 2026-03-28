## 1. Data Model Update

- [ ] 1.1 Add `StrategyFreqData` struct to `AnalysisProcessRecord.h`
- [ ] 1.2 Add `StrategyData` struct to `AnalysisProcessRecord.h`
- [ ] 1.3 Rename `AnalysisProcessRecord` to `StockProcessRecord` with new structure
- [ ] 1.4 Update JSON serialization for nested structures
- [ ] 1.5 Update `AnalysisProcessRecord.cpp` with new serialization logic

## 2. Database Schema

- [ ] 2.1 Create migration script to drop and recreate table
- [ ] 2.2 Update `AnalysisProcessRecordTable.h` - remove strategy_name, freq, signal columns
- [ ] 2.3 Update UNIQUE constraint to (ts_code, trade_date)

## 3. DAO Layer

- [ ] 3.1 Update `AnalysisProcessRecordDAO.h` with new method signatures
- [ ] 3.2 Rewrite `upsert()` for new structure
- [ ] 3.3 Update `findByTsCode()` to return single record
- [ ] 3.4 Add `findAll()` with pagination support

## 4. Main Analysis Logic

- [ ] 4.1 Refactor `analyzeStock()` to collect all strategy/freq data first
- [ ] 4.2 Create `buildStockProcessRecord()` helper function
- [ ] 4.3 Implement per-strategy indicator calculation
- [ ] 4.4 Remove old per-signal saving logic
- [ ] 4.5 Add single upsert call at end of stock analysis

## 5. API Layer

- [ ] 5.1 Update Node.js API to handle new JSON structure
- [ ] 5.2 Ensure backward compatibility for existing consumers
- [ ] 5.3 Update TypeScript types in web-frontend

## 6. Testing & Verification

- [ ] 6.1 Compile C++ project locally
- [ ] 6.2 Test with single stock analysis
- [ ] 6.3 Verify JSON structure in database
- [ ] 6.4 Test API endpoint returns correct data
- [ ] 6.5 Build and deploy to Docker container
- [ ] 6.6 Verify Web frontend displays data correctly

## 7. Cleanup

- [ ] 7.1 Commit changes with conventional commit message
- [ ] 7.2 Archive OpenSpec change