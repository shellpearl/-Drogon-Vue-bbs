import { createRouter, createWebHistory } from 'vue-router'
import Home from '@/views/Home.vue'
import LoginRegister from "@/views/LoginRegister.vue";
import Board from '@/views/Board.vue'
import PostDetail from '@/views/PostDetail.vue'
import Profile from '@/views/Profile.vue'
import ProfileEdit from '@/views/ProfileEdit.vue'
import BoardManage from '@/views/Admin/BoardManage.vue'
import PostManage from '@/views/Admin/PostManage.vue'
import NotificationList from '@/views/NotificationList.vue'
import MyBrowse from "@/views/MyBrowse.vue"
import UserProfile from "@/views/UserProfile.vue"
import FollowList from "@/views/FollowList.vue"
import { useUserStore } from '@/stores/user'

const routes = [
    { path: '/', component: Home },
    { path: '/login', component: LoginRegister },
    { path: '/register', component: LoginRegister },
    { path: '/board/:boardId', component: Board, props: true },
    { path: '/post/:postId', component: PostDetail, props: true },
    { path: '/profile', component: Profile, meta: { requiresAuth: true } },
    { path: '/profile', component: Profile, meta: { requiresAuth: true } },
    { path: '/profile/edit', component: ProfileEdit, meta: { requiresAuth: true } },
    { path: '/admin/boards', component: BoardManage, meta: { requiresAdmin: true } },
    { path: '/admin/posts', component: PostManage, meta: { requiresAdmin: true } },
    { path: '/notifications', component: NotificationList, meta: { requiresAuth: true } },
    { path: '/my', component: MyBrowse, meta: { requiresAuth: true } },
    { path: '/user/:userId', component: UserProfile },
    { path: '/follow/:type', component: FollowList, meta: { requiresAuth: true } },
]

const router = createRouter({
    history: createWebHistory(),
    routes,
})

router.beforeEach((to, _from, next) => {
    const userStore = useUserStore()
    if (to.meta.requiresAuth && !userStore.token) {
        next('/login')
    } else if (to.meta.requiresAdmin && userStore.role !== 'admin') {
        next('/')
    } else {
        next()
    }
})

export default router