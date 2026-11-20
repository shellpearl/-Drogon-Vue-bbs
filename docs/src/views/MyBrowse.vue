<template>
  <div class="my-page">
    <div class="user-card">
      <div class="user-info-left">
        <el-avatar :size="64" :src="userInfo.avatar || ''">
          {{ userInfo.username?.charAt(0) || 'U' }}
        </el-avatar>
        <div class="user-name">
          <h2>{{ userInfo.username || '用户' }}</h2>
          <span class="user-role">{{ userInfo.role === 'admin' ? '管理员' : '学生' }}</span>
        </div>
      </div>
      <div class="user-info-right">
        <div class="stat-item" @click="goToFollowList('following')">
          关注 <strong>{{ followStats.following_count }}</strong>
        </div>
        <div class="stat-item" @click="goToFollowList('followers')">
          粉丝 <strong>{{ followStats.follower_count }}</strong>
        </div>
      </div>
    </div>

    <el-tabs v-model="activeTab" class="my-tabs">
      <el-tab-pane label="我的浏览记录" name="history">
        <div v-if="historyLoading" class="loading">加载中...</div>
        <div v-else-if="boardList.length === 0" class="empty">暂无浏览记录</div>
        <div v-else>
          <div v-for="board in boardList" :key="board.board_id" class="board-section">
            <h3>{{ board.board_name }}</h3>
            <div class="posts-grid">
              <div
                  v-for="post in board.posts"
                  :key="post.post_id"
                  class="post-card"
                  @click="goToPost(post.post_id)"
              >
                <div class="post-title">{{ post.title }}</div>
                <div class="post-time">{{ post.browse_time }}</div>
              </div>
            </div>
          </div>
        </div>
      </el-tab-pane>

      <el-tab-pane label="我的回复" name="replies">
        <div v-if="repliesLoading" class="loading">加载中...</div>
        <div v-else-if="replies.length === 0" class="empty">暂无回复</div>
        <div v-else class="replies-grid">
          <div
              v-for="reply in replies"
              :key="reply.id"
              class="reply-card"
          >
            <div class="reply-content" @click="goToPost(reply.post_id)">
              <div class="reply-text">{{ reply.content }}</div>
              <div class="reply-meta">
                <span class="post-title">{{ reply.post_title }}</span>
                <span class="reply-time">{{ reply.reply_time }}</span>
              </div>
            </div>
            <el-button
                type="danger"
                size="small"
                text
                class="delete-btn"
                @click.stop="handleDeleteReply(reply.id)"
            >
              <el-icon><Delete /></el-icon>
            </el-button>
          </div>
        </div>
      </el-tab-pane>

      <el-tab-pane label="我的点赞" name="likes">
        <div v-if="likesLoading" class="loading">加载中...</div>
        <div v-else-if="likes.length === 0" class="empty">暂无点赞</div>
        <div v-else class="likes-grid">
          <div
              v-for="post in likes"
              :key="post.id"
              class="like-card"
              @click="goToPost(post.id)"
          >
            <div class="post-title">{{ post.title }}</div>
            <div class="post-meta">
              <span class="board-name">{{ post.board_name }}</span>
              <span class="post-time">{{ post.post_time }}</span>
            </div>
          </div>
        </div>
      </el-tab-pane>
    </el-tabs>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import { getBrowseHistory } from '@/api/browse'
import { getMyReplies, deleteReply } from '@/api/reply'
import { getFollowCounts } from '@/api/follow'
import { getMyLikes } from '@/api/like'
import { getUserInfo } from '@/api/user'
import { useUserStore } from '@/stores/user'
import { ElMessage, ElMessageBox } from 'element-plus'
import { Delete } from '@element-plus/icons-vue'

interface Post {
  post_id: number
  title: string
  browse_time: string
}

interface Board {
  board_id: number
  board_name: string
  posts: Post[]
}

interface Reply {
  id: number
  content: string
  reply_time: string
  post_title: string
  post_id: number
}

