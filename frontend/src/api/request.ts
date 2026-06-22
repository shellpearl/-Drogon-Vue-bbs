import axios from 'axios'
import { useUserStore } from '@/stores/user'
import { ElMessage } from 'element-plus'

const request = axios.create({
    baseURL: '/api',
    timeout: 10000,
})

request.interceptors.request.use((config) => {
    const userStore = useUserStore()
    if (userStore.token) {
        config.headers.Authorization = `Bearer ${userStore.token}`
    }
    return config
})

request.interceptors.response.use(
    (response) => {
        const res = response.data
        if (res.code !== 0) {
            ElMessage.error(res.msg || '请求失败')
            return Promise.reject(res)
        }
        return res
    },
    (error) => {
        ElMessage.error(error.response?.data?.msg || '网络错误')
        return Promise.reject(error)
    }
)

export default request