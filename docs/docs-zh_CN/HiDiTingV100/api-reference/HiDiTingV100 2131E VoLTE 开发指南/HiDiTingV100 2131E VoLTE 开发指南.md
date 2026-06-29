# 前言<a name="ZH-CN_TOPIC_0000002223228706"></a>

**概述<a name="zh-cn_topic_0000002175160621_section4537382116410"></a>**

本文档主要介绍通过主控芯片和Hi2131E搭配，实现VoLTE功能的开发相关内容，包括SDK软硬件架构、驱动&接口、系统管理、协议软件和应用软件等，简要描述了VoLTE各接口、功能模块的功能和工作原理，通过实例按场景介绍各接口和功能模块的开发过程及注意事项。

**读者对象<a name="zh-cn_topic_0000002175160621_section4378592816410"></a>**

本文档主要适用于以下工程师：

-   技术支持工程师
-   软件开发工程师

**符号约定<a name="zh-cn_topic_0000002175160621_section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="zh-cn_topic_0000002175160621_table2622507016410"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175160621_row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002175160621_p6450074116410"><a name="zh-cn_topic_0000002175160621_p6450074116410"></a><a name="zh-cn_topic_0000002175160621_p6450074116410"></a><strong id="zh-cn_topic_0000002175160621_b2136615816410"><a name="zh-cn_topic_0000002175160621_b2136615816410"></a><a name="zh-cn_topic_0000002175160621_b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002175160621_p5435366816410"><a name="zh-cn_topic_0000002175160621_p5435366816410"></a><a name="zh-cn_topic_0000002175160621_p5435366816410"></a><strong id="zh-cn_topic_0000002175160621_b5941558116410"><a name="zh-cn_topic_0000002175160621_b5941558116410"></a><a name="zh-cn_topic_0000002175160621_b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175160621_row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175160621_p3734547016410"><a name="zh-cn_topic_0000002175160621_p3734547016410"></a><a name="zh-cn_topic_0000002175160621_p3734547016410"></a><a name="zh-cn_topic_0000002175160621_image2670064316410"></a><a name="zh-cn_topic_0000002175160621_image2670064316410"></a><span><img class="" id="zh-cn_topic_0000002175160621_image2670064316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002339469521.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175160621_p1757432116410"><a name="zh-cn_topic_0000002175160621_p1757432116410"></a><a name="zh-cn_topic_0000002175160621_p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160621_row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175160621_p1432579516410"><a name="zh-cn_topic_0000002175160621_p1432579516410"></a><a name="zh-cn_topic_0000002175160621_p1432579516410"></a><a name="zh-cn_topic_0000002175160621_image4895582316410"></a><a name="zh-cn_topic_0000002175160621_image4895582316410"></a><span><img class="" id="zh-cn_topic_0000002175160621_image4895582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002339589361.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175160621_p959197916410"><a name="zh-cn_topic_0000002175160621_p959197916410"></a><a name="zh-cn_topic_0000002175160621_p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160621_row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175160621_p1232579516410"><a name="zh-cn_topic_0000002175160621_p1232579516410"></a><a name="zh-cn_topic_0000002175160621_p1232579516410"></a><a name="zh-cn_topic_0000002175160621_image1235582316410"></a><a name="zh-cn_topic_0000002175160621_image1235582316410"></a><span><img class="" id="zh-cn_topic_0000002175160621_image1235582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002305630058.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175160621_p123197916410"><a name="zh-cn_topic_0000002175160621_p123197916410"></a><a name="zh-cn_topic_0000002175160621_p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160621_row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175160621_p2204984716410"><a name="zh-cn_topic_0000002175160621_p2204984716410"></a><a name="zh-cn_topic_0000002175160621_p2204984716410"></a><a name="zh-cn_topic_0000002175160621_image4504446716410"></a><a name="zh-cn_topic_0000002175160621_image4504446716410"></a><span><img class="" id="zh-cn_topic_0000002175160621_image4504446716410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002305470338.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175160621_p4388861916410"><a name="zh-cn_topic_0000002175160621_p4388861916410"></a><a name="zh-cn_topic_0000002175160621_p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="zh-cn_topic_0000002175160621_p1238861916410"><a name="zh-cn_topic_0000002175160621_p1238861916410"></a><a name="zh-cn_topic_0000002175160621_p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160621_row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175160621_p5555360116410"><a name="zh-cn_topic_0000002175160621_p5555360116410"></a><a name="zh-cn_topic_0000002175160621_p5555360116410"></a><a name="zh-cn_topic_0000002175160621_image799324016410"></a><a name="zh-cn_topic_0000002175160621_image799324016410"></a><span><img class="" id="zh-cn_topic_0000002175160621_image799324016410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002339469525.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175160621_p4612588116410"><a name="zh-cn_topic_0000002175160621_p4612588116410"></a><a name="zh-cn_topic_0000002175160621_p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="zh-cn_topic_0000002175160621_p1232588116410"><a name="zh-cn_topic_0000002175160621_p1232588116410"></a><a name="zh-cn_topic_0000002175160621_p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# 概述<a name="ZH-CN_TOPIC_0000002223508004"></a>

-   **[VoLTE架构说明](#ZH-CN_TOPIC_0000002223667848)**  

-   **[开发规范和约束](#ZH-CN_TOPIC_0000002258467613)**  

## VoLTE架构说明<a name="ZH-CN_TOPIC_0000002223667848"></a>

VoLTE（Voice over LTE，长期演进语音承载）是3GPP标准定义的，基于IMS网络的LTE语音解决方案。通过IMS网络，移动运营商不仅可以无缝的继承传统的语音、短消息业务，还可以将语音通话与丰富的增强功能相整合，提供多样化的服务。本章节描述VoLTE场景下主控芯片和Hi2131E芯片如何实现软件及硬件的二次开发，以最终完成通话业务。

Hi2131E作为支持4G Cat1的Modem蜂窝芯片，提供4G入网数传能力。主控芯片作为穿戴芯片，可提供语音编解码的处理能力。二者配合成为的VoLTE终端，如[图1](#zh-cn_topic_0000002139919432_fig18919270253)所示。

**图 1**  VoLTE系统架构图<a name="zh-cn_topic_0000002139919432_fig18919270253"></a>  
![](figures/VoLTE系统架构图.png "VoLTE系统架构图")

IMS协议栈（VoLTE基础功能的协议栈）主体代码位于主控芯片，Hi2131E侧支持4G入网功能。和Hi2131E之间存在UART通路和SPI数据传输通路，UART通路传输芯片间的控制指令，SPI通路传输芯片间的数据信息。

**表 1**  VoLTE系统模块描述

<a name="zh-cn_topic_0000002139919432_table1089771605519"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002139919432_row16897151685518"><th class="cellrowborder" valign="top" width="22.63%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002139919432_p108972016145513"><a name="zh-cn_topic_0000002139919432_p108972016145513"></a><a name="zh-cn_topic_0000002139919432_p108972016145513"></a>组件名称</p>
</th>
<th class="cellrowborder" valign="top" width="17.78%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002139919432_p1897716205512"><a name="zh-cn_topic_0000002139919432_p1897716205512"></a><a name="zh-cn_topic_0000002139919432_p1897716205512"></a>归属</p>
</th>
<th class="cellrowborder" valign="top" width="59.589999999999996%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002139919432_p8684194465513"><a name="zh-cn_topic_0000002139919432_p8684194465513"></a><a name="zh-cn_topic_0000002139919432_p8684194465513"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002139919432_row17897101612556"><td class="cellrowborder" valign="top" width="22.63%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002139919432_p148971016115519"><a name="zh-cn_topic_0000002139919432_p148971016115519"></a><a name="zh-cn_topic_0000002139919432_p148971016115519"></a>IMS协议栈</p>
</td>
<td class="cellrowborder" valign="top" width="17.78%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002139919432_p28971316205514"><a name="zh-cn_topic_0000002139919432_p28971316205514"></a><a name="zh-cn_topic_0000002139919432_p28971316205514"></a>主控芯片</p>
</td>
<td class="cellrowborder" valign="top" width="59.589999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002139919432_p196841744135519"><a name="zh-cn_topic_0000002139919432_p196841744135519"></a><a name="zh-cn_topic_0000002139919432_p196841744135519"></a>通过“libims.a”文件，集成编译在主控芯片 SDK版本中。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002139919432_row168971316155517"><td class="cellrowborder" valign="top" width="22.63%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002139919432_p8897101617551"><a name="zh-cn_topic_0000002139919432_p8897101617551"></a><a name="zh-cn_topic_0000002139919432_p8897101617551"></a>RIL接口</p>
</td>
<td class="cellrowborder" valign="top" width="17.78%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002139919432_p989771605519"><a name="zh-cn_topic_0000002139919432_p989771605519"></a><a name="zh-cn_topic_0000002139919432_p989771605519"></a>主控芯片</p>
</td>
<td class="cellrowborder" valign="top" width="59.589999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002139919432_p16684844105510"><a name="zh-cn_topic_0000002139919432_p16684844105510"></a><a name="zh-cn_topic_0000002139919432_p16684844105510"></a>需要通过主控芯片侧RIL头文件调用。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002139919432_row430024129"><td class="cellrowborder" valign="top" width="22.63%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002139919432_p133001141213"><a name="zh-cn_topic_0000002139919432_p133001141213"></a><a name="zh-cn_topic_0000002139919432_p133001141213"></a>VoLTE相关AT命令</p>
</td>
<td class="cellrowborder" valign="top" width="17.78%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002139919432_p630018412216"><a name="zh-cn_topic_0000002139919432_p630018412216"></a><a name="zh-cn_topic_0000002139919432_p630018412216"></a>Hi2131E</p>
</td>
<td class="cellrowborder" valign="top" width="59.589999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002139919432_p9300141128"><a name="zh-cn_topic_0000002139919432_p9300141128"></a><a name="zh-cn_topic_0000002139919432_p9300141128"></a>VoLTE相关AT命令功能，需要通过接入Hi2131E侧的UART串口才可进行发送。</p>
</td>
</tr>
</tbody>
</table>

当前VoLTE终端提供的自能力如[表2](#zh-cn_topic_0000002139919432_table867758163013)所示。

**表 2**  IMS子系统模块划分

<a name="zh-cn_topic_0000002139919432_table867758163013"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002139919432_row46781587301"><th class="cellrowborder" valign="top" width="26.19%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002139919432_p198717404300"><a name="zh-cn_topic_0000002139919432_p198717404300"></a><a name="zh-cn_topic_0000002139919432_p198717404300"></a>VoLTE功能分类</p>
</th>
<th class="cellrowborder" valign="top" width="73.81%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002139919432_p15987144013018"><a name="zh-cn_topic_0000002139919432_p15987144013018"></a><a name="zh-cn_topic_0000002139919432_p15987144013018"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002139919432_row66781684307"><td class="cellrowborder" valign="top" width="26.19%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002139919432_p1359713920102"><a name="zh-cn_topic_0000002139919432_p1359713920102"></a><a name="zh-cn_topic_0000002139919432_p1359713920102"></a><span>基本音频呼叫</span></p>
</td>
<td class="cellrowborder" valign="top" width="73.81%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002139919432_p1459769161019"><a name="zh-cn_topic_0000002139919432_p1459769161019"></a><a name="zh-cn_topic_0000002139919432_p1459769161019"></a><span>支持</span>。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002139919432_row2678198193010"><td class="cellrowborder" valign="top" width="26.19%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002139919432_p85976921011"><a name="zh-cn_topic_0000002139919432_p85976921011"></a><a name="zh-cn_topic_0000002139919432_p85976921011"></a><span>有卡紧急呼叫</span></p>
</td>
<td class="cellrowborder" valign="top" width="73.81%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002139919432_p155972910105"><a name="zh-cn_topic_0000002139919432_p155972910105"></a><a name="zh-cn_topic_0000002139919432_p155972910105"></a><span>支持</span>。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002139919432_row95701223153015"><td class="cellrowborder" valign="top" width="26.19%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002139919432_p12390421131017"><a name="zh-cn_topic_0000002139919432_p12390421131017"></a><a name="zh-cn_topic_0000002139919432_p12390421131017"></a><span>呼叫方式</span></p>
</td>
<td class="cellrowborder" valign="top" width="73.81%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002139919432_p439019211106"><a name="zh-cn_topic_0000002139919432_p439019211106"></a><a name="zh-cn_topic_0000002139919432_p439019211106"></a><span>支持</span><span>Precondition</span><span>资源预留</span>。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002139919432_row16678158143017"><td class="cellrowborder" valign="top" width="26.19%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002139919432_p1339013219103"><a name="zh-cn_topic_0000002139919432_p1339013219103"></a><a name="zh-cn_topic_0000002139919432_p1339013219103"></a><span>短信主被叫</span></p>
</td>
<td class="cellrowborder" valign="top" width="73.81%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002139919432_p11390102181015"><a name="zh-cn_topic_0000002139919432_p11390102181015"></a><a name="zh-cn_topic_0000002139919432_p11390102181015"></a><span>支持</span>。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002139919432_row0678158123016"><td class="cellrowborder" valign="top" width="26.19%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002139919432_p53901221191019"><a name="zh-cn_topic_0000002139919432_p53901221191019"></a><a name="zh-cn_topic_0000002139919432_p53901221191019"></a><span>语音编解码</span></p>
</td>
<td class="cellrowborder" valign="top" width="73.81%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002139919432_p439062161016"><a name="zh-cn_topic_0000002139919432_p439062161016"></a><a name="zh-cn_topic_0000002139919432_p439062161016"></a><span>仅支持</span><span>AMR-WB（mode-set=0-8）</span><span>编解码</span>。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002139919432_row55841927181015"><td class="cellrowborder" valign="top" width="26.19%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002139919432_p1778974315102"><a name="zh-cn_topic_0000002139919432_p1778974315102"></a><a name="zh-cn_topic_0000002139919432_p1778974315102"></a><span>彩铃/漫游</span></p>
</td>
<td class="cellrowborder" valign="top" width="73.81%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002139919432_p858417279108"><a name="zh-cn_topic_0000002139919432_p858417279108"></a><a name="zh-cn_topic_0000002139919432_p858417279108"></a><span>支持（</span><span>IMS</span><span>网络需支持</span>)。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002139919432_row2511203710103"><td class="cellrowborder" valign="top" width="26.19%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002139919432_p57318191216"><a name="zh-cn_topic_0000002139919432_p57318191216"></a><a name="zh-cn_topic_0000002139919432_p57318191216"></a><span>SRVCC</span></p>
</td>
<td class="cellrowborder" valign="top" width="73.81%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002139919432_p1545181521212"><a name="zh-cn_topic_0000002139919432_p1545181521212"></a><a name="zh-cn_topic_0000002139919432_p1545181521212"></a><span>不支持，仅支持</span><span>4G</span><span>网络，无法切换</span>回落。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002139919432_row1146433111104"><td class="cellrowborder" valign="top" width="26.19%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002139919432_p09601011201415"><a name="zh-cn_topic_0000002139919432_p09601011201415"></a><a name="zh-cn_topic_0000002139919432_p09601011201415"></a><span>呼叫转移</span></p>
</td>
<td class="cellrowborder" valign="top" width="73.81%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002139919432_p596014115146"><a name="zh-cn_topic_0000002139919432_p596014115146"></a><a name="zh-cn_topic_0000002139919432_p596014115146"></a><span>不支持</span>。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002139919432_row021357151419"><td class="cellrowborder" valign="top" width="26.19%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002139919432_p896011119148"><a name="zh-cn_topic_0000002139919432_p896011119148"></a><a name="zh-cn_topic_0000002139919432_p896011119148"></a><span>呼叫保持</span></p>
</td>
<td class="cellrowborder" valign="top" width="73.81%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002139919432_p696071111411"><a name="zh-cn_topic_0000002139919432_p696071111411"></a><a name="zh-cn_topic_0000002139919432_p696071111411"></a><span>不支持</span>。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002139919432_row6266151131110"><td class="cellrowborder" valign="top" width="26.19%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002139919432_p1996018117145"><a name="zh-cn_topic_0000002139919432_p1996018117145"></a><a name="zh-cn_topic_0000002139919432_p1996018117145"></a><span>三方会议</span></p>
</td>
<td class="cellrowborder" valign="top" width="73.81%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002139919432_p1296061141415"><a name="zh-cn_topic_0000002139919432_p1296061141415"></a><a name="zh-cn_topic_0000002139919432_p1296061141415"></a><span>不支持</span>。</p>
</td>
</tr>
</tbody>
</table>

## 开发规范和约束<a name="ZH-CN_TOPIC_0000002258467613"></a>

仅在主控芯片的diting版本下支持和Hi2131E芯片的对接调试。

# 环境搭建<a name="ZH-CN_TOPIC_0000002258627513"></a>

VoLTE开发包含Hi2131E侧和主控芯片侧两个部分，两个系统分别独立构建版本，通过UART、SPI、GPIO等连线建立通讯。

-   **[编译构建](#ZH-CN_TOPIC_0000002223508012)**  

-   **[DiTing单板外接连线说明](#ZH-CN_TOPIC_0000002223508016)**  

-   **[VoLTE拨号通话流程](#ZH-CN_TOPIC_0000002223508020)**  

## 编译构建<a name="ZH-CN_TOPIC_0000002223508012"></a>

-   **[Hi2131E侧版本编译](#ZH-CN_TOPIC_0000002258627521)**  

### Hi2131E侧版本编译<a name="ZH-CN_TOPIC_0000002258627521"></a>

当前版本中，Hi2131E单板不提供二次开发界面和接口，功能接口和AT命令均已实现。软件版本已经烧录至单板中，直接使用即可。

## DiTing单板外接连线说明<a name="ZH-CN_TOPIC_0000002223508016"></a>

现DiTing单板已将主控芯片与Hi2131E芯片整合在同一单板上，以下对该单板外接串口小板以及连线进行说明。

**表 1**  DiTing单板外接连线说明

<a name="table108351427105111"></a>
<table><thead align="left"><tr id="row58359275513"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="p4835122795110"><a name="p4835122795110"></a><a name="p4835122795110"></a>DiTing单板模块</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="p1883512795111"><a name="p1883512795111"></a><a name="p1883512795111"></a>连接外设</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="p583522725115"><a name="p583522725115"></a><a name="p583522725115"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row1183582713511"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="p1983512785114"><a name="p1983512785114"></a><a name="p1983512785114"></a>HSUART</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="p14835152795112"><a name="p14835152795112"></a><a name="p14835152795112"></a>串口小板</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p13835162715116"><a name="p13835162715116"></a><a name="p13835162715116"></a>HSUART1、2、3分别连接串口小板TX、RX、GND。</p>
<p id="p2092313381308"><a name="p2092313381308"></a><a name="p2092313381308"></a>负责主控芯片版本烧录。</p>
</td>
</tr>
<tr id="row1683522785117"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="p11835122719516"><a name="p11835122719516"></a><a name="p11835122719516"></a>UART2</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="p5835427205111"><a name="p5835427205111"></a><a name="p5835427205111"></a>串口小板</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p321820222302"><a name="p321820222302"></a><a name="p321820222302"></a>UART2的1、2、3分别连接串口小板TX、RX、GND。</p>
<p id="p138511758203020"><a name="p138511758203020"></a><a name="p138511758203020"></a>负责AT口打印。</p>
</td>
</tr>
<tr id="row1783532775112"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="p15836327195117"><a name="p15836327195117"></a><a name="p15836327195117"></a>USB</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="p1183682765113"><a name="p1183682765113"></a><a name="p1183682765113"></a>Micro USB线</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p18836927205120"><a name="p18836927205120"></a><a name="p18836927205120"></a>负责Hi2131E的烧录以及AT日志。</p>
</td>
</tr>
<tr id="row1397872915615"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="p18978132995615"><a name="p18978132995615"></a><a name="p18978132995615"></a>USB-C</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="p49781629105615"><a name="p49781629105615"></a><a name="p49781629105615"></a>USB TYPE C</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p697812914563"><a name="p697812914563"></a><a name="p697812914563"></a>电源线。</p>
</td>
</tr>
</tbody>
</table>

## VoLTE拨号通话流程<a name="ZH-CN_TOPIC_0000002223508020"></a>

Hi2131E为实现正常的入网功能，需要一张可通过核心网鉴权的USIM卡。

-   **[通过AT命令拨打电话](#ZH-CN_TOPIC_0000002223667872)**  

-   **[通过RIL接口拨打电话](#ZH-CN_TOPIC_0000002258467637)**  

### 通过AT命令拨打电话<a name="ZH-CN_TOPIC_0000002223667872"></a>

AT命令模块在Hi2131E侧，通过在Hi2131E侧调用相关AT命令，可以实现VoLTE通话。

**主叫：**

1.  开机。

    AT+CFUN=1

    预期返回：OK

2.  发起注册。

    AT+CGATT=1

    预期返回：OK

    IMSSRVSTATUS：0，2（请参见“[IMS服务状态更新指示：+IMSSRVSTATUS](#ZH-CN_TOPIC_0000002258467693)”章节。）

3.  拨打电话。

    ATDxxx；

    预期返回：OK

    +ORIG: 1,0（请参见“[发起语音呼叫指示：+ORIG](#ZH-CN_TOPIC_0000002223508080)”章节。）

    +CCALLSTATE: 1,0,1（请参见“[呼叫状态上报：+CALLSTATE](#ZH-CN_TOPIC_0000002223508084)”章节。）

    +CCALLSTATE: 1,1,1（收到网侧100trying）

    +CONF: 1（请参见“[UE收到网络指示：+CONF](#ZH-CN_TOPIC_0000002223667920)”章节。）

    +CHANNELINFO: 0,1（请参见“[主动上报语音带宽信息：+CHANNELINFO](#ZH-CN_TOPIC_0000002223667924)”章节。）

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >无带内音信息，需本地回铃音上报。此时需要自己制作铃声。

    +CCALLSTATE: 1,2,1（收到180ringing。）

    IRING（请参见“[来电指示：IRING](#ZH-CN_TOPIC_0000002258627577)”章节。）

    +CCALLSTATE: 1,3,1（收到invite 200OK，呼叫已经接通。）

    +CONN: 1,0（请参见“[呼叫接通指示：+CONN](#ZH-CN_TOPIC_0000002258467697)”章节。）

4.  挂断电话。

    ATH

    ![](figures/zh-cn_image_0000002333786913.png)预期返回：OK

    +CCALLSTATE: 1,4,1（通话结束）

    +CEND: 1,100,0（请参见“[通话结束指示：+CEND](#ZH-CN_TOPIC_0000002258627581)”章节。）

**被叫**：

1.  开机。

    AT+CFUN=1

    预期返回：OK

2.  发起注册。

    AT+CGATT=1

    预期返回：OK

    IMSSRVSTATUS：0，2（请参见“[IMS服务状态更新指示：+IMSSRVSTATUS](#ZH-CN_TOPIC_0000002258467693)”章节。）

3.  接听。

    手动接听：ATA（请参见“[接听来电：A](#ZH-CN_TOPIC_0000002258627569)”章节。）

    自动接听：ATS0=x（请参见[设置自动接听：S0](#ZH-CN_TOPIC_0000002223667912)”章节。）

    预期返回：OK

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >自动接听命令建议在注册成功后执行，也可以在开机后执行。
    >手动接听则是在收到有来电提示（+CALLSTATE: 1,5,1）后执行。

4.  挂断电话。

    ATH

    ![](figures/zh-cn_image_0000002299947570.png)预期返回：OK

    +CCALLSTATE: 1,4,1（通话结束）

    +CEND: 1,100,0

### 通过RIL接口拨打电话<a name="ZH-CN_TOPIC_0000002258467637"></a>

RIL接口存在于主控芯片侧，通过在调用相关RIL接口，可以实现VoLTE通话。

**主叫：**

1.  开机。

    调用ril\_at\_cmd\_sync\_get\_info接口（请参见“[同步AT命令发送](#ZH-CN_TOPIC_0000002223508068)”章节）。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >AT命令：AT+CFUN=1
    >预期返回结果：OK

2.  注册入网。

    调用ril\_at\_cmd\_sync\_get\_info接口。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >AT命令：AT+CGATT=1
    >预期返回结果：OK
    >IMSSRVSTATUS：0，2

3.  拨打电话。

    调用ril\_set\_call\_orig\_req接口（请参见“[拨打电话](#ZH-CN_TOPIC_0000002258627557)”章节）。

4.  挂断电话。

    调用ril\_set\_call\_rel\_req接口（请参见“[挂断电话](#ZH-CN_TOPIC_0000002223667896)”章节）。

**被叫：**

1.  开机。

    调用ril\_at\_cmd\_sync\_get\_info接口（请参见“[同步AT命令发送](#ZH-CN_TOPIC_0000002223508068)”章节）。

2.  注册入网。

    调用ril\_at\_cmd\_sync\_get\_info接口。

3.  手动接听。

    调用ril\_set\_call\_mt\_accept\_req接口（请参见“[手动接听电话](#ZH-CN_TOPIC_0000002223508064)”章节）。

4.  自动接听。

    调用ril\_set\_auto\_answer接口（请参见“[设置自动接听](#ZH-CN_TOPIC_0000002258467677)”章节）。

5.  挂断电话。

    调用ril\_set\_call\_rel\_req接口（请参见“[挂断电话](#ZH-CN_TOPIC_0000002223667896)”章节）。

# 对外接口描述<a name="ZH-CN_TOPIC_0000002258627529"></a>

**表 1**  RIL接口实现方式

<a name="zh-cn_topic_0000002175160637_table10902934434"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175160637_row1095115344318"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002175160637_p1195116345310"><a name="zh-cn_topic_0000002175160637_p1195116345310"></a><a name="zh-cn_topic_0000002175160637_p1195116345310"></a>一级菜单</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002175160637_p1995163415313"><a name="zh-cn_topic_0000002175160637_p1995163415313"></a><a name="zh-cn_topic_0000002175160637_p1995163415313"></a>二级菜单</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002175160637_p1195119341837"><a name="zh-cn_topic_0000002175160637_p1195119341837"></a><a name="zh-cn_topic_0000002175160637_p1195119341837"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002175160637_p199516341637"><a name="zh-cn_topic_0000002175160637_p199516341637"></a><a name="zh-cn_topic_0000002175160637_p199516341637"></a>实现方式</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175160637_row19511034534"><td class="cellrowborder" rowspan="2" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002175160637_p19951234436"><a name="zh-cn_topic_0000002175160637_p19951234436"></a><a name="zh-cn_topic_0000002175160637_p19951234436"></a>设备信息</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002175160637_p13951193418314"><a name="zh-cn_topic_0000002175160637_p13951193418314"></a><a name="zh-cn_topic_0000002175160637_p13951193418314"></a>软件版本号</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002175160637_p895113341133"><a name="zh-cn_topic_0000002175160637_p895113341133"></a><a name="zh-cn_topic_0000002175160637_p895113341133"></a>获取Modem的软件版本信息。</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002175160637_p99519349315"><a name="zh-cn_topic_0000002175160637_p99519349315"></a><a name="zh-cn_topic_0000002175160637_p99519349315"></a>核内交互</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160637_row295119341313"><td class="cellrowborder" valign="top" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002175160637_p495110347315"><a name="zh-cn_topic_0000002175160637_p495110347315"></a><a name="zh-cn_topic_0000002175160637_p495110347315"></a>IMEI设备识别码</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002175160637_p14951134138"><a name="zh-cn_topic_0000002175160637_p14951134138"></a><a name="zh-cn_topic_0000002175160637_p14951134138"></a>获取Modem设备的识别码。</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002175160637_p129511434036"><a name="zh-cn_topic_0000002175160637_p129511434036"></a><a name="zh-cn_topic_0000002175160637_p129511434036"></a>核内交互</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160637_row189528341031"><td class="cellrowborder" rowspan="3" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002175160637_p59524341033"><a name="zh-cn_topic_0000002175160637_p59524341033"></a><a name="zh-cn_topic_0000002175160637_p59524341033"></a>(U)SIM卡信息</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002175160637_p1895273412310"><a name="zh-cn_topic_0000002175160637_p1895273412310"></a><a name="zh-cn_topic_0000002175160637_p1895273412310"></a>是否在位，Pin-block，Puk-block</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002175160637_p89522347314"><a name="zh-cn_topic_0000002175160637_p89522347314"></a><a name="zh-cn_topic_0000002175160637_p89522347314"></a>获取Modem (U)SIM卡是否在位的信息，Pin码和PUK码是否锁定状态。</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002175160637_p79526341638"><a name="zh-cn_topic_0000002175160637_p79526341638"></a><a name="zh-cn_topic_0000002175160637_p79526341638"></a>核内交互</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160637_row895220341534"><td class="cellrowborder" valign="top" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002175160637_p395215343318"><a name="zh-cn_topic_0000002175160637_p395215343318"></a><a name="zh-cn_topic_0000002175160637_p395215343318"></a>IMSI</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002175160637_p8952153415320"><a name="zh-cn_topic_0000002175160637_p8952153415320"></a><a name="zh-cn_topic_0000002175160637_p8952153415320"></a>获取 (U)SIM卡的IMSI信息。</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002175160637_p1395211346316"><a name="zh-cn_topic_0000002175160637_p1395211346316"></a><a name="zh-cn_topic_0000002175160637_p1395211346316"></a>核内交互</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160637_row29525341732"><td class="cellrowborder" valign="top" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002175160637_p1395213347315"><a name="zh-cn_topic_0000002175160637_p1395213347315"></a><a name="zh-cn_topic_0000002175160637_p1395213347315"></a>本机号码显示</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002175160637_p1895219344318"><a name="zh-cn_topic_0000002175160637_p1895219344318"></a><a name="zh-cn_topic_0000002175160637_p1895219344318"></a>获取保存在 (U)SIM中的本机号码。</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002175160637_p1995219349315"><a name="zh-cn_topic_0000002175160637_p1995219349315"></a><a name="zh-cn_topic_0000002175160637_p1995219349315"></a>核内交互</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160637_row1495263417317"><td class="cellrowborder" rowspan="5" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002175160637_p109524341033"><a name="zh-cn_topic_0000002175160637_p109524341033"></a><a name="zh-cn_topic_0000002175160637_p109524341033"></a>驻网状态</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002175160637_p1695293416317"><a name="zh-cn_topic_0000002175160637_p1695293416317"></a><a name="zh-cn_topic_0000002175160637_p1695293416317"></a>是否驻网</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002175160637_p59524342313"><a name="zh-cn_topic_0000002175160637_p59524342313"></a><a name="zh-cn_topic_0000002175160637_p59524342313"></a>获取终端是否已经驻网。</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002175160637_p1895219349316"><a name="zh-cn_topic_0000002175160637_p1895219349316"></a><a name="zh-cn_topic_0000002175160637_p1895219349316"></a>核内交互</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160637_row695253416315"><td class="cellrowborder" valign="top" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002175160637_p1952143418315"><a name="zh-cn_topic_0000002175160637_p1952143418315"></a><a name="zh-cn_topic_0000002175160637_p1952143418315"></a>显示运营商名称</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002175160637_p29527341831"><a name="zh-cn_topic_0000002175160637_p29527341831"></a><a name="zh-cn_topic_0000002175160637_p29527341831"></a>获取终端驻网的运营商信息。</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002175160637_p695243413312"><a name="zh-cn_topic_0000002175160637_p695243413312"></a><a name="zh-cn_topic_0000002175160637_p695243413312"></a>核内交互</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160637_row14952133417312"><td class="cellrowborder" valign="top" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002175160637_p295263419318"><a name="zh-cn_topic_0000002175160637_p295263419318"></a><a name="zh-cn_topic_0000002175160637_p295263419318"></a>是否IMS（VoLTE）驻网</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002175160637_p99527340312"><a name="zh-cn_topic_0000002175160637_p99527340312"></a><a name="zh-cn_topic_0000002175160637_p99527340312"></a>获取终端是否IMS驻网信息。</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002175160637_p89521734731"><a name="zh-cn_topic_0000002175160637_p89521734731"></a><a name="zh-cn_topic_0000002175160637_p89521734731"></a>核内交互</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160637_row159521534333"><td class="cellrowborder" valign="top" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002175160637_p189521234137"><a name="zh-cn_topic_0000002175160637_p189521234137"></a><a name="zh-cn_topic_0000002175160637_p189521234137"></a>是否PDP驻网，显示当前IP地址</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002175160637_p18952134931"><a name="zh-cn_topic_0000002175160637_p18952134931"></a><a name="zh-cn_topic_0000002175160637_p18952134931"></a>获取终端是否PDP驻网，并显示IP地址。</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002175160637_p1695216341738"><a name="zh-cn_topic_0000002175160637_p1695216341738"></a><a name="zh-cn_topic_0000002175160637_p1695216341738"></a>核内交互</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160637_row79527345314"><td class="cellrowborder" valign="top" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002175160637_p19521734136"><a name="zh-cn_topic_0000002175160637_p19521734136"></a><a name="zh-cn_topic_0000002175160637_p19521734136"></a>手动搜网，并显示可选网络</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002175160637_p795223419311"><a name="zh-cn_topic_0000002175160637_p795223419311"></a><a name="zh-cn_topic_0000002175160637_p795223419311"></a>触发手动搜网，并返回搜网结果。</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002175160637_p59521334238"><a name="zh-cn_topic_0000002175160637_p59521334238"></a><a name="zh-cn_topic_0000002175160637_p59521334238"></a>核间通信</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160637_row11953133419316"><td class="cellrowborder" rowspan="2" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002175160637_p1595317341137"><a name="zh-cn_topic_0000002175160637_p1595317341137"></a><a name="zh-cn_topic_0000002175160637_p1595317341137"></a>通话设置</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002175160637_p12953734939"><a name="zh-cn_topic_0000002175160637_p12953734939"></a><a name="zh-cn_topic_0000002175160637_p12953734939"></a>通话自动接听开关</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002175160637_p99538342320"><a name="zh-cn_topic_0000002175160637_p99538342320"></a><a name="zh-cn_topic_0000002175160637_p99538342320"></a>设置是否通话自动接听。</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002175160637_p16953143417318"><a name="zh-cn_topic_0000002175160637_p16953143417318"></a><a name="zh-cn_topic_0000002175160637_p16953143417318"></a>核间通信</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160637_row109531834539"><td class="cellrowborder" valign="top" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002175160637_p395319344312"><a name="zh-cn_topic_0000002175160637_p395319344312"></a><a name="zh-cn_topic_0000002175160637_p395319344312"></a>拨打电话，如“10086”</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002175160637_p1395310341317"><a name="zh-cn_topic_0000002175160637_p1395310341317"></a><a name="zh-cn_topic_0000002175160637_p1395310341317"></a>通知Modem进行拨打电话。</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002175160637_p5953113414317"><a name="zh-cn_topic_0000002175160637_p5953113414317"></a><a name="zh-cn_topic_0000002175160637_p5953113414317"></a>核间通信</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175160637_row14953034435"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002175160637_p17953203417312"><a name="zh-cn_topic_0000002175160637_p17953203417312"></a><a name="zh-cn_topic_0000002175160637_p17953203417312"></a>短信设置</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002175160637_p149534341032"><a name="zh-cn_topic_0000002175160637_p149534341032"></a><a name="zh-cn_topic_0000002175160637_p149534341032"></a>显示短信中心号码</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002175160637_p149531734132"><a name="zh-cn_topic_0000002175160637_p149531734132"></a><a name="zh-cn_topic_0000002175160637_p149531734132"></a>获取短信中心号码。</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002175160637_p189533341134"><a name="zh-cn_topic_0000002175160637_p189533341134"></a><a name="zh-cn_topic_0000002175160637_p189533341134"></a>核内交互</p>
</td>
</tr>
</tbody>
</table>

-   **[RIL相关接口](#ZH-CN_TOPIC_0000002223508028)**  

-   **[VoLTE相关AT命令](#ZH-CN_TOPIC_0000002223667908)**  

-   **[VoLTE相关NV](#ZH-CN_TOPIC_0000002223508088)**  

## RIL相关接口<a name="ZH-CN_TOPIC_0000002223508028"></a>

RIL相关接口存在于主控芯片 SDK版本中，头文件路径：protocol\\ims\\include\\ril\_interface.h

-   **[设备信息接口](#ZH-CN_TOPIC_0000002223667876)**  

-   **[2131E异常信息回调](#ZH-CN_TOPIC_0000002538513722)**  

-   **[SIM卡信息接口](#ZH-CN_TOPIC_0000002223667880)**  

-   **[小区信息](#ZH-CN_TOPIC_0000002509848179)**  

-   **[驻网状态接口](#ZH-CN_TOPIC_0000002258627549)**  

-   **[通话接口](#ZH-CN_TOPIC_0000002223667892)**  

-   **[AT指令通道](#ZH-CN_TOPIC_0000002258467681)**  

-   **[短信接口](#ZH-CN_TOPIC_0000002509728291)**  

-   **[升级接口](#ZH-CN_TOPIC_0000002436334706)**  

### 设备信息接口<a name="ZH-CN_TOPIC_0000002223667876"></a>

-   **[获取软件版本号](#ZH-CN_TOPIC_0000002258467645)**  

-   **[获取IMEI设备识别号](#ZH-CN_TOPIC_0000002258627537)**  

-   **[设定VoLTE维测日志级别](#ZH-CN_TOPIC_0000002223508036)**  

-   **[判断Hi2131E是否校准](#ZH-CN_TOPIC_0000002509728151)**  

-   **[获取基站UTC时间](#ZH-CN_TOPIC_0000002566425755)**  

#### 获取软件版本号<a name="ZH-CN_TOPIC_0000002258467645"></a>

函数原型：int32\_t ril\_get\_software\_version\(char\* ver, size\_t size\)

函数功能：获取Hi2131E软件版本号，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002227350729_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227350729_row12908457921"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002227350729_p563315171132"><a name="zh-cn_topic_0000002227350729_p563315171132"></a><a name="zh-cn_topic_0000002227350729_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002227350729_p1463351718318"><a name="zh-cn_topic_0000002227350729_p1463351718318"></a><a name="zh-cn_topic_0000002227350729_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="21.78%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002227350729_p16338179319"><a name="zh-cn_topic_0000002227350729_p16338179319"></a><a name="zh-cn_topic_0000002227350729_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="28.22%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002227350729_p1463316170313"><a name="zh-cn_topic_0000002227350729_p1463316170313"></a><a name="zh-cn_topic_0000002227350729_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227350729_row49086571217"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002227350729_p89083571824"><a name="zh-cn_topic_0000002227350729_p89083571824"></a><a name="zh-cn_topic_0000002227350729_p89083571824"></a>ver</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002227350729_p149081857623"><a name="zh-cn_topic_0000002227350729_p149081857623"></a><a name="zh-cn_topic_0000002227350729_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="21.78%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002227350729_p29081571426"><a name="zh-cn_topic_0000002227350729_p29081571426"></a><a name="zh-cn_topic_0000002227350729_p29081571426"></a>char*</p>
</td>
<td class="cellrowborder" valign="top" width="28.22%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002227350729_p139081573210"><a name="zh-cn_topic_0000002227350729_p139081573210"></a><a name="zh-cn_topic_0000002227350729_p139081573210"></a>软件版本号，字符串格式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002227350729_row169088571825"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002227350729_p1908185718216"><a name="zh-cn_topic_0000002227350729_p1908185718216"></a><a name="zh-cn_topic_0000002227350729_p1908185718216"></a>size</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002227350729_p169087570212"><a name="zh-cn_topic_0000002227350729_p169087570212"></a><a name="zh-cn_topic_0000002227350729_p169087570212"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="21.78%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002227350729_p490811571127"><a name="zh-cn_topic_0000002227350729_p490811571127"></a><a name="zh-cn_topic_0000002227350729_p490811571127"></a>size_t</p>
</td>
<td class="cellrowborder" valign="top" width="28.22%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002227350729_p1190818571228"><a name="zh-cn_topic_0000002227350729_p1190818571228"></a><a name="zh-cn_topic_0000002227350729_p1190818571228"></a>软件版本号长度。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002227350729_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227350729_row82122391941"><th class="cellrowborder" valign="top" width="25.182518251825186%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002227350729_p15896553644"><a name="zh-cn_topic_0000002227350729_p15896553644"></a><a name="zh-cn_topic_0000002227350729_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="31.863186318631865%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002227350729_p489612532411"><a name="zh-cn_topic_0000002227350729_p489612532411"></a><a name="zh-cn_topic_0000002227350729_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="42.95429542954295%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002227350729_p148968533417"><a name="zh-cn_topic_0000002227350729_p148968533417"></a><a name="zh-cn_topic_0000002227350729_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227350729_row5213113915410"><td class="cellrowborder" valign="top" width="25.182518251825186%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227350729_p14111210255"><a name="zh-cn_topic_0000002227350729_p14111210255"></a><a name="zh-cn_topic_0000002227350729_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="31.863186318631865%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227350729_p14411151012512"><a name="zh-cn_topic_0000002227350729_p14411151012512"></a><a name="zh-cn_topic_0000002227350729_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="42.95429542954295%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227350729_p184114100511"><a name="zh-cn_topic_0000002227350729_p184114100511"></a><a name="zh-cn_topic_0000002227350729_p184114100511"></a>获取Hi2131E软件版本号接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002227350729_row82131139142"><td class="cellrowborder" valign="top" width="25.182518251825186%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227350729_p1141117102516"><a name="zh-cn_topic_0000002227350729_p1141117102516"></a><a name="zh-cn_topic_0000002227350729_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="31.863186318631865%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227350729_p164111810052"><a name="zh-cn_topic_0000002227350729_p164111810052"></a><a name="zh-cn_topic_0000002227350729_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="42.95429542954295%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227350729_p114111010856"><a name="zh-cn_topic_0000002227350729_p114111010856"></a><a name="zh-cn_topic_0000002227350729_p114111010856"></a>获取Hi2131E软件版本号接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 获取IMEI设备识别号<a name="ZH-CN_TOPIC_0000002258627537"></a>

函数原型：int32\_t ril\_get\_imei\(char\* imei, size\_t len\)

函数功能：获取Hi2131E的IMEI号，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002191825232_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002191825232_row12908457921"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002191825232_p563315171132"><a name="zh-cn_topic_0000002191825232_p563315171132"></a><a name="zh-cn_topic_0000002191825232_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002191825232_p1463351718318"><a name="zh-cn_topic_0000002191825232_p1463351718318"></a><a name="zh-cn_topic_0000002191825232_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002191825232_p16338179319"><a name="zh-cn_topic_0000002191825232_p16338179319"></a><a name="zh-cn_topic_0000002191825232_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002191825232_p1463316170313"><a name="zh-cn_topic_0000002191825232_p1463316170313"></a><a name="zh-cn_topic_0000002191825232_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002191825232_row49086571217"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002191825232_p13145102016548"><a name="zh-cn_topic_0000002191825232_p13145102016548"></a><a name="zh-cn_topic_0000002191825232_p13145102016548"></a>imei</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002191825232_p149081857623"><a name="zh-cn_topic_0000002191825232_p149081857623"></a><a name="zh-cn_topic_0000002191825232_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002191825232_p29081571426"><a name="zh-cn_topic_0000002191825232_p29081571426"></a><a name="zh-cn_topic_0000002191825232_p29081571426"></a>char*</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002191825232_p15928142985417"><a name="zh-cn_topic_0000002191825232_p15928142985417"></a><a name="zh-cn_topic_0000002191825232_p15928142985417"></a>IMEI号，字符串格式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002191825232_row169088571825"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002191825232_p2014552035415"><a name="zh-cn_topic_0000002191825232_p2014552035415"></a><a name="zh-cn_topic_0000002191825232_p2014552035415"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002191825232_p169087570212"><a name="zh-cn_topic_0000002191825232_p169087570212"></a><a name="zh-cn_topic_0000002191825232_p169087570212"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002191825232_p490811571127"><a name="zh-cn_topic_0000002191825232_p490811571127"></a><a name="zh-cn_topic_0000002191825232_p490811571127"></a>size_t</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002191825232_p792832918543"><a name="zh-cn_topic_0000002191825232_p792832918543"></a><a name="zh-cn_topic_0000002191825232_p792832918543"></a>IMEI号长度。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002191825232_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002191825232_row82122391941"><th class="cellrowborder" valign="top" width="24.782478247824784%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002191825232_p15896553644"><a name="zh-cn_topic_0000002191825232_p15896553644"></a><a name="zh-cn_topic_0000002191825232_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="29.27292729272927%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002191825232_p489612532411"><a name="zh-cn_topic_0000002191825232_p489612532411"></a><a name="zh-cn_topic_0000002191825232_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="45.944594459445945%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002191825232_p148968533417"><a name="zh-cn_topic_0000002191825232_p148968533417"></a><a name="zh-cn_topic_0000002191825232_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002191825232_row5213113915410"><td class="cellrowborder" valign="top" width="24.782478247824784%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002191825232_p14111210255"><a name="zh-cn_topic_0000002191825232_p14111210255"></a><a name="zh-cn_topic_0000002191825232_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="29.27292729272927%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002191825232_p14411151012512"><a name="zh-cn_topic_0000002191825232_p14411151012512"></a><a name="zh-cn_topic_0000002191825232_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="45.944594459445945%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002191825232_p151469557531"><a name="zh-cn_topic_0000002191825232_p151469557531"></a><a name="zh-cn_topic_0000002191825232_p151469557531"></a>获取Hi2131E IMEI号函数接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002191825232_row82131139142"><td class="cellrowborder" valign="top" width="24.782478247824784%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002191825232_p1141117102516"><a name="zh-cn_topic_0000002191825232_p1141117102516"></a><a name="zh-cn_topic_0000002191825232_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="29.27292729272927%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002191825232_p164111810052"><a name="zh-cn_topic_0000002191825232_p164111810052"></a><a name="zh-cn_topic_0000002191825232_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="45.944594459445945%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002191825232_p214655565313"><a name="zh-cn_topic_0000002191825232_p214655565313"></a><a name="zh-cn_topic_0000002191825232_p214655565313"></a>获取Hi2131E IMEI号函数接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 设定VoLTE维测日志级别<a name="ZH-CN_TOPIC_0000002223508036"></a>

函数原型：int32\_t ril\_set\_user\_log\_level\(ril\_user\_log\_level level\)

函数功能：设定VoLTE维测功能的日志级别。

**表 1**  参数说明

<a name="zh-cn_topic_0000002207624714_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002207624714_row12908457921"><th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002207624714_p563315171132"><a name="zh-cn_topic_0000002207624714_p563315171132"></a><a name="zh-cn_topic_0000002207624714_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="14.49%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002207624714_p1463351718318"><a name="zh-cn_topic_0000002207624714_p1463351718318"></a><a name="zh-cn_topic_0000002207624714_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="22.71%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002207624714_p16338179319"><a name="zh-cn_topic_0000002207624714_p16338179319"></a><a name="zh-cn_topic_0000002207624714_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="50.42%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002207624714_p1463316170313"><a name="zh-cn_topic_0000002207624714_p1463316170313"></a><a name="zh-cn_topic_0000002207624714_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002207624714_row49086571217"><td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002207624714_p13145102016548"><a name="zh-cn_topic_0000002207624714_p13145102016548"></a><a name="zh-cn_topic_0000002207624714_p13145102016548"></a>level</p>
</td>
<td class="cellrowborder" valign="top" width="14.49%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002207624714_p149081857623"><a name="zh-cn_topic_0000002207624714_p149081857623"></a><a name="zh-cn_topic_0000002207624714_p149081857623"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="22.71%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002207624714_p29081571426"><a name="zh-cn_topic_0000002207624714_p29081571426"></a><a name="zh-cn_topic_0000002207624714_p29081571426"></a>ril_user_log_level</p>
</td>
<td class="cellrowborder" valign="top" width="50.42%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002207624714_p15928142985417"><a name="zh-cn_topic_0000002207624714_p15928142985417"></a><a name="zh-cn_topic_0000002207624714_p15928142985417"></a>RIL_USER_LEVEL_NULL：关闭日志。</p>
<p id="zh-cn_topic_0000002207624714_p17457113112167"><a name="zh-cn_topic_0000002207624714_p17457113112167"></a><a name="zh-cn_topic_0000002207624714_p17457113112167"></a>RIL_USER_LEVEL_INFO：打印INFO级别及更高级别日志。</p>
<p id="zh-cn_topic_0000002207624714_p123301824201615"><a name="zh-cn_topic_0000002207624714_p123301824201615"></a><a name="zh-cn_topic_0000002207624714_p123301824201615"></a>RIL_USER_LEVEL_WARN：打印WARN级别及更高级别日志。</p>
<p id="zh-cn_topic_0000002207624714_p0765250131513"><a name="zh-cn_topic_0000002207624714_p0765250131513"></a><a name="zh-cn_topic_0000002207624714_p0765250131513"></a>RIL_USER_LEVEL_ERROR：打印ERROR级别及更高级别日志。</p>
<p id="zh-cn_topic_0000002207624714_p9727164321719"><a name="zh-cn_topic_0000002207624714_p9727164321719"></a><a name="zh-cn_topic_0000002207624714_p9727164321719"></a>RIL_USER_LEVEL_ALL：打印所有日志。</p>
</td>
</tr>
</tbody>
</table>

>![](public_sys-resources/icon-note.gif) **说明：** 
>当前版本日志级别默认为RIL\_USER\_LEVEL\_ERROR级别。

#### 判断Hi2131E是否校准<a name="ZH-CN_TOPIC_0000002509728151"></a>

函数原型：int32\_t ril\_get\_if\_ue\_calibrated\(bool \*calibrate\)

函数功能：获取当前Hi2131E是否已经校准。

**表 1**  参数说明

<a name="zh-cn_topic_0000002345003593_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002345003593_row12908457921"><th class="cellrowborder" valign="top" width="18.2%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002345003593_p563315171132"><a name="zh-cn_topic_0000002345003593_p563315171132"></a><a name="zh-cn_topic_0000002345003593_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="20.28%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002345003593_p1463351718318"><a name="zh-cn_topic_0000002345003593_p1463351718318"></a><a name="zh-cn_topic_0000002345003593_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="32.029999999999994%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002345003593_p16338179319"><a name="zh-cn_topic_0000002345003593_p16338179319"></a><a name="zh-cn_topic_0000002345003593_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="29.49%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002345003593_p1463316170313"><a name="zh-cn_topic_0000002345003593_p1463316170313"></a><a name="zh-cn_topic_0000002345003593_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002345003593_row49086571217"><td class="cellrowborder" valign="top" width="18.2%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002345003593_p13145102016548"><a name="zh-cn_topic_0000002345003593_p13145102016548"></a><a name="zh-cn_topic_0000002345003593_p13145102016548"></a>calibrate</p>
</td>
<td class="cellrowborder" valign="top" width="20.28%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002345003593_p149081857623"><a name="zh-cn_topic_0000002345003593_p149081857623"></a><a name="zh-cn_topic_0000002345003593_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="32.029999999999994%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002345003593_p29081571426"><a name="zh-cn_topic_0000002345003593_p29081571426"></a><a name="zh-cn_topic_0000002345003593_p29081571426"></a>bool*</p>
</td>
<td class="cellrowborder" valign="top" width="29.49%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002345003593_p143791285174"><a name="zh-cn_topic_0000002345003593_p143791285174"></a><a name="zh-cn_topic_0000002345003593_p143791285174"></a>Hi2131E是否已经校准：</p>
<p id="zh-cn_topic_0000002345003593_p15928142985417"><a name="zh-cn_topic_0000002345003593_p15928142985417"></a><a name="zh-cn_topic_0000002345003593_p15928142985417"></a>true：Hi2131E已校准l；</p>
<p id="zh-cn_topic_0000002345003593_p73569581016"><a name="zh-cn_topic_0000002345003593_p73569581016"></a><a name="zh-cn_topic_0000002345003593_p73569581016"></a>false：Hi2131E未校准。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002345003593_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002345003593_row82122391941"><th class="cellrowborder" valign="top" width="27.82278227822782%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002345003593_p15896553644"><a name="zh-cn_topic_0000002345003593_p15896553644"></a><a name="zh-cn_topic_0000002345003593_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="31.47314731473147%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002345003593_p489612532411"><a name="zh-cn_topic_0000002345003593_p489612532411"></a><a name="zh-cn_topic_0000002345003593_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="40.704070407040696%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002345003593_p148968533417"><a name="zh-cn_topic_0000002345003593_p148968533417"></a><a name="zh-cn_topic_0000002345003593_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002345003593_row5213113915410"><td class="cellrowborder" valign="top" width="27.82278227822782%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002345003593_p14111210255"><a name="zh-cn_topic_0000002345003593_p14111210255"></a><a name="zh-cn_topic_0000002345003593_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="31.47314731473147%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002345003593_p14411151012512"><a name="zh-cn_topic_0000002345003593_p14411151012512"></a><a name="zh-cn_topic_0000002345003593_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="40.704070407040696%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002345003593_p151469557531"><a name="zh-cn_topic_0000002345003593_p151469557531"></a><a name="zh-cn_topic_0000002345003593_p151469557531"></a>获取Hi2131E是否校准处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002345003593_row82131139142"><td class="cellrowborder" valign="top" width="27.82278227822782%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002345003593_p1141117102516"><a name="zh-cn_topic_0000002345003593_p1141117102516"></a><a name="zh-cn_topic_0000002345003593_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="31.47314731473147%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002345003593_p164111810052"><a name="zh-cn_topic_0000002345003593_p164111810052"></a><a name="zh-cn_topic_0000002345003593_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="40.704070407040696%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002345003593_p214655565313"><a name="zh-cn_topic_0000002345003593_p214655565313"></a><a name="zh-cn_topic_0000002345003593_p214655565313"></a>获取Hi2131E是否校准处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 获取基站UTC时间<a name="ZH-CN_TOPIC_0000002566425755"></a>

函数原型：int32\_t ril\_get\_utc\_time\(const char\* utc\_str, uint16\_t size\);

函数功能：获取当前基站UTC时间。

**表 1**  参数说明

<a name="zh-cn_topic_0000002345003593_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002345003593_row12908457921"><th class="cellrowborder" valign="top" width="18.2%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002345003593_p563315171132"><a name="zh-cn_topic_0000002345003593_p563315171132"></a><a name="zh-cn_topic_0000002345003593_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="20.28%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002345003593_p1463351718318"><a name="zh-cn_topic_0000002345003593_p1463351718318"></a><a name="zh-cn_topic_0000002345003593_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="32.029999999999994%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002345003593_p16338179319"><a name="zh-cn_topic_0000002345003593_p16338179319"></a><a name="zh-cn_topic_0000002345003593_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="29.49%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002345003593_p1463316170313"><a name="zh-cn_topic_0000002345003593_p1463316170313"></a><a name="zh-cn_topic_0000002345003593_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002345003593_row49086571217"><td class="cellrowborder" valign="top" width="18.2%" headers="mcps1.2.5.1.1 "><p id="p260943434917"><a name="p260943434917"></a><a name="p260943434917"></a>utc_str</p>
</td>
<td class="cellrowborder" valign="top" width="20.28%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002345003593_p149081857623"><a name="zh-cn_topic_0000002345003593_p149081857623"></a><a name="zh-cn_topic_0000002345003593_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="32.029999999999994%" headers="mcps1.2.5.1.3 "><p id="p1984915463490"><a name="p1984915463490"></a><a name="p1984915463490"></a>char*</p>
</td>
<td class="cellrowborder" valign="top" width="29.49%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002345003593_p73569581016"><a name="zh-cn_topic_0000002345003593_p73569581016"></a><a name="zh-cn_topic_0000002345003593_p73569581016"></a>返回的UTC时间字符串。</p>
</td>
</tr>
<tr id="row848142904918"><td class="cellrowborder" valign="top" width="18.2%" headers="mcps1.2.5.1.1 "><p id="p1249129194914"><a name="p1249129194914"></a><a name="p1249129194914"></a>size</p>
</td>
<td class="cellrowborder" valign="top" width="20.28%" headers="mcps1.2.5.1.2 "><p id="p1849142984916"><a name="p1849142984916"></a><a name="p1849142984916"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="32.029999999999994%" headers="mcps1.2.5.1.3 "><p id="p15491629164913"><a name="p15491629164913"></a><a name="p15491629164913"></a>uint16_t</p>
</td>
<td class="cellrowborder" valign="top" width="29.49%" headers="mcps1.2.5.1.4 "><p id="p9494296499"><a name="p9494296499"></a><a name="p9494296499"></a>出参最大长度。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002345003593_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002345003593_row82122391941"><th class="cellrowborder" valign="top" width="27.82278227822782%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002345003593_p15896553644"><a name="zh-cn_topic_0000002345003593_p15896553644"></a><a name="zh-cn_topic_0000002345003593_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="31.47314731473147%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002345003593_p489612532411"><a name="zh-cn_topic_0000002345003593_p489612532411"></a><a name="zh-cn_topic_0000002345003593_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="40.704070407040696%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002345003593_p148968533417"><a name="zh-cn_topic_0000002345003593_p148968533417"></a><a name="zh-cn_topic_0000002345003593_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002345003593_row5213113915410"><td class="cellrowborder" valign="top" width="27.82278227822782%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002345003593_p14111210255"><a name="zh-cn_topic_0000002345003593_p14111210255"></a><a name="zh-cn_topic_0000002345003593_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="31.47314731473147%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002345003593_p14411151012512"><a name="zh-cn_topic_0000002345003593_p14411151012512"></a><a name="zh-cn_topic_0000002345003593_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="40.704070407040696%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002345003593_p151469557531"><a name="zh-cn_topic_0000002345003593_p151469557531"></a><a name="zh-cn_topic_0000002345003593_p151469557531"></a>获取UTC时间处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002345003593_row82131139142"><td class="cellrowborder" valign="top" width="27.82278227822782%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002345003593_p1141117102516"><a name="zh-cn_topic_0000002345003593_p1141117102516"></a><a name="zh-cn_topic_0000002345003593_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="31.47314731473147%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002345003593_p164111810052"><a name="zh-cn_topic_0000002345003593_p164111810052"></a><a name="zh-cn_topic_0000002345003593_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="40.704070407040696%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002345003593_p214655565313"><a name="zh-cn_topic_0000002345003593_p214655565313"></a><a name="zh-cn_topic_0000002345003593_p214655565313"></a>获取UTC时间处理失败。</p>
</td>
</tr>
</tbody>
</table>

### 2131E异常信息回调<a name="ZH-CN_TOPIC_0000002538513722"></a>

函数原型：void ril\_register\_sys\_err\_report\_callback\(respond\_callback\_t rsp\_callback\);

函数功能：注册回调函数，当2131E发生异常时上报异常原因。

回调函数：typedef int32\_t \(\*respond\_callback\_t\)\(uint8\_t\* data, size\_t len\);

**表 1**  参数说明

<a name="zh-cn_topic_0000002227350737_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227350737_row12908457921"><th class="cellrowborder" valign="top" width="17.4%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002227350737_p563315171132"><a name="zh-cn_topic_0000002227350737_p563315171132"></a><a name="zh-cn_topic_0000002227350737_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="20.96%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002227350737_p1463351718318"><a name="zh-cn_topic_0000002227350737_p1463351718318"></a><a name="zh-cn_topic_0000002227350737_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="26.279999999999998%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002227350737_p16338179319"><a name="zh-cn_topic_0000002227350737_p16338179319"></a><a name="zh-cn_topic_0000002227350737_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="35.36%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002227350737_p1463316170313"><a name="zh-cn_topic_0000002227350737_p1463316170313"></a><a name="zh-cn_topic_0000002227350737_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227350737_row49086571217"><td class="cellrowborder" valign="top" width="17.4%" headers="mcps1.2.5.1.1 "><p id="p435611822811"><a name="p435611822811"></a><a name="p435611822811"></a>rsp_callback</p>
</td>
<td class="cellrowborder" valign="top" width="20.96%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002227350737_p149081857623"><a name="zh-cn_topic_0000002227350737_p149081857623"></a><a name="zh-cn_topic_0000002227350737_p149081857623"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="26.279999999999998%" headers="mcps1.2.5.1.3 "><p id="p1087102262810"><a name="p1087102262810"></a><a name="p1087102262810"></a>respond_callback_t</p>
</td>
<td class="cellrowborder" valign="top" width="35.36%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002227350737_p15928142985417"><a name="zh-cn_topic_0000002227350737_p15928142985417"></a><a name="zh-cn_topic_0000002227350737_p15928142985417"></a>注册函数。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  回调函数respond\_callback\_t说明

<a name="table1478244073114"></a>
<table><thead align="left"><tr id="row478313405319"><th class="cellrowborder" valign="top" width="16.8%" id="mcps1.2.5.1.1"><p id="p1177724611310"><a name="p1177724611310"></a><a name="p1177724611310"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="21.33%" id="mcps1.2.5.1.2"><p id="p577715466318"><a name="p577715466318"></a><a name="p577715466318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="26.400000000000002%" id="mcps1.2.5.1.3"><p id="p1777144653111"><a name="p1777144653111"></a><a name="p1777144653111"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="35.47%" id="mcps1.2.5.1.4"><p id="p577718466314"><a name="p577718466314"></a><a name="p577718466314"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row0783240153111"><td class="cellrowborder" valign="top" width="16.8%" headers="mcps1.2.5.1.1 "><p id="p3777194619319"><a name="p3777194619319"></a><a name="p3777194619319"></a>data</p>
</td>
<td class="cellrowborder" valign="top" width="21.33%" headers="mcps1.2.5.1.2 "><p id="p79841145123217"><a name="p79841145123217"></a><a name="p79841145123217"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="26.400000000000002%" headers="mcps1.2.5.1.3 "><p id="p1114182816322"><a name="p1114182816322"></a><a name="p1114182816322"></a>uint8_t*</p>
</td>
<td class="cellrowborder" valign="top" width="35.47%" headers="mcps1.2.5.1.4 "><p id="p12149108153318"><a name="p12149108153318"></a><a name="p12149108153318"></a>异常打印字符串（真实类型为char*）。</p>
</td>
</tr>
<tr id="row20783164053115"><td class="cellrowborder" valign="top" width="16.8%" headers="mcps1.2.5.1.1 "><p id="p1478320408316"><a name="p1478320408316"></a><a name="p1478320408316"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="21.33%" headers="mcps1.2.5.1.2 "><p id="p9151111153316"><a name="p9151111153316"></a><a name="p9151111153316"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="26.400000000000002%" headers="mcps1.2.5.1.3 "><p id="p978364013112"><a name="p978364013112"></a><a name="p978364013112"></a>size_t</p>
</td>
<td class="cellrowborder" valign="top" width="35.47%" headers="mcps1.2.5.1.4 "><p id="p1278344014318"><a name="p1278344014318"></a><a name="p1278344014318"></a>异常打印字符串长度。</p>
</td>
</tr>
</tbody>
</table>

### SIM卡信息接口<a name="ZH-CN_TOPIC_0000002223667880"></a>

-   **[判断Hi2131E的SIM卡是否在位](#ZH-CN_TOPIC_0000002258467649)**  

-   **[判断Hi2131E的SIM卡PIN码是否被锁](#ZH-CN_TOPIC_0000002258627545)**  

-   **[判断Hi2131E的SIM卡PUK码是否被锁](#ZH-CN_TOPIC_0000002223508040)**  

-   **[获取Hi2131E的SIM卡IMSI号](#ZH-CN_TOPIC_0000002223667884)**  

-   **[获取Hi2131E的SIM卡msisdn](#ZH-CN_TOPIC_0000002258467657)**  

-   **[获取CCHO返回结果](#ZH-CN_TOPIC_0000002566337577)**  

#### 判断Hi2131E的SIM卡是否在位<a name="ZH-CN_TOPIC_0000002258467649"></a>

函数原型：int32\_t ril\_get\_is\_usim\_in\_position\(bool \*status\);

函数功能：获取Hi2131E的\(U\)SIM是否在位，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002227350737_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227350737_row12908457921"><th class="cellrowborder" valign="top" width="18.82%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002227350737_p563315171132"><a name="zh-cn_topic_0000002227350737_p563315171132"></a><a name="zh-cn_topic_0000002227350737_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="21.05%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002227350737_p1463351718318"><a name="zh-cn_topic_0000002227350737_p1463351718318"></a><a name="zh-cn_topic_0000002227350737_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="27.12%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002227350737_p16338179319"><a name="zh-cn_topic_0000002227350737_p16338179319"></a><a name="zh-cn_topic_0000002227350737_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="33.01%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002227350737_p1463316170313"><a name="zh-cn_topic_0000002227350737_p1463316170313"></a><a name="zh-cn_topic_0000002227350737_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227350737_row49086571217"><td class="cellrowborder" valign="top" width="18.82%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002227350737_p13145102016548"><a name="zh-cn_topic_0000002227350737_p13145102016548"></a><a name="zh-cn_topic_0000002227350737_p13145102016548"></a>status</p>
</td>
<td class="cellrowborder" valign="top" width="21.05%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002227350737_p149081857623"><a name="zh-cn_topic_0000002227350737_p149081857623"></a><a name="zh-cn_topic_0000002227350737_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="27.12%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002227350737_p29081571426"><a name="zh-cn_topic_0000002227350737_p29081571426"></a><a name="zh-cn_topic_0000002227350737_p29081571426"></a>bool*</p>
</td>
<td class="cellrowborder" valign="top" width="33.01%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002227350737_p15928142985417"><a name="zh-cn_topic_0000002227350737_p15928142985417"></a><a name="zh-cn_topic_0000002227350737_p15928142985417"></a>是否在位。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002227350737_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227350737_row82122391941"><th class="cellrowborder" valign="top" width="19.131913191319132%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002227350737_p15896553644"><a name="zh-cn_topic_0000002227350737_p15896553644"></a><a name="zh-cn_topic_0000002227350737_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="30.773077307730766%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002227350737_p489612532411"><a name="zh-cn_topic_0000002227350737_p489612532411"></a><a name="zh-cn_topic_0000002227350737_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="50.09500950095009%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002227350737_p148968533417"><a name="zh-cn_topic_0000002227350737_p148968533417"></a><a name="zh-cn_topic_0000002227350737_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227350737_row5213113915410"><td class="cellrowborder" valign="top" width="19.131913191319132%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227350737_p14111210255"><a name="zh-cn_topic_0000002227350737_p14111210255"></a><a name="zh-cn_topic_0000002227350737_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="30.773077307730766%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227350737_p14411151012512"><a name="zh-cn_topic_0000002227350737_p14411151012512"></a><a name="zh-cn_topic_0000002227350737_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="50.09500950095009%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227350737_p151469557531"><a name="zh-cn_topic_0000002227350737_p151469557531"></a><a name="zh-cn_topic_0000002227350737_p151469557531"></a>获取Hi2131E的SIM卡是否在位接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002227350737_row82131139142"><td class="cellrowborder" valign="top" width="19.131913191319132%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227350737_p1141117102516"><a name="zh-cn_topic_0000002227350737_p1141117102516"></a><a name="zh-cn_topic_0000002227350737_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="30.773077307730766%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227350737_p164111810052"><a name="zh-cn_topic_0000002227350737_p164111810052"></a><a name="zh-cn_topic_0000002227350737_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="50.09500950095009%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227350737_p214655565313"><a name="zh-cn_topic_0000002227350737_p214655565313"></a><a name="zh-cn_topic_0000002227350737_p214655565313"></a>获取Hi2131E的SIM卡是否在位接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 判断Hi2131E的SIM卡PIN码是否被锁<a name="ZH-CN_TOPIC_0000002258627545"></a>

函数原型：int32\_t ril\_get\_is\_usim\_pin\_block\(bool \*status\);

函数功能：获取Hi2131E的\(U\)SIM卡PIN码是否被锁，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002191825240_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002191825240_row12908457921"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002191825240_p563315171132"><a name="zh-cn_topic_0000002191825240_p563315171132"></a><a name="zh-cn_topic_0000002191825240_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002191825240_p1463351718318"><a name="zh-cn_topic_0000002191825240_p1463351718318"></a><a name="zh-cn_topic_0000002191825240_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002191825240_p16338179319"><a name="zh-cn_topic_0000002191825240_p16338179319"></a><a name="zh-cn_topic_0000002191825240_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002191825240_p1463316170313"><a name="zh-cn_topic_0000002191825240_p1463316170313"></a><a name="zh-cn_topic_0000002191825240_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002191825240_row49086571217"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002191825240_p13145102016548"><a name="zh-cn_topic_0000002191825240_p13145102016548"></a><a name="zh-cn_topic_0000002191825240_p13145102016548"></a>status</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002191825240_p149081857623"><a name="zh-cn_topic_0000002191825240_p149081857623"></a><a name="zh-cn_topic_0000002191825240_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002191825240_p29081571426"><a name="zh-cn_topic_0000002191825240_p29081571426"></a><a name="zh-cn_topic_0000002191825240_p29081571426"></a>bool*</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002191825240_p645632261211"><a name="zh-cn_topic_0000002191825240_p645632261211"></a><a name="zh-cn_topic_0000002191825240_p645632261211"></a>true：PIN码被锁。</p>
<p id="zh-cn_topic_0000002191825240_p10968105911111"><a name="zh-cn_topic_0000002191825240_p10968105911111"></a><a name="zh-cn_topic_0000002191825240_p10968105911111"></a>false：PIN码没有被锁。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002191825240_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002191825240_row82122391941"><th class="cellrowborder" valign="top" width="24.652465246524653%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002191825240_p15896553644"><a name="zh-cn_topic_0000002191825240_p15896553644"></a><a name="zh-cn_topic_0000002191825240_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="29.192919291929194%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002191825240_p489612532411"><a name="zh-cn_topic_0000002191825240_p489612532411"></a><a name="zh-cn_topic_0000002191825240_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="46.15461546154616%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002191825240_p148968533417"><a name="zh-cn_topic_0000002191825240_p148968533417"></a><a name="zh-cn_topic_0000002191825240_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002191825240_row5213113915410"><td class="cellrowborder" valign="top" width="24.652465246524653%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002191825240_p14111210255"><a name="zh-cn_topic_0000002191825240_p14111210255"></a><a name="zh-cn_topic_0000002191825240_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="29.192919291929194%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002191825240_p14411151012512"><a name="zh-cn_topic_0000002191825240_p14411151012512"></a><a name="zh-cn_topic_0000002191825240_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="46.15461546154616%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002191825240_p151469557531"><a name="zh-cn_topic_0000002191825240_p151469557531"></a><a name="zh-cn_topic_0000002191825240_p151469557531"></a>获取Hi2131E的(U)SIM卡PIN码是否被锁接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002191825240_row82131139142"><td class="cellrowborder" valign="top" width="24.652465246524653%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002191825240_p1141117102516"><a name="zh-cn_topic_0000002191825240_p1141117102516"></a><a name="zh-cn_topic_0000002191825240_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="29.192919291929194%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002191825240_p164111810052"><a name="zh-cn_topic_0000002191825240_p164111810052"></a><a name="zh-cn_topic_0000002191825240_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="46.15461546154616%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002191825240_p214655565313"><a name="zh-cn_topic_0000002191825240_p214655565313"></a><a name="zh-cn_topic_0000002191825240_p214655565313"></a>获取Hi2131E的(U)SIM卡PIN码是否被锁接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 判断Hi2131E的SIM卡PUK码是否被锁<a name="ZH-CN_TOPIC_0000002223508040"></a>

函数原型：int32\_t ril\_get\_is\_usim\_puk\_block\(bool \*status\);

函数功能：获取Hi2131E的\(U\)SIM卡PUK码是否被锁，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002191984868_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002191984868_row12908457921"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002191984868_p563315171132"><a name="zh-cn_topic_0000002191984868_p563315171132"></a><a name="zh-cn_topic_0000002191984868_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002191984868_p1463351718318"><a name="zh-cn_topic_0000002191984868_p1463351718318"></a><a name="zh-cn_topic_0000002191984868_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002191984868_p16338179319"><a name="zh-cn_topic_0000002191984868_p16338179319"></a><a name="zh-cn_topic_0000002191984868_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002191984868_p1463316170313"><a name="zh-cn_topic_0000002191984868_p1463316170313"></a><a name="zh-cn_topic_0000002191984868_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002191984868_row49086571217"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002191984868_p13145102016548"><a name="zh-cn_topic_0000002191984868_p13145102016548"></a><a name="zh-cn_topic_0000002191984868_p13145102016548"></a>status</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002191984868_p149081857623"><a name="zh-cn_topic_0000002191984868_p149081857623"></a><a name="zh-cn_topic_0000002191984868_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002191984868_p29081571426"><a name="zh-cn_topic_0000002191984868_p29081571426"></a><a name="zh-cn_topic_0000002191984868_p29081571426"></a>bool*</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002191984868_p15928142985417"><a name="zh-cn_topic_0000002191984868_p15928142985417"></a><a name="zh-cn_topic_0000002191984868_p15928142985417"></a>true：PUK码被锁。</p>
<p id="zh-cn_topic_0000002191984868_p10968105911111"><a name="zh-cn_topic_0000002191984868_p10968105911111"></a><a name="zh-cn_topic_0000002191984868_p10968105911111"></a>false：PUK码没有被锁。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002191984868_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002191984868_row82122391941"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002191984868_p15896553644"><a name="zh-cn_topic_0000002191984868_p15896553644"></a><a name="zh-cn_topic_0000002191984868_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002191984868_p489612532411"><a name="zh-cn_topic_0000002191984868_p489612532411"></a><a name="zh-cn_topic_0000002191984868_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002191984868_p148968533417"><a name="zh-cn_topic_0000002191984868_p148968533417"></a><a name="zh-cn_topic_0000002191984868_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002191984868_row5213113915410"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002191984868_p14111210255"><a name="zh-cn_topic_0000002191984868_p14111210255"></a><a name="zh-cn_topic_0000002191984868_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002191984868_p14411151012512"><a name="zh-cn_topic_0000002191984868_p14411151012512"></a><a name="zh-cn_topic_0000002191984868_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002191984868_p151469557531"><a name="zh-cn_topic_0000002191984868_p151469557531"></a><a name="zh-cn_topic_0000002191984868_p151469557531"></a>获取Hi2131E的(U)SIM卡PUK码是否被锁接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002191984868_row82131139142"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002191984868_p1141117102516"><a name="zh-cn_topic_0000002191984868_p1141117102516"></a><a name="zh-cn_topic_0000002191984868_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002191984868_p164111810052"><a name="zh-cn_topic_0000002191984868_p164111810052"></a><a name="zh-cn_topic_0000002191984868_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002191984868_p214655565313"><a name="zh-cn_topic_0000002191984868_p214655565313"></a><a name="zh-cn_topic_0000002191984868_p214655565313"></a>获取Hi2131E的(U)SIM卡PUK码是否被锁接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 获取Hi2131E的SIM卡IMSI号<a name="ZH-CN_TOPIC_0000002223667884"></a>

函数原型：int32\_t ril\_get\_imsi\(char\* imsi, size\_t len\);

函数功能：获取Hi2131E的\(U\)SIM卡IMSI号，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002227225221_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227225221_row12908457921"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002227225221_p563315171132"><a name="zh-cn_topic_0000002227225221_p563315171132"></a><a name="zh-cn_topic_0000002227225221_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002227225221_p1463351718318"><a name="zh-cn_topic_0000002227225221_p1463351718318"></a><a name="zh-cn_topic_0000002227225221_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002227225221_p16338179319"><a name="zh-cn_topic_0000002227225221_p16338179319"></a><a name="zh-cn_topic_0000002227225221_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002227225221_p1463316170313"><a name="zh-cn_topic_0000002227225221_p1463316170313"></a><a name="zh-cn_topic_0000002227225221_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227225221_row1830174962413"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002227225221_p13145102016548"><a name="zh-cn_topic_0000002227225221_p13145102016548"></a><a name="zh-cn_topic_0000002227225221_p13145102016548"></a>imsi</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002227225221_p149081857623"><a name="zh-cn_topic_0000002227225221_p149081857623"></a><a name="zh-cn_topic_0000002227225221_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002227225221_p29081571426"><a name="zh-cn_topic_0000002227225221_p29081571426"></a><a name="zh-cn_topic_0000002227225221_p29081571426"></a>char*</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002227225221_p15928142985417"><a name="zh-cn_topic_0000002227225221_p15928142985417"></a><a name="zh-cn_topic_0000002227225221_p15928142985417"></a>IMSI号，字符串格式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002227225221_row49086571217"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002227225221_p2014552035415"><a name="zh-cn_topic_0000002227225221_p2014552035415"></a><a name="zh-cn_topic_0000002227225221_p2014552035415"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002227225221_p169087570212"><a name="zh-cn_topic_0000002227225221_p169087570212"></a><a name="zh-cn_topic_0000002227225221_p169087570212"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002227225221_p490811571127"><a name="zh-cn_topic_0000002227225221_p490811571127"></a><a name="zh-cn_topic_0000002227225221_p490811571127"></a>size_t</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002227225221_p792832918543"><a name="zh-cn_topic_0000002227225221_p792832918543"></a><a name="zh-cn_topic_0000002227225221_p792832918543"></a>IMSI号长度。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002227225221_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227225221_row82122391941"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002227225221_p15896553644"><a name="zh-cn_topic_0000002227225221_p15896553644"></a><a name="zh-cn_topic_0000002227225221_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002227225221_p489612532411"><a name="zh-cn_topic_0000002227225221_p489612532411"></a><a name="zh-cn_topic_0000002227225221_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002227225221_p148968533417"><a name="zh-cn_topic_0000002227225221_p148968533417"></a><a name="zh-cn_topic_0000002227225221_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227225221_row5213113915410"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227225221_p14111210255"><a name="zh-cn_topic_0000002227225221_p14111210255"></a><a name="zh-cn_topic_0000002227225221_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227225221_p14411151012512"><a name="zh-cn_topic_0000002227225221_p14411151012512"></a><a name="zh-cn_topic_0000002227225221_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227225221_p151469557531"><a name="zh-cn_topic_0000002227225221_p151469557531"></a><a name="zh-cn_topic_0000002227225221_p151469557531"></a>获取Hi2131E的(U)SIM卡IMSI号接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002227225221_row82131139142"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227225221_p1141117102516"><a name="zh-cn_topic_0000002227225221_p1141117102516"></a><a name="zh-cn_topic_0000002227225221_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227225221_p164111810052"><a name="zh-cn_topic_0000002227225221_p164111810052"></a><a name="zh-cn_topic_0000002227225221_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227225221_p214655565313"><a name="zh-cn_topic_0000002227225221_p214655565313"></a><a name="zh-cn_topic_0000002227225221_p214655565313"></a>获取Hi2131E的(U)SIM卡IMSI号接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 获取Hi2131E的SIM卡msisdn<a name="ZH-CN_TOPIC_0000002258467657"></a>

函数原型：int32\_t ril\_get\_usim\_msisdn\(char\* msisdn, size\_t len\);

函数功能：获取Hi2131E的\(U\)SIM卡msisdn信息，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002227350745_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227350745_row12908457921"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002227350745_p563315171132"><a name="zh-cn_topic_0000002227350745_p563315171132"></a><a name="zh-cn_topic_0000002227350745_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002227350745_p1463351718318"><a name="zh-cn_topic_0000002227350745_p1463351718318"></a><a name="zh-cn_topic_0000002227350745_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002227350745_p16338179319"><a name="zh-cn_topic_0000002227350745_p16338179319"></a><a name="zh-cn_topic_0000002227350745_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002227350745_p1463316170313"><a name="zh-cn_topic_0000002227350745_p1463316170313"></a><a name="zh-cn_topic_0000002227350745_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227350745_row1830174962413"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002227350745_p13145102016548"><a name="zh-cn_topic_0000002227350745_p13145102016548"></a><a name="zh-cn_topic_0000002227350745_p13145102016548"></a>imsi</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002227350745_p149081857623"><a name="zh-cn_topic_0000002227350745_p149081857623"></a><a name="zh-cn_topic_0000002227350745_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002227350745_p29081571426"><a name="zh-cn_topic_0000002227350745_p29081571426"></a><a name="zh-cn_topic_0000002227350745_p29081571426"></a>char*</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002227350745_p15928142985417"><a name="zh-cn_topic_0000002227350745_p15928142985417"></a><a name="zh-cn_topic_0000002227350745_p15928142985417"></a>msisdn，字符串格式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002227350745_row49086571217"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002227350745_p2014552035415"><a name="zh-cn_topic_0000002227350745_p2014552035415"></a><a name="zh-cn_topic_0000002227350745_p2014552035415"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002227350745_p169087570212"><a name="zh-cn_topic_0000002227350745_p169087570212"></a><a name="zh-cn_topic_0000002227350745_p169087570212"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002227350745_p490811571127"><a name="zh-cn_topic_0000002227350745_p490811571127"></a><a name="zh-cn_topic_0000002227350745_p490811571127"></a>size_t</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002227350745_p792832918543"><a name="zh-cn_topic_0000002227350745_p792832918543"></a><a name="zh-cn_topic_0000002227350745_p792832918543"></a>msisdn长度。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002227350745_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227350745_row82122391941"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002227350745_p15896553644"><a name="zh-cn_topic_0000002227350745_p15896553644"></a><a name="zh-cn_topic_0000002227350745_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002227350745_p489612532411"><a name="zh-cn_topic_0000002227350745_p489612532411"></a><a name="zh-cn_topic_0000002227350745_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002227350745_p148968533417"><a name="zh-cn_topic_0000002227350745_p148968533417"></a><a name="zh-cn_topic_0000002227350745_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227350745_row5213113915410"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227350745_p14111210255"><a name="zh-cn_topic_0000002227350745_p14111210255"></a><a name="zh-cn_topic_0000002227350745_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227350745_p14411151012512"><a name="zh-cn_topic_0000002227350745_p14411151012512"></a><a name="zh-cn_topic_0000002227350745_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227350745_p151469557531"><a name="zh-cn_topic_0000002227350745_p151469557531"></a><a name="zh-cn_topic_0000002227350745_p151469557531"></a>获取Hi2131E的(U)SIM卡msisdn信息接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002227350745_row82131139142"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227350745_p1141117102516"><a name="zh-cn_topic_0000002227350745_p1141117102516"></a><a name="zh-cn_topic_0000002227350745_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227350745_p164111810052"><a name="zh-cn_topic_0000002227350745_p164111810052"></a><a name="zh-cn_topic_0000002227350745_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227350745_p214655565313"><a name="zh-cn_topic_0000002227350745_p214655565313"></a><a name="zh-cn_topic_0000002227350745_p214655565313"></a>获取Hi2131E的(U)SIM卡msisdn信息接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 获取CCHO返回结果<a name="ZH-CN_TOPIC_0000002566337577"></a>

函数原型：int32\_t ril\_get\_ccho\(char\* ccho, size\_t len, char\* ccho\_str, uint16\_t str\_len\)；

函数功能：获取Hi2131E的“AT+CCHO”命令返回结果。

**表 1**  参数说明

<a name="zh-cn_topic_0000002227350745_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227350745_row12908457921"><th class="cellrowborder" valign="top" width="19.12%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002227350745_p563315171132"><a name="zh-cn_topic_0000002227350745_p563315171132"></a><a name="zh-cn_topic_0000002227350745_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="24.08%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002227350745_p1463351718318"><a name="zh-cn_topic_0000002227350745_p1463351718318"></a><a name="zh-cn_topic_0000002227350745_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="22.939999999999998%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002227350745_p16338179319"><a name="zh-cn_topic_0000002227350745_p16338179319"></a><a name="zh-cn_topic_0000002227350745_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="33.86%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002227350745_p1463316170313"><a name="zh-cn_topic_0000002227350745_p1463316170313"></a><a name="zh-cn_topic_0000002227350745_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227350745_row1830174962413"><td class="cellrowborder" valign="top" width="19.12%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002227350745_p13145102016548"><a name="zh-cn_topic_0000002227350745_p13145102016548"></a><a name="zh-cn_topic_0000002227350745_p13145102016548"></a>ccho</p>
</td>
<td class="cellrowborder" valign="top" width="24.08%" headers="mcps1.2.5.1.2 "><p id="p164184403919"><a name="p164184403919"></a><a name="p164184403919"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="22.939999999999998%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002227350745_p29081571426"><a name="zh-cn_topic_0000002227350745_p29081571426"></a><a name="zh-cn_topic_0000002227350745_p29081571426"></a>char*</p>
</td>
<td class="cellrowborder" valign="top" width="33.86%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002227350745_p15928142985417"><a name="zh-cn_topic_0000002227350745_p15928142985417"></a><a name="zh-cn_topic_0000002227350745_p15928142985417"></a>CCHO命令返回结果，字符串格式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002227350745_row49086571217"><td class="cellrowborder" valign="top" width="19.12%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002227350745_p2014552035415"><a name="zh-cn_topic_0000002227350745_p2014552035415"></a><a name="zh-cn_topic_0000002227350745_p2014552035415"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="24.08%" headers="mcps1.2.5.1.2 "><p id="p133715133381"><a name="p133715133381"></a><a name="p133715133381"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="22.939999999999998%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002227350745_p490811571127"><a name="zh-cn_topic_0000002227350745_p490811571127"></a><a name="zh-cn_topic_0000002227350745_p490811571127"></a>size_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.86%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002227350745_p792832918543"><a name="zh-cn_topic_0000002227350745_p792832918543"></a><a name="zh-cn_topic_0000002227350745_p792832918543"></a>CCHO命令返回结果长度。</p>
</td>
</tr>
<tr id="row1336141310388"><td class="cellrowborder" valign="top" width="19.12%" headers="mcps1.2.5.1.1 "><p id="p10283162016402"><a name="p10283162016402"></a><a name="p10283162016402"></a>ccho_str</p>
</td>
<td class="cellrowborder" valign="top" width="24.08%" headers="mcps1.2.5.1.2 "><p id="p0329048133910"><a name="p0329048133910"></a><a name="p0329048133910"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="22.939999999999998%" headers="mcps1.2.5.1.3 "><p id="p5337161393810"><a name="p5337161393810"></a><a name="p5337161393810"></a>char*</p>
</td>
<td class="cellrowborder" valign="top" width="33.86%" headers="mcps1.2.5.1.4 "><p id="p18337613153816"><a name="p18337613153816"></a><a name="p18337613153816"></a>CCHO命令。</p>
</td>
</tr>
<tr id="row17955151389"><td class="cellrowborder" valign="top" width="19.12%" headers="mcps1.2.5.1.1 "><p id="p14171330104011"><a name="p14171330104011"></a><a name="p14171330104011"></a>str_len</p>
</td>
<td class="cellrowborder" valign="top" width="24.08%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002227350745_p169087570212"><a name="zh-cn_topic_0000002227350745_p169087570212"></a><a name="zh-cn_topic_0000002227350745_p169087570212"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="22.939999999999998%" headers="mcps1.2.5.1.3 "><p id="p191771338124011"><a name="p191771338124011"></a><a name="p191771338124011"></a>size_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.86%" headers="mcps1.2.5.1.4 "><p id="p1279601517381"><a name="p1279601517381"></a><a name="p1279601517381"></a>CCHO命令长度。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002227350745_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227350745_row82122391941"><th class="cellrowborder" valign="top" width="19.371937193719376%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002227350745_p15896553644"><a name="zh-cn_topic_0000002227350745_p15896553644"></a><a name="zh-cn_topic_0000002227350745_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="28.682868286828683%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002227350745_p489612532411"><a name="zh-cn_topic_0000002227350745_p489612532411"></a><a name="zh-cn_topic_0000002227350745_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="51.945194519451945%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002227350745_p148968533417"><a name="zh-cn_topic_0000002227350745_p148968533417"></a><a name="zh-cn_topic_0000002227350745_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227350745_row5213113915410"><td class="cellrowborder" valign="top" width="19.371937193719376%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227350745_p14111210255"><a name="zh-cn_topic_0000002227350745_p14111210255"></a><a name="zh-cn_topic_0000002227350745_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="28.682868286828683%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227350745_p14411151012512"><a name="zh-cn_topic_0000002227350745_p14411151012512"></a><a name="zh-cn_topic_0000002227350745_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="51.945194519451945%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227350745_p151469557531"><a name="zh-cn_topic_0000002227350745_p151469557531"></a><a name="zh-cn_topic_0000002227350745_p151469557531"></a>获取Hi2131E的“AT+CCHO”命令返回结果接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002227350745_row82131139142"><td class="cellrowborder" valign="top" width="19.371937193719376%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227350745_p1141117102516"><a name="zh-cn_topic_0000002227350745_p1141117102516"></a><a name="zh-cn_topic_0000002227350745_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="28.682868286828683%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227350745_p164111810052"><a name="zh-cn_topic_0000002227350745_p164111810052"></a><a name="zh-cn_topic_0000002227350745_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="51.945194519451945%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227350745_p214655565313"><a name="zh-cn_topic_0000002227350745_p214655565313"></a><a name="zh-cn_topic_0000002227350745_p214655565313"></a>获取Hi2131E的“AT+CCHO”命令返回结果接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

### 小区信息<a name="ZH-CN_TOPIC_0000002509848179"></a>

-   **[查询当前小区信号状态](#ZH-CN_TOPIC_0000002477648212)**  

-   **[查询当前小区信息](#ZH-CN_TOPIC_0000002477808186)**  

-   **[查询邻区信息](#ZH-CN_TOPIC_0000002509728153)**  

-   **[查询网络下行速率](#ZH-CN_TOPIC_0000002509848181)**  

-   **[查询网络上行速率](#ZH-CN_TOPIC_0000002477648214)**  

-   **[查询上行发数状态](#ZH-CN_TOPIC_0000002477808188)**  

#### 查询当前小区信号状态<a name="ZH-CN_TOPIC_0000002477648212"></a>

函数原型：int32\_t ril\_get\_radio\_signal\_info\(soc\_ue\_cell\_radio\_info \*signal\_info\);

函数功能：获取Hi2131E的当前小区信号状态，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002311124532_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002311124532_row12908457921"><th class="cellrowborder" valign="top" width="16.43%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002311124532_p563315171132"><a name="zh-cn_topic_0000002311124532_p563315171132"></a><a name="zh-cn_topic_0000002311124532_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="22.650000000000002%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002311124532_p1463351718318"><a name="zh-cn_topic_0000002311124532_p1463351718318"></a><a name="zh-cn_topic_0000002311124532_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="26.71%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002311124532_p16338179319"><a name="zh-cn_topic_0000002311124532_p16338179319"></a><a name="zh-cn_topic_0000002311124532_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="34.21%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002311124532_p1463316170313"><a name="zh-cn_topic_0000002311124532_p1463316170313"></a><a name="zh-cn_topic_0000002311124532_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002311124532_row1830174962413"><td class="cellrowborder" valign="top" width="16.43%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002311124532_p13145102016548"><a name="zh-cn_topic_0000002311124532_p13145102016548"></a><a name="zh-cn_topic_0000002311124532_p13145102016548"></a>signal_info</p>
</td>
<td class="cellrowborder" valign="top" width="22.650000000000002%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002311124532_p149081857623"><a name="zh-cn_topic_0000002311124532_p149081857623"></a><a name="zh-cn_topic_0000002311124532_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="26.71%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002311124532_p29081571426"><a name="zh-cn_topic_0000002311124532_p29081571426"></a><a name="zh-cn_topic_0000002311124532_p29081571426"></a>soc_ue_cell_radio_info*</p>
</td>
<td class="cellrowborder" valign="top" width="34.21%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002311124532_p12759105711313"><a name="zh-cn_topic_0000002311124532_p12759105711313"></a><a name="zh-cn_topic_0000002311124532_p12759105711313"></a>int16_t rsrp：参考信号接收功率。单位：1/10dB。</p>
<p id="zh-cn_topic_0000002311124532_p7759105715314"><a name="zh-cn_topic_0000002311124532_p7759105715314"></a><a name="zh-cn_topic_0000002311124532_p7759105715314"></a>int16_t rsrq：参考信号接收质量。单位：1/10dB。</p>
<p id="zh-cn_topic_0000002311124532_p137591157103113"><a name="zh-cn_topic_0000002311124532_p137591157103113"></a><a name="zh-cn_topic_0000002311124532_p137591157103113"></a>int16_t rssi：接收信号强度指示。单位：1/10dB。</p>
<p id="zh-cn_topic_0000002311124532_p975911571314"><a name="zh-cn_topic_0000002311124532_p975911571314"></a><a name="zh-cn_topic_0000002311124532_p975911571314"></a>int16_t snr：信噪比。单位：1/10dB。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002311124532_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002311124532_row82122391941"><th class="cellrowborder" valign="top" width="16.811681168116813%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002311124532_p15896553644"><a name="zh-cn_topic_0000002311124532_p15896553644"></a><a name="zh-cn_topic_0000002311124532_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="32.363236323632364%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002311124532_p489612532411"><a name="zh-cn_topic_0000002311124532_p489612532411"></a><a name="zh-cn_topic_0000002311124532_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="50.825082508250816%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002311124532_p148968533417"><a name="zh-cn_topic_0000002311124532_p148968533417"></a><a name="zh-cn_topic_0000002311124532_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002311124532_row5213113915410"><td class="cellrowborder" valign="top" width="16.811681168116813%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002311124532_p14111210255"><a name="zh-cn_topic_0000002311124532_p14111210255"></a><a name="zh-cn_topic_0000002311124532_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="32.363236323632364%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002311124532_p14411151012512"><a name="zh-cn_topic_0000002311124532_p14411151012512"></a><a name="zh-cn_topic_0000002311124532_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="50.825082508250816%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002311124532_p151469557531"><a name="zh-cn_topic_0000002311124532_p151469557531"></a><a name="zh-cn_topic_0000002311124532_p151469557531"></a>获取Hi2131E的(U)SIM卡msisdn信息接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002311124532_row82131139142"><td class="cellrowborder" valign="top" width="16.811681168116813%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002311124532_p1141117102516"><a name="zh-cn_topic_0000002311124532_p1141117102516"></a><a name="zh-cn_topic_0000002311124532_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="32.363236323632364%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002311124532_p164111810052"><a name="zh-cn_topic_0000002311124532_p164111810052"></a><a name="zh-cn_topic_0000002311124532_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="50.825082508250816%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002311124532_p214655565313"><a name="zh-cn_topic_0000002311124532_p214655565313"></a><a name="zh-cn_topic_0000002311124532_p214655565313"></a>获取Hi2131E的(U)SIM卡msisdn信息接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 查询当前小区信息<a name="ZH-CN_TOPIC_0000002477808186"></a>

函数原型：int32\_t ril\_get\_ue\_registration\_status\(soc\_ue\_cell\_info \*reg\_status\);

函数功能：获取Hi2131E的当前小区信息，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002345123793_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002345123793_row12908457921"><th class="cellrowborder" valign="top" width="16.77%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002345123793_p563315171132"><a name="zh-cn_topic_0000002345123793_p563315171132"></a><a name="zh-cn_topic_0000002345123793_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="22.650000000000002%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002345123793_p1463351718318"><a name="zh-cn_topic_0000002345123793_p1463351718318"></a><a name="zh-cn_topic_0000002345123793_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="26.369999999999997%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002345123793_p16338179319"><a name="zh-cn_topic_0000002345123793_p16338179319"></a><a name="zh-cn_topic_0000002345123793_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="34.21%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002345123793_p1463316170313"><a name="zh-cn_topic_0000002345123793_p1463316170313"></a><a name="zh-cn_topic_0000002345123793_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002345123793_row1830174962413"><td class="cellrowborder" valign="top" width="16.77%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002345123793_p13145102016548"><a name="zh-cn_topic_0000002345123793_p13145102016548"></a><a name="zh-cn_topic_0000002345123793_p13145102016548"></a>reg_status</p>
</td>
<td class="cellrowborder" valign="top" width="22.650000000000002%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002345123793_p149081857623"><a name="zh-cn_topic_0000002345123793_p149081857623"></a><a name="zh-cn_topic_0000002345123793_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="26.369999999999997%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002345123793_p29081571426"><a name="zh-cn_topic_0000002345123793_p29081571426"></a><a name="zh-cn_topic_0000002345123793_p29081571426"></a>soc_ue_cell_info *</p>
</td>
<td class="cellrowborder" valign="top" width="34.21%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002345123793_p13451848132414"><a name="zh-cn_topic_0000002345123793_p13451848132414"></a><a name="zh-cn_topic_0000002345123793_p13451848132414"></a>uint8_t version</p>
<p id="zh-cn_topic_0000002345123793_p18455483245"><a name="zh-cn_topic_0000002345123793_p18455483245"></a><a name="zh-cn_topic_0000002345123793_p18455483245"></a>bool primary_cell：0，搜索到的邻近小区；1，当前服务小区。</p>
<p id="zh-cn_topic_0000002345123793_p045648162410"><a name="zh-cn_topic_0000002345123793_p045648162410"></a><a name="zh-cn_topic_0000002345123793_p045648162410"></a>bool valid：0，无效；1，有效。</p>
<p id="zh-cn_topic_0000002345123793_p54584814247"><a name="zh-cn_topic_0000002345123793_p54584814247"></a><a name="zh-cn_topic_0000002345123793_p54584814247"></a>uint16_t pci：物理小区标识。范围：0~503。</p>
<p id="zh-cn_topic_0000002345123793_p34664810249"><a name="zh-cn_topic_0000002345123793_p34664810249"></a><a name="zh-cn_topic_0000002345123793_p34664810249"></a>uint32_t earfcn：E-UTRA绝对射频信道号。</p>
<p id="zh-cn_topic_0000002345123793_p12759105711313"><a name="zh-cn_topic_0000002345123793_p12759105711313"></a><a name="zh-cn_topic_0000002345123793_p12759105711313"></a>int16_t rsrp：参考信号接收功率。单位：1/10dB。</p>
<p id="zh-cn_topic_0000002345123793_p7759105715314"><a name="zh-cn_topic_0000002345123793_p7759105715314"></a><a name="zh-cn_topic_0000002345123793_p7759105715314"></a>int16_t rsrq：参考信号接收质量。单位：1/10dB。</p>
<p id="zh-cn_topic_0000002345123793_p137591157103113"><a name="zh-cn_topic_0000002345123793_p137591157103113"></a><a name="zh-cn_topic_0000002345123793_p137591157103113"></a>int16_t rssi：接收信号强度指示。单位：1/10dB。</p>
<p id="zh-cn_topic_0000002345123793_p975911571314"><a name="zh-cn_topic_0000002345123793_p975911571314"></a><a name="zh-cn_topic_0000002345123793_p975911571314"></a>int16_t snr：信噪比。单位：1/10dB。</p>
<p id="zh-cn_topic_0000002345123793_p84617484243"><a name="zh-cn_topic_0000002345123793_p84617484243"></a><a name="zh-cn_topic_0000002345123793_p84617484243"></a>int16_t srxlev：小区选择接收电平值。单位：dB。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002345123793_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002345123793_row82122391941"><th class="cellrowborder" valign="top" width="17.47174717471747%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002345123793_p15896553644"><a name="zh-cn_topic_0000002345123793_p15896553644"></a><a name="zh-cn_topic_0000002345123793_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="31.6031603160316%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002345123793_p489612532411"><a name="zh-cn_topic_0000002345123793_p489612532411"></a><a name="zh-cn_topic_0000002345123793_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="50.92509250925092%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002345123793_p148968533417"><a name="zh-cn_topic_0000002345123793_p148968533417"></a><a name="zh-cn_topic_0000002345123793_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002345123793_row5213113915410"><td class="cellrowborder" valign="top" width="17.47174717471747%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002345123793_p14111210255"><a name="zh-cn_topic_0000002345123793_p14111210255"></a><a name="zh-cn_topic_0000002345123793_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="31.6031603160316%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002345123793_p14411151012512"><a name="zh-cn_topic_0000002345123793_p14411151012512"></a><a name="zh-cn_topic_0000002345123793_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="50.92509250925092%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002345123793_p151469557531"><a name="zh-cn_topic_0000002345123793_p151469557531"></a><a name="zh-cn_topic_0000002345123793_p151469557531"></a>获取Hi2131E的(U)SIM卡msisdn信息接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002345123793_row82131139142"><td class="cellrowborder" valign="top" width="17.47174717471747%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002345123793_p1141117102516"><a name="zh-cn_topic_0000002345123793_p1141117102516"></a><a name="zh-cn_topic_0000002345123793_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="31.6031603160316%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002345123793_p164111810052"><a name="zh-cn_topic_0000002345123793_p164111810052"></a><a name="zh-cn_topic_0000002345123793_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="50.92509250925092%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002345123793_p214655565313"><a name="zh-cn_topic_0000002345123793_p214655565313"></a><a name="zh-cn_topic_0000002345123793_p214655565313"></a>获取Hi2131E的(U)SIM卡msisdn信息接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 查询邻区信息<a name="ZH-CN_TOPIC_0000002509728153"></a>

函数原型：int32\_t ril\_get\_ue\_neighbor\_cell\_info\(soc\_ue\_neighbor\_info \*neighbor\_info\);

函数功能：获取Hi2131E的临区信息，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002345003597_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002345003597_row12908457921"><th class="cellrowborder" valign="top" width="17.96%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002345003597_p563315171132"><a name="zh-cn_topic_0000002345003597_p563315171132"></a><a name="zh-cn_topic_0000002345003597_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="21.57%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002345003597_p1463351718318"><a name="zh-cn_topic_0000002345003597_p1463351718318"></a><a name="zh-cn_topic_0000002345003597_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="27.43%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002345003597_p16338179319"><a name="zh-cn_topic_0000002345003597_p16338179319"></a><a name="zh-cn_topic_0000002345003597_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="33.040000000000006%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002345003597_p1463316170313"><a name="zh-cn_topic_0000002345003597_p1463316170313"></a><a name="zh-cn_topic_0000002345003597_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002345003597_row1830174962413"><td class="cellrowborder" valign="top" width="17.96%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002345003597_p13145102016548"><a name="zh-cn_topic_0000002345003597_p13145102016548"></a><a name="zh-cn_topic_0000002345003597_p13145102016548"></a>neighbor_info</p>
</td>
<td class="cellrowborder" valign="top" width="21.57%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002345003597_p149081857623"><a name="zh-cn_topic_0000002345003597_p149081857623"></a><a name="zh-cn_topic_0000002345003597_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="27.43%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002345003597_p29081571426"><a name="zh-cn_topic_0000002345003597_p29081571426"></a><a name="zh-cn_topic_0000002345003597_p29081571426"></a>soc_ue_neighbor_info  *</p>
</td>
<td class="cellrowborder" valign="top" width="33.040000000000006%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002345003597_p158432617372"><a name="zh-cn_topic_0000002345003597_p158432617372"></a><a name="zh-cn_topic_0000002345003597_p158432617372"></a>uint8_t neighbor_cell_num：邻区个数。</p>
<p id="zh-cn_topic_0000002345003597_p6843964374"><a name="zh-cn_topic_0000002345003597_p6843964374"></a><a name="zh-cn_topic_0000002345003597_p6843964374"></a>soc_ue_cell_info cell_info[10]：小区信息。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002345003597_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002345003597_row82122391941"><th class="cellrowborder" valign="top" width="18.011801180118013%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002345003597_p15896553644"><a name="zh-cn_topic_0000002345003597_p15896553644"></a><a name="zh-cn_topic_0000002345003597_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="35.02350235023503%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002345003597_p489612532411"><a name="zh-cn_topic_0000002345003597_p489612532411"></a><a name="zh-cn_topic_0000002345003597_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="46.96469646964697%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002345003597_p148968533417"><a name="zh-cn_topic_0000002345003597_p148968533417"></a><a name="zh-cn_topic_0000002345003597_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002345003597_row5213113915410"><td class="cellrowborder" valign="top" width="18.011801180118013%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002345003597_p14111210255"><a name="zh-cn_topic_0000002345003597_p14111210255"></a><a name="zh-cn_topic_0000002345003597_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="35.02350235023503%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002345003597_p14411151012512"><a name="zh-cn_topic_0000002345003597_p14411151012512"></a><a name="zh-cn_topic_0000002345003597_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="46.96469646964697%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002345003597_p151469557531"><a name="zh-cn_topic_0000002345003597_p151469557531"></a><a name="zh-cn_topic_0000002345003597_p151469557531"></a>获取Hi2131E的临区信息接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002345003597_row82131139142"><td class="cellrowborder" valign="top" width="18.011801180118013%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002345003597_p1141117102516"><a name="zh-cn_topic_0000002345003597_p1141117102516"></a><a name="zh-cn_topic_0000002345003597_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="35.02350235023503%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002345003597_p164111810052"><a name="zh-cn_topic_0000002345003597_p164111810052"></a><a name="zh-cn_topic_0000002345003597_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="46.96469646964697%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002345003597_p214655565313"><a name="zh-cn_topic_0000002345003597_p214655565313"></a><a name="zh-cn_topic_0000002345003597_p214655565313"></a>获取Hi2131E的临区信息接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 查询网络下行速率<a name="ZH-CN_TOPIC_0000002509848181"></a>

函数原型：int32\_t ril\_get\_network\_dl\_rate\(uint32\_t \*dl\_rate\)

函数功能：获取Hi2131E当前网络下行速率信息，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002310964784_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002310964784_row12908457921"><th class="cellrowborder" valign="top" width="18.14%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002310964784_p563315171132"><a name="zh-cn_topic_0000002310964784_p563315171132"></a><a name="zh-cn_topic_0000002310964784_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="23.01%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002310964784_p1463351718318"><a name="zh-cn_topic_0000002310964784_p1463351718318"></a><a name="zh-cn_topic_0000002310964784_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="26.8%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002310964784_p16338179319"><a name="zh-cn_topic_0000002310964784_p16338179319"></a><a name="zh-cn_topic_0000002310964784_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="32.05%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002310964784_p1463316170313"><a name="zh-cn_topic_0000002310964784_p1463316170313"></a><a name="zh-cn_topic_0000002310964784_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002310964784_row1830174962413"><td class="cellrowborder" valign="top" width="18.14%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002310964784_p13145102016548"><a name="zh-cn_topic_0000002310964784_p13145102016548"></a><a name="zh-cn_topic_0000002310964784_p13145102016548"></a>dl_rate</p>
</td>
<td class="cellrowborder" valign="top" width="23.01%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002310964784_p149081857623"><a name="zh-cn_topic_0000002310964784_p149081857623"></a><a name="zh-cn_topic_0000002310964784_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="26.8%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002310964784_p29081571426"><a name="zh-cn_topic_0000002310964784_p29081571426"></a><a name="zh-cn_topic_0000002310964784_p29081571426"></a>uint32_t *</p>
</td>
<td class="cellrowborder" valign="top" width="32.05%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002310964784_p1360415323477"><a name="zh-cn_topic_0000002310964784_p1360415323477"></a><a name="zh-cn_topic_0000002310964784_p1360415323477"></a>下行实际速率。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002310964784_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002310964784_row82122391941"><th class="cellrowborder" valign="top" width="18.28182818281828%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002310964784_p15896553644"><a name="zh-cn_topic_0000002310964784_p15896553644"></a><a name="zh-cn_topic_0000002310964784_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="35.74357435743574%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002310964784_p489612532411"><a name="zh-cn_topic_0000002310964784_p489612532411"></a><a name="zh-cn_topic_0000002310964784_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="45.97459745974597%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002310964784_p148968533417"><a name="zh-cn_topic_0000002310964784_p148968533417"></a><a name="zh-cn_topic_0000002310964784_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002310964784_row5213113915410"><td class="cellrowborder" valign="top" width="18.28182818281828%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002310964784_p14111210255"><a name="zh-cn_topic_0000002310964784_p14111210255"></a><a name="zh-cn_topic_0000002310964784_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="35.74357435743574%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002310964784_p14411151012512"><a name="zh-cn_topic_0000002310964784_p14411151012512"></a><a name="zh-cn_topic_0000002310964784_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="45.97459745974597%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002310964784_p151469557531"><a name="zh-cn_topic_0000002310964784_p151469557531"></a><a name="zh-cn_topic_0000002310964784_p151469557531"></a>获取Hi2131E当前网络下行速率信息成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002310964784_row82131139142"><td class="cellrowborder" valign="top" width="18.28182818281828%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002310964784_p1141117102516"><a name="zh-cn_topic_0000002310964784_p1141117102516"></a><a name="zh-cn_topic_0000002310964784_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="35.74357435743574%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002310964784_p164111810052"><a name="zh-cn_topic_0000002310964784_p164111810052"></a><a name="zh-cn_topic_0000002310964784_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="45.97459745974597%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002310964784_p214655565313"><a name="zh-cn_topic_0000002310964784_p214655565313"></a><a name="zh-cn_topic_0000002310964784_p214655565313"></a>获取Hi2131E当前网络下行速率信息失败。</p>
</td>
</tr>
</tbody>
</table>

#### 查询网络上行速率<a name="ZH-CN_TOPIC_0000002477648214"></a>

函数原型：int32\_t ril\_get\_network\_ul\_rate\(uint32\_t \*ul\_rate\)

函数功能：获取Hi2131E当前网络上行速率信息，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002311124536_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002311124536_row12908457921"><th class="cellrowborder" valign="top" width="19.49%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002311124536_p563315171132"><a name="zh-cn_topic_0000002311124536_p563315171132"></a><a name="zh-cn_topic_0000002311124536_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="22.830000000000002%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002311124536_p1463351718318"><a name="zh-cn_topic_0000002311124536_p1463351718318"></a><a name="zh-cn_topic_0000002311124536_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="25.16%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002311124536_p16338179319"><a name="zh-cn_topic_0000002311124536_p16338179319"></a><a name="zh-cn_topic_0000002311124536_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="32.519999999999996%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002311124536_p1463316170313"><a name="zh-cn_topic_0000002311124536_p1463316170313"></a><a name="zh-cn_topic_0000002311124536_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002311124536_row1830174962413"><td class="cellrowborder" valign="top" width="19.49%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002311124536_p19504438124014"><a name="zh-cn_topic_0000002311124536_p19504438124014"></a><a name="zh-cn_topic_0000002311124536_p19504438124014"></a>ul_rate</p>
</td>
<td class="cellrowborder" valign="top" width="22.830000000000002%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002311124536_p149081857623"><a name="zh-cn_topic_0000002311124536_p149081857623"></a><a name="zh-cn_topic_0000002311124536_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="25.16%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002311124536_p29081571426"><a name="zh-cn_topic_0000002311124536_p29081571426"></a><a name="zh-cn_topic_0000002311124536_p29081571426"></a>uint32_t *</p>
</td>
<td class="cellrowborder" valign="top" width="32.519999999999996%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002311124536_p18894105417409"><a name="zh-cn_topic_0000002311124536_p18894105417409"></a><a name="zh-cn_topic_0000002311124536_p18894105417409"></a>上行实际速率。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002311124536_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002311124536_row82122391941"><th class="cellrowborder" valign="top" width="19.621962196219624%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002311124536_p15896553644"><a name="zh-cn_topic_0000002311124536_p15896553644"></a><a name="zh-cn_topic_0000002311124536_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="35.21352135213521%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002311124536_p489612532411"><a name="zh-cn_topic_0000002311124536_p489612532411"></a><a name="zh-cn_topic_0000002311124536_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="45.16451645164516%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002311124536_p148968533417"><a name="zh-cn_topic_0000002311124536_p148968533417"></a><a name="zh-cn_topic_0000002311124536_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002311124536_row5213113915410"><td class="cellrowborder" valign="top" width="19.621962196219624%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002311124536_p14111210255"><a name="zh-cn_topic_0000002311124536_p14111210255"></a><a name="zh-cn_topic_0000002311124536_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="35.21352135213521%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002311124536_p14411151012512"><a name="zh-cn_topic_0000002311124536_p14411151012512"></a><a name="zh-cn_topic_0000002311124536_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="45.16451645164516%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002311124536_p151469557531"><a name="zh-cn_topic_0000002311124536_p151469557531"></a><a name="zh-cn_topic_0000002311124536_p151469557531"></a>获取Hi2131E当前网络上行速率信息成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002311124536_row82131139142"><td class="cellrowborder" valign="top" width="19.621962196219624%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002311124536_p1141117102516"><a name="zh-cn_topic_0000002311124536_p1141117102516"></a><a name="zh-cn_topic_0000002311124536_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="35.21352135213521%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002311124536_p164111810052"><a name="zh-cn_topic_0000002311124536_p164111810052"></a><a name="zh-cn_topic_0000002311124536_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="45.16451645164516%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002311124536_p214655565313"><a name="zh-cn_topic_0000002311124536_p214655565313"></a><a name="zh-cn_topic_0000002311124536_p214655565313"></a>获取Hi2131E当前网络上行速率信息失败。</p>
</td>
</tr>
</tbody>
</table>

#### 查询上行发数状态<a name="ZH-CN_TOPIC_0000002477808188"></a>

函数原型：int32\_t ril\_get\_network\_ul\_tx\_stats\(bool \*status\)

函数功能：获取Hi2131E上行发数状态，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002345123797_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002345123797_row12908457921"><th class="cellrowborder" valign="top" width="19.49%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002345123797_p563315171132"><a name="zh-cn_topic_0000002345123797_p563315171132"></a><a name="zh-cn_topic_0000002345123797_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="22.2%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002345123797_p1463351718318"><a name="zh-cn_topic_0000002345123797_p1463351718318"></a><a name="zh-cn_topic_0000002345123797_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="24.73%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002345123797_p16338179319"><a name="zh-cn_topic_0000002345123797_p16338179319"></a><a name="zh-cn_topic_0000002345123797_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="33.58%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002345123797_p1463316170313"><a name="zh-cn_topic_0000002345123797_p1463316170313"></a><a name="zh-cn_topic_0000002345123797_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002345123797_row1830174962413"><td class="cellrowborder" valign="top" width="19.49%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002345123797_p13145102016548"><a name="zh-cn_topic_0000002345123797_p13145102016548"></a><a name="zh-cn_topic_0000002345123797_p13145102016548"></a>status</p>
</td>
<td class="cellrowborder" valign="top" width="22.2%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002345123797_p149081857623"><a name="zh-cn_topic_0000002345123797_p149081857623"></a><a name="zh-cn_topic_0000002345123797_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="24.73%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002345123797_p29081571426"><a name="zh-cn_topic_0000002345123797_p29081571426"></a><a name="zh-cn_topic_0000002345123797_p29081571426"></a>bool*</p>
</td>
<td class="cellrowborder" valign="top" width="33.58%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002345123797_p723114844813"><a name="zh-cn_topic_0000002345123797_p723114844813"></a><a name="zh-cn_topic_0000002345123797_p723114844813"></a>true：当前处于发数状态；</p>
<p id="zh-cn_topic_0000002345123797_p17502185754818"><a name="zh-cn_topic_0000002345123797_p17502185754818"></a><a name="zh-cn_topic_0000002345123797_p17502185754818"></a>false：当前未处于发数状态。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002345123797_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002345123797_row82122391941"><th class="cellrowborder" valign="top" width="19.801980198019802%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002345123797_p15896553644"><a name="zh-cn_topic_0000002345123797_p15896553644"></a><a name="zh-cn_topic_0000002345123797_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="35.21352135213521%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002345123797_p489612532411"><a name="zh-cn_topic_0000002345123797_p489612532411"></a><a name="zh-cn_topic_0000002345123797_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="44.984498449844985%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002345123797_p148968533417"><a name="zh-cn_topic_0000002345123797_p148968533417"></a><a name="zh-cn_topic_0000002345123797_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002345123797_row5213113915410"><td class="cellrowborder" valign="top" width="19.801980198019802%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002345123797_p14111210255"><a name="zh-cn_topic_0000002345123797_p14111210255"></a><a name="zh-cn_topic_0000002345123797_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="35.21352135213521%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002345123797_p14411151012512"><a name="zh-cn_topic_0000002345123797_p14411151012512"></a><a name="zh-cn_topic_0000002345123797_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="44.984498449844985%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002345123797_p151469557531"><a name="zh-cn_topic_0000002345123797_p151469557531"></a><a name="zh-cn_topic_0000002345123797_p151469557531"></a>获取Hi2131E上行发数状态成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002345123797_row82131139142"><td class="cellrowborder" valign="top" width="19.801980198019802%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002345123797_p1141117102516"><a name="zh-cn_topic_0000002345123797_p1141117102516"></a><a name="zh-cn_topic_0000002345123797_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="35.21352135213521%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002345123797_p164111810052"><a name="zh-cn_topic_0000002345123797_p164111810052"></a><a name="zh-cn_topic_0000002345123797_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="44.984498449844985%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002345123797_p214655565313"><a name="zh-cn_topic_0000002345123797_p214655565313"></a><a name="zh-cn_topic_0000002345123797_p214655565313"></a>获取Hi2131E上行发数状态失败。</p>
</td>
</tr>
</tbody>
</table>

### 驻网状态接口<a name="ZH-CN_TOPIC_0000002258627549"></a>

-   **[获取驻网状态](#ZH-CN_TOPIC_0000002223508048)**  

-   **[显示运营商信息](#ZH-CN_TOPIC_0000002223667888)**  

-   **[获取IMS驻网状态](#ZH-CN_TOPIC_0000002258467669)**  

-   **[是否PDP驻网，显示IP地址](#ZH-CN_TOPIC_0000002258627553)**  

-   **[手动搜网，并显示可选网络](#ZH-CN_TOPIC_0000002223508052)**  

-   **[设置快速休眠时间](#ZH-CN_TOPIC_0000002533154426)**  

#### 获取驻网状态<a name="ZH-CN_TOPIC_0000002223508048"></a>

函数原型：int32\_t ril\_get\_radio\_reg\_status\(uint8\_t \*status\);

函数功能：获取Hi2131E的驻网状态，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002227896305_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227896305_row12908457921"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002227896305_p563315171132"><a name="zh-cn_topic_0000002227896305_p563315171132"></a><a name="zh-cn_topic_0000002227896305_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002227896305_p1463351718318"><a name="zh-cn_topic_0000002227896305_p1463351718318"></a><a name="zh-cn_topic_0000002227896305_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002227896305_p16338179319"><a name="zh-cn_topic_0000002227896305_p16338179319"></a><a name="zh-cn_topic_0000002227896305_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002227896305_p1463316170313"><a name="zh-cn_topic_0000002227896305_p1463316170313"></a><a name="zh-cn_topic_0000002227896305_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227896305_row1830174962413"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002227896305_p13145102016548"><a name="zh-cn_topic_0000002227896305_p13145102016548"></a><a name="zh-cn_topic_0000002227896305_p13145102016548"></a>status</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002227896305_p149081857623"><a name="zh-cn_topic_0000002227896305_p149081857623"></a><a name="zh-cn_topic_0000002227896305_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002227896305_p29081571426"><a name="zh-cn_topic_0000002227896305_p29081571426"></a><a name="zh-cn_topic_0000002227896305_p29081571426"></a>uint8_t*</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p1744917478444"><a name="p1744917478444"></a><a name="p1744917478444"></a>0：EMM detached。</p>
<p id="zh-cn_topic_0000002227896305_p15928142985417"><a name="zh-cn_topic_0000002227896305_p15928142985417"></a><a name="zh-cn_topic_0000002227896305_p15928142985417"></a>1：EMM attached。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002227896305_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227896305_row82122391941"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002227896305_p15896553644"><a name="zh-cn_topic_0000002227896305_p15896553644"></a><a name="zh-cn_topic_0000002227896305_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="32.22322232223222%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002227896305_p489612532411"><a name="zh-cn_topic_0000002227896305_p489612532411"></a><a name="zh-cn_topic_0000002227896305_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="34.44344434443444%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002227896305_p148968533417"><a name="zh-cn_topic_0000002227896305_p148968533417"></a><a name="zh-cn_topic_0000002227896305_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227896305_row5213113915410"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227896305_p14111210255"><a name="zh-cn_topic_0000002227896305_p14111210255"></a><a name="zh-cn_topic_0000002227896305_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="32.22322232223222%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227896305_p14411151012512"><a name="zh-cn_topic_0000002227896305_p14411151012512"></a><a name="zh-cn_topic_0000002227896305_p14411151012512"></a>uint8_t</p>
</td>
<td class="cellrowborder" valign="top" width="34.44344434443444%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227896305_p151469557531"><a name="zh-cn_topic_0000002227896305_p151469557531"></a><a name="zh-cn_topic_0000002227896305_p151469557531"></a>获取Hi2131E的驻网状态接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002227896305_row82131139142"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227896305_p1141117102516"><a name="zh-cn_topic_0000002227896305_p1141117102516"></a><a name="zh-cn_topic_0000002227896305_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="32.22322232223222%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227896305_p164111810052"><a name="zh-cn_topic_0000002227896305_p164111810052"></a><a name="zh-cn_topic_0000002227896305_p164111810052"></a>uint8_t</p>
</td>
<td class="cellrowborder" valign="top" width="34.44344434443444%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227896305_p214655565313"><a name="zh-cn_topic_0000002227896305_p214655565313"></a><a name="zh-cn_topic_0000002227896305_p214655565313"></a>获取Hi2131E的驻网状态接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 显示运营商信息<a name="ZH-CN_TOPIC_0000002223667888"></a>

函数原型：int32\_t ril\_get\_radio\_operator\_info\(ril\_operator\_info \*operator\_info\);

函数功能：显示Hi2131E的运营商信息，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002192250848_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002192250848_row12908457921"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002192250848_p563315171132"><a name="zh-cn_topic_0000002192250848_p563315171132"></a><a name="zh-cn_topic_0000002192250848_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002192250848_p1463351718318"><a name="zh-cn_topic_0000002192250848_p1463351718318"></a><a name="zh-cn_topic_0000002192250848_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002192250848_p16338179319"><a name="zh-cn_topic_0000002192250848_p16338179319"></a><a name="zh-cn_topic_0000002192250848_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002192250848_p1463316170313"><a name="zh-cn_topic_0000002192250848_p1463316170313"></a><a name="zh-cn_topic_0000002192250848_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002192250848_row1830174962413"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002192250848_p13145102016548"><a name="zh-cn_topic_0000002192250848_p13145102016548"></a><a name="zh-cn_topic_0000002192250848_p13145102016548"></a>operator_info</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002192250848_p149081857623"><a name="zh-cn_topic_0000002192250848_p149081857623"></a><a name="zh-cn_topic_0000002192250848_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002192250848_p29081571426"><a name="zh-cn_topic_0000002192250848_p29081571426"></a><a name="zh-cn_topic_0000002192250848_p29081571426"></a>ril_operator_info*</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002192250848_p15928142985417"><a name="zh-cn_topic_0000002192250848_p15928142985417"></a><a name="zh-cn_topic_0000002192250848_p15928142985417"></a>运营商信息。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  ril\_operator\_info结构体说明

<a name="zh-cn_topic_0000002192250848_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002192250848_row82122391941"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002192250848_p15896553644"><a name="zh-cn_topic_0000002192250848_p15896553644"></a><a name="zh-cn_topic_0000002192250848_p15896553644"></a>字段</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002192250848_p489612532411"><a name="zh-cn_topic_0000002192250848_p489612532411"></a><a name="zh-cn_topic_0000002192250848_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002192250848_p148968533417"><a name="zh-cn_topic_0000002192250848_p148968533417"></a><a name="zh-cn_topic_0000002192250848_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002192250848_row1450319093113"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002192250848_p779118115313"><a name="zh-cn_topic_0000002192250848_p779118115313"></a><a name="zh-cn_topic_0000002192250848_p779118115313"></a>numeric</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002192250848_p14584151815317"><a name="zh-cn_topic_0000002192250848_p14584151815317"></a><a name="zh-cn_topic_0000002192250848_p14584151815317"></a>char[20]</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p195036013310"><a name="p195036013310"></a><a name="p195036013310"></a>数字格式的运营商信息。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002192250848_row5213113915410"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002192250848_p154251893111"><a name="zh-cn_topic_0000002192250848_p154251893111"></a><a name="zh-cn_topic_0000002192250848_p154251893111"></a>name_short</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002192250848_p14411151012512"><a name="zh-cn_topic_0000002192250848_p14411151012512"></a><a name="zh-cn_topic_0000002192250848_p14411151012512"></a>char[20]</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p552874894216"><a name="p552874894216"></a><a name="p552874894216"></a>短字符串格式的运营商信息。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002192250848_row82131139142"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002192250848_p86435132312"><a name="zh-cn_topic_0000002192250848_p86435132312"></a><a name="zh-cn_topic_0000002192250848_p86435132312"></a>name_long</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002192250848_p164111810052"><a name="zh-cn_topic_0000002192250848_p164111810052"></a><a name="zh-cn_topic_0000002192250848_p164111810052"></a>char[20]</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p551364817420"><a name="p551364817420"></a><a name="p551364817420"></a>长字符串格式的运营商信息。</p>
</td>
</tr>
</tbody>
</table>

**表 3**  返回值说明

<a name="zh-cn_topic_0000002192250848_table1247403412435"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002192250848_row847403414435"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002192250848_p20474173418433"><a name="zh-cn_topic_0000002192250848_p20474173418433"></a><a name="zh-cn_topic_0000002192250848_p20474173418433"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002192250848_p1647414340435"><a name="zh-cn_topic_0000002192250848_p1647414340435"></a><a name="zh-cn_topic_0000002192250848_p1647414340435"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002192250848_p347412349437"><a name="zh-cn_topic_0000002192250848_p347412349437"></a><a name="zh-cn_topic_0000002192250848_p347412349437"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002192250848_row147423474312"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002192250848_p14111210255"><a name="zh-cn_topic_0000002192250848_p14111210255"></a><a name="zh-cn_topic_0000002192250848_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002192250848_p6474113454318"><a name="zh-cn_topic_0000002192250848_p6474113454318"></a><a name="zh-cn_topic_0000002192250848_p6474113454318"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002192250848_p151469557531"><a name="zh-cn_topic_0000002192250848_p151469557531"></a><a name="zh-cn_topic_0000002192250848_p151469557531"></a>显示运营商信息接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002192250848_row11474134204312"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002192250848_p1141117102516"><a name="zh-cn_topic_0000002192250848_p1141117102516"></a><a name="zh-cn_topic_0000002192250848_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002192250848_p104740348434"><a name="zh-cn_topic_0000002192250848_p104740348434"></a><a name="zh-cn_topic_0000002192250848_p104740348434"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002192250848_p214655565313"><a name="zh-cn_topic_0000002192250848_p214655565313"></a><a name="zh-cn_topic_0000002192250848_p214655565313"></a>显示运营商信息接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 获取IMS驻网状态<a name="ZH-CN_TOPIC_0000002258467669"></a>

函数原型：int32\_t ril\_get\_radio\_ims\_reg\(bool \*status\);

函数功能：获取IMS的驻网状态，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002227770753_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227770753_row12908457921"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002227770753_p563315171132"><a name="zh-cn_topic_0000002227770753_p563315171132"></a><a name="zh-cn_topic_0000002227770753_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002227770753_p1463351718318"><a name="zh-cn_topic_0000002227770753_p1463351718318"></a><a name="zh-cn_topic_0000002227770753_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002227770753_p16338179319"><a name="zh-cn_topic_0000002227770753_p16338179319"></a><a name="zh-cn_topic_0000002227770753_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002227770753_p1463316170313"><a name="zh-cn_topic_0000002227770753_p1463316170313"></a><a name="zh-cn_topic_0000002227770753_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227770753_row1830174962413"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002227770753_p13145102016548"><a name="zh-cn_topic_0000002227770753_p13145102016548"></a><a name="zh-cn_topic_0000002227770753_p13145102016548"></a>status</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002227770753_p149081857623"><a name="zh-cn_topic_0000002227770753_p149081857623"></a><a name="zh-cn_topic_0000002227770753_p149081857623"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002227770753_p29081571426"><a name="zh-cn_topic_0000002227770753_p29081571426"></a><a name="zh-cn_topic_0000002227770753_p29081571426"></a>bool*</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002227770753_p15928142985417"><a name="zh-cn_topic_0000002227770753_p15928142985417"></a><a name="zh-cn_topic_0000002227770753_p15928142985417"></a>IMS注册状态。</p>
<p id="zh-cn_topic_0000002227770753_p26518341590"><a name="zh-cn_topic_0000002227770753_p26518341590"></a><a name="zh-cn_topic_0000002227770753_p26518341590"></a>true：已注册；</p>
<p id="zh-cn_topic_0000002227770753_p10324115704"><a name="zh-cn_topic_0000002227770753_p10324115704"></a><a name="zh-cn_topic_0000002227770753_p10324115704"></a>false：未注册。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002227770753_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227770753_row82122391941"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002227770753_p15896553644"><a name="zh-cn_topic_0000002227770753_p15896553644"></a><a name="zh-cn_topic_0000002227770753_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002227770753_p489612532411"><a name="zh-cn_topic_0000002227770753_p489612532411"></a><a name="zh-cn_topic_0000002227770753_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002227770753_p148968533417"><a name="zh-cn_topic_0000002227770753_p148968533417"></a><a name="zh-cn_topic_0000002227770753_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227770753_row5213113915410"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227770753_p14111210255"><a name="zh-cn_topic_0000002227770753_p14111210255"></a><a name="zh-cn_topic_0000002227770753_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227770753_p14411151012512"><a name="zh-cn_topic_0000002227770753_p14411151012512"></a><a name="zh-cn_topic_0000002227770753_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227770753_p151469557531"><a name="zh-cn_topic_0000002227770753_p151469557531"></a><a name="zh-cn_topic_0000002227770753_p151469557531"></a>获取IMS的驻网状态接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002227770753_row82131139142"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227770753_p1141117102516"><a name="zh-cn_topic_0000002227770753_p1141117102516"></a><a name="zh-cn_topic_0000002227770753_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227770753_p164111810052"><a name="zh-cn_topic_0000002227770753_p164111810052"></a><a name="zh-cn_topic_0000002227770753_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227770753_p214655565313"><a name="zh-cn_topic_0000002227770753_p214655565313"></a><a name="zh-cn_topic_0000002227770753_p214655565313"></a>获取IMS的驻网状态接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 是否PDP驻网，显示IP地址<a name="ZH-CN_TOPIC_0000002258627553"></a>

函数原型：int32\_t ril\_get\_ue\_addr\(ril\_ip\_addr\_info \*ip\_addr\_info\);

函数功能：获取PDP地址，核内交互（相当于执行：AT+CGPADDR）。

**表 1**  参数说明

<a name="table14125132611420"></a>
<table><thead align="left"><tr id="row152057269148"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p2020520264140"><a name="p2020520264140"></a><a name="p2020520264140"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p62051126141410"><a name="p62051126141410"></a><a name="p62051126141410"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p62051526141419"><a name="p62051526141419"></a><a name="p62051526141419"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p11205102651414"><a name="p11205102651414"></a><a name="p11205102651414"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row122051826111411"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p142051026151419"><a name="p142051026151419"></a><a name="p142051026151419"></a>ril_ip_addr_info</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p32051426111418"><a name="p32051426111418"></a><a name="p32051426111418"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p11205152616147"><a name="p11205152616147"></a><a name="p11205152616147"></a>ril_ip_addr_info</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p8205526121412"><a name="p8205526121412"></a><a name="p8205526121412"></a>IP地址信息。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  ril\_ip\_addr\_info结构体说明

<a name="table61341266145"></a>
<table><thead align="left"><tr id="row92051626171414"><th class="cellrowborder" valign="top" width="25.202520252025202%" id="mcps1.2.4.1.1"><p id="p4205126161416"><a name="p4205126161416"></a><a name="p4205126161416"></a>字段</p>
</th>
<th class="cellrowborder" valign="top" width="29.722972297229727%" id="mcps1.2.4.1.2"><p id="p162057263148"><a name="p162057263148"></a><a name="p162057263148"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="45.074507450745074%" id="mcps1.2.4.1.3"><p id="p720532613143"><a name="p720532613143"></a><a name="p720532613143"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row1120515269141"><td class="cellrowborder" valign="top" width="25.202520252025202%" headers="mcps1.2.4.1.1 "><p id="p920517263144"><a name="p920517263144"></a><a name="p920517263144"></a>ip_addr</p>
</td>
<td class="cellrowborder" valign="top" width="29.722972297229727%" headers="mcps1.2.4.1.2 "><p id="p22054269145"><a name="p22054269145"></a><a name="p22054269145"></a>ip_addr_t [2]</p>
</td>
<td class="cellrowborder" valign="top" width="45.074507450745074%" headers="mcps1.2.4.1.3 "><p id="p18205426121419"><a name="p18205426121419"></a><a name="p18205426121419"></a>IP地址信息。</p>
</td>
</tr>
<tr id="row152051726171410"><td class="cellrowborder" valign="top" width="25.202520252025202%" headers="mcps1.2.4.1.1 "><p id="p1820518265149"><a name="p1820518265149"></a><a name="p1820518265149"></a>ip_num</p>
</td>
<td class="cellrowborder" valign="top" width="29.722972297229727%" headers="mcps1.2.4.1.2 "><p id="p72057264147"><a name="p72057264147"></a><a name="p72057264147"></a>uint8_t</p>
</td>
<td class="cellrowborder" valign="top" width="45.074507450745074%" headers="mcps1.2.4.1.3 "><p id="p4205826151411"><a name="p4205826151411"></a><a name="p4205826151411"></a>IP数量。</p>
</td>
</tr>
</tbody>
</table>

**表 3**  ip\_addr\_t结构体说明

<a name="table1013712601413"></a>
<table><thead align="left"><tr id="row32051526121418"><th class="cellrowborder" valign="top" width="25.502550255025504%" id="mcps1.2.4.1.1"><p id="p5206526191419"><a name="p5206526191419"></a><a name="p5206526191419"></a>字段</p>
</th>
<th class="cellrowborder" valign="top" width="30.323032303230324%" id="mcps1.2.4.1.2"><p id="p32066264141"><a name="p32066264141"></a><a name="p32066264141"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="44.174417441744176%" id="mcps1.2.4.1.3"><p id="p220632621414"><a name="p220632621414"></a><a name="p220632621414"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row1220614268143"><td class="cellrowborder" valign="top" width="25.502550255025504%" headers="mcps1.2.4.1.1 "><p id="p102061226181410"><a name="p102061226181410"></a><a name="p102061226181410"></a>u_addr</p>
</td>
<td class="cellrowborder" valign="top" width="30.323032303230324%" headers="mcps1.2.4.1.2 "><p id="p11206152616140"><a name="p11206152616140"></a><a name="p11206152616140"></a>union</p>
</td>
<td class="cellrowborder" valign="top" width="44.174417441744176%" headers="mcps1.2.4.1.3 "><p id="p1920616268145"><a name="p1920616268145"></a><a name="p1920616268145"></a>地址信息，联合体。</p>
</td>
</tr>
<tr id="row6206142641411"><td class="cellrowborder" valign="top" width="25.502550255025504%" headers="mcps1.2.4.1.1 "><p id="p16206142661419"><a name="p16206142661419"></a><a name="p16206142661419"></a>ip6</p>
</td>
<td class="cellrowborder" valign="top" width="30.323032303230324%" headers="mcps1.2.4.1.2 "><p id="p152061326101410"><a name="p152061326101410"></a><a name="p152061326101410"></a>ip6_addr_t</p>
</td>
<td class="cellrowborder" valign="top" width="44.174417441744176%" headers="mcps1.2.4.1.3 "><p id="p3206326111410"><a name="p3206326111410"></a><a name="p3206326111410"></a>IPv6地址：</p>
<p id="p820672617141"><a name="p820672617141"></a><a name="p820672617141"></a>u32_t addr[4]。</p>
</td>
</tr>
<tr id="row1920682618145"><td class="cellrowborder" valign="top" width="25.502550255025504%" headers="mcps1.2.4.1.1 "><p id="p172061263149"><a name="p172061263149"></a><a name="p172061263149"></a>ip4</p>
</td>
<td class="cellrowborder" valign="top" width="30.323032303230324%" headers="mcps1.2.4.1.2 "><p id="p1420622618147"><a name="p1420622618147"></a><a name="p1420622618147"></a>ip4_addr_t</p>
</td>
<td class="cellrowborder" valign="top" width="44.174417441744176%" headers="mcps1.2.4.1.3 "><p id="p720618268142"><a name="p720618268142"></a><a name="p720618268142"></a>IPv4地址：</p>
<p id="p720632613140"><a name="p720632613140"></a><a name="p720632613140"></a>u32_t addr。</p>
</td>
</tr>
<tr id="row320672651417"><td class="cellrowborder" valign="top" width="25.502550255025504%" headers="mcps1.2.4.1.1 "><p id="p13206726121417"><a name="p13206726121417"></a><a name="p13206726121417"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="30.323032303230324%" headers="mcps1.2.4.1.2 "><p id="p1206192611412"><a name="p1206192611412"></a><a name="p1206192611412"></a>uint8_t</p>
</td>
<td class="cellrowborder" valign="top" width="44.174417441744176%" headers="mcps1.2.4.1.3 "><p id="p120692641410"><a name="p120692641410"></a><a name="p120692641410"></a>IP地址类型。</p>
</td>
</tr>
</tbody>
</table>

**表 4**  返回值说明

<a name="table814316264140"></a>
<table><thead align="left"><tr id="row620617265146"><th class="cellrowborder" valign="top" width="25.082508250825086%" id="mcps1.2.4.1.1"><p id="p620612263143"><a name="p620612263143"></a><a name="p620612263143"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="30.853085308530854%" id="mcps1.2.4.1.2"><p id="p1206126121419"><a name="p1206126121419"></a><a name="p1206126121419"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="44.06440644064406%" id="mcps1.2.4.1.3"><p id="p920616267149"><a name="p920616267149"></a><a name="p920616267149"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row9206112681418"><td class="cellrowborder" valign="top" width="25.082508250825086%" headers="mcps1.2.4.1.1 "><p id="p1520652661413"><a name="p1520652661413"></a><a name="p1520652661413"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="30.853085308530854%" headers="mcps1.2.4.1.2 "><p id="p17206326121412"><a name="p17206326121412"></a><a name="p17206326121412"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="44.06440644064406%" headers="mcps1.2.4.1.3 "><p id="p1820692612142"><a name="p1820692612142"></a><a name="p1820692612142"></a>获取PDP接口处理成功。</p>
</td>
</tr>
<tr id="row16206132631411"><td class="cellrowborder" valign="top" width="25.082508250825086%" headers="mcps1.2.4.1.1 "><p id="p5207426141413"><a name="p5207426141413"></a><a name="p5207426141413"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="30.853085308530854%" headers="mcps1.2.4.1.2 "><p id="p220712267148"><a name="p220712267148"></a><a name="p220712267148"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="44.06440644064406%" headers="mcps1.2.4.1.3 "><p id="p7207132620148"><a name="p7207132620148"></a><a name="p7207132620148"></a>获取PDP接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 手动搜网，并显示可选网络<a name="ZH-CN_TOPIC_0000002223508052"></a>

函数原型：int32\_t ril\_get\_radio\_search\_start\(respond\_callback\_t rsp\_callback\);

函数功能：获取在网络中搜索到的PLMN清单，核内交互（相当于执行：AT+COPS=?）。

**表 1**  参数说明

<a name="zh-cn_topic_0000002227896309_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227896309_row12908457921"><th class="cellrowborder" valign="top" width="21.25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002227896309_p563315171132"><a name="zh-cn_topic_0000002227896309_p563315171132"></a><a name="zh-cn_topic_0000002227896309_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="22.86%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002227896309_p1463351718318"><a name="zh-cn_topic_0000002227896309_p1463351718318"></a><a name="zh-cn_topic_0000002227896309_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="27.13%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002227896309_p16338179319"><a name="zh-cn_topic_0000002227896309_p16338179319"></a><a name="zh-cn_topic_0000002227896309_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="28.76%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002227896309_p1463316170313"><a name="zh-cn_topic_0000002227896309_p1463316170313"></a><a name="zh-cn_topic_0000002227896309_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227896309_row1830174962413"><td class="cellrowborder" valign="top" width="21.25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002227896309_p13145102016548"><a name="zh-cn_topic_0000002227896309_p13145102016548"></a><a name="zh-cn_topic_0000002227896309_p13145102016548"></a>rsp_callback</p>
</td>
<td class="cellrowborder" valign="top" width="22.86%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002227896309_p149081857623"><a name="zh-cn_topic_0000002227896309_p149081857623"></a><a name="zh-cn_topic_0000002227896309_p149081857623"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="27.13%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002227896309_p29081571426"><a name="zh-cn_topic_0000002227896309_p29081571426"></a><a name="zh-cn_topic_0000002227896309_p29081571426"></a>respond_callback_t</p>
</td>
<td class="cellrowborder" valign="top" width="28.76%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002227896309_p20238191416817"><a name="zh-cn_topic_0000002227896309_p20238191416817"></a><a name="zh-cn_topic_0000002227896309_p20238191416817"></a>回调函数：</p>
<p id="zh-cn_topic_0000002227896309_p10324115704"><a name="zh-cn_topic_0000002227896309_p10324115704"></a><a name="zh-cn_topic_0000002227896309_p10324115704"></a>typedef int32_t (*respond_callback_t)(uint8_t* data, size_t len);</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002227896309_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227896309_row82122391941"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002227896309_p15896553644"><a name="zh-cn_topic_0000002227896309_p15896553644"></a><a name="zh-cn_topic_0000002227896309_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002227896309_p489612532411"><a name="zh-cn_topic_0000002227896309_p489612532411"></a><a name="zh-cn_topic_0000002227896309_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002227896309_p148968533417"><a name="zh-cn_topic_0000002227896309_p148968533417"></a><a name="zh-cn_topic_0000002227896309_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227896309_row5213113915410"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227896309_p14111210255"><a name="zh-cn_topic_0000002227896309_p14111210255"></a><a name="zh-cn_topic_0000002227896309_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227896309_p14411151012512"><a name="zh-cn_topic_0000002227896309_p14411151012512"></a><a name="zh-cn_topic_0000002227896309_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227896309_p151469557531"><a name="zh-cn_topic_0000002227896309_p151469557531"></a><a name="zh-cn_topic_0000002227896309_p151469557531"></a>获取PLMN清单接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002227896309_row82131139142"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227896309_p1141117102516"><a name="zh-cn_topic_0000002227896309_p1141117102516"></a><a name="zh-cn_topic_0000002227896309_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227896309_p164111810052"><a name="zh-cn_topic_0000002227896309_p164111810052"></a><a name="zh-cn_topic_0000002227896309_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227896309_p214655565313"><a name="zh-cn_topic_0000002227896309_p214655565313"></a><a name="zh-cn_topic_0000002227896309_p214655565313"></a>获取PLMN清单接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 设置快速休眠时间<a name="ZH-CN_TOPIC_0000002533154426"></a>

函数原型：int32\_t ril\_set\_fast\_sleep\_time\(ril\_set\_fast\_sleep\_t fast\_sleep\);

函数功能：开启增强休眠功能后没有数传时快速进入idle态时间，单位ms，最小100ms。不建议设置时间小于1s，时间太短可能会导致数传失败。

**表 1**  参数说明

<a name="table22371210183312"></a>
<table><thead align="left"><tr id="row11344171003315"><th class="cellrowborder" valign="top" width="19.39%" id="mcps1.2.5.1.1"><p id="p20344810193317"><a name="p20344810193317"></a><a name="p20344810193317"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="22.45%" id="mcps1.2.5.1.2"><p id="p19344131017339"><a name="p19344131017339"></a><a name="p19344131017339"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="22.770000000000003%" id="mcps1.2.5.1.3"><p id="p9344201017339"><a name="p9344201017339"></a><a name="p9344201017339"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="35.39%" id="mcps1.2.5.1.4"><p id="p2344710173314"><a name="p2344710173314"></a><a name="p2344710173314"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row1334431053315"><td class="cellrowborder" valign="top" width="19.39%" headers="mcps1.2.5.1.1 "><p id="p163441410163312"><a name="p163441410163312"></a><a name="p163441410163312"></a>fast_sleep</p>
</td>
<td class="cellrowborder" valign="top" width="22.45%" headers="mcps1.2.5.1.2 "><p id="p1834411016334"><a name="p1834411016334"></a><a name="p1834411016334"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="22.770000000000003%" headers="mcps1.2.5.1.3 "><p id="p334441093319"><a name="p334441093319"></a><a name="p334441093319"></a>ril_set_fast_sleep_t</p>
</td>
<td class="cellrowborder" valign="top" width="35.39%" headers="mcps1.2.5.1.4 "><p id="p33442109334"><a name="p33442109334"></a><a name="p33442109334"></a>设置条件结构体。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  ril\_set\_fast\_sleep\_t结构体说明

<a name="table9243151020330"></a>
<table><thead align="left"><tr id="row143441210143319"><th class="cellrowborder" valign="top" width="19.39%" id="mcps1.2.5.1.1"><p id="p1034461033310"><a name="p1034461033310"></a><a name="p1034461033310"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="22.45%" id="mcps1.2.5.1.2"><p id="p1634417102331"><a name="p1634417102331"></a><a name="p1634417102331"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="23.1%" id="mcps1.2.5.1.3"><p id="p434451023311"><a name="p434451023311"></a><a name="p434451023311"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="35.06%" id="mcps1.2.5.1.4"><p id="p2344111018331"><a name="p2344111018331"></a><a name="p2344111018331"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row163441610123314"><td class="cellrowborder" valign="top" width="19.39%" headers="mcps1.2.5.1.1 "><p id="p163446109332"><a name="p163446109332"></a><a name="p163446109332"></a>fast_sleep_switch</p>
</td>
<td class="cellrowborder" valign="top" width="22.45%" headers="mcps1.2.5.1.2 "><p id="p1634461073315"><a name="p1634461073315"></a><a name="p1634461073315"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="23.1%" headers="mcps1.2.5.1.3 "><p id="p1234451012336"><a name="p1234451012336"></a><a name="p1234451012336"></a>bool</p>
</td>
<td class="cellrowborder" valign="top" width="35.06%" headers="mcps1.2.5.1.4 "><p id="p434421003312"><a name="p434421003312"></a><a name="p434421003312"></a>是否开启快速睡眠。</p>
</td>
</tr>
<tr id="row163441010133312"><td class="cellrowborder" valign="top" width="19.39%" headers="mcps1.2.5.1.1 "><p id="p9344910113310"><a name="p9344910113310"></a><a name="p9344910113310"></a>fast_sleep_time</p>
</td>
<td class="cellrowborder" valign="top" width="22.45%" headers="mcps1.2.5.1.2 "><p id="p193449102339"><a name="p193449102339"></a><a name="p193449102339"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="23.1%" headers="mcps1.2.5.1.3 "><p id="p13441410113311"><a name="p13441410113311"></a><a name="p13441410113311"></a>uint32_t *</p>
</td>
<td class="cellrowborder" valign="top" width="35.06%" headers="mcps1.2.5.1.4 "><p id="p334511017332"><a name="p334511017332"></a><a name="p334511017332"></a>进入快速睡眠的时间，单位ms（默认5000ms）。</p>
</td>
</tr>
</tbody>
</table>

**表 3**  返回值说明

<a name="table112460108335"></a>
<table><thead align="left"><tr id="row1134513103339"><th class="cellrowborder" valign="top" width="19.19191919191919%" id="mcps1.2.4.1.1"><p id="p83451510183319"><a name="p83451510183319"></a><a name="p83451510183319"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="31.083108310831086%" id="mcps1.2.4.1.2"><p id="p13345111083312"><a name="p13345111083312"></a><a name="p13345111083312"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="49.72497249724972%" id="mcps1.2.4.1.3"><p id="p33451110153318"><a name="p33451110153318"></a><a name="p33451110153318"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row17345910113317"><td class="cellrowborder" valign="top" width="19.19191919191919%" headers="mcps1.2.4.1.1 "><p id="p12345210183314"><a name="p12345210183314"></a><a name="p12345210183314"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="31.083108310831086%" headers="mcps1.2.4.1.2 "><p id="p234510100330"><a name="p234510100330"></a><a name="p234510100330"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="49.72497249724972%" headers="mcps1.2.4.1.3 "><p id="p16345191013339"><a name="p16345191013339"></a><a name="p16345191013339"></a>设置进入休眠的时间成功。</p>
</td>
</tr>
<tr id="row534520109336"><td class="cellrowborder" valign="top" width="19.19191919191919%" headers="mcps1.2.4.1.1 "><p id="p9345110163314"><a name="p9345110163314"></a><a name="p9345110163314"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="31.083108310831086%" headers="mcps1.2.4.1.2 "><p id="p1134510105336"><a name="p1134510105336"></a><a name="p1134510105336"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="49.72497249724972%" headers="mcps1.2.4.1.3 "><p id="p1134561043316"><a name="p1134561043316"></a><a name="p1134561043316"></a>设置进入休眠的时间失败。</p>
</td>
</tr>
</tbody>
</table>

### 通话接口<a name="ZH-CN_TOPIC_0000002223667892"></a>

-   **[设置自动接听](#ZH-CN_TOPIC_0000002258467677)**  

-   **[拨打电话](#ZH-CN_TOPIC_0000002258627557)**  

-   **[手动接听电话](#ZH-CN_TOPIC_0000002223508064)**  

-   **[挂断电话](#ZH-CN_TOPIC_0000002223667896)**  

#### 设置自动接听<a name="ZH-CN_TOPIC_0000002258467677"></a>

函数原型：int32\_t ril\_set\_auto\_answer\(uint8\_t time\_s\);

函数功能：设置自动应答，核内交互（相当于执行：ATS0=）。

**表 1**  参数说明

<a name="zh-cn_topic_0000002227770757_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227770757_row12908457921"><th class="cellrowborder" valign="top" width="18.82%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002227770757_p563315171132"><a name="zh-cn_topic_0000002227770757_p563315171132"></a><a name="zh-cn_topic_0000002227770757_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="23.44%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002227770757_p1463351718318"><a name="zh-cn_topic_0000002227770757_p1463351718318"></a><a name="zh-cn_topic_0000002227770757_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="25.66%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002227770757_p16338179319"><a name="zh-cn_topic_0000002227770757_p16338179319"></a><a name="zh-cn_topic_0000002227770757_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="32.08%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002227770757_p1463316170313"><a name="zh-cn_topic_0000002227770757_p1463316170313"></a><a name="zh-cn_topic_0000002227770757_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227770757_row1830174962413"><td class="cellrowborder" valign="top" width="18.82%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002227770757_p13145102016548"><a name="zh-cn_topic_0000002227770757_p13145102016548"></a><a name="zh-cn_topic_0000002227770757_p13145102016548"></a>time_s</p>
</td>
<td class="cellrowborder" valign="top" width="23.44%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002227770757_p149081857623"><a name="zh-cn_topic_0000002227770757_p149081857623"></a><a name="zh-cn_topic_0000002227770757_p149081857623"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="25.66%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002227770757_p29081571426"><a name="zh-cn_topic_0000002227770757_p29081571426"></a><a name="zh-cn_topic_0000002227770757_p29081571426"></a>uint8_t</p>
</td>
<td class="cellrowborder" valign="top" width="32.08%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002227770757_p10324115704"><a name="zh-cn_topic_0000002227770757_p10324115704"></a><a name="zh-cn_topic_0000002227770757_p10324115704"></a>S0定时器时长。</p>
<p id="zh-cn_topic_0000002227770757_p143823131914"><a name="zh-cn_topic_0000002227770757_p143823131914"></a><a name="zh-cn_topic_0000002227770757_p143823131914"></a>0：关闭自动接听；</p>
<p id="zh-cn_topic_0000002227770757_p101411915201917"><a name="zh-cn_topic_0000002227770757_p101411915201917"></a><a name="zh-cn_topic_0000002227770757_p101411915201917"></a>1～255：启动自动应答的时间，单位：s。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002227770757_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002227770757_row82122391941"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002227770757_p15896553644"><a name="zh-cn_topic_0000002227770757_p15896553644"></a><a name="zh-cn_topic_0000002227770757_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002227770757_p489612532411"><a name="zh-cn_topic_0000002227770757_p489612532411"></a><a name="zh-cn_topic_0000002227770757_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002227770757_p148968533417"><a name="zh-cn_topic_0000002227770757_p148968533417"></a><a name="zh-cn_topic_0000002227770757_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002227770757_row5213113915410"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227770757_p14111210255"><a name="zh-cn_topic_0000002227770757_p14111210255"></a><a name="zh-cn_topic_0000002227770757_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227770757_p14411151012512"><a name="zh-cn_topic_0000002227770757_p14411151012512"></a><a name="zh-cn_topic_0000002227770757_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227770757_p151469557531"><a name="zh-cn_topic_0000002227770757_p151469557531"></a><a name="zh-cn_topic_0000002227770757_p151469557531"></a>设置自动接听接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002227770757_row82131139142"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002227770757_p1141117102516"><a name="zh-cn_topic_0000002227770757_p1141117102516"></a><a name="zh-cn_topic_0000002227770757_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002227770757_p164111810052"><a name="zh-cn_topic_0000002227770757_p164111810052"></a><a name="zh-cn_topic_0000002227770757_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002227770757_p214655565313"><a name="zh-cn_topic_0000002227770757_p214655565313"></a><a name="zh-cn_topic_0000002227770757_p214655565313"></a>设置自动接听接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 拨打电话<a name="ZH-CN_TOPIC_0000002258627557"></a>

函数原型：int32\_t ril\_set\_call\_orig\_req\(const char\* call\_str, respond\_callback\_t rsp\_callback\);

函数功能：拨打电话，核内交互（相当于执行：ATDxxx;）。

**表 1**  参数说明

<a name="zh-cn_topic_0000002192410448_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002192410448_row12908457921"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002192410448_p563315171132"><a name="zh-cn_topic_0000002192410448_p563315171132"></a><a name="zh-cn_topic_0000002192410448_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="23.11%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002192410448_p1463351718318"><a name="zh-cn_topic_0000002192410448_p1463351718318"></a><a name="zh-cn_topic_0000002192410448_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="26.889999999999997%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002192410448_p16338179319"><a name="zh-cn_topic_0000002192410448_p16338179319"></a><a name="zh-cn_topic_0000002192410448_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002192410448_p1463316170313"><a name="zh-cn_topic_0000002192410448_p1463316170313"></a><a name="zh-cn_topic_0000002192410448_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002192410448_row2184164910226"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002192410448_p171841249102217"><a name="zh-cn_topic_0000002192410448_p171841249102217"></a><a name="zh-cn_topic_0000002192410448_p171841249102217"></a>call_str</p>
</td>
<td class="cellrowborder" valign="top" width="23.11%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002192410448_p7184204918225"><a name="zh-cn_topic_0000002192410448_p7184204918225"></a><a name="zh-cn_topic_0000002192410448_p7184204918225"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="26.889999999999997%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002192410448_p6184184952213"><a name="zh-cn_topic_0000002192410448_p6184184952213"></a><a name="zh-cn_topic_0000002192410448_p6184184952213"></a>char*</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002192410448_p4184649142215"><a name="zh-cn_topic_0000002192410448_p4184649142215"></a><a name="zh-cn_topic_0000002192410448_p4184649142215"></a>拨打的电话号码。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002192410448_row1830174962413"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002192410448_p13145102016548"><a name="zh-cn_topic_0000002192410448_p13145102016548"></a><a name="zh-cn_topic_0000002192410448_p13145102016548"></a>rsp_callback</p>
</td>
<td class="cellrowborder" valign="top" width="23.11%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002192410448_p149081857623"><a name="zh-cn_topic_0000002192410448_p149081857623"></a><a name="zh-cn_topic_0000002192410448_p149081857623"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="26.889999999999997%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002192410448_p29081571426"><a name="zh-cn_topic_0000002192410448_p29081571426"></a><a name="zh-cn_topic_0000002192410448_p29081571426"></a>respond_callback_t</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002192410448_p20238191416817"><a name="zh-cn_topic_0000002192410448_p20238191416817"></a><a name="zh-cn_topic_0000002192410448_p20238191416817"></a>回调函数：</p>
<p id="zh-cn_topic_0000002192410448_p10324115704"><a name="zh-cn_topic_0000002192410448_p10324115704"></a><a name="zh-cn_topic_0000002192410448_p10324115704"></a>typedef int32_t (*respond_callback_t)(uint8_t* data, size_t len);</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002192410448_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002192410448_row82122391941"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002192410448_p15896553644"><a name="zh-cn_topic_0000002192410448_p15896553644"></a><a name="zh-cn_topic_0000002192410448_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002192410448_p489612532411"><a name="zh-cn_topic_0000002192410448_p489612532411"></a><a name="zh-cn_topic_0000002192410448_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002192410448_p148968533417"><a name="zh-cn_topic_0000002192410448_p148968533417"></a><a name="zh-cn_topic_0000002192410448_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002192410448_row5213113915410"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002192410448_p14111210255"><a name="zh-cn_topic_0000002192410448_p14111210255"></a><a name="zh-cn_topic_0000002192410448_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002192410448_p14411151012512"><a name="zh-cn_topic_0000002192410448_p14411151012512"></a><a name="zh-cn_topic_0000002192410448_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002192410448_p151469557531"><a name="zh-cn_topic_0000002192410448_p151469557531"></a><a name="zh-cn_topic_0000002192410448_p151469557531"></a>拨打电话接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002192410448_row82131139142"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002192410448_p1141117102516"><a name="zh-cn_topic_0000002192410448_p1141117102516"></a><a name="zh-cn_topic_0000002192410448_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002192410448_p164111810052"><a name="zh-cn_topic_0000002192410448_p164111810052"></a><a name="zh-cn_topic_0000002192410448_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002192410448_p214655565313"><a name="zh-cn_topic_0000002192410448_p214655565313"></a><a name="zh-cn_topic_0000002192410448_p214655565313"></a>拨打电话接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 手动接听电话<a name="ZH-CN_TOPIC_0000002223508064"></a>

函数原型：int32\_t ril\_set\_call\_mt\_accept\_req\(void\);

函数功能：手动接听电话，核内交互（相当于执行：ATA）。

**表 1**  返回值说明

<a name="table1256435231414"></a>
<table><thead align="left"><tr id="row1458605221417"><th class="cellrowborder" valign="top" width="27.222722272227223%" id="mcps1.2.4.1.1"><p id="p95861352111415"><a name="p95861352111415"></a><a name="p95861352111415"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="34.9034903490349%" id="mcps1.2.4.1.2"><p id="p185861552191419"><a name="p185861552191419"></a><a name="p185861552191419"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="37.87378737873787%" id="mcps1.2.4.1.3"><p id="p958655231419"><a name="p958655231419"></a><a name="p958655231419"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row7586195251413"><td class="cellrowborder" valign="top" width="27.222722272227223%" headers="mcps1.2.4.1.1 "><p id="p75861852111414"><a name="p75861852111414"></a><a name="p75861852111414"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="34.9034903490349%" headers="mcps1.2.4.1.2 "><p id="p758665214149"><a name="p758665214149"></a><a name="p758665214149"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="37.87378737873787%" headers="mcps1.2.4.1.3 "><p id="p155869528141"><a name="p155869528141"></a><a name="p155869528141"></a>接听电话接口处理成功。</p>
</td>
</tr>
<tr id="row658695210147"><td class="cellrowborder" valign="top" width="27.222722272227223%" headers="mcps1.2.4.1.1 "><p id="p16586135217147"><a name="p16586135217147"></a><a name="p16586135217147"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="34.9034903490349%" headers="mcps1.2.4.1.2 "><p id="p155862522145"><a name="p155862522145"></a><a name="p155862522145"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="37.87378737873787%" headers="mcps1.2.4.1.3 "><p id="p658619529142"><a name="p658619529142"></a><a name="p658619529142"></a>接听电话接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 挂断电话<a name="ZH-CN_TOPIC_0000002223667896"></a>

函数原型：int32\_t ril\_set\_call\_rel\_req\(void\);

函数功能：挂断电话，核内交互（相当于执行：ATH）。

**表 1**  返回值说明

<a name="table1827816911156"></a>
<table><thead align="left"><tr id="row1830617914155"><th class="cellrowborder" valign="top" width="28.232823282328233%" id="mcps1.2.4.1.1"><p id="p730616918150"><a name="p730616918150"></a><a name="p730616918150"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="34.573457345734575%" id="mcps1.2.4.1.2"><p id="p133061291157"><a name="p133061291157"></a><a name="p133061291157"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="37.19371937193719%" id="mcps1.2.4.1.3"><p id="p1930619161518"><a name="p1930619161518"></a><a name="p1930619161518"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row183068910159"><td class="cellrowborder" valign="top" width="28.232823282328233%" headers="mcps1.2.4.1.1 "><p id="p19306891153"><a name="p19306891153"></a><a name="p19306891153"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="34.573457345734575%" headers="mcps1.2.4.1.2 "><p id="p203061599151"><a name="p203061599151"></a><a name="p203061599151"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="37.19371937193719%" headers="mcps1.2.4.1.3 "><p id="p1930769161515"><a name="p1930769161515"></a><a name="p1930769161515"></a>挂断电话接口处理成功。</p>
</td>
</tr>
<tr id="row1307894151"><td class="cellrowborder" valign="top" width="28.232823282328233%" headers="mcps1.2.4.1.1 "><p id="p1930711912151"><a name="p1930711912151"></a><a name="p1930711912151"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="34.573457345734575%" headers="mcps1.2.4.1.2 "><p id="p830713916158"><a name="p830713916158"></a><a name="p830713916158"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="37.19371937193719%" headers="mcps1.2.4.1.3 "><p id="p16307694157"><a name="p16307694157"></a><a name="p16307694157"></a>挂断电话接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

### AT指令通道<a name="ZH-CN_TOPIC_0000002258467681"></a>

Hi2131E侧AT命令遵循3GPP TS 27.007和27.005规范。

-   **[AT指令发送](#ZH-CN_TOPIC_0000002258627565)**  

-   **[同步AT命令发送](#ZH-CN_TOPIC_0000002223508068)**  

-   **[AT命令注册回调](#ZH-CN_TOPIC_0000002492261636)**  

#### AT指令发送<a name="ZH-CN_TOPIC_0000002258627565"></a>

函数原型：int32\_t ril\_at\_cmd\_exc\(const char \*str\);

函数功能：执行AT命令，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002192250864_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002192250864_row12908457921"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002192250864_p563315171132"><a name="zh-cn_topic_0000002192250864_p563315171132"></a><a name="zh-cn_topic_0000002192250864_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002192250864_p1463351718318"><a name="zh-cn_topic_0000002192250864_p1463351718318"></a><a name="zh-cn_topic_0000002192250864_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002192250864_p16338179319"><a name="zh-cn_topic_0000002192250864_p16338179319"></a><a name="zh-cn_topic_0000002192250864_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002192250864_p1463316170313"><a name="zh-cn_topic_0000002192250864_p1463316170313"></a><a name="zh-cn_topic_0000002192250864_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002192250864_row2184164910226"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002192250864_p171841249102217"><a name="zh-cn_topic_0000002192250864_p171841249102217"></a><a name="zh-cn_topic_0000002192250864_p171841249102217"></a>str</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002192250864_p7184204918225"><a name="zh-cn_topic_0000002192250864_p7184204918225"></a><a name="zh-cn_topic_0000002192250864_p7184204918225"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002192250864_p6184184952213"><a name="zh-cn_topic_0000002192250864_p6184184952213"></a><a name="zh-cn_topic_0000002192250864_p6184184952213"></a>char*</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002192250864_p4184649142215"><a name="zh-cn_topic_0000002192250864_p4184649142215"></a><a name="zh-cn_topic_0000002192250864_p4184649142215"></a>AT命令字符串。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002192250864_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002192250864_row82122391941"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002192250864_p15896553644"><a name="zh-cn_topic_0000002192250864_p15896553644"></a><a name="zh-cn_topic_0000002192250864_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002192250864_p489612532411"><a name="zh-cn_topic_0000002192250864_p489612532411"></a><a name="zh-cn_topic_0000002192250864_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002192250864_p148968533417"><a name="zh-cn_topic_0000002192250864_p148968533417"></a><a name="zh-cn_topic_0000002192250864_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002192250864_row5213113915410"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002192250864_p14111210255"><a name="zh-cn_topic_0000002192250864_p14111210255"></a><a name="zh-cn_topic_0000002192250864_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002192250864_p14411151012512"><a name="zh-cn_topic_0000002192250864_p14411151012512"></a><a name="zh-cn_topic_0000002192250864_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002192250864_p151469557531"><a name="zh-cn_topic_0000002192250864_p151469557531"></a><a name="zh-cn_topic_0000002192250864_p151469557531"></a>执行AT命令接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002192250864_row82131139142"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002192250864_p1141117102516"><a name="zh-cn_topic_0000002192250864_p1141117102516"></a><a name="zh-cn_topic_0000002192250864_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002192250864_p164111810052"><a name="zh-cn_topic_0000002192250864_p164111810052"></a><a name="zh-cn_topic_0000002192250864_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002192250864_p214655565313"><a name="zh-cn_topic_0000002192250864_p214655565313"></a><a name="zh-cn_topic_0000002192250864_p214655565313"></a>执行AT命令接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 同步AT命令发送<a name="ZH-CN_TOPIC_0000002223508068"></a>

-   函数原型：int32\_t ril\_at\_cmd\_sync\_get\_info\(const char \*at\_str, uint8\_t \*data, uint16\_t size, uint32\_t timeout\);

    函数功能：同步执行AT命令，核内交互。

    **表 1**  参数说明

    <a name="table172891442181518"></a>
    <table><thead align="left"><tr id="row104006424152"><th class="cellrowborder" valign="top" width="17.349999999999998%" id="mcps1.2.5.1.1"><p id="p7400742141513"><a name="p7400742141513"></a><a name="p7400742141513"></a>参数</p>
    </th>
    <th class="cellrowborder" valign="top" width="23.24%" id="mcps1.2.5.1.2"><p id="p540044217152"><a name="p540044217152"></a><a name="p540044217152"></a>参数描述类型</p>
    </th>
    <th class="cellrowborder" valign="top" width="26.76%" id="mcps1.2.5.1.3"><p id="p84004423153"><a name="p84004423153"></a><a name="p84004423153"></a>参数结构类型</p>
    </th>
    <th class="cellrowborder" valign="top" width="32.65%" id="mcps1.2.5.1.4"><p id="p11400174214151"><a name="p11400174214151"></a><a name="p11400174214151"></a>说明</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row164003422153"><td class="cellrowborder" valign="top" width="17.349999999999998%" headers="mcps1.2.5.1.1 "><p id="p1040012420155"><a name="p1040012420155"></a><a name="p1040012420155"></a>at_str</p>
    </td>
    <td class="cellrowborder" valign="top" width="23.24%" headers="mcps1.2.5.1.2 "><p id="p2400104214159"><a name="p2400104214159"></a><a name="p2400104214159"></a>入参</p>
    </td>
    <td class="cellrowborder" valign="top" width="26.76%" headers="mcps1.2.5.1.3 "><p id="p4400114211511"><a name="p4400114211511"></a><a name="p4400114211511"></a>char*</p>
    </td>
    <td class="cellrowborder" valign="top" width="32.65%" headers="mcps1.2.5.1.4 "><p id="p84001242181517"><a name="p84001242181517"></a><a name="p84001242181517"></a>AT命令字符串。</p>
    </td>
    </tr>
    <tr id="row194001842161520"><td class="cellrowborder" valign="top" width="17.349999999999998%" headers="mcps1.2.5.1.1 "><p id="p64001042101519"><a name="p64001042101519"></a><a name="p64001042101519"></a>data</p>
    </td>
    <td class="cellrowborder" valign="top" width="23.24%" headers="mcps1.2.5.1.2 "><p id="p840013427153"><a name="p840013427153"></a><a name="p840013427153"></a>出参</p>
    </td>
    <td class="cellrowborder" valign="top" width="26.76%" headers="mcps1.2.5.1.3 "><p id="p940019422159"><a name="p940019422159"></a><a name="p940019422159"></a>uint8_t*</p>
    </td>
    <td class="cellrowborder" valign="top" width="32.65%" headers="mcps1.2.5.1.4 "><p id="p1140015428154"><a name="p1140015428154"></a><a name="p1140015428154"></a>AT命令返回的结果字符串。</p>
    </td>
    </tr>
    <tr id="row1640017425159"><td class="cellrowborder" valign="top" width="17.349999999999998%" headers="mcps1.2.5.1.1 "><p id="p640004211518"><a name="p640004211518"></a><a name="p640004211518"></a>size</p>
    </td>
    <td class="cellrowborder" valign="top" width="23.24%" headers="mcps1.2.5.1.2 "><p id="p11400154291510"><a name="p11400154291510"></a><a name="p11400154291510"></a>入参</p>
    </td>
    <td class="cellrowborder" valign="top" width="26.76%" headers="mcps1.2.5.1.3 "><p id="p174001042151513"><a name="p174001042151513"></a><a name="p174001042151513"></a>uint16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="32.65%" headers="mcps1.2.5.1.4 "><p id="p12400124216158"><a name="p12400124216158"></a><a name="p12400124216158"></a>AT命令结果字符串的长度。</p>
    </td>
    </tr>
    <tr id="row24001242161513"><td class="cellrowborder" valign="top" width="17.349999999999998%" headers="mcps1.2.5.1.1 "><p id="p640019424151"><a name="p640019424151"></a><a name="p640019424151"></a>timeout</p>
    </td>
    <td class="cellrowborder" valign="top" width="23.24%" headers="mcps1.2.5.1.2 "><p id="p840074261515"><a name="p840074261515"></a><a name="p840074261515"></a>入参</p>
    </td>
    <td class="cellrowborder" valign="top" width="26.76%" headers="mcps1.2.5.1.3 "><p id="p18400104219154"><a name="p18400104219154"></a><a name="p18400104219154"></a>uint32_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="32.65%" headers="mcps1.2.5.1.4 "><p id="p1740034218155"><a name="p1740034218155"></a><a name="p1740034218155"></a>同步执行AT命令超时时间。</p>
    </td>
    </tr>
    </tbody>
    </table>

    **表 2**  返回值说明

    <a name="table029644217150"></a>
    <table><thead align="left"><tr id="row94002425154"><th class="cellrowborder" valign="top" width="17.349999999999998%" id="mcps1.2.4.1.1"><p id="p16401042131511"><a name="p16401042131511"></a><a name="p16401042131511"></a>返回值</p>
    </th>
    <th class="cellrowborder" valign="top" width="29.59%" id="mcps1.2.4.1.2"><p id="p124011742151510"><a name="p124011742151510"></a><a name="p124011742151510"></a>参数结构类型</p>
    </th>
    <th class="cellrowborder" valign="top" width="53.059999999999995%" id="mcps1.2.4.1.3"><p id="p34013423158"><a name="p34013423158"></a><a name="p34013423158"></a>说明</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row34013421156"><td class="cellrowborder" valign="top" width="17.349999999999998%" headers="mcps1.2.4.1.1 "><p id="p114011542161515"><a name="p114011542161515"></a><a name="p114011542161515"></a>0</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.59%" headers="mcps1.2.4.1.2 "><p id="p16401204219159"><a name="p16401204219159"></a><a name="p16401204219159"></a>int32_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="53.059999999999995%" headers="mcps1.2.4.1.3 "><p id="p184011042121516"><a name="p184011042121516"></a><a name="p184011042121516"></a>执行同步AT命令接口处理成功。</p>
    </td>
    </tr>
    <tr id="row8401342171517"><td class="cellrowborder" valign="top" width="17.349999999999998%" headers="mcps1.2.4.1.1 "><p id="p1940112421151"><a name="p1940112421151"></a><a name="p1940112421151"></a>&gt;0</p>
    </td>
    <td class="cellrowborder" valign="top" width="29.59%" headers="mcps1.2.4.1.2 "><p id="p4401204281516"><a name="p4401204281516"></a><a name="p4401204281516"></a>int32_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="53.059999999999995%" headers="mcps1.2.4.1.3 "><p id="p16401184281510"><a name="p16401184281510"></a><a name="p16401184281510"></a>执行同步AT命令接口处理失败。</p>
    </td>
    </tr>
    </tbody>
    </table>

-   函数原型：int32\_t ril\_at\_sync\_multiple\_get\_info\(const char \*at\_str, uint8\_t \*data, uint32\_t size, uint32\_t times, uint32\_t timeout\)

    函数功能：同步执行AT命令，接收多条AT指令回复内容。

    **表 3**  参数说明

    <a name="table1629874218150"></a>
    <table><thead align="left"><tr id="row2040134211515"><th class="cellrowborder" valign="top" width="17.349999999999998%" id="mcps1.2.5.1.1"><p id="p14011429154"><a name="p14011429154"></a><a name="p14011429154"></a>参数</p>
    </th>
    <th class="cellrowborder" valign="top" width="24.14%" id="mcps1.2.5.1.2"><p id="p13401142181511"><a name="p13401142181511"></a><a name="p13401142181511"></a>参数描述类型</p>
    </th>
    <th class="cellrowborder" valign="top" width="26.700000000000003%" id="mcps1.2.5.1.3"><p id="p1040120427158"><a name="p1040120427158"></a><a name="p1040120427158"></a>参数结构类型</p>
    </th>
    <th class="cellrowborder" valign="top" width="31.81%" id="mcps1.2.5.1.4"><p id="p940115429152"><a name="p940115429152"></a><a name="p940115429152"></a>说明</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row640214261515"><td class="cellrowborder" valign="top" width="17.349999999999998%" headers="mcps1.2.5.1.1 "><p id="p140234251515"><a name="p140234251515"></a><a name="p140234251515"></a>at_str</p>
    </td>
    <td class="cellrowborder" valign="top" width="24.14%" headers="mcps1.2.5.1.2 "><p id="p1540211428154"><a name="p1540211428154"></a><a name="p1540211428154"></a>入参</p>
    </td>
    <td class="cellrowborder" valign="top" width="26.700000000000003%" headers="mcps1.2.5.1.3 "><p id="p1340234251516"><a name="p1340234251516"></a><a name="p1340234251516"></a>char*</p>
    </td>
    <td class="cellrowborder" valign="top" width="31.81%" headers="mcps1.2.5.1.4 "><p id="p1840214220158"><a name="p1840214220158"></a><a name="p1840214220158"></a>AT命令字符串。</p>
    </td>
    </tr>
    <tr id="row840254291514"><td class="cellrowborder" valign="top" width="17.349999999999998%" headers="mcps1.2.5.1.1 "><p id="p1402242131512"><a name="p1402242131512"></a><a name="p1402242131512"></a>data</p>
    </td>
    <td class="cellrowborder" valign="top" width="24.14%" headers="mcps1.2.5.1.2 "><p id="p20402842161518"><a name="p20402842161518"></a><a name="p20402842161518"></a>出参</p>
    </td>
    <td class="cellrowborder" valign="top" width="26.700000000000003%" headers="mcps1.2.5.1.3 "><p id="p12402134211158"><a name="p12402134211158"></a><a name="p12402134211158"></a>uint8_t*</p>
    </td>
    <td class="cellrowborder" valign="top" width="31.81%" headers="mcps1.2.5.1.4 "><p id="p19402144210153"><a name="p19402144210153"></a><a name="p19402144210153"></a>AT命令返回的结果字符串。</p>
    </td>
    </tr>
    <tr id="row0402174281512"><td class="cellrowborder" valign="top" width="17.349999999999998%" headers="mcps1.2.5.1.1 "><p id="p15402942161511"><a name="p15402942161511"></a><a name="p15402942161511"></a>size</p>
    </td>
    <td class="cellrowborder" valign="top" width="24.14%" headers="mcps1.2.5.1.2 "><p id="p17402942171512"><a name="p17402942171512"></a><a name="p17402942171512"></a>入参</p>
    </td>
    <td class="cellrowborder" valign="top" width="26.700000000000003%" headers="mcps1.2.5.1.3 "><p id="p14402114291519"><a name="p14402114291519"></a><a name="p14402114291519"></a>uint16_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="31.81%" headers="mcps1.2.5.1.4 "><p id="p1940284214154"><a name="p1940284214154"></a><a name="p1940284214154"></a>AT命令结果字符串的长度。</p>
    </td>
    </tr>
    <tr id="row74025423154"><td class="cellrowborder" valign="top" width="17.349999999999998%" headers="mcps1.2.5.1.1 "><p id="p74021042111513"><a name="p74021042111513"></a><a name="p74021042111513"></a>times</p>
    </td>
    <td class="cellrowborder" valign="top" width="24.14%" headers="mcps1.2.5.1.2 "><p id="p1940216421153"><a name="p1940216421153"></a><a name="p1940216421153"></a>入参</p>
    </td>
    <td class="cellrowborder" valign="top" width="26.700000000000003%" headers="mcps1.2.5.1.3 "><p id="p5402942181517"><a name="p5402942181517"></a><a name="p5402942181517"></a>uint32_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="31.81%" headers="mcps1.2.5.1.4 "><p id="p94026428156"><a name="p94026428156"></a><a name="p94026428156"></a>指定接收多少条AT命令执行回复结果。</p>
    </td>
    </tr>
    <tr id="row174021242181511"><td class="cellrowborder" valign="top" width="17.349999999999998%" headers="mcps1.2.5.1.1 "><p id="p174021042131518"><a name="p174021042131518"></a><a name="p174021042131518"></a>timeout</p>
    </td>
    <td class="cellrowborder" valign="top" width="24.14%" headers="mcps1.2.5.1.2 "><p id="p1740244218151"><a name="p1740244218151"></a><a name="p1740244218151"></a>入参</p>
    </td>
    <td class="cellrowborder" valign="top" width="26.700000000000003%" headers="mcps1.2.5.1.3 "><p id="p4402164215156"><a name="p4402164215156"></a><a name="p4402164215156"></a>uint32_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="31.81%" headers="mcps1.2.5.1.4 "><p id="p1440244291510"><a name="p1440244291510"></a><a name="p1440244291510"></a>同步执行AT命令超时时间。</p>
    </td>
    </tr>
    </tbody>
    </table>

    **表 4**  返回值说明

    <a name="table0304204251511"></a>
    <table><thead align="left"><tr id="row1340234220152"><th class="cellrowborder" valign="top" width="18.37%" id="mcps1.2.4.1.1"><p id="p84028429158"><a name="p84028429158"></a><a name="p84028429158"></a>返回值</p>
    </th>
    <th class="cellrowborder" valign="top" width="28.57%" id="mcps1.2.4.1.2"><p id="p12402642151510"><a name="p12402642151510"></a><a name="p12402642151510"></a>参数结构类型</p>
    </th>
    <th class="cellrowborder" valign="top" width="53.059999999999995%" id="mcps1.2.4.1.3"><p id="p1402942111512"><a name="p1402942111512"></a><a name="p1402942111512"></a>说明</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row140214420151"><td class="cellrowborder" valign="top" width="18.37%" headers="mcps1.2.4.1.1 "><p id="p04036423155"><a name="p04036423155"></a><a name="p04036423155"></a>0</p>
    </td>
    <td class="cellrowborder" valign="top" width="28.57%" headers="mcps1.2.4.1.2 "><p id="p174031542171512"><a name="p174031542171512"></a><a name="p174031542171512"></a>int32_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="53.059999999999995%" headers="mcps1.2.4.1.3 "><p id="p1040374221513"><a name="p1040374221513"></a><a name="p1040374221513"></a>执行同步AT命令接口处理成功。</p>
    </td>
    </tr>
    <tr id="row3403942161517"><td class="cellrowborder" valign="top" width="18.37%" headers="mcps1.2.4.1.1 "><p id="p11403204251517"><a name="p11403204251517"></a><a name="p11403204251517"></a>&gt;0</p>
    </td>
    <td class="cellrowborder" valign="top" width="28.57%" headers="mcps1.2.4.1.2 "><p id="p74034429152"><a name="p74034429152"></a><a name="p74034429152"></a>int32_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="53.059999999999995%" headers="mcps1.2.4.1.3 "><p id="p140315428150"><a name="p140315428150"></a><a name="p140315428150"></a>执行同步AT命令接口处理失败。</p>
    </td>
    </tr>
    </tbody>
    </table>

#### AT命令注册回调<a name="ZH-CN_TOPIC_0000002492261636"></a>

函数原型：void ril\_register\_at\_reply\_callback\(respond\_callback\_t rsp\_callback\);

函数功能：向AT注册回调。

**表 1**  参数说明

<a name="zh-cn_topic_0000002230881137_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002230881137_row12908457921"><th class="cellrowborder" valign="top" width="17.740000000000002%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002230881137_p563315171132"><a name="zh-cn_topic_0000002230881137_p563315171132"></a><a name="zh-cn_topic_0000002230881137_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="29.330000000000002%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002230881137_p1463351718318"><a name="zh-cn_topic_0000002230881137_p1463351718318"></a><a name="zh-cn_topic_0000002230881137_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002230881137_p16338179319"><a name="zh-cn_topic_0000002230881137_p16338179319"></a><a name="zh-cn_topic_0000002230881137_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="32.35%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002230881137_p1463316170313"><a name="zh-cn_topic_0000002230881137_p1463316170313"></a><a name="zh-cn_topic_0000002230881137_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002230881137_row2184164910226"><td class="cellrowborder" valign="top" width="17.740000000000002%" headers="mcps1.2.5.1.1 "><p id="p10804125817"><a name="p10804125817"></a><a name="p10804125817"></a>rsp_callback</p>
</td>
<td class="cellrowborder" valign="top" width="29.330000000000002%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002230881137_p7184204918225"><a name="zh-cn_topic_0000002230881137_p7184204918225"></a><a name="zh-cn_topic_0000002230881137_p7184204918225"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.2.5.1.3 "><p id="p38851912818"><a name="p38851912818"></a><a name="p38851912818"></a>respond_callback_t</p>
</td>
<td class="cellrowborder" valign="top" width="32.35%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002230881137_p4184649142215"><a name="zh-cn_topic_0000002230881137_p4184649142215"></a><a name="zh-cn_topic_0000002230881137_p4184649142215"></a>回调函数：</p>
<p id="p71811337814"><a name="p71811337814"></a><a name="p71811337814"></a>typedef int32_t (*respond_callback_t)(uint8_t* data, size_t len);</p>
</td>
</tr>
</tbody>
</table>

### 短信接口<a name="ZH-CN_TOPIC_0000002509728291"></a>

-   **[查询短信中心号码](#ZH-CN_TOPIC_0000002477808340)**  

-   **[接收单条短信](#ZH-CN_TOPIC_0000002477648366)**  

-   **[发送单条短信](#ZH-CN_TOPIC_0000002509848323)**  

#### 查询短信中心号码<a name="ZH-CN_TOPIC_0000002477808340"></a>

函数原型：int32\_t  ril\_get\_sms\_smsc\(char\* smsc, size\_t len\);

函数功能：查询当前短信地址。

**表 1**  参数说明

<a name="zh-cn_topic_0000002311124540_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002311124540_row12908457921"><th class="cellrowborder" valign="top" width="21.29%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002311124540_p563315171132"><a name="zh-cn_topic_0000002311124540_p563315171132"></a><a name="zh-cn_topic_0000002311124540_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="28.71%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002311124540_p1463351718318"><a name="zh-cn_topic_0000002311124540_p1463351718318"></a><a name="zh-cn_topic_0000002311124540_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002311124540_p16338179319"><a name="zh-cn_topic_0000002311124540_p16338179319"></a><a name="zh-cn_topic_0000002311124540_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002311124540_p1463316170313"><a name="zh-cn_topic_0000002311124540_p1463316170313"></a><a name="zh-cn_topic_0000002311124540_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002311124540_row720819260296"><td class="cellrowborder" valign="top" width="21.29%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002311124540_p92094262291"><a name="zh-cn_topic_0000002311124540_p92094262291"></a><a name="zh-cn_topic_0000002311124540_p92094262291"></a>smsc</p>
</td>
<td class="cellrowborder" valign="top" width="28.71%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002311124540_p6209182662913"><a name="zh-cn_topic_0000002311124540_p6209182662913"></a><a name="zh-cn_topic_0000002311124540_p6209182662913"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002311124540_p1620982602914"><a name="zh-cn_topic_0000002311124540_p1620982602914"></a><a name="zh-cn_topic_0000002311124540_p1620982602914"></a>char*</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002311124540_p18209162602911"><a name="zh-cn_topic_0000002311124540_p18209162602911"></a><a name="zh-cn_topic_0000002311124540_p18209162602911"></a>短信中心号码。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002311124540_row1830174962413"><td class="cellrowborder" valign="top" width="21.29%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002311124540_p13145102016548"><a name="zh-cn_topic_0000002311124540_p13145102016548"></a><a name="zh-cn_topic_0000002311124540_p13145102016548"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="28.71%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002311124540_p149081857623"><a name="zh-cn_topic_0000002311124540_p149081857623"></a><a name="zh-cn_topic_0000002311124540_p149081857623"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002311124540_p29081571426"><a name="zh-cn_topic_0000002311124540_p29081571426"></a><a name="zh-cn_topic_0000002311124540_p29081571426"></a>size_t</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002311124540_p2672149143020"><a name="zh-cn_topic_0000002311124540_p2672149143020"></a><a name="zh-cn_topic_0000002311124540_p2672149143020"></a>smsc长度。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002311124540_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002311124540_row82122391941"><th class="cellrowborder" valign="top" width="28.622862286228624%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002311124540_p15896553644"><a name="zh-cn_topic_0000002311124540_p15896553644"></a><a name="zh-cn_topic_0000002311124540_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="32.34323432343235%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002311124540_p489612532411"><a name="zh-cn_topic_0000002311124540_p489612532411"></a><a name="zh-cn_topic_0000002311124540_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="39.033903390339034%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002311124540_p148968533417"><a name="zh-cn_topic_0000002311124540_p148968533417"></a><a name="zh-cn_topic_0000002311124540_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002311124540_row5213113915410"><td class="cellrowborder" valign="top" width="28.622862286228624%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002311124540_p14111210255"><a name="zh-cn_topic_0000002311124540_p14111210255"></a><a name="zh-cn_topic_0000002311124540_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="32.34323432343235%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002311124540_p14411151012512"><a name="zh-cn_topic_0000002311124540_p14411151012512"></a><a name="zh-cn_topic_0000002311124540_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="39.033903390339034%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002311124540_p151469557531"><a name="zh-cn_topic_0000002311124540_p151469557531"></a><a name="zh-cn_topic_0000002311124540_p151469557531"></a>执行查询短信中心号码接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002311124540_row82131139142"><td class="cellrowborder" valign="top" width="28.622862286228624%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002311124540_p1141117102516"><a name="zh-cn_topic_0000002311124540_p1141117102516"></a><a name="zh-cn_topic_0000002311124540_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="32.34323432343235%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002311124540_p164111810052"><a name="zh-cn_topic_0000002311124540_p164111810052"></a><a name="zh-cn_topic_0000002311124540_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="39.033903390339034%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002311124540_p214655565313"><a name="zh-cn_topic_0000002311124540_p214655565313"></a><a name="zh-cn_topic_0000002311124540_p214655565313"></a>执行查询短信中心号码接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 接收单条短信<a name="ZH-CN_TOPIC_0000002477648366"></a>

函数原型：int32\_t ril\_ue\_read\_short\_message\(int8\_t sms\_index, uint8\_t \*sms\_data, int16\_t read\_max\_length\);

函数功能：接收单条短信，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002345123801_table13523201771617"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002345123801_row105711317161615"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002345123801_p6571817101618"><a name="zh-cn_topic_0000002345123801_p6571817101618"></a><a name="zh-cn_topic_0000002345123801_p6571817101618"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002345123801_p195715174168"><a name="zh-cn_topic_0000002345123801_p195715174168"></a><a name="zh-cn_topic_0000002345123801_p195715174168"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002345123801_p5571151710161"><a name="zh-cn_topic_0000002345123801_p5571151710161"></a><a name="zh-cn_topic_0000002345123801_p5571151710161"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002345123801_p65711617121614"><a name="zh-cn_topic_0000002345123801_p65711617121614"></a><a name="zh-cn_topic_0000002345123801_p65711617121614"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002345123801_row7571191716162"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002345123801_p5571191701618"><a name="zh-cn_topic_0000002345123801_p5571191701618"></a><a name="zh-cn_topic_0000002345123801_p5571191701618"></a>sms_index</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002345123801_p2571151714169"><a name="zh-cn_topic_0000002345123801_p2571151714169"></a><a name="zh-cn_topic_0000002345123801_p2571151714169"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002345123801_p18571181791616"><a name="zh-cn_topic_0000002345123801_p18571181791616"></a><a name="zh-cn_topic_0000002345123801_p18571181791616"></a>int8_t</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002345123801_p6571181719166"><a name="zh-cn_topic_0000002345123801_p6571181719166"></a><a name="zh-cn_topic_0000002345123801_p6571181719166"></a>查询短信的索引。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002345123801_row1757131771614"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002345123801_p35710174165"><a name="zh-cn_topic_0000002345123801_p35710174165"></a><a name="zh-cn_topic_0000002345123801_p35710174165"></a>sms_data</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002345123801_p05711517161611"><a name="zh-cn_topic_0000002345123801_p05711517161611"></a><a name="zh-cn_topic_0000002345123801_p05711517161611"></a>出参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002345123801_p1857111176164"><a name="zh-cn_topic_0000002345123801_p1857111176164"></a><a name="zh-cn_topic_0000002345123801_p1857111176164"></a>uint8_t *</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002345123801_p165712178166"><a name="zh-cn_topic_0000002345123801_p165712178166"></a><a name="zh-cn_topic_0000002345123801_p165712178166"></a>短信内容。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002345123801_row257113177169"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002345123801_p257101741610"><a name="zh-cn_topic_0000002345123801_p257101741610"></a><a name="zh-cn_topic_0000002345123801_p257101741610"></a>read_max_length</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002345123801_p1357117177163"><a name="zh-cn_topic_0000002345123801_p1357117177163"></a><a name="zh-cn_topic_0000002345123801_p1357117177163"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002345123801_p14571181761618"><a name="zh-cn_topic_0000002345123801_p14571181761618"></a><a name="zh-cn_topic_0000002345123801_p14571181761618"></a>int16_t</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002345123801_p1571617191612"><a name="zh-cn_topic_0000002345123801_p1571617191612"></a><a name="zh-cn_topic_0000002345123801_p1571617191612"></a>短信内容接收长度。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002345123801_table16532121731613"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002345123801_row35710174168"><th class="cellrowborder" valign="top" width="24.490000000000002%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002345123801_p6571131701617"><a name="zh-cn_topic_0000002345123801_p6571131701617"></a><a name="zh-cn_topic_0000002345123801_p6571131701617"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="27.55%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002345123801_p3571817171617"><a name="zh-cn_topic_0000002345123801_p3571817171617"></a><a name="zh-cn_topic_0000002345123801_p3571817171617"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="47.96%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002345123801_p145711217151616"><a name="zh-cn_topic_0000002345123801_p145711217151616"></a><a name="zh-cn_topic_0000002345123801_p145711217151616"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002345123801_row75713172168"><td class="cellrowborder" valign="top" width="24.490000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002345123801_p11571171716166"><a name="zh-cn_topic_0000002345123801_p11571171716166"></a><a name="zh-cn_topic_0000002345123801_p11571171716166"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="27.55%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002345123801_p135715179167"><a name="zh-cn_topic_0000002345123801_p135715179167"></a><a name="zh-cn_topic_0000002345123801_p135715179167"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="47.96%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002345123801_p65725177164"><a name="zh-cn_topic_0000002345123801_p65725177164"></a><a name="zh-cn_topic_0000002345123801_p65725177164"></a>执行接收单条短信接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002345123801_row757210178161"><td class="cellrowborder" valign="top" width="24.490000000000002%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002345123801_p10572181721618"><a name="zh-cn_topic_0000002345123801_p10572181721618"></a><a name="zh-cn_topic_0000002345123801_p10572181721618"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="27.55%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002345123801_p135723179161"><a name="zh-cn_topic_0000002345123801_p135723179161"></a><a name="zh-cn_topic_0000002345123801_p135723179161"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="47.96%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002345123801_p65721178165"><a name="zh-cn_topic_0000002345123801_p65721178165"></a><a name="zh-cn_topic_0000002345123801_p65721178165"></a>执行接收单条短信接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

#### 发送单条短信<a name="ZH-CN_TOPIC_0000002509848323"></a>

函数原型：int32\_t ril\_ue\_send\_short\_message\(const char\* number, const char \*sms\_data, uint16\_t sms\_len\)

函数功能：发送单条短信，核内交互。

**表 1**  参数说明

<a name="zh-cn_topic_0000002345003605_table1908115715211"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002345003605_row12908457921"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002345003605_p563315171132"><a name="zh-cn_topic_0000002345003605_p563315171132"></a><a name="zh-cn_topic_0000002345003605_p563315171132"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="21.83%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002345003605_p1463351718318"><a name="zh-cn_topic_0000002345003605_p1463351718318"></a><a name="zh-cn_topic_0000002345003605_p1463351718318"></a>参数描述类型</p>
</th>
<th class="cellrowborder" valign="top" width="23.46%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002345003605_p16338179319"><a name="zh-cn_topic_0000002345003605_p16338179319"></a><a name="zh-cn_topic_0000002345003605_p16338179319"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="29.709999999999997%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002345003605_p1463316170313"><a name="zh-cn_topic_0000002345003605_p1463316170313"></a><a name="zh-cn_topic_0000002345003605_p1463316170313"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002345003605_row2184164910226"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002345003605_p171841249102217"><a name="zh-cn_topic_0000002345003605_p171841249102217"></a><a name="zh-cn_topic_0000002345003605_p171841249102217"></a>number</p>
</td>
<td class="cellrowborder" valign="top" width="21.83%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002345003605_p7184204918225"><a name="zh-cn_topic_0000002345003605_p7184204918225"></a><a name="zh-cn_topic_0000002345003605_p7184204918225"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="23.46%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002345003605_p6184184952213"><a name="zh-cn_topic_0000002345003605_p6184184952213"></a><a name="zh-cn_topic_0000002345003605_p6184184952213"></a>const char*</p>
</td>
<td class="cellrowborder" valign="top" width="29.709999999999997%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002345003605_p4184649142215"><a name="zh-cn_topic_0000002345003605_p4184649142215"></a><a name="zh-cn_topic_0000002345003605_p4184649142215"></a>短信发送目的号码。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002345003605_row720819260296"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002345003605_p92094262291"><a name="zh-cn_topic_0000002345003605_p92094262291"></a><a name="zh-cn_topic_0000002345003605_p92094262291"></a>sms_data</p>
</td>
<td class="cellrowborder" valign="top" width="21.83%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002345003605_p6209182662913"><a name="zh-cn_topic_0000002345003605_p6209182662913"></a><a name="zh-cn_topic_0000002345003605_p6209182662913"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="23.46%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002345003605_p1620982602914"><a name="zh-cn_topic_0000002345003605_p1620982602914"></a><a name="zh-cn_topic_0000002345003605_p1620982602914"></a>const char*</p>
</td>
<td class="cellrowborder" valign="top" width="29.709999999999997%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002345003605_p18209162602911"><a name="zh-cn_topic_0000002345003605_p18209162602911"></a><a name="zh-cn_topic_0000002345003605_p18209162602911"></a>短信内容。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002345003605_row1830174962413"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002345003605_p13145102016548"><a name="zh-cn_topic_0000002345003605_p13145102016548"></a><a name="zh-cn_topic_0000002345003605_p13145102016548"></a>sms_len</p>
</td>
<td class="cellrowborder" valign="top" width="21.83%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002345003605_p149081857623"><a name="zh-cn_topic_0000002345003605_p149081857623"></a><a name="zh-cn_topic_0000002345003605_p149081857623"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="23.46%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002345003605_p29081571426"><a name="zh-cn_topic_0000002345003605_p29081571426"></a><a name="zh-cn_topic_0000002345003605_p29081571426"></a>uint16_t</p>
</td>
<td class="cellrowborder" valign="top" width="29.709999999999997%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002345003605_p2672149143020"><a name="zh-cn_topic_0000002345003605_p2672149143020"></a><a name="zh-cn_topic_0000002345003605_p2672149143020"></a>短信内容长度。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="zh-cn_topic_0000002345003605_table92121393411"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002345003605_row82122391941"><th class="cellrowborder" valign="top" width="25.302530253025303%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002345003605_p15896553644"><a name="zh-cn_topic_0000002345003605_p15896553644"></a><a name="zh-cn_topic_0000002345003605_p15896553644"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="32.24322432243225%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002345003605_p489612532411"><a name="zh-cn_topic_0000002345003605_p489612532411"></a><a name="zh-cn_topic_0000002345003605_p489612532411"></a>参数结构类型</p>
</th>
<th class="cellrowborder" valign="top" width="42.45424542454245%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002345003605_p148968533417"><a name="zh-cn_topic_0000002345003605_p148968533417"></a><a name="zh-cn_topic_0000002345003605_p148968533417"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002345003605_row5213113915410"><td class="cellrowborder" valign="top" width="25.302530253025303%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002345003605_p14111210255"><a name="zh-cn_topic_0000002345003605_p14111210255"></a><a name="zh-cn_topic_0000002345003605_p14111210255"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="32.24322432243225%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002345003605_p14411151012512"><a name="zh-cn_topic_0000002345003605_p14411151012512"></a><a name="zh-cn_topic_0000002345003605_p14411151012512"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="42.45424542454245%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002345003605_p151469557531"><a name="zh-cn_topic_0000002345003605_p151469557531"></a><a name="zh-cn_topic_0000002345003605_p151469557531"></a>执行发送单条短信接口处理成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002345003605_row82131139142"><td class="cellrowborder" valign="top" width="25.302530253025303%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002345003605_p1141117102516"><a name="zh-cn_topic_0000002345003605_p1141117102516"></a><a name="zh-cn_topic_0000002345003605_p1141117102516"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="32.24322432243225%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002345003605_p164111810052"><a name="zh-cn_topic_0000002345003605_p164111810052"></a><a name="zh-cn_topic_0000002345003605_p164111810052"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="42.45424542454245%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002345003605_p214655565313"><a name="zh-cn_topic_0000002345003605_p214655565313"></a><a name="zh-cn_topic_0000002345003605_p214655565313"></a>执行发送单条短信接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

### 升级接口<a name="ZH-CN_TOPIC_0000002436334706"></a>

Hi2131升级包可参考《HiDiTingV100 FOTA升级包制作工具 用户指南》制作，通过ril升级接口，可实现Hi2131芯片的版本更新、固件升级，升级流程：开始升级 -\> 发送数据包1 -\> 发送数据包2 -\> 发送数据包n -\> 结束升级。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>Hi2131差分升级包大小不能大于350KByte。

-   **[开始升级通知](#ZH-CN_TOPIC_0000002436494538)**  

-   **[升级完成通知](#ZH-CN_TOPIC_0000002469973005)**  

-   **[发送升级包数据](#ZH-CN_TOPIC_0000002469853157)**  

#### 开始升级通知<a name="ZH-CN_TOPIC_0000002436494538"></a>

函数原型：int32\_t ril\_systerm\_updated\_start\(void\)

函数功能：升级开始通知，Hi2131开启升级。

**表 1**  返回值说明

<a name="table138796361156"></a>
<table><thead align="left"><tr id="row1092063610154"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="p19201360157"><a name="p19201360157"></a><a name="p19201360157"></a><strong id="b1692083621511"><a name="b1692083621511"></a><a name="b1692083621511"></a>返回值</strong></p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="p1892018361153"><a name="p1892018361153"></a><a name="p1892018361153"></a><strong id="b1692093691519"><a name="b1692093691519"></a><a name="b1692093691519"></a>参数结构类型</strong></p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="p09211336101519"><a name="p09211336101519"></a><a name="p09211336101519"></a><strong id="b11921536151519"><a name="b11921536151519"></a><a name="b11921536151519"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row1592110366152"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="p1692103681519"><a name="p1692103681519"></a><a name="p1692103681519"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="p1692133618152"><a name="p1692133618152"></a><a name="p1692133618152"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p79211736161511"><a name="p79211736161511"></a><a name="p79211736161511"></a>执行升级开始通知成功。</p>
</td>
</tr>
<tr id="row4921136181514"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="p5921236131514"><a name="p5921236131514"></a><a name="p5921236131514"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="p1921183631511"><a name="p1921183631511"></a><a name="p1921183631511"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p1492143661512"><a name="p1492143661512"></a><a name="p1492143661512"></a>执行升级开始通知失败。</p>
</td>
</tr>
</tbody>
</table>

#### 升级完成通知<a name="ZH-CN_TOPIC_0000002469973005"></a>

函数原型：int32\_t ril\_systerm\_updated\_end\(void\)

函数功能：升级完成通知，Hi2131完成升级。

**表 1**  返回值说明

<a name="table1659421214163"></a>
<table><thead align="left"><tr id="row12611111221616"><th class="cellrowborder" valign="top" width="26.862686268626863%" id="mcps1.2.4.1.1"><p id="p11611151218165"><a name="p11611151218165"></a><a name="p11611151218165"></a><strong id="b1061151219169"><a name="b1061151219169"></a><a name="b1061151219169"></a>返回值</strong></p>
</th>
<th class="cellrowborder" valign="top" width="36.19361936193619%" id="mcps1.2.4.1.2"><p id="p1561141201618"><a name="p1561141201618"></a><a name="p1561141201618"></a><strong id="b861111218162"><a name="b861111218162"></a><a name="b861111218162"></a>参数结构类型</strong></p>
</th>
<th class="cellrowborder" valign="top" width="36.94369436943694%" id="mcps1.2.4.1.3"><p id="p7611812101617"><a name="p7611812101617"></a><a name="p7611812101617"></a><strong id="b061151217168"><a name="b061151217168"></a><a name="b061151217168"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row06111129165"><td class="cellrowborder" valign="top" width="26.862686268626863%" headers="mcps1.2.4.1.1 "><p id="p16112122166"><a name="p16112122166"></a><a name="p16112122166"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="36.19361936193619%" headers="mcps1.2.4.1.2 "><p id="p19611612141615"><a name="p19611612141615"></a><a name="p19611612141615"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="36.94369436943694%" headers="mcps1.2.4.1.3 "><p id="p16112012171619"><a name="p16112012171619"></a><a name="p16112012171619"></a>执行升级完成通知成功。</p>
</td>
</tr>
<tr id="row1861112124163"><td class="cellrowborder" valign="top" width="26.862686268626863%" headers="mcps1.2.4.1.1 "><p id="p161141218160"><a name="p161141218160"></a><a name="p161141218160"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="36.19361936193619%" headers="mcps1.2.4.1.2 "><p id="p16119128165"><a name="p16119128165"></a><a name="p16119128165"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="36.94369436943694%" headers="mcps1.2.4.1.3 "><p id="p461214121162"><a name="p461214121162"></a><a name="p461214121162"></a>执行升级完成通知失败。</p>
</td>
</tr>
</tbody>
</table>

#### 发送升级包数据<a name="ZH-CN_TOPIC_0000002469853157"></a>

函数原型：int32\_t ril\_systerm\_updated\_download\_package\(ril\_upadated\_package\_t  \*upadated\_package\)

函数功能：发送升级包到Hi2131。

**表 1**  参数说明

<a name="table996543451618"></a>
<table><thead align="left"><tr id="row1799410348168"><th class="cellrowborder" valign="top" width="25.510000000000005%" id="mcps1.2.5.1.1"><p id="p1899413417166"><a name="p1899413417166"></a><a name="p1899413417166"></a><strong id="b9994123414167"><a name="b9994123414167"></a><a name="b9994123414167"></a>参数</strong></p>
</th>
<th class="cellrowborder" valign="top" width="19.39%" id="mcps1.2.5.1.2"><p id="p139941434141610"><a name="p139941434141610"></a><a name="p139941434141610"></a><strong id="b1994113417167"><a name="b1994113417167"></a><a name="b1994113417167"></a>参数描述类型</strong></p>
</th>
<th class="cellrowborder" valign="top" width="20.720000000000002%" id="mcps1.2.5.1.3"><p id="p14994834121612"><a name="p14994834121612"></a><a name="p14994834121612"></a><strong id="b19941334181615"><a name="b19941334181615"></a><a name="b19941334181615"></a>参数结构类型</strong></p>
</th>
<th class="cellrowborder" valign="top" width="34.38%" id="mcps1.2.5.1.4"><p id="p7994234101617"><a name="p7994234101617"></a><a name="p7994234101617"></a><strong id="b899583411613"><a name="b899583411613"></a><a name="b899583411613"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row199951134141610"><td class="cellrowborder" valign="top" width="25.510000000000005%" headers="mcps1.2.5.1.1 "><p id="p99955340169"><a name="p99955340169"></a><a name="p99955340169"></a>upadated_package</p>
</td>
<td class="cellrowborder" valign="top" width="19.39%" headers="mcps1.2.5.1.2 "><p id="p19951134131611"><a name="p19951134131611"></a><a name="p19951134131611"></a>入参</p>
</td>
<td class="cellrowborder" valign="top" width="20.720000000000002%" headers="mcps1.2.5.1.3 "><p id="p9995123451619"><a name="p9995123451619"></a><a name="p9995123451619"></a>ril_upadated_package_t;</p>
</td>
<td class="cellrowborder" valign="top" width="34.38%" headers="mcps1.2.5.1.4 "><p id="p499517346163"><a name="p499517346163"></a><a name="p499517346163"></a>download_type：当前固定为1，默认升级包数据保存到文件系统中。</p>
<p id="p15995163421615"><a name="p15995163421615"></a><a name="p15995163421615"></a>Sn：序列号。应从0开始，每个数据包段递增1</p>
<p id="p189951349161"><a name="p189951349161"></a><a name="p189951349161"></a>Len：数据长度，应为32/64/128/256/512。</p>
<p id="p17995434201611"><a name="p17995434201611"></a><a name="p17995434201611"></a>Crc：data段数据校验值crc8（数据包段中每个字节的xor8）。</p>
<p id="p11995183421610"><a name="p11995183421610"></a><a name="p11995183421610"></a>Data：升级包数据。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  返回值说明

<a name="table5968113471613"></a>
<table><thead align="left"><tr id="row0995534141615"><th class="cellrowborder" valign="top" width="24.28242824282428%" id="mcps1.2.4.1.1"><p id="p29951534161618"><a name="p29951534161618"></a><a name="p29951534161618"></a><strong id="b1199518346162"><a name="b1199518346162"></a><a name="b1199518346162"></a>返回值</strong></p>
</th>
<th class="cellrowborder" valign="top" width="28.062806280628067%" id="mcps1.2.4.1.2"><p id="p599503419162"><a name="p599503419162"></a><a name="p599503419162"></a><strong id="b1499516343167"><a name="b1499516343167"></a><a name="b1499516343167"></a>参数结构类型</strong></p>
</th>
<th class="cellrowborder" valign="top" width="47.65476547654765%" id="mcps1.2.4.1.3"><p id="p79952346168"><a name="p79952346168"></a><a name="p79952346168"></a><strong id="b119959348163"><a name="b119959348163"></a><a name="b119959348163"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row1995103412169"><td class="cellrowborder" valign="top" width="24.28242824282428%" headers="mcps1.2.4.1.1 "><p id="p49951347164"><a name="p49951347164"></a><a name="p49951347164"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="28.062806280628067%" headers="mcps1.2.4.1.2 "><p id="p5995183412162"><a name="p5995183412162"></a><a name="p5995183412162"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="47.65476547654765%" headers="mcps1.2.4.1.3 "><p id="p1299543441618"><a name="p1299543441618"></a><a name="p1299543441618"></a>执行升级包发送接口处理成功。</p>
</td>
</tr>
<tr id="row14995934181611"><td class="cellrowborder" valign="top" width="24.28242824282428%" headers="mcps1.2.4.1.1 "><p id="p1199583410163"><a name="p1199583410163"></a><a name="p1199583410163"></a>&gt;0</p>
</td>
<td class="cellrowborder" valign="top" width="28.062806280628067%" headers="mcps1.2.4.1.2 "><p id="p119951834191618"><a name="p119951834191618"></a><a name="p119951834191618"></a>int32_t</p>
</td>
<td class="cellrowborder" valign="top" width="47.65476547654765%" headers="mcps1.2.4.1.3 "><p id="p69951134191613"><a name="p69951134191613"></a><a name="p69951134191613"></a>执行升级包发送接口处理失败。</p>
</td>
</tr>
</tbody>
</table>

## VoLTE相关AT命令<a name="ZH-CN_TOPIC_0000002223667908"></a>

-   **[发起呼叫请求：D](#ZH-CN_TOPIC_0000002258467685)**  

-   **[接听来电：A](#ZH-CN_TOPIC_0000002258627569)**  

-   **[挂断电话：H](#ZH-CN_TOPIC_0000002223508072)**  

-   **[设置自动接听：S0](#ZH-CN_TOPIC_0000002223667912)**  

-   **[查询当前呼叫信息：+CLCC](#ZH-CN_TOPIC_0000002258467689)**  

-   **[查询IMS域注册状态：+CIREG](#ZH-CN_TOPIC_0000002258627573)**  

-   **[主动上报IMS域注册状态：+CIREGU](#ZH-CN_TOPIC_0000002223508076)**  

-   **[查询IMS域的虚拟号码：+VOLTEIMPU](#ZH-CN_TOPIC_0000002223667916)**  

-   **[IMS服务状态更新指示：+IMSSRVSTATUS](#ZH-CN_TOPIC_0000002258467693)**  

-   **[来电指示：IRING](#ZH-CN_TOPIC_0000002258627577)**  

-   **[发起语音呼叫指示：+ORIG](#ZH-CN_TOPIC_0000002223508080)**  

-   **[UE收到网络指示：+CONF](#ZH-CN_TOPIC_0000002223667920)**  

-   **[呼叫接通指示：+CONN](#ZH-CN_TOPIC_0000002258467697)**  

-   **[通话结束指示：+CEND](#ZH-CN_TOPIC_0000002258627581)**  

-   **[呼叫状态上报：+CALLSTATE](#ZH-CN_TOPIC_0000002223508084)**  

-   **[主动上报语音带宽信息：+CHANNELINFO](#ZH-CN_TOPIC_0000002223667924)**  

-   **[上报IMS注册失败原因：+IMSREGERR](#ZH-CN_TOPIC_0000002258467701)**  

-   **[IMS域被叫处理状态信息主动上报：+IMSMTRPT](#ZH-CN_TOPIC_0000002258627585)**  

-   **[关闭IMS功能开关：+IMSSWITCH](#ZH-CN_TOPIC_0000002392384649)**  

### 发起呼叫请求：D<a name="ZH-CN_TOPIC_0000002258467685"></a>

**命令描述<a name="zh-cn_topic_0000002175433040_section4340133117813"></a>**

发起一个呼叫。

**命令语法<a name="zh-cn_topic_0000002175433040_section27467181898"></a>**

<a name="zh-cn_topic_0000002175433040_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175433040_row139817074814"><th class="cellrowborder" valign="top" width="33.3033303330333%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002175433040_p173981104481"><a name="zh-cn_topic_0000002175433040_p173981104481"></a><a name="zh-cn_topic_0000002175433040_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="33.36333633363336%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002175433040_p339915016481"><a name="zh-cn_topic_0000002175433040_p339915016481"></a><a name="zh-cn_topic_0000002175433040_p339915016481"></a>返回结果</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002175433040_p4399902485"><a name="zh-cn_topic_0000002175433040_p4399902485"></a><a name="zh-cn_topic_0000002175433040_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175433040_row23995014487"><td class="cellrowborder" valign="top" width="33.3033303330333%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002175433040_p188599144348"><a name="zh-cn_topic_0000002175433040_p188599144348"></a><a name="zh-cn_topic_0000002175433040_p188599144348"></a>D[&lt;dial_string&gt;][I][G][;]</p>
</td>
<td class="cellrowborder" valign="top" width="33.36333633363336%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002175433040_p8482132103215"><a name="zh-cn_topic_0000002175433040_p8482132103215"></a><a name="zh-cn_topic_0000002175433040_p8482132103215"></a>OK</p>
<p id="zh-cn_topic_0000002175433040_p2795620181018"><a name="zh-cn_topic_0000002175433040_p2795620181018"></a><a name="zh-cn_topic_0000002175433040_p2795620181018"></a>错误情况：</p>
<p id="zh-cn_topic_0000002175433040_p1616mcpsimp"><a name="zh-cn_topic_0000002175433040_p1616mcpsimp"></a><a name="zh-cn_topic_0000002175433040_p1616mcpsimp"></a>+CME ERROR: &lt;err&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002175433040_p2723mcpsimp"><a name="zh-cn_topic_0000002175433040_p2723mcpsimp"></a><a name="zh-cn_topic_0000002175433040_p2723mcpsimp"></a>atd13900000000;</p>
<p id="zh-cn_topic_0000002175433040_p2724mcpsimp"><a name="zh-cn_topic_0000002175433040_p2724mcpsimp"></a><a name="zh-cn_topic_0000002175433040_p2724mcpsimp"></a>OK</p>
<p id="zh-cn_topic_0000002175433040_p182091316183817"><a name="zh-cn_topic_0000002175433040_p182091316183817"></a><a name="zh-cn_topic_0000002175433040_p182091316183817"></a>atd13900000000i;</p>
<p id="zh-cn_topic_0000002175433040_p1320961613816"><a name="zh-cn_topic_0000002175433040_p1320961613816"></a><a name="zh-cn_topic_0000002175433040_p1320961613816"></a>OK</p>
<p id="zh-cn_topic_0000002175433040_p62141718113813"><a name="zh-cn_topic_0000002175433040_p62141718113813"></a><a name="zh-cn_topic_0000002175433040_p62141718113813"></a>atd13900000000I;</p>
<p id="zh-cn_topic_0000002175433040_p1021481863814"><a name="zh-cn_topic_0000002175433040_p1021481863814"></a><a name="zh-cn_topic_0000002175433040_p1021481863814"></a>OK</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002175433040_section45087272918"></a>**

<a name="zh-cn_topic_0000002175433040_table198852334716"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175433040_row1881723154712"><th class="cellrowborder" valign="top" width="24.57%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002175433040_p78862364716"><a name="zh-cn_topic_0000002175433040_p78862364716"></a><a name="zh-cn_topic_0000002175433040_p78862364716"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="75.42999999999999%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002175433040_p4888231472"><a name="zh-cn_topic_0000002175433040_p4888231472"></a><a name="zh-cn_topic_0000002175433040_p4888231472"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175433040_row488023104710"><td class="cellrowborder" valign="top" width="24.57%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433040_p10819326112014"><a name="zh-cn_topic_0000002175433040_p10819326112014"></a><a name="zh-cn_topic_0000002175433040_p10819326112014"></a>&lt;dial_string&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="75.42999999999999%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433040_p15601924111314"><a name="zh-cn_topic_0000002175433040_p15601924111314"></a><a name="zh-cn_topic_0000002175433040_p15601924111314"></a>字符串类型。呼叫号码。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175433040_row38515511417"><td class="cellrowborder" valign="top" width="24.57%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433040_p1813295418202"><a name="zh-cn_topic_0000002175433040_p1813295418202"></a><a name="zh-cn_topic_0000002175433040_p1813295418202"></a>[I]</p>
</td>
<td class="cellrowborder" valign="top" width="75.42999999999999%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433040_p1384412132112"><a name="zh-cn_topic_0000002175433040_p1384412132112"></a><a name="zh-cn_topic_0000002175433040_p1384412132112"></a>如果命令中带有I，表示在这次呼叫中禁止显示主叫号码；</p>
<p id="zh-cn_topic_0000002175433040_p1784415222115"><a name="zh-cn_topic_0000002175433040_p1784415222115"></a><a name="zh-cn_topic_0000002175433040_p1784415222115"></a>如果命令中带有i，表示在这次呼叫中允许显示主叫号码。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175433040_row46721735193618"><td class="cellrowborder" valign="top" width="24.57%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433040_p18672113510361"><a name="zh-cn_topic_0000002175433040_p18672113510361"></a><a name="zh-cn_topic_0000002175433040_p18672113510361"></a>[G]</p>
</td>
<td class="cellrowborder" valign="top" width="75.42999999999999%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433040_p1467212356361"><a name="zh-cn_topic_0000002175433040_p1467212356361"></a><a name="zh-cn_topic_0000002175433040_p1467212356361"></a>暂不支持。一般表示在这次呼叫中使能CUG补充业务。</p>
</td>
</tr>
</tbody>
</table>

### 接听来电：A<a name="ZH-CN_TOPIC_0000002258627569"></a>

**命令描述<a name="zh-cn_topic_0000002175273328_section4340133117813"></a>**

接听当前的来电。

**命令语法<a name="zh-cn_topic_0000002175273328_section27467181898"></a>**

<a name="zh-cn_topic_0000002175273328_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175273328_row139817074814"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002175273328_p173981104481"><a name="zh-cn_topic_0000002175273328_p173981104481"></a><a name="zh-cn_topic_0000002175273328_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002175273328_p339915016481"><a name="zh-cn_topic_0000002175273328_p339915016481"></a><a name="zh-cn_topic_0000002175273328_p339915016481"></a>返回结果</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002175273328_p4399902485"><a name="zh-cn_topic_0000002175273328_p4399902485"></a><a name="zh-cn_topic_0000002175273328_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175273328_row23995014487"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002175273328_p188599144348"><a name="zh-cn_topic_0000002175273328_p188599144348"></a><a name="zh-cn_topic_0000002175273328_p188599144348"></a>A</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002175273328_p8482132103215"><a name="zh-cn_topic_0000002175273328_p8482132103215"></a><a name="zh-cn_topic_0000002175273328_p8482132103215"></a>OK</p>
<p id="zh-cn_topic_0000002175273328_p2795620181018"><a name="zh-cn_topic_0000002175273328_p2795620181018"></a><a name="zh-cn_topic_0000002175273328_p2795620181018"></a>错误情况：</p>
<p id="zh-cn_topic_0000002175273328_p1616mcpsimp"><a name="zh-cn_topic_0000002175273328_p1616mcpsimp"></a><a name="zh-cn_topic_0000002175273328_p1616mcpsimp"></a>+CME ERROR: &lt;err&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002175273328_p2723mcpsimp"><a name="zh-cn_topic_0000002175273328_p2723mcpsimp"></a><a name="zh-cn_topic_0000002175273328_p2723mcpsimp"></a>ATA</p>
<p id="zh-cn_topic_0000002175273328_p2724mcpsimp"><a name="zh-cn_topic_0000002175273328_p2724mcpsimp"></a><a name="zh-cn_topic_0000002175273328_p2724mcpsimp"></a>OK</p>
</td>
</tr>
</tbody>
</table>

### 挂断电话：H<a name="ZH-CN_TOPIC_0000002223508072"></a>

**命令描述<a name="zh-cn_topic_0000002210793761_section4340133117813"></a>**

挂断电话。

**命令语法<a name="zh-cn_topic_0000002210793761_section27467181898"></a>**

<a name="zh-cn_topic_0000002210793761_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210793761_row139817074814"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002210793761_p173981104481"><a name="zh-cn_topic_0000002210793761_p173981104481"></a><a name="zh-cn_topic_0000002210793761_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002210793761_p339915016481"><a name="zh-cn_topic_0000002210793761_p339915016481"></a><a name="zh-cn_topic_0000002210793761_p339915016481"></a>返回结果</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002210793761_p4399902485"><a name="zh-cn_topic_0000002210793761_p4399902485"></a><a name="zh-cn_topic_0000002210793761_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210793761_row23995014487"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002210793761_p45201556142611"><a name="zh-cn_topic_0000002210793761_p45201556142611"></a><a name="zh-cn_topic_0000002210793761_p45201556142611"></a>H[&lt;value&gt;]</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002210793761_p8482132103215"><a name="zh-cn_topic_0000002210793761_p8482132103215"></a><a name="zh-cn_topic_0000002210793761_p8482132103215"></a>OK</p>
<p id="zh-cn_topic_0000002210793761_p2795620181018"><a name="zh-cn_topic_0000002210793761_p2795620181018"></a><a name="zh-cn_topic_0000002210793761_p2795620181018"></a>错误情况：</p>
<p id="zh-cn_topic_0000002210793761_p1616mcpsimp"><a name="zh-cn_topic_0000002210793761_p1616mcpsimp"></a><a name="zh-cn_topic_0000002210793761_p1616mcpsimp"></a>+CME ERROR: &lt;err&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002210793761_p2723mcpsimp"><a name="zh-cn_topic_0000002210793761_p2723mcpsimp"></a><a name="zh-cn_topic_0000002210793761_p2723mcpsimp"></a>ATH</p>
<p id="zh-cn_topic_0000002210793761_p2724mcpsimp"><a name="zh-cn_topic_0000002210793761_p2724mcpsimp"></a><a name="zh-cn_topic_0000002210793761_p2724mcpsimp"></a>OK</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002210793761_section45087272918"></a>**

<a name="zh-cn_topic_0000002210793761_table198852334716"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210793761_row1881723154712"><th class="cellrowborder" valign="top" width="24.64%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002210793761_p78862364716"><a name="zh-cn_topic_0000002210793761_p78862364716"></a><a name="zh-cn_topic_0000002210793761_p78862364716"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="75.36%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002210793761_p4888231472"><a name="zh-cn_topic_0000002210793761_p4888231472"></a><a name="zh-cn_topic_0000002210793761_p4888231472"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210793761_row488023104710"><td class="cellrowborder" valign="top" width="24.64%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210793761_p1994110212518"><a name="zh-cn_topic_0000002210793761_p1994110212518"></a><a name="zh-cn_topic_0000002210793761_p1994110212518"></a>&lt;value&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="75.36%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210793761_p486834713272"><a name="zh-cn_topic_0000002210793761_p486834713272"></a><a name="zh-cn_topic_0000002210793761_p486834713272"></a>整型值，仅支持取值为0。</p>
<p id="zh-cn_topic_0000002210793761_p58686477274"><a name="zh-cn_topic_0000002210793761_p58686477274"></a><a name="zh-cn_topic_0000002210793761_p58686477274"></a>取其他值时，响应结果均为ERROR。</p>
</td>
</tr>
</tbody>
</table>

### 设置自动接听：S0<a name="ZH-CN_TOPIC_0000002223667912"></a>

**命令描述<a name="zh-cn_topic_0000002210639397_section4340133117813"></a>**

设置自动应答。

**命令语法<a name="zh-cn_topic_0000002210639397_section27467181898"></a>**

<a name="zh-cn_topic_0000002210639397_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210639397_row139817074814"><th class="cellrowborder" valign="top" width="33.333333333333336%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002210639397_p173981104481"><a name="zh-cn_topic_0000002210639397_p173981104481"></a><a name="zh-cn_topic_0000002210639397_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="33.29332933293329%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002210639397_p339915016481"><a name="zh-cn_topic_0000002210639397_p339915016481"></a><a name="zh-cn_topic_0000002210639397_p339915016481"></a>返回结果</p>
</th>
<th class="cellrowborder" valign="top" width="33.373337333733375%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002210639397_p4399902485"><a name="zh-cn_topic_0000002210639397_p4399902485"></a><a name="zh-cn_topic_0000002210639397_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210639397_row23995014487"><td class="cellrowborder" valign="top" width="33.333333333333336%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002210639397_p19822026152819"><a name="zh-cn_topic_0000002210639397_p19822026152819"></a><a name="zh-cn_topic_0000002210639397_p19822026152819"></a>S0=&lt;n&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="33.29332933293329%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002210639397_p8482132103215"><a name="zh-cn_topic_0000002210639397_p8482132103215"></a><a name="zh-cn_topic_0000002210639397_p8482132103215"></a>OK</p>
<p id="zh-cn_topic_0000002210639397_p2795620181018"><a name="zh-cn_topic_0000002210639397_p2795620181018"></a><a name="zh-cn_topic_0000002210639397_p2795620181018"></a>错误情况：</p>
<p id="zh-cn_topic_0000002210639397_p1616mcpsimp"><a name="zh-cn_topic_0000002210639397_p1616mcpsimp"></a><a name="zh-cn_topic_0000002210639397_p1616mcpsimp"></a>+CME ERROR: &lt;err&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="33.373337333733375%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002210639397_p2723mcpsimp"><a name="zh-cn_topic_0000002210639397_p2723mcpsimp"></a><a name="zh-cn_topic_0000002210639397_p2723mcpsimp"></a>ATS0=5</p>
<p id="zh-cn_topic_0000002210639397_p2724mcpsimp"><a name="zh-cn_topic_0000002210639397_p2724mcpsimp"></a><a name="zh-cn_topic_0000002210639397_p2724mcpsimp"></a>OK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639397_row165365198451"><td class="cellrowborder" valign="top" width="33.333333333333336%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002210639397_p279153652813"><a name="zh-cn_topic_0000002210639397_p279153652813"></a><a name="zh-cn_topic_0000002210639397_p279153652813"></a>S0?</p>
</td>
<td class="cellrowborder" valign="top" width="33.29332933293329%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002210639397_p722612603918"><a name="zh-cn_topic_0000002210639397_p722612603918"></a><a name="zh-cn_topic_0000002210639397_p722612603918"></a>&lt;CR&gt;&lt;LF&gt;&lt;n&gt;&lt;CR&gt;&lt;LF&gt;</p>
<p id="zh-cn_topic_0000002210639397_p82261643918"><a name="zh-cn_topic_0000002210639397_p82261643918"></a><a name="zh-cn_topic_0000002210639397_p82261643918"></a>&lt;CR&gt;&lt;LF&gt;OK&lt;CR&gt;&lt;LF&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="33.373337333733375%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002210639397_p2734mcpsimp"><a name="zh-cn_topic_0000002210639397_p2734mcpsimp"></a><a name="zh-cn_topic_0000002210639397_p2734mcpsimp"></a>ATS0?</p>
<p id="zh-cn_topic_0000002210639397_p2735mcpsimp"><a name="zh-cn_topic_0000002210639397_p2735mcpsimp"></a><a name="zh-cn_topic_0000002210639397_p2735mcpsimp"></a>5</p>
<p id="zh-cn_topic_0000002210639397_p2737mcpsimp"><a name="zh-cn_topic_0000002210639397_p2737mcpsimp"></a><a name="zh-cn_topic_0000002210639397_p2737mcpsimp"></a>OK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639397_row1531755165811"><td class="cellrowborder" valign="top" width="33.333333333333336%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002210639397_p2741mcpsimp"><a name="zh-cn_topic_0000002210639397_p2741mcpsimp"></a><a name="zh-cn_topic_0000002210639397_p2741mcpsimp"></a>S0=?</p>
</td>
<td class="cellrowborder" valign="top" width="33.29332933293329%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002210639397_p5250203293918"><a name="zh-cn_topic_0000002210639397_p5250203293918"></a><a name="zh-cn_topic_0000002210639397_p5250203293918"></a>&lt;CR&gt;&lt;LF&gt;S0: (list of supported &lt;n&gt;s) &lt;CR&gt;&lt;LF&gt;</p>
<p id="zh-cn_topic_0000002210639397_p1725012322396"><a name="zh-cn_topic_0000002210639397_p1725012322396"></a><a name="zh-cn_topic_0000002210639397_p1725012322396"></a>&lt;CR&gt;&lt;LF&gt;OK&lt;CR&gt;&lt;LF&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="33.373337333733375%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002210639397_p2745mcpsimp"><a name="zh-cn_topic_0000002210639397_p2745mcpsimp"></a><a name="zh-cn_topic_0000002210639397_p2745mcpsimp"></a>ATS0=?</p>
<p id="zh-cn_topic_0000002210639397_p2746mcpsimp"><a name="zh-cn_topic_0000002210639397_p2746mcpsimp"></a><a name="zh-cn_topic_0000002210639397_p2746mcpsimp"></a>S0: (0-255)</p>
<p id="zh-cn_topic_0000002210639397_p2747mcpsimp"><a name="zh-cn_topic_0000002210639397_p2747mcpsimp"></a><a name="zh-cn_topic_0000002210639397_p2747mcpsimp"></a>OK</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002210639397_section45087272918"></a>**

<a name="zh-cn_topic_0000002210639397_table198852334716"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210639397_row1881723154712"><th class="cellrowborder" valign="top" width="19.71%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002210639397_p78862364716"><a name="zh-cn_topic_0000002210639397_p78862364716"></a><a name="zh-cn_topic_0000002210639397_p78862364716"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="80.28999999999999%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002210639397_p4888231472"><a name="zh-cn_topic_0000002210639397_p4888231472"></a><a name="zh-cn_topic_0000002210639397_p4888231472"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210639397_row488023104710"><td class="cellrowborder" valign="top" width="19.71%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210639397_p1994110212518"><a name="zh-cn_topic_0000002210639397_p1994110212518"></a><a name="zh-cn_topic_0000002210639397_p1994110212518"></a>&lt;n&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="80.28999999999999%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210639397_p12966103353019"><a name="zh-cn_topic_0000002210639397_p12966103353019"></a><a name="zh-cn_topic_0000002210639397_p12966103353019"></a>整型值，默认值为0。</p>
<p id="zh-cn_topic_0000002210639397_p896633333010"><a name="zh-cn_topic_0000002210639397_p896633333010"></a><a name="zh-cn_topic_0000002210639397_p896633333010"></a>0：关闭自动应答；</p>
<p id="zh-cn_topic_0000002210639397_p89661336304"><a name="zh-cn_topic_0000002210639397_p89661336304"></a><a name="zh-cn_topic_0000002210639397_p89661336304"></a>1～255：启动自动应答的时间，单位：s。</p>
<p id="zh-cn_topic_0000002210639397_p10192115715305"><a name="zh-cn_topic_0000002210639397_p10192115715305"></a><a name="zh-cn_topic_0000002210639397_p10192115715305"></a><strong id="zh-cn_topic_0000002210639397_b1984243153217"><a name="zh-cn_topic_0000002210639397_b1984243153217"></a><a name="zh-cn_topic_0000002210639397_b1984243153217"></a>注：不同的核心网规定的振铃超时时间不同，设置的自动应答时间过长可能会导致核心网“Altering timeout”异常，从而拒绝呼叫。</strong></p>
<p id="zh-cn_topic_0000002210639397_p1037412395720"><a name="zh-cn_topic_0000002210639397_p1037412395720"></a><a name="zh-cn_topic_0000002210639397_p1037412395720"></a><strong id="zh-cn_topic_0000002210639397_b192451758016"><a name="zh-cn_topic_0000002210639397_b192451758016"></a><a name="zh-cn_topic_0000002210639397_b192451758016"></a>IMS核心网网元通常配置被叫侧呼叫振铃超时定时器时长为60s，建议此命令配置时长不要超过该值。</strong></p>
</td>
</tr>
</tbody>
</table>

### 查询当前呼叫信息：+CLCC<a name="ZH-CN_TOPIC_0000002258467689"></a>

**命令描述<a name="zh-cn_topic_0000002175433044_section4340133117813"></a>**

列出当前所有的呼叫。

**命令语法<a name="zh-cn_topic_0000002175433044_section27467181898"></a>**

<a name="zh-cn_topic_0000002175433044_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175433044_row139817074814"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002175433044_p173981104481"><a name="zh-cn_topic_0000002175433044_p173981104481"></a><a name="zh-cn_topic_0000002175433044_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002175433044_p339915016481"><a name="zh-cn_topic_0000002175433044_p339915016481"></a><a name="zh-cn_topic_0000002175433044_p339915016481"></a>返回结果</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002175433044_p4399902485"><a name="zh-cn_topic_0000002175433044_p4399902485"></a><a name="zh-cn_topic_0000002175433044_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175433044_row23995014487"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002175433044_p19822026152819"><a name="zh-cn_topic_0000002175433044_p19822026152819"></a><a name="zh-cn_topic_0000002175433044_p19822026152819"></a>+CLCC</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002175433044_p8482132103215"><a name="zh-cn_topic_0000002175433044_p8482132103215"></a><a name="zh-cn_topic_0000002175433044_p8482132103215"></a>+CLCC: &lt;id1&gt;,&lt;dir&gt;,&lt;stat&gt;,&lt;mode&gt;,&lt;mpty&gt;[,</p>
<p id="zh-cn_topic_0000002175433044_p229154281913"><a name="zh-cn_topic_0000002175433044_p229154281913"></a><a name="zh-cn_topic_0000002175433044_p229154281913"></a>&lt;number&gt;,&lt;type&gt;[,&lt;alpha&gt;]]</p>
<p id="zh-cn_topic_0000002175433044_p61194313206"><a name="zh-cn_topic_0000002175433044_p61194313206"></a><a name="zh-cn_topic_0000002175433044_p61194313206"></a>&lt;id2&gt;,&lt;dir&gt;,&lt;stat&gt;,&lt;mode&gt;,&lt;mpty&gt;[,</p>
<p id="zh-cn_topic_0000002175433044_p17119431207"><a name="zh-cn_topic_0000002175433044_p17119431207"></a><a name="zh-cn_topic_0000002175433044_p17119431207"></a>&lt;number&gt;,&lt;type&gt;</p>
<p id="zh-cn_topic_0000002175433044_p793018915202"><a name="zh-cn_topic_0000002175433044_p793018915202"></a><a name="zh-cn_topic_0000002175433044_p793018915202"></a>......</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002175433044_p158381914205117"><a name="zh-cn_topic_0000002175433044_p158381914205117"></a><a name="zh-cn_topic_0000002175433044_p158381914205117"></a>AT+CLCC</p>
<p id="zh-cn_topic_0000002175433044_p5838414185113"><a name="zh-cn_topic_0000002175433044_p5838414185113"></a><a name="zh-cn_topic_0000002175433044_p5838414185113"></a>+CLCC: 1,0,0,0,0,"888",129</p>
<p id="zh-cn_topic_0000002175433044_p1781395461812"><a name="zh-cn_topic_0000002175433044_p1781395461812"></a><a name="zh-cn_topic_0000002175433044_p1781395461812"></a>OK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175433044_row165365198451"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002175433044_p279153652813"><a name="zh-cn_topic_0000002175433044_p279153652813"></a><a name="zh-cn_topic_0000002175433044_p279153652813"></a>+CLCC=?</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002175433044_p722612603918"><a name="zh-cn_topic_0000002175433044_p722612603918"></a><a name="zh-cn_topic_0000002175433044_p722612603918"></a>&lt;CR&gt;&lt;LF&gt;&lt;n&gt;&lt;CR&gt;&lt;LF&gt;</p>
<p id="zh-cn_topic_0000002175433044_p82261643918"><a name="zh-cn_topic_0000002175433044_p82261643918"></a><a name="zh-cn_topic_0000002175433044_p82261643918"></a>&lt;CR&gt;&lt;LF&gt;OK&lt;CR&gt;&lt;LF&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002175433044_p68761117145111"><a name="zh-cn_topic_0000002175433044_p68761117145111"></a><a name="zh-cn_topic_0000002175433044_p68761117145111"></a>AT+CLCC=?</p>
<p id="zh-cn_topic_0000002175433044_p950704121913"><a name="zh-cn_topic_0000002175433044_p950704121913"></a><a name="zh-cn_topic_0000002175433044_p950704121913"></a>OK</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002175433044_section45087272918"></a>**

<a name="zh-cn_topic_0000002175433044_table198852334716"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175433044_row1881723154712"><th class="cellrowborder" valign="top" width="25.15%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002175433044_p78862364716"><a name="zh-cn_topic_0000002175433044_p78862364716"></a><a name="zh-cn_topic_0000002175433044_p78862364716"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="74.85000000000001%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002175433044_p4888231472"><a name="zh-cn_topic_0000002175433044_p4888231472"></a><a name="zh-cn_topic_0000002175433044_p4888231472"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175433044_row188458243208"><td class="cellrowborder" valign="top" width="25.15%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433044_p1845124162016"><a name="zh-cn_topic_0000002175433044_p1845124162016"></a><a name="zh-cn_topic_0000002175433044_p1845124162016"></a>&lt;id<em id="zh-cn_topic_0000002175433044_i1575413263207"><a name="zh-cn_topic_0000002175433044_i1575413263207"></a><a name="zh-cn_topic_0000002175433044_i1575413263207"></a>x</em>&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="74.85000000000001%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433044_p1484582418205"><a name="zh-cn_topic_0000002175433044_p1484582418205"></a><a name="zh-cn_topic_0000002175433044_p1484582418205"></a>整型值，呼叫标识。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175433044_row488023104710"><td class="cellrowborder" valign="top" width="25.15%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433044_p1994110212518"><a name="zh-cn_topic_0000002175433044_p1994110212518"></a><a name="zh-cn_topic_0000002175433044_p1994110212518"></a>&lt;dir&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="74.85000000000001%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433044_p4565954182011"><a name="zh-cn_topic_0000002175433044_p4565954182011"></a><a name="zh-cn_topic_0000002175433044_p4565954182011"></a>整型值，呼叫方向。</p>
<p id="zh-cn_topic_0000002175433044_p1356555482011"><a name="zh-cn_topic_0000002175433044_p1356555482011"></a><a name="zh-cn_topic_0000002175433044_p1356555482011"></a>0：主叫；</p>
<p id="zh-cn_topic_0000002175433044_p12565145419200"><a name="zh-cn_topic_0000002175433044_p12565145419200"></a><a name="zh-cn_topic_0000002175433044_p12565145419200"></a>1：被叫。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175433044_row128101115212"><td class="cellrowborder" valign="top" width="25.15%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433044_p9810411142120"><a name="zh-cn_topic_0000002175433044_p9810411142120"></a><a name="zh-cn_topic_0000002175433044_p9810411142120"></a>&lt;state&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="74.85000000000001%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433044_p1318310314215"><a name="zh-cn_topic_0000002175433044_p1318310314215"></a><a name="zh-cn_topic_0000002175433044_p1318310314215"></a>整型值，呼叫状态。</p>
<p id="zh-cn_topic_0000002175433044_p151831313216"><a name="zh-cn_topic_0000002175433044_p151831313216"></a><a name="zh-cn_topic_0000002175433044_p151831313216"></a>0：激活状态；</p>
<p id="zh-cn_topic_0000002175433044_p8693615182715"><a name="zh-cn_topic_0000002175433044_p8693615182715"></a><a name="zh-cn_topic_0000002175433044_p8693615182715"></a>1：呼叫保持状态</p>
<p id="zh-cn_topic_0000002175433044_p1030855514219"><a name="zh-cn_topic_0000002175433044_p1030855514219"></a><a name="zh-cn_topic_0000002175433044_p1030855514219"></a>2：主叫，拨号状态；</p>
<p id="zh-cn_topic_0000002175433044_p1330855515218"><a name="zh-cn_topic_0000002175433044_p1330855515218"></a><a name="zh-cn_topic_0000002175433044_p1330855515218"></a>3：主叫，回铃音状态；</p>
<p id="zh-cn_topic_0000002175433044_p3308855152110"><a name="zh-cn_topic_0000002175433044_p3308855152110"></a><a name="zh-cn_topic_0000002175433044_p3308855152110"></a>4：被叫，来电状态；</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175433044_row17810811102112"><td class="cellrowborder" valign="top" width="25.15%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433044_p15810811162118"><a name="zh-cn_topic_0000002175433044_p15810811162118"></a><a name="zh-cn_topic_0000002175433044_p15810811162118"></a>&lt;mode&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="74.85000000000001%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433044_p106789229223"><a name="zh-cn_topic_0000002175433044_p106789229223"></a><a name="zh-cn_topic_0000002175433044_p106789229223"></a>整型值，呼叫模式。（当前仅支持语音呼叫模式。）</p>
<p id="zh-cn_topic_0000002175433044_p567816224225"><a name="zh-cn_topic_0000002175433044_p567816224225"></a><a name="zh-cn_topic_0000002175433044_p567816224225"></a>0：语音呼叫。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175433044_row87492782220"><td class="cellrowborder" valign="top" width="25.15%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433044_p774162716224"><a name="zh-cn_topic_0000002175433044_p774162716224"></a><a name="zh-cn_topic_0000002175433044_p774162716224"></a>&lt;mpty&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="74.85000000000001%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433044_p1194782717247"><a name="zh-cn_topic_0000002175433044_p1194782717247"></a><a name="zh-cn_topic_0000002175433044_p1194782717247"></a>整型值，多方通话状态。</p>
<p id="zh-cn_topic_0000002175433044_p1194722712412"><a name="zh-cn_topic_0000002175433044_p1194722712412"></a><a name="zh-cn_topic_0000002175433044_p1194722712412"></a>0：不在多方通话中。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175433044_row37419277227"><td class="cellrowborder" valign="top" width="25.15%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433044_p12743273223"><a name="zh-cn_topic_0000002175433044_p12743273223"></a><a name="zh-cn_topic_0000002175433044_p12743273223"></a>&lt;number&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="74.85000000000001%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433044_p107452719222"><a name="zh-cn_topic_0000002175433044_p107452719222"></a><a name="zh-cn_topic_0000002175433044_p107452719222"></a>字符串类型，电话号码。</p>
<p id="zh-cn_topic_0000002175433044_p1796443591217"><a name="zh-cn_topic_0000002175433044_p1796443591217"></a><a name="zh-cn_topic_0000002175433044_p1796443591217"></a>主叫侧显示被叫号码，被叫侧显示主叫号码。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175433044_row1622117612110"><td class="cellrowborder" valign="top" width="25.15%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433044_p1622215616211"><a name="zh-cn_topic_0000002175433044_p1622215616211"></a><a name="zh-cn_topic_0000002175433044_p1622215616211"></a>&lt;type&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="74.85000000000001%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433044_p71941312152416"><a name="zh-cn_topic_0000002175433044_p71941312152416"></a><a name="zh-cn_topic_0000002175433044_p71941312152416"></a>号码地址类型，整型值。</p>
<p id="zh-cn_topic_0000002175433044_p1919481215242"><a name="zh-cn_topic_0000002175433044_p1919481215242"></a><a name="zh-cn_topic_0000002175433044_p1919481215242"></a>129：普通号码；</p>
<p id="zh-cn_topic_0000002175433044_p819451262410"><a name="zh-cn_topic_0000002175433044_p819451262410"></a><a name="zh-cn_topic_0000002175433044_p819451262410"></a>145：国际号码（以“+”开头）。</p>
</td>
</tr>
</tbody>
</table>

### 查询IMS域注册状态：+CIREG<a name="ZH-CN_TOPIC_0000002258627573"></a>

**命令描述<a name="zh-cn_topic_0000002219438641_section4340133117813"></a>**

查询当前IMS域网络注册状态以及设置状态上报方式。

**命令语法<a name="zh-cn_topic_0000002219438641_section27467181898"></a>**

<a name="zh-cn_topic_0000002219438641_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002219438641_row139817074814"><th class="cellrowborder" valign="top" width="33.333333333333336%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002219438641_p173981104481"><a name="zh-cn_topic_0000002219438641_p173981104481"></a><a name="zh-cn_topic_0000002219438641_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="33.29332933293329%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002219438641_p339915016481"><a name="zh-cn_topic_0000002219438641_p339915016481"></a><a name="zh-cn_topic_0000002219438641_p339915016481"></a>返回结果</p>
</th>
<th class="cellrowborder" valign="top" width="33.373337333733375%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002219438641_p4399902485"><a name="zh-cn_topic_0000002219438641_p4399902485"></a><a name="zh-cn_topic_0000002219438641_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002219438641_row23995014487"><td class="cellrowborder" valign="top" width="33.333333333333336%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002219438641_p7480132115477"><a name="zh-cn_topic_0000002219438641_p7480132115477"></a><a name="zh-cn_topic_0000002219438641_p7480132115477"></a>+CIREG=[&lt;n&gt;]</p>
</td>
<td class="cellrowborder" valign="top" width="33.29332933293329%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002219438641_p851635232"><a name="zh-cn_topic_0000002219438641_p851635232"></a><a name="zh-cn_topic_0000002219438641_p851635232"></a>OK</p>
<p id="zh-cn_topic_0000002219438641_p1544139834"><a name="zh-cn_topic_0000002219438641_p1544139834"></a><a name="zh-cn_topic_0000002219438641_p1544139834"></a>错误情况：</p>
<p id="zh-cn_topic_0000002219438641_p1779mcpsimp"><a name="zh-cn_topic_0000002219438641_p1779mcpsimp"></a><a name="zh-cn_topic_0000002219438641_p1779mcpsimp"></a>+CME ERROR: &lt;err&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="33.373337333733375%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002219438641_p2634186145019"><a name="zh-cn_topic_0000002219438641_p2634186145019"></a><a name="zh-cn_topic_0000002219438641_p2634186145019"></a>AT+CIREG=1</p>
<p id="zh-cn_topic_0000002219438641_p1782mcpsimp"><a name="zh-cn_topic_0000002219438641_p1782mcpsimp"></a><a name="zh-cn_topic_0000002219438641_p1782mcpsimp"></a>OK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002219438641_row165365198451"><td class="cellrowborder" valign="top" width="33.333333333333336%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002219438641_p3011mcpsimp"><a name="zh-cn_topic_0000002219438641_p3011mcpsimp"></a><a name="zh-cn_topic_0000002219438641_p3011mcpsimp"></a>+CIREG?</p>
</td>
<td class="cellrowborder" valign="top" width="33.29332933293329%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002219438641_p7374515135815"><a name="zh-cn_topic_0000002219438641_p7374515135815"></a><a name="zh-cn_topic_0000002219438641_p7374515135815"></a>+CIREG: &lt;n&gt;,&lt;reg_info&gt;[,&lt;ext_info&gt;]</p>
<p id="zh-cn_topic_0000002219438641_p1789mcpsimp"><a name="zh-cn_topic_0000002219438641_p1789mcpsimp"></a><a name="zh-cn_topic_0000002219438641_p1789mcpsimp"></a>错误情况：</p>
<p id="zh-cn_topic_0000002219438641_p1790mcpsimp"><a name="zh-cn_topic_0000002219438641_p1790mcpsimp"></a><a name="zh-cn_topic_0000002219438641_p1790mcpsimp"></a>+CME ERROR: &lt;err&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="33.373337333733375%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002219438641_p1793mcpsimp"><a name="zh-cn_topic_0000002219438641_p1793mcpsimp"></a><a name="zh-cn_topic_0000002219438641_p1793mcpsimp"></a>AT+CIREG？</p>
<p id="zh-cn_topic_0000002219438641_p14445163813587"><a name="zh-cn_topic_0000002219438641_p14445163813587"></a><a name="zh-cn_topic_0000002219438641_p14445163813587"></a>+CIREG:0,1,5（IMS已注册且支持语音和短信）</p>
<p id="zh-cn_topic_0000002219438641_p131732055115914"><a name="zh-cn_topic_0000002219438641_p131732055115914"></a><a name="zh-cn_topic_0000002219438641_p131732055115914"></a>OK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002219438641_row1531755165811"><td class="cellrowborder" valign="top" width="33.333333333333336%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002219438641_p13178515588"><a name="zh-cn_topic_0000002219438641_p13178515588"></a><a name="zh-cn_topic_0000002219438641_p13178515588"></a>+CIREG=？</p>
</td>
<td class="cellrowborder" valign="top" width="33.29332933293329%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002219438641_p1799mcpsimp"><a name="zh-cn_topic_0000002219438641_p1799mcpsimp"></a><a name="zh-cn_topic_0000002219438641_p1799mcpsimp"></a>+CIREG: (list of supported &lt;n&gt;s)</p>
<p id="zh-cn_topic_0000002219438641_p13469333104211"><a name="zh-cn_topic_0000002219438641_p13469333104211"></a><a name="zh-cn_topic_0000002219438641_p13469333104211"></a>OK</p>
</td>
<td class="cellrowborder" valign="top" width="33.373337333733375%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002219438641_p1801mcpsimp"><a name="zh-cn_topic_0000002219438641_p1801mcpsimp"></a><a name="zh-cn_topic_0000002219438641_p1801mcpsimp"></a>AT+CIREG=?</p>
<p id="zh-cn_topic_0000002219438641_p611034125011"><a name="zh-cn_topic_0000002219438641_p611034125011"></a><a name="zh-cn_topic_0000002219438641_p611034125011"></a>+CIREG: (0-2)</p>
<p id="zh-cn_topic_0000002219438641_p1802mcpsimp"><a name="zh-cn_topic_0000002219438641_p1802mcpsimp"></a><a name="zh-cn_topic_0000002219438641_p1802mcpsimp"></a>OK</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002219438641_section45087272918"></a>**

<a name="zh-cn_topic_0000002219438641_table198852334716"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002219438641_row1881723154712"><th class="cellrowborder" valign="top" width="25.509999999999998%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002219438641_p78862364716"><a name="zh-cn_topic_0000002219438641_p78862364716"></a><a name="zh-cn_topic_0000002219438641_p78862364716"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="74.49%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002219438641_p4888231472"><a name="zh-cn_topic_0000002219438641_p4888231472"></a><a name="zh-cn_topic_0000002219438641_p4888231472"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002219438641_row1716315410495"><td class="cellrowborder" valign="top" width="25.509999999999998%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002219438641_p15163848497"><a name="zh-cn_topic_0000002219438641_p15163848497"></a><a name="zh-cn_topic_0000002219438641_p15163848497"></a>&lt;n&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="74.49%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002219438641_p46219161495"><a name="zh-cn_topic_0000002219438641_p46219161495"></a><a name="zh-cn_topic_0000002219438641_p46219161495"></a>整型值，主动上报方式，默认值为0。</p>
<p id="zh-cn_topic_0000002219438641_p96219162498"><a name="zh-cn_topic_0000002219438641_p96219162498"></a><a name="zh-cn_topic_0000002219438641_p96219162498"></a>0：禁止+CIREGU的主动上报；</p>
<p id="zh-cn_topic_0000002219438641_p7627161493"><a name="zh-cn_topic_0000002219438641_p7627161493"></a><a name="zh-cn_topic_0000002219438641_p7627161493"></a>1：使能+CIREGU: &lt;reg_info&gt;的主动上报；</p>
<p id="zh-cn_topic_0000002219438641_p962181618494"><a name="zh-cn_topic_0000002219438641_p962181618494"></a><a name="zh-cn_topic_0000002219438641_p962181618494"></a>2：使能+CIREGU: &lt;reg_info&gt;[,&lt;ext_info&gt;]的主动上报。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002219438641_row488023104710"><td class="cellrowborder" valign="top" width="25.509999999999998%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002219438641_p129501746204819"><a name="zh-cn_topic_0000002219438641_p129501746204819"></a><a name="zh-cn_topic_0000002219438641_p129501746204819"></a>&lt;reg_info&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="74.49%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002219438641_p6507255124812"><a name="zh-cn_topic_0000002219438641_p6507255124812"></a><a name="zh-cn_topic_0000002219438641_p6507255124812"></a>整型值，注册状态。</p>
<p id="zh-cn_topic_0000002219438641_p1250710557488"><a name="zh-cn_topic_0000002219438641_p1250710557488"></a><a name="zh-cn_topic_0000002219438641_p1250710557488"></a>0：未注册IMS网络；</p>
<p id="zh-cn_topic_0000002219438641_p175071155164814"><a name="zh-cn_topic_0000002219438641_p175071155164814"></a><a name="zh-cn_topic_0000002219438641_p175071155164814"></a>1：已注册IMS网络。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002219438641_row8822112574915"><td class="cellrowborder" valign="top" width="25.509999999999998%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002219438641_p5822152584914"><a name="zh-cn_topic_0000002219438641_p5822152584914"></a><a name="zh-cn_topic_0000002219438641_p5822152584914"></a>&lt;ext_info&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="74.49%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002219438641_p172074013495"><a name="zh-cn_topic_0000002219438641_p172074013495"></a><a name="zh-cn_topic_0000002219438641_p172074013495"></a>整型值，IMS域能力值，取值范围[1,0xFFFFFFFF]，每个bit位表示不同的能力。</p>
<p id="zh-cn_topic_0000002219438641_p137201740164910"><a name="zh-cn_topic_0000002219438641_p137201740164910"></a><a name="zh-cn_topic_0000002219438641_p137201740164910"></a>1：RTP-based transfer of voice；</p>
<p id="zh-cn_topic_0000002219438641_p177201340104913"><a name="zh-cn_topic_0000002219438641_p177201340104913"></a><a name="zh-cn_topic_0000002219438641_p177201340104913"></a>2：RTP-based transfer of text；</p>
<p id="zh-cn_topic_0000002219438641_p197207409492"><a name="zh-cn_topic_0000002219438641_p197207409492"></a><a name="zh-cn_topic_0000002219438641_p197207409492"></a>4：SMS using IMS functionality；</p>
<p id="zh-cn_topic_0000002219438641_p972084013496"><a name="zh-cn_topic_0000002219438641_p972084013496"></a><a name="zh-cn_topic_0000002219438641_p972084013496"></a>8：RTP-based transfer of video。</p>
<p id="zh-cn_topic_0000002219438641_p13720144016498"><a name="zh-cn_topic_0000002219438641_p13720144016498"></a><a name="zh-cn_topic_0000002219438641_p13720144016498"></a>例如，等于5的时候，表示支持RTP-based transfer of voice和SMS using IMS functionality。</p>
<p id="zh-cn_topic_0000002219438641_p1168118504615"><a name="zh-cn_topic_0000002219438641_p1168118504615"></a><a name="zh-cn_topic_0000002219438641_p1168118504615"></a><strong id="zh-cn_topic_0000002219438641_b1431411813287"><a name="zh-cn_topic_0000002219438641_b1431411813287"></a><a name="zh-cn_topic_0000002219438641_b1431411813287"></a>注：IMS已注册才显示该值</strong></p>
</td>
</tr>
</tbody>
</table>

### 主动上报IMS域注册状态：+CIREGU<a name="ZH-CN_TOPIC_0000002223508076"></a>

**命令描述<a name="zh-cn_topic_0000002236189641_section4340133117813"></a>**

主动上报当前IMS域网络注册状态，能否主动上报受+CIREG命令控制。

**命令语法<a name="zh-cn_topic_0000002236189641_section27467181898"></a>**

<a name="zh-cn_topic_0000002236189641_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002236189641_row139817074814"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002236189641_p173981104481"><a name="zh-cn_topic_0000002236189641_p173981104481"></a><a name="zh-cn_topic_0000002236189641_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002236189641_p339915016481"><a name="zh-cn_topic_0000002236189641_p339915016481"></a><a name="zh-cn_topic_0000002236189641_p339915016481"></a>返回结果</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002236189641_p4399902485"><a name="zh-cn_topic_0000002236189641_p4399902485"></a><a name="zh-cn_topic_0000002236189641_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002236189641_row23995014487"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002236189641_p7480132115477"><a name="zh-cn_topic_0000002236189641_p7480132115477"></a><a name="zh-cn_topic_0000002236189641_p7480132115477"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002236189641_p851635232"><a name="zh-cn_topic_0000002236189641_p851635232"></a><a name="zh-cn_topic_0000002236189641_p851635232"></a>+CIREGU: &lt;reg_info&gt;[,&lt;ext_info&gt;]</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002236189641_p2634186145019"><a name="zh-cn_topic_0000002236189641_p2634186145019"></a><a name="zh-cn_topic_0000002236189641_p2634186145019"></a>+CIREGU：1,5</p>
<p id="zh-cn_topic_0000002236189641_p1782mcpsimp"><a name="zh-cn_topic_0000002236189641_p1782mcpsimp"></a><a name="zh-cn_topic_0000002236189641_p1782mcpsimp"></a>OK</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002236189641_section45087272918"></a>**

<a name="zh-cn_topic_0000002236189641_table198852334716"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002236189641_row1881723154712"><th class="cellrowborder" valign="top" width="29.86%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002236189641_p78862364716"><a name="zh-cn_topic_0000002236189641_p78862364716"></a><a name="zh-cn_topic_0000002236189641_p78862364716"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="70.14%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002236189641_p4888231472"><a name="zh-cn_topic_0000002236189641_p4888231472"></a><a name="zh-cn_topic_0000002236189641_p4888231472"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002236189641_row488023104710"><td class="cellrowborder" valign="top" width="29.86%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002236189641_p129501746204819"><a name="zh-cn_topic_0000002236189641_p129501746204819"></a><a name="zh-cn_topic_0000002236189641_p129501746204819"></a>&lt;reg_info&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="70.14%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002236189641_p6507255124812"><a name="zh-cn_topic_0000002236189641_p6507255124812"></a><a name="zh-cn_topic_0000002236189641_p6507255124812"></a>整型值，注册状态。</p>
<p id="zh-cn_topic_0000002236189641_p1250710557488"><a name="zh-cn_topic_0000002236189641_p1250710557488"></a><a name="zh-cn_topic_0000002236189641_p1250710557488"></a>0：未注册IMS网络；</p>
<p id="zh-cn_topic_0000002236189641_p175071155164814"><a name="zh-cn_topic_0000002236189641_p175071155164814"></a><a name="zh-cn_topic_0000002236189641_p175071155164814"></a>1：注册了IMS网络。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002236189641_row8822112574915"><td class="cellrowborder" valign="top" width="29.86%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002236189641_p5822152584914"><a name="zh-cn_topic_0000002236189641_p5822152584914"></a><a name="zh-cn_topic_0000002236189641_p5822152584914"></a>&lt;ext_info&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="70.14%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002236189641_p172074013495"><a name="zh-cn_topic_0000002236189641_p172074013495"></a><a name="zh-cn_topic_0000002236189641_p172074013495"></a>整型值，IMS域能力值，取值范围[1,0xFFFFFFFF]，每个bit位表示不同的能力。</p>
<p id="zh-cn_topic_0000002236189641_p137201740164910"><a name="zh-cn_topic_0000002236189641_p137201740164910"></a><a name="zh-cn_topic_0000002236189641_p137201740164910"></a>1:  RTP-based transfer of voice；</p>
<p id="zh-cn_topic_0000002236189641_p177201340104913"><a name="zh-cn_topic_0000002236189641_p177201340104913"></a><a name="zh-cn_topic_0000002236189641_p177201340104913"></a>2:  RTP-based transfer of text；</p>
<p id="zh-cn_topic_0000002236189641_p197207409492"><a name="zh-cn_topic_0000002236189641_p197207409492"></a><a name="zh-cn_topic_0000002236189641_p197207409492"></a>4:  SMS using IMS functionality；</p>
<p id="zh-cn_topic_0000002236189641_p972084013496"><a name="zh-cn_topic_0000002236189641_p972084013496"></a><a name="zh-cn_topic_0000002236189641_p972084013496"></a>8：RTP-based transfer of video。</p>
<p id="zh-cn_topic_0000002236189641_p13720144016498"><a name="zh-cn_topic_0000002236189641_p13720144016498"></a><a name="zh-cn_topic_0000002236189641_p13720144016498"></a>例如，等于5时，表示支持RTP-based transfer of voice和SMS using IMS functionality。</p>
</td>
</tr>
</tbody>
</table>

### 查询IMS域的虚拟号码：+VOLTEIMPU<a name="ZH-CN_TOPIC_0000002223667916"></a>

**命令描述<a name="zh-cn_topic_0000002175273332_section4340133117813"></a>**

该命令用于查询IMS域的IMPU信息以及虚拟号码。

**命令语法<a name="zh-cn_topic_0000002175273332_section27467181898"></a>**

<a name="zh-cn_topic_0000002175273332_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175273332_row139817074814"><th class="cellrowborder" valign="top" width="28.46%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002175273332_p173981104481"><a name="zh-cn_topic_0000002175273332_p173981104481"></a><a name="zh-cn_topic_0000002175273332_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="29.01%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002175273332_p1926993319395"><a name="zh-cn_topic_0000002175273332_p1926993319395"></a><a name="zh-cn_topic_0000002175273332_p1926993319395"></a>返回结果</p>
</th>
<th class="cellrowborder" valign="top" width="42.53%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002175273332_p4399902485"><a name="zh-cn_topic_0000002175273332_p4399902485"></a><a name="zh-cn_topic_0000002175273332_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175273332_row23995014487"><td class="cellrowborder" valign="top" width="28.46%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002175273332_p1427mcpsimp"><a name="zh-cn_topic_0000002175273332_p1427mcpsimp"></a><a name="zh-cn_topic_0000002175273332_p1427mcpsimp"></a>+VOLTEIMPU</p>
</td>
<td class="cellrowborder" valign="top" width="29.01%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002175273332_p066314257405"><a name="zh-cn_topic_0000002175273332_p066314257405"></a><a name="zh-cn_topic_0000002175273332_p066314257405"></a>&lt;CR&gt;&lt;LF&gt;OK&lt;CR&gt;&lt;LF&gt;</p>
<p id="zh-cn_topic_0000002175273332_p96631425164015"><a name="zh-cn_topic_0000002175273332_p96631425164015"></a><a name="zh-cn_topic_0000002175273332_p96631425164015"></a>&lt;CR&gt;&lt;LF&gt;+VOLTEIMPU: &lt; IMPU&gt;[,&lt;Virtual_Number  &gt;]&lt;CR&gt;&lt;LF&gt;</p>
<p id="zh-cn_topic_0000002175273332_p5663152512408"><a name="zh-cn_topic_0000002175273332_p5663152512408"></a><a name="zh-cn_topic_0000002175273332_p5663152512408"></a>错误情况：</p>
<p id="zh-cn_topic_0000002175273332_p7664525184015"><a name="zh-cn_topic_0000002175273332_p7664525184015"></a><a name="zh-cn_topic_0000002175273332_p7664525184015"></a>&lt;CR&gt;&lt;LF&gt;+CME ERROR: &lt;err&gt;&lt;CR&gt;&lt;LF&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="42.53%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002175273332_p898161792716"><a name="zh-cn_topic_0000002175273332_p898161792716"></a><a name="zh-cn_topic_0000002175273332_p898161792716"></a>AT+VOLTEIMPU</p>
<p id="zh-cn_topic_0000002175273332_p11141115834020"><a name="zh-cn_topic_0000002175273332_p11141115834020"></a><a name="zh-cn_topic_0000002175273332_p11141115834020"></a>+VOLTEIMPU: abcd,123456</p>
<p id="zh-cn_topic_0000002175273332_p20422155184018"><a name="zh-cn_topic_0000002175273332_p20422155184018"></a><a name="zh-cn_topic_0000002175273332_p20422155184018"></a>OK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175273332_row45571214390"><td class="cellrowborder" valign="top" width="28.46%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002175273332_p65577117393"><a name="zh-cn_topic_0000002175273332_p65577117393"></a><a name="zh-cn_topic_0000002175273332_p65577117393"></a>+VOLTEIMPU?</p>
</td>
<td class="cellrowborder" valign="top" width="29.01%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002175273332_p1269153317397"><a name="zh-cn_topic_0000002175273332_p1269153317397"></a><a name="zh-cn_topic_0000002175273332_p1269153317397"></a>OK</p>
</td>
<td class="cellrowborder" valign="top" width="42.53%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002175273332_p155578143916"><a name="zh-cn_topic_0000002175273332_p155578143916"></a><a name="zh-cn_topic_0000002175273332_p155578143916"></a>AT+VOLTEIMPU?</p>
<p id="zh-cn_topic_0000002175273332_p1741473754013"><a name="zh-cn_topic_0000002175273332_p1741473754013"></a><a name="zh-cn_topic_0000002175273332_p1741473754013"></a>OK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175273332_row9584442124116"><td class="cellrowborder" valign="top" width="28.46%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002175273332_p1491114934112"><a name="zh-cn_topic_0000002175273332_p1491114934112"></a><a name="zh-cn_topic_0000002175273332_p1491114934112"></a>+VOLTEIMPU=?</p>
</td>
<td class="cellrowborder" valign="top" width="29.01%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002175273332_p14911949134117"><a name="zh-cn_topic_0000002175273332_p14911949134117"></a><a name="zh-cn_topic_0000002175273332_p14911949134117"></a>OK</p>
</td>
<td class="cellrowborder" valign="top" width="42.53%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002175273332_p10912493416"><a name="zh-cn_topic_0000002175273332_p10912493416"></a><a name="zh-cn_topic_0000002175273332_p10912493416"></a>AT+VOLTEIMPU=?</p>
<p id="zh-cn_topic_0000002175273332_p139111492419"><a name="zh-cn_topic_0000002175273332_p139111492419"></a><a name="zh-cn_topic_0000002175273332_p139111492419"></a>OK</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002175273332_section1852233104111"></a>**

<a name="zh-cn_topic_0000002175273332_table66122098220"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175273332_row166129916216"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002175273332_p5612109128"><a name="zh-cn_topic_0000002175273332_p5612109128"></a><a name="zh-cn_topic_0000002175273332_p5612109128"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002175273332_p146120912216"><a name="zh-cn_topic_0000002175273332_p146120912216"></a><a name="zh-cn_topic_0000002175273332_p146120912216"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175273332_row196121191024"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175273332_p649965915273"><a name="zh-cn_topic_0000002175273332_p649965915273"></a><a name="zh-cn_topic_0000002175273332_p649965915273"></a>&lt;IMPU&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175273332_p1475913367273"><a name="zh-cn_topic_0000002175273332_p1475913367273"></a><a name="zh-cn_topic_0000002175273332_p1475913367273"></a>字符串类型，IMPU信息，取值范围0～128Byte。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175273332_row15612139427"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175273332_p277185232713"><a name="zh-cn_topic_0000002175273332_p277185232713"></a><a name="zh-cn_topic_0000002175273332_p277185232713"></a>&lt;Virtual_Number&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175273332_p1160211431278"><a name="zh-cn_topic_0000002175273332_p1160211431278"></a><a name="zh-cn_topic_0000002175273332_p1160211431278"></a>字符串类型，网络分配的虚拟电话号码，最大长度为127Byte。</p>
</td>
</tr>
</tbody>
</table>

### IMS服务状态更新指示：+IMSSRVSTATUS<a name="ZH-CN_TOPIC_0000002258467693"></a>

**命令描述<a name="zh-cn_topic_0000002210793765_section4340133117813"></a>**

IMS服务状态或者注册状态发生变化的主动上报。

**命令语法<a name="zh-cn_topic_0000002210793765_section27467181898"></a>**

<a name="zh-cn_topic_0000002210793765_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210793765_row139817074814"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002210793765_p173981104481"><a name="zh-cn_topic_0000002210793765_p173981104481"></a><a name="zh-cn_topic_0000002210793765_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002210793765_p4399902485"><a name="zh-cn_topic_0000002210793765_p4399902485"></a><a name="zh-cn_topic_0000002210793765_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210793765_row23995014487"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210793765_p1427mcpsimp"><a name="zh-cn_topic_0000002210793765_p1427mcpsimp"></a><a name="zh-cn_topic_0000002210793765_p1427mcpsimp"></a>+IMSSRVSTATUS: &lt;sms_srv_status&gt;,&lt;voip_srv_status&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210793765_p1437mcpsimp"><a name="zh-cn_topic_0000002210793765_p1437mcpsimp"></a><a name="zh-cn_topic_0000002210793765_p1437mcpsimp"></a>+IMSSRVSTATUS: 2,0</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002210793765_section45087272918"></a>**

<a name="zh-cn_topic_0000002210793765_table198852334716"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210793765_row1881723154712"><th class="cellrowborder" valign="top" width="25%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002210793765_p78862364716"><a name="zh-cn_topic_0000002210793765_p78862364716"></a><a name="zh-cn_topic_0000002210793765_p78862364716"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="75%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002210793765_p4888231472"><a name="zh-cn_topic_0000002210793765_p4888231472"></a><a name="zh-cn_topic_0000002210793765_p4888231472"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210793765_row1716315410495"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210793765_p15163848497"><a name="zh-cn_topic_0000002210793765_p15163848497"></a><a name="zh-cn_topic_0000002210793765_p15163848497"></a>&lt;sms_srv_status&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="75%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210793765_p576712585136"><a name="zh-cn_topic_0000002210793765_p576712585136"></a><a name="zh-cn_topic_0000002210793765_p576712585136"></a>整型值，IMS短信的服务状态：</p>
<p id="zh-cn_topic_0000002210793765_p18767658171310"><a name="zh-cn_topic_0000002210793765_p18767658171310"></a><a name="zh-cn_topic_0000002210793765_p18767658171310"></a>0：IMS SMS service is not available；</p>
<p id="zh-cn_topic_0000002210793765_p117671587135"><a name="zh-cn_topic_0000002210793765_p117671587135"></a><a name="zh-cn_topic_0000002210793765_p117671587135"></a>1：IMS SMS is in limited service；</p>
<p id="zh-cn_topic_0000002210793765_p476765851316"><a name="zh-cn_topic_0000002210793765_p476765851316"></a><a name="zh-cn_topic_0000002210793765_p476765851316"></a>2：IMS SMS is in full service。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210793765_row1612611592514"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210793765_p121261759650"><a name="zh-cn_topic_0000002210793765_p121261759650"></a><a name="zh-cn_topic_0000002210793765_p121261759650"></a>&lt;voip_srv_status&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="75%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210793765_p6341172981419"><a name="zh-cn_topic_0000002210793765_p6341172981419"></a><a name="zh-cn_topic_0000002210793765_p6341172981419"></a>整型值，IMS VoIP的服务状态：</p>
<p id="zh-cn_topic_0000002210793765_p13229132814107"><a name="zh-cn_topic_0000002210793765_p13229132814107"></a><a name="zh-cn_topic_0000002210793765_p13229132814107"></a>0：IMS VoIP service is not available；</p>
<p id="zh-cn_topic_0000002210793765_p722911289105"><a name="zh-cn_topic_0000002210793765_p722911289105"></a><a name="zh-cn_topic_0000002210793765_p722911289105"></a>1：IMS VoIP is in limited service；</p>
<p id="zh-cn_topic_0000002210793765_p62291128191012"><a name="zh-cn_topic_0000002210793765_p62291128191012"></a><a name="zh-cn_topic_0000002210793765_p62291128191012"></a>2：IMS VoIP is in full service。</p>
</td>
</tr>
</tbody>
</table>

### 来电指示：IRING<a name="ZH-CN_TOPIC_0000002258627577"></a>

**命令描述<a name="zh-cn_topic_0000002210639401_section4340133117813"></a>**

IMS域来电指示。

**命令语法<a name="zh-cn_topic_0000002210639401_section27467181898"></a>**

<a name="zh-cn_topic_0000002210639401_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210639401_row139817074814"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002210639401_p173981104481"><a name="zh-cn_topic_0000002210639401_p173981104481"></a><a name="zh-cn_topic_0000002210639401_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002210639401_p4399902485"><a name="zh-cn_topic_0000002210639401_p4399902485"></a><a name="zh-cn_topic_0000002210639401_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210639401_row23995014487"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210639401_p1427mcpsimp"><a name="zh-cn_topic_0000002210639401_p1427mcpsimp"></a><a name="zh-cn_topic_0000002210639401_p1427mcpsimp"></a>IRING</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210639401_p127510442462"><a name="zh-cn_topic_0000002210639401_p127510442462"></a><a name="zh-cn_topic_0000002210639401_p127510442462"></a>有IMS域来电</p>
<p id="zh-cn_topic_0000002210639401_p1437mcpsimp"><a name="zh-cn_topic_0000002210639401_p1437mcpsimp"></a><a name="zh-cn_topic_0000002210639401_p1437mcpsimp"></a>IRING。</p>
</td>
</tr>
</tbody>
</table>

### 发起语音呼叫指示：+ORIG<a name="ZH-CN_TOPIC_0000002223508080"></a>

**命令描述<a name="zh-cn_topic_0000002175433048_section4340133117813"></a>**

主动上报命令，在用户发起电路域呼叫时，通知用户UE正在向网络发起呼叫。

**命令语法<a name="zh-cn_topic_0000002175433048_section27467181898"></a>**

<a name="zh-cn_topic_0000002175433048_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175433048_row139817074814"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002175433048_p173981104481"><a name="zh-cn_topic_0000002175433048_p173981104481"></a><a name="zh-cn_topic_0000002175433048_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002175433048_p4399902485"><a name="zh-cn_topic_0000002175433048_p4399902485"></a><a name="zh-cn_topic_0000002175433048_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175433048_row23995014487"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433048_p1427mcpsimp"><a name="zh-cn_topic_0000002175433048_p1427mcpsimp"></a><a name="zh-cn_topic_0000002175433048_p1427mcpsimp"></a>+ORIG: &lt;call_id&gt;,&lt;call_type&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433048_p113161340114611"><a name="zh-cn_topic_0000002175433048_p113161340114611"></a><a name="zh-cn_topic_0000002175433048_p113161340114611"></a>UE向网络发起一个语音呼叫</p>
<p id="zh-cn_topic_0000002175433048_p1437mcpsimp"><a name="zh-cn_topic_0000002175433048_p1437mcpsimp"></a><a name="zh-cn_topic_0000002175433048_p1437mcpsimp"></a>+ORIG: 1,0。</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002175433048_section45087272918"></a>**

<a name="zh-cn_topic_0000002175433048_table95301320849"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175433048_row453114201943"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002175433048_p155311201943"><a name="zh-cn_topic_0000002175433048_p155311201943"></a><a name="zh-cn_topic_0000002175433048_p155311201943"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002175433048_p1353142012412"><a name="zh-cn_topic_0000002175433048_p1353142012412"></a><a name="zh-cn_topic_0000002175433048_p1353142012412"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175433048_row353102010411"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433048_p15311201744"><a name="zh-cn_topic_0000002175433048_p15311201744"></a><a name="zh-cn_topic_0000002175433048_p15311201744"></a>call_id</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433048_p175317201240"><a name="zh-cn_topic_0000002175433048_p175317201240"></a><a name="zh-cn_topic_0000002175433048_p175317201240"></a>整型值，呼叫ID。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175433048_row8402823191413"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433048_p1040242315146"><a name="zh-cn_topic_0000002175433048_p1040242315146"></a><a name="zh-cn_topic_0000002175433048_p1040242315146"></a>call_type</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433048_p89684041418"><a name="zh-cn_topic_0000002175433048_p89684041418"></a><a name="zh-cn_topic_0000002175433048_p89684041418"></a>整型值，呼叫类型。</p>
<p id="zh-cn_topic_0000002175433048_p12961840171411"><a name="zh-cn_topic_0000002175433048_p12961840171411"></a><a name="zh-cn_topic_0000002175433048_p12961840171411"></a>0：语音呼叫；</p>
<p id="zh-cn_topic_0000002175433048_p17961140151418"><a name="zh-cn_topic_0000002175433048_p17961140151418"></a><a name="zh-cn_topic_0000002175433048_p17961140151418"></a>9：紧急呼叫。</p>
</td>
</tr>
</tbody>
</table>

### UE收到网络指示：+CONF<a name="ZH-CN_TOPIC_0000002223667920"></a>

**命令描述<a name="zh-cn_topic_0000002175273336_section4340133117813"></a>**

主动上报命令，通知用户发起呼叫后，UE收到了网络接通指示。

**命令语法<a name="zh-cn_topic_0000002175273336_section27467181898"></a>**

<a name="zh-cn_topic_0000002175273336_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175273336_row139817074814"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002175273336_p173981104481"><a name="zh-cn_topic_0000002175273336_p173981104481"></a><a name="zh-cn_topic_0000002175273336_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002175273336_p4399902485"><a name="zh-cn_topic_0000002175273336_p4399902485"></a><a name="zh-cn_topic_0000002175273336_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175273336_row23995014487"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175273336_p1427mcpsimp"><a name="zh-cn_topic_0000002175273336_p1427mcpsimp"></a><a name="zh-cn_topic_0000002175273336_p1427mcpsimp"></a>+CONF: &lt;call_id&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175273336_p108816372468"><a name="zh-cn_topic_0000002175273336_p108816372468"></a><a name="zh-cn_topic_0000002175273336_p108816372468"></a>UE收到网络接通指示</p>
<p id="zh-cn_topic_0000002175273336_p1437mcpsimp"><a name="zh-cn_topic_0000002175273336_p1437mcpsimp"></a><a name="zh-cn_topic_0000002175273336_p1437mcpsimp"></a>+CONF: 1。</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002175273336_section45087272918"></a>**

<a name="zh-cn_topic_0000002175273336_table95301320849"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175273336_row453114201943"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002175273336_p155311201943"><a name="zh-cn_topic_0000002175273336_p155311201943"></a><a name="zh-cn_topic_0000002175273336_p155311201943"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002175273336_p1353142012412"><a name="zh-cn_topic_0000002175273336_p1353142012412"></a><a name="zh-cn_topic_0000002175273336_p1353142012412"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175273336_row353102010411"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175273336_p15311201744"><a name="zh-cn_topic_0000002175273336_p15311201744"></a><a name="zh-cn_topic_0000002175273336_p15311201744"></a>call_id</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175273336_p175317201240"><a name="zh-cn_topic_0000002175273336_p175317201240"></a><a name="zh-cn_topic_0000002175273336_p175317201240"></a>整型值，呼叫ID。</p>
</td>
</tr>
</tbody>
</table>

### 呼叫接通指示：+CONN<a name="ZH-CN_TOPIC_0000002258467697"></a>

**命令描述<a name="zh-cn_topic_0000002210793769_section4340133117813"></a>**

主动上报命令，当呼叫接通后，UE通知用户呼叫已经接通。

**命令语法<a name="zh-cn_topic_0000002210793769_section27467181898"></a>**

<a name="zh-cn_topic_0000002210793769_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210793769_row139817074814"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002210793769_p173981104481"><a name="zh-cn_topic_0000002210793769_p173981104481"></a><a name="zh-cn_topic_0000002210793769_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002210793769_p4399902485"><a name="zh-cn_topic_0000002210793769_p4399902485"></a><a name="zh-cn_topic_0000002210793769_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210793769_row23995014487"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210793769_p1427mcpsimp"><a name="zh-cn_topic_0000002210793769_p1427mcpsimp"></a><a name="zh-cn_topic_0000002210793769_p1427mcpsimp"></a>+CONN:&lt;call_id&gt;, &lt;call_type&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210793769_p666333034613"><a name="zh-cn_topic_0000002210793769_p666333034613"></a><a name="zh-cn_topic_0000002210793769_p666333034613"></a>语音呼叫已经接通</p>
<p id="zh-cn_topic_0000002210793769_p1437mcpsimp"><a name="zh-cn_topic_0000002210793769_p1437mcpsimp"></a><a name="zh-cn_topic_0000002210793769_p1437mcpsimp"></a>+CONN:1,0。</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002210793769_section45087272918"></a>**

<a name="zh-cn_topic_0000002210793769_table95301320849"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210793769_row453114201943"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002210793769_p155311201943"><a name="zh-cn_topic_0000002210793769_p155311201943"></a><a name="zh-cn_topic_0000002210793769_p155311201943"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002210793769_p1353142012412"><a name="zh-cn_topic_0000002210793769_p1353142012412"></a><a name="zh-cn_topic_0000002210793769_p1353142012412"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210793769_row353102010411"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210793769_p15311201744"><a name="zh-cn_topic_0000002210793769_p15311201744"></a><a name="zh-cn_topic_0000002210793769_p15311201744"></a>call_id</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210793769_p175317201240"><a name="zh-cn_topic_0000002210793769_p175317201240"></a><a name="zh-cn_topic_0000002210793769_p175317201240"></a>整型值，呼叫ID。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210793769_row8402823191413"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210793769_p1040242315146"><a name="zh-cn_topic_0000002210793769_p1040242315146"></a><a name="zh-cn_topic_0000002210793769_p1040242315146"></a>call_type</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210793769_p89684041418"><a name="zh-cn_topic_0000002210793769_p89684041418"></a><a name="zh-cn_topic_0000002210793769_p89684041418"></a>整型值，呼叫类型。</p>
<p id="zh-cn_topic_0000002210793769_p12961840171411"><a name="zh-cn_topic_0000002210793769_p12961840171411"></a><a name="zh-cn_topic_0000002210793769_p12961840171411"></a>0：语音呼叫；</p>
<p id="zh-cn_topic_0000002210793769_p17961140151418"><a name="zh-cn_topic_0000002210793769_p17961140151418"></a><a name="zh-cn_topic_0000002210793769_p17961140151418"></a>9：紧急呼叫。</p>
</td>
</tr>
</tbody>
</table>

### 通话结束指示：+CEND<a name="ZH-CN_TOPIC_0000002258627581"></a>

**命令描述<a name="zh-cn_topic_0000002210639405_section4340133117813"></a>**

主动上报命令，在通话结束后，UE通知用户通话结束原因和通话时长。

**命令语法<a name="zh-cn_topic_0000002210639405_section27467181898"></a>**

<a name="zh-cn_topic_0000002210639405_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210639405_row139817074814"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002210639405_p173981104481"><a name="zh-cn_topic_0000002210639405_p173981104481"></a><a name="zh-cn_topic_0000002210639405_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002210639405_p4399902485"><a name="zh-cn_topic_0000002210639405_p4399902485"></a><a name="zh-cn_topic_0000002210639405_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210639405_row23995014487"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210639405_p1427mcpsimp"><a name="zh-cn_topic_0000002210639405_p1427mcpsimp"></a><a name="zh-cn_topic_0000002210639405_p1427mcpsimp"></a>+CEND: &lt;call_id&gt;, &lt;duration&gt;,&lt;cause&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210639405_p101216314618"><a name="zh-cn_topic_0000002210639405_p101216314618"></a><a name="zh-cn_topic_0000002210639405_p101216314618"></a>通话结束</p>
<p id="zh-cn_topic_0000002210639405_p1437mcpsimp"><a name="zh-cn_topic_0000002210639405_p1437mcpsimp"></a><a name="zh-cn_topic_0000002210639405_p1437mcpsimp"></a>+CEND: 1,10,0</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002210639405_section45087272918"></a>**

<a name="zh-cn_topic_0000002210639405_table95301320849"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210639405_row453114201943"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002210639405_p155311201943"><a name="zh-cn_topic_0000002210639405_p155311201943"></a><a name="zh-cn_topic_0000002210639405_p155311201943"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002210639405_p1353142012412"><a name="zh-cn_topic_0000002210639405_p1353142012412"></a><a name="zh-cn_topic_0000002210639405_p1353142012412"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210639405_row353102010411"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210639405_p15311201744"><a name="zh-cn_topic_0000002210639405_p15311201744"></a><a name="zh-cn_topic_0000002210639405_p15311201744"></a>call_id</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210639405_p175317201240"><a name="zh-cn_topic_0000002210639405_p175317201240"></a><a name="zh-cn_topic_0000002210639405_p175317201240"></a>整型值，呼叫ID。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639405_row953112209417"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210639405_p2053113207410"><a name="zh-cn_topic_0000002210639405_p2053113207410"></a><a name="zh-cn_topic_0000002210639405_p2053113207410"></a>duration</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210639405_p253117207411"><a name="zh-cn_topic_0000002210639405_p253117207411"></a><a name="zh-cn_topic_0000002210639405_p253117207411"></a>整型值，通话时长，单位：s。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639405_row253115201840"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210639405_p1553114200414"><a name="zh-cn_topic_0000002210639405_p1553114200414"></a><a name="zh-cn_topic_0000002210639405_p1553114200414"></a>cause</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210639405_p145311220445"><a name="zh-cn_topic_0000002210639405_p145311220445"></a><a name="zh-cn_topic_0000002210639405_p145311220445"></a>整型值，呼叫错误原因值；如<a href="#zh-cn_topic_0000002210639405_table2069216212572">表1</a>所示。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  错误码说明

<a name="zh-cn_topic_0000002210639405_table2069216212572"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210639405_row1069317212576"><th class="cellrowborder" valign="top" width="38.53385338533853%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002210639405_p11693102119578"><a name="zh-cn_topic_0000002210639405_p11693102119578"></a><a name="zh-cn_topic_0000002210639405_p11693102119578"></a>错误码</p>
</th>
<th class="cellrowborder" valign="top" width="8.02080208020802%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002210639405_p6693142119576"><a name="zh-cn_topic_0000002210639405_p6693142119576"></a><a name="zh-cn_topic_0000002210639405_p6693142119576"></a>数值</p>
</th>
<th class="cellrowborder" valign="top" width="53.44534453445344%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002210639405_p7693112195717"><a name="zh-cn_topic_0000002210639405_p7693112195717"></a><a name="zh-cn_topic_0000002210639405_p7693112195717"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210639405_row669310218574"><td class="cellrowborder" valign="top" width="38.53385338533853%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639405_p1519934445816"><a name="zh-cn_topic_0000002210639405_p1519934445816"></a><a name="zh-cn_topic_0000002210639405_p1519934445816"></a>IMSA_CALL_CAUSE_SUCCESS</p>
</td>
<td class="cellrowborder" valign="top" width="8.02080208020802%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639405_p86936215575"><a name="zh-cn_topic_0000002210639405_p86936215575"></a><a name="zh-cn_topic_0000002210639405_p86936215575"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="53.44534453445344%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639405_p17693421125718"><a name="zh-cn_topic_0000002210639405_p17693421125718"></a><a name="zh-cn_topic_0000002210639405_p17693421125718"></a>正常挂断。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639405_row56930216571"><td class="cellrowborder" valign="top" width="38.53385338533853%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639405_p8112152385914"><a name="zh-cn_topic_0000002210639405_p8112152385914"></a><a name="zh-cn_topic_0000002210639405_p8112152385914"></a>IMSA_CALL_CAUSE_IMSA_ERROR</p>
</td>
<td class="cellrowborder" valign="top" width="8.02080208020802%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639405_p169382112572"><a name="zh-cn_topic_0000002210639405_p169382112572"></a><a name="zh-cn_topic_0000002210639405_p169382112572"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="53.44534453445344%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639405_p14829130173518"><a name="zh-cn_topic_0000002210639405_p14829130173518"></a><a name="zh-cn_topic_0000002210639405_p14829130173518"></a>IMSA特殊无服务异常。IMSA_INTRA_CALL_SRV_STATUS_IND消息中no_srv_cause不是IMSA_CALL_NO_SRV_CAUSE_MEDIA_PDP_RELEASE<span>、</span></p>
<p id="zh-cn_topic_0000002210639405_p1169302195713"><a name="zh-cn_topic_0000002210639405_p1169302195713"></a><a name="zh-cn_topic_0000002210639405_p1169302195713"></a>IMSA_CALL_NO_SRV_CAUSE_SIP_PDP_ERR<span>、</span>IMSA_CALL_NO_SRV_CAUSE_REG_ERR<span>、</span>IMSA_CALL_NO_SRV_CAUSE_EMC_SIP_PDP_NO_PCSCF。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639405_row1444218385597"><td class="cellrowborder" valign="top" width="38.53385338533853%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639405_p1781012111445"><a name="zh-cn_topic_0000002210639405_p1781012111445"></a><a name="zh-cn_topic_0000002210639405_p1781012111445"></a>IMSA_CALL_CAUSE_IMSA_TCALL_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" width="8.02080208020802%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639405_p134421038135919"><a name="zh-cn_topic_0000002210639405_p134421038135919"></a><a name="zh-cn_topic_0000002210639405_p134421038135919"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="53.44534453445344%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639405_p105548341464"><a name="zh-cn_topic_0000002210639405_p105548341464"></a><a name="zh-cn_topic_0000002210639405_p105548341464"></a>IMSA Tcall超时。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639405_row1797373111599"><td class="cellrowborder" valign="top" width="38.53385338533853%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639405_p467823019420"><a name="zh-cn_topic_0000002210639405_p467823019420"></a><a name="zh-cn_topic_0000002210639405_p467823019420"></a>IMSA_CALL_CAUSE_IMSA_WAIT_RES_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" width="8.02080208020802%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639405_p12974173119597"><a name="zh-cn_topic_0000002210639405_p12974173119597"></a><a name="zh-cn_topic_0000002210639405_p12974173119597"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="53.44534453445344%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639405_p68061525464"><a name="zh-cn_topic_0000002210639405_p68061525464"></a><a name="zh-cn_topic_0000002210639405_p68061525464"></a>等待资源预留成功定时器超时。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639405_row4974731135911"><td class="cellrowborder" valign="top" width="38.53385338533853%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639405_p19828949141318"><a name="zh-cn_topic_0000002210639405_p19828949141318"></a><a name="zh-cn_topic_0000002210639405_p19828949141318"></a>IMSA_CALL_CAUSE_IMS_EMC_REG_FAIL</p>
</td>
<td class="cellrowborder" valign="top" width="8.02080208020802%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639405_p14974931125912"><a name="zh-cn_topic_0000002210639405_p14974931125912"></a><a name="zh-cn_topic_0000002210639405_p14974931125912"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="53.44534453445344%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639405_p20690425484"><a name="zh-cn_topic_0000002210639405_p20690425484"></a><a name="zh-cn_topic_0000002210639405_p20690425484"></a>紧急注册失败引起紧急呼叫失败。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639405_row38934152113"><td class="cellrowborder" valign="top" width="38.53385338533853%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639405_p68917417218"><a name="zh-cn_topic_0000002210639405_p68917417218"></a><a name="zh-cn_topic_0000002210639405_p68917417218"></a>IMSA_CALL_CAUSE_IMS_INNER_ERROR</p>
</td>
<td class="cellrowborder" valign="top" width="8.02080208020802%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639405_p1989104182114"><a name="zh-cn_topic_0000002210639405_p1989104182114"></a><a name="zh-cn_topic_0000002210639405_p1989104182114"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="53.44534453445344%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639405_p119020415217"><a name="zh-cn_topic_0000002210639405_p119020415217"></a><a name="zh-cn_topic_0000002210639405_p119020415217"></a>IMS协议栈内部异常，IMS侧sip_status_code&lt;100。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639405_row1569312155712"><td class="cellrowborder" valign="top" width="38.53385338533853%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639405_p1469342185710"><a name="zh-cn_topic_0000002210639405_p1469342185710"></a><a name="zh-cn_topic_0000002210639405_p1469342185710"></a>IMSA_CALL_CAUSE_IMSA_STRM_RTP_BREAK</p>
</td>
<td class="cellrowborder" valign="top" width="8.02080208020802%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639405_p46933215570"><a name="zh-cn_topic_0000002210639405_p46933215570"></a><a name="zh-cn_topic_0000002210639405_p46933215570"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="53.44534453445344%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639405_p96932215575"><a name="zh-cn_topic_0000002210639405_p96932215575"></a><a name="zh-cn_topic_0000002210639405_p96932215575"></a>IMS侧sip_status_code=100。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639405_row2693921175719"><td class="cellrowborder" valign="top" width="38.53385338533853%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639405_p1780232411919"><a name="zh-cn_topic_0000002210639405_p1780232411919"></a><a name="zh-cn_topic_0000002210639405_p1780232411919"></a>IMSA_CALL_CAUSE_IMSA_IMS_ERROR</p>
</td>
<td class="cellrowborder" valign="top" width="8.02080208020802%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639405_p669352115712"><a name="zh-cn_topic_0000002210639405_p669352115712"></a><a name="zh-cn_topic_0000002210639405_p669352115712"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="53.44534453445344%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639405_p19693172111579"><a name="zh-cn_topic_0000002210639405_p19693172111579"></a><a name="zh-cn_topic_0000002210639405_p19693172111579"></a>IMS侧100&lt;sip_status_code&lt;300或者sip_status_code≥700。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639405_row2028814610492"><td class="cellrowborder" valign="top" width="38.53385338533853%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639405_p97593101499"><a name="zh-cn_topic_0000002210639405_p97593101499"></a><a name="zh-cn_topic_0000002210639405_p97593101499"></a>IMSA_CALL_CAUSE_NOR_DIAL_ERROR</p>
</td>
<td class="cellrowborder" valign="top" width="8.02080208020802%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639405_p775971034915"><a name="zh-cn_topic_0000002210639405_p775971034915"></a><a name="zh-cn_topic_0000002210639405_p775971034915"></a>8</p>
</td>
<td class="cellrowborder" valign="top" width="53.44534453445344%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639405_p177597109497"><a name="zh-cn_topic_0000002210639405_p177597109497"></a><a name="zh-cn_topic_0000002210639405_p177597109497"></a>IMSA普通呼叫dial请求创建失败。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639405_row634315415462"><td class="cellrowborder" valign="top" width="38.53385338533853%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639405_p19343144114616"><a name="zh-cn_topic_0000002210639405_p19343144114616"></a><a name="zh-cn_topic_0000002210639405_p19343144114616"></a>IMSA_CALL_CAUSE_EMC_DIAL_ERROR</p>
</td>
<td class="cellrowborder" valign="top" width="8.02080208020802%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639405_p1934311419464"><a name="zh-cn_topic_0000002210639405_p1934311419464"></a><a name="zh-cn_topic_0000002210639405_p1934311419464"></a>9</p>
</td>
<td class="cellrowborder" valign="top" width="53.44534453445344%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639405_p10343141114614"><a name="zh-cn_topic_0000002210639405_p10343141114614"></a><a name="zh-cn_topic_0000002210639405_p10343141114614"></a>IMSA紧急呼叫dial请求创建失败。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639405_row6139241431"><td class="cellrowborder" valign="top" width="38.53385338533853%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639405_p3601731893"><a name="zh-cn_topic_0000002210639405_p3601731893"></a><a name="zh-cn_topic_0000002210639405_p3601731893"></a>IMSA_CALL_CAUSE_SIGNAL_PDP_ERR</p>
</td>
<td class="cellrowborder" valign="top" width="8.02080208020802%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639405_p151391541133"><a name="zh-cn_topic_0000002210639405_p151391541133"></a><a name="zh-cn_topic_0000002210639405_p151391541133"></a>10</p>
</td>
<td class="cellrowborder" valign="top" width="53.44534453445344%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639405_p81396410310"><a name="zh-cn_topic_0000002210639405_p81396410310"></a><a name="zh-cn_topic_0000002210639405_p81396410310"></a>默认承载被释放。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639405_row8139341931"><td class="cellrowborder" valign="top" width="38.53385338533853%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639405_p174644101696"><a name="zh-cn_topic_0000002210639405_p174644101696"></a><a name="zh-cn_topic_0000002210639405_p174644101696"></a>IMSA_CALL_CAUSE_MEDIA_PDP_ERR</p>
</td>
<td class="cellrowborder" valign="top" width="8.02080208020802%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639405_p01391441335"><a name="zh-cn_topic_0000002210639405_p01391441335"></a><a name="zh-cn_topic_0000002210639405_p01391441335"></a>11</p>
</td>
<td class="cellrowborder" valign="top" width="53.44534453445344%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639405_p713944114314"><a name="zh-cn_topic_0000002210639405_p713944114314"></a><a name="zh-cn_topic_0000002210639405_p713944114314"></a>专有承载被释放。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639405_row1399833418310"><td class="cellrowborder" valign="top" width="38.53385338533853%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639405_p29984340312"><a name="zh-cn_topic_0000002210639405_p29984340312"></a><a name="zh-cn_topic_0000002210639405_p29984340312"></a>300～700</p>
</td>
<td class="cellrowborder" valign="top" width="8.02080208020802%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639405_p2099815341736"><a name="zh-cn_topic_0000002210639405_p2099815341736"></a><a name="zh-cn_topic_0000002210639405_p2099815341736"></a>300～700</p>
</td>
<td class="cellrowborder" valign="top" width="53.44534453445344%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639405_p139981934531"><a name="zh-cn_topic_0000002210639405_p139981934531"></a><a name="zh-cn_topic_0000002210639405_p139981934531"></a>IMS侧300≤sip_status_code&lt;700。</p>
</td>
</tr>
</tbody>
</table>

### 呼叫状态上报：+CALLSTATE<a name="ZH-CN_TOPIC_0000002223508084"></a>

**命令描述<a name="zh-cn_topic_0000002175433052_section4340133117813"></a>**

主动上报命令，呼叫过程中，主动上报呼叫状态的变化。

**命令语法<a name="zh-cn_topic_0000002175433052_section27467181898"></a>**

<a name="zh-cn_topic_0000002175433052_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175433052_row139817074814"><th class="cellrowborder" valign="top" width="47.39%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002175433052_p173981104481"><a name="zh-cn_topic_0000002175433052_p173981104481"></a><a name="zh-cn_topic_0000002175433052_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="52.61%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002175433052_p4399902485"><a name="zh-cn_topic_0000002175433052_p4399902485"></a><a name="zh-cn_topic_0000002175433052_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175433052_row23995014487"><td class="cellrowborder" valign="top" width="47.39%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433052_p1427mcpsimp"><a name="zh-cn_topic_0000002175433052_p1427mcpsimp"></a><a name="zh-cn_topic_0000002175433052_p1427mcpsimp"></a>+CALLSTATE: &lt;call_id&gt;, &lt;state&gt;,&lt;domain&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="52.61%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433052_p13480133124711"><a name="zh-cn_topic_0000002175433052_p13480133124711"></a><a name="zh-cn_topic_0000002175433052_p13480133124711"></a>有IMS域来电</p>
<p id="zh-cn_topic_0000002175433052_p1437mcpsimp"><a name="zh-cn_topic_0000002175433052_p1437mcpsimp"></a><a name="zh-cn_topic_0000002175433052_p1437mcpsimp"></a>+CALLSTATE: 1,5,1</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002175433052_section9812143715423"></a>**

<a name="zh-cn_topic_0000002175433052_table95301320849"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175433052_row453114201943"><th class="cellrowborder" valign="top" width="36.09%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002175433052_p155311201943"><a name="zh-cn_topic_0000002175433052_p155311201943"></a><a name="zh-cn_topic_0000002175433052_p155311201943"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="63.91%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002175433052_p1353142012412"><a name="zh-cn_topic_0000002175433052_p1353142012412"></a><a name="zh-cn_topic_0000002175433052_p1353142012412"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175433052_row353102010411"><td class="cellrowborder" valign="top" width="36.09%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433052_p15311201744"><a name="zh-cn_topic_0000002175433052_p15311201744"></a><a name="zh-cn_topic_0000002175433052_p15311201744"></a>call_id</p>
</td>
<td class="cellrowborder" valign="top" width="63.91%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433052_p175317201240"><a name="zh-cn_topic_0000002175433052_p175317201240"></a><a name="zh-cn_topic_0000002175433052_p175317201240"></a>整型值，呼叫ID。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175433052_row953112209417"><td class="cellrowborder" valign="top" width="36.09%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433052_p2053113207410"><a name="zh-cn_topic_0000002175433052_p2053113207410"></a><a name="zh-cn_topic_0000002175433052_p2053113207410"></a>state</p>
</td>
<td class="cellrowborder" valign="top" width="63.91%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433052_p9828320113"><a name="zh-cn_topic_0000002175433052_p9828320113"></a><a name="zh-cn_topic_0000002175433052_p9828320113"></a>整数型，指示对应call_id的呼叫状态。</p>
<p id="zh-cn_topic_0000002175433052_p188218323110"><a name="zh-cn_topic_0000002175433052_p188218323110"></a><a name="zh-cn_topic_0000002175433052_p188218323110"></a>0：Call originate（MO），向网络发起呼叫；</p>
<p id="zh-cn_topic_0000002175433052_p17820326119"><a name="zh-cn_topic_0000002175433052_p17820326119"></a><a name="zh-cn_topic_0000002175433052_p17820326119"></a>1：Call is Proceeding，呼叫发起，网络已经开始处理；</p>
<p id="zh-cn_topic_0000002175433052_p5826324118"><a name="zh-cn_topic_0000002175433052_p5826324118"></a><a name="zh-cn_topic_0000002175433052_p5826324118"></a>2：Alerting，振铃；</p>
<p id="zh-cn_topic_0000002175433052_p3821932131114"><a name="zh-cn_topic_0000002175433052_p3821932131114"></a><a name="zh-cn_topic_0000002175433052_p3821932131114"></a>3：Connected，呼叫已经接通；</p>
<p id="zh-cn_topic_0000002175433052_p10822032191112"><a name="zh-cn_topic_0000002175433052_p10822032191112"></a><a name="zh-cn_topic_0000002175433052_p10822032191112"></a>4：Released，呼叫被释放；</p>
<p id="zh-cn_topic_0000002175433052_p982332161117"><a name="zh-cn_topic_0000002175433052_p982332161117"></a><a name="zh-cn_topic_0000002175433052_p982332161117"></a>5：InComing，有来电；</p>
<p id="zh-cn_topic_0000002175433052_p148293291112"><a name="zh-cn_topic_0000002175433052_p148293291112"></a><a name="zh-cn_topic_0000002175433052_p148293291112"></a>6：Waiting，有一个等待的来电；</p>
<p id="zh-cn_topic_0000002175433052_p3821032101112"><a name="zh-cn_topic_0000002175433052_p3821032101112"></a><a name="zh-cn_topic_0000002175433052_p3821032101112"></a>7：Hold，呼叫保持；</p>
<p id="zh-cn_topic_0000002175433052_p58219323118"><a name="zh-cn_topic_0000002175433052_p58219323118"></a><a name="zh-cn_topic_0000002175433052_p58219323118"></a>8：Retrieve，呼叫恢复。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175433052_row253115201840"><td class="cellrowborder" valign="top" width="36.09%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175433052_p1553114200414"><a name="zh-cn_topic_0000002175433052_p1553114200414"></a><a name="zh-cn_topic_0000002175433052_p1553114200414"></a>domain</p>
</td>
<td class="cellrowborder" valign="top" width="63.91%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175433052_p18851449201119"><a name="zh-cn_topic_0000002175433052_p18851449201119"></a><a name="zh-cn_topic_0000002175433052_p18851449201119"></a>整型值，语音电话的呼叫域。</p>
<p id="zh-cn_topic_0000002175433052_p9886174971120"><a name="zh-cn_topic_0000002175433052_p9886174971120"></a><a name="zh-cn_topic_0000002175433052_p9886174971120"></a>0：CS域语音电话；</p>
<p id="zh-cn_topic_0000002175433052_p19886194961115"><a name="zh-cn_topic_0000002175433052_p19886194961115"></a><a name="zh-cn_topic_0000002175433052_p19886194961115"></a>1：IMS域语音电话。</p>
</td>
</tr>
</tbody>
</table>

### 主动上报语音带宽信息：+CHANNELINFO<a name="ZH-CN_TOPIC_0000002223667924"></a>

**命令描述<a name="zh-cn_topic_0000002175273340_section4340133117813"></a>**

主动上报命令，呼叫过程中，主动上报语音带宽的信息。

**命令语法<a name="zh-cn_topic_0000002175273340_section27467181898"></a>**

<a name="zh-cn_topic_0000002175273340_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175273340_row139817074814"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002175273340_p173981104481"><a name="zh-cn_topic_0000002175273340_p173981104481"></a><a name="zh-cn_topic_0000002175273340_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002175273340_p4399902485"><a name="zh-cn_topic_0000002175273340_p4399902485"></a><a name="zh-cn_topic_0000002175273340_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175273340_row23995014487"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175273340_p1427mcpsimp"><a name="zh-cn_topic_0000002175273340_p1427mcpsimp"></a><a name="zh-cn_topic_0000002175273340_p1427mcpsimp"></a>+CHANNELINFO: &lt;status&gt;,&lt;domain&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175273340_p033323711226"><a name="zh-cn_topic_0000002175273340_p033323711226"></a><a name="zh-cn_topic_0000002175273340_p033323711226"></a>IMS域语音电话，当前语音处于宽带语音时，主动上报</p>
<p id="zh-cn_topic_0000002175273340_p131111127633"><a name="zh-cn_topic_0000002175273340_p131111127633"></a><a name="zh-cn_topic_0000002175273340_p131111127633"></a>+CHANNELINFO: 2,1</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002175273340_section45087272918"></a>**

<a name="zh-cn_topic_0000002175273340_table66122098220"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002175273340_row166129916216"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002175273340_p5612109128"><a name="zh-cn_topic_0000002175273340_p5612109128"></a><a name="zh-cn_topic_0000002175273340_p5612109128"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002175273340_p146120912216"><a name="zh-cn_topic_0000002175273340_p146120912216"></a><a name="zh-cn_topic_0000002175273340_p146120912216"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002175273340_row196121191024"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175273340_p66121891215"><a name="zh-cn_topic_0000002175273340_p66121891215"></a><a name="zh-cn_topic_0000002175273340_p66121891215"></a>status</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175273340_p12865451801"><a name="zh-cn_topic_0000002175273340_p12865451801"></a><a name="zh-cn_topic_0000002175273340_p12865451801"></a>整型值，语音信道的状态。</p>
<p id="zh-cn_topic_0000002175273340_p1382513143410"><a name="zh-cn_topic_0000002175273340_p1382513143410"></a><a name="zh-cn_topic_0000002175273340_p1382513143410"></a>0：振铃，无带内音信息（本地回铃音上报）；</p>
<p id="zh-cn_topic_0000002175273340_p15825181412412"><a name="zh-cn_topic_0000002175273340_p15825181412412"></a><a name="zh-cn_topic_0000002175273340_p15825181412412"></a>1：带内音可用，窄带语音（网侧携带铃音）；</p>
<p id="zh-cn_topic_0000002175273340_p482511148413"><a name="zh-cn_topic_0000002175273340_p482511148413"></a><a name="zh-cn_topic_0000002175273340_p482511148413"></a>2：带内音可用，宽带语音（网侧携带铃音）。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002175273340_row15612139427"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002175273340_p10612699214"><a name="zh-cn_topic_0000002175273340_p10612699214"></a><a name="zh-cn_topic_0000002175273340_p10612699214"></a>domain</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002175273340_p37314221746"><a name="zh-cn_topic_0000002175273340_p37314221746"></a><a name="zh-cn_topic_0000002175273340_p37314221746"></a>整型值，语音电话的呼叫域。</p>
<p id="zh-cn_topic_0000002175273340_p1073111221747"><a name="zh-cn_topic_0000002175273340_p1073111221747"></a><a name="zh-cn_topic_0000002175273340_p1073111221747"></a>0：CS域语音电话（本产品无CS）；</p>
<p id="zh-cn_topic_0000002175273340_p117312221418"><a name="zh-cn_topic_0000002175273340_p117312221418"></a><a name="zh-cn_topic_0000002175273340_p117312221418"></a>1：IMS域语音电话。</p>
</td>
</tr>
</tbody>
</table>

### 上报IMS注册失败原因：+IMSREGERR<a name="ZH-CN_TOPIC_0000002258467701"></a>

**命令描述<a name="zh-cn_topic_0000002210793773_section4340133117813"></a>**

该命令用于上报IMS注册失败原因。

**命令语法<a name="zh-cn_topic_0000002210793773_section27467181898"></a>**

<a name="zh-cn_topic_0000002210793773_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210793773_row139817074814"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002210793773_p173981104481"><a name="zh-cn_topic_0000002210793773_p173981104481"></a><a name="zh-cn_topic_0000002210793773_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002210793773_p4399902485"><a name="zh-cn_topic_0000002210793773_p4399902485"></a><a name="zh-cn_topic_0000002210793773_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210793773_row23995014487"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210793773_p1427mcpsimp"><a name="zh-cn_topic_0000002210793773_p1427mcpsimp"></a><a name="zh-cn_topic_0000002210793773_p1427mcpsimp"></a>+IMSREGERR: &lt;err_type&gt;, &lt;err_code&gt;,&lt;sip_reason&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210793773_p558003984715"><a name="zh-cn_topic_0000002210793773_p558003984715"></a><a name="zh-cn_topic_0000002210793773_p558003984715"></a>通话结束</p>
<p id="zh-cn_topic_0000002210793773_p1437mcpsimp"><a name="zh-cn_topic_0000002210793773_p1437mcpsimp"></a><a name="zh-cn_topic_0000002210793773_p1437mcpsimp"></a>+IMSREGERR: 1,3,</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002210793773_section45087272918"></a>**

<a name="zh-cn_topic_0000002210793773_table95301320849"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210793773_row453114201943"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002210793773_p155311201943"><a name="zh-cn_topic_0000002210793773_p155311201943"></a><a name="zh-cn_topic_0000002210793773_p155311201943"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002210793773_p1353142012412"><a name="zh-cn_topic_0000002210793773_p1353142012412"></a><a name="zh-cn_topic_0000002210793773_p1353142012412"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210793773_row353102010411"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210793773_p15311201744"><a name="zh-cn_topic_0000002210793773_p15311201744"></a><a name="zh-cn_topic_0000002210793773_p15311201744"></a>err_type</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210793773_p175317201240"><a name="zh-cn_topic_0000002210793773_p175317201240"></a><a name="zh-cn_topic_0000002210793773_p175317201240"></a>整型值，错误类型。</p>
<p id="zh-cn_topic_0000002210793773_p106425415514"><a name="zh-cn_topic_0000002210793773_p106425415514"></a><a name="zh-cn_topic_0000002210793773_p106425415514"></a>0：PDN建立失败；</p>
<p id="zh-cn_topic_0000002210793773_p18112161105210"><a name="zh-cn_topic_0000002210793773_p18112161105210"></a><a name="zh-cn_topic_0000002210793773_p18112161105210"></a>1：IMS注册失败。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210793773_row953112209417"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210793773_p2053113207410"><a name="zh-cn_topic_0000002210793773_p2053113207410"></a><a name="zh-cn_topic_0000002210793773_p2053113207410"></a>err_code</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210793773_p9828320113"><a name="zh-cn_topic_0000002210793773_p9828320113"></a><a name="zh-cn_topic_0000002210793773_p9828320113"></a>整数型，失败原因值。</p>
<p id="zh-cn_topic_0000002210793773_p4940103725217"><a name="zh-cn_topic_0000002210793773_p4940103725217"></a><a name="zh-cn_topic_0000002210793773_p4940103725217"></a>当err_type为0时：PDN建立失败原因。</p>
<p id="zh-cn_topic_0000002210793773_p15415357175211"><a name="zh-cn_topic_0000002210793773_p15415357175211"></a><a name="zh-cn_topic_0000002210793773_p15415357175211"></a>当err_type为1时：</p>
<p id="zh-cn_topic_0000002210793773_p1366211230275"><a name="zh-cn_topic_0000002210793773_p1366211230275"></a><a name="zh-cn_topic_0000002210793773_p1366211230275"></a>0：注册模块命令执行成功（不会出现）；</p>
<p id="zh-cn_topic_0000002210793773_p19815123319278"><a name="zh-cn_topic_0000002210793773_p19815123319278"></a><a name="zh-cn_topic_0000002210793773_p19815123319278"></a>1：注册模块命令执行失败；</p>
<p id="zh-cn_topic_0000002210793773_p5826324118"><a name="zh-cn_topic_0000002210793773_p5826324118"></a><a name="zh-cn_topic_0000002210793773_p5826324118"></a>2：网侧SIP拒绝，具体原因见sip_reason；</p>
<p id="zh-cn_topic_0000002210793773_p788112168288"><a name="zh-cn_topic_0000002210793773_p788112168288"></a><a name="zh-cn_topic_0000002210793773_p788112168288"></a>3：发送或接收SIP报文超时；</p>
<p id="zh-cn_topic_0000002210793773_p1584012612283"><a name="zh-cn_topic_0000002210793773_p1584012612283"></a><a name="zh-cn_topic_0000002210793773_p1584012612283"></a>4：发送或接收SIP报文时，底层传输发生错误；</p>
<p id="zh-cn_topic_0000002210793773_p1592513392816"><a name="zh-cn_topic_0000002210793773_p1592513392816"></a><a name="zh-cn_topic_0000002210793773_p1592513392816"></a>5：注册成功后，网侧发起的去注册；</p>
<p id="zh-cn_topic_0000002210793773_p1899865922819"><a name="zh-cn_topic_0000002210793773_p1899865922819"></a><a name="zh-cn_topic_0000002210793773_p1899865922819"></a>6：没有可以再使用的注册地址对；</p>
<p id="zh-cn_topic_0000002210793773_p171401960297"><a name="zh-cn_topic_0000002210793773_p171401960297"></a><a name="zh-cn_topic_0000002210793773_p171401960297"></a>7：注册被禁止（暂时仅作预留，不会出现）；</p>
<p id="zh-cn_topic_0000002210793773_p7415943192915"><a name="zh-cn_topic_0000002210793773_p7415943192915"></a><a name="zh-cn_topic_0000002210793773_p7415943192915"></a>8：周期注册，被网侧拒绝；</p>
<p id="zh-cn_topic_0000002210793773_p2099815341736"><a name="zh-cn_topic_0000002210793773_p2099815341736"></a><a name="zh-cn_topic_0000002210793773_p2099815341736"></a>300～700：IMS失败原因值。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210793773_row253115201840"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210793773_p1553114200414"><a name="zh-cn_topic_0000002210793773_p1553114200414"></a><a name="zh-cn_topic_0000002210793773_p1553114200414"></a>sip_reason</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210793773_p18851449201119"><a name="zh-cn_topic_0000002210793773_p18851449201119"></a><a name="zh-cn_topic_0000002210793773_p18851449201119"></a>字符串值类型，网侧SIP消息拒绝携带的原因值。</p>
</td>
</tr>
</tbody>
</table>

### IMS域被叫处理状态信息主动上报：+IMSMTRPT<a name="ZH-CN_TOPIC_0000002258627585"></a>

**命令描述<a name="zh-cn_topic_0000002210639409_section4340133117813"></a>**

该命令用于上报IMSA开始处理被叫。

**命令语法<a name="zh-cn_topic_0000002210639409_section27467181898"></a>**

<a name="zh-cn_topic_0000002210639409_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210639409_row139817074814"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002210639409_p173981104481"><a name="zh-cn_topic_0000002210639409_p173981104481"></a><a name="zh-cn_topic_0000002210639409_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002210639409_p4399902485"><a name="zh-cn_topic_0000002210639409_p4399902485"></a><a name="zh-cn_topic_0000002210639409_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210639409_row23995014487"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210639409_p1427mcpsimp"><a name="zh-cn_topic_0000002210639409_p1427mcpsimp"></a><a name="zh-cn_topic_0000002210639409_p1427mcpsimp"></a>+IMSMTRPT: &lt;number&gt;,&lt;status&gt;,&lt;cause&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210639409_p1549679173420"><a name="zh-cn_topic_0000002210639409_p1549679173420"></a><a name="zh-cn_topic_0000002210639409_p1549679173420"></a>IMSA开始处理被叫主动上报：</p>
<p id="zh-cn_topic_0000002210639409_p1882518433416"><a name="zh-cn_topic_0000002210639409_p1882518433416"></a><a name="zh-cn_topic_0000002210639409_p1882518433416"></a>+IMSMTRPT: +8618000009193,0,0</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002210639409_section1264793011438"></a>**

<a name="zh-cn_topic_0000002210639409_table147251929115518"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210639409_row14726172914553"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002210639409_p1657919435619"><a name="zh-cn_topic_0000002210639409_p1657919435619"></a><a name="zh-cn_topic_0000002210639409_p1657919435619"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002210639409_p2579184175610"><a name="zh-cn_topic_0000002210639409_p2579184175610"></a><a name="zh-cn_topic_0000002210639409_p2579184175610"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210639409_row11726172935518"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210639409_p1772622955513"><a name="zh-cn_topic_0000002210639409_p1772622955513"></a><a name="zh-cn_topic_0000002210639409_p1772622955513"></a>number</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210639409_p137269293556"><a name="zh-cn_topic_0000002210639409_p137269293556"></a><a name="zh-cn_topic_0000002210639409_p137269293556"></a>字符串，被叫来电号码。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row57261298552"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210639409_p17726192985512"><a name="zh-cn_topic_0000002210639409_p17726192985512"></a><a name="zh-cn_topic_0000002210639409_p17726192985512"></a>status</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210639409_p13592247576"><a name="zh-cn_topic_0000002210639409_p13592247576"></a><a name="zh-cn_topic_0000002210639409_p13592247576"></a>整型值，状态。</p>
<p id="zh-cn_topic_0000002210639409_p826814422565"><a name="zh-cn_topic_0000002210639409_p826814422565"></a><a name="zh-cn_topic_0000002210639409_p826814422565"></a>0：收到被叫，开始处理被叫过程；</p>
<p id="zh-cn_topic_0000002210639409_p726815425567"><a name="zh-cn_topic_0000002210639409_p726815425567"></a><a name="zh-cn_topic_0000002210639409_p726815425567"></a>1：被叫异常，未收到RING就结束。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row572602975514"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002210639409_p177269296552"><a name="zh-cn_topic_0000002210639409_p177269296552"></a><a name="zh-cn_topic_0000002210639409_p177269296552"></a>cause</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002210639409_p1272682965518"><a name="zh-cn_topic_0000002210639409_p1272682965518"></a><a name="zh-cn_topic_0000002210639409_p1272682965518"></a>整型值，呼叫错误原因值；如<a href="#zh-cn_topic_0000002210639409_table2069216212572">表1</a>所示。</p>
</td>
</tr>
</tbody>
</table>

**表 1**  错误码说明

<a name="zh-cn_topic_0000002210639409_table2069216212572"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002210639409_row1069317212576"><th class="cellrowborder" valign="top" width="37.14371437143714%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002210639409_p11693102119578"><a name="zh-cn_topic_0000002210639409_p11693102119578"></a><a name="zh-cn_topic_0000002210639409_p11693102119578"></a>错误码</p>
</th>
<th class="cellrowborder" valign="top" width="8.330833083308331%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002210639409_p6693142119576"><a name="zh-cn_topic_0000002210639409_p6693142119576"></a><a name="zh-cn_topic_0000002210639409_p6693142119576"></a>数值</p>
</th>
<th class="cellrowborder" valign="top" width="54.525452545254524%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002210639409_p7693112195717"><a name="zh-cn_topic_0000002210639409_p7693112195717"></a><a name="zh-cn_topic_0000002210639409_p7693112195717"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002210639409_row669310218574"><td class="cellrowborder" valign="top" width="37.14371437143714%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639409_p1401144710519"><a name="zh-cn_topic_0000002210639409_p1401144710519"></a><a name="zh-cn_topic_0000002210639409_p1401144710519"></a>IMSA_CALL_CAUSE_SUCCESS</p>
</td>
<td class="cellrowborder" valign="top" width="8.330833083308331%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639409_p11401144715117"><a name="zh-cn_topic_0000002210639409_p11401144715117"></a><a name="zh-cn_topic_0000002210639409_p11401144715117"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="54.525452545254524%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639409_p1240213475516"><a name="zh-cn_topic_0000002210639409_p1240213475516"></a><a name="zh-cn_topic_0000002210639409_p1240213475516"></a>正常挂断。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row56930216571"><td class="cellrowborder" valign="top" width="37.14371437143714%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639409_p8402194715115"><a name="zh-cn_topic_0000002210639409_p8402194715115"></a><a name="zh-cn_topic_0000002210639409_p8402194715115"></a>IMSA_CALL_CAUSE_IMSA_ERROR</p>
</td>
<td class="cellrowborder" valign="top" width="8.330833083308331%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639409_p840294714511"><a name="zh-cn_topic_0000002210639409_p840294714511"></a><a name="zh-cn_topic_0000002210639409_p840294714511"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="54.525452545254524%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639409_p14402154735114"><a name="zh-cn_topic_0000002210639409_p14402154735114"></a><a name="zh-cn_topic_0000002210639409_p14402154735114"></a>IMSA特殊无服务异常。IMSA_INTRA_CALL_SRV_STATUS_IND消息中no_srv_cause不是IMSA_CALL_NO_SRV_CAUSE_MEDIA_PDP_RELEASE，IMSA_CALL_NO_SRV_CAUSE_SIP_PDP_ERR，IMSA_CALL_NO_SRV_CAUSE_REG_ERR，IMSA_CALL_NO_SRV_CAUSE_EMC_SIP_PDP_NO_PCSCF。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row1444218385597"><td class="cellrowborder" valign="top" width="37.14371437143714%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639409_p5402134715120"><a name="zh-cn_topic_0000002210639409_p5402134715120"></a><a name="zh-cn_topic_0000002210639409_p5402134715120"></a>IMSA_CALL_CAUSE_IMSA_TCALL_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" width="8.330833083308331%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639409_p54021847115117"><a name="zh-cn_topic_0000002210639409_p54021847115117"></a><a name="zh-cn_topic_0000002210639409_p54021847115117"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="54.525452545254524%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639409_p1440217477512"><a name="zh-cn_topic_0000002210639409_p1440217477512"></a><a name="zh-cn_topic_0000002210639409_p1440217477512"></a>IMSA Tcall超时。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row1797373111599"><td class="cellrowborder" valign="top" width="37.14371437143714%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639409_p1040234775112"><a name="zh-cn_topic_0000002210639409_p1040234775112"></a><a name="zh-cn_topic_0000002210639409_p1040234775112"></a>IMSA_CALL_CAUSE_IMSA_WAIT_RES_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" width="8.330833083308331%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639409_p7402647135119"><a name="zh-cn_topic_0000002210639409_p7402647135119"></a><a name="zh-cn_topic_0000002210639409_p7402647135119"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="54.525452545254524%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639409_p340264714514"><a name="zh-cn_topic_0000002210639409_p340264714514"></a><a name="zh-cn_topic_0000002210639409_p340264714514"></a>等待资源预留成功定时器超时。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row13974103111595"><td class="cellrowborder" valign="top" width="37.14371437143714%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639409_p12402147165117"><a name="zh-cn_topic_0000002210639409_p12402147165117"></a><a name="zh-cn_topic_0000002210639409_p12402147165117"></a>IMSA_CALL_CAUSE_IMS_EMC_REG_FAIL</p>
</td>
<td class="cellrowborder" valign="top" width="8.330833083308331%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639409_p12402347145115"><a name="zh-cn_topic_0000002210639409_p12402347145115"></a><a name="zh-cn_topic_0000002210639409_p12402347145115"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="54.525452545254524%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639409_p240244711518"><a name="zh-cn_topic_0000002210639409_p240244711518"></a><a name="zh-cn_topic_0000002210639409_p240244711518"></a>紧急注册失败引起紧急呼叫失败。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row4974731135911"><td class="cellrowborder" valign="top" width="37.14371437143714%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639409_p68917417218"><a name="zh-cn_topic_0000002210639409_p68917417218"></a><a name="zh-cn_topic_0000002210639409_p68917417218"></a>IMSA_CALL_CAUSE_IMS_INNER_ERROR</p>
</td>
<td class="cellrowborder" valign="top" width="8.330833083308331%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639409_p1989104182114"><a name="zh-cn_topic_0000002210639409_p1989104182114"></a><a name="zh-cn_topic_0000002210639409_p1989104182114"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="54.525452545254524%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639409_p119020415217"><a name="zh-cn_topic_0000002210639409_p119020415217"></a><a name="zh-cn_topic_0000002210639409_p119020415217"></a>IMS协议栈内部异常，IMS侧sip_status_code&lt;100。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row29741231105911"><td class="cellrowborder" valign="top" width="37.14371437143714%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639409_p15402047155119"><a name="zh-cn_topic_0000002210639409_p15402047155119"></a><a name="zh-cn_topic_0000002210639409_p15402047155119"></a>IMSA_CALL_CAUSE_IMSA_STRM_RTP_BREAK</p>
</td>
<td class="cellrowborder" valign="top" width="8.330833083308331%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639409_p1440284725114"><a name="zh-cn_topic_0000002210639409_p1440284725114"></a><a name="zh-cn_topic_0000002210639409_p1440284725114"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="54.525452545254524%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639409_p2402154765119"><a name="zh-cn_topic_0000002210639409_p2402154765119"></a><a name="zh-cn_topic_0000002210639409_p2402154765119"></a>IMS侧sip_status_code=100。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row1197453115590"><td class="cellrowborder" valign="top" width="37.14371437143714%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639409_p1840264715518"><a name="zh-cn_topic_0000002210639409_p1840264715518"></a><a name="zh-cn_topic_0000002210639409_p1840264715518"></a>IMSA_CALL_CAUSE_IMSA_IMS_ERROR</p>
</td>
<td class="cellrowborder" valign="top" width="8.330833083308331%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639409_p6402847125119"><a name="zh-cn_topic_0000002210639409_p6402847125119"></a><a name="zh-cn_topic_0000002210639409_p6402847125119"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="54.525452545254524%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639409_p640284713510"><a name="zh-cn_topic_0000002210639409_p640284713510"></a><a name="zh-cn_topic_0000002210639409_p640284713510"></a>IMS侧100&lt;sip_status_code&lt;300或者sip_status_code≥700。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row17693192120576"><td class="cellrowborder" valign="top" width="37.14371437143714%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639409_p97593101499"><a name="zh-cn_topic_0000002210639409_p97593101499"></a><a name="zh-cn_topic_0000002210639409_p97593101499"></a>IMSA_CALL_CAUSE_NOR_DIAL_ERROR</p>
</td>
<td class="cellrowborder" valign="top" width="8.330833083308331%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639409_p775971034915"><a name="zh-cn_topic_0000002210639409_p775971034915"></a><a name="zh-cn_topic_0000002210639409_p775971034915"></a>8</p>
</td>
<td class="cellrowborder" valign="top" width="54.525452545254524%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639409_p177597109497"><a name="zh-cn_topic_0000002210639409_p177597109497"></a><a name="zh-cn_topic_0000002210639409_p177597109497"></a>IMSA普通呼叫dial请求创建失败。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row1569312155712"><td class="cellrowborder" valign="top" width="37.14371437143714%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639409_p19343144114616"><a name="zh-cn_topic_0000002210639409_p19343144114616"></a><a name="zh-cn_topic_0000002210639409_p19343144114616"></a>IMSA_CALL_CAUSE_EMC_DIAL_ERROR</p>
</td>
<td class="cellrowborder" valign="top" width="8.330833083308331%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639409_p1934311419464"><a name="zh-cn_topic_0000002210639409_p1934311419464"></a><a name="zh-cn_topic_0000002210639409_p1934311419464"></a>9</p>
</td>
<td class="cellrowborder" valign="top" width="54.525452545254524%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639409_p10343141114614"><a name="zh-cn_topic_0000002210639409_p10343141114614"></a><a name="zh-cn_topic_0000002210639409_p10343141114614"></a>IMSA紧急呼叫dial请求创建失败。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row2693921175719"><td class="cellrowborder" valign="top" width="37.14371437143714%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639409_p1540213477518"><a name="zh-cn_topic_0000002210639409_p1540213477518"></a><a name="zh-cn_topic_0000002210639409_p1540213477518"></a>IMSA_CALL_CAUSE_SIGNAL_PDP_ERR</p>
</td>
<td class="cellrowborder" valign="top" width="8.330833083308331%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639409_p1340213479511"><a name="zh-cn_topic_0000002210639409_p1340213479511"></a><a name="zh-cn_topic_0000002210639409_p1340213479511"></a>10</p>
</td>
<td class="cellrowborder" valign="top" width="54.525452545254524%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639409_p54021647155119"><a name="zh-cn_topic_0000002210639409_p54021647155119"></a><a name="zh-cn_topic_0000002210639409_p54021647155119"></a>默认承载被释放。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row206931721185711"><td class="cellrowborder" valign="top" width="37.14371437143714%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639409_p14021247165116"><a name="zh-cn_topic_0000002210639409_p14021247165116"></a><a name="zh-cn_topic_0000002210639409_p14021247165116"></a>IMSA_CALL_CAUSE_MEDIA_PDP_ERR</p>
</td>
<td class="cellrowborder" valign="top" width="8.330833083308331%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639409_p134021847165112"><a name="zh-cn_topic_0000002210639409_p134021847165112"></a><a name="zh-cn_topic_0000002210639409_p134021847165112"></a>11</p>
</td>
<td class="cellrowborder" valign="top" width="54.525452545254524%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639409_p1340244712519"><a name="zh-cn_topic_0000002210639409_p1340244712519"></a><a name="zh-cn_topic_0000002210639409_p1340244712519"></a>专有承载被释放。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row6139241431"><td class="cellrowborder" valign="top" width="37.14371437143714%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639409_p7642136111617"><a name="zh-cn_topic_0000002210639409_p7642136111617"></a><a name="zh-cn_topic_0000002210639409_p7642136111617"></a>IMSA_CALL_CAUSE_EMM_EST_ERR</p>
</td>
<td class="cellrowborder" valign="top" width="8.330833083308331%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639409_p206425369165"><a name="zh-cn_topic_0000002210639409_p206425369165"></a><a name="zh-cn_topic_0000002210639409_p206425369165"></a>12</p>
</td>
<td class="cellrowborder" valign="top" width="54.525452545254524%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639409_p36421636111612"><a name="zh-cn_topic_0000002210639409_p36421636111612"></a><a name="zh-cn_topic_0000002210639409_p36421636111612"></a>EMM建链失败引起呼叫失败。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002210639409_row8139341931"><td class="cellrowborder" valign="top" width="37.14371437143714%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002210639409_p144024479514"><a name="zh-cn_topic_0000002210639409_p144024479514"></a><a name="zh-cn_topic_0000002210639409_p144024479514"></a>300～700</p>
</td>
<td class="cellrowborder" valign="top" width="8.330833083308331%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002210639409_p1740254725114"><a name="zh-cn_topic_0000002210639409_p1740254725114"></a><a name="zh-cn_topic_0000002210639409_p1740254725114"></a>300～700</p>
</td>
<td class="cellrowborder" valign="top" width="54.525452545254524%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002210639409_p18403174710514"><a name="zh-cn_topic_0000002210639409_p18403174710514"></a><a name="zh-cn_topic_0000002210639409_p18403174710514"></a>IMS侧300≤sip_status_code&lt;700。</p>
</td>
</tr>
</tbody>
</table>

### 关闭IMS功能开关：+IMSSWITCH<a name="ZH-CN_TOPIC_0000002392384649"></a>

**命令描述<a name="zh-cn_topic_0000002392193713_section4340133117813"></a>**

配置开启/关闭IMS协议栈。

**命令语法<a name="zh-cn_topic_0000002392193713_section27467181898"></a>**

<a name="zh-cn_topic_0000002392193713_table143981800489"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002392193713_row139817074814"><th class="cellrowborder" valign="top" width="33.333333333333336%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002392193713_p173981104481"><a name="zh-cn_topic_0000002392193713_p173981104481"></a><a name="zh-cn_topic_0000002392193713_p173981104481"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="33.29332933293329%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002392193713_p339915016481"><a name="zh-cn_topic_0000002392193713_p339915016481"></a><a name="zh-cn_topic_0000002392193713_p339915016481"></a>返回结果</p>
</th>
<th class="cellrowborder" valign="top" width="33.373337333733375%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002392193713_p4399902485"><a name="zh-cn_topic_0000002392193713_p4399902485"></a><a name="zh-cn_topic_0000002392193713_p4399902485"></a>示例</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002392193713_row23995014487"><td class="cellrowborder" valign="top" width="33.333333333333336%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002392193713_p7480132115477"><a name="zh-cn_topic_0000002392193713_p7480132115477"></a><a name="zh-cn_topic_0000002392193713_p7480132115477"></a>+IMSSWITCH=[&lt;n&gt;]</p>
</td>
<td class="cellrowborder" valign="top" width="33.29332933293329%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002392193713_p851635232"><a name="zh-cn_topic_0000002392193713_p851635232"></a><a name="zh-cn_topic_0000002392193713_p851635232"></a>OK</p>
<p id="zh-cn_topic_0000002392193713_p1544139834"><a name="zh-cn_topic_0000002392193713_p1544139834"></a><a name="zh-cn_topic_0000002392193713_p1544139834"></a>错误情况：</p>
<p id="zh-cn_topic_0000002392193713_p1779mcpsimp"><a name="zh-cn_topic_0000002392193713_p1779mcpsimp"></a><a name="zh-cn_topic_0000002392193713_p1779mcpsimp"></a>+CME ERROR: &lt;err&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="33.373337333733375%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002392193713_p2634186145019"><a name="zh-cn_topic_0000002392193713_p2634186145019"></a><a name="zh-cn_topic_0000002392193713_p2634186145019"></a>AT+IMSSWITCH=1</p>
<p id="zh-cn_topic_0000002392193713_p1782mcpsimp"><a name="zh-cn_topic_0000002392193713_p1782mcpsimp"></a><a name="zh-cn_topic_0000002392193713_p1782mcpsimp"></a>OK</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002392193713_row1531755165811"><td class="cellrowborder" valign="top" width="33.333333333333336%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002392193713_p13178515588"><a name="zh-cn_topic_0000002392193713_p13178515588"></a><a name="zh-cn_topic_0000002392193713_p13178515588"></a>+IMSSWITCH=？</p>
</td>
<td class="cellrowborder" valign="top" width="33.29332933293329%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002392193713_p1799mcpsimp"><a name="zh-cn_topic_0000002392193713_p1799mcpsimp"></a><a name="zh-cn_topic_0000002392193713_p1799mcpsimp"></a>+IMSSWITCH: (list of supported &lt;n&gt;s)</p>
<p id="zh-cn_topic_0000002392193713_p13469333104211"><a name="zh-cn_topic_0000002392193713_p13469333104211"></a><a name="zh-cn_topic_0000002392193713_p13469333104211"></a>OK</p>
</td>
<td class="cellrowborder" valign="top" width="33.373337333733375%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002392193713_p1801mcpsimp"><a name="zh-cn_topic_0000002392193713_p1801mcpsimp"></a><a name="zh-cn_topic_0000002392193713_p1801mcpsimp"></a>AT+IMSSWITCH=?</p>
<p id="zh-cn_topic_0000002392193713_p611034125011"><a name="zh-cn_topic_0000002392193713_p611034125011"></a><a name="zh-cn_topic_0000002392193713_p611034125011"></a>+IMSSWITCH: (0-1)</p>
<p id="zh-cn_topic_0000002392193713_p1802mcpsimp"><a name="zh-cn_topic_0000002392193713_p1802mcpsimp"></a><a name="zh-cn_topic_0000002392193713_p1802mcpsimp"></a>OK</p>
</td>
</tr>
</tbody>
</table>

**参数描述<a name="zh-cn_topic_0000002392193713_section45087272918"></a>**

<a name="zh-cn_topic_0000002392193713_table198852334716"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002392193713_row1881723154712"><th class="cellrowborder" valign="top" width="19.71%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002392193713_p78862364716"><a name="zh-cn_topic_0000002392193713_p78862364716"></a><a name="zh-cn_topic_0000002392193713_p78862364716"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="80.28999999999999%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002392193713_p4888231472"><a name="zh-cn_topic_0000002392193713_p4888231472"></a><a name="zh-cn_topic_0000002392193713_p4888231472"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002392193713_row488023104710"><td class="cellrowborder" valign="top" width="19.71%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002392193713_p1994110212518"><a name="zh-cn_topic_0000002392193713_p1994110212518"></a><a name="zh-cn_topic_0000002392193713_p1994110212518"></a>&lt;n&gt;</p>
</td>
<td class="cellrowborder" valign="top" width="80.28999999999999%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002392193713_p12966103353019"><a name="zh-cn_topic_0000002392193713_p12966103353019"></a><a name="zh-cn_topic_0000002392193713_p12966103353019"></a>整型值，默认值为1。</p>
<p id="zh-cn_topic_0000002392193713_p896633333010"><a name="zh-cn_topic_0000002392193713_p896633333010"></a><a name="zh-cn_topic_0000002392193713_p896633333010"></a>0：关闭IMS协议栈；</p>
<p id="zh-cn_topic_0000002392193713_p89661336304"><a name="zh-cn_topic_0000002392193713_p89661336304"></a><a name="zh-cn_topic_0000002392193713_p89661336304"></a>1：打开IMS协议栈。</p>
</td>
</tr>
</tbody>
</table>

## VoLTE相关NV<a name="ZH-CN_TOPIC_0000002223508088"></a>

VoLTE支持的NV配置，均存在Hi2131E上，可以通过Hi2131E的DebugKits工具对这些NV进行配置修改，建议VoLTE相关的NV全部保持默认值配置。

**定义路径<a name="zh-cn_topic_0000002146510138_section991840193819"></a>**

-   公共配置文件：middleware/chips/cat1/nv/nv\_config/acore/cfg/acore/common.json
-   Hi2131E独有配置文件：middleware/chips/cat1/nv/nv\_config/acore/cfg/acore/acore\_ims.json

-   客户自定义配置文件：middleware/chips/cat1/nv/nv\_config/acore/cfg/acore/customer.json

**定义约束<a name="zh-cn_topic_0000002146510138_section188271020143813"></a>**

-   用户新定义的module\_id应位于1～255之间。

-   用户新定义的module\_id应按顺序编号，避免重复。

**模块说明<a name="zh-cn_topic_0000002146510138_section88522043194615"></a>**

<a name="zh-cn_topic_0000002146510138_table589183419304"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row13891103433012"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p13891153412303"><a name="zh-cn_topic_0000002146510138_p13891153412303"></a><a name="zh-cn_topic_0000002146510138_p13891153412303"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002146510138_p78911347301"><a name="zh-cn_topic_0000002146510138_p78911347301"></a><a name="zh-cn_topic_0000002146510138_p78911347301"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row1789119348309"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p128911934133013"><a name="zh-cn_topic_0000002146510138_p128911934133013"></a><a name="zh-cn_topic_0000002146510138_p128911934133013"></a>模块 ID</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002146510138_p489173411303"><a name="zh-cn_topic_0000002146510138_p489173411303"></a><a name="zh-cn_topic_0000002146510138_p489173411303"></a>31</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row6785175843414"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p18624224123520"><a name="zh-cn_topic_0000002146510138_p18624224123520"></a><a name="zh-cn_topic_0000002146510138_p18624224123520"></a>模块名</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002146510138_p11786135833419"><a name="zh-cn_topic_0000002146510138_p11786135833419"></a><a name="zh-cn_topic_0000002146510138_p11786135833419"></a>IMSA</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row620110313356"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p12201931357"><a name="zh-cn_topic_0000002146510138_p12201931357"></a><a name="zh-cn_topic_0000002146510138_p12201931357"></a>模块描述</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002146510138_p122018373512"><a name="zh-cn_topic_0000002146510138_p122018373512"></a><a name="zh-cn_topic_0000002146510138_p122018373512"></a>IMS适配层</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row2891113416309"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p178911134173019"><a name="zh-cn_topic_0000002146510138_p178911134173019"></a><a name="zh-cn_topic_0000002146510138_p178911134173019"></a>模块类型</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002146510138_p85221648290"><a name="zh-cn_topic_0000002146510138_p85221648290"></a><a name="zh-cn_topic_0000002146510138_p85221648290"></a>Custom</p>
</td>
</tr>
</tbody>
</table>

**模块Key<a name="zh-cn_topic_0000002146510138_section0618183514815"></a>**

<a name="zh-cn_topic_0000002146510138_table1888451015510"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row17885710105517"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p158132515719"><a name="zh-cn_topic_0000002146510138_p158132515719"></a><a name="zh-cn_topic_0000002146510138_p158132515719"></a>IMS_SIP_CONFIG</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row1688551019557"><td class="cellrowborder" valign="top" width="26.38%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p3885410155510"><a name="zh-cn_topic_0000002146510138_p3885410155510"></a><a name="zh-cn_topic_0000002146510138_p3885410155510"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.61999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1388581019557"><a name="zh-cn_topic_0000002146510138_p1388581019557"></a><a name="zh-cn_topic_0000002146510138_p1388581019557"></a>0x5001</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row20885191025513"><td class="cellrowborder" valign="top" width="26.38%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p4885171019551"><a name="zh-cn_topic_0000002146510138_p4885171019551"></a><a name="zh-cn_topic_0000002146510138_p4885171019551"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.61999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p488520102559"><a name="zh-cn_topic_0000002146510138_p488520102559"></a><a name="zh-cn_topic_0000002146510138_p488520102559"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row2885131015514"><td class="cellrowborder" valign="top" width="26.38%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p988531005511"><a name="zh-cn_topic_0000002146510138_p988531005511"></a><a name="zh-cn_topic_0000002146510138_p988531005511"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.61999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1188531075513"><a name="zh-cn_topic_0000002146510138_p1188531075513"></a><a name="zh-cn_topic_0000002146510138_p1188531075513"></a>IMS NV配置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1188541017557"><td class="cellrowborder" valign="top" width="26.38%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p7885141055511"><a name="zh-cn_topic_0000002146510138_p7885141055511"></a><a name="zh-cn_topic_0000002146510138_p7885141055511"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.61999999999999%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock19269277166"><a name="zh-cn_topic_0000002146510138_codeblock19269277166"></a><a name="zh-cn_topic_0000002146510138_codeblock19269277166"></a>#define IMS_PARM_STRING_SIZE  129
#define NUM_TWO_SIZE          2
typedef enum {
    IMS_PARM_URI_FORMAT_TYPE_DEFAULT = 0,        /* default */
    IMS_PARM_URI_FORMAT_TYPE_SIP,                   /* sip */
    IMS_PARM_URI_FORMAT_TYPE_TEL,                   /* tel */
    IMS_PARM_URI_BUTT = 0xFF,
} ims_parm_uri_format_type;
typedef enum {
    IMS_PARM_TRANSPORT_TYPE_UDP = 0,
    IMS_PARM_TRANSPORT_TYPE_TCP,
    IMS_PARM_TRANSPORT_TYPE_BUTT = 0xFF,
} ims_parm_transport_type;
typedef struct {
    ims_parm_transport_type transport_type;
    ims_parm_uri_format_type uri_format;
    uint8_t                 header_compact_enabled;
    uint8_t                 forking_flag;
    uint32_t                server_port;
    uint32_t                timer_sip_reg_value;
    uint32_t                subs_reg_expires;
    uint32_t                rsv1;
    uint32_t                rsv2;
    uint32_t                rsv3;
    uint32_t                rsv4;
    uint32_t                sip_socket_keep_alive_value;
    int8_t                  user_agent[IMS_PARM_STRING_SIZE];
    int8_t                  timer_reg;
    int8_t                  sms_format_timer_dereg[NUM_TWO_SIZE];
    uint32_t                tcp_threshold;
} ims_parm_sip;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1598712391795"><td class="cellrowborder" valign="top" width="26.38%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p13987239593"><a name="zh-cn_topic_0000002146510138_p13987239593"></a><a name="zh-cn_topic_0000002146510138_p13987239593"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.61999999999999%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock14421183114477"><a name="zh-cn_topic_0000002146510138_codeblock14421183114477"></a><a name="zh-cn_topic_0000002146510138_codeblock14421183114477"></a>transport_type					=IMS_PARM_TRANSPORT_TYPE_UDP
uri_format					=IMS_PARM_URI_FORMAT_TYPE_DEFAULT
header_compact_enabled				=0
forking_flag					=0(不支持修改)
server_port					=5060
timer_sip_reg_value				=600000
subs_reg_expires				=2000
rsv1						=16000（不支持修改）
rsv2						=17000（不支持修改）
rsv3						=128000（不支持修改）
rsv4						=128000（不支持修改）
sip_socket_keep_alive_value			=0（不支持修改）
user_agent[IMS_PARM_STRING_SIZE]		={0}（不支持修改）
timer_reg					=0（不支持修改）
sms_format_timer_dereg[NUM_TWO_SIZE]		={2,0}（不支持修改）
tcp_threshold					=16500</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row13240163555916"><td class="cellrowborder" valign="top" width="26.38%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p124113359596"><a name="zh-cn_topic_0000002146510138_p124113359596"></a><a name="zh-cn_topic_0000002146510138_p124113359596"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.61999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p964520117265"><a name="zh-cn_topic_0000002146510138_p964520117265"></a><a name="zh-cn_topic_0000002146510138_p964520117265"></a>-</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table22251817182417"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row92251017162413"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p3225151719249"><a name="zh-cn_topic_0000002146510138_p3225151719249"></a><a name="zh-cn_topic_0000002146510138_p3225151719249"></a>IMS_PARM_VOIP</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row32251717102415"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p13225131719245"><a name="zh-cn_topic_0000002146510138_p13225131719245"></a><a name="zh-cn_topic_0000002146510138_p13225131719245"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p132255175249"><a name="zh-cn_topic_0000002146510138_p132255175249"></a><a name="zh-cn_topic_0000002146510138_p132255175249"></a>0x5002</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1822571722419"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p422561772416"><a name="zh-cn_topic_0000002146510138_p422561772416"></a><a name="zh-cn_topic_0000002146510138_p422561772416"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p13225617142417"><a name="zh-cn_topic_0000002146510138_p13225617142417"></a><a name="zh-cn_topic_0000002146510138_p13225617142417"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row152251317162420"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p92251117202415"><a name="zh-cn_topic_0000002146510138_p92251117202415"></a><a name="zh-cn_topic_0000002146510138_p92251117202415"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1422591752419"><a name="zh-cn_topic_0000002146510138_p1422591752419"></a><a name="zh-cn_topic_0000002146510138_p1422591752419"></a>IMS VoIP配置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row522561715244"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p8225151716247"><a name="zh-cn_topic_0000002146510138_p8225151716247"></a><a name="zh-cn_topic_0000002146510138_p8225151716247"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock6498123620278"><a name="zh-cn_topic_0000002146510138_codeblock6498123620278"></a><a name="zh-cn_topic_0000002146510138_codeblock6498123620278"></a>typedef enum {
    IMS_PARM_SESSION_REFRESH_TYPE_UAC = 0x00,        /* Client Refresh */
    IMS_PARM_SESSION_REFRESH_TYPE_UAS,               /* Server Refresh */
    IMS_PARM_SESSION_REFRESH_TYPE_BUTT = 0xFF,
} ims_parm_session_refresh_type;
typedef enum {
    IMS_PARM_SESSION_REFRESH_METHOD_UPDATE = 0x00,         /* Update */
    IMS_PARM_SESSION_REFRESH_METHOD_REINVITE,              /* Reinvite */
    IMS_PARM_SESSION_REFRESH_METHOD_BUTT = 0xFF,
} ims_parm_session_refresh_method;
typedef struct {
    uint8_t                             session_timer_enabled;
    uint8_t                             pre_condition_enable;
    ims_parm_session_refresh_type       session_refresh_type;
    ims_parm_session_refresh_method     session_refresh_method;
    uint16_t                            session_expires;
    uint8_t                             rtp_inactive_timer[2];
} ims_parm_voip;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row178481414384"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p178484411381"><a name="zh-cn_topic_0000002146510138_p178484411381"></a><a name="zh-cn_topic_0000002146510138_p178484411381"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock314181420116"><a name="zh-cn_topic_0000002146510138_codeblock314181420116"></a><a name="zh-cn_topic_0000002146510138_codeblock314181420116"></a>session_timer_enabled			=1（不支持修改）
pre_condition_enable			=1
session_refresh_type			=IMS_PARM_SESSION_REFRESH_TYPE_UAC（不支持修改）
session_refresh_method			=IMS_PARM_SESSION_REFRESH_METHOD_UPDATE（不支持修改）
session_expires				=1800（不支持修改）
rtp_inactive_timer[2]			={10,1}（不支持修改）</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1822501752418"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p82251117172414"><a name="zh-cn_topic_0000002146510138_p82251117172414"></a><a name="zh-cn_topic_0000002146510138_p82251117172414"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p129627402386"><a name="zh-cn_topic_0000002146510138_p129627402386"></a><a name="zh-cn_topic_0000002146510138_p129627402386"></a>-</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table18413246153811"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row1441313467386"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p3413134611386"><a name="zh-cn_topic_0000002146510138_p3413134611386"></a><a name="zh-cn_topic_0000002146510138_p3413134611386"></a>IMS_PARM_VOIP_CODE</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row13413134663811"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p9413134618381"><a name="zh-cn_topic_0000002146510138_p9413134618381"></a><a name="zh-cn_topic_0000002146510138_p9413134618381"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p5413846143812"><a name="zh-cn_topic_0000002146510138_p5413846143812"></a><a name="zh-cn_topic_0000002146510138_p5413846143812"></a>0x5003</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1841364614387"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p741394619387"><a name="zh-cn_topic_0000002146510138_p741394619387"></a><a name="zh-cn_topic_0000002146510138_p741394619387"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1413194603815"><a name="zh-cn_topic_0000002146510138_p1413194603815"></a><a name="zh-cn_topic_0000002146510138_p1413194603815"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row44131046203813"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p741474613380"><a name="zh-cn_topic_0000002146510138_p741474613380"></a><a name="zh-cn_topic_0000002146510138_p741474613380"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p9414124693810"><a name="zh-cn_topic_0000002146510138_p9414124693810"></a><a name="zh-cn_topic_0000002146510138_p9414124693810"></a>IMS VoIP语音编码配置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1414114620383"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p041464643811"><a name="zh-cn_topic_0000002146510138_p041464643811"></a><a name="zh-cn_topic_0000002146510138_p041464643811"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock61932383396"><a name="zh-cn_topic_0000002146510138_codeblock61932383396"></a><a name="zh-cn_topic_0000002146510138_codeblock61932383396"></a>typedef enum {
    IMS_PARM_AUDIO_CAP_AMR_NB_AND_WB = 0x00,        /* AMR-NB and AMR-WB (AMR-NB preferred) */
    IMS_PARM_AUDIO_CAP_AMR_WB_AND_NB,               /* AMR-WB and AMR-NB (AMR-WB preferred) */
    IMS_PARM_AUDIO_CAP_AMR_NB,                      /* AMR-NB */
    IMS_PARM_AUDIO_CAP_AMR_WB,                      /* AMR-WB */
    IMS_PARM_AUDIO_CAP_EVS_AND_AMR_NB_AND_WB,       /* AMR-NB and AMR-WB and EVS (EVS preferred) */
    IMS_PARM_AUDIO_CAP_EVS = 5,
    IMS_PARM_AUDIO_CAP_TYPE_BUTT
} ims_parm_audio_cap;
typedef enum {
    IMS_PARM_AUDIO_CODE_MODE_OCTECT_ALIGN = 0x00,       /* Octet-aligned Mode */
    IMS_PARM_AUDIO_CODE_MODE_BAND_EFFICIENT,            /* Bandwidth-Efficient Mode */
    IMS_PARM_AUDIO_CODE_MODE_EF_AND_OC,                 /* bandwidth-efficient and octet-aligned Mode */
    IMS_PARM_AUDIO_CODE_MODE_OC_AND_EF,                 /* octet-aligned and bandwidth-efficient Mode */
    IMS_PARM_AUDIO_CODE_TYPE_BUTT
} ims_parm_audio_code_mode;
typedef struct {
    ims_parm_audio_cap                  comm_audio_cap;
    uint8_t                             set_nb_amr_mode_flag;
    uint8_t                             set_wb_amr_mode_flag;
    ims_parm_audio_cap                  nr_audio_cap;
    uint32_t                            nb_amr_mode_set;
    uint32_t                            wb_amr_mode_set;
    ims_parm_audio_code_mode            nb_amr_audio_code_mode;
    ims_parm_audio_code_mode            wb_amr_audio_code_mode;
    uint16_t                            ptimer_value;
    uint8_t                             video_resolution; // 以下参数为保持和IMS一致
    uint8_t                             h264_level;
    uint8_t                             video_fps;
    uint8_t                             cvo_flag;
    uint32_t                            video_bitrate;
} ims_parm_voip_code;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row13414204613386"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p2414124617382"><a name="zh-cn_topic_0000002146510138_p2414124617382"></a><a name="zh-cn_topic_0000002146510138_p2414124617382"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock77175332218"><a name="zh-cn_topic_0000002146510138_codeblock77175332218"></a><a name="zh-cn_topic_0000002146510138_codeblock77175332218"></a>comm_audio_cap			=IMS_PARM_AUDIO_CAP_AMR_WB_AND_NB
set_nb_amr_mode_flag		=0
set_wb_amr_mode_flag		=0
nr_audio_cap			=IMS_PARM_AUDIO_CAP_AMR_NB_AND_WB
nb_amr_mode_set			=0
wb_amr_mode_set			=0
nb_amr_audio_code_mode		=IMS_PARM_AUDIO_CODE_MODE_EF_AND_OC
wb_amr_audio_code_mode		=IMS_PARM_AUDIO_CODE_MODE_EF_AND_OC
ptimer_value		        =20
video_resolution		=0
h264_level			=1
video_fps			=30
cvo_flag			=0
video_bitrate			=640000</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row541414613382"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p341424623816"><a name="zh-cn_topic_0000002146510138_p341424623816"></a><a name="zh-cn_topic_0000002146510138_p341424623816"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p8414184643816"><a name="zh-cn_topic_0000002146510138_p8414184643816"></a><a name="zh-cn_topic_0000002146510138_p8414184643816"></a>不支持修改。</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table4750133734717"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row275063713471"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p975153744711"><a name="zh-cn_topic_0000002146510138_p975153744711"></a><a name="zh-cn_topic_0000002146510138_p975153744711"></a>IMS_PARM_SS_CONF</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row1275123764715"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p975123764710"><a name="zh-cn_topic_0000002146510138_p975123764710"></a><a name="zh-cn_topic_0000002146510138_p975123764710"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1875113711475"><a name="zh-cn_topic_0000002146510138_p1875113711475"></a><a name="zh-cn_topic_0000002146510138_p1875113711475"></a>0x5004</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row5751937104712"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p15751137134711"><a name="zh-cn_topic_0000002146510138_p15751137134711"></a><a name="zh-cn_topic_0000002146510138_p15751137134711"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p14751103734715"><a name="zh-cn_topic_0000002146510138_p14751103734715"></a><a name="zh-cn_topic_0000002146510138_p14751103734715"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row207511937204710"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p8751163764718"><a name="zh-cn_topic_0000002146510138_p8751163764718"></a><a name="zh-cn_topic_0000002146510138_p8751163764718"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p77518372477"><a name="zh-cn_topic_0000002146510138_p77518372477"></a><a name="zh-cn_topic_0000002146510138_p77518372477"></a>IMS VoIP语音编码配置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row77515373474"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p4751113712479"><a name="zh-cn_topic_0000002146510138_p4751113712479"></a><a name="zh-cn_topic_0000002146510138_p4751113712479"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock14758171917507"><a name="zh-cn_topic_0000002146510138_codeblock14758171917507"></a><a name="zh-cn_topic_0000002146510138_codeblock14758171917507"></a>#define IMS_PARM_STRING_SIZE 129
typedef struct {
    uint8_t     conf_factory_uri[IMS_PARM_STRING_SIZE];
    uint8_t     reject_with_486[3];
} ims_parm_ss_conf;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1751173724711"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p13751737154718"><a name="zh-cn_topic_0000002146510138_p13751737154718"></a><a name="zh-cn_topic_0000002146510138_p13751737154718"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock12551194520319"><a name="zh-cn_topic_0000002146510138_codeblock12551194520319"></a><a name="zh-cn_topic_0000002146510138_codeblock12551194520319"></a>conf_factory_uri                       ={0}
reject_with_486                        ={0}</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row13751133764715"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p187511337144718"><a name="zh-cn_topic_0000002146510138_p187511337144718"></a><a name="zh-cn_topic_0000002146510138_p187511337144718"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p137511378470"><a name="zh-cn_topic_0000002146510138_p137511378470"></a><a name="zh-cn_topic_0000002146510138_p137511378470"></a>不支持修改。</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table1526273545310"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row1926283518538"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p132623358532"><a name="zh-cn_topic_0000002146510138_p132623358532"></a><a name="zh-cn_topic_0000002146510138_p132623358532"></a>IMS_PARM_SS_SECURITY</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row19262143510532"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p192621035125316"><a name="zh-cn_topic_0000002146510138_p192621035125316"></a><a name="zh-cn_topic_0000002146510138_p192621035125316"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p32624358537"><a name="zh-cn_topic_0000002146510138_p32624358537"></a><a name="zh-cn_topic_0000002146510138_p32624358537"></a>0x5005</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1326283575320"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1626263505312"><a name="zh-cn_topic_0000002146510138_p1626263505312"></a><a name="zh-cn_topic_0000002146510138_p1626263505312"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p626293510534"><a name="zh-cn_topic_0000002146510138_p626293510534"></a><a name="zh-cn_topic_0000002146510138_p626293510534"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row7262635155315"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p22621035195315"><a name="zh-cn_topic_0000002146510138_p22621035195315"></a><a name="zh-cn_topic_0000002146510138_p22621035195315"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p202621235185316"><a name="zh-cn_topic_0000002146510138_p202621235185316"></a><a name="zh-cn_topic_0000002146510138_p202621235185316"></a>IMS IPSEC配置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row826210356532"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p112621635125310"><a name="zh-cn_topic_0000002146510138_p112621635125310"></a><a name="zh-cn_topic_0000002146510138_p112621635125310"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock176013484558"><a name="zh-cn_topic_0000002146510138_codeblock176013484558"></a><a name="zh-cn_topic_0000002146510138_codeblock176013484558"></a>typedef struct {
    uint8_t     null;
    uint8_t     aes;
    uint8_t     des;（不支持修改）
    uint8_t     rsv[1];
} ealgo_capability;
typedef struct {
    uint8_t     md5;（不支持修改）
    uint8_t     sha;
    uint8_t     rsv[2];
} algo_capability;
typedef struct {
    uint8_t                     srtp_flag;（不支持修改）
    uint8_t                     sock_support_linger[3];（不支持修改）
    ealgo_capability            ealgo;
    algo_capability             algo;
} ims_parm_security;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row4263153525317"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p9263035135318"><a name="zh-cn_topic_0000002146510138_p9263035135318"></a><a name="zh-cn_topic_0000002146510138_p9263035135318"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock6322334413"><a name="zh-cn_topic_0000002146510138_codeblock6322334413"></a><a name="zh-cn_topic_0000002146510138_codeblock6322334413"></a>srtp_flag					=0
sock_support_linger[3]				={2,0,0}
ealgo						={1,0,0,{0}}
algo						={0,1,{0,0}}</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row2263143518535"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p102632351539"><a name="zh-cn_topic_0000002146510138_p102632351539"></a><a name="zh-cn_topic_0000002146510138_p102632351539"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1226383545313"><a name="zh-cn_topic_0000002146510138_p1226383545313"></a><a name="zh-cn_topic_0000002146510138_p1226383545313"></a>-</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table51116911579"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row211210925718"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p111218975717"><a name="zh-cn_topic_0000002146510138_p111218975717"></a><a name="zh-cn_topic_0000002146510138_p111218975717"></a>IMS_PARM_MEDIA</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row3112179195719"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p101121915573"><a name="zh-cn_topic_0000002146510138_p101121915573"></a><a name="zh-cn_topic_0000002146510138_p101121915573"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p13112199185717"><a name="zh-cn_topic_0000002146510138_p13112199185717"></a><a name="zh-cn_topic_0000002146510138_p13112199185717"></a>0x5006</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row181121197575"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p711269155711"><a name="zh-cn_topic_0000002146510138_p711269155711"></a><a name="zh-cn_topic_0000002146510138_p711269155711"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p181129925710"><a name="zh-cn_topic_0000002146510138_p181129925710"></a><a name="zh-cn_topic_0000002146510138_p181129925710"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1211219911575"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p141121498577"><a name="zh-cn_topic_0000002146510138_p141121498577"></a><a name="zh-cn_topic_0000002146510138_p141121498577"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p7112793574"><a name="zh-cn_topic_0000002146510138_p7112793574"></a><a name="zh-cn_topic_0000002146510138_p7112793574"></a>IMS媒体配置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row81121985717"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p151121590571"><a name="zh-cn_topic_0000002146510138_p151121590571"></a><a name="zh-cn_topic_0000002146510138_p151121590571"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock81124955713"><a name="zh-cn_topic_0000002146510138_codeblock81124955713"></a><a name="zh-cn_topic_0000002146510138_codeblock81124955713"></a>typedef struct {
    uint8_t        dtx_enable;
    uint8_t        rsv[3];
} ims_parm_media;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row2112297572"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p121127918578"><a name="zh-cn_topic_0000002146510138_p121127918578"></a><a name="zh-cn_topic_0000002146510138_p121127918578"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock46511558420"><a name="zh-cn_topic_0000002146510138_codeblock46511558420"></a><a name="zh-cn_topic_0000002146510138_codeblock46511558420"></a>dtx_enable                   = 1
rsv[3]                       ={0}</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row711218995712"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1112490579"><a name="zh-cn_topic_0000002146510138_p1112490579"></a><a name="zh-cn_topic_0000002146510138_p1112490579"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p211219975712"><a name="zh-cn_topic_0000002146510138_p211219975712"></a><a name="zh-cn_topic_0000002146510138_p211219975712"></a>不支持修改。</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table115497182591"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row654961813591"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p95499187595"><a name="zh-cn_topic_0000002146510138_p95499187595"></a><a name="zh-cn_topic_0000002146510138_p95499187595"></a>IMS_PARM_UE_CAPABILITY</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row255071817591"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1755081818591"><a name="zh-cn_topic_0000002146510138_p1755081818591"></a><a name="zh-cn_topic_0000002146510138_p1755081818591"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p10550131814591"><a name="zh-cn_topic_0000002146510138_p10550131814591"></a><a name="zh-cn_topic_0000002146510138_p10550131814591"></a>0x5007</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row9550121812598"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p55501718165911"><a name="zh-cn_topic_0000002146510138_p55501718165911"></a><a name="zh-cn_topic_0000002146510138_p55501718165911"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p55502183591"><a name="zh-cn_topic_0000002146510138_p55502183591"></a><a name="zh-cn_topic_0000002146510138_p55502183591"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row85501618185911"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p5550111805920"><a name="zh-cn_topic_0000002146510138_p5550111805920"></a><a name="zh-cn_topic_0000002146510138_p5550111805920"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1655031885917"><a name="zh-cn_topic_0000002146510138_p1655031885917"></a><a name="zh-cn_topic_0000002146510138_p1655031885917"></a>IMS UE能力配置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1955010187591"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p05501118115915"><a name="zh-cn_topic_0000002146510138_p05501118115915"></a><a name="zh-cn_topic_0000002146510138_p05501118115915"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock1555011818594"><a name="zh-cn_topic_0000002146510138_codeblock1555011818594"></a><a name="zh-cn_topic_0000002146510138_codeblock1555011818594"></a>typedef struct {
    uint8_t     reserved0;
    uint8_t     reserved1;
    uint8_t     reserved2;
    uint8_t     reserved3;
    uint8_t     reserved4;
    uint8_t     reserved5;
    uint8_t     reserved6;
    uint8_t     reserved7;
    uint8_t     reserved8;
    uint8_t     reserved9;
    uint8_t     reserved10;
    uint8_t     reserved11;
    uint8_t     reserved12;
    uint8_t     reserved13;
    uint8_t     reserved14;
    uint8_t     reserved15;
} uint8_nv_stru;
typedef struct {
    uint16_t     reserved0;
    uint16_t     reserved1;
    uint16_t     reserved2;
    uint16_t     reserved3;
    uint16_t     reserved4;
    uint16_t     reserved5;
    uint16_t     reserved6;
    uint16_t     reserved7;
} uint16_nv_stru;
typedef struct {
    uint32_t     reserved0;
    uint32_t     reserved1;
    uint32_t     reserved2;
    uint32_t     reserved3;
    uint32_t     reserved4;
    uint32_t     reserved5;
    uint32_t     reserved6;
    uint32_t     reserved7;
} uint32_nv_stru;
typedef struct {
    uint8_t                 rsp_sip_timeout;
    uint8_t                 image_attr_flag;
    uint8_t                 att_feature_flag;
    uint8_t                 vdf_switch;
    uint8_t                 keep_alive_flag;
    uint8_t                 ipsec_flag;
    uint8_t                 rsv[2];
    uint8_nv_stru           uint8_nv_para[16];
    uint16_nv_stru          uint16_nv_para[16];
    uint32_nv_stru          uint32_nv_para[8];
} ims_parm_ue_capability;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row9550218105915"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p3550218195915"><a name="zh-cn_topic_0000002146510138_p3550218195915"></a><a name="zh-cn_topic_0000002146510138_p3550218195915"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock59892013510"><a name="zh-cn_topic_0000002146510138_codeblock59892013510"></a><a name="zh-cn_topic_0000002146510138_codeblock59892013510"></a>rsp_sip_timeout				=0
image_attr_flag				=0
att_feature_flag			=0
vdf_switch				=0
keep_alive_flag				=1
ipsec_flag				=0
rsv[2]					={0}
uint8_nv_para[16]
={{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}}
uint16_nv_para[16]			 ={0}
uint32_nv_para[8]			 ={{0, 0, 0, 0, 64, 536870912, 0, 0}, {0}, {0}, {0}, }, {0}, {0}, {0}}</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1355081812593"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p75500185593"><a name="zh-cn_topic_0000002146510138_p75500185593"></a><a name="zh-cn_topic_0000002146510138_p75500185593"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p13550121815592"><a name="zh-cn_topic_0000002146510138_p13550121815592"></a><a name="zh-cn_topic_0000002146510138_p13550121815592"></a>仅支持ipsec_flag。</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table87582289520"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row1375819281257"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p475917287517"><a name="zh-cn_topic_0000002146510138_p475917287517"></a><a name="zh-cn_topic_0000002146510138_p475917287517"></a>IMS_PARM_SIP_TIMER</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row17759828959"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p2075920281954"><a name="zh-cn_topic_0000002146510138_p2075920281954"></a><a name="zh-cn_topic_0000002146510138_p2075920281954"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1075912820510"><a name="zh-cn_topic_0000002146510138_p1075912820510"></a><a name="zh-cn_topic_0000002146510138_p1075912820510"></a>0x5008</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row27592028853"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p375918281159"><a name="zh-cn_topic_0000002146510138_p375918281159"></a><a name="zh-cn_topic_0000002146510138_p375918281159"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1775912817515"><a name="zh-cn_topic_0000002146510138_p1775912817515"></a><a name="zh-cn_topic_0000002146510138_p1775912817515"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row275916281454"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p157594281559"><a name="zh-cn_topic_0000002146510138_p157594281559"></a><a name="zh-cn_topic_0000002146510138_p157594281559"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p175992816512"><a name="zh-cn_topic_0000002146510138_p175992816512"></a><a name="zh-cn_topic_0000002146510138_p175992816512"></a>IMS SIP定时器配置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row975916283517"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p77591281754"><a name="zh-cn_topic_0000002146510138_p77591281754"></a><a name="zh-cn_topic_0000002146510138_p77591281754"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock8759162810516"><a name="zh-cn_topic_0000002146510138_codeblock8759162810516"></a><a name="zh-cn_topic_0000002146510138_codeblock8759162810516"></a>typedef struct {
    uint32_t    timer_t1_value;
    uint32_t    timer_t2_value;
    uint32_t    timer_t4_value;
    uint32_t    timer_ta_value;
    uint32_t    timer_tb_value;
    uint32_t    timer_tc_value;
    uint32_t    timer_td_value;
    uint32_t    timer_te_value;
    uint32_t    timer_tf_value;
    uint32_t    timer_tg_value;
    uint32_t    timer_th_value;
    uint32_t    timer_ti_value;
    uint32_t    timer_tj_value;
    uint32_t    timer_tk_value;
} ims_parm_sip_timer;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row4760128557"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p376062816511"><a name="zh-cn_topic_0000002146510138_p376062816511"></a><a name="zh-cn_topic_0000002146510138_p376062816511"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock14617218262"><a name="zh-cn_topic_0000002146510138_codeblock14617218262"></a><a name="zh-cn_topic_0000002146510138_codeblock14617218262"></a>timer_t1_value	=2000
timer_t2_value	=16000
timer_t4_value	=17000
timer_ta_value	=2000
timer_tb_value	=128000
timer_tc_value	=200000
timer_td_value	=200000
timer_te_value	=2000
timer_tf_value	=128000
timer_tg_value	=2000
timer_th_value	=128000
timer_ti_value	=17000
timer_tj_value	=128000
timer_tk_value	=128000</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row127601283516"><td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p10760128858"><a name="zh-cn_topic_0000002146510138_p10760128858"></a><a name="zh-cn_topic_0000002146510138_p10760128858"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.7%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1176014286511"><a name="zh-cn_topic_0000002146510138_p1176014286511"></a><a name="zh-cn_topic_0000002146510138_p1176014286511"></a>不支持修改。</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table1715412318166"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row7155723151611"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p315552319160"><a name="zh-cn_topic_0000002146510138_p315552319160"></a><a name="zh-cn_topic_0000002146510138_p315552319160"></a>IMS_NV_EMERGENCY_URN</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row1115511239169"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p12155123181617"><a name="zh-cn_topic_0000002146510138_p12155123181617"></a><a name="zh-cn_topic_0000002146510138_p12155123181617"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p8155152361613"><a name="zh-cn_topic_0000002146510138_p8155152361613"></a><a name="zh-cn_topic_0000002146510138_p8155152361613"></a>0x5009</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row8155152315169"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p315512321620"><a name="zh-cn_topic_0000002146510138_p315512321620"></a><a name="zh-cn_topic_0000002146510138_p315512321620"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p14155192317163"><a name="zh-cn_topic_0000002146510138_p14155192317163"></a><a name="zh-cn_topic_0000002146510138_p14155192317163"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row615513232166"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p4155723111616"><a name="zh-cn_topic_0000002146510138_p4155723111616"></a><a name="zh-cn_topic_0000002146510138_p4155723111616"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p13155723191618"><a name="zh-cn_topic_0000002146510138_p13155723191618"></a><a name="zh-cn_topic_0000002146510138_p13155723191618"></a>IMS紧急呼叫URN。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row915512351615"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1715572341615"><a name="zh-cn_topic_0000002146510138_p1715572341615"></a><a name="zh-cn_topic_0000002146510138_p1715572341615"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock15155923191618"><a name="zh-cn_topic_0000002146510138_codeblock15155923191618"></a><a name="zh-cn_topic_0000002146510138_codeblock15155923191618"></a>#define EMC_CATEGOTY_URN_SIZE 6
#define EMC_URN_SIZE 65
typedef struct {
    uint8_t     category;
    uint8_t     urn[EMC_URN_SIZE];
    uint8_t     rsv[2];
} ims_emc_category_urn;
typedef struct {
    ims_emc_category_urn    emc_cat_urn[EMC_CATEGOTY_URN_SIZE];
} ims_nv_emergency_urn;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1115582311618"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1315542311162"><a name="zh-cn_topic_0000002146510138_p1315542311162"></a><a name="zh-cn_topic_0000002146510138_p1315542311162"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p2064493811243"><a name="zh-cn_topic_0000002146510138_p2064493811243"></a><a name="zh-cn_topic_0000002146510138_p2064493811243"></a>{0}</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1215502311615"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1156202318163"><a name="zh-cn_topic_0000002146510138_p1156202318163"></a><a name="zh-cn_topic_0000002146510138_p1156202318163"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p4156122318160"><a name="zh-cn_topic_0000002146510138_p4156122318160"></a><a name="zh-cn_topic_0000002146510138_p4156122318160"></a>不支持修改。</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table070119509245"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row1870195092420"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p14701165012411"><a name="zh-cn_topic_0000002146510138_p14701165012411"></a><a name="zh-cn_topic_0000002146510138_p14701165012411"></a>IMS_PARM_STRING_SWITCH_VALUE</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row167028506248"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p17702165010249"><a name="zh-cn_topic_0000002146510138_p17702165010249"></a><a name="zh-cn_topic_0000002146510138_p17702165010249"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p0702165010241"><a name="zh-cn_topic_0000002146510138_p0702165010241"></a><a name="zh-cn_topic_0000002146510138_p0702165010241"></a>0x500A</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1670275015246"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p070235020245"><a name="zh-cn_topic_0000002146510138_p070235020245"></a><a name="zh-cn_topic_0000002146510138_p070235020245"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p170255052414"><a name="zh-cn_topic_0000002146510138_p170255052414"></a><a name="zh-cn_topic_0000002146510138_p170255052414"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1570205012411"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p7702155012241"><a name="zh-cn_topic_0000002146510138_p7702155012241"></a><a name="zh-cn_topic_0000002146510138_p7702155012241"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p570275042413"><a name="zh-cn_topic_0000002146510138_p570275042413"></a><a name="zh-cn_topic_0000002146510138_p570275042413"></a>IMS字符串配置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row3702115018248"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p127020509244"><a name="zh-cn_topic_0000002146510138_p127020509244"></a><a name="zh-cn_topic_0000002146510138_p127020509244"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock77024509245"><a name="zh-cn_topic_0000002146510138_codeblock77024509245"></a><a name="zh-cn_topic_0000002146510138_codeblock77024509245"></a>#define IMS_PARM_STRING_SIZE 129
typedef struct {
    char            phone_context[IMS_PARM_STRING_SIZE];
    char            phone_context_impu[IMS_PARM_STRING_SIZE];
    char            dislay_name[IMS_PARM_STRING_SIZE];
    uint8_t         rsv[1];
} ims_parm_string_switch_value;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row2702155014245"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p6702175015244"><a name="zh-cn_topic_0000002146510138_p6702175015244"></a><a name="zh-cn_topic_0000002146510138_p6702175015244"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p197021150132411"><a name="zh-cn_topic_0000002146510138_p197021150132411"></a><a name="zh-cn_topic_0000002146510138_p197021150132411"></a>{0}</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row770265052410"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p167021850152411"><a name="zh-cn_topic_0000002146510138_p167021850152411"></a><a name="zh-cn_topic_0000002146510138_p167021850152411"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p5702185072418"><a name="zh-cn_topic_0000002146510138_p5702185072418"></a><a name="zh-cn_topic_0000002146510138_p5702185072418"></a>暂未使用。</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table18186151202720"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row131861751192717"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p6186155110275"><a name="zh-cn_topic_0000002146510138_p6186155110275"></a><a name="zh-cn_topic_0000002146510138_p6186155110275"></a>IMSA_NV_IMS_CTRL</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row81861851122712"><td class="cellrowborder" valign="top" width="26.240000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p318620514273"><a name="zh-cn_topic_0000002146510138_p318620514273"></a><a name="zh-cn_topic_0000002146510138_p318620514273"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.76%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p201861051192719"><a name="zh-cn_topic_0000002146510138_p201861051192719"></a><a name="zh-cn_topic_0000002146510138_p201861051192719"></a>0x500B</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row9186115172717"><td class="cellrowborder" valign="top" width="26.240000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p718614518276"><a name="zh-cn_topic_0000002146510138_p718614518276"></a><a name="zh-cn_topic_0000002146510138_p718614518276"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.76%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p11186115118272"><a name="zh-cn_topic_0000002146510138_p11186115118272"></a><a name="zh-cn_topic_0000002146510138_p11186115118272"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row18186115118275"><td class="cellrowborder" valign="top" width="26.240000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p2018625114279"><a name="zh-cn_topic_0000002146510138_p2018625114279"></a><a name="zh-cn_topic_0000002146510138_p2018625114279"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.76%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p171861551132715"><a name="zh-cn_topic_0000002146510138_p171861551132715"></a><a name="zh-cn_topic_0000002146510138_p171861551132715"></a>IMSA参数配置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row018785115277"><td class="cellrowborder" valign="top" width="26.240000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1418718516270"><a name="zh-cn_topic_0000002146510138_p1418718516270"></a><a name="zh-cn_topic_0000002146510138_p1418718516270"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.76%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock15187135142718"><a name="zh-cn_topic_0000002146510138_codeblock15187135142718"></a><a name="zh-cn_topic_0000002146510138_codeblock15187135142718"></a>typedef struct {
    uint8_t                            single_cause_num;     // 临时被禁原因数量
    uint8_t                            single_cause_list[IMSA_NV_MAX_TEMP_PDN_SINGLE_CAUSE_NUM];     // 临时被禁列表
} imsa_nv_pdn_temp_rej_cause_list;
typedef struct {
    uint8_t                            single_cause_num;     // 永久被禁原因数量
    uint8_t                            single_cause_list[IMSA_NV_MAX_PERM_PDN_SINGLE_CAUSE_NUM];     // 永久被禁列表
} imsa_nv_pdn_perm_rej_cause_list;
typedef struct {
    uint8_t                             temp_forb_feature_flag;     // 临时被禁特性控制开关
    uint8_t                             perm_forb_feature_flag;     // 永久被禁特性控制开关
    uint8_t                             rsv[2];
    imsa_nv_pdn_temp_rej_cause_list     pdn_temp_rej_cause_list;    // PDN临时被拒原因值列表
    imsa_nv_pdn_perm_rej_cause_list     pdn_perm_rej_cause_list;    // pdn永久被拒原因值列表
} imsa_pdn_rej_forbidden_ctrl;
typedef struct {
    uint32_t    max_time;           // RFC5626计算重试时间时的maxtime，单位：s
    uint32_t    base_time;          // RFC5626计算重试时间时的basetime，单位：s
    uint32_t    retry_period;        // 注册失败后周期性重试时间间隔，单位：s
    uint32_t    save_retry_times;    // 保存NV配置的IMS注册失败重新尝试次数,0表示无限次尝试
   imsa_pdn_rej_forbidden_ctrl pdn_rej_forb_ctrl;  // 承载被拒控制模块
} imsa_nv_ims_ctrl;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row111873519274"><td class="cellrowborder" valign="top" width="26.240000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p14187155112272"><a name="zh-cn_topic_0000002146510138_p14187155112272"></a><a name="zh-cn_topic_0000002146510138_p14187155112272"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.76%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock263115431572"><a name="zh-cn_topic_0000002146510138_codeblock263115431572"></a><a name="zh-cn_topic_0000002146510138_codeblock263115431572"></a>max_time           			=1800
base_time          			=30
retry_period       			=0
save_retry_times   			=5
pdn_rej_forb_ctrl                       =[18, [8, 26, 27, 29, 30, 31, 32, 34, 38, 54, 95, 96, 97, 98, 99, 100, 101, 111, 0, 0, 0, 0, 0]], [2, [28, 33, 0, 0, 0, 0, 0]]</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row2187135114277"><td class="cellrowborder" valign="top" width="26.240000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p518715110271"><a name="zh-cn_topic_0000002146510138_p518715110271"></a><a name="zh-cn_topic_0000002146510138_p518715110271"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.76%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p15187135182712"><a name="zh-cn_topic_0000002146510138_p15187135182712"></a><a name="zh-cn_topic_0000002146510138_p15187135182712"></a>-</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table759319416307"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row859315413016"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p75936473013"><a name="zh-cn_topic_0000002146510138_p75936473013"></a><a name="zh-cn_topic_0000002146510138_p75936473013"></a>IMSA_NV_IMS_CONFIG</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row55938463020"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1659313463010"><a name="zh-cn_topic_0000002146510138_p1659313463010"></a><a name="zh-cn_topic_0000002146510138_p1659313463010"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p4593104133012"><a name="zh-cn_topic_0000002146510138_p4593104133012"></a><a name="zh-cn_topic_0000002146510138_p4593104133012"></a>0x500C</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row05931641304"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1259464163015"><a name="zh-cn_topic_0000002146510138_p1259464163015"></a><a name="zh-cn_topic_0000002146510138_p1259464163015"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p155946443013"><a name="zh-cn_topic_0000002146510138_p155946443013"></a><a name="zh-cn_topic_0000002146510138_p155946443013"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1259415453015"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p105942418303"><a name="zh-cn_topic_0000002146510138_p105942418303"></a><a name="zh-cn_topic_0000002146510138_p105942418303"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p259474133012"><a name="zh-cn_topic_0000002146510138_p259474133012"></a><a name="zh-cn_topic_0000002146510138_p259474133012"></a>IMSA参数配置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row05945493010"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p135945412309"><a name="zh-cn_topic_0000002146510138_p135945412309"></a><a name="zh-cn_topic_0000002146510138_p135945412309"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock15594194173016"><a name="zh-cn_topic_0000002146510138_codeblock15594194173016"></a><a name="zh-cn_topic_0000002146510138_codeblock15594194173016"></a>typedef struct {
    uint8_t user_info_flag;
    uint8_t auth_type;
    
    uint8_t esim_switch;
    uint8_t attach_for_ims;
    char impi[128];
    char impu[128];
    char domain[128];
    char pass_word[128];
} imsa_nv_ims_config;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row75941748306"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p559434153013"><a name="zh-cn_topic_0000002146510138_p559434153013"></a><a name="zh-cn_topic_0000002146510138_p559434153013"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p175941411304"><a name="zh-cn_topic_0000002146510138_p175941411304"></a><a name="zh-cn_topic_0000002146510138_p175941411304"></a>{0}</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row859474203019"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1459419433020"><a name="zh-cn_topic_0000002146510138_p1459419433020"></a><a name="zh-cn_topic_0000002146510138_p1459419433020"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="p1135804714375"><a name="p1135804714375"></a><a name="p1135804714375"></a>attach_for_ims默认值为0，设置为1表示ue入网时使用ims上下文。</p>
<p id="p959411419302"><a name="p959411419302"></a><a name="p959411419302"></a>PCT用例3GPP TS 34.229-1 19.5.9和3GPP TS 36.523-1 9.2.1.1.28需将attach_for_ims设置为1。</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table6443710183517"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row344311015359"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p1044351023520"><a name="zh-cn_topic_0000002146510138_p1044351023520"></a><a name="zh-cn_topic_0000002146510138_p1044351023520"></a>IMSA_UE_CAP</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row18443151053515"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p544315101358"><a name="zh-cn_topic_0000002146510138_p544315101358"></a><a name="zh-cn_topic_0000002146510138_p544315101358"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p16443141073510"><a name="zh-cn_topic_0000002146510138_p16443141073510"></a><a name="zh-cn_topic_0000002146510138_p16443141073510"></a>0x500D</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row8443131043514"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p844391013355"><a name="zh-cn_topic_0000002146510138_p844391013355"></a><a name="zh-cn_topic_0000002146510138_p844391013355"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p15443121017353"><a name="zh-cn_topic_0000002146510138_p15443121017353"></a><a name="zh-cn_topic_0000002146510138_p15443121017353"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row944361017359"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p12443410183513"><a name="zh-cn_topic_0000002146510138_p12443410183513"></a><a name="zh-cn_topic_0000002146510138_p12443410183513"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p944412108351"><a name="zh-cn_topic_0000002146510138_p944412108351"></a><a name="zh-cn_topic_0000002146510138_p944412108351"></a>IMSA UE能力配置。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1044412108351"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p8444310113518"><a name="zh-cn_topic_0000002146510138_p8444310113518"></a><a name="zh-cn_topic_0000002146510138_p8444310113518"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="codeblock167711837135811"><a name="codeblock167711837135811"></a><a name="codeblock167711837135811"></a>typedef struct {
    uint8_t voice_call_on_ims_support_flag; // IMS语音使能项，1:支持，0:不支持
    uint8_t sms_on_ims_support_flag;  // IMS短信使能项，1:支持，0:不支持
    int16_t call_rsrp_threhold;       // 呼叫门限值
    uint32_t res_ready_timer_len;    // 资源预留定时器时长(2500~3000)
} imsa_ue_cap;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row244412105359"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p74441410133516"><a name="zh-cn_topic_0000002146510138_p74441410133516"></a><a name="zh-cn_topic_0000002146510138_p74441410133516"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock17638510188"><a name="zh-cn_topic_0000002146510138_codeblock17638510188"></a><a name="zh-cn_topic_0000002146510138_codeblock17638510188"></a>voice_call_on_ims_support_flag		=1
sms_on_ims_support_flag       		=1
call_rsrp_threhold	=-120
res_ready_timer_len                     =2500</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row154443103353"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p144441110103519"><a name="zh-cn_topic_0000002146510138_p144441110103519"></a><a name="zh-cn_topic_0000002146510138_p144441110103519"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p143541635103616"><a name="zh-cn_topic_0000002146510138_p143541635103616"></a><a name="zh-cn_topic_0000002146510138_p143541635103616"></a>-</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table173261043193614"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row11326843183620"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p632634303617"><a name="zh-cn_topic_0000002146510138_p632634303617"></a><a name="zh-cn_topic_0000002146510138_p632634303617"></a>IMS_PCSCF_DISCOVERY_POLICY</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row632674318367"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p163263439369"><a name="zh-cn_topic_0000002146510138_p163263439369"></a><a name="zh-cn_topic_0000002146510138_p163263439369"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p3326643183617"><a name="zh-cn_topic_0000002146510138_p3326643183617"></a><a name="zh-cn_topic_0000002146510138_p3326643183617"></a>0x500E</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row43261943113611"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p15326243173618"><a name="zh-cn_topic_0000002146510138_p15326243173618"></a><a name="zh-cn_topic_0000002146510138_p15326243173618"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p12327543203617"><a name="zh-cn_topic_0000002146510138_p12327543203617"></a><a name="zh-cn_topic_0000002146510138_p12327543203617"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row183271943103613"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1532711431368"><a name="zh-cn_topic_0000002146510138_p1532711431368"></a><a name="zh-cn_topic_0000002146510138_p1532711431368"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p03271243113617"><a name="zh-cn_topic_0000002146510138_p03271243113617"></a><a name="zh-cn_topic_0000002146510138_p03271243113617"></a>IMSA NV配置PCSCF地址。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1932744320364"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p113271436360"><a name="zh-cn_topic_0000002146510138_p113271436360"></a><a name="zh-cn_topic_0000002146510138_p113271436360"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock832704312363"><a name="zh-cn_topic_0000002146510138_codeblock832704312363"></a><a name="zh-cn_topic_0000002146510138_codeblock832704312363"></a>#define IMSA_NV_IPV4_ADDR_LEN            4
#define IMSA_NV_IPV6_ADDR_LEN            16
typedef struct {
    uint32_t                          bit_op_prim_pcscf_addr  : 1;
    uint32_t                          bit_op_sec_pcscf_addr   : 1; // 如果bitOpSecPcscfAddr为1，bitOpPrimPcscfAddr也必须为1
    uint32_t                          bit_op_spare          : 30;
    uint8_t                           prim_pcscf_addr[IMSA_NV_IPV4_ADDR_LEN];
    uint8_t                           sec_pcscf_addr[IMSA_NV_IPV4_ADDR_LEN];
} imsa_nv_pdp_ipv4_pcscf;
typedef struct {
    uint32_t                          bit_op_prim_pcscf_addr  : 1;
    uint32_t                          bit_op_sec_pcscf_addr   : 1; // 如果bitOpSecPcscfAddr为1，bitOpPrimPcscfAddr也必须为1
    uint32_t                          bit_op_spare          : 30;
    uint8_t                           prim_pcscf_addr[IMSA_NV_IPV6_ADDR_LEN];
    uint8_t                           sec_pcscf_addr[IMSA_NV_IPV6_ADDR_LEN];
} imsa_nv_pdp_ipv6_pcscf;
typedef struct {
    imsa_pcscf_discovery_policy pcscf_discover_policy;  // P-CSCF获取策略
    uint8_t                     rsv[3];
    imsa_nv_pdp_ipv4_pcscf       ipv4_pcscf;            // 承载IPv4 P-CSCF信息
    imsa_nv_pdp_ipv6_pcscf       ipv6_pcscf;            // 承载IPv6 P-CSCF信息
} imsa_nv_pcscsf_discovery_policy;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row183271643193620"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p19327154383615"><a name="zh-cn_topic_0000002146510138_p19327154383615"></a><a name="zh-cn_topic_0000002146510138_p19327154383615"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock076419275818"><a name="zh-cn_topic_0000002146510138_codeblock076419275818"></a><a name="zh-cn_topic_0000002146510138_codeblock076419275818"></a>pcscf_discover_policy		=IMSA_PCSCF_DISCOVERY_POLICY_PCO
rsv[3]				={0}
ipv4_pcscf           		={0}
ipv6_pcscf           		={0}</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row13271043163611"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1432774363614"><a name="zh-cn_topic_0000002146510138_p1432774363614"></a><a name="zh-cn_topic_0000002146510138_p1432774363614"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p12227152510394"><a name="zh-cn_topic_0000002146510138_p12227152510394"></a><a name="zh-cn_topic_0000002146510138_p12227152510394"></a>不支持修改。</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table99512234111"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row3952192144116"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p89526218413"><a name="zh-cn_topic_0000002146510138_p89526218413"></a><a name="zh-cn_topic_0000002146510138_p89526218413"></a>IMS_SIP_PORT</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row9952226415"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p99524274110"><a name="zh-cn_topic_0000002146510138_p99524274110"></a><a name="zh-cn_topic_0000002146510138_p99524274110"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1495216216415"><a name="zh-cn_topic_0000002146510138_p1495216216415"></a><a name="zh-cn_topic_0000002146510138_p1495216216415"></a>0x500F</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row1495217214415"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1195272194111"><a name="zh-cn_topic_0000002146510138_p1195272194111"></a><a name="zh-cn_topic_0000002146510138_p1195272194111"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p19952182204116"><a name="zh-cn_topic_0000002146510138_p19952182204116"></a><a name="zh-cn_topic_0000002146510138_p19952182204116"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row195213216414"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p19521023413"><a name="zh-cn_topic_0000002146510138_p19521023413"></a><a name="zh-cn_topic_0000002146510138_p19521023413"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p2095232104113"><a name="zh-cn_topic_0000002146510138_p2095232104113"></a><a name="zh-cn_topic_0000002146510138_p2095232104113"></a>IMS注册端口范围。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row149529264117"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p29523244116"><a name="zh-cn_topic_0000002146510138_p29523244116"></a><a name="zh-cn_topic_0000002146510138_p29523244116"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock2952192184115"><a name="zh-cn_topic_0000002146510138_codeblock2952192184115"></a><a name="zh-cn_topic_0000002146510138_codeblock2952192184115"></a>typedef struct {
    uint16_t    ims_min_port;
    uint16_t    ims_max_port;
} ims_sip_port;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row495310294116"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p7953721412"><a name="zh-cn_topic_0000002146510138_p7953721412"></a><a name="zh-cn_topic_0000002146510138_p7953721412"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock521218503813"><a name="zh-cn_topic_0000002146510138_codeblock521218503813"></a><a name="zh-cn_topic_0000002146510138_codeblock521218503813"></a>ims_min_port                =5000
ims_max_port                =32000</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row995315213411"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p99531025415"><a name="zh-cn_topic_0000002146510138_p99531025415"></a><a name="zh-cn_topic_0000002146510138_p99531025415"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p20543181919578"><a name="zh-cn_topic_0000002146510138_p20543181919578"></a><a name="zh-cn_topic_0000002146510138_p20543181919578"></a>不支持修改。</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table17813101834218"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row118131818124219"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p1681319187429"><a name="zh-cn_topic_0000002146510138_p1681319187429"></a><a name="zh-cn_topic_0000002146510138_p1681319187429"></a>IMS_COMM_PARA_CONFIG</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row7814111819424"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p158141818124218"><a name="zh-cn_topic_0000002146510138_p158141818124218"></a><a name="zh-cn_topic_0000002146510138_p158141818124218"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p148141818134218"><a name="zh-cn_topic_0000002146510138_p148141818134218"></a><a name="zh-cn_topic_0000002146510138_p148141818134218"></a>0x5010</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row5814151874218"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1881401816422"><a name="zh-cn_topic_0000002146510138_p1881401816422"></a><a name="zh-cn_topic_0000002146510138_p1881401816422"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p081412182422"><a name="zh-cn_topic_0000002146510138_p081412182422"></a><a name="zh-cn_topic_0000002146510138_p081412182422"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row7814121824217"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p10814131816424"><a name="zh-cn_topic_0000002146510138_p10814131816424"></a><a name="zh-cn_topic_0000002146510138_p10814131816424"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p17814201817423"><a name="zh-cn_topic_0000002146510138_p17814201817423"></a><a name="zh-cn_topic_0000002146510138_p17814201817423"></a>IMS语音端口号范围。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row9814181844213"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p9814161854218"><a name="zh-cn_topic_0000002146510138_p9814161854218"></a><a name="zh-cn_topic_0000002146510138_p9814161854218"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock18141718144214"><a name="zh-cn_topic_0000002146510138_codeblock18141718144214"></a><a name="zh-cn_topic_0000002146510138_codeblock18141718144214"></a>typedef struct {
    uint32_t    speech_start;
    uint32_t    speech_end;
    uint32_t    video_start;
    uint32_t    video_end;
} ims_comm_para_config;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row128141418134218"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p17814318114219"><a name="zh-cn_topic_0000002146510138_p17814318114219"></a><a name="zh-cn_topic_0000002146510138_p17814318114219"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p02351022121111"><a name="zh-cn_topic_0000002146510138_p02351022121111"></a><a name="zh-cn_topic_0000002146510138_p02351022121111"></a>{0}</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row681451894213"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p38141118144219"><a name="zh-cn_topic_0000002146510138_p38141118144219"></a><a name="zh-cn_topic_0000002146510138_p38141118144219"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p148141318104216"><a name="zh-cn_topic_0000002146510138_p148141318104216"></a><a name="zh-cn_topic_0000002146510138_p148141318104216"></a>暂未使用。</p>
</td>
</tr>
</tbody>
</table>

<a name="zh-cn_topic_0000002146510138_table7182152918117"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002146510138_row418212914113"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002146510138_p101821329171110"><a name="zh-cn_topic_0000002146510138_p101821329171110"></a><a name="zh-cn_topic_0000002146510138_p101821329171110"></a>IMSA_NV_CALL_IMS_CCWA_CTRL_MODE</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002146510138_row171821829171112"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1918282917116"><a name="zh-cn_topic_0000002146510138_p1918282917116"></a><a name="zh-cn_topic_0000002146510138_p1918282917116"></a>Key ID</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p3182172914115"><a name="zh-cn_topic_0000002146510138_p3182172914115"></a><a name="zh-cn_topic_0000002146510138_p3182172914115"></a>0x5011</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row418219297113"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1183729191113"><a name="zh-cn_topic_0000002146510138_p1183729191113"></a><a name="zh-cn_topic_0000002146510138_p1183729191113"></a>Key 状态</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p6183129191117"><a name="zh-cn_topic_0000002146510138_p6183129191117"></a><a name="zh-cn_topic_0000002146510138_p6183129191117"></a>alive</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row118372931118"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1818392917110"><a name="zh-cn_topic_0000002146510138_p1818392917110"></a><a name="zh-cn_topic_0000002146510138_p1818392917110"></a>Key 描述</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p297404419149"><a name="zh-cn_topic_0000002146510138_p297404419149"></a><a name="zh-cn_topic_0000002146510138_p297404419149"></a>-</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row2183182913111"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p111832029121110"><a name="zh-cn_topic_0000002146510138_p111832029121110"></a><a name="zh-cn_topic_0000002146510138_p111832029121110"></a>结构类型</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock131838297115"><a name="zh-cn_topic_0000002146510138_codeblock131838297115"></a><a name="zh-cn_topic_0000002146510138_codeblock131838297115"></a>typedef enum {
    IMSA_CALL_CCWA_CTRL_BY_NW = 0,
    IMSA_CALL_CCWA_CTRL_BY_UE = 1,
    IMSA_CALL_CCWA_CTRL_MODE_BUTT = 0xFF,
} imsa_call_ccwa_ctrl_mode;
typedef enum {
    IMSA_CALL_CCWAI_MODE_DISABLE = 0,
    IMSA_CALL_CCWAI_MODE_ENABLE = 1,
    IMSA_CALL_CCWAI_MODE_BUTT = 0xFF,
} imsa_call_ccwai_mode;
typedef struct {
    imsa_call_ccwa_ctrl_mode    ccwa_ctrl_mode;
    imsa_call_ccwai_mode        ccwai_mode;
    uint8_t                     rsv[2];
} imsa_nv_call_ims_ccwa_ctrl_mode;</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row9183172921115"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p1818320293111"><a name="zh-cn_topic_0000002146510138_p1818320293111"></a><a name="zh-cn_topic_0000002146510138_p1818320293111"></a>默认值</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><pre class="codeblock" id="zh-cn_topic_0000002146510138_codeblock87984295819"><a name="zh-cn_topic_0000002146510138_codeblock87984295819"></a><a name="zh-cn_topic_0000002146510138_codeblock87984295819"></a>ccwa_ctrl_mode                      =IMSA_CALL_CCWA_CTRL_BY_UE
ccwai_mode                            =IMSA_CALL_CCWAI_MODE_ENABLE
rsv[2]                                       ={0}</pre>
</td>
</tr>
<tr id="zh-cn_topic_0000002146510138_row20183122971116"><td class="cellrowborder" valign="top" width="26.27%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p2018352921111"><a name="zh-cn_topic_0000002146510138_p2018352921111"></a><a name="zh-cn_topic_0000002146510138_p2018352921111"></a>备注</p>
</td>
<td class="cellrowborder" valign="top" width="73.72999999999999%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002146510138_p111836297112"><a name="zh-cn_topic_0000002146510138_p111836297112"></a><a name="zh-cn_topic_0000002146510138_p111836297112"></a>不支持修改。</p>
</td>
</tr>
</tbody>
</table>

