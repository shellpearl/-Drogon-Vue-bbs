<template>
  <div class="post-detail-page">
    <div v-if="error" class="error-container">
      <el-icon class="error-icon"><CircleClose /></el-icon>
      <p class="error-message">该帖子已被删除或不存在</p>
      <el-button @click="$router.back()" type="primary" plain>返回</el-button>
    </div>

    <template v-else>
      <el-card class="post-card" shadow="hover">
        <template #header>
          <div class="post-header">
            <div class="post-author">
              <el-avatar :size="40" :src="post.author_avatar || ''">
                {{ post.author_name?.charAt(0) || 'U' }}
              </el-avatar>
              <div class="author-info">
                <span class="author-name">{{ post.author_name }}</span>
                <span class="post-time">{{ post.post_time }}</span>
              </div>
            </div>
          </div>
        </template>
        <h2 class="post-title">{{ post.title }}</h2>
        <div class="post-content">{{ post.content }}</div>

        <div class="post-like">
          <div
              class="like-container"
              :class="{ 'liked': post.is_liked, 'disabled': !userStore.token }"
              @click="handleLike"
          >
            <div class="like-icon">
              <span class="heart-symbol">{{ post.is_liked ? '♥' : '♡' }}</span>
            </div>
            <div class="like-count">{{ post.like_count }}</div>
          </div>
        </div>
      </el-card>

      <el-card class="reply-card" shadow="hover">
        <template #header>
          <span class="reply-title">回复（{{ replies.length }}）</span>
        </template>

        <div v-if="replies.length === 0" class="empty-replies">暂无回复，快来抢沙发吧 🛋️</div>
        <el-timeline v-else>
          <el-timeline-item
              v-for="reply in replies"
              :key="reply.id"
              :timestamp="reply.reply_time"
              placement="top"
              size="large"
          >
            <div class="reply-item">
              <el-avatar :size="32" class="reply-avatar" :src="reply.student_avatar || ''">
                {{ reply.student_name?.charAt(0) || 'U' }}
              </el-avatar>
              <div class="reply-body">
                <span class="reply-author">{{ reply.student_name }}</span>
                <div class="reply-content">{{ reply.content }}</div>
              </div>
            </div>
          </el-timeline-item>
        </el-timeline>

        <div v-if="userStore.token" class="reply-input-area">
          <el-input
              v-model="replyContent"
              type="textarea"
              :rows="3"
              placeholder="写下你的回复..."
              resize="none"
          />
          <el-button type="primary" @click="submitReply" class="submit-btn">
            发布回复
          </el-button>
        </div>
        <div v-else class="login-tip">
          <el-alert title="请登录后回复" type="info" :closable="false" show-icon />
        </div>
      </el-card>
    </template>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { useRoute } from 'vue-router'
import { getPostDetail } from '@/api/post'
import { createReply } from '@/api/reply'
import { toggleLike } from '@/api/like'
import { useUserStore } from '@/stores/user'
import { ElMessage } from 'element-plus'

interface Reply {
  id: number
  content: string
  student_name: string
  student_avatar: string
  reply_time: string
}

interface PostDetail {
  id: number
  title: string
  content: string
  author_name: string
  author_avatar: string
  post_time: string
  like_count: number
  is_liked: boolean
}

const route = useRoute()
const userStore = useUserStore()
const postId = Number(route.params.postId)
const post = ref<PostDetail>({
  id: 0,
  title: '',
  content: '',
  author_name: '',
  author_avatar: '',
  post_time: '',
  like_count: 0,
  is_liked: false
})
const replies = ref<Reply[]>([])
const replyContent = ref('')
const error = ref(false)
const loading = ref(true)

const fetchDetail = async () => {
  loading.value = true
  try {
    const res = await getPostDetail(postId)
    if (res.data && res.data.post && res.data.post.id) {
      post.value = res.data.post
      replies.value = res.data.replies || []
      error.value = false
    } else {
      error.value = true
    }
  } catch (err) {
    error.value = true
  } finally {
    loading.value = false
  }
}

const submitReply = async () => {
  if (!replyContent.value.trim()) return
  try {
    await createReply({ postId, content: replyContent.value })
    ElMessage.success('回复成功')
    replyContent.value = ''
    await fetchDetail()
  } catch (error) {
    ElMessage.error('回复失败')
  }
}

const handleLike = async () => {
  if (!userStore.token) {
    ElMessage.warning('请先登录')
    return
  }
  try {
    await toggleLike(postId)
    post.value.is_liked = !post.value.is_liked
    post.value.like_count += post.value.is_liked ? 1 : -1
    ElMessage.success(post.value.is_liked ? '点赞成功' : '取消点赞成功')
  } catch (error) {
    ElMessage.error('操作失败')
  }
}


onMounted(fetchDetail)
</script>

<style scoped>
.post-detail-page {
  max-width: 900px;
  margin: 0 auto;
  padding: 20px;
}
.error-container {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  height: 60vh;
  color: #999;
}
.error-icon {
  font-size: 64px;
  color: #d9d9d9;
}
.error-message {
  font-size: 18px;
  margin: 16px 0 24px;
  color: #666;
}
.post-card {
  margin-bottom: 24px;
}
.post-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}
.post-author {
  display: flex;
  align-items: center;
  gap: 12px;
}
.author-info {
  display: flex;
  flex-direction: column;
}
.author-name {
  font-weight: 600;
  font-size: 16px;
}
.post-time {
  font-size: 13px;
  color: #999;
}
.post-title {
  margin: 8px 0 12px 0;
  font-size: 24px;
  font-weight: 700;
  color: #1e1e1e;
}
.post-content {
  font-size: 16px;
  line-height: 1.8;
  color: #333;
  white-space: pre-wrap;
  word-break: break-word;
}

.reply-card {
  margin-top: 20px;
}
.reply-title {
  font-weight: 600;
  font-size: 18px;
}
.empty-replies {
  text-align: center;
  color: #999;
  padding: 20px 0;
}

.reply-item {
  display: flex;
  align-items: flex-start;
  gap: 12px;
}
.reply-avatar {
  flex-shrink: 0;
}
.reply-body {
  flex: 1;
}
.reply-author {
  font-weight: 600;
  font-size: 14px;
  color: #333;
}
.reply-content {
  margin-top: 4px;
  font-size: 15px;
  color: #555;
  line-height: 1.6;
}

.reply-input-area {
  margin-top: 24px;
}
.reply-input-area :deep(.el-textarea__inner) {
  border-radius: 8px;
}
.submit-btn {
  margin-top: 12px;
  float: right;
}
.login-tip {
  margin-top: 20px;
}
.post-like {
  margin-top: 20px;
  border-top: 1px solid #f0f0f0;
  padding-top: 16px;
  display: flex;
  justify-content: flex-end;
}

.like-container {
  display: flex;
  flex-direction: column;
  align-items: center;
  cursor: pointer;
  transition: transform 0.2s;
  user-select: none;
}

.like-container:hover {
  transform: scale(1.1);
}

.like-container.disabled {
  cursor: not-allowed;
  opacity: 0.6;
}

.heart-symbol {
  font-size: 28px;
  line-height: 1;
  transition: color 0.3s;
  color: #909399;
}

.like-container.liked .heart-symbol {
  color: #f56c6c;
}

.like-count {
  font-size: 12px;
  color: #606266;
  margin-top: 2px;
  font-weight: 500;
  transition: color 0.3s;
}

.like-container.liked .like-count {
  color: #f56c6c;
}
</style>