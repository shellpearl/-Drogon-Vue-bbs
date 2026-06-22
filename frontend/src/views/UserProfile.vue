<template>
  <div class="user-profile-page">
    <div v-if="loading" class="loading">加载中...</div>
    <div v-else-if="!user" class="error">用户不存在</div>
    <div v-else>
      <!-- 用户信息卡片 -->
      <div class="user-card">
        <div class="user-left">
          <el-avatar :size="80" :src="user.avatar || ''">
            {{ user.username?.charAt(0) || 'U' }}
          </el-avatar>
          <div class="user-detail">
            <h2>{{ user.username }}</h2>
            <span class="user-type">{{ user.type === 'admin' ? '管理员' : '学生' }}</span>
          </div>
        </div>
        <div class="user-right">
          <div class="stats">
            <span @click="goToFollowList('following')">关注 <strong>{{ user.following_count || 0 }}</strong></span>
            <span @click="goToFollowList('followers')">粉丝 <strong>{{ user.follower_count || 0 }}</strong></span>
          </div>
          <el-button
              v-if="userStore.token && user.id !== userStore.userId"
              :type="isFollowing ? 'default' : 'primary'"
              @click="handleToggleFollow"
          >
            {{ isFollowing ? '取消关注' : '关注' }}
          </el-button>
        </div>
      </div>

      <div class="user-posts" v-if="userPosts.length">
        <h3>TA的帖子</h3>
        <div class="posts-grid">
          <div
              v-for="post in userPosts"
              :key="post.id"
              class="post-card"
              @click="goToPost(post.id)"
          >
            <div class="post-title">{{ post.title }}</div>
            <div class="post-meta">
              <span>{{ post.post_time }}</span>
            </div>
          </div>
        </div>
      </div>
      <div v-else-if="!loading" class="empty">该用户暂无帖子</div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { getPublicUser } from '@/api/user'
import { checkFollow, toggleFollow } from '@/api/follow'
import { getPostsByAuthor } from '@/api/post'
import { useUserStore } from '@/stores/user'
import { ElMessage } from 'element-plus'

const route = useRoute()
const router = useRouter()
const userStore = useUserStore()

const userId = Number(route.params.userId)
const user = ref<any>(null)
const userPosts = ref<any[]>([])
const isFollowing = ref(false)
const loading = ref(true)

const fetchUserPosts = async () => {
  try {
    const res = await getPostsByAuthor(userId)
    userPosts.value = res.data
  } catch (error) {
  }
}

onMounted(async () => {
  await fetchUser()
  await fetchUserPosts()
})

const fetchUser = async () => {
  try {
    const res = await getPublicUser(userId)
    user.value = res.data
    if (userStore.token && userStore.userId !== userId) {
      const checkRes = await checkFollow(userId, user.value.type)
      isFollowing.value = checkRes.data.is_following
    }
  } catch (error) {
    ElMessage.error('获取用户信息失败')
  } finally {
    loading.value = false
  }
}

const handleToggleFollow = async () => {
  try {
    await toggleFollow({
      followee_id: userId,
      followee_type: user.value.type
    })
    isFollowing.value = !isFollowing.value
    ElMessage.success(isFollowing.value ? '关注成功' : '取消关注成功')
    if (isFollowing.value) {
      user.value.following_count = (user.value.following_count || 0) + 1
    } else {
      user.value.following_count = Math.max((user.value.following_count || 0) - 1, 0)
    }
  } catch (error) {
    ElMessage.error('操作失败')
  }
}

const goToFollowList = (type: 'following' | 'followers') => {
  router.push({
    path: `/follow/${type}`,
    query: { userId: String(userId) }
  })
}

const goToPost = (postId: number) => {
  router.push(`/post/${postId}`)
}

onMounted(fetchUser)
</script>

<style scoped>
.user-profile-page {
  max-width: 900px;
  margin: 0 auto;
  padding: 20px;
}
.loading, .error, .empty {
  text-align: center;
  padding: 40px 0;
  color: #999;
}
.user-card {
  background: #fff;
  border-radius: 12px;
  padding: 30px;
  box-shadow: 0 2px 12px rgba(0,0,0,0.06);
  display: flex;
  justify-content: space-between;
  align-items: center;
  flex-wrap: wrap;
  gap: 20px;
}
.user-left {
  display: flex;
  align-items: center;
  gap: 20px;
}
.user-detail h2 {
  margin: 0 0 4px 0;
}
.user-type {
  font-size: 14px;
  color: #999;
  background: #f5f5f5;
  padding: 2px 12px;
  border-radius: 12px;
}
.user-right {
  display: flex;
  align-items: center;
  gap: 30px;
}
.stats {
  display: flex;
  gap: 20px;
}
.stats span {
  cursor: pointer;
  color: #555;
}
.stats span:hover {
  color: #409EFF;
}
.stats strong {
  font-size: 18px;
  margin: 0 4px;
}
.user-posts {
  margin-top: 30px;
}
.user-posts h3 {
  margin-bottom: 16px;
}
.posts-grid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 16px;
}
.post-card {
  background: #fff;
  padding: 16px;
  border-radius: 8px;
  box-shadow: 0 2px 8px rgba(0,0,0,0.06);
  cursor: pointer;
  transition: transform 0.2s;
}
.post-card:hover {
  transform: translateY(-4px);
}
.post-title {
  font-weight: 500;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}
.post-meta {
  margin-top: 8px;
  font-size: 12px;
  color: #999;
}
</style>