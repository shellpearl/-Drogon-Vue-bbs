import request from './request'

export const createReply = (data: { postId: number; content: string }) =>
    request.post('/reply/create', data)
export const deleteReply = (replyId: number) => request.delete(`/reply/${replyId}`)
export const getMyReplies = () => request.get('/reply/my')