# 前言<a name="ZH-CN_TOPIC_0000002499502117"></a>

**概述<a name="zh-cn_topic_0000002444645769_section4537382116410"></a>**

本文档详细描述了HiDiTing的资源部署方案，包括镜像、内存等，并给出内存分析工具及调优方法，供用户参考进行产品适配。

**读者对象<a name="zh-cn_topic_0000002444645769_section4378592816410"></a>**

本文档主要适用于以下工程师：

-   技术支持工程师
-   软件开发工程师

**符号约定<a name="zh-cn_topic_0000002444645769_section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="zh-cn_topic_0000002444645769_table2622507016410"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002444645769_row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002444645769_p6450074116410"><a name="zh-cn_topic_0000002444645769_p6450074116410"></a><a name="zh-cn_topic_0000002444645769_p6450074116410"></a><strong id="zh-cn_topic_0000002444645769_b2136615816410"><a name="zh-cn_topic_0000002444645769_b2136615816410"></a><a name="zh-cn_topic_0000002444645769_b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002444645769_p5435366816410"><a name="zh-cn_topic_0000002444645769_p5435366816410"></a><a name="zh-cn_topic_0000002444645769_p5435366816410"></a><strong id="zh-cn_topic_0000002444645769_b5941558116410"><a name="zh-cn_topic_0000002444645769_b5941558116410"></a><a name="zh-cn_topic_0000002444645769_b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002444645769_row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444645769_p3734547016410"><a name="zh-cn_topic_0000002444645769_p3734547016410"></a><a name="zh-cn_topic_0000002444645769_p3734547016410"></a><a name="zh-cn_topic_0000002444645769_image2670064316410"></a><a name="zh-cn_topic_0000002444645769_image2670064316410"></a><span><img class="" id="zh-cn_topic_0000002444645769_image2670064316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002502283631.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444645769_p1757432116410"><a name="zh-cn_topic_0000002444645769_p1757432116410"></a><a name="zh-cn_topic_0000002444645769_p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444645769_row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444645769_p1432579516410"><a name="zh-cn_topic_0000002444645769_p1432579516410"></a><a name="zh-cn_topic_0000002444645769_p1432579516410"></a><a name="zh-cn_topic_0000002444645769_image4895582316410"></a><a name="zh-cn_topic_0000002444645769_image4895582316410"></a><span><img class="" id="zh-cn_topic_0000002444645769_image4895582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002502403621.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444645769_p959197916410"><a name="zh-cn_topic_0000002444645769_p959197916410"></a><a name="zh-cn_topic_0000002444645769_p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444645769_row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444645769_p1232579516410"><a name="zh-cn_topic_0000002444645769_p1232579516410"></a><a name="zh-cn_topic_0000002444645769_p1232579516410"></a><a name="zh-cn_topic_0000002444645769_image1235582316410"></a><a name="zh-cn_topic_0000002444645769_image1235582316410"></a><span><img class="" id="zh-cn_topic_0000002444645769_image1235582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002469283664.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444645769_p123197916410"><a name="zh-cn_topic_0000002444645769_p123197916410"></a><a name="zh-cn_topic_0000002444645769_p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444645769_row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444645769_p2204984716410"><a name="zh-cn_topic_0000002444645769_p2204984716410"></a><a name="zh-cn_topic_0000002444645769_p2204984716410"></a><a name="zh-cn_topic_0000002444645769_image4504446716410"></a><a name="zh-cn_topic_0000002444645769_image4504446716410"></a><span><img class="" id="zh-cn_topic_0000002444645769_image4504446716410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002469443656.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444645769_p4388861916410"><a name="zh-cn_topic_0000002444645769_p4388861916410"></a><a name="zh-cn_topic_0000002444645769_p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="zh-cn_topic_0000002444645769_p1238861916410"><a name="zh-cn_topic_0000002444645769_p1238861916410"></a><a name="zh-cn_topic_0000002444645769_p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444645769_row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444645769_p5555360116410"><a name="zh-cn_topic_0000002444645769_p5555360116410"></a><a name="zh-cn_topic_0000002444645769_p5555360116410"></a><a name="zh-cn_topic_0000002444645769_image799324016410"></a><a name="zh-cn_topic_0000002444645769_image799324016410"></a><span><img class="" id="zh-cn_topic_0000002444645769_image799324016410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002502283635.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444645769_p4612588116410"><a name="zh-cn_topic_0000002444645769_p4612588116410"></a><a name="zh-cn_topic_0000002444645769_p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="zh-cn_topic_0000002444645769_p1232588116410"><a name="zh-cn_topic_0000002444645769_p1232588116410"></a><a name="zh-cn_topic_0000002444645769_p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# 镜像分区表<a name="ZH-CN_TOPIC_0000002466383172"></a>

