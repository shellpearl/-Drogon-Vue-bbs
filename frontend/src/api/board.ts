import request from './request'

export const getBoardList = () => request.get('/board/list')
export const createBoard = (data: { board_name: string; description: string }) =>
    request.post('/admin/board/create', data)
export const deleteBoard = (boardId: number) => request.delete(`/admin/board/${boardId}`)