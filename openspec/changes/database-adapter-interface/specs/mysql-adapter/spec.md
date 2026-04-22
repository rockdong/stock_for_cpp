## ADDED Requirements

### Requirement: MySQLAdapter implements IDatabaseAdapter for MySQL
The system SHALL provide MySQLAdapter class that implements IDatabaseAdapter interface for MySQL database.

#### Scenario: MySQLAdapter identifies as MySQL
- **WHEN** developer calls getDbType()
- **THEN** MySQLAdapter returns "mysql"

### Requirement: MySQLAdapter generates MySQL-compatible CREATE TABLE SQL
The system SHALL generate MySQL-specific syntax for table creation.

#### Scenario: Generate table with INT AUTO_INCREMENT
- **WHEN** MySQLAdapter generates SQL for column with autoIncrement=true, primaryKey=true
- **THEN** generated SQL contains "INT AUTO_INCREMENT PRIMARY KEY"

#### Scenario: Generate table with VARCHAR type
- **WHEN** MySQLAdapter generates SQL for column with type TEXT and varcharLength=20
- **THEN** generated SQL contains "VARCHAR(20)"

#### Scenario: Generate table with TIMESTAMP
- **WHEN** MySQLAdapter generates SQL for column with type TIMESTAMP
- **THEN** generated SQL contains "TIMESTAMP DEFAULT CURRENT_TIMESTAMP"

#### Scenario: Generate table with engine and charset
- **WHEN** MySQLAdapter generates CREATE TABLE SQL
- **THEN** SQL ends with "ENGINE=InnoDB DEFAULT CHARSET=utf8mb4"

### Requirement: MySQLAdapter generates MySQL-compatible ALTER TABLE SQL
The system SHALL use MySQL-specific IF NOT EXISTS syntax for ALTER TABLE.

#### Scenario: Add column with IF NOT EXISTS
- **WHEN** MySQLAdapter.addColumn(tableName, column, ifNotExists=true) is called
- **THEN** generated SQL contains "ALTER TABLE tableName ADD COLUMN IF NOT EXISTS col_def"

### Requirement: MySQLAdapter generates MySQL-compatible CREATE INDEX SQL
The system SHALL handle CREATE INDEX limitations in MySQL.

#### Scenario: Create index without IF NOT EXISTS
- **WHEN** MySQLAdapter.createIndex(index, ifNotExists=true) is called
- **THEN** adapter first calls indexExists() to check if index exists
- **AND** if index does not exist, generates "CREATE INDEX indexName ON tableName(columns)"
- **AND** if index exists, returns true without executing SQL

### Requirement: MySQLAdapter generates MySQL-compatible INSERT SQL
The system SHALL generate MySQL-specific syntax for insert-or-ignore.

#### Scenario: Insert ignore
- **WHEN** MySQLAdapter.insertOrIgnore(tableName, values) is called
- **THEN** generated SQL contains "INSERT IGNORE INTO"

### Requirement: MySQLAdapter uses INFORMATION_SCHEMA for metadata queries
The system SHALL use MySQL INFORMATION_SCHEMA for checking table and column existence.

#### Scenario: Check column existence via INFORMATION_SCHEMA
- **WHEN** MySQLAdapter.columnExists(tableName, columnName) is called
- **THEN** adapter executes "SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = tableName AND COLUMN_NAME = columnName"
- **AND** adapter returns true if result is non-empty

#### Scenario: Check index existence via INFORMATION_SCHEMA
- **WHEN** MySQLAdapter.indexExists(indexName) is called
- **THEN** adapter executes "SELECT INDEX_NAME FROM INFORMATION_SCHEMA.STATISTICS WHERE INDEX_NAME = indexName"
- **AND** adapter returns true if result is non-empty

### Requirement: MySQLAdapter supports JSON type
The system SHALL generate MySQL JSON type for JSON columns.

#### Scenario: Generate JSON column
- **WHEN** MySQLAdapter generates SQL for column with type JSON
- **THEN** generated SQL contains "JSON"