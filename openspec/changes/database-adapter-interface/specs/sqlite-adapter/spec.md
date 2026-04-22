## ADDED Requirements

### Requirement: SQLiteAdapter implements IDatabaseAdapter for SQLite
The system SHALL provide SQLiteAdapter class that implements IDatabaseAdapter interface for SQLite database.

#### Scenario: SQLiteAdapter identifies as SQLite
- **WHEN** developer calls getDbType()
- **THEN** SQLiteAdapter returns "sqlite"

### Requirement: SQLiteAdapter generates SQLite-compatible CREATE TABLE SQL
The system SHALL generate SQLite-specific syntax for table creation.

#### Scenario: Generate table with INTEGER PRIMARY KEY
- **WHEN** SQLiteAdapter generates SQL for column with autoIncrement=true, primaryKey=true
- **THEN** generated SQL contains "INTEGER PRIMARY KEY AUTOINCREMENT"

#### Scenario: Generate table with TEXT type
- **WHEN** SQLiteAdapter generates SQL for column with type TEXT
- **THEN** generated SQL contains "TEXT" (not VARCHAR)

#### Scenario: Generate table with CHECK constraint
- **WHEN** SQLiteAdapter generates SQL for column with checkConstraint="id = 1"
- **THEN** generated SQL contains "CHECK (id = 1)"

### Requirement: SQLiteAdapter generates SQLite-compatible ALTER TABLE SQL
The system SHALL handle ALTER TABLE limitations in SQLite.

#### Scenario: Add column without IF NOT EXISTS
- **WHEN** SQLiteAdapter.addColumn(tableName, column, ifNotExists=true) is called
- **THEN** adapter first calls columnExists() to check if column exists
- **AND** if column does not exist, generates "ALTER TABLE tableName ADD COLUMN col_def"
- **AND** if column exists, returns true without executing SQL

### Requirement: SQLiteAdapter generates SQLite-compatible CREATE INDEX SQL
The system SHALL generate SQLite-specific syntax for index creation.

#### Scenario: Create index with IF NOT EXISTS
- **WHEN** SQLiteAdapter.createIndex(index, ifNotExists=true) is called
- **THEN** generated SQL contains "CREATE INDEX IF NOT EXISTS"

### Requirement: SQLiteAdapter generates SQLite-compatible INSERT SQL
The system SHALL generate SQLite-specific syntax for insert-or-ignore.

#### Scenario: Insert or ignore
- **WHEN** SQLiteAdapter.insertOrIgnore(tableName, values) is called
- **THEN** generated SQL contains "INSERT OR IGNORE INTO"

### Requirement: SQLiteAdapter uses PRAGMA for metadata queries
The system SHALL use SQLite PRAGMA statements for checking table and column existence.

#### Scenario: Check column existence via PRAGMA
- **WHEN** SQLiteAdapter.columnExists(tableName, columnName) is called
- **THEN** adapter executes "PRAGMA table_info(tableName)"
- **AND** adapter parses result to check if columnName exists

### Requirement: SQLiteAdapter configures performance settings
The system SHALL execute SQLite PRAGMA statements for performance optimization.

#### Scenario: Configure performance on connect
- **WHEN** SQLiteAdapter.connect() succeeds
- **THEN** adapter executes:
  - PRAGMA busy_timeout = 5000
  - PRAGMA journal_mode = WAL
  - PRAGMA synchronous = NORMAL