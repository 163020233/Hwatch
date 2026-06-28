# 前言<a name="ZH-CN_TOPIC_0000002266619301"></a>

**概述<a name="section4537382116410"></a>**

本文档详细的描述了HiDiTing产品GNSS特性的集成和应用，包括驱动适配、AT指令、基本示例、产测功能、固件升级以及常见问题。

**读者对象<a name="section4378592816410"></a>**

本文档主要适用于以下工程师：

-   系统设计工程师
-   产品软件开发工程师
-   技术支持工程师

**符号约定<a name="section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="table2622507016410"></a>
<table><thead align="left"><tr id="row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="p6450074116410"><a name="p6450074116410"></a><a name="p6450074116410"></a><strong id="b2136615816410"><a name="b2136615816410"></a><a name="b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="p5435366816410"><a name="p5435366816410"></a><a name="p5435366816410"></a><strong id="b5941558116410"><a name="b5941558116410"></a><a name="b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p3734547016410"><a name="p3734547016410"></a><a name="p3734547016410"></a><a name="image2670064316410"></a><a name="image2670064316410"></a><span><img class="" id="image2670064316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002266499393.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p1757432116410"><a name="p1757432116410"></a><a name="p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1432579516410"><a name="p1432579516410"></a><a name="p1432579516410"></a><a name="image4895582316410"></a><a name="image4895582316410"></a><span><img class="" id="image4895582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002231659896.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p959197916410"><a name="p959197916410"></a><a name="p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1232579516410"><a name="p1232579516410"></a><a name="p1232579516410"></a><a name="image1235582316410"></a><a name="image1235582316410"></a><span><img class="" id="image1235582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002231500052.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p123197916410"><a name="p123197916410"></a><a name="p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p2204984716410"><a name="p2204984716410"></a><a name="p2204984716410"></a><a name="image4504446716410"></a><a name="image4504446716410"></a><span><img class="" id="image4504446716410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002266619305.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4388861916410"><a name="p4388861916410"></a><a name="p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="p1238861916410"><a name="p1238861916410"></a><a name="p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p5555360116410"><a name="p5555360116410"></a><a name="p5555360116410"></a><a name="image799324016410"></a><a name="image799324016410"></a><span><img class="" id="image799324016410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002266499397.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4612588116410"><a name="p4612588116410"></a><a name="p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="p1232588116410"><a name="p1232588116410"></a><a name="p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# GNSS定位介绍<a name="ZH-CN_TOPIC_0000002313348289"></a>

GNSS（Global Navigation Satellite System）泛指所有的卫星导航系统，包括全球的、区域的和增强的，如美国GPS、俄罗斯Glonass、欧洲Galileo、中国BDS以及日本的QZSS。

