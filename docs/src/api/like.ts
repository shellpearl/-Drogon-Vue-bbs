import request from './request'

export const toggleLike = (postId: number) =>
    request.post('/like/toggle', { post_id: postId })

export const getLikeCount = (postId: number) =>
    request.get(`/like/count/${postId}`)

export const checkLike = (postId: number) =>
    request.get(`/like/check/${postId}`)

export const getMyLikes = () =>
    request.get('/like/my')