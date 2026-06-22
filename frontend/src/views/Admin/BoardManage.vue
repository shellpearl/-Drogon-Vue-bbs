<template>
  <div>
    <h2>板块管理</h2>
    <el-form @submit.prevent="createBoard">
      <el-input v-model="newBoardName" placeholder="板块名称" />
      <el-input v-model="newBoardDesc" placeholder="描述" />
      <el-button type="primary" native-type="submit">创建</el-button>
    </el-form>
    <div v-for="board in boards" :key="board.id">
      {{ board.board_name }} - {{ board.description }}
      <el-button @click="removeBoard(board.id)" size="small">删除</el-button>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { getBoardList, createBoard as apiCreateBoard, deleteBoard } from '@/api/board'

interface Board {
  id: number
  board_name: string
  description: string
  created_at: string
}

const boards = ref<Board[]>([])
const newBoardName = ref('')
const newBoardDesc = ref('')

const fetchBoards = async () => {
  const res = await getBoardList()
  boards.value = res.data
}

const createBoard = async () => {
  await apiCreateBoard({ board_name: newBoardName.value, description: newBoardDesc.value })
  newBoardName.value = ''
  newBoardDesc.value = ''
  await fetchBoards()
}

const removeBoard = async (id: number) => {
  await deleteBoard(id)
  await fetchBoards()
}

onMounted(fetchBoards)
</script>