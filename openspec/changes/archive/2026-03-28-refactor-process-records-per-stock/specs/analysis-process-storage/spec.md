## MODIFIED Requirements

### Requirement: Store analysis process data per stock
The system SHALL store one record per stock containing all strategies and frequencies data.

#### Scenario: Save complete analysis data for a stock
- **WHEN** analysis is completed for a stock
- **THEN** the system SHALL save exactly ONE record to `analysis_process_records` table
- **AND** the record SHALL contain data for ALL registered strategies
- **AND** each strategy SHALL contain data for ALL frequencies (d, w, m)

#### Scenario: Record structure
- **WHEN** a stock process record is saved
- **THEN** the `data` JSON field SHALL have the following structure:
  ```json
  {
    "strategies": [
      {
        "name": "<strategy_name>",
        "freqs": [
          {
            "freq": "d|w|m",
            "signal": "BUY|SELL|HOLD|NONE",
            "candles": [<10 ProcessDataPoint objects or empty array>]
          }
        ]
      }
    ]
  }
  ```

#### Scenario: Empty data handling
- **WHEN** a stock has no data for a specific frequency
- **THEN** the `candles` array SHALL be empty `[]`
- **AND** the `signal` SHALL be "NONE"

#### Scenario: Strategy-specific indicators
- **WHEN** a strategy calculates specific technical indicators (e.g., MACD, RSI)
- **THEN** those indicators SHALL be included in the `ProcessDataPoint` objects
- **AND** indicators not calculated by the strategy MAY be omitted

### Requirement: Unique constraint per stock per day
The system SHALL enforce uniqueness on (ts_code, trade_date).

#### Scenario: Duplicate stock on same day
- **WHEN** the same stock is analyzed multiple times on the same day
- **THEN** the system SHALL update the existing record (upsert behavior)

### Requirement: API backward compatibility
The existing API SHALL remain functional after the change.

#### Scenario: Query process records
- **WHEN** API endpoint `/api/analysis/process` is called
- **THEN** the response SHALL return an array of stock records
- **AND** each record SHALL contain the full `strategies` JSON

#### Scenario: Filter by ts_code
- **WHEN** API endpoint `/api/analysis/process?ts_code=000001.SZ` is called
- **THEN** the system SHALL return records for that specific stock

### Requirement: No duplicate records
Each stock SHALL have exactly one record per trade_date.

#### Scenario: Multiple strategies with signals
- **GIVEN** a stock has multiple strategies with different signals
- **WHEN** saving the process record
- **THEN** there SHALL be only ONE record in the database
- **AND** all strategy signals SHALL be in the JSON `data` field