## ADDED Requirements

### Requirement: Idempotent opt/freq insert
The AnalysisResultDAO.insert() method SHALL ensure that opt and freq fields are idempotent when inserting analysis results with the same (ts_code, trade_date, strategy_name) key.

#### Scenario: First insert creates new record
- **WHEN** inserting an analysis result with a new (ts_code, trade_date, strategy_name) combination
- **THEN** a new record SHALL be created with opt and freq fields set to the provided values

#### Scenario: Duplicate insert does not change data
- **WHEN** inserting an analysis result with an existing (ts_code, trade_date, strategy_name) combination
- **AND** the opt and freq values are already present in the record
- **THEN** the record SHALL remain unchanged
- **AND** no database update SHALL be executed

#### Scenario: New frequency appends to existing record
- **WHEN** inserting an analysis result with an existing (ts_code, trade_date, strategy_name) combination
- **AND** the opt or freq value is NOT already present in the record
- **THEN** the new value SHALL be appended to the existing values using "|" as separator
- **AND** duplicate values SHALL NOT be added

#### Scenario: Empty string handling
- **WHEN** the existing opt or freq field is empty or null
- **THEN** the new value SHALL be set directly without separator

### Requirement: String utility functions
The StringUtil utility class SHALL provide split and join functions for string manipulation.

#### Scenario: Split string by delimiter
- **WHEN** calling split("a|b|c", "|")
- **THEN** a set containing {"a", "b", "c"} SHALL be returned

#### Scenario: Split empty string
- **WHEN** calling split("", "|")
- **THEN** an empty set SHALL be returned

#### Scenario: Join set with delimiter
- **WHEN** calling join({"a", "b", "c"}, "|")
- **THEN** a string with elements joined by "|" SHALL be returned