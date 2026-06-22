import request from './request'

export const getPostsByBoard = (boardId: number) => request.get(`/post/board/${boardId}`)
export const getPostDetail = (postId: number) => request.get(`/post/${postId}`)
export const createPost = (data: { boardId: number; title: string; content: string }) =>
    request.post('/post/create', data)
export const deletePost = (postId: number) => request.delete(`/post/${postId}`)
export const getPostsByAuthor = (authorId: number) => request.get(`/post/author/${authorId}`)