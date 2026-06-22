import request from './request'

export const getBrowseHistory = () => request.get('/browse/history')