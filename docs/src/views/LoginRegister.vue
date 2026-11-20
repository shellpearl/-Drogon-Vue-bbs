<template>
  <div class="login-container">
    <div class="login-card">
      <div class="login-header">
        <h1>📚 校园BBS</h1>
        <p>{{ activeTab === 'login' ? '请登录' : '注册新账号' }}</p>
      </div>

      <el-tabs v-model="activeTab" class="login-tabs">
        <el-tab-pane label="登录" name="login">
          <el-form
            ref="loginFormRef"
            :model="loginForm"
            :rules="loginRules"
            label-width="0"
            @submit.prevent="handleLogin"
          >
            <el-form-item prop="username">
              <el-input v-model="loginForm.username" placeholder="账号" prefix-icon="User" />
            </el-form-item>

            <el-form-item prop="password">
              <el-input
                  v-model="loginForm.password"
                  :type="pwdVisible ? 'text' : 'password'"
                  placeholder="密码"
                  prefix-icon="Lock"
              >
                <template #suffix>
                <span style="display:flex;align-items:center;gap:8px;">
                  <el-icon @click="pwdVisible = !pwdVisible" style="cursor:pointer;font-size:18px;">
                    <component :is="pwdVisible ? View : Hide" />
                  </el-icon>
                  <a
                    href="#"
                    style="color:#409EFF;font-size:14px;text-decoration:none;"
                    @click.prevent="forgotPassword"
                  >忘记密码？</a>
                </span>
              </template>
            </el-input>
          </el-form-item>

            <el-form-item>
              <el-radio-group v-model="loginForm.role" size="small">
                <el-radio-button value="student">学生</el-radio-button>
                <el-radio-button value="admin">管理员</el-radio-button>
              </el-radio-group>
            </el-form-item>
            <el-form-item>
              <el-button type="primary" native-type="submit" style="width:100%;">登录</el-button>
            </el-form-item>
          </el-form>
        </el-tab-pane>

        <el-tab-pane label="注册" name="register">
          <el-form
              ref="registerFormRef"
              :model="registerForm"
              :rules="registerRules"
              label-width="0"
              @submit.prevent="handleRegister"
          >
            <el-form-item prop="username">
              <el-input v-model="registerForm.username" placeholder="学号/账号" prefix-icon="User" />
            </el-form-item>
            <el-form-item prop="name">
              <el-input v-model="registerForm.name" placeholder="姓名" prefix-icon="Edit" />
            </el-form-item>
            <el-form-item prop="password">
              <el-input v-model="registerForm.password" type="password" placeholder="密码" prefix-icon="Lock" show-password />
            </el-form-item>
            <el-form-item prop="gender">
              <el-radio-group v-model="registerForm.gender">
                <el-radio label="男">男</el-radio>
                <el-radio label="女">女</el-radio>
              </el-radio-group>
            </el-form-item>
            <el-form-item prop="birthday">
              <el-date-picker
                  v-model="registerForm.birthday"
                  type="date"
                  placeholder="选择生日"
                  format="YYYY-MM-DD"
                  value-format="YYYY-MM-DD"
                  style="width:100%;"
              />
            </el-form-item>
            <el-form-item prop="major">
              <el-input v-model="registerForm.major" placeholder="专业" />
            </el-form-item>
            <el-form-item>
              <el-button type="primary" native-type="submit" style="width:100%;">注册</el-button>
            </el-form-item>
          </el-form>
        </el-tab-pane>
      </el-tabs>
    </div>
  </div>

  <el-dialog
      v-model="showResetDialog"
      title="忘记密码"
      width="400px"
      :close-on-click-modal="true"
  >
    <p style="text-align:center;font-size:16px;margin-bottom:20px;">
      请联系管理员重置密码
    </p>
    <div style="text-align:center;color:#666;">
      <p><strong>管理员姓名：</strong>{{ adminInfo.name }}</p>
      <p><strong>QQ邮箱：</strong>{{ adminInfo.email }}</p>
    </div>
    <template #footer>
      <el-button @click="showResetDialog = false">确定</el-button>
    </template>
  </el-dialog>
</template>

<script setup lang="ts">
import { ref, reactive } from 'vue'
import { useRouter, useRoute } from 'vue-router'
import { login, register } from '@/api/auth'
import { useUserStore } from '@/stores/user'
import { ElMessage, type FormInstance, type FormRules } from 'element-plus'
import { View, Hide } from '@element-plus/icons-vue'
import { getAdminContact } from '@/api/admin'

const router = useRouter()
const route = useRoute()
const userStore = useUserStore()

const activeTab = ref(route.path === '/register' ? 'register' : 'login')
const loginFormRef = ref<FormInstance>()
const registerFormRef = ref<FormInstance>()

const pwdVisible = ref(false)

const showResetDialog = ref(false)

const adminInfo = ref({
  name: '',
  email: ''
})

const loginForm = reactive({
  username: '',
  password: '',
  role: 'student' as 'student' | 'admin',
})

const loginRules: FormRules = {
  username: [{ required: true, message: '请输入账号', trigger: 'blur' }],
  password: [{ required: true, message: '请输入密码', trigger: 'blur' }],
}

const registerForm = reactive({
  username: '',
  password: '',
  name: '',
  gender: '男',
  birthday: '',
  major: '',
})

const registerRules: FormRules = {
  username: [{ required: true, message: '请输入学号/账号', trigger: 'blur' }],
  password: [{ required: true, message: '请输入密码', trigger: 'blur' }],
  name: [{ required: true, message: '请输入姓名', trigger: 'blur' }],
}

const handleLogin = async () => {
  if (!loginFormRef.value) return
  try {
    await loginFormRef.value.validate()
    const res = await login(loginForm)
    userStore.setUser({
      token: res.data.token,
      userId: res.data.userId,
      role: res.data.role,
    })
    ElMessage.success('登录成功')
    router.push((route.query.redirect as string) || '/')
  } catch (error) {
  }
}

const handleRegister = async () => {
  if (!registerFormRef.value) return
  try {
    await registerFormRef.value.validate()
    await register(registerForm)
    ElMessage.success('注册成功，请登录')
    activeTab.value = 'login'
    loginForm.username = registerForm.username
    loginForm.password = ''
  } catch (error) {
  }
}

const forgotPassword = async () => {
  try {
    const res = await getAdminContact()
    adminInfo.value = res.data
  } catch (error) {
    adminInfo.value = {
      name: '马晓俊毅',
      email: 'ma11111156@qq.com'
    }
  }
  showResetDialog.value = true
}
</script>

<style scoped>
.login-container {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100%;
  background: #f0f2f5;
}
.login-card {
  width: 420px;
  background: #fff;
  border-radius: 12px;
  padding: 40px 30px;
  box-shadow: 0 8px 32px rgba(0,0,0,0.08);
}
.login-header {
  text-align: center;
  margin-bottom: 30px;
}
.login-header h1 {
  font-size: 28px;
  font-weight: 600;
  color: #1e1e1e;
}
.login-header p {
  color: #888;
  margin-top: 8px;
}
.login-tabs :deep(.el-tabs__item) {
  font-size: 16px;
  font-weight: 500;
}
.login-tabs :deep(.el-tabs__active-bar) {
  background-color: #409EFF;
}
.login-tabs :deep(.el-tabs__item.is-active) {
  color: #409EFF;
}
</style>