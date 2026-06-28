# 前言<a name="ZH-CN_TOPIC_0000002466382844"></a>

**概述<a name="zh-cn_topic_0000002422480844_section4537382116410"></a>**

本文档主要描述了DebugKits工具的使用方法。用于帮助用户快速熟悉DebugKits工具，并可以作为查询手册，明确具体特性的正确使用方法。

**读者对象<a name="zh-cn_topic_0000002422480844_section4378592816410"></a>**

本文档主要适用于以下工程师：

-   技术支持工程师
-   软件工程师
-   硬件工程师

**符号约定<a name="zh-cn_topic_0000002422480844_section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="zh-cn_topic_0000002422480844_table2622507016410"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422480844_row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002422480844_p6450074116410"><a name="zh-cn_topic_0000002422480844_p6450074116410"></a><a name="zh-cn_topic_0000002422480844_p6450074116410"></a><strong id="zh-cn_topic_0000002422480844_b2136615816410"><a name="zh-cn_topic_0000002422480844_b2136615816410"></a><a name="zh-cn_topic_0000002422480844_b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002422480844_p5435366816410"><a name="zh-cn_topic_0000002422480844_p5435366816410"></a><a name="zh-cn_topic_0000002422480844_p5435366816410"></a><strong id="zh-cn_topic_0000002422480844_b5941558116410"><a name="zh-cn_topic_0000002422480844_b5941558116410"></a><a name="zh-cn_topic_0000002422480844_b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422480844_row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002422480844_p3734547016410"><a name="zh-cn_topic_0000002422480844_p3734547016410"></a><a name="zh-cn_topic_0000002422480844_p3734547016410"></a><a name="zh-cn_topic_0000002422480844_image2670064316410"></a><a name="zh-cn_topic_0000002422480844_image2670064316410"></a><span><img class="" id="zh-cn_topic_0000002422480844_image2670064316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002469153444.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002422480844_p1757432116410"><a name="zh-cn_topic_0000002422480844_p1757432116410"></a><a name="zh-cn_topic_0000002422480844_p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422480844_row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002422480844_p1432579516410"><a name="zh-cn_topic_0000002422480844_p1432579516410"></a><a name="zh-cn_topic_0000002422480844_p1432579516410"></a><a name="zh-cn_topic_0000002422480844_image4895582316410"></a><a name="zh-cn_topic_0000002422480844_image4895582316410"></a><span><img class="" id="zh-cn_topic_0000002422480844_image4895582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002502153403.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002422480844_p959197916410"><a name="zh-cn_topic_0000002422480844_p959197916410"></a><a name="zh-cn_topic_0000002422480844_p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422480844_row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002422480844_p1232579516410"><a name="zh-cn_topic_0000002422480844_p1232579516410"></a><a name="zh-cn_topic_0000002422480844_p1232579516410"></a><a name="zh-cn_topic_0000002422480844_image1235582316410"></a><a name="zh-cn_topic_0000002422480844_image1235582316410"></a><span><img class="" id="zh-cn_topic_0000002422480844_image1235582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002502033429.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002422480844_p123197916410"><a name="zh-cn_topic_0000002422480844_p123197916410"></a><a name="zh-cn_topic_0000002422480844_p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422480844_row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002422480844_p2204984716410"><a name="zh-cn_topic_0000002422480844_p2204984716410"></a><a name="zh-cn_topic_0000002422480844_p2204984716410"></a><a name="zh-cn_topic_0000002422480844_image4504446716410"></a><a name="zh-cn_topic_0000002422480844_image4504446716410"></a><span><img class="" id="zh-cn_topic_0000002422480844_image4504446716410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002468993436.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002422480844_p4388861916410"><a name="zh-cn_topic_0000002422480844_p4388861916410"></a><a name="zh-cn_topic_0000002422480844_p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="zh-cn_topic_0000002422480844_p1238861916410"><a name="zh-cn_topic_0000002422480844_p1238861916410"></a><a name="zh-cn_topic_0000002422480844_p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422480844_row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002422480844_p5555360116410"><a name="zh-cn_topic_0000002422480844_p5555360116410"></a><a name="zh-cn_topic_0000002422480844_p5555360116410"></a><a name="zh-cn_topic_0000002422480844_image799324016410"></a><a name="zh-cn_topic_0000002422480844_image799324016410"></a><span><img class="" id="zh-cn_topic_0000002422480844_image799324016410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002469153450.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002422480844_p4612588116410"><a name="zh-cn_topic_0000002422480844_p4612588116410"></a><a name="zh-cn_topic_0000002422480844_p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="zh-cn_topic_0000002422480844_p1232588116410"><a name="zh-cn_topic_0000002422480844_p1232588116410"></a><a name="zh-cn_topic_0000002422480844_p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# 概述<a name="ZH-CN_TOPIC_0000002466382860"></a>

DebugKits（以下简称DBK）软件包是支持研发调试、现场测试等多种场景的Windows应用程序。

通用功能主要包括：

-   命令行插件

    支持以命令行的方式向单板发送命令，实现对单板的查询、设置等操作。

-   消息插件

    显示单板上报的维测信息。

-   System插件

    用于支持常用维测功能。

# 安装<a name="ZH-CN_TOPIC_0000002466542444"></a>

>![](public_sys-resources/icon-note.gif) **说明：** 
>-   DBK支持的操作系统版本为Win7/Win8/Win10/Win11。
>-   中文版本DBK在英文操作系统中显示乱码的解决方法：
>    1.  打开控制面板，进入时钟、语言和区域（或者是时钟和区域）的“区域和语言”项（或者是“区域”项）。
>    2.  在“区域和语言”项（或者是“区域”项）的“管理”页签更改系统区域设置，将当前系统区域设置改为“中文\(简体，中国\)”。
>    3.  设置完成后单击确定按钮，然后重启PC即可。

