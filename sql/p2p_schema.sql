-- P2P Hosting System Database Schema

-- Host Management
CREATE TABLE `p2p_hosts` (
    `host_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `host_type` ENUM('p2p', 'vps') NOT NULL DEFAULT 'p2p',
    `address` VARCHAR(128) NOT NULL,
    `port` SMALLINT UNSIGNED NOT NULL,
    `status` ENUM('offline', 'starting', 'online', 'degraded', 'maintenance') NOT NULL DEFAULT 'offline',
    `performance_score` FLOAT NOT NULL DEFAULT 0,
    `max_maps` TINYINT UNSIGNED NOT NULL DEFAULT 5,
    `max_players` SMALLINT UNSIGNED NOT NULL DEFAULT 100,
    `token` VARCHAR(64) NULL,
    `is_banned` TINYINT(1) NOT NULL DEFAULT 0,
    `location` POINT NULL,
    `region` VARCHAR(32) NULL,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    PRIMARY KEY (`host_id`),
    UNIQUE KEY `uk_address_port` (`address`, `port`),
    KEY `idx_status` (`status`),
    KEY `idx_performance` (`performance_score`),
    SPATIAL KEY `idx_location` (`location`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Map Distribution
CREATE TABLE `p2p_map_assignments` (
    `assignment_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `map_id` INT UNSIGNED NOT NULL,
    `host_id` INT UNSIGNED NOT NULL,
    `assignment_type` ENUM('primary', 'backup') NOT NULL DEFAULT 'primary',
    `status` ENUM('pending', 'active', 'transferring', 'failed') NOT NULL DEFAULT 'pending',
    `player_count` SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    PRIMARY KEY (`assignment_id`),
    UNIQUE KEY `uk_map_host` (`map_id`, `host_id`),
    KEY `idx_host_id` (`host_id`),
    KEY `idx_status` (`status`),
    FOREIGN KEY (`host_id`) REFERENCES `p2p_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Map Configuration
CREATE TABLE `p2p_map_config` (
    `map_id` INT UNSIGNED NOT NULL,
    `is_critical` TINYINT(1) NOT NULL DEFAULT 0,
    `min_performance_score` FLOAT NOT NULL DEFAULT 0,
    `max_players` SMALLINT UNSIGNED NOT NULL DEFAULT 100,
    `backup_count` TINYINT UNSIGNED NOT NULL DEFAULT 1,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    PRIMARY KEY (`map_id`),
    KEY `idx_critical` (`is_critical`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Active Player Sessions
CREATE TABLE `p2p_active_sessions` (
    `session_id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    `char_id` INT UNSIGNED NOT NULL,
    `account_id` INT UNSIGNED NOT NULL,
    `host_id` INT UNSIGNED NOT NULL,
    `map_id` INT UNSIGNED NOT NULL,
    `started_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `last_ping` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `connection_data` JSON NOT NULL,
    PRIMARY KEY (`session_id`),
    UNIQUE KEY `uk_char_session` (`char_id`),
    KEY `idx_host_sessions` (`host_id`),
    KEY `idx_map_sessions` (`map_id`),
    FOREIGN KEY (`host_id`) REFERENCES `p2p_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Map State Synchronization
CREATE TABLE `p2p_map_sync_state` (
    `sync_id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    `map_id` INT UNSIGNED NOT NULL,
    `host_id` INT UNSIGNED NOT NULL,
    `last_full_sync` TIMESTAMP NOT NULL,
    `last_delta_sync` TIMESTAMP NOT NULL,
    `state_hash` VARCHAR(64) NOT NULL,
    `sync_status` ENUM('synced', 'syncing', 'out_of_sync', 'error') NOT NULL,
    `sync_details` JSON NULL,
    PRIMARY KEY (`sync_id`),
    UNIQUE KEY `uk_map_host` (`map_id`, `host_id`),
    KEY `idx_status` (`sync_status`),
    FOREIGN KEY (`host_id`) REFERENCES `p2p_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Host Performance Metrics
CREATE TABLE `p2p_host_metrics` (
    `metric_id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    `host_id` INT UNSIGNED NOT NULL,
    `cpu_usage` FLOAT NOT NULL DEFAULT 0,
    `memory_usage` FLOAT NOT NULL DEFAULT 0,
    `network_latency` FLOAT NOT NULL DEFAULT 0,
    `bandwidth_usage` FLOAT NOT NULL DEFAULT 0,
    `player_count` SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    `error_count` SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    `recorded_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (`metric_id`),
    KEY `idx_host_time` (`host_id`, `recorded_at`),
    FOREIGN KEY (`host_id`) REFERENCES `p2p_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Migration History
CREATE TABLE `p2p_migration_history` (
    `migration_id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    `char_id` INT UNSIGNED NOT NULL,
    `source_host_id` INT UNSIGNED NULL,
    `target_host_id` INT UNSIGNED NULL,
    `map_id` INT UNSIGNED NOT NULL,
    `reason` VARCHAR(32) NOT NULL,
    `started_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `completed_at` TIMESTAMP NULL,
    `status` ENUM('pending', 'in_progress', 'completed', 'failed') NOT NULL,
    `state_data` JSON NULL,
    PRIMARY KEY (`migration_id`),
    KEY `idx_char_history` (`char_id`),
    KEY `idx_source_host` (`source_host_id`),
    KEY `idx_target_host` (`target_host_id`),
    FOREIGN KEY (`source_host_id`) REFERENCES `p2p_hosts` (`host_id`) ON DELETE SET NULL,
    FOREIGN KEY (`target_host_id`) REFERENCES `p2p_hosts` (`host_id`) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Network Partition Recovery
CREATE TABLE `p2p_partition_events` (
    `event_id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    `partition_start` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `partition_end` TIMESTAMP NULL,
    `affected_hosts` JSON NOT NULL,
    `recovery_status` ENUM('detected', 'recovering', 'resolved', 'failed') NOT NULL,
    `state_conflicts` JSON NULL,
    `resolution_strategy` VARCHAR(32) NULL,
    PRIMARY KEY (`event_id`),
    KEY `idx_status` (`recovery_status`),
    KEY `idx_timing` (`partition_start`, `partition_end`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Security Events
CREATE TABLE `p2p_security_events` (
    `event_id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    `host_id` INT UNSIGNED NULL,
    `event_type` VARCHAR(32) NOT NULL,
    `severity` ENUM('low', 'medium', 'high', 'critical') NOT NULL,
    `details` JSON NOT NULL,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `resolved_at` TIMESTAMP NULL,
    PRIMARY KEY (`event_id`),
    KEY `idx_host_id` (`host_id`),
    KEY `idx_type_severity` (`event_type`, `severity`),
    KEY `idx_created` (`created_at`),
    FOREIGN KEY (`host_id`) REFERENCES `p2p_hosts` (`host_id`) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Create Procedures
DELIMITER //

-- Update Host Performance Score
CREATE PROCEDURE `update_host_performance_score`(IN p_host_id INT UNSIGNED)
BEGIN
    DECLARE v_cpu_avg FLOAT;
    DECLARE v_memory_avg FLOAT;
    DECLARE v_latency_avg FLOAT;
    DECLARE v_error_rate FLOAT;
    
    -- Calculate averages from recent metrics
    SELECT 
        AVG(cpu_usage) INTO v_cpu_avg,
        AVG(memory_usage) INTO v_memory_avg,
        AVG(network_latency) INTO v_latency_avg,
        SUM(error_count) / COUNT(*) INTO v_error_rate
    FROM p2p_host_metrics
    WHERE host_id = p_host_id
    AND recorded_at >= DATE_SUB(NOW(), INTERVAL 5 MINUTE);
    
    -- Update host performance score
    UPDATE p2p_hosts
    SET performance_score = (
        (100 - v_cpu_avg) * 0.3 +
        (100 - v_memory_avg) * 0.3 +
        (100 - v_latency_avg/2) * 0.3 +
        (100 - v_error_rate*10) * 0.1
    )
    WHERE host_id = p_host_id;
END //

-- Clean Old Data
CREATE PROCEDURE `clean_old_data`()
BEGIN
    -- Delete metrics older than 30 days
    DELETE FROM p2p_host_metrics
    WHERE recorded_at < DATE_SUB(NOW(), INTERVAL 30 DAY);
    
    -- Delete resolved security events older than 90 days
    DELETE FROM p2p_security_events
    WHERE resolved_at IS NOT NULL
    AND resolved_at < DATE_SUB(NOW(), INTERVAL 90 DAY);
    
    -- Delete completed transfers older than 7 days
    DELETE FROM p2p_transfer_queue
    WHERE status = 'completed'
    AND completed_at < DATE_SUB(NOW(), INTERVAL 7 DAY);
    
    -- Clean expired sessions
    DELETE FROM p2p_active_sessions 
    WHERE last_ping < DATE_SUB(NOW(), INTERVAL 15 MINUTE);
    
    -- Clean old resolved partitions
    DELETE FROM p2p_partition_events
    WHERE recovery_status = 'resolved'
    AND partition_end < DATE_SUB(NOW(), INTERVAL 30 DAY);
    
    -- Clean old migrations
    DELETE FROM p2p_migration_history
    WHERE status = 'completed'
    AND completed_at < DATE_SUB(NOW(), INTERVAL 90 DAY);
END //

-- Cleanup Orphaned Sessions
CREATE PROCEDURE `cleanup_orphaned_sessions`()
BEGIN
    DELETE FROM p2p_active_sessions
    WHERE host_id NOT IN (SELECT host_id FROM p2p_hosts WHERE status = 'online');
END //

-- Sync Map State
CREATE PROCEDURE `sync_map_state`(
    IN p_map_id INT UNSIGNED,
    IN p_host_id INT UNSIGNED,
    IN p_state_hash VARCHAR(64)
)
BEGIN
    INSERT INTO p2p_map_sync_state
        (map_id, host_id, last_full_sync, last_delta_sync, state_hash, sync_status)
    VALUES
        (p_map_id, p_host_id, NOW(), NOW(), p_state_hash, 'synced')
    ON DUPLICATE KEY UPDATE
        last_delta_sync = NOW(),
        state_hash = p_state_hash,
        sync_status = 'synced';
END //

DELIMITER ;

-- Create Events
CREATE EVENT `e_update_performance_scores`
ON SCHEDULE EVERY 1 MINUTE
DO
    BEGIN
        DECLARE done INT DEFAULT FALSE;
        DECLARE v_host_id INT UNSIGNED;
        DECLARE cur_hosts CURSOR FOR SELECT host_id FROM p2p_hosts WHERE status = 'online';
        DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;
        
        OPEN cur_hosts;
        read_loop: LOOP
            FETCH cur_hosts INTO v_host_id;
            IF done THEN
                LEAVE read_loop;
            END IF;
            CALL update_host_performance_score(v_host_id);
        END LOOP;
        CLOSE cur_hosts;
    END;

CREATE EVENT `e_clean_old_data`
ON SCHEDULE EVERY 1 DAY
DO CALL clean_old_data();

-- Initialize Critical Maps
INSERT INTO `p2p_map_config` (`map_id`, `is_critical`, `min_performance_score`, `backup_count`) VALUES
(1, 1, 95, 2),  -- prontera
(2, 1, 95, 2),  -- geffen
(3, 1, 95, 2),  -- payon
(4, 1, 95, 2),  -- alberta
(5, 1, 95, 2);  -- morocc