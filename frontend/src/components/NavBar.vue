<template>
  <el-container style="height: 100vh;">

    <el-aside width="200px" style="background-color: #304156; color: #fff; height: 100vh; overflow-y: auto;">
      <div class="logo" style="padding: 20px; text-align: center; font-size: 20px; font-weight: bold;">
        📚 校园BBS
      </div>
      <el-menu
          background-color="#304156"
          text-color="#bfcbd9"
          active-text-color="#409EFF"
          :default-active="activeMenu"
          router
      >
        <el-menu-item index="/">
          <span>首页</span>
        </el-menu-item>
        <template v-if="userStore.token">
          <el-menu-item index="/profile">
            <span>个人中心</span>
          </el-menu-item>
          <el-menu-item index="/my">
            <span>我的</span>
          </el-menu-item>
          <el-menu-item index="/notifications">
            <span>
              信箱
              <el-badge :value="unreadCount" :hidden="unreadCount === 0" class="badge" />
            </span>
          </el-menu-item>
          <template v-if="userStore.role === 'admin'">
            <el-sub-menu index="admin">
              <template #title>管理</template>
              <el-menu-item index="/admin/boards">板块管理</el-menu-item>
              <el-menu-item index="/admin/posts">帖子管理</el-menu-item>
            </el-sub-menu>
          </template>
          <el-menu-item @click="handleLogout">
            <span>退出登录</span>
          </el-menu-item>
        </template>
        <template v-else>
          <el-menu-item index="/login">
            <span>登录</span>
          </el-menu-item>
          <el-menu-item index="/register">
            <span>注册</span>
          </el-menu-item>
        </template>
      </el-menu>
    </el-aside>

    <el-main style="padding: 20px; background: #f0f2f5; height: 100vh; overflow-y: auto;">
      <router-view />
    </el-main>
  </el-container>
</template>

<script setup lang="ts">
import { onMounted, onUnmounted, computed } from 'vue'
import { useRouter, useRoute } from 'vue-router'
import { useUserStore } from '@/stores/user'
import { useNotificationStore } from '@/stores/notification'
import { ElMessageBox, ElMessage } from 'element-plus'

const router = useRouter()
const route = useRoute()
const userStore = useUserStore()
const notificationStore = useNotificationStore()
const unreadCount = computed(() => notificationStore.unreadCount)

const activeMenu = computed(() => route.path)

let pollTimer: number | null = null

const handleLogout = async () => {
  try {
    await ElMessageBox.confirm('确认退出登录吗？', '提示', {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      type: 'warning',
    })
    userStore.logout()
    notificationStore.reset()
    ElMessage.success('已退出')
    router.push('/login')
  } catch {
  }
}

onMounted(() => {
  if (userStore.token) {
    notificationStore.fetchUnread()
    pollTimer = window.setInterval(() => {
      if (userStore.token) {
        notificationStore.fetchUnread()
      }
    }, 10000)
  }
})

onUnmounted(() => {
  if (pollTimer) {
    clearInterval(pollTimer)
    pollTimer = null
  }
})
</script>

<style scoped>
.el-menu {
  border-right: none;
}
.badge {
  margin-left: 8px;
}
.el-menu-item .el-badge {
  display: inline-flex;
  align-items: center;
}
</style>