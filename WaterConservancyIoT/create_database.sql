-- 智能水利系统数据库创建脚本
-- 执行前请先删除现有的 sensordatabase 数据库

-- 创建数据库
CREATE DATABASE IF NOT EXISTS `sensordatabase` 
DEFAULT CHARACTER SET utf8mb4 
DEFAULT COLLATE utf8mb4_0900_ai_ci;

-- 使用数据库
USE `sensordatabase`;

-- 创建传感器信息表
CREATE TABLE `sensors` (
  `sensor_id` varchar(50) NOT NULL,
  `name` varchar(100) DEFAULT NULL,
  `sensor_type` varchar(50) NOT NULL,
  `location` varchar(100) DEFAULT NULL,
  `description` text,
  `states` json DEFAULT NULL,
  PRIMARY KEY (`sensor_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- 创建传感器读数表
CREATE TABLE `sensor_readings` (
  `id` int NOT NULL AUTO_INCREMENT,
  `sensor_id` varchar(50) NOT NULL,
  `timestamp` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  `value` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `idx_sensor_timestamp` (`sensor_id`,`timestamp`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- 创建用户表
CREATE TABLE `users` (
  `id` int NOT NULL AUTO_INCREMENT,
  `username` varchar(50) NOT NULL,
  `password_hash` varchar(255) NOT NULL,
  `role` enum('admin','operator','viewer') NOT NULL DEFAULT 'viewer',
  `created_at` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  `last_login` timestamp NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `username` (`username`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- 注意: 默认管理员将通过程序自动创建，用户名: admin, 密码: admin123

-- 显示创建结果
SHOW TABLES;
SELECT '数据库创建完成！' as status;
