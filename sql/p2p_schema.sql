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
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    PRIMARY KEY (`host_id`),
    UNIQUE KEY `uk_address_port` (`address`, `port`),
    KEY `idx_status` (`status`),
    KEY `idx_performance` (`performance_score`)
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

-- Host State History
CREATE TABLE `p2p_host_history` (
    `history_id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    `host_id` INT UNSIGNED NOT NULL,
    `event_type` VARCHAR(32) NOT NULL,
    `previous_state` JSON NOT NULL,
    `new_state` JSON NOT NULL,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (`history_id`),
    KEY `idx_host_time` (`host_id`, `created_at`),
    FOREIGN KEY (`host_id`) REFERENCES `p2p_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Map Transfer Queue
CREATE TABLE `p2p_transfer_queue` (
    `transfer_id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    `map_id` INT UNSIGNED NOT NULL,
    `source_host_id` INT UNSIGNED NOT NULL,
    `target_host_id` INT UNSIGNED NOT NULL,
    `status` ENUM('queued', 'in_progress', 'completed', 'failed') NOT NULL DEFAULT 'queued',
    `priority` TINYINT UNSIGNED NOT NULL DEFAULT 5,
    `player_count` SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `started_at` TIMESTAMP NULL,
    `completed_at` TIMESTAMP NULL,
    PRIMARY KEY (`transfer_id`),
    KEY `idx_status_priority` (`status`, `priority`),
    KEY `idx_source_host` (`source_host_id`),
    KEY `idx_target_host` (`target_host_id`),
    FOREIGN KEY (`source_host_id`) REFERENCES `p2p_hosts` (`host_id`),
    FOREIGN KEY (`target_host_id`) REFERENCES `p2p_hosts` (`host_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Rate Limiting
CREATE TABLE `p2p_rate_limits` (
    `limit_id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    `host_id` INT UNSIGNED NOT NULL,
    `limit_type` VARCHAR(32) NOT NULL,
    `request_count` INT UNSIGNED NOT NULL DEFAULT 0,
    `last_request` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `window_start` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (`limit_id`),
    UNIQUE KEY `uk_host_type` (`host_id`, `limit_type`),
    FOREIGN KEY (`host_id`) REFERENCES `p2p_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Admin Actions Log
CREATE TABLE `p2p_admin_log` (
    `log_id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    `admin_id` INT UNSIGNED NOT NULL,
    `action` VARCHAR(32) NOT NULL,
    `target_type` VARCHAR(32) NOT NULL,
    `target_id` INT UNSIGNED NOT NULL,
    `details` JSON NOT NULL,
    `ip_address` VARCHAR(45) NOT NULL,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (`log_id`),
    KEY `idx_admin_time` (`admin_id`, `created_at`),
    KEY `idx_action` (`action`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Initialize Critical Maps
INSERT INTO `p2p_map_config` (`map_id`, `is_critical`, `min_performance_score`, `backup_count`) VALUES
(1, 1, 95, 2),  -- prontera
(2, 1, 95, 2),  -- geffen
(3, 1, 95, 2),  -- payon
(4, 1, 95, 2),  -- alberta
(5, 1, 95, 2);  -- morocc

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

-- Clean Old Metrics
CREATE PROCEDURE `clean_old_metrics`()
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
DO CALL clean_old_metrics();