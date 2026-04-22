## ADDED Requirements

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