## MODIFIED Requirements

### Requirement: MySQL connector must be enabled when MySQL development library is available
The system SHALL enable MySQL connector when the MySQL development library (libmysqlclient-dev) is installed on the build system, allowing the program to connect to MySQL databases when `DB_TYPE=mysql` is configured.

#### Scenario: MySQL available on Ubuntu CI environment
- **WHEN** libmysqlclient-dev is installed (Ubuntu CI runner)
- **AND** CMake configures the project with CMAKE_MODULE_PATH pointing to sqlpp11's cmake modules
- **AND** find_package(MySQL) using FindMySQL.cmake finds MySQL headers and libraries
- **THEN** MySQL_FOUND SHALL be true
- **AND** BUILD_MYSQL_CONNECTOR SHALL be set to ON
- **AND** sqlpp11::mysql target SHALL be created
- **AND** HAS_MYSQL SHALL be set to TRUE
- **AND** program SHALL be able to connect to MySQL database
- **AND** all database tables SHALL be created successfully

#### Scenario: MySQL not available on local macOS
- **WHEN** MySQL development library is not installed (macOS local development)
- **AND** CMake configures the project with CMAKE_MODULE_PATH pointing to sqlpp11's cmake modules
- **AND** find_package(MySQL) using FindMySQL.cmake does not find MySQL
- **THEN** MySQL_FOUND SHALL be false
- **AND** BUILD_MYSQL_CONNECTOR SHALL be set to OFF
- **AND** sqlpp11::mysql target SHALL NOT be created
- **AND** HAS_MYSQL SHALL be set to FALSE
- **AND** program SHALL compile successfully
- **AND** program SHALL throw exception when DB_TYPE=mysql is configured (strict failure mode)

## ADDED Requirements

### Requirement: MySQL table creation must not use DEFAULT values for JSON columns
The system SHALL NOT use DEFAULT values for JSON type columns in MySQL table creation statements, as MySQL does not allow DEFAULT values for JSON, BLOB, TEXT, or GEOMETRY type columns.

#### Scenario: Create analysis_process_records table without JSON DEFAULT
- **WHEN** MySQLConnection::createTables() creates the analysis_process_records table
- **AND** the table has a JSON column named 'data'
- **THEN** the SQL statement SHALL NOT include DEFAULT value for the JSON column
- **AND** the column SHALL be defined as `JSON NOT NULL`
- **AND** the table creation SHALL succeed

#### Scenario: Insert data into analysis_process_records with default JSON value
- **WHEN** AnalysisProcessRecordDAO inserts a new record into analysis_process_records table
- **AND** no explicit 'data' value is provided
- **THEN** the system SHALL provide the default value `'{"strategies":[]}'`
- **AND** the insert SHALL succeed