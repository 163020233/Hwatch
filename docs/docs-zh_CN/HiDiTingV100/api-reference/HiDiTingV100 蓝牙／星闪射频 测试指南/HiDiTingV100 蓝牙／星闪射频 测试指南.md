# 前言<a name="ZH-CN_TOPIC_0000002516141531"></a>

**概述<a name="zh-cn_topic_0000002512316477_section4537382116410"></a>**

本文档主要介绍HiDiTingV100的射频非信令相关的测试指令和指令的组包示例，测试前务必阅读注意事项。

**读者对象<a name="zh-cn_topic_0000002512316477_section4378592816410"></a>**

本文档主要适用于以下工程师：

-   技术支持工程师。
-   软件开发工程师。

**符号约定<a name="zh-cn_topic_0000002512316477_section133020216410"></a>**

在本文中可能出现下列标志,它们所代表的含义如下。

<a name="zh-cn_topic_0000002512316477_table2622507016410"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002512316477_row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002512316477_p6450074116410"><a name="zh-cn_topic_0000002512316477_p6450074116410"></a><a name="zh-cn_topic_0000002512316477_p6450074116410"></a><strong id="zh-cn_topic_0000002512316477_b2136615816410"><a name="zh-cn_topic_0000002512316477_b2136615816410"></a><a name="zh-cn_topic_0000002512316477_b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002512316477_p5435366816410"><a name="zh-cn_topic_0000002512316477_p5435366816410"></a><a name="zh-cn_topic_0000002512316477_p5435366816410"></a><strong id="zh-cn_topic_0000002512316477_b5941558116410"><a name="zh-cn_topic_0000002512316477_b5941558116410"></a><a name="zh-cn_topic_0000002512316477_b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002512316477_row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316477_p3734547016410"><a name="zh-cn_topic_0000002512316477_p3734547016410"></a><a name="zh-cn_topic_0000002512316477_p3734547016410"></a><a name="zh-cn_topic_0000002512316477_image2670064316410"></a><a name="zh-cn_topic_0000002512316477_image2670064316410"></a><span><img class="" id="zh-cn_topic_0000002512316477_image2670064316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002483742364.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316477_p1757432116410"><a name="zh-cn_topic_0000002512316477_p1757432116410"></a><a name="zh-cn_topic_0000002512316477_p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316477_row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316477_p1432579516410"><a name="zh-cn_topic_0000002512316477_p1432579516410"></a><a name="zh-cn_topic_0000002512316477_p1432579516410"></a><a name="zh-cn_topic_0000002512316477_image4895582316410"></a><a name="zh-cn_topic_0000002512316477_image4895582316410"></a><span><img class="" id="zh-cn_topic_0000002512316477_image4895582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002483902338.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316477_p959197916410"><a name="zh-cn_topic_0000002512316477_p959197916410"></a><a name="zh-cn_topic_0000002512316477_p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316477_row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316477_p1232579516410"><a name="zh-cn_topic_0000002512316477_p1232579516410"></a><a name="zh-cn_topic_0000002512316477_p1232579516410"></a><a name="zh-cn_topic_0000002512316477_image1235582316410"></a><a name="zh-cn_topic_0000002512316477_image1235582316410"></a><span><img class="" id="zh-cn_topic_0000002512316477_image1235582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002516022315.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316477_p123197916410"><a name="zh-cn_topic_0000002512316477_p123197916410"></a><a name="zh-cn_topic_0000002512316477_p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316477_row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316477_p2204984716410"><a name="zh-cn_topic_0000002512316477_p2204984716410"></a><a name="zh-cn_topic_0000002512316477_p2204984716410"></a><a name="zh-cn_topic_0000002512316477_image4504446716410"></a><a name="zh-cn_topic_0000002512316477_image4504446716410"></a><span><img class="" id="zh-cn_topic_0000002512316477_image4504446716410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002516142315.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316477_p4388861916410"><a name="zh-cn_topic_0000002512316477_p4388861916410"></a><a name="zh-cn_topic_0000002512316477_p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="zh-cn_topic_0000002512316477_p1238861916410"><a name="zh-cn_topic_0000002512316477_p1238861916410"></a><a name="zh-cn_topic_0000002512316477_p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316477_row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316477_p5555360116410"><a name="zh-cn_topic_0000002512316477_p5555360116410"></a><a name="zh-cn_topic_0000002512316477_p5555360116410"></a><a name="zh-cn_topic_0000002512316477_image799324016410"></a><a name="zh-cn_topic_0000002512316477_image799324016410"></a><span><img class="" id="zh-cn_topic_0000002512316477_image799324016410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002483742366.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316477_p4612588116410"><a name="zh-cn_topic_0000002512316477_p4612588116410"></a><a name="zh-cn_topic_0000002512316477_p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="zh-cn_topic_0000002512316477_p1232588116410"><a name="zh-cn_topic_0000002512316477_p1232588116410"></a><a name="zh-cn_topic_0000002512316477_p1232588116410"></a>“说明”不是安全警示信息,不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# 测试准备<a name="ZH-CN_TOPIC_0000002516141505"></a>

