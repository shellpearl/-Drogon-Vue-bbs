import request from './request'

export const getUserInfo = () => request.get('/user/info')
export const updateUserInfo = (data: { name: string; gender: string; birthday: string; major: string }) =>
    request.post('/user/update', data)
export const changePassword = (data: { old_password: string; new_password: string }) =>
    request.post('/user/password', data)
export const uploadAvatar = (file: File) => {
    const formData = new FormData()
    formData.append('file', file)
    return request.post('/user/avatar', formData, {
        headers: { 'Content-Type': 'multipart/form-data' },
    })
}

export const getMyPosts = () => request.get('/user/posts')
export const getPublicUser = (userId: number) => request.get(`/user/profile/${userId}`)