import { defineStore } from 'pinia'
import { ref } from 'vue'

export const useUserStore = defineStore('user', () => {
    const token = ref(localStorage.getItem('token') || '')
    const userId = ref(Number(localStorage.getItem('userId')) || 0)
    const role = ref(localStorage.getItem('role') || '')
    const username = ref(localStorage.getItem('username') || '')
    const avatar = ref(localStorage.getItem('avatar') || '')

    const setUser = (data: {
        token: string
        userId: number
        role: string
        username?: string
        avatar?: string
    }) => {
        token.value = data.token
        userId.value = data.userId
        role.value = data.role
        if (data.username) username.value = data.username
        if (data.avatar) avatar.value = data.avatar
        localStorage.setItem('token', data.token)
        localStorage.setItem('userId', String(data.userId))
        localStorage.setItem('role', data.role)
        if (data.username) localStorage.setItem('username', data.username)
        if (data.avatar) localStorage.setItem('avatar', data.avatar)
    }

    const logout = () => {
        token.value = ''
        userId.value = 0
        role.value = ''
        username.value = ''
        avatar.value = ''
        localStorage.clear()
    }

    return { token, userId, role, username, avatar, setUser, logout }
})