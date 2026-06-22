<template>
  <div class="profile-page">
    <div class="profile-card">
      <div class="profile-header">
        <el-avatar :size="80" :src="userInfo.avatar || ''">
          {{ userInfo.username?.charAt(0) || 'U' }}
        </el-avatar>
        <div class="profile-name">
          <h2>{{ userInfo.username }}</h2>
          <el-button type="primary" size="small" @click="showDetail = !showDetail">
            {{ showDetail ? '收起' : '详情' }}
          </el-button>
          <el-button type="success" size="small" @click="goToEdit">
            编辑
          </el-button>
        </div>
      </div>
      <div v-if="showDetail" class="profile-detail">
        <el-descriptions :column="2" border>
          <el-descriptions-item label="姓名">{{ userInfo.name || '未设置' }}</el-descriptions-item>
          <el-descriptions-item label="性别">{{ userInfo.gender || '未设置' }}</el-descriptions-item>
          <el-descriptions-item label="生日">{{ userInfo.birthday || '未设置' }}</el-descriptions-item>
          <el-descriptions-item label="专业">{{ userInfo.major || '未设置' }}</el-descriptions-item>
          <el-descriptions-item label="账号">{{ userInfo.account || '未设置'}}</el-descriptions-item>
        </el-descriptions>
      </div>
    </div>

    <div class="posts-section">
      <h3>我的帖子</h3>
      <div v-if="posts.length === 0" class="empty-posts">暂无帖子</div>
      <div v-else class="posts-grid">
        <div
            v-for="post in posts"
            :key="post.id"
            class="post-card"
            @click="goToPost(post.id)"
        >
          <div class="post-title">{{ post.title }}</div>
          <div class="post-meta">
            <span class="board-name">{{ post.board_name }}</span>
            <span class="post-time">{{ post.post_time }}</span>
          </div>
          <el-button
              type="danger"
              size="small"
              text
              class="delete-btn"
              @click.stop="handleDeletePost(post.id)"
          >
            <el-icon><Delete /></el-icon>
          </el-button>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import { getUserInfo, getMyPosts } from '@/api/user'
import { deletePost } from '@/api/post'
import { getFollowCounts } from '@/api/follow'
import { useUserStore } from '@/stores/user'
import { ElMessage, ElMessageBox } from 'element-plus'
import { Delete } from '@element-plus/icons-vue'

interface Post {
  id: number
  title: string
  board_name: string
  post_time: string
}

const router = useRouter()
const userStore = useUserStore()

const userInfo = ref({
  account: '',
  username: '',
  name: '',
  gender: '',
  birthday: '',
  major: '',
  avatar: ''
})

const posts = ref<Post[]>([])
const showDetail = ref(false)
const followStats = ref({ following: 0, followers: 0 })

const fetchUserInfo = async () => {
  try {
    const res = await getUserInfo()
    userInfo.value = res.data
  } catch (error) {
    ElMessage.error('获取用户信息失败')
  }
}

const fetchPosts = async () => {
  try {
    const res = await getMyPosts()
    posts.value = res.data
  } catch (error) {
    ElMessage.error('获取帖子失败')
  }
}

const fetchFollowStats = async () => {
  try {
    const res = await getFollowCounts(userStore.userId)
    followStats.value = res.data
  } catch (error) {
  }
}

const goToPost = (id: number) => {
  router.push(`/post/${id}`)
}

const goToEdit = () => {
  router.push('/profile/edit')
}

const handleDeletePost = async (postId: number) => {
  try {
    await ElMessageBox.confirm('您确定删除该条帖子吗？', '提示', {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      type: 'warning'
    })
    await deletePost(postId)
    ElMessage.success('删除成功')
    await fetchPosts()
  } catch (error) {
    if (error !== 'cancel') {
      ElMessage.error('删除失败')
    }
  }
}

onMounted(() => {
  fetchUserInfo()
  fetchPosts()
  fetchFollowStats()
})
</script>

<style scoped>
.post-card {
  position: relative;
  background: #fff;
  border-radius: 8px;
  padding: 16px;
  box-shadow: 0 2px 8px rgba(0,0,0,0.06);
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  min-height: 120px;
  transition: transform 0.2s, box-shadow 0.2s;
}
.post-card:hover {
  transform: translateY(-4px);
  box-shadow: 0 4px 16px rgba(0,0,0,0.12);
}
.post-content {
  cursor: pointer;
  flex: 1;
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
.post-meta {
  margin-top: 12px;
  font-size: 12px;
  color: #999;
  display: flex;
  justify-content: space-between;
}
.board-name {
  background: #ecf5ff;
  color: #409EFF;
  padding: 2px 8px;
  border-radius: 12px;
}
.post-time {
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

.profile-page {
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
}

.profile-card {
  background: #fff;
  border-radius: 12px;
  padding: 30px;
  box-shadow: 0 2px 12px rgba(0,0,0,0.06);
  margin-bottom: 30px;
}
.profile-header {
  display: flex;
  align-items: center;
  gap: 20px;
}
.profile-name {
  display: flex;
  align-items: center;
  gap: 15px;
}
.profile-name h2 {
  margin: 0;
}
.profile-detail {
  margin-top: 20px;
}

.posts-section h3 {
  margin-bottom: 16px;
}
.empty-posts {
  text-align: center;
  color: #999;
  padding: 40px 0;
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
  min-height: 120px;
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
.post-meta {
  margin-top: 12px;
  font-size: 12px;
  color: #999;
  display: flex;
  justify-content: space-between;
}
.board-name {
  background: #ecf5ff;
  color: #409EFF;
  padding: 2px 8px;
  border-radius: 12px;
}
.post-time {
  color: #bbb;
}
</style>