import request from './request'

export const toggleFollow = (data: { followee_id: number; followee_type: string }) =>
    request.post('/follow/toggle', data)

export const getFollowCounts = (userId: number) =>
    request.get(`/follow/count/${userId}`)

export const getFollowingList = (userId?: number) => {
    const params = userId ? { userId } : {}
    return request.get('/follow/following', { params })
}
export const getFollowersList = (userId?: number) => {
    const params = userId ? { userId } : {}
    return request.get('/follow/followers', { params })
}
export const checkFollow = (userId: number, followeeType: string) =>
    request.get(`/follow/check/${userId}`, { params: { followee_type: followeeType } })