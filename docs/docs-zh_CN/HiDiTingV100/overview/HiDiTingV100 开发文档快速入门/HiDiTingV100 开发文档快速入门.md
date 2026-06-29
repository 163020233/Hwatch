# 前言<a name="ZH-CN_TOPIC_0000002509502731"></a>

**概述<a name="section4537382116410"></a>**

本文为首次使用版本的开发人员而写。目的是供开发人员快速掌握文档的整体架构脉络，指导开发人员一步一步深入开发。同时帮助不同的开发人员快速找到自己所需要的文档。

>![](public_sys-resources/icon-notice.gif) **须知：** 
> -   第1章节为版本包文档的目录，描述文档总体的结构。
> -   第2章节为基础功能开发、业务特性开发以及工具维测相关文档介绍部分。

**读者对象<a name="section4378592816410"></a>**

本文档主要适用于以下工程师：

-   技术支持工程师
-   软件开发工程师

**符号约定<a name="section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="table2622507016410"></a>
<table><thead align="left"><tr id="row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="p6450074116410"><a name="p6450074116410"></a><a name="p6450074116410"></a><strong id="b2136615816410"><a name="b2136615816410"></a><a name="b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="p5435366816410"><a name="p5435366816410"></a><a name="p5435366816410"></a><strong id="b5941558116410"><a name="b5941558116410"></a><a name="b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p3734547016410"><a name="p3734547016410"></a><a name="p3734547016410"></a><a name="image2670064316410"></a><a name="image2670064316410"></a><span><img class="" id="image2670064316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002509382697.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p1757432116410"><a name="p1757432116410"></a><a name="p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1432579516410"><a name="p1432579516410"></a><a name="p1432579516410"></a><a name="image4895582316410"></a><a name="image4895582316410"></a><span><img class="" id="image4895582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002477462748.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p959197916410"><a name="p959197916410"></a><a name="p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1232579516410"><a name="p1232579516410"></a><a name="p1232579516410"></a><a name="image1235582316410"></a><a name="image1235582316410"></a><span><img class="" id="image1235582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002477302770.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p123197916410"><a name="p123197916410"></a><a name="p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p2204984716410"><a name="p2204984716410"></a><a name="p2204984716410"></a><a name="image4504446716410"></a><a name="image4504446716410"></a><span><img class="" id="image4504446716410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002509502739.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4388861916410"><a name="p4388861916410"></a><a name="p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="p1238861916410"><a name="p1238861916410"></a><a name="p1238861916410"></a>"须知"不涉及人身伤害。</p>
</td>
</tr>
<tr id="row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p5555360116410"><a name="p5555360116410"></a><a name="p5555360116410"></a><a name="image799324016410"></a><a name="image799324016410"></a><span><img class="" id="image799324016410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002509382699.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4612588116410"><a name="p4612588116410"></a><a name="p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="p1232588116410"><a name="p1232588116410"></a><a name="p1232588116410"></a>"说明"不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# 文档目录<a name="ZH-CN_TOPIC_0000002535908873"></a>

在docs-zh_CN目录：