interface Like {
  id: number
  title: string
  post_time: string
  board_name: string
}

const router = useRouter()
const userStore = useUserStore()

const activeTab = ref('history')

const userInfo = ref({
  username: '',
  avatar: '',
  role: 'student'
})

const followStats = ref({ following_count: 0, follower_count: 0 })

const replies = ref<Reply[]>([])
const repliesLoading = ref(true)

const boardList = ref<Board[]>([])
const historyLoading = ref(true)

const likes = ref<Like[]>([])
const likesLoading = ref(true)

const fetchLikes = async () => {
  likesLoading.value = true
  try {
    const res = await getMyLikes()
    likes.value = res.data || []
  } catch (error) {
    ElMessage.error('获取点赞列表失败')
  } finally {
    likesLoading.value = false
  }
}

const fetchUserInfo = async () => {
  try {
    const res = await getUserInfo()
    userInfo.value = {
      username: res.data.username || '用户',
      avatar: res.data.avatar || '',
      role: userStore.role || 'student'
    }
  } catch (error) {
    userInfo.value = {
      username: userStore.username || '用户',
      avatar: userStore.avatar || '',
      role: userStore.role || 'student'
    }
  }
}

const fetchFollowStats = async () => {
  try {
    const res = await getFollowCounts(userStore.userId)
    followStats.value = {
          following_count: res.data.following_count || 0,
          follower_count: res.data.follower_count || 0
    }
  } catch (error) {
  }
}

const fetchReplies = async () => {
  repliesLoading.value = true
  try {
    const res = await getMyReplies()
    replies.value = res.data || []
  } catch (error) {
    ElMessage.error('获取回复失败')
  } finally {
    repliesLoading.value = false
  }
}

const fetchHistory = async () => {
  try {
    const res = await getBrowseHistory()
    boardList.value = res.data || []
  } catch (error) {
    ElMessage.error('获取浏览记录失败')
  } finally {
    historyLoading.value = false
  }
}

const handleDeleteReply = async (replyId: number) => {
  try {
    await ElMessageBox.confirm('您确定删除该条回复吗？', '提示', {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      type: 'warning'
    })
    await deleteReply(replyId)
    ElMessage.success('删除成功')
    await fetchReplies()
  } catch (error) {
    if (error !== 'cancel') {
      ElMessage.error('删除失败')
    }
  }
}

const goToPost = (postId: number) => {
  router.push(`/post/${postId}`)
}

const goToFollowList = (type: 'following' | 'followers') => {
  router.push(`/follow/${type}`)
}

onMounted(() => {
  fetchUserInfo()
  fetchFollowStats()
  fetchReplies()
  fetchHistory()
  fetchLikes()
})
</script>

<style scoped>
.likes-grid {
  display: grid;
  grid-template-columns: repeat(5, 1fr);
  gap: 16px;
}
.like-card {
  background: #fff;
  border-radius: 8px;
  padding: 16px;
  box-shadow: 0 2px 8px rgba(0,0,0,0.06);
  cursor: pointer;
  transition: transform 0.2s, box-shadow 0.2s;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  min-height: 100px;
}
.like-card:hover {
  transform: translateY(-4px);
  box-shadow: 0 4px 16px rgba(0,0,0,0.12);
}
.like-card .post-title {
  font-weight: 500;
  font-size: 15px;
  overflow: hidden;
  text-overflow: ellipsis;
  display: -webkit-box;
  -webkit-line-clamp: 2;
  -webkit-box-orient: vertical;
}
.like-card .post-meta {
  margin-top: 12px;
  font-size: 12px;
  color: #999;
  display: flex;
  justify-content: space-between;
}
.like-card .board-name {
  background: #ecf5ff;
  color: #409EFF;
  padding: 2px 8px;
  border-radius: 12px;
}
.like-card .post-time {
  color: #bbb;
}
.my-page {
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
}