当前镜像默认存储在外部NorFlash上，并按照8M NorFlash容量进行镜像地址划分。EVB（Evaluate Verification Board）单板上镜像地址信息请参考文件“drivers\\boards\\3322\_evb\\memory\_config\\include\\memory\_config\_flash.h”。镜像分区表如[表1](#zh-cn_topic_0000002411046716_table060811521594)所示。

**表 1**  镜像分区表

<a name="zh-cn_topic_0000002411046716_table060811521594"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002411046716_row6608135235910"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="zh-cn_topic_0000002411046716_p1043575913598"><a name="zh-cn_topic_0000002411046716_p1043575913598"></a><a name="zh-cn_topic_0000002411046716_p1043575913598"></a>名称</p>
</th>
<th class="cellrowborder" valign="top" width="24.97%" id="mcps1.2.5.1.2"><p id="zh-cn_topic_0000002411046716_p54351959145914"><a name="zh-cn_topic_0000002411046716_p54351959145914"></a><a name="zh-cn_topic_0000002411046716_p54351959145914"></a>存储设备</p>
</th>
<th class="cellrowborder" valign="top" width="24.91%" id="mcps1.2.5.1.3"><p id="zh-cn_topic_0000002411046716_p16435359155918"><a name="zh-cn_topic_0000002411046716_p16435359155918"></a><a name="zh-cn_topic_0000002411046716_p16435359155918"></a>地址</p>
</th>
<th class="cellrowborder" valign="top" width="25.119999999999997%" id="mcps1.2.5.1.4"><p id="zh-cn_topic_0000002411046716_p9435115915918"><a name="zh-cn_topic_0000002411046716_p9435115915918"></a><a name="zh-cn_topic_0000002411046716_p9435115915918"></a>长度</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002411046716_row1360885216593"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002411046716_p44361059145916"><a name="zh-cn_topic_0000002411046716_p44361059145916"></a><a name="zh-cn_topic_0000002411046716_p44361059145916"></a>loaderboot_signed.bin</p>
</td>
<td class="cellrowborder" valign="top" width="24.97%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002411046716_p74361759185912"><a name="zh-cn_topic_0000002411046716_p74361759185912"></a><a name="zh-cn_topic_0000002411046716_p74361759185912"></a>norflash</p>
</td>
<td class="cellrowborder" valign="top" width="24.91%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002411046716_p1043675917596"><a name="zh-cn_topic_0000002411046716_p1043675917596"></a><a name="zh-cn_topic_0000002411046716_p1043675917596"></a>0x00000000</p>
</td>
<td class="cellrowborder" valign="top" width="25.119999999999997%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002411046716_p1243619590595"><a name="zh-cn_topic_0000002411046716_p1243619590595"></a><a name="zh-cn_topic_0000002411046716_p1243619590595"></a>0x00010000</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411046716_row0608052135914"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002411046716_p74362598592"><a name="zh-cn_topic_0000002411046716_p74362598592"></a><a name="zh-cn_topic_0000002411046716_p74362598592"></a>rootkey_partition_cfg.bin</p>
</td>
<td class="cellrowborder" valign="top" width="24.97%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002411046716_p1843645915912"><a name="zh-cn_topic_0000002411046716_p1843645915912"></a><a name="zh-cn_topic_0000002411046716_p1843645915912"></a>norflash</p>
</td>
<td class="cellrowborder" valign="top" width="24.91%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002411046716_p184361959135910"><a name="zh-cn_topic_0000002411046716_p184361959135910"></a><a name="zh-cn_topic_0000002411046716_p184361959135910"></a>0x26000000</p>
</td>
<td class="cellrowborder" valign="top" width="25.119999999999997%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002411046716_p94360598593"><a name="zh-cn_topic_0000002411046716_p94360598593"></a><a name="zh-cn_topic_0000002411046716_p94360598593"></a>0x00001000</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411046716_row760985235915"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002411046716_p2436195935917"><a name="zh-cn_topic_0000002411046716_p2436195935917"></a><a name="zh-cn_topic_0000002411046716_p2436195935917"></a>ssb_signed.bin</p>
</td>
<td class="cellrowborder" valign="top" width="24.97%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002411046716_p14436059205920"><a name="zh-cn_topic_0000002411046716_p14436059205920"></a><a name="zh-cn_topic_0000002411046716_p14436059205920"></a>norflash</p>
</td>
<td class="cellrowborder" valign="top" width="24.91%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002411046716_p12436259135913"><a name="zh-cn_topic_0000002411046716_p12436259135913"></a><a name="zh-cn_topic_0000002411046716_p12436259135913"></a>0x26001000</p>
</td>
<td class="cellrowborder" valign="top" width="25.119999999999997%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002411046716_p124361359125913"><a name="zh-cn_topic_0000002411046716_p124361359125913"></a><a name="zh-cn_topic_0000002411046716_p124361359125913"></a>0x0000F000</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411046716_row1660985295919"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002411046716_p1543618597599"><a name="zh-cn_topic_0000002411046716_p1543618597599"></a><a name="zh-cn_topic_0000002411046716_p1543618597599"></a>ssb_backup_signed.bin</p>
</td>
<td class="cellrowborder" valign="top" width="24.97%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002411046716_p04369596596"><a name="zh-cn_topic_0000002411046716_p04369596596"></a><a name="zh-cn_topic_0000002411046716_p04369596596"></a>norflash</p>
</td>
<td class="cellrowborder" valign="top" width="24.91%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002411046716_p1436135920591"><a name="zh-cn_topic_0000002411046716_p1436135920591"></a><a name="zh-cn_topic_0000002411046716_p1436135920591"></a>0x26010000</p>
</td>
<td class="cellrowborder" valign="top" width="25.119999999999997%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002411046716_p1443625918594"><a name="zh-cn_topic_0000002411046716_p1443625918594"></a><a name="zh-cn_topic_0000002411046716_p1443625918594"></a>0x0000f000</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411046716_row860911523598"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002411046716_p144365592595"><a name="zh-cn_topic_0000002411046716_p144365592595"></a><a name="zh-cn_topic_0000002411046716_p144365592595"></a>recovery_signed.bin</p>
</td>
<td class="cellrowborder" valign="top" width="24.97%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002411046716_p1143665995914"><a name="zh-cn_topic_0000002411046716_p1143665995914"></a><a name="zh-cn_topic_0000002411046716_p1143665995914"></a>norflash</p>
</td>
<td class="cellrowborder" valign="top" width="24.91%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002411046716_p14367593592"><a name="zh-cn_topic_0000002411046716_p14367593592"></a><a name="zh-cn_topic_0000002411046716_p14367593592"></a>0x2601F000</p>
</td>
<td class="cellrowborder" valign="top" width="25.119999999999997%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002411046716_p124361259135912"><a name="zh-cn_topic_0000002411046716_p124361259135912"></a><a name="zh-cn_topic_0000002411046716_p124361259135912"></a>0x00061000</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411046716_row66092052125919"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002411046716_p84361590591"><a name="zh-cn_topic_0000002411046716_p84361590591"></a><a name="zh-cn_topic_0000002411046716_p84361590591"></a>seliteos_signed.bin</p>
</td>
<td class="cellrowborder" valign="top" width="24.97%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002411046716_p84361359155917"><a name="zh-cn_topic_0000002411046716_p84361359155917"></a><a name="zh-cn_topic_0000002411046716_p84361359155917"></a>norflash</p>
</td>
<td class="cellrowborder" valign="top" width="24.91%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002411046716_p24366598599"><a name="zh-cn_topic_0000002411046716_p24366598599"></a><a name="zh-cn_topic_0000002411046716_p24366598599"></a>0x26080000</p>
</td>
<td class="cellrowborder" valign="top" width="25.119999999999997%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002411046716_p243655914598"><a name="zh-cn_topic_0000002411046716_p243655914598"></a><a name="zh-cn_topic_0000002411046716_p243655914598"></a>0x00040000</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411046716_row14609352135915"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002411046716_p1343635935910"><a name="zh-cn_topic_0000002411046716_p1343635935910"></a><a name="zh-cn_topic_0000002411046716_p1343635935910"></a>application_signed.bin</p>
</td>
<td class="cellrowborder" valign="top" width="24.97%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002411046716_p18436359105920"><a name="zh-cn_topic_0000002411046716_p18436359105920"></a><a name="zh-cn_topic_0000002411046716_p18436359105920"></a>norflash</p>
</td>
<td class="cellrowborder" valign="top" width="24.91%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002411046716_p12436135919598"><a name="zh-cn_topic_0000002411046716_p12436135919598"></a><a name="zh-cn_topic_0000002411046716_p12436135919598"></a>0x260C0000</p>
</td>
<td class="cellrowborder" valign="top" width="25.119999999999997%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002411046716_p9436859115916"><a name="zh-cn_topic_0000002411046716_p9436859115916"></a><a name="zh-cn_topic_0000002411046716_p9436859115916"></a>0x6E3000</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411046716_row3609135295916"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="zh-cn_topic_0000002411046716_p164364593592"><a name="zh-cn_topic_0000002411046716_p164364593592"></a><a name="zh-cn_topic_0000002411046716_p164364593592"></a>3322_all_nv.bin</p>
</td>
<td class="cellrowborder" valign="top" width="24.97%" headers="mcps1.2.5.1.2 "><p id="zh-cn_topic_0000002411046716_p194361959145916"><a name="zh-cn_topic_0000002411046716_p194361959145916"></a><a name="zh-cn_topic_0000002411046716_p194361959145916"></a>norflash</p>
</td>
<td class="cellrowborder" valign="top" width="24.91%" headers="mcps1.2.5.1.3 "><p id="zh-cn_topic_0000002411046716_p1843625915599"><a name="zh-cn_topic_0000002411046716_p1843625915599"></a><a name="zh-cn_topic_0000002411046716_p1843625915599"></a>0x267FA000</p>
</td>
<td class="cellrowborder" valign="top" width="25.119999999999997%" headers="mcps1.2.5.1.4 "><p id="zh-cn_topic_0000002411046716_p17436559165915"><a name="zh-cn_topic_0000002411046716_p17436559165915"></a><a name="zh-cn_topic_0000002411046716_p17436559165915"></a>0x00006000</p>
</td>
</tr>
</tbody>
</table>

>![](public_sys-resources/icon-notice.gif) **须知：** 
>-   由于BT、DSP镜像对外闭源交付，起始地址固定，因此除application镜像外，不建议用户自行修改其余地址划分，需要修改时请告知FAE。
>-   分区表无法通过OTA（Over the Air）升级更改，因此要注意，分区表的划分需要在产品出货前确定。

# 内存部署<a name="ZH-CN_TOPIC_0000002499582165"></a>

芯片可使用内存可具体分为ITCM/DTCM、L2MEM、PSRAM三部分，其中TCM由MCU和DSP共享，L2MEM（Level-2-Memory）与DSP、GPU共享。各类内存属性如下，访问性能 ITCM/DTCM＞L2MEM＞PSRAM，读写功耗 ITCM/DTCM＜L2MEM＜PSRAM，大小ITCM/DTCM＜L2MEM＜PSRAM，实际综合性能、功耗、大小进行内存部署。

内存部署具体信息可参考链接脚本“drivers\\boards\\3322\_evb\\linker\\standard\\acore\\acore.prelds”。

-   代码段部署

    代码部署在Flash中，芯片启动时将代码搬到PSRAM（Pseudostatic RAM）和ITCM（Instruction Tightly-Coupled Memory）中运行，包括Flash驱动代码、低功耗相关代码以及部分基础库。

    如需配置代码到芯片ITCM上运行，可在“.plt\_ramtext”段中增加对应静态库即可，具体可参考[图1](#zh-cn_topic_0000002411046696_fig166417919594)。

    **图 1**  代码段部署至ITCM<a name="zh-cn_topic_0000002411046696_fig166417919594"></a>  
    ![](figures/代码段部署至ITCM.png "代码段部署至ITCM")

-   静态数据段部署，静态数据段包括data段和bss段：

    -   bss段指初始化为0或者未初始化的全局变量。
    -   data段指初始化非0的全局变量。

    bss段默认部署至内部DTCM（Data Tightly-Coupled Memory）上，data段默认部署在PSRAM上具体可参考[图2](#zh-cn_topic_0000002411046696_fig574783510599)、[图3](#zh-cn_topic_0000002411046696_fig1546854217225)。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >外部PSRAM内存更大，但是访问性能低于SRAM（Static Random Access Memory），功耗也高于SRAM，因此考虑系统低功耗，适合将访问频次低或亮屏执行的部分放至PSRAM上。

    **图 2**  psram\_data段部署<a name="zh-cn_topic_0000002411046696_fig574783510599"></a>  
    ![](figures/psram_data段部署.png "psram_data段部署")

    **图 3**  psram\_bss段部署<a name="zh-cn_topic_0000002411046696_fig1546854217225"></a>  
    ![](figures/psram_bss段部署.png "psram_bss段部署")

-   栈空间

    系统中断栈、异常栈默认部署在DTCM，不支持修改；任务栈支持部署在DTCM或者PSRAM上（\\application\\3322\\3322\_app\_standard\\thread\_init.c），具体示例如[图4](#zh-cn_topic_0000002411046696_fig447441719017)所示。

    **图 4**  任务栈支持DTCM/PSRAM部署<a name="zh-cn_topic_0000002411046696_fig447441719017"></a>  
    ![](figures/任务栈支持DTCM-PSRAM部署.png "任务栈支持DTCM-PSRAM部署")

-   堆空间

    堆空间分为两部分，一部分为kernel OS内部使用的系统堆空间，默认部署至内部DTCM上；另一部分为各个任务使用的内存堆空间，如各个任务malloc申请空间，默认部署至外部PSRAM上，具体可参考“drivers/boards/3322\_evb/memory\_config/psram/normal/psram.h”头文件。

    此外，堆空间支持自定义内存池使用，调用osal接口“osal\_pool\_mem\_init”、“osal\_pool\_mem\_alloc”（/kernel/osal/src/liteos/osal\_addr.c）实现。

# 内存分析工具<a name="ZH-CN_TOPIC_0000002466383176"></a>

-   **[静态内存](#ZH-CN_TOPIC_0000002499582161)**  

-   **[动态内存](#ZH-CN_TOPIC_0000002466542796)**  

## 静态内存<a name="ZH-CN_TOPIC_0000002499582161"></a>

IDE工具提供了镜像分析功能，可以方便分析数据段、代码段等静态内存占用情况。具体使用方法请参见《HiSpark Studio for VS Code 使用指南》“镜像分析”章节。

其中镜像分析功能中默认按照模块进行数据统计，如graphic、media、driver等模块。

## 动态内存<a name="ZH-CN_TOPIC_0000002466542796"></a>

支持AT命令查看堆、栈内存信息，具体命令如[表1](#zh-cn_topic_0000002444725853_table181208297502)所示。

**表 1**  堆栈内存信息查看命令

<a name="zh-cn_topic_0000002444725853_table181208297502"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002444725853_row191201429105016"><th class="cellrowborder" valign="top" width="34.71%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002444725853_p712002995010"><a name="zh-cn_topic_0000002444725853_p712002995010"></a><a name="zh-cn_topic_0000002444725853_p712002995010"></a>AT命令</p>
</th>
<th class="cellrowborder" valign="top" width="65.29%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002444725853_p11120192985018"><a name="zh-cn_topic_0000002444725853_p11120192985018"></a><a name="zh-cn_topic_0000002444725853_p11120192985018"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002444725853_row212042914507"><td class="cellrowborder" valign="top" width="34.71%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002444725853_p1312002910508"><a name="zh-cn_topic_0000002444725853_p1312002910508"></a><a name="zh-cn_topic_0000002444725853_p1312002910508"></a>AT+HEAPINFO</p>
</td>
<td class="cellrowborder" valign="top" width="65.29%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002444725853_p81209291509"><a name="zh-cn_topic_0000002444725853_p81209291509"></a><a name="zh-cn_topic_0000002444725853_p81209291509"></a>系统堆内存信息查看。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444725853_row18120929135015"><td class="cellrowborder" valign="top" width="34.71%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002444725853_p9120122925012"><a name="zh-cn_topic_0000002444725853_p9120122925012"></a><a name="zh-cn_topic_0000002444725853_p9120122925012"></a>AT+STACKINFO</p>
</td>
<td class="cellrowborder" valign="top" width="65.29%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002444725853_p161201929155011"><a name="zh-cn_topic_0000002444725853_p161201929155011"></a><a name="zh-cn_topic_0000002444725853_p161201929155011"></a>系统任务栈内存信息查看，包括任务栈名称、大小、峰值、使用情况等。</p>
</td>
</tr>
</tbody>
</table>

# 常见内存优化方法<a name="ZH-CN_TOPIC_0000002466542800"></a>

-   SDK组件裁剪

    SDK当前除功能代码外，默认包含大量测试代码、维测代码以及Sample示例代码，实际用户集成SDK时，不需要该部分功能，因此可直接裁剪。具体可裁剪组件如[表1](#zh-cn_topic_0000002411206560_table1374450141518)所示。

    **表 1**  SDK可裁剪组件

    <a name="zh-cn_topic_0000002411206560_table1374450141518"></a>
    <table><thead align="left"><tr id="zh-cn_topic_0000002411206560_row57441204159"><th class="cellrowborder" valign="top" width="29.212921292129213%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000002411206560_p17744120201517"><a name="zh-cn_topic_0000002411206560_p17744120201517"></a><a name="zh-cn_topic_0000002411206560_p17744120201517"></a>组件名</p>
    </th>
    <th class="cellrowborder" valign="top" width="37.453745374537455%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000002411206560_p13744110171510"><a name="zh-cn_topic_0000002411206560_p13744110171510"></a><a name="zh-cn_topic_0000002411206560_p13744110171510"></a>对应功能宏</p>
    </th>
    <th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000002411206560_p107441604159"><a name="zh-cn_topic_0000002411206560_p107441604159"></a><a name="zh-cn_topic_0000002411206560_p107441604159"></a>功能描述</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="zh-cn_topic_0000002411206560_row674480141510"><td class="cellrowborder" valign="top" width="29.212921292129213%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411206560_p10744502155"><a name="zh-cn_topic_0000002411206560_p10744502155"></a><a name="zh-cn_topic_0000002411206560_p10744502155"></a>'graphic_at_service'</p>
    </td>
    <td class="cellrowborder" valign="top" width="37.453745374537455%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411206560_p3744601159"><a name="zh-cn_topic_0000002411206560_p3744601159"></a><a name="zh-cn_topic_0000002411206560_p3744601159"></a>-</p>
    </td>
    <td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411206560_p1375182213156"><a name="zh-cn_topic_0000002411206560_p1375182213156"></a><a name="zh-cn_topic_0000002411206560_p1375182213156"></a>图形测试组件。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002411206560_row07441402151"><td class="cellrowborder" valign="top" width="29.212921292129213%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411206560_p37441307154"><a name="zh-cn_topic_0000002411206560_p37441307154"></a><a name="zh-cn_topic_0000002411206560_p37441307154"></a>'app_at_service'</p>
    </td>
    <td class="cellrowborder" valign="top" width="37.453745374537455%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411206560_p7744201154"><a name="zh-cn_topic_0000002411206560_p7744201154"></a><a name="zh-cn_topic_0000002411206560_p7744201154"></a>-</p>
    </td>
    <td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411206560_p137444021518"><a name="zh-cn_topic_0000002411206560_p137444021518"></a><a name="zh-cn_topic_0000002411206560_p137444021518"></a>系统BSP测试组件。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002411206560_row147441014155"><td class="cellrowborder" valign="top" width="29.212921292129213%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411206560_p97444071518"><a name="zh-cn_topic_0000002411206560_p97444071518"></a><a name="zh-cn_topic_0000002411206560_p97444071518"></a>'audio_at_service'</p>
    </td>
    <td class="cellrowborder" valign="top" width="37.453745374537455%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411206560_p074411020155"><a name="zh-cn_topic_0000002411206560_p074411020155"></a><a name="zh-cn_topic_0000002411206560_p074411020155"></a>-</p>
    </td>
    <td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411206560_p77446018155"><a name="zh-cn_topic_0000002411206560_p77446018155"></a><a name="zh-cn_topic_0000002411206560_p77446018155"></a>音频测试组件。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002411206560_row174490181517"><td class="cellrowborder" valign="top" width="29.212921292129213%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411206560_p107441507156"><a name="zh-cn_topic_0000002411206560_p107441507156"></a><a name="zh-cn_topic_0000002411206560_p107441507156"></a>'media_at_service'</p>
    </td>
    <td class="cellrowborder" valign="top" width="37.453745374537455%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411206560_p274415081517"><a name="zh-cn_topic_0000002411206560_p274415081517"></a><a name="zh-cn_topic_0000002411206560_p274415081517"></a>-</p>
    </td>
    <td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411206560_p1174410012157"><a name="zh-cn_topic_0000002411206560_p1174410012157"></a><a name="zh-cn_topic_0000002411206560_p1174410012157"></a>媒体测试组件。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002411206560_row193411448121515"><td class="cellrowborder" valign="top" width="29.212921292129213%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411206560_p19341184811154"><a name="zh-cn_topic_0000002411206560_p19341184811154"></a><a name="zh-cn_topic_0000002411206560_p19341184811154"></a>'bt_manager_at_service'</p>
    </td>
    <td class="cellrowborder" valign="top" width="37.453745374537455%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411206560_p334194816158"><a name="zh-cn_topic_0000002411206560_p334194816158"></a><a name="zh-cn_topic_0000002411206560_p334194816158"></a>-</p>
    </td>
    <td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411206560_p113419489153"><a name="zh-cn_topic_0000002411206560_p113419489153"></a><a name="zh-cn_topic_0000002411206560_p113419489153"></a>蓝牙测试组件。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002411206560_row75419496155"><td class="cellrowborder" valign="top" width="29.212921292129213%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411206560_p135554941517"><a name="zh-cn_topic_0000002411206560_p135554941517"></a><a name="zh-cn_topic_0000002411206560_p135554941517"></a>'audio_proc'</p>
    </td>
    <td class="cellrowborder" valign="top" width="37.453745374537455%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411206560_p2055114951517"><a name="zh-cn_topic_0000002411206560_p2055114951517"></a><a name="zh-cn_topic_0000002411206560_p2055114951517"></a>‘CONFIG_ZDIAG_AUDIO_PROC_SUPPORT’</p>
    </td>
    <td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411206560_p145514914152"><a name="zh-cn_topic_0000002411206560_p145514914152"></a><a name="zh-cn_topic_0000002411206560_p145514914152"></a>音频维测相关组件。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002411206560_row1099154910152"><td class="cellrowborder" valign="top" width="29.212921292129213%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411206560_p20991749111520"><a name="zh-cn_topic_0000002411206560_p20991749111520"></a><a name="zh-cn_topic_0000002411206560_p20991749111520"></a>'audio_dump'</p>
    </td>
    <td class="cellrowborder" valign="top" width="37.453745374537455%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411206560_p799194961512"><a name="zh-cn_topic_0000002411206560_p799194961512"></a><a name="zh-cn_topic_0000002411206560_p799194961512"></a>‘CONFIG_ZDIAG_AUDIO_DUMP_SUPPORT’</p>
    </td>
    <td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411206560_p399144919153"><a name="zh-cn_topic_0000002411206560_p399144919153"></a><a name="zh-cn_topic_0000002411206560_p399144919153"></a>音频维测相关组件。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002411206560_row17646152051616"><td class="cellrowborder" rowspan="3" valign="top" width="29.212921292129213%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411206560_p156461120141610"><a name="zh-cn_topic_0000002411206560_p156461120141610"></a><a name="zh-cn_topic_0000002411206560_p156461120141610"></a>'audio_probe'</p>
    </td>
    <td class="cellrowborder" valign="top" width="37.453745374537455%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411206560_p1364682071617"><a name="zh-cn_topic_0000002411206560_p1364682071617"></a><a name="zh-cn_topic_0000002411206560_p1364682071617"></a>‘CONFIG_ZDIAG_AUDIO_PROBE_SUPPORT’</p>
    </td>
    <td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000002411206560_p2064614207163"><a name="zh-cn_topic_0000002411206560_p2064614207163"></a><a name="zh-cn_topic_0000002411206560_p2064614207163"></a>音频维测相关组件。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002411206560_row13457155785612"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411206560_p8457157175620"><a name="zh-cn_topic_0000002411206560_p8457157175620"></a><a name="zh-cn_topic_0000002411206560_p8457157175620"></a>‘SUPPORT_GPU_OPENVG’</p>
    </td>
    <td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411206560_p204811927192810"><a name="zh-cn_topic_0000002411206560_p204811927192810"></a><a name="zh-cn_topic_0000002411206560_p204811927192810"></a>GPU（Graphical Processing Unit）支持OpenVG接口。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002411206560_row1948322318583"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000002411206560_p2483132315812"><a name="zh-cn_topic_0000002411206560_p2483132315812"></a><a name="zh-cn_topic_0000002411206560_p2483132315812"></a>‘SW_UART_DEBUG’</p>
    </td>
    <td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000002411206560_p948319239587"><a name="zh-cn_topic_0000002411206560_p948319239587"></a><a name="zh-cn_topic_0000002411206560_p948319239587"></a>支持串口直接打印。</p>
    </td>
    </tr>
    </tbody>
    </table>

-   内部部署调整

    内部TCM空间不足时，可以将部分数据段、任务栈部署至外部PSRAM。

-   代码优化

    结合内存分析工具，排查当前哪一个模块占用内存较大，结合代码进行优化。代码优化手段较多，此处列出常见的几种优化手段帮助用户优化内存。

    -   局部变量不应过大，会撑大栈空间，可考虑使用动态malloc的形式申请内存。
    -   超大全局变量数组，需要考虑合理性，可考虑修改为链表的形式进行动态申请。
    -   根据栈峰值使用情况，合理规划栈大小，根据经验，中等复杂程度任务一般栈空间3KB左右即可。
    -   正常的printf接口无法对打印字符串进行压缩，会导致CodeSize迅速膨胀，因此可使用Diag压缩日志接口打印。
    -   将不变的数据段尽量修改为const类型，即由data段转变为rodata段，内存部署也从内部SRAM转变为NorFlash。

# 缩略语<a name="ZH-CN_TOPIC_0000002499502121"></a>

<a name="zh-cn_topic_0000002458194889_table1775318138357"></a>
<table><tbody><tr id="zh-cn_topic_0000002458194889_row9766111812369"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p1476601843610"><a name="zh-cn_topic_0000002458194889_p1476601843610"></a><a name="zh-cn_topic_0000002458194889_p1476601843610"></a><strong id="zh-cn_topic_0000002458194889_b183805107810"><a name="zh-cn_topic_0000002458194889_b183805107810"></a><a name="zh-cn_topic_0000002458194889_b183805107810"></a>B</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row276671814366"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p16766718133619"><a name="zh-cn_topic_0000002458194889_p16766718133619"></a><a name="zh-cn_topic_0000002458194889_p16766718133619"></a><strong id="zh-cn_topic_0000002458194889_b13383131011812"><a name="zh-cn_topic_0000002458194889_b13383131011812"></a><a name="zh-cn_topic_0000002458194889_b13383131011812"></a>BT</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%"><p id="zh-cn_topic_0000002458194889_p494518513361"><a name="zh-cn_topic_0000002458194889_p494518513361"></a><a name="zh-cn_topic_0000002458194889_p494518513361"></a>Bluetooth</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002458194889_p19945125116367"><a name="zh-cn_topic_0000002458194889_p19945125116367"></a><a name="zh-cn_topic_0000002458194889_p19945125116367"></a>蓝牙</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row14766141817366"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row12767118143620"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p3767131893616"><a name="zh-cn_topic_0000002458194889_p3767131893616"></a><a name="zh-cn_topic_0000002458194889_p3767131893616"></a><strong id="zh-cn_topic_0000002458194889_b18384310685"><a name="zh-cn_topic_0000002458194889_b18384310685"></a><a name="zh-cn_topic_0000002458194889_b18384310685"></a>D</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row187671618203618"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p1676711189366"><a name="zh-cn_topic_0000002458194889_p1676711189366"></a><a name="zh-cn_topic_0000002458194889_p1676711189366"></a><strong id="zh-cn_topic_0000002458194889_b6385310786"><a name="zh-cn_topic_0000002458194889_b6385310786"></a><a name="zh-cn_topic_0000002458194889_b6385310786"></a>DSP</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%"><p id="zh-cn_topic_0000002458194889_p207671118113620"><a name="zh-cn_topic_0000002458194889_p207671118113620"></a><a name="zh-cn_topic_0000002458194889_p207671118113620"></a>Digital signal processor</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002458194889_p12767111811367"><a name="zh-cn_topic_0000002458194889_p12767111811367"></a><a name="zh-cn_topic_0000002458194889_p12767111811367"></a>数字信号处理器</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row1584812192518"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p1284151282511"><a name="zh-cn_topic_0000002458194889_p1284151282511"></a><a name="zh-cn_topic_0000002458194889_p1284151282511"></a><strong id="zh-cn_topic_0000002458194889_b738561020813"><a name="zh-cn_topic_0000002458194889_b738561020813"></a><a name="zh-cn_topic_0000002458194889_b738561020813"></a>DTCM</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%"><p id="zh-cn_topic_0000002458194889_p28401220250"><a name="zh-cn_topic_0000002458194889_p28401220250"></a><a name="zh-cn_topic_0000002458194889_p28401220250"></a>Data Tightly-Coupled Memory</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002458194889_p484812182519"><a name="zh-cn_topic_0000002458194889_p484812182519"></a><a name="zh-cn_topic_0000002458194889_p484812182519"></a>数据紧耦合内存</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row579343523617"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row1675310135355"><td class="nocellnorowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p4754913173516"><a name="zh-cn_topic_0000002458194889_p4754913173516"></a><a name="zh-cn_topic_0000002458194889_p4754913173516"></a><strong id="zh-cn_topic_0000002458194889_b2386510789"><a name="zh-cn_topic_0000002458194889_b2386510789"></a><a name="zh-cn_topic_0000002458194889_b2386510789"></a>E</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row3754141343516"><td class="nocellnorowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p0754513193511"><a name="zh-cn_topic_0000002458194889_p0754513193511"></a><a name="zh-cn_topic_0000002458194889_p0754513193511"></a><strong id="zh-cn_topic_0000002458194889_b638713101787"><a name="zh-cn_topic_0000002458194889_b638713101787"></a><a name="zh-cn_topic_0000002458194889_b638713101787"></a>EVB</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="41.89418941894189%"><p id="zh-cn_topic_0000002458194889_p675441323517"><a name="zh-cn_topic_0000002458194889_p675441323517"></a><a name="zh-cn_topic_0000002458194889_p675441323517"></a>Evaluate Verification Board</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002458194889_p14754313163514"><a name="zh-cn_topic_0000002458194889_p14754313163514"></a><a name="zh-cn_topic_0000002458194889_p14754313163514"></a>评估验证板</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row5746125614287"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row47465565289"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p10746125682812"><a name="zh-cn_topic_0000002458194889_p10746125682812"></a><a name="zh-cn_topic_0000002458194889_p10746125682812"></a><strong id="zh-cn_topic_0000002458194889_b238819101589"><a name="zh-cn_topic_0000002458194889_b238819101589"></a><a name="zh-cn_topic_0000002458194889_b238819101589"></a>G</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row1974614562284"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p107461156182814"><a name="zh-cn_topic_0000002458194889_p107461156182814"></a><a name="zh-cn_topic_0000002458194889_p107461156182814"></a><strong id="zh-cn_topic_0000002458194889_b138914100820"><a name="zh-cn_topic_0000002458194889_b138914100820"></a><a name="zh-cn_topic_0000002458194889_b138914100820"></a>GPU</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%"><p id="zh-cn_topic_0000002458194889_p20746356192811"><a name="zh-cn_topic_0000002458194889_p20746356192811"></a><a name="zh-cn_topic_0000002458194889_p20746356192811"></a>Graphical Processing Unit</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002458194889_p97461256142812"><a name="zh-cn_topic_0000002458194889_p97461256142812"></a><a name="zh-cn_topic_0000002458194889_p97461256142812"></a>图形处理单元</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row1928220259255"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row3282202511251"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p192821025132510"><a name="zh-cn_topic_0000002458194889_p192821025132510"></a><a name="zh-cn_topic_0000002458194889_p192821025132510"></a><strong id="zh-cn_topic_0000002458194889_b6390181014815"><a name="zh-cn_topic_0000002458194889_b6390181014815"></a><a name="zh-cn_topic_0000002458194889_b6390181014815"></a>I</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row728210250255"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p12821625102514"><a name="zh-cn_topic_0000002458194889_p12821625102514"></a><a name="zh-cn_topic_0000002458194889_p12821625102514"></a><strong id="zh-cn_topic_0000002458194889_b5390151013814"><a name="zh-cn_topic_0000002458194889_b5390151013814"></a><a name="zh-cn_topic_0000002458194889_b5390151013814"></a>ITCM</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%"><p id="zh-cn_topic_0000002458194889_p4282125142510"><a name="zh-cn_topic_0000002458194889_p4282125142510"></a><a name="zh-cn_topic_0000002458194889_p4282125142510"></a>Instruction Tightly-Coupled Memory</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002458194889_p12282142514251"><a name="zh-cn_topic_0000002458194889_p12282142514251"></a><a name="zh-cn_topic_0000002458194889_p12282142514251"></a>指令紧密耦合内存</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row875431315358"><td class="nocellnorowborder" style="border:none" valign="top" width="24.772477247724773%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row11805643182516"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p1680524362515"><a name="zh-cn_topic_0000002458194889_p1680524362515"></a><a name="zh-cn_topic_0000002458194889_p1680524362515"></a><strong id="zh-cn_topic_0000002458194889_b1439161016815"><a name="zh-cn_topic_0000002458194889_b1439161016815"></a><a name="zh-cn_topic_0000002458194889_b1439161016815"></a>L</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row12805124315258"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p12805204312251"><a name="zh-cn_topic_0000002458194889_p12805204312251"></a><a name="zh-cn_topic_0000002458194889_p12805204312251"></a><strong id="zh-cn_topic_0000002458194889_b13922101082"><a name="zh-cn_topic_0000002458194889_b13922101082"></a><a name="zh-cn_topic_0000002458194889_b13922101082"></a>L2MEM</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%"><p id="zh-cn_topic_0000002458194889_p48058433256"><a name="zh-cn_topic_0000002458194889_p48058433256"></a><a name="zh-cn_topic_0000002458194889_p48058433256"></a>Level-2-Memory</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002458194889_p14805843192513"><a name="zh-cn_topic_0000002458194889_p14805843192513"></a><a name="zh-cn_topic_0000002458194889_p14805843192513"></a>二级存储器</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row2805443112510"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row137557406396"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p1175513408399"><a name="zh-cn_topic_0000002458194889_p1175513408399"></a><a name="zh-cn_topic_0000002458194889_p1175513408399"></a><strong id="zh-cn_topic_0000002458194889_b23931103817"><a name="zh-cn_topic_0000002458194889_b23931103817"></a><a name="zh-cn_topic_0000002458194889_b23931103817"></a>O</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row117541413203520"><td class="nocellnorowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p0754191319352"><a name="zh-cn_topic_0000002458194889_p0754191319352"></a><a name="zh-cn_topic_0000002458194889_p0754191319352"></a><strong id="zh-cn_topic_0000002458194889_b439418105810"><a name="zh-cn_topic_0000002458194889_b439418105810"></a><a name="zh-cn_topic_0000002458194889_b439418105810"></a>OTA</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="41.89418941894189%"><p id="zh-cn_topic_0000002458194889_p106431034014"><a name="zh-cn_topic_0000002458194889_p106431034014"></a><a name="zh-cn_topic_0000002458194889_p106431034014"></a>Over the Air</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002458194889_p206418101401"><a name="zh-cn_topic_0000002458194889_p206418101401"></a><a name="zh-cn_topic_0000002458194889_p206418101401"></a>空中下载</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row1575471313356"><td class="nocellnorowborder" style="border:none" valign="top" width="24.772477247724773%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row16754101363513"><td class="nocellnorowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p87541213173517"><a name="zh-cn_topic_0000002458194889_p87541213173517"></a><a name="zh-cn_topic_0000002458194889_p87541213173517"></a><strong id="zh-cn_topic_0000002458194889_b43941010187"><a name="zh-cn_topic_0000002458194889_b43941010187"></a><a name="zh-cn_topic_0000002458194889_b43941010187"></a>P</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row1775418131353"><td class="nocellnorowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p9754413153515"><a name="zh-cn_topic_0000002458194889_p9754413153515"></a><a name="zh-cn_topic_0000002458194889_p9754413153515"></a><strong id="zh-cn_topic_0000002458194889_b183957101882"><a name="zh-cn_topic_0000002458194889_b183957101882"></a><a name="zh-cn_topic_0000002458194889_b183957101882"></a>PSRAM</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="41.89418941894189%"><p id="zh-cn_topic_0000002458194889_p17754513193511"><a name="zh-cn_topic_0000002458194889_p17754513193511"></a><a name="zh-cn_topic_0000002458194889_p17754513193511"></a>Pseudostatic RAM</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002458194889_p15754713203517"><a name="zh-cn_topic_0000002458194889_p15754713203517"></a><a name="zh-cn_topic_0000002458194889_p15754713203517"></a>伪静态随机存储器</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row1275581373515"><td class="nocellnorowborder" style="border:none" valign="top" width="24.772477247724773%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row8533143118266"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p653373152613"><a name="zh-cn_topic_0000002458194889_p653373152613"></a><a name="zh-cn_topic_0000002458194889_p653373152613"></a><strong id="zh-cn_topic_0000002458194889_b63968108817"><a name="zh-cn_topic_0000002458194889_b63968108817"></a><a name="zh-cn_topic_0000002458194889_b63968108817"></a>S</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002458194889_row1453373152619"><td class="cellrowborder" style="border:none" valign="top" width="24.772477247724773%"><p id="zh-cn_topic_0000002458194889_p14533631142619"><a name="zh-cn_topic_0000002458194889_p14533631142619"></a><a name="zh-cn_topic_0000002458194889_p14533631142619"></a><strong id="zh-cn_topic_0000002458194889_b2397201011820"><a name="zh-cn_topic_0000002458194889_b2397201011820"></a><a name="zh-cn_topic_0000002458194889_b2397201011820"></a>SRAM</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="41.89418941894189%"><p id="zh-cn_topic_0000002458194889_p453373118266"><a name="zh-cn_topic_0000002458194889_p453373118266"></a><a name="zh-cn_topic_0000002458194889_p453373118266"></a>Static Random Access Memory</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002458194889_p1653316313269"><a name="zh-cn_topic_0000002458194889_p1653316313269"></a><a name="zh-cn_topic_0000002458194889_p1653316313269"></a>静态随机存取存储器</p>
</td>
</tr>
</tbody>
</table>

