-- ============================================
-- 校园BBS 数据库初始化脚本
-- 数据库: campus_bbs
-- 字符集: utf8mb4
-- ============================================

CREATE DATABASE IF NOT EXISTS campus_bbs
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE campus_bbs;

-- ============================================
-- 1. 管理员表
-- ============================================
CREATE TABLE admin (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '管理员ID',
    admin_name VARCHAR(50) UNIQUE NOT NULL COMMENT '管理员账号',
    password VARCHAR(255) NOT NULL COMMENT '密码（bcrypt哈希）',
    email VARCHAR(100) DEFAULT '' COMMENT '联系邮箱',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='管理员表';

-- ============================================
-- 2. 学生表
-- ============================================
CREATE TABLE student (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '学生ID',
    account VARCHAR(50) UNIQUE NOT NULL COMMENT '登录账号',
    username VARCHAR(50) NOT NULL COMMENT '昵称',
    name VARCHAR(50) NOT NULL COMMENT '真实姓名',
    password VARCHAR(255) NOT NULL COMMENT '密码（bcrypt哈希）',
    gender ENUM('男', '女') DEFAULT '男' COMMENT '性别',
    birthday DATE COMMENT '出生日期',
    major VARCHAR(100) COMMENT '专业',
    avatar VARCHAR(200) COMMENT '头像URL',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '注册时间'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='学生表';

-- ============================================
-- 3. 板块表
-- ============================================
CREATE TABLE board (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '板块ID',
    board_name VARCHAR(100) UNIQUE NOT NULL COMMENT '板块名称',
    description TEXT COMMENT '板块描述',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='板块表';

-- ============================================
-- 4. 帖子表
-- ============================================
CREATE TABLE post (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '帖子ID',
    board_id INT NOT NULL COMMENT '所属板块ID',
    author_id INT NOT NULL COMMENT '作者ID',
    author_type ENUM('student', 'admin') NOT NULL COMMENT '作者类型',
    title VARCHAR(200) NOT NULL COMMENT '标题',
    content TEXT NOT NULL COMMENT '内容',
    post_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '发布时间',
    update_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    is_deleted BOOLEAN DEFAULT FALSE COMMENT '是否已删除（软删除）',
    FOREIGN KEY (board_id) REFERENCES board(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='帖子表';

-- ============================================
-- 5. 回复表
-- ============================================
CREATE TABLE reply (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '回复ID',
    post_id INT NOT NULL COMMENT '所属帖子ID',
    student_id INT NOT NULL COMMENT '回复者ID（学生）',
    content TEXT NOT NULL COMMENT '回复内容',
    reply_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '回复时间',
    FOREIGN KEY (post_id) REFERENCES post(id) ON DELETE CASCADE,
    FOREIGN KEY (student_id) REFERENCES student(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='回复表';

-- ============================================
-- 6. 关注关系表
-- ============================================
CREATE TABLE user_follows (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '关注记录ID',
    follower_id INT NOT NULL COMMENT '关注者ID（学生）',
    followee_id INT NOT NULL COMMENT '被关注者ID',
    followee_type ENUM('student', 'admin') NOT NULL COMMENT '被关注者类型',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '关注时间',
    UNIQUE KEY uk_follower_followee (follower_id, followee_id, followee_type)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='关注关系表';

-- ============================================
-- 7. 点赞记录表
-- ============================================
CREATE TABLE post_like (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '点赞记录ID',
    user_id INT NOT NULL COMMENT '点赞者ID（学生）',
    post_id INT NOT NULL COMMENT '被点赞帖子ID',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '点赞时间',
    UNIQUE KEY uk_user_post (user_id, post_id),
    FOREIGN KEY (post_id) REFERENCES post(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='点赞记录表';

-- ============================================
-- 8. 通知表
-- ============================================
CREATE TABLE notification (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '通知ID',
    receiver_id INT NOT NULL COMMENT '接收者ID',
    receiver_type ENUM('student', 'admin') NOT NULL COMMENT '接收者类型',
    sender_id INT NOT NULL COMMENT '发送者ID',
    sender_type ENUM('student', 'admin') NOT NULL COMMENT '发送者类型',
    post_id INT NULL COMMENT '关联帖子ID（可为NULL）',
    reply_id INT NULL COMMENT '关联回复ID（可为NULL）',
    type ENUM('reply', 'like', 'visit') NOT NULL COMMENT '通知类型',
    content VARCHAR(255) NOT NULL COMMENT '通知内容',
    is_read BOOLEAN DEFAULT FALSE COMMENT '是否已读',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    FOREIGN KEY (post_id) REFERENCES post(id) ON DELETE CASCADE,
    FOREIGN KEY (reply_id) REFERENCES reply(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='通知表';

-- ============================================
-- 9. 浏览历史表
-- ============================================
CREATE TABLE browse_history (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '浏览记录ID',
    user_id INT NOT NULL COMMENT '学生ID',
    post_id INT NOT NULL COMMENT '浏览的帖子ID',
    browse_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '浏览时间',
    UNIQUE KEY uk_user_post (user_id, post_id),
    FOREIGN KEY (post_id) REFERENCES post(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='浏览历史表';

-- ============================================
-- 10. 主页访客记录表
-- ============================================
CREATE TABLE profile_view (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '访客记录ID',
    viewer_id INT NOT NULL COMMENT '访客ID（0表示游客，>0为学生ID）',
    target_user_id INT NOT NULL COMMENT '被访问用户ID',
    target_user_type ENUM('student', 'admin') NOT NULL COMMENT '被访问用户类型',
    viewed_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '访问时间'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='主页访客记录表';

-- ============================================
-- 插入初始数据
-- ============================================

-- 插入默认管理员（密码为 'admin123' 的 bcrypt 哈希，成本因子10）
-- 实际密码需用 bcrypt 生成，此处为示例
INSERT INTO admin (admin_name, password, email) VALUES
('admin', '$2a$10$N9qo8uLOickgx2ZMRZoMy.Mr4u6Zz1Z6Jp5x5cFzX3h9z1q5m7kA', 'admin@bbs.com');

-- 插入两个默认板块
INSERT INTO board (board_name, description) VALUES
('技术交流', '讨论编程、算法、前沿技术'),
('生活娱乐', '闲聊、音乐、电影、美食');

-- ============================================
-- 可选：插入测试学生（密码均为 '123456'）
-- 实际使用时请删除或修改
-- ============================================
-- INSERT INTO student (account, username, name, password, gender, birthday, major) VALUES
-- ('test1', '小明', '张三', '$2a$10$N9qo8uLOickgx2ZMRZoMy.Mr4u6Zz1Z6Jp5x5cFzX3h9z1q5m7kA', '男', '2000-01-01', '计算机科学与技术'),
-- ('test2', '小红', '李四', '$2a$10$N9qo8uLOickgx2ZMRZoMy.Mr4u6Zz1Z6Jp5x5cFzX3h9z1q5m7kA', '女', '2001-02-02', '软件工程');

-- ============================================
-- 完成
-- ============================================
