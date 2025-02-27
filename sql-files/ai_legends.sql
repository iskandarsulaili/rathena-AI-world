-- AI Legends SQL Schema
-- This file contains the database schema for the AI Legends system.

-- Drop tables if they exist
DROP TABLE IF EXISTS `ai_bloodline_conversations`;
DROP TABLE IF EXISTS `ai_bloodline_stories`;
DROP TABLE IF EXISTS `ai_character_bloodline_skills`;
DROP TABLE IF EXISTS `ai_bloodline_skills`;
DROP TABLE IF EXISTS `ai_character_bloodlines`;
DROP TABLE IF EXISTS `ai_bloodlines`;

-- Create bloodlines table
CREATE TABLE `ai_bloodlines` (
  `bloodline_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `description` text NOT NULL,
  `attributes` text NOT NULL COMMENT 'JSON string of attributes',
  `unlock_requirements` text NOT NULL COMMENT 'JSON string of unlock requirements',
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`bloodline_id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Create character bloodlines table
CREATE TABLE `ai_character_bloodlines` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `char_id` int(11) NOT NULL,
  `bloodline_id` int(11) NOT NULL,
  `level` int(11) NOT NULL DEFAULT '1',
  `experience` int(11) NOT NULL DEFAULT '0',
  `unlock_date` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `last_interaction` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `char_bloodline` (`char_id`,`bloodline_id`),
  KEY `bloodline_id` (`bloodline_id`),
  CONSTRAINT `ai_character_bloodlines_ibfk_1` FOREIGN KEY (`char_id`) REFERENCES `char` (`char_id`) ON DELETE CASCADE,
  CONSTRAINT `ai_character_bloodlines_ibfk_2` FOREIGN KEY (`bloodline_id`) REFERENCES `ai_bloodlines` (`bloodline_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Create bloodline skills table
CREATE TABLE `ai_bloodline_skills` (
  `skill_id` int(11) NOT NULL AUTO_INCREMENT,
  `bloodline_id` int(11) NOT NULL,
  `name` varchar(50) NOT NULL,
  `description` text NOT NULL,
  `max_level` int(11) NOT NULL DEFAULT '10',
  `cooldown` int(11) NOT NULL DEFAULT '60',
  `effect_type` varchar(20) NOT NULL DEFAULT 'passive',
  `target_type` varchar(20) NOT NULL DEFAULT 'self',
  `skill_range` int(11) NOT NULL DEFAULT '0',
  `aoe_range` int(11) NOT NULL DEFAULT '0',
  `cast_time` float NOT NULL DEFAULT '0',
  `duration` int(11) NOT NULL DEFAULT '0',
  `required_level` int(11) NOT NULL DEFAULT '1',
  `attributes` text NOT NULL COMMENT 'JSON string of attributes',
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`skill_id`),
  KEY `bloodline_id` (`bloodline_id`),
  CONSTRAINT `ai_bloodline_skills_ibfk_1` FOREIGN KEY (`bloodline_id`) REFERENCES `ai_bloodlines` (`bloodline_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Create character bloodline skills table
CREATE TABLE `ai_character_bloodline_skills` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `char_id` int(11) NOT NULL,
  `bloodline_id` int(11) NOT NULL,
  `skill_id` int(11) NOT NULL,
  `level` int(11) NOT NULL DEFAULT '1',
  `last_used` datetime DEFAULT NULL,
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `char_bloodline_skill` (`char_id`,`bloodline_id`,`skill_id`),
  KEY `bloodline_id` (`bloodline_id`),
  KEY `skill_id` (`skill_id`),
  CONSTRAINT `ai_character_bloodline_skills_ibfk_1` FOREIGN KEY (`char_id`) REFERENCES `char` (`char_id`) ON DELETE CASCADE,
  CONSTRAINT `ai_character_bloodline_skills_ibfk_2` FOREIGN KEY (`bloodline_id`) REFERENCES `ai_bloodlines` (`bloodline_id`) ON DELETE CASCADE,
  CONSTRAINT `ai_character_bloodline_skills_ibfk_3` FOREIGN KEY (`skill_id`) REFERENCES `ai_bloodline_skills` (`skill_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Create bloodline stories table
CREATE TABLE `ai_bloodline_stories` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `char_id` int(11) NOT NULL,
  `bloodline_id` int(11) NOT NULL,
  `story` text NOT NULL,
  `generated_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `char_bloodline` (`char_id`,`bloodline_id`),
  KEY `bloodline_id` (`bloodline_id`),
  CONSTRAINT `ai_bloodline_stories_ibfk_1` FOREIGN KEY (`char_id`) REFERENCES `char` (`char_id`) ON DELETE CASCADE,
  CONSTRAINT `ai_bloodline_stories_ibfk_2` FOREIGN KEY (`bloodline_id`) REFERENCES `ai_bloodlines` (`bloodline_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Create bloodline conversations table
CREATE TABLE `ai_bloodline_conversations` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `char_id` int(11) NOT NULL,
  `bloodline_id` int(11) NOT NULL,
  `message` text NOT NULL,
  `response` text NOT NULL,
  `timestamp` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `char_id` (`char_id`),
  KEY `bloodline_id` (`bloodline_id`),
  CONSTRAINT `ai_bloodline_conversations_ibfk_1` FOREIGN KEY (`char_id`) REFERENCES `char` (`char_id`) ON DELETE CASCADE,
  CONSTRAINT `ai_bloodline_conversations_ibfk_2` FOREIGN KEY (`bloodline_id`) REFERENCES `ai_bloodlines` (`bloodline_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Insert sample bloodlines
INSERT INTO `ai_bloodlines` (`name`, `description`, `attributes`, `unlock_requirements`) VALUES
('Dragon Ancestry', 'The blood of ancient dragons flows through your veins, granting you power over fire and a connection to these mighty creatures.', '{"element": "fire", "affinity": "dragon", "primary_stat": "str", "secondary_stat": "int"}', '{"min_level": 40, "min_job_level": 40, "min_stats": {"str": 50}, "required_quests": [7000]}'),
('Celestial Lineage', 'Your ancestors were blessed by the stars, giving you cosmic powers and insight into the mysteries of the universe.', '{"element": "holy", "affinity": "star", "primary_stat": "int", "secondary_stat": "luk"}', '{"min_level": 40, "min_job_level": 40, "min_stats": {"int": 50}, "required_quests": [7000]}'),
('Shadow Bloodline', 'Darkness runs in your family, providing stealth and control over shadows and the night.', '{"element": "shadow", "affinity": "darkness", "primary_stat": "agi", "secondary_stat": "dex"}', '{"min_level": 40, "min_job_level": 40, "min_stats": {"agi": 50}, "required_quests": [7000]}'),
('Nature\'s Chosen', 'The spirits of nature have selected your lineage to channel their power, giving you control over plants and animals.', '{"element": "earth", "affinity": "nature", "primary_stat": "vit", "secondary_stat": "str"}', '{"min_level": 40, "min_job_level": 40, "min_stats": {"vit": 50}, "required_quests": [7000]}'),
('Arcane Heritage', 'Magic has been in your family for generations, making spellcasting come naturally to you.', '{"element": "neutral", "affinity": "magic", "primary_stat": "int", "secondary_stat": "dex"}', '{"min_level": 40, "min_job_level": 40, "min_stats": {"int": 50}, "required_quests": [7000]}');

-- Insert sample bloodline skills
INSERT INTO `ai_bloodline_skills` (`bloodline_id`, `name`, `description`, `max_level`, `cooldown`, `effect_type`, `target_type`, `required_level`, `attributes`) VALUES
(1, 'Dragon\'s Breath', 'Channel the fire of your dragon ancestors to breathe flames at your enemies.', 10, 60, 'active', 'aoe', 5, '{"damage_type": "fire", "scaling_stat": "str", "base_damage": 100, "damage_per_level": 50}'),
(1, 'Scales of Protection', 'Your skin hardens like dragon scales, reducing incoming damage.', 10, 120, 'buff', 'self', 10, '{"defense_bonus": 10, "defense_per_level": 5, "duration": 60, "duration_per_level": 10}'),
(2, 'Starfall', 'Call down a shower of stars to damage enemies in an area.', 10, 90, 'active', 'aoe', 5, '{"damage_type": "holy", "scaling_stat": "int", "base_damage": 80, "damage_per_level": 40}'),
(2, 'Cosmic Insight', 'Gain temporary insight into the future, increasing accuracy and critical rate.', 10, 180, 'buff', 'self', 10, '{"hit_bonus": 5, "hit_per_level": 2, "crit_bonus": 3, "crit_per_level": 1, "duration": 60, "duration_per_level": 10}'),
(3, 'Shadow Step', 'Meld with the shadows to teleport a short distance.', 10, 30, 'active', 'self', 5, '{"range": 5, "range_per_level": 1}'),
(3, 'Cloak of Darkness', 'Wrap yourself in shadows, becoming invisible to enemies.', 10, 120, 'buff', 'self', 10, '{"duration": 10, "duration_per_level": 2}'),
(4, 'Nature\'s Wrath', 'Command roots and vines to entangle and damage enemies.', 10, 60, 'active', 'aoe', 5, '{"damage_type": "earth", "scaling_stat": "vit", "base_damage": 70, "damage_per_level": 35, "root_duration": 2, "root_duration_per_level": 0.5}'),
(4, 'Healing Bloom', 'Channel nature\'s energy to heal yourself and nearby allies.', 10, 90, 'active', 'aoe', 10, '{"healing": 100, "healing_per_level": 50, "range": 3, "range_per_level": 0.5}'),
(5, 'Arcane Missile', 'Fire a barrage of magic missiles at your target.', 10, 15, 'active', 'single', 5, '{"damage_type": "neutral", "scaling_stat": "int", "base_damage": 50, "damage_per_level": 25, "missiles": 3, "missiles_per_level": 1}'),
(5, 'Spell Mastery', 'Your innate understanding of magic reduces the cast time and SP cost of your spells.', 10, 0, 'passive', 'self', 10, '{"cast_time_reduction": 5, "cast_time_reduction_per_level": 1, "sp_cost_reduction": 3, "sp_cost_reduction_per_level": 1}');