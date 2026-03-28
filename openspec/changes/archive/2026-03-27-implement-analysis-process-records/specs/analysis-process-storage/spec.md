## ADDED Requirements

### Requirement: Store analysis process data
The system SHALL store detailed analysis process data when a trading signal is detected, including the last 10 periods of K-line data and technical indicators.

#### Scenario: Save process data on buy signal
- **WHEN** a strategy detects a BUY signal
- **THEN** the system SHALL save a record to `analysis_process_records` table containing:
  - ts_code (stock code)
  - stock_name
  - strategy_name
  - trade_date
  - freq (period: d/w/m)
  - signal (BUY/SELL/HOLD/NONE)
  - data (JSON array with 10 periods of K-line and indicators)

#### Scenario: Save process data on sell signal
- **WHEN** a strategy detects a SELL signal
- **THEN** the system SHALL save a record to `analysis_process_records` table with the same structure as BUY signal

#### Scenario: Upsert on duplicate
- **WHEN** the same (ts_code, strategy_name, trade_date, freq) combination already exists
- **THEN** the system SHALL update the existing record instead of creating a duplicate

### Requirement: Process data JSON structure
The `data` field SHALL contain a JSON array with up to 10 data points, each containing K-line and technical indicator values.

#### Scenario: Valid JSON structure
- **WHEN** process data is saved
- **THEN** the `data` JSON array SHALL contain objects with the following fields:
  - time (string, YYYY-MM-DD format)
  - open, high, low, close (numbers)
  - volume (integer)
  - ema17, ema25, macd, rsi (optional numbers, only if calculated by strategy)

#### Scenario: Less than 10 periods available
- **WHEN** less than 10 periods of historical data are available
- **THEN** the system SHALL save all available periods without padding

### Requirement: Query process records
The system SHALL support querying process records by various filters.

#### Scenario: Query by stock code
- **WHEN** API endpoint `/api/analysis/process?ts_code=000001.SZ` is called
- **THEN** the system SHALL return all process records for that stock

#### Scenario: Query by strategy
- **WHEN** API endpoint `/api/analysis/process?strategy=EMA17TO25` is called
- **THEN** the system SHALL return all process records for that strategy

#### Scenario: Query by signal type
- **WHEN** API endpoint `/api/analysis/process?signal=BUY` is called
- **THEN** the system SHALL return all process records with BUY signal

#### Scenario: Query by date range
- **WHEN** API endpoint `/api/analysis/process?start_date=20260301&end_date=20260327` is called
- **THEN** the system SHALL return all process records within that date range

### Requirement: Automatic expiration
Records SHALL automatically expire after 7 days.

#### Scenario: Auto-expire old records
- **WHEN** a record is created
- **THEN** the `expires_at` field SHALL be set to 7 days from creation time
- **AND** the database trigger SHALL manage the expiration timestamp

### Requirement: No impact on analysis performance
The system SHALL NOT significantly degrade analysis performance when saving process data.

#### Scenario: Performance benchmark
- **WHEN** saving process data for 1000 stocks
- **THEN** the total additional time SHALL NOT exceed 5% of total analysis time