-   **[产品安装](#ZH-CN_TOPIC_0000002608393119)**  

## 产品安装<a name="ZH-CN_TOPIC_0000002608393119"></a>

DBK安装程序的安装步骤如下：

1.  双击“DebugKits.exe”打开DBK安装程序。
2.  弹出DBK安装路径选择界面，如[图1](#zh-cn_topic_0000002607953877_zh-cn_topic_0000002602569251_zh-cn_topic_0000001206282628_fig18864201320498)所示，选择好安装路径后单击“Next”按钮。若安装路径选择为C盘“Program Files”需要在运行时使用管理员权限。

    **图 1**  DBK安装路径选择界面<a name="zh-cn_topic_0000002607953877_zh-cn_topic_0000002602569251_zh-cn_topic_0000001206282628_fig18864201320498"></a>  
    ![](figures/DBK安装路径选择界面.png "DBK安装路径选择界面")

3.  弹出DBK选择产品界面，如[图2](#zh-cn_topic_0000002607953877_zh-cn_topic_0000002602569251_zh-cn_topic_0000001206282628_fig1227110548425)所示，选择对应的产品，单击“Next”按钮。

    **图 2**  DBK选择产品界面<a name="zh-cn_topic_0000002607953877_zh-cn_topic_0000002602569251_zh-cn_topic_0000001206282628_fig1227110548425"></a>  
    ![](figures/DBK选择产品界面.png "DBK选择产品界面")

4.  弹出DBK额外任务选择界面，如[图3](#zh-cn_topic_0000002607953877_zh-cn_topic_0000002602569251_zh-cn_topic_0000001206282628_fig686471334913)所示，复选框勾选为创建桌面快捷方式，为方便后续快速打开工具，建议保持复选框勾选，单击“Next”按钮。

    **图 3**  DBK额外任务选择界面<a name="zh-cn_topic_0000002607953877_zh-cn_topic_0000002602569251_zh-cn_topic_0000001206282628_fig686471334913"></a>  
    ![](figures/DBK额外任务选择界面.png "DBK额外任务选择界面")

5.  弹出DBK正在安装界面，请等待约10秒钟，直至安装完成。如[图4](#zh-cn_topic_0000002607953877_zh-cn_topic_0000002602569251_zh-cn_topic_0000001206282628_fig181671847487)所示。

    **图 4**  DBK正在安装界面<a name="zh-cn_topic_0000002607953877_zh-cn_topic_0000002602569251_zh-cn_topic_0000001206282628_fig181671847487"></a>  
    ![](figures/DBK正在安装界面.png "DBK正在安装界面")

6.  弹出DBK安装完成界面，如[图5](#zh-cn_topic_0000002607953877_zh-cn_topic_0000002602569251_zh-cn_topic_0000001206282628_fig1786441312496)所示，单击“Finish”按钮退出安装。复选框选中后则默认打开DBK程序。

    **图 5**  DBK安装完成界面<a name="zh-cn_topic_0000002607953877_zh-cn_topic_0000002602569251_zh-cn_topic_0000001206282628_fig1786441312496"></a>  
    ![](figures/DBK安装完成界面.png "DBK安装完成界面")

# 启动<a name="ZH-CN_TOPIC_0000002466542408"></a>

-   **[主界面](#ZH-CN_TOPIC_0000002499581813)**  

-   **[切换芯片](#ZH-CN_TOPIC_0000002466382812)**  

-   **[更新数据库](#ZH-CN_TOPIC_0000002499501801)**  

-   **[连接设置](#ZH-CN_TOPIC_0000002466542484)**  

## 主界面<a name="ZH-CN_TOPIC_0000002499581813"></a>

DBK主界面如[图1](#zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_fig493827115713)所示，说明内容如[表1](#zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_table18366459726)所示。

**图 1**  主界面示例<a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_fig493827115713"></a>  
![](figures/主界面示例.png "主界面示例")

**表 1**  主界面说明

<a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_table18366459726"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_row59445735"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p50375200"><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p50375200"></a><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p50375200"></a>区域</p>
</th>
<th class="cellrowborder" valign="top" width="84.85000000000001%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p53859361"><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p53859361"></a><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p53859361"></a>功能说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_row532115"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p43101341"><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p43101341"></a><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p43101341"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="84.85000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p1547752"><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p1547752"></a><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p1547752"></a>标题及连接状态。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_row13929773"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p54569852"><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p54569852"></a><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p54569852"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="84.85000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p58081887"><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p58081887"></a><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p58081887"></a>菜单栏。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_row52974940"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p63111737"><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p63111737"></a><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p63111737"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="84.85000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p11777054"><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p11777054"></a><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_zh-cn_topic_0279615131_p11777054"></a>插件选择及连接控制。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_row19945321192410"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_p13945192120241"><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_p13945192120241"></a><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_p13945192120241"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="84.85000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_p1794572115241"><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_p1794572115241"></a><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_p1794572115241"></a>用户区，插件界面显示和实际操作区域。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_row201402917249"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_p4140119112416"><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_p4140119112416"></a><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_p4140119112416"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="84.85000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_p13140129102416"><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_p13140129102416"></a><a name="zh-cn_topic_0000002422655410_zh-cn_topic_0000001665168754_zh-cn_topic_0000001208001913_p13140129102416"></a>状态栏。</p>
</td>
</tr>
</tbody>
</table>

## 切换芯片<a name="ZH-CN_TOPIC_0000002466382812"></a>

在连接单板之前，需要根据产品切换成对应芯片。操作步骤如下：

1.  在菜单栏中选择“Options”→“Change Chip”。

    **图 1**  切换芯片示意图<a name="zh-cn_topic_0000002456134221_zh-cn_topic_0000001665009026_zh-cn_topic_0000001484714061_fig14677171432"></a>  
    ![](figures/切换芯片示意图.png "切换芯片示意图")

2.  选择需要的芯片名称并单击“确定”按钮。

## 更新数据库<a name="ZH-CN_TOPIC_0000002499501801"></a>

数据库（database）是板端编译过程中产生的一系列的配置文件，在板端代码的output目录中生成，包括多个XML文件和TXT文件。

数据库的作用主要体现在两个方面：

1.  将板端到工具方向的报文结构体化或者文本化。
2.  将工具端结构体化的数据报文化。

因此在连接单板之前，需要将工具数据库更新为对应产品的数据库，以便板端运行的程序与工具端数据库一致。操作步骤如下：

1.  在菜单栏中选择“Options”→“Update HDB”，如[图1](#zh-cn_topic_0000002456054329_zh-cn_topic_0000001713128585_fig15417321132715)所示。

    **图 1**  更新数据库示意图<a name="zh-cn_topic_0000002456054329_zh-cn_topic_0000001713128585_fig15417321132715"></a>  
    ![](figures/更新数据库示意图.png "更新数据库示意图")

2.  选择数据库进行更新操作。
    1.  单击“Choose File”或者“Choose Folder”按钮，选择fwpkg文件或数据库文件夹。
    2.  单击“OK”按钮，更新数据库。

>![](public_sys-resources/icon-note.gif) **说明：** 
>-   数据库可以选择产品对应的数据库文件夹。
>-   Choose Folder可以选择名为database或database\_xxx的文件夹，一般的database文件夹会在编译后于output目录中生成，可以在output中搜索“database”关键词查找。
>-   数据库覆盖默认勾选。如果仅需要当次有效，可以通过取消勾选“覆盖数据库”选择框，不进行覆盖操作。

## 连接设置<a name="ZH-CN_TOPIC_0000002466542484"></a>

-   **[串口连接](#ZH-CN_TOPIC_0000002466382840)**  

### 串口连接<a name="ZH-CN_TOPIC_0000002466382840"></a>

在对单板进行操作前，DBK需要先通过串口与单板建立连接。

连接设置方法如下：

1.  打开连接设置对话框。

    在工具栏中单击“![](figures/zh-cn_image_0000001665009158.png)”或在菜单栏中选择“Connection”→“connect”或“Settings”。

2.  完成工具配置，并通过串口连接单板。

    **图 1**  连接设置示意图（串口）<a name="zh-cn_topic_0000002422655414_zh-cn_topic_0000001665168762_zh-cn_topic_0000001250983483_zh-cn_topic_0279615142_fig58021926"></a>  
    ![](figures/连接设置示意图（串口）.png "连接设置示意图（串口）")

    1.  选择“SerialPort”。
    2.  使用默认波特率。
    3.  选择串口。
    4.  （可选）根据产品需要输入密码，对于部分无密码机制的产品则直接单击确定。对于有密码机制的产品，提供的软件版本默认未预置密码，因此无法建立连接。若需要设置密码，请参见产品NV/KV介绍文档中关于预置密码的信息。
    5.  单击“OK”按钮，连接单板。

>![](public_sys-resources/icon-note.gif) **说明：** 
>-   连接失败常见告警如下：
>    -   提示“Connection failed. 4 attempts left. Check your password or cable connection.”，一般为密码未设置、密码输入错误或连线松动等原因。设置密码请查看产品NV/KV介绍文档中关于预置密码的信息。
>    -   提示“Your PCB has been locked. 4 s remained”，一般为之前尝试过5次以上输入错误密码导致的单板锁定，请复位单板或等待锁定结束。
>    -   若无提示，同时也未显示连接成功，则需要排查：串口连线是否正确；支持睡眠机制的单板是否已进入睡眠状态。
>        睡眠唤醒方法请查看产品AT介绍文档 。
>-   连接用户名固定为“admin”，无法增删改，故此处不体现。
>-   若串口线断开重连后，则需要手动重新连接单板。
>-   左下角复选框中“show this dialog at startup”的含义是启动时自动弹出，“show this dialog if click connect button”的含义是点击connect按钮后弹出。

# 命令行插件<a name="ZH-CN_TOPIC_0000002466542424"></a>

-   **[简介](#ZH-CN_TOPIC_0000002466542456)**  

-   **[主要功能及使用说明](#ZH-CN_TOPIC_0000002499581837)**  

## 简介<a name="ZH-CN_TOPIC_0000002466542456"></a>

DBK命令行插件（Command Line Viewer）允许用户以命令行的方式向单板发送命令，实现对单板的查询、设置等操作。

打开命令行插件的方法：单击菜单栏或工具栏上的命令行图标即可。如[图1](#zh-cn_topic_0000002456054333_zh-cn_topic_0000001665168766_fig1978810112710)、[图2](#zh-cn_topic_0000002456054333_zh-cn_topic_0000001665168766_fig728093122617)所示。

**图 1**  打开命令行插件示例（菜单栏）<a name="zh-cn_topic_0000002456054333_zh-cn_topic_0000001665168766_fig1978810112710"></a>  
![](figures/打开命令行插件示例（菜单栏）.png "打开命令行插件示例（菜单栏）")

**图 2**  打开命令行插件示例（工具栏）<a name="zh-cn_topic_0000002456054333_zh-cn_topic_0000001665168766_fig728093122617"></a>  
![](figures/打开命令行插件示例（工具栏）.png "打开命令行插件示例（工具栏）")

## 主要功能及使用说明<a name="ZH-CN_TOPIC_0000002499581837"></a>

**命令行查询<a name="zh-cn_topic_0000002422495526_zh-cn_topic_0000001665009046_zh-cn_topic_0000001162163408_zh-cn_topic_0279615184_section7330142"></a>**

命令行输入与大部分串口工具类似，输入对应指令后回车即可，此处使用的具体命令根据产品会有差异，输入命令的首字母会出现联想列出支持的命令或者通过输入"?"或"help"得到支持的命令列表，请根据实际情况填写。

**清屏<a name="zh-cn_topic_0000002422495526_zh-cn_topic_0000001665009046_zh-cn_topic_0000001162163408_zh-cn_topic_0279615184_section65971280"></a>**

在命令行界面按下“Shift+Delete”组合键即可。

**设置超时时间<a name="zh-cn_topic_0000002422495526_zh-cn_topic_0000001665009046_section10177202219303"></a>**

如果某些命令需要比较长时间才能回复，可以设置超时时间。在命令行空白处单击右键-“Configure...”，在弹出的页面中选择“Popular”，找到“Timeout Value”，修改超时时间，单位：ms。

-   **[默认保存路径](#ZH-CN_TOPIC_0000002608553037)**  

### 默认保存路径<a name="ZH-CN_TOPIC_0000002608553037"></a>

在命令行界面右键菜单-“Configure”-“Save”可以查看默认保存路径，如[图1](#zh-cn_topic_0000002577553984_zh-cn_topic_0000002555750224_fig6503551585)所示。

**图 1**  命令行默认保存路径<a name="zh-cn_topic_0000002577553984_zh-cn_topic_0000002555750224_fig6503551585"></a>  
![](figures/命令行默认保存路径.png "命令行默认保存路径")

# 消息插件<a name="ZH-CN_TOPIC_0000002499581777"></a>

-   **[简介](#ZH-CN_TOPIC_0000002466542464)**  

-   **[主要功能及使用说明](#ZH-CN_TOPIC_0000002499501789)**  

## 简介<a name="ZH-CN_TOPIC_0000002466542464"></a>

DBK消息插件（Message）显示单板上报的维测信息，方便对单板进行调试及问题定位。

打开消息插件的方法：单击菜单栏或工具栏上的消息图标即可，如[图1](#zh-cn_topic_0000002456054337_zh-cn_topic_0000001665009050_fig1982323412711)、[图2](#zh-cn_topic_0000002456054337_zh-cn_topic_0000001665009050_fig20449192412278)所示。

**图 1**  打开消息插件示例（菜单栏）<a name="zh-cn_topic_0000002456054337_zh-cn_topic_0000001665009050_fig1982323412711"></a>  
![](figures/打开消息插件示例（菜单栏）.png "打开消息插件示例（菜单栏）")

**图 2**  打开消息插件示例（工具栏）<a name="zh-cn_topic_0000002456054337_zh-cn_topic_0000001665009050_fig20449192412278"></a>  
![](figures/打开消息插件示例（工具栏）.png "打开消息插件示例（工具栏）")

## 主要功能及使用说明<a name="ZH-CN_TOPIC_0000002499501789"></a>

-   **[界面说明](#ZH-CN_TOPIC_0000002466382820)**  

-   **[菜单栏说明](#ZH-CN_TOPIC_0000002466382848)**  

-   **[工具栏说明](#ZH-CN_TOPIC_0000002466382824)**  

-   **[操作说明](#ZH-CN_TOPIC_0000002466542468)**  

### 界面说明<a name="ZH-CN_TOPIC_0000002466382820"></a>

**图 1**  消息插件界面示例<a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_fig1384014284584"></a>  
![](figures/消息插件界面示例.png "消息插件界面示例")

**表 1**  消息插件界面说明

<a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_table24185505"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_row66967736"><th class="cellrowborder" valign="top" width="10.100000000000001%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p55677530"><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p55677530"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p55677530"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="89.9%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p13586068"><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p13586068"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p13586068"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_row26729743"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p17625587"><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p17625587"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p17625587"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p18386406"><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p18386406"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p18386406"></a>控制面板。其中：</p>
<a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_ul31259934"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_ul31259934"></a><ul id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_ul31259934"><li>被选中的信息将会上报。</li><li>未选中的信息不会上报。<p id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p38587424"><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p38587424"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p38587424"></a>控制面板中的模块信息各产品不同，会根据当前选择的产品自动切换。更改被选中的信息后需要点击Message工具栏图标<a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_image2265123215251"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_image2265123215251"></a><span><img id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_image2265123215251" src="figures/zh-cn_image_0000001665168914.png"></span>图标下发配置。</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_row11742498"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p11618275"><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p11618275"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p11618275"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p1556184"><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p1556184"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p1556184"></a>解析面板。其中：</p>
<a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_ul14005657"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_ul14005657"></a><ul id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_ul14005657"><li>左侧：该信息的十六进制字节流。</li><li>右侧：按结构体解析的各域的值。</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_row9577344"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p37567362"><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p37567362"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p37567362"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p23057497"><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p23057497"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p23057497"></a>维测信息表格。其中：</p>
<a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_ul1436062312522"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_ul1436062312522"></a><ul id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_ul1436062312522"><li>NO：日志编号。</li><li>Category：日志类型。</li></ul>
<a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_ul6190888"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_ul6190888"></a><ul id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_ul6190888"><li>Time：DBK收到该信息时记录下的时间。</li><li>TimeStamp：单板上报信息附带的时间戳，描述的是单板打印这条信息的时间。</li><li>Prim：该信息的描述。</li><li>Source：日志来源模块。</li><li>Destination：日志目的模块。（默认隐藏，可通过<a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_image113681141171113"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_image113681141171113"></a><span><img id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_image113681141171113" src="figures/zh-cn_image_0000002607837349.png"></span>显示）</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_row17557458"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p12868028"><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p12868028"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p12868028"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p35677375"><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p35677375"></a><a name="zh-cn_topic_0000002422655422_zh-cn_topic_0000001713048609_zh-cn_topic_0000001162003414_zh-cn_topic_0279615198_p35677375"></a>工具栏，功能请参见“<a href="#ZH-CN_TOPIC_0000002466382824">工具栏说明</a>”章节。</p>
</td>
</tr>
</tbody>
</table>

### 菜单栏说明<a name="ZH-CN_TOPIC_0000002466382848"></a>

维测信息表格界面右击可以出现菜单栏，回放日志时为[图1](#zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_fig57465396)，实时日志时为[图2](#zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_fig422435718357)所示。菜单说明如[表1](#zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_table24185505)所示。

**图 1**  消息插件界面右键菜单示例1<a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_fig57465396"></a>  
![](figures/消息插件界面右键菜单示例1.png "消息插件界面右键菜单示例1")

**图 2**  消息插件界面右键菜单示例2<a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_fig422435718357"></a>  
![](figures/消息插件界面右键菜单示例2.png "消息插件界面右键菜单示例2")

**表 1**  消息插件菜单说明

<a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_table24185505"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_row66967736"><th class="cellrowborder" valign="top" width="10.100000000000001%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p55677530"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p55677530"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p55677530"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="89.9%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p13586068"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p13586068"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p13586068"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_row26729743"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p17625587"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p17625587"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p17625587"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p18386406"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p18386406"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p18386406"></a>快速过滤。功能使用请参见“<a href="#ZH-CN_TOPIC_0000002499581825">快速过滤</a>”章节。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_row11742498"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p11618275"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p11618275"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p11618275"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p1764732494417"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p1764732494417"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p1764732494417"></a>清除快速过滤。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_row9577344"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p37567362"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p37567362"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p37567362"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p88951639453"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p88951639453"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p88951639453"></a>清除所有过滤。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_row17557458"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p12868028"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p12868028"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p12868028"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p35677375"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p35677375"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p35677375"></a>过滤模板。功能使用请参见“<a href="#ZH-CN_TOPIC_0000002466382832">过滤模板</a>”章节。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_row52660921"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p37676179"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p37676179"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p37676179"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p31871641"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p31871641"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p31871641"></a>颜色设置。功能使用请参见“<a href="#ZH-CN_TOPIC_0000002499501773">颜色设置</a>”章节。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_row31466165"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p65731404"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p65731404"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p65731404"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p22643528"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p22643528"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_p22643528"></a>行跳转。快捷键为Ctrl+G。功能使用请参见“<a href="#ZH-CN_TOPIC_0000002466542448">行跳转</a>”章节。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_row18852142017542"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p9852120145420"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p9852120145420"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p9852120145420"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p48521920175411"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p48521920175411"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p48521920175411"></a>查找。快捷键为Ctrl+F。功能使用请参见“<a href="#ZH-CN_TOPIC_0000002499501733">查找日志</a>”章节。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_row14236112495710"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p323622414578"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p323622414578"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p323622414578"></a>8</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p3236152485712"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p3236152485712"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p3236152485712"></a>删除列。只能删除新增列。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_row1599044212574"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p1999034275713"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p1999034275713"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p1999034275713"></a>9</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p129901142165710"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p129901142165710"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p129901142165710"></a>导出选中行。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_row1430414467576"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p830494610570"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p830494610570"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p830494610570"></a>10</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p73042461571"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p73042461571"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p73042461571"></a>导出所有日志。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_row218444915717"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p418418492576"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p418418492576"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p418418492576"></a>11</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p15184114975711"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p15184114975711"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p15184114975711"></a>选中所有日志。快捷键为Ctrl+A。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_row8503191017579"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p95031010205714"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p95031010205714"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p95031010205714"></a>12</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p173753015817"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p173753015817"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p173753015817"></a>复制日志。快捷键为Ctrl+C。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_row204914361572"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p64963615577"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p64963615577"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p64963615577"></a>13</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p124933615579"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p124933615579"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p124933615579"></a>关闭回放文件。功能使用请参见“<a href="#ZH-CN_TOPIC_0000002466382856">关闭回放文件</a>”章节。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_row462523212576"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p156251632165713"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p156251632165713"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p156251632165713"></a>14</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p13625143218570"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p13625143218570"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p13625143218570"></a>视图。其中：</p>
<a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_ul14005657"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_ul14005657"></a><ul id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_zh-cn_topic_0279615198_ul14005657"><li>Tool Bar：打开/关闭工具栏。</li><li>Control Panel：打开/关闭控制台。</li><li>Parser Pane：打开/关闭解析窗口。</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_row6263128125716"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p13263172895715"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p13263172895715"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p13263172895715"></a>15</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p16263112812579"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p16263112812579"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p16263112812579"></a>高级。其中：</p>
<a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_ul1829234815344"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_ul1829234815344"></a><ul id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_ul1829234815344"><li>No Format Alignment：修改No.列的格式。</li><li>Show Enum Value：显示枚举值。</li><li>Show Prim ID：prim列显示具体数值。</li><li>Combine Replay Files：合并两个hlf文件。</li><li>Tshark Path：设置Tshark路径。</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_row01121817135713"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p16112101765710"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p16112101765710"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p16112101765710"></a>16</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p18112141785716"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p18112141785716"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p18112141785716"></a>打开配置界面，包括快捷键，颜色，自动保存策略等配置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_row178311146572"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p148311348574"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p148311348574"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p148311348574"></a>17</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p883174105711"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p883174105711"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p883174105711"></a>冻结窗口。暂停显示后续接收的日志，解除后重新更新日志。快捷键为Shift＋RightButton。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_row12418513185811"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p341816132586"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p341816132586"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p341816132586"></a>18</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p1041815136589"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p1041815136589"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p1041815136589"></a>自动滚动窗口。随着日志增加，自动滚动到最后。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_row194081118115810"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p10408161812585"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p10408161812585"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p10408161812585"></a>19</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p24081418115816"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p24081418115816"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p24081418115816"></a>清空当前表格显示的日志。快捷键为Shift＋Del。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_row97331285587"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p673314286584"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p673314286584"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p673314286584"></a>20</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p0733428105816"><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p0733428105816"></a><a name="zh-cn_topic_0000002456134241_zh-cn_topic_0000001665168774_zh-cn_topic_0000001434555101_p0733428105816"></a>一键导出。目前只有一键导出json格式的功能，使用此功能时需配置Msg.xml文件。</p>
</td>
</tr>
</tbody>
</table>

### 工具栏说明<a name="ZH-CN_TOPIC_0000002466382824"></a>

工具栏如[图1](#zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_fig9956164916233)所示，工具栏说明如[表1](#zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_table24185505)所示。

**图 1**  工具栏示意图<a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_fig9956164916233"></a>  
![](figures/工具栏示意图.png "工具栏示意图")

**表 1**  消息插件工具栏说明

<a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_table24185505"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_row66967736"><th class="cellrowborder" valign="top" width="13%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p55677530"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p55677530"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p55677530"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="87%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p13586068"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p13586068"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p13586068"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_row26729743"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p17625587"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p17625587"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p17625587"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p18386406"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p18386406"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p18386406"></a>导出全部日志。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_row11742498"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p11618275"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p11618275"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p11618275"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p1764732494417"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p1764732494417"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p1764732494417"></a>打开/关闭Control Panel。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_row9577344"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p37567362"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p37567362"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p37567362"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p951618215274"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p951618215274"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p951618215274"></a>打开/关闭Parser Pane。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_row52660921"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p37676179"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p37676179"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p37676179"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p31871641"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p31871641"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p31871641"></a>清除筛选模板。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_row31466165"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p65731404"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p65731404"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_zh-cn_topic_0279615198_p65731404"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p13738104113285"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p13738104113285"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p13738104113285"></a>查找。快捷键为Ctrl+F。功能使用请参见“<a href="#ZH-CN_TOPIC_0000002499501733">查找日志</a>”章节。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_row18852142017542"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p9852120145420"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p9852120145420"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p9852120145420"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p48521920175411"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p48521920175411"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p48521920175411"></a>设置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_row14236112495710"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p323622414578"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p323622414578"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p323622414578"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p3236152485712"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p3236152485712"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p3236152485712"></a>显示/隐藏列。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_row1599044212574"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p1999034275713"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p1999034275713"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p1999034275713"></a>8</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p129901142165710"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p129901142165710"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p129901142165710"></a>打开书签列表。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_row1430414467576"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p830494610570"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p830494610570"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p830494610570"></a>9</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p73042461571"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p73042461571"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p73042461571"></a>添加新书签。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_row218444915717"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p418418492576"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p418418492576"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p418418492576"></a>10</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p15184114975711"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p15184114975711"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p15184114975711"></a>跳转到上一个书签位置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_row8503191017579"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p95031010205714"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p95031010205714"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p95031010205714"></a>11</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p0441151823110"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p0441151823110"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p0441151823110"></a>跳转到下一个书签位置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_row204914361572"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p64963615577"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p64963615577"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p64963615577"></a>12</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p124933615579"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p124933615579"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p124933615579"></a>删除书签。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_row462523212576"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p156251632165713"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p156251632165713"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p156251632165713"></a>13</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p15910192053214"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p15910192053214"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p15910192053214"></a>控制面板状态改变后需要按下该按钮才能生效。功能使用请参见“<a href="#ZH-CN_TOPIC_0000002499501725">筛选上报的信息</a>”章节。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_row6263128125716"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p13263172895715"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p13263172895715"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p13263172895715"></a>14</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p112841328123320"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p112841328123320"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p112841328123320"></a>冻结窗口。暂停显示后续接收的日志，解除后重新更新日志。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_row01121817135713"><td class="cellrowborder" valign="top" width="13%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p16112101765710"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p16112101765710"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p16112101765710"></a>15</p>
</td>
<td class="cellrowborder" valign="top" width="87%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p142671215163413"><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p142671215163413"></a><a name="zh-cn_topic_0000002456054341_zh-cn_topic_0000001665009054_zh-cn_topic_0000001434674653_p142671215163413"></a>模糊搜索栏。对勾按钮代表开始模糊搜索。功能使用请参见“<a href="#ZH-CN_TOPIC_0000002499501761">模糊搜索</a>”章节。</p>
</td>
</tr>
</tbody>
</table>

### 操作说明<a name="ZH-CN_TOPIC_0000002466542468"></a>

-   **[筛选上报的信息](#ZH-CN_TOPIC_0000002499501725)**  

-   **[快速过滤](#ZH-CN_TOPIC_0000002499581825)**  

-   **[模糊搜索](#ZH-CN_TOPIC_0000002499501761)**  

-   **[颜色设置](#ZH-CN_TOPIC_0000002499501773)**  

-   **[表格信息导出回放](#ZH-CN_TOPIC_0000002466542416)**  

-   **[新增表格列](#ZH-CN_TOPIC_0000002499581821)**  

-   **[自动保存](#ZH-CN_TOPIC_0000002466382828)**  

-   **[过滤模板](#ZH-CN_TOPIC_0000002466382832)**  

-   **[关闭回放文件](#ZH-CN_TOPIC_0000002466382856)**  

-   **[行跳转](#ZH-CN_TOPIC_0000002466542448)**  

-   **[查找日志](#ZH-CN_TOPIC_0000002499501733)**  

#### 筛选上报的信息<a name="ZH-CN_TOPIC_0000002499501725"></a>

可控制单板上报信息的模块，常用于过滤某些不重要信息或仅开启重要信息上报。

操作步骤如下：

1.  在控制面板中选中需要上报的项。
2.  单击工具栏中开始按钮“![](figures/zh-cn_image_0000001665168926.png)”，使选中状态生效。

#### 快速过滤<a name="ZH-CN_TOPIC_0000002499581825"></a>

操作步骤如下：

1.  在右侧表格中需要过滤的列处单击右键。
2.  在右键菜单中选择最上方4行（Quick Filter By:…）之一，即可分别过滤出该列中等于、不等于、小于等于、大于等于该行的条目。

    **图 1**  快速过滤示意图<a name="zh-cn_topic_0000002456134245_zh-cn_topic_0000001665168778_zh-cn_topic_0000001207603369_fig2615845205814"></a>  
    ![](figures/快速过滤示意图.png "快速过滤示意图")

#### 模糊搜索<a name="ZH-CN_TOPIC_0000002499501761"></a>

操作步骤如下：

1.  在表格上方输入框中输入需要搜索的关键词，若有多个关键词用‘|’分隔开。
2.  单击“![](figures/zh-cn_image_0000001713128757.png)”或输入回车即可在现有报文的“Prim”字段中过滤出包含关键词的报文。

    **图 1**  模糊过滤示意图<a name="zh-cn_topic_0000002456054345_zh-cn_topic_0000001665009058_zh-cn_topic_0000001207761893_fig19827165435812"></a>  
    ![](figures/模糊过滤示意图.png "模糊过滤示意图")

#### 颜色设置<a name="ZH-CN_TOPIC_0000002499501773"></a>

操作步骤如下：

1.  在表格中需要设置颜色的单元格处单击右键。
2.  在右键菜单中选择“Set Color”，弹出颜色设置界面，如[图1](#zh-cn_topic_0000002422495538_zh-cn_topic_0000001713128605_zh-cn_topic_0000001162163404_fig1277146595)所示。

    **图 1**  颜色设置菜单<a name="zh-cn_topic_0000002422495538_zh-cn_topic_0000001713128605_zh-cn_topic_0000001162163404_fig1277146595"></a>  
    ![](figures/颜色设置菜单.png "颜色设置菜单")

3.  在Color页面（默认）中选择“Set Text Color”或者“Set Background Color”的颜色。
4.  颜色设置效果如[图2](#zh-cn_topic_0000002422495538_zh-cn_topic_0000001713128605_zh-cn_topic_0000001162163404_fig931714194593)所示。

    **图 2**  颜色设置效果示例<a name="zh-cn_topic_0000002422495538_zh-cn_topic_0000001713128605_zh-cn_topic_0000001162163404_fig931714194593"></a>  
    ![](figures/颜色设置效果示例.png "颜色设置效果示例")

#### 表格信息导出回放<a name="ZH-CN_TOPIC_0000002466542416"></a>

**导出<a name="zh-cn_topic_0000002422655438_zh-cn_topic_0000001713048617_zh-cn_topic_0000001162481894_zh-cn_topic_0279615205_section7534858"></a>**

全部导出的操作步骤如下：

1.  在表格处单击右键。
2.  在右键菜单中选择“Export All…”，弹出导出对话框，如[图1](#zh-cn_topic_0000002422655438_zh-cn_topic_0000001713048617_zh-cn_topic_0000001162481894_fig18126125120447)所示，选择需要导出的列（Columns）和导出的文件类型。

    **图 1**  文件导出对话框<a name="zh-cn_topic_0000002422655438_zh-cn_topic_0000001713048617_zh-cn_topic_0000001162481894_fig18126125120447"></a>  
    ![](figures/文件导出对话框.png "文件导出对话框")

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >-   当选择导出文件类型为 \*.txt、\*.csv格式时，可按需选择要导出的列。
    >-   当选择导出文件类型为 \*.hlf格式时，对应导出的是整个数据库，无论选择某列，导出结果均为所有列。
    >-   导出路径和导出文件名称都不能出现中文字符。

3.  单击“![](figures/zh-cn_image_0000001713048769.jpg)”，选择文件路径及文件类型（当前支持的文件类型有\*.hlf、\*.txt、\*.csv），并保存。
4.  如[图1](#zh-cn_topic_0000002422655438_zh-cn_topic_0000001713048617_zh-cn_topic_0000001162481894_fig18126125120447)所示，单击“OK”按钮，导出文件。

部分导出的操作步骤如下：

1.  在表格中用鼠标拖拽选择要导出的单元格。选择时可以不选整行，hlf文件类型导出时会以整行为单位导出，其他文件类型只导出选中单元格。
2.  在选中的范围内单击右键，在右键菜单中选择“Export Items…”，弹出路径选择框。选择保存路径及文件类型，并保存。

**回放<a name="zh-cn_topic_0000002422655438_zh-cn_topic_0000001713048617_zh-cn_topic_0000001162481894_zh-cn_topic_0279615205_section704859"></a>**

>![](public_sys-resources/icon-note.gif) **说明：** 
>DBK工具仅支持\*.hlf、\*.bin、\*.draw、\*.asc格式文件的回放。

-   方法1：将文件直接拖拽到DBK界面中即可回放。
-   方法2：点击左上角菜单“File”→“Open”选择待回放的文件即可回放。

#### 新增表格列<a name="ZH-CN_TOPIC_0000002499581821"></a>

新增表格列操作步骤如下：

在“Parser Pane”右侧结构体解析窗口中，如[图1](#zh-cn_topic_0000002456134249_zh-cn_topic_0000001665009062_zh-cn_topic_0000001162003416_fig2012381818516)所示，选择需要添加到列的成员，右键菜单“Add item to grid”，如[图2](#zh-cn_topic_0000002456134249_zh-cn_topic_0000001665009062_zh-cn_topic_0000001162003416_fig64971928366)所示。

**图 1**  Parser Pane示意图<a name="zh-cn_topic_0000002456134249_zh-cn_topic_0000001665009062_zh-cn_topic_0000001162003416_fig2012381818516"></a>  
![](figures/Parser-Pane示意图.png "Parser-Pane示意图")

**图 2**  添加到列菜单示意图<a name="zh-cn_topic_0000002456134249_zh-cn_topic_0000001665009062_zh-cn_topic_0000001162003416_fig64971928366"></a>  
![](figures/添加到列菜单示意图.png "添加到列菜单示意图")

#### 自动保存<a name="ZH-CN_TOPIC_0000002466382828"></a>

实时消息支持自动保存，操作步骤如下：

1.  单击右键，在菜单中选择“Configure...”，弹出设置界面。
2.  在设置界面中选择“Save”窗口，如[图1](#zh-cn_topic_0000002456054349_zh-cn_topic_0000001713128609_zh-cn_topic_0000001207883375_fig683475615598)所示。

    **图 1**  设置界面<a name="zh-cn_topic_0000002456054349_zh-cn_topic_0000001713128609_zh-cn_topic_0000001207883375_fig683475615598"></a>  
    ![](figures/设置界面.png "设置界面")

3.  （可选）勾选“Custom Path”，并选择路径。
4.  （可选）在“Advanced Options”中根据需要勾选“Setting Replay File Size”设置每个文件的大小和“Total Files Limitation”总文件个数限制。
5.  单击“OK”按钮。

#### 过滤模板<a name="ZH-CN_TOPIC_0000002466382832"></a>

表格信息过滤支持过滤条件保存和使用，具体步骤如下：

1.  根据需求可以进行模糊搜索（具体操作请参见“[模糊搜索](#ZH-CN_TOPIC_0000002499501761)”小节），结果如[图1](#zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_fig187555507354)所示。
2.  <a name="zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_li852478648"></a>根据需求可以新增表格列（具体操作请参见“[新增表格列](#ZH-CN_TOPIC_0000002499581821)”小节），结果如[图1](#zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_fig187555507354)所示。

    **图 1**  模糊过滤和新增表格列结果图<a name="zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_fig187555507354"></a>  
    ![](figures/模糊过滤和新增表格列结果图.png "模糊过滤和新增表格列结果图")

3.  单击右键，选择“Filter Template”→“Save Filter Template”，如[图2](#zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_fig6476172143715)所示。

    **图 2**  保存过滤条件<a name="zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_fig6476172143715"></a>  
    ![](figures/保存过滤条件.png "保存过滤条件")

4.  <a name="zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_li9342173415244"></a>输入需要保存过滤模板的名称，单击“Save”按钮，保存过滤模板，如[图3](#zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_fig1652028203718)所示。可以重复[2](#zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_li852478648)～[4](#zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_li9342173415244)，通过命名区分保存多个过滤模板。

    **图 3**  过滤模板命名<a name="zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_fig1652028203718"></a>  
    ![](figures/过滤模板命名.png "过滤模板命名")

5.  单击右键，选择Filter Template → Open Filter Template，如[图4](#zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_fig14998164713716)所示。

    **图 4**  打开过滤模板<a name="zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_fig14998164713716"></a>  
    ![](figures/打开过滤模板.png "打开过滤模板")

6.  然后通过之前保存到过滤模板名字选择需要的过滤模板，如[图5](#zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_fig19948115519378)所示，单击“Apply”按钮，界面会显示过滤后的内容。

    **图 5**  选择过滤模板<a name="zh-cn_topic_0000002422495542_zh-cn_topic_0000001713048621_zh-cn_topic_0000001227209019_fig19948115519378"></a>  
    ![](figures/选择过滤模板.png "选择过滤模板")

>![](public_sys-resources/icon-note.gif) **说明：** 
>过滤模板仅支持模糊过滤，不支持快速过滤。

#### 关闭回放文件<a name="ZH-CN_TOPIC_0000002466382856"></a>

表格支持关闭回放的文件，具体操作步骤如下：

1.  打开本地存放的日志文件，如[图1](#zh-cn_topic_0000002422655442_zh-cn_topic_0000001665168782_zh-cn_topic_0000001434339653_fig14319411535)所示。

    **图 1**  回放文件<a name="zh-cn_topic_0000002422655442_zh-cn_topic_0000001665168782_zh-cn_topic_0000001434339653_fig14319411535"></a>  
    ![](figures/回放文件.png "回放文件")

2.  右键点击表格，获取到菜单列表，并选择“Close Replay File”。

#### 行跳转<a name="ZH-CN_TOPIC_0000002466542448"></a>

表格支持跳转到某一行，具体操作步骤如下：

1.  右键点击表格，获取到菜单列表并选择“Go To Item...”或者使用快捷键Ctrl+G。
2.  填写跳转至哪一行并点击OK，如图[图1](#zh-cn_topic_0000002456134253_zh-cn_topic_0000001665009066_zh-cn_topic_0000001384481140_fig1544913519527)所示。

    **图 1**  行跳转界面<a name="zh-cn_topic_0000002456134253_zh-cn_topic_0000001665009066_zh-cn_topic_0000001384481140_fig1544913519527"></a>  
    ![](figures/行跳转界面.png "行跳转界面")

#### 查找日志<a name="ZH-CN_TOPIC_0000002499501733"></a>

>![](public_sys-resources/icon-note.gif) **说明：** 
>Find功能搜索大日志量时效率较低，建议使用模糊搜索功能。

表格支持查找日志，界面如[图1](#zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_fig218117141511)，功能介绍参考[表1](#zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_table24185505)。

**图 1**  查找日志界面<a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_fig218117141511"></a>  
![](figures/查找日志界面.png "查找日志界面")

**表 1**  查找日志界面说明

<a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_table24185505"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_row66967736"><th class="cellrowborder" valign="top" width="10.100000000000001%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p55677530"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p55677530"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p55677530"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="89.9%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p13586068"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p13586068"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p13586068"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_row26729743"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p17625587"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p17625587"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p17625587"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p8348195714814"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p8348195714814"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p8348195714814"></a>查找的起始位置，数字为当前选中行。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_row11742498"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p11618275"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p11618275"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p11618275"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p1764732494417"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p1764732494417"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p1764732494417"></a>查找的内容。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_row9577344"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p37567362"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p37567362"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p37567362"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p88951639453"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p88951639453"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p88951639453"></a>时间。查找从一个时间到另一个时间的内容。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_row17557458"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p12868028"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p12868028"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p12868028"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p35677375"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p35677375"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p35677375"></a>时间戳。查找从一个时间戳到另一个时间戳的内容。（ms为单位）</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_row52660921"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p37676179"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p37676179"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p37676179"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p31871641"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p31871641"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p31871641"></a>列名。查找所选列的内容。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_row31466165"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p65731404"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p65731404"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_p65731404"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p612085521112"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p612085521112"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p612085521112"></a>Match Case：匹配大小写。</p>
<p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p148632018199"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p148632018199"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p148632018199"></a>Regular Expressions：使用正则表达式匹配。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_row18852142017542"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p9852120145420"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p9852120145420"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p9852120145420"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p9314117181915"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p9314117181915"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p9314117181915"></a>仅整个单词匹配。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_row14236112495710"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p323622414578"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p323622414578"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p323622414578"></a>8</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_ul14005657"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_ul14005657"></a><ul id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_zh-cn_topic_0279615198_ul14005657"><li>Find Count：查找到的行数。</li><li>Row Count：总行数。</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_row1599044212574"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p1281151511410"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p1281151511410"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p1281151511410"></a>9</p>
<p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p3106157191417"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p3106157191417"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p3106157191417"></a></p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_ul5106117131415"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_ul5106117131415"></a><ul id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_ul5106117131415"><li>Cancel：停止并取消查找。</li><li>Find：开始查找。</li><li>List：显示/关闭查找列表显示区。</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_row1430414467576"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p830494610570"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p830494610570"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p830494610570"></a>10</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p73042461571"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p73042461571"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_zh-cn_topic_0000001384170644_p73042461571"></a>查找列表显示区。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_row11233115203113"><td class="cellrowborder" valign="top" width="10.100000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p12332510312"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p12332510312"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p12332510312"></a>11</p>
</td>
<td class="cellrowborder" valign="top" width="89.9%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p12338515310"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p12338515310"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p12338515310"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_image153351934183119"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_image153351934183119"></a><span><img id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_image153351934183119" src="figures/zh-cn_image_0000002046419282.png"></span>：折叠3，4，5区域。</p>
<p id="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p1368311476310"><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p1368311476310"></a><a name="zh-cn_topic_0000002456054357_zh-cn_topic_0000001713128613_p1368311476310"></a>Dock：Find界面停靠到左侧。</p>
</td>
</tr>
</tbody>
</table>

查找日志操作步骤如下：

1.  右键点击表格，获取到菜单列表并选择“Find...”或者使用快捷键Ctrl+F或者点击工具栏![](figures/zh-cn_image_0000001665009230.png)按钮，打开查找界面。
2.  配置查找参数。
3.  点击Find。

# System插件<a name="ZH-CN_TOPIC_0000002499581809"></a>

-   **[简介](#ZH-CN_TOPIC_0000002499581833)**  

-   **[主要功能及使用说明](#ZH-CN_TOPIC_0000002499501765)**  

## 简介<a name="ZH-CN_TOPIC_0000002499581833"></a>

System插件主要包含常用维测功能。

打开System插件的方法：如[图1](#zh-cn_topic_0000002422655446_zh-cn_topic_0000001665009078_fig1787083216283)、[图2](#zh-cn_topic_0000002422655446_zh-cn_topic_0000001665009078_fig1529407162817)所示，单击菜单栏或工具栏上的System图标即可。

**图 1**  打开system插件示例（菜单栏）<a name="zh-cn_topic_0000002422655446_zh-cn_topic_0000001665009078_fig1787083216283"></a>  
![](figures/打开system插件示例（菜单栏）.png "打开system插件示例（菜单栏）")

**图 2**  打开system插件示例（工具栏）<a name="zh-cn_topic_0000002422655446_zh-cn_topic_0000001665009078_fig1529407162817"></a>  
![](figures/打开system插件示例（工具栏）.png "打开system插件示例（工具栏）")

显示信息栏的方法为：打开System插件后右键点击“Control Panel”的空白处，点击显示信息栏。如[图3](#zh-cn_topic_0000002422655446_zh-cn_topic_0000001665009078_fig12734194310518)所示。

**图 3**  打开显示信息栏<a name="zh-cn_topic_0000002422655446_zh-cn_topic_0000001665009078_fig12734194310518"></a>  
![](figures/打开显示信息栏.png "打开显示信息栏")

## 主要功能及使用说明<a name="ZH-CN_TOPIC_0000002499501765"></a>

-   **[数据上传与下载](#ZH-CN_TOPIC_0000002499501793)**  

-   **[内存读取](#ZH-CN_TOPIC_0000002466542432)**  

-   **[寄存器控制](#ZH-CN_TOPIC_0000002499501797)**  

-   **[死机分析](#ZH-CN_TOPIC_0000002499581845)**  

-   **[多通道数据采集](#ZH-CN_TOPIC_0000002499501769)**  

-   **[自定义参数配置](#ZH-CN_TOPIC_0000002499581817)**  

-   **[多语言脚本调用](#ZH-CN_TOPIC_0000002499501781)**  

-   **[自定义参数转bin](#ZH-CN_TOPIC_0000002499581769)**  

-   **[RF接口测试](#ZH-CN_TOPIC_0000002466382852)**  

-   **[图形压缩对比](#ZH-CN_TOPIC_0000002466382784)**  

-   **[音频质量界面](#ZH-CN_TOPIC_0000002499581829)**  

-   **[提示音工具](#ZH-CN_TOPIC_0000002466542452)**  

-   **[打点文件解析](#ZH-CN_TOPIC_0000002466542476)**  

### 数据上传与下载<a name="ZH-CN_TOPIC_0000002499501793"></a>

-   上传（To Board）是指将工具端的数据（通常是指文件）发送给单板。
-   下载（From Board）是指从单板获取数据传递到工具端，从单板传递的数据包括某段内存的可维可测信息或者某个路径下的文件。

-   **[导出文件和删除文件](#ZH-CN_TOPIC_0000002499501741)**  

-   **[上传文件](#ZH-CN_TOPIC_0000002466542460)**  

#### 导出文件和删除文件<a name="ZH-CN_TOPIC_0000002499501741"></a>

导出文件用于从单板导出指定路径下的文件。

删除文件用于删除单板指定路径下的文件。

**图 1**  导出和删除文件操作界面<a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_fig88511591199"></a>  
![](figures/导出和删除文件操作界面.png "导出和删除文件操作界面")

**表 1**  导出文件界面说明

<a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_table16393344141111"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_row0393204410113"><th class="cellrowborder" valign="top" width="11.360000000000001%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p8393744171119"><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p8393744171119"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p8393744171119"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="88.64%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p5393114415116"><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p5393114415116"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p5393114415116"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_row339364410112"><td class="cellrowborder" valign="top" width="11.360000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p193937442111"><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p193937442111"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p193937442111"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="88.64%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p17393844181115"><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p17393844181115"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p17393844181115"></a>导出的数据存放到工具端的文件夹。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_row6393194411117"><td class="cellrowborder" valign="top" width="11.360000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p1739394417117"><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p1739394417117"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p1739394417117"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="88.64%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p739314451114"><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p739314451114"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p739314451114"></a>单板文件路径，可直接手动输入，先输入'/'可联想出基础路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_row1639394491113"><td class="cellrowborder" valign="top" width="11.360000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p1739316446114"><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p1739316446114"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p1739316446114"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="88.64%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p830319795417"><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p830319795417"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p830319795417"></a>存放[2]对应的文件列表。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_row93931044151116"><td class="cellrowborder" valign="top" width="11.360000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p15393144491118"><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p15393144491118"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p15393144491118"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="88.64%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p1513918335410"><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p1513918335410"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p1513918335410"></a>获取[2]对应路径下的文件列表，显示在[4]中。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_row139374418110"><td class="cellrowborder" valign="top" width="11.360000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p1239384441114"><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p1239384441114"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p1239384441114"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="88.64%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p212718910554"><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p212718910554"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p212718910554"></a>操作：</p>
<a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_ul5106117131415"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_ul5106117131415"></a><ul id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_ul5106117131415"><li>Download：下载[4]中选择的文件。</li><li>Delete：删除[4]中选择的文件。</li><li>Download All：下载[2]路径中所有文件。注意，批量下载只会下载文件列表中的文件，若有新文件产生应再次点击“Get file List”按钮。</li><li>Delete All：删除[2]路径中所有文件。注意，批量删除只会删除文件列表中的文件，若有新文件产生应再次点击“Get file List”按钮。</li></ul>
<a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_ul134751821165816"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_ul134751821165816"></a><ul id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_ul134751821165816"><li>Stop：停止导出文件。</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_row146701843154417"><td class="cellrowborder" valign="top" width="11.360000000000001%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p126709432445"><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p126709432445"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p126709432445"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="88.64%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p26704432445"><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p26704432445"></a><a name="zh-cn_topic_0000002422495550_zh-cn_topic_0000001713128633_zh-cn_topic_0000001237473377_p26704432445"></a>进度条。</p>
</td>
</tr>
</tbody>
</table>

-   **[导出文件](#ZH-CN_TOPIC_0000002466382836)**  

-   **[批量导出文件](#ZH-CN_TOPIC_0000002499581797)**  

-   **[删除文件](#ZH-CN_TOPIC_0000002499581841)**  

-   **[批量删除文件](#ZH-CN_TOPIC_0000002499501757)**  

##### 导出文件<a name="ZH-CN_TOPIC_0000002466382836"></a>

导出文件操作步骤如下：

1.  在“Save To”中选择导出文件保存的路径。
2.  在“File Path”处选择路径。
3.  单击“Get File List”获取上述路径下的文件列表。
4.  在“File Name”处选择要下载的文件名字后，单击“Download”开始下载，待下方进度条更新到100%完成下载。

##### 批量导出文件<a name="ZH-CN_TOPIC_0000002499581797"></a>

批量导出文件操作步骤如下：

1.  在“Save To”中选择导出文件保存的路径。
2.  在“File Path”处选择路径。
3.  单击“Get File List”获取上述路径下的文件列表。
4.  单击“Download All”开始下载，下方进度条每次达到100%证明导出完一个文件。

##### 删除文件<a name="ZH-CN_TOPIC_0000002499581841"></a>

删除文件操作步骤如下：

1.  在“File Path”处选择路径。
2.  单击“Get File List”获取上述路径下的文件列表。
3.  单击“Delete”开始删除。

##### 批量删除文件<a name="ZH-CN_TOPIC_0000002499501757"></a>

批量删除文件操作步骤如下：

1.  在“File Path”处选择路径。
2.  单击“Get File List”获取上述路径下的文件列表。
3.  单击“Delete All”开始删除。

#### 上传文件<a name="ZH-CN_TOPIC_0000002466542460"></a>

上传文件用于向单板指定路径下上传文件。

上传文件支持文件夹上传。

**图 1**  上传文件操作界面<a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_fig11970126195416"></a>  
![](figures/上传文件操作界面.png "上传文件操作界面")

**表 1**  上传文件界面说明

<a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_table16393344141111"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_row0393204410113"><th class="cellrowborder" valign="top" width="11.459999999999999%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p8393744171119"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p8393744171119"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p8393744171119"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="88.53999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p5393114415116"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p5393114415116"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p5393114415116"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_row339364410112"><td class="cellrowborder" valign="top" width="11.459999999999999%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p193937442111"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p193937442111"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p193937442111"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="88.53999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p8860134131813"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p8860134131813"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p8860134131813"></a>待上传文件的绝对路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_row6393194411117"><td class="cellrowborder" valign="top" width="11.459999999999999%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p1739394417117"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p1739394417117"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p1739394417117"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="88.53999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p739314451114"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p739314451114"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p739314451114"></a>选择文件，单文件上传。结果显示在[1]中。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_row1639394491113"><td class="cellrowborder" valign="top" width="11.459999999999999%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p1739316446114"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p1739316446114"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p1739316446114"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="88.53999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p1039311448118"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p1039311448118"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p1039311448118"></a>选择文件夹，文件批量上传。结果显示在[1]中</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_row93931044151116"><td class="cellrowborder" valign="top" width="11.459999999999999%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p15393144491118"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p15393144491118"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p15393144491118"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="88.53999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p139354420119"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p139354420119"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p139354420119"></a>填写单板路径。</p>
<div class="note" id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_note2950415104611"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_note2950415104611"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_note2950415104611"></a><span class="notetitle"> 说明： </span><div class="notebody"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_ul107106151794"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_ul107106151794"></a><ul id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_ul107106151794"><li>当Local File Path选择文件时，Board File Path填写精确到文件名。</li><li>当Local File Path选择文件夹时，Board File Path填写文件夹，并以'/'结尾。</li></ul>
</div></div>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_row139374418110"><td class="cellrowborder" valign="top" width="11.459999999999999%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p1239384441114"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p1239384441114"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p1239384441114"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="88.53999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p5726153414208"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p5726153414208"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p5726153414208"></a>开始上传文件。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_row146701843154417"><td class="cellrowborder" valign="top" width="11.459999999999999%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p126709432445"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p126709432445"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p126709432445"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="88.53999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p26704432445"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p26704432445"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p26704432445"></a>停止。在upload过程中可以主动停止传输。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_row19191159135918"><td class="cellrowborder" valign="top" width="11.459999999999999%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p919110599598"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p919110599598"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p919110599598"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="88.53999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p121913590596"><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p121913590596"></a><a name="zh-cn_topic_0000002422655458_zh-cn_topic_0000001713048657_zh-cn_topic_0000001451473085_p121913590596"></a>进度条。</p>
</td>
</tr>
</tbody>
</table>

上传文件操作步骤如下：

1.  点击“Choose File”或者“Choose Folder”选择待上传文件。
2.  在“Board File Path”填写板端路径。注意，文件或者文件夹均要与“Local File Path”对应。
3.  单击“Upload”开始上传，待进度条达到100%后传输完成。

### 内存读取<a name="ZH-CN_TOPIC_0000002466542432"></a>

内存读取功能用于将设备中一段内存保存到计算机指定文件中。

**图 1**  内存读取界面<a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_fig1824193615262"></a>  
![](figures/内存读取界面.png "内存读取界面")

**表 1**  内存读取说明

<a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_toc32853064"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_row29866445"><th class="cellrowborder" valign="top" width="19.31%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p3262944"><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p3262944"></a><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p3262944"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="80.69%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p62971949"><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p62971949"></a><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p62971949"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_row454234"><td class="cellrowborder" valign="top" width="19.31%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p36792975"><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p36792975"></a><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p36792975"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="80.69%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p27441013"><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p27441013"></a><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p27441013"></a>单击“...”选择文件。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_row45642530"><td class="cellrowborder" valign="top" width="19.31%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p6057427"><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p6057427"></a><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p6057427"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="80.69%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p20889610"><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p20889610"></a><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615208_p20889610"></a>设置参数。</p>
<a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615198_ul14005657"></a><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615198_ul14005657"></a><ul id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_zh-cn_topic_0279615198_ul14005657"><li>Address：内存读取起始地址（16进制）。</li><li>Data Length：内存读取数据长度（一般为10进制，若16进制请在前面添加0x，输入数据应为4的整数倍）</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_row1068983935814"><td class="cellrowborder" valign="top" width="19.31%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_p069011399586"><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_p069011399586"></a><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_p069011399586"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="80.69%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_p16901939135815"><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_p16901939135815"></a><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_p16901939135815"></a>内存读取开始按钮。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_row6914455301"><td class="cellrowborder" valign="top" width="19.31%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_p1491513510307"><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_p1491513510307"></a><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_p1491513510307"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="80.69%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_p691565183014"><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_p691565183014"></a><a name="zh-cn_topic_0000002456134265_zh-cn_topic_0000001665168822_zh-cn_topic_0000001178795102_p691565183014"></a>内存导出进度条。</p>
</td>
</tr>
</tbody>
</table>

操作步骤如下：

1.  在“Save To”中选择需要保存的路径。
2.  设定参数。

    填写“Address”、“Data Length”。

3.  单击“Read”按钮。

    设备中读取出数据保存到“Save To”填写的路径中。

### 寄存器控制<a name="ZH-CN_TOPIC_0000002499501797"></a>

RegisterControl界面主要用于寄存器的读写。

**图 1**  寄存器读写功能主要界面<a name="zh-cn_topic_0000002456054369_zh-cn_topic_0000001665168834_zh-cn_topic_0000001283705438_fig112051548154519"></a>  
![](figures/寄存器读写功能主要界面.png "寄存器读写功能主要界面")

-   **[连接方式](#ZH-CN_TOPIC_0000002466382868)**  

-   **[单寄存器读写](#ZH-CN_TOPIC_0000002466542440)**  

-   **[寄存器表格](#ZH-CN_TOPIC_0000002499581785)**  

-   **[寄存器面板](#ZH-CN_TOPIC_0000002499501777)**  

#### 连接方式<a name="ZH-CN_TOPIC_0000002466382868"></a>

设置读写方式、读写长度。如[图1](#zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_fig47161122310)所示。参数说明如[表1](#zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_toc32853064)所示。

**图 1**  寄存器读写方式选择<a name="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_fig47161122310"></a>  
![](figures/寄存器读写方式选择.png "寄存器读写方式选择")

**表 1**  寄存器读写方式选择说明

<a name="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_toc32853064"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_row29866445"><th class="cellrowborder" valign="top" width="13.07%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_p3262944"><a name="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_p3262944"></a><a name="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_p3262944"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="86.92999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_p62971949"><a name="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_p62971949"></a><a name="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_p62971949"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_row454234"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_p36792975"><a name="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_p36792975"></a><a name="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_p36792975"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_p166391945113316"><a name="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_p166391945113316"></a><a name="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_p166391945113316"></a>读写方式。默认为Cmd方式，若选择jLink方式需要在菜单栏Options中选择jlink路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_row45642530"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_p6057427"><a name="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_p6057427"></a><a name="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_zh-cn_topic_0279615208_p6057427"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_p1095616403146"><a name="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_p1095616403146"></a><a name="zh-cn_topic_0000002422495562_zh-cn_topic_0000001665009102_zh-cn_topic_0000001409066533_p1095616403146"></a>读写长度。默认为32位读写。</p>
</td>
</tr>
</tbody>
</table>

#### 单寄存器读写<a name="ZH-CN_TOPIC_0000002466542440"></a>

**图 1**  单个寄存器读写<a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_fig88341020276"></a>  
![](figures/单个寄存器读写.png "单个寄存器读写")

**表 1**  寄存器界面说明

<a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_zh-cn_topic_0279615208_toc32853064"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_zh-cn_topic_0279615208_row29866445"><th class="cellrowborder" valign="top" width="13.07%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_zh-cn_topic_0279615208_p3262944"><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_zh-cn_topic_0279615208_p3262944"></a><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_zh-cn_topic_0279615208_p3262944"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="86.92999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_zh-cn_topic_0279615208_p62971949"><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_zh-cn_topic_0279615208_p62971949"></a><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_zh-cn_topic_0279615208_p62971949"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_zh-cn_topic_0279615208_row45642530"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_zh-cn_topic_0279615208_p6057427"><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_zh-cn_topic_0279615208_p6057427"></a><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_zh-cn_topic_0279615208_p6057427"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p135441240183510"><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p135441240183510"></a><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p135441240183510"></a>寄存器地址和值。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_row139646576421"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p5964105710420"><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p5964105710420"></a><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p5964105710420"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p125967511655"><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p125967511655"></a><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p125967511655"></a>寄存器值以二进制方式显示。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_row1482132153810"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p1283526382"><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p1283526382"></a><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p1283526382"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p111251123505"><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p111251123505"></a><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p111251123505"></a>读寄存器按钮。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_row0583171014382"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p14583101083816"><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p14583101083816"></a><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p14583101083816"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p145153185018"><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p145153185018"></a><a name="zh-cn_topic_0000002422655466_zh-cn_topic_0000001713128653_zh-cn_topic_0000001408946741_p145153185018"></a>写寄存器按钮。</p>
</td>
</tr>
</tbody>
</table>

单个寄存器读写功能的步骤如下：

1.  输入寄存器地址和值，如果是读寄存器可不输入值。
2.  点击读寄存器或写寄存器查看结果。

#### 寄存器表格<a name="ZH-CN_TOPIC_0000002499581785"></a>

**图 1**  寄存器列表界面<a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_fig202928454418"></a>  
![](figures/寄存器列表界面.png "寄存器列表界面")

**表 1**  寄存器界面说明

<a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_zh-cn_topic_0279615208_toc32853064"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_zh-cn_topic_0279615208_row29866445"><th class="cellrowborder" valign="top" width="13.07%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_zh-cn_topic_0279615208_p3262944"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_zh-cn_topic_0279615208_p3262944"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_zh-cn_topic_0279615208_p3262944"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="86.92999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_zh-cn_topic_0279615208_p62971949"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_zh-cn_topic_0279615208_p62971949"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_zh-cn_topic_0279615208_p62971949"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_zh-cn_topic_0279615208_row45642530"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_zh-cn_topic_0279615208_p6057427"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_zh-cn_topic_0279615208_p6057427"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_zh-cn_topic_0279615208_p6057427"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p135441240183510"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p135441240183510"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p135441240183510"></a>寄存器表格。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row139646576421"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p5964105710420"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p5964105710420"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p5964105710420"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p125967511655"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p125967511655"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p125967511655"></a>写寄存器表格。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row1482132153810"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1283526382"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1283526382"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1283526382"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p3165123210465"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p3165123210465"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p3165123210465"></a>读寄存器表格。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row0583171014382"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p14583101083816"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p14583101083816"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p14583101083816"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p145153185018"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p145153185018"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p145153185018"></a>加载寄存器表格。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row121968516461"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1819665111463"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1819665111463"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1819665111463"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p17196451204615"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p17196451204615"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p17196451204615"></a>保存寄存器表格。</p>
</td>
</tr>
</tbody>
</table>

**图 2**  表头界面<a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_fig942513282543"></a>  
![](figures/表头界面.png "表头界面")

**图 3**  表头界面补充<a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_fig2818191110587"></a>  
![](figures/表头界面补充.png "表头界面补充")

**表 2**  表头界面说明

<a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_table9682126175510"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row136828611554"><th class="cellrowborder" valign="top" width="13.07%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p186825618558"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p186825618558"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p186825618558"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="86.92999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p068216695510"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p068216695510"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p068216695510"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row116821611554"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p268346195510"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p268346195510"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p268346195510"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p56835619554"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p56835619554"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p56835619554"></a>寄存器名字。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row136834619551"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p06834610558"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p06834610558"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p06834610558"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1668336155517"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1668336155517"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1668336155517"></a>寄存器地址。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row17683561553"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1468314612556"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1468314612556"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1468314612556"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1468306125518"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1468306125518"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1468306125518"></a>寄存器值。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row176831612559"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p368312635515"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p368312635515"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p368312635515"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p268312615517"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p268312615517"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p268312615517"></a>读写寄存器间隔时间。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row107376422585"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p27372424580"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p27372424580"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p27372424580"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1873710424588"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1873710424588"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1873710424588"></a>读写长度。支持32位和16位长度读写。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row196835665511"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1568314610550"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1568314610550"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1568314610550"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p16830611550"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p16830611550"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p16830611550"></a>备注。</p>
</td>
</tr>
</tbody>
</table>

**图 4**  右键菜单功能<a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_fig46231232114917"></a>  
![](figures/右键菜单功能.png "右键菜单功能")

**表 3**  菜单界面说明

<a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_table1290610589498"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row19061358194914"><th class="cellrowborder" valign="top" width="13.07%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p7906185811494"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p7906185811494"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p7906185811494"></a>功能</p>
</th>
<th class="cellrowborder" valign="top" width="86.92999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p12906658204912"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p12906658204912"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p12906658204912"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row1190645854913"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p10906195824913"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p10906195824913"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p10906195824913"></a>Add Register</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1290605874916"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1290605874916"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1290605874916"></a>在当前选中行上方添加一行。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row490635834919"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p99063589492"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p99063589492"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p99063589492"></a>Delete Register</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p2906185815499"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p2906185815499"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p2906185815499"></a>删除当前行。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_row1590625894914"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p189065582491"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p189065582491"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p189065582491"></a>Clear Table</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1990685814916"><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1990685814916"></a><a name="zh-cn_topic_0000002456134269_zh-cn_topic_0000001713048673_zh-cn_topic_0000001359506500_p1990685814916"></a>清空表格。</p>
</td>
</tr>
</tbody>
</table>

寄存器表格读写功能的步骤如下：

1.  右键Add Register逐个添加寄存器或点击Load Config读取Excel表格添加寄存器。
2.  点击Write Table或Read Table读写寄存器。

    注：读寄存器时Address列和Interval列不能为空；写寄存器时Address列、Value列和Interval列不能为空。

3.  点击Save Config保存表格到Excel文件中。

#### 寄存器面板<a name="ZH-CN_TOPIC_0000002499501777"></a>

**图 1**  寄存器面板界面<a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_fig14242132169"></a>  
![](figures/寄存器面板界面.png "寄存器面板界面")

**图 2**  寄存器面板表头界面<a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_fig11692194595"></a>  
![](figures/寄存器面板表头界面.png "寄存器面板表头界面")

**表 1**  表头界面说明

<a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_table9682126175510"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_row136828611554"><th class="cellrowborder" valign="top" width="13.07%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p186825618558"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p186825618558"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p186825618558"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="86.92999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p068216695510"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p068216695510"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p068216695510"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_row116821611554"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p268346195510"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p268346195510"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p268346195510"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p56835619554"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p56835619554"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p56835619554"></a>寄存器名字。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_row136834619551"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p06834610558"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p06834610558"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p06834610558"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1668336155517"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1668336155517"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1668336155517"></a>寄存器值（十六进制）。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_row17683561553"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1468314612556"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1468314612556"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1468314612556"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1468306125518"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1468306125518"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1468306125518"></a>寄存器值（二进制）。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_row176831612559"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p368312635515"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p368312635515"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p368312635515"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p268312615517"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p268312615517"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p268312615517"></a>寄存器初始值。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_row196835665511"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1568314610550"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1568314610550"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1568314610550"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p16830611550"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p16830611550"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p16830611550"></a>寄存器访问权限。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_row9628857191011"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p206285570100"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p206285570100"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p206285570100"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p262855721016"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p262855721016"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p262855721016"></a>寄存器地址。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_row9283181121111"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p4283319114"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p4283319114"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p4283319114"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p9284512117"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p9284512117"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p9284512117"></a>说明。</p>
</td>
</tr>
</tbody>
</table>

**图 3**  右键菜单功能<a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_fig851313474611"></a>  
![](figures/右键菜单功能-0.png "右键菜单功能-0")

**表 2**  菜单界面说明

<a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_table1290610589498"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_row19061358194914"><th class="cellrowborder" valign="top" width="13.07%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p7906185811494"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p7906185811494"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p7906185811494"></a>功能</p>
</th>
<th class="cellrowborder" valign="top" width="86.92999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p12906658204912"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p12906658204912"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p12906658204912"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_row1190645854913"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p10906195824913"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p10906195824913"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p10906195824913"></a>Link</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1290605874916"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1290605874916"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p1290605874916"></a>选中Link时，当前寄存器和左面单个寄存器界面联动。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_row490635834919"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p99063589492"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p99063589492"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p99063589492"></a>Unlink</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p2906185815499"><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p2906185815499"></a><a name="zh-cn_topic_0000002456054373_zh-cn_topic_0000001665168838_zh-cn_topic_0000001408786821_p2906185815499"></a>取消联动。</p>
</td>
</tr>
</tbody>
</table>

寄存器表格读写功能的步骤如下：

1.  双击寄存器名字，读取寄存器的值。
2.  修改父节点的值，子节点的值会根据所占位数改变；修改子节点的值，父节点的值也会改变。
3.  右键选择Link，当前寄存器或其父节点会和左面单个寄存器界面联动，选择Unlink取消联动。

### 死机分析<a name="ZH-CN_TOPIC_0000002499581845"></a>

DumpAnalys功能用于解析死机信息。

>![](public_sys-resources/icon-note.gif) **说明：** 
>此功能需要python3.8及以上版本环境。

**图 1**  DumpAnalys界面<a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_fig19781956102713"></a>  
![](figures/DumpAnalys界面.png "DumpAnalys界面")

**表 1**  DumpAnalys界面区域说明

<a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_table5268451152811"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_row1926835114287"><th class="cellrowborder" valign="top" width="13.87%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p526855119282"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p526855119282"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p526855119282"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="86.13%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p7268351122813"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p7268351122813"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p7268351122813"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_row1126865111283"><td class="cellrowborder" valign="top" width="13.87%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p9268165120281"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p9268165120281"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p9268165120281"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="86.13%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p426815513288"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p426815513288"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p426815513288"></a>名为“parse_main_xxxx_phase2.py”的解析脚本路径，一般来自单板版本发布包。其中“xxxx”表示操作系统名称，包含liteos207和freertos，需要根据使用的固件包对应的操作系统选择对应的脚本。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_row626895116286"><td class="cellrowborder" valign="top" width="13.87%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p22681651142810"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p22681651142810"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p22681651142810"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="86.13%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p02680513282"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p02680513282"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p02680513282"></a>NM文件路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_row6564162611361"><td class="cellrowborder" valign="top" width="13.87%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1656412683613"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1656412683613"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1656412683613"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="86.13%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p5565122613616"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p5565122613616"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p5565122613616"></a>Global文件路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_row1431272918374"><td class="cellrowborder" valign="top" width="13.87%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1312152917378"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1312152917378"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1312152917378"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="86.13%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p113128298379"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p113128298379"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p113128298379"></a>死机信息文件路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_row92541019185417"><td class="cellrowborder" valign="top" width="13.87%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_p1325411935417"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_p1325411935417"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_p1325411935417"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="86.13%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_p18254919145411"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_p18254919145411"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_p18254919145411"></a>输出文件路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_row62981517164020"><td class="cellrowborder" valign="top" width="13.87%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1299417134014"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1299417134014"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1299417134014"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="86.13%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p19299171754011"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p19299171754011"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p19299171754011"></a>解析按钮。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_row1829516123714"><td class="cellrowborder" valign="top" width="13.87%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1929601220713"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1929601220713"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1929601220713"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="86.13%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1429611125710"><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1429611125710"></a><a name="zh-cn_topic_0000002422495566_zh-cn_topic_0000001713128665_zh-cn_topic_0000001447725041_p1429611125710"></a>解析结果展示区。</p>
</td>
</tr>
</tbody>
</table>

死机信息解析功能的步骤如下：

1.  分别选择解析脚本“parse\_main\_xxxx\_phase2.py”、NM文件、Global文件、死机信息文件夹路径、输出文件路径。
2.  点击“Execute”按钮，分析结果展示在白框中。例如

    ![](figures/zh-cn_image_0000002483337600.png)

### 多通道数据采集<a name="ZH-CN_TOPIC_0000002499501769"></a>

多通道数据采集界面主要用来持续接收单板业务上传的数据，并将数据保存。

-   **[蓝牙数据采集](#ZH-CN_TOPIC_0000002499501805)**  

#### 蓝牙数据采集<a name="ZH-CN_TOPIC_0000002499501805"></a>

蓝牙数据采集界面如[图1](#zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_fig6430197314)所示。

**图 1**  蓝牙数据采集主界面<a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_fig6430197314"></a>  
![](figures/蓝牙数据采集主界面.png "蓝牙数据采集主界面")

**表 1**  蓝牙数据采集界面说明

<a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_table9682126175510"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_row136828611554"><th class="cellrowborder" valign="top" width="13.07%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p186825618558"><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p186825618558"></a><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p186825618558"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="86.92999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p068216695510"><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p068216695510"></a><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p068216695510"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_row116821611554"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p268346195510"><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p268346195510"></a><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p268346195510"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p56835619554"><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p56835619554"></a><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p56835619554"></a>保存路径选择区。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_row136834619551"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p06834610558"><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p06834610558"></a><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p06834610558"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p101102812411"><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p101102812411"></a><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p101102812411"></a>多通道选择区，用来选择需要数据采集的通道。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_row17683561553"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p1468314612556"><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p1468314612556"></a><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p1468314612556"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p8360192513213"><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p8360192513213"></a><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p8360192513213"></a>Collection Size：数据保存大小展示区。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_row35998104119"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p1599161104112"><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p1599161104112"></a><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p1599161104112"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p1259910113418"><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p1259910113418"></a><a name="zh-cn_topic_0000002456134273_zh-cn_topic_0000001718143957_zh-cn_topic_0000001411144749_p1259910113418"></a>任务开始和结束选择区。</p>
</td>
</tr>
</tbody>
</table>

蓝牙数据采集的步骤如下：

1.  在保存路径选择区点击“...”按钮，在弹出的框中选择要保存的路径。
2.  在多通道选择区选择一个或多个需要数据采集的通道。
3.  点击任务开始和结束区的“Start”按钮，开始采集任务，会在Collection Size中显示采集出的数据大小。
4.  最后点击“Stop”按钮结束数据采集，将会在要保存的路径下生成对应个数的bin文件。

### 自定义参数配置<a name="ZH-CN_TOPIC_0000002499581817"></a>

PQUI界面是自定义参数配置界面，可将界面中存在的参数下发到单板。

**图 1**  PQUI主界面<a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_fig152216171310"></a>  

![](figures/ScreenShot_20251222212339.png)

**表 1**  PQUI界面说明

<a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_toc32853064"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_row29866445"><th class="cellrowborder" valign="top" width="13.07%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_p3262944"><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_p3262944"></a><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_p3262944"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="86.92999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_p62971949"><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_p62971949"></a><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_p62971949"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_row454234"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_p36792975"><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_p36792975"></a><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_p36792975"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p142081143125110"><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p142081143125110"></a><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p142081143125110"></a>参数区：配置需要下发的参数即回显单板数据。单击参数区Write或者Read，对单个结构体进行写入或者读取操作。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_row45642530"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_p6057427"><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_p6057427"></a><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_zh-cn_topic_0279615208_p6057427"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p4572195912517"><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p4572195912517"></a><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p4572195912517"></a>All Read按钮：读取全部参数在单板中的数据值并显示在对应参数区。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_row1068983935814"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p069011399586"><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p069011399586"></a><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p069011399586"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p968014405219"><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p968014405219"></a><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p968014405219"></a>All Write按钮：向单板中写入全部参数区设置的数据。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_row482383262514"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p18824113202520"><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p18824113202520"></a><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p18824113202520"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p82323104529"><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p82323104529"></a><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p82323104529"></a>Read Page按钮：读取当前页面参数在单板中的数据值并显示在对应参数区。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_row2060583181218"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p126061532122"><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p126061532122"></a><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p126061532122"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p0228167522"><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p0228167522"></a><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p0228167522"></a>Write Page按钮：向单板中写入当前页参数区设置的数据。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_row2040165117142"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p1240151141416"><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p1240151141416"></a><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p1240151141416"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p855353011522"><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p855353011522"></a><a name="zh-cn_topic_0000002456054377_zh-cn_topic_0000001713128645_zh-cn_topic_0000001239734721_p855353011522"></a>Auto Write：选择是否每次修改都向单板发送一次此时当前参数区结构体的数据。</p>
</td>
</tr>
</tbody>
</table>

界面修改参数后向板端read或write的操作步骤如下：

1.  修改对应界面中某些项的值。
2.  单击group的“Write”，单独下发当前group的值。

    单击“Write Page”按钮，下发当前界面所有group的值。

    单击“All Write”按钮，下发全部group的值。

    单击group的“Read”，读取单板中当前group的值。

    单击“Read Page”按钮，读取单板中当前界面所有group的值。

    单击“All Read”按钮，读取单板中全部group的值。

### 多语言脚本调用<a name="ZH-CN_TOPIC_0000002499501781"></a>

多语言脚本调用界面用于将xml信息转换为.lang文件，单板将使用lang文件作为资源支持不同语言字符展示。

>![](public_sys-resources/icon-note.gif) **说明：** 
>此功能需要python3.8及以上版本环境。

**图 1**  多语言脚本调用界面<a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_fig81541245165014"></a>  
![](figures/多语言脚本调用界面.png "多语言脚本调用界面")

**表 1**  多语言脚本调用界面说明

<a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_table12399332111610"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_row23991732151617"><th class="cellrowborder" valign="top" width="16.580000000000002%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p1639923291612"><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p1639923291612"></a><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p1639923291612"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="83.42%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p199034394176"><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p199034394176"></a><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p199034394176"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_row15399153291618"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p1399732161618"><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p1399732161618"></a><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p1399732161618"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p495255512170"><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p495255512170"></a><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p495255512170"></a>名为“language_trans.py”的python脚本，一般来自单板版本发布包。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_row439913218165"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p133991032101612"><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p133991032101612"></a><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p133991032101612"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p87308145221"><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p87308145221"></a><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p87308145221"></a>调用文件路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_row19399163251615"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p3399163251616"><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p3399163251616"></a><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p3399163251616"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p7399143241611"><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p7399143241611"></a><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p7399143241611"></a>输出路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_row1339983218161"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p139923291615"><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p139923291615"></a><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p139923291615"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p1771986218"><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p1771986218"></a><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p1771986218"></a>调用按钮。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_row239903218165"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p1399113281617"><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p1399113281617"></a><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p1399113281617"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p107081943202115"><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p107081943202115"></a><a name="zh-cn_topic_0000002422495570_zh-cn_topic_0000001713048689_zh-cn_topic_0000001506912113_p107081943202115"></a>调用结果。</p>
</td>
</tr>
</tbody>
</table>

多语言脚本调用步骤如下：

1.  分别选择“language\_trans.py”脚本、转换文件路径和输出路径。
2.  点击“Execute”按钮，调用结果展示在白框中。

### 自定义参数转bin<a name="ZH-CN_TOPIC_0000002499581769"></a>

自定义参数转bin用于设置参数并将其写入到bin文件中。界面如[图1](#zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_fig395161991915)所示

**图 1**  自定义参数转bin界面<a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_fig395161991915"></a>  

![](figures/ScreenShot_20251222212339-1.png)

**表 1**  自定义参数转bin界面说明

<a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_table5268451152811"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_row1926835114287"><th class="cellrowborder" valign="top" width="6.45%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p526855119282"><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p526855119282"></a><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p526855119282"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="93.55%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p7268351122813"><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p7268351122813"></a><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p7268351122813"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_row1126865111283"><td class="cellrowborder" valign="top" width="6.45%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p9268165120281"><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p9268165120281"></a><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p9268165120281"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="93.55%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p426815513288"><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p426815513288"></a><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p426815513288"></a>文件选择。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_row626895116286"><td class="cellrowborder" valign="top" width="6.45%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p22681651142810"><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p22681651142810"></a><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p22681651142810"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="93.55%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p02680513282"><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p02680513282"></a><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p02680513282"></a>Page Write：整页写入bin文件</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_row6564162611361"><td class="cellrowborder" valign="top" width="6.45%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p1656412683613"><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p1656412683613"></a><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p1656412683613"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="93.55%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p162173772113"><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p162173772113"></a><a name="zh-cn_topic_0000002422655474_zh-cn_topic_0000001713048685_zh-cn_topic_0000001424704660_p162173772113"></a>参数设置信息页。</p>
</td>
</tr>
</tbody>
</table>

-   **[选择文件并读取](#ZH-CN_TOPIC_0000002466382864)**  

-   **[写入bin文件](#ZH-CN_TOPIC_0000002499581801)**  

#### 选择文件并读取<a name="ZH-CN_TOPIC_0000002466382864"></a>

选择文件并读取步骤如下：

1.  点击“Browser”选择文件。并显示出选择文件界面。如[图1](#zh-cn_topic_0000002456134277_zh-cn_topic_0000001665168854_zh-cn_topic_0000001424388380_fig641824911422)所示

    **图 1**  选择文件界面<a name="zh-cn_topic_0000002456134277_zh-cn_topic_0000001665168854_zh-cn_topic_0000001424388380_fig641824911422"></a>  
    ![](figures/选择文件界面.png "选择文件界面")

2.  选择一个bin文件。将bin文件数据显示在界面中。

#### 写入bin文件<a name="ZH-CN_TOPIC_0000002499581801"></a>

写入bin文件步骤如下：

1.  点击“Browser”选择文件。
2.  在参数设置信息页设置参数。
3.  点击“Page Write”将设置的参数写入选择的文件。

### RF接口测试<a name="ZH-CN_TOPIC_0000002466382852"></a>

RF接口测试界面主要根据界面上的参数拼接命令通过串口下发到单板。

**图 1**  RF接口测试主界面<a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_fig1320112392155"></a>  

![](figures/ScreenShot_20251222212339-2.png)

**表 1**  RF接口测试界面说明

<a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_table12399332111610"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_row23991732151617"><th class="cellrowborder" valign="top" width="16.580000000000002%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p1639923291612"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p1639923291612"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p1639923291612"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="83.42%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p199034394176"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p199034394176"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p199034394176"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_row15399153291618"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p1399732161618"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p1399732161618"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p1399732161618"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p495255512170"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p495255512170"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p495255512170"></a>配置收发数据的端口号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_row439913218165"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p133991032101612"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p133991032101612"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p133991032101612"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p87308145221"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p87308145221"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p87308145221"></a>配置收发命令的波特率等串口常用的参数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_row19399163251615"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p3399163251616"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p3399163251616"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p3399163251616"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p7399143241611"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p7399143241611"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p7399143241611"></a>选择收发命令展示区显示内容。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_row245119192812"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p34517910281"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p34517910281"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p34517910281"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p184512992812"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p184512992812"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p184512992812"></a>当命令是由多条组成时，设置命令发送的间隔。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_row3750613132815"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p275010139285"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p275010139285"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p275010139285"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p175011133285"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p175011133285"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p175011133285"></a>字符串搜索框。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_row1339983218161"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p139923291615"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p139923291615"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p139923291615"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p1771986218"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p1771986218"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p1771986218"></a>收发命令展示区。绿色字符为串口接收数据，黑色字符为串口发送数据。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_row239903218165"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p1399113281617"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p1399113281617"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001474467993_p1399113281617"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001212193080_p064271394213"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001212193080_p064271394213"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_zh-cn_topic_0000001212193080_p064271394213"></a>配置需要收发命令的参数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_row19621131620116"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p9621616121111"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p9621616121111"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p9621616121111"></a>8</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p3621111611113"><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p3621111611113"></a><a name="zh-cn_topic_0000002422495574_zh-cn_topic_0000001713128669_p3621111611113"></a>字符串搜索框。</p>
</td>
</tr>
</tbody>
</table>

### 图形压缩对比<a name="ZH-CN_TOPIC_0000002466382784"></a>

PictureCompress界面主要是对图片的压缩和对比。

**图 1**  PictureCompress主界面<a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_fig1159068527"></a>  
![](figures/PictureCompress主界面.png "PictureCompress主界面")

**表 1**  界面说明

<a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_toc32853064"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_row29866445"><th class="cellrowborder" valign="top" width="13.07%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_p3262944"><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_p3262944"></a><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_p3262944"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="86.92999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_p62971949"><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_p62971949"></a><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_p62971949"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_row454234"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_p36792975"><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_p36792975"></a><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_p36792975"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p1390045223217"><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p1390045223217"></a><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p1390045223217"></a>待压缩图片路径，压缩算法选择按钮和设置按钮。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_row45642530"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_p6057427"><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_p6057427"></a><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_zh-cn_topic_0279615208_p6057427"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p135441240183510"><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p135441240183510"></a><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p135441240183510"></a>压缩文件保存路径，开始压缩按钮。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_row139646576421"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p5964105710420"><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p5964105710420"></a><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p5964105710420"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p496435754210"><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p496435754210"></a><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p496435754210"></a>输入图片展示区。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_row1482132153810"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p1283526382"><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p1283526382"></a><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p1283526382"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p683152183818"><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p683152183818"></a><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p683152183818"></a>压缩结果图片展示区。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_row0583171014382"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p14583101083816"><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p14583101083816"></a><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p14583101083816"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p15832108389"><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p15832108389"></a><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p15832108389"></a>对比参数设置区。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_row33529469408"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p16352164684015"><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p16352164684015"></a><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p16352164684015"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p83525463406"><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p83525463406"></a><a name="zh-cn_topic_0000002422655478_zh-cn_topic_0000001665009098_zh-cn_topic_0000001249728618_p83525463406"></a>对比效果展示区。</p>
</td>
</tr>
</tbody>
</table>

-   **[压缩功能-CS](#ZH-CN_TOPIC_0000002466542472)**  

-   **[压缩功能-FB](#ZH-CN_TOPIC_0000002499581805)**  

-   **[对比功能](#ZH-CN_TOPIC_0000002499501749)**  

#### 压缩功能-CS<a name="ZH-CN_TOPIC_0000002466542472"></a>

将png、jpg、bmp图片按照不同的压缩方式压缩为bin文件并保存。

**图 1**  压缩功能主要界面<a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_fig467519201425"></a>  
![](figures/压缩功能主要界面.png "压缩功能主要界面")

**表 1**  压缩界面说明

<a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_toc32853064"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_row29866445"><th class="cellrowborder" valign="top" width="13.07%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p3262944"><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p3262944"></a><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p3262944"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="86.92999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p62971949"><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p62971949"></a><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p62971949"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_row454234"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p36792975"><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p36792975"></a><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p36792975"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p1390045223217"><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p1390045223217"></a><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p1390045223217"></a>待压缩图片的文件夹路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_row45642530"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p6057427"><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p6057427"></a><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p6057427"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p135441240183510"><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p135441240183510"></a><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p135441240183510"></a>压缩完成的二进制和头文件的输出文件夹路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_row7715326315"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_p18783213316"><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_p18783213316"></a><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_p18783213316"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_p3743213313"><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_p3743213313"></a><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_p3743213313"></a>压缩算法选择按钮。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_row139646576421"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p5964105710420"><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p5964105710420"></a><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p5964105710420"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p496435754210"><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p496435754210"></a><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p496435754210"></a>压缩开始按钮。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_row1482132153810"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p1283526382"><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p1283526382"></a><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p1283526382"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p111251123505"><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p111251123505"></a><a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_zh-cn_topic_0000001249568898_p111251123505"></a>配置压缩参数。</p>
</td>
</tr>
</tbody>
</table>

CS算法图片压缩功能的步骤如下：

1.  点击图片路径选择按钮“...” 选择需要压缩的图片的路径。
2.  点击“Setting”按钮，显示如下所示界面，配置压缩的条件。

    **图 2**  压缩参数选择<a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_fig11173132612347"></a>  
    ![](figures/压缩参数选择.png "压缩参数选择")

3.  点击“Compress”按钮，开始进行压缩。
4.  <a name="zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_li10173126153414"></a>点击右上角Pre或者Next选择合适的结果，点击OK确定结果。
5.  重复[4](#zh-cn_topic_0000002456134281_zh-cn_topic_0000001713128649_li10173126153414)直到流程结束。

>![](public_sys-resources/icon-note.gif) **说明：** 
>-   输入文件支持ARBG888、RGB888、RGB565三种格式，输出文件和输入文件的格式相同。输入文件的像素大小应小于1280×800。
>-   参数说明
>    -   TileWidth：压缩单元宽度，4--4x4；6--6x4；8--8x4；16--16x4，也叫压缩倍率。在ARGB8888格式时分别对应4倍压缩，6倍压缩，8倍压缩，16倍压缩，在RGB888时分别对应3倍压缩，4.5倍压缩，6倍压缩，12倍压缩，在RGB565时分别对应2倍压缩，3倍压缩，4倍压缩，8倍压缩。
>    -   ModeAlpha：压缩模式：0-- 非压缩；1-- 压缩。
>    -   ModeRGB：压缩模式：0--非压缩；1-- 压缩。
>    -   Autocmpmode：压缩模式：1--自动压缩模式，对压缩文件进行4倍、6倍、8倍、16倍压缩；0--手动压缩模式，按照固定配置参数进行压缩。若选择自动压缩模式，每完成一个图片压缩，会生成四个备选压缩结果，同时将四个压缩结果在区域4中预览，通过观察或者对比功能选择合适的结果，确定结果时保留选择的图片。
>    -   IsPackage：压缩流程中是否额外生成打包bin文件。
>    -   FullPreview：1--查看每张图的压缩结果，每完成一张图片压缩并选择合适的结果后开始下一张的压缩流程，直到所有的图片压缩完成；0--不查看压缩结果。
>-   压缩倍率是与裸数据大小进行对比，而非与源文件大小对比，例如466×466的RGBA图片，无压缩大小为848k，6倍压缩时为142k，16倍压缩时大约为54k。
>-   压缩操作为有损压缩，压缩倍率越大，图损越大。

#### 压缩功能-FB<a name="ZH-CN_TOPIC_0000002499581805"></a>

将png、jpg、bmp图片按照不同的压缩方式压缩为bin文件并保存。

**图 1**  压缩功能主要界面<a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_fig467519201425"></a>  
![](figures/压缩功能主要界面-3.png "压缩功能主要界面-3")

**表 1**  压缩界面说明

<a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_toc32853064"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_row29866445"><th class="cellrowborder" valign="top" width="13.07%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p3262944"><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p3262944"></a><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p3262944"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="86.92999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p62971949"><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p62971949"></a><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p62971949"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_row454234"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p36792975"><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p36792975"></a><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p36792975"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p1390045223217"><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p1390045223217"></a><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p1390045223217"></a>待压缩图片的文件夹路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_row45642530"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p6057427"><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p6057427"></a><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_zh-cn_topic_0279615208_p6057427"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p135441240183510"><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p135441240183510"></a><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p135441240183510"></a>压缩完成的二进制和头文件的输出文件夹路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_row7715326315"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_p18783213316"><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_p18783213316"></a><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_p18783213316"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_p3743213313"><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_p3743213313"></a><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_p3743213313"></a>压缩算法选择按钮。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_row139646576421"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p5964105710420"><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p5964105710420"></a><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p5964105710420"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p496435754210"><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p496435754210"></a><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p496435754210"></a>压缩开始按钮。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_row1482132153810"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p1283526382"><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p1283526382"></a><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p1283526382"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p111251123505"><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p111251123505"></a><a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_p111251123505"></a>配置压缩参数。</p>
</td>
</tr>
</tbody>
</table>

FB算法图片压缩功能的步骤如下：

1.  点击图片路径选择按钮“...” 选择需要压缩的图片的路径。
2.  点击“Setting”按钮，显示如下所示界面，配置压缩的条件。

    **图 2**  压缩参数选择<a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_fig1464814282216"></a>  
    ![](figures/压缩参数选择-4.png "压缩参数选择-4")

3.  点击“Compress”按钮，开始进行压缩。
4.  <a name="zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_li684024916523"></a>点击右上角Pre或者Next选择合适的结果，点击OK确定结果。
5.  重复[4](#zh-cn_topic_0000002422495582_zh-cn_topic_0000002380811560_zh-cn_topic_0000001249568898_li684024916523)直到流程结束。

>![](public_sys-resources/icon-note.gif) **说明：** 
>-   输入文件支持ARBG888、RGB888、RGB565三种格式，输出文件和输入文件的格式相同。输入文件的像素大小应小于1280×800。
>-   参数说明
>    -   FullPreview：1--查看每张图的压缩结果，每完成一张图片压缩并选择合适的结果后开始下一张的压缩流程，直到所有的图片压缩完成；0--不查看压缩结果。
>    -   Compress Ratio：压缩倍率选择：2--2倍压缩，3--3倍压缩

#### 对比功能<a name="ZH-CN_TOPIC_0000002499501749"></a>

**图 1**  对比效果图<a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_fig19661351523"></a>  
![](figures/对比效果图.png "对比效果图")

**表 1**  对比界面说明

<a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_toc32853064"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_row29866445"><th class="cellrowborder" valign="top" width="13.07%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_p3262944"><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_p3262944"></a><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_p3262944"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="86.92999999999999%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_p62971949"><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_p62971949"></a><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_p62971949"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_row454234"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_p36792975"><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_p36792975"></a><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_p36792975"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_p166391945113316"><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_p166391945113316"></a><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_p166391945113316"></a>像素差值容忍度。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_row45642530"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_p6057427"><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_p6057427"></a><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_zh-cn_topic_0279615208_p6057427"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_p1095616403146"><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_p1095616403146"></a><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_p1095616403146"></a>两张图片小范围像素级对比显示。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_row139646576421"><td class="cellrowborder" valign="top" width="13.07%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_p5964105710420"><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_p5964105710420"></a><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_p5964105710420"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="86.92999999999999%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_p496435754210"><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_p496435754210"></a><a name="zh-cn_topic_0000002422655482_zh-cn_topic_0000001713048669_zh-cn_topic_0000001296087345_p496435754210"></a>两张图片整体对比展示。</p>
</td>
</tr>
</tbody>
</table>

图片对比功能的步骤如下：

1.  点击右键打开菜单，点击“compare”按钮开始对比。
2.  将鼠标放置在图片上，按住Ctrl同时滚动鼠标滚轮，调整整体效果的图片大小。
3.  拖动容差滑块，调整选取颜色时所设置的选取范围。

>![](public_sys-resources/icon-note.gif) **说明：** 
>两张图片不同的地方会显示为红色，拖动容差后，在容差范围内的色块会显示为深蓝色。

### 音频质量界面<a name="ZH-CN_TOPIC_0000002499581829"></a>

音频质量界面用于设置音频音效相关的参数，并将其下发到单板、写入bin文件或保存到NV。

-   **[3A参数界面](#ZH-CN_TOPIC_0000002499581765)**  

#### 3A参数界面<a name="ZH-CN_TOPIC_0000002499581765"></a>

3A参数界面用于设置音频相关的参数，并将其下发到单板。注意需让设备先进入蓝牙通话业务。

**图 1**  3A参数界面<a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_fig137816404814"></a>  

![](figures/ScreenShot_20251222212339-5.png)

**表 1**  3A参数下发界面说明

<a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_table1454610018237"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_row25478015236"><th class="cellrowborder" valign="top" width="18.17%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p1354710202311"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p1354710202311"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p1354710202311"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="81.83%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p145471806235"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p145471806235"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p145471806235"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_row854720092317"><td class="cellrowborder" valign="top" width="18.17%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p155475042311"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p155475042311"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p155475042311"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="81.83%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p16547180142312"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p16547180142312"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p16547180142312"></a>bin文件路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_row105478010237"><td class="cellrowborder" valign="top" width="18.17%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p654714014236"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p654714014236"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p654714014236"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="81.83%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p115477002311"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p115477002311"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p115477002311"></a>选择bin文件。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_row125472082316"><td class="cellrowborder" valign="top" width="18.17%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p6547180152312"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p6547180152312"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p6547180152312"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="81.83%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p1354740162316"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p1354740162316"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p1354740162316"></a>导出到bin文件中。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_row155477014237"><td class="cellrowborder" valign="top" width="18.17%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p1754710082316"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p1754710082316"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p1754710082316"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="81.83%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p754710102314"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p754710102314"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001630796581_p754710102314"></a>保存到NV。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_row52772417114"><td class="cellrowborder" valign="top" width="18.17%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p427824161111"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p427824161111"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p427824161111"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="81.83%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p142781749115"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p142781749115"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p142781749115"></a>显示二进制数据。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_row1685729173611"><td class="cellrowborder" valign="top" width="18.17%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p185229103610"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p185229103610"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p185229103610"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="81.83%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p98512903614"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p98512903614"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p98512903614"></a>参数页。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_row185112910367"><td class="cellrowborder" valign="top" width="18.17%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p108572993610"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p108572993610"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p108572993610"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="81.83%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p685329113611"><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p685329113611"></a><a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_p685329113611"></a>读写参数。</p>
</td>
</tr>
</tbody>
</table>

选择Bin文件并读取的步骤：

1.  点击“Browser”选择文件，并显示出选择文件界面。如[图2](#zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001580973000_fig183273113715)所示。

    **图 2**  选择bin文件界面<a name="zh-cn_topic_0000002456054393_zh-cn_topic_0000001778653077_zh-cn_topic_0000001580973000_fig183273113715"></a>  
    ![](figures/选择bin文件界面.png "选择bin文件界面")

2.  选择一个bin文件。将bin文件数据显示在界面中。

写入Bin文件的步骤：

1.  点击“Browser”选择文件。
2.  点击“Export”把参数导出到bin文件。

保存数据到NV的步骤：

1.  在参数页设置参数。
2.  点击“Save”按钮把参数保存到NV。

显示二进制数据的步骤：

1.  在参数页设置参数。
2.  点击“Binary”按钮显示3A算法的二进制数据。

### 提示音工具<a name="ZH-CN_TOPIC_0000002466542452"></a>

提示音工具用于将.wav文件转换为.sbc文件。

**图 1**  提示音工具界面<a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_fig1928016510266"></a>  
![](figures/提示音工具界面.png "提示音工具界面")

**表 1**  提示音工具界面说明

<a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_table12399332111610"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_row23991732151617"><th class="cellrowborder" valign="top" width="16.580000000000002%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p1639923291612"><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p1639923291612"></a><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p1639923291612"></a>标号</p>
</th>
<th class="cellrowborder" valign="top" width="83.42%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p199034394176"><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p199034394176"></a><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p199034394176"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_row15399153291618"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p1399732161618"><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p1399732161618"></a><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p1399732161618"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p495255512170"><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p495255512170"></a><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p495255512170"></a>输入文件路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_row439913218165"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p133991032101612"><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p133991032101612"></a><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p133991032101612"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p87308145221"><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p87308145221"></a><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p87308145221"></a>输出路径。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_row19399163251615"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p3399163251616"><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p3399163251616"></a><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p3399163251616"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p7399143241611"><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p7399143241611"></a><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p7399143241611"></a>解析按钮。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_row1339983218161"><td class="cellrowborder" valign="top" width="16.580000000000002%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p139923291615"><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p139923291615"></a><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p139923291615"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="83.42%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p1771986218"><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p1771986218"></a><a name="zh-cn_topic_0000002422495586_zh-cn_topic_0000001665168858_zh-cn_topic_0000001610622677_p1771986218"></a>解析结果。</p>
</td>
</tr>
</tbody>
</table>

多语言脚本调用步骤如下：

1.  分别选择输入路径和输出路径（输出路径为空时默认存到输入路径下）。
2.  点击“Parse”按钮，结果和输出路径展示在白框中。

>![](public_sys-resources/icon-note.gif) **说明：** 
>提示音输入的wav文件需要采样率固定为16k，位宽16bit，声道数1声道。

### 打点文件解析<a name="ZH-CN_TOPIC_0000002466542476"></a>

打点文件解析界面是用来解析打点文件，切换设置了打点文件解析界面配置的芯片，点击system插件，会出现展示该界面的选项Mark File，点击该选项后，右侧会出现打点文件解析界面。

**图 1**  打点文件解析界面<a name="zh-cn_topic_0000002422655486_zh-cn_topic_0000002416806700_fig33221448112719"></a>  
![](figures/打点文件解析界面.png "打点文件解析界面")

使用步骤：点击Browser，打开打点文件的以bin.idx为后缀的文件路径，例如“log\_file\_chr.bin.idx”，即可在下方看到解析后的文件内容。

# FAQ<a name="ZH-CN_TOPIC_0000002499501785"></a>

-   **[命令无响应或提示超时](#ZH-CN_TOPIC_0000002466382792)**  

## 命令无响应或提示超时<a name="ZH-CN_TOPIC_0000002466382792"></a>

**问题描述<a name="zh-cn_topic_0000002456054397_zh-cn_topic_0000001829992601_zh-cn_topic_0279549084_section11476770"></a>**

连接成功，但是所有命令无响应或者提示超时。

**解决办法<a name="zh-cn_topic_0000002456054397_zh-cn_topic_0000001829992601_section64551741862"></a>**

1.  排查连接的串口是否确认为Debugkits使用的串口而非其他业务占用的串口。
2.  排查串口连接方式正常，无连接松动或少接线。
3.  排查设备运行状态，确保处于正常运行状态，而非低功耗状态，睡眠状态或死机。