```
└── HiDiTingV100                                         # HiDiTingV100整体中文开发文档根目录
    ├── api-reference                                       # API接口参考总目录
    │   ├── API 规范                                            # API规范文档
    │   ├── HiDiTingV100 2131E AT命令 用户指南                  # Hi2131E相关指南
    │   ├── HiDiTingV100 2131E NV 用户指南                      # Hi2131E相关指南
    │   ├── HiDiTingV100 2131E VoLTE 开发指南                   # Hi2131E相关指南
    │   ├── HiDiTingV100 2131E 维测功能 指导书                  # Hi2131E相关指南
    │   ├── HiDiTingV100 AEF overlay 开发指南                   # 音频音效算法集成开发指南 
    │   ├── HiDiTingV100 AT指令 使用指南                        # 开发调试命令使用介绍
    │   ├── HiDiTingV100 CAT1 综测指南                          # CAT1测试相关适配指导以及流程参考
    │   ├── HiDiTingV100 FOTA 开发指南                          # OTA升级特性开发指导
    │   ├── HiDiTingV100 GNSS接口协议 开发指南                  # GS21相关指南
    │   ├── HiDiTingV100 GNSS硬测 测试指南                      # GS21相关指南
    │   ├── HiDiTingV100 GNSS集成 开发指南                      # GS21相关指南
    │   ├── HiDiTingV100 LwIP 调测指南                          # 开发以及适配LwIP组件调试指导
    │   ├── HiDiTingV100 mbed TLS 开发指南                      # 专用安全标准接口对接适配指导
    │   ├── HiDiTingV100 NV存储 用户指南                        # NV特性使用介绍
    │   ├── HiDiTingV100 OpenHarmony JS应用开发 用户指南        # 基于HiDiTingV100R001解决方案的JS应用开发指导
    │   ├── HiDiTingV100 OpenHarmony Native Slice开发 用户指南  # 基于HiDiTingV100R001解决方案的native slice相关组件开发指导
    │   ├── HiDiTingV100 OpenHarmony Native组件开发 用户指南    # 基于HiDiTingV100R001解决方案的native相关组件开发指导
    │   ├── HiDiTingV100 OpenHarmony XTS认证 开发指南           # 描述OpenHarmony XTS开发适配及认证指导
    │   ├── HiDiTingV100 SELiteOS软件 开发指南                  # 安全TES环境OS软件介绍以及开发指导
    │   ├── HiDiTingV100 UIKit 开发指南                         # UIKit图形引擎开发指导
    │   ├── HiDiTingV100 UX API 使用指南                        # UX API手册
    │   ├── HiDiTingV100 XGNSS 开发指南                         # GS21相关指南
    │   ├── HiDiTingV100 三方应用 开发指南                      # 三方应用集成开发指导，主要包含类似阿里支付等三方功能集成
    │   ├── HiDiTingV100 产品简介                               # 简介相关指南
    │   ├── HiDiTingV100 低功耗软件 开发指南                    # 低功耗系统介绍以及适配开发指导
    │   ├── HiDiTingV100 典型外设适配 开发指南                  # 外设存储器件等典型外设适配开发指导
    │   ├── HiDiTingV100 图形视频驱动 开发指南                  # GPU相关驱动功能的适配开发指导
    │   ├── HiDiTingV100 图形视频驱动 调试指南                  # GPU相关驱动功能的适配开发指导
    │   ├── HiDiTingV100 多媒体软件 开发指南                    # 媒体相关功能开发指导
    │   ├── HiDiTingV100 安全启动 使用指南                      # 安全启动流程介绍以及开发指导
    │   ├── HiDiTingV100 安全软件 使用指南                      # 安全相关软件以及加密算法开发指导
    │   ├── HiDiTingV100 文件系统 使用指南                      # 文件系统相关功能接口的适配开发指导
    │   ├── HiDiTingV100 星闪API 使用指南                       # 星闪API手册
    │   ├── HiDiTingV100 消息中心 开发指南                      # 描述消息中心的实现机制及客户应用开发指导
    │   ├── HiDiTingV100 蓝牙API 使用指南                       # 蓝牙API手册
    │   ├── HiDiTingV100 蓝牙／星闪射频 测试指南                # 射频相关测试指导
    │   ├── HiDiTingV100 蓝牙认证 指导书                        # 蓝牙认证测试指导
    │   ├── HiDiTingV100 设备驱动 开发指南                      # 基础设备驱动使用开发指导
    │   ├── HiDiTingV100 资源部署 用户指南                      # 系统资源部署介绍以及资源部署调整方法介绍
    │   ├── HiDiTingV100 软件开发指南                           # 解决方案系统性介绍，入口文档
    │   ├── HiDiTingV100 音频算法插件 开发指南                  # 开发以及适配音频算法插件的指导
    │   ├── HiDiTingV100 音频驱动 开发指南                      # 音频驱动相关功能接口使用适配指导
    │   ├── HiDiTingV100 音频驱动 调试指南                      # 音频驱动相关调试手段介绍
    │   └── 穿戴类产品对外交互协议                              # HiDiTingV100R001解决方案对外通讯格式以及协议介绍
    ├── FAQ                                                 # 开发常见问题汇总目录
    │   └── HiDiTingV100 常见问题FAQ                            #描述导入过程中遇到的常见问题以及解决方案
    ├── get-started                                         # 新手快速入门引导目录
    │   └── HiDiTingV100 开发环境快速入门                       # 介绍整个SDK的环境准备、编译、烧写 
    ├── hardware-guide                                      # 硬件设计参考文档目录
    │   └── HiDiTingV100 硬件 用户指南                          # 硬件相关指南
    ├── others                                              # 产线、二次开发说明文档目录
    │   ├── HiDiTingV100 二次开发网络安全注意事项               # 用户开发可能会用到的网络安全注意事项说明
    │   └── HiDiTingV100 产线 测试指南                          # 产线测试相关适配指导以及流程参考
    ├── overall-architecture                                    # 整体软件架构设计文档目录
    │   ├── figures                                             # 架构框图、模块交互示意图素材文件夹
    │   └── HiDiTingV100 代码框架说明                           # 源码分层结构、组件依赖、编译体系说明
    ├── overview                                            # 产品全局总览介绍目录
    │   └── HiDiTingV100 开发文档快速入门                       # 介绍整个文档体系的指导性文档
    ├── samples                                             # 各功能模块Demo示例工程存放目录
    │    ├── helloworld                                        # Hello World界面式应用示例说明
    │    ├── gpio                                              # gpio（通用输入输出）驱动Demo示例说明
    │    ├── pwm                                               # pwm（脉冲宽度调制）驱动Demo示例说明
    │    ├── timer                                             # timer（定时器）驱动Demo示例说明
    │    └── watchdog                                          # watchdog （看门狗）驱动Demo示例说明
    └── tools                                               # 配套开发、烧录、量产工具文档总目录
        ├── HiDiTingV100 2131E FOTA升级包制作工具 用户指南      # Hi2131E升级包制作工具指南
        ├── HiDiTingV100 BurnTool工具 使用指南                  # 整机固件烧录量产工具使用说明
        ├── HiDiTingV100 DebugKits工具 使用指南                 # 系统日志/调试工具使用指南
        ├── HiDiTingV100 DFX软件 开发指南                       # 系统维测相关功能开发使用指导
        ├── HiDiTingV100 产测工具 使用指南                      # 产线测试工具使用指导
        └── HiDiTingV100 图形工具 使用指南                      # 图形工具使用介绍
```

