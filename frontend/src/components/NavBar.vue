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
import { onMounted, onUnmounted, computed, watch } from 'vue'
import { useRouter, useRoute } from 'vue-router'
import { useUserStore } from '@/stores/user'
import { ElMessageBox, ElMessage } from 'element-plus'
import { useNotificationStore } from '@/stores/notification'

const router = useRouter()
const route = useRoute()
const userStore = useUserStore()
const notificationStore = useNotificationStore()
const unreadCount = computed(() => notificationStore.unreadCount)

const activeMenu = computed(() => route.path)

let ws: WebSocket | null = null
let heartbeatTimer: number | null = null
let reconnectTimer: number | null = null

const connectNotificationWS = () => {
  if (ws) {
    ws.close()
    ws = null
  }
  const token = localStorage.getItem('token')
  if (!token) {
    return
  }
  const wsUrl = `ws://localhost:8080/api/notifications/ws?token=${token}`
  ws = new WebSocket(wsUrl)

  ws.onopen = () => {
    console.log('🔔 Notification WebSocket connected')
    notificationStore.setConnected(true)
    if (heartbeatTimer) clearInterval(heartbeatTimer)
    heartbeatTimer = window.setInterval(() => {
      if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send('ping')
      }
    }, 30000)
  }

  ws.onmessage = (event) => {
    try {
      const data = JSON.parse(event.data)
      if (data.type === 'history') {
        notificationStore.setNotifications(data.data || [])
      } else if (data.type === 'new') {
        notificationStore.addNotification(data.data)
        ElMessage.info(`📩 ${data.data.content}`)
      }
    } catch (e) {
      console.error('解析消息失败', e)
    }
  }

  ws.onclose = () => {
    console.log('🔔 Notification WebSocket closed')
    notificationStore.setConnected(false)
    if (heartbeatTimer) {
      clearInterval(heartbeatTimer)
      heartbeatTimer = null
    }
    if (reconnectTimer) clearTimeout(reconnectTimer)
    reconnectTimer = window.setTimeout(connectNotificationWS, 5000)
  }

  ws.onerror = (error) => {
    console.error('Notification WS error', error)
    ws?.close()
  }
}

watch(() => userStore.token, (newToken, oldToken) => {
  if (newToken && !oldToken) {
    connectNotificationWS()
  } else if (!newToken && oldToken) {
    if (ws) {
      ws.close()
      ws = null
    }
    notificationStore.setConnected(false)
  }
}, { immediate: false })


const handleLogout = async () => {
  try {
    await ElMessageBox.confirm('确认退出登录吗？', '提示', {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      type: 'warning',
    })
    userStore.logout()
    notificationStore.reset()
    notificationStore.setConnected(false)
    if (ws) {
      ws.close()
      ws = null
    }
    ElMessage.success('已退出')
    router.push('/login')
  } catch {
  }
}

onMounted(() => {
  if(userStore.token){
    connectNotificationWS()
  }
  notificationStore.fetchUnread()
})

onUnmounted(() => {
  if (ws) ws.close()
  if (heartbeatTimer) clearInterval(heartbeatTimer)
  if (reconnectTimer) clearTimeout(reconnectTimer)
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