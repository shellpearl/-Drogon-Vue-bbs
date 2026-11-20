<template>
  <div class="home">
    <h1>校园BBS</h1>
    <div v-for="board in boards" :key="board.id" class="board-card">
      <router-link :to="`/board/${board.id}`">
        <h3>{{ board.board_name }}</h3>
        <p>{{ board.description }}</p>
      </router-link>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { getBoardList } from '@/api/board'
import Board from "@/views/Board.vue";

interface Board {
  id: number
  board_name: string
  description: string
  created_at: string
}

const boards = ref<Board[]>([])
onMounted(async () => {
  const res = await getBoardList()
  boards.value = res.data
})
</script>

<style scoped>
.board-card { border: 1px solid #ddd; padding: 16px; margin: 12px 0; border-radius: 8px; }
</style>