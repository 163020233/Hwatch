# 前言<a name="ZH-CN_TOPIC_0000002499501689"></a>

**概述<a name="zh-cn_topic_0000002470231369_section4537382116410"></a>**

本文档从网络安全的角度，重点分析基于本交付包开发的产品在使用过程中，可能面临的与本交付包中SDK软件包相关的网络安全的威胁，同时，针对性地给出相应的解决方案。


**读者对象<a name="zh-cn_topic_0000002470231369_section4378592816410"></a>**

本文档主要适用于以下工程师：

-   技术支持工程师
-   软件开发工程师

**符号约定<a name="zh-cn_topic_0000002470231369_section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="zh-cn_topic_0000002470231369_table2622507016410"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002470231369_row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002470231369_p6450074116410"><a name="zh-cn_topic_0000002470231369_p6450074116410"></a><a name="zh-cn_topic_0000002470231369_p6450074116410"></a><strong id="zh-cn_topic_0000002470231369_b2136615816410"><a name="zh-cn_topic_0000002470231369_b2136615816410"></a><a name="zh-cn_topic_0000002470231369_b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002470231369_p5435366816410"><a name="zh-cn_topic_0000002470231369_p5435366816410"></a><a name="zh-cn_topic_0000002470231369_p5435366816410"></a><strong id="zh-cn_topic_0000002470231369_b5941558116410"><a name="zh-cn_topic_0000002470231369_b5941558116410"></a><a name="zh-cn_topic_0000002470231369_b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002470231369_row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002470231369_p3734547016410"><a name="zh-cn_topic_0000002470231369_p3734547016410"></a><a name="zh-cn_topic_0000002470231369_p3734547016410"></a><a name="zh-cn_topic_0000002470231369_image2670064316410"></a><a name="zh-cn_topic_0000002470231369_image2670064316410"></a><span><img class="" id="zh-cn_topic_0000002470231369_image2670064316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002502250751.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002470231369_p1757432116410"><a name="zh-cn_topic_0000002470231369_p1757432116410"></a><a name="zh-cn_topic_0000002470231369_p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002470231369_row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002470231369_p1432579516410"><a name="zh-cn_topic_0000002470231369_p1432579516410"></a><a name="zh-cn_topic_0000002470231369_p1432579516410"></a><a name="zh-cn_topic_0000002470231369_image4895582316410"></a><a name="zh-cn_topic_0000002470231369_image4895582316410"></a><span><img class="" id="zh-cn_topic_0000002470231369_image4895582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002469250780.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002470231369_p959197916410"><a name="zh-cn_topic_0000002470231369_p959197916410"></a><a name="zh-cn_topic_0000002470231369_p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002470231369_row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002470231369_p1232579516410"><a name="zh-cn_topic_0000002470231369_p1232579516410"></a><a name="zh-cn_topic_0000002470231369_p1232579516410"></a><a name="zh-cn_topic_0000002470231369_image1235582316410"></a><a name="zh-cn_topic_0000002470231369_image1235582316410"></a><span><img class="" id="zh-cn_topic_0000002470231369_image1235582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002469410776.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002470231369_p123197916410"><a name="zh-cn_topic_0000002470231369_p123197916410"></a><a name="zh-cn_topic_0000002470231369_p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002470231369_row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002470231369_p2204984716410"><a name="zh-cn_topic_0000002470231369_p2204984716410"></a><a name="zh-cn_topic_0000002470231369_p2204984716410"></a><a name="zh-cn_topic_0000002470231369_image4504446716410"></a><a name="zh-cn_topic_0000002470231369_image4504446716410"></a><span><img class="" id="zh-cn_topic_0000002470231369_image4504446716410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002502130767.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002470231369_p4388861916410"><a name="zh-cn_topic_0000002470231369_p4388861916410"></a><a name="zh-cn_topic_0000002470231369_p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="zh-cn_topic_0000002470231369_p1238861916410"><a name="zh-cn_topic_0000002470231369_p1238861916410"></a><a name="zh-cn_topic_0000002470231369_p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002470231369_row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002470231369_p5555360116410"><a name="zh-cn_topic_0000002470231369_p5555360116410"></a><a name="zh-cn_topic_0000002470231369_p5555360116410"></a><a name="zh-cn_topic_0000002470231369_image799324016410"></a><a name="zh-cn_topic_0000002470231369_image799324016410"></a><span><img class="" id="zh-cn_topic_0000002470231369_image799324016410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002502250763.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002470231369_p4612588116410"><a name="zh-cn_topic_0000002470231369_p4612588116410"></a><a name="zh-cn_topic_0000002470231369_p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="zh-cn_topic_0000002470231369_p1232588116410"><a name="zh-cn_topic_0000002470231369_p1232588116410"></a><a name="zh-cn_topic_0000002470231369_p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# 产品安全解决方案<a name="ZH-CN_TOPIC_0000002499501701"></a>

