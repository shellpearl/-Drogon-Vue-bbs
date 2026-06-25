<template>
  <div>
    <h2>消息中心</h2>
    <div style="margin-bottom: 16px;">
      <template v-if="!editMode">
        <el-button @click="markAll" type="primary" size="small">全部已读</el-button>
        <el-button @click="refresh" size="small" type="default">
          <el-icon><Refresh /></el-icon> 刷新
        </el-button>
        <el-button @click="editMode = true" size="small" type="warning">编辑</el-button>
      </template>
      <template v-else>
        <el-button @click="deleteSelected" type="danger" size="small">删除</el-button>
        <el-button @click="editMode = false" size="small" type="default">取消</el-button>
        <el-button @click="selectAll" size="small" type="info">全选</el-button>
        <el-button @click="selectRead" size="small" type="info">选中已读</el-button>
        <span style="margin-left: 12px; font-size: 14px; color: #555;">
          已选 {{ selectedIds.length }} 条
        </span>
      </template>
    </div>

    <el-table
        ref="tableRef"
        :data="notifications"
        style="width:100%"
        @selection-change="handleSelectionChange"
    >
      <el-table-column v-if="editMode" type="selection" width="55" />
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

          <el-button
              v-if="row.post_id > 0"
              @click="goToPost(row)"
              size="small"
              type="info"
              plain
          >
            查看帖子
          </el-button>

          <el-button
              v-if="row.type === 'visit' && row.sender_id"
              @click="goToUser(row.sender_id)"
              size="small"
              type="success"
              plain
          >
            查看用户
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
import { computed, ref, onMounted, onUnmounted } from 'vue'
import { useRouter } from 'vue-router'
import { markAsRead as apiMarkAsRead, markAllRead as apiMarkAllRead, deleteBatch } from '@/api/notification'
import { useNotificationStore } from '@/stores/notification'
import { ElMessage, ElMessageBox } from 'element-plus'
import { Refresh } from '@element-plus/icons-vue'
import type { ElTable } from 'element-plus'

const router = useRouter()
const store = useNotificationStore()
const notifications = computed(() => store.notifications)

const editMode = ref(false)
const selectedIds = ref<number[]>([])
const tableRef = ref<any>(null)

let pollTimer: number | null = null

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

const refresh = () => {
  store.fetchNotifications()
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

const handleSelectionChange = (selection: any[]) => {
  selectedIds.value = selection.map(item => item.id)
}

const selectAll = () => {
  if (tableRef.value) {
    tableRef.value.clearSelection()
    notifications.value.forEach(row => {
      tableRef.value?.toggleRowSelection(row, true)
    })
  }
}

const selectRead = () => {
  if (tableRef.value) {
    tableRef.value.clearSelection()
    notifications.value.forEach(row => {
      if (row.is_read) {
        tableRef.value?.toggleRowSelection(row, true)
      }
    })
  }
}

const deleteSelected = async () => {
  if (selectedIds.value.length === 0) {
    ElMessage.warning('请至少选择一条消息')
    return
  }
  try {
    await ElMessageBox.confirm(`确认删除选中的 ${selectedIds.value.length} 条消息吗？`, '提示', {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      type: 'warning'
    })
    await deleteBatch(selectedIds.value)
    ElMessage.success('删除成功')
    editMode.value = false
    selectedIds.value = []
    await refresh()
  } catch (error) {
    if (error !== 'cancel') {
      ElMessage.error('删除失败')
    }
  }
}

const goToUser = (userId: number) => {
  router.push(`/user/${userId}`)
}

onMounted(() => {
  store.fetchNotifications()
  pollTimer = window.setInterval(() => {
    store.fetchNotifications()
  }, 10000)
})

onUnmounted(() => {
  if (pollTimer) {
    clearInterval(pollTimer)
    pollTimer = null
  }
})
</script>