import { defineStore } from 'pinia'
import { ref } from 'vue'
import { getUnreadCount } from '@/api/notification'

export interface Notification {
    id: number
    content: string
    created_at: string
    is_read: boolean
    post_id: number
}

export const useNotificationStore = defineStore('notification', () => {
    const unreadCount = ref(0)
    const notifications = ref<Notification[]>([])
    const connected = ref(false)

    const fetchUnread = async () => {
        try {
            const res = await getUnreadCount()
            unreadCount.value = res.data.count
        } catch (error) {
            console.error('获取未读数量失败', error)
        }
    }

    const setNotifications = (list: Notification[]) => {
        notifications.value = list
    }

    const addNotification = (notif: Notification) => {
        notifications.value.unshift(notif)
        unreadCount.value += 1
    }

    const markAsRead = (id: number) => {
        const item = notifications.value.find(n => n.id === id)
        if (item && !item.is_read) {
            item.is_read = true
            unreadCount.value = Math.max(0, unreadCount.value - 1)
        }
    }

    const markAllRead = () => {
        notifications.value.forEach(n => n.is_read = true)
        unreadCount.value = 0
    }

    const setConnected = (status: boolean) => {
        connected.value = status
    }

    const increment = () => { unreadCount.value += 1 }
    const decrement = (num = 1) => { unreadCount.value = Math.max(0, unreadCount.value - num) }
    const reset = () => { unreadCount.value = 0 }

    return {
        unreadCount,
        notifications,
        connected,
        fetchUnread,
        setNotifications,
        addNotification,
        markAsRead,
        markAllRead,
        setConnected,
        increment,
        decrement,
        reset
    }
})