-   **[安全架构](#ZH-CN_TOPIC_0000002466542376)**  

-   **[设备安全](#ZH-CN_TOPIC_0000002499501693)**  

-   **[安全启动](#ZH-CN_TOPIC_0000002499501697)**  

-   **[关键数据安全存储](#ZH-CN_TOPIC_0000002466382752)**  

-   **[驱动安全注意事项](#ZH-CN_TOPIC_0000002466542380)**  

-   **[其他使用安全注意事项](#ZH-CN_TOPIC_0000002466382744)**  

## 安全架构<a name="ZH-CN_TOPIC_0000002466542376"></a>

产品的网络安全是一个系统工程，涉及到整个产品的各个层面。

HiDiTingV100版本可能涉及的威胁如下：

-   开机启动安全：

    该部分主要涉及到系统启动过程中每一级镜像的校验机制。HiDiTingV100提供安全启动方案，包含完整的启动信任链校验，信任根是BootRom和eFuse，先通过BootRom校验根公钥，再校验二级签名，之后依次校验SSB签名、SELiteOS签名、APP签名和BT/DSP签名。

-   系统升级安全：

    系统升级方案存在的安全威胁是文件本身的合法性和完整性。SSB和Firmware都可升级，采用数字签名方式对升级文件进行校验，SSB和Firmware都有版本防回滚机制，其版本号分别与eFuse中的版本进行认证，认证不通过将无法启动。

-   JTAG（Joint Test Action Group）调试安全：

    JTAG调测功能在芯片出厂时默认打开，建议用户在出厂时通过eFuse永久关闭。

## 设备安全<a name="ZH-CN_TOPIC_0000002499501693"></a>

基于安全性考虑，建议用户在最终产品中执行以下措施：

-   启用安全启动特性。
-   修改lifecycle为End User模式。
-   SeLiteOS加密。请参见《安全启动 使用指南》的“使能安全启动特性”章节。
-   应用安装时使能应用完整性校验，避免恶意应用安装。

## 安全启动<a name="ZH-CN_TOPIC_0000002499501697"></a>

HiDiTingV100支持安全启动。

-   **[安全启动开关](#ZH-CN_TOPIC_0000002466542372)**  

### 安全启动开关<a name="ZH-CN_TOPIC_0000002466542372"></a>

安全启动开关请参见《安全启动 使用指南》中“安全启动配置”相关说明。

## 关键数据安全存储<a name="ZH-CN_TOPIC_0000002466382752"></a>

用户记录账号、密码等保密信息时，需要保证这些数据的存储安全，所以要求这些数据在存储之前进行加密保护。

使用密钥派生时，派生出的硬件密钥软件不可见，无法直接访问。

## 驱动安全注意事项<a name="ZH-CN_TOPIC_0000002466542380"></a>

-   **[Cipher驱动](#ZH-CN_TOPIC_0000002466382748)**  

-   **[Cipher JSI](#ZH-CN_TOPIC_0000002517316906)**  

### Cipher驱动<a name="ZH-CN_TOPIC_0000002466382748"></a>

Cipher驱动实现了标准的对称加密AES、非对称加密RSA、ECC（Elliptic Curve Cryptography，椭圆曲线密码学）、SM2（商密2椭圆曲线公钥密码算法）签名验签、密钥协商、加解密、大数运算等非对称算法，随机数算法TRNG（True Random Number Generator，真随机数发生器）、摘要算法SHA256、密钥派生算法等，未使用任何私有算法。

使用时请注意：Cipher密钥的长度越长，安全等级越高，因此建议使用AES 128bit及以上的密钥、RSA 2048bit及以上的密钥，详细内容请参见《安全软件 开发指南》。

### Cipher JSI<a name="ZH-CN_TOPIC_0000002517316906"></a>

OpenHarmony huks部件下crypto\_lite模块封装第三方软件Mbedtls的C接口，提供AES、SHA256、SHA256-HMAC、Base64、MD5等算法接口给JS应用使用，其中AES ECB模式以及Base64、MD5属于弱密码算法，不建议使用在如加解密等安全场景，否则会有安全风险。

## 其他使用安全注意事项<a name="ZH-CN_TOPIC_0000002466382744"></a>

-   **[代码安全注意事项](#ZH-CN_TOPIC_0000002466382756)**  

-   **[可维可测注意事项](#ZH-CN_TOPIC_0000002466542368)**  

-   **[OpenHarmony认证镜像烧写注意事项](#ZH-CN_TOPIC_0000002499581733)**  

-   **[应用及表盘市场安全注意事项](#ZH-CN_TOPIC_0000002541294689)**  

-   **[安全调试](#ZH-CN_TOPIC_0000002541254709)**  

-   **[安全启动](#ZH-CN_TOPIC_0000002541254713)**  

### 代码安全注意事项<a name="ZH-CN_TOPIC_0000002466382756"></a>

代码错误引发的网络安全问题，一般都是源于最基本的代码规范问题，例如：指针越界、数组越界、入参不检查等错误。建议通过如下方法检查：

-   使用业界通用的代码健康扫描工具进行全覆盖扫描。
-   使用模糊测试工具，对所有API接口（包括设备驱动接口）进行全范围模糊测试。
-   使用业界通用的漏洞扫描工具对所使用的开源软件进行扫描。

### 可维可测注意事项<a name="ZH-CN_TOPIC_0000002466542368"></a>

-   可维可测方案仅在调试时打开，Release版本建议用户关闭。
-   HSO调试工具目前仅用于SDK问题定位时的调测日志打印。
-   可维可测接口详细使用方法请参见《DFX软件 开发指南》。

### OpenHarmony认证镜像烧写注意事项<a name="ZH-CN_TOPIC_0000002499581733"></a>

OpenHarmony认证包含HUKS测试，工作密钥经加密后默认保存在“/user/app/user/maindata”中。当设备或单板烧写过旧版本OpenHarmony XTS镜像，则需要在烧写新版XTS镜像前，删除历史OpenHarmony版本在文件系统中创建的旧密钥，避免新版本HUKS使用错误的工作密钥导致XTS用例失败。OpenHarmony XTS认证镜像准备步骤如下：

1.  烧写XTS镜像前，检查文件系统中是否有“/user/app/user/maindata”目录。如果已经存在该目录，则手动删除或格式化擦除FS。
2.  烧写OpenHarmony XTS镜像。
3.  开机执行XTS用例，检查HUKS用例结果是否全部通过。

### 应用及表盘市场安全注意事项<a name="ZH-CN_TOPIC_0000002541294689"></a>

穿戴应用市场和表盘市场用于完整性验证的根公钥的保护方案由安全启动保障防篡改。当前方案作为参考设计使用，基于网络安全风险考虑，建议厂商最终的安全方案将根公钥预置到安全OS的安全存储或eFuse中。

### 安全调试<a name="ZH-CN_TOPIC_0000002541254709"></a>

-   **[确保支持关闭JTAG/SSI调试接口](#ZH-CN_TOPIC_0000002509734724)**  

-   **[确保支持关闭USB虚拟串口功能](#ZH-CN_TOPIC_0000002509694714)**  

-   **[确保支持关闭UART日志接口](#ZH-CN_TOPIC_0000002541294693)**  

#### 确保支持关闭JTAG/SSI调试接口<a name="ZH-CN_TOPIC_0000002509734724"></a>

**规则描述<a name="zh-cn_topic_0000002540766517_section1540529217"></a>**

确保关闭JTAG/SSI/JLink等调试接口。

**风险等级<a name="zh-cn_topic_0000002540766517_section1963008177"></a>**

高

**风险描述<a name="zh-cn_topic_0000002540766517_section1098648225"></a>**

如不禁止，商用后产品可以通过对应JTAG/SSI/JLink接口调试。

**修复建议<a name="zh-cn_topic_0000002540766517_section399310287"></a>**

客户商用阶段关闭调试接口，即

swd\_soft\_debug\_en \(bit 298\)：1

swd\_en（bit299）：1

ssi\_en \(bit300\)：1

**修复影响<a name="zh-cn_topic_0000002540766517_section251463828"></a>**

禁止调试口后，产品无法通过对应JTAG/SSI/JLink接口调试。

**安全推荐值<a name="zh-cn_topic_0000002540766517_section1574688996"></a>**

禁止调试口，

swd\_soft\_debug\_en \(bit 298\)：1

swd\_en（bit299）：1

ssi\_en \(bit300\)：1

**缺省值<a name="zh-cn_topic_0000002540766517_section88325088"></a>**

swd\_soft\_debug\_en \(bit 298\)：0、swd\_en（bit299）：0、 ssi\_en \(bit300\)：0

**取值范围<a name="zh-cn_topic_0000002540766517_section1736062345"></a>**

使能（1）禁止（1）

**是否必选<a name="zh-cn_topic_0000002540766517_section80548161"></a>**

是

**是否出厂默认安全<a name="zh-cn_topic_0000002540766517_section1833554699"></a>**

否

#### 确保支持关闭USB虚拟串口功能<a name="ZH-CN_TOPIC_0000002509694714"></a>

**规则描述<a name="zh-cn_topic_0000002540966531_section1737702932"></a>**

确保关闭USB虚拟串口功能，不通过USB打印日志或接收AT命令。

**风险等级<a name="zh-cn_topic_0000002540966531_section541647892"></a>**

中

**风险描述<a name="zh-cn_topic_0000002540966531_section1414144305"></a>**

如不禁止，商用后产品可以通过USB接口打印日志或接收AT。

**修复建议<a name="zh-cn_topic_0000002540966531_section2060114186"></a>**

客户商用阶段关闭USB调试接口，即不定义CFG\_DRIVERS\_USB\_AS\_SERIAL。

**修复影响<a name="zh-cn_topic_0000002540966531_section1964767415"></a>**

禁止USB调试口后，产品无法通过USB接口打印日志或接收AT。

**安全推荐值<a name="zh-cn_topic_0000002540966531_section1272242133"></a>**

不定义CFG\_DRIVERS\_USB\_AS\_SERIAL。

**缺省值<a name="zh-cn_topic_0000002540966531_section1539461961"></a>**

不定义CFG\_DRIVERS\_USB\_AS\_SERIAL。

**取值范围<a name="zh-cn_topic_0000002540966531_section478613925"></a>**

通过CFG\_DRIVERS\_USB\_AS\_SERIAL宏控制。

**是否必选<a name="zh-cn_topic_0000002540966531_section958128093"></a>**

是

**是否出厂默认安全<a name="zh-cn_topic_0000002540966531_section1045752703"></a>**

是

#### 确保支持关闭UART日志接口<a name="ZH-CN_TOPIC_0000002541294693"></a>

**规则描述<a name="zh-cn_topic_0000002540966533_section927395425"></a>**

确保支持关闭UART日志接口。

**风险等级<a name="zh-cn_topic_0000002540966533_section297030841"></a>**

中

**风险描述<a name="zh-cn_topic_0000002540966533_section1503331297"></a>**

如不禁止，商用后产品可以通过对应UART接口打印日志。

**修复建议<a name="zh-cn_topic_0000002540966533_section18678002"></a>**

客户商用阶段关闭调试接口，即不定义SW\_UART\_DEBUG宏。

**修复影响<a name="zh-cn_topic_0000002540966533_section1050579542"></a>**

禁止调试口后，产品无法通过对应UART接口打印日志，无法响应AT命令。

**安全推荐值<a name="zh-cn_topic_0000002540966533_section1956682313"></a>**

不定义SW\_UART\_DEBUG宏。

**缺省值<a name="zh-cn_topic_0000002540966533_section1473169529"></a>**

定义SW\_UART\_DEBUG宏。

**取值范围<a name="zh-cn_topic_0000002540966533_section1965235362"></a>**

通过SW\_UART\_DEBUG宏控制。

**是否必选<a name="zh-cn_topic_0000002540966533_section500418331"></a>**

是

**是否出厂默认安全<a name="zh-cn_topic_0000002540966533_section1937940056"></a>**

否

### 安全启动<a name="ZH-CN_TOPIC_0000002541254713"></a>

-   **[确保支持安全启动使能配置](#ZH-CN_TOPIC_0000002509734728)**  

#### 确保支持安全启动使能配置<a name="ZH-CN_TOPIC_0000002509734728"></a>

**规则描述<a name="zh-cn_topic_0000002509126520_section34550331"></a>**

确保REE、TEE固件的安全启动特性使能。

**风险等级<a name="zh-cn_topic_0000002509126520_section855747754"></a>**

高

**风险描述<a name="zh-cn_topic_0000002509126520_section1099258232"></a>**

REE/TEE镜像可被篡改。

**修复建议<a name="zh-cn_topic_0000002509126520_section1859155652"></a>**

商用阶段客户使能该配置，写eFuse对应secure\_boot\_enable位域（bit715-718）为全FF。

**修复影响<a name="zh-cn_topic_0000002509126520_section1004409878"></a>**

启动时间比禁止安全启动慢百ms量级时间。

**安全推荐值<a name="zh-cn_topic_0000002509126520_section1720164999"></a>**

使能，对应secure\_boot\_enable位域（bit715-718）为全F。

**缺省值<a name="zh-cn_topic_0000002509126520_section1402332838"></a>**

禁止，对应secure\_boot\_enable位域（bit715-718）为0xA。

**取值范围<a name="zh-cn_topic_0000002509126520_section1595631514"></a>**

使能（1）禁止（0）

**是否必选<a name="zh-cn_topic_0000002509126520_section1137254401"></a>**

是

**是否出厂默认安全<a name="zh-cn_topic_0000002509126520_section116381074"></a>**

否

# 结论<a name="ZH-CN_TOPIC_0000002499581729"></a>

HiDiTingV100产品有必要基于安全威胁分析采取相对应的安全措施。以下安全原则供参考：

-   适度的安全：

    安全设计是基于特定的安全危险场景分析，考虑到性能、成本、业务影响，决策采用最合适的安全措施。

-   最小授权：

    根据职责的需要，给用户、维护人员、网络单元、程序、进程等授予最小的权限和资源。这样能减少潜在的安全风险。

-   主动协同防御：

    及时识别恶意攻击源，并在攻击造成显著危害前自动删除恶意用户与网络之间的连接。也可以降低连接的带宽和服务质量，以尽量减少负面影响。

-   纵深防御：

    纵深防御原则涉及到对威胁的多重防御。例如，当一个防御层不够时，另一个防御层将防止造成进一步破坏。

# 免责声明<a name="ZH-CN_TOPIC_0000002466382760"></a>

客户应充分评估自身产品的网络安全要求（包括但不限于：在实际量产产品中，选择安全加解密算法、关闭不需要的调试接口和命令等），并承担最终责任。HiDiTingV100产品提供的本注意事项文档帮助客户对自身产品进行网络安全加固。

HiDiTingV100产品在本文档中所描述的部分功能使用到了密钥来提供安全机制，客户需要妥善生成、烧写、使用并管理密钥（包括但不限于非对称密钥、对称密钥等），否则将自行承担相关风险。

# 缩略语<a name="ZH-CN_TOPIC_0000002499581741"></a>

<a name="zh-cn_topic_0000002436640124_table14680183271418"></a>
<table><tbody><tr id="zh-cn_topic_0000002436640124_row18339329149"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p1783333261419"><a name="zh-cn_topic_0000002436640124_p1783333261419"></a><a name="zh-cn_topic_0000002436640124_p1783333261419"></a><strong id="zh-cn_topic_0000002436640124_b1083363251415"><a name="zh-cn_topic_0000002436640124_b1083363251415"></a><a name="zh-cn_topic_0000002436640124_b1083363251415"></a>A</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row1833103271418"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p208331032171414"><a name="zh-cn_topic_0000002436640124_p208331032171414"></a><a name="zh-cn_topic_0000002436640124_p208331032171414"></a><strong id="zh-cn_topic_0000002436640124_b13833183221411"><a name="zh-cn_topic_0000002436640124_b13833183221411"></a><a name="zh-cn_topic_0000002436640124_b13833183221411"></a>AES</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p583393291418"><a name="zh-cn_topic_0000002436640124_p583393291418"></a><a name="zh-cn_topic_0000002436640124_p583393291418"></a>Advanced Encryption Standard</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p58334327145"><a name="zh-cn_topic_0000002436640124_p58334327145"></a><a name="zh-cn_topic_0000002436640124_p58334327145"></a>高级加密标准</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row08336325149"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p1383314323149"><a name="zh-cn_topic_0000002436640124_p1383314323149"></a><a name="zh-cn_topic_0000002436640124_p1383314323149"></a><strong id="zh-cn_topic_0000002436640124_b583333291414"><a name="zh-cn_topic_0000002436640124_b583333291414"></a><a name="zh-cn_topic_0000002436640124_b583333291414"></a>API</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p138331632191410"><a name="zh-cn_topic_0000002436640124_p138331632191410"></a><a name="zh-cn_topic_0000002436640124_p138331632191410"></a>Application Programming Interface</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p1783320321146"><a name="zh-cn_topic_0000002436640124_p1783320321146"></a><a name="zh-cn_topic_0000002436640124_p1783320321146"></a>应用程序编程接口</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row56901428142719"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row551520325278"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p11516163262715"><a name="zh-cn_topic_0000002436640124_p11516163262715"></a><a name="zh-cn_topic_0000002436640124_p11516163262715"></a><strong id="zh-cn_topic_0000002436640124_b092072313326"><a name="zh-cn_topic_0000002436640124_b092072313326"></a><a name="zh-cn_topic_0000002436640124_b092072313326"></a>B</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row151633213271"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p55161532102717"><a name="zh-cn_topic_0000002436640124_p55161532102717"></a><a name="zh-cn_topic_0000002436640124_p55161532102717"></a><strong id="zh-cn_topic_0000002436640124_b192142320325"><a name="zh-cn_topic_0000002436640124_b192142320325"></a><a name="zh-cn_topic_0000002436640124_b192142320325"></a>BT</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p18436145263116"><a name="zh-cn_topic_0000002436640124_p18436145263116"></a><a name="zh-cn_topic_0000002436640124_p18436145263116"></a>Bluetooth</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p1143695263118"><a name="zh-cn_topic_0000002436640124_p1143695263118"></a><a name="zh-cn_topic_0000002436640124_p1143695263118"></a>蓝牙</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row4690162811271"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row829313241270"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p629316247271"><a name="zh-cn_topic_0000002436640124_p629316247271"></a><a name="zh-cn_topic_0000002436640124_p629316247271"></a><strong id="zh-cn_topic_0000002436640124_b59221723173214"><a name="zh-cn_topic_0000002436640124_b59221723173214"></a><a name="zh-cn_topic_0000002436640124_b59221723173214"></a>D</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row1653114482713"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p453184413275"><a name="zh-cn_topic_0000002436640124_p453184413275"></a><a name="zh-cn_topic_0000002436640124_p453184413275"></a><strong id="zh-cn_topic_0000002436640124_b1792272319326"><a name="zh-cn_topic_0000002436640124_b1792272319326"></a><a name="zh-cn_topic_0000002436640124_b1792272319326"></a>DSP</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p1466617161319"><a name="zh-cn_topic_0000002436640124_p1466617161319"></a><a name="zh-cn_topic_0000002436640124_p1466617161319"></a>Digital signal processor</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p1666116163111"><a name="zh-cn_topic_0000002436640124_p1666116163111"></a><a name="zh-cn_topic_0000002436640124_p1666116163111"></a>数字信号处理器</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row1783323217145"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row1583415322147"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p188341732191419"><a name="zh-cn_topic_0000002436640124_p188341732191419"></a><a name="zh-cn_topic_0000002436640124_p188341732191419"></a><strong id="zh-cn_topic_0000002436640124_b983418321148"><a name="zh-cn_topic_0000002436640124_b983418321148"></a><a name="zh-cn_topic_0000002436640124_b983418321148"></a>E</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row7774194213436"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p127741442184316"><a name="zh-cn_topic_0000002436640124_p127741442184316"></a><a name="zh-cn_topic_0000002436640124_p127741442184316"></a><strong id="zh-cn_topic_0000002436640124_b418416502432"><a name="zh-cn_topic_0000002436640124_b418416502432"></a><a name="zh-cn_topic_0000002436640124_b418416502432"></a>ECB</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p1775742194314"><a name="zh-cn_topic_0000002436640124_p1775742194314"></a><a name="zh-cn_topic_0000002436640124_p1775742194314"></a>Electronic Codebook</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p11775104294319"><a name="zh-cn_topic_0000002436640124_p11775104294319"></a><a name="zh-cn_topic_0000002436640124_p11775104294319"></a>电子密码本模式</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row82351251314"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p6235154112"><a name="zh-cn_topic_0000002436640124_p6235154112"></a><a name="zh-cn_topic_0000002436640124_p6235154112"></a><strong id="zh-cn_topic_0000002436640124_b158473233111"><a name="zh-cn_topic_0000002436640124_b158473233111"></a><a name="zh-cn_topic_0000002436640124_b158473233111"></a>ECC</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p132351651114"><a name="zh-cn_topic_0000002436640124_p132351651114"></a><a name="zh-cn_topic_0000002436640124_p132351651114"></a>Elliptic Curve Cryptography</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p5235651115"><a name="zh-cn_topic_0000002436640124_p5235651115"></a><a name="zh-cn_topic_0000002436640124_p5235651115"></a>椭圆曲线密码学</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row11835143221410"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p3835103213141"><a name="zh-cn_topic_0000002436640124_p3835103213141"></a><a name="zh-cn_topic_0000002436640124_p3835103213141"></a><strong id="zh-cn_topic_0000002436640124_b1583583211149"><a name="zh-cn_topic_0000002436640124_b1583583211149"></a><a name="zh-cn_topic_0000002436640124_b1583583211149"></a>eFuse</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p168357325144"><a name="zh-cn_topic_0000002436640124_p168357325144"></a><a name="zh-cn_topic_0000002436640124_p168357325144"></a>electronic Fuse</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p1383513211146"><a name="zh-cn_topic_0000002436640124_p1383513211146"></a><a name="zh-cn_topic_0000002436640124_p1383513211146"></a><span>一次性可编程存储器</span></p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row183351546300"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row954116016"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p1054121612110"><a name="zh-cn_topic_0000002436640124_p1054121612110"></a><a name="zh-cn_topic_0000002436640124_p1054121612110"></a><strong id="zh-cn_topic_0000002436640124_b83016220119"><a name="zh-cn_topic_0000002436640124_b83016220119"></a><a name="zh-cn_topic_0000002436640124_b83016220119"></a>F</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row118353327144"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p9480115265710"><a name="zh-cn_topic_0000002436640124_p9480115265710"></a><a name="zh-cn_topic_0000002436640124_p9480115265710"></a><strong id="zh-cn_topic_0000002436640124_b1948075265711"><a name="zh-cn_topic_0000002436640124_b1948075265711"></a><a name="zh-cn_topic_0000002436640124_b1948075265711"></a>FS</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p1048075217571"><a name="zh-cn_topic_0000002436640124_p1048075217571"></a><a name="zh-cn_topic_0000002436640124_p1048075217571"></a>File System</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p114804522578"><a name="zh-cn_topic_0000002436640124_p114804522578"></a><a name="zh-cn_topic_0000002436640124_p114804522578"></a>文件系统</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row1748319235014"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row1246818818010"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p204689813012"><a name="zh-cn_topic_0000002436640124_p204689813012"></a><a name="zh-cn_topic_0000002436640124_p204689813012"></a><strong id="zh-cn_topic_0000002436640124_b4488181816017"><a name="zh-cn_topic_0000002436640124_b4488181816017"></a><a name="zh-cn_topic_0000002436640124_b4488181816017"></a>J</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row783543251411"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p1835632171410"><a name="zh-cn_topic_0000002436640124_p1835632171410"></a><a name="zh-cn_topic_0000002436640124_p1835632171410"></a><strong id="zh-cn_topic_0000002436640124_b483513211142"><a name="zh-cn_topic_0000002436640124_b483513211142"></a><a name="zh-cn_topic_0000002436640124_b483513211142"></a>JTAG</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p9835173219146"><a name="zh-cn_topic_0000002436640124_p9835173219146"></a><a name="zh-cn_topic_0000002436640124_p9835173219146"></a>Joint Test Action Group</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p38356329143"><a name="zh-cn_topic_0000002436640124_p38356329143"></a><a name="zh-cn_topic_0000002436640124_p38356329143"></a>联合测试行为组织，是一种国际标准测试协议</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row269571374417"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row158361732161411"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p118361532101410"><a name="zh-cn_topic_0000002436640124_p118361532101410"></a><a name="zh-cn_topic_0000002436640124_p118361532101410"></a><strong id="zh-cn_topic_0000002436640124_b183693218147"><a name="zh-cn_topic_0000002436640124_b183693218147"></a><a name="zh-cn_topic_0000002436640124_b183693218147"></a>R</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row78791216202620"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p687912169264"><a name="zh-cn_topic_0000002436640124_p687912169264"></a><a name="zh-cn_topic_0000002436640124_p687912169264"></a><strong id="zh-cn_topic_0000002436640124_b13395142112619"><a name="zh-cn_topic_0000002436640124_b13395142112619"></a><a name="zh-cn_topic_0000002436640124_b13395142112619"></a>REE</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p11623104494719"><a name="zh-cn_topic_0000002436640124_p11623104494719"></a><a name="zh-cn_topic_0000002436640124_p11623104494719"></a>Rich Execution Environment</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p1662334434716"><a name="zh-cn_topic_0000002436640124_p1662334434716"></a><a name="zh-cn_topic_0000002436640124_p1662334434716"></a>富执行环境</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row1683611326142"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p1383673201418"><a name="zh-cn_topic_0000002436640124_p1383673201418"></a><a name="zh-cn_topic_0000002436640124_p1383673201418"></a><strong id="zh-cn_topic_0000002436640124_b18362325145"><a name="zh-cn_topic_0000002436640124_b18362325145"></a><a name="zh-cn_topic_0000002436640124_b18362325145"></a>ROM</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p13836113218143"><a name="zh-cn_topic_0000002436640124_p13836113218143"></a><a name="zh-cn_topic_0000002436640124_p13836113218143"></a>Read-Only Memory</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p2836193221414"><a name="zh-cn_topic_0000002436640124_p2836193221414"></a><a name="zh-cn_topic_0000002436640124_p2836193221414"></a>只读内存</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row9836203218144"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p583683221420"><a name="zh-cn_topic_0000002436640124_p583683221420"></a><a name="zh-cn_topic_0000002436640124_p583683221420"></a><strong id="zh-cn_topic_0000002436640124_b283615324144"><a name="zh-cn_topic_0000002436640124_b283615324144"></a><a name="zh-cn_topic_0000002436640124_b283615324144"></a>RSA</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p483615325142"><a name="zh-cn_topic_0000002436640124_p483615325142"></a><a name="zh-cn_topic_0000002436640124_p483615325142"></a>RSA Algorithm</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p1883663220149"><a name="zh-cn_topic_0000002436640124_p1883663220149"></a><a name="zh-cn_topic_0000002436640124_p1883663220149"></a>RSA算法</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row1836232181419"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row15836183213149"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p118369329140"><a name="zh-cn_topic_0000002436640124_p118369329140"></a><a name="zh-cn_topic_0000002436640124_p118369329140"></a><strong id="zh-cn_topic_0000002436640124_b138365323141"><a name="zh-cn_topic_0000002436640124_b138365323141"></a><a name="zh-cn_topic_0000002436640124_b138365323141"></a>S</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row11836163219145"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p9836153251411"><a name="zh-cn_topic_0000002436640124_p9836153251411"></a><a name="zh-cn_topic_0000002436640124_p9836153251411"></a><strong id="zh-cn_topic_0000002436640124_b883683215140"><a name="zh-cn_topic_0000002436640124_b883683215140"></a><a name="zh-cn_topic_0000002436640124_b883683215140"></a>SDK</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p19836123261419"><a name="zh-cn_topic_0000002436640124_p19836123261419"></a><a name="zh-cn_topic_0000002436640124_p19836123261419"></a>Software Development Kit</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p15836932181417"><a name="zh-cn_topic_0000002436640124_p15836932181417"></a><a name="zh-cn_topic_0000002436640124_p15836932181417"></a>软件开发工具包</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row583643220142"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p1883673261418"><a name="zh-cn_topic_0000002436640124_p1883673261418"></a><a name="zh-cn_topic_0000002436640124_p1883673261418"></a><strong id="zh-cn_topic_0000002436640124_b0836232171413"><a name="zh-cn_topic_0000002436640124_b0836232171413"></a><a name="zh-cn_topic_0000002436640124_b0836232171413"></a>SHA</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p1783683217147"><a name="zh-cn_topic_0000002436640124_p1783683217147"></a><a name="zh-cn_topic_0000002436640124_p1783683217147"></a>Secure Hash Algorithm</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p98360322147"><a name="zh-cn_topic_0000002436640124_p98360322147"></a><a name="zh-cn_topic_0000002436640124_p98360322147"></a>安全散列算法</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row1983733261420"><td class="nocellnorowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p38378320147"><a name="zh-cn_topic_0000002436640124_p38378320147"></a><a name="zh-cn_topic_0000002436640124_p38378320147"></a><strong id="zh-cn_topic_0000002436640124_b1483711328142"><a name="zh-cn_topic_0000002436640124_b1483711328142"></a><a name="zh-cn_topic_0000002436640124_b1483711328142"></a>SSB</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p28371732121412"><a name="zh-cn_topic_0000002436640124_p28371732121412"></a><a name="zh-cn_topic_0000002436640124_p28371732121412"></a>Second Stage Boot</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p2837932131415"><a name="zh-cn_topic_0000002436640124_p2837932131415"></a><a name="zh-cn_topic_0000002436640124_p2837932131415"></a>二级Boot</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row1579434882416"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p1979464813243"><a name="zh-cn_topic_0000002436640124_p1979464813243"></a><a name="zh-cn_topic_0000002436640124_p1979464813243"></a><strong id="zh-cn_topic_0000002436640124_b138621212152620"><a name="zh-cn_topic_0000002436640124_b138621212152620"></a><a name="zh-cn_topic_0000002436640124_b138621212152620"></a>SSI</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p2098916456298"><a name="zh-cn_topic_0000002436640124_p2098916456298"></a><a name="zh-cn_topic_0000002436640124_p2098916456298"></a>Synchronous Serial Interface</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p1299074511293"><a name="zh-cn_topic_0000002436640124_p1299074511293"></a><a name="zh-cn_topic_0000002436640124_p1299074511293"></a>同步串行接口</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row193886134115"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row173886134119"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p1538815131219"><a name="zh-cn_topic_0000002436640124_p1538815131219"></a><a name="zh-cn_topic_0000002436640124_p1538815131219"></a><strong id="zh-cn_topic_0000002436640124_b424392012116"><a name="zh-cn_topic_0000002436640124_b424392012116"></a><a name="zh-cn_topic_0000002436640124_b424392012116"></a>T</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row12968102432618"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p796882411266"><a name="zh-cn_topic_0000002436640124_p796882411266"></a><a name="zh-cn_topic_0000002436640124_p796882411266"></a><strong id="zh-cn_topic_0000002436640124_b0812132912262"><a name="zh-cn_topic_0000002436640124_b0812132912262"></a><a name="zh-cn_topic_0000002436640124_b0812132912262"></a>TEE</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p1457316524479"><a name="zh-cn_topic_0000002436640124_p1457316524479"></a><a name="zh-cn_topic_0000002436640124_p1457316524479"></a>Trusted Execution Environment</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p257395211472"><a name="zh-cn_topic_0000002436640124_p257395211472"></a><a name="zh-cn_topic_0000002436640124_p257395211472"></a>可信执行环境</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row63883131916"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p738810134112"><a name="zh-cn_topic_0000002436640124_p738810134112"></a><a name="zh-cn_topic_0000002436640124_p738810134112"></a><strong id="zh-cn_topic_0000002436640124_b112461201215"><a name="zh-cn_topic_0000002436640124_b112461201215"></a><a name="zh-cn_topic_0000002436640124_b112461201215"></a>TRNG</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p538881312112"><a name="zh-cn_topic_0000002436640124_p538881312112"></a><a name="zh-cn_topic_0000002436640124_p538881312112"></a>True Random Number Generator</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p17388181317113"><a name="zh-cn_topic_0000002436640124_p17388181317113"></a><a name="zh-cn_topic_0000002436640124_p17388181317113"></a>真随机数发生器</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row6873736122611"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row10874173620269"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p16874123610267"><a name="zh-cn_topic_0000002436640124_p16874123610267"></a><a name="zh-cn_topic_0000002436640124_p16874123610267"></a><strong id="zh-cn_topic_0000002436640124_b3468184562614"><a name="zh-cn_topic_0000002436640124_b3468184562614"></a><a name="zh-cn_topic_0000002436640124_b3468184562614"></a>U</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row487413615267"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p15874836132619"><a name="zh-cn_topic_0000002436640124_p15874836132619"></a><a name="zh-cn_topic_0000002436640124_p15874836132619"></a><strong id="zh-cn_topic_0000002436640124_b6469745112617"><a name="zh-cn_topic_0000002436640124_b6469745112617"></a><a name="zh-cn_topic_0000002436640124_b6469745112617"></a>UART</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p1630863484710"><a name="zh-cn_topic_0000002436640124_p1630863484710"></a><a name="zh-cn_topic_0000002436640124_p1630863484710"></a>Universal Asynchronous Receiver & Transmitter</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p930883412471"><a name="zh-cn_topic_0000002436640124_p930883412471"></a><a name="zh-cn_topic_0000002436640124_p930883412471"></a>通用异步收发器</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002436640124_row1473024914285"><td class="cellrowborder" style="border:none" valign="top" width="21.515151515151516%"><p id="zh-cn_topic_0000002436640124_p3730104912814"><a name="zh-cn_topic_0000002436640124_p3730104912814"></a><a name="zh-cn_topic_0000002436640124_p3730104912814"></a><strong id="zh-cn_topic_0000002436640124_b12904155542817"><a name="zh-cn_topic_0000002436640124_b12904155542817"></a><a name="zh-cn_topic_0000002436640124_b12904155542817"></a>USB</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.151515151515156%"><p id="zh-cn_topic_0000002436640124_p202961942125019"><a name="zh-cn_topic_0000002436640124_p202961942125019"></a><a name="zh-cn_topic_0000002436640124_p202961942125019"></a>Universal Serial Bus</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.333333333333336%"><p id="zh-cn_topic_0000002436640124_p229754285018"><a name="zh-cn_topic_0000002436640124_p229754285018"></a><a name="zh-cn_topic_0000002436640124_p229754285018"></a>通用串行总线</p>
</td>
</tr>
</tbody>
</table>

