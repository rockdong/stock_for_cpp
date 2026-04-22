## MODIFIED Requirements

### Requirement: MySQL connector must be enabled when MySQL development library is available
The system SHALL enable MySQL connector when the MySQL development library (libmysqlclient-dev) is installed on the build system, allowing the program to connect to MySQL databases when `DB_TYPE=mysql` is configured.

#### Scenario: MySQL available on Ubuntu CI environment
- **WHEN** libmysqlclient-dev is installed (Ubuntu CI runner)
- **AND** CMake configures the project with BUILD_MYSQL_CONNECTOR=ON
- **THEN** sqlpp11's FindMySQL.cmake SHALL locate MySQL headers and libraries
- **AND** sqlpp11::mysql target SHALL be created
- **AND** HAS_MYSQL SHALL be set to TRUE
- **AND** program SHALL be able to connect to MySQL database

#### Scenario: MySQL not available on local macOS
- **WHEN** MySQL development library is not installed (macOS local development)
- **AND** CMake configures the project with BUILD_MYSQL_CONNECTOR=ON
- **AND** DEPENDENCY_CHECK=OFF (non-strict mode)
- **THEN** CMake configuration SHALL succeed (not fail)
- **AND** sqlpp11::mysql target SHALL NOT be created
- **AND** HAS_MYSQL SHALL be set to FALSE
- **AND** program SHALL throw exception when DB_TYPE=mysql is configured (strict failure mode)