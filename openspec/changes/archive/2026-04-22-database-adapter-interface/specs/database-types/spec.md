## ADDED Requirements

### Requirement: DataType enumeration defines database column types
The system SHALL provide a DataType enumeration that represents abstract database column types independent of specific database implementations.

#### Scenario: DataType values cover common database types
- **WHEN** developer uses DataType enumeration
- **THEN** the following values are available: TEXT, INTEGER, DECIMAL, DATETIME, TIMESTAMP, DATE, JSON, BOOLEAN

### Requirement: ColumnDefinition structure defines column properties
The system SHALL provide a ColumnDefinition structure that encapsulates all properties needed to define a database column.

#### Scenario: ColumnDefinition contains all column properties
- **WHEN** developer creates a ColumnDefinition instance
- **THEN** the structure contains: name, type, precision, scale, varcharLength, nullable, defaultValue, autoIncrement, primaryKey, unique, checkConstraint

### Requirement: ColumnDefinition supports DECIMAL precision
The system SHALL allow specifying precision and scale for DECIMAL type columns.

#### Scenario: Define DECIMAL column with precision
- **WHEN** developer creates ColumnDefinition with type DECIMAL and precision=18, scale=4
- **THEN** SQLite adapter generates "REAL" type
- **AND** MySQL adapter generates "DECIMAL(18,4)" type

### Requirement: ColumnDefinition supports VARCHAR length
The system SHALL allow specifying maximum length for text columns.

#### Scenario: Define text column with length limit
- **WHEN** developer creates ColumnDefinition with type TEXT and varcharLength=20
- **THEN** SQLite adapter generates "TEXT" type
- **AND** MySQL adapter generates "VARCHAR(20)" type

### Requirement: IndexDefinition structure defines index properties
The system SHALL provide an IndexDefinition structure that encapsulates all properties needed to define a database index.

#### Scenario: IndexDefinition contains all index properties
- **WHEN** developer creates an IndexDefinition instance
- **THEN** the structure contains: name, tableName, columns, unique

### Requirement: TableDefinition structure defines complete table
The system SHALL provide a TableDefinition structure that combines table name with its column definitions.

#### Scenario: TableDefinition combines name and columns
- **WHEN** developer creates a TableDefinition instance
- **THEN** the structure contains: name (string), columns (vector of ColumnDefinition)