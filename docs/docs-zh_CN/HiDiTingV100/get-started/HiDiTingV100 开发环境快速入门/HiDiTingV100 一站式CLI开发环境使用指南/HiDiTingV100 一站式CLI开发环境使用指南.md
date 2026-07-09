# HiDiTingV100 一站式CLI开发环境使用指南

一站式CLI开发环境通过统一的命令行工具 `fbb` 封装了工程配置、编译、烧写、监视等完整工作流。开发者无需下载 CMake/Ninja 等环境，也无需操作这些底层命令，只需使用 `fbb` 系列子命令即可完成全部操作。

主要特性：

- 支持 SDK 源码树外执行工程构建
- 支持一键快速创建空工程
- 通过图形化功能裁剪与配置
- 内置烧写与串口监视器
- 跨平台：Windows、Linux

---

## 环境准备

以下步骤将搭建一套与系统完全隔离的虚拟编译构建环境，所有工具链和 Python 依赖均安装在 `~/.hs-fbb/` 目录下，不会对系统已有的 Python 或开发工具产生任何影响。

---

**方式一（推荐）：一键安装**

一行命令完成 uv 安装、fbb-cli 安装和 `fbb setup`：

**Windows（PowerShell）：**

```powershell
irm https://dl.hispark.hisilicon.com/bootstrap.ps1 | iex
```

> **⚠️ Windows 执行策略**
>
> 若执行安装脚本时报错 *PowerShell requires an execution policy in [Unrestricted, RemoteSigned, Bypass]*，需先在 PowerShell 中运行以下命令开放脚本执行权限，选择 **[Y] 是** 确认：
>
> ```powershell
> Set-ExecutionPolicy RemoteSigned -scope CurrentUser
> ```

> **⚠️ 应用程序控制策略阻止**
>
> 安装或后续运行过程中，若出现「此应用已被你的组织的应用程序控制策略阻止」等提示，可尝试临时关闭杀毒软件实时防护后重试；若由企业安全策略统一管控，需联系 IT 管理员将 `~/.hs-fbb/` 目录加入白名单。

**Linux：**

```bash
curl -fsSL https://dl.hispark.hisilicon.com/bootstrap.sh | sh
```

