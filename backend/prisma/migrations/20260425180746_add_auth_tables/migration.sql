-- CreateTable
CREATE TABLE `stocks` (
    `id` INTEGER NOT NULL AUTO_INCREMENT,
    `ts_code` VARCHAR(20) NOT NULL,
    `symbol` VARCHAR(10) NULL,
    `name` VARCHAR(100) NULL,
    `area` VARCHAR(20) NULL,
    `industry` VARCHAR(50) NULL,
    `market` VARCHAR(20) NULL,
    `exchange` VARCHAR(20) NULL,
    `list_status` VARCHAR(10) NULL,
    `created_at` DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3),
    `updated_at` DATETIME(3) NOT NULL,

    UNIQUE INDEX `stocks_ts_code_key`(`ts_code`),
    PRIMARY KEY (`id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- CreateTable
CREATE TABLE `analysis_results` (
    `id` INTEGER NOT NULL AUTO_INCREMENT,
    `ts_code` VARCHAR(20) NOT NULL,
    `strategy_name` VARCHAR(50) NOT NULL,
    `trade_date` DATE NOT NULL,
    `label` VARCHAR(20) NOT NULL,
    `freq` VARCHAR(5) NULL,
    `created_at` DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3),

    INDEX `analysis_results_ts_code_idx`(`ts_code`),
    INDEX `analysis_results_strategy_name_idx`(`strategy_name`),
    INDEX `analysis_results_trade_date_idx`(`trade_date`),
    UNIQUE INDEX `analysis_results_ts_code_strategy_name_trade_date_freq_key`(`ts_code`, `strategy_name`, `trade_date`, `freq`),
    PRIMARY KEY (`id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- CreateTable
CREATE TABLE `analysis_progress` (
    `id` INTEGER NOT NULL,
    `phase1_status` VARCHAR(20) NOT NULL DEFAULT 'idle',
    `phase1_total` INTEGER NOT NULL DEFAULT 0,
    `phase1_completed` INTEGER NOT NULL DEFAULT 0,
    `phase1_qualified` INTEGER NOT NULL DEFAULT 0,
    `phase2_status` VARCHAR(20) NOT NULL DEFAULT 'idle',
    `phase2_total` INTEGER NOT NULL DEFAULT 0,
    `phase2_completed` INTEGER NOT NULL DEFAULT 0,
    `phase2_failed` INTEGER NOT NULL DEFAULT 0,
    `started_at` DATETIME(3) NULL,
    `phase1_completed_at` DATETIME(3) NULL,
    `updated_at` DATETIME(3) NOT NULL,

    PRIMARY KEY (`id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- CreateTable
CREATE TABLE `analysis_process_records` (
    `id` INTEGER NOT NULL AUTO_INCREMENT,
    `ts_code` VARCHAR(20) NOT NULL,
    `stock_name` VARCHAR(100) NULL,
    `trade_date` DATE NOT NULL,
    `data` JSON NOT NULL,
    `created_at` DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3),
    `expires_at` DATETIME(3) NULL,

    INDEX `analysis_process_records_ts_code_idx`(`ts_code`),
    INDEX `analysis_process_records_trade_date_idx`(`trade_date`),
    UNIQUE INDEX `analysis_process_records_ts_code_trade_date_key`(`ts_code`, `trade_date`),
    PRIMARY KEY (`id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- CreateTable
CREATE TABLE `chart_data` (
    `id` INTEGER NOT NULL AUTO_INCREMENT,
    `ts_code` VARCHAR(20) NOT NULL,
    `freq` VARCHAR(5) NOT NULL,
    `analysis_date` DATE NOT NULL,
    `data` JSON NOT NULL,
    `updated_at` DATETIME(3) NOT NULL,

    UNIQUE INDEX `chart_data_ts_code_freq_analysis_date_key`(`ts_code`, `freq`, `analysis_date`),
    PRIMARY KEY (`id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- CreateTable
CREATE TABLE `users` (
    `id` INTEGER NOT NULL AUTO_INCREMENT,
    `created_at` DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3),

    PRIMARY KEY (`id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- CreateTable
CREATE TABLE `wechat_bindings` (
    `id` INTEGER NOT NULL AUTO_INCREMENT,
    `user_id` INTEGER NOT NULL,
    `openid` VARCHAR(50) NOT NULL,
    `subscribed_at` DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3),
    `unsubscribed_at` DATETIME(3) NULL,

    UNIQUE INDEX `wechat_bindings_openid_key`(`openid`),
    PRIMARY KEY (`id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- CreateTable
CREATE TABLE `login_sessions` (
    `session_id` VARCHAR(36) NOT NULL,
    `status` VARCHAR(20) NOT NULL DEFAULT 'pending',
    `user_id` INTEGER NULL,
    `created_at` DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3),
    `expires_at` DATETIME(3) NOT NULL,

    INDEX `login_sessions_status_idx`(`status`),
    INDEX `login_sessions_expires_at_idx`(`expires_at`),
    PRIMARY KEY (`session_id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- AddForeignKey
ALTER TABLE `wechat_bindings` ADD CONSTRAINT `wechat_bindings_user_id_fkey` FOREIGN KEY (`user_id`) REFERENCES `users`(`id`) ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE `login_sessions` ADD CONSTRAINT `login_sessions_user_id_fkey` FOREIGN KEY (`user_id`) REFERENCES `users`(`id`) ON DELETE SET NULL ON UPDATE CASCADE;
