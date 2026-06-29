# 前言<a name="ZH-CN_TOPIC_0000002466383680"></a>

**概述<a name="zh-cn_topic_0000002411047416_section4537382116410"></a>**

本文档主要介绍图形驱动调试相关内容，包括工作原理、使用方法和注意事项，用于指导客户快速抓取需要的调试信息。

**读者对象<a name="zh-cn_topic_0000002411047416_section8791633172512"></a>**

本文档主要适用于以下工程师：

-   技术支持工程师
-   软件开发工程师

**符号约定<a name="zh-cn_topic_0000002411047416_section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="zh-cn_topic_0000002411047416_table2622507016410"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002411047416_row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002411047416_p6450074116410"><a name="zh-cn_topic_0000002411047416_p6450074116410"></a><a name="zh-cn_topic_0000002411047416_p6450074116410"></a><strong id="zh-cn_topic_0000002411047416_b2136615816410"><a name="zh-cn_topic_0000002411047416_b2136615816410"></a><a name="zh-cn_topic_0000002411047416_b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002411047416_p5435366816410"><a name="zh-cn_topic_0000002411047416_p5435366816410"></a><a name="zh-cn_topic_0000002411047416_p5435366816410"></a><strong id="zh-cn_topic_0000002411047416_b5941558116410"><a name="zh-cn_topic_0000002411047416_b5941558116410"></a><a name="zh-cn_topic_0000002411047416_b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002411047416_row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002411047416_p3734547016410"><a name="zh-cn_topic_0000002411047416_p3734547016410"></a><a name="zh-cn_topic_0000002411047416_p3734547016410"></a><a name="zh-cn_topic_0000002411047416_image2670064316410"></a><a name="zh-cn_topic_0000002411047416_image2670064316410"></a><span><img class="" id="zh-cn_topic_0000002411047416_image2670064316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002469425054.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002411047416_p1757432116410"><a name="zh-cn_topic_0000002411047416_p1757432116410"></a><a name="zh-cn_topic_0000002411047416_p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047416_row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002411047416_p1432579516410"><a name="zh-cn_topic_0000002411047416_p1432579516410"></a><a name="zh-cn_topic_0000002411047416_p1432579516410"></a><a name="zh-cn_topic_0000002411047416_image4895582316410"></a><a name="zh-cn_topic_0000002411047416_image4895582316410"></a><span><img class="" id="zh-cn_topic_0000002411047416_image4895582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002469265052.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002411047416_p959197916410"><a name="zh-cn_topic_0000002411047416_p959197916410"></a><a name="zh-cn_topic_0000002411047416_p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047416_row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002411047416_p1232579516410"><a name="zh-cn_topic_0000002411047416_p1232579516410"></a><a name="zh-cn_topic_0000002411047416_p1232579516410"></a><a name="zh-cn_topic_0000002411047416_image1235582316410"></a><a name="zh-cn_topic_0000002411047416_image1235582316410"></a><span><img class="" id="zh-cn_topic_0000002411047416_image1235582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002502265043.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002411047416_p123197916410"><a name="zh-cn_topic_0000002411047416_p123197916410"></a><a name="zh-cn_topic_0000002411047416_p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047416_row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002411047416_p2204984716410"><a name="zh-cn_topic_0000002411047416_p2204984716410"></a><a name="zh-cn_topic_0000002411047416_p2204984716410"></a><a name="zh-cn_topic_0000002411047416_image4504446716410"></a><a name="zh-cn_topic_0000002411047416_image4504446716410"></a><span><img class="" id="zh-cn_topic_0000002411047416_image4504446716410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002502385035.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002411047416_p4388861916410"><a name="zh-cn_topic_0000002411047416_p4388861916410"></a><a name="zh-cn_topic_0000002411047416_p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="zh-cn_topic_0000002411047416_p1238861916410"><a name="zh-cn_topic_0000002411047416_p1238861916410"></a><a name="zh-cn_topic_0000002411047416_p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047416_row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002411047416_p5555360116410"><a name="zh-cn_topic_0000002411047416_p5555360116410"></a><a name="zh-cn_topic_0000002411047416_p5555360116410"></a><a name="zh-cn_topic_0000002411047416_image799324016410"></a><a name="zh-cn_topic_0000002411047416_image799324016410"></a><span><img class="" id="zh-cn_topic_0000002411047416_image799324016410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002469425072.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002411047416_p4612588116410"><a name="zh-cn_topic_0000002411047416_p4612588116410"></a><a name="zh-cn_topic_0000002411047416_p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="zh-cn_topic_0000002411047416_p1232588116410"><a name="zh-cn_topic_0000002411047416_p1232588116410"></a><a name="zh-cn_topic_0000002411047416_p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# 概述<a name="ZH-CN_TOPIC_0000002499582681"></a>

通过下发命令查询图形驱动各模块基本信息、状态。

只有在有对应图形业务的时候，抓取的proc信息才有实际内容，否则为空。

# 各模块AT命令汇总<a name="ZH-CN_TOPIC_0000002466543324"></a>

**表 1**  各模块AT命令

<a name="zh-cn_topic_0000002411047420_table707mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002411047420_row713mcpsimp"><th class="cellrowborder" valign="top" width="29.12%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002411047420_p715mcpsimp"><a name="zh-cn_topic_0000002411047420_p715mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p715mcpsimp"></a>命令</p>
</th>
<th class="cellrowborder" valign="top" width="33.77%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002411047420_p718mcpsimp"><a name="zh-cn_topic_0000002411047420_p718mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p718mcpsimp"></a>功能</p>
</th>
<th class="cellrowborder" valign="top" width="37.11%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002411047420_p721mcpsimp"><a name="zh-cn_topic_0000002411047420_p721mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p721mcpsimp"></a>参数说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002411047420_row723mcpsimp"><td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047420_p725mcpsimp"><a name="zh-cn_topic_0000002411047420_p725mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p725mcpsimp"></a>AT+GPU=smoke</p>
</td>
<td class="cellrowborder" valign="top" width="33.77%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047420_p727mcpsimp"><a name="zh-cn_topic_0000002411047420_p727mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p727mcpsimp"></a>GPU冒烟用例：颜色填充＋显示。</p>
</td>
<td class="cellrowborder" valign="top" width="37.11%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411047420_p729mcpsimp"><a name="zh-cn_topic_0000002411047420_p729mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p729mcpsimp"></a>无参。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047420_row730mcpsimp"><td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047420_p732mcpsimp"><a name="zh-cn_topic_0000002411047420_p732mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p732mcpsimp"></a>AT+GPU=dpu_cbar,0/1</p>
</td>
<td class="cellrowborder" valign="top" width="33.77%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047420_p734mcpsimp"><a name="zh-cn_topic_0000002411047420_p734mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p734mcpsimp"></a>color_bar使能控制。</p>
</td>
<td class="cellrowborder" valign="top" width="37.11%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411047420_p1770181225310"><a name="zh-cn_topic_0000002411047420_p1770181225310"></a><a name="zh-cn_topic_0000002411047420_p1770181225310"></a>使能标记。</p>
<p id="zh-cn_topic_0000002411047420_p13790191095311"><a name="zh-cn_topic_0000002411047420_p13790191095311"></a><a name="zh-cn_topic_0000002411047420_p13790191095311"></a>0：不使能；</p>
<p id="zh-cn_topic_0000002411047420_p736mcpsimp"><a name="zh-cn_topic_0000002411047420_p736mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p736mcpsimp"></a>1：使能。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047420_row737mcpsimp"><td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047420_p1030194916189"><a name="zh-cn_topic_0000002411047420_p1030194916189"></a><a name="zh-cn_topic_0000002411047420_p1030194916189"></a>AT+GPU=dpu_wbc_screen_cap,layer_id,is_wbc_cap</p>
</td>
<td class="cellrowborder" valign="top" width="33.77%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047420_p741mcpsimp"><a name="zh-cn_topic_0000002411047420_p741mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p741mcpsimp"></a>DPU截屏，截屏成功后会输出文件路径和stride\height信息。</p>
</td>
<td class="cellrowborder" valign="top" width="37.11%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411047420_p743mcpsimp"><a name="zh-cn_topic_0000002411047420_p743mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p743mcpsimp"></a>layer_id(0~2) is_wbc_cap(0/1)。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047420_row744mcpsimp"><td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047420_p746mcpsimp"><a name="zh-cn_topic_0000002411047420_p746mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p746mcpsimp"></a>AT+GPU=vau_proc</p>
</td>
<td class="cellrowborder" valign="top" width="33.77%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047420_p748mcpsimp"><a name="zh-cn_topic_0000002411047420_p748mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p748mcpsimp"></a>输出VAU的DFX信息。</p>
</td>
<td class="cellrowborder" valign="top" width="37.11%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411047420_p750mcpsimp"><a name="zh-cn_topic_0000002411047420_p750mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p750mcpsimp"></a>无参。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047420_row751mcpsimp"><td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047420_p753mcpsimp"><a name="zh-cn_topic_0000002411047420_p753mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p753mcpsimp"></a>AT+GPU=jpeg_proc</p>
</td>
<td class="cellrowborder" valign="top" width="33.77%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047420_p755mcpsimp"><a name="zh-cn_topic_0000002411047420_p755mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p755mcpsimp"></a>输出JPEG的DFX信息。</p>
</td>
<td class="cellrowborder" valign="top" width="37.11%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411047420_p757mcpsimp"><a name="zh-cn_topic_0000002411047420_p757mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p757mcpsimp"></a>无参。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047420_row758mcpsimp"><td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047420_p760mcpsimp"><a name="zh-cn_topic_0000002411047420_p760mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p760mcpsimp"></a>AT+GPU=dpu_proc,0/1</p>
</td>
<td class="cellrowborder" valign="top" width="33.77%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047420_p762mcpsimp"><a name="zh-cn_topic_0000002411047420_p762mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p762mcpsimp"></a>输出DPU层的信息。</p>
</td>
<td class="cellrowborder" valign="top" width="37.11%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411047420_p764mcpsimp"><a name="zh-cn_topic_0000002411047420_p764mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p764mcpsimp"></a>0/1代表layer_id。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047420_row765mcpsimp"><td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047420_p767mcpsimp"><a name="zh-cn_topic_0000002411047420_p767mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p767mcpsimp"></a>AT+GPU=dpu_print,0/1</p>
</td>
<td class="cellrowborder" valign="top" width="33.77%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047420_p769mcpsimp"><a name="zh-cn_topic_0000002411047420_p769mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p769mcpsimp"></a>送显接口耗时统计输出使能。</p>
</td>
<td class="cellrowborder" valign="top" width="37.11%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411047420_p771mcpsimp"><a name="zh-cn_topic_0000002411047420_p771mcpsimp"></a><a name="zh-cn_topic_0000002411047420_p771mcpsimp"></a>使能标记。</p>
<p id="zh-cn_topic_0000002411047420_p19651621125318"><a name="zh-cn_topic_0000002411047420_p19651621125318"></a><a name="zh-cn_topic_0000002411047420_p19651621125318"></a>0：不使能；</p>
<p id="zh-cn_topic_0000002411047420_p7652214532"><a name="zh-cn_topic_0000002411047420_p7652214532"></a><a name="zh-cn_topic_0000002411047420_p7652214532"></a>1：使能。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047420_row02821155164818"><td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047420_p9772163015494"><a name="zh-cn_topic_0000002411047420_p9772163015494"></a><a name="zh-cn_topic_0000002411047420_p9772163015494"></a>AT+GPU=dpu_test_layer_destroy,0/1</p>
</td>
<td class="cellrowborder" valign="top" width="33.77%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047420_p1475164011490"><a name="zh-cn_topic_0000002411047420_p1475164011490"></a><a name="zh-cn_topic_0000002411047420_p1475164011490"></a>销毁layer 0/1</p>
</td>
<td class="cellrowborder" valign="top" width="37.11%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411047420_p1698334884913"><a name="zh-cn_topic_0000002411047420_p1698334884913"></a><a name="zh-cn_topic_0000002411047420_p1698334884913"></a>0/1代表layer_id。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047420_row1228819154498"><td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047420_p127721308491"><a name="zh-cn_topic_0000002411047420_p127721308491"></a><a name="zh-cn_topic_0000002411047420_p127721308491"></a>AT+GPU=dpu_test_layer_show,layer,show</p>
</td>
<td class="cellrowborder" valign="top" width="33.77%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047420_p127511540184917"><a name="zh-cn_topic_0000002411047420_p127511540184917"></a><a name="zh-cn_topic_0000002411047420_p127511540184917"></a>打开或关闭layer 0/1</p>
</td>
<td class="cellrowborder" valign="top" width="37.11%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411047420_p874523517256"><a name="zh-cn_topic_0000002411047420_p874523517256"></a><a name="zh-cn_topic_0000002411047420_p874523517256"></a>layer：0/1。</p>
<p id="zh-cn_topic_0000002411047420_p340495013258"><a name="zh-cn_topic_0000002411047420_p340495013258"></a><a name="zh-cn_topic_0000002411047420_p340495013258"></a>show：0：关闭；1：打开。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047420_row1857821813499"><td class="cellrowborder" valign="top" width="29.12%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047420_p07723309491"><a name="zh-cn_topic_0000002411047420_p07723309491"></a><a name="zh-cn_topic_0000002411047420_p07723309491"></a>AT+GPU=dpu_save_fb,0,1,1,10</p>
</td>
<td class="cellrowborder" valign="top" width="33.77%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047420_p5751114017493"><a name="zh-cn_topic_0000002411047420_p5751114017493"></a><a name="zh-cn_topic_0000002411047420_p5751114017493"></a>保存DPU送显前FB数据。</p>
</td>
<td class="cellrowborder" valign="top" width="37.11%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411047420_p7983548194913"><a name="zh-cn_topic_0000002411047420_p7983548194913"></a><a name="zh-cn_topic_0000002411047420_p7983548194913"></a>参数1：layer_id取值0/1。</p>
<p id="zh-cn_topic_0000002411047420_p19836488498"><a name="zh-cn_topic_0000002411047420_p19836488498"></a><a name="zh-cn_topic_0000002411047420_p19836488498"></a>参数2：数据保存使能标记：0/1。</p>
<p id="zh-cn_topic_0000002411047420_p10983148204916"><a name="zh-cn_topic_0000002411047420_p10983148204916"></a><a name="zh-cn_topic_0000002411047420_p10983148204916"></a>参数3：全屏保存标记：对局刷场景来说，填0只保存局刷区域数据，填1保存全屏数据，对全局送显场景，保存数据无差别。取值范围0/1。</p>
<p id="zh-cn_topic_0000002411047420_p1798334844911"><a name="zh-cn_topic_0000002411047420_p1798334844911"></a><a name="zh-cn_topic_0000002411047420_p1798334844911"></a>参数4：save_cnt：保存的帧数。</p>
</td>
</tr>
</tbody>
</table>

# VAU proc<a name="ZH-CN_TOPIC_0000002466543300"></a>

