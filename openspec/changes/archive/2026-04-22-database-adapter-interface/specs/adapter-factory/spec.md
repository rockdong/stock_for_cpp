## ADDED Requirements

### Requirement: AdapterFactory creates appropriate adapter based on configuration
The system SHALL provide AdapterFactory class that creates the correct database adapter based on configuration.

#### Scenario: Create SQLite adapter
- **WHEN** configuration specifies DB_TYPE=sqlite or dbPath is provided
- **THEN** AdapterFactory.create() returns SQLiteAdapter instance

#### Scenario: Create MySQL adapter
- **WHEN** configuration specifies DB_TYPE=mysql with host/port/user/password
- **THEN** AdapterFactory.create() returns MySQLAdapter instance

### Requirement: AdapterFactory integrates with ConnectionFactory
The system SHALL allow AdapterFactory to work with existing ConnectionFactory.

#### Scenario: AdapterFactory receives connection parameters
- **WHEN** developer calls AdapterFactory.create(connectionConfig)
- **THEN** factory reads DB_TYPE from connectionConfig
- **AND** creates appropriate adapter initialized with connection parameters

### Requirement: AdapterFactory supports default configuration
The system SHALL provide default adapter creation without explicit configuration.

#### Scenario: Default to SQLite adapter
- **WHEN** no configuration is provided
- **THEN** AdapterFactory.create() returns SQLiteAdapter with default dbPath="stock_db.db"