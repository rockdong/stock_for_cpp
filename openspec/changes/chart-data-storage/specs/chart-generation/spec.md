## ADDED Requirements

### Requirement: K-line chart generation
The system SHALL generate candlestick charts with EMA indicator overlays.

#### Scenario: Chart generation from stored data
- **WHEN** a chart is requested for a stock code
- **THEN** the system SHALL query the chart_data table for the specified stock
- **AND** the system SHALL generate a PNG image showing candlesticks with EMA17 and EMA25 lines

#### Scenario: Chart visual elements
- **WHEN** a chart is generated
- **THEN** it SHALL display candlesticks showing open, high, low, close prices
- **AND** it SHALL display EMA17 line in blue color
- **AND** it SHALL display EMA25 line in orange color
- **AND** it SHALL display exactly 10 candlesticks

#### Scenario: Chart with no data
- **WHEN** chart generation is requested for a stock without stored data
- **THEN** the system SHALL return an error message indicating no data available
- **AND** no image SHALL be generated

### Requirement: Feishu chart command
The system SHALL support chart query via Feishu messages.

#### Scenario: Valid chart command
- **WHEN** user sends "图表 000001.SZ" in Feishu
- **THEN** the system SHALL generate a chart for stock 000001.SZ
- **AND** the system SHALL send the chart image to the user

#### Scenario: Chart command with frequency
- **WHEN** user sends "图表 000001.SZ w" in Feishu
- **THEN** the system SHALL generate a weekly chart for stock 000001.SZ

#### Scenario: Invalid stock code
- **WHEN** user sends "图表 INVALID" in Feishu
- **THEN** the system SHALL respond with an error message
- **AND** the message SHALL indicate the stock code format is invalid

### Requirement: Chart image upload to Feishu
The system SHALL upload generated chart images to Feishu.

#### Scenario: Successful image upload
- **WHEN** a chart image is generated
- **THEN** the system SHALL upload the image to Feishu
- **AND** the system SHALL send the image in the same chat as the user's request

#### Scenario: Image upload failure
- **WHEN** image upload to Feishu fails
- **THEN** the system SHALL log the error
- **AND** the system SHALL send an error message to the user