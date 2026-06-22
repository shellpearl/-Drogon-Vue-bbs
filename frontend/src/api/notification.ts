import request from './request'

export const markAsRead = (id: number) => request.put(`/notifications/${id}/read`)
export const markAllRead = () => request.put('/notifications/read-all')
export const getUnreadCount = () => request.get('/notifications/unread-count')