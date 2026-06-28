# Hello World 应用开发入门指南

本文档以 `helloworldpage` 为例，带你在 FBB Diting 可穿戴开发板上快速跑通第一个原生 UI 应用，并了解如何基于它构建自己的 App。

---

## 目录

1. [应用框架背景知识](#1-应用框架背景知识)
2. [快速跑通 Hello World](#2-快速跑通-hello-world)
3. [文件结构与代码走读](#3-文件结构与代码走读)
4. [基于 Hello World 新建自己的 App](#4-基于-hello-world-新建自己的-app)
5. [注意事项](#5-注意事项)
6. [常见错误](#6-常见错误)
7. [社区贡献](#7-社区贡献)

---

## 1. 应用框架背景知识

### 1.1 MVP + SlicePage 调用流程

```
用户点击应用图标
    │
    ▼
Ability Manager Service（应用生命周期管理）
    │
    ▼
NativeAbility（全局调度）
    │
    ▼
AbilitySliceProxy<View, Presenter>   ← REGIST_MENU 注册（绑定 View 和 Presenter）
    ├──► View（应用视图）
    └──► Presenter（业务逻辑调度）
              │  通过 SlicePageFactory 创建 Hello World 应用页面
              ▼
         SlicePage（Hello World 应用页面）  ← REGIST_SLICE_PAGE 注册
              │  通过 OnStart 启动页面
              ▼
         Hello World 页面显示        
```

- **View**：负责 Hello World 应用页面展示 
- **Presenter**：处理页面业务逻辑，将数据变化反映到 View 层
- **SlicePage**：实现页面的生命周期管理

### 1.2 页面生命周期

```
OnStart(data)  →  OnResume()  →  OnPause()  →  OnStop()  →  ~Page()
   创建控件        页面可见        页面不可见      销毁非UI资源   释放控件
```

### 1.3 CMake 收集机制和新增文件

`nativeapp/CMakeLists.txt` ：通过 CMakeList 语法将源码文件添加到编译中

```cmake
file(GLOB_RECURSE SOURCES "${NativeApp}/nativeui/*.cpp")
```

- `.cpp` 文件放在 `nativeui/<myapp>/` 下，此目录下源文件会通过路径添加到编译列表。
-  新增头文件路径在CMake里的 PRIVATE_HEADER 变量：
```cmake
set(PRIVATE_HEADER
${NativeApp}/nativeui/include/helloworldpage
)
```

> 如果新增 `.cpp` 后没有被编译进去，先确认文件路径正确，再执行 `python3 build.py -c diting-community`，排除 CMake 缓存导致的生成文件未更新问题。

---

## 2. 快速跑通 Hello World

### 编译

在 `src/` 目录下执行：

```bash
# 打包成可烧录固件(-c 执行清理动作)
python3 build.py -c pack_diting_community
```

### 预期结果 

烧录后逐项验收：

- [ ] `python3 build.py -c pack_diting_community` 打包通过
- [ ] 设备应用列表中出现 **HelloWorld**
- [ ] 点击 HelloWorld，页面显示 **Hello World** 文字
- [ ] 串口日志能看到 `HelloWorldPage OnStart`（串口日志打印INFO级别：WEARABLE_LOGI）

---

## 3. 文件结构与代码走读

### 3.1 文件结构

Hello World 的文件分布在两个目录下，不在同一处：

```
src/application/wearable/nativeapp/
├── nativeui/
│   └── helloworldpage/
│       ├── HelloWorldPresenter.cpp   # 注册到应用列表
│       └── HelloWorldPage.cpp        # 页面应用实现
└── nativeui/include/
    └── helloworldpage/
        ├── HelloWorldView.h          # View 接口
        ├── HelloWorldPresenter.h     # Presenter 声明
        └── HelloWorldPage.h          # Page 声明
```

`.cpp` 和 `.h` 分开存放是整个 `nativeui` 层的统一惯例，新建应用时需要遵循同样的目录结构。

### 3.2 各文件职责

| 文件 | 功能 | 关键内容 |
|---|---|---|
| `HelloWorldView.h` | 定义页面应用接口 | 继承 `View<HelloWorldPresenter>`，可按需声明纯虚接口 |
| `HelloWorldPresenter.h/.cpp` | 业务逻辑 + 注册入口 | `.cpp` 里用 `REGIST_MENU` 注册到应用列表 |
| `HelloWorldPage.h/.cpp` | Hello World 页面逻辑实现 | `.cpp` 里用 `REGIST_SLICE_PAGE` 注册页面，`OnStart` 启动页面 |

#### 关键类说明
| 类名 | 说明 |
|------|------|
| `HelloWorldPage` | 管理UI控件和页面生命周期 |
| `HelloWorldView` | 定义视图交互接口 |
| `HelloWorldPresenter` | 处理业务逻辑和数据 |

### 3.3 HelloWorldPresenter.cpp —— 注册到应用列表

```cpp
namespace OHOS {
#ifdef SDK_DITING_COMMUNITY
REGIST_MENU(VIEW_HELLOWORLD, HelloWorldView, HelloWorldPresenter, EMPTY_ICON, EMPTY_ICON, "HelloWorld");
//          ↑ 唯一应用 ID     ↑ View 类       ↑ Presenter 类       ↑ 圆形图标   ↑ 六边形图标  ↑ 应用名称
#endif
}
```

SDK_DITING_COMMUNITY 确定是被定义的，在程序启动时（全局静态对象初始化阶段）自动执行，无需手动调用。
`VIEW_HELLOWORLD` 定义在 `nativelauncher/include/AppViewIDs.h` 的枚举中，是该应用的全局唯一 ID。

> **`EMPTY_ICON` / `EMPTY_IMAGE` 是应用资源**：
> - `EMPTY_IMAGE` 路径为 `user/res/EMPTY.bin`
> - `EMPTY_ICON` 路径为 `user/res/EMPTY_ICON.bin` 
>
> 开发者请导入实际需要的资源（参见[5. 注意事项](#5-注意事项)）。

### 3.4 HelloWorldPage.cpp —— 核心逻辑实现

#### 注册页面

```cpp
static constexpr uint16_t HELLOWORLD_PAGE = 1;

#ifdef SDK_DITING_COMMUNITY
REGIST_SLICE_PAGE(VIEW_HELLOWORLD, HELLOWORLD_PAGE, HelloWorldPage, true);
//                ↑ 所属应用 ID     ↑ 页面 ID        ↑ 页面类         ↑ 是否为入口页
#endif
```

一个应用可以注册多个页面，`isMainPage = true` 用于设置主页面。

#### OnStart： 启动应用

```cpp
void HelloWorldPage::OnStart(void *data)
{
    // step1. 预加载图片资源（Hello World 用空资源占位）
    bool ret = ImageCacheManager::GetInstance().LoadAllInMultiRes(EMPTY_IMAGE);
    if (ret == false) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LoadEmptyImage fail");
        return;
    }

    // step2. 创建根容器，设置容器大小（400×400）
    container_ = new UIViewGroup();
    if (container_ == nullptr) { return; }  // new 可能返回 nullptr 的方式做防御检查
    container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    container_->SetDraggable(true);  // 允许手势拖动（支持向右滑退出应用）
    container_->SetTouchable(true);

    // step3. 创建文本标签，设置文本布局
    helloWorldLabel_ = new UILabel();
    if (helloWorldLabel_ == nullptr) { return; }
    // x=121, y=161, w=193, h=40 → 在 400×400 屏幕上大致水平居中、略高于中央
    helloWorldLabel_->SetPosition(121, 161, 193, 40);
    helloWorldLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    helloWorldLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30);
    helloWorldLabel_->SetText("Hello World");

    // step4. 组装视图树，交给图形框架渲染
    container_->Add(helloWorldLabel_);
    AddViewToPageContainer(container_);  // 必须调用，否则页面为空
}
```

#### 析构：释放所有资源

```cpp
HelloWorldPage::~HelloWorldPage()
{
    if (container_ != nullptr) {
        container_->RemoveAll();  // 先断开子节点引用，再 delete 容器
        delete container_;
        container_ = nullptr;
    }
    if (helloWorldLabel_ != nullptr) {
        delete helloWorldLabel_;
        helloWorldLabel_ = nullptr;
    }
    // Load 和 Unload 必须成对，否则造成资源泄漏
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(EMPTY_IMAGE);
}
```

---

## 4. 基于 Hello World 新建自己的 App

### 4.1 改动清单

新建一个 App（以 `MyApp` 为例）通常只需要以下改动（正式应用可能还需额外的资源文件、多语言支持或服务依赖）：

- [ ] 新建 `nativeui/myapp/MyAppPresenter.cpp`
- [ ] 新建 `nativeui/myapp/MyAppPage.cpp`
- [ ] 新建 `nativeui/include/myapp/MyAppView.h`
- [ ] 新建 `nativeui/include/myapp/MyAppPresenter.h`
- [ ] 新建 `nativeui/include/myapp/MyAppPage.h`
- [ ] 在 `AppViewIDs.h` 中新增 `VIEW_MYAPP`
- [ ] 在 `MyAppPresenter.cpp` 中调用 `REGIST_MENU`
- [ ] 在 `MyAppPage.cpp` 中调用 `REGIST_SLICE_PAGE`

`CMakeLists.txt`（见 [1.3 CMake 收集机制和新增文件](#13-cmake-收集机制和新增文件)）。

### 4.2 名称修改表

如果开发者想基于 Hello World 开发自己的应用，请将 Hello World 的所有命名按如下规则修改：

| Hello World | MyApp | 位置 |
|---|---|---|
| `VIEW_HELLOWORLD` | `VIEW_MYAPP` | `AppViewIDs.h` 枚举、两个注册宏 |
| `HelloWorldView` | `MyAppView` | `REGIST_MENU` 参数、头文件类名 |
| `HelloWorldPresenter` | `MyAppPresenter` | `REGIST_MENU` 参数、头文件类名、`SlicePage<>` 模板参数 |
| `HelloWorldPage` | `MyAppPage` | `REGIST_SLICE_PAGE` 参数、头文件类名 |
| `HELLOWORLD_PAGE` | `MYAPP_MAIN_PAGE` | `REGIST_SLICE_PAGE` 参数 |
| `"HelloWorld"` | `"MyApp"` | `REGIST_MENU` 应用名称参数 |
| `helloworldpage/` | `myapp/` | 目录名、`#include` 路径 |

### 4.3 关键代码片段

**AppViewIDs.h** —— 分配唯一 ID
```cpp
typedef enum : uint16_t {
    ...
    VIEW_HELLOWORLD,
    VIEW_MYAPP,          // 添加在 VIEW_MAX_INTER_ARRY_APP 之前
    ...
    VIEW_MAX_INTER_ARRY_APP = 0x7FFF,
} AppViewId;
```

**MyAppPresenter.cpp** —— 注册到应用列表
```cpp


#include "NativeRegisterManager.h"
#include "myapp/MyAppView.h"
#include "myapp/MyAppPresenter.h"

namespace OHOS {
#ifdef SDK_DITING_COMMUNITY
REGIST_MENU(VIEW_MYAPP, MyAppView, MyAppPresenter, EMPTY_ICON, EMPTY_ICON, "MyApp");
#endif
}
```

**MyAppPage.cpp 关键片段**

> 以下为可直接参考的完整 `OnStart`页面启动，在此基础上替换文字或添加控件。

```cpp
static constexpr uint16_t MYAPP_MAIN_PAGE = 1;

#ifdef SDK_DITING_COMMUNITY
REGIST_SLICE_PAGE(VIEW_MYAPP, MYAPP_MAIN_PAGE, MyAppPage, true);
#endif

void MyAppPage::OnStart(void *data)
{
    bool ret = ImageCacheManager::GetInstance().LoadAllInMultiRes(EMPTY_IMAGE);
    if (ret == false) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LoadEmptyImage fail");
        return;
    }

    container_ = new UIViewGroup();
    if (container_ == nullptr) { return; }
    container_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    container_->SetDraggable(true);
    container_->SetTouchable(true);

    label_ = new UILabel();
    if (label_ == nullptr) { return; }
    label_->SetPosition(100, 175, 200, 50);
    label_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 28);
    label_->SetText("My First App");    // 替换为你的内容

    container_->Add(label_);
    AddViewToPageContainer(container_); // 必须调用，否则页面为空
}
```

析构函数参照 `HelloWorldPage::~HelloWorldPage()` 原样实现，将 `helloWorldLabel_` 替换为 `label_` 即可。

### 4.4 验收标准

新 App 完成后，同样逐项验收（首次验证用完整构建；后续只改 Native 页面可用组件构建）：

- [ ] `python3 build.py -c pack_diting_community` 通过
- [ ] 应用列表出现新应用名称
- [ ] 点击后页面显示正常
- [ ] 串口日志能看到 `MyAppPage OnStart`

---

## 5. 注意事项

**不要做什么：**

- 不要修改 `interim_binary/` 下的预编译库，这些是闭源组件。
- 不要复用已有的 `VIEW_XXX` ID，每个应用必须有唯一 ID，重复会导致运行时冲突。
- 不要提交编译产物（`output/` 目录）到代码仓库。
- 普通 UI 示例不要修改 `build/config/target_config/3322/config.py`，无需改动编译目标配置。
- 不要修改与自己应用无关的模块，保持 diff 最小化。

**资源说明：**

`EMPTY_ICON` 和 `EMPTY_IMAGE` 是开发者的应用资源：
- `EMPTY_IMAGE`（`user/res/EMPTY.bin`）：页面加载的图片资源包。
- `EMPTY_ICON`（`user/res/EMPTY_ICON.bin`）：应用列表图标，正式应用需替换为实际图标资源，否则应用列表显示空白图标。

---

## 6. 常见错误

| 错误现象 | 原因 | 解决方法 |
|---|---|---|
| `VIEW_MYAPP` undeclared identifier | 未在 `AppViewIDs.h` 中添加枚举项 | 在 `VIEW_MAX_INTER_ARRY_APP` 前添加 `VIEW_MYAPP` |
| `fatal error: myapp/MyAppPage.h: No such file` | include 路径写错，或头文件放错目录 | 头文件必须在 `nativeui/include/myapp/` 下，不是 `nativeui/myapp/` |
| `undefined reference to 'OHOS::MyAppPage::...'` | .cpp 文件没放在 `nativeui/` 下，未被 GLOB_RECURSE 收集 | 确认文件路径为 `nativeui/myapp/MyAppPage.cpp` |
| 应用列表不出现新应用 | `REGIST_MENU` 的类名与实际类名不一致，或宏在错误的 `#ifdef` 分支内 | 核对宏参数中的 View/Presenter 类名，确认在 `#ifdef SDK_DITING_COMMUNITY` 内 |
| 点进去页面空白 | `AddViewToPageContainer(container_)` 未被调用，或 `new` 失败后提前 `return` | 检查 `OnStart` 中的 `nullptr` 判断和最后一行的 `AddViewToPageContainer` |
| `namespace OHOS` | 类定义未在 `OHOS` 命名空间，实现文件未包裹 | `.cpp` 文件的实现，代码需包在 `namespace OHOS { }` 中 |

---

## 7. 社区贡献

向社区贡献代码时，PR 描述建议包含

```
## 改动说明
新增 <AppName> 示例应用，展示 <功能点>。

## 验证结果
- [ ] python3 build.py -c pack_diting_community 通过
- [ ] 设备应用列表出现 <AppName>
- [ ] 点击后页面显示正常

## 影响范围
仅新增 nativeui/<appname>/ 及对应头文件，AppViewIDs.h 新增 VIEW_XXX，未修改其他模块。
```
