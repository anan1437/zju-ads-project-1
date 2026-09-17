# Git 使用提示（重点：远端已有新提交时的处理）

> 本项目三人分工独立（BST / AVL / Splay 各改各的文件），原则上**永远不会产生代码冲突**。
> 因此本组统一采用 **fast-forward（快进）** 方式同步代码，保持提交历史是一条直线，方便在仓库历史中查证每个人的工作（见分工文档的免责声明）。
> 请先完整读一遍本文再动手；遇到报错时回到第 3 节照做即可。

## 0. 开始前的一次性配置（每人只需执行一次）

```bash
git config pull.rebase true
```

作用：以后执行 `git pull` 时自动按「变基（rebase）」方式合并，而不是默认的「合并（merge）」方式。
这样 `git pull` 拉取远端更新后，你本地未推送的提交会被放到远端新提交的**后面**，保证之后 `git push` 永远是 fast-forward，不会产生 `Merge branch ...` 这种多余的合并提交。

## 1. 日常标准流程（90% 的时间只用这几条）

### 1.1 第一次把仓库下载到本地

```bash
git clone https://github.com/anan1437/zju-ads-project-1.git
```

### 1.2 每天的开发流程

```bash
git pull              # ① 动手写代码前：先同步远端最新内容
                      # ……编写/修改自己负责的文件……
git add 文件名         # ② 把改动放入暂存区（改了多个文件可用 git add -A）
git commit -m "写清楚这次改了什么"   # ③ 提交到本地
git pull              # ④ 推送前再同步一次（防止这几分钟里别人推了新提交）
git push              # ⑤ 推送到远端
```

口诀：**动手前先 pull，推送前再 pull**。
第 ④ 步的第二次 pull 是关键：如果远端这期间有了新提交，不先 pull 就 push 一定会被拒绝（见第 3 节）。

## 2. 核心概念：什么是 fast-forward

Git 的历史就是一串首尾相接的提交。**fast-forward（快进）** 是指：远端历史只是"往前挪"到包含你的提交为止，中间不产生任何多余的"合并提交"。

理想情况（你 push 时远端没动过，push 本身就是 fast-forward）：

```
远端: A---B                push 后: A---B---C（你的提交）
```

麻烦的情况：你在本地提交了 D，但推送前别人已经把提交 C 推上去了。
这时两人的历史"分叉"了：

```
公共起点 A---B
             \
              C   ← 远端在这里（别人的提交）
              \
               D  ← 你在这里（你的提交）
```

Git 会**直接拒绝你的 push**（报错见第 3 节），因为直接推会覆盖掉别人的提交 C。
正确做法是先"追平"远端：把你的 D 搬到 C 后面再推，追平后历史变成：

```
A---B---C---D    ← 依然是一条直线，push 成功（这就是 fast-forward）
```

因为我们三人改的是完全不同的文件，把 D 挪到 C 后面**不可能产生冲突**，所以可以放心地让 Git 自动完成。

## 3. 重点：push 被拒绝时的标准处理流程

### 3.1 你会看到的报错

```
To https://github.com/anan1437/zju-ads-project-1.git
 ! [rejected]        main -> main (fetch first)
error: failed to push some refs to ...
hint: Updates were rejected because the remote contains work that you do not have
```

含义：远端已经有了新提交（别人的工作），而你本地没有这些提交，Git 不允许你直接覆盖。**这是正常的保护机制，不是出了故障。**

### 3.2 标准处理（照抄两条命令即可）

```bash
git pull --rebase    # 把你本地未推送的提交“搬到”远端新提交的后面
git push             # 此时一定能成功
```

`git pull --rebase` 做了什么（理解即可，不用记）：

1. 先把远端的新提交（C）取下来；
2. 把你本地还没推送的提交（D）暂时"摘下来"；
3. 把 D 重新接在 C 的后面。

完成后回到第 1 节的正常流程即可。

> 如果你已经做了第 0 节的配置，这里直接 `git pull` 效果相同；写 `--rebase` 是为了在没配置的情况下也保险。

### 3.3 万一 pull 时提示冲突（正常分工下不会发生）

只有两个人改了**同一个文件的同一处**才会冲突。处理原则：

```bash
git status           # 查看哪个文件冲突了
```

- **不确定怎么改**（推荐）：完全取消本次操作，回到 pull 之前的状态，然后到群里问：

  ```bash
  git rebase --abort
  ```

- **确定怎么改**：打开冲突文件，把 `<<<<<<<`、`=======`、`>>>>>>>` 三行标记之间的内容整理成想要的样子，然后：

  ```bash
  git add 冲突文件
  git rebase --continue
  ```

> 本项目各模块文件相互独立，正常情况下走不到这一步。真遇到了优先 `--abort` 再沟通。

### 3.4 补充说明：为什么不直接用默认的 `git pull`

默认的 `git pull` 在历史分叉时会"合并"出一个额外的 `Merge branch 'main' ...` 提交，历史不再是一条直线，查证每个人的工作时会比较乱。
用 rebase 方式（第 0 节配置 / 显式加 `--rebase`）之后，push 永远是 fast-forward，历史永远是一条直线——这就是本组的约定。

## 4. 绝对禁止的操作

| 操作 | 为什么禁止 |
| ---- | ---------- |
| `git push --force`（`-f`） | 会**直接覆盖远端历史、删掉别人的提交**，且几乎无法恢复 |
| 不 pull 就连续 commit 好几天 | 本地与远端分叉越来越大，越难处理；勤 pull 勤 push |
| 用"删掉文件夹重新 clone"来解决问题 | 会丢掉本地未推送的提交；按第 3 节流程即可解决 |
| 修改/删除别人负责模块的文件 | 制造冲突源头，违反分工约定 |

## 5. 常用自查命令

```bash
git status                        # 我改了什么？本地领先/落后远端多少？
git log --oneline -10             # 最近 10 条提交
git log --oneline --graph --all   # 画图看整条历史（确认没有分叉）
git diff                          # 查看尚未 add 的改动内容
```

`git status` 中两句话的含义：
- `Your branch is ahead of 'origin/main' by 1 commit.` → 你有 1 个提交还没 push；
- `Your branch is behind 'origin/main' by 2 commits.` → 远端有 2 个新提交，先 pull。

## 6. FAQ

**Q1：pull 时提示本地修改会被覆盖（`Your local changes ... would be overwritten` 或 `cannot pull with rebase: You have unstaged changes`）？**
说明你手头有改了一半、还没 commit 的文件。先提交再 pull：

```bash
git add -A
git commit -m "进行中的修改"
git pull --rebase
```

**Q2：rebase 之后我的提交丢了吗？**
没有。你的提交只是被搬到了远端提交的后面，`git log --oneline -5` 能看到。真出意外时 `git reflog` 可以找回任何历史状态（把这句转告组里熟悉 Git 的同学即可）。

**Q3：不小心产生了一个 `Merge branch ...` 的合并提交怎么办？**
无伤大雅，直接 `git push` 推上去即可，不要试图"消灭"它。做好第 0 节配置后以后不会再出现。

**Q4：`git pull` 时突然弹出一个编辑器让我写合并信息？**
说明这次 pull 走了 merge 流程（多半没做第 0 节配置）。在 vim 里输入 `:wq` 回车保存退出即可，然后补做第 0 节的配置。

**Q5：我不确定现在能不能 push？**
只要同时满足：① 已 commit；② 执行 pull（或 `git pull --rebase`）没有报任何错——就可以放心 push。git 本身在 push 时也会拒绝非 fast-forward 的推送，不会误伤别人的工作。