.user-card {
  background: #fff;
  border-radius: 12px;
  padding: 24px 30px;
  margin-bottom: 30px;
  box-shadow: 0 2px 12px rgba(0,0,0,0.06);
  display: flex;
  justify-content: space-between;
  align-items: center;
  flex-wrap: wrap;
  gap: 20px;
}
.user-info-left {
  display: flex;
  align-items: center;
  gap: 16px;
}
.user-name h2 {
  margin: 0;
  font-size: 22px;
}
.user-role {
  font-size: 13px;
  color: #999;
  background: #f5f5f5;
  padding: 2px 10px;
  border-radius: 12px;
}
.user-info-right {
  display: flex;
  gap: 30px;
}
.stat-item {
  font-size: 15px;
  color: #555;
  cursor: pointer;
  padding: 4px 12px;
  border-radius: 4px;
  transition: background-color 0.2s, color 0.2s;
}
.stat-item:hover {
  background-color: #ecf5ff;
  color: #409EFF;
}
.stat-item strong {
  font-size: 20px;
  margin: 0 4px;
}

.my-tabs {
  margin-top: 10px;
}
.my-tabs :deep(.el-tabs__header) {
  border-bottom: 2px solid #e8e8e8;
}
.my-tabs :deep(.el-tabs__item) {
  font-size: 16px;
  font-weight: 500;
}

.loading, .empty {
  text-align: center;
  color: #999;
  padding: 40px 0;
}

.replies-grid {
  display: grid;
  grid-template-columns: repeat(5, 1fr);
  gap: 16px;
}
.reply-card {
  background: #fff;
  border-radius: 8px;
  padding: 16px;
  box-shadow: 0 2px 8px rgba(0,0,0,0.06);
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  min-height: 130px;
  transition: transform 0.2s, box-shadow 0.2s;
  position: relative;
}
.reply-card:hover {
  transform: translateY(-4px);
  box-shadow: 0 4px 16px rgba(0,0,0,0.12);
}
.reply-content {
  cursor: pointer;
  flex: 1;
}
.reply-text {
  font-size: 14px;
  color: #333;
  line-height: 1.5;
  overflow: hidden;
  text-overflow: ellipsis;
  display: -webkit-box;
  -webkit-line-clamp: 3;
  -webkit-box-orient: vertical;
  margin-bottom: 8px;
}
.reply-meta {
  font-size: 12px;
  color: #999;
  display: flex;
  justify-content: space-between;
}
.post-title {
  color: #409EFF;
  max-width: 60%;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}
.reply-time {
  color: #bbb;
}
.delete-btn {
  position: absolute;
  bottom: 8px;
  right: 8px;
  font-size: 18px;
  color: #f56c6c;
}
.delete-btn:hover {
  color: #f00;
}

/* 浏览记录部分（与原样式一致） */
.board-section {
  margin-bottom: 30px;
}
.board-section h3 {
  margin-bottom: 16px;
  color: #333;
  border-left: 4px solid #409EFF;
  padding-left: 12px;
}
.posts-grid {
  display: grid;
  grid-template-columns: repeat(5, 1fr);
  gap: 16px;
}
.post-card {
  background: #fff;
  border-radius: 8px;
  padding: 16px;
  box-shadow: 0 2px 8px rgba(0,0,0,0.06);
  cursor: pointer;
  transition: transform 0.2s, box-shadow 0.2s;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  min-height: 100px;
}
.post-card:hover {
  transform: translateY(-4px);
  box-shadow: 0 4px 16px rgba(0,0,0,0.12);
}
.post-title {
  font-weight: 500;
  font-size: 15px;
  overflow: hidden;
  text-overflow: ellipsis;
  display: -webkit-box;
  -webkit-line-clamp: 2;
  -webkit-box-orient: vertical;
}
.post-time {
  margin-top: 10px;
  font-size: 12px;
  color: #bbb;
  text-align: right;
}
</style>