-   **[GNSS功能介绍](#ZH-CN_TOPIC_0000002313509293)**  

-   **[GNSS特性规格](#ZH-CN_TOPIC_0000002280284198)**  

## GNSS功能介绍<a name="ZH-CN_TOPIC_0000002313509293"></a>

HiDiTing产品集成GNSS芯片，主MCU通过UART接口与GNSS芯片通信，MCU控制GNSS芯片的上下电、固件加载、命令配置、启动停止，GNSS芯片上报MCU位置、速度、时间、卫星状态、芯片维测等信息。

## GNSS特性规格<a name="ZH-CN_TOPIC_0000002280284198"></a>

-   支持GPS L1C/A、GLONASS L1OF、BDS B1I、Galileo E1C、QZSS L1C/A定位标准。
-   支持GPS、GLONASS、BDS、Galileo单模定位。
-   支持GPS、GLONASS、BDS、Galileo、QZSS多模定位。
-   支持AGNSS（Assisted Global Navigation Satellite System）、PGNSS。
-   支持自动干扰检测。
-   支持对外授时。
-   灵敏度：
    -   冷启动捕获灵敏度：-149dBm
    -   热启动捕获灵敏度：-159dBm
    -   跟踪灵敏度：-162dBm

-   TTFF：
    -   冷启动TTFF：avg≤34s@-130dBm
    -   热启动TTFF：avg≤1s@-130dBm

-   定位精度<sup>\[1\]</sup>：
    -   水平位置：1.5m
    -   水平速度：0.15m/s

-   应用极限：
    -   速度：500m/s
    -   高度：18000m

-   功耗<sup>\[2\]</sup>：
    -   捕获：6.40mA@3.6V
    -   跟踪：4.95mA@3.6V
    -   关机漏电流：14μA@3.6V

>![](public_sys-resources/icon-note.gif) **说明：** 
>\[1\] 定位精度数据在-130dBm信号源环境下测试，取CEP 68%统计值。
>\[2\] 功耗数据源自-130dBm信号源测试环境，仅包含GNSS芯片功耗，不含外设TCXO/LNA等器件功耗。

# GNSS特性集成<a name="ZH-CN_TOPIC_0000002278979758"></a>

-   **[GNSS架构框图](#ZH-CN_TOPIC_0000002278876584)**  

-   **[GNSS代码结构](#ZH-CN_TOPIC_0000002313509297)**  

-   **[GNSS驱动接口](#ZH-CN_TOPIC_0000002313476281)**  

-   **[GNSS应用接口](#ZH-CN_TOPIC_0000002278979762)**  

-   **[GNSS编译适配](#ZH-CN_TOPIC_0000002278876588)**  

-   **[GNSS固件升级](#ZH-CN_TOPIC_0000002278979766)**  

-   **[GNSS维测日志](#ZH-CN_TOPIC_0000002278876592)**  

## GNSS架构框图<a name="ZH-CN_TOPIC_0000002278876584"></a>

HiDiTing整体提供GNSS芯片及配套的软件包，包括GNSS SDK、GNSS驱动以及GNSS示例。软件分层如[图1](#fig202629443204)所示。

**图 1**  GNSS软件架构分层框图<a name="fig202629443204"></a>  
![](figures/GNSS软件架构分层框图.png "GNSS软件架构分层框图")

## GNSS代码结构<a name="ZH-CN_TOPIC_0000002313509297"></a>

GNSS相关代码在HiDiTing SDK中的目录结构如[表1](#table1054022012314)所示。

**表 1**  GNSS代码路径表

<a name="table1054022012314"></a>
<table><thead align="left"><tr id="row105401420132316"><th class="cellrowborder" valign="top" width="21.73%" id="mcps1.2.3.1.1"><p id="p19540112032319"><a name="p19540112032319"></a><a name="p19540112032319"></a>代码说明</p>
</th>
<th class="cellrowborder" valign="top" width="78.27%" id="mcps1.2.3.1.2"><p id="p15540132042320"><a name="p15540132042320"></a><a name="p15540132042320"></a>代码路径</p>
</th>
</tr>
</thead>
<tbody><tr id="row165402204233"><td class="cellrowborder" valign="top" width="21.73%" headers="mcps1.2.3.1.1 "><p id="p1554052010234"><a name="p1554052010234"></a><a name="p1554052010234"></a>功能示例代码</p>
</td>
<td class="cellrowborder" valign="top" width="78.27%" headers="mcps1.2.3.1.2 "><p id="p185418208235"><a name="p185418208235"></a><a name="p185418208235"></a>application/samples/gnss</p>
</td>
</tr>
<tr id="row135411520162311"><td class="cellrowborder" valign="top" width="21.73%" headers="mcps1.2.3.1.1 "><p id="p454117204232"><a name="p454117204232"></a><a name="p454117204232"></a>应用接口代码</p>
</td>
<td class="cellrowborder" valign="top" width="78.27%" headers="mcps1.2.3.1.2 "><p id="p4541152020231"><a name="p4541152020231"></a><a name="p4541152020231"></a>middleware/chips/brandy/gnss</p>
</td>
</tr>
<tr id="row1854113203234"><td class="cellrowborder" valign="top" width="21.73%" headers="mcps1.2.3.1.1 "><p id="p17541420162312"><a name="p17541420162312"></a><a name="p17541420162312"></a>驱动接口代码</p>
</td>
<td class="cellrowborder" valign="top" width="78.27%" headers="mcps1.2.3.1.2 "><p id="p4541920162318"><a name="p4541920162318"></a><a name="p4541920162318"></a>middleware/services/srv_tiot_host/tiot_driver</p>
</td>
</tr>
</tbody>
</table>

## GNSS驱动接口<a name="ZH-CN_TOPIC_0000002313476281"></a>

GNSS驱动接口对接系统SDK以及板极接口，用于适配GNSS芯片上下电，以及GNSS芯片与主MCU芯片的通信交互。

此部分驱动接口，用户无需直接调用。

**表 1**  GNSS驱动接口

<a name="table191641626142515"></a>
<table><thead align="left"><tr id="row816482618259"><th class="cellrowborder" valign="top" width="33.3033303330333%" id="mcps1.2.4.1.1"><p id="p1716514266253"><a name="p1716514266253"></a><a name="p1716514266253"></a>接口名称</p>
</th>
<th class="cellrowborder" valign="top" width="33.36333633363336%" id="mcps1.2.4.1.2"><p id="p0165526112514"><a name="p0165526112514"></a><a name="p0165526112514"></a>参数说明</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="p21651026182517"><a name="p21651026182517"></a><a name="p21651026182517"></a>功能介绍</p>
</th>
</tr>
</thead>
<tbody><tr id="row116542618258"><td class="cellrowborder" valign="top" width="33.3033303330333%" headers="mcps1.2.4.1.1 "><p id="p86234568203"><a name="p86234568203"></a><a name="p86234568203"></a>int32_t tiot_service_init(void)</p>
</td>
<td class="cellrowborder" valign="top" width="33.36333633363336%" headers="mcps1.2.4.1.2 "><p id="p161651426142517"><a name="p161651426142517"></a><a name="p161651426142517"></a>NA</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p201659267255"><a name="p201659267255"></a><a name="p201659267255"></a>tiot service初始化接口。</p>
</td>
</tr>
<tr id="row952620551263"><td class="cellrowborder" valign="top" width="33.3033303330333%" headers="mcps1.2.4.1.1 "><p id="p131651262259"><a name="p131651262259"></a><a name="p131651262259"></a>tiot_handle tiot_service_open(const char *device_name, void *param)</p>
</td>
<td class="cellrowborder" valign="top" width="33.36333633363336%" headers="mcps1.2.4.1.2 "><p id="p1916542632513"><a name="p1916542632513"></a><a name="p1916542632513"></a>device_name：设备名称。</p>
<p id="p2062019612302"><a name="p2062019612302"></a><a name="p2062019612302"></a>param：回调函数地址及参数，详见tiot_service_open_param类型。</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p4165326192518"><a name="p4165326192518"></a><a name="p4165326192518"></a>开启tiot service。</p>
</td>
</tr>
<tr id="row151651326152519"><td class="cellrowborder" valign="top" width="33.3033303330333%" headers="mcps1.2.4.1.1 "><p id="p171659264258"><a name="p171659264258"></a><a name="p171659264258"></a>int32_t tiot_service_write(tiot_handle handle, const uint8_t *data, uint32_t len)</p>
</td>
<td class="cellrowborder" valign="top" width="33.36333633363336%" headers="mcps1.2.4.1.2 "><p id="p1616512619255"><a name="p1616512619255"></a><a name="p1616512619255"></a>handle：tiot句柄。</p>
<p id="p1841214262374"><a name="p1841214262374"></a><a name="p1841214262374"></a>data：待写入数据的地址。</p>
<p id="p187313305370"><a name="p187313305370"></a><a name="p187313305370"></a>len：待写入数据的长度。</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p11165122610257"><a name="p11165122610257"></a><a name="p11165122610257"></a>通过tiot service进行数据传输。</p>
</td>
</tr>
<tr id="row969441112277"><td class="cellrowborder" valign="top" width="33.3033303330333%" headers="mcps1.2.4.1.1 "><p id="p3812438139"><a name="p3812438139"></a><a name="p3812438139"></a>void tiot_service_close(tiot_handle handle)</p>
</td>
<td class="cellrowborder" valign="top" width="33.36333633363336%" headers="mcps1.2.4.1.2 "><p id="p67811137201212"><a name="p67811137201212"></a><a name="p67811137201212"></a>handle：tiot句柄。</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p15454131312138"><a name="p15454131312138"></a><a name="p15454131312138"></a>关闭tiot service。</p>
</td>
</tr>
<tr id="row729617142212"><td class="cellrowborder" valign="top" width="33.3033303330333%" headers="mcps1.2.4.1.1 "><p id="p8198175110127"><a name="p8198175110127"></a><a name="p8198175110127"></a>void tiot_service_deinit(void)</p>
</td>
<td class="cellrowborder" valign="top" width="33.36333633363336%" headers="mcps1.2.4.1.2 "><p id="p1619815510123"><a name="p1619815510123"></a><a name="p1619815510123"></a>NA</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p348513173131"><a name="p348513173131"></a><a name="p348513173131"></a>tiot service去初始化。</p>
</td>
</tr>
</tbody>
</table>

## GNSS应用接口<a name="ZH-CN_TOPIC_0000002278979762"></a>

GNSS应用接口调用并封装了GNSS驱动接口，作为GNSS SDK，提供GNSS API，供用户进行APP开发。

**表 1**  GNSS应用接口

<a name="table154341417152811"></a>
<table><thead align="left"><tr id="row1843481782810"><th class="cellrowborder" valign="top" width="37.23372337233724%" id="mcps1.2.4.1.1"><p id="p4434161715287"><a name="p4434161715287"></a><a name="p4434161715287"></a>接口名称</p>
</th>
<th class="cellrowborder" valign="top" width="29.432943294329434%" id="mcps1.2.4.1.2"><p id="p143441722813"><a name="p143441722813"></a><a name="p143441722813"></a>参数说明</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="p2434111713289"><a name="p2434111713289"></a><a name="p2434111713289"></a>功能介绍</p>
</th>
</tr>
</thead>
<tbody><tr id="row86738572232"><td class="cellrowborder" valign="top" width="37.23372337233724%" headers="mcps1.2.4.1.1 "><p id="p20011652413"><a name="p20011652413"></a><a name="p20011652413"></a>errcode_t uapi_gnss_power_on(void)</p>
</td>
<td class="cellrowborder" valign="top" width="29.432943294329434%" headers="mcps1.2.4.1.2 "><p id="p443441782814"><a name="p443441782814"></a><a name="p443441782814"></a>NA</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p1243491752817"><a name="p1243491752817"></a><a name="p1243491752817"></a>GNSS芯片上电。</p>
</td>
</tr>
<tr id="row18434117102813"><td class="cellrowborder" valign="top" width="37.23372337233724%" headers="mcps1.2.4.1.1 "><p id="p184341017162817"><a name="p184341017162817"></a><a name="p184341017162817"></a>errcode_t uapi_gnss_init(void)</p>
</td>
<td class="cellrowborder" valign="top" width="29.432943294329434%" headers="mcps1.2.4.1.2 "><p id="p8434317112819"><a name="p8434317112819"></a><a name="p8434317112819"></a>NA</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p20434191713285"><a name="p20434191713285"></a><a name="p20434191713285"></a>初始化，创建GNSS任务线程。</p>
</td>
</tr>
<tr id="row0263200162514"><td class="cellrowborder" valign="top" width="37.23372337233724%" headers="mcps1.2.4.1.1 "><p id="p10366741181715"><a name="p10366741181715"></a><a name="p10366741181715"></a>errcode_t uapi_gnss_register_callback(uint32_t callback_type, gnss_callback_func callback_func)</p>
</td>
<td class="cellrowborder" valign="top" width="29.432943294329434%" headers="mcps1.2.4.1.2 "><p id="p3468202310176"><a name="p3468202310176"></a><a name="p3468202310176"></a>callback_type：回调消息类型。</p>
<p id="p68671754102216"><a name="p68671754102216"></a><a name="p68671754102216"></a>callback_func：回调函数地址。</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p17160123941714"><a name="p17160123941714"></a><a name="p17160123941714"></a>GNSS消息回调函数注册接口。</p>
</td>
</tr>
<tr id="row183471829122510"><td class="cellrowborder" valign="top" width="37.23372337233724%" headers="mcps1.2.4.1.1 "><p id="p1849154031510"><a name="p1849154031510"></a><a name="p1849154031510"></a>errcode_t  uapi_gnss_open(void)</p>
</td>
<td class="cellrowborder" valign="top" width="29.432943294329434%" headers="mcps1.2.4.1.2 "><p id="p1147373718191"><a name="p1147373718191"></a><a name="p1147373718191"></a>NA</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p12771193101517"><a name="p12771193101517"></a><a name="p12771193101517"></a>GNSS加载接口，打开设备节点并加载固件。需要确保调用前板级已给GNSS芯片上电。</p>
</td>
</tr>
<tr id="row151367592464"><td class="cellrowborder" valign="top" width="37.23372337233724%" headers="mcps1.2.4.1.1 "><p id="p154701241194710"><a name="p154701241194710"></a><a name="p154701241194710"></a>errcode_t uapi_gnss_config(gnss_message_t *msg)</p>
</td>
<td class="cellrowborder" valign="top" width="29.432943294329434%" headers="mcps1.2.4.1.2 "><p id="p3136115918466"><a name="p3136115918466"></a><a name="p3136115918466"></a>msg：配置项指针消息。</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p813625994619"><a name="p813625994619"></a><a name="p813625994619"></a>GNSS配置接口，需在调用GNSS加载接口后调用此接口，部分配置项需在调用GNSS启动接口前调用。</p>
</td>
</tr>
<tr id="row841766192616"><td class="cellrowborder" valign="top" width="37.23372337233724%" headers="mcps1.2.4.1.1 "><p id="p1914751420150"><a name="p1914751420150"></a><a name="p1914751420150"></a>errcode_t  uapi_gnss_start(void)</p>
</td>
<td class="cellrowborder" valign="top" width="29.432943294329434%" headers="mcps1.2.4.1.2 "><p id="p17481926122313"><a name="p17481926122313"></a><a name="p17481926122313"></a>NA</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p21472148153"><a name="p21472148153"></a><a name="p21472148153"></a>GNSS启动接口，GNSS开始上报定位信息。需在调用初始化接口后调用。</p>
</td>
</tr>
<tr id="row57851636192610"><td class="cellrowborder" valign="top" width="37.23372337233724%" headers="mcps1.2.4.1.1 "><p id="p564911781517"><a name="p564911781517"></a><a name="p564911781517"></a>errcode_t  uapi_gnss_stop(void)</p>
</td>
<td class="cellrowborder" valign="top" width="29.432943294329434%" headers="mcps1.2.4.1.2 "><p id="p33907278239"><a name="p33907278239"></a><a name="p33907278239"></a>NA</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p1264931715157"><a name="p1264931715157"></a><a name="p1264931715157"></a>GNSS停止接口，GNSS停止上报定位信息。需在调用启动接口后调用。</p>
</td>
</tr>
<tr id="row106741522716"><td class="cellrowborder" valign="top" width="37.23372337233724%" headers="mcps1.2.4.1.1 "><p id="p0212107171519"><a name="p0212107171519"></a><a name="p0212107171519"></a>errcode_t  uapi_gnss_close(void)</p>
</td>
<td class="cellrowborder" valign="top" width="29.432943294329434%" headers="mcps1.2.4.1.2 "><p id="p163275380192"><a name="p163275380192"></a><a name="p163275380192"></a>NA</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p421214713151"><a name="p421214713151"></a><a name="p421214713151"></a>GNSS卸载接口，关闭GNSS设备节点。调用此接口后方可执行GNSS芯片板级下电。</p>
</td>
</tr>
<tr id="row6434111702812"><td class="cellrowborder" valign="top" width="37.23372337233724%" headers="mcps1.2.4.1.1 "><p id="p15368235142"><a name="p15368235142"></a><a name="p15368235142"></a>uapi_gnss_deinit(void)</p>
</td>
<td class="cellrowborder" valign="top" width="29.432943294329434%" headers="mcps1.2.4.1.2 "><p id="p73513230145"><a name="p73513230145"></a><a name="p73513230145"></a>NA</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p119151942131417"><a name="p119151942131417"></a><a name="p119151942131417"></a>GNSS去初始化接口，退出任务线程。</p>
</td>
</tr>
<tr id="row0434151718282"><td class="cellrowborder" valign="top" width="37.23372337233724%" headers="mcps1.2.4.1.1 "><p id="p154353173288"><a name="p154353173288"></a><a name="p154353173288"></a>uapi_gnss_power_off(void)</p>
</td>
<td class="cellrowborder" valign="top" width="29.432943294329434%" headers="mcps1.2.4.1.2 "><p id="p1643510170289"><a name="p1643510170289"></a><a name="p1643510170289"></a>NA</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p194351317122812"><a name="p194351317122812"></a><a name="p194351317122812"></a>GNSS下电接口。</p>
</td>
</tr>
</tbody>
</table>

## GNSS编译适配<a name="ZH-CN_TOPIC_0000002278876588"></a>

-   **[代码适配](#ZH-CN_TOPIC_0000002313509301)**  

-   **[版本编译](#ZH-CN_TOPIC_0000002313476285)**  

### 代码适配<a name="ZH-CN_TOPIC_0000002313509301"></a>

HiDiTing中已集成GNSS相关代码，无需用户适配。

### 版本编译<a name="ZH-CN_TOPIC_0000002313476285"></a>

GNSS相关代码会编译至application对应的target中，无须用户额外编译target。

## GNSS固件升级<a name="ZH-CN_TOPIC_0000002278979766"></a>

GNSS默认使用资源文件方式加载固件，用户替换该资源文件，即可实现GNSS固件独立升级。

GNSS固件相关信息如[表1](#table2473653194820)所示。

**表 1**  GNSS固件信息

<a name="table2473653194820"></a>
<table><thead align="left"><tr id="row16473165384815"><th class="cellrowborder" valign="top" width="25.88%" id="mcps1.2.4.1.1"><p id="p174731553164810"><a name="p174731553164810"></a><a name="p174731553164810"></a>固件名称</p>
</th>
<th class="cellrowborder" valign="top" width="31.95%" id="mcps1.2.4.1.2"><p id="p347316538487"><a name="p347316538487"></a><a name="p347316538487"></a>资源路径</p>
</th>
<th class="cellrowborder" valign="top" width="42.17%" id="mcps1.2.4.1.3"><p id="p0830182931516"><a name="p0830182931516"></a><a name="p0830182931516"></a>tartget</p>
</th>
</tr>
</thead>
<tbody><tr id="row16473185314486"><td class="cellrowborder" valign="top" width="25.88%" headers="mcps1.2.4.1.1 "><p id="p7473185312480"><a name="p7473185312480"></a><a name="p7473185312480"></a>gnss_config.bin</p>
</td>
<td class="cellrowborder" valign="top" width="31.95%" headers="mcps1.2.4.1.2 "><p id="p1647314533482"><a name="p1647314533482"></a><a name="p1647314533482"></a>/boot/gnss/gnss_config.bin</p>
</td>
<td class="cellrowborder" valign="top" width="42.17%" headers="mcps1.2.4.1.3 "><p id="p3830112911516"><a name="p3830112911516"></a><a name="p3830112911516"></a>diting-community</p>
</td>
</tr>
<tr id="row8368152641611"><td class="cellrowborder" valign="top" width="25.88%" headers="mcps1.2.4.1.1 "><p id="p136815268165"><a name="p136815268165"></a><a name="p136815268165"></a>gnss_config.bin</p>
</td>
<td class="cellrowborder" valign="top" width="31.95%" headers="mcps1.2.4.1.2 "><p id="p03682026191614"><a name="p03682026191614"></a><a name="p03682026191614"></a>/user/gnss/firmware/gnss_config.bin</p>
</td>
<td class="cellrowborder" valign="top" width="42.17%" headers="mcps1.2.4.1.3 "><p id="p1536815265163"><a name="p1536815265163"></a><a name="p1536815265163"></a>3322-single-norflash</p>
</td>
</tr>
</tbody>
</table>

固件升级步骤：

1.  获取“gnss\_config.bin”。
2.  将“gnss\_config.bin”更新至系统资源文件对应路径。
3.  重启系统，完成升级。

## GNSS维测日志<a name="ZH-CN_TOPIC_0000002278876592"></a>

GNSS维测日志包括芯片上报日志和用户自定义日志。

-   **[GNSS芯片日志](#ZH-CN_TOPIC_0000002315191789)**  

-   **[用户自定义日志](#ZH-CN_TOPIC_0000002280519082)**  

### GNSS芯片日志<a name="ZH-CN_TOPIC_0000002315191789"></a>

GNSS芯片日志用于存储GNSS芯片通过UART上报给MCU的日志，包括文本格式的NMEA定位结果，以及二进制格式的REPLAY维测数据。

在HiDiTing SDK中，GNSS功能示例部分，提供了存储GNSS芯片日志的示例，将文本和二进制数据混合存储在同一个日志文件中。

用户修改时，需确保GNSS芯片日志的完整性，不可将无关数据追加到GNSS芯片日志中。

### 用户自定义日志<a name="ZH-CN_TOPIC_0000002280519082"></a>

用户自定义日志用于记录系统关键流程和异常行为，辅助定位问题，此部分无示例，需用户按需自行实现。

# GNSS功能示例<a name="ZH-CN_TOPIC_0000002313509305"></a>

GNSS功能示例基于GNSS API实现，提供了相对完整的GNSS定位功能，旨在指导用户在APP中进行GNSS应用的开发。

-   **[AT指令集合](#ZH-CN_TOPIC_0000002313476289)**  

-   **[维测示例](#ZH-CN_TOPIC_0000002278979770)**  

-   **[冷启动定位](#ZH-CN_TOPIC_0000002278876596)**  

-   **[热启动定位](#ZH-CN_TOPIC_0000002313509309)**  

-   **[AGNSS辅助定位](#ZH-CN_TOPIC_0000002313476293)**  

-   **[PGNSS辅助定位](#ZH-CN_TOPIC_0000002278979774)**  

## AT指令集合<a name="ZH-CN_TOPIC_0000002313476289"></a>

**表 1**  GNSS AT指令集合

<a name="table1075018241263"></a>
<table><thead align="left"><tr id="row207507241362"><th class="cellrowborder" valign="top" width="26.68266826682668%" id="mcps1.2.4.1.1"><p id="p1975052411610"><a name="p1975052411610"></a><a name="p1975052411610"></a>AT指令</p>
</th>
<th class="cellrowborder" valign="top" width="39.93399339933993%" id="mcps1.2.4.1.2"><p id="p11750224961"><a name="p11750224961"></a><a name="p11750224961"></a>参数说明</p>
</th>
<th class="cellrowborder" valign="top" width="33.383338333833386%" id="mcps1.2.4.1.3"><p id="p17751124461"><a name="p17751124461"></a><a name="p17751124461"></a>功能介绍</p>
</th>
</tr>
</thead>
<tbody><tr id="row17751224664"><td class="cellrowborder" valign="top" width="26.68266826682668%" headers="mcps1.2.4.1.1 "><p id="p97512241560"><a name="p97512241560"></a><a name="p97512241560"></a>AT^GNSSINIT=1,1,0,0</p>
</td>
<td class="cellrowborder" valign="top" width="39.93399339933993%" headers="mcps1.2.4.1.2 "><p id="p759915311141"><a name="p759915311141"></a><a name="p759915311141"></a>0：禁用；1：使能。</p>
<a name="ul205008369519"></a><a name="ul205008369519"></a><ul id="ul205008369519"><li>参数1：保存NMEA到离线文件。</li><li>参数2：保存REPLAY到离线文件。</li><li>参数3：保留。</li><li>参数4：保留。</li></ul>
</td>
<td class="cellrowborder" valign="top" width="33.383338333833386%" headers="mcps1.2.4.1.3 "><p id="p17511724464"><a name="p17511724464"></a><a name="p17511724464"></a>执行GNSS初始化，包括上电、加载固件，初始化线程。</p>
</td>
</tr>
<tr id="row97511024466"><td class="cellrowborder" valign="top" width="26.68266826682668%" headers="mcps1.2.4.1.1 "><p id="p275118241668"><a name="p275118241668"></a><a name="p275118241668"></a>AT^GNSSHEX=xxx</p>
</td>
<td class="cellrowborder" valign="top" width="39.93399339933993%" headers="mcps1.2.4.1.2 "><p id="p577414544513"><a name="p577414544513"></a><a name="p577414544513"></a>16进制配置指令，不加空格。</p>
</td>
<td class="cellrowborder" valign="top" width="33.383338333833386%" headers="mcps1.2.4.1.3 "><p id="p1975132415619"><a name="p1975132415619"></a><a name="p1975132415619"></a>下发配置指令，不带驱动的包头包尾，不加0x前缀，需在启动定位前配置，如配置请求版本号：<span>AT^GNSSHEX=0500000000000100000</span>。</p>
</td>
</tr>
<tr id="row8751152413614"><td class="cellrowborder" valign="top" width="26.68266826682668%" headers="mcps1.2.4.1.1 "><p id="p76291051282"><a name="p76291051282"></a><a name="p76291051282"></a>AT^GNSSSAMPLE=0</p>
</td>
<td class="cellrowborder" valign="top" width="39.93399339933993%" headers="mcps1.2.4.1.2 "><a name="ul1732491281116"></a><a name="ul1732491281116"></a><ul id="ul1732491281116"><li>0：获取当前支持的sample及编号。</li><li>1：GNSS冷启动用例配置。</li><li>2：GNSS热启动用例配置。</li><li>3：AGNSS辅助定位用例配置。</li><li>4：PGNSS辅助定位用例配置。</li><li>5：GNSS产测用例配置。</li></ul>
</td>
<td class="cellrowborder" valign="top" width="33.383338333833386%" headers="mcps1.2.4.1.3 "><p id="p17629257812"><a name="p17629257812"></a><a name="p17629257812"></a>下发指令sample的配置，注意此处仅下发配置，具体执行用例还需要执行“AT^GNSSSTART”。</p>
<p id="p1843012538309"><a name="p1843012538309"></a><a name="p1843012538309"></a>随着用例的扩展，部分用例编号可能会有变动。</p>
</td>
</tr>
<tr id="row10751424067"><td class="cellrowborder" valign="top" width="26.68266826682668%" headers="mcps1.2.4.1.1 "><p id="p387121218103"><a name="p387121218103"></a><a name="p387121218103"></a>AT^GNSSSTART</p>
</td>
<td class="cellrowborder" valign="top" width="39.93399339933993%" headers="mcps1.2.4.1.2 "><p id="p17871312171014"><a name="p17871312171014"></a><a name="p17871312171014"></a>NA</p>
</td>
<td class="cellrowborder" valign="top" width="33.383338333833386%" headers="mcps1.2.4.1.3 "><p id="p88717125100"><a name="p88717125100"></a><a name="p88717125100"></a>启动GNSS定位。</p>
</td>
</tr>
<tr id="row5466856201211"><td class="cellrowborder" valign="top" width="26.68266826682668%" headers="mcps1.2.4.1.1 "><p id="p19467125611218"><a name="p19467125611218"></a><a name="p19467125611218"></a>AT^GNSSSTOP</p>
</td>
<td class="cellrowborder" valign="top" width="39.93399339933993%" headers="mcps1.2.4.1.2 "><p id="p1246717561128"><a name="p1246717561128"></a><a name="p1246717561128"></a>NA</p>
</td>
<td class="cellrowborder" valign="top" width="33.383338333833386%" headers="mcps1.2.4.1.3 "><p id="p94679569129"><a name="p94679569129"></a><a name="p94679569129"></a>停止GNSS定位。</p>
</td>
</tr>
<tr id="row159101256191219"><td class="cellrowborder" valign="top" width="26.68266826682668%" headers="mcps1.2.4.1.1 "><p id="p19910135691215"><a name="p19910135691215"></a><a name="p19910135691215"></a>AT^GNSSDEINIT</p>
</td>
<td class="cellrowborder" valign="top" width="39.93399339933993%" headers="mcps1.2.4.1.2 "><p id="p1391015681220"><a name="p1391015681220"></a><a name="p1391015681220"></a>NA</p>
</td>
<td class="cellrowborder" valign="top" width="33.383338333833386%" headers="mcps1.2.4.1.3 "><p id="p1391015616122"><a name="p1391015616122"></a><a name="p1391015616122"></a>执行GNSS去初始化，包括退出线程、下电。</p>
</td>
</tr>
<tr id="row152480571125"><td class="cellrowborder" valign="top" width="26.68266826682668%" headers="mcps1.2.4.1.1 "><p id="p42481057151220"><a name="p42481057151220"></a><a name="p42481057151220"></a>AT^HSUARTREINIT</p>
</td>
<td class="cellrowborder" valign="top" width="39.93399339933993%" headers="mcps1.2.4.1.2 "><p id="p5248457141220"><a name="p5248457141220"></a><a name="p5248457141220"></a>NA</p>
</td>
<td class="cellrowborder" valign="top" width="33.383338333833386%" headers="mcps1.2.4.1.3 "><p id="p22481657121217"><a name="p22481657121217"></a><a name="p22481657121217"></a>重新初始化HSUART，使能Debugkits对接HSUART，需在“AT^GNSSDEINIT”之后执行。</p>
</td>
</tr>
</tbody>
</table>

## 维测示例<a name="ZH-CN_TOPIC_0000002278979770"></a>

-   **[使能控制](#ZH-CN_TOPIC_0000002279994148)**  

-   **[日志路径](#ZH-CN_TOPIC_0000002279890914)**  

-   **[日志类型](#ZH-CN_TOPIC_0000002314570573)**  

-   **[日志内容](#ZH-CN_TOPIC_0000002314443653)**  

-   **[日志用途](#ZH-CN_TOPIC_0000002279994156)**  

### 使能控制<a name="ZH-CN_TOPIC_0000002279994148"></a>

在GNSS初始化时，通过“AT^GNSSINIT”的参数进行控制，详见[表1](#table1075018241263)。

### 日志路径<a name="ZH-CN_TOPIC_0000002279890914"></a>

默认日志路径为“/user/gnss/log/gnss\_data.log”，如需修改，用户可以在代码中重新指定路径。

### 日志类型<a name="ZH-CN_TOPIC_0000002314570573"></a>

日志采用追加形式生成，日志类型为文本与二进制的混合类型，从中可提取出NMEA文本文件。

### 日志内容<a name="ZH-CN_TOPIC_0000002314443653"></a>

根据GNSS初始化时的配置，日志文件可支持保存NMEA数据和GNSS芯片上报的REPLAY维测数据。

### 日志用途<a name="ZH-CN_TOPIC_0000002279994156"></a>

日志文件中的NEMA数据，可用于分析GNSS信号环境和定位结果。

日志文件中的REPLAY数据，可交由研发/技术支持人员处理，用于回放复现定位过程，支撑定位复杂问题。

## 冷启动定位<a name="ZH-CN_TOPIC_0000002278876596"></a>

-   **[场景定义](#ZH-CN_TOPIC_0000002314568097)**  

-   **[前置条件](#ZH-CN_TOPIC_0000002279991660)**  

-   **[操作步骤](#ZH-CN_TOPIC_0000002279888458)**  

-   **[结果确认](#ZH-CN_TOPIC_0000002314441181)**  

### 场景定义<a name="ZH-CN_TOPIC_0000002314568097"></a>

冷启动是指GNSS上电后，无任何时间、位置、星历等辅助信息，仅依赖GNSS芯片自身搜星定位的场景，开阔场景下通常需要30多秒完成定位。

### 前置条件<a name="ZH-CN_TOPIC_0000002279991660"></a>

-   GNSS芯片上电、并调用初始化接口实现初始化。
-   GNSS天线正常连接，且处于良好卫星信号环境，常见如开阔天空，或者传导接仿真器信号。

### 操作步骤<a name="ZH-CN_TOPIC_0000002279888458"></a>

1.  发送“AT^GNSSSAMPLE=1”，配置冷启动。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >GNSS芯片下电前，发送的配置指令会持续生效。

2.  发送“AT^GNSSSTART”，启动定位。
3.  接收NMEA，等待定位。
4.  发送“AT^GNSSSTOP”，停止定位。
5.  获取离线日志或者观察AT串口实时输出。

### 结果确认<a name="ZH-CN_TOPIC_0000002314441181"></a>

分析NMEA日志，当RMC语句定位状态字段（紧随时间戳之后的字段）为A时，表示定位成功。

## 热启动定位<a name="ZH-CN_TOPIC_0000002313509309"></a>

-   **[场景定义](#ZH-CN_TOPIC_0000002314568109)**  

-   **[前置条件](#ZH-CN_TOPIC_0000002279991668)**  

-   **[操作步骤](#ZH-CN_TOPIC_0000002279888462)**  

-   **[结果确认](#ZH-CN_TOPIC_0000002314441189)**  

### 场景定义<a name="ZH-CN_TOPIC_0000002314568109"></a>

热启动是指GNSS上电后，在上一次定位成功后，GNSS不下电的情况下，1分钟以内再次启动定位的场景，此时GNSS时间、位置、星历均有效，且时间精度较高，能够快速捕获上星，完成定位，开阔场景下通常在2s左右的时间完成定位。

### 前置条件<a name="ZH-CN_TOPIC_0000002279991668"></a>

-   GNSS芯片上电、并调用初始化接口实现初始化。
-   不下电期间，距上次成功定位时间不超过1分钟。
-   GNSS天线正常连接，且处于良好卫星信号环境，常见如开阔天空，或者传导接仿真器信号。

### 操作步骤<a name="ZH-CN_TOPIC_0000002279888462"></a>

1.  发送“AT^GNSSSAMPLE=2”，配置热启动（默认配置）。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >GNSS芯片下电前，发送的配置指令会持续生效。

2.  发送“AT^GNSSSTART”，启动定位。
3.  接收NMEA，等待定位。
4.  发送“AT^GNSSSTOP”，停止定位。
5.  获取离线日志或者观察AT串口实时输出。

### 结果确认<a name="ZH-CN_TOPIC_0000002314441189"></a>

分析NMEA日志，当RMC语句定位状态字段（紧随时间戳之后的字段）为A时，表示定位成功。

## AGNSS辅助定位<a name="ZH-CN_TOPIC_0000002313476293"></a>

-   **[场景定义](#ZH-CN_TOPIC_0000002314568117)**  

-   **[前置条件](#ZH-CN_TOPIC_0000002279991680)**  

-   **[操作步骤](#ZH-CN_TOPIC_0000002279888470)**  

-   **[结果确认](#ZH-CN_TOPIC_0000002314441205)**  

### 场景定义<a name="ZH-CN_TOPIC_0000002314568117"></a>

AGNSS辅助定位，是指在定位前，通过注入AGNSS在线辅助信息，给GNSS提供有效的时间、星历等信息，加速GNSS定位，开阔场景下通常在10s内完成定位。

### 前置条件<a name="ZH-CN_TOPIC_0000002279991680"></a>

-   GNSS芯片上电、并调用初始化接口实现初始化。
-   GNSS天线正常连接，且处于良好卫星信号环境，常见如开阔天空，或者传导接仿真器信号。
-   用户需确保“/user/gnss/xgnss”目录下，存在有效的AGNSS辅助星历原始文件，请参考《HiDiTingV100 XGNSS 开发指南》“AGNSS集成示例”章节。

### 操作步骤<a name="ZH-CN_TOPIC_0000002279888470"></a>

1.  发送“AT^GNSSSAMPLE=3”，配置AGNSS用例。
2.  发送“AT^GNSSSTART”，启动定位。
3.  接收NMEA，等待定位。
4.  发送“AT^GNSSSTOP”，停止定位。
5.  获取离线日志或者观察AT串口实时输出。

### 结果确认<a name="ZH-CN_TOPIC_0000002314441205"></a>

分析NMEA日志，注入AGNSS辅助信息后，PNTH001语句最后一个字段（有效星历数量）通常大于80；

分析NMEA日志，当RMC语句定位状态字段（紧随时间戳之后的字段）为A时，表示定位成功。

## PGNSS辅助定位<a name="ZH-CN_TOPIC_0000002278979774"></a>

-   **[场景定义](#ZH-CN_TOPIC_0000002314568133)**  

-   **[前置条件](#ZH-CN_TOPIC_0000002279991688)**  

-   **[操作步骤](#ZH-CN_TOPIC_0000002279888482)**  

-   **[结果确认](#ZH-CN_TOPIC_0000002314441217)**  

### 场景定义<a name="ZH-CN_TOPIC_0000002314568133"></a>

PGNSS辅助定位，是指在定位前，通过注入PGNSS离线辅助信息，给GNSS提供有效的时间、星历等信息，加速GNSS定位，开阔场景下通常在10s内完成定位。

### 前置条件<a name="ZH-CN_TOPIC_0000002279991688"></a>

-   GNSS芯片上电、并调用初始化接口实现初始化。
-   GNSS天线正常连接，且处于良好卫星信号环境，常见如开阔天空，或者传导接仿真器信号。
-   用户需确保“/user/gnss/xgnss”目录下，存在有效的经扩展后的PGNSS辅助星历文件，参考《HiDiTingV100 XGNSS 开发指南》PGNSS集成示例章节。

### 操作步骤<a name="ZH-CN_TOPIC_0000002279888482"></a>

1.  发送“AT^GNSSSAMPLE=4”，配置PGNSS用例。
2.  发送“AT^GNSSSTART”，启动定位。
3.  接收NMEA，等待定位。
4.  发送“AT^GNSSSTOP”，停止定位。
5.  获取离线日志或者观察AT串口实时输出。

### 结果确认<a name="ZH-CN_TOPIC_0000002314441217"></a>

分析NMEA日志，注入PGNSS辅助信息后，PNTH001语句最后一个字段（有效星历数量）通常大于80；

分析NMEA日志，当RMC语句定位状态字段（紧随时间戳之后的字段）为A时，表示定位成功。

# GNSS产线测试<a name="ZH-CN_TOPIC_0000002278876600"></a>

-   **[产测验证范围](#ZH-CN_TOPIC_0000002278979782)**  

-   **[产测方案架构](#ZH-CN_TOPIC_0000002313476305)**  

-   **[产测操作流程](#ZH-CN_TOPIC_0000002278979790)**  

## 产测验证范围<a name="ZH-CN_TOPIC_0000002278979782"></a>

-   **[验证指标](#ZH-CN_TOPIC_0000002278876608)**  

-   **[管脚覆盖](#ZH-CN_TOPIC_0000002313509317)**  

### 验证指标<a name="ZH-CN_TOPIC_0000002278876608"></a>

GNSS产测主要验证射频信号及TCXO时钟指标，确保板级射频链路设计及时钟选型满足要求。

-   CN0：验证GNSS接收到的信号强度，此指标会影响GNSS跟踪信号强度。
-   频偏：验证板级TCXO频偏，此指标会影响GNSS搜星能力。
-   频漂：验证板级TCXO频漂，此指标会影响GNSS信号解调能力。

### 管脚覆盖<a name="ZH-CN_TOPIC_0000002313509317"></a>

产测方案覆盖管脚示意参考[图1](#fig544812521275)，其中高亮管脚为产测可覆盖管脚。

**图 1**  GNSS产测方案覆盖管脚示意图<a name="fig544812521275"></a>  
![](figures/GNSS产测方案覆盖管脚示意图.png "GNSS产测方案覆盖管脚示意图")

产测方案覆盖的管脚情况如[表1](#table7303522122719)所示。

**表 1**  GNSS产测管脚覆盖情况

<a name="table7303522122719"></a>
<table><thead align="left"><tr id="row23036229272"><th class="cellrowborder" valign="top" width="8.48915108489151%" id="mcps1.2.6.1.1"><p id="p330322216278"><a name="p330322216278"></a><a name="p330322216278"></a>PIN NO</p>
</th>
<th class="cellrowborder" valign="top" width="16.48835116488351%" id="mcps1.2.6.1.2"><p id="p630332211278"><a name="p630332211278"></a><a name="p630332211278"></a>管脚名称</p>
</th>
<th class="cellrowborder" valign="top" width="11.798820117988202%" id="mcps1.2.6.1.3"><p id="p1330342272711"><a name="p1330342272711"></a><a name="p1330342272711"></a>I/O类型</p>
</th>
<th class="cellrowborder" valign="top" width="52.274772522747725%" id="mcps1.2.6.1.4"><p id="p19303622122711"><a name="p19303622122711"></a><a name="p19303622122711"></a>功能说明</p>
</th>
<th class="cellrowborder" valign="top" width="10.948905109489052%" id="mcps1.2.6.1.5"><p id="p184151208555"><a name="p184151208555"></a><a name="p184151208555"></a>覆盖情况</p>
</th>
</tr>
</thead>
<tbody><tr id="row143030222271"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p1230392242711"><a name="p1230392242711"></a><a name="p1230392242711"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p830362212274"><a name="p830362212274"></a><a name="p830362212274"></a>VDD_PLL</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p23033229277"><a name="p23033229277"></a><a name="p23033229277"></a>电源</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p430320223274"><a name="p430320223274"></a><a name="p430320223274"></a>PLL（Phase-Locked Loops）模块LDO（low dropout regulator）电源输出，外接1μF电容。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p441542017554"><a name="p441542017554"></a><a name="p441542017554"></a>YES</p>
</td>
</tr>
<tr id="row113031222142713"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p530310227271"><a name="p530310227271"></a><a name="p530310227271"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p630322214278"><a name="p630322214278"></a><a name="p630322214278"></a>VDD_XLDO</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p19303142215275"><a name="p19303142215275"></a><a name="p19303142215275"></a>电源</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p13304222182715"><a name="p13304222182715"></a><a name="p13304222182715"></a>TCXO LDO电源输出，外接1μF电容。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p1741572011557"><a name="p1741572011557"></a><a name="p1741572011557"></a>YES</p>
</td>
</tr>
<tr id="row19304172292716"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p14304112218272"><a name="p14304112218272"></a><a name="p14304112218272"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p13304152214275"><a name="p13304152214275"></a><a name="p13304152214275"></a>XREF</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p15304192282714"><a name="p15304192282714"></a><a name="p15304192282714"></a>时钟</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p13042222276"><a name="p13042222276"></a><a name="p13042222276"></a>TCXO时钟输入。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p18415132016556"><a name="p18415132016556"></a><a name="p18415132016556"></a>YES</p>
</td>
</tr>
<tr id="row1330402212711"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p183041922192717"><a name="p183041922192717"></a><a name="p183041922192717"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p17304722122715"><a name="p17304722122715"></a><a name="p17304722122715"></a>NC</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p73041322142718"><a name="p73041322142718"></a><a name="p73041322142718"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p123048221274"><a name="p123048221274"></a><a name="p123048221274"></a>空置。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p124151205553"><a name="p124151205553"></a><a name="p124151205553"></a>-</p>
</td>
</tr>
<tr id="row16304162212713"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p10304422152715"><a name="p10304422152715"></a><a name="p10304422152715"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p130472252718"><a name="p130472252718"></a><a name="p130472252718"></a>VDD_IO_1P8</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p1430432262711"><a name="p1430432262711"></a><a name="p1430432262711"></a>电源</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p173041822202718"><a name="p173041822202718"></a><a name="p173041822202718"></a>IO LDO电源输出，外接1μF电容。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p64154205559"><a name="p64154205559"></a><a name="p64154205559"></a>YES</p>
</td>
</tr>
<tr id="row030419226276"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p73048228273"><a name="p73048228273"></a><a name="p73048228273"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p562983305010"><a name="p562983305010"></a><a name="p562983305010"></a>VDD_IO_3P3_1P8</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p206292335500"><a name="p206292335500"></a><a name="p206292335500"></a>电源</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p1562983320503"><a name="p1562983320503"></a><a name="p1562983320503"></a>VDDIO电源输入，外接1μF电容。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p84151420175511"><a name="p84151420175511"></a><a name="p84151420175511"></a>YES</p>
</td>
</tr>
<tr id="row19304192213273"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p16304112210278"><a name="p16304112210278"></a><a name="p16304112210278"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p1368831195111"><a name="p1368831195111"></a><a name="p1368831195111"></a>VDD_BAT2</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p7688813514"><a name="p7688813514"></a><a name="p7688813514"></a>电源</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p26881113516"><a name="p26881113516"></a><a name="p26881113516"></a>VDD_XLDO电源输入端。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p1441572055516"><a name="p1441572055516"></a><a name="p1441572055516"></a>YES</p>
</td>
</tr>
<tr id="row1230422210278"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p530442252716"><a name="p530442252716"></a><a name="p530442252716"></a>8</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p544441211511"><a name="p544441211511"></a><a name="p544441211511"></a>RTC_O</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p1844417125511"><a name="p1844417125511"></a><a name="p1844417125511"></a>时钟</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p64441212145114"><a name="p64441212145114"></a><a name="p64441212145114"></a>芯片RTC（Real Time Clock）时钟信号输出。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p541512011558"><a name="p541512011558"></a><a name="p541512011558"></a>YES</p>
</td>
</tr>
<tr id="row430422282710"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p93041122102713"><a name="p93041122102713"></a><a name="p93041122102713"></a>9</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p444471213514"><a name="p444471213514"></a><a name="p444471213514"></a>RTC_I</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p544416128512"><a name="p544416128512"></a><a name="p544416128512"></a>时钟</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p1425293213387"><a name="p1425293213387"></a><a name="p1425293213387"></a>芯片RTC时钟信号输入。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p24156203553"><a name="p24156203553"></a><a name="p24156203553"></a>YES</p>
</td>
</tr>
<tr id="row6304022132717"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p2304122282714"><a name="p2304122282714"></a><a name="p2304122282714"></a>10</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p74430277511"><a name="p74430277511"></a><a name="p74430277511"></a>GPIO4</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p444313276515"><a name="p444313276515"></a><a name="p444313276515"></a>数字IO</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p1130519227279"><a name="p1130519227279"></a><a name="p1130519227279"></a>通用GPIO，默认为debug UART，电平3.3V/1.8V，带施密特触发器。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p163091649504"><a name="p163091649504"></a><a name="p163091649504"></a>-</p>
</td>
</tr>
<tr id="row4304142218275"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p230419223273"><a name="p230419223273"></a><a name="p230419223273"></a>11</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p1544362713516"><a name="p1544362713516"></a><a name="p1544362713516"></a>GPIO5</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p2044372716516"><a name="p2044372716516"></a><a name="p2044372716516"></a>数字IO</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p183179546446"><a name="p183179546446"></a><a name="p183179546446"></a>通用GPIO，默认为外置LNA（Low Noise Amplifier）使能控制，电平3.3V/1.8V，带施密特触发器。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p19416142019559"><a name="p19416142019559"></a><a name="p19416142019559"></a>YES</p>
</td>
</tr>
<tr id="row1130452232714"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p183041622142714"><a name="p183041622142714"></a><a name="p183041622142714"></a>12</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p19709135019511"><a name="p19709135019511"></a><a name="p19709135019511"></a>GPIO1</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p5709850105113"><a name="p5709850105113"></a><a name="p5709850105113"></a>数字IO</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p63054227278"><a name="p63054227278"></a><a name="p63054227278"></a>通用GPIO，默认为UART RXD，电平3.3V/1.8V，带施密特触发器。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p14416132017551"><a name="p14416132017551"></a><a name="p14416132017551"></a>YES</p>
</td>
</tr>
<tr id="row930582292715"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p1730522210277"><a name="p1730522210277"></a><a name="p1730522210277"></a>13</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p670915065119"><a name="p670915065119"></a><a name="p670915065119"></a>GPIO0</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p1670985016514"><a name="p1670985016514"></a><a name="p1670985016514"></a>数字IO</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p330520228277"><a name="p330520228277"></a><a name="p330520228277"></a>通用GPIO，默认为UART TXD，电平3.3V/1.8V，带施密特触发器。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p11416920115515"><a name="p11416920115515"></a><a name="p11416920115515"></a>YES</p>
</td>
</tr>
<tr id="row63051022122711"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p630512222712"><a name="p630512222712"></a><a name="p630512222712"></a>14</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p1744355411515"><a name="p1744355411515"></a><a name="p1744355411515"></a>GPIO2</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p1344335419510"><a name="p1344335419510"></a><a name="p1344335419510"></a>数字IO，内部下拉</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p44436547510"><a name="p44436547510"></a><a name="p44436547510"></a>通用GPIO，输入，电平3.3V/1.8V，带施密特触发器。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p1416192035516"><a name="p1416192035516"></a><a name="p1416192035516"></a>YES</p>
</td>
</tr>
<tr id="row030518226275"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p103051122182718"><a name="p103051122182718"></a><a name="p103051122182718"></a>15</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p16305192218279"><a name="p16305192218279"></a><a name="p16305192218279"></a>GPIO3</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p6305122142712"><a name="p6305122142712"></a><a name="p6305122142712"></a>数字IO，内部下拉</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p173053221276"><a name="p173053221276"></a><a name="p173053221276"></a>通用GPIO，双向，电平3.3V/1.8V，带施密特触发器。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p6416120175511"><a name="p6416120175511"></a><a name="p6416120175511"></a>YES</p>
</td>
</tr>
<tr id="row23051722112712"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p8305122218279"><a name="p8305122218279"></a><a name="p8305122218279"></a>16</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p79061915195220"><a name="p79061915195220"></a><a name="p79061915195220"></a>VDD_BAT1</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p19906111545218"><a name="p19906111545218"></a><a name="p19906111545218"></a>电源</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p1890651516523"><a name="p1890651516523"></a><a name="p1890651516523"></a>芯片内部DCDC输入。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p1941618201553"><a name="p1941618201553"></a><a name="p1941618201553"></a>YES</p>
</td>
</tr>
<tr id="row230582212278"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p230513228275"><a name="p230513228275"></a><a name="p230513228275"></a>17</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p387515258526"><a name="p387515258526"></a><a name="p387515258526"></a>BUCK_LX</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p17875152518524"><a name="p17875152518524"></a><a name="p17875152518524"></a>电源</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p387510250524"><a name="p387510250524"></a><a name="p387510250524"></a>BUCK输出。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p98701756507"><a name="p98701756507"></a><a name="p98701756507"></a>YES</p>
</td>
</tr>
<tr id="row53051022182711"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p8305202211278"><a name="p8305202211278"></a><a name="p8305202211278"></a>18</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p2019212437524"><a name="p2019212437524"></a><a name="p2019212437524"></a>VDD_1P05</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p8192164315520"><a name="p8192164315520"></a><a name="p8192164315520"></a>电源</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p10192194335211"><a name="p10192194335211"></a><a name="p10192194335211"></a>芯片内部主电源输入。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p5416102015515"><a name="p5416102015515"></a><a name="p5416102015515"></a>YES</p>
</td>
</tr>
<tr id="row12305522132710"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p1630515228272"><a name="p1630515228272"></a><a name="p1630515228272"></a>19</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p4329105119523"><a name="p4329105119523"></a><a name="p4329105119523"></a>VDD_CLDO</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p16329205145217"><a name="p16329205145217"></a><a name="p16329205145217"></a>电源</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p17329651185210"><a name="p17329651185210"></a><a name="p17329651185210"></a>Core LDO输出。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p10416142011551"><a name="p10416142011551"></a><a name="p10416142011551"></a>YES</p>
</td>
</tr>
<tr id="row153051922162714"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p430562212272"><a name="p430562212272"></a><a name="p430562212272"></a>20</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p49546265312"><a name="p49546265312"></a><a name="p49546265312"></a>GPIO7</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p1995415225320"><a name="p1995415225320"></a><a name="p1995415225320"></a>数字IO，内部下拉</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p15954421537"><a name="p15954421537"></a><a name="p15954421537"></a>通用GPIO，默认为GNSS唤醒HOST信号，上电启动复用管脚，电平3.3V/1.8V，带施密特触发器。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p6416120115512"><a name="p6416120115512"></a><a name="p6416120115512"></a>YES</p>
</td>
</tr>
<tr id="row83051222132711"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p1305172213274"><a name="p1305172213274"></a><a name="p1305172213274"></a>21</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p13851771532"><a name="p13851771532"></a><a name="p13851771532"></a>GPIO9</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p885120765312"><a name="p885120765312"></a><a name="p885120765312"></a>数字IO，内部上拉</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p730615228272"><a name="p730615228272"></a><a name="p730615228272"></a>通用GPIO，默认为HOST唤醒GNSS信号，上电启动复用管脚，电平3.3V/1.8V，带施密特触发器。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p18416112025520"><a name="p18416112025520"></a><a name="p18416112025520"></a>YES</p>
</td>
</tr>
<tr id="row330582282717"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p030516228276"><a name="p030516228276"></a><a name="p030516228276"></a>22</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p3220132964210"><a name="p3220132964210"></a><a name="p3220132964210"></a>PWREN</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p82201229154210"><a name="p82201229154210"></a><a name="p82201229154210"></a>模拟IO</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p16220182934212"><a name="p16220182934212"></a><a name="p16220182934212"></a>芯片上电使能信号，高电平有效。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p9416720105516"><a name="p9416720105516"></a><a name="p9416720105516"></a>YES</p>
</td>
</tr>
<tr id="row1630516226274"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p183053222275"><a name="p183053222275"></a><a name="p183053222275"></a>23</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p13220112924211"><a name="p13220112924211"></a><a name="p13220112924211"></a>GPIO16</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p1220192904217"><a name="p1220192904217"></a><a name="p1220192904217"></a>数字IO，内部下拉</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p15220102964214"><a name="p15220102964214"></a><a name="p15220102964214"></a>通用GPIO，默认为UART CTS，上电启动复用管脚，电平3.3V/1.8V，带施密特触发器。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p16416202095511"><a name="p16416202095511"></a><a name="p16416202095511"></a>YES</p>
</td>
</tr>
<tr id="row1330622232717"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p1730652215279"><a name="p1730652215279"></a><a name="p1730652215279"></a>24</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p1322017295428"><a name="p1322017295428"></a><a name="p1322017295428"></a>GPIO13</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p172204297428"><a name="p172204297428"></a><a name="p172204297428"></a>数字IO</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p20220112914213"><a name="p20220112914213"></a><a name="p20220112914213"></a>通用GPIO，双向，默认为PPS_OUT，电平3.3V/1.8V，带施密特触发器。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p34161520185516"><a name="p34161520185516"></a><a name="p34161520185516"></a>-</p>
</td>
</tr>
<tr id="row63064228275"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p153062022142717"><a name="p153062022142717"></a><a name="p153062022142717"></a>25</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p7220729134213"><a name="p7220729134213"></a><a name="p7220729134213"></a>GPIO14</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p122014297420"><a name="p122014297420"></a><a name="p122014297420"></a>数字IO</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p20220172918422"><a name="p20220172918422"></a><a name="p20220172918422"></a>通用GPIO，双向，默认为UART RTS，电平3.3V/1.8V，带施密特触发器。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p8416102025512"><a name="p8416102025512"></a><a name="p8416102025512"></a>YES</p>
</td>
</tr>
<tr id="row130682292715"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p19306422202714"><a name="p19306422202714"></a><a name="p19306422202714"></a>26</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p1422022915422"><a name="p1422022915422"></a><a name="p1422022915422"></a>VDD_ANA</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p18220229144212"><a name="p18220229144212"></a><a name="p18220229144212"></a>电源</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p132201729124212"><a name="p132201729124212"></a><a name="p132201729124212"></a>模拟LDO输出。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p441618208555"><a name="p441618208555"></a><a name="p441618208555"></a>YES</p>
</td>
</tr>
<tr id="row1130618227274"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p16306102213279"><a name="p16306102213279"></a><a name="p16306102213279"></a>27</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p16221172964214"><a name="p16221172964214"></a><a name="p16221172964214"></a>VDD_RF</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p3221729164219"><a name="p3221729164219"></a><a name="p3221729164219"></a>电源</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p18221182917426"><a name="p18221182917426"></a><a name="p18221182917426"></a>RF（Radio Frequency） LDO输出。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p64164201557"><a name="p64164201557"></a><a name="p64164201557"></a>YES</p>
</td>
</tr>
<tr id="row103061722172710"><td class="cellrowborder" valign="top" width="8.48915108489151%" headers="mcps1.2.6.1.1 "><p id="p530662252710"><a name="p530662252710"></a><a name="p530662252710"></a>28</p>
</td>
<td class="cellrowborder" valign="top" width="16.48835116488351%" headers="mcps1.2.6.1.2 "><p id="p1622162917421"><a name="p1622162917421"></a><a name="p1622162917421"></a>RF_IN</p>
</td>
<td class="cellrowborder" valign="top" width="11.798820117988202%" headers="mcps1.2.6.1.3 "><p id="p14221132913424"><a name="p14221132913424"></a><a name="p14221132913424"></a>射频</p>
</td>
<td class="cellrowborder" valign="top" width="52.274772522747725%" headers="mcps1.2.6.1.4 "><p id="p1522172914420"><a name="p1522172914420"></a><a name="p1522172914420"></a>RF输入口。</p>
</td>
<td class="cellrowborder" valign="top" width="10.948905109489052%" headers="mcps1.2.6.1.5 "><p id="p1767515317111"><a name="p1767515317111"></a><a name="p1767515317111"></a>YES</p>
</td>
</tr>
</tbody>
</table>

## 产测方案架构<a name="ZH-CN_TOPIC_0000002313476305"></a>

-   **[测试架构](#ZH-CN_TOPIC_0000002278979786)**  

-   **[环境搭建](#ZH-CN_TOPIC_0000002278876612)**  

### 测试架构<a name="ZH-CN_TOPIC_0000002278979786"></a>

GNSS产测方案需要电脑、DUT（Device Under Test 待测设备）与测试仪表，DUT通过串口连接电脑，在串口执行产测指令，接收产测结果。

产测方案架构如[图1](#fig122615195219)所示，其中相关组件介绍如下：

-   R&S CMW500/270：单音测试设备，能够发送连续载波信号，支持1.5GHz频率。
-   DUT：待测产品，使用射频线缆与单音测试设备连接。

**图 1**  GNSS产测方案架构图<a name="fig122615195219"></a>  
![](figures/GNSS产测方案架构图.png "GNSS产测方案架构图")

### 环境搭建<a name="ZH-CN_TOPIC_0000002278876612"></a>

-   **[搭建PC端环境](#ZH-CN_TOPIC_0000002313509321)**  

-   **[搭建DUT端环境](#ZH-CN_TOPIC_0000002313476309)**  

#### 搭建PC端环境<a name="ZH-CN_TOPIC_0000002313509321"></a>

1.  安装单音设备CMW500/270的商用上位机软件，请联系供应商获取。
2.  安装DUT上位机软件，通常可直接使用串口工具。

#### 搭建DUT端环境<a name="ZH-CN_TOPIC_0000002313476309"></a>

1.  使用RF线缆连接DUT待测设备和单音测试设备。
2.  使用串口连接PC和DUT待测设备。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>需要在产品断电的情况下，安装RF线缆扣线。

## 产测操作流程<a name="ZH-CN_TOPIC_0000002278979790"></a>

-   **[整体流程](#ZH-CN_TOPIC_0000002278876616)**  

-   **[结果判别](#ZH-CN_TOPIC_0000002313509325)**  

### 整体流程<a name="ZH-CN_TOPIC_0000002278876616"></a>

1.  搭建PC端环境和DUT端环境。
2.  配置单音设备，播放**1575.62**MHz单音信号（**相对于1575.42MHz偏移200kHz**\)，补偿单音设备至DUT设备射频口的线损，配置信号功率，使射频口信号为-130dBm。
3.  通过串口工具，执行GNSS INIT的指令：AT^GNSSINIT=1,1,0,0

    ![](figures/gnssinit.jpg)

4.  通过串口工具，发送GNSS产测示例对应的指令：AT^GNSSSAMPLE=5

    ![](figures/factory.jpg)

5.  通过串口工具，发送GNSS启动指令：AT^GNSSSTART
6.  发送GNSS启动指令后，芯片自动执行GNSS产测过程，此过程约耗时5s，执行完成，芯片上报产测结果。

    ![](figures/factory_start.jpg)

7.  通过串口工具，发送GNSS停止指令：AT^GNSSSTOP

    ![](figures/stop.jpg)

8.  通过串口工具，发送GNSS去初始化指令：AT^GNSSDEINIT

    ![](figures/deinit.jpg)

### 结果判别<a name="ZH-CN_TOPIC_0000002313509325"></a>

产测结果判别依据如[表1](#table1312118482515)所示。

**表 1**  GNSS产测结果判别表

<a name="table1312118482515"></a>
<table><thead align="left"><tr id="row1712194895120"><th class="cellrowborder" valign="top" width="15.551555155515551%" id="mcps1.2.4.1.1"><p id="p312114819514"><a name="p312114819514"></a><a name="p312114819514"></a>测试项</p>
</th>
<th class="cellrowborder" valign="top" width="18.801880188018803%" id="mcps1.2.4.1.2"><p id="p5121948205113"><a name="p5121948205113"></a><a name="p5121948205113"></a>说明</p>
</th>
<th class="cellrowborder" valign="top" width="65.64656465646564%" id="mcps1.2.4.1.3"><p id="p41211248175118"><a name="p41211248175118"></a><a name="p41211248175118"></a>预期结果</p>
</th>
</tr>
</thead>
<tbody><tr id="row91219483514"><td class="cellrowborder" valign="top" width="15.551555155515551%" headers="mcps1.2.4.1.1 "><p id="p1212184865118"><a name="p1212184865118"></a><a name="p1212184865118"></a>satellite cn0</p>
</td>
<td class="cellrowborder" valign="top" width="18.801880188018803%" headers="mcps1.2.4.1.2 "><p id="p2121194895111"><a name="p2121194895111"></a><a name="p2121194895111"></a>CN0测试</p>
</td>
<td class="cellrowborder" valign="top" width="65.64656465646564%" headers="mcps1.2.4.1.3 "><p id="p912124855112"><a name="p912124855112"></a><a name="p912124855112"></a>板级射频口接收信号功率-130dBm时，预期CN0为40dBHz，且波动不超过&plusmn;2dB。</p>
</td>
</tr>
<tr id="row41219483518"><td class="cellrowborder" valign="top" width="15.551555155515551%" headers="mcps1.2.4.1.1 "><p id="p11211348135115"><a name="p11211348135115"></a><a name="p11211348135115"></a>frequency bias</p>
</td>
<td class="cellrowborder" valign="top" width="18.801880188018803%" headers="mcps1.2.4.1.2 "><p id="p1012144885120"><a name="p1012144885120"></a><a name="p1012144885120"></a>频偏测试</p>
</td>
<td class="cellrowborder" valign="top" width="65.64656465646564%" headers="mcps1.2.4.1.3 "><p id="p13121194820512"><a name="p13121194820512"></a><a name="p13121194820512"></a>max值不超过3000ppb，对应1575.42MHz中心频点，约为4726Hz。</p>
</td>
</tr>
<tr id="row6121184845112"><td class="cellrowborder" valign="top" width="15.551555155515551%" headers="mcps1.2.4.1.1 "><p id="p9121104805110"><a name="p9121104805110"></a><a name="p9121104805110"></a>frequency drift</p>
</td>
<td class="cellrowborder" valign="top" width="18.801880188018803%" headers="mcps1.2.4.1.2 "><p id="p11121134855112"><a name="p11121134855112"></a><a name="p11121134855112"></a>频漂测试</p>
</td>
<td class="cellrowborder" valign="top" width="65.64656465646564%" headers="mcps1.2.4.1.3 "><p id="p112115487516"><a name="p112115487516"></a><a name="p112115487516"></a>max值不超过5ppb/s，对应1575.42MHz中心频点，约为8Hz/s。</p>
</td>
</tr>
</tbody>
</table>

>![](public_sys-resources/icon-notice.gif) **须知：** 
>产测示例代码中预留了接口，供用户自行设置判别门限，此功能仅用于方便用户调试产测环境，不建议客户在正式测试中修改门限，以免影响产测效果，造成产线对问题单板拦截失败。

# GNSS常见问题<a name="ZH-CN_TOPIC_0000002313509313"></a>

-   **[消息上报相关问题](#ZH-CN_TOPIC_0000002313476325)**  

-   **[驱动适配相关问题](#ZH-CN_TOPIC_0000002278876632)**  

-   **[定位结果相关问题](#ZH-CN_TOPIC_0000002313476333)**  

-   **[测试环境相关问题](#ZH-CN_TOPIC_0000002313476337)**  

-   **[功耗测试相关问题](#ZH-CN_TOPIC_0000002278979822)**  

-   **[产线测试相关问题](#ZH-CN_TOPIC_0000002278979826)**  

## 消息上报相关问题<a name="ZH-CN_TOPIC_0000002313476325"></a>

-   **[GNSS上报数据缺失](#ZH-CN_TOPIC_0000002278979806)**  

### GNSS上报数据缺失<a name="ZH-CN_TOPIC_0000002278979806"></a>

**问题描述<a name="section206386301236"></a>**

GNSS上报的定位数据缺失，比如NMEA语句整句缺失，或者语句不完整。

**解决方案<a name="section5653141620416"></a>**

1.  通过预留的GNSS UART TX测试点，通过串口工具，监测数据是否完整，界定是GNSS问题还是主控集成问题。
2.  排查驱动中的流控使能状态是否与板级流控状态一致，建议板极对接流控管脚，同时驱动代码中使能流控功能。
3.  排查是否有主控UART RX中断被打断的情况，这种情况下，可能会造成传输数据丢失。

## 驱动适配相关问题<a name="ZH-CN_TOPIC_0000002278876632"></a>

-   **[驱动加载固件需要多长时间](#ZH-CN_TOPIC_0000002313509337)**  

### 驱动加载固件需要多长时间<a name="ZH-CN_TOPIC_0000002313509337"></a>

**问题描述<a name="section13582739122017"></a>**

GNSS驱动加载固件，一般需要多长时间？

**解决方案<a name="section4451124312200"></a>**

1.  加载固件时间与固件大小和波特率相关，以16KB固件和115200bit/s计算，加载固件约耗时1.4s。
2.  加载完成后，需要0.6s的延时，用于GNSS初始化，完成线程加载。
3.  综上，整个GNSS固件加载时间约需2s，这部分逻辑封装在驱动代码中，可通过调用tiot\_service\_open完成。

## 定位结果相关问题<a name="ZH-CN_TOPIC_0000002313476333"></a>

-   **[如何判断定位是否成功](#ZH-CN_TOPIC_0000002278979814)**  

-   **[如何判断定位失败的原因](#ZH-CN_TOPIC_0000002278876640)**  

-   **[如何判断星历是否注入成功](#ZH-CN_TOPIC_0000002313509345)**  

### 如何判断定位是否成功<a name="ZH-CN_TOPIC_0000002278979814"></a>

**问题描述<a name="section234618519314"></a>**

如何判断GNSS是否定位成功？

**解决方案<a name="section173411014311"></a>**

通过GNSS上报的标准NMEA消息，可以判断定位是否成功。

1.  GGA语句，定位质量（第7字段）显示为1时，表示定位成功，显示为0时，表示定位失败。
2.  RMC语句，位置有效标志（第3字段）显示为A时，表示定位成功，显示为V时，表示定位失败。

### 如何判断定位失败的原因<a name="ZH-CN_TOPIC_0000002278876640"></a>

**问题描述<a name="section11629185753512"></a>**

GNSS定位失败时，如何判断导致定位失败的原因？

**解决方案<a name="section66846314360"></a>**

通过GNSS上报的自定义NMEA消息，可以快速判断定位失败的原因。

1.  PNTH001语句，定位错误码（第6字段）。

    1表示卫星跟踪数量不足，不满足定位条件；

    2表示表示有效星历数量不足，不满足定位条件；

    3表示定位精度不足，定位结果被拦截。

2.  PNTH001语句，定位质量评估因子（第5字段）显示了当前评估出的定位精度，定位过程中，有效范围为3～200，值越小定位精度越好。
3.  PNTH001语句，实际跟踪卫星数量（第7字段）显示了当前实际跟踪的卫星数量。
4.  PNTH001语句，有效星历数量（第8字段）显示了当前有效的星历数量。

### 如何判断星历是否注入成功<a name="ZH-CN_TOPIC_0000002313509345"></a>

**问题描述<a name="section2235124810375"></a>**

使用XGNSS服务时，如何判断辅助星历是否成功注入？

**解决方案<a name="section1774410587376"></a>**

通过GNSS上报的自定义NMEA消息，可以快速判断定位失败的原因。

PNTH001语句，有效星历数量（第8字段）显示了当前有效的星历数量，如果此字段在GNSS一开始启动时就有值，且值与预期注入的卫星星历数量相符，即表示注入星历成功，典型情况下，全星座星历注入时，此值约为100左右。

## 测试环境相关问题<a name="ZH-CN_TOPIC_0000002313476337"></a>

-   **[如何确认当前信号环境](#ZH-CN_TOPIC_0000002278979818)**  

-   **[如何判断是否可能存在干扰](#ZH-CN_TOPIC_0000002278876644)**  

-   **[如何改善定位慢的问题](#ZH-CN_TOPIC_0000002313509349)**  

-   **[如何改善搜星慢的问题](#ZH-CN_TOPIC_0000002313476341)**  

### 如何确认当前信号环境<a name="ZH-CN_TOPIC_0000002278979818"></a>

**问题描述<a name="section1946937113812"></a>**

在GNSS外场测试时，如何判断当前GNSS信号环境好坏？

**解决方案<a name="section043839163911"></a>**

使用GNSS射频信号指标正常的开发板或者设备，通过GNSS上报的标准NMEA消息，可以判断定位是否成功，NMEA的消息协议请参见《HiDiTingV100 GNSS接口协议 开发指南》：

1.  GSV语句会输出当前所有跟踪的卫星情况，包括卫星大体位置和信号强度。
2.  GSV语句中每颗卫星的高度角和方位角，描述了该颗卫星的空间位置，通过分析多颗卫星的空间分布，可以看出当前是否有明显遮挡情况：如果卫星多数集中在天空一侧，说明可能存在半边遮挡。如果卫星大部分高角度都很低，说明可能在GNSS上方存在遮挡。
3.  GSV语句中每颗卫星的载噪比字段，描述了该颗卫星的信号强度，开阔天空理想信号下，此值可达40+，如果此值低于30，可认为该颗星信号较弱。如果大部分卫星的载噪比都在30以下，说明该环境下整体GNSS信号较差，不利于定位。

### 如何判断是否可能存在干扰<a name="ZH-CN_TOPIC_0000002278876644"></a>

**问题描述<a name="section172115589395"></a>**

在GNSS外场测试时，如何判断是否可能存在GNSS信号干扰情况？

**解决方案<a name="section810713374019"></a>**

通过GNSS上报的自定义NMEA消息，可以快速判断定位失败的原因。

PNTH000语句，VGA档位（第2字段）描述了当前的信号增益档位，有效范围为2～13，正常信号时，其档位值为7～9；如果档位值显示为2，说明可能存在强干扰，导致增益饱和；如果档位值为13，说明信号特别弱，可能射频链路增益不足。

### 如何改善定位慢的问题<a name="ZH-CN_TOPIC_0000002313509349"></a>

**问题描述<a name="section19885105918283"></a>**

外场测试中，可能会遇到首次定位时间很长的问题，如何改善这种情况？

**解决方案<a name="section16212432915"></a>**

1.  确认天线正常，可将GNSS置于开阔天空下，观察跟踪到的卫星载噪比是否能达到35以上，或者在手机上安装GNSS测试软件，如Gps Test，对比手机接收到的载噪比，判断天线是否正常。
2.  确认当前环境有GNSS信号，可通过手机导航定位小程序在相同环境下观察信号强度是否正常。
3.  澄清天线和信号问题后，在没有注入星历的情况下，开阔天空首次定位时间约30s，信号环境较差时，定位时间会变长。
4.  用户可以通过注入XGNSS辅助星历，提升首次定位时间。
5.  GNSS完成定位后，发送STOP指令让GNSS停止工作，不给GNSS下电，在星历有效期内，再次启动GNSS，此时会执行热启动，会将定位时间缩短至秒级。

### 如何改善搜星慢的问题<a name="ZH-CN_TOPIC_0000002313476341"></a>

**问题描述<a name="section9935331187"></a>**

现网测试时，从NMEA语句看，上星比较慢，卫星数长时间处于较低水平，如何改善？

**解决方案<a name="section144958371784"></a>**

1.  首先确认信号环境正常，确保有正常的GNSS信号。
2.  测试时天线不要靠近地面或者金属底面。
3.  获取并注入XGNSS辅助信息，提高搜星效率。

## 功耗测试相关问题<a name="ZH-CN_TOPIC_0000002278979822"></a>

-   **[为何功耗实测值与标称值存在偏差](#ZH-CN_TOPIC_0000002278876648)**  

-   **[为何在GNSS启动阶段功耗偏高](#ZH-CN_TOPIC_0000002313509353)**  

-   **[为何上报数据量较大时功耗偏高](#ZH-CN_TOPIC_0000002313476345)**  

-   **[GNSS业务过程中，各阶段的功耗通常是多少](#ZH-CN_TOPIC_0000002302681046)**  

### 为何功耗实测值与标称值存在偏差<a name="ZH-CN_TOPIC_0000002278876648"></a>

**问题描述<a name="section1468435814211"></a>**

使用开发板实测GNSS功耗值，为何比GNSS手册中的标称值偏大？

**解决方案<a name="section315111382219"></a>**

1.  GNSS手册中标称的功耗值，是GNSS芯片的功耗，并不包含板级上的器件功耗，如TCXO，LNA等。
2.  GNSS手册中标称的功耗值，是在-130dBm信号环境下测试出的标准值，用户实际测试信号环境较差时，会导致实测功耗偏高。

### 为何在GNSS启动阶段功耗偏高<a name="ZH-CN_TOPIC_0000002313509353"></a>

**问题描述<a name="section18671826172510"></a>**

在GNSS刚开始启动时，功耗偏高，GNSS定位后，功耗变低，为何有这种现象？

**解决方案<a name="section1943943118251"></a>**

1.  GNSS启动时，捕获和跟踪并行，此时测出的功耗包括捕获功耗和跟踪功耗，功耗较高。
2.  GNSS定位后，通常只有跟踪在工作，此时测出的功耗仅包括跟踪功耗，功耗较低。
3.  当GNSS信号环境较差时，会触发启动捕获任务，此时测出的功耗包括捕获功耗和跟踪功耗，功耗较高。

### 为何上报数据量较大时功耗偏高<a name="ZH-CN_TOPIC_0000002313476345"></a>

**问题描述<a name="section1384753252912"></a>**

GNSS工作在高频定位模式时，如10Hz/5Hz，或者配置上报的数据量较大时，如同时上报NMEA和维测日志，为何功耗会偏高？

**解决方案<a name="section168016374293"></a>**

1.  GNSS工作在高频定位模式时，定位模块工作时间变长，导致功耗偏高。
2.  GNSS上报数据量较大时，会使得UART传输时间变长，间接导致GNSS工作时间变长，导致功耗偏高，此时，可通过提高GNSS UART波特率，来缩短UART传输时间，降低功耗。

### GNSS业务过程中，各阶段的功耗通常是多少<a name="ZH-CN_TOPIC_0000002302681046"></a>

**问题描述<a name="section1384753252912"></a>**

GNSS在初始化、启动、停止、去初始化阶段，功耗分别是什么样的？

**解决方案<a name="section168016374293"></a>**

1.  调用uapi\_gnss\_init\(\)后，GNSS完成上电，处于等待固件加载状态，此时纯GNSS芯片约有1.5mA@3.3V的功耗。
2.  调用uapi\_gnss\_open\(\)后，GNSS完成固件加载，处于就绪状态，此时纯GNSS芯片约有2.7mA@3.3V的功耗，需尽快下发配置命令或者启动GNSS，减少此阶段的停留时间。
3.  调用uapi\_gnss\_start\(\)后，GNSS处于工作状态，具体功耗与工作环境相关，外场步行纯GNSS芯片约为7mA@3.3V。
4.  调用uapi\_gnss\_stop\(\)后，GNSS处于停止状态，前35秒，纯芯片功耗约为0.5mA@3.3V，此后60μA@3.3V。
5.  调用uapi\_gnss\_close\(\)后，GNSS处于处于等待固件加载状态，此时纯GNSS芯片约有18μA@3.3V的功耗。
6.  调用uapi\_gnss\_deinit\(\)后，GNSS处于下电状态，此时纯GNSS芯片约有18μA@3.3V的漏电功耗。

## 产线测试相关问题<a name="ZH-CN_TOPIC_0000002278979826"></a>

-   **[无产测结果怎么办](#ZH-CN_TOPIC_0000002280641220)**  

-   **[搜星失败怎么办](#ZH-CN_TOPIC_0000002315210701)**  

-   **[CN0测试失败怎么办](#ZH-CN_TOPIC_0000002280537992)**  

-   **[频偏测试失败怎么办](#ZH-CN_TOPIC_0000002315297641)**  

-   **[频漂测试失败怎么办](#ZH-CN_TOPIC_0000002280641224)**  

### 无产测结果怎么办<a name="ZH-CN_TOPIC_0000002280641220"></a>

**问题描述<a name="section1687694311412"></a>**

执行产测流程后，未返回产测结果。

**解决方案<a name="section13244849154113"></a>**

1.  确认产测指令正确。
2.  确认先发送产测指令，后发送GNSS启动指令。
3.  GNSS启动指令发送后，约5s后返回产测结果。

### 搜星失败怎么办<a name="ZH-CN_TOPIC_0000002315210701"></a>

**问题描述<a name="section10757194314315"></a>**

产测结果显示搜星失败。

**解决方案<a name="section11484154615432"></a>**

1.  确认配置的测试类型为CN0测试。
2.  确认配置的信号源为单音信号源。
3.  确认在执行产测过程前，单音信号已正常发送。

### CN0测试失败怎么办<a name="ZH-CN_TOPIC_0000002280537992"></a>

**问题描述<a name="section112281612441"></a>**

产测结果显示CN0测试失败。

**解决方案<a name="section1865202813310"></a>**

1.  确认射频线缆连接正常。
2.  确认补偿线损。
3.  确认单音设备配置输出功率正常。
4.  对比正常单板，分析是否为单板射频链路问题。

### 频偏测试失败怎么办<a name="ZH-CN_TOPIC_0000002315297641"></a>

**问题描述<a name="section16676752114417"></a>**

产测结果显示频偏测试失败。

**解决方案<a name="section142991237163313"></a>**

1.  大概率是TCXO器件问题，可更换TCXO器件复测。
2.  外灌26MHz时钟信号，澄清器件问题还是单板问题。

### 频漂测试失败怎么办<a name="ZH-CN_TOPIC_0000002280641224"></a>

**问题描述<a name="section15839113611451"></a>**

产测结果显示频漂测试失败。

**解决方案<a name="section10682184619333"></a>**

1.  排查测试环境，确保TCXO器件不受温度、风等影响。
2.  更换TCXO器件。

