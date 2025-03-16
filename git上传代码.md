你可以自己在本地创建一个 `.md` 文件，并将以下内容复制粘贴进去：  

---

**文件名：`git_upload_guide.md`**  

```md
# Git 上传代码到 GitHub 操作总结

## 🚀 第一步：创建 GitHub 仓库
1. 登录 [GitHub](https://github.com/) 并点击右上角 `+` → `New repository`。
2. 填写仓库名称（如 `my-project`），选择 **Public** 或 **Private**。
3. **不要勾选** "Initialize this repository with a README"（否则后续可能冲突）。
4. 点击 `Create repository` 创建仓库，GitHub 会提供仓库地址（HTTPS 或 SSH）。

---

## 💻 第二步：在本地初始化 Git 仓库
如果你的项目目录还没有 Git 版本控制，先进行初始化：
```sh
cd /你的项目目录   # 进入项目文件夹
git init            # 初始化 Git 仓库
```
如果你的仓库已经存在 Git 版本控制，直接跳到 **第三步**。

---

## 📌 第三步：添加文件并提交到本地仓库
```sh
git add .                           # 添加所有文件到暂存区
git commit -m "Initial commit"      # 提交代码并添加描述
```
如果你的 Git 仓库是空的，没有提交，`git push` 会失败，所以一定要先 commit。

---

## 🔗 第四步：关联远程 GitHub 仓库
```sh
git remote add origin https://github.com/你的GitHub用户名/你的仓库名.git
```
如果你想使用 SSH 方式：
```sh
git remote add origin git@github.com:你的GitHub用户名/你的仓库名.git
```
然后检查远程仓库是否正确：
```sh
git remote -v
```

---

## 📤 第五步：推送代码到 GitHub
如果你本地的分支是 `master`，但 GitHub 默认使用 `main`，可以先重命名：
```sh
git branch -M main
```
然后推送代码：
```sh
git push -u origin main
```

---

## 🛠 常见问题解决方案

### ❌ 问题 1：`SSL_ERROR_SYSCALL`
如果 `git push` 时报错：
```sh
fatal: unable to access 'https://github.com/...': OpenSSL SSL_read: SSL_ERROR_SYSCALL
```
**解决方法**：
- 确保网络正常，尝试使用 VPN 或代理。
- 切换为 SSH 方式：
  ```sh
  git remote set-url origin git@github.com:你的GitHub用户名/你的仓库名.git
  ```
- 关闭 SSL 验证（不推荐长期使用）：
  ```sh
  git config --global http.sslVerify false
  ```

---

### ❌ 问题 2：`error: failed to push some refs to ... (fetch first)`
如果 `git push` 时报错：
```sh
error: failed to push some refs to 'https://github.com/...'
hint: Updates were rejected because the remote contains work that you do not
hint: have locally.
```
**原因**：远程仓库有更新，而本地仓库没有这些更改。  
**解决方法**：
```sh
git pull --rebase origin main  # 先同步远程仓库
git push origin main           # 然后再推送
```
如果你确定要覆盖远程代码（慎用）：
```sh
git push -u origin main --force  # 强制推送，会覆盖远程修改
```

---

### ❌ 问题 3：`error: src refspec main does not match any`
如果 `git push` 时报错：
```sh
error: src refspec main does not match any
```
**原因**：你的本地仓库 **没有 `main` 分支** 或 **没有提交代码**。  
**解决方法**：
```sh
git branch -M main   # 确保分支名称正确
git add .            # 添加文件
git commit -m "Initial commit"  # 提交代码
git push -u origin main
```

---

## ✅ 后续更新代码
以后有新的代码更新时，按以下步骤同步：
```sh
git add .
git commit -m "更新描述"
git push origin main
```
如果远程仓库有更新，先拉取：
```sh
git pull origin main --rebase
```

---