完成后执行 `fbb doctor` 验证环境是否就绪，若输出正常可直接跳至[代码下载与编译](#代码下载与编译)。

---

**方式二（备选）：分步手动安装**

### 1. 安装 uv 工具

[uv](https://github.com/astral-sh/uv) 是一个极速的 Python 包管理工具，用于隔离安装 fbb-cli 及其依赖，避免污染系统 Python 环境。


**Windows（PowerShell）：**

```powershell
irm https://astral.sh/uv/install.ps1 | iex
```

**Linux：**

```bash
# 安装 uv
curl -LsSf https://astral.sh/uv/install.sh | sh
# 重新加载环境变量（或重启终端）
source $HOME/.local/bin/env
```

---

### 2. 安装 fbb-cli

> **📌 前置依赖：Git**
>
> 安装命令通过 Git 协议拉取源码，需确保系统已安装 Git。
>
> - **Windows** 推荐通过 winget 安装：
>   ```powershell
>   winget install --id Git.Git -e --source winget
>   ```
> - **Linux**：`sudo apt install git`（Debian/Ubuntu）或 `sudo dnf install git`（Fedora）
>
> 安装后重新打开终端，执行 `git --version` 确认安装成功。

使用 `uv tool install` 安装 fbb-cli 命令行工具：

```bash
uv tool install git+https://gitcode.com/HiSpark/hs-fbb-cli.git
```

> **⚠️ Python 版本过低导致安装失败**
>
> 若安装时出现如下错误：
> ```
> ImportError: cannot import name 'TypeGuard' from 'typing'
> ```
> 需先安装 Python 3.11：
> ```powershell
> uv python install 3.11
> ```
> 再重新执行 `uv tool install`。

安装完成后运行 `fbb setup` 完成依赖初始化：

```bash
fbb setup
```

`fbb setup` 会自动完成以下操作：

- 在 `~/.hs-fbb/`（Windows：`C:\Users\<用户名>\.hs-fbb\`）下创建隔离虚拟环境
- 安装构建所需的 Python 依赖包（pyserial 等）
- 下载并配置工具链（BurnToolCmd 等）

> **⚠️ fbb setup 失败：Python 版本过低**
>
> 若出现以下错误，说明系统 PATH 中存在旧版 Python（如 Python 2.7），uv 无法使用：
> ```
> error: Failed to inspect Python interpreter from first executable in the search path at `C:\Python27\python.exe`
>   Caused by: Python executable does not support `-I` flag. Please use Python 3.6 or newer.
> ```
> 可尝试以下方式修复：
> ```bash
> uv python install 3.12.10
> fbb setup --force
> ```
> 或跳过 OBS Python 镜像：
> ```bash
> UV_PYTHON_INSTALL_MIRROR="" fbb setup --force
> ```

---


## 代码下载与编译

### 1. SDK 下载

执行以下命令下载 SDK，`<sdk_dir>` 替换为用户指定的存放路径：

```bash
fbb sdk install hs-fbb@master --dir <sdk_dir>
cd <sdk_dir>
fbb setup
```

> **⚠️ 注意**：SDK 目录路径不能包含空格或中文字符，否则 CMake 工具链解析会出现异常。

---

### 2. 创建并编译工程

> **📌 前置编译依赖检查：patch 工具**：编译过程中部分脚本依赖 `patch`，若未安装会报错。编译前请确认已安装：
> ```bash
> patch -v
> ```
> - **Linux**：`sudo apt install patch`（Debian/Ubuntu）或 `sudo dnf install patch`（Fedora）
> - **Windows**：通过 Git for Windows 自带的 bash 环境可直接使用，将 `C:\Program Files\Git\usr\bin` 添加到环境变量 PATH 即可；

编译时工具链会将应用代码与所依赖的 SDK 组件一并编译，最终生成可直接烧录到开发板的完整固件镜像（`.fwpkg`）。

```bash
# 1. 创建工程（在任意目录）
fbb create-project myapp
cd myapp

# 2. 查看可用目标
fbb list-targets

# 3. 设置编译目标
fbb set-target pack_diting_community

# 4. 编译
fbb build
```

#### 工程目录结构

`fbb create-project myapp` 生成的标准工程结构如下：

```
myapp/
├── fbb-project.toml                # 工程描述文件（chip、target、SDK 版本等）
├── CMakeLists.txt                  # 顶层工程文件
├── README.md
└── main/
    ├── CMakeLists.txt              # 主组件构建文件
    ├── include/
    │   └── samplepage/
    │       ├── SamplePage.h
    │       ├── SamplePresenter.h
    │       └── SampleView.h
    └── src/
        ├── SamplePage.cpp
        └── SamplePresenter.cpp
```

> **📌 说明**：模板固定使用 `samplepage` 作为组件名，生成的源文件以 `Sample` 前缀命名。开发者可直接在此基础上修改，或重命名为自己的组件名。

顶层 `CMakeLists.txt` 示例：

```cmake
cmake_minimum_required(VERSION 3.14.1)

set(FBB_PROJECT_COMPONENT_NAME "samplepage")
find_package(fbb REQUIRED HINTS
    "$ENV{FBB_SDK_DIR}/build/cmake"
    "$ENV{FBB_SDK_ROOT_DIR}/build/cmake")

project(myapp)
```

`main/CMakeLists.txt` 示例：

```cmake
set(COMPONENT_NAME "samplepage")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SamplePage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SamplePresenter.cpp
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(WHOLE_LINK true)
set(MAIN_COMPONENT true)
set(APP_TARGETS "diting-community")

build_component()
```

---

## 烧写与调试

```bash
# 烧写（-f 指定固件包路径，--manual-reset 用于手动触发复位）
# Windows
fbb flash -f "output\3322\fwpkg\diting-community.fwpkg" --port COM3 --chip Hi3322 --manual-reset --timeout 600
# Linux（普通用户登录烧写前需先授予串口权限 chmod 777 /dev/ttyUSB0）
fbb flash -f "output/3322/fwpkg/diting-community.fwpkg" --port /dev/ttyUSB0 --chip Hi3322 --manual-reset --timeout 600

# 串口监视（根据实际需要选择波特率）
fbb monitor --port COM3 --baud 750000
fbb monitor --port COM3 --baud 115200
```

> **💡 串口号说明**：`--port COM3` 中的串口号需根据实际连接的设备端口修改。Windows 下可在**设备管理器 → 端口 (COM 和 LPT)** 中查看当前设备对应的 COM 号；Linux 下通常为 `/dev/ttyUSB0` 或 `/dev/ttyACM0`。

---

## 其他

### 命令参考

#### fbb list-targets

列出 SDK 中所有可用的编译目标（Individual Target）和目标组（Target Group）。

```bash
fbb list-targets
```

**输出示例：**

```
可用目标:
==================================================

[3322] 单目标:
  3322-loaderboot
  3322-recovery
  3322-seliteos-release
  3322-ssb
  diting-community
  diting-community-native-js
  diting-community-xts

[3322] 目标组:
  pack_diting_community
  pack_diting_community_native_js
  pack_diting_community_xts

共 7 个目标、3 个目标组,跨 1 个芯片
```

---

#### fbb set-target

设置当前工程的默认编译目标，目标名保存在工程目录下的 `.fbb-target` 文件中，后续 `fbb build`、`fbb flash`、`fbb menuconfig` 在未指定目标时均会读取此值。

```bash
fbb set-target <目标名>
```

**示例：**

```bash
fbb set-target pack_diting_community
```

---

#### fbb get-target

查看当前工程已保存的默认目标。

```bash
fbb get-target
```

---

#### fbb create-project

在指定路径创建新工程，支持使用内置空白模板或基于示例工程创建。

```bash
fbb create-project <工程名> [--path <目录>]
```

| 参数 | 说明 |
|------|------|
| `<工程名>` | 必填。新工程的名称 |
| `--path <目录>` | 工程创建的父目录，默认为当前目录 |

**示例：**

```bash
# 使用空白模板在当前目录创建
fbb create-project myapp

# 在指定目录创建
fbb create-project myapp --path /home/user/projects
```

---

#### fbb create-component

在指定路径创建新组件，生成标准的组件目录结构和 CMakeLists.txt。命令会**自动**将组件放入工程根目录下的 `components/` 子目录中。

```bash
fbb create-component <组件名> [--path <工程根目录>]
```

> **⚠️ 执行目录要求**：必须在**工程根目录**下执行该命令。组件将被创建到工程目录内，构建系统才能自动发现并注册该组件。

**示例：**

```bash
cd myapp
fbb create-component my_sensor
```

执行后生成：

```
myapp/
├── components/
│   └── my_sensor/
│       ├── CMakeLists.txt
│       ├── Kconfig
│       ├── component.c
│       └── include/
│           └── component.h
└── ...
```

> **📌 空组件说明**：新建组件仅包含**空函数占位实现**，`component.c` 和 `component.h` 中的函数体均为空，开发者可在此基础上添加扩展功能逻辑。

---

#### fbb config get / set / unset

不打开图形界面，直接读取或修改目标的 Kconfig 选项（适合脚本/CI 场景）。

```bash
fbb config get <CONFIG_NAME>
fbb config set <CONFIG_NAME>=<值>
fbb config unset <CONFIG_NAME>
```

**示例：**

```bash
fbb config get CONFIG_ENABLE_FOO
fbb config set CONFIG_HEAP_SIZE=4096
fbb config unset CONFIG_DEBUG_LOG
```

> **📌 前提**：目标需已构建过一次（或运行过 `fbb menuconfig --mode defconfig`），`.config` 文件需存在。
>
> **⚠️ `fbb config set` 注意**：`set` 命令配置的 `CONFIG_NAME` 必须在当前 `.config` 中已存在（即 SDK 支持该选项）。若配置项不存在，命令会报错。可先用 `fbb config get <CONFIG_NAME>` 确认选项是否可用。

---

#### fbb menuconfig

打开基于 Kconfig 的图形化配置界面，可裁剪功能模块、调整编译开关。配置结果保存至对应目标的 `.config` 文件。

```bash
fbb menuconfig <目标名>
# 示例
fbb menuconfig diting-community
```

> **⚠️ 仅支持 Individual Target**：`menuconfig` **不支持 Group Target**（如 `pack_diting_community`）。必须指定组内的具体 Individual Target，例如：`fbb menuconfig diting-community`。

> **📌 说明**：修改配置后需重新运行 `fbb build` 使配置生效。

---

#### fbb build

编译当前目标；`--clean` 先清理构建输出再全量编译。

```bash
# 编译已保存的默认目标
fbb build

# 编译指定目标
fbb build <目标名>

# 清理后全量重新编译
fbb build --clean <目标名>
```

> **💡 增量编译**：`fbb build` 默认进行增量编译。CMake 会检测源文件变更，仅重新编译受影响的模块。

| 参数 | 说明 |
|------|------|
| `<目标名>` | 可选。未指定时使用 `fbb set-target` 保存的默认目标 |
| `--clean` | 清理构建输出后全量编译 |

---

#### fbb flash

通过 BurnToolCmd 将固件包（`.fwpkg`）烧写到开发板。

```bash
fbb flash -f <固件路径> [选项]
```

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `-f/--file <路径>` | 指定 `.fwpkg` 固件包路径 | 从 target 自动定位 |
| `--port <COMx>` | 指定串口（如 COM3、/dev/ttyUSB0） | 自动检测 |
| `--baud <N>` | 烧写波特率 | 读取 chip JSON |
| `--chip <名称>` | 手动指定芯片型号（如 Hi3322） | 从路径/目标名自动推断 |
| `--manual-reset` | 手动复位模式（首次烧写或板子无响应时） | 否 |
| `--load-only` | 仅烧写 load 分区（`*_load_only.fwpkg`） | 否 |
| `--timeout <秒>` | 烧写超时时间 | 180 |
| `--json-summary` | 在 stdout 末尾输出一行 JSON 摘要（适合脚本解析） | 否 |

**示例：**

```bash
# 标准烧写（手动按 RESET）
# Windows
fbb flash -f "output\3322\fwpkg\diting-community.fwpkg" --port COM3 --chip Hi3322 --manual-reset --timeout 600
# Linux（普通用户登录烧写前需先授予串口权限 chmod 777 /dev/ttyUSB0）
fbb flash -f "output/3322/fwpkg/diting-community.fwpkg" --port /dev/ttyUSB0 --chip Hi3322 --manual-reset --timeout 600
```

---

#### fbb monitor

打开串口监视器，实时查看开发板输出日志，支持超时退出与日志保存。

```bash
fbb monitor [选项]
```

| 选项 | 说明 |
|------|------|
| `--port <COMx>` | 串口名（自动检测时可省略） |
| `--baud <N>` | 波特率，默认 115200 |
| `--chip <名称>` | 芯片型号（单芯片 SDK 自动选择） |
| `--until <正则>` | 匹配到正则表达式时自动退出（exit 0） |
| `--timeout <秒>` | 超时秒数，超时未匹配 `--until` 则退出（exit 4） |
| `--reset` | 打开串口后发送复位命令 |
| `--log <文件>` | 同时将输出写入日志文件（如 `fbb-monitor.log`）；不指定则不记录 |

**示例：**

```bash
# 交互式监视（Ctrl+C 退出，根据实际需要选择波特率）
fbb monitor --port COM3 --baud 750000
fbb monitor --port COM3 --baud 115200

# 等待启动完成后自动退出（常用于 CI/CD）
fbb monitor --port COM3 --baud 115200 --until "boot ok" --timeout 30 --log boot.log
```

---

#### fbb list-examples

列出 SDK 内所有可用的示例工程。

```bash
fbb list-examples
```

---

#### fbb create-project-from-example

基于 SDK 内置示例工程创建新工程，适合快速开发特定功能。

```bash
fbb create-project-from-example <示例名> [--name <工程名>] [--path <目录>]
```

| 参数 | 说明 |
|------|------|
| `<示例名>` | 必填。示例名见 `fbb list-examples` |
| `--name <工程名>` | 可选。新工程目录名，默认与示例名相同 |
| `--path <目录>` | 工程创建的父目录，默认为当前目录 |

**示例：**

```bash
# 查看所有可用示例
fbb list-examples

# 基于示例创建（工程名与示例名相同）
fbb create-project-from-example heartrate --path ~/projects

# 指定工程名
fbb create-project-from-example heartrate --name myheartapp --path ~/projects
```

---

### 命令速查表

| 命令 | 功能 |
|------|------|
| `fbb setup` | 初始化构建环境（工具链、venv） |
| `fbb list-targets` | 列出所有可用目标和目标组 |
| `fbb set-target <目标>` | 设置默认编译目标（保存到工程 `.fbb-target`） |
| `fbb get-target` | 查看已保存的默认目标 |
| `fbb create-project <名称>` | 创建新工程 |
| `fbb create-component <名称>` | 创建新组件 |
| `fbb config get/set/unset` | 非交互式读写 Kconfig 选项 |
| `fbb menuconfig <目标>` | 图形化功能配置（仅 Individual Target） |
| `fbb build [目标]` | 增量编译 |
| `fbb flash -f <固件> --port <串口>` | 烧写固件 |
| `fbb monitor --port <串口> --baud <波特率>` | 串口监视器 |
| `fbb list-examples` | 列出可用示例 |
| `fbb create-project-from-example <示例>` | 基于示例创建工程 |
| `fbb describe` | 打印环境、SDK、工具链状态（诊断用） |
| `fbb doctor` | 诊断构建环境 |