-   **[proc简介](#ZH-CN_TOPIC_0000002466543316)**  

-   **[proc信息](#ZH-CN_TOPIC_0000002499502641)**  

-   **[proc详解](#ZH-CN_TOPIC_0000002499502621)**  

## proc简介<a name="ZH-CN_TOPIC_0000002466543316"></a>

查询VAU主要函数耗时、主要函数调用次数、任务链表相关信息。

## proc信息<a name="ZH-CN_TOPIC_0000002499502641"></a>

下发命令： AT+GPU=vau\_proc

VAU模块的proc信息如[图1](#zh-cn_topic_0000002411207292_fig154294588820)所示。

**图 1**  VAU proc信息<a name="zh-cn_topic_0000002411207292_fig154294588820"></a>  
![](figures/VAU-proc信息.png "VAU-proc信息")

## proc详解<a name="ZH-CN_TOPIC_0000002499502621"></a>

**表 1**  VAU proc信息说明

<a name="zh-cn_topic_0000002411047432_table106mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002411047432_row112mcpsimp"><th class="cellrowborder" valign="top" width="27%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002411047432_p114mcpsimp"><a name="zh-cn_topic_0000002411047432_p114mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p114mcpsimp"></a>类别</p>
</th>
<th class="cellrowborder" valign="top" width="28.999999999999996%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002411047432_p043961881419"><a name="zh-cn_topic_0000002411047432_p043961881419"></a><a name="zh-cn_topic_0000002411047432_p043961881419"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="44%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002411047432_p104396185145"><a name="zh-cn_topic_0000002411047432_p104396185145"></a><a name="zh-cn_topic_0000002411047432_p104396185145"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002411047432_row116mcpsimp"><td class="cellrowborder" rowspan="8" valign="top" width="27%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p118mcpsimp"><a name="zh-cn_topic_0000002411047432_p118mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p118mcpsimp"></a>time info</p>
<p id="zh-cn_topic_0000002411047432_p119mcpsimp"><a name="zh-cn_topic_0000002411047432_p119mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p119mcpsimp"></a>主要函数单次运行时长，单位：μs</p>
</td>
<td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p121mcpsimp"><a name="zh-cn_topic_0000002411047432_p121mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p121mcpsimp"></a>create_cost</p>
</td>
<td class="cellrowborder" valign="top" width="44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411047432_p123mcpsimp"><a name="zh-cn_topic_0000002411047432_p123mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p123mcpsimp"></a>vau_create函数运行时长。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row124mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p126mcpsimp"><a name="zh-cn_topic_0000002411047432_p126mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p126mcpsimp"></a>destroy_cost</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p128mcpsimp"><a name="zh-cn_topic_0000002411047432_p128mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p128mcpsimp"></a>vau_destroy函数运行时长。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row129mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p131mcpsimp"><a name="zh-cn_topic_0000002411047432_p131mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p131mcpsimp"></a>blit_cost</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p133mcpsimp"><a name="zh-cn_topic_0000002411047432_p133mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p133mcpsimp"></a>vau_blit函数运行时长。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row134mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p136mcpsimp"><a name="zh-cn_topic_0000002411047432_p136mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p136mcpsimp"></a>compose_cost</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p138mcpsimp"><a name="zh-cn_topic_0000002411047432_p138mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p138mcpsimp"></a>vau_compose函数运行时长。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row139mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p141mcpsimp"><a name="zh-cn_topic_0000002411047432_p141mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p141mcpsimp"></a>submit_cost</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p143mcpsimp"><a name="zh-cn_topic_0000002411047432_p143mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p143mcpsimp"></a>vau_submit函数运行时长。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row144mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p146mcpsimp"><a name="zh-cn_topic_0000002411047432_p146mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p146mcpsimp"></a>wait done cose</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p148mcpsimp"><a name="zh-cn_topic_0000002411047432_p148mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p148mcpsimp"></a>vau_wait_for_done函数运行时长。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row149mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p151mcpsimp"><a name="zh-cn_topic_0000002411047432_p151mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p151mcpsimp"></a>list pfcnt/time_us</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p153mcpsimp"><a name="zh-cn_topic_0000002411047432_p153mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p153mcpsimp"></a>链表性能统计（时钟周期）/链表性能统计（μs）。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row154mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p156mcpsimp"><a name="zh-cn_topic_0000002411047432_p156mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p156mcpsimp"></a>suspend/resume</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p158mcpsimp"><a name="zh-cn_topic_0000002411047432_p158mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p158mcpsimp"></a>suspend/resume次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row159mcpsimp"><td class="cellrowborder" rowspan="25" valign="top" width="27%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p161mcpsimp"><a name="zh-cn_topic_0000002411047432_p161mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p161mcpsimp"></a>call count info</p>
<p id="zh-cn_topic_0000002411047432_p162mcpsimp"><a name="zh-cn_topic_0000002411047432_p162mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p162mcpsimp"></a>主要函数调用次数</p>
</td>
<td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p164mcpsimp"><a name="zh-cn_topic_0000002411047432_p164mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p164mcpsimp"></a>create_cnt</p>
</td>
<td class="cellrowborder" valign="top" width="44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411047432_p166mcpsimp"><a name="zh-cn_topic_0000002411047432_p166mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p166mcpsimp"></a>vau_create函数调用次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row167mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p169mcpsimp"><a name="zh-cn_topic_0000002411047432_p169mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p169mcpsimp"></a>cancel_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p171mcpsimp"><a name="zh-cn_topic_0000002411047432_p171mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p171mcpsimp"></a>vau_cancel函数调用次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row172mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p174mcpsimp"><a name="zh-cn_topic_0000002411047432_p174mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p174mcpsimp"></a>destroy_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p176mcpsimp"><a name="zh-cn_topic_0000002411047432_p176mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p176mcpsimp"></a>vau_destroy函数调用次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row177mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p179mcpsimp"><a name="zh-cn_topic_0000002411047432_p179mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p179mcpsimp"></a>blit_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p181mcpsimp"><a name="zh-cn_topic_0000002411047432_p181mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p181mcpsimp"></a>vau_blit函数调用次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row182mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p184mcpsimp"><a name="zh-cn_topic_0000002411047432_p184mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p184mcpsimp"></a>compose_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p186mcpsimp"><a name="zh-cn_topic_0000002411047432_p186mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p186mcpsimp"></a>vau_compose函数调用次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row187mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p189mcpsimp"><a name="zh-cn_topic_0000002411047432_p189mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p189mcpsimp"></a>submit_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p191mcpsimp"><a name="zh-cn_topic_0000002411047432_p191mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p191mcpsimp"></a>vau_submit函数调用次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row192mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p194mcpsimp"><a name="zh-cn_topic_0000002411047432_p194mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p194mcpsimp"></a>usr</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p196mcpsimp"><a name="zh-cn_topic_0000002411047432_p196mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p196mcpsimp"></a>vau_submit函数被用户调用次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row197mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p199mcpsimp"><a name="zh-cn_topic_0000002411047432_p199mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p199mcpsimp"></a>drv</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p201mcpsimp"><a name="zh-cn_topic_0000002411047432_p201mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p201mcpsimp"></a>vau_submit函数被驱动调用次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row202mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p204mcpsimp"><a name="zh-cn_topic_0000002411047432_p204mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p204mcpsimp"></a>submit from int</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p206mcpsimp"><a name="zh-cn_topic_0000002411047432_p206mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p206mcpsimp"></a>中断触发的submit次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row207mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p209mcpsimp"><a name="zh-cn_topic_0000002411047432_p209mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p209mcpsimp"></a>submit from user</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p211mcpsimp"><a name="zh-cn_topic_0000002411047432_p211mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p211mcpsimp"></a>用户触发的submit次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row212mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p214mcpsimp"><a name="zh-cn_topic_0000002411047432_p214mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p214mcpsimp"></a>wait_done_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p216mcpsimp"><a name="zh-cn_topic_0000002411047432_p216mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p216mcpsimp"></a>vau_wait_for_doen函数调用次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row217mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p219mcpsimp"><a name="zh-cn_topic_0000002411047432_p219mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p219mcpsimp"></a>isr_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p221mcpsimp"><a name="zh-cn_topic_0000002411047432_p221mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p221mcpsimp"></a>中断触发次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row222mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p224mcpsimp"><a name="zh-cn_topic_0000002411047432_p224mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p224mcpsimp"></a>node_end</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p226mcpsimp"><a name="zh-cn_topic_0000002411047432_p226mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p226mcpsimp"></a>节点完成中断次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row227mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p229mcpsimp"><a name="zh-cn_topic_0000002411047432_p229mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p229mcpsimp"></a>tunl_done</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p231mcpsimp"><a name="zh-cn_topic_0000002411047432_p231mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p231mcpsimp"></a>低时延中断次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row232mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p234mcpsimp"><a name="zh-cn_topic_0000002411047432_p234mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p234mcpsimp"></a>timeout</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p236mcpsimp"><a name="zh-cn_topic_0000002411047432_p236mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p236mcpsimp"></a>中断超时次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row237mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p239mcpsimp"><a name="zh-cn_topic_0000002411047432_p239mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p239mcpsimp"></a>bus_err</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p241mcpsimp"><a name="zh-cn_topic_0000002411047432_p241mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p241mcpsimp"></a>总线错误中断次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row242mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p244mcpsimp"><a name="zh-cn_topic_0000002411047432_p244mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p244mcpsimp"></a>list_end</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p246mcpsimp"><a name="zh-cn_topic_0000002411047432_p246mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p246mcpsimp"></a>链表结束中断次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row247mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p249mcpsimp"><a name="zh-cn_topic_0000002411047432_p249mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p249mcpsimp"></a>disp_done</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p251mcpsimp"><a name="zh-cn_topic_0000002411047432_p251mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p251mcpsimp"></a>送显node完成中断次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row252mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p254mcpsimp"><a name="zh-cn_topic_0000002411047432_p254mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p254mcpsimp"></a>conflict</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p256mcpsimp"><a name="zh-cn_topic_0000002411047432_p256mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p256mcpsimp"></a>中断冲突次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row257mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p259mcpsimp"><a name="zh-cn_topic_0000002411047432_p259mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p259mcpsimp"></a>isr zero value</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p261mcpsimp"><a name="zh-cn_topic_0000002411047432_p261mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p261mcpsimp"></a>中断状态为0次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row262mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p264mcpsimp"><a name="zh-cn_topic_0000002411047432_p264mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p264mcpsimp"></a>tasklet_func_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p266mcpsimp"><a name="zh-cn_topic_0000002411047432_p266mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p266mcpsimp"></a>tasklet_func函数调用次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row267mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p269mcpsimp"><a name="zh-cn_topic_0000002411047432_p269mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p269mcpsimp"></a>execute_node_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p271mcpsimp"><a name="zh-cn_topic_0000002411047432_p271mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p271mcpsimp"></a>节点执行次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row272mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p274mcpsimp"><a name="zh-cn_topic_0000002411047432_p274mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p274mcpsimp"></a>hard int cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p276mcpsimp"><a name="zh-cn_topic_0000002411047432_p276mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p276mcpsimp"></a>寄存器值读取：VAU中断个数的计数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row277mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p279mcpsimp"><a name="zh-cn_topic_0000002411047432_p279mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p279mcpsimp"></a>hard start cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p281mcpsimp"><a name="zh-cn_topic_0000002411047432_p281mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p281mcpsimp"></a>寄存器值读取：链表start的个数计数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row282mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p284mcpsimp"><a name="zh-cn_topic_0000002411047432_p284mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p284mcpsimp"></a>hard list finish cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p286mcpsimp"><a name="zh-cn_topic_0000002411047432_p286mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p286mcpsimp"></a>寄存器值读取：链表完成的个数计数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row287mcpsimp"><td class="cellrowborder" rowspan="8" valign="top" width="27%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p289mcpsimp"><a name="zh-cn_topic_0000002411047432_p289mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p289mcpsimp"></a>job_list</p>
<p id="zh-cn_topic_0000002411047432_p290mcpsimp"><a name="zh-cn_topic_0000002411047432_p290mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p290mcpsimp"></a>任务信息查询</p>
</td>
<td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p292mcpsimp"><a name="zh-cn_topic_0000002411047432_p292mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p292mcpsimp"></a>last finished handle</p>
</td>
<td class="cellrowborder" valign="top" width="44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411047432_p294mcpsimp"><a name="zh-cn_topic_0000002411047432_p294mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p294mcpsimp"></a>最后一次任务完成的handle号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row295mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p297mcpsimp"><a name="zh-cn_topic_0000002411047432_p297mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p297mcpsimp"></a>job create num</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p299mcpsimp"><a name="zh-cn_topic_0000002411047432_p299mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p299mcpsimp"></a>任务创建总次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row300mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p302mcpsimp"><a name="zh-cn_topic_0000002411047432_p302mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p302mcpsimp"></a>job submit(delay)</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p304mcpsimp"><a name="zh-cn_topic_0000002411047432_p304mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p304mcpsimp"></a>延迟递交次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row305mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p307mcpsimp"><a name="zh-cn_topic_0000002411047432_p307mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p307mcpsimp"></a>job unfinish</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p309mcpsimp"><a name="zh-cn_topic_0000002411047432_p309mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p309mcpsimp"></a>等待递交任务个数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row310mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p312mcpsimp"><a name="zh-cn_topic_0000002411047432_p312mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p312mcpsimp"></a>job resume num</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p314mcpsimp"><a name="zh-cn_topic_0000002411047432_p314mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p314mcpsimp"></a>硬件处理超时导致的resume次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row315mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p317mcpsimp"><a name="zh-cn_topic_0000002411047432_p317mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p317mcpsimp"></a>job timeout num</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_entry318mcpsimpp0"><a name="zh-cn_topic_0000002411047432_entry318mcpsimpp0"></a><a name="zh-cn_topic_0000002411047432_entry318mcpsimpp0"></a>软件超时次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row319mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p321mcpsimp"><a name="zh-cn_topic_0000002411047432_p321mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p321mcpsimp"></a>workqueen start</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p323mcpsimp"><a name="zh-cn_topic_0000002411047432_p323mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p323mcpsimp"></a>workqueen start次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411047432_row324mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411047432_p326mcpsimp"><a name="zh-cn_topic_0000002411047432_p326mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p326mcpsimp"></a>workqueen end</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411047432_p328mcpsimp"><a name="zh-cn_topic_0000002411047432_p328mcpsimp"></a><a name="zh-cn_topic_0000002411047432_p328mcpsimp"></a>workqueen end次数。</p>
</td>
</tr>
</tbody>
</table>

# DPU proc<a name="ZH-CN_TOPIC_0000002466543320"></a>

-   **[proc简介](#ZH-CN_TOPIC_0000002466543308)**  

-   **[proc信息](#ZH-CN_TOPIC_0000002499582677)**  

-   **[proc详解](#ZH-CN_TOPIC_0000002499582665)**  

## proc简介<a name="ZH-CN_TOPIC_0000002466543308"></a>

支持查询DPU层的相关信息，并且支持实时打印当前送显帧率与TE帧率等。

## proc信息<a name="ZH-CN_TOPIC_0000002499582677"></a>

下发命令：

AT+GPU=dpu\_proc,0/1

AT+GPU=dpu\_print,0/1

DPU模块的proc信息如[图1](#zh-cn_topic_0000002444726577_fig10779981098)所示。

**图 1**  DPU proc信息<a name="zh-cn_topic_0000002444726577_fig10779981098"></a>  
![](figures/DPU-proc信息.png "DPU-proc信息")

![](figures/zh-cn_image_0000002483751576.png)

## proc详解<a name="ZH-CN_TOPIC_0000002499582665"></a>

**表 1**  DPU proc信息说明

<a name="zh-cn_topic_0000002444646489_table413mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002444646489_row419mcpsimp"><th class="cellrowborder" valign="top" width="28.999999999999996%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002444646489_p421mcpsimp"><a name="zh-cn_topic_0000002444646489_p421mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p421mcpsimp"></a>类别</p>
</th>
<th class="cellrowborder" valign="top" width="30%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002444646489_p10923681168"><a name="zh-cn_topic_0000002444646489_p10923681168"></a><a name="zh-cn_topic_0000002444646489_p10923681168"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="41%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002444646489_p2092317817161"><a name="zh-cn_topic_0000002444646489_p2092317817161"></a><a name="zh-cn_topic_0000002444646489_p2092317817161"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002444646489_row423mcpsimp"><td class="cellrowborder" rowspan="13" valign="top" width="28.999999999999996%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p425mcpsimp"><a name="zh-cn_topic_0000002444646489_p425mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p425mcpsimp"></a>layer 0/1 proc</p>
</td>
<td class="cellrowborder" valign="top" width="30%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p427mcpsimp"><a name="zh-cn_topic_0000002444646489_p427mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p427mcpsimp"></a>create</p>
</td>
<td class="cellrowborder" valign="top" width="41%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002444646489_p429mcpsimp"><a name="zh-cn_topic_0000002444646489_p429mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p429mcpsimp"></a>layer 0/1是否创建。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row430mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p432mcpsimp"><a name="zh-cn_topic_0000002444646489_p432mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p432mcpsimp"></a>create_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p434mcpsimp"><a name="zh-cn_topic_0000002444646489_p434mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p434mcpsimp"></a>创建次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row435mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p437mcpsimp"><a name="zh-cn_topic_0000002444646489_p437mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p437mcpsimp"></a>width</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p439mcpsimp"><a name="zh-cn_topic_0000002444646489_p439mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p439mcpsimp"></a>宽。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row440mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p442mcpsimp"><a name="zh-cn_topic_0000002444646489_p442mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p442mcpsimp"></a>height</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p444mcpsimp"><a name="zh-cn_topic_0000002444646489_p444mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p444mcpsimp"></a>高。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row445mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p447mcpsimp"><a name="zh-cn_topic_0000002444646489_p447mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p447mcpsimp"></a>stride</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p1237311517367"><a name="zh-cn_topic_0000002444646489_p1237311517367"></a><a name="zh-cn_topic_0000002444646489_p1237311517367"></a>buffer行间距。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row450mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p452mcpsimp"><a name="zh-cn_topic_0000002444646489_p452mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p452mcpsimp"></a>uv_stride</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p1939417326350"><a name="zh-cn_topic_0000002444646489_p1939417326350"></a><a name="zh-cn_topic_0000002444646489_p1939417326350"></a>uv数据行间距。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row455mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p457mcpsimp"><a name="zh-cn_topic_0000002444646489_p457mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p457mcpsimp"></a>uv_offset</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p113549468358"><a name="zh-cn_topic_0000002444646489_p113549468358"></a><a name="zh-cn_topic_0000002444646489_p113549468358"></a>uv数据偏移量。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row460mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p462mcpsimp"><a name="zh-cn_topic_0000002444646489_p462mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p462mcpsimp"></a>format</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p464mcpsimp"><a name="zh-cn_topic_0000002444646489_p464mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p464mcpsimp"></a>颜色格式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row465mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p467mcpsimp"><a name="zh-cn_topic_0000002444646489_p467mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p467mcpsimp"></a>cmp_mode</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p469mcpsimp"><a name="zh-cn_topic_0000002444646489_p469mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p469mcpsimp"></a>压缩模式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row470mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p472mcpsimp"><a name="zh-cn_topic_0000002444646489_p472mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p472mcpsimp"></a>vblank</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p615032021313"><a name="zh-cn_topic_0000002444646489_p615032021313"></a><a name="zh-cn_topic_0000002444646489_p615032021313"></a>是否开启vblank。</p>
<p id="zh-cn_topic_0000002444646489_p474mcpsimp"><a name="zh-cn_topic_0000002444646489_p474mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p474mcpsimp"></a>true：单buffer同步送显。false：双buffer异步送显。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row475mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p477mcpsimp"><a name="zh-cn_topic_0000002444646489_p477mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p477mcpsimp"></a>chksum[ar/gb]</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p479mcpsimp"><a name="zh-cn_topic_0000002444646489_p479mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p479mcpsimp"></a>输入checksum值。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row480mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p482mcpsimp"><a name="zh-cn_topic_0000002444646489_p482mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p482mcpsimp"></a>disp_rect</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p484mcpsimp"><a name="zh-cn_topic_0000002444646489_p484mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p484mcpsimp"></a>显示区域。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row485mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p487mcpsimp"><a name="zh-cn_topic_0000002444646489_p487mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p487mcpsimp"></a>crop_rect</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p489mcpsimp"><a name="zh-cn_topic_0000002444646489_p489mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p489mcpsimp"></a>剪切区域。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row490mcpsimp"><td class="cellrowborder" rowspan="17" valign="top" width="28.999999999999996%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p492mcpsimp"><a name="zh-cn_topic_0000002444646489_p492mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p492mcpsimp"></a>DISP proc</p>
</td>
<td class="cellrowborder" valign="top" width="30%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p494mcpsimp"><a name="zh-cn_topic_0000002444646489_p494mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p494mcpsimp"></a>work_mode</p>
</td>
<td class="cellrowborder" valign="top" width="41%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002444646489_p496mcpsimp"><a name="zh-cn_topic_0000002444646489_p496mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p496mcpsimp"></a>是否cmd_mode。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row497mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p499mcpsimp"><a name="zh-cn_topic_0000002444646489_p499mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p499mcpsimp"></a>disp_open</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p501mcpsimp"><a name="zh-cn_topic_0000002444646489_p501mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p501mcpsimp"></a>打开标志。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row502mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p504mcpsimp"><a name="zh-cn_topic_0000002444646489_p504mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p504mcpsimp"></a>open_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p506mcpsimp"><a name="zh-cn_topic_0000002444646489_p506mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p506mcpsimp"></a>打开次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row507mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p509mcpsimp"><a name="zh-cn_topic_0000002444646489_p509mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p509mcpsimp"></a>pixel_clk</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p511mcpsimp"><a name="zh-cn_topic_0000002444646489_p511mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p511mcpsimp"></a>像素时钟。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row512mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p514mcpsimp"><a name="zh-cn_topic_0000002444646489_p514mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p514mcpsimp"></a>refresh_rate</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p516mcpsimp"><a name="zh-cn_topic_0000002444646489_p516mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p516mcpsimp"></a>刷新率。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row517mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p519mcpsimp"><a name="zh-cn_topic_0000002444646489_p519mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p519mcpsimp"></a>hact/vact</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p521mcpsimp"><a name="zh-cn_topic_0000002444646489_p521mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p521mcpsimp"></a>水平/垂直有效显示区。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row522mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p524mcpsimp"><a name="zh-cn_topic_0000002444646489_p524mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p524mcpsimp"></a>hfb/vfb</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p526mcpsimp"><a name="zh-cn_topic_0000002444646489_p526mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p526mcpsimp"></a>水平/垂直前向消隐区。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row527mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p529mcpsimp"><a name="zh-cn_topic_0000002444646489_p529mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p529mcpsimp"></a>hbb/vbb</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p531mcpsimp"><a name="zh-cn_topic_0000002444646489_p531mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p531mcpsimp"></a>水平/垂直后向消隐区。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row532mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p534mcpsimp"><a name="zh-cn_topic_0000002444646489_p534mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p534mcpsimp"></a>hpw/vpw</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p536mcpsimp"><a name="zh-cn_topic_0000002444646489_p536mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p536mcpsimp"></a>水平/垂直脉冲宽度。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row537mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p539mcpsimp"><a name="zh-cn_topic_0000002444646489_p539mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p539mcpsimp"></a>dhd0_region</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p541mcpsimp"><a name="zh-cn_topic_0000002444646489_p541mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p541mcpsimp"></a>显示区域。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row542mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p544mcpsimp"><a name="zh-cn_topic_0000002444646489_p544mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p544mcpsimp"></a>lowband_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p546mcpsimp"><a name="zh-cn_topic_0000002444646489_p546mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p546mcpsimp"></a>低带宽次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row547mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p549mcpsimp"><a name="zh-cn_topic_0000002444646489_p549mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p549mcpsimp"></a>bus_err_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p551mcpsimp"><a name="zh-cn_topic_0000002444646489_p551mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p551mcpsimp"></a>总线错误次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row552mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p554mcpsimp"><a name="zh-cn_topic_0000002444646489_p554mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p554mcpsimp"></a>te_signal</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p556mcpsimp"><a name="zh-cn_topic_0000002444646489_p556mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p556mcpsimp"></a>TE信号次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row557mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p559mcpsimp"><a name="zh-cn_topic_0000002444646489_p559mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p559mcpsimp"></a>fresh/frm_done</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p561mcpsimp"><a name="zh-cn_topic_0000002444646489_p561mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p561mcpsimp"></a>refresh次数/frm_done中断次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row562mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p564mcpsimp"><a name="zh-cn_topic_0000002444646489_p564mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p564mcpsimp"></a>draw/flip/te(t)</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p566mcpsimp"><a name="zh-cn_topic_0000002444646489_p566mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p566mcpsimp"></a>刷新/显示/TE中断的次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row567mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p569mcpsimp"><a name="zh-cn_topic_0000002444646489_p569mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p569mcpsimp"></a>suspend/resume</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p571mcpsimp"><a name="zh-cn_topic_0000002444646489_p571mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p571mcpsimp"></a>suspemd/resume次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row572mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p574mcpsimp"><a name="zh-cn_topic_0000002444646489_p574mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p574mcpsimp"></a>chksum[RGB]</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p576mcpsimp"><a name="zh-cn_topic_0000002444646489_p576mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p576mcpsimp"></a>输出checksum值。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row577mcpsimp"><td class="cellrowborder" rowspan="4" valign="top" width="28.999999999999996%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p579mcpsimp"><a name="zh-cn_topic_0000002444646489_p579mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p579mcpsimp"></a>送显接口耗时统计</p>
</td>
<td class="cellrowborder" valign="top" width="30%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p581mcpsimp"><a name="zh-cn_topic_0000002444646489_p581mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p581mcpsimp"></a>wait_frm_done(us)</p>
</td>
<td class="cellrowborder" valign="top" width="41%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002444646489_p583mcpsimp"><a name="zh-cn_topic_0000002444646489_p583mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p583mcpsimp"></a>函数wait_frm_done耗时。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row584mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p586mcpsimp"><a name="zh-cn_topic_0000002444646489_p586mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p586mcpsimp"></a>fresh_intf_cost(us)</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p588mcpsimp"><a name="zh-cn_topic_0000002444646489_p588mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p588mcpsimp"></a>函数dpu_layer_refresh耗时。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row589mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p591mcpsimp"><a name="zh-cn_topic_0000002444646489_p591mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p591mcpsimp"></a>te_interrupt(us)</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p593mcpsimp"><a name="zh-cn_topic_0000002444646489_p593mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p593mcpsimp"></a>两次TE中断的间隔时间。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444646489_row594mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002444646489_p596mcpsimp"><a name="zh-cn_topic_0000002444646489_p596mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p596mcpsimp"></a>draw/flip/te(fps)</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002444646489_p598mcpsimp"><a name="zh-cn_topic_0000002444646489_p598mcpsimp"></a><a name="zh-cn_topic_0000002444646489_p598mcpsimp"></a>刷新/显示/TE中断的帧率。</p>
</td>
</tr>
</tbody>
</table>

# JPEG proc<a name="ZH-CN_TOPIC_0000002499582661"></a>

-   **[proc简介](#ZH-CN_TOPIC_0000002466383676)**  

-   **[proc信息](#ZH-CN_TOPIC_0000002499502613)**  

-   **[proc详解](#ZH-CN_TOPIC_0000002466383688)**  

## proc简介<a name="ZH-CN_TOPIC_0000002466383676"></a>

查询JPEG模块基本信息。

## proc信息<a name="ZH-CN_TOPIC_0000002499502613"></a>

下发命令：AT+GPU=jpeg\_proc

JPEG模块的proc信息如[图1](#zh-cn_topic_0000002444726565_fig44112237915)所示。

**图 1**  JPEG proc信息<a name="zh-cn_topic_0000002444726565_fig44112237915"></a>  
![](figures/JPEG-proc信息.png "JPEG-proc信息")

## proc详解<a name="ZH-CN_TOPIC_0000002466383688"></a>

**表 1**  JPEG proc信息说明

<a name="zh-cn_topic_0000002411207304_table613mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002411207304_row619mcpsimp"><th class="cellrowborder" valign="top" width="15%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002411207304_p621mcpsimp"><a name="zh-cn_topic_0000002411207304_p621mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p621mcpsimp"></a>类别</p>
</th>
<th class="cellrowborder" valign="top" width="31%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002411207304_p11714851151617"><a name="zh-cn_topic_0000002411207304_p11714851151617"></a><a name="zh-cn_topic_0000002411207304_p11714851151617"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="54%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002411207304_p8714155141620"><a name="zh-cn_topic_0000002411207304_p8714155141620"></a><a name="zh-cn_topic_0000002411207304_p8714155141620"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002411207304_row623mcpsimp"><td class="cellrowborder" rowspan="11" valign="top" width="15%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411207304_p625mcpsimp"><a name="zh-cn_topic_0000002411207304_p625mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p625mcpsimp"></a>jpeg_proc</p>
</td>
<td class="cellrowborder" valign="top" width="31%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411207304_p627mcpsimp"><a name="zh-cn_topic_0000002411207304_p627mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p627mcpsimp"></a>open_times</p>
</td>
<td class="cellrowborder" valign="top" width="54%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411207304_p629mcpsimp"><a name="zh-cn_topic_0000002411207304_p629mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p629mcpsimp"></a>设备打开次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411207304_row630mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411207304_p632mcpsimp"><a name="zh-cn_topic_0000002411207304_p632mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p632mcpsimp"></a>close_times</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411207304_p634mcpsimp"><a name="zh-cn_topic_0000002411207304_p634mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p634mcpsimp"></a>设备关闭次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411207304_row635mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411207304_p637mcpsimp"><a name="zh-cn_topic_0000002411207304_p637mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p637mcpsimp"></a>stream_buf_size</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411207304_p639mcpsimp"><a name="zh-cn_topic_0000002411207304_p639mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p639mcpsimp"></a>流buf大小。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411207304_row640mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411207304_p642mcpsimp"><a name="zh-cn_topic_0000002411207304_p642mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p642mcpsimp"></a>data_buf_size</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411207304_p644mcpsimp"><a name="zh-cn_topic_0000002411207304_p644mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p644mcpsimp"></a>数据buf大小。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411207304_row645mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411207304_p647mcpsimp"><a name="zh-cn_topic_0000002411207304_p647mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p647mcpsimp"></a>y_buf_stride</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411207304_p649mcpsimp"><a name="zh-cn_topic_0000002411207304_p649mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p649mcpsimp"></a>y数据buf的stride。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411207304_row650mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411207304_p652mcpsimp"><a name="zh-cn_topic_0000002411207304_p652mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p652mcpsimp"></a>y_mcu_height</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411207304_p654mcpsimp"><a name="zh-cn_topic_0000002411207304_p654mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p654mcpsimp"></a>y MCU的高。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411207304_row655mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411207304_p657mcpsimp"><a name="zh-cn_topic_0000002411207304_p657mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p657mcpsimp"></a>uv_buf_stride</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411207304_p659mcpsimp"><a name="zh-cn_topic_0000002411207304_p659mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p659mcpsimp"></a>uv数据buf的stride。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411207304_row660mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411207304_p662mcpsimp"><a name="zh-cn_topic_0000002411207304_p662mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p662mcpsimp"></a>uv_mcu_height</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411207304_p664mcpsimp"><a name="zh-cn_topic_0000002411207304_p664mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p664mcpsimp"></a>uv MCU的高。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411207304_row665mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411207304_p667mcpsimp"><a name="zh-cn_topic_0000002411207304_p667mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p667mcpsimp"></a>jpeg_color_space</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411207304_p669mcpsimp"><a name="zh-cn_topic_0000002411207304_p669mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p669mcpsimp"></a>输入颜色格式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411207304_row670mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411207304_p672mcpsimp"><a name="zh-cn_topic_0000002411207304_p672mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p672mcpsimp"></a>output_color_space</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411207304_p674mcpsimp"><a name="zh-cn_topic_0000002411207304_p674mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p674mcpsimp"></a>输出颜色格式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411207304_row675mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411207304_p677mcpsimp"><a name="zh-cn_topic_0000002411207304_p677mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p677mcpsimp"></a>scale</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411207304_p679mcpsimp"><a name="zh-cn_topic_0000002411207304_p679mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p679mcpsimp"></a>缩放系数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411207304_row680mcpsimp"><td class="cellrowborder" rowspan="3" valign="top" width="15%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411207304_p682mcpsimp"><a name="zh-cn_topic_0000002411207304_p682mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p682mcpsimp"></a>time info</p>
</td>
<td class="cellrowborder" valign="top" width="31%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411207304_p684mcpsimp"><a name="zh-cn_topic_0000002411207304_p684mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p684mcpsimp"></a>init_cost(us)</p>
</td>
<td class="cellrowborder" valign="top" width="54%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411207304_p686mcpsimp"><a name="zh-cn_topic_0000002411207304_p686mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p686mcpsimp"></a>函数jpeg_ioctl_decompress_init耗时。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411207304_row687mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411207304_p689mcpsimp"><a name="zh-cn_topic_0000002411207304_p689mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p689mcpsimp"></a>start_cost(us)</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411207304_p691mcpsimp"><a name="zh-cn_topic_0000002411207304_p691mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p691mcpsimp"></a>函数jpeg_ioctl_start_decompress耗时。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411207304_row692mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411207304_p694mcpsimp"><a name="zh-cn_topic_0000002411207304_p694mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p694mcpsimp"></a>get_status_cost(us)</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411207304_p696mcpsimp"><a name="zh-cn_topic_0000002411207304_p696mcpsimp"></a><a name="zh-cn_topic_0000002411207304_p696mcpsimp"></a>函数jpeg_ioctl_get_decompress_status耗时。</p>
</td>
</tr>
</tbody>
</table>

# VENC<a name="ZH-CN_TOPIC_0000002499502625"></a>

-   **[proc简介](#ZH-CN_TOPIC_0000002466383692)**  

-   **[proc信息](#ZH-CN_TOPIC_0000002466383672)**  

-   **[proc详解](#ZH-CN_TOPIC_0000002499582673)**  

## proc简介<a name="ZH-CN_TOPIC_0000002466383692"></a>

查询VENC（Video Encoder）模块基本信息。

## proc信息<a name="ZH-CN_TOPIC_0000002466383672"></a>

下发命令：AT+VENC=venc\_proc,cat,venc

VENC模块proc信息如[图1](#zh-cn_topic_0000002484810301_fig14325145711811)所示。

**图 1**  VENC proc信息<a name="zh-cn_topic_0000002484810301_fig14325145711811"></a>  
![](figures/VENC-proc信息.png "VENC-proc信息")

## proc详解<a name="ZH-CN_TOPIC_0000002499582673"></a>

**表 1**  VENC proc信息说明

<a name="zh-cn_topic_0000002451730862_table613mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002451730862_row619mcpsimp"><th class="cellrowborder" valign="top" width="18.22%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002451730862_p621mcpsimp"><a name="zh-cn_topic_0000002451730862_p621mcpsimp"></a><a name="zh-cn_topic_0000002451730862_p621mcpsimp"></a>类别</p>
</th>
<th class="cellrowborder" valign="top" width="27.83%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002451730862_p11714851151617"><a name="zh-cn_topic_0000002451730862_p11714851151617"></a><a name="zh-cn_topic_0000002451730862_p11714851151617"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="53.949999999999996%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002451730862_p8714155141620"><a name="zh-cn_topic_0000002451730862_p8714155141620"></a><a name="zh-cn_topic_0000002451730862_p8714155141620"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002451730862_row623mcpsimp"><td class="cellrowborder" rowspan="2" valign="top" width="18.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p18768248284"><a name="zh-cn_topic_0000002451730862_p18768248284"></a><a name="zh-cn_topic_0000002451730862_p18768248284"></a>global info</p>
</td>
<td class="cellrowborder" valign="top" width="27.83%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p182176317285"><a name="zh-cn_topic_0000002451730862_p182176317285"></a><a name="zh-cn_topic_0000002451730862_p182176317285"></a>version</p>
</td>
<td class="cellrowborder" valign="top" width="53.949999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002451730862_p1068994322810"><a name="zh-cn_topic_0000002451730862_p1068994322810"></a><a name="zh-cn_topic_0000002451730862_p1068994322810"></a>版本号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row630mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p32171431102814"><a name="zh-cn_topic_0000002451730862_p32171431102814"></a><a name="zh-cn_topic_0000002451730862_p32171431102814"></a>max_chn_num</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1786511324282"><a name="zh-cn_topic_0000002451730862_p1786511324282"></a><a name="zh-cn_topic_0000002451730862_p1786511324282"></a>最大编码通道数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row680mcpsimp"><td class="cellrowborder" rowspan="10" valign="top" width="18.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p06411337133017"><a name="zh-cn_topic_0000002451730862_p06411337133017"></a><a name="zh-cn_topic_0000002451730862_p06411337133017"></a>chn attr</p>
</td>
<td class="cellrowborder" valign="top" width="27.83%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p9944339113019"><a name="zh-cn_topic_0000002451730862_p9944339113019"></a><a name="zh-cn_topic_0000002451730862_p9944339113019"></a>id</p>
</td>
<td class="cellrowborder" valign="top" width="53.949999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002451730862_p194414399308"><a name="zh-cn_topic_0000002451730862_p194414399308"></a><a name="zh-cn_topic_0000002451730862_p194414399308"></a>编码通道号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row687mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p13944339113012"><a name="zh-cn_topic_0000002451730862_p13944339113012"></a><a name="zh-cn_topic_0000002451730862_p13944339113012"></a>standard</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p59431392309"><a name="zh-cn_topic_0000002451730862_p59431392309"></a><a name="zh-cn_topic_0000002451730862_p59431392309"></a>编码通道协议。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1712996222"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p12769109220"><a name="zh-cn_topic_0000002451730862_p12769109220"></a><a name="zh-cn_topic_0000002451730862_p12769109220"></a>profile</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p15129461226"><a name="zh-cn_topic_0000002451730862_p15129461226"></a><a name="zh-cn_topic_0000002451730862_p15129461226"></a>编码协议档次。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row53611983410"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p73611698340"><a name="zh-cn_topic_0000002451730862_p73611698340"></a><a name="zh-cn_topic_0000002451730862_p73611698340"></a>max_width</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p03617917341"><a name="zh-cn_topic_0000002451730862_p03617917341"></a><a name="zh-cn_topic_0000002451730862_p03617917341"></a>通道最大宽度，以像素为单位。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row12360453933"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p136005316310"><a name="zh-cn_topic_0000002451730862_p136005316310"></a><a name="zh-cn_topic_0000002451730862_p136005316310"></a>max_height</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p93603531319"><a name="zh-cn_topic_0000002451730862_p93603531319"></a><a name="zh-cn_topic_0000002451730862_p93603531319"></a>通道最大高度，以像素为单位。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row163061914163413"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p193061214183417"><a name="zh-cn_topic_0000002451730862_p193061214183417"></a><a name="zh-cn_topic_0000002451730862_p193061214183417"></a>width</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p2030641433415"><a name="zh-cn_topic_0000002451730862_p2030641433415"></a><a name="zh-cn_topic_0000002451730862_p2030641433415"></a>图像宽度，以像素为单位。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1896113618415"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p42463401347"><a name="zh-cn_topic_0000002451730862_p42463401347"></a><a name="zh-cn_topic_0000002451730862_p42463401347"></a>height</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p5961123615415"><a name="zh-cn_topic_0000002451730862_p5961123615415"></a><a name="zh-cn_topic_0000002451730862_p5961123615415"></a>图像高度，以像素为单位。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row230617140343"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p93061143344"><a name="zh-cn_topic_0000002451730862_p93061143344"></a><a name="zh-cn_topic_0000002451730862_p93061143344"></a>buf_size</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p115384834013"><a name="zh-cn_topic_0000002451730862_p115384834013"></a><a name="zh-cn_topic_0000002451730862_p115384834013"></a>码流缓存大小。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row153061314133416"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1230631473416"><a name="zh-cn_topic_0000002451730862_p1230631473416"></a><a name="zh-cn_topic_0000002451730862_p1230631473416"></a>max_usd</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p6306014173411"><a name="zh-cn_topic_0000002451730862_p6306014173411"></a><a name="zh-cn_topic_0000002451730862_p6306014173411"></a>最大用户数据长度。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row692mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p14943939113017"><a name="zh-cn_topic_0000002451730862_p14943939113017"></a><a name="zh-cn_topic_0000002451730862_p14943939113017"></a>power_mode</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1943183914307"><a name="zh-cn_topic_0000002451730862_p1943183914307"></a><a name="zh-cn_topic_0000002451730862_p1943183914307"></a>编码功耗模式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row226155171616"><td class="cellrowborder" rowspan="10" valign="top" width="18.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p4731105017362"><a name="zh-cn_topic_0000002451730862_p4731105017362"></a><a name="zh-cn_topic_0000002451730862_p4731105017362"></a>chn info</p>
</td>
<td class="cellrowborder" valign="top" width="27.83%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p62611351151618"><a name="zh-cn_topic_0000002451730862_p62611351151618"></a><a name="zh-cn_topic_0000002451730862_p62611351151618"></a>id</p>
</td>
<td class="cellrowborder" valign="top" width="53.949999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002451730862_p1773417719177"><a name="zh-cn_topic_0000002451730862_p1773417719177"></a><a name="zh-cn_topic_0000002451730862_p1773417719177"></a>编码通道号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1164318318357"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1464493117358"><a name="zh-cn_topic_0000002451730862_p1464493117358"></a><a name="zh-cn_topic_0000002451730862_p1464493117358"></a>start</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p126441831183516"><a name="zh-cn_topic_0000002451730862_p126441831183516"></a><a name="zh-cn_topic_0000002451730862_p126441831183516"></a>编码通道是否启动。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row76441731153510"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p137465219557"><a name="zh-cn_topic_0000002451730862_p137465219557"></a><a name="zh-cn_topic_0000002451730862_p137465219557"></a>snap_en</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1464418315359"><a name="zh-cn_topic_0000002451730862_p1464418315359"></a><a name="zh-cn_topic_0000002451730862_p1464418315359"></a>抓拍是否使能。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1431217191616"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p159141910973"><a name="zh-cn_topic_0000002451730862_p159141910973"></a><a name="zh-cn_topic_0000002451730862_p159141910973"></a>snapping</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p53121719169"><a name="zh-cn_topic_0000002451730862_p53121719169"></a><a name="zh-cn_topic_0000002451730862_p53121719169"></a>是否正在抓拍。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row12644631103518"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p5644133114353"><a name="zh-cn_topic_0000002451730862_p5644133114353"></a><a name="zh-cn_topic_0000002451730862_p5644133114353"></a>recv_left</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1064473173513"><a name="zh-cn_topic_0000002451730862_p1064473173513"></a><a name="zh-cn_topic_0000002451730862_p1064473173513"></a>剩余接收帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row5638923068"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p17692023715"><a name="zh-cn_topic_0000002451730862_p17692023715"></a><a name="zh-cn_topic_0000002451730862_p17692023715"></a>enc_left</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1363932312612"><a name="zh-cn_topic_0000002451730862_p1363932312612"></a><a name="zh-cn_topic_0000002451730862_p1363932312612"></a>剩余编码帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row192321226183515"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p102332026173512"><a name="zh-cn_topic_0000002451730862_p102332026173512"></a><a name="zh-cn_topic_0000002451730862_p102332026173512"></a>buf_full</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p14233112617354"><a name="zh-cn_topic_0000002451730862_p14233112617354"></a><a name="zh-cn_topic_0000002451730862_p14233112617354"></a>码流buf满次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row159181727567"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p199211252366"><a name="zh-cn_topic_0000002451730862_p199211252366"></a><a name="zh-cn_topic_0000002451730862_p199211252366"></a>time_out</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1491862717616"><a name="zh-cn_topic_0000002451730862_p1491862717616"></a><a name="zh-cn_topic_0000002451730862_p1491862717616"></a>硬件超时次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1491819321960"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1791815322612"><a name="zh-cn_topic_0000002451730862_p1791815322612"></a><a name="zh-cn_topic_0000002451730862_p1791815322612"></a>wbus_err</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p2918232865"><a name="zh-cn_topic_0000002451730862_p2918232865"></a><a name="zh-cn_topic_0000002451730862_p2918232865"></a>写总线错误次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1423362610356"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p623315265352"><a name="zh-cn_topic_0000002451730862_p623315265352"></a><a name="zh-cn_topic_0000002451730862_p623315265352"></a>rbus_err</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1623322663517"><a name="zh-cn_topic_0000002451730862_p1623322663517"></a><a name="zh-cn_topic_0000002451730862_p1623322663517"></a>读总线错误次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row15519719131719"><td class="cellrowborder" rowspan="10" valign="top" width="18.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p106271237193018"><a name="zh-cn_topic_0000002451730862_p106271237193018"></a><a name="zh-cn_topic_0000002451730862_p106271237193018"></a>chn status</p>
</td>
<td class="cellrowborder" valign="top" width="27.83%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p45192019141712"><a name="zh-cn_topic_0000002451730862_p45192019141712"></a><a name="zh-cn_topic_0000002451730862_p45192019141712"></a>id</p>
</td>
<td class="cellrowborder" valign="top" width="53.949999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002451730862_p48171925101715"><a name="zh-cn_topic_0000002451730862_p48171925101715"></a><a name="zh-cn_topic_0000002451730862_p48171925101715"></a>编码通道号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row63429345306"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p19941193910304"><a name="zh-cn_topic_0000002451730862_p19941193910304"></a><a name="zh-cn_topic_0000002451730862_p19941193910304"></a>get_frame</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p094113916306"><a name="zh-cn_topic_0000002451730862_p094113916306"></a><a name="zh-cn_topic_0000002451730862_p094113916306"></a>收帧次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row0677336576"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p2489840578"><a name="zh-cn_topic_0000002451730862_p2489840578"></a><a name="zh-cn_topic_0000002451730862_p2489840578"></a>free_frame</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p26779369714"><a name="zh-cn_topic_0000002451730862_p26779369714"></a><a name="zh-cn_topic_0000002451730862_p26779369714"></a>还帧次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1242852615363"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p15429112673613"><a name="zh-cn_topic_0000002451730862_p15429112673613"></a><a name="zh-cn_topic_0000002451730862_p15429112673613"></a>slice_int</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p124297267369"><a name="zh-cn_topic_0000002451730862_p124297267369"></a><a name="zh-cn_topic_0000002451730862_p124297267369"></a>slice中断次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row184291726103613"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p642919267363"><a name="zh-cn_topic_0000002451730862_p642919267363"></a><a name="zh-cn_topic_0000002451730862_p642919267363"></a>wait_enc</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p15429122603616"><a name="zh-cn_topic_0000002451730862_p15429122603616"></a><a name="zh-cn_topic_0000002451730862_p15429122603616"></a>等待编码帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row104291526133617"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p442917267368"><a name="zh-cn_topic_0000002451730862_p442917267368"></a><a name="zh-cn_topic_0000002451730862_p442917267368"></a>left_recv</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p3429182683611"><a name="zh-cn_topic_0000002451730862_p3429182683611"></a><a name="zh-cn_topic_0000002451730862_p3429182683611"></a>剩余接收帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row18209618286"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1075511219811"><a name="zh-cn_topic_0000002451730862_p1075511219811"></a><a name="zh-cn_topic_0000002451730862_p1075511219811"></a>left_enc</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p15550924281"><a name="zh-cn_topic_0000002451730862_p15550924281"></a><a name="zh-cn_topic_0000002451730862_p15550924281"></a>剩余编码帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row75839228366"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p105831229368"><a name="zh-cn_topic_0000002451730862_p105831229368"></a><a name="zh-cn_topic_0000002451730862_p105831229368"></a>wait_bytes</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p258392212368"><a name="zh-cn_topic_0000002451730862_p258392212368"></a><a name="zh-cn_topic_0000002451730862_p258392212368"></a>等待收走码流大小。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row255233117819"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p555393119813"><a name="zh-cn_topic_0000002451730862_p555393119813"></a><a name="zh-cn_topic_0000002451730862_p555393119813"></a>wait_pics</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p135531311483"><a name="zh-cn_topic_0000002451730862_p135531311483"></a><a name="zh-cn_topic_0000002451730862_p135531311483"></a>等待收走码流帧数</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row3342113415306"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p3941839183019"><a name="zh-cn_topic_0000002451730862_p3941839183019"></a><a name="zh-cn_topic_0000002451730862_p3941839183019"></a>cur_packs</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p79401739103019"><a name="zh-cn_topic_0000002451730862_p79401739103019"></a><a name="zh-cn_topic_0000002451730862_p79401739103019"></a>当前帧码流包数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row104983515172"><td class="cellrowborder" rowspan="10" valign="top" width="18.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1841494793111"><a name="zh-cn_topic_0000002451730862_p1841494793111"></a><a name="zh-cn_topic_0000002451730862_p1841494793111"></a>work status</p>
</td>
<td class="cellrowborder" valign="top" width="27.83%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1149635161716"><a name="zh-cn_topic_0000002451730862_p1149635161716"></a><a name="zh-cn_topic_0000002451730862_p1149635161716"></a>id</p>
</td>
<td class="cellrowborder" valign="top" width="53.949999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002451730862_p5910124013173"><a name="zh-cn_topic_0000002451730862_p5910124013173"></a><a name="zh-cn_topic_0000002451730862_p5910124013173"></a>编码通道号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row9189436183112"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1912335153111"><a name="zh-cn_topic_0000002451730862_p1912335153111"></a><a name="zh-cn_topic_0000002451730862_p1912335153111"></a>query</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p121220355310"><a name="zh-cn_topic_0000002451730862_p121220355310"></a><a name="zh-cn_topic_0000002451730862_p121220355310"></a>查询任务次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row630844786"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p9830553089"><a name="zh-cn_topic_0000002451730862_p9830553089"></a><a name="zh-cn_topic_0000002451730862_p9830553089"></a>query_ok</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p930244987"><a name="zh-cn_topic_0000002451730862_p930244987"></a><a name="zh-cn_topic_0000002451730862_p930244987"></a>查询成功次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row6189173673112"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p111223514315"><a name="zh-cn_topic_0000002451730862_p111223514315"></a><a name="zh-cn_topic_0000002451730862_p111223514315"></a>start</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p201243513317"><a name="zh-cn_topic_0000002451730862_p201243513317"></a><a name="zh-cn_topic_0000002451730862_p201243513317"></a>启动编码次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row32656470810"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1438114211915"><a name="zh-cn_topic_0000002451730862_p1438114211915"></a><a name="zh-cn_topic_0000002451730862_p1438114211915"></a>start_ok</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p92659471819"><a name="zh-cn_topic_0000002451730862_p92659471819"></a><a name="zh-cn_topic_0000002451730862_p92659471819"></a>启动成功次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row124715331521"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1912735113120"><a name="zh-cn_topic_0000002451730862_p1912735113120"></a><a name="zh-cn_topic_0000002451730862_p1912735113120"></a>config</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p10471533125210"><a name="zh-cn_topic_0000002451730862_p10471533125210"></a><a name="zh-cn_topic_0000002451730862_p10471533125210"></a>配置硬件次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row183413501583"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p325415101919"><a name="zh-cn_topic_0000002451730862_p325415101919"></a><a name="zh-cn_topic_0000002451730862_p325415101919"></a>interrupt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1983414507819"><a name="zh-cn_topic_0000002451730862_p1983414507819"></a><a name="zh-cn_topic_0000002451730862_p1983414507819"></a>硬件中断次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1312752945211"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p20127142975215"><a name="zh-cn_topic_0000002451730862_p20127142975215"></a><a name="zh-cn_topic_0000002451730862_p20127142975215"></a>ch_reso_lost</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1412732945215"><a name="zh-cn_topic_0000002451730862_p1412732945215"></a><a name="zh-cn_topic_0000002451730862_p1412732945215"></a>分辨率变化次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row121991539528"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p18199175315521"><a name="zh-cn_topic_0000002451730862_p18199175315521"></a><a name="zh-cn_topic_0000002451730862_p18199175315521"></a>rc_discard</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p11199653105216"><a name="zh-cn_topic_0000002451730862_p11199653105216"></a><a name="zh-cn_topic_0000002451730862_p11199653105216"></a>码控丢帧次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1020525005213"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p7205950195210"><a name="zh-cn_topic_0000002451730862_p7205950195210"></a><a name="zh-cn_topic_0000002451730862_p7205950195210"></a>status</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p17205165035213"><a name="zh-cn_topic_0000002451730862_p17205165035213"></a><a name="zh-cn_topic_0000002451730862_p17205165035213"></a>编码任务状态。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row2883195314175"><td class="cellrowborder" rowspan="7" valign="top" width="18.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p751315514318"><a name="zh-cn_topic_0000002451730862_p751315514318"></a><a name="zh-cn_topic_0000002451730862_p751315514318"></a>frame rate</p>
</td>
<td class="cellrowborder" valign="top" width="27.83%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1688305315177"><a name="zh-cn_topic_0000002451730862_p1688305315177"></a><a name="zh-cn_topic_0000002451730862_p1688305315177"></a>id</p>
</td>
<td class="cellrowborder" valign="top" width="53.949999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002451730862_p954813010187"><a name="zh-cn_topic_0000002451730862_p954813010187"></a><a name="zh-cn_topic_0000002451730862_p954813010187"></a>编码通道号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row16129439113113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p51439381312"><a name="zh-cn_topic_0000002451730862_p51439381312"></a><a name="zh-cn_topic_0000002451730862_p51439381312"></a>src_fps</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p12143133833120"><a name="zh-cn_topic_0000002451730862_p12143133833120"></a><a name="zh-cn_topic_0000002451730862_p12143133833120"></a>源帧率。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row187723491914"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p163113562912"><a name="zh-cn_topic_0000002451730862_p163113562912"></a><a name="zh-cn_topic_0000002451730862_p163113562912"></a>dst_fps</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p367583121020"><a name="zh-cn_topic_0000002451730862_p367583121020"></a><a name="zh-cn_topic_0000002451730862_p367583121020"></a>目标帧率。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row99760452920"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1997613451297"><a name="zh-cn_topic_0000002451730862_p1997613451297"></a><a name="zh-cn_topic_0000002451730862_p1997613451297"></a>real_fps</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p129762457917"><a name="zh-cn_topic_0000002451730862_p129762457917"></a><a name="zh-cn_topic_0000002451730862_p129762457917"></a>实际帧率。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row17517516419"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p185117516415"><a name="zh-cn_topic_0000002451730862_p185117516415"></a><a name="zh-cn_topic_0000002451730862_p185117516415"></a>first_pts</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p10514512044"><a name="zh-cn_topic_0000002451730862_p10514512044"></a><a name="zh-cn_topic_0000002451730862_p10514512044"></a>第1帧帧时间戳。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row0618950141019"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p294465315101"><a name="zh-cn_topic_0000002451730862_p294465315101"></a><a name="zh-cn_topic_0000002451730862_p294465315101"></a>cur_pts</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p04601857131015"><a name="zh-cn_topic_0000002451730862_p04601857131015"></a><a name="zh-cn_topic_0000002451730862_p04601857131015"></a>当前帧时间戳。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row3129139133112"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p193001035822"><a name="zh-cn_topic_0000002451730862_p193001035822"></a><a name="zh-cn_topic_0000002451730862_p193001035822"></a>time_cost</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p81438381314"><a name="zh-cn_topic_0000002451730862_p81438381314"></a><a name="zh-cn_topic_0000002451730862_p81438381314"></a>编码耗时（单位：μs）。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row206821011131814"><td class="cellrowborder" rowspan="10" valign="top" width="18.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p12061884324"><a name="zh-cn_topic_0000002451730862_p12061884324"></a><a name="zh-cn_topic_0000002451730862_p12061884324"></a>frame status</p>
</td>
<td class="cellrowborder" valign="top" width="27.83%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p36821611161814"><a name="zh-cn_topic_0000002451730862_p36821611161814"></a><a name="zh-cn_topic_0000002451730862_p36821611161814"></a>id</p>
</td>
<td class="cellrowborder" valign="top" width="53.949999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002451730862_p205321718141816"><a name="zh-cn_topic_0000002451730862_p205321718141816"></a><a name="zh-cn_topic_0000002451730862_p205321718141816"></a>编码通道号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row655518073216"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p3356125943117"><a name="zh-cn_topic_0000002451730862_p3356125943117"></a><a name="zh-cn_topic_0000002451730862_p3356125943117"></a>user</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p13567596313"><a name="zh-cn_topic_0000002451730862_p13567596313"></a><a name="zh-cn_topic_0000002451730862_p13567596313"></a>用户送帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1604197111111"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p2720610111118"><a name="zh-cn_topic_0000002451730862_p2720610111118"></a><a name="zh-cn_topic_0000002451730862_p2720610111118"></a>user_err</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p4604672111"><a name="zh-cn_topic_0000002451730862_p4604672111"></a><a name="zh-cn_topic_0000002451730862_p4604672111"></a>用户送帧错误数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1855414053212"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p153561459193112"><a name="zh-cn_topic_0000002451730862_p153561459193112"></a><a name="zh-cn_topic_0000002451730862_p153561459193112"></a>auto</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p153561859113117"><a name="zh-cn_topic_0000002451730862_p153561859113117"></a><a name="zh-cn_topic_0000002451730862_p153561859113117"></a>前级送帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row14925171413115"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p20928717171111"><a name="zh-cn_topic_0000002451730862_p20928717171111"></a><a name="zh-cn_topic_0000002451730862_p20928717171111"></a>auto_err</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p19926131461112"><a name="zh-cn_topic_0000002451730862_p19926131461112"></a><a name="zh-cn_topic_0000002451730862_p19926131461112"></a>前级送帧错误数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1717818393714"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p191785396712"><a name="zh-cn_topic_0000002451730862_p191785396712"></a><a name="zh-cn_topic_0000002451730862_p191785396712"></a>send</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p15918021101110"><a name="zh-cn_topic_0000002451730862_p15918021101110"></a><a name="zh-cn_topic_0000002451730862_p15918021101110"></a>正确发送图像帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row17741201082"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p55919271483"><a name="zh-cn_topic_0000002451730862_p55919271483"></a><a name="zh-cn_topic_0000002451730862_p55919271483"></a>stop</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p87416019819"><a name="zh-cn_topic_0000002451730862_p87416019819"></a><a name="zh-cn_topic_0000002451730862_p87416019819"></a>停止通道丢帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row481911526119"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1574015517114"><a name="zh-cn_topic_0000002451730862_p1574015517114"></a><a name="zh-cn_topic_0000002451730862_p1574015517114"></a>full</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p178191152161110"><a name="zh-cn_topic_0000002451730862_p178191152161110"></a><a name="zh-cn_topic_0000002451730862_p178191152161110"></a>码流buf满丢帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row15615033819"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p126151231982"><a name="zh-cn_topic_0000002451730862_p126151231982"></a><a name="zh-cn_topic_0000002451730862_p126151231982"></a>direct</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p106151031888"><a name="zh-cn_topic_0000002451730862_p106151031888"></a><a name="zh-cn_topic_0000002451730862_p106151031888"></a>直接送编码队列帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row15554208327"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p03574595311"><a name="zh-cn_topic_0000002451730862_p03574595311"></a><a name="zh-cn_topic_0000002451730862_p03574595311"></a>size_err</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p113575596315"><a name="zh-cn_topic_0000002451730862_p113575596315"></a><a name="zh-cn_topic_0000002451730862_p113575596315"></a>分辨率错误帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row335012308180"><td class="cellrowborder" rowspan="7" valign="top" width="18.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p157611514183217"><a name="zh-cn_topic_0000002451730862_p157611514183217"></a><a name="zh-cn_topic_0000002451730862_p157611514183217"></a>frame info</p>
</td>
<td class="cellrowborder" valign="top" width="27.83%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1068643471817"><a name="zh-cn_topic_0000002451730862_p1068643471817"></a><a name="zh-cn_topic_0000002451730862_p1068643471817"></a>id</p>
</td>
<td class="cellrowborder" valign="top" width="53.949999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002451730862_p1935115303188"><a name="zh-cn_topic_0000002451730862_p1935115303188"></a><a name="zh-cn_topic_0000002451730862_p1935115303188"></a>编码通道号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1733173183211"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p9218126327"><a name="zh-cn_topic_0000002451730862_p9218126327"></a><a name="zh-cn_topic_0000002451730862_p9218126327"></a>free</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p321812217324"><a name="zh-cn_topic_0000002451730862_p321812217324"></a><a name="zh-cn_topic_0000002451730862_p321812217324"></a>原图空闲队列帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row45044551216"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p99411212141212"><a name="zh-cn_topic_0000002451730862_p99411212141212"></a><a name="zh-cn_topic_0000002451730862_p99411212141212"></a>busy</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p279192313125"><a name="zh-cn_topic_0000002451730862_p279192313125"></a><a name="zh-cn_topic_0000002451730862_p279192313125"></a>原图有效队列帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row33319310328"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1521818283213"><a name="zh-cn_topic_0000002451730862_p1521818283213"></a><a name="zh-cn_topic_0000002451730862_p1521818283213"></a>thumb_free</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p142189293215"><a name="zh-cn_topic_0000002451730862_p142189293215"></a><a name="zh-cn_topic_0000002451730862_p142189293215"></a>缩略图空闲队列帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row875739141210"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p434774120121"><a name="zh-cn_topic_0000002451730862_p434774120121"></a><a name="zh-cn_topic_0000002451730862_p434774120121"></a>thumb_busy</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p47578911212"><a name="zh-cn_topic_0000002451730862_p47578911212"></a><a name="zh-cn_topic_0000002451730862_p47578911212"></a>缩略图有效队列帧数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row10462164314154"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1446274311520"><a name="zh-cn_topic_0000002451730862_p1446274311520"></a><a name="zh-cn_topic_0000002451730862_p1446274311520"></a>time_ref</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p54626437151"><a name="zh-cn_topic_0000002451730862_p54626437151"></a><a name="zh-cn_topic_0000002451730862_p54626437151"></a>最新输入图像时间戳。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row13462114341517"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p446217432156"><a name="zh-cn_topic_0000002451730862_p446217432156"></a><a name="zh-cn_topic_0000002451730862_p446217432156"></a>format</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p246234301514"><a name="zh-cn_topic_0000002451730862_p246234301514"></a><a name="zh-cn_topic_0000002451730862_p246234301514"></a>最新输入图像格式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row22821047151818"><td class="cellrowborder" rowspan="7" valign="top" width="18.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p168617269321"><a name="zh-cn_topic_0000002451730862_p168617269321"></a><a name="zh-cn_topic_0000002451730862_p168617269321"></a>stream status</p>
</td>
<td class="cellrowborder" valign="top" width="27.83%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p867350121813"><a name="zh-cn_topic_0000002451730862_p867350121813"></a><a name="zh-cn_topic_0000002451730862_p867350121813"></a>id</p>
</td>
<td class="cellrowborder" valign="top" width="53.949999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002451730862_p20424195214181"><a name="zh-cn_topic_0000002451730862_p20424195214181"></a><a name="zh-cn_topic_0000002451730862_p20424195214181"></a>编码通道号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row116956191325"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1057714182329"><a name="zh-cn_topic_0000002451730862_p1057714182329"></a><a name="zh-cn_topic_0000002451730862_p1057714182329"></a>free_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p14577818183217"><a name="zh-cn_topic_0000002451730862_p14577818183217"></a><a name="zh-cn_topic_0000002451730862_p14577818183217"></a>码流buffer中空闲节点数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row3790282267"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p117901892610"><a name="zh-cn_topic_0000002451730862_p117901892610"></a><a name="zh-cn_topic_0000002451730862_p117901892610"></a>busy_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1579019802618"><a name="zh-cn_topic_0000002451730862_p1579019802618"></a><a name="zh-cn_topic_0000002451730862_p1579019802618"></a>码流buffer中有效节点数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row7483195112615"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p184781514182617"><a name="zh-cn_topic_0000002451730862_p184781514182617"></a><a name="zh-cn_topic_0000002451730862_p184781514182617"></a>user_cnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p104833522619"><a name="zh-cn_topic_0000002451730862_p104833522619"></a><a name="zh-cn_topic_0000002451730862_p104833522619"></a>码流buffer中用户节点数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row12695151923213"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p557711814322"><a name="zh-cn_topic_0000002451730862_p557711814322"></a><a name="zh-cn_topic_0000002451730862_p557711814322"></a>user_get</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p757701893219"><a name="zh-cn_topic_0000002451730862_p757701893219"></a><a name="zh-cn_topic_0000002451730862_p757701893219"></a>获取码流的码流包数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row11246195692518"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p327320002615"><a name="zh-cn_topic_0000002451730862_p327320002615"></a><a name="zh-cn_topic_0000002451730862_p327320002615"></a>user_rel</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p724611562252"><a name="zh-cn_topic_0000002451730862_p724611562252"></a><a name="zh-cn_topic_0000002451730862_p724611562252"></a>释放码流的码流包数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1269561903217"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p357751873219"><a name="zh-cn_topic_0000002451730862_p357751873219"></a><a name="zh-cn_topic_0000002451730862_p357751873219"></a>get_times</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p8577418133212"><a name="zh-cn_topic_0000002451730862_p8577418133212"></a><a name="zh-cn_topic_0000002451730862_p8577418133212"></a>用户获取码流的次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row133211513171911"><td class="cellrowborder" rowspan="9" valign="top" width="18.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p135922933213"><a name="zh-cn_topic_0000002451730862_p135922933213"></a><a name="zh-cn_topic_0000002451730862_p135922933213"></a>stream info</p>
</td>
<td class="cellrowborder" valign="top" width="27.83%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1781981601911"><a name="zh-cn_topic_0000002451730862_p1781981601911"></a><a name="zh-cn_topic_0000002451730862_p1781981601911"></a>id</p>
</td>
<td class="cellrowborder" valign="top" width="53.949999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002451730862_p9973151816197"><a name="zh-cn_topic_0000002451730862_p9973151816197"></a><a name="zh-cn_topic_0000002451730862_p9973151816197"></a>编码通道号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1929418218323"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p2294152153214"><a name="zh-cn_topic_0000002451730862_p2294152153214"></a><a name="zh-cn_topic_0000002451730862_p2294152153214"></a>base</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1229422193215"><a name="zh-cn_topic_0000002451730862_p1229422193215"></a><a name="zh-cn_topic_0000002451730862_p1229422193215"></a>码流buf基地址。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1871345415354"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p47137548356"><a name="zh-cn_topic_0000002451730862_p47137548356"></a><a name="zh-cn_topic_0000002451730862_p47137548356"></a>read_tail</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1371311541359"><a name="zh-cn_topic_0000002451730862_p1371311541359"></a><a name="zh-cn_topic_0000002451730862_p1371311541359"></a>码流buf读尾指针。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row5329103020135"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p163299303131"><a name="zh-cn_topic_0000002451730862_p163299303131"></a><a name="zh-cn_topic_0000002451730862_p163299303131"></a>read_head</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1784833611316"><a name="zh-cn_topic_0000002451730862_p1784833611316"></a><a name="zh-cn_topic_0000002451730862_p1784833611316"></a>码流buf读头指针。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1692725113357"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p09275518351"><a name="zh-cn_topic_0000002451730862_p09275518351"></a><a name="zh-cn_topic_0000002451730862_p09275518351"></a>write_tail</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p486530153818"><a name="zh-cn_topic_0000002451730862_p486530153818"></a><a name="zh-cn_topic_0000002451730862_p486530153818"></a>码流buf写尾指针。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row89161244151311"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1291613441133"><a name="zh-cn_topic_0000002451730862_p1291613441133"></a><a name="zh-cn_topic_0000002451730862_p1291613441133"></a>write_head</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p169161544131312"><a name="zh-cn_topic_0000002451730862_p169161544131312"></a><a name="zh-cn_topic_0000002451730862_p169161544131312"></a>码流buf写头指针。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row173836484358"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p5384114817353"><a name="zh-cn_topic_0000002451730862_p5384114817353"></a><a name="zh-cn_topic_0000002451730862_p5384114817353"></a>buf_len</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p103846485356"><a name="zh-cn_topic_0000002451730862_p103846485356"></a><a name="zh-cn_topic_0000002451730862_p103846485356"></a>码流buf长度。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1729482193217"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p142941521193214"><a name="zh-cn_topic_0000002451730862_p142941521193214"></a><a name="zh-cn_topic_0000002451730862_p142941521193214"></a>data_len</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p929442119327"><a name="zh-cn_topic_0000002451730862_p929442119327"></a><a name="zh-cn_topic_0000002451730862_p929442119327"></a>码流数据长度。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row829442193217"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p2295132120329"><a name="zh-cn_topic_0000002451730862_p2295132120329"></a><a name="zh-cn_topic_0000002451730862_p2295132120329"></a>buf_free</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1729572163213"><a name="zh-cn_topic_0000002451730862_p1729572163213"></a><a name="zh-cn_topic_0000002451730862_p1729572163213"></a>码流buf空闲长度。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row267711296192"><td class="cellrowborder" rowspan="6" valign="top" width="18.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p3631143913322"><a name="zh-cn_topic_0000002451730862_p3631143913322"></a><a name="zh-cn_topic_0000002451730862_p3631143913322"></a>roi</p>
</td>
<td class="cellrowborder" valign="top" width="27.83%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1077053861912"><a name="zh-cn_topic_0000002451730862_p1077053861912"></a><a name="zh-cn_topic_0000002451730862_p1077053861912"></a>id</p>
</td>
<td class="cellrowborder" valign="top" width="53.949999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002451730862_p2079114113190"><a name="zh-cn_topic_0000002451730862_p2079114113190"></a><a name="zh-cn_topic_0000002451730862_p2079114113190"></a>编码通道号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row37851037113219"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p573633620324"><a name="zh-cn_topic_0000002451730862_p573633620324"></a><a name="zh-cn_topic_0000002451730862_p573633620324"></a>rect_id</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p373633623214"><a name="zh-cn_topic_0000002451730862_p373633623214"></a><a name="zh-cn_topic_0000002451730862_p373633623214"></a>ROI区域编号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row0785123718328"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p5736133643216"><a name="zh-cn_topic_0000002451730862_p5736133643216"></a><a name="zh-cn_topic_0000002451730862_p5736133643216"></a>x</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1873643663212"><a name="zh-cn_topic_0000002451730862_p1873643663212"></a><a name="zh-cn_topic_0000002451730862_p1873643663212"></a>ROI区域坐标x。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row11535164143"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1753515651419"><a name="zh-cn_topic_0000002451730862_p1753515651419"></a><a name="zh-cn_topic_0000002451730862_p1753515651419"></a>y</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1251942131411"><a name="zh-cn_topic_0000002451730862_p1251942131411"></a><a name="zh-cn_topic_0000002451730862_p1251942131411"></a>ROI区域坐标y。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1399101061411"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p173991510201419"><a name="zh-cn_topic_0000002451730862_p173991510201419"></a><a name="zh-cn_topic_0000002451730862_p173991510201419"></a>width</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p1139991017141"><a name="zh-cn_topic_0000002451730862_p1139991017141"></a><a name="zh-cn_topic_0000002451730862_p1139991017141"></a>ROI区域宽度。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1195193443910"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1995153415399"><a name="zh-cn_topic_0000002451730862_p1995153415399"></a><a name="zh-cn_topic_0000002451730862_p1995153415399"></a>height</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p199511234133915"><a name="zh-cn_topic_0000002451730862_p199511234133915"></a><a name="zh-cn_topic_0000002451730862_p199511234133915"></a>ROI区域高度。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row161661859111918"><td class="cellrowborder" rowspan="5" valign="top" width="18.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p06292039193219"><a name="zh-cn_topic_0000002451730862_p06292039193219"></a><a name="zh-cn_topic_0000002451730862_p06292039193219"></a>roi param</p>
</td>
<td class="cellrowborder" valign="top" width="27.83%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p83124210206"><a name="zh-cn_topic_0000002451730862_p83124210206"></a><a name="zh-cn_topic_0000002451730862_p83124210206"></a>id</p>
</td>
<td class="cellrowborder" valign="top" width="53.949999999999996%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002451730862_p1316765911910"><a name="zh-cn_topic_0000002451730862_p1316765911910"></a><a name="zh-cn_topic_0000002451730862_p1316765911910"></a>编码通道号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row97851237143214"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p15736113683213"><a name="zh-cn_topic_0000002451730862_p15736113683213"></a><a name="zh-cn_topic_0000002451730862_p15736113683213"></a>fg_level</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p8736143683211"><a name="zh-cn_topic_0000002451730862_p8736143683211"></a><a name="zh-cn_topic_0000002451730862_p8736143683211"></a>ROI区域等级。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1378503713217"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1073618367321"><a name="zh-cn_topic_0000002451730862_p1073618367321"></a><a name="zh-cn_topic_0000002451730862_p1073618367321"></a>bg_level</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p157368368324"><a name="zh-cn_topic_0000002451730862_p157368368324"></a><a name="zh-cn_topic_0000002451730862_p157368368324"></a>非ROI区域等级。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row123689204493"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1973513560491"><a name="zh-cn_topic_0000002451730862_p1973513560491"></a><a name="zh-cn_topic_0000002451730862_p1973513560491"></a>is_abs_qp[i/pb/vi]</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p23681520194917"><a name="zh-cn_topic_0000002451730862_p23681520194917"></a><a name="zh-cn_topic_0000002451730862_p23681520194917"></a>i/pb/vi帧是否绝对qp。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451730862_row1778543719320"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451730862_p1588171115012"><a name="zh-cn_topic_0000002451730862_p1588171115012"></a><a name="zh-cn_topic_0000002451730862_p1588171115012"></a>qp[i/pb/vi]</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451730862_p173615363326"><a name="zh-cn_topic_0000002451730862_p173615363326"></a><a name="zh-cn_topic_0000002451730862_p173615363326"></a>i/pb/vi帧qp值。</p>
</td>
</tr>
</tbody>
</table>

# VDEC<a name="ZH-CN_TOPIC_0000002466383684"></a>

-   **[proc简介](#ZH-CN_TOPIC_0000002499582685)**  

-   **[proc信息](#ZH-CN_TOPIC_0000002499502617)**  

-   **[proc详解](#ZH-CN_TOPIC_0000002466543304)**  

## proc简介<a name="ZH-CN_TOPIC_0000002499582685"></a>

查询VDEC（Video Decoder）模块基本信息。

## proc信息<a name="ZH-CN_TOPIC_0000002499502617"></a>

下发命令：AT+VDEC=vdec\_proc,cat,vdec

VDEC模块proc信息如[图1](#zh-cn_topic_0000002451890526_fig14854174112716)所示。

**图 1**  VDEC proc信息<a name="zh-cn_topic_0000002451890526_fig14854174112716"></a>  
![](figures/VDEC-proc信息.png "VDEC-proc信息")

## proc详解<a name="ZH-CN_TOPIC_0000002466543304"></a>

**表 1**  VDEC proc参数说明

<a name="zh-cn_topic_0000002484810349_table168622058192320"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002484810349_row148631658192315"><th class="cellrowborder" valign="top" width="41.68%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002484810349_p18631958132315"><a name="zh-cn_topic_0000002484810349_p18631958132315"></a><a name="zh-cn_topic_0000002484810349_p18631958132315"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="58.32000000000001%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002484810349_p386325862317"><a name="zh-cn_topic_0000002484810349_p386325862317"></a><a name="zh-cn_topic_0000002484810349_p386325862317"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002484810349_row8863135813238"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p7863205817235"><a name="zh-cn_topic_0000002484810349_p7863205817235"></a><a name="zh-cn_topic_0000002484810349_p7863205817235"></a>open_count</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p68631158142317"><a name="zh-cn_topic_0000002484810349_p68631158142317"></a><a name="zh-cn_topic_0000002484810349_p68631158142317"></a>VDEC设备打开次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row28631358122313"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p486315812233"><a name="zh-cn_topic_0000002484810349_p486315812233"></a><a name="zh-cn_topic_0000002484810349_p486315812233"></a>setup</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p1286345819231"><a name="zh-cn_topic_0000002484810349_p1286345819231"></a><a name="zh-cn_topic_0000002484810349_p1286345819231"></a>0：VDEC设备未注册；1：VDEC设备注册成功。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row4863165822313"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p8863115812236"><a name="zh-cn_topic_0000002484810349_p8863115812236"></a><a name="zh-cn_topic_0000002484810349_p8863115812236"></a>cur_proc_chan</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p486395813239"><a name="zh-cn_topic_0000002484810349_p486395813239"></a><a name="zh-cn_topic_0000002484810349_p486395813239"></a>proc查看的实例VDEC ID。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row68631458152316"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p1863165815235"><a name="zh-cn_topic_0000002484810349_p1863165815235"></a><a name="zh-cn_topic_0000002484810349_p1863165815235"></a>bypass_chan_id</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p18863125819238"><a name="zh-cn_topic_0000002484810349_p18863125819238"></a><a name="zh-cn_topic_0000002484810349_p18863125819238"></a>VDEC bypass调试功能，即解码内部循环收还帧，不对外输出图像。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row1986313589239"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p16863165813231"><a name="zh-cn_topic_0000002484810349_p16863165813231"></a><a name="zh-cn_topic_0000002484810349_p16863165813231"></a>cycle_mode_chan</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p17863458152311"><a name="zh-cn_topic_0000002484810349_p17863458152311"></a><a name="zh-cn_topic_0000002484810349_p17863458152311"></a>循环输出固定帧模式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row15863105822312"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p10863125817232"><a name="zh-cn_topic_0000002484810349_p10863125817232"></a><a name="zh-cn_topic_0000002484810349_p10863125817232"></a>version</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p13863145822310"><a name="zh-cn_topic_0000002484810349_p13863145822310"></a><a name="zh-cn_topic_0000002484810349_p13863145822310"></a>VDEC版本号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row18863155819234"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p2863105842314"><a name="zh-cn_topic_0000002484810349_p2863105842314"></a><a name="zh-cn_topic_0000002484810349_p2863105842314"></a>chan_num</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p2863758152317"><a name="zh-cn_topic_0000002484810349_p2863758152317"></a><a name="zh-cn_topic_0000002484810349_p2863758152317"></a>当前运行的实例个数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row8863185842318"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p10863165842317"><a name="zh-cn_topic_0000002484810349_p10863165842317"></a><a name="zh-cn_topic_0000002484810349_p10863165842317"></a>chan_id</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p886375822310"><a name="zh-cn_topic_0000002484810349_p886375822310"></a><a name="zh-cn_topic_0000002484810349_p886375822310"></a>VDEC实例ID。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row12863185811237"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p158632585235"><a name="zh-cn_topic_0000002484810349_p158632585235"></a><a name="zh-cn_topic_0000002484810349_p158632585235"></a>vfmw_id</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p3863175822315"><a name="zh-cn_topic_0000002484810349_p3863175822315"></a><a name="zh-cn_topic_0000002484810349_p3863175822315"></a>vfmw实例ID。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row2863155892314"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p186375818234"><a name="zh-cn_topic_0000002484810349_p186375818234"></a><a name="zh-cn_topic_0000002484810349_p186375818234"></a>status</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p2086375812239"><a name="zh-cn_topic_0000002484810349_p2086375812239"></a><a name="zh-cn_topic_0000002484810349_p2086375812239"></a>模块状态。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row1863358192314"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p15863125812319"><a name="zh-cn_topic_0000002484810349_p15863125812319"></a><a name="zh-cn_topic_0000002484810349_p15863125812319"></a>standard</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p10863165816233"><a name="zh-cn_topic_0000002484810349_p10863165816233"></a><a name="zh-cn_topic_0000002484810349_p10863165816233"></a>解码协议。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row2863158172314"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p2863115813231"><a name="zh-cn_topic_0000002484810349_p2863115813231"></a><a name="zh-cn_topic_0000002484810349_p2863115813231"></a>dec_mode</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p786318588237"><a name="zh-cn_topic_0000002484810349_p786318588237"></a><a name="zh-cn_topic_0000002484810349_p786318588237"></a>解码模式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row5863165812316"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p7863158102316"><a name="zh-cn_topic_0000002484810349_p7863158102316"></a><a name="zh-cn_topic_0000002484810349_p7863158102316"></a>out_order</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p0863158142319"><a name="zh-cn_topic_0000002484810349_p0863158142319"></a><a name="zh-cn_topic_0000002484810349_p0863158142319"></a>输出模式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row5863958132311"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p386355852311"><a name="zh-cn_topic_0000002484810349_p386355852311"></a><a name="zh-cn_topic_0000002484810349_p386355852311"></a>frame_layout</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p586385862316"><a name="zh-cn_topic_0000002484810349_p586385862316"></a><a name="zh-cn_topic_0000002484810349_p586385862316"></a>图像布局方式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row11863205818236"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p14863105802318"><a name="zh-cn_topic_0000002484810349_p14863105802318"></a><a name="zh-cn_topic_0000002484810349_p14863105802318"></a>cycle_mode</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p7863958122312"><a name="zh-cn_topic_0000002484810349_p7863958122312"></a><a name="zh-cn_topic_0000002484810349_p7863958122312"></a>循环测试模式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row2863115813235"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p18863145819235"><a name="zh-cn_topic_0000002484810349_p18863145819235"></a><a name="zh-cn_topic_0000002484810349_p18863145819235"></a>max_stream_size</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p168631058142314"><a name="zh-cn_topic_0000002484810349_p168631058142314"></a><a name="zh-cn_topic_0000002484810349_p168631058142314"></a>最大码流缓冲大小。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row98631458192315"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p3863105811238"><a name="zh-cn_topic_0000002484810349_p3863105811238"></a><a name="zh-cn_topic_0000002484810349_p3863105811238"></a>max_stream_num</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p28639586231"><a name="zh-cn_topic_0000002484810349_p28639586231"></a><a name="zh-cn_topic_0000002484810349_p28639586231"></a>最大码流个数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row10863165812317"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p20863195892313"><a name="zh-cn_topic_0000002484810349_p20863195892313"></a><a name="zh-cn_topic_0000002484810349_p20863195892313"></a>error_threshold</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p11863125817231"><a name="zh-cn_topic_0000002484810349_p11863125817231"></a><a name="zh-cn_topic_0000002484810349_p11863125817231"></a>错误阈值。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row486435862318"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p086465862319"><a name="zh-cn_topic_0000002484810349_p086465862319"></a><a name="zh-cn_topic_0000002484810349_p086465862319"></a>extra_frame_num</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p0864135812231"><a name="zh-cn_topic_0000002484810349_p0864135812231"></a><a name="zh-cn_topic_0000002484810349_p0864135812231"></a>额外解码帧存个数，不包含解码所需的最少帧存数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row1686410581230"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p3864058122319"><a name="zh-cn_topic_0000002484810349_p3864058122319"></a><a name="zh-cn_topic_0000002484810349_p3864058122319"></a>out_format</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p158641958112312"><a name="zh-cn_topic_0000002484810349_p158641958112312"></a><a name="zh-cn_topic_0000002484810349_p158641958112312"></a>输出YUV格式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row16864165832319"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p15864115817238"><a name="zh-cn_topic_0000002484810349_p15864115817238"></a><a name="zh-cn_topic_0000002484810349_p15864115817238"></a>event_map</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p2864185812235"><a name="zh-cn_topic_0000002484810349_p2864185812235"></a><a name="zh-cn_topic_0000002484810349_p2864185812235"></a>注册事件。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row1386465892317"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p13864158192316"><a name="zh-cn_topic_0000002484810349_p13864158192316"></a><a name="zh-cn_topic_0000002484810349_p13864158192316"></a>lowdelay</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p11864155815230"><a name="zh-cn_topic_0000002484810349_p11864155815230"></a><a name="zh-cn_topic_0000002484810349_p11864155815230"></a>低延时使能。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row14864165812316"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p10864658112318"><a name="zh-cn_topic_0000002484810349_p10864658112318"></a><a name="zh-cn_topic_0000002484810349_p10864658112318"></a>try_que_stm</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p886485813234"><a name="zh-cn_topic_0000002484810349_p886485813234"></a><a name="zh-cn_topic_0000002484810349_p886485813234"></a>尝试入队次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row19864125811235"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p986435862320"><a name="zh-cn_topic_0000002484810349_p986435862320"></a><a name="zh-cn_topic_0000002484810349_p986435862320"></a>que_stm_ok</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p16864155817239"><a name="zh-cn_topic_0000002484810349_p16864155817239"></a><a name="zh-cn_topic_0000002484810349_p16864155817239"></a>入队成功次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row2086411589238"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p486435822311"><a name="zh-cn_topic_0000002484810349_p486435822311"></a><a name="zh-cn_topic_0000002484810349_p486435822311"></a>try_deq_stm</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p286485892318"><a name="zh-cn_topic_0000002484810349_p286485892318"></a><a name="zh-cn_topic_0000002484810349_p286485892318"></a>尝试获取码流buffer次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row5864145811236"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p1486415816237"><a name="zh-cn_topic_0000002484810349_p1486415816237"></a><a name="zh-cn_topic_0000002484810349_p1486415816237"></a>deq_stm_ok</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p0864558172318"><a name="zh-cn_topic_0000002484810349_p0864558172318"></a><a name="zh-cn_topic_0000002484810349_p0864558172318"></a>获取码流buffer成功次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row686495812317"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p158642582236"><a name="zh-cn_topic_0000002484810349_p158642582236"></a><a name="zh-cn_topic_0000002484810349_p158642582236"></a>try_acq_frm</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p886445813234"><a name="zh-cn_topic_0000002484810349_p886445813234"></a><a name="zh-cn_topic_0000002484810349_p886445813234"></a>尝试获取帧存次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row5306192274"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p12301319112712"><a name="zh-cn_topic_0000002484810349_p12301319112712"></a><a name="zh-cn_topic_0000002484810349_p12301319112712"></a>acq_frm_ok</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p1630319172713"><a name="zh-cn_topic_0000002484810349_p1630319172713"></a><a name="zh-cn_topic_0000002484810349_p1630319172713"></a>获取帧存成功次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row12971642715"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p1691816142713"><a name="zh-cn_topic_0000002484810349_p1691816142713"></a><a name="zh-cn_topic_0000002484810349_p1691816142713"></a>try_rls_frm</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p199171692711"><a name="zh-cn_topic_0000002484810349_p199171692711"></a><a name="zh-cn_topic_0000002484810349_p199171692711"></a>尝试释放帧存次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row229141322710"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p2029181352715"><a name="zh-cn_topic_0000002484810349_p2029181352715"></a><a name="zh-cn_topic_0000002484810349_p2029181352715"></a>rls_frm_ok</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p162916137274"><a name="zh-cn_topic_0000002484810349_p162916137274"></a><a name="zh-cn_topic_0000002484810349_p162916137274"></a>释放帧存成功次数。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row38648589235"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p128641658132311"><a name="zh-cn_topic_0000002484810349_p128641658132311"></a><a name="zh-cn_topic_0000002484810349_p128641658132311"></a>is_eos</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p1386465872317"><a name="zh-cn_topic_0000002484810349_p1386465872317"></a><a name="zh-cn_topic_0000002484810349_p1386465872317"></a>eos使能。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002484810349_row68641658122318"><td class="cellrowborder" valign="top" width="41.68%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002484810349_p158640581239"><a name="zh-cn_topic_0000002484810349_p158640581239"></a><a name="zh-cn_topic_0000002484810349_p158640581239"></a>reg_event</p>
</td>
<td class="cellrowborder" valign="top" width="58.32000000000001%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002484810349_p486416580239"><a name="zh-cn_topic_0000002484810349_p486416580239"></a><a name="zh-cn_topic_0000002484810349_p486416580239"></a>注册事件耗时（单位：μs）。</p>
</td>
</tr>
</tbody>
</table>

# VICAP<a name="ZH-CN_TOPIC_0000002466543312"></a>

-   **[proc简介](#ZH-CN_TOPIC_0000002466383696)**  

-   **[proc信息](#ZH-CN_TOPIC_0000002499582669)**  

-   **[proc详解](#ZH-CN_TOPIC_0000002499502637)**  

## proc简介<a name="ZH-CN_TOPIC_0000002466383696"></a>

查询VICAP（Video Input Capture）模块基本信息。

## proc信息<a name="ZH-CN_TOPIC_0000002499582669"></a>

下发命令：AT+ISP=test\_isp\_dfx\_show

VICAP模块proc信息如[图1](#zh-cn_topic_0000002484970373_fig2969123217011)所示。

**图 1**  VICAP proc信息<a name="zh-cn_topic_0000002484970373_fig2969123217011"></a>  
![](figures/VICAP-proc信息.png "VICAP-proc信息")

## proc详解<a name="ZH-CN_TOPIC_0000002499502637"></a>

**表 1**  VICAP proc参数说明

<a name="zh-cn_topic_0000002451890578_table56422423599"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002451890578_row13831542175914"><th class="cellrowborder" valign="top" width="17.031703170317034%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002451890578_p168311842205919"><a name="zh-cn_topic_0000002451890578_p168311842205919"></a><a name="zh-cn_topic_0000002451890578_p168311842205919"></a>类别</p>
</th>
<th class="cellrowborder" valign="top" width="24.442444244424443%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002451890578_p2831154210594"><a name="zh-cn_topic_0000002451890578_p2831154210594"></a><a name="zh-cn_topic_0000002451890578_p2831154210594"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="58.525852585258534%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002451890578_p118311542175912"><a name="zh-cn_topic_0000002451890578_p118311542175912"></a><a name="zh-cn_topic_0000002451890578_p118311542175912"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002451890578_row183174205920"><td class="cellrowborder" rowspan="3" valign="top" width="17.031703170317034%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451890578_p3831114215591"><a name="zh-cn_topic_0000002451890578_p3831114215591"></a><a name="zh-cn_topic_0000002451890578_p3831114215591"></a>dev信息</p>
</td>
<td class="cellrowborder" valign="top" width="24.442444244424443%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451890578_p178311842155910"><a name="zh-cn_topic_0000002451890578_p178311842155910"></a><a name="zh-cn_topic_0000002451890578_p178311842155910"></a>dev id</p>
</td>
<td class="cellrowborder" valign="top" width="58.525852585258534%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002451890578_p1883184210598"><a name="zh-cn_topic_0000002451890578_p1883184210598"></a><a name="zh-cn_topic_0000002451890578_p1883184210598"></a>dev编号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451890578_row9831542195911"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451890578_p283210424595"><a name="zh-cn_topic_0000002451890578_p283210424595"></a><a name="zh-cn_topic_0000002451890578_p283210424595"></a>dev_en</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451890578_p12832542195917"><a name="zh-cn_topic_0000002451890578_p12832542195917"></a><a name="zh-cn_topic_0000002451890578_p12832542195917"></a>dev使能。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451890578_row5832242155910"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451890578_p15832184235912"><a name="zh-cn_topic_0000002451890578_p15832184235912"></a><a name="zh-cn_topic_0000002451890578_p15832184235912"></a>intf_mode</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451890578_p1283214218590"><a name="zh-cn_topic_0000002451890578_p1283214218590"></a><a name="zh-cn_topic_0000002451890578_p1283214218590"></a>6：自建时序；3：对接sensor模式。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451890578_row083284216590"><td class="cellrowborder" rowspan="6" valign="top" width="17.031703170317034%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451890578_p13832642125915"><a name="zh-cn_topic_0000002451890578_p13832642125915"></a><a name="zh-cn_topic_0000002451890578_p13832642125915"></a>pipe信息</p>
</td>
<td class="cellrowborder" valign="top" width="24.442444244424443%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451890578_p58321042175916"><a name="zh-cn_topic_0000002451890578_p58321042175916"></a><a name="zh-cn_topic_0000002451890578_p58321042175916"></a>pipe id</p>
</td>
<td class="cellrowborder" valign="top" width="58.525852585258534%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002451890578_p14832842125911"><a name="zh-cn_topic_0000002451890578_p14832842125911"></a><a name="zh-cn_topic_0000002451890578_p14832842125911"></a>pipe编号。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451890578_row12832144214594"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451890578_p9832134255917"><a name="zh-cn_topic_0000002451890578_p9832134255917"></a><a name="zh-cn_topic_0000002451890578_p9832134255917"></a>start</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451890578_p118321842105914"><a name="zh-cn_topic_0000002451890578_p118321842105914"></a><a name="zh-cn_topic_0000002451890578_p118321842105914"></a>是否启动。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451890578_row158321642195918"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451890578_p168321242115910"><a name="zh-cn_topic_0000002451890578_p168321242115910"></a><a name="zh-cn_topic_0000002451890578_p168321242115910"></a>frameRate</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451890578_p183210422592"><a name="zh-cn_topic_0000002451890578_p183210422592"></a><a name="zh-cn_topic_0000002451890578_p183210422592"></a>pipe帧率。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451890578_row1083224211595"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451890578_p7832104255914"><a name="zh-cn_topic_0000002451890578_p7832104255914"></a><a name="zh-cn_topic_0000002451890578_p7832104255914"></a>intCnt</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451890578_p3832124214592"><a name="zh-cn_topic_0000002451890578_p3832124214592"></a><a name="zh-cn_topic_0000002451890578_p3832124214592"></a>pipe中断数量，正常启动时候应该不断增加。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451890578_row283217421593"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451890578_p88326427592"><a name="zh-cn_topic_0000002451890578_p88326427592"></a><a name="zh-cn_topic_0000002451890578_p88326427592"></a>width</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451890578_p5832114216596"><a name="zh-cn_topic_0000002451890578_p5832114216596"></a><a name="zh-cn_topic_0000002451890578_p5832114216596"></a>pipe宽度。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002451890578_row98322042195919"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002451890578_p783219421593"><a name="zh-cn_topic_0000002451890578_p783219421593"></a><a name="zh-cn_topic_0000002451890578_p783219421593"></a>height</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002451890578_p3832342145919"><a name="zh-cn_topic_0000002451890578_p3832342145919"></a><a name="zh-cn_topic_0000002451890578_p3832342145919"></a>pipe高度。</p>
</td>
</tr>
</tbody>
</table>

# 缩略语<a name="ZH-CN_TOPIC_0000002499502633"></a>

<a name="zh-cn_topic_0000002434369178_table1617320542357"></a>
<table><tbody><tr id="zh-cn_topic_0000002434369178_row2675161913543"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p14675201911544"><a name="zh-cn_topic_0000002434369178_p14675201911544"></a><a name="zh-cn_topic_0000002434369178_p14675201911544"></a><strong id="zh-cn_topic_0000002434369178_b2058011209272"><a name="zh-cn_topic_0000002434369178_b2058011209272"></a><a name="zh-cn_topic_0000002434369178_b2058011209272"></a>B</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row16427131510544"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p124270150545"><a name="zh-cn_topic_0000002434369178_p124270150545"></a><a name="zh-cn_topic_0000002434369178_p124270150545"></a><strong id="zh-cn_topic_0000002434369178_b5582112015277"><a name="zh-cn_topic_0000002434369178_b5582112015277"></a><a name="zh-cn_topic_0000002434369178_b5582112015277"></a>Blit</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p16427515145416"><a name="zh-cn_topic_0000002434369178_p16427515145416"></a><a name="zh-cn_topic_0000002434369178_p16427515145416"></a>Bliting</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p3427111545413"><a name="zh-cn_topic_0000002434369178_p3427111545413"></a><a name="zh-cn_topic_0000002434369178_p3427111545413"></a>位图组合操作</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row1376101214547"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row17173854113517"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p317313542357"><a name="zh-cn_topic_0000002434369178_p317313542357"></a><a name="zh-cn_topic_0000002434369178_p317313542357"></a><strong id="zh-cn_topic_0000002434369178_b165841320122715"><a name="zh-cn_topic_0000002434369178_b165841320122715"></a><a name="zh-cn_topic_0000002434369178_b165841320122715"></a>D</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row874423314417"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p77441333184410"><a name="zh-cn_topic_0000002434369178_p77441333184410"></a><a name="zh-cn_topic_0000002434369178_p77441333184410"></a><strong id="zh-cn_topic_0000002434369178_b358552012717"><a name="zh-cn_topic_0000002434369178_b358552012717"></a><a name="zh-cn_topic_0000002434369178_b358552012717"></a>DFX</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p63491118194912"><a name="zh-cn_topic_0000002434369178_p63491118194912"></a><a name="zh-cn_topic_0000002434369178_p63491118194912"></a>Design For X</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p63491118134912"><a name="zh-cn_topic_0000002434369178_p63491118134912"></a><a name="zh-cn_topic_0000002434369178_p63491118134912"></a>面向X的设计（可测试性、可制造性、可服务性等）</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row1517495412353"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p21741354173514"><a name="zh-cn_topic_0000002434369178_p21741354173514"></a><a name="zh-cn_topic_0000002434369178_p21741354173514"></a><strong id="zh-cn_topic_0000002434369178_b10586152092716"><a name="zh-cn_topic_0000002434369178_b10586152092716"></a><a name="zh-cn_topic_0000002434369178_b10586152092716"></a>DPU</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p917465410357"><a name="zh-cn_topic_0000002434369178_p917465410357"></a><a name="zh-cn_topic_0000002434369178_p917465410357"></a>Display Processing Unit</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p7174754133514"><a name="zh-cn_topic_0000002434369178_p7174754133514"></a><a name="zh-cn_topic_0000002434369178_p7174754133514"></a>显示处理单元</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row31741354163512"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row162511916567"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p196261393569"><a name="zh-cn_topic_0000002434369178_p196261393569"></a><a name="zh-cn_topic_0000002434369178_p196261393569"></a><strong id="zh-cn_topic_0000002434369178_b11587420162718"><a name="zh-cn_topic_0000002434369178_b11587420162718"></a><a name="zh-cn_topic_0000002434369178_b11587420162718"></a>F</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row106261292567"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p166268965620"><a name="zh-cn_topic_0000002434369178_p166268965620"></a><a name="zh-cn_topic_0000002434369178_p166268965620"></a><strong id="zh-cn_topic_0000002434369178_b2058862011278"><a name="zh-cn_topic_0000002434369178_b2058862011278"></a><a name="zh-cn_topic_0000002434369178_b2058862011278"></a>FB</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p116264985612"><a name="zh-cn_topic_0000002434369178_p116264985612"></a><a name="zh-cn_topic_0000002434369178_p116264985612"></a>Frame Buffer</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p36261393565"><a name="zh-cn_topic_0000002434369178_p36261393565"></a><a name="zh-cn_topic_0000002434369178_p36261393565"></a>帧缓冲区</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row36263918560"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row61741254103517"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p15174125493520"><a name="zh-cn_topic_0000002434369178_p15174125493520"></a><a name="zh-cn_topic_0000002434369178_p15174125493520"></a><strong id="zh-cn_topic_0000002434369178_b1358912013277"><a name="zh-cn_topic_0000002434369178_b1358912013277"></a><a name="zh-cn_topic_0000002434369178_b1358912013277"></a>G</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row3174175493518"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p111741754183510"><a name="zh-cn_topic_0000002434369178_p111741754183510"></a><a name="zh-cn_topic_0000002434369178_p111741754183510"></a><strong id="zh-cn_topic_0000002434369178_b25908201270"><a name="zh-cn_topic_0000002434369178_b25908201270"></a><a name="zh-cn_topic_0000002434369178_b25908201270"></a>GPU</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p15695416185516"><a name="zh-cn_topic_0000002434369178_p15695416185516"></a><a name="zh-cn_topic_0000002434369178_p15695416185516"></a>Graphical Processing Unit</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p61749543353"><a name="zh-cn_topic_0000002434369178_p61749543353"></a><a name="zh-cn_topic_0000002434369178_p61749543353"></a>图形处理单元</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row1897614824517"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row151741354143511"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p417413545352"><a name="zh-cn_topic_0000002434369178_p417413545352"></a><a name="zh-cn_topic_0000002434369178_p417413545352"></a><strong id="zh-cn_topic_0000002434369178_b2592182015277"><a name="zh-cn_topic_0000002434369178_b2592182015277"></a><a name="zh-cn_topic_0000002434369178_b2592182015277"></a>J</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row1174145453518"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p81741354173516"><a name="zh-cn_topic_0000002434369178_p81741354173516"></a><a name="zh-cn_topic_0000002434369178_p81741354173516"></a><strong id="zh-cn_topic_0000002434369178_b659311205273"><a name="zh-cn_topic_0000002434369178_b659311205273"></a><a name="zh-cn_topic_0000002434369178_b659311205273"></a>JPEG</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p217420542357"><a name="zh-cn_topic_0000002434369178_p217420542357"></a><a name="zh-cn_topic_0000002434369178_p217420542357"></a>Joint Photographic Experts Group</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p18174145473513"><a name="zh-cn_topic_0000002434369178_p18174145473513"></a><a name="zh-cn_topic_0000002434369178_p18174145473513"></a>联合活动图像专家组</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row62723410573"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row16273124110575"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p1327318412575"><a name="zh-cn_topic_0000002434369178_p1327318412575"></a><a name="zh-cn_topic_0000002434369178_p1327318412575"></a><strong id="zh-cn_topic_0000002434369178_b759492082720"><a name="zh-cn_topic_0000002434369178_b759492082720"></a><a name="zh-cn_topic_0000002434369178_b759492082720"></a>M</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row1527374119572"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p1327318412573"><a name="zh-cn_topic_0000002434369178_p1327318412573"></a><a name="zh-cn_topic_0000002434369178_p1327318412573"></a><strong id="zh-cn_topic_0000002434369178_b1359532019279"><a name="zh-cn_topic_0000002434369178_b1359532019279"></a><a name="zh-cn_topic_0000002434369178_b1359532019279"></a>MCU</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p5273194115719"><a name="zh-cn_topic_0000002434369178_p5273194115719"></a><a name="zh-cn_topic_0000002434369178_p5273194115719"></a>Micro Control Unit</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p227374175716"><a name="zh-cn_topic_0000002434369178_p227374175716"></a><a name="zh-cn_topic_0000002434369178_p227374175716"></a>微控制单元</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row12880193520458"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row11415124284216"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p3415442144217"><a name="zh-cn_topic_0000002434369178_p3415442144217"></a><a name="zh-cn_topic_0000002434369178_p3415442144217"></a><strong id="zh-cn_topic_0000002434369178_b145972020152714"><a name="zh-cn_topic_0000002434369178_b145972020152714"></a><a name="zh-cn_topic_0000002434369178_b145972020152714"></a>P</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row6415542104216"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p18415104234211"><a name="zh-cn_topic_0000002434369178_p18415104234211"></a><a name="zh-cn_topic_0000002434369178_p18415104234211"></a><strong id="zh-cn_topic_0000002434369178_b19598720162711"><a name="zh-cn_topic_0000002434369178_b19598720162711"></a><a name="zh-cn_topic_0000002434369178_b19598720162711"></a>PSRAM</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p5651194615434"><a name="zh-cn_topic_0000002434369178_p5651194615434"></a><a name="zh-cn_topic_0000002434369178_p5651194615434"></a>Pseudostatic RAM</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p465116468431"><a name="zh-cn_topic_0000002434369178_p465116468431"></a><a name="zh-cn_topic_0000002434369178_p465116468431"></a>伪静态随机存储器</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row106631511384"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p266395193815"><a name="zh-cn_topic_0000002434369178_p266395193815"></a><a name="zh-cn_topic_0000002434369178_p266395193815"></a><strong id="zh-cn_topic_0000002434369178_b260016206279"><a name="zh-cn_topic_0000002434369178_b260016206279"></a><a name="zh-cn_topic_0000002434369178_b260016206279"></a>Proc</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p116631351387"><a name="zh-cn_topic_0000002434369178_p116631351387"></a><a name="zh-cn_topic_0000002434369178_p116631351387"></a>Process</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p196635510383"><a name="zh-cn_topic_0000002434369178_p196635510383"></a><a name="zh-cn_topic_0000002434369178_p196635510383"></a>proc文件系统</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row181971881565"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row1566314512381"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p56634583816"><a name="zh-cn_topic_0000002434369178_p56634583816"></a><a name="zh-cn_topic_0000002434369178_p56634583816"></a><strong id="zh-cn_topic_0000002434369178_b1860162082713"><a name="zh-cn_topic_0000002434369178_b1860162082713"></a><a name="zh-cn_topic_0000002434369178_b1860162082713"></a>Q</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row18663195143812"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p1866312514384"><a name="zh-cn_topic_0000002434369178_p1866312514384"></a><a name="zh-cn_topic_0000002434369178_p1866312514384"></a><strong id="zh-cn_topic_0000002434369178_b9602720172716"><a name="zh-cn_topic_0000002434369178_b9602720172716"></a><a name="zh-cn_topic_0000002434369178_b9602720172716"></a>QSPI</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p123269710442"><a name="zh-cn_topic_0000002434369178_p123269710442"></a><a name="zh-cn_topic_0000002434369178_p123269710442"></a>Queued Synchronous Peripheral Interface</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p19326877449"><a name="zh-cn_topic_0000002434369178_p19326877449"></a><a name="zh-cn_topic_0000002434369178_p19326877449"></a>队列串行外设接口</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row165901217153812"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row20660135318423"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p1366016536421"><a name="zh-cn_topic_0000002434369178_p1366016536421"></a><a name="zh-cn_topic_0000002434369178_p1366016536421"></a><strong id="zh-cn_topic_0000002434369178_b860317201277"><a name="zh-cn_topic_0000002434369178_b860317201277"></a><a name="zh-cn_topic_0000002434369178_b860317201277"></a>S</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row11660155394210"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p1660145320420"><a name="zh-cn_topic_0000002434369178_p1660145320420"></a><a name="zh-cn_topic_0000002434369178_p1660145320420"></a><strong id="zh-cn_topic_0000002434369178_b10604520112712"><a name="zh-cn_topic_0000002434369178_b10604520112712"></a><a name="zh-cn_topic_0000002434369178_b10604520112712"></a>SRAM</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p926085112438"><a name="zh-cn_topic_0000002434369178_p926085112438"></a><a name="zh-cn_topic_0000002434369178_p926085112438"></a>Static Random Access Memory</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p162601951154311"><a name="zh-cn_topic_0000002434369178_p162601951154311"></a><a name="zh-cn_topic_0000002434369178_p162601951154311"></a>静态随机存取存储器</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row1066011531421"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row164641547175620"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p646454719563"><a name="zh-cn_topic_0000002434369178_p646454719563"></a><a name="zh-cn_topic_0000002434369178_p646454719563"></a><strong id="zh-cn_topic_0000002434369178_b12605020192718"><a name="zh-cn_topic_0000002434369178_b12605020192718"></a><a name="zh-cn_topic_0000002434369178_b12605020192718"></a>T</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row946434716565"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p846416471563"><a name="zh-cn_topic_0000002434369178_p846416471563"></a><a name="zh-cn_topic_0000002434369178_p846416471563"></a><strong id="zh-cn_topic_0000002434369178_b1460714209275"><a name="zh-cn_topic_0000002434369178_b1460714209275"></a><a name="zh-cn_topic_0000002434369178_b1460714209275"></a>TE</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p105394917517"><a name="zh-cn_topic_0000002434369178_p105394917517"></a><a name="zh-cn_topic_0000002434369178_p105394917517"></a>Tearing Effect Sign</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p159396198313"><a name="zh-cn_topic_0000002434369178_p159396198313"></a><a name="zh-cn_topic_0000002434369178_p159396198313"></a>防撕裂反馈信号</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row6464114765614"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row259018170381"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p95909175383"><a name="zh-cn_topic_0000002434369178_p95909175383"></a><a name="zh-cn_topic_0000002434369178_p95909175383"></a><strong id="zh-cn_topic_0000002434369178_b960872017271"><a name="zh-cn_topic_0000002434369178_b960872017271"></a><a name="zh-cn_topic_0000002434369178_b960872017271"></a>V</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row159041714382"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p5590111743820"><a name="zh-cn_topic_0000002434369178_p5590111743820"></a><a name="zh-cn_topic_0000002434369178_p5590111743820"></a><strong id="zh-cn_topic_0000002434369178_b8609102052712"><a name="zh-cn_topic_0000002434369178_b8609102052712"></a><a name="zh-cn_topic_0000002434369178_b8609102052712"></a>VAU</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p7590111773819"><a name="zh-cn_topic_0000002434369178_p7590111773819"></a><a name="zh-cn_topic_0000002434369178_p7590111773819"></a>Vector Acceleration Unit</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p12590171715380"><a name="zh-cn_topic_0000002434369178_p12590171715380"></a><a name="zh-cn_topic_0000002434369178_p12590171715380"></a>矢量加速单元</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row15269184815120"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p1882503651310"><a name="zh-cn_topic_0000002434369178_p1882503651310"></a><a name="zh-cn_topic_0000002434369178_p1882503651310"></a><strong id="zh-cn_topic_0000002434369178_b17375250153816"><a name="zh-cn_topic_0000002434369178_b17375250153816"></a><a name="zh-cn_topic_0000002434369178_b17375250153816"></a>VDEC</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p1182523615137"><a name="zh-cn_topic_0000002434369178_p1182523615137"></a><a name="zh-cn_topic_0000002434369178_p1182523615137"></a>Video Decoder</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p68251536121313"><a name="zh-cn_topic_0000002434369178_p68251536121313"></a><a name="zh-cn_topic_0000002434369178_p68251536121313"></a>视频解码器</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row11261251165115"><td class="nocellnorowborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p5818155981319"><a name="zh-cn_topic_0000002434369178_p5818155981319"></a><a name="zh-cn_topic_0000002434369178_p5818155981319"></a><strong id="zh-cn_topic_0000002434369178_b43761650123810"><a name="zh-cn_topic_0000002434369178_b43761650123810"></a><a name="zh-cn_topic_0000002434369178_b43761650123810"></a>VENC</strong></p>
<p id="zh-cn_topic_0000002434369178_p169981010131511"><a name="zh-cn_topic_0000002434369178_p169981010131511"></a><a name="zh-cn_topic_0000002434369178_p169981010131511"></a><strong id="zh-cn_topic_0000002434369178_b1937710504382"><a name="zh-cn_topic_0000002434369178_b1937710504382"></a><a name="zh-cn_topic_0000002434369178_b1937710504382"></a>VICAP</strong></p>
<p id="zh-cn_topic_0000002434369178_p71841435121514"><a name="zh-cn_topic_0000002434369178_p71841435121514"></a><a name="zh-cn_topic_0000002434369178_p71841435121514"></a></p>
<p id="zh-cn_topic_0000002434369178_p18262205175118"><a name="zh-cn_topic_0000002434369178_p18262205175118"></a><a name="zh-cn_topic_0000002434369178_p18262205175118"></a><strong id="zh-cn_topic_0000002434369178_b1537845033815"><a name="zh-cn_topic_0000002434369178_b1537845033815"></a><a name="zh-cn_topic_0000002434369178_b1537845033815"></a>Y</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p1262951145114"><a name="zh-cn_topic_0000002434369178_p1262951145114"></a><a name="zh-cn_topic_0000002434369178_p1262951145114"></a>Video Encoder</p>
<p id="zh-cn_topic_0000002434369178_p1910813016150"><a name="zh-cn_topic_0000002434369178_p1910813016150"></a><a name="zh-cn_topic_0000002434369178_p1910813016150"></a>Video Input Capture</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p132622051175119"><a name="zh-cn_topic_0000002434369178_p132622051175119"></a><a name="zh-cn_topic_0000002434369178_p132622051175119"></a>视频编码器</p>
<p id="zh-cn_topic_0000002434369178_p39201427181518"><a name="zh-cn_topic_0000002434369178_p39201427181518"></a><a name="zh-cn_topic_0000002434369178_p39201427181518"></a>视频输入捕获</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002434369178_row6458145716518"><td class="row-nocellborder" style="border:none" valign="top" width="21.81218121812181%"><p id="zh-cn_topic_0000002434369178_p2458195713518"><a name="zh-cn_topic_0000002434369178_p2458195713518"></a><a name="zh-cn_topic_0000002434369178_p2458195713518"></a><strong id="zh-cn_topic_0000002434369178_b126110205276"><a name="zh-cn_topic_0000002434369178_b126110205276"></a><a name="zh-cn_topic_0000002434369178_b126110205276"></a>YUV</strong></p>
</td>
<td class="row-nocellborder" style="border:none" valign="top" width="44.85448544854486%"><p id="zh-cn_topic_0000002434369178_p13458115785120"><a name="zh-cn_topic_0000002434369178_p13458115785120"></a><a name="zh-cn_topic_0000002434369178_p13458115785120"></a>YUV</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002434369178_p1045810572513"><a name="zh-cn_topic_0000002434369178_p1045810572513"></a><a name="zh-cn_topic_0000002434369178_p1045810572513"></a>色彩格式</p>
</td>
</tr>
</tbody>
</table>

