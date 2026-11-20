<template>
  <div>
    <h2>{{ boardName }}</h2>
    <div v-if="userStore.token">
      <el-input v-model="newPost.title" placeholder="标题" />
      <el-input type="textarea" v-model="newPost.content" placeholder="内容" />
      <el-button type="primary" @click="submitPost">发帖</el-button>
    </div>
    <div v-else>请登录后发帖</div>

    <div v-for="post in posts" :key="post.id" class="post-item">
      <div class="post-author">
        <el-avatar
            :size="40"
            :src="post.author_avatar || ''"
            @click="goToUser(post.author_id)"
            style="cursor:pointer;"
        />
        <span class="author-name" @click="goToUser(post.author_id)" style="cursor:pointer;font-weight:500;">
          {{ post.author_name }}
        </span>
        <el-button
            v-if="userStore.token && post.author_id !== userStore.userId"
            size="small"
            :type="post.is_following ? 'default' : 'primary'"
            @click="handleToggleFollow(post)"
            style="margin-left:auto;"
        >
          {{ post.is_following ? '取消关注' : '关注' }}
        </el-button>
      </div>
      <router-link :to="`/post/${post.id}`" class="post-link">
        <h4>{{ post.title }}</h4>
        <span class="post-time">{{ post.post_time }}</span>
      </router-link>
      <div class="post-footer">
        <el-button
            type="text"
            :icon="post.is_liked ? 'star' : 'star-outline'"
            @click="handleLike(post)"
            size="small"
            :disabled="!userStore.token"
        >
          {{ post.like_count }} 赞
        </el-button>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { getPostsByBoard, createPost } from '@/api/post'
import { toggleFollow as toggleFollowApi } from '@/api/follow'
import { toggleLike } from '@/api/like'
import { useUserStore } from '@/stores/user'
import { ElMessage } from 'element-plus'

interface Post {
  id: number
  title: string
  post_time: string
  author_id: number
  author_type: string
  author_name: string
  author_avatar: string
  is_following: boolean
  like_count: number
  is_liked: boolean
}

const route = useRoute()
const router = useRouter()
const userStore = useUserStore()
const boardId = Number(route.params.boardId)
const boardName = ref('')
const posts = ref<Post[]>([])
const newPost = ref({ title: '', content: '' })

const fetchPosts = async () => {
  try {
    const res = await getPostsByBoard(boardId)
    posts.value = res.data
  } catch (error) {
    ElMessage.error('获取帖子失败')
  }
}

const submitPost = async () => {
  try {
    await createPost({ boardId, ...newPost.value })
    newPost.value = { title: '', content: '' }
    await fetchPosts()
    ElMessage.success('发帖成功')
  } catch (error) {
    ElMessage.error('发帖失败')
  }
}

const handleToggleFollow = async (post: Post) => {
  try {
    await toggleFollowApi({
      followee_id: post.author_id,
      followee_type: post.author_type
    })
    post.is_following = !post.is_following
    ElMessage.success(post.is_following ? '关注成功' : '取消关注成功')
  } catch (error) {
    ElMessage.error('操作失败')
  }
}

const handleLike = async (post: Post) => {
  if (!userStore.token) {
    ElMessage.warning('请先登录')
    return
  }
  try {
    await toggleLike(post.id)
    post.is_liked = !post.is_liked
    post.like_count += post.is_liked ? 1 : -1
    ElMessage.success(post.is_liked ? '点赞成功' : '取消点赞成功')
  } catch (error) {
    ElMessage.error('操作失败')
  }
}

const goToUser = (userId: number) => {
  router.push(`/user/${userId}`)
}

onMounted(fetchPosts)
</script>

<style scoped>
.post-item {
  border-bottom: 1px solid #eee;
  padding: 16px 0;
}
.post-author {
  display: flex;
  align-items: center;
  gap: 10px;
  margin-bottom: 8px;
}
.author-name {
  font-size: 14px;
  color: #333;
}
.author-name:hover {
  color: #409EFF;
}
.post-link {
  display: block;
  text-decoration: none;
  color: inherit;
}
.post-link h4 {
  margin: 4px 0 6px;
  font-size: 16px;
}
.post-time {
  font-size: 12px;
  color: #999;
}
</style>