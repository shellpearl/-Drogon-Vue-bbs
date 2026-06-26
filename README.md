# 📚 校园BBS

一个基于 **Drogon(C++17)** 后端和 **Vue 3 + TypeScript** 前端的全功能校园论坛系统。

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![C++](https://img.shields.io/badge/C++-17-blue)
![Vue](https://img.shields.io/badge/Vue-3-green)
![TypeScript](https://img.shields.io/badge/TypeScript-5.0-blue)
![MySQL](https://img.shields.io/badge/MySQL-8.0-orange)

---

## 📖 项目简介

校园BBS是一个面向高校学生的技术交流与生活娱乐论坛系统。系统支持学生注册登录、发帖、回复、点赞、关注、实时通知等核心社交功能，并提供管理员后台进行板块和帖子管理。

### ✨ 主要特性

- 🔐 **JWT 身份认证**：安全可靠的 Token 认证机制
- 📝 **帖子管理**：发帖、浏览、软删除（支持作者本人及管理员删除）
- 💬 **回复系统**：帖子回复，回复后自动通知作者
- ❤️ **点赞功能**：点赞/取消点赞，实时更新点赞数，点赞通知
- 👥 **关注/粉丝**：关注/取消关注用户，查看关注/粉丝列表
- 🔔 **实时通知**：基于 HTTP 轮询的通知系统，支持回复、点赞、主页访问等通知
- 👤 **个人信息管理**：查看/编辑个人信息、上传头像、修改密码
- 📊 **浏览历史**：自动记录用户浏览过的帖子，按板块分组展示
- 👁️ **主页访客记录**：记录谁查看了你的主页（区分登录用户和游客）
- 🛡️ **管理员后台**：板块管理（创建/删除）、帖子管理（强制删除）
- 📱 **响应式设计**：基于 Element Plus 的现代化 UI

---

## 🛠️ 技术栈

### 后端

| 技术 | 版本 | 用途 |
|------|------|------|
| C++ | 17 | 开发语言 |
| Drogon | 1.9.12 | Web 框架 |
| MySQL | 8.0 | 数据库 |
| JWT | jwt-cpp | 身份认证 |
| bcrypt | hilch/Bcrypt.cpp | 密码加密 |

### 前端

| 技术 | 版本 | 用途 |
|------|------|------|
| Vue | 3.3.x | 前端框架 |
| TypeScript | 5.x | 开发语言 |
| Vite | 5.x | 构建工具 |
| Element Plus | 2.x | UI 组件库 |
| Pinia | 2.x | 状态管理 |
| Vue Router | 4.x | 路由管理 |
| Axios | 1.x | HTTP 请求 |

---

## 🚀 快速开始

### 环境要求

- Windows 10/11 / Linux / macOS
- CMake 3.15+
- MySQL 8.0+
- Node.js 18+
- Visual Studio 2022 / CLion / GCC

### 后端安装

1. **克隆项目**

```bash
git clone https://github.com/yourusername/campus-bbs.git
cd campus-bbs
```

2. **创建数据库**

```bash
mysql -u root -p < init.sql
```

3. **配置数据库连接**

修改 `config.json` 中的数据库配置：

```json
"db_clients": [
    {
        "name": "default",
        "rdbms": "mysql",
        "host": "127.0.0.1",
        "port": 3306,
        "dbname": "campus_bbs",
        "user": "root",
        "passwd": "your_password"
    }
]
```

4. **构建并运行**

```bash
mkdir build && cd build
cmake ..
make
./bbs_backend
```

后端服务默认运行在 `http://localhost:8080`

### 前端安装

```bash
cd frontend
npm install
npm run dev
```

前端开发服务器默认运行在 `http://localhost:5173`

---

## 📁 项目结构

```
campus-bbs/
├── backend/                          # 后端 (Drogon C++)
│   ├── controllers/                  # 控制器
│   │   ├── AuthController.h/.cc      # 认证（注册/登录）
│   │   ├── PostController.h/.cc      # 帖子管理
│   │   ├── ReplyController.h/.cc     # 回复管理
│   │   ├── UserController.h/.cc      # 个人信息管理
│   │   ├── FollowController.h/.cc    # 关注/粉丝
│   │   ├── LikeController.h/.cc      # 点赞
│   │   ├── NotificationController.h/.cc # 通知（HTTP轮询）
│   │   └── AdminController.h/.cc     # 管理员后台
│   ├── filters/                      # 过滤器
│   │   ├── TokenFilter.h/.cc         # JWT认证
│   │   └── AdminFilter.h/.cc         # 管理员权限
│   ├── models/                       # 数据模型
│   ├── utils/                        # 工具类
│   │   ├── JwtUtils.h/.cc            # JWT生成/验证
│   │   ├── BcryptUtils.h/.cc         # bcrypt加密
│   │   └── ResponseUtil.h/.cc        # 统一响应
│   ├── third_party/                  # 第三方源码
│   │   └── bcrypt/                   # bcrypt.cpp 源码
│   ├── uploads/                      # 上传文件目录
│   │   └── avatars/                  # 用户头像
│   ├── CMakeLists.txt
│   └── config.json                   # 配置文件
│
└── frontend/                         # 前端 (Vue 3 + TypeScript)
    ├── src/
    │   ├── api/                      # API 请求
    │   │   ├── auth.ts
    │   │   ├── post.ts
    │   │   ├── user.ts
    │   │   ├── follow.ts
    │   │   ├── like.ts
    │   │   └── notification.ts
    │   ├── components/               # 公共组件
    │   │   └── NavBar.vue            # 侧边栏导航
    │   ├── stores/                   # Pinia 状态管理
    │   │   ├── user.ts
    │   │   └── notification.ts
    │   ├── views/                    # 页面组件
    │   │   ├── Home.vue              # 首页（板块列表）
    │   │   ├── LoginRegister.vue     # 登录/注册
    │   │   ├── Board.vue             # 板块帖子列表
    │   │   ├── PostDetail.vue        # 帖子详情
    │   │   ├── Profile.vue           # 个人中心
    │   │   ├── ProfileEdit.vue       # 编辑个人信息
    │   │   ├── UserProfile.vue       # 用户主页
    │   │   ├── MyBrowse.vue          # 我的（浏览/回复/点赞）
    │   │   ├── NotificationList.vue  # 消息中心
    │   │   └── Admin/                # 管理员页面
    │   │       ├── BoardManage.vue
    │   │       └── PostManage.vue
    │   ├── router/                   # 路由配置
    │   ├── App.vue
    │   └── main.ts
    ├── package.json
    └── vite.config.ts
```

---

## 📡 API 概览

| 模块 | 方法 | 路径 | 说明 |
|------|------|------|------|
| **认证** | POST | `/api/auth/register` | 注册 |
| | POST | `/api/auth/login` | 登录 |
| **用户** | GET | `/api/user/info` | 获取个人信息 |
| | POST | `/api/user/update` | 更新个人信息 |
| | POST | `/api/user/password` | 修改密码 |
| | POST | `/api/user/avatar` | 上传头像 |
| | GET | `/api/user/public/{userId}` | 获取用户公开信息 |
| **帖子** | GET | `/api/post/board/{boardId}` | 获取板块帖子列表 |
| | GET | `/api/post/{postId}` | 获取帖子详情 |
| | POST | `/api/post/create` | 发帖 |
| | DELETE | `/api/post/{postId}` | 删除帖子 |
| | GET | `/api/post/author/{authorId}` | 获取用户帖子 |
| **回复** | POST | `/api/reply/create` | 回复帖子 |
| | DELETE | `/api/reply/{replyId}` | 删除回复 |
| | GET | `/api/reply/my` | 我的回复 |
| **关注** | POST | `/api/follow/toggle` | 关注/取消关注 |
| | GET | `/api/follow/following` | 关注列表 |
| | GET | `/api/follow/followers` | 粉丝列表 |
| | GET | `/api/follow/count/{userId}` | 关注/粉丝数 |
| **点赞** | POST | `/api/like/toggle` | 点赞/取消点赞 |
| | GET | `/api/like/my` | 我的点赞 |
| | GET | `/api/like/count/{postId}` | 点赞数 |
| **通知** | GET | `/api/notifications` | 通知列表 |
| | PUT | `/api/notifications/{id}/read` | 标记已读 |
| | PUT | `/api/notifications/read-all` | 全部已读 |
| | POST | `/api/notifications/delete` | 批量删除 |
| | GET | `/api/notifications/unread-count` | 未读数量 |
| **管理员** | POST | `/api/admin/board/create` | 创建板块 |
| | DELETE | `/api/admin/board/{boardId}` | 删除板块 |
| | GET | `/api/admin/posts` | 所有帖子 |
| | DELETE | `/api/admin/post/{postId}` | 强制删除帖子 |

---

## 🖥️ 功能截图

*（请替换为实际截图，建议每张截图尺寸一致）*

| 功能 | 截图 |
|------|------|
| 登录/注册 | |
| 首页板块列表 | |
| 帖子列表 | |
| 帖子详情 | |
| 个人中心 | |
| 我的页面 | |
| 消息中心 | |
| 管理员后台 | |

---

## 📄 许可证

本项目仅供学习交流使用，未经授权不得用于商业用途。

---

## 👤 作者

**贝壳** - [GitHub](https://github.com/yourusername)

---

## 🙏 致谢

- [Drogon](https://github.com/drogonframework/drogon) - 高性能 C++ Web 框架
- [Vue.js](https://vuejs.org/) - 渐进式 JavaScript 框架
- [Element Plus](https://element-plus.org/) - Vue 3 组件库
- [jwt-cpp](https://github.com/Thalhammer/jwt-cpp) - JWT 库
- [bcrypt.cpp](https://github.com/hilch/Bcrypt.cpp) - bcrypt 实现

---

⭐️ 如果这个项目对你有帮助，欢迎 Star！
