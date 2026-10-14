# 图书馆检索系统（C 语言模块化版）

这是一个适合大学程序设计课程小组作业的控制台程序，支持：

- 查看全部图书
- 按书名、作者或分类检索，并可按分类标签检索
- 管理员登录后添加、修改、删除图书
- 管理员添加、修改、删除和查看借阅者
- 借阅者登录后借阅、归还图书
- 借阅日期自动识别（默认今天，也可手动输入 YYYY-MM-DD），自动计算应还日期
- 保存借阅日期、应还日期和借阅记录
- 关键写操作（借阅、归还、增删改）执行前二次确认，防止误操作

## 文件分工

- `main.c`：登录和菜单流程
- `models.h`：公共数据结构和容量定义
- `book.c` / `book.h`：图书检索、增删改和保存
- `user.c` / `user.h`：用户登录和借阅者管理
- `loan.c` / `loan.h`：借阅、归还和借阅记录
- `utils.c` / `utils.h`：输入和通用字符串工具

## 编译运行

需要安装 GCC 或 MinGW。在项目目录执行：

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 main.c book.c user.c loan.c utils.c -o library
./library
```

Windows PowerShell 下运行：

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 main.c book.c user.c loan.c utils.c -o library.exe -lwinmm
chcp 65001
.\library.exe
```

也可以使用 Makefile（需已安装 `make`）：

```bash
make
./library
```

### 一键编译并运行（Windows 推荐）

项目自带 `build.bat`，无需安装 `make`，双击或执行：

```powershell
.\build.bat
```

它会自动用 gcc 编译并启动程序。VS Code 中也可以：

- 按 `F5`，选择「编译并运行图书馆程序」（已适配 MinGW + gdb）；
- 或运行任务「一键编译并运行」（`Ctrl+Shift+B`）。

> 注意：程序需要在**项目根目录**下运行，因为它按相对路径读取 `books.txt`、`users.txt`、`loans.txt`、`ending.txt` 和三个 `mp3` 音乐文件。


程序启动时会读取当前目录的 `books.txt`、`users.txt` 和 `loans.txt`。文件使用 `|` 分隔字段，格式如下：

```text
图书编号|书名|作者|分类|出版年份|是否可借阅
用户编号|登录账号|登录密码|姓名|是否管理员|已归还数量
图书编号|用户编号|借阅者姓名|借阅日期|应还日期
```

其中 `1` 表示可借阅、管理员或真，`0` 表示已借出、普通借阅者或假。不要在文本字段中输入 `|`。借阅者和借阅者的姓名不能包含数字；借阅日期使用 `YYYY-MM-DD` 格式，留空时自动使用当天日期，应还日期由借阅日期加 30 天自动生成。

## 初始账号

- 管理员：账号 `admin`，密码 `admin123`
- 借阅者：账号 `student`，密码 `student123`

## 小组分工建议

1. 一人负责 `book.c`：图书检索和管理员图书维护。
2. 一人负责 `user.c`：登录、用户和权限。
3. 一人负责 `loan.c`：借阅、归还和记录查询。
4. 一人负责 `main.c`、测试和项目文档。