-   **[测试仪器及辅助器件](#ZH-CN_TOPIC_0000002483741582)**  

-   **[测试硬件连接图](#ZH-CN_TOPIC_0000002483901582)**  

## 测试仪器及辅助器件<a name="ZH-CN_TOPIC_0000002483741582"></a>

测试仪器及辅助器材：

-   计算机（PC）1台。
-   无线综测仪1台（需支持非信令测试，建议同时支持信令测试）。
-   信号发生器1台（需支持最高频率12.75GHz）。
-   HiDiTing系列单板1块。
-   SMA\(M\)-SMA\(M\)射频线缆2条，N\(M\)- SMA\(M\)射频线缆2条（需至少支持2GHz～12.75GHz的频率）。
-   双工器SMA\(F\) 1个。
-   USB转Type-C线、USB（公对公）延长线和USB转console串口线各1条。
-   5V/2A USB电源适配器1个。
-   屏蔽房或屏蔽箱1个（屏蔽箱需支持对内部提供110V～240V /10A电源插排、SMA接头、USB接口各一个，隔离度≥80dB）以及其配套电源线1条。
-   网线2条。

## 测试硬件连接图<a name="ZH-CN_TOPIC_0000002483901582"></a>

射频测试硬件连接组网图如[图1](#zh-cn_topic_0000002512356519_fig182541853155019)所示，屏蔽箱内部硬件连线图如[图2](#zh-cn_topic_0000002512356519_fig181659412518)所示。

**图 1**  射频测试硬件连接组网图<a name="zh-cn_topic_0000002512356519_fig182541853155019"></a>  
![](figures/射频测试硬件连接组网图.png "射频测试硬件连接组网图")

**图 2**  屏蔽箱内硬件连接组网图<a name="zh-cn_topic_0000002512356519_fig181659412518"></a>  
![](figures/屏蔽箱内硬件连接组网图.png "屏蔽箱内硬件连接组网图")

硬件连接的具体操作如下：

1.  通过网线将PC机与无线综测仪和信号发生器进行连接。在PC机综测仪器和信号发生器的控制软件或控制页面上，输入无线综测仪和信号发生器的IP地址，将PC机与无线综测仪器和信号发生器连接，实现PC机对无线综测仪和信号发生器的控制。
2.  将屏蔽箱与其配套电源线连接，并将被测单板放至屏蔽箱内。在屏蔽箱中，采用5V/2A USB电源适配器和USB转Type-C线，将电源插排与被测单板电源端口连接，同时采用USB延长线缆，将屏蔽箱与PC机连接。在屏蔽箱中，采用USB转console串口线，USB端与屏蔽箱USB端口连接，console端与单板的命令下发串口连接。
3.  在无线综测仪的端口上采用N\(M\)-SMA\(M\)同轴线缆，信号发生器的端口采用SMA\(M\)-SMA\(M\)同轴线缆，将无线综测仪端口和信号发生器的端口连接到双工器同一端的端口上，并在另一端与屏蔽箱通过N\(M\)-SMA\(M\)同轴线缆连接。
4.  在屏蔽箱中，采用SMA\(M\)-SMA\(M\)同轴线缆，将屏蔽箱内侧对应的射频端口与被测单板的天线座子连接。
5.  完成以上操作后，测试环境搭建完毕。

>![](public_sys-resources/icon-note.gif) **说明：** 
>-   本文中所提到的射频线缆类型可能和用户实际使用的存在差异，具体以实际使用仪表要求为准。
>-   本文中提到的信号发生器，只有当抗干扰测试时才会用到，大部分测试只需要无线综测仪一台仪器就可完成。如果没有信号发生器，可以暂且将双工器去掉，使无线综测仪和屏蔽箱直接通过N\(M\)- SMA\(M\)射频线缆连接。

# 射频测试AT指令<a name="ZH-CN_TOPIC_0000002516141493"></a>

-   **[射频测试相关AT指令一览表](#ZH-CN_TOPIC_0000002516021525)**  

-   **[BT射频测试相关AT指令描述](#ZH-CN_TOPIC_0000002483741596)**  

-   **[BLE射频测试相关AT指令描述](#ZH-CN_TOPIC_0000002483901568)**  

-   **[SLE射频测试相关AT指令描述](#ZH-CN_TOPIC_0000002483741568)**  

-   **[BT进入信令模式指令](#ZH-CN_TOPIC_0000002516141467)**  

-   **[射频单音](#ZH-CN_TOPIC_0000002516021467)**  

-   **[XO 32M校准 执行CTRIM校准指令](#ZH-CN_TOPIC_0000002483741558)**  

-   **[XO 32M校准 自动校准指令](#ZH-CN_TOPIC_0000002483901508)**  

-   **[AT指令status状态对应表](#ZH-CN_TOPIC_0000002516021505)**  

## 射频测试相关AT指令一览表<a name="ZH-CN_TOPIC_0000002516021525"></a>

**表 1**  射频测试相关AT指令一览表

<a name="zh-cn_topic_0000002480356616_table924mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002480356616_row929mcpsimp"><th class="cellrowborder" valign="top" width="37.54%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002480356616_p931mcpsimp"><a name="zh-cn_topic_0000002480356616_p931mcpsimp"></a><a name="zh-cn_topic_0000002480356616_p931mcpsimp"></a>指令</p>
</th>
<th class="cellrowborder" valign="top" width="62.46000000000001%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002480356616_p933mcpsimp"><a name="zh-cn_topic_0000002480356616_p933mcpsimp"></a><a name="zh-cn_topic_0000002480356616_p933mcpsimp"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002480356616_row62124430577"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p11817185910398"><a name="zh-cn_topic_0000002480356616_p11817185910398"></a><a name="zh-cn_topic_0000002480356616_p11817185910398"></a>AT^BTENABLE</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p108172059123915"><a name="zh-cn_topic_0000002480356616_p108172059123915"></a><a name="zh-cn_topic_0000002480356616_p108172059123915"></a>BT使能指令（没有相应退出操作）。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row8198182553811"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p48177593393"><a name="zh-cn_topic_0000002480356616_p48177593393"></a><a name="zh-cn_topic_0000002480356616_p48177593393"></a>AT^BTFACCALLBACK</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p18817135914392"><a name="zh-cn_topic_0000002480356616_p18817135914392"></a><a name="zh-cn_topic_0000002480356616_p18817135914392"></a>注册BT命令上报回调（没有相应退出操作）。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row1764463389"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p12817659103914"><a name="zh-cn_topic_0000002480356616_p12817659103914"></a><a name="zh-cn_topic_0000002480356616_p12817659103914"></a>AT^BTTX</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p208175595393"><a name="zh-cn_topic_0000002480356616_p208175595393"></a><a name="zh-cn_topic_0000002480356616_p208175595393"></a>BT常发指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row153504492381"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p19817125914391"><a name="zh-cn_topic_0000002480356616_p19817125914391"></a><a name="zh-cn_topic_0000002480356616_p19817125914391"></a>AT^BTRX</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p1181711599391"><a name="zh-cn_topic_0000002480356616_p1181711599391"></a><a name="zh-cn_topic_0000002480356616_p1181711599391"></a>BT常收指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row101421152193813"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p1481795920398"><a name="zh-cn_topic_0000002480356616_p1481795920398"></a><a name="zh-cn_topic_0000002480356616_p1481795920398"></a>AT^BTTRXEND</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p19817155943910"><a name="zh-cn_topic_0000002480356616_p19817155943910"></a><a name="zh-cn_topic_0000002480356616_p19817155943910"></a>BT常发、常收停止指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row5861155623812"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p2081745963919"><a name="zh-cn_topic_0000002480356616_p2081745963919"></a><a name="zh-cn_topic_0000002480356616_p2081745963919"></a>AT^BTRST</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p8817259163913"><a name="zh-cn_topic_0000002480356616_p8817259163913"></a><a name="zh-cn_topic_0000002480356616_p8817259163913"></a>BT软件复位指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row88931655133815"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p1462791573911"><a name="zh-cn_topic_0000002480356616_p1462791573911"></a><a name="zh-cn_topic_0000002480356616_p1462791573911"></a>AT^BLEENABLE</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p1362715154396"><a name="zh-cn_topic_0000002480356616_p1362715154396"></a><a name="zh-cn_topic_0000002480356616_p1362715154396"></a>BLE使能指令（没有相应退出操作）。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row1695745417388"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p1162861517398"><a name="zh-cn_topic_0000002480356616_p1162861517398"></a><a name="zh-cn_topic_0000002480356616_p1162861517398"></a>AT^BLEFACCALLBACK</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p562821533917"><a name="zh-cn_topic_0000002480356616_p562821533917"></a><a name="zh-cn_topic_0000002480356616_p562821533917"></a>注册BLE命令上报回调（没有相应退出操作）。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row1478135373817"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p4628181553912"><a name="zh-cn_topic_0000002480356616_p4628181553912"></a><a name="zh-cn_topic_0000002480356616_p4628181553912"></a>AT^BLETX</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p26284150392"><a name="zh-cn_topic_0000002480356616_p26284150392"></a><a name="zh-cn_topic_0000002480356616_p26284150392"></a>BLE常发指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row11902175012383"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p14628191583913"><a name="zh-cn_topic_0000002480356616_p14628191583913"></a><a name="zh-cn_topic_0000002480356616_p14628191583913"></a>AT^BLERX</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p662891513910"><a name="zh-cn_topic_0000002480356616_p662891513910"></a><a name="zh-cn_topic_0000002480356616_p662891513910"></a>BLE常收指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row5317131593814"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p2628111520396"><a name="zh-cn_topic_0000002480356616_p2628111520396"></a><a name="zh-cn_topic_0000002480356616_p2628111520396"></a>AT^BLETRXEND</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p1562891563911"><a name="zh-cn_topic_0000002480356616_p1562891563911"></a><a name="zh-cn_topic_0000002480356616_p1562891563911"></a>BLE常发、常收停止指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row1645464353817"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p1162811152398"><a name="zh-cn_topic_0000002480356616_p1162811152398"></a><a name="zh-cn_topic_0000002480356616_p1162811152398"></a>AT^BLERST</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p11628121553920"><a name="zh-cn_topic_0000002480356616_p11628121553920"></a><a name="zh-cn_topic_0000002480356616_p11628121553920"></a>BLE软件复位指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row106370376445"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p648144317444"><a name="zh-cn_topic_0000002480356616_p648144317444"></a><a name="zh-cn_topic_0000002480356616_p648144317444"></a>AT^SLEENABLE</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p4637153717449"><a name="zh-cn_topic_0000002480356616_p4637153717449"></a><a name="zh-cn_topic_0000002480356616_p4637153717449"></a>SLE使能指令（没有相应退出操作）。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row148978381669"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p38982431364"><a name="zh-cn_topic_0000002480356616_p38982431364"></a><a name="zh-cn_topic_0000002480356616_p38982431364"></a>AT^SLEFACCALLBACK</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p163352047560"><a name="zh-cn_topic_0000002480356616_p163352047560"></a><a name="zh-cn_topic_0000002480356616_p163352047560"></a>注册SLE命令上报回调（没有相应退出操作）。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row958mcpsimp"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p960mcpsimp"><a name="zh-cn_topic_0000002480356616_p960mcpsimp"></a><a name="zh-cn_topic_0000002480356616_p960mcpsimp"></a>AT^SLETX</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p963mcpsimp"><a name="zh-cn_topic_0000002480356616_p963mcpsimp"></a><a name="zh-cn_topic_0000002480356616_p963mcpsimp"></a>SLE常发指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row964mcpsimp"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p966mcpsimp"><a name="zh-cn_topic_0000002480356616_p966mcpsimp"></a><a name="zh-cn_topic_0000002480356616_p966mcpsimp"></a>AT^SLERX</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p969mcpsimp"><a name="zh-cn_topic_0000002480356616_p969mcpsimp"></a><a name="zh-cn_topic_0000002480356616_p969mcpsimp"></a>SLE常收指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row970mcpsimp"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p972mcpsimp"><a name="zh-cn_topic_0000002480356616_p972mcpsimp"></a><a name="zh-cn_topic_0000002480356616_p972mcpsimp"></a>AT^SLETRXEND</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p975mcpsimp"><a name="zh-cn_topic_0000002480356616_p975mcpsimp"></a><a name="zh-cn_topic_0000002480356616_p975mcpsimp"></a>SLE常发、常收停止指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row976mcpsimp"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p978mcpsimp"><a name="zh-cn_topic_0000002480356616_p978mcpsimp"></a><a name="zh-cn_topic_0000002480356616_p978mcpsimp"></a>AT^SLERST</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p981mcpsimp"><a name="zh-cn_topic_0000002480356616_p981mcpsimp"></a><a name="zh-cn_topic_0000002480356616_p981mcpsimp"></a>SLE软件复位指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row550171118411"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p25019118410"><a name="zh-cn_topic_0000002480356616_p25019118410"></a><a name="zh-cn_topic_0000002480356616_p25019118410"></a>AT^BTSIGNAL</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p7501111184113"><a name="zh-cn_topic_0000002480356616_p7501111184113"></a><a name="zh-cn_topic_0000002480356616_p7501111184113"></a>BT进入信令模式指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row988mcpsimp"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p990mcpsimp"><a name="zh-cn_topic_0000002480356616_p990mcpsimp"></a><a name="zh-cn_topic_0000002480356616_p990mcpsimp"></a>AT^BTTXLO（暂未支持）</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p993mcpsimp"><a name="zh-cn_topic_0000002480356616_p993mcpsimp"></a><a name="zh-cn_topic_0000002480356616_p993mcpsimp"></a>射频单音指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row1000mcpsimp"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p1002mcpsimp"><a name="zh-cn_topic_0000002480356616_p1002mcpsimp"></a><a name="zh-cn_topic_0000002480356616_p1002mcpsimp"></a>AT^XOCALI（暂未支持）</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p1006mcpsimp"><a name="zh-cn_topic_0000002480356616_p1006mcpsimp"></a><a name="zh-cn_topic_0000002480356616_p1006mcpsimp"></a>晶体频偏校准指令。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356616_row9588191717108"><td class="cellrowborder" valign="top" width="37.54%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002480356616_p3679192811112"><a name="zh-cn_topic_0000002480356616_p3679192811112"></a><a name="zh-cn_topic_0000002480356616_p3679192811112"></a>AT^BTAUTOCALI（暂未支持）</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002480356616_p2588111711102"><a name="zh-cn_topic_0000002480356616_p2588111711102"></a><a name="zh-cn_topic_0000002480356616_p2588111711102"></a>晶体自动频偏校准指令。</p>
</td>
</tr>
</tbody>
</table>

## BT射频测试相关AT指令描述<a name="ZH-CN_TOPIC_0000002483741596"></a>

-   **[BT测试建议流程](#ZH-CN_TOPIC_0000002516021513)**  

-   **[BT使能指令](#ZH-CN_TOPIC_0000002516021547)**  

-   **[BT注册回调](#ZH-CN_TOPIC_0000002516141545)**  

-   **[常发指令](#ZH-CN_TOPIC_0000002483901494)**  

-   **[常收指令](#ZH-CN_TOPIC_0000002516141525)**  

-   **[常发常收停止指令](#ZH-CN_TOPIC_0000002483901518)**  

-   **[BT复位指令](#ZH-CN_TOPIC_0000002516021533)**  

### BT测试建议流程<a name="ZH-CN_TOPIC_0000002516021513"></a>

**图 1**  BT产测AT测试流程<a name="zh-cn_topic_0000002480356598_fig3905013716"></a>  
![](figures/BT产测AT测试流程.png "BT产测AT测试流程")

>![](public_sys-resources/icon-note.gif) **说明：** 
>“AT^BTENABLE”与“AT^BTFACCALLBACK”在上电之后只需要执行一次，若想重复执行该指令，需要复位系统。常用的复位方式为重新上下电。

### BT使能指令<a name="ZH-CN_TOPIC_0000002516021547"></a>

<a name="zh-cn_topic_0000002480196634_table170mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002480196634_row175mcpsimp"><th class="firstcol" valign="top" width="18.43%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002480196634_p177mcpsimp"><a name="zh-cn_topic_0000002480196634_p177mcpsimp"></a><a name="zh-cn_topic_0000002480196634_p177mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="81.57%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002480196634_p9326226125914"><a name="zh-cn_topic_0000002480196634_p9326226125914"></a><a name="zh-cn_topic_0000002480196634_p9326226125914"></a>AT^BTENABLE</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196634_row181mcpsimp"><th class="firstcol" valign="top" width="18.43%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002480196634_p183mcpsimp"><a name="zh-cn_topic_0000002480196634_p183mcpsimp"></a><a name="zh-cn_topic_0000002480196634_p183mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="81.57%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002480196634_ul1315815292311"></a><a name="zh-cn_topic_0000002480196634_ul1315815292311"></a><ul id="zh-cn_topic_0000002480196634_ul1315815292311"><li>成功：OK</li><li>失败：ERROR</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196634_row188mcpsimp"><th class="firstcol" valign="top" width="18.43%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002480196634_p190mcpsimp"><a name="zh-cn_topic_0000002480196634_p190mcpsimp"></a><a name="zh-cn_topic_0000002480196634_p190mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="81.57%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000002480196634_p532632611596"><a name="zh-cn_topic_0000002480196634_p532632611596"></a><a name="zh-cn_topic_0000002480196634_p532632611596"></a>AT^BTENABLE</p>
<p id="zh-cn_topic_0000002480196634_p108631214219"><a name="zh-cn_topic_0000002480196634_p108631214219"></a><a name="zh-cn_topic_0000002480196634_p108631214219"></a>回复：</p>
<p id="zh-cn_topic_0000002480196634_p13536525123"><a name="zh-cn_topic_0000002480196634_p13536525123"></a><a name="zh-cn_topic_0000002480196634_p13536525123"></a>OK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196634_row194mcpsimp"><th class="firstcol" valign="top" width="18.43%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002480196634_p196mcpsimp"><a name="zh-cn_topic_0000002480196634_p196mcpsimp"></a><a name="zh-cn_topic_0000002480196634_p196mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="81.57%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002480196634_p932618269598"><a name="zh-cn_topic_0000002480196634_p932618269598"></a><a name="zh-cn_topic_0000002480196634_p932618269598"></a>在进行BT所有指令前，必须先执行BT使能指令，使能BT相关业务接口。</p>
<p id="zh-cn_topic_0000002480196634_p764mcpsimp"><a name="zh-cn_topic_0000002480196634_p764mcpsimp"></a><a name="zh-cn_topic_0000002480196634_p764mcpsimp"></a>本使能指令是测试BT指令的前置条件之一，没有相应退出操作。</p>
<p id="zh-cn_topic_0000002480196634_p1344886123718"><a name="zh-cn_topic_0000002480196634_p1344886123718"></a><a name="zh-cn_topic_0000002480196634_p1344886123718"></a>上电后发一次即可。</p>
</td>
</tr>
</tbody>
</table>

### BT注册回调<a name="ZH-CN_TOPIC_0000002516141545"></a>

<a name="zh-cn_topic_0000002512356517_table170mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002512356517_row175mcpsimp"><th class="firstcol" valign="top" width="18.41%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002512356517_p177mcpsimp"><a name="zh-cn_topic_0000002512356517_p177mcpsimp"></a><a name="zh-cn_topic_0000002512356517_p177mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="81.58999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512356517_p9326226125914"><a name="zh-cn_topic_0000002512356517_p9326226125914"></a><a name="zh-cn_topic_0000002512356517_p9326226125914"></a>AT^BTFACCALLBACK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356517_row181mcpsimp"><th class="firstcol" valign="top" width="18.41%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002512356517_p183mcpsimp"><a name="zh-cn_topic_0000002512356517_p183mcpsimp"></a><a name="zh-cn_topic_0000002512356517_p183mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="81.58999999999999%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002512356517_ul1315815292311"></a><a name="zh-cn_topic_0000002512356517_ul1315815292311"></a><ul id="zh-cn_topic_0000002512356517_ul1315815292311"><li>成功：OK</li><li>失败：ERROR</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356517_row188mcpsimp"><th class="firstcol" valign="top" width="18.41%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002512356517_p190mcpsimp"><a name="zh-cn_topic_0000002512356517_p190mcpsimp"></a><a name="zh-cn_topic_0000002512356517_p190mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="81.58999999999999%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000002512356517_p532632611596"><a name="zh-cn_topic_0000002512356517_p532632611596"></a><a name="zh-cn_topic_0000002512356517_p532632611596"></a>AT^BTFACCALLBACK</p>
<p id="zh-cn_topic_0000002512356517_p108631214219"><a name="zh-cn_topic_0000002512356517_p108631214219"></a><a name="zh-cn_topic_0000002512356517_p108631214219"></a>回复：</p>
<p id="zh-cn_topic_0000002512356517_p13536525123"><a name="zh-cn_topic_0000002512356517_p13536525123"></a><a name="zh-cn_topic_0000002512356517_p13536525123"></a>OK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356517_row194mcpsimp"><th class="firstcol" valign="top" width="18.41%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002512356517_p196mcpsimp"><a name="zh-cn_topic_0000002512356517_p196mcpsimp"></a><a name="zh-cn_topic_0000002512356517_p196mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="81.58999999999999%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002512356517_p48051432152818"><a name="zh-cn_topic_0000002512356517_p48051432152818"></a><a name="zh-cn_topic_0000002512356517_p48051432152818"></a>BT测试前先注册上报回调，否则业务指令没有上报状态打印。</p>
<p id="zh-cn_topic_0000002512356517_p16760847162819"><a name="zh-cn_topic_0000002512356517_p16760847162819"></a><a name="zh-cn_topic_0000002512356517_p16760847162819"></a>本指令是测试BT指令的前置条件，没有相应退出操作。</p>
<p id="zh-cn_topic_0000002512356517_p18100141912813"><a name="zh-cn_topic_0000002512356517_p18100141912813"></a><a name="zh-cn_topic_0000002512356517_p18100141912813"></a>上电后发一次即可。</p>
</td>
</tr>
</tbody>
</table>

### 常发指令<a name="ZH-CN_TOPIC_0000002483901494"></a>

<a name="zh-cn_topic_0000002480356600_table836mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002480356600_row841mcpsimp"><th class="firstcol" valign="top" width="17.96%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002480356600_p843mcpsimp"><a name="zh-cn_topic_0000002480356600_p843mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p843mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="82.04%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002480356600_p845mcpsimp"><a name="zh-cn_topic_0000002480356600_p845mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p845mcpsimp"></a>AT^BTTX=&lt;payload_len&gt;,&lt;addr&gt;,&lt;freq&gt;,&lt;module_type&gt;,&lt;code_type&gt;,&lt;payload_type&gt;,&lt;pwr&gt;,&lt;chnnl&gt;</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356600_row847mcpsimp"><th class="firstcol" valign="top" width="17.96%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002480356600_p849mcpsimp"><a name="zh-cn_topic_0000002480356600_p849mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p849mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="82.04%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002480356600_ul557114134252"></a><a name="zh-cn_topic_0000002480356600_ul557114134252"></a><ul id="zh-cn_topic_0000002480356600_ul557114134252"><li>成功：OK<p id="zh-cn_topic_0000002480356600_p380481862514"><a name="zh-cn_topic_0000002480356600_p380481862514"></a><a name="zh-cn_topic_0000002480356600_p380481862514"></a>status：0</p>
<p id="zh-cn_topic_0000002480356600_p621816151389"><a name="zh-cn_topic_0000002480356600_p621816151389"></a><a name="zh-cn_topic_0000002480356600_p621816151389"></a>注意：status为非0值表示该指令执行出错。</p>
</li><li>失败：ERROR</li></ul>
<p id="zh-cn_topic_0000002480356600_p478792610912"><a name="zh-cn_topic_0000002480356600_p478792610912"></a><a name="zh-cn_topic_0000002480356600_p478792610912"></a>ERROR原因：指令不支持或格式错误。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356600_row854mcpsimp"><th class="firstcol" valign="top" width="17.96%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002480356600_p856mcpsimp"><a name="zh-cn_topic_0000002480356600_p856mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p856mcpsimp"></a>参数说明</p>
</th>
<td class="cellrowborder" valign="top" width="82.04%" headers="mcps1.1.3.3.1 "><a name="zh-cn_topic_0000002480356600_ul5234101317310"></a><a name="zh-cn_topic_0000002480356600_ul5234101317310"></a><ul id="zh-cn_topic_0000002480356600_ul5234101317310"><li>&lt;payload_len&gt;：包长度<p id="zh-cn_topic_0000002480356600_p871mcpsimp"><a name="zh-cn_topic_0000002480356600_p871mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p871mcpsimp"></a>根据标准蓝牙协议，包长最大值范围是27Byte～1021Byte，不同包类型下包长最大值不同。</p>
</li><li>&lt;addr&gt;：对端设备地址</li><li>&lt;freq&gt;：发送频点<p id="zh-cn_topic_0000002480356600_p14713165513359"><a name="zh-cn_topic_0000002480356600_p14713165513359"></a><a name="zh-cn_topic_0000002480356600_p14713165513359"></a>0～78：射频信道号依次对应对应2402MHz～2480MHz。</p>
<p id="zh-cn_topic_0000002480356600_p1070445035014"><a name="zh-cn_topic_0000002480356600_p1070445035014"></a><a name="zh-cn_topic_0000002480356600_p1070445035014"></a>254：顺序频点发送，在2402MHz～2480MHz逐个遍历发送。</p>
<p id="zh-cn_topic_0000002480356600_p10704155013509"><a name="zh-cn_topic_0000002480356600_p10704155013509"></a><a name="zh-cn_topic_0000002480356600_p10704155013509"></a>255：随机频点发送，在2402MHz～2480MHz随机跳频发送。</p>
</li><li>&lt;module_type&gt;: 调制类型<p id="zh-cn_topic_0000002480356600_p263442720517"><a name="zh-cn_topic_0000002480356600_p263442720517"></a><a name="zh-cn_topic_0000002480356600_p263442720517"></a>0：1M BR GFSK</p>
<p id="zh-cn_topic_0000002480356600_p03823352710"><a name="zh-cn_topic_0000002480356600_p03823352710"></a><a name="zh-cn_topic_0000002480356600_p03823352710"></a>3：1M EDR QPSK</p>
<p id="zh-cn_topic_0000002480356600_p763402711517"><a name="zh-cn_topic_0000002480356600_p763402711517"></a><a name="zh-cn_topic_0000002480356600_p763402711517"></a>6：1M EDR 8PSK</p>
</li><li>&lt;code_type&gt;：包类型（同样的码字在BR和EDR下含义不同，前者为BR下包类型，后者为EDR下包类型）<p id="zh-cn_topic_0000002480356600_p884mcpsimp"><a name="zh-cn_topic_0000002480356600_p884mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p884mcpsimp"></a>3：DM1</p>
<p id="zh-cn_topic_0000002480356600_p885mcpsimp"><a name="zh-cn_topic_0000002480356600_p885mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p885mcpsimp"></a>4：DH1/2DH1</p>
<p id="zh-cn_topic_0000002480356600_p886mcpsimp"><a name="zh-cn_topic_0000002480356600_p886mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p886mcpsimp"></a>8：3DH1</p>
<p id="zh-cn_topic_0000002480356600_p322412557375"><a name="zh-cn_topic_0000002480356600_p322412557375"></a><a name="zh-cn_topic_0000002480356600_p322412557375"></a>10：2DH3</p>
<p id="zh-cn_topic_0000002480356600_p1256217817381"><a name="zh-cn_topic_0000002480356600_p1256217817381"></a><a name="zh-cn_topic_0000002480356600_p1256217817381"></a>11：DH3/3DH3</p>
<p id="zh-cn_topic_0000002480356600_p918061011385"><a name="zh-cn_topic_0000002480356600_p918061011385"></a><a name="zh-cn_topic_0000002480356600_p918061011385"></a>14：2DH5</p>
<p id="zh-cn_topic_0000002480356600_p20183161293818"><a name="zh-cn_topic_0000002480356600_p20183161293818"></a><a name="zh-cn_topic_0000002480356600_p20183161293818"></a>15：DH5/3DH5</p>
</li><li>&lt;payload_type&gt;：发包模式<p id="zh-cn_topic_0000002480356600_p874mcpsimp"><a name="zh-cn_topic_0000002480356600_p874mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p874mcpsimp"></a>0：PRBS9</p>
<p id="zh-cn_topic_0000002480356600_p875mcpsimp"><a name="zh-cn_topic_0000002480356600_p875mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p875mcpsimp"></a>1：11110000</p>
<p id="zh-cn_topic_0000002480356600_p876mcpsimp"><a name="zh-cn_topic_0000002480356600_p876mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p876mcpsimp"></a>2：10101010</p>
<p id="zh-cn_topic_0000002480356600_p194873283469"><a name="zh-cn_topic_0000002480356600_p194873283469"></a><a name="zh-cn_topic_0000002480356600_p194873283469"></a>3：PRBS15</p>
<p id="zh-cn_topic_0000002480356600_p143991931174619"><a name="zh-cn_topic_0000002480356600_p143991931174619"></a><a name="zh-cn_topic_0000002480356600_p143991931174619"></a>4：11111111</p>
<p id="zh-cn_topic_0000002480356600_p1145903384617"><a name="zh-cn_topic_0000002480356600_p1145903384617"></a><a name="zh-cn_topic_0000002480356600_p1145903384617"></a>5：00000000</p>
<p id="zh-cn_topic_0000002480356600_p1776753514616"><a name="zh-cn_topic_0000002480356600_p1776753514616"></a><a name="zh-cn_topic_0000002480356600_p1776753514616"></a>6：00001111</p>
<p id="zh-cn_topic_0000002480356600_p18968422174611"><a name="zh-cn_topic_0000002480356600_p18968422174611"></a><a name="zh-cn_topic_0000002480356600_p18968422174611"></a>7：01010101</p>
</li><li>&lt;pwr&gt;: 发送功率（最高档位5档，最低0档，每档依次降低4dB）<a name="zh-cn_topic_0000002480356600_ul11870527173711"></a><a name="zh-cn_topic_0000002480356600_ul11870527173711"></a><ul id="zh-cn_topic_0000002480356600_ul11870527173711"><li>GFSK：<p id="zh-cn_topic_0000002480356600_p868mcpsimp"><a name="zh-cn_topic_0000002480356600_p868mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p868mcpsimp"></a>5档：14dBm</p>
</li><li>BPSK/QPSK/8PSK：<p id="zh-cn_topic_0000002480356600_p67141135401"><a name="zh-cn_topic_0000002480356600_p67141135401"></a><a name="zh-cn_topic_0000002480356600_p67141135401"></a>5档：10dBm</p>
</li></ul>
</li><li>&lt;chnnl&gt;：通道选择（本芯片只有通道1）<p id="zh-cn_topic_0000002480356600_p462244675217"><a name="zh-cn_topic_0000002480356600_p462244675217"></a><a name="zh-cn_topic_0000002480356600_p462244675217"></a>1：通道1</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356600_row912mcpsimp"><th class="firstcol" valign="top" width="17.96%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002480356600_p914mcpsimp"><a name="zh-cn_topic_0000002480356600_p914mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p914mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="82.04%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002480356600_p523914118374"><a name="zh-cn_topic_0000002480356600_p523914118374"></a><a name="zh-cn_topic_0000002480356600_p523914118374"></a>如配置定频信道0：AT^BTTX=12,1122332211,0,0,4,0,5,1</p>
<p id="zh-cn_topic_0000002480356600_p10533101020518"><a name="zh-cn_topic_0000002480356600_p10533101020518"></a><a name="zh-cn_topic_0000002480356600_p10533101020518"></a>上述命令修改为顺序频点发送：AT^BTTX=12,1122332211,254,0,4,0,5,1</p>
<p id="zh-cn_topic_0000002480356600_p135014615525"><a name="zh-cn_topic_0000002480356600_p135014615525"></a><a name="zh-cn_topic_0000002480356600_p135014615525"></a>上述命令修改为随机跳频发送：AT^BTTX=12,1122332211,255,0,4,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356600_row918mcpsimp"><th class="firstcol" valign="top" width="17.96%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000002480356600_p920mcpsimp"><a name="zh-cn_topic_0000002480356600_p920mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p920mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="82.04%" headers="mcps1.1.3.5.1 "><p id="zh-cn_topic_0000002480356600_p922mcpsimp"><a name="zh-cn_topic_0000002480356600_p922mcpsimp"></a><a name="zh-cn_topic_0000002480356600_p922mcpsimp"></a>地址参数为十六进制字符串，其余参数均为十进制，中间“,”不可省略。</p>
<p id="zh-cn_topic_0000002480356600_p133801525318"><a name="zh-cn_topic_0000002480356600_p133801525318"></a><a name="zh-cn_topic_0000002480356600_p133801525318"></a>参数需要根据蓝牙协议规定进行配置，否则会出现错误。</p>
</td>
</tr>
</tbody>
</table>

### 常收指令<a name="ZH-CN_TOPIC_0000002516141525"></a>

<a name="zh-cn_topic_0000002512356511_table608mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002512356511_row613mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002512356511_p615mcpsimp"><a name="zh-cn_topic_0000002512356511_p615mcpsimp"></a><a name="zh-cn_topic_0000002512356511_p615mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512356511_p617mcpsimp"><a name="zh-cn_topic_0000002512356511_p617mcpsimp"></a><a name="zh-cn_topic_0000002512356511_p617mcpsimp"></a>AT^BTRX=&lt;payload_len&gt;,&lt;addr&gt;,&lt;freq&gt;,&lt;module_type&gt;,&lt;code_type&gt;,&lt;chnnl&gt;</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356511_row619mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002512356511_p621mcpsimp"><a name="zh-cn_topic_0000002512356511_p621mcpsimp"></a><a name="zh-cn_topic_0000002512356511_p621mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002512356511_ul169551831172513"></a><a name="zh-cn_topic_0000002512356511_ul169551831172513"></a><ul id="zh-cn_topic_0000002512356511_ul169551831172513"><li>成功：OK<p id="zh-cn_topic_0000002512356511_p10384437142516"><a name="zh-cn_topic_0000002512356511_p10384437142516"></a><a name="zh-cn_topic_0000002512356511_p10384437142516"></a>status：0</p>
<p id="zh-cn_topic_0000002512356511_p621816151389"><a name="zh-cn_topic_0000002512356511_p621816151389"></a><a name="zh-cn_topic_0000002512356511_p621816151389"></a>注意：status为非0值表示该指令执行出错。</p>
</li><li>失败：ERROR</li></ul>
<p id="zh-cn_topic_0000002512356511_p589604419152"><a name="zh-cn_topic_0000002512356511_p589604419152"></a><a name="zh-cn_topic_0000002512356511_p589604419152"></a>ERROR原因：指令不支持或格式错误。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356511_row626mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002512356511_p628mcpsimp"><a name="zh-cn_topic_0000002512356511_p628mcpsimp"></a><a name="zh-cn_topic_0000002512356511_p628mcpsimp"></a>参数说明</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.3.1 "><a name="zh-cn_topic_0000002512356511_ul5234101317310"></a><a name="zh-cn_topic_0000002512356511_ul5234101317310"></a><ul id="zh-cn_topic_0000002512356511_ul5234101317310"><li>&lt;payload_len&gt;：包长度<p id="zh-cn_topic_0000002512356511_p871mcpsimp"><a name="zh-cn_topic_0000002512356511_p871mcpsimp"></a><a name="zh-cn_topic_0000002512356511_p871mcpsimp"></a>根据标准蓝牙协议，包长最大值范围是27Byte～1021Byte，不同包类型下包长最大值不同。</p>
</li><li>&lt;addr&gt;：对端设备地址</li><li>&lt;freq&gt;：发送频点<p id="zh-cn_topic_0000002512356511_p14713165513359"><a name="zh-cn_topic_0000002512356511_p14713165513359"></a><a name="zh-cn_topic_0000002512356511_p14713165513359"></a>0～78：射频信道号依次对应对应2402MHz～2480MHz。</p>
</li><li>&lt;module_type&gt;: 调制类型<p id="zh-cn_topic_0000002512356511_p263442720517"><a name="zh-cn_topic_0000002512356511_p263442720517"></a><a name="zh-cn_topic_0000002512356511_p263442720517"></a>0：1M BR GFSK</p>
<p id="zh-cn_topic_0000002512356511_p03823352710"><a name="zh-cn_topic_0000002512356511_p03823352710"></a><a name="zh-cn_topic_0000002512356511_p03823352710"></a>3：1M EDR QPSK</p>
<p id="zh-cn_topic_0000002512356511_p763402711517"><a name="zh-cn_topic_0000002512356511_p763402711517"></a><a name="zh-cn_topic_0000002512356511_p763402711517"></a>6：1M EDR 8PSK</p>
</li><li>&lt;code_type&gt;：包类型（同样的码字在BR和EDR下含义不同，前者为BR下包类型，后者为EDR下包类型）<p id="zh-cn_topic_0000002512356511_p884mcpsimp"><a name="zh-cn_topic_0000002512356511_p884mcpsimp"></a><a name="zh-cn_topic_0000002512356511_p884mcpsimp"></a>3：DM1</p>
<p id="zh-cn_topic_0000002512356511_p885mcpsimp"><a name="zh-cn_topic_0000002512356511_p885mcpsimp"></a><a name="zh-cn_topic_0000002512356511_p885mcpsimp"></a>4：DH1/2DH1</p>
<p id="zh-cn_topic_0000002512356511_p886mcpsimp"><a name="zh-cn_topic_0000002512356511_p886mcpsimp"></a><a name="zh-cn_topic_0000002512356511_p886mcpsimp"></a>8：3DH1</p>
<p id="zh-cn_topic_0000002512356511_p322412557375"><a name="zh-cn_topic_0000002512356511_p322412557375"></a><a name="zh-cn_topic_0000002512356511_p322412557375"></a>10：2DH3</p>
<p id="zh-cn_topic_0000002512356511_p1256217817381"><a name="zh-cn_topic_0000002512356511_p1256217817381"></a><a name="zh-cn_topic_0000002512356511_p1256217817381"></a>11：DH3/3DH3</p>
<p id="zh-cn_topic_0000002512356511_p918061011385"><a name="zh-cn_topic_0000002512356511_p918061011385"></a><a name="zh-cn_topic_0000002512356511_p918061011385"></a>14：2DH5</p>
<p id="zh-cn_topic_0000002512356511_p20183161293818"><a name="zh-cn_topic_0000002512356511_p20183161293818"></a><a name="zh-cn_topic_0000002512356511_p20183161293818"></a>15：DH5/3DH5</p>
</li><li>&lt;chnnl&gt;：通道选择<p id="zh-cn_topic_0000002512356511_p462244675217"><a name="zh-cn_topic_0000002512356511_p462244675217"></a><a name="zh-cn_topic_0000002512356511_p462244675217"></a>1：通道1</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356511_row652mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002512356511_p654mcpsimp"><a name="zh-cn_topic_0000002512356511_p654mcpsimp"></a><a name="zh-cn_topic_0000002512356511_p654mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002512356511_p656mcpsimp"><a name="zh-cn_topic_0000002512356511_p656mcpsimp"></a><a name="zh-cn_topic_0000002512356511_p656mcpsimp"></a>AT^BTRX=12,1122332211,0,0,4,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356511_row658mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000002512356511_p660mcpsimp"><a name="zh-cn_topic_0000002512356511_p660mcpsimp"></a><a name="zh-cn_topic_0000002512356511_p660mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.5.1 "><p id="zh-cn_topic_0000002512356511_p922mcpsimp"><a name="zh-cn_topic_0000002512356511_p922mcpsimp"></a><a name="zh-cn_topic_0000002512356511_p922mcpsimp"></a>地址参数为十六进制字符串，其余参数均为十进制，中间“,”不可省略。</p>
<p id="zh-cn_topic_0000002512356511_p133801525318"><a name="zh-cn_topic_0000002512356511_p133801525318"></a><a name="zh-cn_topic_0000002512356511_p133801525318"></a>参数需要根据蓝牙协议规定进行配置，否则会出现错误。</p>
</td>
</tr>
</tbody>
</table>

### 常发常收停止指令<a name="ZH-CN_TOPIC_0000002483901518"></a>

<a name="zh-cn_topic_0000002512316493_table796mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002512316493_row801mcpsimp"><th class="firstcol" valign="top" width="18.029999999999998%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002512316493_p803mcpsimp"><a name="zh-cn_topic_0000002512316493_p803mcpsimp"></a><a name="zh-cn_topic_0000002512316493_p803mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="81.97%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316493_p805mcpsimp"><a name="zh-cn_topic_0000002512316493_p805mcpsimp"></a><a name="zh-cn_topic_0000002512316493_p805mcpsimp"></a>AT^BTTRXEND</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316493_row807mcpsimp"><th class="firstcol" valign="top" width="18.029999999999998%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002512316493_p809mcpsimp"><a name="zh-cn_topic_0000002512316493_p809mcpsimp"></a><a name="zh-cn_topic_0000002512316493_p809mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="81.97%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002512316493_ul243013532612"></a><a name="zh-cn_topic_0000002512316493_ul243013532612"></a><ul id="zh-cn_topic_0000002512316493_ul243013532612"><li>成功：OK<p id="zh-cn_topic_0000002512316493_p64931110102619"><a name="zh-cn_topic_0000002512316493_p64931110102619"></a><a name="zh-cn_topic_0000002512316493_p64931110102619"></a>status：0,num_packet:3e8,rssi:-127</p>
<p id="zh-cn_topic_0000002512316493_p621816151389"><a name="zh-cn_topic_0000002512316493_p621816151389"></a><a name="zh-cn_topic_0000002512316493_p621816151389"></a>注意：status为非0值表示该指令执行出错。</p>
</li><li>失败：ERROR</li></ul>
<p id="zh-cn_topic_0000002512316493_p1821619014162"><a name="zh-cn_topic_0000002512316493_p1821619014162"></a><a name="zh-cn_topic_0000002512316493_p1821619014162"></a>ERROR原因：格式错误。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316493_row824mcpsimp"><th class="firstcol" valign="top" width="18.029999999999998%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002512316493_p826mcpsimp"><a name="zh-cn_topic_0000002512316493_p826mcpsimp"></a><a name="zh-cn_topic_0000002512316493_p826mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="81.97%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000002512316493_p941392314720"><a name="zh-cn_topic_0000002512316493_p941392314720"></a><a name="zh-cn_topic_0000002512316493_p941392314720"></a>AT^BTTRXEND</p>
<p id="zh-cn_topic_0000002512316493_p541311235715"><a name="zh-cn_topic_0000002512316493_p541311235715"></a><a name="zh-cn_topic_0000002512316493_p541311235715"></a>回复：</p>
<p id="zh-cn_topic_0000002512316493_p941317231572"><a name="zh-cn_topic_0000002512316493_p941317231572"></a><a name="zh-cn_topic_0000002512316493_p941317231572"></a>OK</p>
<p id="zh-cn_topic_0000002512316493_p1928617193613"><a name="zh-cn_topic_0000002512316493_p1928617193613"></a><a name="zh-cn_topic_0000002512316493_p1928617193613"></a>status:0x0,num_packet:0x3e8,rssi:-127</p>
<p id="zh-cn_topic_0000002512316493_p641319231272"><a name="zh-cn_topic_0000002512316493_p641319231272"></a><a name="zh-cn_topic_0000002512316493_p641319231272"></a>说明：如仪表发1000包后发结束指令，测试RX的收报数为1000包（0x3E8），测试RX的接收能量rssi为-127dBm。</p>
<p id="zh-cn_topic_0000002512316493_p1524191765310"><a name="zh-cn_topic_0000002512316493_p1524191765310"></a><a name="zh-cn_topic_0000002512316493_p1524191765310"></a>如果发TX后执行结束指令，则num_packet指代发射包数，rssi指代发射档位。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316493_row830mcpsimp"><th class="firstcol" valign="top" width="18.029999999999998%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002512316493_p832mcpsimp"><a name="zh-cn_topic_0000002512316493_p832mcpsimp"></a><a name="zh-cn_topic_0000002512316493_p832mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="81.97%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002512316493_p1595516720517"><a name="zh-cn_topic_0000002512316493_p1595516720517"></a><a name="zh-cn_topic_0000002512316493_p1595516720517"></a>此指令在执行BT RF TX、RX后需要停止时执行。</p>
<p id="zh-cn_topic_0000002512316493_p4981119519"><a name="zh-cn_topic_0000002512316493_p4981119519"></a><a name="zh-cn_topic_0000002512316493_p4981119519"></a>rssi和num_packet在RX结束后关注。</p>
<a name="zh-cn_topic_0000002512316493_ul19293817552"></a><a name="zh-cn_topic_0000002512316493_ul19293817552"></a><ul id="zh-cn_topic_0000002512316493_ul19293817552"><li>rssi：接收能量值。</li><li>num_packet：收包数。</li></ul>
<p id="zh-cn_topic_0000002512316493_p834mcpsimp"><a name="zh-cn_topic_0000002512316493_p834mcpsimp"></a><a name="zh-cn_topic_0000002512316493_p834mcpsimp"></a>为避免丢包，停止RX指令发送前要保证仪表已经完成发包。</p>
</td>
</tr>
</tbody>
</table>

### BT复位指令<a name="ZH-CN_TOPIC_0000002516021533"></a>

<a name="zh-cn_topic_0000002480356618_table137mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002480356618_row142mcpsimp"><th class="firstcol" valign="top" width="16.150000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002480356618_p144mcpsimp"><a name="zh-cn_topic_0000002480356618_p144mcpsimp"></a><a name="zh-cn_topic_0000002480356618_p144mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="83.85000000000001%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002480356618_p146mcpsimp"><a name="zh-cn_topic_0000002480356618_p146mcpsimp"></a><a name="zh-cn_topic_0000002480356618_p146mcpsimp"></a>AT^BTRST</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356618_row148mcpsimp"><th class="firstcol" valign="top" width="16.150000000000002%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002480356618_p150mcpsimp"><a name="zh-cn_topic_0000002480356618_p150mcpsimp"></a><a name="zh-cn_topic_0000002480356618_p150mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="83.85000000000001%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002480356618_ul104486224265"></a><a name="zh-cn_topic_0000002480356618_ul104486224265"></a><ul id="zh-cn_topic_0000002480356618_ul104486224265"><li>成功：OK<p id="zh-cn_topic_0000002480356618_p12333627162612"><a name="zh-cn_topic_0000002480356618_p12333627162612"></a><a name="zh-cn_topic_0000002480356618_p12333627162612"></a>status:0</p>
</li><li>失败：ERROR</li></ul>
<p id="zh-cn_topic_0000002480356618_p8676123131615"><a name="zh-cn_topic_0000002480356618_p8676123131615"></a><a name="zh-cn_topic_0000002480356618_p8676123131615"></a>ERROR原因：格式错误。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356618_row155mcpsimp"><th class="firstcol" valign="top" width="16.150000000000002%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002480356618_p157mcpsimp"><a name="zh-cn_topic_0000002480356618_p157mcpsimp"></a><a name="zh-cn_topic_0000002480356618_p157mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="83.85000000000001%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000002480356618_p159mcpsimp"><a name="zh-cn_topic_0000002480356618_p159mcpsimp"></a><a name="zh-cn_topic_0000002480356618_p159mcpsimp"></a>AT^BTRST</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356618_row161mcpsimp"><th class="firstcol" valign="top" width="16.150000000000002%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002480356618_p163mcpsimp"><a name="zh-cn_topic_0000002480356618_p163mcpsimp"></a><a name="zh-cn_topic_0000002480356618_p163mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="83.85000000000001%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002480356618_p165mcpsimp"><a name="zh-cn_topic_0000002480356618_p165mcpsimp"></a><a name="zh-cn_topic_0000002480356618_p165mcpsimp"></a>BT软件复位指令，在进行BT RF测试前先发BT复位指令恢复软件状态。 只复位BT TX与RX状态，不代表复位整个系统。</p>
</td>
</tr>
</tbody>
</table>

## BLE射频测试相关AT指令描述<a name="ZH-CN_TOPIC_0000002483901568"></a>

-   **[BLE测试建议流程](#ZH-CN_TOPIC_0000002483901576)**  

-   **[BLE使能指令](#ZH-CN_TOPIC_0000002483741610)**  

-   **[BLE注册回调](#ZH-CN_TOPIC_0000002483901530)**  

-   **[常发指令](#ZH-CN_TOPIC_0000002516021539)**  

-   **[常收指令](#ZH-CN_TOPIC_0000002483901540)**  

-   **[常发常收停止指令](#ZH-CN_TOPIC_0000002516141539)**  

-   **[BLE复位指令](#ZH-CN_TOPIC_0000002483901554)**  

### BLE测试建议流程<a name="ZH-CN_TOPIC_0000002483901576"></a>

**图 1**  BLE产测AT测试流程<a name="zh-cn_topic_0000002480196636_fig1524811916210"></a>  
![](figures/BLE产测AT测试流程.png "BLE产测AT测试流程")

>![](public_sys-resources/icon-note.gif) **说明：** 
>“AT^BLEENABLE”与“AT^BLEFACCALLBACK”在上电之后只需要执行一次，若想重复执行该指令，需要复位系统。常用的复位方式为重新上下电。

### BLE使能指令<a name="ZH-CN_TOPIC_0000002483741610"></a>

<a name="zh-cn_topic_0000002480196618_table170mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002480196618_row175mcpsimp"><th class="firstcol" valign="top" width="18.43%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002480196618_p177mcpsimp"><a name="zh-cn_topic_0000002480196618_p177mcpsimp"></a><a name="zh-cn_topic_0000002480196618_p177mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="81.57%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002480196618_p9326226125914"><a name="zh-cn_topic_0000002480196618_p9326226125914"></a><a name="zh-cn_topic_0000002480196618_p9326226125914"></a>AT^BLEENABLE</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196618_row181mcpsimp"><th class="firstcol" valign="top" width="18.43%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002480196618_p183mcpsimp"><a name="zh-cn_topic_0000002480196618_p183mcpsimp"></a><a name="zh-cn_topic_0000002480196618_p183mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="81.57%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002480196618_ul1315815292311"></a><a name="zh-cn_topic_0000002480196618_ul1315815292311"></a><ul id="zh-cn_topic_0000002480196618_ul1315815292311"><li>成功：OK</li><li>失败：ERROR</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196618_row188mcpsimp"><th class="firstcol" valign="top" width="18.43%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002480196618_p190mcpsimp"><a name="zh-cn_topic_0000002480196618_p190mcpsimp"></a><a name="zh-cn_topic_0000002480196618_p190mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="81.57%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000002480196618_p532632611596"><a name="zh-cn_topic_0000002480196618_p532632611596"></a><a name="zh-cn_topic_0000002480196618_p532632611596"></a>AT^BLEENABLE</p>
<p id="zh-cn_topic_0000002480196618_p108631214219"><a name="zh-cn_topic_0000002480196618_p108631214219"></a><a name="zh-cn_topic_0000002480196618_p108631214219"></a>回复：</p>
<p id="zh-cn_topic_0000002480196618_p13536525123"><a name="zh-cn_topic_0000002480196618_p13536525123"></a><a name="zh-cn_topic_0000002480196618_p13536525123"></a>OK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196618_row194mcpsimp"><th class="firstcol" valign="top" width="18.43%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002480196618_p196mcpsimp"><a name="zh-cn_topic_0000002480196618_p196mcpsimp"></a><a name="zh-cn_topic_0000002480196618_p196mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="81.57%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002480196618_p932618269598"><a name="zh-cn_topic_0000002480196618_p932618269598"></a><a name="zh-cn_topic_0000002480196618_p932618269598"></a>在进行BLE所有指令前，必须先执行BLE使能指令，使能BLE相关业务接口。</p>
<p id="zh-cn_topic_0000002480196618_p764mcpsimp"><a name="zh-cn_topic_0000002480196618_p764mcpsimp"></a><a name="zh-cn_topic_0000002480196618_p764mcpsimp"></a>本使能指令是测试BLE指令的前置条件之一，没有相应退出操作。</p>
<p id="zh-cn_topic_0000002480196618_p1344886123718"><a name="zh-cn_topic_0000002480196618_p1344886123718"></a><a name="zh-cn_topic_0000002480196618_p1344886123718"></a>上电后发一次即可。</p>
</td>
</tr>
</tbody>
</table>

### BLE注册回调<a name="ZH-CN_TOPIC_0000002483901530"></a>

<a name="zh-cn_topic_0000002480196622_table170mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002480196622_row175mcpsimp"><th class="firstcol" valign="top" width="18.41%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002480196622_p177mcpsimp"><a name="zh-cn_topic_0000002480196622_p177mcpsimp"></a><a name="zh-cn_topic_0000002480196622_p177mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="81.58999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002480196622_p9326226125914"><a name="zh-cn_topic_0000002480196622_p9326226125914"></a><a name="zh-cn_topic_0000002480196622_p9326226125914"></a>AT^BLEFACCALLBACK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196622_row181mcpsimp"><th class="firstcol" valign="top" width="18.41%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002480196622_p183mcpsimp"><a name="zh-cn_topic_0000002480196622_p183mcpsimp"></a><a name="zh-cn_topic_0000002480196622_p183mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="81.58999999999999%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002480196622_ul1315815292311"></a><a name="zh-cn_topic_0000002480196622_ul1315815292311"></a><ul id="zh-cn_topic_0000002480196622_ul1315815292311"><li>成功：OK</li><li>失败：ERROR</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196622_row188mcpsimp"><th class="firstcol" valign="top" width="18.41%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002480196622_p190mcpsimp"><a name="zh-cn_topic_0000002480196622_p190mcpsimp"></a><a name="zh-cn_topic_0000002480196622_p190mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="81.58999999999999%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000002480196622_p532632611596"><a name="zh-cn_topic_0000002480196622_p532632611596"></a><a name="zh-cn_topic_0000002480196622_p532632611596"></a>AT^BLEFACCALLBACK</p>
<p id="zh-cn_topic_0000002480196622_p108631214219"><a name="zh-cn_topic_0000002480196622_p108631214219"></a><a name="zh-cn_topic_0000002480196622_p108631214219"></a>回复：</p>
<p id="zh-cn_topic_0000002480196622_p13536525123"><a name="zh-cn_topic_0000002480196622_p13536525123"></a><a name="zh-cn_topic_0000002480196622_p13536525123"></a>OK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196622_row194mcpsimp"><th class="firstcol" valign="top" width="18.41%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002480196622_p196mcpsimp"><a name="zh-cn_topic_0000002480196622_p196mcpsimp"></a><a name="zh-cn_topic_0000002480196622_p196mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="81.58999999999999%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002480196622_p48051432152818"><a name="zh-cn_topic_0000002480196622_p48051432152818"></a><a name="zh-cn_topic_0000002480196622_p48051432152818"></a>BLE测试前先注册上报回调，否则业务指令没有上报状态打印。</p>
<p id="zh-cn_topic_0000002480196622_p16760847162819"><a name="zh-cn_topic_0000002480196622_p16760847162819"></a><a name="zh-cn_topic_0000002480196622_p16760847162819"></a>本指令是测试BLE指令的前置条件，没有相应退出操作。</p>
<p id="zh-cn_topic_0000002480196622_p18100141912813"><a name="zh-cn_topic_0000002480196622_p18100141912813"></a><a name="zh-cn_topic_0000002480196622_p18100141912813"></a>上电后发一次即可。</p>
</td>
</tr>
</tbody>
</table>

### 常发指令<a name="ZH-CN_TOPIC_0000002516021539"></a>

<a name="zh-cn_topic_0000002512316495_table836mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002512316495_row841mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002512316495_p843mcpsimp"><a name="zh-cn_topic_0000002512316495_p843mcpsimp"></a><a name="zh-cn_topic_0000002512316495_p843mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316495_p845mcpsimp"><a name="zh-cn_topic_0000002512316495_p845mcpsimp"></a><a name="zh-cn_topic_0000002512316495_p845mcpsimp"></a>AT^BLETX=&lt;freq&gt;,&lt;payload_len&gt;,&lt;payload_type&gt;,&lt;phy&gt;</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316495_row847mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002512316495_p849mcpsimp"><a name="zh-cn_topic_0000002512316495_p849mcpsimp"></a><a name="zh-cn_topic_0000002512316495_p849mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002512316495_ul557114134252"></a><a name="zh-cn_topic_0000002512316495_ul557114134252"></a><ul id="zh-cn_topic_0000002512316495_ul557114134252"><li>成功：OK<p id="zh-cn_topic_0000002512316495_p380481862514"><a name="zh-cn_topic_0000002512316495_p380481862514"></a><a name="zh-cn_topic_0000002512316495_p380481862514"></a>status：0</p>
<p id="zh-cn_topic_0000002512316495_p621816151389"><a name="zh-cn_topic_0000002512316495_p621816151389"></a><a name="zh-cn_topic_0000002512316495_p621816151389"></a>注意：status为非0值表示该指令执行出错。</p>
</li><li>失败：ERROR</li></ul>
<p id="zh-cn_topic_0000002512316495_p478792610912"><a name="zh-cn_topic_0000002512316495_p478792610912"></a><a name="zh-cn_topic_0000002512316495_p478792610912"></a>ERROR原因：指令不支持或格式错误。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316495_row854mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002512316495_p856mcpsimp"><a name="zh-cn_topic_0000002512316495_p856mcpsimp"></a><a name="zh-cn_topic_0000002512316495_p856mcpsimp"></a>参数说明</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.3.1 "><a name="zh-cn_topic_0000002512316495_ul5234101317310"></a><a name="zh-cn_topic_0000002512316495_ul5234101317310"></a><ul id="zh-cn_topic_0000002512316495_ul5234101317310"><li>&lt;freq&gt;：发送频点<p id="zh-cn_topic_0000002512316495_p14713165513359"><a name="zh-cn_topic_0000002512316495_p14713165513359"></a><a name="zh-cn_topic_0000002512316495_p14713165513359"></a>发送频率范围是（2N+2402）MHz，其中N即freq，范围0～39，频率有效范围为2402MHz～2480MHz。</p>
</li><li>&lt;payload_len&gt;：包长度<p id="zh-cn_topic_0000002512316495_p871mcpsimp"><a name="zh-cn_topic_0000002512316495_p871mcpsimp"></a><a name="zh-cn_topic_0000002512316495_p871mcpsimp"></a>根据BLE协议，包长范围是37Byte～255Byte</p>
</li><li>&lt;payload_type&gt;：发包模式<p id="zh-cn_topic_0000002512316495_p874mcpsimp"><a name="zh-cn_topic_0000002512316495_p874mcpsimp"></a><a name="zh-cn_topic_0000002512316495_p874mcpsimp"></a>0：PRBS9</p>
<p id="zh-cn_topic_0000002512316495_p875mcpsimp"><a name="zh-cn_topic_0000002512316495_p875mcpsimp"></a><a name="zh-cn_topic_0000002512316495_p875mcpsimp"></a>1：11110000</p>
<p id="zh-cn_topic_0000002512316495_p876mcpsimp"><a name="zh-cn_topic_0000002512316495_p876mcpsimp"></a><a name="zh-cn_topic_0000002512316495_p876mcpsimp"></a>2：10101010</p>
<p id="zh-cn_topic_0000002512316495_p194873283469"><a name="zh-cn_topic_0000002512316495_p194873283469"></a><a name="zh-cn_topic_0000002512316495_p194873283469"></a>3：PRBS15</p>
<p id="zh-cn_topic_0000002512316495_p143991931174619"><a name="zh-cn_topic_0000002512316495_p143991931174619"></a><a name="zh-cn_topic_0000002512316495_p143991931174619"></a>4：11111111</p>
<p id="zh-cn_topic_0000002512316495_p1145903384617"><a name="zh-cn_topic_0000002512316495_p1145903384617"></a><a name="zh-cn_topic_0000002512316495_p1145903384617"></a>5：00000000</p>
<p id="zh-cn_topic_0000002512316495_p1776753514616"><a name="zh-cn_topic_0000002512316495_p1776753514616"></a><a name="zh-cn_topic_0000002512316495_p1776753514616"></a>6：00001111</p>
<p id="zh-cn_topic_0000002512316495_p18968422174611"><a name="zh-cn_topic_0000002512316495_p18968422174611"></a><a name="zh-cn_topic_0000002512316495_p18968422174611"></a>7：01010101</p>
</li><li>&lt;phy&gt;：PHY类型<p id="zh-cn_topic_0000002512316495_p10498145874518"><a name="zh-cn_topic_0000002512316495_p10498145874518"></a><a name="zh-cn_topic_0000002512316495_p10498145874518"></a>1：1M PHY</p>
<p id="zh-cn_topic_0000002512316495_p8815130104612"><a name="zh-cn_topic_0000002512316495_p8815130104612"></a><a name="zh-cn_topic_0000002512316495_p8815130104612"></a>2：2M PHY</p>
<p id="zh-cn_topic_0000002512316495_p9168545463"><a name="zh-cn_topic_0000002512316495_p9168545463"></a><a name="zh-cn_topic_0000002512316495_p9168545463"></a>3：Coded PHY with S=8 data coding</p>
<p id="zh-cn_topic_0000002512316495_p745720542454"><a name="zh-cn_topic_0000002512316495_p745720542454"></a><a name="zh-cn_topic_0000002512316495_p745720542454"></a>4：Coded PHY with S=2 data coding</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316495_row912mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002512316495_p914mcpsimp"><a name="zh-cn_topic_0000002512316495_p914mcpsimp"></a><a name="zh-cn_topic_0000002512316495_p914mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002512316495_p916mcpsimp"><a name="zh-cn_topic_0000002512316495_p916mcpsimp"></a><a name="zh-cn_topic_0000002512316495_p916mcpsimp"></a>AT^BLETX=0,255,0,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316495_row918mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000002512316495_p920mcpsimp"><a name="zh-cn_topic_0000002512316495_p920mcpsimp"></a><a name="zh-cn_topic_0000002512316495_p920mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.5.1 "><p id="zh-cn_topic_0000002512316495_p922mcpsimp"><a name="zh-cn_topic_0000002512316495_p922mcpsimp"></a><a name="zh-cn_topic_0000002512316495_p922mcpsimp"></a>参数均为十进制，中间“,”不可省略。</p>
<p id="zh-cn_topic_0000002512316495_p133801525318"><a name="zh-cn_topic_0000002512316495_p133801525318"></a><a name="zh-cn_topic_0000002512316495_p133801525318"></a>参数需要根据BLE协议规定进行配置，否则会出现错误。</p>
</td>
</tr>
</tbody>
</table>

### 常收指令<a name="ZH-CN_TOPIC_0000002483901540"></a>

>![](public_sys-resources/icon-notice.gif) **须知：** 
>1.  测试BT常收功能时，务必将仪表波形文件中的蓝牙地址设置为NAP=1122、UAP=33、LAP=332211。如果不按照此设置，测试将无法正确查询到收包数。
>2.  使能BT常收功能后，如果需要发送其他命令，请先参照“[常发常收停止指令](#ZH-CN_TOPIC_0000002516141539)”小节，关闭BT常收功能。

<a name="zh-cn_topic_0000002480196624_table608mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002480196624_row613mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002480196624_p615mcpsimp"><a name="zh-cn_topic_0000002480196624_p615mcpsimp"></a><a name="zh-cn_topic_0000002480196624_p615mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002480196624_p617mcpsimp"><a name="zh-cn_topic_0000002480196624_p617mcpsimp"></a><a name="zh-cn_topic_0000002480196624_p617mcpsimp"></a>AT^BLERX=&lt;freq&gt;,&lt;phy&gt;,&lt;modulation_index&gt;</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196624_row619mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002480196624_p621mcpsimp"><a name="zh-cn_topic_0000002480196624_p621mcpsimp"></a><a name="zh-cn_topic_0000002480196624_p621mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002480196624_ul169551831172513"></a><a name="zh-cn_topic_0000002480196624_ul169551831172513"></a><ul id="zh-cn_topic_0000002480196624_ul169551831172513"><li>成功：OK<p id="zh-cn_topic_0000002480196624_p10384437142516"><a name="zh-cn_topic_0000002480196624_p10384437142516"></a><a name="zh-cn_topic_0000002480196624_p10384437142516"></a>status：0</p>
<p id="zh-cn_topic_0000002480196624_p621816151389"><a name="zh-cn_topic_0000002480196624_p621816151389"></a><a name="zh-cn_topic_0000002480196624_p621816151389"></a>注意：status为非0值表示该指令执行出错。</p>
</li><li>失败：ERROR</li></ul>
<p id="zh-cn_topic_0000002480196624_p939791411614"><a name="zh-cn_topic_0000002480196624_p939791411614"></a><a name="zh-cn_topic_0000002480196624_p939791411614"></a>ERROR原因：指令不支持或格式错误。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196624_row626mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002480196624_p628mcpsimp"><a name="zh-cn_topic_0000002480196624_p628mcpsimp"></a><a name="zh-cn_topic_0000002480196624_p628mcpsimp"></a>参数说明</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.3.1 "><a name="zh-cn_topic_0000002480196624_ul2073045574113"></a><a name="zh-cn_topic_0000002480196624_ul2073045574113"></a><ul id="zh-cn_topic_0000002480196624_ul2073045574113"><li>&lt;freq&gt;：发送频点<p id="zh-cn_topic_0000002480196624_p14713165513359"><a name="zh-cn_topic_0000002480196624_p14713165513359"></a><a name="zh-cn_topic_0000002480196624_p14713165513359"></a>发送频率范围是（2N+2402）MHz，其中N即freq，范围0～39，频率有效范围为2402MHz～2480MHz。</p>
</li><li>&lt;phy&gt;：PHY类型<p id="zh-cn_topic_0000002480196624_p10498145874518"><a name="zh-cn_topic_0000002480196624_p10498145874518"></a><a name="zh-cn_topic_0000002480196624_p10498145874518"></a>1：1M PHY</p>
<p id="zh-cn_topic_0000002480196624_p8815130104612"><a name="zh-cn_topic_0000002480196624_p8815130104612"></a><a name="zh-cn_topic_0000002480196624_p8815130104612"></a>2：2M PHY</p>
<p id="zh-cn_topic_0000002480196624_p9168545463"><a name="zh-cn_topic_0000002480196624_p9168545463"></a><a name="zh-cn_topic_0000002480196624_p9168545463"></a>3：Coded PHY with S=8 data coding</p>
<p id="zh-cn_topic_0000002480196624_p745720542454"><a name="zh-cn_topic_0000002480196624_p745720542454"></a><a name="zh-cn_topic_0000002480196624_p745720542454"></a>4：Coded PHY with S=2 data coding</p>
</li><li>&lt;modulation_index&gt;：调制指数<p id="zh-cn_topic_0000002480196624_p147901214125019"><a name="zh-cn_topic_0000002480196624_p147901214125019"></a><a name="zh-cn_topic_0000002480196624_p147901214125019"></a>0：发射机以标准调制指数发送</p>
<p id="zh-cn_topic_0000002480196624_p1875120225501"><a name="zh-cn_topic_0000002480196624_p1875120225501"></a><a name="zh-cn_topic_0000002480196624_p1875120225501"></a>1：发射机以稳定调制指数发送</p>
<p id="zh-cn_topic_0000002480196624_p20710111195017"><a name="zh-cn_topic_0000002480196624_p20710111195017"></a><a name="zh-cn_topic_0000002480196624_p20710111195017"></a>2～256：Reserved</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196624_row652mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002480196624_p654mcpsimp"><a name="zh-cn_topic_0000002480196624_p654mcpsimp"></a><a name="zh-cn_topic_0000002480196624_p654mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002480196624_p656mcpsimp"><a name="zh-cn_topic_0000002480196624_p656mcpsimp"></a><a name="zh-cn_topic_0000002480196624_p656mcpsimp"></a>AT^BLERX=0,1,0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196624_row658mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000002480196624_p660mcpsimp"><a name="zh-cn_topic_0000002480196624_p660mcpsimp"></a><a name="zh-cn_topic_0000002480196624_p660mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.5.1 "><p id="zh-cn_topic_0000002480196624_p662mcpsimp"><a name="zh-cn_topic_0000002480196624_p662mcpsimp"></a><a name="zh-cn_topic_0000002480196624_p662mcpsimp"></a>参数为十进制，中间“,”不可省略。</p>
<p id="zh-cn_topic_0000002480196624_p133801525318"><a name="zh-cn_topic_0000002480196624_p133801525318"></a><a name="zh-cn_topic_0000002480196624_p133801525318"></a>参数需要根据BLE协议规定进行配置，否则会出现错误。</p>
</td>
</tr>
</tbody>
</table>

### 常发常收停止指令<a name="ZH-CN_TOPIC_0000002516141539"></a>

<a name="zh-cn_topic_0000002480196630_table796mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002480196630_row801mcpsimp"><th class="firstcol" valign="top" width="18.029999999999998%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002480196630_p803mcpsimp"><a name="zh-cn_topic_0000002480196630_p803mcpsimp"></a><a name="zh-cn_topic_0000002480196630_p803mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="81.97%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002480196630_p805mcpsimp"><a name="zh-cn_topic_0000002480196630_p805mcpsimp"></a><a name="zh-cn_topic_0000002480196630_p805mcpsimp"></a>AT^BLETRXEND</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196630_row807mcpsimp"><th class="firstcol" valign="top" width="18.029999999999998%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002480196630_p809mcpsimp"><a name="zh-cn_topic_0000002480196630_p809mcpsimp"></a><a name="zh-cn_topic_0000002480196630_p809mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="81.97%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002480196630_ul243013532612"></a><a name="zh-cn_topic_0000002480196630_ul243013532612"></a><ul id="zh-cn_topic_0000002480196630_ul243013532612"><li>成功：OK<p id="zh-cn_topic_0000002480196630_p64931110102619"><a name="zh-cn_topic_0000002480196630_p64931110102619"></a><a name="zh-cn_topic_0000002480196630_p64931110102619"></a>status：0,num_packets:3e8</p>
<p id="zh-cn_topic_0000002480196630_p621816151389"><a name="zh-cn_topic_0000002480196630_p621816151389"></a><a name="zh-cn_topic_0000002480196630_p621816151389"></a>注意：status为非0值表示该指令执行出错。</p>
</li><li>失败：ERROR</li></ul>
<p id="zh-cn_topic_0000002480196630_p1437533210164"><a name="zh-cn_topic_0000002480196630_p1437533210164"></a><a name="zh-cn_topic_0000002480196630_p1437533210164"></a>ERROR原因：格式错误。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196630_row824mcpsimp"><th class="firstcol" valign="top" width="18.029999999999998%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002480196630_p826mcpsimp"><a name="zh-cn_topic_0000002480196630_p826mcpsimp"></a><a name="zh-cn_topic_0000002480196630_p826mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="81.97%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000002480196630_p941392314720"><a name="zh-cn_topic_0000002480196630_p941392314720"></a><a name="zh-cn_topic_0000002480196630_p941392314720"></a>AT^BLETRXEND</p>
<p id="zh-cn_topic_0000002480196630_p541311235715"><a name="zh-cn_topic_0000002480196630_p541311235715"></a><a name="zh-cn_topic_0000002480196630_p541311235715"></a>回复：</p>
<p id="zh-cn_topic_0000002480196630_p941317231572"><a name="zh-cn_topic_0000002480196630_p941317231572"></a><a name="zh-cn_topic_0000002480196630_p941317231572"></a>OK</p>
<p id="zh-cn_topic_0000002480196630_p186651255363"><a name="zh-cn_topic_0000002480196630_p186651255363"></a><a name="zh-cn_topic_0000002480196630_p186651255363"></a>status:0x0,num_packets:0x3e8</p>
<p id="zh-cn_topic_0000002480196630_p641319231272"><a name="zh-cn_topic_0000002480196630_p641319231272"></a><a name="zh-cn_topic_0000002480196630_p641319231272"></a>说明：如仪表发1000包后发结束指令，测试RX的收报数为1000包（0x3E8）。</p>
<p id="zh-cn_topic_0000002480196630_p2413323272"><a name="zh-cn_topic_0000002480196630_p2413323272"></a><a name="zh-cn_topic_0000002480196630_p2413323272"></a>如果发TX后执行结束指令，则num_packets指代发射包数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196630_row830mcpsimp"><th class="firstcol" valign="top" width="18.029999999999998%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002480196630_p832mcpsimp"><a name="zh-cn_topic_0000002480196630_p832mcpsimp"></a><a name="zh-cn_topic_0000002480196630_p832mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="81.97%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002480196630_p1595516720517"><a name="zh-cn_topic_0000002480196630_p1595516720517"></a><a name="zh-cn_topic_0000002480196630_p1595516720517"></a>此指令在执行BLE RF TX、RX后需要停止时执行。</p>
<p id="zh-cn_topic_0000002480196630_p4981119519"><a name="zh-cn_topic_0000002480196630_p4981119519"></a><a name="zh-cn_topic_0000002480196630_p4981119519"></a>num_packets在RX结束后关注。</p>
<a name="zh-cn_topic_0000002480196630_ul19293817552"></a><a name="zh-cn_topic_0000002480196630_ul19293817552"></a><ul id="zh-cn_topic_0000002480196630_ul19293817552"><li>num_packets：收包数</li></ul>
<p id="zh-cn_topic_0000002480196630_p834mcpsimp"><a name="zh-cn_topic_0000002480196630_p834mcpsimp"></a><a name="zh-cn_topic_0000002480196630_p834mcpsimp"></a>为避免丢包，停止RX指令发送前要保证仪表已经完成发包。</p>
</td>
</tr>
</tbody>
</table>

### BLE复位指令<a name="ZH-CN_TOPIC_0000002483901554"></a>

<a name="zh-cn_topic_0000002480356610_table137mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002480356610_row142mcpsimp"><th class="firstcol" valign="top" width="16.150000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002480356610_p144mcpsimp"><a name="zh-cn_topic_0000002480356610_p144mcpsimp"></a><a name="zh-cn_topic_0000002480356610_p144mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="83.85000000000001%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002480356610_p146mcpsimp"><a name="zh-cn_topic_0000002480356610_p146mcpsimp"></a><a name="zh-cn_topic_0000002480356610_p146mcpsimp"></a>AT^BLERST</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356610_row148mcpsimp"><th class="firstcol" valign="top" width="16.150000000000002%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002480356610_p150mcpsimp"><a name="zh-cn_topic_0000002480356610_p150mcpsimp"></a><a name="zh-cn_topic_0000002480356610_p150mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="83.85000000000001%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002480356610_ul104486224265"></a><a name="zh-cn_topic_0000002480356610_ul104486224265"></a><ul id="zh-cn_topic_0000002480356610_ul104486224265"><li>成功：OK<p id="zh-cn_topic_0000002480356610_p12333627162612"><a name="zh-cn_topic_0000002480356610_p12333627162612"></a><a name="zh-cn_topic_0000002480356610_p12333627162612"></a>status:0</p>
</li><li>失败：ERROR</li></ul>
<p id="zh-cn_topic_0000002480356610_p14755326181911"><a name="zh-cn_topic_0000002480356610_p14755326181911"></a><a name="zh-cn_topic_0000002480356610_p14755326181911"></a>ERROR原因：格式错误。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356610_row155mcpsimp"><th class="firstcol" valign="top" width="16.150000000000002%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002480356610_p157mcpsimp"><a name="zh-cn_topic_0000002480356610_p157mcpsimp"></a><a name="zh-cn_topic_0000002480356610_p157mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="83.85000000000001%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000002480356610_p159mcpsimp"><a name="zh-cn_topic_0000002480356610_p159mcpsimp"></a><a name="zh-cn_topic_0000002480356610_p159mcpsimp"></a>AT^BLERST</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356610_row161mcpsimp"><th class="firstcol" valign="top" width="16.150000000000002%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002480356610_p163mcpsimp"><a name="zh-cn_topic_0000002480356610_p163mcpsimp"></a><a name="zh-cn_topic_0000002480356610_p163mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="83.85000000000001%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002480356610_p165mcpsimp"><a name="zh-cn_topic_0000002480356610_p165mcpsimp"></a><a name="zh-cn_topic_0000002480356610_p165mcpsimp"></a>BLE软件复位指令，在进行BLE RF测试前先发BLE复位指令恢复软件状态。 只复位BLE TX与RX状态，不代表复位整个系统。</p>
</td>
</tr>
</tbody>
</table>

## SLE射频测试相关AT指令描述<a name="ZH-CN_TOPIC_0000002483741568"></a>

-   **[SLE测试建议流程](#ZH-CN_TOPIC_0000002516021519)**  

-   **[SLE使能指令](#ZH-CN_TOPIC_0000002483741534)**  

-   **[SLE注册回调](#ZH-CN_TOPIC_0000002516021553)**  

-   **[常发指令](#ZH-CN_TOPIC_0000002483741574)**  

-   **[常收指令](#ZH-CN_TOPIC_0000002483741588)**  

-   **[常发常收停止指令](#ZH-CN_TOPIC_0000002483901548)**  

-   **[SL复位指令](#ZH-CN_TOPIC_0000002516021485)**  

### SLE测试建议流程<a name="ZH-CN_TOPIC_0000002516021519"></a>

**图 1**  SLE产测AT测试流程<a name="zh-cn_topic_0000002512356513_fig1060617136475"></a>  
![](figures/SLE产测AT测试流程.png "SLE产测AT测试流程")

>![](public_sys-resources/icon-note.gif) **说明：** 
>“AT^SLEENABLE”与“AT^SLEFACCALLBACK”在上电之后只需要执行一次，若想重复执行该指令，需要复位系统。常用的复位方式为重新上下电。“AT^SLERST”复位命令只复位SLE下的TRX状态，不能当做复位系统。

### SLE使能指令<a name="ZH-CN_TOPIC_0000002483741534"></a>

<a name="zh-cn_topic_0000002512356521_table170mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002512356521_row175mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002512356521_p177mcpsimp"><a name="zh-cn_topic_0000002512356521_p177mcpsimp"></a><a name="zh-cn_topic_0000002512356521_p177mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512356521_p9326226125914"><a name="zh-cn_topic_0000002512356521_p9326226125914"></a><a name="zh-cn_topic_0000002512356521_p9326226125914"></a>AT^SLEENABLE</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356521_row181mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002512356521_p183mcpsimp"><a name="zh-cn_topic_0000002512356521_p183mcpsimp"></a><a name="zh-cn_topic_0000002512356521_p183mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.2.1 "><p id="zh-cn_topic_0000002512356521_p191487318431"><a name="zh-cn_topic_0000002512356521_p191487318431"></a><a name="zh-cn_topic_0000002512356521_p191487318431"></a><span>enter saf_at_common_registerOK</span></p>
<a name="zh-cn_topic_0000002512356521_ul122864642510"></a><a name="zh-cn_topic_0000002512356521_ul122864642510"></a><ul id="zh-cn_topic_0000002512356521_ul122864642510"><li>成功：OK</li><li>失败：ERROR</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356521_row188mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002512356521_p190mcpsimp"><a name="zh-cn_topic_0000002512356521_p190mcpsimp"></a><a name="zh-cn_topic_0000002512356521_p190mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000002512356521_p532632611596"><a name="zh-cn_topic_0000002512356521_p532632611596"></a><a name="zh-cn_topic_0000002512356521_p532632611596"></a>AT^SLEENABLE</p>
<p id="zh-cn_topic_0000002512356521_p108631214219"><a name="zh-cn_topic_0000002512356521_p108631214219"></a><a name="zh-cn_topic_0000002512356521_p108631214219"></a>回复：</p>
<p id="zh-cn_topic_0000002512356521_p13536525123"><a name="zh-cn_topic_0000002512356521_p13536525123"></a><a name="zh-cn_topic_0000002512356521_p13536525123"></a>OK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356521_row194mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002512356521_p196mcpsimp"><a name="zh-cn_topic_0000002512356521_p196mcpsimp"></a><a name="zh-cn_topic_0000002512356521_p196mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002512356521_p932618269598"><a name="zh-cn_topic_0000002512356521_p932618269598"></a><a name="zh-cn_topic_0000002512356521_p932618269598"></a>在进行SLE所有指令前，必须先执行SLE使能指令。</p>
<p id="zh-cn_topic_0000002512356521_p764mcpsimp"><a name="zh-cn_topic_0000002512356521_p764mcpsimp"></a><a name="zh-cn_topic_0000002512356521_p764mcpsimp"></a>本使能指令是测试SLE指令的前置条件之一，没有相应退出操作。</p>
<p id="zh-cn_topic_0000002512356521_p1344886123718"><a name="zh-cn_topic_0000002512356521_p1344886123718"></a><a name="zh-cn_topic_0000002512356521_p1344886123718"></a>上电后发一次即可。</p>
</td>
</tr>
</tbody>
</table>

### SLE注册回调<a name="ZH-CN_TOPIC_0000002516021553"></a>

<a name="zh-cn_topic_0000002480196638_table710mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002480196638_row715mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002480196638_p717mcpsimp"><a name="zh-cn_topic_0000002480196638_p717mcpsimp"></a><a name="zh-cn_topic_0000002480196638_p717mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002480196638_p719mcpsimp"><a name="zh-cn_topic_0000002480196638_p719mcpsimp"></a><a name="zh-cn_topic_0000002480196638_p719mcpsimp"></a>AT^SLEFACCALLBACK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196638_row721mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002480196638_p723mcpsimp"><a name="zh-cn_topic_0000002480196638_p723mcpsimp"></a><a name="zh-cn_topic_0000002480196638_p723mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.2.1 "><p id="zh-cn_topic_0000002480196638_p329918431903"><a name="zh-cn_topic_0000002480196638_p329918431903"></a><a name="zh-cn_topic_0000002480196638_p329918431903"></a>[AT] sle factory register cbks.</p>
<a name="zh-cn_topic_0000002480196638_ul1814913916254"></a><a name="zh-cn_topic_0000002480196638_ul1814913916254"></a><ul id="zh-cn_topic_0000002480196638_ul1814913916254"><li>成功：OK</li><li>失败：ERROR</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196638_row754mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002480196638_p756mcpsimp"><a name="zh-cn_topic_0000002480196638_p756mcpsimp"></a><a name="zh-cn_topic_0000002480196638_p756mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000002480196638_p1342815521314"><a name="zh-cn_topic_0000002480196638_p1342815521314"></a><a name="zh-cn_topic_0000002480196638_p1342815521314"></a>AT^SLEFACCALLBACK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196638_row760mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002480196638_p762mcpsimp"><a name="zh-cn_topic_0000002480196638_p762mcpsimp"></a><a name="zh-cn_topic_0000002480196638_p762mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002480196638_p3537544102313"><a name="zh-cn_topic_0000002480196638_p3537544102313"></a><a name="zh-cn_topic_0000002480196638_p3537544102313"></a>SLE测试前先注册回调。</p>
<p id="zh-cn_topic_0000002480196638_p764mcpsimp"><a name="zh-cn_topic_0000002480196638_p764mcpsimp"></a><a name="zh-cn_topic_0000002480196638_p764mcpsimp"></a>本指令是测试SLE指令的前置条件之一，没有相应退出操作。</p>
<p id="zh-cn_topic_0000002480196638_p434041243710"><a name="zh-cn_topic_0000002480196638_p434041243710"></a><a name="zh-cn_topic_0000002480196638_p434041243710"></a>上电后发一次即可。</p>
</td>
</tr>
</tbody>
</table>

### 常发指令<a name="ZH-CN_TOPIC_0000002483741574"></a>

<a name="zh-cn_topic_0000002480196628_table836mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002480196628_row841mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002480196628_p843mcpsimp"><a name="zh-cn_topic_0000002480196628_p843mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p843mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002480196628_p845mcpsimp"><a name="zh-cn_topic_0000002480196628_p845mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p845mcpsimp"></a>AT^SLETX=&lt;freq&gt;,&lt;pwr&gt;,&lt;payload_len&gt;,&lt;payload_type&gt;,&lt;phy&gt;,&lt;format&gt;,&lt;TX_rate&gt;,&lt;pilot&gt;,&lt;polar&gt;,&lt;interval&gt;</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196628_row847mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002480196628_p849mcpsimp"><a name="zh-cn_topic_0000002480196628_p849mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p849mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002480196628_ul557114134252"></a><a name="zh-cn_topic_0000002480196628_ul557114134252"></a><ul id="zh-cn_topic_0000002480196628_ul557114134252"><li>成功：OK<p id="zh-cn_topic_0000002480196628_p380481862514"><a name="zh-cn_topic_0000002480196628_p380481862514"></a><a name="zh-cn_topic_0000002480196628_p380481862514"></a>status：0</p>
<p id="zh-cn_topic_0000002480196628_p621816151389"><a name="zh-cn_topic_0000002480196628_p621816151389"></a><a name="zh-cn_topic_0000002480196628_p621816151389"></a>注意：status为非0值表示该指令执行出错。</p>
</li><li>失败：ERROR</li></ul>
<p id="zh-cn_topic_0000002480196628_p478792610912"><a name="zh-cn_topic_0000002480196628_p478792610912"></a><a name="zh-cn_topic_0000002480196628_p478792610912"></a>ERROR原因：指令不支持或格式错误。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196628_row854mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002480196628_p856mcpsimp"><a name="zh-cn_topic_0000002480196628_p856mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p856mcpsimp"></a>参数说明</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.3.1 "><a name="zh-cn_topic_0000002480196628_ul5234101317310"></a><a name="zh-cn_topic_0000002480196628_ul5234101317310"></a><ul id="zh-cn_topic_0000002480196628_ul5234101317310"><li>&lt;freq&gt;：发送频点<p id="zh-cn_topic_0000002480196628_p14713165513359"><a name="zh-cn_topic_0000002480196628_p14713165513359"></a><a name="zh-cn_topic_0000002480196628_p14713165513359"></a>0～78：射频信道号依次对应对应2402MHz～2480MHz，详见<a href="#zh-cn_topic_0000002512316475_fig12728841183211">图1</a>和<a href="#zh-cn_topic_0000002512316475_fig3784926143510">图2</a>。</p>
<p id="zh-cn_topic_0000002480196628_p1015718585537"><a name="zh-cn_topic_0000002480196628_p1015718585537"></a><a name="zh-cn_topic_0000002480196628_p1015718585537"></a>发送频点为16bit非负整数，当bit9置为1时，表示开启跳频功能。</p>
</li><li>&lt;pwr&gt;: 发送功率（最高档位5档，最低0档，每档依次降低4dB）<a name="zh-cn_topic_0000002480196628_ul11870527173711"></a><a name="zh-cn_topic_0000002480196628_ul11870527173711"></a><ul id="zh-cn_topic_0000002480196628_ul11870527173711"><li>GFSK：<p id="zh-cn_topic_0000002480196628_p868mcpsimp"><a name="zh-cn_topic_0000002480196628_p868mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p868mcpsimp"></a>5档：14dBm</p>
</li><li>BPSK/QPSK/8PSK：<p id="zh-cn_topic_0000002480196628_p67141135401"><a name="zh-cn_topic_0000002480196628_p67141135401"></a><a name="zh-cn_topic_0000002480196628_p67141135401"></a>5档：10dBm</p>
</li><li>16QAM：<p id="zh-cn_topic_0000002480196628_p1537695914387"><a name="zh-cn_topic_0000002480196628_p1537695914387"></a><a name="zh-cn_topic_0000002480196628_p1537695914387"></a>5档：8dBm</p>
</li></ul>
</li><li>&lt;payload_len&gt;：包长度<p id="zh-cn_topic_0000002480196628_p871mcpsimp"><a name="zh-cn_topic_0000002480196628_p871mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p871mcpsimp"></a>根据星闪协议，包长范围是0～255Byte</p>
</li><li>&lt;payload_type&gt;：发包模式<p id="zh-cn_topic_0000002480196628_p874mcpsimp"><a name="zh-cn_topic_0000002480196628_p874mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p874mcpsimp"></a>0：PRBS9</p>
<p id="zh-cn_topic_0000002480196628_p875mcpsimp"><a name="zh-cn_topic_0000002480196628_p875mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p875mcpsimp"></a>1：11110000</p>
<p id="zh-cn_topic_0000002480196628_p876mcpsimp"><a name="zh-cn_topic_0000002480196628_p876mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p876mcpsimp"></a>2：10101010</p>
</li><li>&lt;phy&gt;：PHY类型<p id="zh-cn_topic_0000002480196628_p884mcpsimp"><a name="zh-cn_topic_0000002480196628_p884mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p884mcpsimp"></a>0: 1M</p>
<p id="zh-cn_topic_0000002480196628_p885mcpsimp"><a name="zh-cn_topic_0000002480196628_p885mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p885mcpsimp"></a>1: 2M</p>
<p id="zh-cn_topic_0000002480196628_p886mcpsimp"><a name="zh-cn_topic_0000002480196628_p886mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p886mcpsimp"></a>4: 4M</p>
</li><li>&lt;format&gt;：帧类型<p id="zh-cn_topic_0000002480196628_p311455218217"><a name="zh-cn_topic_0000002480196628_p311455218217"></a><a name="zh-cn_topic_0000002480196628_p311455218217"></a>0：无线帧类型1</p>
<p id="zh-cn_topic_0000002480196628_p125714711138"><a name="zh-cn_topic_0000002480196628_p125714711138"></a><a name="zh-cn_topic_0000002480196628_p125714711138"></a>1：无线帧类型4</p>
<p id="zh-cn_topic_0000002480196628_p131147524210"><a name="zh-cn_topic_0000002480196628_p131147524210"></a><a name="zh-cn_topic_0000002480196628_p131147524210"></a>2：无线帧类型2</p>
<p id="zh-cn_topic_0000002480196628_p32267235513"><a name="zh-cn_topic_0000002480196628_p32267235513"></a><a name="zh-cn_topic_0000002480196628_p32267235513"></a>说明：无线帧类型1对应GFSK速率，无线帧类型2对应PSK速率</p>
</li><li>&lt;TX_rate&gt;：速率<p id="zh-cn_topic_0000002480196628_p894mcpsimp"><a name="zh-cn_topic_0000002480196628_p894mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p894mcpsimp"></a>0：GFSK/BPSK</p>
<p id="zh-cn_topic_0000002480196628_p14809184415132"><a name="zh-cn_topic_0000002480196628_p14809184415132"></a><a name="zh-cn_topic_0000002480196628_p14809184415132"></a>1：16QAM</p>
<p id="zh-cn_topic_0000002480196628_p895mcpsimp"><a name="zh-cn_topic_0000002480196628_p895mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p895mcpsimp"></a>2：QPSK</p>
<p id="zh-cn_topic_0000002480196628_p896mcpsimp"><a name="zh-cn_topic_0000002480196628_p896mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p896mcpsimp"></a>3：8PSK</p>
</li><li>&lt;pilot&gt;：导频<p id="zh-cn_topic_0000002480196628_p899mcpsimp"><a name="zh-cn_topic_0000002480196628_p899mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p899mcpsimp"></a>0：无导频</p>
<p id="zh-cn_topic_0000002480196628_p900mcpsimp"><a name="zh-cn_topic_0000002480196628_p900mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p900mcpsimp"></a>1：1:1（私有定义下）/8:1（标准定义下）</p>
<p id="zh-cn_topic_0000002480196628_p901mcpsimp"><a name="zh-cn_topic_0000002480196628_p901mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p901mcpsimp"></a>2：4:1</p>
<p id="zh-cn_topic_0000002480196628_p902mcpsimp"><a name="zh-cn_topic_0000002480196628_p902mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p902mcpsimp"></a>3：16:1</p>
</li><li>&lt;polar&gt;：编码<p id="zh-cn_topic_0000002480196628_p905mcpsimp"><a name="zh-cn_topic_0000002480196628_p905mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p905mcpsimp"></a>0：无编码</p>
<p id="zh-cn_topic_0000002480196628_p907mcpsimp"><a name="zh-cn_topic_0000002480196628_p907mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p907mcpsimp"></a>2：1/4</p>
<p id="zh-cn_topic_0000002480196628_p106462579329"><a name="zh-cn_topic_0000002480196628_p106462579329"></a><a name="zh-cn_topic_0000002480196628_p106462579329"></a>3：3/8</p>
<p id="zh-cn_topic_0000002480196628_p14124657331"><a name="zh-cn_topic_0000002480196628_p14124657331"></a><a name="zh-cn_topic_0000002480196628_p14124657331"></a>4：1/2</p>
<p id="zh-cn_topic_0000002480196628_p1638416112330"><a name="zh-cn_topic_0000002480196628_p1638416112330"></a><a name="zh-cn_topic_0000002480196628_p1638416112330"></a>5：5/8</p>
<p id="zh-cn_topic_0000002480196628_p864819103315"><a name="zh-cn_topic_0000002480196628_p864819103315"></a><a name="zh-cn_topic_0000002480196628_p864819103315"></a>6：3/4</p>
<p id="zh-cn_topic_0000002480196628_p15803172619336"><a name="zh-cn_topic_0000002480196628_p15803172619336"></a><a name="zh-cn_topic_0000002480196628_p15803172619336"></a>7：7/8</p>
</li><li>&lt;interval&gt;：间隔<p id="zh-cn_topic_0000002480196628_p911mcpsimp"><a name="zh-cn_topic_0000002480196628_p911mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p911mcpsimp"></a>经验证间隔需要[6,255] （单位125μs）, payload length为255Byte时，建议interval设置为50。</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196628_row912mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002480196628_p914mcpsimp"><a name="zh-cn_topic_0000002480196628_p914mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p914mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002480196628_p916mcpsimp"><a name="zh-cn_topic_0000002480196628_p916mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p916mcpsimp"></a>AT^SLETX=0,5,255,0,1,2,2,0,0,50</p>
<p id="zh-cn_topic_0000002480196628_p1629703916184"><a name="zh-cn_topic_0000002480196628_p1629703916184"></a><a name="zh-cn_topic_0000002480196628_p1629703916184"></a>如改为跳频，则配置为：AT^SLETX=512,5,255,0,1,2,2,0,0,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480196628_row918mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000002480196628_p920mcpsimp"><a name="zh-cn_topic_0000002480196628_p920mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p920mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.5.1 "><p id="zh-cn_topic_0000002480196628_p922mcpsimp"><a name="zh-cn_topic_0000002480196628_p922mcpsimp"></a><a name="zh-cn_topic_0000002480196628_p922mcpsimp"></a>参数均为十进制，中间“,”不可省略。</p>
<p id="zh-cn_topic_0000002480196628_p133801525318"><a name="zh-cn_topic_0000002480196628_p133801525318"></a><a name="zh-cn_topic_0000002480196628_p133801525318"></a>参数需要根据星闪协议规定进行配置，否则会出现错误。</p>
</td>
</tr>
</tbody>
</table>

### 常收指令<a name="ZH-CN_TOPIC_0000002483741588"></a>

<a name="zh-cn_topic_0000002480356614_table608mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002480356614_row613mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002480356614_p615mcpsimp"><a name="zh-cn_topic_0000002480356614_p615mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p615mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002480356614_p617mcpsimp"><a name="zh-cn_topic_0000002480356614_p617mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p617mcpsimp"></a>AT^SLERX=&lt;freq&gt;,&lt;phy&gt;,&lt;format&gt;,&lt;pilot&gt;,&lt;interval&gt;</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356614_row619mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002480356614_p621mcpsimp"><a name="zh-cn_topic_0000002480356614_p621mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p621mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002480356614_ul169551831172513"></a><a name="zh-cn_topic_0000002480356614_ul169551831172513"></a><ul id="zh-cn_topic_0000002480356614_ul169551831172513"><li>成功：OK<p id="zh-cn_topic_0000002480356614_p10384437142516"><a name="zh-cn_topic_0000002480356614_p10384437142516"></a><a name="zh-cn_topic_0000002480356614_p10384437142516"></a>status：0</p>
<p id="zh-cn_topic_0000002480356614_p621816151389"><a name="zh-cn_topic_0000002480356614_p621816151389"></a><a name="zh-cn_topic_0000002480356614_p621816151389"></a>注意：status为非0值表示该指令执行出错。</p>
</li><li>失败：ERROR</li></ul>
<p id="zh-cn_topic_0000002480356614_p1218013424195"><a name="zh-cn_topic_0000002480356614_p1218013424195"></a><a name="zh-cn_topic_0000002480356614_p1218013424195"></a>ERROR原因：指令不支持或格式错误。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356614_row626mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002480356614_p628mcpsimp"><a name="zh-cn_topic_0000002480356614_p628mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p628mcpsimp"></a>参数说明</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.3.1 "><a name="zh-cn_topic_0000002480356614_ul2073045574113"></a><a name="zh-cn_topic_0000002480356614_ul2073045574113"></a><ul id="zh-cn_topic_0000002480356614_ul2073045574113"><li>&lt;freq&gt;：发送频点<p id="zh-cn_topic_0000002480356614_p1801043154110"><a name="zh-cn_topic_0000002480356614_p1801043154110"></a><a name="zh-cn_topic_0000002480356614_p1801043154110"></a>0～78：射频信道号依次对应对应2402MHz～2480MHz，详见<a href="#zh-cn_topic_0000002512316475_fig12728841183211">图1</a>和<a href="#zh-cn_topic_0000002512316475_fig3784926143510">图2</a></p>
</li><li>&lt;phy&gt;：PHY类型<p id="zh-cn_topic_0000002480356614_p635mcpsimp"><a name="zh-cn_topic_0000002480356614_p635mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p635mcpsimp"></a>0：1M</p>
<p id="zh-cn_topic_0000002480356614_p636mcpsimp"><a name="zh-cn_topic_0000002480356614_p636mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p636mcpsimp"></a>1：2M</p>
<p id="zh-cn_topic_0000002480356614_p637mcpsimp"><a name="zh-cn_topic_0000002480356614_p637mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p637mcpsimp"></a>4：4M</p>
</li><li>&lt;format&gt;：帧类型<p id="zh-cn_topic_0000002480356614_p95001325958"><a name="zh-cn_topic_0000002480356614_p95001325958"></a><a name="zh-cn_topic_0000002480356614_p95001325958"></a>0：无线帧类型1</p>
<p id="zh-cn_topic_0000002480356614_p129869174403"><a name="zh-cn_topic_0000002480356614_p129869174403"></a><a name="zh-cn_topic_0000002480356614_p129869174403"></a>1：无线帧类型4</p>
<p id="zh-cn_topic_0000002480356614_p55006251251"><a name="zh-cn_topic_0000002480356614_p55006251251"></a><a name="zh-cn_topic_0000002480356614_p55006251251"></a>2：无线帧类型2</p>
<p id="zh-cn_topic_0000002480356614_p32267235513"><a name="zh-cn_topic_0000002480356614_p32267235513"></a><a name="zh-cn_topic_0000002480356614_p32267235513"></a>说明：无线帧类型1对应GFSK速率，无线帧类型2对应PSK速率</p>
</li><li>&lt;pilot&gt;：导频<p id="zh-cn_topic_0000002480356614_p645mcpsimp"><a name="zh-cn_topic_0000002480356614_p645mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p645mcpsimp"></a>0：无导频</p>
<p id="zh-cn_topic_0000002480356614_p646mcpsimp"><a name="zh-cn_topic_0000002480356614_p646mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p646mcpsimp"></a>1：1:1</p>
<p id="zh-cn_topic_0000002480356614_p647mcpsimp"><a name="zh-cn_topic_0000002480356614_p647mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p647mcpsimp"></a>2：4:1</p>
<p id="zh-cn_topic_0000002480356614_p648mcpsimp"><a name="zh-cn_topic_0000002480356614_p648mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p648mcpsimp"></a>3：16:1</p>
</li><li>&lt;interval&gt;：间隔<p id="zh-cn_topic_0000002480356614_p911mcpsimp"><a name="zh-cn_topic_0000002480356614_p911mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p911mcpsimp"></a>经验证间隔需要[6,255] （单位125μs）, payload length为255Byte时，建议interval设置为50。</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356614_row652mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002480356614_p654mcpsimp"><a name="zh-cn_topic_0000002480356614_p654mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p654mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002480356614_p656mcpsimp"><a name="zh-cn_topic_0000002480356614_p656mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p656mcpsimp"></a>AT^SLERX=0,0,0,0,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356614_row658mcpsimp"><th class="firstcol" valign="top" width="18%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000002480356614_p660mcpsimp"><a name="zh-cn_topic_0000002480356614_p660mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p660mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="82%" headers="mcps1.1.3.5.1 "><p id="zh-cn_topic_0000002480356614_p662mcpsimp"><a name="zh-cn_topic_0000002480356614_p662mcpsimp"></a><a name="zh-cn_topic_0000002480356614_p662mcpsimp"></a>参数为十进制，中间“,”不可省略。</p>
<p id="zh-cn_topic_0000002480356614_p0369211420"><a name="zh-cn_topic_0000002480356614_p0369211420"></a><a name="zh-cn_topic_0000002480356614_p0369211420"></a>参数需要根据星闪协议规定进行配置。否则会出现错误。</p>
</td>
</tr>
</tbody>
</table>

### 常发常收停止指令<a name="ZH-CN_TOPIC_0000002483901548"></a>

<a name="zh-cn_topic_0000002512356509_table796mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002512356509_row801mcpsimp"><th class="firstcol" valign="top" width="18.029999999999998%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002512356509_p803mcpsimp"><a name="zh-cn_topic_0000002512356509_p803mcpsimp"></a><a name="zh-cn_topic_0000002512356509_p803mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="81.97%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512356509_p805mcpsimp"><a name="zh-cn_topic_0000002512356509_p805mcpsimp"></a><a name="zh-cn_topic_0000002512356509_p805mcpsimp"></a>AT^SLETRXEND</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356509_row807mcpsimp"><th class="firstcol" valign="top" width="18.029999999999998%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002512356509_p809mcpsimp"><a name="zh-cn_topic_0000002512356509_p809mcpsimp"></a><a name="zh-cn_topic_0000002512356509_p809mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="81.97%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002512356509_ul243013532612"></a><a name="zh-cn_topic_0000002512356509_ul243013532612"></a><ul id="zh-cn_topic_0000002512356509_ul243013532612"><li>成功：OK<p id="zh-cn_topic_0000002512356509_p17160307354"><a name="zh-cn_topic_0000002512356509_p17160307354"></a><a name="zh-cn_topic_0000002512356509_p17160307354"></a>status：0,num_packet:3e8,rssi:-127</p>
<p id="zh-cn_topic_0000002512356509_p64931110102619"><a name="zh-cn_topic_0000002512356509_p64931110102619"></a><a name="zh-cn_topic_0000002512356509_p64931110102619"></a>注意：status为非0值表示该指令执行出错。</p>
</li><li>失败：ERROR</li></ul>
<p id="zh-cn_topic_0000002512356509_p213364661910"><a name="zh-cn_topic_0000002512356509_p213364661910"></a><a name="zh-cn_topic_0000002512356509_p213364661910"></a>ERROR原因：格式错误。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356509_row824mcpsimp"><th class="firstcol" valign="top" width="18.029999999999998%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002512356509_p826mcpsimp"><a name="zh-cn_topic_0000002512356509_p826mcpsimp"></a><a name="zh-cn_topic_0000002512356509_p826mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="81.97%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000002512356509_p941392314720"><a name="zh-cn_topic_0000002512356509_p941392314720"></a><a name="zh-cn_topic_0000002512356509_p941392314720"></a>AT^SLETRXEND</p>
<p id="zh-cn_topic_0000002512356509_p541311235715"><a name="zh-cn_topic_0000002512356509_p541311235715"></a><a name="zh-cn_topic_0000002512356509_p541311235715"></a>回复：</p>
<p id="zh-cn_topic_0000002512356509_p941317231572"><a name="zh-cn_topic_0000002512356509_p941317231572"></a><a name="zh-cn_topic_0000002512356509_p941317231572"></a>OK</p>
<p id="zh-cn_topic_0000002512356509_p641319231272"><a name="zh-cn_topic_0000002512356509_p641319231272"></a><a name="zh-cn_topic_0000002512356509_p641319231272"></a>status:0x0,num_packet:0x3e8,rssi:-127</p>
<p id="zh-cn_topic_0000002512356509_p1959135415350"><a name="zh-cn_topic_0000002512356509_p1959135415350"></a><a name="zh-cn_topic_0000002512356509_p1959135415350"></a>说明：如仪表发1000包后发结束指令，测试RX的收报数为1000包（0x3E8），测试RX的接收能量rssi为-127dBm</p>
<p id="zh-cn_topic_0000002512356509_p2413323272"><a name="zh-cn_topic_0000002512356509_p2413323272"></a><a name="zh-cn_topic_0000002512356509_p2413323272"></a>如果发TX后执行结束指令，则收包数为0，rssi为0。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356509_row830mcpsimp"><th class="firstcol" valign="top" width="18.029999999999998%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002512356509_p832mcpsimp"><a name="zh-cn_topic_0000002512356509_p832mcpsimp"></a><a name="zh-cn_topic_0000002512356509_p832mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="81.97%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002512356509_p1595516720517"><a name="zh-cn_topic_0000002512356509_p1595516720517"></a><a name="zh-cn_topic_0000002512356509_p1595516720517"></a>此指令在执行SLE RF TX、RX后需要停止时执行。</p>
<p id="zh-cn_topic_0000002512356509_p4981119519"><a name="zh-cn_topic_0000002512356509_p4981119519"></a><a name="zh-cn_topic_0000002512356509_p4981119519"></a>rssi和num_packet在RX结束后关注。</p>
<a name="zh-cn_topic_0000002512356509_ul19293817552"></a><a name="zh-cn_topic_0000002512356509_ul19293817552"></a><ul id="zh-cn_topic_0000002512356509_ul19293817552"><li>rssi：接收能量值</li><li>num_packet：收包数</li></ul>
<p id="zh-cn_topic_0000002512356509_p834mcpsimp"><a name="zh-cn_topic_0000002512356509_p834mcpsimp"></a><a name="zh-cn_topic_0000002512356509_p834mcpsimp"></a>为避免丢包，停止RX指令发送前要保证仪表已经完成发包。</p>
</td>
</tr>
</tbody>
</table>

### SL复位指令<a name="ZH-CN_TOPIC_0000002516021485"></a>

<a name="zh-cn_topic_0000002512356523_table137mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002512356523_row142mcpsimp"><th class="firstcol" valign="top" width="16.17%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002512356523_p144mcpsimp"><a name="zh-cn_topic_0000002512356523_p144mcpsimp"></a><a name="zh-cn_topic_0000002512356523_p144mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="83.83%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512356523_p146mcpsimp"><a name="zh-cn_topic_0000002512356523_p146mcpsimp"></a><a name="zh-cn_topic_0000002512356523_p146mcpsimp"></a>AT^SLERST</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356523_row148mcpsimp"><th class="firstcol" valign="top" width="16.17%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002512356523_p150mcpsimp"><a name="zh-cn_topic_0000002512356523_p150mcpsimp"></a><a name="zh-cn_topic_0000002512356523_p150mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="83.83%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002512356523_ul104486224265"></a><a name="zh-cn_topic_0000002512356523_ul104486224265"></a><ul id="zh-cn_topic_0000002512356523_ul104486224265"><li>成功：OK<p id="zh-cn_topic_0000002512356523_p12333627162612"><a name="zh-cn_topic_0000002512356523_p12333627162612"></a><a name="zh-cn_topic_0000002512356523_p12333627162612"></a>status:0</p>
</li><li>失败：ERROR</li></ul>
<p id="zh-cn_topic_0000002512356523_p1878165481919"><a name="zh-cn_topic_0000002512356523_p1878165481919"></a><a name="zh-cn_topic_0000002512356523_p1878165481919"></a>ERROR原因：格式错误。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356523_row155mcpsimp"><th class="firstcol" valign="top" width="16.17%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002512356523_p157mcpsimp"><a name="zh-cn_topic_0000002512356523_p157mcpsimp"></a><a name="zh-cn_topic_0000002512356523_p157mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="83.83%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000002512356523_p159mcpsimp"><a name="zh-cn_topic_0000002512356523_p159mcpsimp"></a><a name="zh-cn_topic_0000002512356523_p159mcpsimp"></a>AT^SLERST</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356523_row161mcpsimp"><th class="firstcol" valign="top" width="16.17%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002512356523_p163mcpsimp"><a name="zh-cn_topic_0000002512356523_p163mcpsimp"></a><a name="zh-cn_topic_0000002512356523_p163mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="83.83%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002512356523_p165mcpsimp"><a name="zh-cn_topic_0000002512356523_p165mcpsimp"></a><a name="zh-cn_topic_0000002512356523_p165mcpsimp"></a>软件复位指令。只复位SLE TX与RX状态，不代表复位整个系统。</p>
</td>
</tr>
</tbody>
</table>

## BT进入信令模式指令<a name="ZH-CN_TOPIC_0000002516141467"></a>

<a name="zh-cn_topic_0000002480356606_table200mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002480356606_row205mcpsimp"><th class="firstcol" valign="top" width="16.439999999999998%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002480356606_p207mcpsimp"><a name="zh-cn_topic_0000002480356606_p207mcpsimp"></a><a name="zh-cn_topic_0000002480356606_p207mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="83.56%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002480356606_p209mcpsimp"><a name="zh-cn_topic_0000002480356606_p209mcpsimp"></a><a name="zh-cn_topic_0000002480356606_p209mcpsimp"></a>AT^BTSIGNAL=&lt;enable&gt;</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356606_row211mcpsimp"><th class="firstcol" valign="top" width="16.439999999999998%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002480356606_p213mcpsimp"><a name="zh-cn_topic_0000002480356606_p213mcpsimp"></a><a name="zh-cn_topic_0000002480356606_p213mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="83.56%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002480356606_ul1626613812261"></a><a name="zh-cn_topic_0000002480356606_ul1626613812261"></a><ul id="zh-cn_topic_0000002480356606_ul1626613812261"><li>成功：OK</li><li>失败：ERROR<p id="zh-cn_topic_0000002480356606_p988667112015"><a name="zh-cn_topic_0000002480356606_p988667112015"></a><a name="zh-cn_topic_0000002480356606_p988667112015"></a>ERROR原因：指令不支持或格式错误。</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356606_row218mcpsimp"><th class="firstcol" valign="top" width="16.439999999999998%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002480356606_p443mcpsimp"><a name="zh-cn_topic_0000002480356606_p443mcpsimp"></a><a name="zh-cn_topic_0000002480356606_p443mcpsimp"></a>参数说明</p>
</th>
<td class="cellrowborder" valign="top" width="83.56%" headers="mcps1.1.3.3.1 "><p id="zh-cn_topic_0000002480356606_p62662244196"><a name="zh-cn_topic_0000002480356606_p62662244196"></a><a name="zh-cn_topic_0000002480356606_p62662244196"></a>&lt;enable&gt;：开关</p>
<p id="zh-cn_topic_0000002480356606_p15141911184114"><a name="zh-cn_topic_0000002480356606_p15141911184114"></a><a name="zh-cn_topic_0000002480356606_p15141911184114"></a>0：退出BT信令模式</p>
<p id="zh-cn_topic_0000002480356606_p10662015151410"><a name="zh-cn_topic_0000002480356606_p10662015151410"></a><a name="zh-cn_topic_0000002480356606_p10662015151410"></a>1：进入BT信令模式</p>
<p id="zh-cn_topic_0000002480356606_p196621715131418"><a name="zh-cn_topic_0000002480356606_p196621715131418"></a><a name="zh-cn_topic_0000002480356606_p196621715131418"></a>其他值无效</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356606_row230mcpsimp"><th class="firstcol" valign="top" width="16.439999999999998%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002480356606_p232mcpsimp"><a name="zh-cn_topic_0000002480356606_p232mcpsimp"></a><a name="zh-cn_topic_0000002480356606_p232mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="83.56%" headers="mcps1.1.3.4.1 "><a name="zh-cn_topic_0000002480356606_ul528816452518"></a><a name="zh-cn_topic_0000002480356606_ul528816452518"></a><ul id="zh-cn_topic_0000002480356606_ul528816452518"><li>AT^BTSIGNAL=1<p id="zh-cn_topic_0000002480356606_p21441650156"><a name="zh-cn_topic_0000002480356606_p21441650156"></a><a name="zh-cn_topic_0000002480356606_p21441650156"></a>说明：进入BT信令模式。</p>
</li><li>AT^BTSIGNAL=0<p id="zh-cn_topic_0000002480356606_p2562722865"><a name="zh-cn_topic_0000002480356606_p2562722865"></a><a name="zh-cn_topic_0000002480356606_p2562722865"></a>说明：退出BT信令模式。</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356606_row236mcpsimp"><th class="firstcol" valign="top" width="16.439999999999998%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000002480356606_p238mcpsimp"><a name="zh-cn_topic_0000002480356606_p238mcpsimp"></a><a name="zh-cn_topic_0000002480356606_p238mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="83.56%" headers="mcps1.1.3.5.1 "><p id="zh-cn_topic_0000002480356606_p114015124713"><a name="zh-cn_topic_0000002480356606_p114015124713"></a><a name="zh-cn_topic_0000002480356606_p114015124713"></a>发进入信令模式指令前需要先发BT业务使能信号。</p>
</td>
</tr>
</tbody>
</table>

## 射频单音<a name="ZH-CN_TOPIC_0000002516021467"></a>

<a name="zh-cn_topic_0000002512356505_table200mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002512356505_row205mcpsimp"><th class="firstcol" valign="top" width="16.439999999999998%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002512356505_p207mcpsimp"><a name="zh-cn_topic_0000002512356505_p207mcpsimp"></a><a name="zh-cn_topic_0000002512356505_p207mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="83.56%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512356505_p209mcpsimp"><a name="zh-cn_topic_0000002512356505_p209mcpsimp"></a><a name="zh-cn_topic_0000002512356505_p209mcpsimp"></a>AT^BTTXLO=&lt;freq&gt;,&lt;mode&gt;</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356505_row211mcpsimp"><th class="firstcol" valign="top" width="16.439999999999998%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002512356505_p213mcpsimp"><a name="zh-cn_topic_0000002512356505_p213mcpsimp"></a><a name="zh-cn_topic_0000002512356505_p213mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="83.56%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002512356505_ul1626613812261"></a><a name="zh-cn_topic_0000002512356505_ul1626613812261"></a><ul id="zh-cn_topic_0000002512356505_ul1626613812261"><li>成功：OK</li><li>失败：ERROR<p id="zh-cn_topic_0000002512356505_p1822115274201"><a name="zh-cn_topic_0000002512356505_p1822115274201"></a><a name="zh-cn_topic_0000002512356505_p1822115274201"></a>ERROR原因：指令不支持或格式错误。</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356505_row218mcpsimp"><th class="firstcol" valign="top" width="16.439999999999998%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002512356505_p443mcpsimp"><a name="zh-cn_topic_0000002512356505_p443mcpsimp"></a><a name="zh-cn_topic_0000002512356505_p443mcpsimp"></a>参数说明</p>
</th>
<td class="cellrowborder" valign="top" width="83.56%" headers="mcps1.1.3.3.1 "><a name="zh-cn_topic_0000002512356505_ul14483426194314"></a><a name="zh-cn_topic_0000002512356505_ul14483426194314"></a><ul id="zh-cn_topic_0000002512356505_ul14483426194314"><li>&lt;freq&gt;：频点<p id="zh-cn_topic_0000002512356505_p17931122015448"><a name="zh-cn_topic_0000002512356505_p17931122015448"></a><a name="zh-cn_topic_0000002512356505_p17931122015448"></a>0～78：射频信道号依次对应对应2402～2480 MHz</p>
</li><li>&lt;mode&gt;：开关<p id="zh-cn_topic_0000002512356505_p15141911184114"><a name="zh-cn_topic_0000002512356505_p15141911184114"></a><a name="zh-cn_topic_0000002512356505_p15141911184114"></a>0：停止单音</p>
<p id="zh-cn_topic_0000002512356505_p10662015151410"><a name="zh-cn_topic_0000002512356505_p10662015151410"></a><a name="zh-cn_topic_0000002512356505_p10662015151410"></a>1：发单音</p>
<p id="zh-cn_topic_0000002512356505_p196621715131418"><a name="zh-cn_topic_0000002512356505_p196621715131418"></a><a name="zh-cn_topic_0000002512356505_p196621715131418"></a>其他值无效</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356505_row230mcpsimp"><th class="firstcol" valign="top" width="16.439999999999998%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002512356505_p232mcpsimp"><a name="zh-cn_topic_0000002512356505_p232mcpsimp"></a><a name="zh-cn_topic_0000002512356505_p232mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="83.56%" headers="mcps1.1.3.4.1 "><a name="zh-cn_topic_0000002512356505_ul528816452518"></a><a name="zh-cn_topic_0000002512356505_ul528816452518"></a><ul id="zh-cn_topic_0000002512356505_ul528816452518"><li>AT^BTTXLO=4,1<p id="zh-cn_topic_0000002512356505_p21441650156"><a name="zh-cn_topic_0000002512356505_p21441650156"></a><a name="zh-cn_topic_0000002512356505_p21441650156"></a>说明：在（2402+4）MHz发单音。</p>
</li><li>AT^BTTXLO=4,0<p id="zh-cn_topic_0000002512356505_p7144105151516"><a name="zh-cn_topic_0000002512356505_p7144105151516"></a><a name="zh-cn_topic_0000002512356505_p7144105151516"></a>说明：在（2402+4）MHz停止单音。</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002512356505_row236mcpsimp"><th class="firstcol" valign="top" width="16.439999999999998%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000002512356505_p238mcpsimp"><a name="zh-cn_topic_0000002512356505_p238mcpsimp"></a><a name="zh-cn_topic_0000002512356505_p238mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="83.56%" headers="mcps1.1.3.5.1 "><a name="zh-cn_topic_0000002512356505_ol215510331415"></a><a name="zh-cn_topic_0000002512356505_ol215510331415"></a><ol id="zh-cn_topic_0000002512356505_ol215510331415"><li>发单音前需要先发BT/BLE/SLE业务使能信号。</li><li>发单音后不会自动停止，测试TRX性能前需要发指令“AT^BTTXLO=0,0”停止单音，避免影响。</li><li>BT/SLE发单音均使用本条指令，发单音前先发“AT^BTENABLE”。</li></ol>
</td>
</tr>
</tbody>
</table>

## XO 32M校准 执行CTRIM校准指令<a name="ZH-CN_TOPIC_0000002483741558"></a>

<a name="zh-cn_topic_0000002512316483_table422mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002512316483_row427mcpsimp"><th class="firstcol" valign="top" width="17.549999999999997%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002512316483_p429mcpsimp"><a name="zh-cn_topic_0000002512316483_p429mcpsimp"></a><a name="zh-cn_topic_0000002512316483_p429mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="82.45%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316483_p431mcpsimp"><a name="zh-cn_topic_0000002512316483_p431mcpsimp"></a><a name="zh-cn_topic_0000002512316483_p431mcpsimp"></a>AT^XOCALI=&lt;increase&gt;,&lt;step_num&gt;</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316483_row434mcpsimp"><th class="firstcol" valign="top" width="17.549999999999997%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002512316483_p436mcpsimp"><a name="zh-cn_topic_0000002512316483_p436mcpsimp"></a><a name="zh-cn_topic_0000002512316483_p436mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="82.45%" headers="mcps1.1.3.2.1 "><p id="zh-cn_topic_0000002512316483_p1142159124411"><a name="zh-cn_topic_0000002512316483_p1142159124411"></a><a name="zh-cn_topic_0000002512316483_p1142159124411"></a>xo_ctrim_reg_val:0x0</p>
<a name="zh-cn_topic_0000002512316483_ul185881851172219"></a><a name="zh-cn_topic_0000002512316483_ul185881851172219"></a><ul id="zh-cn_topic_0000002512316483_ul185881851172219"><li>成功：OK</li><li>失败：ERROR<p id="zh-cn_topic_0000002512316483_p1739718326207"><a name="zh-cn_topic_0000002512316483_p1739718326207"></a><a name="zh-cn_topic_0000002512316483_p1739718326207"></a>ERROR原因：指令不支持或格式错误。</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316483_row441mcpsimp"><th class="firstcol" valign="top" width="17.549999999999997%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002512316483_p443mcpsimp"><a name="zh-cn_topic_0000002512316483_p443mcpsimp"></a><a name="zh-cn_topic_0000002512316483_p443mcpsimp"></a>参数说明</p>
</th>
<td class="cellrowborder" valign="top" width="82.45%" headers="mcps1.1.3.3.1 "><a name="zh-cn_topic_0000002512316483_ul1226823164815"></a><a name="zh-cn_topic_0000002512316483_ul1226823164815"></a><ul id="zh-cn_topic_0000002512316483_ul1226823164815"><li>&lt; increase &gt;：增加或减小电容值<p id="zh-cn_topic_0000002512316483_p449mcpsimp"><a name="zh-cn_topic_0000002512316483_p449mcpsimp"></a><a name="zh-cn_topic_0000002512316483_p449mcpsimp"></a>0：减小</p>
<p id="zh-cn_topic_0000002512316483_p448mcpsimp"><a name="zh-cn_topic_0000002512316483_p448mcpsimp"></a><a name="zh-cn_topic_0000002512316483_p448mcpsimp"></a>1：增加</p>
</li><li>&lt; step_num &gt;：电容值调整步进<p id="zh-cn_topic_0000002512316483_p5440154214818"><a name="zh-cn_topic_0000002512316483_p5440154214818"></a><a name="zh-cn_topic_0000002512316483_p5440154214818"></a>取值范围[0,255]，默认0（十进制）</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316483_row460mcpsimp"><th class="firstcol" valign="top" width="17.549999999999997%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002512316483_p462mcpsimp"><a name="zh-cn_topic_0000002512316483_p462mcpsimp"></a><a name="zh-cn_topic_0000002512316483_p462mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="82.45%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002512316483_p174371818123311"><a name="zh-cn_topic_0000002512316483_p174371818123311"></a><a name="zh-cn_topic_0000002512316483_p174371818123311"></a>以初始电容码字为0x6为例：</p>
<a name="zh-cn_topic_0000002512316483_ul623313239496"></a><a name="zh-cn_topic_0000002512316483_ul623313239496"></a><ul id="zh-cn_topic_0000002512316483_ul623313239496"><li>AT^XOCALI=1,10<p id="zh-cn_topic_0000002512316483_p1037845365211"><a name="zh-cn_topic_0000002512316483_p1037845365211"></a><a name="zh-cn_topic_0000002512316483_p1037845365211"></a>返回值：</p>
<p id="zh-cn_topic_0000002512316483_p118315136467"><a name="zh-cn_topic_0000002512316483_p118315136467"></a><a name="zh-cn_topic_0000002512316483_p118315136467"></a>xo_ctrim_reg_val:0x10</p>
<p id="zh-cn_topic_0000002512316483_p13419132316346"><a name="zh-cn_topic_0000002512316483_p13419132316346"></a><a name="zh-cn_topic_0000002512316483_p13419132316346"></a>OK</p>
<p id="zh-cn_topic_0000002512316483_p4236322446"><a name="zh-cn_topic_0000002512316483_p4236322446"></a><a name="zh-cn_topic_0000002512316483_p4236322446"></a>说明：电容增加10steps后，码字从0x6变为0x10。具体效果通过发送单音或调制信号观测。</p>
</li><li>AT^XOCALI=0,20<p id="zh-cn_topic_0000002512316483_p187781520172215"><a name="zh-cn_topic_0000002512316483_p187781520172215"></a><a name="zh-cn_topic_0000002512316483_p187781520172215"></a>返回值：</p>
<p id="zh-cn_topic_0000002512316483_p748744554618"><a name="zh-cn_topic_0000002512316483_p748744554618"></a><a name="zh-cn_topic_0000002512316483_p748744554618"></a>xo_ctrim_reg_val:0x0</p>
<p id="zh-cn_topic_0000002512316483_p996052617340"><a name="zh-cn_topic_0000002512316483_p996052617340"></a><a name="zh-cn_topic_0000002512316483_p996052617340"></a>OK</p>
<p id="zh-cn_topic_0000002512316483_p577916207225"><a name="zh-cn_topic_0000002512316483_p577916207225"></a><a name="zh-cn_topic_0000002512316483_p577916207225"></a>说明：电容在第一步增加10后减少20，超出范围，默认减为最小值0。</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316483_row476mcpsimp"><th class="firstcol" valign="top" width="17.549999999999997%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000002512316483_p478mcpsimp"><a name="zh-cn_topic_0000002512316483_p478mcpsimp"></a><a name="zh-cn_topic_0000002512316483_p478mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="82.45%" headers="mcps1.1.3.5.1 "><p id="zh-cn_topic_0000002512316483_p480mcpsimp"><a name="zh-cn_topic_0000002512316483_p480mcpsimp"></a><a name="zh-cn_topic_0000002512316483_p480mcpsimp"></a>超出[0,255]的步进无效，默认按照最大/最小配置寄存器，校准时无特殊环境约束。</p>
<p id="zh-cn_topic_0000002512316483_p1640463554719"><a name="zh-cn_topic_0000002512316483_p1640463554719"></a><a name="zh-cn_topic_0000002512316483_p1640463554719"></a>步进和电容变化非严格线性变化，需要上位机程序使用二分法调整步进，通常常温频偏门限为&plusmn;24kHz。</p>
</td>
</tr>
</tbody>
</table>

## XO 32M校准 自动校准指令<a name="ZH-CN_TOPIC_0000002483901508"></a>

<a name="zh-cn_topic_0000002480356604_table422mcpsimp"></a>
<table><tbody><tr id="zh-cn_topic_0000002480356604_row427mcpsimp"><th class="firstcol" valign="top" width="17.549999999999997%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002480356604_p429mcpsimp"><a name="zh-cn_topic_0000002480356604_p429mcpsimp"></a><a name="zh-cn_topic_0000002480356604_p429mcpsimp"></a>格式</p>
</th>
<td class="cellrowborder" valign="top" width="82.45%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002480356604_p431mcpsimp"><a name="zh-cn_topic_0000002480356604_p431mcpsimp"></a><a name="zh-cn_topic_0000002480356604_p431mcpsimp"></a>AT^BTAUTOCALI=&lt;opera&gt;,&lt;addr&gt;</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356604_row434mcpsimp"><th class="firstcol" valign="top" width="17.549999999999997%" id="mcps1.1.3.2.1"><p id="zh-cn_topic_0000002480356604_p436mcpsimp"><a name="zh-cn_topic_0000002480356604_p436mcpsimp"></a><a name="zh-cn_topic_0000002480356604_p436mcpsimp"></a>响应</p>
</th>
<td class="cellrowborder" valign="top" width="82.45%" headers="mcps1.1.3.2.1 "><a name="zh-cn_topic_0000002480356604_ul185881851172219"></a><a name="zh-cn_topic_0000002480356604_ul185881851172219"></a><ul id="zh-cn_topic_0000002480356604_ul185881851172219"><li>成功：OK</li><li>失败：ERROR<p id="zh-cn_topic_0000002480356604_p24051537102012"><a name="zh-cn_topic_0000002480356604_p24051537102012"></a><a name="zh-cn_topic_0000002480356604_p24051537102012"></a>ERROR原因：指令不支持或格式错误。</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356604_row441mcpsimp"><th class="firstcol" valign="top" width="17.549999999999997%" id="mcps1.1.3.3.1"><p id="zh-cn_topic_0000002480356604_p443mcpsimp"><a name="zh-cn_topic_0000002480356604_p443mcpsimp"></a><a name="zh-cn_topic_0000002480356604_p443mcpsimp"></a>参数说明</p>
</th>
<td class="cellrowborder" valign="top" width="82.45%" headers="mcps1.1.3.3.1 "><a name="zh-cn_topic_0000002480356604_ul1226823164815"></a><a name="zh-cn_topic_0000002480356604_ul1226823164815"></a><ul id="zh-cn_topic_0000002480356604_ul1226823164815"><li>&lt; opera&gt;：操作指令<p id="zh-cn_topic_0000002480356604_p1470653193317"><a name="zh-cn_topic_0000002480356604_p1470653193317"></a><a name="zh-cn_topic_0000002480356604_p1470653193317"></a>操作指令分金机指令和受机指令：</p>
<p id="zh-cn_topic_0000002480356604_p1458255412422"><a name="zh-cn_topic_0000002480356604_p1458255412422"></a><a name="zh-cn_topic_0000002480356604_p1458255412422"></a><strong id="zh-cn_topic_0000002480356604_b44921023134317"><a name="zh-cn_topic_0000002480356604_b44921023134317"></a><a name="zh-cn_topic_0000002480356604_b44921023134317"></a>金机指令：</strong></p>
<p id="zh-cn_topic_0000002480356604_p774602114312"><a name="zh-cn_topic_0000002480356604_p774602114312"></a><a name="zh-cn_topic_0000002480356604_p774602114312"></a>GOLDEN：金机初始化指令，金机在开机后下发该指令使金机进入自动频偏校准测试模式，同时保存&lt;addr&gt;参数下待测受机的蓝牙地址。</p>
<p id="zh-cn_topic_0000002480356604_p430031764316"><a name="zh-cn_topic_0000002480356604_p430031764316"></a><a name="zh-cn_topic_0000002480356604_p430031764316"></a>CONNECT：金机建立连接指令。金机接收该指令后与指定蓝牙地址的受机建立连接。</p>
<p id="zh-cn_topic_0000002480356604_p95423331436"><a name="zh-cn_topic_0000002480356604_p95423331436"></a><a name="zh-cn_topic_0000002480356604_p95423331436"></a>DISCONNECT：金机断开连接指令，当测试结束或受机异常PC端长时间无法获取校准数据时，PC端主动发起给金机，金机断开与受机的连接。</p>
<p id="zh-cn_topic_0000002480356604_p13799154154312"><a name="zh-cn_topic_0000002480356604_p13799154154312"></a><a name="zh-cn_topic_0000002480356604_p13799154154312"></a><strong id="zh-cn_topic_0000002480356604_b7531195154310"><a name="zh-cn_topic_0000002480356604_b7531195154310"></a><a name="zh-cn_topic_0000002480356604_b7531195154310"></a>受机指令：</strong></p>
<p id="zh-cn_topic_0000002480356604_p38212046124318"><a name="zh-cn_topic_0000002480356604_p38212046124318"></a><a name="zh-cn_topic_0000002480356604_p38212046124318"></a>TEST：受机初始化指令，受机接收该指令后保存&lt;addr&gt;参数下金机的蓝牙地址。</p>
<p id="zh-cn_topic_0000002480356604_p58221358184319"><a name="zh-cn_topic_0000002480356604_p58221358184319"></a><a name="zh-cn_topic_0000002480356604_p58221358184319"></a>OPEN：受机启动指令，受机接收该指令后进入自动频偏校准测试模式。</p>
<p id="zh-cn_topic_0000002480356604_p913514610446"><a name="zh-cn_topic_0000002480356604_p913514610446"></a><a name="zh-cn_topic_0000002480356604_p913514610446"></a>CLOSE：受机停止指令，受机接收该指令后退出自动频偏校准测试模式。</p>
<p id="zh-cn_topic_0000002480356604_p1637920137448"><a name="zh-cn_topic_0000002480356604_p1637920137448"></a><a name="zh-cn_topic_0000002480356604_p1637920137448"></a>START：受机自动频偏校准指令，受机收到该指令后会获取BTC频偏值并自动进行频偏校准。</p>
</li><li>&lt; addr&gt;：金机/受机地址<p id="zh-cn_topic_0000002480356604_p5440154214818"><a name="zh-cn_topic_0000002480356604_p5440154214818"></a><a name="zh-cn_topic_0000002480356604_p5440154214818"></a>该参数仅在&lt;opera&gt;为GOLDEN和TEST时生效，在&lt;opera&gt;为GOLDEN时，该地址指示受机地址，在&lt;opera&gt;为TEST时，该地址指示金机地址。</p>
</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356604_row460mcpsimp"><th class="firstcol" valign="top" width="17.549999999999997%" id="mcps1.1.3.4.1"><p id="zh-cn_topic_0000002480356604_p462mcpsimp"><a name="zh-cn_topic_0000002480356604_p462mcpsimp"></a><a name="zh-cn_topic_0000002480356604_p462mcpsimp"></a>示例</p>
</th>
<td class="cellrowborder" valign="top" width="82.45%" headers="mcps1.1.3.4.1 "><p id="zh-cn_topic_0000002480356604_p19849192716499"><a name="zh-cn_topic_0000002480356604_p19849192716499"></a><a name="zh-cn_topic_0000002480356604_p19849192716499"></a>参考<a href="#zh-cn_topic_0000002480356604_fig62612916313">图1</a>，自动校准整体流程如下：</p>
<a name="zh-cn_topic_0000002480356604_ol3955121214515"></a><a name="zh-cn_topic_0000002480356604_ol3955121214515"></a><ol id="zh-cn_topic_0000002480356604_ol3955121214515"><li>PC端发起，下发AT命令“AT^BTCFG?=btaddr”到受机，查询受机1的蓝牙地址。</li><li id="zh-cn_topic_0000002480356604_li1395571275113"><a name="zh-cn_topic_0000002480356604_li1395571275113"></a><a name="zh-cn_topic_0000002480356604_li1395571275113"></a>PC端接收，收到受机1 AT串口回显蓝牙地址，PC端读取并保存。</li><li>PC端发起，下发AT命令“AT^BTAUTOCALI=GOLDEN,1122332211”到金机，1122332211为<a href="#zh-cn_topic_0000002480356604_li1395571275113">2.</a>回显的受机1地址，金机获得受机1的蓝牙地址。</li><li>PC端发起，下发AT命令“AT^BTAUTOCALI=TEST,AABBCCDDEEFF”到受机1，AABBCCDDEEFF为金机开机时配置的蓝牙地址，受机1获得金机的蓝牙地址。</li><li>PC端发起，下发AT命令“AT^BTAUTOCALI=OPEN”到受机1，令受机1进入自动校准模式。</li><li>PC端发起，下发AT命令“AT^BTAUTOCALI=CONNECT”到金机，令金机与受机1建立连接。</li><li>PC端接收，收到金机的AT串口回显连接成功“AUTO CALI CONNECTED”字样。如未收到该字段或超时，则连接失败。</li></ol>
<a name="zh-cn_topic_0000002480356604_ol5103192675116"></a><a name="zh-cn_topic_0000002480356604_ol5103192675116"></a><ol id="zh-cn_topic_0000002480356604_ol5103192675116"><li>PC端发起，下发AT命令“AT^BTAUTOCALI=START”到受机1，令受机1开启自动校准。</li><li>受机1获取校准值并进行调整，直到校准值满足条件。</li><li>PC端接收，收到受机1的AT串口回显校准成功“AUTO CALI SUCCESS”字样。如收到“AUTO CALI FAILED”字样或超期未收到字段，则校准失败。</li><li>PC端发起，下发AT命令“AT^BTAUTOCALI=STOP”到受机1，令受机1退出自动校准模式。</li><li>PC端发起，下发AT命令“AT^BTAUTOCALI=DISCONNECT”到金机，断开金机与受机1的连接。</li></ol>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356604_row476mcpsimp"><th class="firstcol" valign="top" width="17.549999999999997%" id="mcps1.1.3.5.1"><p id="zh-cn_topic_0000002480356604_p478mcpsimp"><a name="zh-cn_topic_0000002480356604_p478mcpsimp"></a><a name="zh-cn_topic_0000002480356604_p478mcpsimp"></a>注意</p>
</th>
<td class="cellrowborder" valign="top" width="82.45%" headers="mcps1.1.3.5.1 "><a name="zh-cn_topic_0000002480356604_ol9165131414529"></a><a name="zh-cn_topic_0000002480356604_ol9165131414529"></a><ol id="zh-cn_topic_0000002480356604_ol9165131414529"><li>金机被PC下发建立连接命令后，1s内金机未成功返回连接成功字段，则判定连接失败，该受机无法进行自动频偏校准，对下一个受机发起自动频偏校准请求。</li><li>受机被PC下发查询BT地址命令后，1s内受机未成功返回BT地址，则判定受机异常，该受机无法进行自动频偏校准，对下一个受机发起自动频偏校准请求。</li><li>受机被PC下发自动校准命令后，5s内未返回校准结束字段，则判断受机异常，PC需主动发送“AT^BTAUTOCALI=DISCONNECT”命令到金机，令金机断开与该受机的连接，再对下一个受机发起自动频偏校准请求。</li><li>受机自动校准完成后返回“AUTO CALI FAILED”字样，则判断该芯片不符合规格，无法进行校准。</li></ol>
</td>
</tr>
</tbody>
</table>

**图 1**  自动频偏校准流程<a name="zh-cn_topic_0000002480356604_fig62612916313"></a>  
![](figures/自动频偏校准流程.png "自动频偏校准流程")

>![](public_sys-resources/icon-notice.gif) **须知：** 
>自动校准基于两板的信号连接，板间可以通过射频线进行连接，也可以使用天线通过空口进行连接。需要注意的是，使用射频线连接可以有效避免干扰，提高信号接收质量，同时减少校准误差。我们推荐实际进行频偏校准时使用射频线进行连接。如果实在没有条件，采用空口连接时，应尽量选择较好的天线，或在信号屏蔽环境中进行校准，避免出现其他蓝牙/星闪信号干扰，减少校准失败的可能性。

## AT指令status状态对应表<a name="ZH-CN_TOPIC_0000002516021505"></a>

<a name="zh-cn_topic_0000002512316485_table112961750155810"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002512316485_row1259615103143"><th class="cellrowborder" valign="top" width="29.509999999999998%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002512316485_p16597131051411"><a name="zh-cn_topic_0000002512316485_p16597131051411"></a><a name="zh-cn_topic_0000002512316485_p16597131051411"></a>状态码</p>
</th>
<th class="cellrowborder" valign="top" width="70.49%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002512316485_p98196395395"><a name="zh-cn_topic_0000002512316485_p98196395395"></a><a name="zh-cn_topic_0000002512316485_p98196395395"></a>状态含义</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002512316485_row43371150145820"><td class="cellrowborder" valign="top" width="29.509999999999998%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316485_p123932049204119"><a name="zh-cn_topic_0000002512316485_p123932049204119"></a><a name="zh-cn_topic_0000002512316485_p123932049204119"></a>0x00</p>
</td>
<td class="cellrowborder" valign="top" width="70.49%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316485_p1138575974617"><a name="zh-cn_topic_0000002512316485_p1138575974617"></a><a name="zh-cn_topic_0000002512316485_p1138575974617"></a>成功</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316485_row113370500581"><td class="cellrowborder" valign="top" width="29.509999999999998%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316485_p12391049154118"><a name="zh-cn_topic_0000002512316485_p12391049154118"></a><a name="zh-cn_topic_0000002512316485_p12391049154118"></a>0x01</p>
</td>
<td class="cellrowborder" valign="top" width="70.49%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316485_p666212394710"><a name="zh-cn_topic_0000002512316485_p666212394710"></a><a name="zh-cn_topic_0000002512316485_p666212394710"></a>未知命令</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316485_row1133725045811"><td class="cellrowborder" valign="top" width="29.509999999999998%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316485_p1639034917415"><a name="zh-cn_topic_0000002512316485_p1639034917415"></a><a name="zh-cn_topic_0000002512316485_p1639034917415"></a>0x03</p>
</td>
<td class="cellrowborder" valign="top" width="70.49%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316485_p785317744717"><a name="zh-cn_topic_0000002512316485_p785317744717"></a><a name="zh-cn_topic_0000002512316485_p785317744717"></a>硬件问题</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316485_row533765015582"><td class="cellrowborder" valign="top" width="29.509999999999998%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316485_p19388144912419"><a name="zh-cn_topic_0000002512316485_p19388144912419"></a><a name="zh-cn_topic_0000002512316485_p19388144912419"></a>0x06</p>
</td>
<td class="cellrowborder" valign="top" width="70.49%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316485_p961212164714"><a name="zh-cn_topic_0000002512316485_p961212164714"></a><a name="zh-cn_topic_0000002512316485_p961212164714"></a>内存受限</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316485_row1533716501581"><td class="cellrowborder" valign="top" width="29.509999999999998%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316485_p103862499418"><a name="zh-cn_topic_0000002512316485_p103862499418"></a><a name="zh-cn_topic_0000002512316485_p103862499418"></a>0x07</p>
</td>
<td class="cellrowborder" valign="top" width="70.49%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316485_p8778615164715"><a name="zh-cn_topic_0000002512316485_p8778615164715"></a><a name="zh-cn_topic_0000002512316485_p8778615164715"></a>连接超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316485_row93376502585"><td class="cellrowborder" valign="top" width="29.509999999999998%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316485_p18384164924114"><a name="zh-cn_topic_0000002512316485_p18384164924114"></a><a name="zh-cn_topic_0000002512316485_p18384164924114"></a>0x0B</p>
</td>
<td class="cellrowborder" valign="top" width="70.49%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316485_p1769618216477"><a name="zh-cn_topic_0000002512316485_p1769618216477"></a><a name="zh-cn_topic_0000002512316485_p1769618216477"></a>指令不允许</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316485_row19456131183911"><td class="cellrowborder" valign="top" width="29.509999999999998%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316485_p63826497416"><a name="zh-cn_topic_0000002512316485_p63826497416"></a><a name="zh-cn_topic_0000002512316485_p63826497416"></a>0X0C</p>
</td>
<td class="cellrowborder" valign="top" width="70.49%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316485_p7986224164715"><a name="zh-cn_topic_0000002512316485_p7986224164715"></a><a name="zh-cn_topic_0000002512316485_p7986224164715"></a>地址不合法</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316485_row1273582117394"><td class="cellrowborder" valign="top" width="29.509999999999998%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316485_p2038034954111"><a name="zh-cn_topic_0000002512316485_p2038034954111"></a><a name="zh-cn_topic_0000002512316485_p2038034954111"></a>0X0D</p>
</td>
<td class="cellrowborder" valign="top" width="70.49%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316485_p184382984714"><a name="zh-cn_topic_0000002512316485_p184382984714"></a><a name="zh-cn_topic_0000002512316485_p184382984714"></a>指令超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316485_row163981525153917"><td class="cellrowborder" valign="top" width="29.509999999999998%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316485_p18378649124120"><a name="zh-cn_topic_0000002512316485_p18378649124120"></a><a name="zh-cn_topic_0000002512316485_p18378649124120"></a>0X0F</p>
</td>
<td class="cellrowborder" valign="top" width="70.49%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316485_p166523334471"><a name="zh-cn_topic_0000002512316485_p166523334471"></a><a name="zh-cn_topic_0000002512316485_p166523334471"></a>无效的指令参数</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316485_row109901831163913"><td class="cellrowborder" valign="top" width="29.509999999999998%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316485_p337618498417"><a name="zh-cn_topic_0000002512316485_p337618498417"></a><a name="zh-cn_topic_0000002512316485_p337618498417"></a>0X1F</p>
</td>
<td class="cellrowborder" valign="top" width="70.49%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316485_p17345104212470"><a name="zh-cn_topic_0000002512316485_p17345104212470"></a><a name="zh-cn_topic_0000002512316485_p17345104212470"></a>包太长出错</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002512316485_row3646111254210"><td class="cellrowborder" valign="top" width="29.509999999999998%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002512316485_p116473128427"><a name="zh-cn_topic_0000002512316485_p116473128427"></a><a name="zh-cn_topic_0000002512316485_p116473128427"></a>0X20</p>
</td>
<td class="cellrowborder" valign="top" width="70.49%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002512316485_p135844459476"><a name="zh-cn_topic_0000002512316485_p135844459476"></a><a name="zh-cn_topic_0000002512316485_p135844459476"></a>未定义错误</p>
</td>
</tr>
</tbody>
</table>

# AT指令组包示例<a name="ZH-CN_TOPIC_0000002483741518"></a>

以下为AT常发常收指令的组包示例。

**表 1**  BT TX组包

<a name="zh-cn_topic_0000002480356602_table2062815173537"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002480356602_row1762831755310"><th class="cellrowborder" valign="top" width="24.822482248224823%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002480356602_p1962810173538"><a name="zh-cn_topic_0000002480356602_p1962810173538"></a><a name="zh-cn_topic_0000002480356602_p1962810173538"></a>调制速率</p>
</th>
<th class="cellrowborder" valign="top" width="22.182218221822183%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002480356602_p156282171538"><a name="zh-cn_topic_0000002480356602_p156282171538"></a><a name="zh-cn_topic_0000002480356602_p156282171538"></a>信道</p>
</th>
<th class="cellrowborder" valign="top" width="52.995299529953%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002480356602_p1362821705320"><a name="zh-cn_topic_0000002480356602_p1362821705320"></a><a name="zh-cn_topic_0000002480356602_p1362821705320"></a>指令组包</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002480356602_row36289178534"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1762817170538"><a name="zh-cn_topic_0000002480356602_p1762817170538"></a><a name="zh-cn_topic_0000002480356602_p1762817170538"></a>1M BR DH1</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p6628151715535"><a name="zh-cn_topic_0000002480356602_p6628151715535"></a><a name="zh-cn_topic_0000002480356602_p6628151715535"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p11628101795317"><a name="zh-cn_topic_0000002480356602_p11628101795317"></a><a name="zh-cn_topic_0000002480356602_p11628101795317"></a>AT^BTTX=27,11223322111122332211,0,0,4,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row8628121785318"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p862810179530"><a name="zh-cn_topic_0000002480356602_p862810179530"></a><a name="zh-cn_topic_0000002480356602_p862810179530"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1197511191815"><a name="zh-cn_topic_0000002480356602_p1197511191815"></a><a name="zh-cn_topic_0000002480356602_p1197511191815"></a>AT^BTTX=27,1122332211,38,0,4,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row86283177539"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p136291417155319"><a name="zh-cn_topic_0000002480356602_p136291417155319"></a><a name="zh-cn_topic_0000002480356602_p136291417155319"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p73611822211"><a name="zh-cn_topic_0000002480356602_p73611822211"></a><a name="zh-cn_topic_0000002480356602_p73611822211"></a>AT^BTTX=27,1122332211,78,0,4,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row126299173533"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p01081021135412"><a name="zh-cn_topic_0000002480356602_p01081021135412"></a><a name="zh-cn_topic_0000002480356602_p01081021135412"></a>1M BR DH3</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p164631273578"><a name="zh-cn_topic_0000002480356602_p164631273578"></a><a name="zh-cn_topic_0000002480356602_p164631273578"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p2666131915215"><a name="zh-cn_topic_0000002480356602_p2666131915215"></a><a name="zh-cn_topic_0000002480356602_p2666131915215"></a>AT^BTTX=183,1122332211,0,0,11,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row16629161711534"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1546312720570"><a name="zh-cn_topic_0000002480356602_p1546312720570"></a><a name="zh-cn_topic_0000002480356602_p1546312720570"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p9667419621"><a name="zh-cn_topic_0000002480356602_p9667419621"></a><a name="zh-cn_topic_0000002480356602_p9667419621"></a>AT^BTTX=183,1122332211,38,0,11,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row962913178530"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p19463112719578"><a name="zh-cn_topic_0000002480356602_p19463112719578"></a><a name="zh-cn_topic_0000002480356602_p19463112719578"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p366719191129"><a name="zh-cn_topic_0000002480356602_p366719191129"></a><a name="zh-cn_topic_0000002480356602_p366719191129"></a>AT^BTTX=183,1122332211,78,0,11,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1862921711539"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1662914170531"><a name="zh-cn_topic_0000002480356602_p1662914170531"></a><a name="zh-cn_topic_0000002480356602_p1662914170531"></a>1M BR DH5</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1851503195719"><a name="zh-cn_topic_0000002480356602_p1851503195719"></a><a name="zh-cn_topic_0000002480356602_p1851503195719"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p8363821639"><a name="zh-cn_topic_0000002480356602_p8363821639"></a><a name="zh-cn_topic_0000002480356602_p8363821639"></a>AT^BTTX=339,1122332211,0,0,15,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row7629217115313"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p851533135720"><a name="zh-cn_topic_0000002480356602_p851533135720"></a><a name="zh-cn_topic_0000002480356602_p851533135720"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p13639212317"><a name="zh-cn_topic_0000002480356602_p13639212317"></a><a name="zh-cn_topic_0000002480356602_p13639212317"></a>AT^BTTX=339,1122332211,38,0,15,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row9629121714534"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p105151231115711"><a name="zh-cn_topic_0000002480356602_p105151231115711"></a><a name="zh-cn_topic_0000002480356602_p105151231115711"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p536312835"><a name="zh-cn_topic_0000002480356602_p536312835"></a><a name="zh-cn_topic_0000002480356602_p536312835"></a>AT^BTTX=339,1122332211,78,0,15,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row166304172535"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1602028552"><a name="zh-cn_topic_0000002480356602_p1602028552"></a><a name="zh-cn_topic_0000002480356602_p1602028552"></a>1M EDR 2DH1</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p152651386573"><a name="zh-cn_topic_0000002480356602_p152651386573"></a><a name="zh-cn_topic_0000002480356602_p152651386573"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p12564162619410"><a name="zh-cn_topic_0000002480356602_p12564162619410"></a><a name="zh-cn_topic_0000002480356602_p12564162619410"></a>AT^BTTX=54,1122332211,0,3,4,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1363061715315"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p19266173885713"><a name="zh-cn_topic_0000002480356602_p19266173885713"></a><a name="zh-cn_topic_0000002480356602_p19266173885713"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1356415263416"><a name="zh-cn_topic_0000002480356602_p1356415263416"></a><a name="zh-cn_topic_0000002480356602_p1356415263416"></a>AT^BTTX=54,1122332211,38,3,4,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row8630141765319"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1126623815717"><a name="zh-cn_topic_0000002480356602_p1126623815717"></a><a name="zh-cn_topic_0000002480356602_p1126623815717"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1156412611414"><a name="zh-cn_topic_0000002480356602_p1156412611414"></a><a name="zh-cn_topic_0000002480356602_p1156412611414"></a>AT^BTTX=54,1122332211,78,3,4,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row10630917125310"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p18630817205317"><a name="zh-cn_topic_0000002480356602_p18630817205317"></a><a name="zh-cn_topic_0000002480356602_p18630817205317"></a>1M EDR 3DH1</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p64971639175711"><a name="zh-cn_topic_0000002480356602_p64971639175711"></a><a name="zh-cn_topic_0000002480356602_p64971639175711"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p183442411519"><a name="zh-cn_topic_0000002480356602_p183442411519"></a><a name="zh-cn_topic_0000002480356602_p183442411519"></a>AT^BTTX=83,1122332211,0,6,8,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row166301917105318"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p7497839115718"><a name="zh-cn_topic_0000002480356602_p7497839115718"></a><a name="zh-cn_topic_0000002480356602_p7497839115718"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p4344641457"><a name="zh-cn_topic_0000002480356602_p4344641457"></a><a name="zh-cn_topic_0000002480356602_p4344641457"></a>AT^BTTX=83,1122332211,38,6,8,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row863116171535"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p24973393570"><a name="zh-cn_topic_0000002480356602_p24973393570"></a><a name="zh-cn_topic_0000002480356602_p24973393570"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p19344649520"><a name="zh-cn_topic_0000002480356602_p19344649520"></a><a name="zh-cn_topic_0000002480356602_p19344649520"></a>AT^BTTX=83,1122332211,78,6,8,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row263117171533"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1149718258555"><a name="zh-cn_topic_0000002480356602_p1149718258555"></a><a name="zh-cn_topic_0000002480356602_p1149718258555"></a>1M EDR 2DH3</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p12107124115577"><a name="zh-cn_topic_0000002480356602_p12107124115577"></a><a name="zh-cn_topic_0000002480356602_p12107124115577"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p3539202613614"><a name="zh-cn_topic_0000002480356602_p3539202613614"></a><a name="zh-cn_topic_0000002480356602_p3539202613614"></a>AT^BTTX=367,1122332211,0,3,10,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row36311217155315"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p2107184155717"><a name="zh-cn_topic_0000002480356602_p2107184155717"></a><a name="zh-cn_topic_0000002480356602_p2107184155717"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p9539132617611"><a name="zh-cn_topic_0000002480356602_p9539132617611"></a><a name="zh-cn_topic_0000002480356602_p9539132617611"></a>AT^BTTX=367,1122332211,38,3,10,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row36310176533"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p81073418578"><a name="zh-cn_topic_0000002480356602_p81073418578"></a><a name="zh-cn_topic_0000002480356602_p81073418578"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p155390261561"><a name="zh-cn_topic_0000002480356602_p155390261561"></a><a name="zh-cn_topic_0000002480356602_p155390261561"></a>AT^BTTX=367,1122332211,78,3,10,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row20631151795311"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p563101714533"><a name="zh-cn_topic_0000002480356602_p563101714533"></a><a name="zh-cn_topic_0000002480356602_p563101714533"></a>1M EDR 3DH3</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1885644135713"><a name="zh-cn_topic_0000002480356602_p1885644135713"></a><a name="zh-cn_topic_0000002480356602_p1885644135713"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p44636161574"><a name="zh-cn_topic_0000002480356602_p44636161574"></a><a name="zh-cn_topic_0000002480356602_p44636161574"></a>AT^BTTX=552,1122332211,0,6,11,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row86311317195317"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p198859441578"><a name="zh-cn_topic_0000002480356602_p198859441578"></a><a name="zh-cn_topic_0000002480356602_p198859441578"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p84631016379"><a name="zh-cn_topic_0000002480356602_p84631016379"></a><a name="zh-cn_topic_0000002480356602_p84631016379"></a>AT^BTTX=552,1122332211,38,6,11,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1263220174532"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p16885194416574"><a name="zh-cn_topic_0000002480356602_p16885194416574"></a><a name="zh-cn_topic_0000002480356602_p16885194416574"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p124631916579"><a name="zh-cn_topic_0000002480356602_p124631916579"></a><a name="zh-cn_topic_0000002480356602_p124631916579"></a>AT^BTTX=552,1122332211,78,6,11,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1683711364561"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p15830336185611"><a name="zh-cn_topic_0000002480356602_p15830336185611"></a><a name="zh-cn_topic_0000002480356602_p15830336185611"></a>1M EDR 2DH5</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1421713469572"><a name="zh-cn_topic_0000002480356602_p1421713469572"></a><a name="zh-cn_topic_0000002480356602_p1421713469572"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p1999318015819"><a name="zh-cn_topic_0000002480356602_p1999318015819"></a><a name="zh-cn_topic_0000002480356602_p1999318015819"></a>AT^BTTX=679,1122332211,0,3,14,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row18371436135616"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p6217144645717"><a name="zh-cn_topic_0000002480356602_p6217144645717"></a><a name="zh-cn_topic_0000002480356602_p6217144645717"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p699380782"><a name="zh-cn_topic_0000002480356602_p699380782"></a><a name="zh-cn_topic_0000002480356602_p699380782"></a>AT^BTTX=679,1122332211,38,3,14,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1783703635619"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1921784655718"><a name="zh-cn_topic_0000002480356602_p1921784655718"></a><a name="zh-cn_topic_0000002480356602_p1921784655718"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1099380787"><a name="zh-cn_topic_0000002480356602_p1099380787"></a><a name="zh-cn_topic_0000002480356602_p1099380787"></a>AT^BTTX=679,1122332211,78,3,14,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row283713625610"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p15830153605618"><a name="zh-cn_topic_0000002480356602_p15830153605618"></a><a name="zh-cn_topic_0000002480356602_p15830153605618"></a>1M EDR 3DH5</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p545016476577"><a name="zh-cn_topic_0000002480356602_p545016476577"></a><a name="zh-cn_topic_0000002480356602_p545016476577"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p9599143713818"><a name="zh-cn_topic_0000002480356602_p9599143713818"></a><a name="zh-cn_topic_0000002480356602_p9599143713818"></a>AT^BTTX=1021,1122332211,0,6,15,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row8836436195611"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p5450124775720"><a name="zh-cn_topic_0000002480356602_p5450124775720"></a><a name="zh-cn_topic_0000002480356602_p5450124775720"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p15599153714818"><a name="zh-cn_topic_0000002480356602_p15599153714818"></a><a name="zh-cn_topic_0000002480356602_p15599153714818"></a>AT^BTTX=1021,1122332211,38,6,15,0,5,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row383613610563"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p945074745718"><a name="zh-cn_topic_0000002480356602_p945074745718"></a><a name="zh-cn_topic_0000002480356602_p945074745718"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p206001375819"><a name="zh-cn_topic_0000002480356602_p206001375819"></a><a name="zh-cn_topic_0000002480356602_p206001375819"></a>AT^BTTX=1021,1122332211,78,6,15,0,5,1</p>
</td>
</tr>
</tbody>
</table>

**表 2**  BT RX组包

<a name="zh-cn_topic_0000002480356602_table1649516161119"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002480356602_row14491316201115"><th class="cellrowborder" valign="top" width="24.822482248224823%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002480356602_p124981631117"><a name="zh-cn_topic_0000002480356602_p124981631117"></a><a name="zh-cn_topic_0000002480356602_p124981631117"></a>调制速率</p>
</th>
<th class="cellrowborder" valign="top" width="22.182218221822183%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002480356602_p349216151110"><a name="zh-cn_topic_0000002480356602_p349216151110"></a><a name="zh-cn_topic_0000002480356602_p349216151110"></a>信道</p>
</th>
<th class="cellrowborder" valign="top" width="52.995299529953%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002480356602_p1349016191116"><a name="zh-cn_topic_0000002480356602_p1349016191116"></a><a name="zh-cn_topic_0000002480356602_p1349016191116"></a>指令组包</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002480356602_row134911613117"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p10491816141117"><a name="zh-cn_topic_0000002480356602_p10491816141117"></a><a name="zh-cn_topic_0000002480356602_p10491816141117"></a>1M BR DH1</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1849316171116"><a name="zh-cn_topic_0000002480356602_p1849316171116"></a><a name="zh-cn_topic_0000002480356602_p1849316171116"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p194910164119"><a name="zh-cn_topic_0000002480356602_p194910164119"></a><a name="zh-cn_topic_0000002480356602_p194910164119"></a>AT^BTRX=27,1122332211,0,0,4,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row184981615118"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p15491168117"><a name="zh-cn_topic_0000002480356602_p15491168117"></a><a name="zh-cn_topic_0000002480356602_p15491168117"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p2049201617110"><a name="zh-cn_topic_0000002480356602_p2049201617110"></a><a name="zh-cn_topic_0000002480356602_p2049201617110"></a>AT^BTRX=27,1122332211,38,0,4,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1849181601118"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p749201671119"><a name="zh-cn_topic_0000002480356602_p749201671119"></a><a name="zh-cn_topic_0000002480356602_p749201671119"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p949816121111"><a name="zh-cn_topic_0000002480356602_p949816121111"></a><a name="zh-cn_topic_0000002480356602_p949816121111"></a>AT^BTRX=27,1122332211,78,0,4,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row54951631113"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p949111613115"><a name="zh-cn_topic_0000002480356602_p949111613115"></a><a name="zh-cn_topic_0000002480356602_p949111613115"></a>1M BR DH3</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p164981612116"><a name="zh-cn_topic_0000002480356602_p164981612116"></a><a name="zh-cn_topic_0000002480356602_p164981612116"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p049181612118"><a name="zh-cn_topic_0000002480356602_p049181612118"></a><a name="zh-cn_topic_0000002480356602_p049181612118"></a>AT^BTRX=183,1122332211,0,0,11,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row17491416171118"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1249201620111"><a name="zh-cn_topic_0000002480356602_p1249201620111"></a><a name="zh-cn_topic_0000002480356602_p1249201620111"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p5493169116"><a name="zh-cn_topic_0000002480356602_p5493169116"></a><a name="zh-cn_topic_0000002480356602_p5493169116"></a>AT^BTRX=183,1122332211,38,0,11,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1949141611114"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p44991631111"><a name="zh-cn_topic_0000002480356602_p44991631111"></a><a name="zh-cn_topic_0000002480356602_p44991631111"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p15491216151111"><a name="zh-cn_topic_0000002480356602_p15491216151111"></a><a name="zh-cn_topic_0000002480356602_p15491216151111"></a>AT^BTRX=183,1122332211,78,0,11,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row549161671115"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p17491616101118"><a name="zh-cn_topic_0000002480356602_p17491616101118"></a><a name="zh-cn_topic_0000002480356602_p17491616101118"></a>1M BR DH5</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p165013168117"><a name="zh-cn_topic_0000002480356602_p165013168117"></a><a name="zh-cn_topic_0000002480356602_p165013168117"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p155031601114"><a name="zh-cn_topic_0000002480356602_p155031601114"></a><a name="zh-cn_topic_0000002480356602_p155031601114"></a>AT^BTRX=339,1122332211,0,0,15,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row450111651118"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1150131631118"><a name="zh-cn_topic_0000002480356602_p1150131631118"></a><a name="zh-cn_topic_0000002480356602_p1150131631118"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p3501816111115"><a name="zh-cn_topic_0000002480356602_p3501816111115"></a><a name="zh-cn_topic_0000002480356602_p3501816111115"></a>AT^BTRX=339,1122332211,38,0,15,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row2050131631115"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p350131616116"><a name="zh-cn_topic_0000002480356602_p350131616116"></a><a name="zh-cn_topic_0000002480356602_p350131616116"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p2501416151112"><a name="zh-cn_topic_0000002480356602_p2501416151112"></a><a name="zh-cn_topic_0000002480356602_p2501416151112"></a>AT^BTRX=339,1122332211,78,0,15,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row3501316111114"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1050111617114"><a name="zh-cn_topic_0000002480356602_p1050111617114"></a><a name="zh-cn_topic_0000002480356602_p1050111617114"></a>1M EDR 2DH1</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p155071611111"><a name="zh-cn_topic_0000002480356602_p155071611111"></a><a name="zh-cn_topic_0000002480356602_p155071611111"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p125017162119"><a name="zh-cn_topic_0000002480356602_p125017162119"></a><a name="zh-cn_topic_0000002480356602_p125017162119"></a>AT^BTRX=54,1122332211,0,3,4,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row850191671120"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p165018164110"><a name="zh-cn_topic_0000002480356602_p165018164110"></a><a name="zh-cn_topic_0000002480356602_p165018164110"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p150171613118"><a name="zh-cn_topic_0000002480356602_p150171613118"></a><a name="zh-cn_topic_0000002480356602_p150171613118"></a>AT^BTRX=54,1122332211,38,3,4,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row3507163113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p85021661115"><a name="zh-cn_topic_0000002480356602_p85021661115"></a><a name="zh-cn_topic_0000002480356602_p85021661115"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1650101611111"><a name="zh-cn_topic_0000002480356602_p1650101611111"></a><a name="zh-cn_topic_0000002480356602_p1650101611111"></a>AT^BTRX=54,1122332211,78,3,4,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row650141612116"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p85111691113"><a name="zh-cn_topic_0000002480356602_p85111691113"></a><a name="zh-cn_topic_0000002480356602_p85111691113"></a>1M EDR 3DH1</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p155161618115"><a name="zh-cn_topic_0000002480356602_p155161618115"></a><a name="zh-cn_topic_0000002480356602_p155161618115"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p6511165115"><a name="zh-cn_topic_0000002480356602_p6511165115"></a><a name="zh-cn_topic_0000002480356602_p6511165115"></a>AT^BTRX=83,1122332211,0,6,8,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row12517166119"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1251111681117"><a name="zh-cn_topic_0000002480356602_p1251111681117"></a><a name="zh-cn_topic_0000002480356602_p1251111681117"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1451141691111"><a name="zh-cn_topic_0000002480356602_p1451141691111"></a><a name="zh-cn_topic_0000002480356602_p1451141691111"></a>AT^BTRX=83,1122332211,38,6,8,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row45151614113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p55118160116"><a name="zh-cn_topic_0000002480356602_p55118160116"></a><a name="zh-cn_topic_0000002480356602_p55118160116"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p351141616117"><a name="zh-cn_topic_0000002480356602_p351141616117"></a><a name="zh-cn_topic_0000002480356602_p351141616117"></a>AT^BTRX=83,1122332211,78,6,8,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row451416191111"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p165121671112"><a name="zh-cn_topic_0000002480356602_p165121671112"></a><a name="zh-cn_topic_0000002480356602_p165121671112"></a>1M EDR 2DH3</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p951151621114"><a name="zh-cn_topic_0000002480356602_p951151621114"></a><a name="zh-cn_topic_0000002480356602_p951151621114"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p2051111619117"><a name="zh-cn_topic_0000002480356602_p2051111619117"></a><a name="zh-cn_topic_0000002480356602_p2051111619117"></a>AT^BTRX=367,1122332211,0,3,10,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row135131651115"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p35112169110"><a name="zh-cn_topic_0000002480356602_p35112169110"></a><a name="zh-cn_topic_0000002480356602_p35112169110"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p75116161110"><a name="zh-cn_topic_0000002480356602_p75116161110"></a><a name="zh-cn_topic_0000002480356602_p75116161110"></a>AT^BTRX=367,1122332211,38,3,10,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row55191661112"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p85141613112"><a name="zh-cn_topic_0000002480356602_p85141613112"></a><a name="zh-cn_topic_0000002480356602_p85141613112"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p851416111119"><a name="zh-cn_topic_0000002480356602_p851416111119"></a><a name="zh-cn_topic_0000002480356602_p851416111119"></a>AT^BTRX=367,1122332211,78,3,10,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row45111631114"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1451151612111"><a name="zh-cn_topic_0000002480356602_p1451151612111"></a><a name="zh-cn_topic_0000002480356602_p1451151612111"></a>1M EDR 3DH3</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p205161661117"><a name="zh-cn_topic_0000002480356602_p205161661117"></a><a name="zh-cn_topic_0000002480356602_p205161661117"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p25151651116"><a name="zh-cn_topic_0000002480356602_p25151651116"></a><a name="zh-cn_topic_0000002480356602_p25151651116"></a>AT^BTRX=552,1122332211,0,6,11,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1551216131119"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p45121651111"><a name="zh-cn_topic_0000002480356602_p45121651111"></a><a name="zh-cn_topic_0000002480356602_p45121651111"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1951121631114"><a name="zh-cn_topic_0000002480356602_p1951121631114"></a><a name="zh-cn_topic_0000002480356602_p1951121631114"></a>AT^BTRX=552,1122332211,38,6,11,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row18511716131118"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p751191610117"><a name="zh-cn_topic_0000002480356602_p751191610117"></a><a name="zh-cn_topic_0000002480356602_p751191610117"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p751316101115"><a name="zh-cn_topic_0000002480356602_p751316101115"></a><a name="zh-cn_topic_0000002480356602_p751316101115"></a>AT^BTRX=552,1122332211,78,6,11,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row20512016171119"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p6511216191110"><a name="zh-cn_topic_0000002480356602_p6511216191110"></a><a name="zh-cn_topic_0000002480356602_p6511216191110"></a>1M EDR 2DH5</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p55141610113"><a name="zh-cn_topic_0000002480356602_p55141610113"></a><a name="zh-cn_topic_0000002480356602_p55141610113"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p55111616112"><a name="zh-cn_topic_0000002480356602_p55111616112"></a><a name="zh-cn_topic_0000002480356602_p55111616112"></a>AT^BTRX=679,1122332211,0,3,14,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row155110169116"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p6519161111"><a name="zh-cn_topic_0000002480356602_p6519161111"></a><a name="zh-cn_topic_0000002480356602_p6519161111"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p45151610113"><a name="zh-cn_topic_0000002480356602_p45151610113"></a><a name="zh-cn_topic_0000002480356602_p45151610113"></a>AT^BTRX=679,1122332211,38,3,14,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row451141613119"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p185151691114"><a name="zh-cn_topic_0000002480356602_p185151691114"></a><a name="zh-cn_topic_0000002480356602_p185151691114"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p852161610116"><a name="zh-cn_topic_0000002480356602_p852161610116"></a><a name="zh-cn_topic_0000002480356602_p852161610116"></a>AT^BTRX=679,1122332211,78,3,14,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row152216161120"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p652516171120"><a name="zh-cn_topic_0000002480356602_p652516171120"></a><a name="zh-cn_topic_0000002480356602_p652516171120"></a>1M EDR 3DH5</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p13523169112"><a name="zh-cn_topic_0000002480356602_p13523169112"></a><a name="zh-cn_topic_0000002480356602_p13523169112"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p16521516161114"><a name="zh-cn_topic_0000002480356602_p16521516161114"></a><a name="zh-cn_topic_0000002480356602_p16521516161114"></a>AT^BTRX=1021,1122332211,0,6,15,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row25281615118"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1152216111118"><a name="zh-cn_topic_0000002480356602_p1152216111118"></a><a name="zh-cn_topic_0000002480356602_p1152216111118"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p152191621113"><a name="zh-cn_topic_0000002480356602_p152191621113"></a><a name="zh-cn_topic_0000002480356602_p152191621113"></a>AT^BTRX=1021,1122332211,38,6,15,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1152191651113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p16521416131113"><a name="zh-cn_topic_0000002480356602_p16521416131113"></a><a name="zh-cn_topic_0000002480356602_p16521416131113"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p195271621110"><a name="zh-cn_topic_0000002480356602_p195271621110"></a><a name="zh-cn_topic_0000002480356602_p195271621110"></a>AT^BTRX=1021,1122332211,78,6,15,1</p>
</td>
</tr>
</tbody>
</table>

**表 3**  BLE TX组包

<a name="zh-cn_topic_0000002480356602_table01722012165316"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002480356602_row6173512195316"><th class="cellrowborder" valign="top" width="24.822482248224823%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002480356602_p11733124537"><a name="zh-cn_topic_0000002480356602_p11733124537"></a><a name="zh-cn_topic_0000002480356602_p11733124537"></a>调制速率</p>
</th>
<th class="cellrowborder" valign="top" width="22.182218221822183%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002480356602_p1017361210534"><a name="zh-cn_topic_0000002480356602_p1017361210534"></a><a name="zh-cn_topic_0000002480356602_p1017361210534"></a>信道</p>
</th>
<th class="cellrowborder" valign="top" width="52.995299529953%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002480356602_p817391255317"><a name="zh-cn_topic_0000002480356602_p817391255317"></a><a name="zh-cn_topic_0000002480356602_p817391255317"></a>指令组包</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002480356602_row21731012115312"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p171731123537"><a name="zh-cn_topic_0000002480356602_p171731123537"></a><a name="zh-cn_topic_0000002480356602_p171731123537"></a>BLE 1M</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p15173912165312"><a name="zh-cn_topic_0000002480356602_p15173912165312"></a><a name="zh-cn_topic_0000002480356602_p15173912165312"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p7173101275316"><a name="zh-cn_topic_0000002480356602_p7173101275316"></a><a name="zh-cn_topic_0000002480356602_p7173101275316"></a>AT^BLETX=0,255,0,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row217311216533"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p121731129531"><a name="zh-cn_topic_0000002480356602_p121731129531"></a><a name="zh-cn_topic_0000002480356602_p121731129531"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p6650153101617"><a name="zh-cn_topic_0000002480356602_p6650153101617"></a><a name="zh-cn_topic_0000002480356602_p6650153101617"></a>AT^BLETX=19,255,0,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row121731312135319"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p6173912105319"><a name="zh-cn_topic_0000002480356602_p6173912105319"></a><a name="zh-cn_topic_0000002480356602_p6173912105319"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p017312126539"><a name="zh-cn_topic_0000002480356602_p017312126539"></a><a name="zh-cn_topic_0000002480356602_p017312126539"></a>AT^BLETX=39,255,0,1</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1717311225318"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p7173312155310"><a name="zh-cn_topic_0000002480356602_p7173312155310"></a><a name="zh-cn_topic_0000002480356602_p7173312155310"></a>BLE 2M</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p71558248148"><a name="zh-cn_topic_0000002480356602_p71558248148"></a><a name="zh-cn_topic_0000002480356602_p71558248148"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p139972037101712"><a name="zh-cn_topic_0000002480356602_p139972037101712"></a><a name="zh-cn_topic_0000002480356602_p139972037101712"></a>AT^BLETX=0,255,0,2</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1617311122537"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p11155724101419"><a name="zh-cn_topic_0000002480356602_p11155724101419"></a><a name="zh-cn_topic_0000002480356602_p11155724101419"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p099713715170"><a name="zh-cn_topic_0000002480356602_p099713715170"></a><a name="zh-cn_topic_0000002480356602_p099713715170"></a>AT^BLETX=19,255,0,2</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row51741129539"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p17155182414142"><a name="zh-cn_topic_0000002480356602_p17155182414142"></a><a name="zh-cn_topic_0000002480356602_p17155182414142"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p199710378170"><a name="zh-cn_topic_0000002480356602_p199710378170"></a><a name="zh-cn_topic_0000002480356602_p199710378170"></a>AT^BLETX=39,255,0,2</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row81741012105312"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p19174121255317"><a name="zh-cn_topic_0000002480356602_p19174121255317"></a><a name="zh-cn_topic_0000002480356602_p19174121255317"></a>BLE S8</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p15478192521419"><a name="zh-cn_topic_0000002480356602_p15478192521419"></a><a name="zh-cn_topic_0000002480356602_p15478192521419"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p14482918181"><a name="zh-cn_topic_0000002480356602_p14482918181"></a><a name="zh-cn_topic_0000002480356602_p14482918181"></a>AT^BLETX=0,255,0,3</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row17174191275311"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p6478112513140"><a name="zh-cn_topic_0000002480356602_p6478112513140"></a><a name="zh-cn_topic_0000002480356602_p6478112513140"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p13448494181"><a name="zh-cn_topic_0000002480356602_p13448494181"></a><a name="zh-cn_topic_0000002480356602_p13448494181"></a>AT^BLETX=19,255,0,3</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1174101215538"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1547842515149"><a name="zh-cn_topic_0000002480356602_p1547842515149"></a><a name="zh-cn_topic_0000002480356602_p1547842515149"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p12448109101814"><a name="zh-cn_topic_0000002480356602_p12448109101814"></a><a name="zh-cn_topic_0000002480356602_p12448109101814"></a>AT^BLETX=39,255,0,3</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row41741812195319"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p7174111235311"><a name="zh-cn_topic_0000002480356602_p7174111235311"></a><a name="zh-cn_topic_0000002480356602_p7174111235311"></a>BLE S2</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p19768182681411"><a name="zh-cn_topic_0000002480356602_p19768182681411"></a><a name="zh-cn_topic_0000002480356602_p19768182681411"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p88791810141816"><a name="zh-cn_topic_0000002480356602_p88791810141816"></a><a name="zh-cn_topic_0000002480356602_p88791810141816"></a>AT^BLETX=0,255,0,4</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row41741712175313"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1276815264141"><a name="zh-cn_topic_0000002480356602_p1276815264141"></a><a name="zh-cn_topic_0000002480356602_p1276815264141"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p11879101016186"><a name="zh-cn_topic_0000002480356602_p11879101016186"></a><a name="zh-cn_topic_0000002480356602_p11879101016186"></a>AT^BLETX=19,255,0,4</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row217491275311"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p47681126191412"><a name="zh-cn_topic_0000002480356602_p47681126191412"></a><a name="zh-cn_topic_0000002480356602_p47681126191412"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p14879141015182"><a name="zh-cn_topic_0000002480356602_p14879141015182"></a><a name="zh-cn_topic_0000002480356602_p14879141015182"></a>AT^BLETX=39,255,0,4</p>
</td>
</tr>
</tbody>
</table>

**表 4**  BLE RX组包

<a name="zh-cn_topic_0000002480356602_table1117512125539"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002480356602_row1817617123534"><th class="cellrowborder" valign="top" width="24.832483248324834%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002480356602_p317612128533"><a name="zh-cn_topic_0000002480356602_p317612128533"></a><a name="zh-cn_topic_0000002480356602_p317612128533"></a>调制速率</p>
</th>
<th class="cellrowborder" valign="top" width="22.922292229222922%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002480356602_p15176171205315"><a name="zh-cn_topic_0000002480356602_p15176171205315"></a><a name="zh-cn_topic_0000002480356602_p15176171205315"></a>信道</p>
</th>
<th class="cellrowborder" valign="top" width="52.24522452245224%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002480356602_p1176141245319"><a name="zh-cn_topic_0000002480356602_p1176141245319"></a><a name="zh-cn_topic_0000002480356602_p1176141245319"></a>指令组包</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002480356602_row0176101210537"><td class="cellrowborder" rowspan="3" valign="top" width="24.832483248324834%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p53231111516"><a name="zh-cn_topic_0000002480356602_p53231111516"></a><a name="zh-cn_topic_0000002480356602_p53231111516"></a>BLE 1M</p>
</td>
<td class="cellrowborder" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p11780550171511"><a name="zh-cn_topic_0000002480356602_p11780550171511"></a><a name="zh-cn_topic_0000002480356602_p11780550171511"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.24522452245224%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p656mcpsimp"><a name="zh-cn_topic_0000002480356602_p656mcpsimp"></a><a name="zh-cn_topic_0000002480356602_p656mcpsimp"></a>AT^BLERX=0,1,0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row201761126534"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p778055012151"><a name="zh-cn_topic_0000002480356602_p778055012151"></a><a name="zh-cn_topic_0000002480356602_p778055012151"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p7563163912191"><a name="zh-cn_topic_0000002480356602_p7563163912191"></a><a name="zh-cn_topic_0000002480356602_p7563163912191"></a>AT^BLERX=19,1,0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1717620128532"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p15780185018151"><a name="zh-cn_topic_0000002480356602_p15780185018151"></a><a name="zh-cn_topic_0000002480356602_p15780185018151"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p16176412105320"><a name="zh-cn_topic_0000002480356602_p16176412105320"></a><a name="zh-cn_topic_0000002480356602_p16176412105320"></a>AAT^BLERX=39,1,0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1517671215318"><td class="cellrowborder" rowspan="3" valign="top" width="24.832483248324834%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p134231056151515"><a name="zh-cn_topic_0000002480356602_p134231056151515"></a><a name="zh-cn_topic_0000002480356602_p134231056151515"></a>BLE 2M</p>
</td>
<td class="cellrowborder" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p278065051511"><a name="zh-cn_topic_0000002480356602_p278065051511"></a><a name="zh-cn_topic_0000002480356602_p278065051511"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.24522452245224%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p69841858161911"><a name="zh-cn_topic_0000002480356602_p69841858161911"></a><a name="zh-cn_topic_0000002480356602_p69841858161911"></a>AT^BLERX=0,2,0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row51761412165313"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p37801650141516"><a name="zh-cn_topic_0000002480356602_p37801650141516"></a><a name="zh-cn_topic_0000002480356602_p37801650141516"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p179847588191"><a name="zh-cn_topic_0000002480356602_p179847588191"></a><a name="zh-cn_topic_0000002480356602_p179847588191"></a>AT^BLERX=19,2,0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row2176171216535"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p7780135041517"><a name="zh-cn_topic_0000002480356602_p7780135041517"></a><a name="zh-cn_topic_0000002480356602_p7780135041517"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p598475813197"><a name="zh-cn_topic_0000002480356602_p598475813197"></a><a name="zh-cn_topic_0000002480356602_p598475813197"></a>AAT^BLERX=39,2,0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row12176191295316"><td class="cellrowborder" rowspan="3" valign="top" width="24.832483248324834%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1442318568152"><a name="zh-cn_topic_0000002480356602_p1442318568152"></a><a name="zh-cn_topic_0000002480356602_p1442318568152"></a>BLE S8</p>
</td>
<td class="cellrowborder" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p2780185021517"><a name="zh-cn_topic_0000002480356602_p2780185021517"></a><a name="zh-cn_topic_0000002480356602_p2780185021517"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.24522452245224%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p15515803209"><a name="zh-cn_topic_0000002480356602_p15515803209"></a><a name="zh-cn_topic_0000002480356602_p15515803209"></a>AT^BLERX=0,3,0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1017751211530"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p13780195001512"><a name="zh-cn_topic_0000002480356602_p13780195001512"></a><a name="zh-cn_topic_0000002480356602_p13780195001512"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p195157013207"><a name="zh-cn_topic_0000002480356602_p195157013207"></a><a name="zh-cn_topic_0000002480356602_p195157013207"></a>AT^BLERX=19,3,0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row717712126539"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p57809509158"><a name="zh-cn_topic_0000002480356602_p57809509158"></a><a name="zh-cn_topic_0000002480356602_p57809509158"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p85151008206"><a name="zh-cn_topic_0000002480356602_p85151008206"></a><a name="zh-cn_topic_0000002480356602_p85151008206"></a>AAT^BLERX=39,3,0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row8177111215318"><td class="cellrowborder" rowspan="3" valign="top" width="24.832483248324834%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p142395671516"><a name="zh-cn_topic_0000002480356602_p142395671516"></a><a name="zh-cn_topic_0000002480356602_p142395671516"></a>BLE S2</p>
</td>
<td class="cellrowborder" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1578015019152"><a name="zh-cn_topic_0000002480356602_p1578015019152"></a><a name="zh-cn_topic_0000002480356602_p1578015019152"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.24522452245224%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p1875031132016"><a name="zh-cn_topic_0000002480356602_p1875031132016"></a><a name="zh-cn_topic_0000002480356602_p1875031132016"></a>AT^BLERX=0,4,0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row6177212175316"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p147801850101513"><a name="zh-cn_topic_0000002480356602_p147801850101513"></a><a name="zh-cn_topic_0000002480356602_p147801850101513"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p97502172013"><a name="zh-cn_topic_0000002480356602_p97502172013"></a><a name="zh-cn_topic_0000002480356602_p97502172013"></a>AT^BLERX=19,4,0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row9177201214536"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1878055019151"><a name="zh-cn_topic_0000002480356602_p1878055019151"></a><a name="zh-cn_topic_0000002480356602_p1878055019151"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p167510114205"><a name="zh-cn_topic_0000002480356602_p167510114205"></a><a name="zh-cn_topic_0000002480356602_p167510114205"></a>AAT^BLERX=39,4,0</p>
</td>
</tr>
</tbody>
</table>

**表 5**  SLE TX组包

<a name="zh-cn_topic_0000002480356602_table112961750155810"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002480356602_row1259615103143"><th class="cellrowborder" valign="top" width="24.822482248224823%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002480356602_p9597151010147"><a name="zh-cn_topic_0000002480356602_p9597151010147"></a><a name="zh-cn_topic_0000002480356602_p9597151010147"></a>调制速率</p>
</th>
<th class="cellrowborder" valign="top" width="22.182218221822183%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002480356602_p1659714109148"><a name="zh-cn_topic_0000002480356602_p1659714109148"></a><a name="zh-cn_topic_0000002480356602_p1659714109148"></a>信道</p>
</th>
<th class="cellrowborder" valign="top" width="52.995299529953%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002480356602_p16597131051411"><a name="zh-cn_topic_0000002480356602_p16597131051411"></a><a name="zh-cn_topic_0000002480356602_p16597131051411"></a>指令组包</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002480356602_row43371150145820"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p23371850175814"><a name="zh-cn_topic_0000002480356602_p23371850175814"></a><a name="zh-cn_topic_0000002480356602_p23371850175814"></a>1M GFSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1771814105279"><a name="zh-cn_topic_0000002480356602_p1771814105279"></a><a name="zh-cn_topic_0000002480356602_p1771814105279"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p1337155005814"><a name="zh-cn_topic_0000002480356602_p1337155005814"></a><a name="zh-cn_topic_0000002480356602_p1337155005814"></a>AT^SLETX=0,5,255,0,0,0,0,0,0,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row113370500581"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p671815108271"><a name="zh-cn_topic_0000002480356602_p671815108271"></a><a name="zh-cn_topic_0000002480356602_p671815108271"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p33371150205811"><a name="zh-cn_topic_0000002480356602_p33371150205811"></a><a name="zh-cn_topic_0000002480356602_p33371150205811"></a>AT^SLETX=38,5,255,0,0,0,0,0,0,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1133725045811"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p77181810172719"><a name="zh-cn_topic_0000002480356602_p77181810172719"></a><a name="zh-cn_topic_0000002480356602_p77181810172719"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p13337135035819"><a name="zh-cn_topic_0000002480356602_p13337135035819"></a><a name="zh-cn_topic_0000002480356602_p13337135035819"></a>AT^SLETX=78,5,255,0,0,0,0,0,0,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row533765015582"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p633735045813"><a name="zh-cn_topic_0000002480356602_p633735045813"></a><a name="zh-cn_topic_0000002480356602_p633735045813"></a>2M GFSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1719141015274"><a name="zh-cn_topic_0000002480356602_p1719141015274"></a><a name="zh-cn_topic_0000002480356602_p1719141015274"></a>2404</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p16337115018589"><a name="zh-cn_topic_0000002480356602_p16337115018589"></a><a name="zh-cn_topic_0000002480356602_p16337115018589"></a>AT^SLETX=2,5,255,0,1,0,0,0,0,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1533716501581"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1271911012716"><a name="zh-cn_topic_0000002480356602_p1271911012716"></a><a name="zh-cn_topic_0000002480356602_p1271911012716"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p5337195016583"><a name="zh-cn_topic_0000002480356602_p5337195016583"></a><a name="zh-cn_topic_0000002480356602_p5337195016583"></a>AT^SLETX=38,5,255,0,1,0,0,0,0,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row93376502585"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1971915100273"><a name="zh-cn_topic_0000002480356602_p1971915100273"></a><a name="zh-cn_topic_0000002480356602_p1971915100273"></a>2478</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p13379504588"><a name="zh-cn_topic_0000002480356602_p13379504588"></a><a name="zh-cn_topic_0000002480356602_p13379504588"></a>AT^SLETX=76,5,255,0,1,0,0,0,0,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row3337195019589"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p18337050165818"><a name="zh-cn_topic_0000002480356602_p18337050165818"></a><a name="zh-cn_topic_0000002480356602_p18337050165818"></a>4M GFSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p18719121015271"><a name="zh-cn_topic_0000002480356602_p18719121015271"></a><a name="zh-cn_topic_0000002480356602_p18719121015271"></a>2405</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p19337165010587"><a name="zh-cn_topic_0000002480356602_p19337165010587"></a><a name="zh-cn_topic_0000002480356602_p19337165010587"></a>AT^SLETX=3,5,255,0,4,0,0,0,0,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row333716500585"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p97191104271"><a name="zh-cn_topic_0000002480356602_p97191104271"></a><a name="zh-cn_topic_0000002480356602_p97191104271"></a>2441</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p103371350105810"><a name="zh-cn_topic_0000002480356602_p103371350105810"></a><a name="zh-cn_topic_0000002480356602_p103371350105810"></a>AT^SLETX=39,5,255,0,4,0,0,0,0,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row16337195015585"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p4719310192711"><a name="zh-cn_topic_0000002480356602_p4719310192711"></a><a name="zh-cn_topic_0000002480356602_p4719310192711"></a>2477</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p433713509585"><a name="zh-cn_topic_0000002480356602_p433713509585"></a><a name="zh-cn_topic_0000002480356602_p433713509585"></a>AT^SLETX=75,5,255,0,4,0,0,0,0,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row18337155013582"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1833795011582"><a name="zh-cn_topic_0000002480356602_p1833795011582"></a><a name="zh-cn_topic_0000002480356602_p1833795011582"></a>1M QPSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p158991116202711"><a name="zh-cn_topic_0000002480356602_p158991116202711"></a><a name="zh-cn_topic_0000002480356602_p158991116202711"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p193371550135820"><a name="zh-cn_topic_0000002480356602_p193371550135820"></a><a name="zh-cn_topic_0000002480356602_p193371550135820"></a>AT^SLETX=0,5,255,0,0,2,2,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1433725025811"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p28991416142715"><a name="zh-cn_topic_0000002480356602_p28991416142715"></a><a name="zh-cn_topic_0000002480356602_p28991416142715"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1133716502589"><a name="zh-cn_topic_0000002480356602_p1133716502589"></a><a name="zh-cn_topic_0000002480356602_p1133716502589"></a>AT^SLETX=38,5,255,0,0,2,2,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row533735095816"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1489981610277"><a name="zh-cn_topic_0000002480356602_p1489981610277"></a><a name="zh-cn_topic_0000002480356602_p1489981610277"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1533755015581"><a name="zh-cn_topic_0000002480356602_p1533755015581"></a><a name="zh-cn_topic_0000002480356602_p1533755015581"></a>AT^SLETX=78,5,255,0,0,2,2,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row23377509588"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p333712501589"><a name="zh-cn_topic_0000002480356602_p333712501589"></a><a name="zh-cn_topic_0000002480356602_p333712501589"></a>2M QPSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p128994167278"><a name="zh-cn_topic_0000002480356602_p128994167278"></a><a name="zh-cn_topic_0000002480356602_p128994167278"></a>2404</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p163377507588"><a name="zh-cn_topic_0000002480356602_p163377507588"></a><a name="zh-cn_topic_0000002480356602_p163377507588"></a>AT^SLETX=2,5,255,0,1,2,2,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row5337135020583"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p98994169272"><a name="zh-cn_topic_0000002480356602_p98994169272"></a><a name="zh-cn_topic_0000002480356602_p98994169272"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p19337850105811"><a name="zh-cn_topic_0000002480356602_p19337850105811"></a><a name="zh-cn_topic_0000002480356602_p19337850105811"></a>AT^SLETX=38,5,255,0,1,2,2,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row93371450105817"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p889941619279"><a name="zh-cn_topic_0000002480356602_p889941619279"></a><a name="zh-cn_topic_0000002480356602_p889941619279"></a>2478</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p3337450205819"><a name="zh-cn_topic_0000002480356602_p3337450205819"></a><a name="zh-cn_topic_0000002480356602_p3337450205819"></a>AT^SLETX=76,5,255,0,1,2,2,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row103378505588"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1337165011585"><a name="zh-cn_topic_0000002480356602_p1337165011585"></a><a name="zh-cn_topic_0000002480356602_p1337165011585"></a>4M QPSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p10899141662718"><a name="zh-cn_topic_0000002480356602_p10899141662718"></a><a name="zh-cn_topic_0000002480356602_p10899141662718"></a>2405</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p1833705016580"><a name="zh-cn_topic_0000002480356602_p1833705016580"></a><a name="zh-cn_topic_0000002480356602_p1833705016580"></a>AT^SLETX=3,5,255,0,4,2,2,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row23375506588"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p2089931692713"><a name="zh-cn_topic_0000002480356602_p2089931692713"></a><a name="zh-cn_topic_0000002480356602_p2089931692713"></a>2441</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p6338350105815"><a name="zh-cn_topic_0000002480356602_p6338350105815"></a><a name="zh-cn_topic_0000002480356602_p6338350105815"></a>AT^SLETX=38,5,255,0,4,2,2,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row15338250195812"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p208997165279"><a name="zh-cn_topic_0000002480356602_p208997165279"></a><a name="zh-cn_topic_0000002480356602_p208997165279"></a>2477</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p8338450115810"><a name="zh-cn_topic_0000002480356602_p8338450115810"></a><a name="zh-cn_topic_0000002480356602_p8338450115810"></a>AT^SLETX=75,5,255,0,4,2,2,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row5338650175811"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p15338195010584"><a name="zh-cn_topic_0000002480356602_p15338195010584"></a><a name="zh-cn_topic_0000002480356602_p15338195010584"></a>1M 8PSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p575631902714"><a name="zh-cn_topic_0000002480356602_p575631902714"></a><a name="zh-cn_topic_0000002480356602_p575631902714"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p173388505584"><a name="zh-cn_topic_0000002480356602_p173388505584"></a><a name="zh-cn_topic_0000002480356602_p173388505584"></a>AT^SLETX=0,5,255,0,0,2,3,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row153381350195818"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p6756111912712"><a name="zh-cn_topic_0000002480356602_p6756111912712"></a><a name="zh-cn_topic_0000002480356602_p6756111912712"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p43382050165810"><a name="zh-cn_topic_0000002480356602_p43382050165810"></a><a name="zh-cn_topic_0000002480356602_p43382050165810"></a>AT^SLETX=38,5,255,0,0,2,3,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row2338350195815"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p975661920277"><a name="zh-cn_topic_0000002480356602_p975661920277"></a><a name="zh-cn_topic_0000002480356602_p975661920277"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p3338650145813"><a name="zh-cn_topic_0000002480356602_p3338650145813"></a><a name="zh-cn_topic_0000002480356602_p3338650145813"></a>AT^SLETX=78,5,255,0,0,2,3,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row163385502585"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p93381150135818"><a name="zh-cn_topic_0000002480356602_p93381150135818"></a><a name="zh-cn_topic_0000002480356602_p93381150135818"></a>2M 8PSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1675618197271"><a name="zh-cn_topic_0000002480356602_p1675618197271"></a><a name="zh-cn_topic_0000002480356602_p1675618197271"></a>2404</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p1133815502587"><a name="zh-cn_topic_0000002480356602_p1133815502587"></a><a name="zh-cn_topic_0000002480356602_p1133815502587"></a>AT^SLETX=2,5,255,0,1,2,3,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row533819503584"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p117561019112713"><a name="zh-cn_topic_0000002480356602_p117561019112713"></a><a name="zh-cn_topic_0000002480356602_p117561019112713"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p9338135011589"><a name="zh-cn_topic_0000002480356602_p9338135011589"></a><a name="zh-cn_topic_0000002480356602_p9338135011589"></a>AT^SLETX=38,5,255,0,1,2,3,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row83384503581"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p15756171918276"><a name="zh-cn_topic_0000002480356602_p15756171918276"></a><a name="zh-cn_topic_0000002480356602_p15756171918276"></a>2478</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p2338550145812"><a name="zh-cn_topic_0000002480356602_p2338550145812"></a><a name="zh-cn_topic_0000002480356602_p2338550145812"></a>AT^SLETX=76,5,255,0,1,2,3,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row123381750105811"><td class="cellrowborder" rowspan="3" valign="top" width="24.822482248224823%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p18338105016583"><a name="zh-cn_topic_0000002480356602_p18338105016583"></a><a name="zh-cn_topic_0000002480356602_p18338105016583"></a>4M 8PSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.182218221822183%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p875651982715"><a name="zh-cn_topic_0000002480356602_p875651982715"></a><a name="zh-cn_topic_0000002480356602_p875651982715"></a>2405</p>
</td>
<td class="cellrowborder" valign="top" width="52.995299529953%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p73391950205817"><a name="zh-cn_topic_0000002480356602_p73391950205817"></a><a name="zh-cn_topic_0000002480356602_p73391950205817"></a>AT^SLETX=3,5,255,0,4,2,3,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row8339105018584"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p12756111902710"><a name="zh-cn_topic_0000002480356602_p12756111902710"></a><a name="zh-cn_topic_0000002480356602_p12756111902710"></a>2441</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p733985065815"><a name="zh-cn_topic_0000002480356602_p733985065815"></a><a name="zh-cn_topic_0000002480356602_p733985065815"></a>AT^SLETX=39,5,255,0,4,2,3,3,2,50</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row23391050145813"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1975615197277"><a name="zh-cn_topic_0000002480356602_p1975615197277"></a><a name="zh-cn_topic_0000002480356602_p1975615197277"></a>2477</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p203399502589"><a name="zh-cn_topic_0000002480356602_p203399502589"></a><a name="zh-cn_topic_0000002480356602_p203399502589"></a>AT^SLETX=75,5,255,0,4,2,3,3,2,50</p>
</td>
</tr>
</tbody>
</table>

**表 6**  SLE RX组包

<a name="zh-cn_topic_0000002480356602_table594854518"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002480356602_row1110821314352"><th class="cellrowborder" valign="top" width="24.832483248324834%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002480356602_p022613167353"><a name="zh-cn_topic_0000002480356602_p022613167353"></a><a name="zh-cn_topic_0000002480356602_p022613167353"></a>调制速率</p>
</th>
<th class="cellrowborder" valign="top" width="22.922292229222922%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002480356602_p192262163352"><a name="zh-cn_topic_0000002480356602_p192262163352"></a><a name="zh-cn_topic_0000002480356602_p192262163352"></a>信道</p>
</th>
<th class="cellrowborder" valign="top" width="52.24522452245224%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002480356602_p6226121643518"><a name="zh-cn_topic_0000002480356602_p6226121643518"></a><a name="zh-cn_topic_0000002480356602_p6226121643518"></a>指令组包</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002480356602_row177419817451"><td class="cellrowborder" rowspan="3" valign="top" width="24.832483248324834%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1274198114519"><a name="zh-cn_topic_0000002480356602_p1274198114519"></a><a name="zh-cn_topic_0000002480356602_p1274198114519"></a>1M GFSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p8746816452"><a name="zh-cn_topic_0000002480356602_p8746816452"></a><a name="zh-cn_topic_0000002480356602_p8746816452"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.24522452245224%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p17741810456"><a name="zh-cn_topic_0000002480356602_p17741810456"></a><a name="zh-cn_topic_0000002480356602_p17741810456"></a>AT^SLERX=0,0,0,0,19</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1974888452"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p47520815459"><a name="zh-cn_topic_0000002480356602_p47520815459"></a><a name="zh-cn_topic_0000002480356602_p47520815459"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p177516820457"><a name="zh-cn_topic_0000002480356602_p177516820457"></a><a name="zh-cn_topic_0000002480356602_p177516820457"></a>AT^SLERX=38,0,0,0,19</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row375118154510"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p15752816459"><a name="zh-cn_topic_0000002480356602_p15752816459"></a><a name="zh-cn_topic_0000002480356602_p15752816459"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p37511874516"><a name="zh-cn_topic_0000002480356602_p37511874516"></a><a name="zh-cn_topic_0000002480356602_p37511874516"></a>AT^SLERX=78,0,0,0,19</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row3750814512"><td class="cellrowborder" rowspan="3" valign="top" width="24.832483248324834%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1275178174510"><a name="zh-cn_topic_0000002480356602_p1275178174510"></a><a name="zh-cn_topic_0000002480356602_p1275178174510"></a>2M GFSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p137511874515"><a name="zh-cn_topic_0000002480356602_p137511874515"></a><a name="zh-cn_topic_0000002480356602_p137511874515"></a>2404</p>
</td>
<td class="cellrowborder" valign="top" width="52.24522452245224%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p1375108144512"><a name="zh-cn_topic_0000002480356602_p1375108144512"></a><a name="zh-cn_topic_0000002480356602_p1375108144512"></a>AT^SLERX=2,1,0,0,10</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row117508124512"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p27518844512"><a name="zh-cn_topic_0000002480356602_p27518844512"></a><a name="zh-cn_topic_0000002480356602_p27518844512"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p6752815458"><a name="zh-cn_topic_0000002480356602_p6752815458"></a><a name="zh-cn_topic_0000002480356602_p6752815458"></a>AT^SLERX=38,0,0,0,10</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row187528144517"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p07538134511"><a name="zh-cn_topic_0000002480356602_p07538134511"></a><a name="zh-cn_topic_0000002480356602_p07538134511"></a>2478</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p57598154516"><a name="zh-cn_topic_0000002480356602_p57598154516"></a><a name="zh-cn_topic_0000002480356602_p57598154516"></a>AT^SLERX=76,1,0,0,10</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row197519812458"><td class="cellrowborder" rowspan="3" valign="top" width="24.832483248324834%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p9751584451"><a name="zh-cn_topic_0000002480356602_p9751584451"></a><a name="zh-cn_topic_0000002480356602_p9751584451"></a>4M GFSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1275208134512"><a name="zh-cn_topic_0000002480356602_p1275208134512"></a><a name="zh-cn_topic_0000002480356602_p1275208134512"></a>2405</p>
</td>
<td class="cellrowborder" valign="top" width="52.24522452245224%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p167514819451"><a name="zh-cn_topic_0000002480356602_p167514819451"></a><a name="zh-cn_topic_0000002480356602_p167514819451"></a>AT^SLERX=3,4,0,0,6</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row6752819456"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p127568154514"><a name="zh-cn_topic_0000002480356602_p127568154514"></a><a name="zh-cn_topic_0000002480356602_p127568154514"></a>2441</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1375118174511"><a name="zh-cn_topic_0000002480356602_p1375118174511"></a><a name="zh-cn_topic_0000002480356602_p1375118174511"></a>AT^SLERX=39,4,0,0,6</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row157538164511"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p87558164515"><a name="zh-cn_topic_0000002480356602_p87558164515"></a><a name="zh-cn_topic_0000002480356602_p87558164515"></a>2477</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p375887455"><a name="zh-cn_topic_0000002480356602_p375887455"></a><a name="zh-cn_topic_0000002480356602_p375887455"></a>AT^SLERX=75,4,0,0,6</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row47519812453"><td class="cellrowborder" rowspan="3" valign="top" width="24.832483248324834%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p187514854519"><a name="zh-cn_topic_0000002480356602_p187514854519"></a><a name="zh-cn_topic_0000002480356602_p187514854519"></a>1M QPSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p7759818455"><a name="zh-cn_topic_0000002480356602_p7759818455"></a><a name="zh-cn_topic_0000002480356602_p7759818455"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.24522452245224%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p17751384459"><a name="zh-cn_topic_0000002480356602_p17751384459"></a><a name="zh-cn_topic_0000002480356602_p17751384459"></a>AT^SLERX=0,0,2,3,14</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row17516814515"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p27588134512"><a name="zh-cn_topic_0000002480356602_p27588134512"></a><a name="zh-cn_topic_0000002480356602_p27588134512"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p127518894510"><a name="zh-cn_topic_0000002480356602_p127518894510"></a><a name="zh-cn_topic_0000002480356602_p127518894510"></a>AT^SLERX=38,0,2,3,14</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row17510854515"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p177538174516"><a name="zh-cn_topic_0000002480356602_p177538174516"></a><a name="zh-cn_topic_0000002480356602_p177538174516"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p275289454"><a name="zh-cn_topic_0000002480356602_p275289454"></a><a name="zh-cn_topic_0000002480356602_p275289454"></a>AT^SLERX=78,0,2,3,14</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row57558194511"><td class="cellrowborder" rowspan="3" valign="top" width="24.832483248324834%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p5751085455"><a name="zh-cn_topic_0000002480356602_p5751085455"></a><a name="zh-cn_topic_0000002480356602_p5751085455"></a>2M QPSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p177518813456"><a name="zh-cn_topic_0000002480356602_p177518813456"></a><a name="zh-cn_topic_0000002480356602_p177518813456"></a>2404</p>
</td>
<td class="cellrowborder" valign="top" width="52.24522452245224%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p14752814455"><a name="zh-cn_topic_0000002480356602_p14752814455"></a><a name="zh-cn_topic_0000002480356602_p14752814455"></a>AT^SLERX=2,1,2,3,8</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row4751811459"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p675138204517"><a name="zh-cn_topic_0000002480356602_p675138204517"></a><a name="zh-cn_topic_0000002480356602_p675138204517"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p975138104513"><a name="zh-cn_topic_0000002480356602_p975138104513"></a><a name="zh-cn_topic_0000002480356602_p975138104513"></a>AT^SLERX=38,1,2,3,8</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1075281451"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p207519820455"><a name="zh-cn_topic_0000002480356602_p207519820455"></a><a name="zh-cn_topic_0000002480356602_p207519820455"></a>2478</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p197515834518"><a name="zh-cn_topic_0000002480356602_p197515834518"></a><a name="zh-cn_topic_0000002480356602_p197515834518"></a>AT^SLERX=76,1,2,3,8</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row97519844514"><td class="cellrowborder" rowspan="3" valign="top" width="24.832483248324834%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p575589457"><a name="zh-cn_topic_0000002480356602_p575589457"></a><a name="zh-cn_topic_0000002480356602_p575589457"></a>4M QPSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p57513894515"><a name="zh-cn_topic_0000002480356602_p57513894515"></a><a name="zh-cn_topic_0000002480356602_p57513894515"></a>2405</p>
</td>
<td class="cellrowborder" valign="top" width="52.24522452245224%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p16755874517"><a name="zh-cn_topic_0000002480356602_p16755874517"></a><a name="zh-cn_topic_0000002480356602_p16755874517"></a>AT^SLERX=3,4,2,3,6</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1675788455"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1075187453"><a name="zh-cn_topic_0000002480356602_p1075187453"></a><a name="zh-cn_topic_0000002480356602_p1075187453"></a>2441</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1675198114518"><a name="zh-cn_topic_0000002480356602_p1675198114518"></a><a name="zh-cn_topic_0000002480356602_p1675198114518"></a>AT^SLERX=39,4,2,3,6</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row187512820451"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p57513824516"><a name="zh-cn_topic_0000002480356602_p57513824516"></a><a name="zh-cn_topic_0000002480356602_p57513824516"></a>2477</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p8752864514"><a name="zh-cn_topic_0000002480356602_p8752864514"></a><a name="zh-cn_topic_0000002480356602_p8752864514"></a>AT^SLERX=75,4,2,3,6</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row1075886457"><td class="cellrowborder" rowspan="3" valign="top" width="24.832483248324834%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1751389451"><a name="zh-cn_topic_0000002480356602_p1751389451"></a><a name="zh-cn_topic_0000002480356602_p1751389451"></a>1M 8PSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p127598174513"><a name="zh-cn_topic_0000002480356602_p127598174513"></a><a name="zh-cn_topic_0000002480356602_p127598174513"></a>2402</p>
</td>
<td class="cellrowborder" valign="top" width="52.24522452245224%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p6755818458"><a name="zh-cn_topic_0000002480356602_p6755818458"></a><a name="zh-cn_topic_0000002480356602_p6755818458"></a>AT^SLERX=0,0,2,3,10</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row975986457"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p127515844512"><a name="zh-cn_topic_0000002480356602_p127515844512"></a><a name="zh-cn_topic_0000002480356602_p127515844512"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p1762081456"><a name="zh-cn_topic_0000002480356602_p1762081456"></a><a name="zh-cn_topic_0000002480356602_p1762081456"></a>AT^SLERX=38,0,2,3,10</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row176188134516"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p1176989459"><a name="zh-cn_topic_0000002480356602_p1176989459"></a><a name="zh-cn_topic_0000002480356602_p1176989459"></a>2480</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p576158154516"><a name="zh-cn_topic_0000002480356602_p576158154516"></a><a name="zh-cn_topic_0000002480356602_p576158154516"></a>AT^SLERX=78,0,2,3,10</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row137618884514"><td class="cellrowborder" rowspan="3" valign="top" width="24.832483248324834%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p876188144517"><a name="zh-cn_topic_0000002480356602_p876188144517"></a><a name="zh-cn_topic_0000002480356602_p876188144517"></a>2M 8PSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p27614834519"><a name="zh-cn_topic_0000002480356602_p27614834519"></a><a name="zh-cn_topic_0000002480356602_p27614834519"></a>2404</p>
</td>
<td class="cellrowborder" valign="top" width="52.24522452245224%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p167616815457"><a name="zh-cn_topic_0000002480356602_p167616815457"></a><a name="zh-cn_topic_0000002480356602_p167616815457"></a>AT^SLERX=2,1,2,3,6</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row187617884510"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p876148174514"><a name="zh-cn_topic_0000002480356602_p876148174514"></a><a name="zh-cn_topic_0000002480356602_p876148174514"></a>2440</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p14766815456"><a name="zh-cn_topic_0000002480356602_p14766815456"></a><a name="zh-cn_topic_0000002480356602_p14766815456"></a>AT^SLERX=38,1,2,3,6</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row11765814511"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p117613817450"><a name="zh-cn_topic_0000002480356602_p117613817450"></a><a name="zh-cn_topic_0000002480356602_p117613817450"></a>2478</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p4761384456"><a name="zh-cn_topic_0000002480356602_p4761384456"></a><a name="zh-cn_topic_0000002480356602_p4761384456"></a>AT^SLERX=76,1,2,3,6</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row137613854519"><td class="cellrowborder" rowspan="3" valign="top" width="24.832483248324834%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p6764864511"><a name="zh-cn_topic_0000002480356602_p6764864511"></a><a name="zh-cn_topic_0000002480356602_p6764864511"></a>4M 8PSK</p>
</td>
<td class="cellrowborder" valign="top" width="22.922292229222922%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p137615834517"><a name="zh-cn_topic_0000002480356602_p137615834517"></a><a name="zh-cn_topic_0000002480356602_p137615834517"></a>2405</p>
</td>
<td class="cellrowborder" valign="top" width="52.24522452245224%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002480356602_p11760814510"><a name="zh-cn_topic_0000002480356602_p11760814510"></a><a name="zh-cn_topic_0000002480356602_p11760814510"></a>AT^SLERX=3,4,2,3,6</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row117612816452"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p12763819457"><a name="zh-cn_topic_0000002480356602_p12763819457"></a><a name="zh-cn_topic_0000002480356602_p12763819457"></a>2441</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p2767818454"><a name="zh-cn_topic_0000002480356602_p2767818454"></a><a name="zh-cn_topic_0000002480356602_p2767818454"></a>AT^SLERX=39,4,2,3,6</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002480356602_row3761782453"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002480356602_p6765810457"><a name="zh-cn_topic_0000002480356602_p6765810457"></a><a name="zh-cn_topic_0000002480356602_p6765810457"></a>2477</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002480356602_p4762854519"><a name="zh-cn_topic_0000002480356602_p4762854519"></a><a name="zh-cn_topic_0000002480356602_p4762854519"></a>AT^SLERX=75,4,2,3,6</p>
</td>
</tr>
</tbody>
</table>

# 注意事项<a name="ZH-CN_TOPIC_0000002516141483"></a>

-   **[除地址参数外，所有指令的参数输入格式必须为十进制](#ZH-CN_TOPIC_0000002483741546)**  

-   **[部分指令返回值为十六进制](#ZH-CN_TOPIC_0000002483901562)**  

-   **[所有参数的间隔符逗号为英文字符","](#ZH-CN_TOPIC_0000002516141513)**  

-   **[串口回复说明](#ZH-CN_TOPIC_0000002516141519)**  

-   **[星闪协议频点表](#ZH-CN_TOPIC_0000002483741604)**  

## 除地址参数外，所有指令的参数输入格式必须为十进制<a name="ZH-CN_TOPIC_0000002483741546"></a>

例如当包长0xFF需输入255时：

-   正确示例： AT^SLETX=0,5,255,0,1,2,2,0,0,50
-   错误示例： AT^SLETX=0,5,FF,0,1,2,2,0,0,50

>![](public_sys-resources/icon-note.gif) **说明：** 
>除地址参数外，其余参数必须是十进制。

## 部分指令返回值为十六进制<a name="ZH-CN_TOPIC_0000002483901562"></a>

-   发送：

    AT^SLETRXEND

-   回复：

    OK

    status:0x0, num\_packet:0x3e8, rssi:-127

>![](public_sys-resources/icon-note.gif) **说明：** 
>测试RX的收报数为1000包（0x3E8\)，RX的当前接收RSSI为-127dBm。

## 所有参数的间隔符逗号为英文字符","<a name="ZH-CN_TOPIC_0000002516141513"></a>

示例如下：

-   正确示例：AT^SLETX=0,5,255,0,1,2,2,0,0,50
-   错误示例：AT^SLETX=0，5，55,0,1,2,2,0,0,50

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >不识别中文字符逗号。

## 串口回复说明<a name="ZH-CN_TOPIC_0000002516141519"></a>

-   TRX射频指令测试时，串口回复重点关注“status:0“、“num\_packets:3e8“、“rssi:-127“等字样。
-   由于指令执行时序异步，“OK”和其他业务上报打印的先后顺序不是固定的，且涉及BT/BLE/SLE TRX业务的指令，回复“OK”不代表指令正常执行。

## 星闪协议频点表<a name="ZH-CN_TOPIC_0000002483741604"></a>

星闪协议频点如[图1](#zh-cn_topic_0000002512316475_fig12728841183211)和[图2](#zh-cn_topic_0000002512316475_fig3784926143510)所示。

**图 1**  2400MHz频段不同带宽信道对应的射频信道中心频率和射频信道号1<a name="zh-cn_topic_0000002512316475_fig12728841183211"></a>  
![](figures/2400MHz频段不同带宽信道对应的射频信道中心频率和射频信道号1.png "2400MHz频段不同带宽信道对应的射频信道中心频率和射频信道号1")

**图 2**  2400MHz频段不同带宽信道对应的射频信道中心频率和射频信道号2<a name="zh-cn_topic_0000002512316475_fig3784926143510"></a>  
![](figures/2400MHz频段不同带宽信道对应的射频信道中心频率和射频信道号2.png "2400MHz频段不同带宽信道对应的射频信道中心频率和射频信道号2")

>![](public_sys-resources/icon-note.gif) **说明：** 
>不同带宽的射频测试项需要测的频点不同，例如SLE 4M GFSK指令，测试低频的频点为2405MHz，而不是2402MHz。

