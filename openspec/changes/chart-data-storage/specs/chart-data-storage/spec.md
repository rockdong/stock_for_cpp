## ADDED Requirements

### Requirement: Chart data storage table
The system SHALL provide a database table for storing chart data with EMA indicators.

#### Scenario: Table creation on startup
- **WHEN** the C++ application starts
- **THEN** the chart_data table SHALL be created if it does not exist

#### Scenario: Table structure validation
- **WHEN** examining the chart_data table schema
- **THEN** it SHALL contain columns: id, ts_code, freq, analysis_date, data, updated_at
- **AND** it SHALL have a unique constraint on (ts_code, freq, analysis_date)

### Requirement: Chart data storage during analysis
The system SHALL store chart data when performing stock analysis.

#### Scenario: Daily chart data storage
- **WHEN** analysis is performed for a stock with daily frequency
- **THEN** the system SHALL calculate EMA17 and EMA25 indicators
- **AND** the system SHALL store the last 10 candlesticks with OHLC and EMA values as JSON
- **AND** the data field SHALL contain trade_date, open, high, low, close, volume, ema17, ema25

#### Scenario: Multiple frequency support
- **WHEN** analysis is performed for weekly or monthly frequency
- **THEN** the system SHALL store chart data with freq value 'w' or 'm'
- **AND** each frequency SHALL have a separate record

#### Scenario: Analysis date tracking
- **WHEN** chart data is stored
- **THEN** the analysis_date field SHALL record the date when analysis was performed
- **AND** each stock-frequency-analysis_date combination SHALL be unique

### Requirement: Chart data query interface
The system SHALL provide a query interface for retrieving stored chart data.

#### Scenario: Query by stock code and frequency
- **WHEN** chart data is requested for a specific stock and frequency
- **THEN** the system SHALL return the most recent record matching the criteria
- **AND** the data field SHALL be a valid JSON array of 10 candlesticks

#### Scenario: Query with no data available
- **WHEN** chart data is requested for a stock that has not been analyzed
- **THEN** the system SHALL return an empty result
- **AND** no error SHALL be raised