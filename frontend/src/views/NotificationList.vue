<template>
  <div>
    <h2>消息中心</h2>
    <div style="margin-bottom: 16px;">
      <el-button @click="markAll" type="primary" size="small">全部已读</el-button>
      <el-tag v-if="store.connected" type="success" size="small" style="margin-left: 8px;">
        <el-icon><Connection /></el-icon> 实时连接
      </el-tag>
      <el-tag v-else type="danger" size="small" style="margin-left: 8px;">
        <el-icon><CircleClose /></el-icon> 未连接
      </el-tag>
    </div>

    <el-table :data="notifications" style="width:100%">
      <el-table-column prop="content" label="内容" />
      <el-table-column label="时间" width="180" >
        <template #default="{ row }">
          {{ formatTime(row.created_at) }}
        </template>
      </el-table-column>
      <el-table-column label="状态" width="120">
        <template #default="{ row }">
          <el-tag v-if="row.is_read" type="success" size="small">已读</el-tag>
          <el-tag v-else type="warning" size="small">未读</el-tag>
        </template>
      </el-table-column>
      <el-table-column label="操作" width="200">
        <template #default="{ row }">
          <el-button
              v-if="!row.is_read"
              @click="markRead(row.id)"
              size="small"
              type="primary"
              plain
          >
            标记已读
          </el-button>
          <el-button @click="goToPost(row)" size="small" type="info" plain>
            查看帖子
          </el-button>
        </template>
      </el-table-column>
    </el-table>

    <div v-if="notifications.length === 0" style="text-align:center;color:#999;padding:40px 0;">
      暂无消息 🕊️
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { useRouter } from 'vue-router'
import { markAsRead as apiMarkAsRead, markAllRead as apiMarkAllRead } from '@/api/notification'
import { ElMessage } from 'element-plus'
import {CircleClose, Connection} from '@element-plus/icons-vue'
import { useNotificationStore } from '@/stores/notification'

const router = useRouter()
const store = useNotificationStore()
const notifications = computed(() => store.notifications)

const formatTime = (timeStr: string) => {
  if (!timeStr) return ''
  try {
    const d = new Date(timeStr.replace(' ', 'T'))
    if (isNaN(d.getTime())) return timeStr
    const pad = (n: number) => String(n).padStart(2, '0')
    return `${d.getFullYear()}-${pad(d.getMonth()+1)}-${pad(d.getDate())} ${pad(d.getHours())}:${pad(d.getMinutes())}:${pad(d.getSeconds())}`
  } catch {
    return timeStr
  }
}

const markRead = async (id: number) => {
  try {
    await apiMarkAsRead(id)
    store.markAsRead(id)
    ElMessage.success('已标记为已读')
  } catch (error) {
    ElMessage.error('标记失败')
  }
}

const markAll = async () => {
  try {
    await apiMarkAllRead()
    store.markAllRead()
    ElMessage.success('全部已读')
  } catch (error) {
    ElMessage.error('操作失败')
  }
}

const goToPost = async (row: any) => {
  if (!row.is_read) {
    store.markAsRead(row.id)
    apiMarkAsRead(row.id).catch(() => {})
  }
  router.push(`/post/${row.post_id}`)
}
</script>