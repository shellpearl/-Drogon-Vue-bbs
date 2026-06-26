<template>
  <div class="board-manage">
    <el-card class="create-card" shadow="hover">
      <template #header>
        <div class="card-header">
          <span class="title">📁 创建新板块</span>
        </div>
      </template>
      <el-form @submit.prevent="createBoard" label-width="80px">
        <el-form-item label="板块名称">
          <el-input v-model="newBoardName" placeholder="请输入板块名称" clearable />
        </el-form-item>
        <el-form-item label="板块描述">
          <el-input v-model="newBoardDesc" placeholder="请输入板块描述" clearable />
        </el-form-item>
        <el-form-item>
          <el-button type="primary" native-type="submit" :disabled="!newBoardName.trim()">
            创建板块
          </el-button>
        </el-form-item>
      </el-form>
    </el-card>

    <el-card class="list-card" shadow="hover">
      <template #header>
        <div class="card-header">
          <span class="title">📋 板块列表</span>
          <el-tag type="info" size="small">共 {{ boards.length }} 个板块</el-tag>
        </div>
      </template>
      <el-table :data="boards" style="width:100%" stripe>
        <el-table-column prop="id" label="ID" width="80" align="center" />
        <el-table-column prop="board_name" label="板块名称" min-width="150" />
        <el-table-column prop="description" label="描述" min-width="200" />
        <el-table-column prop="created_at" label="创建时间" width="180" align="center">
          <template #default="{ row }">
            {{ formatTime(row.created_at) }}
          </template>
        </el-table-column>
        <el-table-column label="操作" width="120" align="center">
          <template #default="{ row }">
            <el-button type="danger" size="small" plain @click="removeBoard(row.id)">
              删除
            </el-button>
          </template>
        </el-table-column>
      </el-table>
      <div v-if="boards.length === 0" class="empty-tip">暂无板块，请创建一个</div>
    </el-card>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { getBoardList, createBoard as apiCreateBoard, deleteBoard } from '@/api/board'
import { ElMessage, ElMessageBox } from 'element-plus'

interface Board {
  id: number
  board_name: string
  description: string
  created_at: string
}

const boards = ref<Board[]>([])
const newBoardName = ref('')
const newBoardDesc = ref('')

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

const fetchBoards = async () => {
  try {
    const res = await getBoardList()
    boards.value = res.data
  } catch (error) {
    ElMessage.error('获取板块列表失败')
  }
}

const createBoard = async () => {
  if (!newBoardName.value.trim()) {
    ElMessage.warning('请输入板块名称')
    return
  }
  try {
    await apiCreateBoard({
      board_name: newBoardName.value.trim(),
      description: newBoardDesc.value.trim()
    })
    ElMessage.success('板块创建成功')
    newBoardName.value = ''
    newBoardDesc.value = ''
    await fetchBoards()
  } catch (error) {
    ElMessage.error('创建失败，请检查板块名称是否重复')
  }
}

const removeBoard = async (id: number) => {
  try {
    await ElMessageBox.confirm('确定要删除该板块吗？删除后板块下的所有帖子也将被删除！', '警告', {
      confirmButtonText: '确定删除',
      cancelButtonText: '取消',
      type: 'warning'
    })
    await deleteBoard(id)
    ElMessage.success('删除成功')
    await fetchBoards()
  } catch (error) {
    if (error !== 'cancel') {
      ElMessage.error('删除失败')
    }
  }
}

onMounted(fetchBoards)
</script>

<style scoped>
.board-manage {
  max-width: 1000px;
  margin: 0 auto;
  padding: 20px;
}

.create-card {
  margin-bottom: 24px;
  border-radius: 12px;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.06);
}

.list-card {
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