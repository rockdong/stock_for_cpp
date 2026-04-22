## ADDED Requirements

### Requirement: Restart script uses error handling
The restart script SHALL use `set -e` to abort execution on any command failure.

#### Scenario: Abort on command failure
- **WHEN** any command in the script fails (non-zero exit code)
- **THEN** script immediately aborts execution
- **AND** does not proceed to subsequent steps

### Requirement: Restart script displays current status
The restart script SHALL display current container status before stopping services.

#### Scenario: Show status before restart
- **WHEN** user executes `./restart.sh`
- **THEN** script outputs `docker-compose ps` table showing current running containers

### Requirement: Restart script waits for MySQL health
The restart script SHALL wait for MySQL container health check to pass after starting services.

#### Scenario: Wait for MySQL healthy
- **WHEN** services start with `docker-compose up -d`
- **THEN** script polls MySQL health status
- **AND** waits up to 90 seconds
- **AND** outputs "MySQL 已就绪" when healthy

#### Scenario: MySQL health timeout
- **WHEN** MySQL does not become healthy within 90 seconds
- **THEN** script outputs warning message
- **AND** continues waiting (relies on docker-compose healthcheck config)

### Requirement: Restart script stops Docker monitor
The restart script SHALL stop any running Docker events monitor process before restarting services.

#### Scenario: Stop existing monitor
- **WHEN** restart script executes
- **THEN** script kills any process matching "docker-events-monitor.sh"
- **AND** suppresses error if no process exists (2>/dev/null)

### Requirement: Restart script starts Docker monitor conditionally
The restart script SHALL start Docker monitor if `~/scripts/docker-monitor.sh` exists.

#### Scenario: Start monitor when available
- **WHEN** `~/scripts/docker-monitor.sh` exists and is executable
- **THEN** script executes `~/scripts/docker-monitor.sh start`
- **AND** outputs monitor PID

#### Scenario: Skip monitor when unavailable
- **WHEN** `~/scripts/docker-monitor.sh` does not exist
- **THEN** script outputs "监控脚本未找到，跳过" message
- **AND** continues without error

### Requirement: Restart script outputs detailed logs with timestamps
The restart script SHALL output execution logs with timestamps for troubleshooting.

#### Scenario: Timestamped logging
- **WHEN** script executes each step
- **THEN** output includes timestamp prefix
- **AND** describes current action

### Requirement: Restart script displays correct access information
The restart script SHALL display accurate service port mappings after completion.

#### Scenario: Correct port display
- **WHEN** restart completes successfully
- **THEN** script outputs correct mappings:
  - 飞书 Bot: http://localhost:3000
  - REST API: http://localhost:3001
  - Web前端: http://localhost:80
  - Adminer: http://localhost:8080