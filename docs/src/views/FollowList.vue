<template>
  <div class="follow-list-page">
    <h2>{{ title }}</h2>
    <div v-if="loading" class="loading">加载中...</div>
    <div v-else-if="users.length === 0" class="empty">暂无用户</div>
    <div v-else class="user-grid">
      <div v-for="user in users" :key="user.id" class="user-card">
        <div class="user-info" @click="goToUser(user.id)">
          <el-avatar :size="48" :src="user.avatar || ''">
            {{ user.username?.charAt(0) || 'U' }}
          </el-avatar>
          <span class="username">{{ user.username }}</span>
        </div>
        <el-button
            v-if="userStore.token && user.id !== userStore.userId"
            size="small"
            :type="user.is_following ? 'default' : 'primary'"
            @click="toggleFollowHandler(user)"
        >
          {{ user.is_following ? '取消关注' : '关注' }}
        </el-button>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, onMounted } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { getFollowingList, getFollowersList, toggleFollow } from '@/api/follow'
import { useUserStore } from '@/stores/user'
import { ElMessage } from 'element-plus'

const route = useRoute()
const router = useRouter()
const userStore = useUserStore()

const type = computed(() => route.params.type as 'following' | 'followers')
const targetUserId = Number(route.query.userId) || 0
const isSelf = targetUserId === 0 || targetUserId === userStore.userId

const title = computed(() => {
  if (isSelf) {
    return type.value === 'following' ? '我关注的' : '我的粉丝'
  } else {
    return type.value === 'following' ? 'TA的关注' : 'TA的粉丝'
  }
})

const users = ref<any[]>([])
const loading = ref(true)

const fetchList = async () => {
  try {
    let res
    if (type.value === 'following') {
      res = await getFollowingList(targetUserId || undefined)
    } else {
      res = await getFollowersList(targetUserId || undefined)
    }
    users.value = res.data || []
  } catch (error) {
    ElMessage.error('获取列表失败')
  } finally {
    loading.value = false
  }
}

const toggleFollowHandler = async (user: any) => {
  try {
    await toggleFollow({
      followee_id: user.id,
      followee_type: user.type || 'student'
    })
    user.is_following = !user.is_following
    ElMessage.success(user.is_following ? '关注成功' : '取消关注成功')
  } catch (error) {
    ElMessage.error('操作失败')
  }
}

const goToUser = (userId: number) => {
  router.push(`/user/${userId}`)
}

onMounted(fetchList)
</script>

<style scoped>
.follow-list-page {
  max-width: 900px;
  margin: 0 auto;
  padding: 20px;
}
.loading, .empty {
  text-align: center;
  padding: 40px 0;
  color: #999;
}
.user-grid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 16px;
}
.user-card {
  background: #fff;
  padding: 16px;
  border-radius: 8px;
  box-shadow: 0 2px 8px rgba(0,0,0,0.06);
  display: flex;
  align-items: center;
  justify-content: space-between;
}
.user-info {
  display: flex;
  align-items: center;
  gap: 12px;
  cursor: pointer;
  flex: 1;
}
.username {
  font-weight: 500;
  color: #333;
}
.username:hover {
  color: #409EFF;
}
</style>