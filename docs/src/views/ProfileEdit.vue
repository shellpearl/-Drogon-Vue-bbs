<template>
  <div class="profile-edit-page">
    <el-card class="edit-card" shadow="hover">
      <template #header>
        <div class="card-header">
          <span class="card-title">
            {{ userStore.role === 'admin' ? '🔐 修改密码' : '✏️ 编辑个人信息' }}
          </span>
          <span class="card-subtitle">
            {{ userStore.role === 'admin' ? '管理员仅可修改密码' : '修改你的个人资料' }}
          </span>
        </div>
      </template>

      <template v-if="userStore.role !== 'admin'">
        <el-form :model="form" label-width="100px" class="edit-form">
          <el-form-item label="头像" class="avatar-item">
            <div class="avatar-wrapper">
              <div class="avatar-display">
                <img v-if="avatarPreview" :src="avatarPreview" class="avatar-preview" />
                <el-avatar v-else :size="80" :src="currentAvatar" class="avatar-image" />
                <div class="avatar-overlay" @click="triggerFileInput">
                  <el-icon><Camera /></el-icon>
                  <span>更换头像</span>
                </div>
              </div>
              <input
                  type="file"
                  ref="fileInput"
                  accept="image/*"
                  @change="handleFileChange"
                  style="display: none"
              />
              <div class="upload-info">
                <el-button type="primary" size="small" @click="triggerFileInput">
                  <el-icon><Upload /></el-icon> 选择图片
                </el-button>
                <span v-if="uploading" class="uploading-text">上传中...</span>
                <div class="upload-tip">支持 jpg / png / gif，大小不超过 2MB</div>
              </div>
            </div>
          </el-form-item>

          <el-divider />

          <el-row :gutter="24">
            <el-col :span="12">
              <el-form-item label="昵称">
                <el-input v-model="form.username" placeholder="请输入昵称" />
              </el-form-item>
            </el-col>
            <el-col :span="12">
              <el-form-item label="真实姓名">
                <el-input v-model="form.name" placeholder="请输入真实姓名" />
              </el-form-item>
            </el-col>
          </el-row>

          <el-row :gutter="24">
            <el-col :span="12">
              <el-form-item label="性别">
                <el-radio-group v-model="form.gender">
                  <el-radio label="男">👨 男</el-radio>
                  <el-radio label="女">👩 女</el-radio>
                </el-radio-group>
              </el-form-item>
            </el-col>
            <el-col :span="12">
              <el-form-item label="生日">
                <el-date-picker
                    v-model="form.birthday"
                    type="date"
                    placeholder="选择生日"
                    format="YYYY-MM-DD"
                    value-format="YYYY-MM-DD"
                    style="width:100%;"
                />
              </el-form-item>
            </el-col>
          </el-row>

          <el-form-item label="专业">
            <el-input v-model="form.major" placeholder="请输入所学专业" />
          </el-form-item>

          <el-divider />
        </el-form>
      </template>

      <div v-else style="padding: 20px 0; text-align: center; color: #999;">
        <el-icon style="font-size: 48px; color: #409EFF;"><Lock /></el-icon>
        <p style="margin-top: 12px; font-size: 16px;">管理员请使用下方按钮修改密码</p>
      </div>
      <div class="action-buttons">
        <template v-if="userStore.role !== 'admin'">
          <el-button type="primary" size="large" @click="saveInfo" :loading="saving">
            <el-icon><Check /></el-icon> 保存修改
          </el-button>
          <el-button size="large" @click="router.back()">
            <el-icon><Back /></el-icon> 取消
          </el-button>
        </template>
        <template v-else>
          <el-button size="large" @click="router.back()">
            <el-icon><Back /></el-icon> 返回
          </el-button>
        </template>
        <el-button type="warning" size="large" @click="passwordDialogVisible = true">
          <el-icon><Lock /></el-icon> 更改密码
        </el-button>
      </div>
    </el-card>

    <el-dialog
        v-model="passwordDialogVisible"
        title="🔐 更改密码"
        width="440px"
        :close-on-click-modal="false"
        class="password-dialog"
    >
      <div class="dialog-tip">为了账户安全，请先验证旧密码</div>
      <el-form
          ref="passwordFormRef"
          :model="passwordForm"
          :rules="passwordRules"
          label-width="100px"
      >
        <el-form-item label="旧密码" prop="oldPassword">
          <el-input
              v-model="passwordForm.oldPassword"
              type="password"
              placeholder="请输入旧密码"
              show-password
          />
        </el-form-item>
        <el-form-item label="新密码" prop="newPassword">
          <el-input
              v-model="passwordForm.newPassword"
              type="password"
              placeholder="请输入新密码（至少6位）"
              show-password
          />
        </el-form-item>
        <el-form-item label="确认新密码" prop="confirmPassword">
          <el-input
              v-model="passwordForm.confirmPassword"
              type="password"
              placeholder="请再次输入新密码"
              show-password
          />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="passwordDialogVisible = false">取消</el-button>
        <el-button type="primary" @click="handleChangePassword" :loading="passwordLoading">
          确认修改
        </el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup lang="ts">
