import request from './request'

export const register = (data: {
    username: string
    password: string
    name: string
    gender: string
    birthday: string
    major: string
}) => request.post('/auth/register', data)

export const login = (data: {
    username: string
    password: string
    role?: 'student' | 'admin'
}) => request.post('/auth/login', data)