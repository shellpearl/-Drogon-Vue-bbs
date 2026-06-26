<template>
  <div class="post-manage">
    <el-card class="manage-card" shadow="hover">
      <template #header>
        <div class="card-header">
          <span class="title">📋 帖子管理</span>
          <el-tag type="info" size="small">共 {{ posts.length }} 条帖子</el-tag>
        </div>
      </template>

      <el-table :data="posts" style="width:100%" stripe>
        <el-table-column prop="id" label="ID" width="80" align="center" />
        <el-table-column prop="title" label="标题" min-width="150" />
        <el-table-column prop="author_name" label="作者" width="120" align="center" />
        <el-table-column prop="board_name" label="板块" width="120" align="center" />
        <el-table-column prop="post_time" label="发布时间" width="180" align="center">
          <template #default="{ row }">
            {{ formatTime(row.post_time) }}
          </template>
        </el-table-column>
        <el-table-column label="状态" width="100" align="center">
          <template #default="{ row }">
            <el-tag :type="row.is_deleted ? 'danger' : 'success'" size="small">
              {{ row.is_deleted ? '已删除' : '正常' }}
            </el-tag>
          </template>
        </el-table-column>
        <el-table-column label="操作" width="140" align="center">
          <template #default="{ row }">
            <el-button
                type="danger"
                size="small"
                plain
                @click="deletePost(row.id)"
                :disabled="row.is_deleted"
            >
              {{ row.is_deleted ? '已删除' : '删除' }}
            </el-button>
          </template>
        </el-table-column>
      </el-table>

      <div v-if="posts.length === 0" class="empty-tip">暂无帖子</div>
    </el-card>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import request from '@/api/request'
import { ElMessage, ElMessageBox } from 'element-plus'

interface Post {
  id: number
  title: string
  author_name: string
  board_name: string
  post_time: string
  is_deleted: boolean
}

const posts = ref<Post[]>([])

const formatTime = (timeStr: string) => {
  if (!timeStr) return ''
  try {
    const d = new Date(timeStr.replace(' ', 'T'))
    if (isNaN(d.getTime())) return timeStr
    const pad = (n: number) => String(n).padStart(2, '0')
    return `${d.getFullYear()}-${pad(d.getMonth() + 1)}-${pad(d.getDate())} ${pad(d.getHours())}:${pad(d.getMinutes())}`
  } catch {
    return timeStr
  }
}

const fetchPosts = async () => {
  try {
    const res = await request.get('/admin/posts')
    posts.value = res.data || []
  } catch (error) {
    ElMessage.error('获取帖子列表失败')
  }
}

const deletePost = async (id: number) => {
  try {
    await ElMessageBox.confirm('确定要永久删除该帖子吗？此操作不可恢复！', '警告', {
      confirmButtonText: '确定删除',
      cancelButtonText: '取消',
      type: 'warning'
    })
    await request.delete(`/admin/post/${id}`)
    ElMessage.success('删除成功')
    await fetchPosts()
  } catch (error) {
    if (error !== 'cancel') {
      ElMessage.error('删除失败')
    }
  }
}

onMounted(fetchPosts)
</script>

<style scoped>
.post-manage {
  max-width: 1100px;
  margin: 0 auto;
  padding: 20px;
}

.manage-card {
  border-radius: 12px;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.06);
}

.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.title {
  font-size: 18px;
  font-weight: 600;
  color: #1e1e1e;
}

.empty-tip {
  text-align: center;
  color: #999;
  padding: 30px 0;
  font-size: 14px;
}
</style>