import { ref, reactive, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import { getUserInfo, updateUserInfo, uploadAvatar, changePassword } from '@/api/user'
import { ElMessage } from 'element-plus'
import { useUserStore } from '@/stores/user'
import type { FormInstance, FormRules } from 'element-plus'
import { Camera, Upload, Check, Back, Lock } from '@element-plus/icons-vue'

const router = useRouter()
const userStore = useUserStore()

const form = reactive({
  username: '',
  name: '',
  gender: '',
  birthday: '',
  major: ''
})

const currentAvatar = ref('')
const avatarPreview = ref('')
const uploading = ref(false)
const saving = ref(false)
const fileInput = ref<HTMLInputElement | null>(null)

const passwordDialogVisible = ref(false)
const passwordLoading = ref(false)
const passwordFormRef = ref<FormInstance>()
const passwordForm = reactive({
  oldPassword: '',
  newPassword: '',
  confirmPassword: ''
})

const passwordRules: FormRules = {
  oldPassword: [
    { required: true, message: '请输入旧密码', trigger: 'blur' }
  ],
  newPassword: [
    { required: true, message: '请输入新密码', trigger: 'blur' },
    { min: 6, message: '密码长度至少6位', trigger: 'blur' }
  ],
  confirmPassword: [
    { required: true, message: '请确认新密码', trigger: 'blur' },
    {
      validator: (_rule, value, callback) => {
        if (value !== passwordForm.newPassword) {
          callback(new Error('两次输入密码不一致'))
        } else {
          callback()
        }
      },
      trigger: 'blur'
    }
  ]
}

const fetchInfo = async () => {
  try {
    const res = await getUserInfo()
    const data = res.data
    form.username = data.username || ''
    form.name = data.name || ''
    form.gender = data.gender || ''
    form.birthday = data.birthday || ''
    form.major = data.major || ''
    currentAvatar.value = data.avatar || ''
    if (data.avatar) {
      userStore.avatar = data.avatar
    }
  } catch (error) {
    ElMessage.error('加载个人信息失败')
  }
}

const triggerFileInput = () => {
  fileInput.value?.click()
}

const handleFileChange = async (event: Event) => {
  const target = event.target as HTMLInputElement
  const file = target.files?.[0]
  if (!file) return

  const validTypes = ['image/jpeg', 'image/png', 'image/gif']
  if (!validTypes.includes(file.type)) {
    ElMessage.error('只支持 jpg / png / gif 格式')
    target.value = ''
    return
  }
  if (file.size > 2 * 1024 * 1024) {
    ElMessage.error('图片大小不能超过 2MB')
    target.value = ''
    return
  }

  const reader = new FileReader()
  reader.onload = (e) => {
    avatarPreview.value = e.target?.result as string
  }
  reader.readAsDataURL(file)

  uploading.value = true
  try {
    const res = await uploadAvatar(file)
    const newAvatarUrl = res.data.avatar_url
    if (newAvatarUrl) {
      currentAvatar.value = newAvatarUrl
      userStore.avatar = newAvatarUrl
      avatarPreview.value = ''
      ElMessage.success('头像上传成功')
    } else {
      ElMessage.error('上传失败，未返回图片地址')
    }
  } catch (error) {
    ElMessage.error('头像上传失败')
  } finally {
    uploading.value = false
    target.value = ''
  }
}

const saveInfo = async () => {
  saving.value = true
  try {
    await updateUserInfo(form)
    ElMessage.success('更新成功')
    router.push('/profile')
  } catch (error) {
    ElMessage.error('更新失败')
  } finally {
    saving.value = false
  }
}

const handleChangePassword = async () => {
  if (!passwordFormRef.value) return
  try {
    await passwordFormRef.value.validate()
    passwordLoading.value = true
    await changePassword({
      old_password: passwordForm.oldPassword,
      new_password: passwordForm.newPassword
    })
    ElMessage.success('密码修改成功，请重新登录')
    passwordDialogVisible.value = false
    passwordForm.oldPassword = ''
    passwordForm.newPassword = ''
    passwordForm.confirmPassword = ''
  } catch (error) {
    if (error !== 'cancel') {
      ElMessage.error('密码修改失败')
    }
  } finally {
    passwordLoading.value = false
  }
}

onMounted(fetchInfo)
</script>

<style scoped>
.profile-edit-page {
  max-width: 750px;
  margin: 0 auto;
  padding: 20px;
}

.edit-card {
  border-radius: 16px;
  box-shadow: 0 4px 24px rgba(0, 0, 0, 0.06);
  border: 1px solid #f0f0f0;
}

.edit-card :deep(.el-card__header) {
  border-bottom: 1px solid #f0f0f0;
  padding: 20px 28px;
}

.card-header {
  display: flex;
  flex-direction: column;
  gap: 2px;
}

.card-title {
  font-size: 20px;
  font-weight: 600;
  color: #1e1e1e;
}

.card-subtitle {
  font-size: 13px;
  color: #999;
}

.edit-form {
  padding: 8px 8px 0 8px;
}

.avatar-item :deep(.el-form-item__label) {
  padding-top: 0;
}

.avatar-wrapper {
  display: flex;
  align-items: center;
  gap: 28px;
  flex-wrap: wrap;
}

.avatar-display {
  position: relative;
  width: 80px;
  height: 80px;
  flex-shrink: 0;
}

.avatar-image {
  width: 80px !important;
  height: 80px !important;
}

.avatar-preview {
  width: 80px;
  height: 80px;
  border-radius: 50%;
  object-fit: cover;
  border: 2px solid #e8e8e8;
}

.avatar-overlay {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  border-radius: 50%;
  background: rgba(0, 0, 0, 0.4);
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  color: #fff;
  cursor: pointer;
  opacity: 0;
  transition: opacity 0.3s;
  font-size: 12px;
}

.avatar-display:hover .avatar-overlay {
  opacity: 1;
}

.avatar-overlay .el-icon {
  font-size: 20px;
  margin-bottom: 2px;
}

.upload-info {
  display: flex;
  flex-direction: column;
  gap: 6px;
}

.uploading-text {
  font-size: 13px;
  color: #409EFF;
}

.upload-tip {
  font-size: 12px;
  color: #bbb;
}

.el-divider {
  margin: 20px 0;
}

.action-buttons {
  display: flex;
  gap: 12px;
  flex-wrap: wrap;
  margin-top: 4px;
}

.action-buttons .el-button {
  min-width: 120px;
  display: inline-flex;
  align-items: center;
  gap: 6px;
}

.password-dialog :deep(.el-dialog__header) {
  border-bottom: 1px solid #f0f0f0;
  padding: 18px 24px;
}

.password-dialog :deep(.el-dialog__body) {
  padding: 24px;
}

.dialog-tip {
  font-size: 13px;
  color: #999;
  margin-bottom: 18px;
  padding: 10px 14px;
  background: #f8f9fa;
  border-radius: 8px;
  border-left: 3px solid #409EFF;
}

.password-dialog :deep(.el-dialog__footer) {
  border-top: 1px solid #f0f0f0;
  padding: 16px 24px;
}

@media (max-width: 600px) {
  .avatar-wrapper {
    flex-direction: column;
    align-items: center;
    text-align: center;
  }
  .action-buttons {
    flex-direction: column;
  }
  .action-buttons .el-button {
    width: 100%;
    justify-content: center;
  }
}
</style>