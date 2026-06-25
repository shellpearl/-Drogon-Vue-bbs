import request from './request'

export const getNotifications = () => request.get('/notifications')
export const markAsRead = (id: number) => request.put(`/notifications/${id}/read`)
export const markAllRead = () => request.put('/notifications/read-all')
export const getUnreadCount = () => request.get('/notifications/unread-count')
export const deleteBatch = (ids: number[]) => request.delete('/notifications/batch', { data: { ids } })