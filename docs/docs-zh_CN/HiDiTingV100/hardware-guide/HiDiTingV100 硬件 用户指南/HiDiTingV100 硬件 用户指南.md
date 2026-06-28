# 前言<a name="ZH-CN_TOPIC_0000002545918495"></a>

**概述<a name="title1221mcpsimp"></a>**

本文档提供HiDiTingV100芯片管脚列表、单板设计指导及关键器件选型指导建议，对客户如何成功实现芯片板级设计给出指导。

**读者对象<a name="title1224mcpsimp"></a>**

本文档主要适用于以下工程师：

-   技术支持工程师
-   产品硬件开发工程师
-   PCB设计工程师

**符号约定<a name="section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="table2622507016410"></a>
<table><thead align="left"><tr id="row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="p6450074116410"><a name="p6450074116410"></a><a name="p6450074116410"></a><strong id="b2136615816410"><a name="b2136615816410"></a><a name="b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="p5435366816410"><a name="p5435366816410"></a><a name="p5435366816410"></a><strong id="b5941558116410"><a name="b5941558116410"></a><a name="b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p3734547016410"><a name="p3734547016410"></a><a name="p3734547016410"></a><a name="image2670064316410"></a><a name="image2670064316410"></a><span><img class="" id="image2670064316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002514318584.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p1757432116410"><a name="p1757432116410"></a><a name="p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1432579516410"><a name="p1432579516410"></a><a name="p1432579516410"></a><a name="image4895582316410"></a><a name="image4895582316410"></a><span><img class="" id="image4895582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002545918521.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p959197916410"><a name="p959197916410"></a><a name="p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1232579516410"><a name="p1232579516410"></a><a name="p1232579516410"></a><a name="image1235582316410"></a><a name="image1235582316410"></a><span><img class="" id="image1235582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002514158658.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p123197916410"><a name="p123197916410"></a><a name="p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p2204984716410"><a name="p2204984716410"></a><a name="p2204984716410"></a><a name="image4504446716410"></a><a name="image4504446716410"></a><span><img class="" id="image4504446716410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002545798543.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4388861916410"><a name="p4388861916410"></a><a name="p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="p1238861916410"><a name="p1238861916410"></a><a name="p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p5555360116410"><a name="p5555360116410"></a><a name="p5555360116410"></a><a name="image799324016410"></a><a name="image799324016410"></a><span><img class="" id="image799324016410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002514318598.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4612588116410"><a name="p4612588116410"></a><a name="p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="p1232588116410"><a name="p1232588116410"></a><a name="p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# 模组封装与管脚定义<a name="ZH-CN_TOPIC_0000002545901573"></a>

