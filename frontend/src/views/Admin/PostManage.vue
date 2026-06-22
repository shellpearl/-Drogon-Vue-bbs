<template>
  <div>
    <h2>帖子管理</h2>
    <el-table :data="posts" style="width:100%">
      <el-table-column prop="title" label="标题" />
      <el-table-column prop="author_name" label="作者" />
      <el-table-column prop="board_name" label="板块" />
      <el-table-column prop="post_time" label="时间" />
      <el-table-column label="操作">
        <template #default="{ row }">
          <el-button @click="deletePost(row.id)" size="small">删除</el-button>
        </template>
      </el-table-column>
    </el-table>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import request from '@/api/request'

const posts = ref([])

const fetchPosts = async () => {
  const res = await request.get('/admin/posts')
  posts.value = res.data
}

const deletePost = async (id: number) => {
  await request.delete(`/admin/post/${id}`)
  await fetchPosts()
}

onMounted(fetchPosts)
</script>