# 文档详细描述<a name="ZH-CN_TOPIC_0000002535948847"></a>

HiDiTingV100R001解决方案的文档体系包含软硬件设计类、环境类介绍文档、基础功能介绍文档、业务特性增量开发相关文档以及工具维测相关文档。对于首次阅读的用户来讲，首先阅读《HiDiTingV100 开发文档快速入门》，了解文档系统介绍。其次阅读《HiDiTingV100 软件开发指南》和《HiDiTingV100 代码框架说明》，了解SDK基础业务功能、代码框架、系统框架介绍。

- [HiDiTingV100 软件开发指南](../../api-reference/HiDiTingV100%20软件开发指南/HiDiTingV100%20软件开发指南.md)

- [HiDiTingV100 代码框架说明](../../overall-architecture/HiDiTingV100%20代码框架说明.md)

- [HiDiTingV100 硬件 用户指南](../../hardware-guide/HiDiTingV100%20硬件%20用户指南/HiDiTingV100%20硬件%20用户指南.md)

- [HiDiTingV100 产品简介](../../api-reference/HiDiTingV100%20产品简介/HiDiTingV100%20产品简介.md) 

-   **[基础功能开发](#ZH-CN_TOPIC_0000002535948849)**  

-   **[特性开发](#ZH-CN_TOPIC_0000002504109060)**  

-   **[工具维测](#ZH-CN_TOPIC_0000002535908891)**  

## 基础功能开发<a name="ZH-CN_TOPIC_0000002535948849"></a>

-   **[安全相关](#ZH-CN_TOPIC_0000002504109056)**  

-   **[设备驱动](#ZH-CN_TOPIC_0000002535908879)**  

-   **[资源部署](#ZH-CN_TOPIC_0000002535948851)**  

-   **[典型外设适配](#ZH-CN_TOPIC_0000002504268904)**  

-   **[低功耗](#ZH-CN_TOPIC_0000002504109058)**  

-   **[文件系统](#ZH-CN_TOPIC_0000002535908881)**  

-   **[NV存储](#ZH-CN_TOPIC_0000002535948853)**  

-   **[产测](#ZH-CN_TOPIC_0000002504268906)**  

-   **[AT](#ZH-CN_TOPIC_0000002549288337)**  

-   **[VoLTE](#ZH-CN_TOPIC_0000002517608552)**  

### 安全相关<a name="ZH-CN_TOPIC_0000002504109056"></a>

- [HiDiTingV100 安全启动 使用指南](../../api-reference/HiDiTingV100%20安全启动%20使用指南/HiDiTingV100%20安全启动%20使用指南.md)

- [HiDiTingV100 安全软件 使用指南](../../api-reference/HiDiTingV100%20安全软件%20使用指南/HiDiTingV100%20安全软件%20使用指南.md)

- [HiDiTingV100 SELiteOS软件 开发指南](../../api-reference/HiDiTingV100%20SELiteOS软件%20开发指南/HiDiTingV100%20SELiteOS软件%20开发指南.md)

- [HiDiTingV100 二次开发网络安全 注意事项](../../others/HiDiTingV100%20二次开发网络安全注意事项/HiDiTingV100%20二次开发网络安全注意事项.md)

- [HiDiTingV100 mbed TLS 开发指南](../../api-reference/HiDiTingV100%20mbed%20TLS%20开发指南/HiDiTingV100%20mbed%20TLS%20开发指南.md)

### 设备驱动<a name="ZH-CN_TOPIC_0000002535908879"></a>

- [HiDiTingV100 设备驱动 开发指南](../../api-reference/HiDiTingV100%20设备驱动%20开发指南/HiDiTingV100%20设备驱动%20开发指南.md)

### 资源部署<a name="ZH-CN_TOPIC_0000002535948851"></a>

- [HiDiTingV100 资源部署 用户指南](../../api-reference/HiDiTingV100%20资源部署%20用户指南/HiDiTingV100%20资源部署%20用户指南.md)

### 典型外设适配<a name="ZH-CN_TOPIC_0000002504268904"></a>

- [HiDiTingV100 典型外设适配 开发指南](../../api-reference/HiDiTingV100%20典型外设适配%20开发指南/HiDiTingV100%20典型外设适配%20开发指南.md)

### 低功耗<a name="ZH-CN_TOPIC_0000002504109058"></a>

- [HiDiTingV100 低功耗软件 开发指南](../../api-reference/HiDiTingV100%20低功耗软件%20开发指南/HiDiTingV100%20低功耗软件%20开发指南.md)

### 文件系统<a name="ZH-CN_TOPIC_0000002535908881"></a>

- [HiDiTingV100 文件系统 使用指南](../../api-reference/HiDiTingV100%20文件系统%20使用指南/HiDiTingV100%20文件系统%20使用指南.md)

### NV存储<a name="ZH-CN_TOPIC_0000002535948853"></a>

- [HiDiTingV100 NV存储 用户指南](../../api-reference/HiDiTingV100%20NV存储%20用户指南/HiDiTingV100%20NV存储%20用户指南.md)

- [HiDiTingV100 2131E NV 用户指南](../../api-reference/HiDiTingV100%202131E%20NV%20用户指南/HiDiTingV100%202131E%20NV%20用户指南.md)（Hi2131E）

### 产测<a name="ZH-CN_TOPIC_0000002504268906"></a>

- [HiDiTingV100 产线 测试指南](../../others/HiDiTingV100%20产线%20测试指南/HiDiTingV100%20产线%20测试指南.md)

- [HiDiTingV100 产测工具 使用指南](../../tools/HiDiTingV100%20产测工具%20使用指南/HiDiTingV100%20产测工具%20使用指南.md)

- [HiDiTingV100 CAT1 综测指南](../../api-reference/HiDiTingV100%20CAT1%20综测指南/HiDiTingV100%20CAT1%20综测指南.md)

### AT<a name="ZH-CN_TOPIC_0000002549288337"></a>

- [HiDiTingV100 AT指令 使用指南](../../api-reference/HiDiTingV100%20AT指令%20使用指南/HiDiTingV100%20AT指令%20使用指南.md)

- [HiDiTingV100 2131E AT命令 用户指南](../../api-reference/HiDiTingV100%202131E%20AT命令%20用户指南/HiDiTingV100%202131E%20AT命令%20用户指南.md)（Hi2131E）

### VoLTE<a name="ZH-CN_TOPIC_0000002517608552"></a>

- [HiDiTingV100 2131E VoLTE 开发指南](../../api-reference/HiDiTingV100%202131E%20VoLTE%20开发指南/HiDiTingV100%202131E%20VoLTE%20开发指南.md)

## 特性开发<a name="ZH-CN_TOPIC_0000002504109060"></a>

-   **[蓝牙/星闪](#ZH-CN_TOPIC_0000002535908883)**  

-   **[CAT1](#ZH-CN_TOPIC_0000002549208857)**  

-   **[GNSS](#ZH-CN_TOPIC_0000002517768990)**  

-   **[升级](#ZH-CN_TOPIC_0000002535948855)**  

-   **[鸿蒙JS应用](#ZH-CN_TOPIC_0000002504268908)**  

-   **[鸿蒙Native应用](#ZH-CN_TOPIC_0000002504109062)**  

-   **[UIKit](#ZH-CN_TOPIC_0000002535908885)**  

-   **[多媒体](#ZH-CN_TOPIC_0000002535948857)**  

-   **[射频测试](#ZH-CN_TOPIC_0000002504268910)**  

-   **[图形驱动](#ZH-CN_TOPIC_0000002504109064)**  

-   **[音频驱动](#ZH-CN_TOPIC_0000002535908889)**  

-   **[消息中心](#ZH-CN_TOPIC_0000002535948859)**  

-   **[三方应用](#ZH-CN_TOPIC_0000002504268912)**  

-   **[OpenHarmony XTS认证](#ZH-CN_TOPIC_0000002504109066)**  

-   **[定位](#ZH-CN_TOPIC_0000002549288857)**  

### 蓝牙/星闪<a name="ZH-CN_TOPIC_0000002535908883"></a>

- [HiDiTingV100 蓝牙API 使用指南](../../api-reference/HiDiTingV100%20蓝牙API%20使用指南/HiDiTingV100%20蓝牙API%20使用指南.md)

- [HiDiTingV100 星闪API 使用指南](../../api-reference/HiDiTingV100%20星闪API%20使用指南/HiDiTingV100%20星闪API%20使用指南.md)

- [穿戴类产品对外交互协议](../../api-reference/穿戴类产品对外交互协议/穿戴类产品对外交互协议.md)

- [HiDiTingV100 蓝牙认证 指导书](../../api-reference/HiDiTingV100%20蓝牙认证%20指导书/HiDiTingV100%20蓝牙认证%20指导书.md)

### CAT1<a name="ZH-CN_TOPIC_0000002549208857"></a>

- [HiDiTingV100 CAT1 综测指南](../../api-reference/HiDiTingV100%20CAT1%20综测指南/HiDiTingV100%20CAT1%20综测指南.md)

- [HiDiTingV100 2131E AT命令 用户指南](../../api-reference/HiDiTingV100%202131E%20AT命令%20用户指南/HiDiTingV100%202131E%20AT命令%20用户指南.md)

- [HiDiTingV100 2131E VoLTE 开发指南](../../api-reference/HiDiTingV100%202131E%20VoLTE%20开发指南/HiDiTingV100%202131E%20VoLTE%20开发指南.md)

- [HiDiTingV100 2131E NV 用户指南](../../api-reference/HiDiTingV100%202131E%20NV%20用户指南/HiDiTingV100%202131E%20NV%20用户指南.md)

- [HiDiTingV100 2131E 维测功能 指导书](../../api-reference/HiDiTingV100%202131E%20维测功能%20指导书/HiDiTingV100%202131E%20维测功能%20指导书.md)

- [HiDiTingV100 2131E FOTA升级包制作工具 用户指南](../../tools/HiDiTingV100%202131E%20FOTA升级包制作工具%20用户指南/HiDiTingV100%202131E%20FOTA升级包制作工具%20用户指南.md)

### GNSS<a name="ZH-CN_TOPIC_0000002517768990"></a>

- [HiDiTingV100 GNSS集成 开发指南](../../api-reference/HiDiTingV100%20GNSS集成%20开发指南/HiDiTingV100%20GNSS集成%20开发指南.md)

- [HiDiTingV100 XGNSS 开发指南](../../api-reference/HiDiTingV100%20XGNSS%20开发指南/HiDiTingV100%20XGNSS%20开发指南.md)

- [HiDiTingV100 GNSS接口协议 开发指南](../../api-reference/HiDiTingV100%20GNSS接口协议%20开发指南/HiDiTingV100%20GNSS接口协议%20开发指南.md)

### 升级<a name="ZH-CN_TOPIC_0000002535948855"></a>

- [HiDiTingV100 FOTA 开发指南](../../api-reference/HiDiTingV100%20FOTA%20开发指南/HiDiTingV100%20FOTA%20开发指南.md)

### 鸿蒙JS应用<a name="ZH-CN_TOPIC_0000002504268908"></a>

- [HiDiTingV100 OpenHarmony JS应用开发 用户指南](../../api-reference/HiDiTingV100%20OpenHarmony%20JS应用开发%20用户指南/HiDiTingV100%20OpenHarmony%20JS应用开发%20用户指南.md)

### 鸿蒙Native应用<a name="ZH-CN_TOPIC_0000002504109062"></a>

- [HiDiTingV100 OpenHarmony Native组件开发 用户指南](../../api-reference/HiDiTingV100%20OpenHarmony%20Native组件开发%20用户指南/HiDiTingV100%20OpenHarmony%20Native组件开发%20用户指南.md)

- [HiDiTingV100 OpenHarmony Native Slice开发 用户指南](../../api-reference/HiDiTingV100%20OpenHarmony%20Native%20Slice开发%20用户指南/HiDiTingV100%20OpenHarmony%20Native%20Slice开发%20用户指南.md)

- [HiDiTingV100 LwIP 调测指南](../../api-reference/HiDiTingV100%20LwIP%20调测指南/HiDiTingV100%20LwIP%20调测指南.md)

### UIKit<a name="ZH-CN_TOPIC_0000002535908885"></a>

- [HiDiTingV100 UIKit 开发指南](../../api-reference/HiDiTingV100%20UIKit%20开发指南/HiDiTingV100%20UIKit%20开发指南.md)

- [HiDiTingV100 HiSpark Studio for VS Code 使用指南](https://docs.hisilicon.com/repos/fbb_ide/zh-CN/master/tool/HiSpark%20Studio%20for%20VS%20Code%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/index.html)

### 多媒体<a name="ZH-CN_TOPIC_0000002535948857"></a>

- [HiDiTingV100 多媒体软件 开发指南](../../api-reference/HiDiTingV100%20多媒体软件%20开发指南/HiDiTingV100%20多媒体软件%20开发指南.md)

### 射频测试<a name="ZH-CN_TOPIC_0000002504268910"></a>

- [HiDiTingV100 蓝牙／星闪射频 测试指南](../../api-reference/HiDiTingV100%20蓝牙／星闪射频%20测试指南/HiDiTingV100%20蓝牙／星闪射频%20测试指南.md)

### 图形驱动<a name="ZH-CN_TOPIC_0000002504109064"></a>

- [HiDiTingV100 图形工具 使用指南](../../tools/HiDiTingV100%20图形工具%20使用指南/HiDiTingV100%20图形工具%20使用指南.md)

- [HiDiTingV100 图形视频驱动 调试指南](../../api-reference/HiDiTingV100%20图形视频驱动%20调试指南/HiDiTingV100%20图形视频驱动%20调试指南.md)

- [HiDiTingV100 图形视频驱动 开发指南](../../api-reference/HiDiTingV100%20图形视频驱动%20开发指南/HiDiTingV100%20图形视频驱动%20开发指南.md)

### 音频驱动<a name="ZH-CN_TOPIC_0000002535908889"></a>

- [HiDiTingV100 音频驱动 调试指南](../../api-reference/HiDiTingV100%20音频驱动%20调试指南/HiDiTingV100%20音频驱动%20调试指南.md)

- [HiDiTingV100 音频驱动 开发指南](../../api-reference/HiDiTingV100%20音频驱动%20开发指南/HiDiTingV100%20音频驱动%20开发指南.md)

- [HiDiTingV100 音频算法插件 开发指南](../../api-reference/HiDiTingV100%20音频算法插件%20开发指南/HiDiTingV100%20音频算法插件%20开发指南.md)

### 消息中心<a name="ZH-CN_TOPIC_0000002535948859"></a>

- [HiDiTingV100 消息中心 开发指南](../../api-reference/HiDiTingV100%20消息中心%20开发指南/HiDiTingV100%20消息中心%20开发指南.md)

### 三方应用<a name="ZH-CN_TOPIC_0000002504268912"></a>

- [HiDiTingV100 UX API 使用指南](../../api-reference/HiDiTingV100%20UX%20API%20使用指南/HiDiTingV100%20UX%20API%20使用指南.md)

- [HiDiTingV100 三方应用 开发指南](../../api-reference/HiDiTingV100%20三方应用%20开发指南/HiDiTingV100%20三方应用%20开发指南.md)

### OpenHarmony XTS认证<a name="ZH-CN_TOPIC_0000002504109066"></a>

- [HiDiTingV100 OpenHarmony XTS认证 开发指南](../../api-reference/HiDiTingV100%20OpenHarmony%20XTS认证%20开发指南/HiDiTingV100%20OpenHarmony%20XTS认证%20开发指南.md)

### 定位<a name="ZH-CN_TOPIC_0000002549288857"></a>

- [HiDiTingV100 GNSS集成 开发指南](../../api-reference/HiDiTingV100%20GNSS集成%20开发指南/HiDiTingV100%20GNSS集成%20开发指南.md)

- [HiDiTingV100 XGNSS 开发指南](../../api-reference/HiDiTingV100%20XGNSS%20开发指南/HiDiTingV100%20XGNSS%20开发指南.md)

- [HiDiTingV100 GNSS接口协议 开发指南](../../api-reference/HiDiTingV100%20GNSS接口协议%20开发指南/HiDiTingV100%20GNSS接口协议%20开发指南.md)

## 工具维测<a name="ZH-CN_TOPIC_0000002535908891"></a>

HiDiTingV100R001解决方案提供了完整的开发调试、烧录加载、日志解析、维测定位、UI开发、资源压缩转换等工具链，同时提供了日志和维测系统，用于支持客户进行日志功能扩展和新增。

工具类相关文档主要涉及如下：

- [HiDiTingV100 BurnTool工具 使用指南](../../tools/HiDiTingV100%20BurnTool工具%20使用指南/HiDiTingV100%20BurnTool工具%20使用指南.md)

- [HiDiTingV100 DebugKits工具 使用指南](../../tools/HiDiTingV100%20DebugKits工具%20使用指南/HiDiTingV100%20DebugKits工具%20使用指南.md)

- [HiDiTingV100 HiSpark Studio for VS Code 使用指南](https://docs.hisilicon.com/repos/fbb_ide/zh-CN/master/tool/HiSpark%20Studio%20for%20VS%20Code%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/index.html)

- [HiDiTingV100 图形工具 使用指南](../../tools/HiDiTingV100%20图形工具%20使用指南/HiDiTingV100%20图形工具%20使用指南.md)

- [HiDiTingV100 产测工具 使用指南](../../tools/HiDiTingV100%20产测工具%20使用指南/HiDiTingV100%20产测工具%20使用指南.md)

日志和维测系统相关文档主要涉及如下：

- [HiDiTingV100 DFX软件 开发指南](../../tools/HiDiTingV100%20DFX软件%20开发指南/HiDiTingV100%20DFX软件%20开发指南.md)
