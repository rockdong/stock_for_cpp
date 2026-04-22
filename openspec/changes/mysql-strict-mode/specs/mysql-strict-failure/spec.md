## ADDED Requirements

### Requirement: MySQL configuration must be strictly enforced
When `DB_TYPE` is configured as `mysql`, the system SHALL NOT silently fallback to SQLite. If MySQL support is not compiled or connection fails, the system SHALL terminate with an explicit error message.

#### Scenario: MySQL configured but not compiled
- **WHEN** `DB_TYPE=mysql` is set in environment configuration
- **AND** MySQL development library was not found during compilation (HAS_MYSQL undefined)
- **THEN** system SHALL throw `std::runtime_error` with message "MySQL support not compiled. Please install libmysqlclient-dev and recompile."
- **AND** system SHALL NOT create any SQLite database files
- **AND** system SHALL NOT proceed with analysis tasks

#### Scenario: MySQL configured and compiled but connection fails
- **WHEN** `DB_TYPE=mysql` is configured
- **AND** MySQL support is compiled (HAS_MYSQL defined)
- **AND** MySQL server connection fails (authentication error, network unreachable, database not found)
- **THEN** system SHALL log error with connection details
- **AND** system SHALL NOT fallback to SQLite
- **AND** system SHALL return failure from `initializeDatabase()` causing startup to abort

#### Scenario: SQLite configured normally
- **WHEN** `DB_TYPE=sqlite` is configured (or default)
- **THEN** system SHALL use SQLiteAdapter as normal
- **AND** system SHALL create SQLite database file at configured path
- **AND** no MySQL-related code paths SHALL be executed

### Requirement: Clear error messages for MySQL configuration issues
The system SHALL provide actionable error messages when MySQL configuration fails, guiding users to resolve the issue.

#### Scenario: Missing MySQL development library
- **WHEN** startup fails due to HAS_MYSQL undefined
- **THEN** error message SHALL include:
  - "DB_TYPE=mysql but MySQL support not compiled"
  - "Install libmysqlclient-dev (Ubuntu) or mysql-client (macOS)"
  - "Rebuild with cmake -DHAS_MYSQL=ON .."

#### Scenario: MySQL connection refused
- **WHEN** MySQL server is unreachable
- **THEN** error message SHALL include:
  - Host and port attempted
  - Original error from sqlpp11/mysql library
  - Suggestion to verify MySQL server is running and credentials are correct