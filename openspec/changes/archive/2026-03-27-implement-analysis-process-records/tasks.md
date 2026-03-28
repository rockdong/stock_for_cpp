## 1. Data Model

- [x] 1.1 Create `AnalysisProcessRecord.h` with `ProcessDataPoint` and `AnalysisProcessRecord` structs
- [x] 1.2 Implement JSON serialization using nlohmann/json (`to_json`/`from_json` functions)
- [x] 1.3 Create `AnalysisProcessRecordTable.h` for sqlpp11 table definition

## 2. DAO Layer

- [x] 2.1 Create `AnalysisProcessRecordDAO.h` with interface declaration
- [x] 2.2 Implement `insert()` method for inserting new records
- [x] 2.3 Implement `upsert()` method using INSERT OR REPLACE
- [x] 2.4 Implement `findByTsCode()` method
- [x] 2.5 Implement `findByStrategy()` method
- [x] 2.6 Implement `findLatest()` method for single record query
- [x] 2.7 Add include to `Data.h` for unified header

## 3. Strategy Integration

- [x] 3.1 Modify `main.cpp` to create `AnalysisProcessRecordDAO` instance
- [x] 3.2 Create helper function `extractProcessData()` to extract last N periods
- [x] 3.3 Integrate process record saving in analysis workflow (after signal detection)
- [x] 3.4 Add stock_name lookup from stocks table

## 4. Build System

- [x] 4.1 Update `CMakeLists.txt` to include new source files
- [x] 4.2 Ensure nlohmann/json dependency is available

## 5. Testing & Verification

- [x] 5.1 Compile C++ project locally
- [x] 5.2 Run analysis for test stock and verify record is saved
- [x] 5.3 Query `/api/analysis/process` API to verify data is retrievable
- [x] 5.4 Test with Docker container (rebuild and restart)
- [x] 5.5 Verify Web frontend displays process data

## 6. Cleanup

- [x] 6.1 Commit changes with conventional commit message
- [x] 6.2 Archive OpenSpec change