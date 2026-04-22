## ADDED Requirements

### Requirement: Update script displays current image versions
The update script SHALL display the current Docker image versions (image ID and creation time) for all project containers before starting the update process.

#### Scenario: Display current versions before update
- **WHEN** user executes `./update.sh`
- **THEN** script outputs a table listing all rockdong13 images with their current image IDs and creation timestamps

### Requirement: Update script stops services gracefully
The update script SHALL stop all Docker Compose services using `docker-compose down` command, preserving MySQL data volumes.

#### Scenario: Services stop without data loss
- **WHEN** update script executes `docker-compose down`
- **THEN** all containers stop gracefully
- **AND** mysql_data volume is preserved (not deleted)

### Requirement: Update script pulls latest images
The update script SHALL pull the latest Docker images from Docker Hub using `docker-compose pull` command.

#### Scenario: Pull latest images successfully
- **WHEN** Docker Hub is reachable
- **THEN** script pulls latest versions of all images defined in docker-compose.yml
- **AND** outputs download progress for each image

#### Scenario: Pull fails gracefully
- **WHEN** Docker Hub connection fails or image does not exist
- **THEN** script aborts with error message
- **AND** existing images remain intact for continued operation

### Requirement: Update script displays version changes
The update script SHALL display a comparison of image versions before and after the update, highlighting which images changed.

#### Scenario: Show version comparison
- **WHEN** images are pulled successfully
- **THEN** script outputs a table showing old vs new image IDs
- **AND** marks images with changes (e.g., "CHANGED" or "SAME")

### Requirement: Update script starts services with health check
The update script SHALL start all services using `docker-compose up -d` and wait for MySQL health check to pass before completing.

#### Scenario: Services start with health verification
- **WHEN** `docker-compose up -d` executes
- **THEN** script waits for MySQL container to report healthy status
- **AND** waits up to 90 seconds for health check
- **AND** outputs final container status table

### Requirement: Update script cleans dangling images
The update script SHALL execute `docker image prune -f` after successful startup to remove dangling (unused) images.

#### Scenario: Clean unused images
- **WHEN** all services start successfully
- **THEN** script removes dangling images
- **AND** outputs number of images removed

### Requirement: Update script starts Docker monitor
The update script SHALL start the Docker events monitor if `~/scripts/docker-monitor.sh` exists.

#### Scenario: Start monitor when script exists
- **WHEN** `~/scripts/docker-monitor.sh` file exists
- **THEN** script executes `~/scripts/docker-monitor.sh start`
- **AND** outputs monitor start status

#### Scenario: Skip monitor when script missing
- **WHEN** `~/scripts/docker-monitor.sh` file does not exist
- **THEN** script outputs "监控脚本未找到，跳过" message
- **AND** continues without error

### Requirement: Update script outputs detailed logs with timestamps
The update script SHALL output detailed execution logs with timestamps for each major step.

#### Scenario: Timestamped output
- **WHEN** script executes each major step
- **THEN** output includes `[YYYY-MM-DD HH:MM:SS]` timestamp prefix
- **AND** describes the action being performed

### Requirement: Update script displays access information
The update script SHALL display correct service access URLs after completion.

#### Scenario: Display correct ports
- **WHEN** update completes successfully
- **THEN** script outputs:
  - 飞书 Bot: http://localhost:3000
  - REST API: http://localhost:3001
  - Web前端: http://localhost:80
  - Adminer: http://localhost:8080