-   **[模组封装](#ZH-CN_TOPIC_0000002514301630)**  

-   **[管脚分布图](#ZH-CN_TOPIC_0000002545781583)**  

-   **[管脚类型说明](#ZH-CN_TOPIC_0000002514141708)**  

-   **[管脚数量统计](#ZH-CN_TOPIC_0000002545901577)**  

-   **[管脚描述](#ZH-CN_TOPIC_0000002514301632)**  

## 模组封装<a name="ZH-CN_TOPIC_0000002514301630"></a>

模组封装如[图1](#p1179mcpsimp)、[图2](#p1181mcpsimp)、[图3](#p1183mcpsimp)所示。

**图 1**  模组封装俯视及侧视图<a name="p1179mcpsimp"></a>  
![](figures/模组封装俯视及侧视图.png "模组封装俯视及侧视图")

**图 2**  模组封装底视图<a name="p1181mcpsimp"></a>  
![](figures/模组封装底视图.png "模组封装底视图")

**图 3**  模组推荐封装尺寸<a name="p1183mcpsimp"></a>  
![](figures/模组推荐封装尺寸.png "模组推荐封装尺寸")

## 管脚分布图<a name="ZH-CN_TOPIC_0000002545781583"></a>

HiDiTingV100管脚分布如[图1](#_fig13815123319534)所示。

**图 1**  管脚分布图（视图方向：TOP VIEW，即从封装的顶部往下看）<a name="_fig13815123319534"></a>  

![](figures/zh-cn_image_0000002547177151.png)

## 管脚类型说明<a name="ZH-CN_TOPIC_0000002514141708"></a>

管脚I/O类型和电平类型如[表1](#__Toc470247167)所示。

**表 1**  管脚I/O类型和电平类型说明

<a name="__Toc470247167"></a>
<table><thead align="left"><tr id="row2127mcpsimp"><th class="cellrowborder" valign="top" width="28.999999999999996%" id="mcps1.2.3.1.1"><p id="p2129mcpsimp"><a name="p2129mcpsimp"></a><a name="p2129mcpsimp"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="71%" id="mcps1.2.3.1.2"><p id="p2131mcpsimp"><a name="p2131mcpsimp"></a><a name="p2131mcpsimp"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row2133mcpsimp"><td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.3.1.1 "><p id="p2135mcpsimp"><a name="p2135mcpsimp"></a><a name="p2135mcpsimp"></a>I</p>
</td>
<td class="cellrowborder" valign="top" width="71%" headers="mcps1.2.3.1.2 "><p id="p2137mcpsimp"><a name="p2137mcpsimp"></a><a name="p2137mcpsimp"></a>数字管脚，普通输入端口。</p>
</td>
</tr>
<tr id="row2138mcpsimp"><td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.3.1.1 "><p id="p2140mcpsimp"><a name="p2140mcpsimp"></a><a name="p2140mcpsimp"></a>O</p>
</td>
<td class="cellrowborder" valign="top" width="71%" headers="mcps1.2.3.1.2 "><p id="p2142mcpsimp"><a name="p2142mcpsimp"></a><a name="p2142mcpsimp"></a>数字管脚，普通输出端口。</p>
</td>
</tr>
<tr id="row2143mcpsimp"><td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.3.1.1 "><p id="p2145mcpsimp"><a name="p2145mcpsimp"></a><a name="p2145mcpsimp"></a>I<sub id="sub2146mcpsimp"><a name="sub2146mcpsimp"></a><a name="sub2146mcpsimp"></a>PU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="71%" headers="mcps1.2.3.1.2 "><p id="p2148mcpsimp"><a name="p2148mcpsimp"></a><a name="p2148mcpsimp"></a>数字管脚，带有内部上拉的输入端口。</p>
</td>
</tr>
<tr id="row2149mcpsimp"><td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.3.1.1 "><p id="p2151mcpsimp"><a name="p2151mcpsimp"></a><a name="p2151mcpsimp"></a>I<sub id="sub2152mcpsimp"><a name="sub2152mcpsimp"></a><a name="sub2152mcpsimp"></a>PD</sub></p>
</td>
<td class="cellrowborder" valign="top" width="71%" headers="mcps1.2.3.1.2 "><p id="p2154mcpsimp"><a name="p2154mcpsimp"></a><a name="p2154mcpsimp"></a>数字管脚，带有内部下拉的输入端口。</p>
</td>
</tr>
<tr id="row2155mcpsimp"><td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.3.1.1 "><p id="p2157mcpsimp"><a name="p2157mcpsimp"></a><a name="p2157mcpsimp"></a>I<sub id="sub2158mcpsimp"><a name="sub2158mcpsimp"></a><a name="sub2158mcpsimp"></a>ANA</sub></p>
</td>
<td class="cellrowborder" valign="top" width="71%" headers="mcps1.2.3.1.2 "><p id="p2160mcpsimp"><a name="p2160mcpsimp"></a><a name="p2160mcpsimp"></a>模拟管脚，输入端口。</p>
</td>
</tr>
<tr id="row2161mcpsimp"><td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.3.1.1 "><p id="p2163mcpsimp"><a name="p2163mcpsimp"></a><a name="p2163mcpsimp"></a>O<sub id="sub2164mcpsimp"><a name="sub2164mcpsimp"></a><a name="sub2164mcpsimp"></a>ANA</sub></p>
</td>
<td class="cellrowborder" valign="top" width="71%" headers="mcps1.2.3.1.2 "><p id="p2166mcpsimp"><a name="p2166mcpsimp"></a><a name="p2166mcpsimp"></a>模拟管脚，输出端口。</p>
</td>
</tr>
<tr id="row2167mcpsimp"><td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.3.1.1 "><p id="p2169mcpsimp"><a name="p2169mcpsimp"></a><a name="p2169mcpsimp"></a>IO<sub id="sub2170mcpsimp"><a name="sub2170mcpsimp"></a><a name="sub2170mcpsimp"></a>RF</sub></p>
</td>
<td class="cellrowborder" valign="top" width="71%" headers="mcps1.2.3.1.2 "><p id="p2172mcpsimp"><a name="p2172mcpsimp"></a><a name="p2172mcpsimp"></a>RF管脚，输入输出端口。</p>
</td>
</tr>
<tr id="row2173mcpsimp"><td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.3.1.1 "><p id="p2175mcpsimp"><a name="p2175mcpsimp"></a><a name="p2175mcpsimp"></a>I<sub id="sub2176mcpsimp"><a name="sub2176mcpsimp"></a><a name="sub2176mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="71%" headers="mcps1.2.3.1.2 "><p id="p2178mcpsimp"><a name="p2178mcpsimp"></a><a name="p2178mcpsimp"></a>电源管脚，输入端口。</p>
</td>
</tr>
<tr id="row2179mcpsimp"><td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.3.1.1 "><p id="p2181mcpsimp"><a name="p2181mcpsimp"></a><a name="p2181mcpsimp"></a>O<sub id="sub2182mcpsimp"><a name="sub2182mcpsimp"></a><a name="sub2182mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="71%" headers="mcps1.2.3.1.2 "><p id="p2184mcpsimp"><a name="p2184mcpsimp"></a><a name="p2184mcpsimp"></a>电源管脚，输出端口。</p>
</td>
</tr>
</tbody>
</table>

## 管脚数量统计<a name="ZH-CN_TOPIC_0000002545901577"></a>

各功能管脚数量信息如[表1](#__Toc139708563)所示。

**表 1**  管脚数量统计

<a name="__Toc139708563"></a>
<table><thead align="left"><tr id="row3618mcpsimp"><th class="cellrowborder" valign="top" width="44.24%" id="mcps1.2.3.1.1"><p id="p3620mcpsimp"><a name="p3620mcpsimp"></a><a name="p3620mcpsimp"></a>管脚类别</p>
</th>
<th class="cellrowborder" valign="top" width="55.76%" id="mcps1.2.3.1.2"><p id="p3622mcpsimp"><a name="p3622mcpsimp"></a><a name="p3622mcpsimp"></a>数量</p>
</th>
</tr>
</thead>
<tbody><tr id="row3624mcpsimp"><td class="cellrowborder" valign="top" width="44.24%" headers="mcps1.2.3.1.1 "><p id="p3626mcpsimp"><a name="p3626mcpsimp"></a><a name="p3626mcpsimp"></a>RFIO接口</p>
</td>
<td class="cellrowborder" valign="top" width="55.76%" headers="mcps1.2.3.1.2 "><p id="p3628mcpsimp"><a name="p3628mcpsimp"></a><a name="p3628mcpsimp"></a>2</p>
</td>
</tr>
<tr id="row3629mcpsimp"><td class="cellrowborder" valign="top" width="44.24%" headers="mcps1.2.3.1.1 "><p id="p3631mcpsimp"><a name="p3631mcpsimp"></a><a name="p3631mcpsimp"></a>电源</p>
</td>
<td class="cellrowborder" valign="top" width="55.76%" headers="mcps1.2.3.1.2 "><p id="p3633mcpsimp"><a name="p3633mcpsimp"></a><a name="p3633mcpsimp"></a>13</p>
</td>
</tr>
<tr id="row3634mcpsimp"><td class="cellrowborder" valign="top" width="44.24%" headers="mcps1.2.3.1.1 "><p id="p3636mcpsimp"><a name="p3636mcpsimp"></a><a name="p3636mcpsimp"></a>音频信号</p>
</td>
<td class="cellrowborder" valign="top" width="55.76%" headers="mcps1.2.3.1.2 "><p id="p3638mcpsimp"><a name="p3638mcpsimp"></a><a name="p3638mcpsimp"></a>4</p>
</td>
</tr>
<tr id="row3639mcpsimp"><td class="cellrowborder" valign="top" width="44.24%" headers="mcps1.2.3.1.1 "><p id="p3641mcpsimp"><a name="p3641mcpsimp"></a><a name="p3641mcpsimp"></a>MIPI信号</p>
</td>
<td class="cellrowborder" valign="top" width="55.76%" headers="mcps1.2.3.1.2 "><p id="p3643mcpsimp"><a name="p3643mcpsimp"></a><a name="p3643mcpsimp"></a>4</p>
</td>
</tr>
<tr id="row3644mcpsimp"><td class="cellrowborder" valign="top" width="44.24%" headers="mcps1.2.3.1.1 "><p id="p3646mcpsimp"><a name="p3646mcpsimp"></a><a name="p3646mcpsimp"></a>AUXADC接口</p>
</td>
<td class="cellrowborder" valign="top" width="55.76%" headers="mcps1.2.3.1.2 "><p id="p3648mcpsimp"><a name="p3648mcpsimp"></a><a name="p3648mcpsimp"></a>8，其中4个与GPIO复用</p>
</td>
</tr>
<tr id="row3649mcpsimp"><td class="cellrowborder" valign="top" width="44.24%" headers="mcps1.2.3.1.1 "><p id="p3651mcpsimp"><a name="p3651mcpsimp"></a><a name="p3651mcpsimp"></a>USB接口</p>
</td>
<td class="cellrowborder" valign="top" width="55.76%" headers="mcps1.2.3.1.2 "><p id="p3653mcpsimp"><a name="p3653mcpsimp"></a><a name="p3653mcpsimp"></a>6</p>
</td>
</tr>
<tr id="row3654mcpsimp"><td class="cellrowborder" valign="top" width="44.24%" headers="mcps1.2.3.1.1 "><p id="p3656mcpsimp"><a name="p3656mcpsimp"></a><a name="p3656mcpsimp"></a>USIM接口</p>
</td>
<td class="cellrowborder" valign="top" width="55.76%" headers="mcps1.2.3.1.2 "><p id="p3658mcpsimp"><a name="p3658mcpsimp"></a><a name="p3658mcpsimp"></a>4</p>
</td>
</tr>
<tr id="row3659mcpsimp"><td class="cellrowborder" valign="top" width="44.24%" headers="mcps1.2.3.1.1 "><p id="p3661mcpsimp"><a name="p3661mcpsimp"></a><a name="p3661mcpsimp"></a>上电控制信号</p>
</td>
<td class="cellrowborder" valign="top" width="55.76%" headers="mcps1.2.3.1.2 "><p id="p3663mcpsimp"><a name="p3663mcpsimp"></a><a name="p3663mcpsimp"></a>2</p>
</td>
</tr>
<tr id="row3664mcpsimp"><td class="cellrowborder" valign="top" width="44.24%" headers="mcps1.2.3.1.1 "><p id="p3666mcpsimp"><a name="p3666mcpsimp"></a><a name="p3666mcpsimp"></a>GND</p>
</td>
<td class="cellrowborder" valign="top" width="55.76%" headers="mcps1.2.3.1.2 "><p id="p3668mcpsimp"><a name="p3668mcpsimp"></a><a name="p3668mcpsimp"></a>30</p>
</td>
</tr>
<tr id="row3669mcpsimp"><td class="cellrowborder" valign="top" width="44.24%" headers="mcps1.2.3.1.1 "><p id="p3671mcpsimp"><a name="p3671mcpsimp"></a><a name="p3671mcpsimp"></a>数字IO</p>
</td>
<td class="cellrowborder" valign="top" width="55.76%" headers="mcps1.2.3.1.2 "><p id="p3673mcpsimp"><a name="p3673mcpsimp"></a><a name="p3673mcpsimp"></a>70</p>
</td>
</tr>
<tr id="row3674mcpsimp"><td class="cellrowborder" valign="top" width="44.24%" headers="mcps1.2.3.1.1 "><p id="p3676mcpsimp"><a name="p3676mcpsimp"></a><a name="p3676mcpsimp"></a>维测管脚</p>
</td>
<td class="cellrowborder" valign="top" width="55.76%" headers="mcps1.2.3.1.2 "><p id="p3678mcpsimp"><a name="p3678mcpsimp"></a><a name="p3678mcpsimp"></a>5</p>
</td>
</tr>
<tr id="row3679mcpsimp"><td class="cellrowborder" valign="top" width="44.24%" headers="mcps1.2.3.1.1 "><p id="p3681mcpsimp"><a name="p3681mcpsimp"></a><a name="p3681mcpsimp"></a>总计</p>
</td>
<td class="cellrowborder" valign="top" width="55.76%" headers="mcps1.2.3.1.2 "><p id="p3683mcpsimp"><a name="p3683mcpsimp"></a><a name="p3683mcpsimp"></a>144</p>
</td>
</tr>
</tbody>
</table>

## 管脚描述<a name="ZH-CN_TOPIC_0000002514301632"></a>

本小节描述HiDiTingV100的各个管脚，包括电源、RF、显示、音频、AUXADC、数字管脚、地等。

-   **[电源管脚](#ZH-CN_TOPIC_0000002545781587)**  

-   **[上电控制信号](#ZH-CN_TOPIC_0000002514141710)**  

-   **[RF接口](#ZH-CN_TOPIC_0000002545901579)**  

-   **[音频信号管脚](#ZH-CN_TOPIC_0000002514301636)**  

-   **[MIPI信号管脚](#ZH-CN_TOPIC_0000002545781589)**  

-   **[AUX ADC接口](#ZH-CN_TOPIC_0000002514141714)**  

-   **[USB接口](#ZH-CN_TOPIC_0000002545901583)**  

-   **[GND管脚](#ZH-CN_TOPIC_0000002514301638)**  

-   **[USIM管脚](#ZH-CN_TOPIC_0000002545781593)**  

-   **[数字管脚](#ZH-CN_TOPIC_0000002514141716)**  

-   **[维测管脚](#ZH-CN_TOPIC_0000002545901585)**  

### 电源管脚<a name="ZH-CN_TOPIC_0000002545781587"></a>

HiDiTingV100电源管脚如[表1](#__Ref438453666)所示。

**表 1**  电源管脚列表

<a name="__Ref438453666"></a>
<table><thead align="left"><tr id="row2556mcpsimp"><th class="cellrowborder" valign="top" width="13.200000000000001%" id="mcps1.2.6.1.1"><p id="p2558mcpsimp"><a name="p2558mcpsimp"></a><a name="p2558mcpsimp"></a>管脚编号</p>
</th>
<th class="cellrowborder" valign="top" width="24.529999999999998%" id="mcps1.2.6.1.2"><p id="p2560mcpsimp"><a name="p2560mcpsimp"></a><a name="p2560mcpsimp"></a>管脚名称</p>
</th>
<th class="cellrowborder" valign="top" width="11.25%" id="mcps1.2.6.1.3"><p id="p2562mcpsimp"><a name="p2562mcpsimp"></a><a name="p2562mcpsimp"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="34.04%" id="mcps1.2.6.1.4"><p id="p2564mcpsimp"><a name="p2564mcpsimp"></a><a name="p2564mcpsimp"></a>管脚说明</p>
</th>
<th class="cellrowborder" valign="top" width="16.98%" id="mcps1.2.6.1.5"><p id="p2566mcpsimp"><a name="p2566mcpsimp"></a><a name="p2566mcpsimp"></a>电压（典型值）</p>
</th>
</tr>
</thead>
<tbody><tr id="row2568mcpsimp"><td class="cellrowborder" valign="top" width="13.200000000000001%" headers="mcps1.2.6.1.1 "><p id="p2570mcpsimp"><a name="p2570mcpsimp"></a><a name="p2570mcpsimp"></a>58</p>
</td>
<td class="cellrowborder" valign="top" width="24.529999999999998%" headers="mcps1.2.6.1.2 "><p id="p2572mcpsimp"><a name="p2572mcpsimp"></a><a name="p2572mcpsimp"></a>VSYS</p>
</td>
<td class="cellrowborder" valign="top" width="11.25%" headers="mcps1.2.6.1.3 "><p id="p2574mcpsimp"><a name="p2574mcpsimp"></a><a name="p2574mcpsimp"></a>I<sub id="sub2575mcpsimp"><a name="sub2575mcpsimp"></a><a name="sub2575mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="34.04%" headers="mcps1.2.6.1.4 "><p id="p2577mcpsimp"><a name="p2577mcpsimp"></a><a name="p2577mcpsimp"></a>板级接charger后级，主控芯片电源输入。</p>
</td>
<td class="cellrowborder" valign="top" width="16.98%" headers="mcps1.2.6.1.5 "><p id="p2579mcpsimp"><a name="p2579mcpsimp"></a><a name="p2579mcpsimp"></a>3.8V</p>
</td>
</tr>
<tr id="row2580mcpsimp"><td class="cellrowborder" valign="top" width="13.200000000000001%" headers="mcps1.2.6.1.1 "><p id="p2582mcpsimp"><a name="p2582mcpsimp"></a><a name="p2582mcpsimp"></a>57</p>
</td>
<td class="cellrowborder" valign="top" width="24.529999999999998%" headers="mcps1.2.6.1.2 "><p id="p2584mcpsimp"><a name="p2584mcpsimp"></a><a name="p2584mcpsimp"></a>VDD_PMU_1P8_SW0</p>
</td>
<td class="cellrowborder" valign="top" width="11.25%" headers="mcps1.2.6.1.3 "><p id="p2586mcpsimp"><a name="p2586mcpsimp"></a><a name="p2586mcpsimp"></a>O<sub id="sub2587mcpsimp"><a name="sub2587mcpsimp"></a><a name="sub2587mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="34.04%" headers="mcps1.2.6.1.4 "><p id="p2589mcpsimp"><a name="p2589mcpsimp"></a><a name="p2589mcpsimp"></a>主控芯片1.8V switch输出。</p>
<p id="p2590mcpsimp"><a name="p2590mcpsimp"></a><a name="p2590mcpsimp"></a>用于Nand Flash供电。</p>
</td>
<td class="cellrowborder" valign="top" width="16.98%" headers="mcps1.2.6.1.5 "><p id="p2592mcpsimp"><a name="p2592mcpsimp"></a><a name="p2592mcpsimp"></a>1.8V</p>
</td>
</tr>
<tr id="row2593mcpsimp"><td class="cellrowborder" valign="top" width="13.200000000000001%" headers="mcps1.2.6.1.1 "><p id="p2595mcpsimp"><a name="p2595mcpsimp"></a><a name="p2595mcpsimp"></a>85</p>
</td>
<td class="cellrowborder" valign="top" width="24.529999999999998%" headers="mcps1.2.6.1.2 "><p id="p2597mcpsimp"><a name="p2597mcpsimp"></a><a name="p2597mcpsimp"></a>VDD_PMU_MICLDO0</p>
</td>
<td class="cellrowborder" valign="top" width="11.25%" headers="mcps1.2.6.1.3 "><p id="p2599mcpsimp"><a name="p2599mcpsimp"></a><a name="p2599mcpsimp"></a>O<sub id="sub2600mcpsimp"><a name="sub2600mcpsimp"></a><a name="sub2600mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="34.04%" headers="mcps1.2.6.1.4 "><p id="p2602mcpsimp"><a name="p2602mcpsimp"></a><a name="p2602mcpsimp"></a>MICBIAS供电，电压0.9/1.2/1.8 V。</p>
</td>
<td class="cellrowborder" valign="top" width="16.98%" headers="mcps1.2.6.1.5 "><p id="p2604mcpsimp"><a name="p2604mcpsimp"></a><a name="p2604mcpsimp"></a>1.8V</p>
</td>
</tr>
<tr id="row2605mcpsimp"><td class="cellrowborder" valign="top" width="13.200000000000001%" headers="mcps1.2.6.1.1 "><p id="p2607mcpsimp"><a name="p2607mcpsimp"></a><a name="p2607mcpsimp"></a>59</p>
</td>
<td class="cellrowborder" valign="top" width="24.529999999999998%" headers="mcps1.2.6.1.2 "><p id="p2609mcpsimp"><a name="p2609mcpsimp"></a><a name="p2609mcpsimp"></a>VDD_PMU_IOLDO2</p>
</td>
<td class="cellrowborder" valign="top" width="11.25%" headers="mcps1.2.6.1.3 "><p id="p2611mcpsimp"><a name="p2611mcpsimp"></a><a name="p2611mcpsimp"></a>O<sub id="sub2612mcpsimp"><a name="sub2612mcpsimp"></a><a name="sub2612mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="34.04%" headers="mcps1.2.6.1.4 "><p id="p2614mcpsimp"><a name="p2614mcpsimp"></a><a name="p2614mcpsimp"></a>IOLDO2电源输出，AUXADC基准电源。</p>
</td>
<td class="cellrowborder" valign="top" width="16.98%" headers="mcps1.2.6.1.5 "><p id="p2616mcpsimp"><a name="p2616mcpsimp"></a><a name="p2616mcpsimp"></a>1.8V</p>
</td>
</tr>
<tr id="row2617mcpsimp"><td class="cellrowborder" valign="top" width="13.200000000000001%" headers="mcps1.2.6.1.1 "><p id="p2619mcpsimp"><a name="p2619mcpsimp"></a><a name="p2619mcpsimp"></a>63</p>
</td>
<td class="cellrowborder" valign="top" width="24.529999999999998%" headers="mcps1.2.6.1.2 "><p id="p2621mcpsimp"><a name="p2621mcpsimp"></a><a name="p2621mcpsimp"></a>VDD_PMU_IOLDO3</p>
</td>
<td class="cellrowborder" valign="top" width="11.25%" headers="mcps1.2.6.1.3 "><p id="p2623mcpsimp"><a name="p2623mcpsimp"></a><a name="p2623mcpsimp"></a>O<sub id="sub2624mcpsimp"><a name="sub2624mcpsimp"></a><a name="sub2624mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="34.04%" headers="mcps1.2.6.1.4 "><p id="p2626mcpsimp"><a name="p2626mcpsimp"></a><a name="p2626mcpsimp"></a>IOLDO3电源输出。</p>
</td>
<td class="cellrowborder" valign="top" width="16.98%" headers="mcps1.2.6.1.5 "><p id="p2628mcpsimp"><a name="p2628mcpsimp"></a><a name="p2628mcpsimp"></a>3.3V</p>
</td>
</tr>
<tr id="row2629mcpsimp"><td class="cellrowborder" valign="top" width="13.200000000000001%" headers="mcps1.2.6.1.1 "><p id="p2631mcpsimp"><a name="p2631mcpsimp"></a><a name="p2631mcpsimp"></a>56</p>
</td>
<td class="cellrowborder" valign="top" width="24.529999999999998%" headers="mcps1.2.6.1.2 "><p id="p2633mcpsimp"><a name="p2633mcpsimp"></a><a name="p2633mcpsimp"></a>VDD_PMU_IOLDO6</p>
</td>
<td class="cellrowborder" valign="top" width="11.25%" headers="mcps1.2.6.1.3 "><p id="p2635mcpsimp"><a name="p2635mcpsimp"></a><a name="p2635mcpsimp"></a>O<sub id="sub2636mcpsimp"><a name="sub2636mcpsimp"></a><a name="sub2636mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="34.04%" headers="mcps1.2.6.1.4 "><p id="p2638mcpsimp"><a name="p2638mcpsimp"></a><a name="p2638mcpsimp"></a>IOLDO6电源输出，用于Nor Flash供电。</p>
</td>
<td class="cellrowborder" valign="top" width="16.98%" headers="mcps1.2.6.1.5 "><p id="p2640mcpsimp"><a name="p2640mcpsimp"></a><a name="p2640mcpsimp"></a>1.8V</p>
</td>
</tr>
<tr id="row2641mcpsimp"><td class="cellrowborder" valign="top" width="13.200000000000001%" headers="mcps1.2.6.1.1 "><p id="p2643mcpsimp"><a name="p2643mcpsimp"></a><a name="p2643mcpsimp"></a>24</p>
</td>
<td class="cellrowborder" valign="top" width="24.529999999999998%" headers="mcps1.2.6.1.2 "><p id="p2645mcpsimp"><a name="p2645mcpsimp"></a><a name="p2645mcpsimp"></a>B4_VBAT_2871</p>
</td>
<td class="cellrowborder" valign="top" width="11.25%" headers="mcps1.2.6.1.3 "><p id="p2647mcpsimp"><a name="p2647mcpsimp"></a><a name="p2647mcpsimp"></a>I<sub id="sub2648mcpsimp"><a name="sub2648mcpsimp"></a><a name="sub2648mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="34.04%" headers="mcps1.2.6.1.4 "><p id="p2650mcpsimp"><a name="p2650mcpsimp"></a><a name="p2650mcpsimp"></a>GNSS芯片VSYS供电。</p>
</td>
<td class="cellrowborder" valign="top" width="16.98%" headers="mcps1.2.6.1.5 "><p id="p2652mcpsimp"><a name="p2652mcpsimp"></a><a name="p2652mcpsimp"></a>3.3V</p>
</td>
</tr>
<tr id="row2653mcpsimp"><td class="cellrowborder" valign="top" width="13.200000000000001%" headers="mcps1.2.6.1.1 "><p id="p2655mcpsimp"><a name="p2655mcpsimp"></a><a name="p2655mcpsimp"></a>47</p>
</td>
<td class="cellrowborder" valign="top" width="24.529999999999998%" headers="mcps1.2.6.1.2 "><p id="p2657mcpsimp"><a name="p2657mcpsimp"></a><a name="p2657mcpsimp"></a>B4_VDD_1PB</p>
</td>
<td class="cellrowborder" valign="top" width="11.25%" headers="mcps1.2.6.1.3 "><p id="p2659mcpsimp"><a name="p2659mcpsimp"></a><a name="p2659mcpsimp"></a>I<sub id="sub2660mcpsimp"><a name="sub2660mcpsimp"></a><a name="sub2660mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="34.04%" headers="mcps1.2.6.1.4 "><p id="p2662mcpsimp"><a name="p2662mcpsimp"></a><a name="p2662mcpsimp"></a>GNSS芯片VDD18供电。</p>
</td>
<td class="cellrowborder" valign="top" width="16.98%" headers="mcps1.2.6.1.5 "><p id="p2664mcpsimp"><a name="p2664mcpsimp"></a><a name="p2664mcpsimp"></a>1.8V</p>
</td>
</tr>
<tr id="row2665mcpsimp"><td class="cellrowborder" valign="top" width="13.200000000000001%" headers="mcps1.2.6.1.1 "><p id="p2667mcpsimp"><a name="p2667mcpsimp"></a><a name="p2667mcpsimp"></a>12</p>
</td>
<td class="cellrowborder" valign="top" width="24.529999999999998%" headers="mcps1.2.6.1.2 "><p id="p2669mcpsimp"><a name="p2669mcpsimp"></a><a name="p2669mcpsimp"></a>VDDIO_B</p>
</td>
<td class="cellrowborder" valign="top" width="11.25%" headers="mcps1.2.6.1.3 "><p id="p2671mcpsimp"><a name="p2671mcpsimp"></a><a name="p2671mcpsimp"></a>O<sub id="sub2672mcpsimp"><a name="sub2672mcpsimp"></a><a name="sub2672mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="34.04%" headers="mcps1.2.6.1.4 "><p id="p2674mcpsimp"><a name="p2674mcpsimp"></a><a name="p2674mcpsimp"></a>USIM电源。</p>
</td>
<td class="cellrowborder" valign="top" width="16.98%" headers="mcps1.2.6.1.5 "><p id="p2676mcpsimp"><a name="p2676mcpsimp"></a><a name="p2676mcpsimp"></a>1.8V/3V</p>
</td>
</tr>
<tr id="row2677mcpsimp"><td class="cellrowborder" valign="top" width="13.200000000000001%" headers="mcps1.2.6.1.1 "><p id="p2679mcpsimp"><a name="p2679mcpsimp"></a><a name="p2679mcpsimp"></a>118</p>
</td>
<td class="cellrowborder" valign="top" width="24.529999999999998%" headers="mcps1.2.6.1.2 "><p id="p2681mcpsimp"><a name="p2681mcpsimp"></a><a name="p2681mcpsimp"></a>CAT1_VBAT1</p>
</td>
<td class="cellrowborder" valign="top" width="11.25%" headers="mcps1.2.6.1.3 "><p id="p2683mcpsimp"><a name="p2683mcpsimp"></a><a name="p2683mcpsimp"></a>I<sub id="sub2684mcpsimp"><a name="sub2684mcpsimp"></a><a name="sub2684mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="34.04%" headers="mcps1.2.6.1.4 "><p id="p2686mcpsimp"><a name="p2686mcpsimp"></a><a name="p2686mcpsimp"></a>CAT1 PA供电电源。</p>
</td>
<td class="cellrowborder" valign="top" width="16.98%" headers="mcps1.2.6.1.5 "><p id="p2688mcpsimp"><a name="p2688mcpsimp"></a><a name="p2688mcpsimp"></a>3.8V</p>
</td>
</tr>
<tr id="row2689mcpsimp"><td class="cellrowborder" valign="top" width="13.200000000000001%" headers="mcps1.2.6.1.1 "><p id="p2691mcpsimp"><a name="p2691mcpsimp"></a><a name="p2691mcpsimp"></a>119</p>
</td>
<td class="cellrowborder" valign="top" width="24.529999999999998%" headers="mcps1.2.6.1.2 "><p id="p2693mcpsimp"><a name="p2693mcpsimp"></a><a name="p2693mcpsimp"></a>CAT1_VBAT2</p>
</td>
<td class="cellrowborder" valign="top" width="11.25%" headers="mcps1.2.6.1.3 "><p id="p2695mcpsimp"><a name="p2695mcpsimp"></a><a name="p2695mcpsimp"></a>I<sub id="sub2696mcpsimp"><a name="sub2696mcpsimp"></a><a name="sub2696mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="34.04%" headers="mcps1.2.6.1.4 "><p id="p2698mcpsimp"><a name="p2698mcpsimp"></a><a name="p2698mcpsimp"></a>CAT1芯片供电电源。</p>
</td>
<td class="cellrowborder" valign="top" width="16.98%" headers="mcps1.2.6.1.5 "><p id="p2700mcpsimp"><a name="p2700mcpsimp"></a><a name="p2700mcpsimp"></a>3.8V</p>
</td>
</tr>
</tbody>
</table>

### 上电控制信号<a name="ZH-CN_TOPIC_0000002514141710"></a>

上电控制信号如[表1](#__Toc367870709)所示。

**表 1**  上电控制信号管脚列表

<a name="__Toc367870709"></a>
<table><thead align="left"><tr id="row2711mcpsimp"><th class="cellrowborder" valign="top" width="17.17171717171717%" id="mcps1.2.5.1.1"><p id="p2713mcpsimp"><a name="p2713mcpsimp"></a><a name="p2713mcpsimp"></a>管脚编号</p>
</th>
<th class="cellrowborder" valign="top" width="21.962196219621962%" id="mcps1.2.5.1.2"><p id="p2715mcpsimp"><a name="p2715mcpsimp"></a><a name="p2715mcpsimp"></a>管脚名称</p>
</th>
<th class="cellrowborder" valign="top" width="13.971397139713973%" id="mcps1.2.5.1.3"><p id="p2717mcpsimp"><a name="p2717mcpsimp"></a><a name="p2717mcpsimp"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="46.8946894689469%" id="mcps1.2.5.1.4"><p id="p2719mcpsimp"><a name="p2719mcpsimp"></a><a name="p2719mcpsimp"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row2721mcpsimp"><td class="cellrowborder" valign="top" width="17.17171717171717%" headers="mcps1.2.5.1.1 "><p id="p2723mcpsimp"><a name="p2723mcpsimp"></a><a name="p2723mcpsimp"></a>48</p>
</td>
<td class="cellrowborder" valign="top" width="21.962196219621962%" headers="mcps1.2.5.1.2 "><p id="p2725mcpsimp"><a name="p2725mcpsimp"></a><a name="p2725mcpsimp"></a>PWR_KEY</p>
</td>
<td class="cellrowborder" valign="top" width="13.971397139713973%" headers="mcps1.2.5.1.3 "><p id="p2727mcpsimp"><a name="p2727mcpsimp"></a><a name="p2727mcpsimp"></a>I<sub id="sub2728mcpsimp"><a name="sub2728mcpsimp"></a><a name="sub2728mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="46.8946894689469%" headers="mcps1.2.5.1.4 "><p id="p2730mcpsimp"><a name="p2730mcpsimp"></a><a name="p2730mcpsimp"></a>开关机按键，下降沿有效。</p>
</td>
</tr>
<tr id="row2731mcpsimp"><td class="cellrowborder" valign="top" width="17.17171717171717%" headers="mcps1.2.5.1.1 "><p id="p2733mcpsimp"><a name="p2733mcpsimp"></a><a name="p2733mcpsimp"></a>65</p>
</td>
<td class="cellrowborder" valign="top" width="21.962196219621962%" headers="mcps1.2.5.1.2 "><p id="p2735mcpsimp"><a name="p2735mcpsimp"></a><a name="p2735mcpsimp"></a>VBUS_SENSE</p>
</td>
<td class="cellrowborder" valign="top" width="13.971397139713973%" headers="mcps1.2.5.1.3 "><p id="p2737mcpsimp"><a name="p2737mcpsimp"></a><a name="p2737mcpsimp"></a>I<sub id="sub2738mcpsimp"><a name="sub2738mcpsimp"></a><a name="sub2738mcpsimp"></a>PMU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="46.8946894689469%" headers="mcps1.2.5.1.4 "><p id="p2740mcpsimp"><a name="p2740mcpsimp"></a><a name="p2740mcpsimp"></a>上电检测，充电开机专用管脚，高电平有效。</p>
<p id="p2741mcpsimp"><a name="p2741mcpsimp"></a><a name="p2741mcpsimp"></a>高电平有效范围：0.8V&lt;VBUS_SENSE&lt;1.8V。</p>
</td>
</tr>
</tbody>
</table>

>![](public_sys-resources/icon-note.gif) **说明：** 
>上述上电控制信号管脚属于专用管脚，不可复用GPIO。

### RF接口<a name="ZH-CN_TOPIC_0000002545901579"></a>

射频RF口如[表1](#__Toc367870719)所示。

**表 1**  RF接口管脚列表

<a name="__Toc367870719"></a>
<table><thead align="left"><tr id="row114mcpsimp"><th class="cellrowborder" valign="top" width="16.421642164216426%" id="mcps1.2.6.1.1"><p id="p116mcpsimp"><a name="p116mcpsimp"></a><a name="p116mcpsimp"></a>管脚编号</p>
</th>
<th class="cellrowborder" valign="top" width="20.072007200720073%" id="mcps1.2.6.1.2"><p id="p118mcpsimp"><a name="p118mcpsimp"></a><a name="p118mcpsimp"></a>管脚名称</p>
</th>
<th class="cellrowborder" valign="top" width="10.961096109610962%" id="mcps1.2.6.1.3"><p id="p120mcpsimp"><a name="p120mcpsimp"></a><a name="p120mcpsimp"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="14.281428142814281%" id="mcps1.2.6.1.4"><p id="p122mcpsimp"><a name="p122mcpsimp"></a><a name="p122mcpsimp"></a>电压（V）</p>
</th>
<th class="cellrowborder" valign="top" width="38.263826382638264%" id="mcps1.2.6.1.5"><p id="p124mcpsimp"><a name="p124mcpsimp"></a><a name="p124mcpsimp"></a>功能描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row126mcpsimp"><td class="cellrowborder" valign="top" width="16.421642164216426%" headers="mcps1.2.6.1.1 "><p id="p128mcpsimp"><a name="p128mcpsimp"></a><a name="p128mcpsimp"></a>39</p>
</td>
<td class="cellrowborder" valign="top" width="20.072007200720073%" headers="mcps1.2.6.1.2 "><p id="p130mcpsimp"><a name="p130mcpsimp"></a><a name="p130mcpsimp"></a>BT_GNSS_RFO</p>
</td>
<td class="cellrowborder" valign="top" width="10.961096109610962%" headers="mcps1.2.6.1.3 "><p id="p132mcpsimp"><a name="p132mcpsimp"></a><a name="p132mcpsimp"></a>IO<sub id="sub133mcpsimp"><a name="sub133mcpsimp"></a><a name="sub133mcpsimp"></a>RF</sub></p>
</td>
<td class="cellrowborder" valign="top" width="14.281428142814281%" headers="mcps1.2.6.1.4 "><p id="p135mcpsimp"><a name="p135mcpsimp"></a><a name="p135mcpsimp"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="38.263826382638264%" headers="mcps1.2.6.1.5 "><p id="p137mcpsimp"><a name="p137mcpsimp"></a><a name="p137mcpsimp"></a>BT和GNSS合路射频TX/RX接口。</p>
</td>
</tr>
<tr id="row138mcpsimp"><td class="cellrowborder" valign="top" width="16.421642164216426%" headers="mcps1.2.6.1.1 "><p id="p140mcpsimp"><a name="p140mcpsimp"></a><a name="p140mcpsimp"></a>99</p>
</td>
<td class="cellrowborder" valign="top" width="20.072007200720073%" headers="mcps1.2.6.1.2 "><p id="p142mcpsimp"><a name="p142mcpsimp"></a><a name="p142mcpsimp"></a>CAT1_RFO</p>
</td>
<td class="cellrowborder" valign="top" width="10.961096109610962%" headers="mcps1.2.6.1.3 "><p id="p144mcpsimp"><a name="p144mcpsimp"></a><a name="p144mcpsimp"></a>IO<sub id="sub145mcpsimp"><a name="sub145mcpsimp"></a><a name="sub145mcpsimp"></a>RF</sub></p>
</td>
<td class="cellrowborder" valign="top" width="14.281428142814281%" headers="mcps1.2.6.1.4 "><p id="p147mcpsimp"><a name="p147mcpsimp"></a><a name="p147mcpsimp"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="38.263826382638264%" headers="mcps1.2.6.1.5 "><p id="p149mcpsimp"><a name="p149mcpsimp"></a><a name="p149mcpsimp"></a>CAT_1射频接口。</p>
</td>
</tr>
</tbody>
</table>

### 音频信号管脚<a name="ZH-CN_TOPIC_0000002514301636"></a>

音频信号管脚如[表1](#__Ref54941678)所示。

模组支持1路模拟AMIC、1路模拟输出接口。

**表 1**  CODEC信号管脚列表

<a name="__Ref54941678"></a>
<table><thead align="left"><tr id="row2205mcpsimp"><th class="cellrowborder" valign="top" width="17.82178217821782%" id="mcps1.2.5.1.1"><p id="p2207mcpsimp"><a name="p2207mcpsimp"></a><a name="p2207mcpsimp"></a>管脚编号</p>
</th>
<th class="cellrowborder" valign="top" width="23.762376237623766%" id="mcps1.2.5.1.2"><p id="p2209mcpsimp"><a name="p2209mcpsimp"></a><a name="p2209mcpsimp"></a>管脚名称</p>
</th>
<th class="cellrowborder" valign="top" width="16.011601160116008%" id="mcps1.2.5.1.3"><p id="p2211mcpsimp"><a name="p2211mcpsimp"></a><a name="p2211mcpsimp"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="42.404240424042406%" id="mcps1.2.5.1.4"><p id="p2213mcpsimp"><a name="p2213mcpsimp"></a><a name="p2213mcpsimp"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row2215mcpsimp"><td class="cellrowborder" valign="top" width="17.82178217821782%" headers="mcps1.2.5.1.1 "><p id="p2217mcpsimp"><a name="p2217mcpsimp"></a><a name="p2217mcpsimp"></a>80</p>
</td>
<td class="cellrowborder" valign="top" width="23.762376237623766%" headers="mcps1.2.5.1.2 "><p id="p2219mcpsimp"><a name="p2219mcpsimp"></a><a name="p2219mcpsimp"></a>AMIC_P</p>
</td>
<td class="cellrowborder" valign="top" width="16.011601160116008%" headers="mcps1.2.5.1.3 "><p id="p2221mcpsimp"><a name="p2221mcpsimp"></a><a name="p2221mcpsimp"></a>I<sub id="sub2222mcpsimp"><a name="sub2222mcpsimp"></a><a name="sub2222mcpsimp"></a>ANA</sub></p>
</td>
<td class="cellrowborder" valign="top" width="42.404240424042406%" headers="mcps1.2.5.1.4 "><p id="p2224mcpsimp"><a name="p2224mcpsimp"></a><a name="p2224mcpsimp"></a>AMIC接口输入信号正。</p>
</td>
</tr>
<tr id="row2225mcpsimp"><td class="cellrowborder" valign="top" width="17.82178217821782%" headers="mcps1.2.5.1.1 "><p id="p2227mcpsimp"><a name="p2227mcpsimp"></a><a name="p2227mcpsimp"></a>78</p>
</td>
<td class="cellrowborder" valign="top" width="23.762376237623766%" headers="mcps1.2.5.1.2 "><p id="p2229mcpsimp"><a name="p2229mcpsimp"></a><a name="p2229mcpsimp"></a>AMIC_N</p>
</td>
<td class="cellrowborder" valign="top" width="16.011601160116008%" headers="mcps1.2.5.1.3 "><p id="p2231mcpsimp"><a name="p2231mcpsimp"></a><a name="p2231mcpsimp"></a>I<sub id="sub2232mcpsimp"><a name="sub2232mcpsimp"></a><a name="sub2232mcpsimp"></a>ANA</sub></p>
</td>
<td class="cellrowborder" valign="top" width="42.404240424042406%" headers="mcps1.2.5.1.4 "><p id="p2234mcpsimp"><a name="p2234mcpsimp"></a><a name="p2234mcpsimp"></a>AMIC接口输入信号负。</p>
</td>
</tr>
<tr id="row2235mcpsimp"><td class="cellrowborder" valign="top" width="17.82178217821782%" headers="mcps1.2.5.1.1 "><p id="p2237mcpsimp"><a name="p2237mcpsimp"></a><a name="p2237mcpsimp"></a>77</p>
</td>
<td class="cellrowborder" valign="top" width="23.762376237623766%" headers="mcps1.2.5.1.2 "><p id="p2239mcpsimp"><a name="p2239mcpsimp"></a><a name="p2239mcpsimp"></a>HSP_P</p>
</td>
<td class="cellrowborder" valign="top" width="16.011601160116008%" headers="mcps1.2.5.1.3 "><p id="p2241mcpsimp"><a name="p2241mcpsimp"></a><a name="p2241mcpsimp"></a>O<sub id="sub2242mcpsimp"><a name="sub2242mcpsimp"></a><a name="sub2242mcpsimp"></a>ANA</sub></p>
</td>
<td class="cellrowborder" valign="top" width="42.404240424042406%" headers="mcps1.2.5.1.4 "><p id="p2244mcpsimp"><a name="p2244mcpsimp"></a><a name="p2244mcpsimp"></a>HS接口输出信号正。</p>
</td>
</tr>
<tr id="row2245mcpsimp"><td class="cellrowborder" valign="top" width="17.82178217821782%" headers="mcps1.2.5.1.1 "><p id="p2247mcpsimp"><a name="p2247mcpsimp"></a><a name="p2247mcpsimp"></a>75</p>
</td>
<td class="cellrowborder" valign="top" width="23.762376237623766%" headers="mcps1.2.5.1.2 "><p id="p2249mcpsimp"><a name="p2249mcpsimp"></a><a name="p2249mcpsimp"></a>HSP_N</p>
</td>
<td class="cellrowborder" valign="top" width="16.011601160116008%" headers="mcps1.2.5.1.3 "><p id="p2251mcpsimp"><a name="p2251mcpsimp"></a><a name="p2251mcpsimp"></a>O<sub id="sub2252mcpsimp"><a name="sub2252mcpsimp"></a><a name="sub2252mcpsimp"></a>ANA</sub></p>
</td>
<td class="cellrowborder" valign="top" width="42.404240424042406%" headers="mcps1.2.5.1.4 "><p id="p2254mcpsimp"><a name="p2254mcpsimp"></a><a name="p2254mcpsimp"></a>HS接口输出信号负。</p>
</td>
</tr>
</tbody>
</table>

### MIPI信号管脚<a name="ZH-CN_TOPIC_0000002545781589"></a>

MIPI显示信号管脚如[表1](#__Ref54941516)所示。

芯片支持1lane MIPI显示接口，高速数字IO口。

**表 1**  MIPI显示信号管脚列表

<a name="__Ref54941516"></a>
<table><thead align="left"><tr id="row3108mcpsimp"><th class="cellrowborder" valign="top" width="18%" id="mcps1.2.6.1.1"><p id="p3110mcpsimp"><a name="p3110mcpsimp"></a><a name="p3110mcpsimp"></a>管脚编号</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.2.6.1.2"><p id="p3112mcpsimp"><a name="p3112mcpsimp"></a><a name="p3112mcpsimp"></a>管脚名称</p>
</th>
<th class="cellrowborder" valign="top" width="12%" id="mcps1.2.6.1.3"><p id="p3114mcpsimp"><a name="p3114mcpsimp"></a><a name="p3114mcpsimp"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="24.55%" id="mcps1.2.6.1.4"><p id="p3116mcpsimp"><a name="p3116mcpsimp"></a><a name="p3116mcpsimp"></a>电平标准</p>
</th>
<th class="cellrowborder" valign="top" width="24.45%" id="mcps1.2.6.1.5"><p id="p3118mcpsimp"><a name="p3118mcpsimp"></a><a name="p3118mcpsimp"></a>功能描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row3120mcpsimp"><td class="cellrowborder" valign="top" width="18%" headers="mcps1.2.6.1.1 "><p id="p3122mcpsimp"><a name="p3122mcpsimp"></a><a name="p3122mcpsimp"></a>68</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.2.6.1.2 "><p id="p3124mcpsimp"><a name="p3124mcpsimp"></a><a name="p3124mcpsimp"></a>MIPI_CLK_P</p>
</td>
<td class="cellrowborder" valign="top" width="12%" headers="mcps1.2.6.1.3 "><p id="p3126mcpsimp"><a name="p3126mcpsimp"></a><a name="p3126mcpsimp"></a>O</p>
</td>
<td class="cellrowborder" valign="top" width="24.55%" headers="mcps1.2.6.1.4 "><p id="p3128mcpsimp"><a name="p3128mcpsimp"></a><a name="p3128mcpsimp"></a>满足DSI接口协议。</p>
</td>
<td class="cellrowborder" valign="top" width="24.45%" headers="mcps1.2.6.1.5 "><p id="p3130mcpsimp"><a name="p3130mcpsimp"></a><a name="p3130mcpsimp"></a>MIPI显示接口CLK正。</p>
</td>
</tr>
<tr id="row3131mcpsimp"><td class="cellrowborder" valign="top" width="18%" headers="mcps1.2.6.1.1 "><p id="p3133mcpsimp"><a name="p3133mcpsimp"></a><a name="p3133mcpsimp"></a>66</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.2.6.1.2 "><p id="p3135mcpsimp"><a name="p3135mcpsimp"></a><a name="p3135mcpsimp"></a>MIPI_CLK_N</p>
</td>
<td class="cellrowborder" valign="top" width="12%" headers="mcps1.2.6.1.3 "><p id="p3137mcpsimp"><a name="p3137mcpsimp"></a><a name="p3137mcpsimp"></a>O</p>
</td>
<td class="cellrowborder" valign="top" width="24.55%" headers="mcps1.2.6.1.4 "><p id="p3139mcpsimp"><a name="p3139mcpsimp"></a><a name="p3139mcpsimp"></a>满足DSI接口协议。</p>
</td>
<td class="cellrowborder" valign="top" width="24.45%" headers="mcps1.2.6.1.5 "><p id="p3141mcpsimp"><a name="p3141mcpsimp"></a><a name="p3141mcpsimp"></a>MIPI显示接口CLK负。</p>
</td>
</tr>
<tr id="row3142mcpsimp"><td class="cellrowborder" valign="top" width="18%" headers="mcps1.2.6.1.1 "><p id="p3144mcpsimp"><a name="p3144mcpsimp"></a><a name="p3144mcpsimp"></a>71</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.2.6.1.2 "><p id="p3146mcpsimp"><a name="p3146mcpsimp"></a><a name="p3146mcpsimp"></a>MIPI_D0_P</p>
</td>
<td class="cellrowborder" valign="top" width="12%" headers="mcps1.2.6.1.3 "><p id="p3148mcpsimp"><a name="p3148mcpsimp"></a><a name="p3148mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="24.55%" headers="mcps1.2.6.1.4 "><p id="p3150mcpsimp"><a name="p3150mcpsimp"></a><a name="p3150mcpsimp"></a>满足DSI接口协议。</p>
</td>
<td class="cellrowborder" valign="top" width="24.45%" headers="mcps1.2.6.1.5 "><p id="p3152mcpsimp"><a name="p3152mcpsimp"></a><a name="p3152mcpsimp"></a>MIPI显示接口DATA0正。</p>
</td>
</tr>
<tr id="row3153mcpsimp"><td class="cellrowborder" valign="top" width="18%" headers="mcps1.2.6.1.1 "><p id="p3155mcpsimp"><a name="p3155mcpsimp"></a><a name="p3155mcpsimp"></a>69</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.2.6.1.2 "><p id="p3157mcpsimp"><a name="p3157mcpsimp"></a><a name="p3157mcpsimp"></a>MIPI_D0_N</p>
</td>
<td class="cellrowborder" valign="top" width="12%" headers="mcps1.2.6.1.3 "><p id="p3159mcpsimp"><a name="p3159mcpsimp"></a><a name="p3159mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="24.55%" headers="mcps1.2.6.1.4 "><p id="p3161mcpsimp"><a name="p3161mcpsimp"></a><a name="p3161mcpsimp"></a>满足DSI接口协议。</p>
</td>
<td class="cellrowborder" valign="top" width="24.45%" headers="mcps1.2.6.1.5 "><p id="p3163mcpsimp"><a name="p3163mcpsimp"></a><a name="p3163mcpsimp"></a>MIPI显示接口DATA0负。</p>
</td>
</tr>
</tbody>
</table>

### AUX ADC接口<a name="ZH-CN_TOPIC_0000002514141714"></a>

ADC信号管脚如[表1](#_Ref218523819)所示。模组支持8路ADC，其中AUX\_ADC\_CH\_4、AUX\_ADC\_CH\_5、AUX\_ADC\_CH\_6、AUX\_ADC\_CH\_7与数字管脚复用。

**表 1**  AUX\_ADC信号管脚列表

<a name="_Ref218523819"></a>
<table><thead align="left"><tr id="row2773mcpsimp"><th class="cellrowborder" valign="top" width="13.861386138613863%" id="mcps1.2.5.1.1"><p id="p2775mcpsimp"><a name="p2775mcpsimp"></a><a name="p2775mcpsimp"></a>管脚编号</p>
</th>
<th class="cellrowborder" valign="top" width="24.752475247524753%" id="mcps1.2.5.1.2"><p id="p2777mcpsimp"><a name="p2777mcpsimp"></a><a name="p2777mcpsimp"></a>管脚名称</p>
</th>
<th class="cellrowborder" valign="top" width="14.85148514851485%" id="mcps1.2.5.1.3"><p id="p2779mcpsimp"><a name="p2779mcpsimp"></a><a name="p2779mcpsimp"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="46.53465346534653%" id="mcps1.2.5.1.4"><p id="p2781mcpsimp"><a name="p2781mcpsimp"></a><a name="p2781mcpsimp"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row2783mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2785mcpsimp"><a name="p2785mcpsimp"></a><a name="p2785mcpsimp"></a>86</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2787mcpsimp"><a name="p2787mcpsimp"></a><a name="p2787mcpsimp"></a>AUX_ADC_CH_0</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2789mcpsimp"><a name="p2789mcpsimp"></a><a name="p2789mcpsimp"></a>I<sub id="sub2790mcpsimp"><a name="sub2790mcpsimp"></a><a name="sub2790mcpsimp"></a>ANA</sub></p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p2792mcpsimp"><a name="p2792mcpsimp"></a><a name="p2792mcpsimp"></a>ADC通道0。支持电平输入范围：0V～1.8V。</p>
</td>
</tr>
<tr id="row2793mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2795mcpsimp"><a name="p2795mcpsimp"></a><a name="p2795mcpsimp"></a>55</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2797mcpsimp"><a name="p2797mcpsimp"></a><a name="p2797mcpsimp"></a>AUX_ADC_CH_1</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2799mcpsimp"><a name="p2799mcpsimp"></a><a name="p2799mcpsimp"></a>I<sub id="sub2800mcpsimp"><a name="sub2800mcpsimp"></a><a name="sub2800mcpsimp"></a>ANA</sub></p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p2802mcpsimp"><a name="p2802mcpsimp"></a><a name="p2802mcpsimp"></a>ADC通道1。支持电平输入范围：0V～1.8V。</p>
</td>
</tr>
<tr id="row2803mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2805mcpsimp"><a name="p2805mcpsimp"></a><a name="p2805mcpsimp"></a>10</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2807mcpsimp"><a name="p2807mcpsimp"></a><a name="p2807mcpsimp"></a>AUX_ADC_CH_2</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2809mcpsimp"><a name="p2809mcpsimp"></a><a name="p2809mcpsimp"></a>I<sub id="sub2810mcpsimp"><a name="sub2810mcpsimp"></a><a name="sub2810mcpsimp"></a>ANA</sub></p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p2812mcpsimp"><a name="p2812mcpsimp"></a><a name="p2812mcpsimp"></a>ADC通道2。支持电平输入范围：0V～1.8V。</p>
</td>
</tr>
<tr id="row2813mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2815mcpsimp"><a name="p2815mcpsimp"></a><a name="p2815mcpsimp"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2817mcpsimp"><a name="p2817mcpsimp"></a><a name="p2817mcpsimp"></a>AUX_ADC_CH_3</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2819mcpsimp"><a name="p2819mcpsimp"></a><a name="p2819mcpsimp"></a>I<sub id="sub2820mcpsimp"><a name="sub2820mcpsimp"></a><a name="sub2820mcpsimp"></a>ANA</sub></p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p2822mcpsimp"><a name="p2822mcpsimp"></a><a name="p2822mcpsimp"></a>ADC通道3。支持电平输入范围：0V～1.8V。</p>
</td>
</tr>
<tr id="row2823mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2825mcpsimp"><a name="p2825mcpsimp"></a><a name="p2825mcpsimp"></a>44</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2827mcpsimp"><a name="p2827mcpsimp"></a><a name="p2827mcpsimp"></a>AUX_ADC_CH_4</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2829mcpsimp"><a name="p2829mcpsimp"></a><a name="p2829mcpsimp"></a>I<sub id="sub2830mcpsimp"><a name="sub2830mcpsimp"></a><a name="sub2830mcpsimp"></a>ANA</sub></p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p2832mcpsimp"><a name="p2832mcpsimp"></a><a name="p2832mcpsimp"></a>ADC通道4。支持电平输入范围：0V～1.8V。</p>
</td>
</tr>
<tr id="row2833mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2835mcpsimp"><a name="p2835mcpsimp"></a><a name="p2835mcpsimp"></a>34</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2837mcpsimp"><a name="p2837mcpsimp"></a><a name="p2837mcpsimp"></a>AUX_ADC_CH_5</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2839mcpsimp"><a name="p2839mcpsimp"></a><a name="p2839mcpsimp"></a>I<sub id="sub2840mcpsimp"><a name="sub2840mcpsimp"></a><a name="sub2840mcpsimp"></a>ANA</sub></p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p2842mcpsimp"><a name="p2842mcpsimp"></a><a name="p2842mcpsimp"></a>ADC通道5。支持电平输入范围：0V～1.8V。</p>
</td>
</tr>
<tr id="row2843mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2845mcpsimp"><a name="p2845mcpsimp"></a><a name="p2845mcpsimp"></a>52</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2847mcpsimp"><a name="p2847mcpsimp"></a><a name="p2847mcpsimp"></a>AUX_ADC_CH_6</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2849mcpsimp"><a name="p2849mcpsimp"></a><a name="p2849mcpsimp"></a>I<sub id="sub2850mcpsimp"><a name="sub2850mcpsimp"></a><a name="sub2850mcpsimp"></a>ANA</sub></p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p2852mcpsimp"><a name="p2852mcpsimp"></a><a name="p2852mcpsimp"></a>ADC通道6。支持电平输入范围：0V～1.8V。</p>
</td>
</tr>
<tr id="row2853mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2855mcpsimp"><a name="p2855mcpsimp"></a><a name="p2855mcpsimp"></a>41</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2857mcpsimp"><a name="p2857mcpsimp"></a><a name="p2857mcpsimp"></a>AUX_ADC_CH_7</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2859mcpsimp"><a name="p2859mcpsimp"></a><a name="p2859mcpsimp"></a>I<sub id="sub2860mcpsimp"><a name="sub2860mcpsimp"></a><a name="sub2860mcpsimp"></a>ANA</sub></p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p2862mcpsimp"><a name="p2862mcpsimp"></a><a name="p2862mcpsimp"></a>ADC通道7。支持电平输入范围：0V～1.8V。</p>
</td>
</tr>
</tbody>
</table>

### USB接口<a name="ZH-CN_TOPIC_0000002545901583"></a>

模组支持USB 2.0接口。

USB信号如[表1](#_d1e2531)所示。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>CAT1\_USB\_VBUS上电应在CAT1\_VBAT之后，产测烧录时应注意。

**表 1**  USB信号管脚列表

<a name="_d1e2531"></a>
<table><thead align="left"><tr id="row2365mcpsimp"><th class="cellrowborder" valign="top" width="13.861386138613863%" id="mcps1.2.5.1.1"><p id="p2367mcpsimp"><a name="p2367mcpsimp"></a><a name="p2367mcpsimp"></a>管脚编号</p>
</th>
<th class="cellrowborder" valign="top" width="24.752475247524753%" id="mcps1.2.5.1.2"><p id="p2369mcpsimp"><a name="p2369mcpsimp"></a><a name="p2369mcpsimp"></a>管脚名称</p>
</th>
<th class="cellrowborder" valign="top" width="14.85148514851485%" id="mcps1.2.5.1.3"><p id="p2371mcpsimp"><a name="p2371mcpsimp"></a><a name="p2371mcpsimp"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="46.53465346534653%" id="mcps1.2.5.1.4"><p id="p2373mcpsimp"><a name="p2373mcpsimp"></a><a name="p2373mcpsimp"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row2375mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2377mcpsimp"><a name="p2377mcpsimp"></a><a name="p2377mcpsimp"></a>19</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2379mcpsimp"><a name="p2379mcpsimp"></a><a name="p2379mcpsimp"></a>USB_DP</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2381mcpsimp"><a name="p2381mcpsimp"></a><a name="p2381mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p2383mcpsimp"><a name="p2383mcpsimp"></a><a name="p2383mcpsimp"></a>主控USB_DP差分信号管脚。</p>
</td>
</tr>
<tr id="row2384mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2386mcpsimp"><a name="p2386mcpsimp"></a><a name="p2386mcpsimp"></a>21</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2388mcpsimp"><a name="p2388mcpsimp"></a><a name="p2388mcpsimp"></a>USB _DM</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2390mcpsimp"><a name="p2390mcpsimp"></a><a name="p2390mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p2392mcpsimp"><a name="p2392mcpsimp"></a><a name="p2392mcpsimp"></a>主控USB_DM差分信号管脚。</p>
</td>
</tr>
<tr id="row2393mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2395mcpsimp"><a name="p2395mcpsimp"></a><a name="p2395mcpsimp"></a>132</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2397mcpsimp"><a name="p2397mcpsimp"></a><a name="p2397mcpsimp"></a>CAT1_USB_DP</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2399mcpsimp"><a name="p2399mcpsimp"></a><a name="p2399mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p2401mcpsimp"><a name="p2401mcpsimp"></a><a name="p2401mcpsimp"></a>CAT1 USB_DP差分信号管脚</p>
</td>
</tr>
<tr id="row2402mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2404mcpsimp"><a name="p2404mcpsimp"></a><a name="p2404mcpsimp"></a>131</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2406mcpsimp"><a name="p2406mcpsimp"></a><a name="p2406mcpsimp"></a>CAT1_USB_DM</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2408mcpsimp"><a name="p2408mcpsimp"></a><a name="p2408mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p2410mcpsimp"><a name="p2410mcpsimp"></a><a name="p2410mcpsimp"></a>CAT1 USB_DM差分信号管脚。</p>
</td>
</tr>
<tr id="row2411mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2413mcpsimp"><a name="p2413mcpsimp"></a><a name="p2413mcpsimp"></a>130</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2415mcpsimp"><a name="p2415mcpsimp"></a><a name="p2415mcpsimp"></a>CAT1_USB_VBUS</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2417mcpsimp"><a name="p2417mcpsimp"></a><a name="p2417mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p2419mcpsimp"><a name="p2419mcpsimp"></a><a name="p2419mcpsimp"></a>CAT1 USB插拔检测引脚。</p>
</td>
</tr>
<tr id="row2420mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2422mcpsimp"><a name="p2422mcpsimp"></a><a name="p2422mcpsimp"></a>129</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2424mcpsimp"><a name="p2424mcpsimp"></a><a name="p2424mcpsimp"></a>CAT1_USB_BOOT</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2426mcpsimp"><a name="p2426mcpsimp"></a><a name="p2426mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p2428mcpsimp"><a name="p2428mcpsimp"></a><a name="p2428mcpsimp"></a>CAT1 USB加载引脚。</p>
</td>
</tr>
</tbody>
</table>

### GND管脚<a name="ZH-CN_TOPIC_0000002514301638"></a>

GND管脚如[表1](#table8841mcpsimp)所示。

**表 1**  GND管脚列表

<a name="table8841mcpsimp"></a>
<table><thead align="left"><tr id="row2513mcpsimp"><th class="cellrowborder" valign="top" width="17.71%" id="mcps1.2.4.1.1"><p id="p2515mcpsimp"><a name="p2515mcpsimp"></a><a name="p2515mcpsimp"></a>管脚名称</p>
</th>
<th class="cellrowborder" valign="top" width="51.38%" id="mcps1.2.4.1.2"><p id="p2517mcpsimp"><a name="p2517mcpsimp"></a><a name="p2517mcpsimp"></a>管脚编号</p>
</th>
<th class="cellrowborder" valign="top" width="30.91%" id="mcps1.2.4.1.3"><p id="p2519mcpsimp"><a name="p2519mcpsimp"></a><a name="p2519mcpsimp"></a>功能描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row2521mcpsimp"><td class="cellrowborder" valign="top" width="17.71%" headers="mcps1.2.4.1.1 "><p id="p2523mcpsimp"><a name="p2523mcpsimp"></a><a name="p2523mcpsimp"></a>GND</p>
</td>
<td class="cellrowborder" valign="top" width="51.38%" headers="mcps1.2.4.1.2 "><p id="p2525mcpsimp"><a name="p2525mcpsimp"></a><a name="p2525mcpsimp"></a>1，2，36，37，38，40，54，60，61，62，79，96</p>
<p id="p2526mcpsimp"><a name="p2526mcpsimp"></a><a name="p2526mcpsimp"></a>97，98，100，116，120，126，127，128，133</p>
</td>
<td class="cellrowborder" valign="top" width="30.91%" headers="mcps1.2.4.1.3 "><p id="p2528mcpsimp"><a name="p2528mcpsimp"></a><a name="p2528mcpsimp"></a>地。</p>
</td>
</tr>
<tr id="row2529mcpsimp"><td class="cellrowborder" valign="top" width="17.71%" headers="mcps1.2.4.1.1 "><p id="p2531mcpsimp"><a name="p2531mcpsimp"></a><a name="p2531mcpsimp"></a>GND_ePAD</p>
</td>
<td class="cellrowborder" valign="top" width="51.38%" headers="mcps1.2.4.1.2 "><p id="p2533mcpsimp"><a name="p2533mcpsimp"></a><a name="p2533mcpsimp"></a>133,134,135,136,137,138,139,140,141,142,143,144</p>
</td>
<td class="cellrowborder" valign="top" width="30.91%" headers="mcps1.2.4.1.3 "><p id="entry2534mcpsimpp0"><a name="entry2534mcpsimpp0"></a><a name="entry2534mcpsimpp0"></a>-</p>
</td>
</tr>
</tbody>
</table>

### USIM管脚<a name="ZH-CN_TOPIC_0000002545781593"></a>

模组内SIM卡功能遵守SIM卡协议标准，详细的接口分配管脚如[表1](#_Ref218524178)所示。

**表 1**  USIM管脚列表

<a name="_Ref218524178"></a>
<table><thead align="left"><tr id="row2948mcpsimp"><th class="cellrowborder" valign="top" width="13.861386138613863%" id="mcps1.2.5.1.1"><p id="p2950mcpsimp"><a name="p2950mcpsimp"></a><a name="p2950mcpsimp"></a>管脚编号</p>
</th>
<th class="cellrowborder" valign="top" width="24.752475247524753%" id="mcps1.2.5.1.2"><p id="p2952mcpsimp"><a name="p2952mcpsimp"></a><a name="p2952mcpsimp"></a>管脚名称</p>
</th>
<th class="cellrowborder" valign="top" width="14.85148514851485%" id="mcps1.2.5.1.3"><p id="p2954mcpsimp"><a name="p2954mcpsimp"></a><a name="p2954mcpsimp"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="46.53465346534654%" id="mcps1.2.5.1.4"><p id="p2956mcpsimp"><a name="p2956mcpsimp"></a><a name="p2956mcpsimp"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row2958mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2960mcpsimp"><a name="p2960mcpsimp"></a><a name="p2960mcpsimp"></a>13</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2962mcpsimp"><a name="p2962mcpsimp"></a><a name="p2962mcpsimp"></a>USIM_RST</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2964mcpsimp"><a name="p2964mcpsimp"></a><a name="p2964mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534654%" headers="mcps1.2.5.1.4 "><p id="p2966mcpsimp"><a name="p2966mcpsimp"></a><a name="p2966mcpsimp"></a>USIM卡复位</p>
</td>
</tr>
<tr id="row2967mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2969mcpsimp"><a name="p2969mcpsimp"></a><a name="p2969mcpsimp"></a>11</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2971mcpsimp"><a name="p2971mcpsimp"></a><a name="p2971mcpsimp"></a>USIM_CLK</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2973mcpsimp"><a name="p2973mcpsimp"></a><a name="p2973mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534654%" headers="mcps1.2.5.1.4 "><p id="p2975mcpsimp"><a name="p2975mcpsimp"></a><a name="p2975mcpsimp"></a>USIM卡时钟。</p>
</td>
</tr>
<tr id="row2976mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2978mcpsimp"><a name="p2978mcpsimp"></a><a name="p2978mcpsimp"></a>9</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2980mcpsimp"><a name="p2980mcpsimp"></a><a name="p2980mcpsimp"></a>USIM_DATA</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2982mcpsimp"><a name="p2982mcpsimp"></a><a name="p2982mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534654%" headers="mcps1.2.5.1.4 "><p id="p2984mcpsimp"><a name="p2984mcpsimp"></a><a name="p2984mcpsimp"></a>USIM卡数据。</p>
</td>
</tr>
<tr id="row2985mcpsimp"><td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.5.1.1 "><p id="p2987mcpsimp"><a name="p2987mcpsimp"></a><a name="p2987mcpsimp"></a>115</p>
</td>
<td class="cellrowborder" valign="top" width="24.752475247524753%" headers="mcps1.2.5.1.2 "><p id="p2989mcpsimp"><a name="p2989mcpsimp"></a><a name="p2989mcpsimp"></a>USIM_DET</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p2991mcpsimp"><a name="p2991mcpsimp"></a><a name="p2991mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534654%" headers="mcps1.2.5.1.4 "><p id="p2993mcpsimp"><a name="p2993mcpsimp"></a><a name="p2993mcpsimp"></a>USIM卡插拔检测。</p>
</td>
</tr>
</tbody>
</table>

### 数字管脚<a name="ZH-CN_TOPIC_0000002514141716"></a>

>![](public_sys-resources/icon-notice.gif) **须知：** 
>-   不使用的管脚，不允许直接悬空，需板级进行处理或禁止输入使能。
>-   各管脚需结合对接器件的电平状态，确认不同工作模式下的管脚上下拉及输入使能配置。

HiDiTingV100模组共有70个通用数字管脚，数字功能包括：GPIO、QSPI、SPI、I2C、UART、eMMC、JLINK、PDM、I2S、PWM、FEM控制等。部分管脚复用模拟功能，如：AUX ADC等。

管脚描述如[表1](#table290mcpsimp)所示。

**表 1**  数字管脚描述

<a name="table290mcpsimp"></a>
<table><thead align="left"><tr id="row299mcpsimp"><th class="cellrowborder" valign="top" width="11.341134113411341%" id="mcps1.2.7.1.1"><p id="p301mcpsimp"><a name="p301mcpsimp"></a><a name="p301mcpsimp"></a>管脚编号</p>
</th>
<th class="cellrowborder" valign="top" width="20.432043204320433%" id="mcps1.2.7.1.2"><p id="p303mcpsimp"><a name="p303mcpsimp"></a><a name="p303mcpsimp"></a>管脚名称</p>
</th>
<th class="cellrowborder" valign="top" width="20.7020702070207%" id="mcps1.2.7.1.3"><p id="p305mcpsimp"><a name="p305mcpsimp"></a><a name="p305mcpsimp"></a>VDDIO电源域</p>
</th>
<th class="cellrowborder" valign="top" width="8.6008600860086%" id="mcps1.2.7.1.4"><p id="p307mcpsimp"><a name="p307mcpsimp"></a><a name="p307mcpsimp"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="26.05260526052605%" id="mcps1.2.7.1.5"><p id="p309mcpsimp"><a name="p309mcpsimp"></a><a name="p309mcpsimp"></a>数字功能复用</p>
</th>
<th class="cellrowborder" valign="top" width="12.871287128712872%" id="mcps1.2.7.1.6"><p id="p311mcpsimp"><a name="p311mcpsimp"></a><a name="p311mcpsimp"></a>模拟复用</p>
</th>
</tr>
</thead>
<tbody><tr id="row313mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p315mcpsimp"><a name="p315mcpsimp"></a><a name="p315mcpsimp"></a>111</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p317mcpsimp"><a name="p317mcpsimp"></a><a name="p317mcpsimp"></a>UART2_TXD</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p319mcpsimp"><a name="p319mcpsimp"></a><a name="p319mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p321mcpsimp"><a name="p321mcpsimp"></a><a name="p321mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p323mcpsimp"><a name="p323mcpsimp"></a><a name="p323mcpsimp"></a>UART2_TXD</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry324mcpsimpp0"><a name="entry324mcpsimpp0"></a><a name="entry324mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row325mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p327mcpsimp"><a name="p327mcpsimp"></a><a name="p327mcpsimp"></a>113</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p329mcpsimp"><a name="p329mcpsimp"></a><a name="p329mcpsimp"></a>UART2_RXD</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p331mcpsimp"><a name="p331mcpsimp"></a><a name="p331mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p333mcpsimp"><a name="p333mcpsimp"></a><a name="p333mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p335mcpsimp"><a name="p335mcpsimp"></a><a name="p335mcpsimp"></a>UART2_RXD/ MPWM0_N</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry336mcpsimpp0"><a name="entry336mcpsimpp0"></a><a name="entry336mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row337mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p339mcpsimp"><a name="p339mcpsimp"></a><a name="p339mcpsimp"></a>53</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p341mcpsimp"><a name="p341mcpsimp"></a><a name="p341mcpsimp"></a>SSI_CLK</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p343mcpsimp"><a name="p343mcpsimp"></a><a name="p343mcpsimp"></a>IOLDO4（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p345mcpsimp"><a name="p345mcpsimp"></a><a name="p345mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p347mcpsimp"><a name="p347mcpsimp"></a><a name="p347mcpsimp"></a>SWD_CLK</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry348mcpsimpp0"><a name="entry348mcpsimpp0"></a><a name="entry348mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row349mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p351mcpsimp"><a name="p351mcpsimp"></a><a name="p351mcpsimp"></a>50</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p353mcpsimp"><a name="p353mcpsimp"></a><a name="p353mcpsimp"></a>SSI_DATA</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p355mcpsimp"><a name="p355mcpsimp"></a><a name="p355mcpsimp"></a>IOLDO4（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p357mcpsimp"><a name="p357mcpsimp"></a><a name="p357mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p359mcpsimp"><a name="p359mcpsimp"></a><a name="p359mcpsimp"></a>SWD_DATA</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry360mcpsimpp0"><a name="entry360mcpsimpp0"></a><a name="entry360mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row361mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p363mcpsimp"><a name="p363mcpsimp"></a><a name="p363mcpsimp"></a>20</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p365mcpsimp"><a name="p365mcpsimp"></a><a name="p365mcpsimp"></a>I2C0_SCL_SENSOR</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p367mcpsimp"><a name="p367mcpsimp"></a><a name="p367mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p369mcpsimp"><a name="p369mcpsimp"></a><a name="p369mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p371mcpsimp"><a name="p371mcpsimp"></a><a name="p371mcpsimp"></a>I2C0_SCL/UART1_TXD</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry372mcpsimpp0"><a name="entry372mcpsimpp0"></a><a name="entry372mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row373mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p375mcpsimp"><a name="p375mcpsimp"></a><a name="p375mcpsimp"></a>22</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p377mcpsimp"><a name="p377mcpsimp"></a><a name="p377mcpsimp"></a>I2C0_SDA_SENSOR</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p379mcpsimp"><a name="p379mcpsimp"></a><a name="p379mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p381mcpsimp"><a name="p381mcpsimp"></a><a name="p381mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p383mcpsimp"><a name="p383mcpsimp"></a><a name="p383mcpsimp"></a>I2C0_SDA/UART1_RXD</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry384mcpsimpp0"><a name="entry384mcpsimpp0"></a><a name="entry384mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row385mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p387mcpsimp"><a name="p387mcpsimp"></a><a name="p387mcpsimp"></a>94</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p389mcpsimp"><a name="p389mcpsimp"></a><a name="p389mcpsimp"></a>I2C1_SCL_TP</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p391mcpsimp"><a name="p391mcpsimp"></a><a name="p391mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p393mcpsimp"><a name="p393mcpsimp"></a><a name="p393mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p395mcpsimp"><a name="p395mcpsimp"></a><a name="p395mcpsimp"></a>I2C1_SCL/UART1_CTS</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry396mcpsimpp0"><a name="entry396mcpsimpp0"></a><a name="entry396mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row397mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p399mcpsimp"><a name="p399mcpsimp"></a><a name="p399mcpsimp"></a>92</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p401mcpsimp"><a name="p401mcpsimp"></a><a name="p401mcpsimp"></a>I2C1_SDA_TP</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p403mcpsimp"><a name="p403mcpsimp"></a><a name="p403mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p405mcpsimp"><a name="p405mcpsimp"></a><a name="p405mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p407mcpsimp"><a name="p407mcpsimp"></a><a name="p407mcpsimp"></a>I2C1_SDA/UART1_RTS</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry408mcpsimpp0"><a name="entry408mcpsimpp0"></a><a name="entry408mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row409mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p411mcpsimp"><a name="p411mcpsimp"></a><a name="p411mcpsimp"></a>107</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p413mcpsimp"><a name="p413mcpsimp"></a><a name="p413mcpsimp"></a>I2C2_SCL_NFC</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p415mcpsimp"><a name="p415mcpsimp"></a><a name="p415mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p417mcpsimp"><a name="p417mcpsimp"></a><a name="p417mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p419mcpsimp"><a name="p419mcpsimp"></a><a name="p419mcpsimp"></a>I2C2_SCL/SPI1_DI</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry420mcpsimpp0"><a name="entry420mcpsimpp0"></a><a name="entry420mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row421mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p423mcpsimp"><a name="p423mcpsimp"></a><a name="p423mcpsimp"></a>109</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p425mcpsimp"><a name="p425mcpsimp"></a><a name="p425mcpsimp"></a>I2C2_SDA_NFC</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p427mcpsimp"><a name="p427mcpsimp"></a><a name="p427mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p429mcpsimp"><a name="p429mcpsimp"></a><a name="p429mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p431mcpsimp"><a name="p431mcpsimp"></a><a name="p431mcpsimp"></a>I2C2_SDA/SPI1_DO</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p149581133539"><a name="p149581133539"></a><a name="p149581133539"></a>-</p>
</td>
</tr>
<tr id="row433mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p435mcpsimp"><a name="p435mcpsimp"></a><a name="p435mcpsimp"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p437mcpsimp"><a name="p437mcpsimp"></a><a name="p437mcpsimp"></a>I2C3_SCL</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p439mcpsimp"><a name="p439mcpsimp"></a><a name="p439mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p441mcpsimp"><a name="p441mcpsimp"></a><a name="p441mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p443mcpsimp"><a name="p443mcpsimp"></a><a name="p443mcpsimp"></a>I2C3_SCL/SPI1_CLK</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p895815317539"><a name="p895815317539"></a><a name="p895815317539"></a>-</p>
</td>
</tr>
<tr id="row445mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p447mcpsimp"><a name="p447mcpsimp"></a><a name="p447mcpsimp"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p449mcpsimp"><a name="p449mcpsimp"></a><a name="p449mcpsimp"></a>I2C3_SDA</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p451mcpsimp"><a name="p451mcpsimp"></a><a name="p451mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p453mcpsimp"><a name="p453mcpsimp"></a><a name="p453mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p455mcpsimp"><a name="p455mcpsimp"></a><a name="p455mcpsimp"></a>I2C3_SDA/SPI1_CS</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p595863135314"><a name="p595863135314"></a><a name="p595863135314"></a>-</p>
</td>
</tr>
<tr id="row457mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p459mcpsimp"><a name="p459mcpsimp"></a><a name="p459mcpsimp"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p461mcpsimp"><a name="p461mcpsimp"></a><a name="p461mcpsimp"></a>S_HGPIO_10/VICAP_D0</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p463mcpsimp"><a name="p463mcpsimp"></a><a name="p463mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p465mcpsimp"><a name="p465mcpsimp"></a><a name="p465mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p467mcpsimp"><a name="p467mcpsimp"></a><a name="p467mcpsimp"></a>SDIO_D0/VICAP_D0</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p17958153125313"><a name="p17958153125313"></a><a name="p17958153125313"></a>-</p>
</td>
</tr>
<tr id="row469mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p471mcpsimp"><a name="p471mcpsimp"></a><a name="p471mcpsimp"></a>117</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p473mcpsimp"><a name="p473mcpsimp"></a><a name="p473mcpsimp"></a>S_HGPIO_11/VICAP_D1</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p475mcpsimp"><a name="p475mcpsimp"></a><a name="p475mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p477mcpsimp"><a name="p477mcpsimp"></a><a name="p477mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p479mcpsimp"><a name="p479mcpsimp"></a><a name="p479mcpsimp"></a>SDIO_D1/ VICAP_D1</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p179587335318"><a name="p179587335318"></a><a name="p179587335318"></a>-</p>
</td>
</tr>
<tr id="row481mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p483mcpsimp"><a name="p483mcpsimp"></a><a name="p483mcpsimp"></a>45</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p485mcpsimp"><a name="p485mcpsimp"></a><a name="p485mcpsimp"></a>S_HGPIO_12/VICAP_D2</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p487mcpsimp"><a name="p487mcpsimp"></a><a name="p487mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p489mcpsimp"><a name="p489mcpsimp"></a><a name="p489mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p491mcpsimp"><a name="p491mcpsimp"></a><a name="p491mcpsimp"></a>SDIO_D2/VICAP_D2</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p179581039533"><a name="p179581039533"></a><a name="p179581039533"></a>-</p>
</td>
</tr>
<tr id="row493mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p495mcpsimp"><a name="p495mcpsimp"></a><a name="p495mcpsimp"></a>33</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p497mcpsimp"><a name="p497mcpsimp"></a><a name="p497mcpsimp"></a>S_HGPIO_13/VICAP_D3</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p499mcpsimp"><a name="p499mcpsimp"></a><a name="p499mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p501mcpsimp"><a name="p501mcpsimp"></a><a name="p501mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p503mcpsimp"><a name="p503mcpsimp"></a><a name="p503mcpsimp"></a>SDIO_D3/VICAP_D3</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p18958133185312"><a name="p18958133185312"></a><a name="p18958133185312"></a>-</p>
</td>
</tr>
<tr id="row505mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p507mcpsimp"><a name="p507mcpsimp"></a><a name="p507mcpsimp"></a>8</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p509mcpsimp"><a name="p509mcpsimp"></a><a name="p509mcpsimp"></a>S_HGPIO_14/VICAP_D4</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p511mcpsimp"><a name="p511mcpsimp"></a><a name="p511mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p513mcpsimp"><a name="p513mcpsimp"></a><a name="p513mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p515mcpsimp"><a name="p515mcpsimp"></a><a name="p515mcpsimp"></a>SDIO_CMD/VICAP_D4</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p69580325318"><a name="p69580325318"></a><a name="p69580325318"></a>-</p>
</td>
</tr>
<tr id="row517mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p519mcpsimp"><a name="p519mcpsimp"></a><a name="p519mcpsimp"></a>88</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p521mcpsimp"><a name="p521mcpsimp"></a><a name="p521mcpsimp"></a>S_HGPIO_15/VICAP_CLK</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p523mcpsimp"><a name="p523mcpsimp"></a><a name="p523mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p525mcpsimp"><a name="p525mcpsimp"></a><a name="p525mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p527mcpsimp"><a name="p527mcpsimp"></a><a name="p527mcpsimp"></a>SDIO_CLK/VICAP_CLK</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p189581313535"><a name="p189581313535"></a><a name="p189581313535"></a>-</p>
</td>
</tr>
<tr id="row529mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p531mcpsimp"><a name="p531mcpsimp"></a><a name="p531mcpsimp"></a>83</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p533mcpsimp"><a name="p533mcpsimp"></a><a name="p533mcpsimp"></a>S_MGPIO_12/VICAP_D5</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p535mcpsimp"><a name="p535mcpsimp"></a><a name="p535mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p537mcpsimp"><a name="p537mcpsimp"></a><a name="p537mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p539mcpsimp"><a name="p539mcpsimp"></a><a name="p539mcpsimp"></a>SPI0_DI/UART0_TXD/VICAP_D5</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p1940595135311"><a name="p1940595135311"></a><a name="p1940595135311"></a>-</p>
</td>
</tr>
<tr id="row541mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p543mcpsimp"><a name="p543mcpsimp"></a><a name="p543mcpsimp"></a>114</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p545mcpsimp"><a name="p545mcpsimp"></a><a name="p545mcpsimp"></a>S_MGPIO_13/VICAP_D6</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p547mcpsimp"><a name="p547mcpsimp"></a><a name="p547mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p549mcpsimp"><a name="p549mcpsimp"></a><a name="p549mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p551mcpsimp"><a name="p551mcpsimp"></a><a name="p551mcpsimp"></a>SPI0_DO/UART0_RXD/VICAP_D6</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p17405452533"><a name="p17405452533"></a><a name="p17405452533"></a>-</p>
</td>
</tr>
<tr id="row553mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p555mcpsimp"><a name="p555mcpsimp"></a><a name="p555mcpsimp"></a>81</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p557mcpsimp"><a name="p557mcpsimp"></a><a name="p557mcpsimp"></a>S_MGPIO_14/VICAP_D7</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p559mcpsimp"><a name="p559mcpsimp"></a><a name="p559mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p561mcpsimp"><a name="p561mcpsimp"></a><a name="p561mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p563mcpsimp"><a name="p563mcpsimp"></a><a name="p563mcpsimp"></a>SPI0_CLK/UART0_CTS/VICAP_D7</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p1640514555317"><a name="p1640514555317"></a><a name="p1640514555317"></a>-</p>
</td>
</tr>
<tr id="row565mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p567mcpsimp"><a name="p567mcpsimp"></a><a name="p567mcpsimp"></a>112</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p569mcpsimp"><a name="p569mcpsimp"></a><a name="p569mcpsimp"></a>S_MGPIO_15/VICAP_VSYNC</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p571mcpsimp"><a name="p571mcpsimp"></a><a name="p571mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p573mcpsimp"><a name="p573mcpsimp"></a><a name="p573mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p575mcpsimp"><a name="p575mcpsimp"></a><a name="p575mcpsimp"></a>SPI0_CS/UART0_RTS/VICAP_VSYNC</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p24057511535"><a name="p24057511535"></a><a name="p24057511535"></a>-</p>
</td>
</tr>
<tr id="row577mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p579mcpsimp"><a name="p579mcpsimp"></a><a name="p579mcpsimp"></a>110</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p581mcpsimp"><a name="p581mcpsimp"></a><a name="p581mcpsimp"></a>S_MGPIO_16/VICAP_HSYNC</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p583mcpsimp"><a name="p583mcpsimp"></a><a name="p583mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p585mcpsimp"><a name="p585mcpsimp"></a><a name="p585mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p587mcpsimp"><a name="p587mcpsimp"></a><a name="p587mcpsimp"></a>SPI0_CS1/MPWM0_P/VICAP_HSYNC</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p740511575319"><a name="p740511575319"></a><a name="p740511575319"></a>-</p>
</td>
</tr>
<tr id="row589mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p591mcpsimp"><a name="p591mcpsimp"></a><a name="p591mcpsimp"></a>95</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p593mcpsimp"><a name="p593mcpsimp"></a><a name="p593mcpsimp"></a>QSPI2_D0_DISPLAY</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p595mcpsimp"><a name="p595mcpsimp"></a><a name="p595mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p597mcpsimp"><a name="p597mcpsimp"></a><a name="p597mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p599mcpsimp"><a name="p599mcpsimp"></a><a name="p599mcpsimp"></a>QSPI2_D0</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p440514505311"><a name="p440514505311"></a><a name="p440514505311"></a>-</p>
</td>
</tr>
<tr id="row601mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p603mcpsimp"><a name="p603mcpsimp"></a><a name="p603mcpsimp"></a>93</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p605mcpsimp"><a name="p605mcpsimp"></a><a name="p605mcpsimp"></a>QSPI2_D1_DISPLAY</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p607mcpsimp"><a name="p607mcpsimp"></a><a name="p607mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p609mcpsimp"><a name="p609mcpsimp"></a><a name="p609mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p611mcpsimp"><a name="p611mcpsimp"></a><a name="p611mcpsimp"></a>QSPI2_D1</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p640512512535"><a name="p640512512535"></a><a name="p640512512535"></a>-</p>
</td>
</tr>
<tr id="row613mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p615mcpsimp"><a name="p615mcpsimp"></a><a name="p615mcpsimp"></a>101</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p617mcpsimp"><a name="p617mcpsimp"></a><a name="p617mcpsimp"></a>QSPI2_D2_DISPLAY</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p619mcpsimp"><a name="p619mcpsimp"></a><a name="p619mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p621mcpsimp"><a name="p621mcpsimp"></a><a name="p621mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p623mcpsimp"><a name="p623mcpsimp"></a><a name="p623mcpsimp"></a>QSPI2_D2</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p840515515537"><a name="p840515515537"></a><a name="p840515515537"></a>-</p>
</td>
</tr>
<tr id="row625mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p627mcpsimp"><a name="p627mcpsimp"></a><a name="p627mcpsimp"></a>91</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p629mcpsimp"><a name="p629mcpsimp"></a><a name="p629mcpsimp"></a>QSPI2_D3_DISPLAY</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p631mcpsimp"><a name="p631mcpsimp"></a><a name="p631mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p633mcpsimp"><a name="p633mcpsimp"></a><a name="p633mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p635mcpsimp"><a name="p635mcpsimp"></a><a name="p635mcpsimp"></a>QSPI2_D3</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p1406157535"><a name="p1406157535"></a><a name="p1406157535"></a>-</p>
</td>
</tr>
<tr id="row637mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p639mcpsimp"><a name="p639mcpsimp"></a><a name="p639mcpsimp"></a>87</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p641mcpsimp"><a name="p641mcpsimp"></a><a name="p641mcpsimp"></a>QSPI2_CS0_DISPLAY</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p643mcpsimp"><a name="p643mcpsimp"></a><a name="p643mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p645mcpsimp"><a name="p645mcpsimp"></a><a name="p645mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p647mcpsimp"><a name="p647mcpsimp"></a><a name="p647mcpsimp"></a>QSPI2_CS</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p228868165312"><a name="p228868165312"></a><a name="p228868165312"></a>-</p>
</td>
</tr>
<tr id="row649mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p651mcpsimp"><a name="p651mcpsimp"></a><a name="p651mcpsimp"></a>89</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p653mcpsimp"><a name="p653mcpsimp"></a><a name="p653mcpsimp"></a>QSPI2_CLK_DISPLAY</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p655mcpsimp"><a name="p655mcpsimp"></a><a name="p655mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p657mcpsimp"><a name="p657mcpsimp"></a><a name="p657mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p659mcpsimp"><a name="p659mcpsimp"></a><a name="p659mcpsimp"></a>QSPI2_CLK</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p928818814539"><a name="p928818814539"></a><a name="p928818814539"></a>-</p>
</td>
</tr>
<tr id="row661mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p663mcpsimp"><a name="p663mcpsimp"></a><a name="p663mcpsimp"></a>28</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p665mcpsimp"><a name="p665mcpsimp"></a><a name="p665mcpsimp"></a>NOR_QSPI0_D0</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p667mcpsimp"><a name="p667mcpsimp"></a><a name="p667mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p669mcpsimp"><a name="p669mcpsimp"></a><a name="p669mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p671mcpsimp"><a name="p671mcpsimp"></a><a name="p671mcpsimp"></a>QSPI0_D0</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p42881885531"><a name="p42881885531"></a><a name="p42881885531"></a>-</p>
</td>
</tr>
<tr id="row673mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p675mcpsimp"><a name="p675mcpsimp"></a><a name="p675mcpsimp"></a>26</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p677mcpsimp"><a name="p677mcpsimp"></a><a name="p677mcpsimp"></a>NOR_QSPI0_D1</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p679mcpsimp"><a name="p679mcpsimp"></a><a name="p679mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p681mcpsimp"><a name="p681mcpsimp"></a><a name="p681mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p683mcpsimp"><a name="p683mcpsimp"></a><a name="p683mcpsimp"></a>QSPI0_D1</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p172880825316"><a name="p172880825316"></a><a name="p172880825316"></a>-</p>
</td>
</tr>
<tr id="row685mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p687mcpsimp"><a name="p687mcpsimp"></a><a name="p687mcpsimp"></a>32</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p689mcpsimp"><a name="p689mcpsimp"></a><a name="p689mcpsimp"></a>NOR_QSPI0_D2</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p691mcpsimp"><a name="p691mcpsimp"></a><a name="p691mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p693mcpsimp"><a name="p693mcpsimp"></a><a name="p693mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p695mcpsimp"><a name="p695mcpsimp"></a><a name="p695mcpsimp"></a>QSPI0_D2</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p628848185314"><a name="p628848185314"></a><a name="p628848185314"></a>-</p>
</td>
</tr>
<tr id="row697mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p699mcpsimp"><a name="p699mcpsimp"></a><a name="p699mcpsimp"></a>27</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p701mcpsimp"><a name="p701mcpsimp"></a><a name="p701mcpsimp"></a>NOR_QSPI0_D3</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p703mcpsimp"><a name="p703mcpsimp"></a><a name="p703mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p705mcpsimp"><a name="p705mcpsimp"></a><a name="p705mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p707mcpsimp"><a name="p707mcpsimp"></a><a name="p707mcpsimp"></a>QSPI0_D3</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p52884815533"><a name="p52884815533"></a><a name="p52884815533"></a>-</p>
</td>
</tr>
<tr id="row709mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p711mcpsimp"><a name="p711mcpsimp"></a><a name="p711mcpsimp"></a>29</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p713mcpsimp"><a name="p713mcpsimp"></a><a name="p713mcpsimp"></a>NOR_QSPI0_CS</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p715mcpsimp"><a name="p715mcpsimp"></a><a name="p715mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p717mcpsimp"><a name="p717mcpsimp"></a><a name="p717mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p719mcpsimp"><a name="p719mcpsimp"></a><a name="p719mcpsimp"></a>QSPI0_CS</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p1228814811536"><a name="p1228814811536"></a><a name="p1228814811536"></a>-</p>
</td>
</tr>
<tr id="row721mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p723mcpsimp"><a name="p723mcpsimp"></a><a name="p723mcpsimp"></a>30</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p725mcpsimp"><a name="p725mcpsimp"></a><a name="p725mcpsimp"></a>NOR_QSPI0_CLK</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p727mcpsimp"><a name="p727mcpsimp"></a><a name="p727mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p729mcpsimp"><a name="p729mcpsimp"></a><a name="p729mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p731mcpsimp"><a name="p731mcpsimp"></a><a name="p731mcpsimp"></a>QSPI0_CLK</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p112894815311"><a name="p112894815311"></a><a name="p112894815311"></a>-</p>
</td>
</tr>
<tr id="row733mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p735mcpsimp"><a name="p735mcpsimp"></a><a name="p735mcpsimp"></a>108</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p737mcpsimp"><a name="p737mcpsimp"></a><a name="p737mcpsimp"></a>S_HGPIO_0</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p739mcpsimp"><a name="p739mcpsimp"></a><a name="p739mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p741mcpsimp"><a name="p741mcpsimp"></a><a name="p741mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p743mcpsimp"><a name="p743mcpsimp"></a><a name="p743mcpsimp"></a>eMMC_D0/QSPI1_D0</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p18289188125317"><a name="p18289188125317"></a><a name="p18289188125317"></a>-</p>
</td>
</tr>
<tr id="row745mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p747mcpsimp"><a name="p747mcpsimp"></a><a name="p747mcpsimp"></a>105</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p749mcpsimp"><a name="p749mcpsimp"></a><a name="p749mcpsimp"></a>S_HGPIO_1</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p751mcpsimp"><a name="p751mcpsimp"></a><a name="p751mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p753mcpsimp"><a name="p753mcpsimp"></a><a name="p753mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p755mcpsimp"><a name="p755mcpsimp"></a><a name="p755mcpsimp"></a>eMMC_D1/QSPI1_D1</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p116217101533"><a name="p116217101533"></a><a name="p116217101533"></a>-</p>
</td>
</tr>
<tr id="row757mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p759mcpsimp"><a name="p759mcpsimp"></a><a name="p759mcpsimp"></a>102</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p761mcpsimp"><a name="p761mcpsimp"></a><a name="p761mcpsimp"></a>S_HGPIO_2</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p763mcpsimp"><a name="p763mcpsimp"></a><a name="p763mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p765mcpsimp"><a name="p765mcpsimp"></a><a name="p765mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p767mcpsimp"><a name="p767mcpsimp"></a><a name="p767mcpsimp"></a>eMMC_D2/QSPI1_D2</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p51621410155312"><a name="p51621410155312"></a><a name="p51621410155312"></a>-</p>
</td>
</tr>
<tr id="row769mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p771mcpsimp"><a name="p771mcpsimp"></a><a name="p771mcpsimp"></a>104</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p773mcpsimp"><a name="p773mcpsimp"></a><a name="p773mcpsimp"></a>S_HGPIO_3</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p775mcpsimp"><a name="p775mcpsimp"></a><a name="p775mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p777mcpsimp"><a name="p777mcpsimp"></a><a name="p777mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p779mcpsimp"><a name="p779mcpsimp"></a><a name="p779mcpsimp"></a>eMMC_D3/QSPI1_D3</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p1162210185311"><a name="p1162210185311"></a><a name="p1162210185311"></a>-</p>
</td>
</tr>
<tr id="row781mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p783mcpsimp"><a name="p783mcpsimp"></a><a name="p783mcpsimp"></a>103</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p785mcpsimp"><a name="p785mcpsimp"></a><a name="p785mcpsimp"></a>S_HGPIO_4</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p787mcpsimp"><a name="p787mcpsimp"></a><a name="p787mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p789mcpsimp"><a name="p789mcpsimp"></a><a name="p789mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p791mcpsimp"><a name="p791mcpsimp"></a><a name="p791mcpsimp"></a>eMMC_D4/QSPI1_CS</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p1816211010533"><a name="p1816211010533"></a><a name="p1816211010533"></a>-</p>
</td>
</tr>
<tr id="row793mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p795mcpsimp"><a name="p795mcpsimp"></a><a name="p795mcpsimp"></a>18</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p797mcpsimp"><a name="p797mcpsimp"></a><a name="p797mcpsimp"></a>S_HGPIO_5</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p799mcpsimp"><a name="p799mcpsimp"></a><a name="p799mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p801mcpsimp"><a name="p801mcpsimp"></a><a name="p801mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p803mcpsimp"><a name="p803mcpsimp"></a><a name="p803mcpsimp"></a>eMMC_D5</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p1316231035318"><a name="p1316231035318"></a><a name="p1316231035318"></a>-</p>
</td>
</tr>
<tr id="row805mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p807mcpsimp"><a name="p807mcpsimp"></a><a name="p807mcpsimp"></a>23</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p809mcpsimp"><a name="p809mcpsimp"></a><a name="p809mcpsimp"></a>S_HGPIO_6</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p811mcpsimp"><a name="p811mcpsimp"></a><a name="p811mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p813mcpsimp"><a name="p813mcpsimp"></a><a name="p813mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p815mcpsimp"><a name="p815mcpsimp"></a><a name="p815mcpsimp"></a>eMMC_D6</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p2162510125310"><a name="p2162510125310"></a><a name="p2162510125310"></a>-</p>
</td>
</tr>
<tr id="row817mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p819mcpsimp"><a name="p819mcpsimp"></a><a name="p819mcpsimp"></a>25</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p821mcpsimp"><a name="p821mcpsimp"></a><a name="p821mcpsimp"></a>S_HGPIO_7</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p823mcpsimp"><a name="p823mcpsimp"></a><a name="p823mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p825mcpsimp"><a name="p825mcpsimp"></a><a name="p825mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p827mcpsimp"><a name="p827mcpsimp"></a><a name="p827mcpsimp"></a>eMMC_D7</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p1516219101539"><a name="p1516219101539"></a><a name="p1516219101539"></a>-</p>
</td>
</tr>
<tr id="row829mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p831mcpsimp"><a name="p831mcpsimp"></a><a name="p831mcpsimp"></a>17</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p833mcpsimp"><a name="p833mcpsimp"></a><a name="p833mcpsimp"></a>S_HGPIO_8</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p835mcpsimp"><a name="p835mcpsimp"></a><a name="p835mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p837mcpsimp"><a name="p837mcpsimp"></a><a name="p837mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p839mcpsimp"><a name="p839mcpsimp"></a><a name="p839mcpsimp"></a>eMMC_CMD</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p71624108530"><a name="p71624108530"></a><a name="p71624108530"></a>-</p>
</td>
</tr>
<tr id="row841mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p843mcpsimp"><a name="p843mcpsimp"></a><a name="p843mcpsimp"></a>106</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p845mcpsimp"><a name="p845mcpsimp"></a><a name="p845mcpsimp"></a>S_HGPIO_9</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p847mcpsimp"><a name="p847mcpsimp"></a><a name="p847mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p849mcpsimp"><a name="p849mcpsimp"></a><a name="p849mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p851mcpsimp"><a name="p851mcpsimp"></a><a name="p851mcpsimp"></a>eMMC_CLK/QSPI1_CLK</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p6163101012538"><a name="p6163101012538"></a><a name="p6163101012538"></a>-</p>
</td>
</tr>
<tr id="row853mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p855mcpsimp"><a name="p855mcpsimp"></a><a name="p855mcpsimp"></a>46</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p857mcpsimp"><a name="p857mcpsimp"></a><a name="p857mcpsimp"></a>S_AGPIO_0</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p859mcpsimp"><a name="p859mcpsimp"></a><a name="p859mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p861mcpsimp"><a name="p861mcpsimp"></a><a name="p861mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p863mcpsimp"><a name="p863mcpsimp"></a><a name="p863mcpsimp"></a>BT_WIFI_SW/I2C0_SCL</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry864mcpsimpp0"><a name="entry864mcpsimpp0"></a><a name="entry864mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row865mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p867mcpsimp"><a name="p867mcpsimp"></a><a name="p867mcpsimp"></a>15</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p869mcpsimp"><a name="p869mcpsimp"></a><a name="p869mcpsimp"></a>S_AGPIO_1</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p871mcpsimp"><a name="p871mcpsimp"></a><a name="p871mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p873mcpsimp"><a name="p873mcpsimp"></a><a name="p873mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p875mcpsimp"><a name="p875mcpsimp"></a><a name="p875mcpsimp"></a>RF_TEST_IP/BT_ACT/I2C0_SDA</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry876mcpsimpp0"><a name="entry876mcpsimpp0"></a><a name="entry876mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row877mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p879mcpsimp"><a name="p879mcpsimp"></a><a name="p879mcpsimp"></a>35</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p881mcpsimp"><a name="p881mcpsimp"></a><a name="p881mcpsimp"></a>S_AGPIO_2</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p883mcpsimp"><a name="p883mcpsimp"></a><a name="p883mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p885mcpsimp"><a name="p885mcpsimp"></a><a name="p885mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p887mcpsimp"><a name="p887mcpsimp"></a><a name="p887mcpsimp"></a>BTS_SAMPLE/I2C1_SCL</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry888mcpsimpp0"><a name="entry888mcpsimpp0"></a><a name="entry888mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row889mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p891mcpsimp"><a name="p891mcpsimp"></a><a name="p891mcpsimp"></a>51</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p893mcpsimp"><a name="p893mcpsimp"></a><a name="p893mcpsimp"></a>S_AGPIO_6</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p895mcpsimp"><a name="p895mcpsimp"></a><a name="p895mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p897mcpsimp"><a name="p897mcpsimp"></a><a name="p897mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p899mcpsimp"><a name="p899mcpsimp"></a><a name="p899mcpsimp"></a>WLAN_ACT/CLK_IN</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry900mcpsimpp0"><a name="entry900mcpsimpp0"></a><a name="entry900mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row901mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p903mcpsimp"><a name="p903mcpsimp"></a><a name="p903mcpsimp"></a>14</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p905mcpsimp"><a name="p905mcpsimp"></a><a name="p905mcpsimp"></a>S_AGPIO_7</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p907mcpsimp"><a name="p907mcpsimp"></a><a name="p907mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p909mcpsimp"><a name="p909mcpsimp"></a><a name="p909mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p911mcpsimp"><a name="p911mcpsimp"></a><a name="p911mcpsimp"></a>UART3_TXD/I2C3_SCL</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry912mcpsimpp0"><a name="entry912mcpsimpp0"></a><a name="entry912mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row913mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p915mcpsimp"><a name="p915mcpsimp"></a><a name="p915mcpsimp"></a>52</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p917mcpsimp"><a name="p917mcpsimp"></a><a name="p917mcpsimp"></a>S_AGPIO_8</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p919mcpsimp"><a name="p919mcpsimp"></a><a name="p919mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p921mcpsimp"><a name="p921mcpsimp"></a><a name="p921mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p923mcpsimp"><a name="p923mcpsimp"></a><a name="p923mcpsimp"></a>UART3_RXD/I2C3_SDA</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p925mcpsimp"><a name="p925mcpsimp"></a><a name="p925mcpsimp"></a>AUX_ADC_CH_6</p>
</td>
</tr>
<tr id="row926mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p928mcpsimp"><a name="p928mcpsimp"></a><a name="p928mcpsimp"></a>16</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p930mcpsimp"><a name="p930mcpsimp"></a><a name="p930mcpsimp"></a>S_AGPIO_10</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p932mcpsimp"><a name="p932mcpsimp"></a><a name="p932mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p934mcpsimp"><a name="p934mcpsimp"></a><a name="p934mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p936mcpsimp"><a name="p936mcpsimp"></a><a name="p936mcpsimp"></a>CLKOUT1/UART3_RXD</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry937mcpsimpp0"><a name="entry937mcpsimpp0"></a><a name="entry937mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row938mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p940mcpsimp"><a name="p940mcpsimp"></a><a name="p940mcpsimp"></a>44</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p942mcpsimp"><a name="p942mcpsimp"></a><a name="p942mcpsimp"></a>S_AGPIO_11</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p944mcpsimp"><a name="p944mcpsimp"></a><a name="p944mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p946mcpsimp"><a name="p946mcpsimp"></a><a name="p946mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p948mcpsimp"><a name="p948mcpsimp"></a><a name="p948mcpsimp"></a>UART4_TXD/APWM1_P</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p950mcpsimp"><a name="p950mcpsimp"></a><a name="p950mcpsimp"></a>AUX_ADC_CH_4</p>
</td>
</tr>
<tr id="row951mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p953mcpsimp"><a name="p953mcpsimp"></a><a name="p953mcpsimp"></a>49</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p955mcpsimp"><a name="p955mcpsimp"></a><a name="p955mcpsimp"></a>S_AGPIO_12</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p957mcpsimp"><a name="p957mcpsimp"></a><a name="p957mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p959mcpsimp"><a name="p959mcpsimp"></a><a name="p959mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p961mcpsimp"><a name="p961mcpsimp"></a><a name="p961mcpsimp"></a>UART4_RXD/APWM1_N</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry962mcpsimpp0"><a name="entry962mcpsimpp0"></a><a name="entry962mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row963mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p965mcpsimp"><a name="p965mcpsimp"></a><a name="p965mcpsimp"></a>73</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p967mcpsimp"><a name="p967mcpsimp"></a><a name="p967mcpsimp"></a>S_AGPIO_13</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p969mcpsimp"><a name="p969mcpsimp"></a><a name="p969mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p971mcpsimp"><a name="p971mcpsimp"></a><a name="p971mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p973mcpsimp"><a name="p973mcpsimp"></a><a name="p973mcpsimp"></a>UART4_CTS/CLKOUT0</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry974mcpsimpp0"><a name="entry974mcpsimpp0"></a><a name="entry974mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row975mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p977mcpsimp"><a name="p977mcpsimp"></a><a name="p977mcpsimp"></a>42</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p979mcpsimp"><a name="p979mcpsimp"></a><a name="p979mcpsimp"></a>S_AGPIO_14</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p981mcpsimp"><a name="p981mcpsimp"></a><a name="p981mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p983mcpsimp"><a name="p983mcpsimp"></a><a name="p983mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p985mcpsimp"><a name="p985mcpsimp"></a><a name="p985mcpsimp"></a>UART4_RTS/CLKOUT1</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry986mcpsimpp0"><a name="entry986mcpsimpp0"></a><a name="entry986mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row987mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p989mcpsimp"><a name="p989mcpsimp"></a><a name="p989mcpsimp"></a>34</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p991mcpsimp"><a name="p991mcpsimp"></a><a name="p991mcpsimp"></a>S_AGPIO_15</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p993mcpsimp"><a name="p993mcpsimp"></a><a name="p993mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p995mcpsimp"><a name="p995mcpsimp"></a><a name="p995mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p997mcpsimp"><a name="p997mcpsimp"></a><a name="p997mcpsimp"></a>APWM0_P/FEM_CTRL3</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p999mcpsimp"><a name="p999mcpsimp"></a><a name="p999mcpsimp"></a>AUX_ADC_CH_5</p>
</td>
</tr>
<tr id="row1000mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p1002mcpsimp"><a name="p1002mcpsimp"></a><a name="p1002mcpsimp"></a>41</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p1004mcpsimp"><a name="p1004mcpsimp"></a><a name="p1004mcpsimp"></a>S_AGPIO_16</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p1006mcpsimp"><a name="p1006mcpsimp"></a><a name="p1006mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p1008mcpsimp"><a name="p1008mcpsimp"></a><a name="p1008mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p1010mcpsimp"><a name="p1010mcpsimp"></a><a name="p1010mcpsimp"></a>APWM0_N/FEM_CTRL2</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p1012mcpsimp"><a name="p1012mcpsimp"></a><a name="p1012mcpsimp"></a>AUX_ADC_CH_7</p>
</td>
</tr>
<tr id="row1013mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p1015mcpsimp"><a name="p1015mcpsimp"></a><a name="p1015mcpsimp"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p1017mcpsimp"><a name="p1017mcpsimp"></a><a name="p1017mcpsimp"></a>S_AGPIO_17</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p1019mcpsimp"><a name="p1019mcpsimp"></a><a name="p1019mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p1021mcpsimp"><a name="p1021mcpsimp"></a><a name="p1021mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p1023mcpsimp"><a name="p1023mcpsimp"></a><a name="p1023mcpsimp"></a>GLP_SYNCP/USB_OVR_CUR</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p415081995310"><a name="p415081995310"></a><a name="p415081995310"></a>-</p>
</td>
</tr>
<tr id="row1025mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p1027mcpsimp"><a name="p1027mcpsimp"></a><a name="p1027mcpsimp"></a>31</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p1029mcpsimp"><a name="p1029mcpsimp"></a><a name="p1029mcpsimp"></a>S_AGPIO_18</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p1031mcpsimp"><a name="p1031mcpsimp"></a><a name="p1031mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p1033mcpsimp"><a name="p1033mcpsimp"></a><a name="p1033mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p1035mcpsimp"><a name="p1035mcpsimp"></a><a name="p1035mcpsimp"></a>USB_VBUS/DIAG0</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p015020197532"><a name="p015020197532"></a><a name="p015020197532"></a>-</p>
</td>
</tr>
<tr id="row1037mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p1039mcpsimp"><a name="p1039mcpsimp"></a><a name="p1039mcpsimp"></a>43</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p1041mcpsimp"><a name="p1041mcpsimp"></a><a name="p1041mcpsimp"></a>S_AGPIO_19</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p1043mcpsimp"><a name="p1043mcpsimp"></a><a name="p1043mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p1045mcpsimp"><a name="p1045mcpsimp"></a><a name="p1045mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p1047mcpsimp"><a name="p1047mcpsimp"></a><a name="p1047mcpsimp"></a>DSI_TE</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p1615019195533"><a name="p1615019195533"></a><a name="p1615019195533"></a>-</p>
</td>
</tr>
<tr id="row1049mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p1051mcpsimp"><a name="p1051mcpsimp"></a><a name="p1051mcpsimp"></a>90</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p1053mcpsimp"><a name="p1053mcpsimp"></a><a name="p1053mcpsimp"></a>S_AGPIO_20</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p1055mcpsimp"><a name="p1055mcpsimp"></a><a name="p1055mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p1057mcpsimp"><a name="p1057mcpsimp"></a><a name="p1057mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p1059mcpsimp"><a name="p1059mcpsimp"></a><a name="p1059mcpsimp"></a>USB_PWR_EN</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p9150619125310"><a name="p9150619125310"></a><a name="p9150619125310"></a>-</p>
</td>
</tr>
<tr id="row1061mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p1063mcpsimp"><a name="p1063mcpsimp"></a><a name="p1063mcpsimp"></a>82</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p1065mcpsimp"><a name="p1065mcpsimp"></a><a name="p1065mcpsimp"></a>S_AGPIO_21</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p1067mcpsimp"><a name="p1067mcpsimp"></a><a name="p1067mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p1069mcpsimp"><a name="p1069mcpsimp"></a><a name="p1069mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p1071mcpsimp"><a name="p1071mcpsimp"></a><a name="p1071mcpsimp"></a>I2S0_CLK</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p5150191915315"><a name="p5150191915315"></a><a name="p5150191915315"></a>-</p>
</td>
</tr>
<tr id="row1073mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p1075mcpsimp"><a name="p1075mcpsimp"></a><a name="p1075mcpsimp"></a>64</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p1077mcpsimp"><a name="p1077mcpsimp"></a><a name="p1077mcpsimp"></a>S_AGPIO_22</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p1079mcpsimp"><a name="p1079mcpsimp"></a><a name="p1079mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p1081mcpsimp"><a name="p1081mcpsimp"></a><a name="p1081mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p1083mcpsimp"><a name="p1083mcpsimp"></a><a name="p1083mcpsimp"></a>I2S0_DO</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p3150419155313"><a name="p3150419155313"></a><a name="p3150419155313"></a>-</p>
</td>
</tr>
<tr id="row1085mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p1087mcpsimp"><a name="p1087mcpsimp"></a><a name="p1087mcpsimp"></a>74</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p1089mcpsimp"><a name="p1089mcpsimp"></a><a name="p1089mcpsimp"></a>S_AGPIO_23</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p1091mcpsimp"><a name="p1091mcpsimp"></a><a name="p1091mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p1093mcpsimp"><a name="p1093mcpsimp"></a><a name="p1093mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p1095mcpsimp"><a name="p1095mcpsimp"></a><a name="p1095mcpsimp"></a>I2S0_DI</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p16150111995319"><a name="p16150111995319"></a><a name="p16150111995319"></a>-</p>
</td>
</tr>
<tr id="row1097mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p1099mcpsimp"><a name="p1099mcpsimp"></a><a name="p1099mcpsimp"></a>76</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p1101mcpsimp"><a name="p1101mcpsimp"></a><a name="p1101mcpsimp"></a>S_AGPIO_24</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p1103mcpsimp"><a name="p1103mcpsimp"></a><a name="p1103mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p1105mcpsimp"><a name="p1105mcpsimp"></a><a name="p1105mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p1107mcpsimp"><a name="p1107mcpsimp"></a><a name="p1107mcpsimp"></a>I2S0_WS</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p915019193534"><a name="p915019193534"></a><a name="p915019193534"></a>-</p>
</td>
</tr>
<tr id="row1109mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p1111mcpsimp"><a name="p1111mcpsimp"></a><a name="p1111mcpsimp"></a>84</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p1113mcpsimp"><a name="p1113mcpsimp"></a><a name="p1113mcpsimp"></a>S_AGPIO_27</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p1115mcpsimp"><a name="p1115mcpsimp"></a><a name="p1115mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p1117mcpsimp"><a name="p1117mcpsimp"></a><a name="p1117mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p1119mcpsimp"><a name="p1119mcpsimp"></a><a name="p1119mcpsimp"></a>I2S1_DI</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="p6151201917532"><a name="p6151201917532"></a><a name="p6151201917532"></a>-</p>
</td>
</tr>
<tr id="row1121mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p1123mcpsimp"><a name="p1123mcpsimp"></a><a name="p1123mcpsimp"></a>70</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p1125mcpsimp"><a name="p1125mcpsimp"></a><a name="p1125mcpsimp"></a>S_AGPIO_28</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p1127mcpsimp"><a name="p1127mcpsimp"></a><a name="p1127mcpsimp"></a>IOLDO1（1.8V）)</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p1129mcpsimp"><a name="p1129mcpsimp"></a><a name="p1129mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p1131mcpsimp"><a name="p1131mcpsimp"></a><a name="p1131mcpsimp"></a>I2S1_WS</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry1132mcpsimpp0"><a name="entry1132mcpsimpp0"></a><a name="entry1132mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row1133mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p1135mcpsimp"><a name="p1135mcpsimp"></a><a name="p1135mcpsimp"></a>67</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p1137mcpsimp"><a name="p1137mcpsimp"></a><a name="p1137mcpsimp"></a>S_AGPIO_29</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p1139mcpsimp"><a name="p1139mcpsimp"></a><a name="p1139mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p1141mcpsimp"><a name="p1141mcpsimp"></a><a name="p1141mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p1143mcpsimp"><a name="p1143mcpsimp"></a><a name="p1143mcpsimp"></a>PDM_RX_CLK</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry1144mcpsimpp0"><a name="entry1144mcpsimpp0"></a><a name="entry1144mcpsimpp0"></a>-</p>
</td>
</tr>
<tr id="row1145mcpsimp"><td class="cellrowborder" valign="top" width="11.341134113411341%" headers="mcps1.2.7.1.1 "><p id="p1147mcpsimp"><a name="p1147mcpsimp"></a><a name="p1147mcpsimp"></a>72</p>
</td>
<td class="cellrowborder" valign="top" width="20.432043204320433%" headers="mcps1.2.7.1.2 "><p id="p1149mcpsimp"><a name="p1149mcpsimp"></a><a name="p1149mcpsimp"></a>S_AGPIO_30</p>
</td>
<td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.2.7.1.3 "><p id="p1151mcpsimp"><a name="p1151mcpsimp"></a><a name="p1151mcpsimp"></a>IOLDO1（1.8V）</p>
</td>
<td class="cellrowborder" valign="top" width="8.6008600860086%" headers="mcps1.2.7.1.4 "><p id="p1153mcpsimp"><a name="p1153mcpsimp"></a><a name="p1153mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="26.05260526052605%" headers="mcps1.2.7.1.5 "><p id="p1155mcpsimp"><a name="p1155mcpsimp"></a><a name="p1155mcpsimp"></a>PDM_RX_DI</p>
</td>
<td class="cellrowborder" valign="top" width="12.871287128712872%" headers="mcps1.2.7.1.6 "><p id="entry1156mcpsimpp0"><a name="entry1156mcpsimpp0"></a><a name="entry1156mcpsimpp0"></a>-</p>
</td>
</tr>
</tbody>
</table>

>![](public_sys-resources/icon-note.gif) **说明：** 
>S\_AGPIO18为USB插入检测管脚，使用USB功能时需对接分压后的USB供电电源平面。

### 维测管脚<a name="ZH-CN_TOPIC_0000002545901585"></a>

模组内有4个管脚用于CAT1维测，1个管脚用于GNSS维测，详细的接口分配管脚如[表1](#_Ref218688493)所示。

**表 1**  维测管脚列表

<a name="_Ref218688493"></a>
<table><thead align="left"><tr id="row3004mcpsimp"><th class="cellrowborder" valign="top" width="15.46154615461546%" id="mcps1.2.5.1.1"><p id="p3006mcpsimp"><a name="p3006mcpsimp"></a><a name="p3006mcpsimp"></a>管脚编号</p>
</th>
<th class="cellrowborder" valign="top" width="23.152315231523154%" id="mcps1.2.5.1.2"><p id="p3008mcpsimp"><a name="p3008mcpsimp"></a><a name="p3008mcpsimp"></a>管脚名称</p>
</th>
<th class="cellrowborder" valign="top" width="14.85148514851485%" id="mcps1.2.5.1.3"><p id="p3010mcpsimp"><a name="p3010mcpsimp"></a><a name="p3010mcpsimp"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="46.53465346534653%" id="mcps1.2.5.1.4"><p id="p3012mcpsimp"><a name="p3012mcpsimp"></a><a name="p3012mcpsimp"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row3014mcpsimp"><td class="cellrowborder" valign="top" width="15.46154615461546%" headers="mcps1.2.5.1.1 "><p id="p3016mcpsimp"><a name="p3016mcpsimp"></a><a name="p3016mcpsimp"></a>124</p>
</td>
<td class="cellrowborder" valign="top" width="23.152315231523154%" headers="mcps1.2.5.1.2 "><p id="p3018mcpsimp"><a name="p3018mcpsimp"></a><a name="p3018mcpsimp"></a>DEBUG_TXD</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p3020mcpsimp"><a name="p3020mcpsimp"></a><a name="p3020mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p3022mcpsimp"><a name="p3022mcpsimp"></a><a name="p3022mcpsimp"></a>CAT1维测。</p>
</td>
</tr>
<tr id="row3023mcpsimp"><td class="cellrowborder" valign="top" width="15.46154615461546%" headers="mcps1.2.5.1.1 "><p id="p3025mcpsimp"><a name="p3025mcpsimp"></a><a name="p3025mcpsimp"></a>123</p>
</td>
<td class="cellrowborder" valign="top" width="23.152315231523154%" headers="mcps1.2.5.1.2 "><p id="p3027mcpsimp"><a name="p3027mcpsimp"></a><a name="p3027mcpsimp"></a>DEBUG_RXD</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p3029mcpsimp"><a name="p3029mcpsimp"></a><a name="p3029mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p3031mcpsimp"><a name="p3031mcpsimp"></a><a name="p3031mcpsimp"></a>CAT1维测。</p>
</td>
</tr>
<tr id="row3032mcpsimp"><td class="cellrowborder" valign="top" width="15.46154615461546%" headers="mcps1.2.5.1.1 "><p id="p3034mcpsimp"><a name="p3034mcpsimp"></a><a name="p3034mcpsimp"></a>121</p>
</td>
<td class="cellrowborder" valign="top" width="23.152315231523154%" headers="mcps1.2.5.1.2 "><p id="p3036mcpsimp"><a name="p3036mcpsimp"></a><a name="p3036mcpsimp"></a>DEBUG_CTS</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p3038mcpsimp"><a name="p3038mcpsimp"></a><a name="p3038mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p3040mcpsimp"><a name="p3040mcpsimp"></a><a name="p3040mcpsimp"></a>CAT1维测。</p>
</td>
</tr>
<tr id="row3041mcpsimp"><td class="cellrowborder" valign="top" width="15.46154615461546%" headers="mcps1.2.5.1.1 "><p id="p3043mcpsimp"><a name="p3043mcpsimp"></a><a name="p3043mcpsimp"></a>122</p>
</td>
<td class="cellrowborder" valign="top" width="23.152315231523154%" headers="mcps1.2.5.1.2 "><p id="p3045mcpsimp"><a name="p3045mcpsimp"></a><a name="p3045mcpsimp"></a>DEBUG_RTS</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p3047mcpsimp"><a name="p3047mcpsimp"></a><a name="p3047mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p3049mcpsimp"><a name="p3049mcpsimp"></a><a name="p3049mcpsimp"></a>CAT1维测。</p>
</td>
</tr>
<tr id="row3050mcpsimp"><td class="cellrowborder" valign="top" width="15.46154615461546%" headers="mcps1.2.5.1.1 "><p id="p3052mcpsimp"><a name="p3052mcpsimp"></a><a name="p3052mcpsimp"></a>125</p>
</td>
<td class="cellrowborder" valign="top" width="23.152315231523154%" headers="mcps1.2.5.1.2 "><p id="p3054mcpsimp"><a name="p3054mcpsimp"></a><a name="p3054mcpsimp"></a>B4_UART1_TXD</p>
</td>
<td class="cellrowborder" valign="top" width="14.85148514851485%" headers="mcps1.2.5.1.3 "><p id="p3056mcpsimp"><a name="p3056mcpsimp"></a><a name="p3056mcpsimp"></a>IO</p>
</td>
<td class="cellrowborder" valign="top" width="46.53465346534653%" headers="mcps1.2.5.1.4 "><p id="p3058mcpsimp"><a name="p3058mcpsimp"></a><a name="p3058mcpsimp"></a>GNSS维测。</p>
</td>
</tr>
</tbody>
</table>

# 原理图设计指导<a name="ZH-CN_TOPIC_0000002514301642"></a>

-   **[模组内模块电路框图](#ZH-CN_TOPIC_0000002549710745)**  

-   **[电源参考设计](#ZH-CN_TOPIC_0000002545781595)**  

-   **[射频电路设计](#ZH-CN_TOPIC_0000002514141720)**  

-   **[接口参考设计](#ZH-CN_TOPIC_0000002514301644)**  

-   **[下载调测接口](#ZH-CN_TOPIC_0000002545781611)**  

## 模组内模块电路框图<a name="ZH-CN_TOPIC_0000002549710745"></a>

模组内模块电路框图如[图1](#fig8430125517115)所示。

**图 1**  模组内模块电路框图<a name="fig8430125517115"></a>  
![](figures/模组内模块电路框图.png "模组内模块电路框图")

## 电源参考设计<a name="ZH-CN_TOPIC_0000002545781595"></a>

本小节介绍模组电源参考设计，模组内芯片的电源管理模块为芯片其他模块提供所需要的电源。电源供电框图如[图1](#fig887135116260)所示，上电检测方案如[图2](#fig110712498194)所示，原理图设计时建议采用相同方案。

**图 1**  模组供电框图<a name="fig887135116260"></a>  
![](figures/模组供电框图.png "模组供电框图")

**图 2**  开机电路参考<a name="fig110712498194"></a>  
![](figures/开机电路参考.png "开机电路参考")

![](figures/zh-cn_image_0000002514141754.png)

>![](public_sys-resources/icon-note.gif) **说明：** 
>VBUS\_SENSE为充电开机上电检测管脚，高电平有效。高电平有效范围：0.8V<VBUS\_SENSE<1.8V，通过电阻分压方式实现上电开机功能。
>PWR\_KEY为按键开机上电检测管脚，下降沿有效。芯片内部上拉，板级通过按键实现开机功能。

## 射频电路设计<a name="ZH-CN_TOPIC_0000002514141720"></a>

-   **[RFIO电路](#ZH-CN_TOPIC_0000002545901589)**  

### RFIO电路<a name="ZH-CN_TOPIC_0000002545901589"></a>

GNSS、CAT1天线连接参考电路设计请参考[图1](#fig161813557119)所示。

**图 1**  RF参考电路图<a name="fig161813557119"></a>  
![](figures/RF参考电路图.png "RF参考电路图")

>![](public_sys-resources/icon-note.gif) **说明：** 
>红色方框为选焊器件。
>滤波网络的电容/电感取值需要根据PCB、整机天线等环境调试得到。

## 接口参考设计<a name="ZH-CN_TOPIC_0000002514301644"></a>

-   **[GPIO口](#ZH-CN_TOPIC_0000002545781599)**  

-   **[QSPI接口](#ZH-CN_TOPIC_0000002514141724)**  

-   **[SPI接口](#ZH-CN_TOPIC_0000002545901591)**  

-   **[UART接口](#ZH-CN_TOPIC_0000002514301648)**  

-   **[I2C接口](#ZH-CN_TOPIC_0000002545781601)**  

-   **[eMMC](#ZH-CN_TOPIC_0000002514141726)**  

-   **[SDIO](#ZH-CN_TOPIC_0000002545901595)**  

-   **[PWM接口](#ZH-CN_TOPIC_0000002514301650)**  

-   **[MIPI接口](#ZH-CN_TOPIC_0000002545781605)**  

-   **[ADC接口](#ZH-CN_TOPIC_0000002514141728)**  

-   **[USB电路设计](#ZH-CN_TOPIC_0000002545901597)**  

-   **[I2S接口](#ZH-CN_TOPIC_0000002514301654)**  

-   **[PDM输入接口](#ZH-CN_TOPIC_0000002545781607)**  

-   **[AMIC接口](#ZH-CN_TOPIC_0000002514141732)**  

-   **[VICAP接口](#ZH-CN_TOPIC_0000002545901601)**  

-   **[HS音频输出接口](#ZH-CN_TOPIC_0000002514301658)**  

### GPIO口<a name="ZH-CN_TOPIC_0000002545781599"></a>

模组有70个IO口，每个都可以配置为GPIO口模式。用于生成或采集特性应用的输出或输入信号。

-   GPIO每个管脚可以独立配置为输入或者输出。
    -   作为输入管脚时，可作用为中断源。
    -   作为输出管脚时，每个GPIO都可以独立输出高电平、低电平。

-   每个管脚都可独立配置上下拉、驱动等参数。

1.8V IO口DC特性可参考[表1](#__Ref8920216)所示。

**表 1**  1.8V IO口DC特性

<a name="__Ref8920216"></a>
<table><thead align="left"><tr id="row1319mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.2.8.1.1"><p id="p1321mcpsimp"><a name="p1321mcpsimp"></a><a name="p1321mcpsimp"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="17%" id="mcps1.2.8.1.2"><p id="p1323mcpsimp"><a name="p1323mcpsimp"></a><a name="p1323mcpsimp"></a>标识</p>
</th>
<th class="cellrowborder" valign="top" width="14.32%" id="mcps1.2.8.1.3"><p id="p1325mcpsimp"><a name="p1325mcpsimp"></a><a name="p1325mcpsimp"></a>测试条件</p>
</th>
<th class="cellrowborder" valign="top" width="15.659999999999998%" id="mcps1.2.8.1.4"><p id="p1327mcpsimp"><a name="p1327mcpsimp"></a><a name="p1327mcpsimp"></a>最小值</p>
</th>
<th class="cellrowborder" valign="top" width="13.81%" id="mcps1.2.8.1.5"><p id="p1329mcpsimp"><a name="p1329mcpsimp"></a><a name="p1329mcpsimp"></a>典型值</p>
</th>
<th class="cellrowborder" valign="top" width="16.21%" id="mcps1.2.8.1.6"><p id="p1331mcpsimp"><a name="p1331mcpsimp"></a><a name="p1331mcpsimp"></a>最大值</p>
</th>
<th class="cellrowborder" valign="top" width="7.000000000000001%" id="mcps1.2.8.1.7"><p id="p1333mcpsimp"><a name="p1333mcpsimp"></a><a name="p1333mcpsimp"></a>单位</p>
</th>
</tr>
</thead>
<tbody><tr id="row1335mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.2.8.1.1 "><p id="p1337mcpsimp"><a name="p1337mcpsimp"></a><a name="p1337mcpsimp"></a>I/O电压</p>
</td>
<td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.8.1.2 "><p id="p1339mcpsimp"><a name="p1339mcpsimp"></a><a name="p1339mcpsimp"></a>IOLDO1/IOLDO4</p>
</td>
<td class="cellrowborder" valign="top" width="14.32%" headers="mcps1.2.8.1.3 "><p id="p1341mcpsimp"><a name="p1341mcpsimp"></a><a name="p1341mcpsimp"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="15.659999999999998%" headers="mcps1.2.8.1.4 "><p id="p1343mcpsimp"><a name="p1343mcpsimp"></a><a name="p1343mcpsimp"></a>1.62</p>
</td>
<td class="cellrowborder" valign="top" width="13.81%" headers="mcps1.2.8.1.5 "><p id="p1345mcpsimp"><a name="p1345mcpsimp"></a><a name="p1345mcpsimp"></a>1.8</p>
</td>
<td class="cellrowborder" valign="top" width="16.21%" headers="mcps1.2.8.1.6 "><p id="p1347mcpsimp"><a name="p1347mcpsimp"></a><a name="p1347mcpsimp"></a>1.98</p>
</td>
<td class="cellrowborder" valign="top" width="7.000000000000001%" headers="mcps1.2.8.1.7 "><p id="p1349mcpsimp"><a name="p1349mcpsimp"></a><a name="p1349mcpsimp"></a>V</p>
</td>
</tr>
<tr id="row1350mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.2.8.1.1 "><p id="p1352mcpsimp"><a name="p1352mcpsimp"></a><a name="p1352mcpsimp"></a>输入高电平</p>
</td>
<td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.8.1.2 "><p id="p1354mcpsimp"><a name="p1354mcpsimp"></a><a name="p1354mcpsimp"></a>V<sub id="sub1355mcpsimp"><a name="sub1355mcpsimp"></a><a name="sub1355mcpsimp"></a>IH</sub></p>
</td>
<td class="cellrowborder" valign="top" width="14.32%" headers="mcps1.2.8.1.3 "><p id="p1357mcpsimp"><a name="p1357mcpsimp"></a><a name="p1357mcpsimp"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="15.659999999999998%" headers="mcps1.2.8.1.4 "><p id="p1359mcpsimp"><a name="p1359mcpsimp"></a><a name="p1359mcpsimp"></a>0.65×VDDIO</p>
</td>
<td class="cellrowborder" valign="top" width="13.81%" headers="mcps1.2.8.1.5 "><p id="p1361mcpsimp"><a name="p1361mcpsimp"></a><a name="p1361mcpsimp"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="16.21%" headers="mcps1.2.8.1.6 "><p id="p1363mcpsimp"><a name="p1363mcpsimp"></a><a name="p1363mcpsimp"></a>1.98＋0.3</p>
</td>
<td class="cellrowborder" valign="top" width="7.000000000000001%" headers="mcps1.2.8.1.7 "><p id="p1365mcpsimp"><a name="p1365mcpsimp"></a><a name="p1365mcpsimp"></a>V</p>
</td>
</tr>
<tr id="row1366mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.2.8.1.1 "><p id="p1368mcpsimp"><a name="p1368mcpsimp"></a><a name="p1368mcpsimp"></a>输入低电平</p>
</td>
<td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.8.1.2 "><p id="p1370mcpsimp"><a name="p1370mcpsimp"></a><a name="p1370mcpsimp"></a>V<sub id="sub1371mcpsimp"><a name="sub1371mcpsimp"></a><a name="sub1371mcpsimp"></a>IL</sub></p>
</td>
<td class="cellrowborder" valign="top" width="14.32%" headers="mcps1.2.8.1.3 "><p id="p1373mcpsimp"><a name="p1373mcpsimp"></a><a name="p1373mcpsimp"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="15.659999999999998%" headers="mcps1.2.8.1.4 "><p id="p1375mcpsimp"><a name="p1375mcpsimp"></a><a name="p1375mcpsimp"></a>-0.3</p>
</td>
<td class="cellrowborder" valign="top" width="13.81%" headers="mcps1.2.8.1.5 "><p id="p1377mcpsimp"><a name="p1377mcpsimp"></a><a name="p1377mcpsimp"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="16.21%" headers="mcps1.2.8.1.6 "><p id="p1379mcpsimp"><a name="p1379mcpsimp"></a><a name="p1379mcpsimp"></a>0.35×VDDIO</p>
</td>
<td class="cellrowborder" valign="top" width="7.000000000000001%" headers="mcps1.2.8.1.7 "><p id="p1381mcpsimp"><a name="p1381mcpsimp"></a><a name="p1381mcpsimp"></a>V</p>
</td>
</tr>
<tr id="row1382mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.2.8.1.1 "><p id="p1384mcpsimp"><a name="p1384mcpsimp"></a><a name="p1384mcpsimp"></a>输出高电平</p>
</td>
<td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.8.1.2 "><p id="p1386mcpsimp"><a name="p1386mcpsimp"></a><a name="p1386mcpsimp"></a>V<sub id="sub1387mcpsimp"><a name="sub1387mcpsimp"></a><a name="sub1387mcpsimp"></a>OH</sub></p>
</td>
<td class="cellrowborder" valign="top" width="14.32%" headers="mcps1.2.8.1.3 "><p id="p1389mcpsimp"><a name="p1389mcpsimp"></a><a name="p1389mcpsimp"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="15.659999999999998%" headers="mcps1.2.8.1.4 "><p id="p1391mcpsimp"><a name="p1391mcpsimp"></a><a name="p1391mcpsimp"></a>0.75×VDDIO</p>
</td>
<td class="cellrowborder" valign="top" width="13.81%" headers="mcps1.2.8.1.5 "><p id="p1393mcpsimp"><a name="p1393mcpsimp"></a><a name="p1393mcpsimp"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="16.21%" headers="mcps1.2.8.1.6 "><p id="p1395mcpsimp"><a name="p1395mcpsimp"></a><a name="p1395mcpsimp"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="7.000000000000001%" headers="mcps1.2.8.1.7 "><p id="p1397mcpsimp"><a name="p1397mcpsimp"></a><a name="p1397mcpsimp"></a>V</p>
</td>
</tr>
<tr id="row1398mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.2.8.1.1 "><p id="p1400mcpsimp"><a name="p1400mcpsimp"></a><a name="p1400mcpsimp"></a>输出低电平</p>
</td>
<td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.8.1.2 "><p id="p1402mcpsimp"><a name="p1402mcpsimp"></a><a name="p1402mcpsimp"></a>V<sub id="sub1403mcpsimp"><a name="sub1403mcpsimp"></a><a name="sub1403mcpsimp"></a>OL</sub></p>
</td>
<td class="cellrowborder" valign="top" width="14.32%" headers="mcps1.2.8.1.3 "><p id="p1405mcpsimp"><a name="p1405mcpsimp"></a><a name="p1405mcpsimp"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="15.659999999999998%" headers="mcps1.2.8.1.4 "><p id="p1407mcpsimp"><a name="p1407mcpsimp"></a><a name="p1407mcpsimp"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="13.81%" headers="mcps1.2.8.1.5 "><p id="p1409mcpsimp"><a name="p1409mcpsimp"></a><a name="p1409mcpsimp"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="16.21%" headers="mcps1.2.8.1.6 "><p id="p1411mcpsimp"><a name="p1411mcpsimp"></a><a name="p1411mcpsimp"></a>0.25×VDDIO</p>
</td>
<td class="cellrowborder" valign="top" width="7.000000000000001%" headers="mcps1.2.8.1.7 "><p id="p1413mcpsimp"><a name="p1413mcpsimp"></a><a name="p1413mcpsimp"></a>V</p>
</td>
</tr>
<tr id="row1414mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.2.8.1.1 "><p id="p1416mcpsimp"><a name="p1416mcpsimp"></a><a name="p1416mcpsimp"></a>上拉电阻</p>
</td>
<td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.8.1.2 "><p id="p1418mcpsimp"><a name="p1418mcpsimp"></a><a name="p1418mcpsimp"></a>R<sub id="sub1419mcpsimp"><a name="sub1419mcpsimp"></a><a name="sub1419mcpsimp"></a>PU</sub></p>
</td>
<td class="cellrowborder" valign="top" width="14.32%" headers="mcps1.2.8.1.3 "><p id="entry1420mcpsimpp0"><a name="entry1420mcpsimpp0"></a><a name="entry1420mcpsimpp0"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="15.659999999999998%" headers="mcps1.2.8.1.4 "><p id="entry1421mcpsimpp0"><a name="entry1421mcpsimpp0"></a><a name="entry1421mcpsimpp0"></a>35k</p>
</td>
<td class="cellrowborder" valign="top" width="13.81%" headers="mcps1.2.8.1.5 "><p id="entry1422mcpsimpp0"><a name="entry1422mcpsimpp0"></a><a name="entry1422mcpsimpp0"></a>51k</p>
</td>
<td class="cellrowborder" valign="top" width="16.21%" headers="mcps1.2.8.1.6 "><p id="entry1423mcpsimpp0"><a name="entry1423mcpsimpp0"></a><a name="entry1423mcpsimpp0"></a>79k</p>
</td>
<td class="cellrowborder" valign="top" width="7.000000000000001%" headers="mcps1.2.8.1.7 "><p id="entry1424mcpsimpp0"><a name="entry1424mcpsimpp0"></a><a name="entry1424mcpsimpp0"></a>Ω</p>
</td>
</tr>
<tr id="row1425mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.2.8.1.1 "><p id="p1427mcpsimp"><a name="p1427mcpsimp"></a><a name="p1427mcpsimp"></a>下拉电阻</p>
</td>
<td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.8.1.2 "><p id="p1429mcpsimp"><a name="p1429mcpsimp"></a><a name="p1429mcpsimp"></a>R<sub id="sub1430mcpsimp"><a name="sub1430mcpsimp"></a><a name="sub1430mcpsimp"></a>PD</sub></p>
</td>
<td class="cellrowborder" valign="top" width="14.32%" headers="mcps1.2.8.1.3 "><p id="entry1431mcpsimpp0"><a name="entry1431mcpsimpp0"></a><a name="entry1431mcpsimpp0"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="15.659999999999998%" headers="mcps1.2.8.1.4 "><p id="entry1432mcpsimpp0"><a name="entry1432mcpsimpp0"></a><a name="entry1432mcpsimpp0"></a>29k</p>
</td>
<td class="cellrowborder" valign="top" width="13.81%" headers="mcps1.2.8.1.5 "><p id="entry1433mcpsimpp0"><a name="entry1433mcpsimpp0"></a><a name="entry1433mcpsimpp0"></a>48k</p>
</td>
<td class="cellrowborder" valign="top" width="16.21%" headers="mcps1.2.8.1.6 "><p id="entry1434mcpsimpp0"><a name="entry1434mcpsimpp0"></a><a name="entry1434mcpsimpp0"></a>88k</p>
</td>
<td class="cellrowborder" valign="top" width="7.000000000000001%" headers="mcps1.2.8.1.7 "><p id="entry1435mcpsimpp0"><a name="entry1435mcpsimpp0"></a><a name="entry1435mcpsimpp0"></a>Ω</p>
</td>
</tr>
</tbody>
</table>

>![](public_sys-resources/icon-note.gif) **说明：** 
>上表中VDDIO代表各个IO电源域电压，如IOLDO1、IOLDO4等。
>GPIO唤醒源说明：
>GPIO分为3组，分别是S\_AGPIO、S \_MGPIO、S\_HGPIO。
>S\_AGPIO在芯片深睡时可以作为唤醒源来唤醒芯片，并在深睡时上述管脚的PINMUX功能和PAD功能（包括驱动能力配置、输入使能配置、上下拉配置）都能正常保持。S\_HGPIO、S\_MGPIO在深睡时不能作为唤醒源，并且深睡时PINMUX功能失效，只有PAD功能可以进行保持。
>对于S\_HGPIO、S\_MGPIO管脚，在深睡时PINMUX功能会丢失，在唤醒恢复前其PINMUX是作为GPIO的功能。对于睡眠时上下拉有强约束的管脚，需要进行特殊配置。例如对于SPI的CS管脚，需要在深睡中持续保持高，可以通过配置默认上拉来进行保证；对于I2C的管脚，空闲态为高，需要配置外部上拉电阻或者配置上拉。

### QSPI接口<a name="ZH-CN_TOPIC_0000002514141724"></a>

模组支持3路QSPI接口，其中默认QSPI0对接Nor Flash；QSPI1默认对接Nand Flash；QSPI2默认对接屏幕。外接NorFlash最高频率100MHz；外接NandFlash接口最高频率100MHz；外接屏幕扩展，支持SDR/HDR（55MHz）。

Nor Flash/Nand Flash进行电路设计时，WP、CS、HOLD管脚要配置上拉电阻。以NorFlash为例，其参考设计如[图1](#fig5883194812123)所示，Nand Flash同理。QSPI2接屏幕仅要求CS配置上拉电阻。

**图 1**  QSPI接口设计<a name="fig5883194812123"></a>  
![](figures/QSPI接口设计.png "QSPI接口设计")

>![](public_sys-resources/icon-note.gif) **说明：** 
>-   对于送显等需要长走线的应用，为达到接口最高速率规格，建议QSPI接口信号线源端都预留串接电阻。
>-   对于QSPI Nor Flash等应用场景建议QSPI\_CS/QSPI\_D3/QSPI\_D2预留上拉电阻，其他应用场景按需增加。
>-   NorFlash上拉电阻使用的电源IOLDO6为器件供电电源。NandFlash使用的上拉电源为VDD\_PMU\_1P8\_SW0，供电电源应保持一致。

### SPI接口<a name="ZH-CN_TOPIC_0000002545901591"></a>

HiDiTingV100模组支持1个SPI接口，支持Master/Slave模式，Master模式，最大频率32MHz；Slave模式，最大频率16MHz。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>-   SPI相关数据线不支持Master、Slave数据方向自动切换。
>-   须确保SPI上拉电压VDD和SPI外设VDD的供电保持一致，以避免因电压不一致而导致电源通过IO管脚漏电现象。

以SPI0接口且对端设备片选信号低有效为例，其接口设计如[图1](#fig0208190103917)所示。

**图 1**  SPI接口设计参考<a name="fig0208190103917"></a>  
![](figures/SPI接口设计参考.png "SPI接口设计参考")

### UART接口<a name="ZH-CN_TOPIC_0000002514301648"></a>

模组有三路UART（UART2、UART3、UART4）用于调测，如需对接外设，请适配软件使用UART2或UART3。以四线UART为例，其接口设计如[图1](#fig206311755153914)所示。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>UART4不能用于对接外设。

**图 1**  UART接口参考设计<a name="fig206311755153914"></a>  
![](figures/UART接口参考设计.png "UART接口参考设计")

>![](public_sys-resources/icon-note.gif) **说明：** 
>示意图仅表示连线关系。

### I2C接口<a name="ZH-CN_TOPIC_0000002545781601"></a>

模组内提供4路I2C接口，支持Master和Slave。可以工作在以下速度模式下：标准模式（100kbit/s）、快速模式（400kbit/s）、快速模式+（1Mbit/s）。

-   I2C总线协议，既支持Master模式，也支持Slave模式。
-   I2C主机可以向从机写入数据，也可以接收从机发来的数据。

以I2C0为例，典型电路如[图1](#fig132192044422)所示。

**图 1**  I2C接口<a name="fig132192044422"></a>  
![](figures/I2C接口.png "I2C接口")

>![](public_sys-resources/icon-notice.gif) **须知：** 
>-   须确保I2C上拉电压VDD和I2C外设VDD的供电保持一致，以避免因电压不一致而导致电源通过IO管脚漏电现象。
>-   为确保可靠性，慎用I2C的SCL、SDA可自动识别的器件选型，推荐使用固定I2C管脚定义的器件。

### eMMC<a name="ZH-CN_TOPIC_0000002514141726"></a>

-   模组支持10线eMMC接口，接口最高时钟频率100MHz。
-   CMD、D0管脚增加上拉电阻，CLK管脚增加pF级滤波电容，其余管脚不作要求。
-   外部的eMMC存储芯片，不使用的通信DATA口可悬空处理。
-   外部的eMMC存储芯片，RST\_N信号建议进行上拉处理。

以两路供电eMMC芯片为例：VCC电源为3.3V，VCCQ电源为1.8V。具体参考eMMC器件手册，硬件设计时需满足其电源上电时序，典型设计如[图1](#fig460113464217)所示。

**图 1**  eMMC接口设计<a name="fig460113464217"></a>  
![](figures/eMMC接口设计.png "eMMC接口设计")

>![](public_sys-resources/icon-notice.gif) **须知：** 
>须确保eMMC上拉电压VCCQ和eMMC外设VCCQ供电保持一致，以避免因电压不一致而导致电源通过IO管脚漏电现象。

### SDIO<a name="ZH-CN_TOPIC_0000002545901595"></a>

支持1组SDIO接口，仅支持Master模式，接口最高时钟频率为50MHz。上下拉电阻的设置根据对端器件使用要求进行设计，板级空间充裕的情况下，建议CMD管脚、D0～D3管脚预留上拉电阻，CLK管脚预留pF级滤波电容，如[图1](#fig467610105437)所示。

**图 1**  SDIO接口参考<a name="fig467610105437"></a>  
![](figures/SDIO接口参考.png "SDIO接口参考")

>![](public_sys-resources/icon-notice.gif) **须知：** 
>须确保SDIO上拉电压VCC和SDIO外设VCC供电保持一致，以避免因电压不一致而导致电源通过IO管脚漏电现象。

### PWM接口<a name="ZH-CN_TOPIC_0000002514301650"></a>

模组支持同时输出多组不同的互补输出PWM信号：APWM0\_N/APWM0\_P、APWM1\_N/APWM1\_P、MPWM0\_N/MPWM0\_P。

支持对多路PWM进行分组，位于同一组内的PWM同步输出（软件可合理通过配置周期、相位和占空比实现同频同相、互补等功能）。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>受芯片设计、后端设计和物理实现的影响，同一组内的PWM之间可能存在1～2个PWM时钟周期的相位误差，无法做到完全同步，应用需要评估影响。

PWM接口使用32MHz时钟作为计数时钟，占空比可调，其高电平为对应管脚电源域电压。

如果需要使用1MHz以上PWM信号，PCB设计时需要特别注意控制阻抗匹配，确保信号完整性。

### MIPI接口<a name="ZH-CN_TOPIC_0000002545781605"></a>

-   支持1lane的MIPI显示接口。
-   由于高速接口可能带来辐射，建议预留共模电感，抑制辐射。
-   外设屏幕送出的帧同步信号DSI\_TE，必须接到对应的复用管脚上，不能作为普通输入信号处理。

    **图 1**  MIPI连接参考<a name="fig177261246144318"></a>  
    ![](figures/MIPI连接参考.png "MIPI连接参考")

### ADC接口<a name="ZH-CN_TOPIC_0000002514141728"></a>

-   HiDiTingV100模组内有集成AUXADC（Auxiliary Analog-to-Digital Converter），12bit精度，主要用于电压检测场景。
-   其中IOLDO2为ADC参考源，最大供电30mA，可以作为外部ADC信号的输入参考源，但注意不要给外部器件供电。
-   ADC采样速率越高，对外部电路驱动能力要求越强。
-   推荐外置BUFFER作为ADC的输入源。如果需要使用分压采样的方式给ADC输入，建议在管脚处预留对地10nF电容；其中上拉电阻R1和下拉电阻R2的并联值建议＜100kΩ（随着采样速率增加，阻值需对应降低）。

ADC典型应用场景如[图1](#fig115700184441)所示。

**图 1**  ADC使用场景<a name="fig115700184441"></a>  
![](figures/ADC使用场景.png "ADC使用场景")

>![](public_sys-resources/icon-note.gif) **说明：** 
>-   对于电池电压检测等对电压检测精度有需求的应用场景，首选AUX\_ADC0/AUX\_ADC1/AUX\_ADC2/AUX\_ADC3。
>-   为保证AUX\_ADC测量精度，AUX\_ADC管脚输入电压范围控制在0.2V～1.6V之间。
>-   AUX\_ADC分压电阻建议选用1%精度电阻。

### USB电路设计<a name="ZH-CN_TOPIC_0000002545901597"></a>

模组主控芯片支持USB Host/Device，支持USB 2.0接口：

-   芯片不支持5V直接供电，如果使用USB的VBUS给系统供电，中间必须增加LDO/BUCK/CHARGER等做电压转换，为了确保电源安全，建议预留板级防浪涌器件。
-   VBUS的插入识别，可以使用S\_AGPIO\_18的USB\_VBUS来实现。USB\_VBUS只能用来判断插入/拔出，无法测试VBUS电压。
-   USB DP/DM为通信管脚，建议预留串阻（2.2Ω级别）或者共模电感、ESD器件等，以抑制长线和PC端可能引起的ESD破坏。

模组内CAT1芯片提供一个USB2.0 Device接口，支持FS（Full-Speed）、HS（High-Speed）两种模式，速率分别可达12Mbit/s和480Mbit/s。可用于AT命令传送、数据传输、抓取log和固件升级。详细的接口分配如[表1](#_Ref218589330)所示。

**表 1**  模组CAT1芯片接口分配

<a name="_Ref218589330"></a>
<table><thead align="left"><tr id="row2076mcpsimp"><th class="cellrowborder" valign="top" width="15%" id="mcps1.2.4.1.1"><p id="p2078mcpsimp"><a name="p2078mcpsimp"></a><a name="p2078mcpsimp"></a>功能</p>
</th>
<th class="cellrowborder" valign="top" width="28.000000000000004%" id="mcps1.2.4.1.2"><p id="p2080mcpsimp"><a name="p2080mcpsimp"></a><a name="p2080mcpsimp"></a>引脚定义</p>
</th>
<th class="cellrowborder" valign="top" width="56.99999999999999%" id="mcps1.2.4.1.3"><p id="p2082mcpsimp"><a name="p2082mcpsimp"></a><a name="p2082mcpsimp"></a>备注</p>
</th>
</tr>
</thead>
<tbody><tr id="row2084mcpsimp"><td class="cellrowborder" rowspan="4" valign="top" width="15%" headers="mcps1.2.4.1.1 "><p id="p2086mcpsimp"><a name="p2086mcpsimp"></a><a name="p2086mcpsimp"></a>USB</p>
</td>
<td class="cellrowborder" valign="top" width="28.000000000000004%" headers="mcps1.2.4.1.2 "><p id="p2088mcpsimp"><a name="p2088mcpsimp"></a><a name="p2088mcpsimp"></a>CAT1_USB_DP</p>
</td>
<td class="cellrowborder" valign="top" width="56.99999999999999%" headers="mcps1.2.4.1.3 "><p id="p2090mcpsimp"><a name="p2090mcpsimp"></a><a name="p2090mcpsimp"></a>USB差分数据＋，需要做90Ω差分阻抗控制，不用则悬空。</p>
</td>
</tr>
<tr id="row2091mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p2093mcpsimp"><a name="p2093mcpsimp"></a><a name="p2093mcpsimp"></a>CAT1_USB_DM</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p2095mcpsimp"><a name="p2095mcpsimp"></a><a name="p2095mcpsimp"></a>USB差分数据－，需要做90Ω差分阻抗控制，不用则悬空。</p>
</td>
</tr>
<tr id="row2096mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p2098mcpsimp"><a name="p2098mcpsimp"></a><a name="p2098mcpsimp"></a>CAT1_USB_BOOT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p2100mcpsimp"><a name="p2100mcpsimp"></a><a name="p2100mcpsimp"></a>强制加载引脚，拉低芯片在开机时进入强制加载模式，可通过USB接口进行固件升级。</p>
</td>
</tr>
<tr id="row2101mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p2103mcpsimp"><a name="p2103mcpsimp"></a><a name="p2103mcpsimp"></a>CAT1_USB_VBUS</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p2105mcpsimp"><a name="p2105mcpsimp"></a><a name="p2105mcpsimp"></a>USB插拔检测，不用则悬空。</p>
</td>
</tr>
</tbody>
</table>

### I2S接口<a name="ZH-CN_TOPIC_0000002514301654"></a>

模组支持1路I2S（Inter-IC Sound）口（I2S0），可以外接Smart PA与音频外设。

-   支持I2S/PCM/TDM 16bit、20bit、24bit、32bit通道有效位宽（数据精度）可选。
-   I2S的时钟源频率支持：12.288MHz、24.576MHz（标准采样率）；12.8MHz、25.6MHz（非标采样率）。
-   支持16.7k/50k/100k/200k/400k Hz非标采样率，位宽16/24 bit。
-   支持8k/16k/32k/48k/96k/192k/384k Hz标准采样率，位宽16/24 bit。

SmartPA可选用商用芯片，配置接口使用I2C等，数字信号选择I2S信号。

模组对接SmartPA参考图如[图1](#fig1373516171415)所示。

**图 1**  I2S接口<a name="fig1373516171415"></a>  
![](figures/I2S接口.png "I2S接口")

>![](public_sys-resources/icon-note.gif) **说明：** 
>为防止I2S工作异常，软件配置上I2S\_CLK/I2S\_WS/I2S\_DI/I2S\_DO需要遵循如下操作：
>-   芯片上电初始化时，保持接口为上拉。
>-   复用为I2S功能时，推荐配置接口为下拉（其中I2S\_DI需要根据对端器件选择上拉/下拉）。
>-   业务使用时，接口不做上下拉配置。
>-   业务结束后，配置接口为输入下拉I<sub>PD。</sub>

### PDM输入接口<a name="ZH-CN_TOPIC_0000002545781607"></a>

模组支持1组PDM接口，支持对接DMIC器件。每组接口支持对接两个DMIC，PDM数据和时钟信号复用到GPIO管脚。通过L/R选择管脚区分两个声道。

DMIC的时钟频率为768kHz～3.2MHz，其中支持频点：1.536MHz、3.072MHz（标准采样率）、1.6MHz、3.2MHz（非标采样率）。

[图1](#fig10637511162115)为PDM接口对接2个DMIC器件的参考电路。

**图 1**  DMIC设计<a name="fig10637511162115"></a>  
![](figures/DMIC设计.png "DMIC设计")

### AMIC接口<a name="ZH-CN_TOPIC_0000002514141732"></a>

模组支持1路模拟MIC输入，模拟MIC输入为差分信号，芯片侧有0.9V偏置。

AMIC器件与模组端须加隔直电容，隔直电容必须选用2.2μF及以上电容（建议选用2.2μF，可以根据单板情况调整）。

为保证信号质量，建议：

-   AMIC器件的偏置电源输入放置去耦电容（建议470nF）。
-   放置在射频天线区域的AMIC，在Codec输出管脚处需预留串联电感的位置，用于抑制EMI干扰。
-   模组侧AMIC差分输入信号线上增加33pF滤波电容，如果走线临近射频区域，两边都预留33pF电容。
-   当AMIC在副板时，建议在主板侧BTB连接器旁的AMIC信号和MICBIAS电压上增加33pF滤波电容。

    **图 1**  模拟AMIC设计<a name="fig15313142510221"></a>  
    ![](figures/模拟AMIC设计.png "模拟AMIC设计")

如模拟MIC为单端输出方式，模组的MIC\_N端通过隔直电容单点接地。

**图 2**  驻极体MIC设计<a name="fig818391342318"></a>  
![](figures/驻极体MIC设计.png "驻极体MIC设计")

如驻极体MIC为单端输出方式，驻极体MIC的GND端建议串接2.2kΩ电阻后单点接地。

驻极体的OUT信号输出端，需要串接2.2kΩ到MICB，给驻极体MIC供电。

>![](public_sys-resources/icon-note.gif) **说明：** 
>-   模拟AMIC功能不使用时，相应管脚悬空即可。
>-   如果芯片MIC输入端接2.2μF电容，需要约束电容的有效容值为±20%（容值以仪器测试结果为准）。

### VICAP接口<a name="ZH-CN_TOPIC_0000002545901601"></a>

模组支持VICAP对接摄像头模组，摄像头时钟建议使用板级时钟。

为保证信号质量，建议：

-   VICAP的DATA与CLK建议同向走线等长，保证并行信号线信号质量。
-   有源晶振需要靠近摄像头连接器放置。

**图 1**  VICAP设计<a name="fig107mcpsimp"></a>  
![](figures/VICAP设计.png "VICAP设计")

![](figures/zh-cn_image_0000002514476354.png)

### HS音频输出接口<a name="ZH-CN_TOPIC_0000002514301658"></a>

模组支持两路HeadSet（HS）下行差分输出，共模偏置0.9V，可对接模拟音频PA。

为保证信号质量，建议：

-   模组DAC输出为Class D，靠近模组侧建议增加固定RC滤波电路，串阻47Ω，对地电容100nF，对高频载波进行有效过滤，避免影响后级模拟PA放大。
-   模拟PA侧匹配需根据器件型号进行调整。

    **图 1**  HS接口外接模拟PA设计<a name="fig478718489240"></a>  
    ![](figures/HS接口外接模拟PA设计.png "HS接口外接模拟PA设计")

## 下载调测接口<a name="ZH-CN_TOPIC_0000002545781611"></a>

>![](public_sys-resources/icon-note.gif) **说明：** 
>JLINK调试管脚、UART管脚等调测接口建议预留板级TP点。

-   **[主控芯片下载调测接口](#ZH-CN_TOPIC_0000002514141736)**  

-   **[CAT1与GNSS下载调测接口](#ZH-CN_TOPIC_0000002545901603)**  

### 主控芯片下载调测接口<a name="ZH-CN_TOPIC_0000002514141736"></a>

HiDiTingV100模组提供多种调测接口满足不同的调测需求，其中主控芯片调测接口包含JLINK、UART、USB等接口，参考[表1](#table1949mcpsimp)。

-   USB用于主控空片烧录（高速），使用USB烧录时S\_AGPIO 18需配置为USB\_VBUS作为插入检测功能，UART2\_RX同时需要拉低接地处理。
-   UART\_2用于主控烧录、AT指令口，UART\_3用于连接DebugKits工具，支持在线压缩日志、资源文件加载、虚拟Shell命令。
-   JLINK调试接口建议采用1.8V电平，对接JLINK调试盒子时，接口电平必须为1.8V，不建议通过中间加电平转换芯片将其转换成3.3V。

**表 1**  主控芯片调测接口说明

<a name="table1949mcpsimp"></a>
<table><thead align="left"><tr id="row1955mcpsimp"><th class="cellrowborder" valign="top" width="17.731773177317734%" id="mcps1.2.4.1.1"><p id="p1957mcpsimp"><a name="p1957mcpsimp"></a><a name="p1957mcpsimp"></a>管脚编号</p>
</th>
<th class="cellrowborder" valign="top" width="23.832383238323835%" id="mcps1.2.4.1.2"><p id="p1959mcpsimp"><a name="p1959mcpsimp"></a><a name="p1959mcpsimp"></a>管脚功能</p>
</th>
<th class="cellrowborder" valign="top" width="58.43584358435844%" id="mcps1.2.4.1.3"><p id="p1961mcpsimp"><a name="p1961mcpsimp"></a><a name="p1961mcpsimp"></a>功能描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1963mcpsimp"><td class="cellrowborder" valign="top" width="17.731773177317734%" headers="mcps1.2.4.1.1 "><p id="p1965mcpsimp"><a name="p1965mcpsimp"></a><a name="p1965mcpsimp"></a>19</p>
</td>
<td class="cellrowborder" valign="top" width="23.832383238323835%" headers="mcps1.2.4.1.2 "><p id="p1967mcpsimp"><a name="p1967mcpsimp"></a><a name="p1967mcpsimp"></a>USB_DP</p>
</td>
<td class="cellrowborder" valign="top" width="58.43584358435844%" headers="mcps1.2.4.1.3 "><p id="p1969mcpsimp"><a name="p1969mcpsimp"></a><a name="p1969mcpsimp"></a>主控芯片差分信号管脚。</p>
</td>
</tr>
<tr id="row1970mcpsimp"><td class="cellrowborder" valign="top" width="17.731773177317734%" headers="mcps1.2.4.1.1 "><p id="p1972mcpsimp"><a name="p1972mcpsimp"></a><a name="p1972mcpsimp"></a>21</p>
</td>
<td class="cellrowborder" valign="top" width="23.832383238323835%" headers="mcps1.2.4.1.2 "><p id="p1974mcpsimp"><a name="p1974mcpsimp"></a><a name="p1974mcpsimp"></a>USB_DM</p>
</td>
<td class="cellrowborder" valign="top" width="58.43584358435844%" headers="mcps1.2.4.1.3 "><p id="p1976mcpsimp"><a name="p1976mcpsimp"></a><a name="p1976mcpsimp"></a>主控芯片差分信号管脚。</p>
</td>
</tr>
<tr id="row1977mcpsimp"><td class="cellrowborder" valign="top" width="17.731773177317734%" headers="mcps1.2.4.1.1 "><p id="p1979mcpsimp"><a name="p1979mcpsimp"></a><a name="p1979mcpsimp"></a>111</p>
</td>
<td class="cellrowborder" valign="top" width="23.832383238323835%" headers="mcps1.2.4.1.2 "><p id="p1981mcpsimp"><a name="p1981mcpsimp"></a><a name="p1981mcpsimp"></a>UART2_TXD</p>
</td>
<td class="cellrowborder" valign="top" width="58.43584358435844%" headers="mcps1.2.4.1.3 "><p id="p1983mcpsimp"><a name="p1983mcpsimp"></a><a name="p1983mcpsimp"></a>芯片空片时：默认复用为UART2_TXD（空片烧录使用）。</p>
</td>
</tr>
<tr id="row1984mcpsimp"><td class="cellrowborder" valign="top" width="17.731773177317734%" headers="mcps1.2.4.1.1 "><p id="p1986mcpsimp"><a name="p1986mcpsimp"></a><a name="p1986mcpsimp"></a>113</p>
</td>
<td class="cellrowborder" valign="top" width="23.832383238323835%" headers="mcps1.2.4.1.2 "><p id="p1988mcpsimp"><a name="p1988mcpsimp"></a><a name="p1988mcpsimp"></a>UART2_RXD</p>
</td>
<td class="cellrowborder" valign="top" width="58.43584358435844%" headers="mcps1.2.4.1.3 "><p id="p1990mcpsimp"><a name="p1990mcpsimp"></a><a name="p1990mcpsimp"></a>芯片空片时：默认复用为UART2_RXD（空片烧录使用）。</p>
</td>
</tr>
<tr id="row1991mcpsimp"><td class="cellrowborder" valign="top" width="17.731773177317734%" headers="mcps1.2.4.1.1 "><p id="p1993mcpsimp"><a name="p1993mcpsimp"></a><a name="p1993mcpsimp"></a>53</p>
</td>
<td class="cellrowborder" valign="top" width="23.832383238323835%" headers="mcps1.2.4.1.2 "><p id="p1995mcpsimp"><a name="p1995mcpsimp"></a><a name="p1995mcpsimp"></a>SWD_CLK</p>
</td>
<td class="cellrowborder" valign="top" width="58.43584358435844%" headers="mcps1.2.4.1.3 "><p id="p1997mcpsimp"><a name="p1997mcpsimp"></a><a name="p1997mcpsimp"></a>为保证芯片正常工作，管脚禁止外接上拉电阻、外挂电容负载等。</p>
</td>
</tr>
<tr id="row1998mcpsimp"><td class="cellrowborder" valign="top" width="17.731773177317734%" headers="mcps1.2.4.1.1 "><p id="p2000mcpsimp"><a name="p2000mcpsimp"></a><a name="p2000mcpsimp"></a>50</p>
</td>
<td class="cellrowborder" valign="top" width="23.832383238323835%" headers="mcps1.2.4.1.2 "><p id="p2002mcpsimp"><a name="p2002mcpsimp"></a><a name="p2002mcpsimp"></a>SWD_DATA</p>
</td>
<td class="cellrowborder" valign="top" width="58.43584358435844%" headers="mcps1.2.4.1.3 "><p id="p2004mcpsimp"><a name="p2004mcpsimp"></a><a name="p2004mcpsimp"></a>为保证芯片正常工作，管脚禁止外接上拉电阻、外挂电容负载等。</p>
</td>
</tr>
</tbody>
</table>

### CAT1与GNSS下载调测接口<a name="ZH-CN_TOPIC_0000002545901603"></a>

-   CAT1芯片调测接口包含USB、UART等接口。

    其中USB2.0 Device接口支持FS（Full-Speed）、HS（High-Speed）两种模式，速率分别可达12Mbit/s和480Mbit/s。可用于AT命令传送、数据传输、抓取log和固件升级。

    CAT1 DEBUG\_UART仅用于内部调测。

-   GNSS芯片留有一个B4\_UART1\_TXD调测接口用于DEBUG日志上报。

**表 1**  CAT1与GNSS调测接口说明

<a name="_table3423mcpsimp"></a>
<table><thead align="left"><tr id="row2017mcpsimp"><th class="cellrowborder" valign="top" width="20.5020502050205%" id="mcps1.2.4.1.1"><p id="p2019mcpsimp"><a name="p2019mcpsimp"></a><a name="p2019mcpsimp"></a>管脚编号</p>
</th>
<th class="cellrowborder" valign="top" width="30.993099309930994%" id="mcps1.2.4.1.2"><p id="p2021mcpsimp"><a name="p2021mcpsimp"></a><a name="p2021mcpsimp"></a>管脚功能</p>
</th>
<th class="cellrowborder" valign="top" width="48.5048504850485%" id="mcps1.2.4.1.3"><p id="p2023mcpsimp"><a name="p2023mcpsimp"></a><a name="p2023mcpsimp"></a>功能描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row2025mcpsimp"><td class="cellrowborder" valign="top" width="20.5020502050205%" headers="mcps1.2.4.1.1 "><p id="p2027mcpsimp"><a name="p2027mcpsimp"></a><a name="p2027mcpsimp"></a>132</p>
</td>
<td class="cellrowborder" valign="top" width="30.993099309930994%" headers="mcps1.2.4.1.2 "><p id="p2029mcpsimp"><a name="p2029mcpsimp"></a><a name="p2029mcpsimp"></a>CAT1_USB_DP</p>
</td>
<td class="cellrowborder" valign="top" width="48.5048504850485%" headers="mcps1.2.4.1.3 "><p id="p2031mcpsimp"><a name="p2031mcpsimp"></a><a name="p2031mcpsimp"></a>CAT1芯片USB差分输入输出管脚。</p>
</td>
</tr>
<tr id="row2032mcpsimp"><td class="cellrowborder" valign="top" width="20.5020502050205%" headers="mcps1.2.4.1.1 "><p id="p2034mcpsimp"><a name="p2034mcpsimp"></a><a name="p2034mcpsimp"></a>131</p>
</td>
<td class="cellrowborder" valign="top" width="30.993099309930994%" headers="mcps1.2.4.1.2 "><p id="p2036mcpsimp"><a name="p2036mcpsimp"></a><a name="p2036mcpsimp"></a>CAT1_USB_DM</p>
</td>
<td class="cellrowborder" valign="top" width="48.5048504850485%" headers="mcps1.2.4.1.3 "><p id="p2038mcpsimp"><a name="p2038mcpsimp"></a><a name="p2038mcpsimp"></a>CAT1芯片USB差分输入输出管脚。</p>
</td>
</tr>
<tr id="row2039mcpsimp"><td class="cellrowborder" valign="top" width="20.5020502050205%" headers="mcps1.2.4.1.1 "><p id="p2041mcpsimp"><a name="p2041mcpsimp"></a><a name="p2041mcpsimp"></a>130</p>
</td>
<td class="cellrowborder" valign="top" width="30.993099309930994%" headers="mcps1.2.4.1.2 "><p id="p2043mcpsimp"><a name="p2043mcpsimp"></a><a name="p2043mcpsimp"></a>CAT1_USB_VBUS</p>
</td>
<td class="cellrowborder" valign="top" width="48.5048504850485%" headers="mcps1.2.4.1.3 "><p id="p2045mcpsimp"><a name="p2045mcpsimp"></a><a name="p2045mcpsimp"></a>CAT1芯片USB检测信号。</p>
</td>
</tr>
<tr id="row2046mcpsimp"><td class="cellrowborder" valign="top" width="20.5020502050205%" headers="mcps1.2.4.1.1 "><p id="p2048mcpsimp"><a name="p2048mcpsimp"></a><a name="p2048mcpsimp"></a>129</p>
</td>
<td class="cellrowborder" valign="top" width="30.993099309930994%" headers="mcps1.2.4.1.2 "><p id="p2050mcpsimp"><a name="p2050mcpsimp"></a><a name="p2050mcpsimp"></a>CAT1_USB_BOOT</p>
</td>
<td class="cellrowborder" valign="top" width="48.5048504850485%" headers="mcps1.2.4.1.3 "><p id="p2052mcpsimp"><a name="p2052mcpsimp"></a><a name="p2052mcpsimp"></a>CAT1芯片USB强制加载信号，低电平有效。</p>
</td>
</tr>
<tr id="row2053mcpsimp"><td class="cellrowborder" valign="top" width="20.5020502050205%" headers="mcps1.2.4.1.1 "><p id="p2055mcpsimp"><a name="p2055mcpsimp"></a><a name="p2055mcpsimp"></a>125</p>
</td>
<td class="cellrowborder" valign="top" width="30.993099309930994%" headers="mcps1.2.4.1.2 "><p id="p2057mcpsimp"><a name="p2057mcpsimp"></a><a name="p2057mcpsimp"></a>B4_UART1_TXD</p>
</td>
<td class="cellrowborder" valign="top" width="48.5048504850485%" headers="mcps1.2.4.1.3 "><p id="p2059mcpsimp"><a name="p2059mcpsimp"></a><a name="p2059mcpsimp"></a>GNSS DEBUG日志输出管脚。</p>
</td>
</tr>
</tbody>
</table>

# PCB设计指导<a name="ZH-CN_TOPIC_0000002514301660"></a>

-   **[通流参考](#ZH-CN_TOPIC_0000002545781613)**  

-   **[关键信号约束](#ZH-CN_TOPIC_0000002514141738)**  

## 通流参考<a name="ZH-CN_TOPIC_0000002545781613"></a>

Layout走线通流请参考[表1](#d1e7175)。

**表 1**  通流参考

<a name="d1e7175"></a>
<table><thead align="left"><tr id="row170mcpsimp"><th class="cellrowborder" valign="top" width="27%" id="mcps1.2.4.1.1"><p id="p172mcpsimp"><a name="p172mcpsimp"></a><a name="p172mcpsimp"></a>管脚编号</p>
</th>
<th class="cellrowborder" valign="top" width="42.51%" id="mcps1.2.4.1.2"><p id="p174mcpsimp"><a name="p174mcpsimp"></a><a name="p174mcpsimp"></a>管脚名称</p>
</th>
<th class="cellrowborder" valign="top" width="30.490000000000002%" id="mcps1.2.4.1.3"><p id="p176mcpsimp"><a name="p176mcpsimp"></a><a name="p176mcpsimp"></a>参考通流（mA）</p>
</th>
</tr>
</thead>
<tbody><tr id="row178mcpsimp"><td class="cellrowborder" valign="top" width="27%" headers="mcps1.2.4.1.1 "><p id="p180mcpsimp"><a name="p180mcpsimp"></a><a name="p180mcpsimp"></a>58</p>
</td>
<td class="cellrowborder" valign="top" width="42.51%" headers="mcps1.2.4.1.2 "><p id="p182mcpsimp"><a name="p182mcpsimp"></a><a name="p182mcpsimp"></a>VSYS</p>
</td>
<td class="cellrowborder" valign="top" width="30.490000000000002%" headers="mcps1.2.4.1.3 "><p id="p184mcpsimp"><a name="p184mcpsimp"></a><a name="p184mcpsimp"></a>900</p>
</td>
</tr>
<tr id="row185mcpsimp"><td class="cellrowborder" valign="top" width="27%" headers="mcps1.2.4.1.1 "><p id="p187mcpsimp"><a name="p187mcpsimp"></a><a name="p187mcpsimp"></a>57</p>
</td>
<td class="cellrowborder" valign="top" width="42.51%" headers="mcps1.2.4.1.2 "><p id="p189mcpsimp"><a name="p189mcpsimp"></a><a name="p189mcpsimp"></a>VDD_PMU_1P8_SW0</p>
</td>
<td class="cellrowborder" valign="top" width="30.490000000000002%" headers="mcps1.2.4.1.3 "><p id="p191mcpsimp"><a name="p191mcpsimp"></a><a name="p191mcpsimp"></a>80</p>
</td>
</tr>
<tr id="row192mcpsimp"><td class="cellrowborder" valign="top" width="27%" headers="mcps1.2.4.1.1 "><p id="p194mcpsimp"><a name="p194mcpsimp"></a><a name="p194mcpsimp"></a>85</p>
</td>
<td class="cellrowborder" valign="top" width="42.51%" headers="mcps1.2.4.1.2 "><p id="p196mcpsimp"><a name="p196mcpsimp"></a><a name="p196mcpsimp"></a>VDD_PMU_MICLDO0</p>
</td>
<td class="cellrowborder" valign="top" width="30.490000000000002%" headers="mcps1.2.4.1.3 "><p id="p198mcpsimp"><a name="p198mcpsimp"></a><a name="p198mcpsimp"></a>10</p>
</td>
</tr>
<tr id="row199mcpsimp"><td class="cellrowborder" valign="top" width="27%" headers="mcps1.2.4.1.1 "><p id="p201mcpsimp"><a name="p201mcpsimp"></a><a name="p201mcpsimp"></a>59</p>
</td>
<td class="cellrowborder" valign="top" width="42.51%" headers="mcps1.2.4.1.2 "><p id="p203mcpsimp"><a name="p203mcpsimp"></a><a name="p203mcpsimp"></a>VDD_PMU_IOLDO2</p>
</td>
<td class="cellrowborder" valign="top" width="30.490000000000002%" headers="mcps1.2.4.1.3 "><p id="p205mcpsimp"><a name="p205mcpsimp"></a><a name="p205mcpsimp"></a>100</p>
</td>
</tr>
<tr id="row206mcpsimp"><td class="cellrowborder" valign="top" width="27%" headers="mcps1.2.4.1.1 "><p id="p208mcpsimp"><a name="p208mcpsimp"></a><a name="p208mcpsimp"></a>63</p>
</td>
<td class="cellrowborder" valign="top" width="42.51%" headers="mcps1.2.4.1.2 "><p id="p210mcpsimp"><a name="p210mcpsimp"></a><a name="p210mcpsimp"></a>VDD_PMU_IOLDO3</p>
</td>
<td class="cellrowborder" valign="top" width="30.490000000000002%" headers="mcps1.2.4.1.3 "><p id="p212mcpsimp"><a name="p212mcpsimp"></a><a name="p212mcpsimp"></a>300</p>
</td>
</tr>
<tr id="row213mcpsimp"><td class="cellrowborder" valign="top" width="27%" headers="mcps1.2.4.1.1 "><p id="p215mcpsimp"><a name="p215mcpsimp"></a><a name="p215mcpsimp"></a>56</p>
</td>
<td class="cellrowborder" valign="top" width="42.51%" headers="mcps1.2.4.1.2 "><p id="p217mcpsimp"><a name="p217mcpsimp"></a><a name="p217mcpsimp"></a>VDD_PMU_IOLDO6</p>
</td>
<td class="cellrowborder" valign="top" width="30.490000000000002%" headers="mcps1.2.4.1.3 "><p id="p219mcpsimp"><a name="p219mcpsimp"></a><a name="p219mcpsimp"></a>100</p>
</td>
</tr>
<tr id="row220mcpsimp"><td class="cellrowborder" valign="top" width="27%" headers="mcps1.2.4.1.1 "><p id="p222mcpsimp"><a name="p222mcpsimp"></a><a name="p222mcpsimp"></a>24</p>
</td>
<td class="cellrowborder" valign="top" width="42.51%" headers="mcps1.2.4.1.2 "><p id="p224mcpsimp"><a name="p224mcpsimp"></a><a name="p224mcpsimp"></a>B4_VBAT_2871</p>
</td>
<td class="cellrowborder" valign="top" width="30.490000000000002%" headers="mcps1.2.4.1.3 "><p id="p226mcpsimp"><a name="p226mcpsimp"></a><a name="p226mcpsimp"></a>300</p>
</td>
</tr>
<tr id="row227mcpsimp"><td class="cellrowborder" valign="top" width="27%" headers="mcps1.2.4.1.1 "><p id="p229mcpsimp"><a name="p229mcpsimp"></a><a name="p229mcpsimp"></a>47</p>
</td>
<td class="cellrowborder" valign="top" width="42.51%" headers="mcps1.2.4.1.2 "><p id="p231mcpsimp"><a name="p231mcpsimp"></a><a name="p231mcpsimp"></a>B4_VDD_1P8</p>
</td>
<td class="cellrowborder" valign="top" width="30.490000000000002%" headers="mcps1.2.4.1.3 "><p id="p233mcpsimp"><a name="p233mcpsimp"></a><a name="p233mcpsimp"></a>200</p>
</td>
</tr>
<tr id="row234mcpsimp"><td class="cellrowborder" valign="top" width="27%" headers="mcps1.2.4.1.1 "><p id="p236mcpsimp"><a name="p236mcpsimp"></a><a name="p236mcpsimp"></a>12</p>
</td>
<td class="cellrowborder" valign="top" width="42.51%" headers="mcps1.2.4.1.2 "><p id="p238mcpsimp"><a name="p238mcpsimp"></a><a name="p238mcpsimp"></a>VDDIO_B</p>
</td>
<td class="cellrowborder" valign="top" width="30.490000000000002%" headers="mcps1.2.4.1.3 "><p id="p240mcpsimp"><a name="p240mcpsimp"></a><a name="p240mcpsimp"></a>50</p>
</td>
</tr>
<tr id="row241mcpsimp"><td class="cellrowborder" valign="top" width="27%" headers="mcps1.2.4.1.1 "><p id="p243mcpsimp"><a name="p243mcpsimp"></a><a name="p243mcpsimp"></a>118</p>
</td>
<td class="cellrowborder" valign="top" width="42.51%" headers="mcps1.2.4.1.2 "><p id="p245mcpsimp"><a name="p245mcpsimp"></a><a name="p245mcpsimp"></a>CAT1_VBAT1</p>
</td>
<td class="cellrowborder" valign="top" width="30.490000000000002%" headers="mcps1.2.4.1.3 "><p id="p247mcpsimp"><a name="p247mcpsimp"></a><a name="p247mcpsimp"></a>900</p>
</td>
</tr>
<tr id="row248mcpsimp"><td class="cellrowborder" valign="top" width="27%" headers="mcps1.2.4.1.1 "><p id="p250mcpsimp"><a name="p250mcpsimp"></a><a name="p250mcpsimp"></a>119</p>
</td>
<td class="cellrowborder" valign="top" width="42.51%" headers="mcps1.2.4.1.2 "><p id="p252mcpsimp"><a name="p252mcpsimp"></a><a name="p252mcpsimp"></a>CAT1_VBAT2</p>
</td>
<td class="cellrowborder" valign="top" width="30.490000000000002%" headers="mcps1.2.4.1.3 "><p id="p254mcpsimp"><a name="p254mcpsimp"></a><a name="p254mcpsimp"></a>1000</p>
</td>
</tr>
</tbody>
</table>

## 关键信号约束<a name="ZH-CN_TOPIC_0000002514141738"></a>

-   MIPI高速信号，需要同层等长走线和包地处理，尽量走表层，MIPI信号按照单端50Ω、差分100Ω走线阻抗处理，以保证信号完整性。
-   高速IO和高速时钟信号如QSPI0，尽量在内层同层等长走线，包地隔离处理。
-   AUX ADC模拟信号应该内层走线，包地保护。
-   AMIC差分输入信号按照差分紧耦合走线，四面包地（非射频地）；走线通路，在空间上（不同层之间）尽量避免电源、时钟、射频线等强干扰信号的交叉、并行走线；尽量避免表层布线，不能避免时表层连续走线尽量短；隔直电容和33pF滤波电容靠近MIC器件处放置；MICLDO等为AMIC供电走线尽量四面包地。
-   AMIC单端连接方式下，AMIC器件地需要单点接地 ，且需要通过2.2μF电容连接至芯片MIC0\_N/MIC1\_N/MIC2\_N管脚。
-   HSP/N峰值电流225mA，到负载端的电阻控制在0.05Ω以下，建议走线宽度不小于6mil。按照差分紧耦合走线，四面包地（非射频地）；走线通路，在空间上（不同层之间）尽量避免电源、时钟、射频线等强干扰信号的交叉、并行走线；尽量避免表层布线，不能避免时表层连续走线尽量短。
-   高速IO和高速时钟信号线走内层，包地隔离处理，远离辐射干扰。
-   USB\_DP/USB\_DM差分走线，差分阻抗控制90Ω；要求立体包地，减少干扰。

# 射频特性<a name="ZH-CN_TOPIC_0000002518193780"></a>

-   **[蜂窝网络](#ZH-CN_TOPIC_0000002518194218)**  

-   **[BT](#ZH-CN_TOPIC_0000002518194220)**  

-   **[GNSS](#ZH-CN_TOPIC_0000002549833987)**  

## 蜂窝网络<a name="ZH-CN_TOPIC_0000002518194218"></a>

-   **[工作频段](#ZH-CN_TOPIC_0000002549833985)**  

-   **[发射功率](#ZH-CN_TOPIC_0000002518354138)**  

-   **[接收灵敏度](#ZH-CN_TOPIC_0000002549713993)**  

### 工作频段<a name="ZH-CN_TOPIC_0000002549833985"></a>

**表 1**  工作频段

<a name="table16662155831410"></a>
<table><thead align="left"><tr id="row16971858171416"><th class="cellrowborder" valign="top" width="32.653265326532654%" id="mcps1.2.4.1.1"><p id="p869718583149"><a name="p869718583149"></a><a name="p869718583149"></a>工作频段</p>
</th>
<th class="cellrowborder" valign="top" width="32.653265326532654%" id="mcps1.2.4.1.2"><p id="p3697125811141"><a name="p3697125811141"></a><a name="p3697125811141"></a>发送（MHz）</p>
</th>
<th class="cellrowborder" valign="top" width="34.69346934693469%" id="mcps1.2.4.1.3"><p id="p869895831416"><a name="p869895831416"></a><a name="p869895831416"></a>接收（MHz）</p>
</th>
</tr>
</thead>
<tbody><tr id="row14698105831411"><td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.1 "><p id="p1169895871414"><a name="p1169895871414"></a><a name="p1169895871414"></a>LTE-FDD B1</p>
</td>
<td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.2 "><p id="p06981858151412"><a name="p06981858151412"></a><a name="p06981858151412"></a>1920～1980</p>
</td>
<td class="cellrowborder" valign="top" width="34.69346934693469%" headers="mcps1.2.4.1.3 "><p id="p126981258161414"><a name="p126981258161414"></a><a name="p126981258161414"></a>2110～2170</p>
</td>
</tr>
<tr id="row206981058111417"><td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.1 "><p id="p1698205831410"><a name="p1698205831410"></a><a name="p1698205831410"></a>LTE-FDD B3</p>
</td>
<td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.2 "><p id="p15698145819147"><a name="p15698145819147"></a><a name="p15698145819147"></a>1710～1785</p>
</td>
<td class="cellrowborder" valign="top" width="34.69346934693469%" headers="mcps1.2.4.1.3 "><p id="p46981458141414"><a name="p46981458141414"></a><a name="p46981458141414"></a>1805～1880</p>
</td>
</tr>
<tr id="row1469811580145"><td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.1 "><p id="p1169855871412"><a name="p1169855871412"></a><a name="p1169855871412"></a>LTE-FDD B5</p>
</td>
<td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.2 "><p id="p20698105841410"><a name="p20698105841410"></a><a name="p20698105841410"></a>824～849</p>
</td>
<td class="cellrowborder" valign="top" width="34.69346934693469%" headers="mcps1.2.4.1.3 "><p id="p2698115891414"><a name="p2698115891414"></a><a name="p2698115891414"></a>869～894</p>
</td>
</tr>
<tr id="row1169815891417"><td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.1 "><p id="p1669815585148"><a name="p1669815585148"></a><a name="p1669815585148"></a>LTE-FDD B8</p>
</td>
<td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.2 "><p id="p4698958131410"><a name="p4698958131410"></a><a name="p4698958131410"></a>880～915</p>
</td>
<td class="cellrowborder" valign="top" width="34.69346934693469%" headers="mcps1.2.4.1.3 "><p id="p5698558131419"><a name="p5698558131419"></a><a name="p5698558131419"></a>925～960</p>
</td>
</tr>
<tr id="row3698205818141"><td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.1 "><p id="p56984589143"><a name="p56984589143"></a><a name="p56984589143"></a>LTE-TDD B34</p>
</td>
<td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.2 "><p id="p86981588147"><a name="p86981588147"></a><a name="p86981588147"></a>2010～2025</p>
</td>
<td class="cellrowborder" valign="top" width="34.69346934693469%" headers="mcps1.2.4.1.3 "><p id="p12698258161414"><a name="p12698258161414"></a><a name="p12698258161414"></a>2010～2025</p>
</td>
</tr>
<tr id="row5698858141417"><td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.1 "><p id="p16698195814143"><a name="p16698195814143"></a><a name="p16698195814143"></a>LTE-TDD B38</p>
</td>
<td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.2 "><p id="p126986587146"><a name="p126986587146"></a><a name="p126986587146"></a>2570～2620</p>
</td>
<td class="cellrowborder" valign="top" width="34.69346934693469%" headers="mcps1.2.4.1.3 "><p id="p206989588146"><a name="p206989588146"></a><a name="p206989588146"></a>2570～2620</p>
</td>
</tr>
<tr id="row3698145811146"><td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.1 "><p id="p1769817581145"><a name="p1769817581145"></a><a name="p1769817581145"></a>LTE-TDD B39</p>
</td>
<td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.2 "><p id="p11698115819144"><a name="p11698115819144"></a><a name="p11698115819144"></a>1880～1920</p>
</td>
<td class="cellrowborder" valign="top" width="34.69346934693469%" headers="mcps1.2.4.1.3 "><p id="p16981658141411"><a name="p16981658141411"></a><a name="p16981658141411"></a>1880～1920</p>
</td>
</tr>
<tr id="row86981158131418"><td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.1 "><p id="p069875831411"><a name="p069875831411"></a><a name="p069875831411"></a>LTE-TDD B40</p>
</td>
<td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.2 "><p id="p10698458181417"><a name="p10698458181417"></a><a name="p10698458181417"></a>2300～2400</p>
</td>
<td class="cellrowborder" valign="top" width="34.69346934693469%" headers="mcps1.2.4.1.3 "><p id="p76982058141415"><a name="p76982058141415"></a><a name="p76982058141415"></a>2300～2400</p>
</td>
</tr>
<tr id="row1669815810145"><td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.1 "><p id="p869845813141"><a name="p869845813141"></a><a name="p869845813141"></a>LTE-TDD B41</p>
</td>
<td class="cellrowborder" valign="top" width="32.653265326532654%" headers="mcps1.2.4.1.2 "><p id="p156982058181411"><a name="p156982058181411"></a><a name="p156982058181411"></a>2535～2675</p>
</td>
<td class="cellrowborder" valign="top" width="34.69346934693469%" headers="mcps1.2.4.1.3 "><p id="p66985588148"><a name="p66985588148"></a><a name="p66985588148"></a>2535～2675</p>
</td>
</tr>
</tbody>
</table>

### 发射功率<a name="ZH-CN_TOPIC_0000002518354138"></a>

**表 1**  射频发射功率

<a name="table1341183710165"></a>
<table><thead align="left"><tr id="row1543017375166"><th class="cellrowborder" rowspan="1" valign="top" width="44.620000000000005%" id="mcps1.2.4.1.1"><p id="p643020377169"><a name="p643020377169"></a><a name="p643020377169"></a>频段</p>
</th>
<th class="cellrowborder" rowspan="1" valign="top" width="25.830000000000002%" id="mcps1.2.4.1.2"><p id="p843023716161"><a name="p843023716161"></a><a name="p843023716161"></a>最大值</p>
</th>
<th class="cellrowborder" rowspan="1" valign="top" width="29.550000000000004%" id="mcps1.2.4.1.3"><p id="p74303373163"><a name="p74303373163"></a><a name="p74303373163"></a>最小值</p>
</th>
</tr>
</thead>
<tbody><tr id="row1643143716166"><td class="cellrowborder" valign="top" width="44.620000000000005%" headers="mcps1.2.4.1.1 "><p id="p11431137181617"><a name="p11431137181617"></a><a name="p11431137181617"></a>LTE-FDD B1/B3/B5/B8</p>
</td>
<td class="cellrowborder" valign="top" width="25.830000000000002%" headers="mcps1.2.4.1.2 "><p id="p1843133710164"><a name="p1843133710164"></a><a name="p1843133710164"></a>23dBm&plusmn;2dB</p>
</td>
<td class="cellrowborder" valign="top" width="29.550000000000004%" headers="mcps1.2.4.1.3 "><p id="p8431537191613"><a name="p8431537191613"></a><a name="p8431537191613"></a>&lt;-39dBm</p>
</td>
</tr>
<tr id="row1643133713160"><td class="cellrowborder" valign="top" width="44.620000000000005%" headers="mcps1.2.4.1.1 "><p id="p194311379167"><a name="p194311379167"></a><a name="p194311379167"></a>LTE-TDD B34/B38/B39/B40/B41</p>
</td>
<td class="cellrowborder" valign="top" width="25.830000000000002%" headers="mcps1.2.4.1.2 "><p id="p643123710164"><a name="p643123710164"></a><a name="p643123710164"></a>23dBm&plusmn;2dB</p>
</td>
<td class="cellrowborder" valign="top" width="29.550000000000004%" headers="mcps1.2.4.1.3 "><p id="p34312372164"><a name="p34312372164"></a><a name="p34312372164"></a>&lt;-39dBm</p>
</td>
</tr>
</tbody>
</table>

### 接收灵敏度<a name="ZH-CN_TOPIC_0000002549713993"></a>

**表 1**  射频接收灵敏度

<a name="table7469457161617"></a>
<table><thead align="left"><tr id="row16501175718168"><th class="cellrowborder" valign="top" width="41.760000000000005%" id="mcps1.2.4.1.1"><p id="p6356132415616"><a name="p6356132415616"></a><a name="p6356132415616"></a>频段</p>
</th>
<th class="cellrowborder" valign="top" width="29.12%" id="mcps1.2.4.1.2"><p id="p145011571169"><a name="p145011571169"></a><a name="p145011571169"></a>典型值（dBm）</p>
</th>
<th class="cellrowborder" valign="top" width="29.12%" id="mcps1.2.4.1.3"><p id="p250195791616"><a name="p250195791616"></a><a name="p250195791616"></a>CAT1规范要求（dBm）</p>
</th>
</tr>
</thead>
<tbody><tr id="row125012573162"><td class="cellrowborder" valign="top" width="41.760000000000005%" headers="mcps1.2.4.1.1 "><p id="p195011857151615"><a name="p195011857151615"></a><a name="p195011857151615"></a>LTE-FDD B1（10MHz）</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.2 "><p id="p1250117576167"><a name="p1250117576167"></a><a name="p1250117576167"></a>-99</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.3 "><p id="p16501157131610"><a name="p16501157131610"></a><a name="p16501157131610"></a>-94</p>
</td>
</tr>
<tr id="row15011057131617"><td class="cellrowborder" valign="top" width="41.760000000000005%" headers="mcps1.2.4.1.1 "><p id="p18501115714168"><a name="p18501115714168"></a><a name="p18501115714168"></a>LTE-FDD B3（10MHz）</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.2 "><p id="p2501757141615"><a name="p2501757141615"></a><a name="p2501757141615"></a>-99</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.3 "><p id="p5501357101616"><a name="p5501357101616"></a><a name="p5501357101616"></a>-91</p>
</td>
</tr>
<tr id="row15501185714169"><td class="cellrowborder" valign="top" width="41.760000000000005%" headers="mcps1.2.4.1.1 "><p id="p250118577168"><a name="p250118577168"></a><a name="p250118577168"></a>LTE-FDD B5（10MHz）</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.2 "><p id="p14501757161611"><a name="p14501757161611"></a><a name="p14501757161611"></a>-99</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.3 "><p id="p145014576163"><a name="p145014576163"></a><a name="p145014576163"></a>-92.5</p>
</td>
</tr>
<tr id="row95018572163"><td class="cellrowborder" valign="top" width="41.760000000000005%" headers="mcps1.2.4.1.1 "><p id="p18501125710168"><a name="p18501125710168"></a><a name="p18501125710168"></a>LTE-FDD B8（10MHz）</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.2 "><p id="p11501185761613"><a name="p11501185761613"></a><a name="p11501185761613"></a>-98.5</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.3 "><p id="p19501657101611"><a name="p19501657101611"></a><a name="p19501657101611"></a>-91.5</p>
</td>
</tr>
<tr id="row195011757161619"><td class="cellrowborder" valign="top" width="41.760000000000005%" headers="mcps1.2.4.1.1 "><p id="p9502957191619"><a name="p9502957191619"></a><a name="p9502957191619"></a>LTE-TDD B34（10MHz）</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.2 "><p id="p1350225718165"><a name="p1350225718165"></a><a name="p1350225718165"></a>-101</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.3 "><p id="p1050225781619"><a name="p1050225781619"></a><a name="p1050225781619"></a>-94.5</p>
</td>
</tr>
<tr id="row19502155751611"><td class="cellrowborder" valign="top" width="41.760000000000005%" headers="mcps1.2.4.1.1 "><p id="p350213573162"><a name="p350213573162"></a><a name="p350213573162"></a>LTE-TDD B38（10MHz）</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.2 "><p id="p16502357201619"><a name="p16502357201619"></a><a name="p16502357201619"></a>-101</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.3 "><p id="p1050235761614"><a name="p1050235761614"></a><a name="p1050235761614"></a>-92.5</p>
</td>
</tr>
<tr id="row18502165741611"><td class="cellrowborder" valign="top" width="41.760000000000005%" headers="mcps1.2.4.1.1 "><p id="p1150235731613"><a name="p1150235731613"></a><a name="p1150235731613"></a>LTE-TDD B39（10MHz）</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.2 "><p id="p16502155761614"><a name="p16502155761614"></a><a name="p16502155761614"></a>-100.5</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.3 "><p id="p13502175741617"><a name="p13502175741617"></a><a name="p13502175741617"></a>-94.5</p>
</td>
</tr>
<tr id="row850214576169"><td class="cellrowborder" valign="top" width="41.760000000000005%" headers="mcps1.2.4.1.1 "><p id="p115021357101616"><a name="p115021357101616"></a><a name="p115021357101616"></a>LTE-TDD B40（10MHz）</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.2 "><p id="p450275714166"><a name="p450275714166"></a><a name="p450275714166"></a>-100.5</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.3 "><p id="p135021057161612"><a name="p135021057161612"></a><a name="p135021057161612"></a>-94.5</p>
</td>
</tr>
<tr id="row1850215716161"><td class="cellrowborder" valign="top" width="41.760000000000005%" headers="mcps1.2.4.1.1 "><p id="p8502125711164"><a name="p8502125711164"></a><a name="p8502125711164"></a>LTE-TDD B41（10MHz）</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.2 "><p id="p16502195715165"><a name="p16502195715165"></a><a name="p16502195715165"></a>-101</p>
</td>
<td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.3 "><p id="p2050211571166"><a name="p2050211571166"></a><a name="p2050211571166"></a>-92.5</p>
</td>
</tr>
</tbody>
</table>

## BT<a name="ZH-CN_TOPIC_0000002518194220"></a>

-   **[工作频段](#ZH-CN_TOPIC_0000002549834557)**  

-   **[发射功率及灵敏度](#ZH-CN_TOPIC_0000002518194802)**  

### 工作频段<a name="ZH-CN_TOPIC_0000002549834557"></a>

**表 1**  BT工作频段

<a name="table1789985018191"></a>
<table><thead align="left"><tr id="row792715071920"><th class="cellrowborder" valign="top" width="41.54%" id="mcps1.2.3.1.1"><p id="p169271250171917"><a name="p169271250171917"></a><a name="p169271250171917"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="58.46%" id="mcps1.2.3.1.2"><p id="p592745041912"><a name="p592745041912"></a><a name="p592745041912"></a>频段（MHz）</p>
</th>
</tr>
</thead>
<tbody><tr id="row189271250131913"><td class="cellrowborder" valign="top" width="41.54%" headers="mcps1.2.3.1.1 "><p id="p792715504199"><a name="p792715504199"></a><a name="p792715504199"></a>BT/SLE（星闪）</p>
</td>
<td class="cellrowborder" valign="top" width="58.46%" headers="mcps1.2.3.1.2 "><p id="p29271750151920"><a name="p29271750151920"></a><a name="p29271750151920"></a>2402～2480</p>
</td>
</tr>
</tbody>
</table>

### 发射功率及灵敏度<a name="ZH-CN_TOPIC_0000002518194802"></a>

**表 1**  发射功率及灵敏度

<a name="table176731622012"></a>
<table><thead align="left"><tr id="row7109141616205"><th class="cellrowborder" valign="top" width="26.919999999999998%" id="mcps1.2.4.1.1"><p id="p910951652013"><a name="p910951652013"></a><a name="p910951652013"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="36.78%" id="mcps1.2.4.1.2"><p id="p1310981615204"><a name="p1310981615204"></a><a name="p1310981615204"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="36.3%" id="mcps1.2.4.1.3"><p id="p12109416152013"><a name="p12109416152013"></a><a name="p12109416152013"></a>典型值（dBm）</p>
</th>
</tr>
</thead>
<tbody><tr id="row310991615201"><td class="cellrowborder" rowspan="8" valign="top" width="26.919999999999998%" headers="mcps1.2.4.1.1 "><p id="p2109101652017"><a name="p2109101652017"></a><a name="p2109101652017"></a>TX Power(dBm)</p>
</td>
<td class="cellrowborder" valign="top" width="36.78%" headers="mcps1.2.4.1.2 "><p id="p181091716142020"><a name="p181091716142020"></a><a name="p181091716142020"></a>BR（GFSK）</p>
</td>
<td class="cellrowborder" valign="top" width="36.3%" headers="mcps1.2.4.1.3 "><p id="p6109131632017"><a name="p6109131632017"></a><a name="p6109131632017"></a>10</p>
</td>
</tr>
<tr id="row510961613205"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p16109916152015"><a name="p16109916152015"></a><a name="p16109916152015"></a>2EDR（QPSK）</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p0109151682012"><a name="p0109151682012"></a><a name="p0109151682012"></a>6</p>
</td>
</tr>
<tr id="row31091416102011"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p810915167208"><a name="p810915167208"></a><a name="p810915167208"></a>3EDR（8PSK）</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1210915166202"><a name="p1210915166202"></a><a name="p1210915166202"></a>6</p>
</td>
</tr>
<tr id="row611013161206"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1711071619200"><a name="p1711071619200"></a><a name="p1711071619200"></a>BLE 1M</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p7110161672019"><a name="p7110161672019"></a><a name="p7110161672019"></a>10</p>
</td>
</tr>
<tr id="row811021615202"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p6110916192013"><a name="p6110916192013"></a><a name="p6110916192013"></a>BLE 2M</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p211021619206"><a name="p211021619206"></a><a name="p211021619206"></a>10</p>
</td>
</tr>
<tr id="row3110101652011"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p111101716122014"><a name="p111101716122014"></a><a name="p111101716122014"></a>SLE（FT1）</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1110116202012"><a name="p1110116202012"></a><a name="p1110116202012"></a>10</p>
</td>
</tr>
<tr id="row111014161205"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p31101716122013"><a name="p31101716122013"></a><a name="p31101716122013"></a>SLE（FT2）</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p10110121622018"><a name="p10110121622018"></a><a name="p10110121622018"></a>6</p>
</td>
</tr>
<tr id="row17110181618206"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p10110161612014"><a name="p10110161612014"></a><a name="p10110161612014"></a>SLE（FT4）</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1311051672018"><a name="p1311051672018"></a><a name="p1311051672018"></a>6</p>
</td>
</tr>
<tr id="row21101616142018"><td class="cellrowborder" rowspan="8" valign="top" width="26.919999999999998%" headers="mcps1.2.4.1.1 "><p id="p211010166207"><a name="p211010166207"></a><a name="p211010166207"></a>RX Sensitivity(dBm)</p>
</td>
<td class="cellrowborder" valign="top" width="36.78%" headers="mcps1.2.4.1.2 "><p id="p911011622018"><a name="p911011622018"></a><a name="p911011622018"></a>BR（GFSK）</p>
</td>
<td class="cellrowborder" valign="top" width="36.3%" headers="mcps1.2.4.1.3 "><p id="p1611021692020"><a name="p1611021692020"></a><a name="p1611021692020"></a>-94</p>
</td>
</tr>
<tr id="row211001614203"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p10110101620207"><a name="p10110101620207"></a><a name="p10110101620207"></a>2EDR（QPSK）</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p21101516102010"><a name="p21101516102010"></a><a name="p21101516102010"></a>-93</p>
</td>
</tr>
<tr id="row1811011618201"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p191104168202"><a name="p191104168202"></a><a name="p191104168202"></a>3EDR（8PSK）</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p17110161618202"><a name="p17110161618202"></a><a name="p17110161618202"></a>-87</p>
</td>
</tr>
<tr id="row611011162207"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p611021617204"><a name="p611021617204"></a><a name="p611021617204"></a>BLE 1M</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p18110151672018"><a name="p18110151672018"></a><a name="p18110151672018"></a>-96</p>
</td>
</tr>
<tr id="row17110181619206"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p511081615203"><a name="p511081615203"></a><a name="p511081615203"></a>BLE 2M</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p411051632015"><a name="p411051632015"></a><a name="p411051632015"></a>-93</p>
</td>
</tr>
<tr id="row91101816112011"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p10110181616208"><a name="p10110181616208"></a><a name="p10110181616208"></a>SLE（FT1）</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p7110016172010"><a name="p7110016172010"></a><a name="p7110016172010"></a>-96</p>
</td>
</tr>
<tr id="row3111616102018"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1211118164202"><a name="p1211118164202"></a><a name="p1211118164202"></a>SLE（FT2）</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1111191619208"><a name="p1111191619208"></a><a name="p1111191619208"></a>-99</p>
</td>
</tr>
<tr id="row141111016152017"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p211117168203"><a name="p211117168203"></a><a name="p211117168203"></a>SLE（FT4）</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p211116161202"><a name="p211116161202"></a><a name="p211116161202"></a>-107</p>
</td>
</tr>
</tbody>
</table>

## GNSS<a name="ZH-CN_TOPIC_0000002549833987"></a>

-   **[工作频段](#ZH-CN_TOPIC_0000002549714553)**  

-   **[GNSS灵敏度](#ZH-CN_TOPIC_0000002518354712)**  

### 工作频段<a name="ZH-CN_TOPIC_0000002549714553"></a>

**表 1**  GNSS工作频段

<a name="table7209103792020"></a>
<table><thead align="left"><tr id="row6228193715201"><th class="cellrowborder" valign="top" width="41.69%" id="mcps1.2.3.1.1"><p id="p142281437182019"><a name="p142281437182019"></a><a name="p142281437182019"></a>GNSS星系类型</p>
</th>
<th class="cellrowborder" valign="top" width="58.309999999999995%" id="mcps1.2.3.1.2"><p id="p1228337152016"><a name="p1228337152016"></a><a name="p1228337152016"></a>频段（MHz）</p>
</th>
</tr>
</thead>
<tbody><tr id="row322883792016"><td class="cellrowborder" valign="top" width="41.69%" headers="mcps1.2.3.1.1 "><p id="p1922883714203"><a name="p1922883714203"></a><a name="p1922883714203"></a>GPS</p>
</td>
<td class="cellrowborder" valign="top" width="58.309999999999995%" headers="mcps1.2.3.1.2 "><p id="p19228837152012"><a name="p19228837152012"></a><a name="p19228837152012"></a>1575.42&plusmn;1.023（L1）</p>
</td>
</tr>
<tr id="row2022812377204"><td class="cellrowborder" valign="top" width="41.69%" headers="mcps1.2.3.1.1 "><p id="p14228173716204"><a name="p14228173716204"></a><a name="p14228173716204"></a>BDS</p>
</td>
<td class="cellrowborder" valign="top" width="58.309999999999995%" headers="mcps1.2.3.1.2 "><p id="p19228163712209"><a name="p19228163712209"></a><a name="p19228163712209"></a>1561.098&plusmn;2.046（B1I）</p>
</td>
</tr>
<tr id="row1822863720205"><td class="cellrowborder" valign="top" width="41.69%" headers="mcps1.2.3.1.1 "><p id="p722820372202"><a name="p722820372202"></a><a name="p722820372202"></a>Galileo</p>
</td>
<td class="cellrowborder" valign="top" width="58.309999999999995%" headers="mcps1.2.3.1.2 "><p id="p202281137102020"><a name="p202281137102020"></a><a name="p202281137102020"></a>1575.42&plusmn;2.046（E1）</p>
</td>
</tr>
<tr id="row4228137142017"><td class="cellrowborder" valign="top" width="41.69%" headers="mcps1.2.3.1.1 "><p id="p1522863712201"><a name="p1522863712201"></a><a name="p1522863712201"></a>GLONASS</p>
</td>
<td class="cellrowborder" valign="top" width="58.309999999999995%" headers="mcps1.2.3.1.2 "><p id="p638913501099"><a name="p638913501099"></a><a name="p638913501099"></a>1597.5～1605.8（L1）</p>
</td>
</tr>
</tbody>
</table>

### GNSS灵敏度<a name="ZH-CN_TOPIC_0000002518354712"></a>

**表 1**  GNSS灵敏度

<a name="table14181115214201"></a>
<table><thead align="left"><tr id="row13200115212202"><th class="cellrowborder" valign="top" width="30.946905309469052%" id="mcps1.2.4.1.1"><p id="p2200652172016"><a name="p2200652172016"></a><a name="p2200652172016"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="32.14678532146785%" id="mcps1.2.4.1.2"><p id="p92007522202"><a name="p92007522202"></a><a name="p92007522202"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="36.906309369063095%" id="mcps1.2.4.1.3"><p id="p6200552162015"><a name="p6200552162015"></a><a name="p6200552162015"></a>典型值（dBm）</p>
</th>
</tr>
</thead>
<tbody><tr id="row15200185282012"><td class="cellrowborder" rowspan="3" valign="top" width="30.946905309469052%" headers="mcps1.2.4.1.1 "><p id="p17200155262012"><a name="p17200155262012"></a><a name="p17200155262012"></a>灵敏度</p>
</td>
<td class="cellrowborder" valign="top" width="32.14678532146785%" headers="mcps1.2.4.1.2 "><p id="p9200125212019"><a name="p9200125212019"></a><a name="p9200125212019"></a>冷启动捕获灵敏度</p>
</td>
<td class="cellrowborder" valign="top" width="36.906309369063095%" headers="mcps1.2.4.1.3 "><p id="p112001352182017"><a name="p112001352182017"></a><a name="p112001352182017"></a>-149</p>
</td>
</tr>
<tr id="row172003526207"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1720012520205"><a name="p1720012520205"></a><a name="p1720012520205"></a>热启动捕获灵敏度</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1520018529202"><a name="p1520018529202"></a><a name="p1520018529202"></a>-159</p>
</td>
</tr>
<tr id="row8200252132018"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1020065214209"><a name="p1020065214209"></a><a name="p1020065214209"></a>跟踪灵敏度</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p62003521205"><a name="p62003521205"></a><a name="p62003521205"></a>-162</p>
</td>
</tr>
</tbody>
</table>

# 热设计建议<a name="ZH-CN_TOPIC_0000002545901607"></a>

-   **[工作温度环境](#ZH-CN_TOPIC_0000002514301662)**  

-   **[电路热设计参考](#ZH-CN_TOPIC_0000002545781615)**  

## 工作温度环境<a name="ZH-CN_TOPIC_0000002514301662"></a>

模组的工作温度环境参数如[表1](#table1877mcpsimp)所示。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>超过扩展工作环境参数数值，可能导致模组内芯片物理损伤。
>-   T<sub>N</sub>温度范围表示当模块在此温度范围工作时，模块的相关性能满足3GPP标准要求。
>-   T<sub>E</sub>温度范围表示当模块在此温度范围工作时，模块仍能保持各功能正常工作状态。

**表 1**  极限工作环境参数

<a name="table1877mcpsimp"></a>
<table><thead align="left"><tr id="row1885mcpsimp"><th class="cellrowborder" valign="top" width="35%" id="mcps1.2.6.1.1"><p id="p1887mcpsimp"><a name="p1887mcpsimp"></a><a name="p1887mcpsimp"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="15.000000000000002%" id="mcps1.2.6.1.2"><p id="p1889mcpsimp"><a name="p1889mcpsimp"></a><a name="p1889mcpsimp"></a>符号</p>
</th>
<th class="cellrowborder" valign="top" width="19.000000000000004%" id="mcps1.2.6.1.3"><p id="p1891mcpsimp"><a name="p1891mcpsimp"></a><a name="p1891mcpsimp"></a>最小值</p>
</th>
<th class="cellrowborder" valign="top" width="18.000000000000004%" id="mcps1.2.6.1.4"><p id="p1893mcpsimp"><a name="p1893mcpsimp"></a><a name="p1893mcpsimp"></a>最大值</p>
</th>
<th class="cellrowborder" valign="top" width="13.000000000000004%" id="mcps1.2.6.1.5"><p id="p1895mcpsimp"><a name="p1895mcpsimp"></a><a name="p1895mcpsimp"></a>单位</p>
</th>
</tr>
</thead>
<tbody><tr id="row1897mcpsimp"><td class="cellrowborder" valign="top" width="35%" headers="mcps1.2.6.1.1 "><p id="p1899mcpsimp"><a name="p1899mcpsimp"></a><a name="p1899mcpsimp"></a>正常工作温度</p>
</td>
<td class="cellrowborder" valign="top" width="15.000000000000002%" headers="mcps1.2.6.1.2 "><p id="p1901mcpsimp"><a name="p1901mcpsimp"></a><a name="p1901mcpsimp"></a>T<sub id="sub1902mcpsimp"><a name="sub1902mcpsimp"></a><a name="sub1902mcpsimp"></a>N</sub></p>
</td>
<td class="cellrowborder" valign="top" width="19.000000000000004%" headers="mcps1.2.6.1.3 "><p id="p1904mcpsimp"><a name="p1904mcpsimp"></a><a name="p1904mcpsimp"></a>-30</p>
</td>
<td class="cellrowborder" valign="top" width="18.000000000000004%" headers="mcps1.2.6.1.4 "><p id="p1906mcpsimp"><a name="p1906mcpsimp"></a><a name="p1906mcpsimp"></a>75</p>
</td>
<td class="cellrowborder" valign="top" width="13.000000000000004%" headers="mcps1.2.6.1.5 "><p id="p1908mcpsimp"><a name="p1908mcpsimp"></a><a name="p1908mcpsimp"></a>℃</p>
</td>
</tr>
<tr id="row1909mcpsimp"><td class="cellrowborder" valign="top" width="35%" headers="mcps1.2.6.1.1 "><p id="p1911mcpsimp"><a name="p1911mcpsimp"></a><a name="p1911mcpsimp"></a>扩展工作温度</p>
</td>
<td class="cellrowborder" valign="top" width="15.000000000000002%" headers="mcps1.2.6.1.2 "><p id="p1913mcpsimp"><a name="p1913mcpsimp"></a><a name="p1913mcpsimp"></a>T<sub id="sub1914mcpsimp"><a name="sub1914mcpsimp"></a><a name="sub1914mcpsimp"></a>E</sub></p>
</td>
<td class="cellrowborder" valign="top" width="19.000000000000004%" headers="mcps1.2.6.1.3 "><p id="p1916mcpsimp"><a name="p1916mcpsimp"></a><a name="p1916mcpsimp"></a>-40</p>
</td>
<td class="cellrowborder" valign="top" width="18.000000000000004%" headers="mcps1.2.6.1.4 "><p id="p1918mcpsimp"><a name="p1918mcpsimp"></a><a name="p1918mcpsimp"></a>85</p>
</td>
<td class="cellrowborder" valign="top" width="13.000000000000004%" headers="mcps1.2.6.1.5 "><p id="p1920mcpsimp"><a name="p1920mcpsimp"></a><a name="p1920mcpsimp"></a>℃</p>
</td>
</tr>
<tr id="row1921mcpsimp"><td class="cellrowborder" valign="top" width="35%" headers="mcps1.2.6.1.1 "><p id="p1923mcpsimp"><a name="p1923mcpsimp"></a><a name="p1923mcpsimp"></a>焊接温度</p>
</td>
<td class="cellrowborder" valign="top" width="15.000000000000002%" headers="mcps1.2.6.1.2 "><p id="p1925mcpsimp"><a name="p1925mcpsimp"></a><a name="p1925mcpsimp"></a>T<sub id="sub1926mcpsimp"><a name="sub1926mcpsimp"></a><a name="sub1926mcpsimp"></a>P</sub></p>
</td>
<td class="cellrowborder" valign="top" width="19.000000000000004%" headers="mcps1.2.6.1.3 "><p id="p1928mcpsimp"><a name="p1928mcpsimp"></a><a name="p1928mcpsimp"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="18.000000000000004%" headers="mcps1.2.6.1.4 "><p id="p1930mcpsimp"><a name="p1930mcpsimp"></a><a name="p1930mcpsimp"></a>260</p>
</td>
<td class="cellrowborder" valign="top" width="13.000000000000004%" headers="mcps1.2.6.1.5 "><p id="p1932mcpsimp"><a name="p1932mcpsimp"></a><a name="p1932mcpsimp"></a>℃</p>
</td>
</tr>
</tbody>
</table>

## 电路热设计参考<a name="ZH-CN_TOPIC_0000002545781615"></a>

结合产品结构和热设计，器件布局建议如下：

-   单板上大功耗且易产生热量器件要均匀分布，避免局部过热，影响器件可靠性和散热效率。
-   合理设计结构，保证产品内部与外界有热交换途径。
-   对单板关键发热器件充分进行极端应用场景的温升测试，确保器件在安全的温度范围内长期可靠工作。
-   必要情况下，关键发热器件可以增加散热片，进一步提升散热效果。

# 焊接工艺建议<a name="ZH-CN_TOPIC_0000002514141740"></a>

-   **[概述](#ZH-CN_TOPIC_0000002545901609)**  

-   **[无铅回流焊工艺参数要求](#ZH-CN_TOPIC_0000002514301664)**  

-   **[混合回流焊工艺参数要求](#ZH-CN_TOPIC_0000002545781617)**  

## 概述<a name="ZH-CN_TOPIC_0000002545901609"></a>

【目的】Objective

本章提供了客户端在用模组SMT（Surface Mount Technology）时各温区温度基本设置的参数建议。

【适用范围】Scope

HiDiTingV100模组相关产品。

【基本信息】Basic information

提供给客户端的产品均为RoHS产品，即均是Lead-free（无铅）产品；本章主要介绍客户端在使用模组做回流焊时工艺控制：主要是无铅工艺和混合工艺两类。

【回流焊工艺控制】Reflow Chart

定义说明：

模组：给客户的模组均为RoHS产品，均满足无铅要求。

无铅工艺：所有器件（主板/所有IC/电容电阻等）均为无铅器件，并使用无铅锡膏的纯无铅工艺。

## 无铅回流焊工艺参数要求<a name="ZH-CN_TOPIC_0000002514301664"></a>

-   无铅回流焊接工艺曲线如[图1](#_fig0760045811)所示。

    **图 1**  无铅回流焊接工艺曲线<a name="_fig0760045811"></a>  
    ![](figures/无铅回流焊接工艺曲线.jpg "无铅回流焊接工艺曲线")

-   无铅回流焊工艺参数如[表1](#_table3079mcpsimp)所示。

    **表 1**  无铅回流焊工艺参数

    <a name="_table3079mcpsimp"></a>
    <table><thead align="left"><tr id="row2270mcpsimp"><th class="cellrowborder" valign="top" width="30%" id="mcps1.2.6.1.1"><p id="p2272mcpsimp"><a name="p2272mcpsimp"></a><a name="p2272mcpsimp"></a>区域</p>
    </th>
    <th class="cellrowborder" valign="top" width="14.000000000000002%" id="mcps1.2.6.1.2"><p id="p2274mcpsimp"><a name="p2274mcpsimp"></a><a name="p2274mcpsimp"></a>时间</p>
    </th>
    <th class="cellrowborder" valign="top" width="14.000000000000002%" id="mcps1.2.6.1.3"><p id="p2276mcpsimp"><a name="p2276mcpsimp"></a><a name="p2276mcpsimp"></a>升温速率</p>
    </th>
    <th class="cellrowborder" valign="top" width="17%" id="mcps1.2.6.1.4"><p id="p2278mcpsimp"><a name="p2278mcpsimp"></a><a name="p2278mcpsimp"></a>峰值温度</p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.2.6.1.5"><p id="p2280mcpsimp"><a name="p2280mcpsimp"></a><a name="p2280mcpsimp"></a>降温速率</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row2282mcpsimp"><td class="cellrowborder" valign="top" width="30%" headers="mcps1.2.6.1.1 "><p id="p2284mcpsimp"><a name="p2284mcpsimp"></a><a name="p2284mcpsimp"></a>预热区（40～150℃）</p>
    </td>
    <td class="cellrowborder" valign="top" width="14.000000000000002%" headers="mcps1.2.6.1.2 "><p id="p2286mcpsimp"><a name="p2286mcpsimp"></a><a name="p2286mcpsimp"></a>60s～150s</p>
    </td>
    <td class="cellrowborder" valign="top" width="14.000000000000002%" headers="mcps1.2.6.1.3 "><p id="p2288mcpsimp"><a name="p2288mcpsimp"></a><a name="p2288mcpsimp"></a>≤2.0℃/s</p>
    </td>
    <td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.6.1.4 "><p id="p2290mcpsimp"><a name="p2290mcpsimp"></a><a name="p2290mcpsimp"></a>-</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.6.1.5 "><p id="p2292mcpsimp"><a name="p2292mcpsimp"></a><a name="p2292mcpsimp"></a>-</p>
    </td>
    </tr>
    <tr id="row2293mcpsimp"><td class="cellrowborder" valign="top" width="30%" headers="mcps1.2.6.1.1 "><p id="p2295mcpsimp"><a name="p2295mcpsimp"></a><a name="p2295mcpsimp"></a>均温区（150～200℃）</p>
    </td>
    <td class="cellrowborder" valign="top" width="14.000000000000002%" headers="mcps1.2.6.1.2 "><p id="p2297mcpsimp"><a name="p2297mcpsimp"></a><a name="p2297mcpsimp"></a>60s～120s</p>
    </td>
    <td class="cellrowborder" valign="top" width="14.000000000000002%" headers="mcps1.2.6.1.3 "><p id="p2299mcpsimp"><a name="p2299mcpsimp"></a><a name="p2299mcpsimp"></a>＜1.0℃/s</p>
    </td>
    <td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.6.1.4 "><p id="p2301mcpsimp"><a name="p2301mcpsimp"></a><a name="p2301mcpsimp"></a>-</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.6.1.5 "><p id="p2303mcpsimp"><a name="p2303mcpsimp"></a><a name="p2303mcpsimp"></a>-</p>
    </td>
    </tr>
    <tr id="row2304mcpsimp"><td class="cellrowborder" valign="top" width="30%" headers="mcps1.2.6.1.1 "><p id="p2306mcpsimp"><a name="p2306mcpsimp"></a><a name="p2306mcpsimp"></a>回流区（＞217℃）</p>
    </td>
    <td class="cellrowborder" valign="top" width="14.000000000000002%" headers="mcps1.2.6.1.2 "><p id="p2308mcpsimp"><a name="p2308mcpsimp"></a><a name="p2308mcpsimp"></a>30s～90s</p>
    </td>
    <td class="cellrowborder" valign="top" width="14.000000000000002%" headers="mcps1.2.6.1.3 "><p id="p2310mcpsimp"><a name="p2310mcpsimp"></a><a name="p2310mcpsimp"></a>-</p>
    </td>
    <td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.6.1.4 "><p id="p2312mcpsimp"><a name="p2312mcpsimp"></a><a name="p2312mcpsimp"></a>230℃～260℃</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.6.1.5 "><p id="p2314mcpsimp"><a name="p2314mcpsimp"></a><a name="p2314mcpsimp"></a>-</p>
    </td>
    </tr>
    <tr id="row2315mcpsimp"><td class="cellrowborder" valign="top" width="30%" headers="mcps1.2.6.1.1 "><p id="p2317mcpsimp"><a name="p2317mcpsimp"></a><a name="p2317mcpsimp"></a>冷却区（Tmax～180℃）</p>
    </td>
    <td class="cellrowborder" valign="top" width="14.000000000000002%" headers="mcps1.2.6.1.2 "><p id="p2319mcpsimp"><a name="p2319mcpsimp"></a><a name="p2319mcpsimp"></a>-</p>
    </td>
    <td class="cellrowborder" valign="top" width="14.000000000000002%" headers="mcps1.2.6.1.3 "><p id="p2321mcpsimp"><a name="p2321mcpsimp"></a><a name="p2321mcpsimp"></a>-</p>
    </td>
    <td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.6.1.4 "><p id="p2323mcpsimp"><a name="p2323mcpsimp"></a><a name="p2323mcpsimp"></a>-</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.6.1.5 "><p id="p2325mcpsimp"><a name="p2325mcpsimp"></a><a name="p2325mcpsimp"></a>1.0℃/s≤Slope≤4.0℃/s</p>
    </td>
    </tr>
    </tbody>
    </table>

>![](public_sys-resources/icon-note.gif) **说明：** 
>-   预热区：温度40℃～150℃，温度上升速率控制在2℃/s左右，该温区时间为60～150 s。
>-   均温区：温度150℃～200℃，稳定缓慢升温，温度上升速率小于1℃/s，且该区域时间控制在60s\~120s**（注意：该区域一定缓慢受热，否则易导致焊接不良）**。
>-   回流区：温度217℃～Tmax～217℃，整个区间时间控制在30s～90s。
>-   冷却区：温度Tmax～180℃，温度下降速率最大不能超过4℃/s。
>-   温度从室温25℃升温到250℃时间不应该超过6分钟。
>-   该回流焊曲线仅为推荐值，客户端需根据实际生产情况做相应调整。
>-   回流时间以60s～90s为目标，对于一些热容较大无法满足时间要求的单板可将回流时间放宽至120s。封装体耐温标准参考IPC/JEDEC J-STD-020D标准，封装体测温方法参考JEP 140标准。

回流焊接工艺曲线测量方法：

JEP140推荐：对于厚度较小的器件，测量封装体温度时，直接将热电偶贴放在器件表面，对于厚度较大的器件，在器件表面钻孔埋入热电偶进行测量。由于量化器件厚度的要求，推荐全部采用在封装体表面钻孔埋入热电偶的方式（特别是薄器件，无法钻孔除外），如[图2](#_fig9143421646)所示。

**图 2**  封装体测温示意图<a name="_fig9143421646"></a>  
![](figures/封装体测温示意图.png "封装体测温示意图")

>![](public_sys-resources/icon-note.gif) **说明：** 
>上图仅为测温示意图，并非模组实际照片；仅供参考使用。

## 混合回流焊工艺参数要求<a name="ZH-CN_TOPIC_0000002545781617"></a>

回流焊接过程中，如果出现器件混装现象，应首先保证无铅器件的正常焊接。具体要求如[表1](#_table3225mcpsimp)所示。

**表 1**  混装回流焊工艺参数表

<a name="_table3225mcpsimp"></a>
<table><thead align="left"><tr id="row2874mcpsimp"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.2.6.1.1"><p id="p2876mcpsimp"><a name="p2876mcpsimp"></a><a name="p2876mcpsimp"></a>数值要求</p>
</th>
<th class="cellrowborder" valign="top" id="mcps1.2.6.1.2"><p id="p2878mcpsimp"><a name="p2878mcpsimp"></a><a name="p2878mcpsimp"></a>有铅BGA</p>
</th>
<th class="cellrowborder" valign="top" id="mcps1.2.6.1.3"><p id="p2880mcpsimp"><a name="p2880mcpsimp"></a><a name="p2880mcpsimp"></a>无铅BGA</p>
</th>
<th class="cellrowborder" valign="top" id="mcps1.2.6.1.4"><p id="p2882mcpsimp"><a name="p2882mcpsimp"></a><a name="p2882mcpsimp"></a>其他器件</p>
</th>
</tr>
</thead>
<tbody><tr id="row2884mcpsimp"><td class="cellrowborder" rowspan="2" valign="top" headers="mcps1.2.6.1.1 "><p id="p2886mcpsimp"><a name="p2886mcpsimp"></a><a name="p2886mcpsimp"></a>预热区（40℃～150℃）</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p2888mcpsimp"><a name="p2888mcpsimp"></a><a name="p2888mcpsimp"></a>时间</p>
</td>
<td class="cellrowborder" colspan="3" valign="top" headers="mcps1.2.6.1.2 mcps1.2.6.1.3 mcps1.2.6.1.4 "><p id="p2890mcpsimp"><a name="p2890mcpsimp"></a><a name="p2890mcpsimp"></a>60s～150s</p>
</td>
</tr>
<tr id="row2891mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p2893mcpsimp"><a name="p2893mcpsimp"></a><a name="p2893mcpsimp"></a>升温斜率</p>
</td>
<td class="cellrowborder" colspan="3" valign="top" headers="mcps1.2.6.1.1 mcps1.2.6.1.2 mcps1.2.6.1.3 "><p id="p2895mcpsimp"><a name="p2895mcpsimp"></a><a name="p2895mcpsimp"></a>＜2.5℃/s</p>
</td>
</tr>
<tr id="row2896mcpsimp"><td class="cellrowborder" rowspan="2" valign="top" headers="mcps1.2.6.1.1 "><p id="p2898mcpsimp"><a name="p2898mcpsimp"></a><a name="p2898mcpsimp"></a>均温区（150℃～183℃）</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p2900mcpsimp"><a name="p2900mcpsimp"></a><a name="p2900mcpsimp"></a>时间</p>
</td>
<td class="cellrowborder" colspan="3" valign="top" headers="mcps1.2.6.1.2 mcps1.2.6.1.3 mcps1.2.6.1.4 "><p id="p2902mcpsimp"><a name="p2902mcpsimp"></a><a name="p2902mcpsimp"></a>30s～90s</p>
</td>
</tr>
<tr id="row2903mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p2905mcpsimp"><a name="p2905mcpsimp"></a><a name="p2905mcpsimp"></a>升温斜率</p>
</td>
<td class="cellrowborder" colspan="3" valign="top" headers="mcps1.2.6.1.1 mcps1.2.6.1.2 mcps1.2.6.1.3 "><p id="p2907mcpsimp"><a name="p2907mcpsimp"></a><a name="p2907mcpsimp"></a>＜1.0℃/s</p>
</td>
</tr>
<tr id="row2908mcpsimp"><td class="cellrowborder" rowspan="2" valign="top" width="27.722772277227726%" headers="mcps1.2.6.1.1 "><p id="p2910mcpsimp"><a name="p2910mcpsimp"></a><a name="p2910mcpsimp"></a>回流区（＞183℃）</p>
</td>
<td class="cellrowborder" valign="top" width="15.841584158415841%" headers="mcps1.2.6.1.1 "><p id="p2912mcpsimp"><a name="p2912mcpsimp"></a><a name="p2912mcpsimp"></a>峰值温度</p>
</td>
<td class="cellrowborder" valign="top" width="17.82178217821782%" headers="mcps1.2.6.1.2 "><p id="p2914mcpsimp"><a name="p2914mcpsimp"></a><a name="p2914mcpsimp"></a>210℃～240℃</p>
</td>
<td class="cellrowborder" valign="top" width="17.82178217821782%" headers="mcps1.2.6.1.3 "><p id="p2916mcpsimp"><a name="p2916mcpsimp"></a><a name="p2916mcpsimp"></a>220℃～240℃</p>
</td>
<td class="cellrowborder" valign="top" width="20.792079207920793%" headers="mcps1.2.6.1.4 "><p id="p2918mcpsimp"><a name="p2918mcpsimp"></a><a name="p2918mcpsimp"></a>210℃～245℃</p>
</td>
</tr>
<tr id="row2919mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p2921mcpsimp"><a name="p2921mcpsimp"></a><a name="p2921mcpsimp"></a>时间</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p2923mcpsimp"><a name="p2923mcpsimp"></a><a name="p2923mcpsimp"></a>30s～120s</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.2 "><p id="p2925mcpsimp"><a name="p2925mcpsimp"></a><a name="p2925mcpsimp"></a>60s～120s</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.3 "><p id="p2927mcpsimp"><a name="p2927mcpsimp"></a><a name="p2927mcpsimp"></a>30s～120s</p>
</td>
</tr>
<tr id="row2928mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p2930mcpsimp"><a name="p2930mcpsimp"></a><a name="p2930mcpsimp"></a>冷却区（Tmax～150℃）</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p2932mcpsimp"><a name="p2932mcpsimp"></a><a name="p2932mcpsimp"></a>降温斜率</p>
</td>
<td class="cellrowborder" colspan="3" valign="top" headers="mcps1.2.6.1.2 mcps1.2.6.1.3 mcps1.2.6.1.4 "><p id="p2934mcpsimp"><a name="p2934mcpsimp"></a><a name="p2934mcpsimp"></a>1.0℃/s≤Slope≤4.0℃/s</p>
</td>
</tr>
</tbody>
</table>

>![](public_sys-resources/icon-note.gif) **说明：** 
>以上工艺参数要求均针对焊点温度。单板上焊点最热点和最冷点均需要满足以上规范要求。

曲线调制中，还需要满足单板上元器件的封装体耐温要求。封装体耐温标准按照IPC/JEDEC J-STD-020D标准，封装体测温方法按照JEP 140标准。

# 存储和包装<a name="ZH-CN_TOPIC_0000002514141742"></a>

-   **[存放与使用](#ZH-CN_TOPIC_0000002545901611)**  

-   **[重新烘烤](#ZH-CN_TOPIC_0000002514301666)**  

-   **[包装规格](#ZH-CN_TOPIC_0000002545781619)**  

## 存放与使用<a name="ZH-CN_TOPIC_0000002545901611"></a>

**【**使用范围】

所有HiDiTingV100模组的存放和使用。

**【**存放环境】

建议产品真空包装存放，-40℃≤存放温度≤150℃。推荐存放在25℃的环境温度下。

**【**存储期限】（shelf life）

存放环境<30°C/60% RH下，真空包装存放，存储期限（shelf life）不少于12个月。

**【**车间寿命】（floor life）

在环境条件<30°C/60%下，floor life参照表如[表1](#_table659mcpsimp)所示。

**表 1**  车间寿命（floor life）参照表

<a name="_table659mcpsimp"></a>
<table><thead align="left"><tr id="row2450mcpsimp"><th class="cellrowborder" valign="top" width="20.150000000000002%" id="mcps1.2.3.1.1"><p id="p2452mcpsimp"><a name="p2452mcpsimp"></a><a name="p2452mcpsimp"></a>潮湿敏感等级</p>
<p id="p2453mcpsimp"><a name="p2453mcpsimp"></a><a name="p2453mcpsimp"></a>（MSL）</p>
</th>
<th class="cellrowborder" valign="top" width="79.85%" id="mcps1.2.3.1.2"><p id="p2455mcpsimp"><a name="p2455mcpsimp"></a><a name="p2455mcpsimp"></a>含义（即拆分后放存条件及最长时间）</p>
</th>
</tr>
</thead>
<tbody><tr id="row2457mcpsimp"><td class="cellrowborder" valign="top" width="20.150000000000002%" headers="mcps1.2.3.1.1 "><p id="p2459mcpsimp"><a name="p2459mcpsimp"></a><a name="p2459mcpsimp"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="79.85%" headers="mcps1.2.3.1.2 "><p id="p2461mcpsimp"><a name="p2461mcpsimp"></a><a name="p2461mcpsimp"></a>无限制，环境温湿度≦30℃/85% RH（Relative Humidity）</p>
</td>
</tr>
<tr id="row2462mcpsimp"><td class="cellrowborder" valign="top" width="20.150000000000002%" headers="mcps1.2.3.1.1 "><p id="p2464mcpsimp"><a name="p2464mcpsimp"></a><a name="p2464mcpsimp"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="79.85%" headers="mcps1.2.3.1.2 "><p id="p2466mcpsimp"><a name="p2466mcpsimp"></a><a name="p2466mcpsimp"></a>1年，30℃/60%RH。</p>
</td>
</tr>
<tr id="row2467mcpsimp"><td class="cellrowborder" valign="top" width="20.150000000000002%" headers="mcps1.2.3.1.1 "><p id="p2469mcpsimp"><a name="p2469mcpsimp"></a><a name="p2469mcpsimp"></a>2a</p>
</td>
<td class="cellrowborder" valign="top" width="79.85%" headers="mcps1.2.3.1.2 "><p id="p2471mcpsimp"><a name="p2471mcpsimp"></a><a name="p2471mcpsimp"></a>1月，30℃/60%RH。</p>
</td>
</tr>
<tr id="row2472mcpsimp"><td class="cellrowborder" valign="top" width="20.150000000000002%" headers="mcps1.2.3.1.1 "><p id="p2474mcpsimp"><a name="p2474mcpsimp"></a><a name="p2474mcpsimp"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="79.85%" headers="mcps1.2.3.1.2 "><p id="p2476mcpsimp"><a name="p2476mcpsimp"></a><a name="p2476mcpsimp"></a>1周，30℃/60%RH。</p>
</td>
</tr>
<tr id="row2477mcpsimp"><td class="cellrowborder" valign="top" width="20.150000000000002%" headers="mcps1.2.3.1.1 "><p id="p2479mcpsimp"><a name="p2479mcpsimp"></a><a name="p2479mcpsimp"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="79.85%" headers="mcps1.2.3.1.2 "><p id="p2481mcpsimp"><a name="p2481mcpsimp"></a><a name="p2481mcpsimp"></a>72h，30℃/60%RH。</p>
</td>
</tr>
<tr id="row2482mcpsimp"><td class="cellrowborder" valign="top" width="20.150000000000002%" headers="mcps1.2.3.1.1 "><p id="p2484mcpsimp"><a name="p2484mcpsimp"></a><a name="p2484mcpsimp"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="79.85%" headers="mcps1.2.3.1.2 "><p id="p2486mcpsimp"><a name="p2486mcpsimp"></a><a name="p2486mcpsimp"></a>48h，30℃/60%RH。</p>
</td>
</tr>
<tr id="row2487mcpsimp"><td class="cellrowborder" valign="top" width="20.150000000000002%" headers="mcps1.2.3.1.1 "><p id="p2489mcpsimp"><a name="p2489mcpsimp"></a><a name="p2489mcpsimp"></a>5a</p>
</td>
<td class="cellrowborder" valign="top" width="79.85%" headers="mcps1.2.3.1.2 "><p id="p2491mcpsimp"><a name="p2491mcpsimp"></a><a name="p2491mcpsimp"></a>24h，30℃/60%RH。</p>
</td>
</tr>
<tr id="row2492mcpsimp"><td class="cellrowborder" valign="top" width="20.150000000000002%" headers="mcps1.2.3.1.1 "><p id="p2494mcpsimp"><a name="p2494mcpsimp"></a><a name="p2494mcpsimp"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="79.85%" headers="mcps1.2.3.1.2 "><p id="p2496mcpsimp"><a name="p2496mcpsimp"></a><a name="p2496mcpsimp"></a>Time on Label，30℃/60%RH。</p>
</td>
</tr>
</tbody>
</table>

**【**潮敏产品的使用】

-   产品在≦30℃/60%RH下连续或累计暴露超过2个小时，建议进行重新烘烤后再真空干燥包装。
-   产品在≦30℃/60%RH下暴露累计没有超过2个小时，可以不用重新烘烤，但要更换新的干燥剂，进行真空干燥包装。
-   本产品的潮敏参数等级为：3级。
-   本文没有提到的存储及使用原则，请直接参考JEDEC J-STD-033A。

## 重新烘烤<a name="ZH-CN_TOPIC_0000002514301666"></a>

【适用产品】

所有HiDiTingV100模组（潮敏产品）

【使用范围】

需要重新烘烤的模组（潮敏产品）

【重新烘烤参考表】

重新烘烤要求如[表1](#_table4462mcpsimp)所示。

**表 1**  模组重新烘烤参考表

<a name="_table4462mcpsimp"></a>
<table><thead align="left"><tr id="row1461mcpsimp"><th class="cellrowborder" valign="top" width="17.82178217821782%" id="mcps1.2.6.1.1"><p id="p1463mcpsimp"><a name="p1463mcpsimp"></a><a name="p1463mcpsimp"></a>模组厚度</p>
</th>
<th class="cellrowborder" valign="top" width="13.861386138613863%" id="mcps1.2.6.1.2"><p id="p1465mcpsimp"><a name="p1465mcpsimp"></a><a name="p1465mcpsimp"></a>MSL潮敏等级</p>
</th>
<th class="cellrowborder" valign="top" width="18.81188118811881%" id="mcps1.2.6.1.3"><p id="p1467mcpsimp"><a name="p1467mcpsimp"></a><a name="p1467mcpsimp"></a>烘烤125℃</p>
</th>
<th class="cellrowborder" valign="top" width="25.742574257425744%" id="mcps1.2.6.1.4"><p id="p1469mcpsimp"><a name="p1469mcpsimp"></a><a name="p1469mcpsimp"></a>烘烤90℃/≦5% RH</p>
</th>
<th class="cellrowborder" valign="top" width="23.762376237623766%" id="mcps1.2.6.1.5"><p id="p1471mcpsimp"><a name="p1471mcpsimp"></a><a name="p1471mcpsimp"></a>烘烤40℃/≦5% RH</p>
</th>
</tr>
</thead>
<tbody><tr id="row1473mcpsimp"><td class="cellrowborder" rowspan="5" valign="top" width="17.82178217821782%" headers="mcps1.2.6.1.1 "><p id="p1475mcpsimp"><a name="p1475mcpsimp"></a><a name="p1475mcpsimp"></a>≤2.0mm</p>
</td>
<td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.6.1.2 "><p id="p1477mcpsimp"><a name="p1477mcpsimp"></a><a name="p1477mcpsimp"></a>2a</p>
</td>
<td class="cellrowborder" valign="top" width="18.81188118811881%" headers="mcps1.2.6.1.3 "><p id="p1479mcpsimp"><a name="p1479mcpsimp"></a><a name="p1479mcpsimp"></a>16h</p>
</td>
<td class="cellrowborder" valign="top" width="25.742574257425744%" headers="mcps1.2.6.1.4 "><p id="p1481mcpsimp"><a name="p1481mcpsimp"></a><a name="p1481mcpsimp"></a>2days</p>
</td>
<td class="cellrowborder" valign="top" width="23.762376237623766%" headers="mcps1.2.6.1.5 "><p id="p1483mcpsimp"><a name="p1483mcpsimp"></a><a name="p1483mcpsimp"></a>22days</p>
</td>
</tr>
<tr id="row1484mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p1486mcpsimp"><a name="p1486mcpsimp"></a><a name="p1486mcpsimp"></a>3</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.2 "><p id="p1488mcpsimp"><a name="p1488mcpsimp"></a><a name="p1488mcpsimp"></a>17h</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.3 "><p id="p1490mcpsimp"><a name="p1490mcpsimp"></a><a name="p1490mcpsimp"></a>2days</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.4 "><p id="p1492mcpsimp"><a name="p1492mcpsimp"></a><a name="p1492mcpsimp"></a>23days</p>
</td>
</tr>
<tr id="row1493mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p1495mcpsimp"><a name="p1495mcpsimp"></a><a name="p1495mcpsimp"></a>4</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.2 "><p id="p1497mcpsimp"><a name="p1497mcpsimp"></a><a name="p1497mcpsimp"></a>20h</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.3 "><p id="p1499mcpsimp"><a name="p1499mcpsimp"></a><a name="p1499mcpsimp"></a>3days</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.4 "><p id="p1501mcpsimp"><a name="p1501mcpsimp"></a><a name="p1501mcpsimp"></a>28days</p>
</td>
</tr>
<tr id="row1502mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p1504mcpsimp"><a name="p1504mcpsimp"></a><a name="p1504mcpsimp"></a>5</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.2 "><p id="p1506mcpsimp"><a name="p1506mcpsimp"></a><a name="p1506mcpsimp"></a>25h</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.3 "><p id="p1508mcpsimp"><a name="p1508mcpsimp"></a><a name="p1508mcpsimp"></a>4days</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.4 "><p id="p1510mcpsimp"><a name="p1510mcpsimp"></a><a name="p1510mcpsimp"></a>35days</p>
</td>
</tr>
<tr id="row1511mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p1513mcpsimp"><a name="p1513mcpsimp"></a><a name="p1513mcpsimp"></a>5a</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.2 "><p id="p1515mcpsimp"><a name="p1515mcpsimp"></a><a name="p1515mcpsimp"></a>40h</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.3 "><p id="p1517mcpsimp"><a name="p1517mcpsimp"></a><a name="p1517mcpsimp"></a>6days</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.4 "><p id="p1519mcpsimp"><a name="p1519mcpsimp"></a><a name="p1519mcpsimp"></a>56days</p>
</td>
</tr>
<tr id="row1520mcpsimp"><td class="cellrowborder" rowspan="5" valign="top" width="17.82178217821782%" headers="mcps1.2.6.1.1 "><p id="p1522mcpsimp"><a name="p1522mcpsimp"></a><a name="p1522mcpsimp"></a>≤4.5mm</p>
</td>
<td class="cellrowborder" valign="top" width="13.861386138613863%" headers="mcps1.2.6.1.2 "><p id="p1524mcpsimp"><a name="p1524mcpsimp"></a><a name="p1524mcpsimp"></a>2a</p>
</td>
<td class="cellrowborder" valign="top" width="18.81188118811881%" headers="mcps1.2.6.1.3 "><p id="p1526mcpsimp"><a name="p1526mcpsimp"></a><a name="p1526mcpsimp"></a>48h</p>
</td>
<td class="cellrowborder" valign="top" width="25.742574257425744%" headers="mcps1.2.6.1.4 "><p id="p1528mcpsimp"><a name="p1528mcpsimp"></a><a name="p1528mcpsimp"></a>7days</p>
</td>
<td class="cellrowborder" valign="top" width="23.762376237623766%" headers="mcps1.2.6.1.5 "><p id="p1530mcpsimp"><a name="p1530mcpsimp"></a><a name="p1530mcpsimp"></a>67days</p>
</td>
</tr>
<tr id="row1531mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p1533mcpsimp"><a name="p1533mcpsimp"></a><a name="p1533mcpsimp"></a>3</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.2 "><p id="p1535mcpsimp"><a name="p1535mcpsimp"></a><a name="p1535mcpsimp"></a>48h</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.3 "><p id="p1537mcpsimp"><a name="p1537mcpsimp"></a><a name="p1537mcpsimp"></a>8days</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.4 "><p id="p1539mcpsimp"><a name="p1539mcpsimp"></a><a name="p1539mcpsimp"></a>67days</p>
</td>
</tr>
<tr id="row1540mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p1542mcpsimp"><a name="p1542mcpsimp"></a><a name="p1542mcpsimp"></a>4</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.2 "><p id="p1544mcpsimp"><a name="p1544mcpsimp"></a><a name="p1544mcpsimp"></a>48h</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.3 "><p id="p1546mcpsimp"><a name="p1546mcpsimp"></a><a name="p1546mcpsimp"></a>10days</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.4 "><p id="p1548mcpsimp"><a name="p1548mcpsimp"></a><a name="p1548mcpsimp"></a>67days</p>
</td>
</tr>
<tr id="row1549mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p1551mcpsimp"><a name="p1551mcpsimp"></a><a name="p1551mcpsimp"></a>5</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.2 "><p id="p1553mcpsimp"><a name="p1553mcpsimp"></a><a name="p1553mcpsimp"></a>48h</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.3 "><p id="p1555mcpsimp"><a name="p1555mcpsimp"></a><a name="p1555mcpsimp"></a>10days</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.4 "><p id="p1557mcpsimp"><a name="p1557mcpsimp"></a><a name="p1557mcpsimp"></a>67days</p>
</td>
</tr>
<tr id="row1558mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.6.1.1 "><p id="p1560mcpsimp"><a name="p1560mcpsimp"></a><a name="p1560mcpsimp"></a>5a</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.2 "><p id="p1562mcpsimp"><a name="p1562mcpsimp"></a><a name="p1562mcpsimp"></a>48h</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.3 "><p id="p1564mcpsimp"><a name="p1564mcpsimp"></a><a name="p1564mcpsimp"></a>10days</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.1.4 "><p id="p1566mcpsimp"><a name="p1566mcpsimp"></a><a name="p1566mcpsimp"></a>67days</p>
</td>
</tr>
</tbody>
</table>

对于HiDiTingV100芯片默认卷带包装，厚度1.02mm的建议烘烤条件为：

-   tray包装类为温度125±5℃，相对湿度不大于60%，烘烤7小时；或者温度90±5℃，相对湿度不大于5%，烘烤23小时。
-   料带包装类（不耐高温，烘烤前注意包装耐高温度）为温度40℃，相对湿度不大于5%，烘烤216小时。

>![](public_sys-resources/icon-note.gif) **说明：** 
>[表1](#_table4462mcpsimp)中显示的均是受潮后，必须的最小烘烤时间。重新烘烤优先选择低温烘烤。详细情况请参考JEDEC。

## 包装规格<a name="ZH-CN_TOPIC_0000002545781619"></a>

载带包装的尺寸图标如下。

**图 1**  载带尺寸图<a name="fig144mcpsimp"></a>  
![](figures/载带尺寸图.png "载带尺寸图")

**图 2**  胶盘尺寸图<a name="fig146mcpsimp"></a>  
![](figures/胶盘尺寸图.png "胶盘尺寸图")

**图 3**  胶盘实物样品图<a name="fig148mcpsimp"></a>  
![](figures/胶盘实物样品图.png "胶盘实物样品图")

# 关键器件规格<a name="ZH-CN_TOPIC_0000002514141744"></a>

本文提到的关键器件是指器件参数会直接影响产品的关键性能、系统可靠性等关键指标，客户要遵循下面的器件选型建议。

-   **[Nor Flash](#ZH-CN_TOPIC_0000002545901613)**  

-   **[Nand Flash](#ZH-CN_TOPIC_0000002514141746)**  

## Nor Flash<a name="ZH-CN_TOPIC_0000002545901613"></a>

-   **[Nor Flash规格](#ZH-CN_TOPIC_0000002514301668)**  

-   **[Nor Flash测试指导](#ZH-CN_TOPIC_0000002545781621)**  

### Nor Flash规格<a name="ZH-CN_TOPIC_0000002514301668"></a>

单芯片SPI Nor Flash基本要求：

-   支持单线SPI及4线QSPI接口。
-   主芯片Nor Flash接口最高工作速率100MHz。
-   根据实际应用规模进行容量选型：≥128Mbit。
-   工作电压：1.8V（1.7V～1.95V）。
-   支持4KB sector/32KB block/64KB block/chip ERASE。

### Nor Flash测试指导<a name="ZH-CN_TOPIC_0000002545781621"></a>

建议针对Nor Flash器件做如下测试：

-   单块写、读：成功读出并跟写入的数据相同。
-   多块写、读：成功读出并跟写入的数据相同。
-   擦除：可成功擦除访问地址的数据。
-   测试最大100MHz速率下，可正常读写。
-   可靠性测试：低温-40℃/高温85℃环境下，循环读写操作＞24h，检测写入数据正确性。

## Nand Flash<a name="ZH-CN_TOPIC_0000002514141746"></a>

-   **[Nand Flash规格](#ZH-CN_TOPIC_0000002545901615)**  

-   **[Nand Flash测试指导](#ZH-CN_TOPIC_0000002514301670)**  

### Nand Flash规格<a name="ZH-CN_TOPIC_0000002545901615"></a>

单芯片SPI Nand Flash基本要求：

-   支持单线SPI及4线QSPI接口。
-   主芯片Nand Flash接口最高工作速率67MHz。
-   根据实际应用规模进行容量选型：推荐1Gbit。
-   工作电压：1.8V（1.7V～1.95V）。
-   IO驱动可调：优选IO的Driver strength有多档位可调。
-   Flash ID唯一。

### Nand Flash测试指导<a name="ZH-CN_TOPIC_0000002514301670"></a>

建议针对Nand Flash器件做如下测试：

-   单块写、读：成功读出并跟写入的数据相同。
-   多块写、读：成功读出并跟写入的数据相同。
-   擦除：可成功擦除访问地址的数据。
-   测试最大67MHz速率下，可正常读写。
-   可靠性测试：低温-40℃/高温85℃环境下，循环读写操作＞24h，检测写入数据正确性。

# 硬件设计Checklist<a name="ZH-CN_TOPIC_0000002545781623"></a>

用户进行原理图、PCB设计时，需要按照如下checklist进行自检。

**表 1**  HiDiTingV100原理图设计checklist

<a name="table1583mcpsimp"></a>
<table><thead align="left"><tr id="row1591mcpsimp"><th class="cellrowborder" valign="top" width="6.93%" id="mcps1.2.6.1.1"><p id="p1593mcpsimp"><a name="p1593mcpsimp"></a><a name="p1593mcpsimp"></a>序号</p>
</th>
<th class="cellrowborder" valign="top" width="10.89%" id="mcps1.2.6.1.2"><p id="p1595mcpsimp"><a name="p1595mcpsimp"></a><a name="p1595mcpsimp"></a>模块</p>
</th>
<th class="cellrowborder" valign="top" width="61.39%" id="mcps1.2.6.1.3"><p id="p1597mcpsimp"><a name="p1597mcpsimp"></a><a name="p1597mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="10.89%" id="mcps1.2.6.1.4"><p id="p1599mcpsimp"><a name="p1599mcpsimp"></a><a name="p1599mcpsimp"></a>必要性</p>
</th>
<th class="cellrowborder" valign="top" width="9.9%" id="mcps1.2.6.1.5"><p id="p1601mcpsimp"><a name="p1601mcpsimp"></a><a name="p1601mcpsimp"></a>自检结果</p>
</th>
</tr>
</thead>
<tbody><tr id="row1603mcpsimp"><td class="cellrowborder" valign="top" width="6.93%" headers="mcps1.2.6.1.1 "><p id="p1605mcpsimp"><a name="p1605mcpsimp"></a><a name="p1605mcpsimp"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.2 "><p id="p1607mcpsimp"><a name="p1607mcpsimp"></a><a name="p1607mcpsimp"></a>PMU</p>
</td>
<td class="cellrowborder" valign="top" width="61.39%" headers="mcps1.2.6.1.3 "><p id="p1609mcpsimp"><a name="p1609mcpsimp"></a><a name="p1609mcpsimp"></a>IOLDO2外部仅可用作AUX_ADC采样上拉电阻电源，不要直接给外设供电。</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.4 "><p id="p1611mcpsimp"><a name="p1611mcpsimp"></a><a name="p1611mcpsimp"></a>必须</p>
</td>
<td class="cellrowborder" valign="top" width="9.9%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1613mcpsimp"><td class="cellrowborder" valign="top" width="6.93%" headers="mcps1.2.6.1.1 "><p id="p1615mcpsimp"><a name="p1615mcpsimp"></a><a name="p1615mcpsimp"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.2 "><p id="p1617mcpsimp"><a name="p1617mcpsimp"></a><a name="p1617mcpsimp"></a>PMU</p>
</td>
<td class="cellrowborder" valign="top" width="61.39%" headers="mcps1.2.6.1.3 "><p id="p1619mcpsimp"><a name="p1619mcpsimp"></a><a name="p1619mcpsimp"></a>IOLDO6仅限于给Nor Flash供电以及作为其上拉电源域使用。</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.4 "><p id="p1621mcpsimp"><a name="p1621mcpsimp"></a><a name="p1621mcpsimp"></a>必须</p>
</td>
<td class="cellrowborder" valign="top" width="9.9%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1623mcpsimp"><td class="cellrowborder" valign="top" width="6.93%" headers="mcps1.2.6.1.1 "><p id="p1625mcpsimp"><a name="p1625mcpsimp"></a><a name="p1625mcpsimp"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.2 "><p id="p1627mcpsimp"><a name="p1627mcpsimp"></a><a name="p1627mcpsimp"></a>上电复位</p>
</td>
<td class="cellrowborder" valign="top" width="61.39%" headers="mcps1.2.6.1.3 "><p id="p1629mcpsimp"><a name="p1629mcpsimp"></a><a name="p1629mcpsimp"></a>PWR_KEY管脚芯片内部有上拉电阻，按键低电平开机。不使用时悬空。</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.4 "><p id="p1631mcpsimp"><a name="p1631mcpsimp"></a><a name="p1631mcpsimp"></a>必须</p>
</td>
<td class="cellrowborder" valign="top" width="9.9%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1633mcpsimp"><td class="cellrowborder" valign="top" width="6.93%" headers="mcps1.2.6.1.1 "><p id="p1635mcpsimp"><a name="p1635mcpsimp"></a><a name="p1635mcpsimp"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.2 "><p id="p1637mcpsimp"><a name="p1637mcpsimp"></a><a name="p1637mcpsimp"></a>上电复位</p>
</td>
<td class="cellrowborder" valign="top" width="61.39%" headers="mcps1.2.6.1.3 "><p id="p1639mcpsimp"><a name="p1639mcpsimp"></a><a name="p1639mcpsimp"></a>VBUS_SENSE管脚电压上限1.8V，当使用VBUS开机时需要外接分压电阻。</p>
<p id="p1640mcpsimp"><a name="p1640mcpsimp"></a><a name="p1640mcpsimp"></a>管脚不使用时，外接下拉电阻到地，不允许悬空。</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.4 "><p id="p1642mcpsimp"><a name="p1642mcpsimp"></a><a name="p1642mcpsimp"></a>建议</p>
</td>
<td class="cellrowborder" valign="top" width="9.9%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1644mcpsimp"><td class="cellrowborder" valign="top" width="6.93%" headers="mcps1.2.6.1.1 "><p id="p1646mcpsimp"><a name="p1646mcpsimp"></a><a name="p1646mcpsimp"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.2 "><p id="p1648mcpsimp"><a name="p1648mcpsimp"></a><a name="p1648mcpsimp"></a>上电复位</p>
</td>
<td class="cellrowborder" valign="top" width="61.39%" headers="mcps1.2.6.1.3 "><p id="p1650mcpsimp"><a name="p1650mcpsimp"></a><a name="p1650mcpsimp"></a>CAT1默认上电启动，如需控制其上下电，请预留GPIO控制CAT1_VBAT通断。</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.4 "><p id="p1652mcpsimp"><a name="p1652mcpsimp"></a><a name="p1652mcpsimp"></a>建议</p>
</td>
<td class="cellrowborder" valign="top" width="9.9%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1654mcpsimp"><td class="cellrowborder" valign="top" width="6.93%" headers="mcps1.2.6.1.1 "><p id="p1656mcpsimp"><a name="p1656mcpsimp"></a><a name="p1656mcpsimp"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.2 "><p id="p1658mcpsimp"><a name="p1658mcpsimp"></a><a name="p1658mcpsimp"></a>RF</p>
</td>
<td class="cellrowborder" valign="top" width="61.39%" headers="mcps1.2.6.1.3 "><p id="p1660mcpsimp"><a name="p1660mcpsimp"></a><a name="p1660mcpsimp"></a>RFIO通道建议预留pi型匹配。</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.4 "><p id="p1662mcpsimp"><a name="p1662mcpsimp"></a><a name="p1662mcpsimp"></a>建议</p>
</td>
<td class="cellrowborder" valign="top" width="9.9%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1664mcpsimp"><td class="cellrowborder" valign="top" width="6.93%" headers="mcps1.2.6.1.1 "><p id="p1666mcpsimp"><a name="p1666mcpsimp"></a><a name="p1666mcpsimp"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.2 "><p id="p1668mcpsimp"><a name="p1668mcpsimp"></a><a name="p1668mcpsimp"></a>接口</p>
</td>
<td class="cellrowborder" valign="top" width="61.39%" headers="mcps1.2.6.1.3 "><p id="p1670mcpsimp"><a name="p1670mcpsimp"></a><a name="p1670mcpsimp"></a>外接显示屏使用QSPI2、Nand Flash使用QSPI1、Nor Flash使用QSPI0。</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.4 "><p id="p1672mcpsimp"><a name="p1672mcpsimp"></a><a name="p1672mcpsimp"></a>建议</p>
</td>
<td class="cellrowborder" valign="top" width="9.9%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1674mcpsimp"><td class="cellrowborder" valign="top" width="6.93%" headers="mcps1.2.6.1.1 "><p id="p1676mcpsimp"><a name="p1676mcpsimp"></a><a name="p1676mcpsimp"></a>8</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.2 "><p id="p1678mcpsimp"><a name="p1678mcpsimp"></a><a name="p1678mcpsimp"></a>接口</p>
</td>
<td class="cellrowborder" valign="top" width="61.39%" headers="mcps1.2.6.1.3 "><p id="p1680mcpsimp"><a name="p1680mcpsimp"></a><a name="p1680mcpsimp"></a>S_AGPIO17/S_AGPIO18为空片烧录管脚，板级预留测试点用于产线烧录。模组默认USB烧录。</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.4 "><p id="p1682mcpsimp"><a name="p1682mcpsimp"></a><a name="p1682mcpsimp"></a>必须</p>
</td>
<td class="cellrowborder" valign="top" width="9.9%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1684mcpsimp"><td class="cellrowborder" valign="top" width="6.93%" headers="mcps1.2.6.1.1 "><p id="p1686mcpsimp"><a name="p1686mcpsimp"></a><a name="p1686mcpsimp"></a>9</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.2 "><p id="p1688mcpsimp"><a name="p1688mcpsimp"></a><a name="p1688mcpsimp"></a>接口</p>
</td>
<td class="cellrowborder" valign="top" width="61.39%" headers="mcps1.2.6.1.3 "><p id="p1690mcpsimp"><a name="p1690mcpsimp"></a><a name="p1690mcpsimp"></a>模组内芯片不支持防倒灌，不支持使用1.8V IO倒灌方式驱动LED。</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.4 "><p id="p1692mcpsimp"><a name="p1692mcpsimp"></a><a name="p1692mcpsimp"></a>必须</p>
</td>
<td class="cellrowborder" valign="top" width="9.9%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1694mcpsimp"><td class="cellrowborder" valign="top" width="6.93%" headers="mcps1.2.6.1.1 "><p id="p1696mcpsimp"><a name="p1696mcpsimp"></a><a name="p1696mcpsimp"></a>10</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.2 "><p id="p1698mcpsimp"><a name="p1698mcpsimp"></a><a name="p1698mcpsimp"></a>接口</p>
</td>
<td class="cellrowborder" valign="top" width="61.39%" headers="mcps1.2.6.1.3 "><p id="p1700mcpsimp"><a name="p1700mcpsimp"></a><a name="p1700mcpsimp"></a>所有对外接口建议全部预留ESD防护管。</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.4 "><p id="p1702mcpsimp"><a name="p1702mcpsimp"></a><a name="p1702mcpsimp"></a>建议</p>
</td>
<td class="cellrowborder" valign="top" width="9.9%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1704mcpsimp"><td class="cellrowborder" valign="top" width="6.93%" headers="mcps1.2.6.1.1 "><p id="p1706mcpsimp"><a name="p1706mcpsimp"></a><a name="p1706mcpsimp"></a>11</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.2 "><p id="p1708mcpsimp"><a name="p1708mcpsimp"></a><a name="p1708mcpsimp"></a>音频</p>
</td>
<td class="cellrowborder" valign="top" width="61.39%" headers="mcps1.2.6.1.3 "><p id="p1710mcpsimp"><a name="p1710mcpsimp"></a><a name="p1710mcpsimp"></a>支持1路模拟MIC输入，AMIC器件与模组端须加隔直电容，隔直电容必须选用2.2μF及以上电容。</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.4 "><p id="p1712mcpsimp"><a name="p1712mcpsimp"></a><a name="p1712mcpsimp"></a>必须</p>
</td>
<td class="cellrowborder" valign="top" width="9.9%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1714mcpsimp"><td class="cellrowborder" valign="top" width="6.93%" headers="mcps1.2.6.1.1 "><p id="p1716mcpsimp"><a name="p1716mcpsimp"></a><a name="p1716mcpsimp"></a>12</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.2 "><p id="p1718mcpsimp"><a name="p1718mcpsimp"></a><a name="p1718mcpsimp"></a>音频</p>
</td>
<td class="cellrowborder" valign="top" width="61.39%" headers="mcps1.2.6.1.3 "><p id="p1720mcpsimp"><a name="p1720mcpsimp"></a><a name="p1720mcpsimp"></a>芯片HS为差分输出，芯片输出端通过RC滤波后，再给到模拟PA。</p>
</td>
<td class="cellrowborder" valign="top" width="10.89%" headers="mcps1.2.6.1.4 "><p id="p1722mcpsimp"><a name="p1722mcpsimp"></a><a name="p1722mcpsimp"></a>必须</p>
</td>
<td class="cellrowborder" valign="top" width="9.9%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
</tbody>
</table>

**表 2**  HiDiTingV100 PCB设计checklist

<a name="table1724mcpsimp"></a>
<table><thead align="left"><tr id="row1732mcpsimp"><th class="cellrowborder" valign="top" width="7.000000000000001%" id="mcps1.2.6.1.1"><p id="p1734mcpsimp"><a name="p1734mcpsimp"></a><a name="p1734mcpsimp"></a>序号</p>
</th>
<th class="cellrowborder" valign="top" width="11%" id="mcps1.2.6.1.2"><p id="p1736mcpsimp"><a name="p1736mcpsimp"></a><a name="p1736mcpsimp"></a>模块</p>
</th>
<th class="cellrowborder" valign="top" width="62%" id="mcps1.2.6.1.3"><p id="p1738mcpsimp"><a name="p1738mcpsimp"></a><a name="p1738mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="10%" id="mcps1.2.6.1.4"><p id="p1740mcpsimp"><a name="p1740mcpsimp"></a><a name="p1740mcpsimp"></a>必要性</p>
</th>
<th class="cellrowborder" valign="top" width="10%" id="mcps1.2.6.1.5"><p id="p1742mcpsimp"><a name="p1742mcpsimp"></a><a name="p1742mcpsimp"></a>自检结果</p>
</th>
</tr>
</thead>
<tbody><tr id="row1744mcpsimp"><td class="cellrowborder" valign="top" width="7.000000000000001%" headers="mcps1.2.6.1.1 "><p id="p1746mcpsimp"><a name="p1746mcpsimp"></a><a name="p1746mcpsimp"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="11%" headers="mcps1.2.6.1.2 "><p id="p1748mcpsimp"><a name="p1748mcpsimp"></a><a name="p1748mcpsimp"></a>RF</p>
</td>
<td class="cellrowborder" valign="top" width="62%" headers="mcps1.2.6.1.3 "><p id="p1750mcpsimp"><a name="p1750mcpsimp"></a><a name="p1750mcpsimp"></a>芯片到pi型匹配的射频表层走线尽量短；建议内层走线。射频前端走线阻抗控制50Ω。</p>
</td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.6.1.4 "><p id="p1752mcpsimp"><a name="p1752mcpsimp"></a><a name="p1752mcpsimp"></a>建议</p>
</td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1754mcpsimp"><td class="cellrowborder" valign="top" width="7.000000000000001%" headers="mcps1.2.6.1.1 "><p id="p1756mcpsimp"><a name="p1756mcpsimp"></a><a name="p1756mcpsimp"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="11%" headers="mcps1.2.6.1.2 "><p id="p1758mcpsimp"><a name="p1758mcpsimp"></a><a name="p1758mcpsimp"></a>RF</p>
</td>
<td class="cellrowborder" valign="top" width="62%" headers="mcps1.2.6.1.3 "><p id="p1760mcpsimp"><a name="p1760mcpsimp"></a><a name="p1760mcpsimp"></a>RFIO的layout阻抗配置要求：VSWR＝1.5，Phase＝-90&deg;～-80&deg;。</p>
</td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.6.1.4 "><p id="p1762mcpsimp"><a name="p1762mcpsimp"></a><a name="p1762mcpsimp"></a>必须</p>
</td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1764mcpsimp"><td class="cellrowborder" valign="top" width="7.000000000000001%" headers="mcps1.2.6.1.1 "><p id="p1766mcpsimp"><a name="p1766mcpsimp"></a><a name="p1766mcpsimp"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="11%" headers="mcps1.2.6.1.2 "><p id="p1768mcpsimp"><a name="p1768mcpsimp"></a><a name="p1768mcpsimp"></a>模拟</p>
</td>
<td class="cellrowborder" valign="top" width="62%" headers="mcps1.2.6.1.3 "><p id="p1770mcpsimp"><a name="p1770mcpsimp"></a><a name="p1770mcpsimp"></a>ADC模拟信号立体包地保护。</p>
</td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.6.1.4 "><p id="p1772mcpsimp"><a name="p1772mcpsimp"></a><a name="p1772mcpsimp"></a>建议</p>
</td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1774mcpsimp"><td class="cellrowborder" valign="top" width="7.000000000000001%" headers="mcps1.2.6.1.1 "><p id="p1776mcpsimp"><a name="p1776mcpsimp"></a><a name="p1776mcpsimp"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="11%" headers="mcps1.2.6.1.2 "><p id="p1778mcpsimp"><a name="p1778mcpsimp"></a><a name="p1778mcpsimp"></a>CODEC</p>
</td>
<td class="cellrowborder" valign="top" width="62%" headers="mcps1.2.6.1.3 "><p id="p1780mcpsimp"><a name="p1780mcpsimp"></a><a name="p1780mcpsimp"></a>AMIC差分输入信号按照差分紧耦合走线，四面包地。</p>
</td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.6.1.4 "><p id="p1782mcpsimp"><a name="p1782mcpsimp"></a><a name="p1782mcpsimp"></a>建议</p>
</td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1784mcpsimp"><td class="cellrowborder" valign="top" width="7.000000000000001%" headers="mcps1.2.6.1.1 "><p id="p1786mcpsimp"><a name="p1786mcpsimp"></a><a name="p1786mcpsimp"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="11%" headers="mcps1.2.6.1.2 "><p id="p1788mcpsimp"><a name="p1788mcpsimp"></a><a name="p1788mcpsimp"></a>CODEC</p>
</td>
<td class="cellrowborder" valign="top" width="62%" headers="mcps1.2.6.1.3 "><p id="p1790mcpsimp"><a name="p1790mcpsimp"></a><a name="p1790mcpsimp"></a>HSP/N到负载端的电阻控制在0.05Ω以下，走线宽度不小于6mil。按照差分紧耦合走线，四面包地（非射频地）。</p>
</td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.6.1.4 "><p id="p1792mcpsimp"><a name="p1792mcpsimp"></a><a name="p1792mcpsimp"></a>建议</p>
</td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
<tr id="row1794mcpsimp"><td class="cellrowborder" valign="top" width="7.000000000000001%" headers="mcps1.2.6.1.1 "><p id="p1796mcpsimp"><a name="p1796mcpsimp"></a><a name="p1796mcpsimp"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="11%" headers="mcps1.2.6.1.2 "><p id="p1798mcpsimp"><a name="p1798mcpsimp"></a><a name="p1798mcpsimp"></a>接口</p>
</td>
<td class="cellrowborder" valign="top" width="62%" headers="mcps1.2.6.1.3 "><p id="p1800mcpsimp"><a name="p1800mcpsimp"></a><a name="p1800mcpsimp"></a>高速IO、USB等接口避免stub走线。</p>
</td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.6.1.4 "><p id="p1802mcpsimp"><a name="p1802mcpsimp"></a><a name="p1802mcpsimp"></a>建议</p>
</td>
<td class="cellrowborder" valign="top" width="10%" headers="mcps1.2.6.1.5 ">&nbsp;&nbsp;</td>
</tr>
</tbody>
</table>

# 缩略语<a name="ZH-CN_TOPIC_0000002514158636"></a>

<a name="table3165mcpsimp"></a>
<table><tbody><tr id="row3171mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3173mcpsimp"><a name="p3173mcpsimp"></a><a name="p3173mcpsimp"></a><strong id="b3174mcpsimp"><a name="b3174mcpsimp"></a><a name="b3174mcpsimp"></a>A</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3177mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3179mcpsimp"><a name="p3179mcpsimp"></a><a name="p3179mcpsimp"></a><strong id="b3180mcpsimp"><a name="b3180mcpsimp"></a><a name="b3180mcpsimp"></a>ADC</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3182mcpsimp"><a name="p3182mcpsimp"></a><a name="p3182mcpsimp"></a>Analog-to-Digital Converter</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3184mcpsimp"><a name="p3184mcpsimp"></a><a name="p3184mcpsimp"></a>模拟数字转换器</p>
</td>
</tr>
<tr id="row3185mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3187mcpsimp"><a name="p3187mcpsimp"></a><a name="p3187mcpsimp"></a><strong id="b3188mcpsimp"><a name="b3188mcpsimp"></a><a name="b3188mcpsimp"></a>AMIC</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3190mcpsimp"><a name="p3190mcpsimp"></a><a name="p3190mcpsimp"></a>Analog Microphone</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3192mcpsimp"><a name="p3192mcpsimp"></a><a name="p3192mcpsimp"></a>模拟麦克风</p>
</td>
</tr>
<tr id="row3193mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3195mcpsimp"><a name="p3195mcpsimp"></a><a name="p3195mcpsimp"></a><strong id="b3196mcpsimp"><a name="b3196mcpsimp"></a><a name="b3196mcpsimp"></a>AUXADC</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3198mcpsimp"><a name="p3198mcpsimp"></a><a name="p3198mcpsimp"></a>Auxiliary Digital-to-Analog Converter</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3200mcpsimp"><a name="p3200mcpsimp"></a><a name="p3200mcpsimp"></a>辅助模拟数字转换器</p>
</td>
</tr>
<tr id="row3201mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3205mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3207mcpsimp"><a name="p3207mcpsimp"></a><a name="p3207mcpsimp"></a><strong id="b3208mcpsimp"><a name="b3208mcpsimp"></a><a name="b3208mcpsimp"></a>C</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row144971407424"><td class="cellrowborder" style="border:none" valign="top" width="21.33%"><p id="p04973019427"><a name="p04973019427"></a><a name="p04973019427"></a><strong id="b186686634214"><a name="b186686634214"></a><a name="b186686634214"></a>CAT1</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="44.67%"><p id="p749718010421"><a name="p749718010421"></a><a name="p749718010421"></a>Category 1</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="34%"><p id="p1749700194220"><a name="p1749700194220"></a><a name="p1749700194220"></a>4G物联网终端</p>
</td>
</tr>
<tr id="row3219mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3223mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3225mcpsimp"><a name="p3225mcpsimp"></a><a name="p3225mcpsimp"></a><strong id="b3226mcpsimp"><a name="b3226mcpsimp"></a><a name="b3226mcpsimp"></a>D</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3237mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3239mcpsimp"><a name="p3239mcpsimp"></a><a name="p3239mcpsimp"></a><strong id="b3240mcpsimp"><a name="b3240mcpsimp"></a><a name="b3240mcpsimp"></a>DMIC</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3242mcpsimp"><a name="p3242mcpsimp"></a><a name="p3242mcpsimp"></a>Digital Microphone</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3244mcpsimp"><a name="p3244mcpsimp"></a><a name="p3244mcpsimp"></a>数字麦克风</p>
</td>
</tr>
<tr id="row15605144024310"><td class="cellrowborder" style="border:none" valign="top" width="21.33%"><p id="p160524017434"><a name="p160524017434"></a><a name="p160524017434"></a><strong id="b162184453437"><a name="b162184453437"></a><a name="b162184453437"></a>DSI</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="44.67%"><p id="p160504064314"><a name="p160504064314"></a><a name="p160504064314"></a>Display Serial Interface</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="34%"><p id="p136059406433"><a name="p136059406433"></a><a name="p136059406433"></a>显示串行接口</p>
</td>
</tr>
<tr id="row3253mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3257mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3259mcpsimp"><a name="p3259mcpsimp"></a><a name="p3259mcpsimp"></a><strong id="b3260mcpsimp"><a name="b3260mcpsimp"></a><a name="b3260mcpsimp"></a>E</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row1898883964410"><td class="cellrowborder" style="border:none" valign="top" width="21.33%"><p id="p16988163964416"><a name="p16988163964416"></a><a name="p16988163964416"></a><strong id="b1150775034418"><a name="b1150775034418"></a><a name="b1150775034418"></a>eMMC</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="44.67%"><p id="p159886397446"><a name="p159886397446"></a><a name="p159886397446"></a>Embedded MultiMediaCard</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="34%"><p id="p169889391440"><a name="p169889391440"></a><a name="p169889391440"></a>嵌入式多媒体卡</p>
</td>
</tr>
<tr id="row3263mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3265mcpsimp"><a name="p3265mcpsimp"></a><a name="p3265mcpsimp"></a><strong id="b3266mcpsimp"><a name="b3266mcpsimp"></a><a name="b3266mcpsimp"></a>ESD</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3268mcpsimp"><a name="p3268mcpsimp"></a><a name="p3268mcpsimp"></a>Electrostatic Discharge</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3270mcpsimp"><a name="p3270mcpsimp"></a><a name="p3270mcpsimp"></a>静电放电</p>
</td>
</tr>
<tr id="row3279mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3283mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3285mcpsimp"><a name="p3285mcpsimp"></a><a name="p3285mcpsimp"></a><strong id="b3286mcpsimp"><a name="b3286mcpsimp"></a><a name="b3286mcpsimp"></a>F</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3289mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3291mcpsimp"><a name="p3291mcpsimp"></a><a name="p3291mcpsimp"></a><strong id="b3292mcpsimp"><a name="b3292mcpsimp"></a><a name="b3292mcpsimp"></a>FEM</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3294mcpsimp"><a name="p3294mcpsimp"></a><a name="p3294mcpsimp"></a>Front End Module</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3296mcpsimp"><a name="p3296mcpsimp"></a><a name="p3296mcpsimp"></a>射频前端模组</p>
</td>
</tr>
<tr id="row1952346165613"><td class="cellrowborder" style="border:none" valign="top" width="21.33%"><p id="p65313465564"><a name="p65313465564"></a><a name="p65313465564"></a><strong id="b195421648302"><a name="b195421648302"></a><a name="b195421648302"></a>FS</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="44.67%"><p id="p65334635616"><a name="p65334635616"></a><a name="p65334635616"></a>Full-Speed</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="34%"><p id="p1534467561"><a name="p1534467561"></a><a name="p1534467561"></a>全速</p>
</td>
</tr>
<tr id="row3297mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3301mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3303mcpsimp"><a name="p3303mcpsimp"></a><a name="p3303mcpsimp"></a><strong id="b3304mcpsimp"><a name="b3304mcpsimp"></a><a name="b3304mcpsimp"></a>G</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row92491249174112"><td class="cellrowborder" style="border:none" valign="top" width="21.33%"><p id="p1325013492417"><a name="p1325013492417"></a><a name="p1325013492417"></a><strong id="b4839356184110"><a name="b4839356184110"></a><a name="b4839356184110"></a>GNSS</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="44.67%"><p id="p64414188325"><a name="p64414188325"></a><a name="p64414188325"></a>Global Navigation Satellite System</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="34%"><p id="p194412181322"><a name="p194412181322"></a><a name="p194412181322"></a>全球卫星导航系统</p>
</td>
</tr>
<tr id="row3307mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3309mcpsimp"><a name="p3309mcpsimp"></a><a name="p3309mcpsimp"></a><strong id="b3310mcpsimp"><a name="b3310mcpsimp"></a><a name="b3310mcpsimp"></a>GPIO</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3312mcpsimp"><a name="p3312mcpsimp"></a><a name="p3312mcpsimp"></a>General Purpose Input/Output</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3314mcpsimp"><a name="p3314mcpsimp"></a><a name="p3314mcpsimp"></a>通用输入输出</p>
</td>
</tr>
<tr id="row3315mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row257715884319"><td class="cellrowborder" style="border:none" valign="top" width="21.33%"><p id="p115781488432"><a name="p115781488432"></a><a name="p115781488432"></a><strong id="b611571444310"><a name="b611571444310"></a><a name="b611571444310"></a>H</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row141251250153112"><td class="cellrowborder" style="border:none" valign="top" width="21.33%"><p id="p11252503317"><a name="p11252503317"></a><a name="p11252503317"></a><strong id="b17117114164316"><a name="b17117114164316"></a><a name="b17117114164316"></a>HS</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="44.67%"><p id="p41251150123118"><a name="p41251150123118"></a><a name="p41251150123118"></a>High-Speed</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="34%"><p id="p18126185063110"><a name="p18126185063110"></a><a name="p18126185063110"></a>高速</p>
</td>
</tr>
<tr id="row11578688431"><td class="cellrowborder" style="border:none" valign="top" width="21.33%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3319mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3321mcpsimp"><a name="p3321mcpsimp"></a><a name="p3321mcpsimp"></a><strong id="b3322mcpsimp"><a name="b3322mcpsimp"></a><a name="b3322mcpsimp"></a>I</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3325mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3327mcpsimp"><a name="p3327mcpsimp"></a><a name="p3327mcpsimp"></a><strong id="b3328mcpsimp"><a name="b3328mcpsimp"></a><a name="b3328mcpsimp"></a>I2C</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3330mcpsimp"><a name="p3330mcpsimp"></a><a name="p3330mcpsimp"></a>Inter-Integrated Circuit</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3332mcpsimp"><a name="p3332mcpsimp"></a><a name="p3332mcpsimp"></a>内部整合电路</p>
</td>
</tr>
<tr id="row3333mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3335mcpsimp"><a name="p3335mcpsimp"></a><a name="p3335mcpsimp"></a><strong id="b3336mcpsimp"><a name="b3336mcpsimp"></a><a name="b3336mcpsimp"></a>I2S</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3338mcpsimp"><a name="p3338mcpsimp"></a><a name="p3338mcpsimp"></a>Inter-IC Sound</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3340mcpsimp"><a name="p3340mcpsimp"></a><a name="p3340mcpsimp"></a>I2S总线</p>
</td>
</tr>
<tr id="row3341mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3343mcpsimp"><a name="p3343mcpsimp"></a><a name="p3343mcpsimp"></a><strong id="b3344mcpsimp"><a name="b3344mcpsimp"></a><a name="b3344mcpsimp"></a>IPC</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3346mcpsimp"><a name="p3346mcpsimp"></a><a name="p3346mcpsimp"></a>Inter-Processor Communication</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3348mcpsimp"><a name="p3348mcpsimp"></a><a name="p3348mcpsimp"></a>核间通信</p>
</td>
</tr>
<tr id="row163860219546"><td class="cellrowborder" style="border:none" valign="top" width="21.33%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3353mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3355mcpsimp"><a name="p3355mcpsimp"></a><a name="p3355mcpsimp"></a><strong id="b3356mcpsimp"><a name="b3356mcpsimp"></a><a name="b3356mcpsimp"></a>M</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3359mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3361mcpsimp"><a name="p3361mcpsimp"></a><a name="p3361mcpsimp"></a><strong id="b3362mcpsimp"><a name="b3362mcpsimp"></a><a name="b3362mcpsimp"></a>MCU</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3364mcpsimp"><a name="p3364mcpsimp"></a><a name="p3364mcpsimp"></a>Micro Controller Unit</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3366mcpsimp"><a name="p3366mcpsimp"></a><a name="p3366mcpsimp"></a>微控制单元</p>
</td>
</tr>
<tr id="row3367mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3369mcpsimp"><a name="p3369mcpsimp"></a><a name="p3369mcpsimp"></a><strong id="b3370mcpsimp"><a name="b3370mcpsimp"></a><a name="b3370mcpsimp"></a>MIC</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3372mcpsimp"><a name="p3372mcpsimp"></a><a name="p3372mcpsimp"></a>Microphone</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3374mcpsimp"><a name="p3374mcpsimp"></a><a name="p3374mcpsimp"></a>麦克风</p>
</td>
</tr>
<tr id="row3375mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3397mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3399mcpsimp"><a name="p3399mcpsimp"></a><a name="p3399mcpsimp"></a><strong id="b3400mcpsimp"><a name="b3400mcpsimp"></a><a name="b3400mcpsimp"></a>P</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3403mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3405mcpsimp"><a name="p3405mcpsimp"></a><a name="p3405mcpsimp"></a><strong id="b3406mcpsimp"><a name="b3406mcpsimp"></a><a name="b3406mcpsimp"></a>PA</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3408mcpsimp"><a name="p3408mcpsimp"></a><a name="p3408mcpsimp"></a>Power Amplifier</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3410mcpsimp"><a name="p3410mcpsimp"></a><a name="p3410mcpsimp"></a>功率放大器</p>
</td>
</tr>
<tr id="row3411mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3413mcpsimp"><a name="p3413mcpsimp"></a><a name="p3413mcpsimp"></a><strong id="b3414mcpsimp"><a name="b3414mcpsimp"></a><a name="b3414mcpsimp"></a>PCB</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3416mcpsimp"><a name="p3416mcpsimp"></a><a name="p3416mcpsimp"></a>Printed Circuit Board</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3418mcpsimp"><a name="p3418mcpsimp"></a><a name="p3418mcpsimp"></a>印刷电路板</p>
</td>
</tr>
<tr id="row3427mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3429mcpsimp"><a name="p3429mcpsimp"></a><a name="p3429mcpsimp"></a><strong id="b3430mcpsimp"><a name="b3430mcpsimp"></a><a name="b3430mcpsimp"></a>PCM</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3432mcpsimp"><a name="p3432mcpsimp"></a><a name="p3432mcpsimp"></a>Pulse Code Modulation</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3434mcpsimp"><a name="p3434mcpsimp"></a><a name="p3434mcpsimp"></a>脉冲编码调制</p>
</td>
</tr>
<tr id="row3435mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3437mcpsimp"><a name="p3437mcpsimp"></a><a name="p3437mcpsimp"></a><strong id="b3438mcpsimp"><a name="b3438mcpsimp"></a><a name="b3438mcpsimp"></a>PDM</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3440mcpsimp"><a name="p3440mcpsimp"></a><a name="p3440mcpsimp"></a>Pulse Density Modulation</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3442mcpsimp"><a name="p3442mcpsimp"></a><a name="p3442mcpsimp"></a>脉冲密度调制</p>
</td>
</tr>
<tr id="row3451mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3453mcpsimp"><a name="p3453mcpsimp"></a><a name="p3453mcpsimp"></a><strong id="b3454mcpsimp"><a name="b3454mcpsimp"></a><a name="b3454mcpsimp"></a>PMU</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3456mcpsimp"><a name="p3456mcpsimp"></a><a name="p3456mcpsimp"></a>Power Management Unit</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3458mcpsimp"><a name="p3458mcpsimp"></a><a name="p3458mcpsimp"></a>电源管理单元</p>
</td>
</tr>
<tr id="row3459mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3461mcpsimp"><a name="p3461mcpsimp"></a><a name="p3461mcpsimp"></a><strong id="b3462mcpsimp"><a name="b3462mcpsimp"></a><a name="b3462mcpsimp"></a>PWM</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3464mcpsimp"><a name="p3464mcpsimp"></a><a name="p3464mcpsimp"></a>Pulse-Width Modulation</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3466mcpsimp"><a name="p3466mcpsimp"></a><a name="p3466mcpsimp"></a>脉冲宽度调制</p>
</td>
</tr>
<tr id="row3467mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3471mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3473mcpsimp"><a name="p3473mcpsimp"></a><a name="p3473mcpsimp"></a><strong id="b3474mcpsimp"><a name="b3474mcpsimp"></a><a name="b3474mcpsimp"></a>Q</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3477mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3479mcpsimp"><a name="p3479mcpsimp"></a><a name="p3479mcpsimp"></a><strong id="b3480mcpsimp"><a name="b3480mcpsimp"></a><a name="b3480mcpsimp"></a>QSPI</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3482mcpsimp"><a name="p3482mcpsimp"></a><a name="p3482mcpsimp"></a>Queued Serial Peripheral Interface</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3484mcpsimp"><a name="p3484mcpsimp"></a><a name="p3484mcpsimp"></a>队列串行外设接口</p>
</td>
</tr>
<tr id="row3485mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3489mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3491mcpsimp"><a name="p3491mcpsimp"></a><a name="p3491mcpsimp"></a><strong id="b3492mcpsimp"><a name="b3492mcpsimp"></a><a name="b3492mcpsimp"></a>S</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row12101742134617"><td class="cellrowborder" style="border:none" valign="top" width="21.33%"><p id="p410118422469"><a name="p410118422469"></a><a name="p410118422469"></a><strong id="b913565313463"><a name="b913565313463"></a><a name="b913565313463"></a>SCL</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="44.67%"><p id="p16101642134611"><a name="p16101642134611"></a><a name="p16101642134611"></a>Serial Clock</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="34%"><p id="p8101134217460"><a name="p8101134217460"></a><a name="p8101134217460"></a>串行时钟线</p>
</td>
</tr>
<tr id="row1281684815466"><td class="cellrowborder" style="border:none" valign="top" width="21.33%"><p id="p381617485464"><a name="p381617485464"></a><a name="p381617485464"></a><strong id="b191362053114610"><a name="b191362053114610"></a><a name="b191362053114610"></a>SDA</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="44.67%"><p id="p18816144813462"><a name="p18816144813462"></a><a name="p18816144813462"></a>Serial Data</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="34%"><p id="p178163486461"><a name="p178163486461"></a><a name="p178163486461"></a>串行数据线</p>
</td>
</tr>
<tr id="row3495mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3497mcpsimp"><a name="p3497mcpsimp"></a><a name="p3497mcpsimp"></a><strong id="b3498mcpsimp"><a name="b3498mcpsimp"></a><a name="b3498mcpsimp"></a>SDIO</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3500mcpsimp"><a name="p3500mcpsimp"></a><a name="p3500mcpsimp"></a>Secure Digital Input/Output</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3502mcpsimp"><a name="p3502mcpsimp"></a><a name="p3502mcpsimp"></a>安全数字输入输出接口</p>
</td>
</tr>
<tr id="row3503mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3505mcpsimp"><a name="p3505mcpsimp"></a><a name="p3505mcpsimp"></a><strong id="b3506mcpsimp"><a name="b3506mcpsimp"></a><a name="b3506mcpsimp"></a>SMT</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3508mcpsimp"><a name="p3508mcpsimp"></a><a name="p3508mcpsimp"></a>Surface Mount Technology</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3510mcpsimp"><a name="p3510mcpsimp"></a><a name="p3510mcpsimp"></a>表面贴装技术</p>
</td>
</tr>
<tr id="row3511mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3513mcpsimp"><a name="p3513mcpsimp"></a><a name="p3513mcpsimp"></a><strong id="b3514mcpsimp"><a name="b3514mcpsimp"></a><a name="b3514mcpsimp"></a>SPI</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3516mcpsimp"><a name="p3516mcpsimp"></a><a name="p3516mcpsimp"></a>Synchronous Peripheral Interface</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3518mcpsimp"><a name="p3518mcpsimp"></a><a name="p3518mcpsimp"></a>串行外设接口</p>
</td>
</tr>
<tr id="row3519mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3523mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3525mcpsimp"><a name="p3525mcpsimp"></a><a name="p3525mcpsimp"></a><strong id="b3526mcpsimp"><a name="b3526mcpsimp"></a><a name="b3526mcpsimp"></a>T</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row7349135911489"><td class="cellrowborder" style="border:none" valign="top" width="21.33%"><p id="p1634915910483"><a name="p1634915910483"></a><a name="p1634915910483"></a><strong id="b14360171316491"><a name="b14360171316491"></a><a name="b14360171316491"></a>TDM</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="44.67%"><p id="p10350145914486"><a name="p10350145914486"></a><a name="p10350145914486"></a>Time Division Multiplexing</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="34%"><p id="p103501959204819"><a name="p103501959204819"></a><a name="p103501959204819"></a>时分复用</p>
</td>
</tr>
<tr id="row3537mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3541mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3543mcpsimp"><a name="p3543mcpsimp"></a><a name="p3543mcpsimp"></a><strong id="b3544mcpsimp"><a name="b3544mcpsimp"></a><a name="b3544mcpsimp"></a>U</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%">&nbsp;&nbsp;</td>
</tr>
<tr id="row3547mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3549mcpsimp"><a name="p3549mcpsimp"></a><a name="p3549mcpsimp"></a><strong id="b3550mcpsimp"><a name="b3550mcpsimp"></a><a name="b3550mcpsimp"></a>UART</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3552mcpsimp"><a name="p3552mcpsimp"></a><a name="p3552mcpsimp"></a>Universal Asynchronous Receiver/Transmitter</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3554mcpsimp"><a name="p3554mcpsimp"></a><a name="p3554mcpsimp"></a>通用异步收发器</p>
</td>
</tr>
<tr id="row3563mcpsimp"><td class="nocellnorowborder" style="border:none" valign="top" width="21.33%"><p id="p3565mcpsimp"><a name="p3565mcpsimp"></a><a name="p3565mcpsimp"></a><strong id="b3566mcpsimp"><a name="b3566mcpsimp"></a><a name="b3566mcpsimp"></a>USB</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.67%"><p id="p3568mcpsimp"><a name="p3568mcpsimp"></a><a name="p3568mcpsimp"></a>Universal Serial Bus</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="34%"><p id="p3570mcpsimp"><a name="p3570mcpsimp"></a><a name="p3570mcpsimp"></a>通用串行总线</p>
</td>
</tr>
<tr id="row14987102617409"><td class="cellrowborder" style="border:none" valign="top" width="21.33%"><p id="p1198812265404"><a name="p1198812265404"></a><a name="p1198812265404"></a><strong id="b627234164014"><a name="b627234164014"></a><a name="b627234164014"></a>USIM</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="44.67%"><p id="p10988626104010"><a name="p10988626104010"></a><a name="p10988626104010"></a>Universal Subscriber Identity Module</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="34%"><p id="p19988926204015"><a name="p19988926204015"></a><a name="p19988926204015"></a>通用用户识别模块</p>
</td>
</tr>
</tbody>
</table>

