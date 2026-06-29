# 前言<a name="ZH-CN_TOPIC_0000002466382656"></a>

**概述<a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_section64031159132213"></a>**

本文档主要描述了CIPHER DRIVER的结构及其软件接口的使用方法。


**读者对象<a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_section297mcpsimp"></a>**

本文档主要适用于以下工程师：

-   技术支持工程师
-   软件开发工程师

**符号约定<a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_section303mcpsimp"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_table306mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_row311mcpsimp"><th class="cellrowborder" valign="top" width="21%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p313mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p313mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p313mcpsimp"></a><strong id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_b314mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_b314mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_b314mcpsimp"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p316mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p316mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p316mcpsimp"></a><strong id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_b317mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_b317mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_b317mcpsimp"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_row319mcpsimp"><td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.3.1.1 "><p class="msonormal" id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p321mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p321mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p321mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image123"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image123"></a><span><img id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image123" src="figures/zh-cn_image_0000002469435290.png" height="23.94" width="67.83"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p323mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p323mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p323mcpsimp"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_row324mcpsimp"><td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.3.1.1 "><p class="msonormal" id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p326mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p326mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p326mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image124"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image124"></a><span><img id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image124" src="figures/zh-cn_image_0000002469275312.png" height="23.94" width="67.83"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p328mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p328mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p328mcpsimp"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_row329mcpsimp"><td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.3.1.1 "><p class="msonormal" id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p331mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p331mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p331mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image125"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image125"></a><span><img id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image125" src="figures/zh-cn_image_0000002502395265.png" height="23.94" width="67.83"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p333mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p333mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p333mcpsimp"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_row334mcpsimp"><td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.3.1.1 "><p class="msonormal" id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p336mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p336mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p336mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image126"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image126"></a><span><img id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image126" src="figures/zh-cn_image_0000002502275293.png" height="23.94" width="67.83"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p338mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p338mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p338mcpsimp"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p339mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p339mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p339mcpsimp"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_row340mcpsimp"><td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.3.1.1 "><p class="msonormal" id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p342mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p342mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p342mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image127"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image127"></a><span><img id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_image127" src="figures/zh-cn_image_0000002469435292.png" height="23.94" width="67.83"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p344mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p344mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p344mcpsimp"></a>对正文中重点信息的补充说明。</p>
<p id="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p345mcpsimp"><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p345mcpsimp"></a><a name="zh-cn_topic_0000002446160629_zh-cn_topic_0000002112799948_p345mcpsimp"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# 概述<a name="ZH-CN_TOPIC_0000002466382540"></a>

-   **[功能描述](#ZH-CN_TOPIC_0000002499581561)**  

-   **[使用流程](#ZH-CN_TOPIC_0000002466542176)**  

## 功能描述<a name="ZH-CN_TOPIC_0000002499581561"></a>

CIPHER DRIVER为安全算法模块，主要用于AIOT产品安全启动、安全存储、安全通讯等场景，提供mbedtls API和service layer两类接口，如[图1](#zh-cn_topic_0000002412561380_zh-cn_topic_0000002112799920_fig727383712313)所示。

-   mbedtls API
    -   安全驱动对接开源第三方mbedtls接口，可以通过调用mbedtls API使用硬件安全能力，通过定义MBEDTLS\_HARDEN\_OPEN宏开启硬化能力，默认已开启。
    -   对于硬件支持的规格，均完成mbedtls对接，基于mbedtls开启的硬化宏具体请参考《mbed TLS 开发指南》。

-   service layer
    -   自研安全驱动接口，为上层业务提供直接使用硬件安全的能力，支持LiteOS环境，及flashboot/bootrom等无OS环境。

-   CIPHER DRIVER包括5个子模块：SYMC（Symmetric-key algorithm，对称密钥算法）、HASH（HASH function，散列函数）、PKE（Public Key Encryption，公开密钥加密算法）、KM（Key Manage，密钥管理）、TRNG。提供如下算法实现：
    -   AES（Advanced Encryption Standard，高级加密标准）、SM4（商密4分组密码算法）等对称密钥算法。
    -   SHA2（Secure Hash Algorithm 2，安全散列算法2）、SM3（商密3密码杂凑算法）及HMAC（hash-based message authentication code，散列消息认证码）-SHA2、HMAC-SM3摘要算法。
    -   RSA（Rivest–Shamir–Adleman算法）、ECC（Elliptic Curve Cryptography，椭圆曲线密码学）、SM2（商密2椭圆曲线公钥密码算法）签名验签、密钥协商、加解密、大数运算等非对称算法。
    -   硬件密钥管理及硬件密钥派生算法PBKDF2（Password-Based Key Derivation Function 2，基于密码的密钥派生函数2）-HMAC-SHA2、PBKDF2-HMAC-SM3、HKDF（HMAC-based Key Derivation Function，基于HMAC的密钥派生算法）。
    -   随机数算法TRNG（True Random Number Generator，真随机数发生器）。
    -   Flash在线解密。

**图 1**  CIPHER DRIVER上下文关系图<a name="zh-cn_topic_0000002412561380_zh-cn_topic_0000002112799920_fig727383712313"></a>  
![https://cloudmodeling.tools.huawei.com/projectspace/detail/a365a085655c41c9a23ba3b13b280203?openedDiagramUUID=bd87d1d60ddd4705b5772a3b09c77a62&versionUUID=](figures/CIPHER-DRIVER上下文关系图.png "CIPHER-DRIVER上下文关系图")

-   **[对称加解密算法](#ZH-CN_TOPIC_0000002499581661)**  

-   **[摘要算法](#ZH-CN_TOPIC_0000002499581513)**  

-   **[PKE](#ZH-CN_TOPIC_0000002499501509)**  

-   **[KM](#ZH-CN_TOPIC_0000002466382696)**  

-   **[TRNG](#ZH-CN_TOPIC_0000002466542316)**  

### 对称加解密算法<a name="ZH-CN_TOPIC_0000002499581661"></a>

-   AES：支持ECB（Electronic Codebook，电子密码本）、CBC（Cipher Block Chaining，密码分组链接）、CFB（Cipher Feedback，密码反馈）、OFB（Output Feedback，输出反馈）、CTR（Counter，计数器）、CCM（Counter with CBC-MAC，带有CBC-MAC 的CTR）、GCM（Galois/Counter Mode，伽罗瓦/计数器模式）、CMAC（Cipher-based Message Authentication Code，加密消息认证码）、CBC\_MAC（Cipher Block Chaining Message Authentication Code，密文分组链接消息验证码）共9种工作模式。其中：
    -   CFB模式支持的加密数据位宽可为8/128 bit。
    -   OFB模式支持的加密数据位宽可为128bit。
    -   CCM/GCM模式下，加解密结束后需获取一次TAG值。
    -   ECB、CBC模式加密数据长度必须为16Byte对齐，CMAC允许最后一块加密数据长度非16Byte对齐。
    -   支持AES-128、AES-192、AES-256（128、192、256均指传入的密钥长度，单位为bit）。

-   SM4：支持ECB/CBC/CTR/CFB/OFB/CCM/GCM/CBC\_MAC共8种工作模式。支持SM4-128；对于ECB/CBC模式，加密数据长度须为16Byte对齐。

支持低功耗模式，默认开启低功耗。

>![](public_sys-resources/icon-note.gif) **说明：** 
>ECB模式属于非安全算法，不建议使用。
>CBC\_MAC支持对长度为0的数据计算MAC值；CMAC算法不支持对长度为0的数据计算MAC值。

### 摘要算法<a name="ZH-CN_TOPIC_0000002499581513"></a>

-   HASH：支持SHA224/SHA256/SHA384/SHA512/SM3。
-   HMAC：支持HMAC-SHA224/HMAC-SHA256/HMAC-SHA384/HMAC-SHA512/HMAC-SM3。

HASH和HMAC支持中途传入非block size对齐的数据块；支持低功耗模式，默认开启低功耗。

>![](public_sys-resources/icon-note.gif) **说明：** 
>SHA224属于非安全算法，不建议使用。
>某些芯片不支持部分摘要算法，接口调用会返回不支持或失败，以实际芯片规格为准。

### PKE<a name="ZH-CN_TOPIC_0000002499501509"></a>

PKE（Public Key Encryption，公钥加密），即非对称加解密算法。常用于文件的签名和验签，以及对对称密钥的加解密，主要包括以下算法：

-   RSA
    -   支持PKCS\#1 v1.5和 PKCS\#1 v2.1PSS两种模式的私钥签名和公钥验签。
    -   支持PKCS\#1 v1.5 和 PKCS\#1 v2.1OAEP两种模式的公钥加密和私钥解密。
    -   支持密钥位宽1024/2048/3072/4096。

-   ECC
    -   支持RFC 5639-Brainpool P256/384/512r1、NIST FIPS 186-4 P192/224/256/384/521和RFC 8032-ED25519（即EDDSA）的私钥签名和公钥验签。
    -   支持密钥位宽192/224/256/384/512\(521\)。

-   SM2
    -   支持SM2公钥加密和私钥解密。
    -   支持SM2私钥签名和公钥验签。

-   密钥协商
    -   支持RFC 5639-Brainpool P256/384/512r1、NIST FIPS 186-4 P192/224/256/384/521、RFC 7748-Curve25519/Curve448、 RFC 8032 - ED25519（即EDDSA）和SM2的公私钥对生成，及由给定的私钥生成其对应的公钥。
    -   支持RFC 5639-Brainpool P256/384/512r1、NIST FIPS 186-4 P192/224/256/384/521、RFC 7748-Curve25519和SM2的密钥交换（ECDH）。
    -   支持192/224/256/384/512/521/1024/2048/3072/4096 bit DH密钥协商。

-   曲线上点的验证
    -   支持RFC 5639-Brainpool P256/384/512r1、NIST FIPS 186-4 P192/224/256/384/521和SM2的点是否在曲线上的验证。

-   大数运算
    -   支持小于等于4096位的模加、模减、模乘、模逆、模幂和模运算。
    -   支持小于等于2048位的常规大数乘运算。

PKE支持低功耗模式，并默认开启低功耗。仅支持轮询模式。

>![](public_sys-resources/icon-note.gif) **说明：** 
>NIST FIPS 186-4 P192/224/256/384/521，PKCS\#1 v1.5，RSA1024/2048，低于256bit的ECDH及低于2048bit的DH为不安全算法，不建议使用。

### KM<a name="ZH-CN_TOPIC_0000002466382696"></a>

KM是密钥管理模块，增加密钥安全强度，用于将key加载到keyslot中，供对称加解密算法、摘要算法和Flash在线解密使用。支持硬件key和软件key；支持2个cipher keyslot通道和2个HMAC keyslot通道。

### TRNG<a name="ZH-CN_TOPIC_0000002466542316"></a>

随机数获取模块，获取硬件产生的真随机数。

## 使用流程<a name="ZH-CN_TOPIC_0000002466542176"></a>

系统初始化时会主动调用uapi\_drv\_cipher\_env\_init完成CIPHER DRIVER基础的初始化环境配置，用户不需要再单独调用。

-   **[密钥配置](#ZH-CN_TOPIC_0000002499581669)**  

-   **[HKDF密钥派生](#ZH-CN_TOPIC_0000002499501541)**  

-   **[对称加解密](#ZH-CN_TOPIC_0000002499581529)**  

-   **[MAC值获取](#ZH-CN_TOPIC_0000002499581605)**  

-   **[HASH计算](#ZH-CN_TOPIC_0000002466382632)**  

-   **[HMAC计算](#ZH-CN_TOPIC_0000002466382616)**  

-   **[RSA加解密](#ZH-CN_TOPIC_0000002466382648)**  

-   **[RSA签名验签](#ZH-CN_TOPIC_0000002466382640)**  

-   **[SM2加解密](#ZH-CN_TOPIC_0000002466382688)**  

-   **[SM2签名验签](#ZH-CN_TOPIC_0000002499581677)**  

-   **[ECC签名验签](#ZH-CN_TOPIC_0000002499501485)**  

-   **[EDDSA签名验签](#ZH-CN_TOPIC_0000002499581591)**  

-   **[大数运算](#ZH-CN_TOPIC_0000002466382604)**  

-   **[ECDH密钥协商](#ZH-CN_TOPIC_0000002499581613)**  

-   **[DH密钥协商](#ZH-CN_TOPIC_0000002466382588)**  

-   **[TRNG随机数获取](#ZH-CN_TOPIC_0000002466382596)**  

-   **[安全相关eFuse烧写建议](#ZH-CN_TOPIC_0000002466542252)**  

### 密钥配置<a name="ZH-CN_TOPIC_0000002499581669"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002499501501)**  

-   **[软件密钥配置工作流程](#ZH-CN_TOPIC_0000002499501601)**  

-   **[硬件密钥配置工作流程](#ZH-CN_TOPIC_0000002499501557)**  

-   **[注意事项](#ZH-CN_TOPIC_0000002466382664)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002499501501"></a>

生成和配置SPACC、在线解密模块所需要的软件key和硬件Key，软件key可由PBKDF2和HKDF算法生成。keyslot存放最终的软件或硬件key，供SPACC和在线解密使用。支持AES 128/192/256 bit加解密，支持HMAC。

#### 软件密钥配置工作流程<a name="ZH-CN_TOPIC_0000002499501601"></a>

1.  创建keyslot句柄，调用uapi\_drv\_keyslot\_setup创建keyslot句柄，创建时可选择keyslot类型（对称加解密、HMAC、Flash在线解密）。
2.  配置软件key，调用uapi\_drv\_keyslot\_set\_clear\_key接口完成。
3.  <a name="zh-cn_topic_0000002446040557_zh-cn_topic_0000002148438417_li158mcpsimp"></a>销毁keyslot句柄，调用uapi\_drv\_keyslot\_teardown接口完成。

>![](public_sys-resources/icon-note.gif) **说明：** 
>[3](#zh-cn_topic_0000002446040557_zh-cn_topic_0000002148438417_li158mcpsimp)的操作需要在对称加解密、HMAC使用完keyslot之后再执行，否则可能导致计算出错。
>即要求keyslot的生命周期要大于等于对称加解密、HMAC计算的生命周期。
>Flash在线解密没有这个限制。

#### 硬件密钥配置工作流程<a name="ZH-CN_TOPIC_0000002499501557"></a>

1.  创建keyslot句柄，调用uapi\_drv\_keyslot\_setup创建keyslot句柄，创建时可选择keyslot类型（对称加解密、HMAC、Flash在线解密）。
2.  配置硬件key，调用uapi\_drv\_keyslot\_set\_hard\_key接口完成。
3.  <a name="zh-cn_topic_0000002412561392_zh-cn_topic_0000002148438393_li1364mcpsimp"></a>销毁keyslot句柄，调用uapi\_drv\_keyslot\_teardown接口完成。

>![](public_sys-resources/icon-note.gif) **说明：** 
>-   [3](#zh-cn_topic_0000002412561392_zh-cn_topic_0000002148438393_li1364mcpsimp)的要求同上。
>-   硬件key配置需要烧写对应eFuse才可在keyslot通道中获得其对应的正确的工作key，否则此key用于计算得到的结果是错误的，具体eFuse烧写过程请参见“[安全相关eFuse烧写建议](#ZH-CN_TOPIC_0000002466542252)”小节。

#### 注意事项<a name="ZH-CN_TOPIC_0000002466382664"></a>

硬件密钥配置依赖对应eFuse，具体eFuse烧写方式详见“[安全相关eFuse烧写建议](#ZH-CN_TOPIC_0000002466542252)”小节。

**表 1**  硬件密钥类型说明

<a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_table873316114307"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_row15734318308"><th class="cellrowborder" valign="top" width="40.29%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p1973417133016"><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p1973417133016"></a><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p1973417133016"></a>硬件密钥</p>
</th>
<th class="cellrowborder" valign="top" width="59.709999999999994%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p1734121153010"><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p1734121153010"></a><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p1734121153010"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_row17844135233317"><td class="cellrowborder" valign="top" width="40.29%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p148446523338"><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p148446523338"></a><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p148446523338"></a>UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_SEC_STORE</p>
</td>
<td class="cellrowborder" valign="top" width="59.709999999999994%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p284455217333"><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p284455217333"></a><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p284455217333"></a>安全存储使用。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_row165151320143416"><td class="cellrowborder" valign="top" width="40.29%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p4515220123411"><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p4515220123411"></a><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p4515220123411"></a>UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_ODRK_TEE</p>
</td>
<td class="cellrowborder" valign="top" width="59.709999999999994%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p9515162012349"><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p9515162012349"></a><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p9515162012349"></a>TEE下用于Flash在线解密。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_row14951931133414"><td class="cellrowborder" valign="top" width="40.29%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p169511531203417"><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p169511531203417"></a><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p169511531203417"></a>UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_USER_REE</p>
</td>
<td class="cellrowborder" valign="top" width="59.709999999999994%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p13952133111347"><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p13952133111347"></a><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p13952133111347"></a>REE下用户密钥，可自定义使用场景。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_row94642313351"><td class="cellrowborder" valign="top" width="40.29%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p1746414333517"><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p1746414333517"></a><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p1746414333517"></a>UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_USER_TEE</p>
</td>
<td class="cellrowborder" valign="top" width="59.709999999999994%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p346443183514"><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p346443183514"></a><a name="zh-cn_topic_0000002412401508_zh-cn_topic_0000002112959732_p346443183514"></a>TEE下用户密钥，可自定义使用场景。</p>
</td>
</tr>
</tbody>
</table>

### HKDF密钥派生<a name="ZH-CN_TOPIC_0000002499501541"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002499501477)**  

-   **[密钥提取工作流程](#ZH-CN_TOPIC_0000002499581577)**  

-   **[密钥拓展工作流程](#ZH-CN_TOPIC_0000002499581569)**  

-   **[密钥派生工作流程](#ZH-CN_TOPIC_0000002499581637)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002499501477"></a>

根据派生材料，生成密钥，供加解密、HMAC摘要计算等场景使用

#### 密钥提取工作流程<a name="ZH-CN_TOPIC_0000002499581577"></a>

1.  调用uapi\_drv\_cipher\_hkdf\_extract接口完成加密钥提取操作。

#### 密钥拓展工作流程<a name="ZH-CN_TOPIC_0000002499581569"></a>

1.  调用uapi\_drv\_cipher\_hkdf\_expand接口完成加密钥拓展操作。

#### 密钥派生工作流程<a name="ZH-CN_TOPIC_0000002499581637"></a>

1.  调用uapi\_drv\_cipher\_hkdf接口完成加密钥派生操作。

>![](public_sys-resources/icon-note.gif) **说明：** 
>密钥派生包含了密钥提取和密钥拓展两步，可以直接通过密钥派生流程得到密钥。

### 对称加解密<a name="ZH-CN_TOPIC_0000002499581529"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002466542268)**  

-   **[单包加密工作流程](#ZH-CN_TOPIC_0000002466382612)**  

-   **[多包加密工作流程](#ZH-CN_TOPIC_0000002499501517)**  

-   **[注意事项](#ZH-CN_TOPIC_0000002499501525)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002466542268"></a>

对数据进行加密或解密，支持以下场景数据的加解密操作：

-   源数据所在位置为RAM、Flash。
-   目的数据所在位置为RAM。

支持原地加解密操作，即输入和输出使用同一块buffer地址。

#### 单包加密工作流程<a name="ZH-CN_TOPIC_0000002466382612"></a>

-   ECB/CBC/CTR/OFB/CFB8/CFB128工作流程：

1.  调用uapi\_drv\_cipher\_symc\_crypt接口完成加解密操作。

-   CCM加密工作流程：

1.  调用uapi\_drv\_cipher\_symc\_ccm\_encrypt接口完成CCM加密操作，并获取tag值。

-   CCM解密工作流程：

1.  调用uapi\_drv\_cipher\_symc\_ccm\_decrypt\_verify接口完成CCM解密操作，并校验tag值。

-   GCM加密工作流程：

1.  调用uapi\_drv\_cipher\_symc\_gcm\_encrypt接口完成GCM加密操作，并获取tag值。

-   GCM解密工作流程：

1.  调用uapi\_drv\_cipher\_symc\_gcm\_decrypt\_verify接口完成GCM解密操作，并校验tag值。

>![](public_sys-resources/icon-note.gif) **说明：** 
>其中的key有两种传入方式，明文（软件） key传入建议使用方式二，硬件key传入只能使用方式一。
>-   方式一：传入key对应的 keyslot，keyslot获取方式见“[密钥配置](#ZH-CN_TOPIC_0000002499581669)”。
>-   方式二：使用key 和 key\_len参数直接传入明文key。

#### 多包加密工作流程<a name="ZH-CN_TOPIC_0000002499501517"></a>

-   ECB/CBC/CTR/OFB/CFB8/CFB128工作流程：

1.  <a name="zh-cn_topic_0000002412401512_zh-cn_topic_0000002148438373_li1035715471201"></a>建一路symc，并获取symc句柄。调用**uapi\_drv\_cipher\_symc\_setup**接口完成。其中的key有两种传入方式，明文（软件） key 传入建议使用方式二，硬件 key 传入只能使用方式一。
    -   方式一：传入key对应的keyslot，keyslot获取方式见“[密钥配置](#ZH-CN_TOPIC_0000002499581669)”。
    -   方式二：是用key和key\_len参数直接传入明文key。

2.  <a name="zh-cn_topic_0000002412401512_zh-cn_topic_0000002148438373_li1429712311106"></a>根据加密模式，选择下面接口中对应的加密接口，并多次调用，直至完成所有数据的加密。
    -   ECB：uapi\_drv\_cipher\_symc\_ecb\_update。
    -   CBC：uapi\_drv\_cipher\_symc\_cbc\_update。
    -   CTR：uapi\_drv\_cipher\_symc\_ctr\_update。
    -   OFB：uapi\_drv\_cipher\_symc\_ofb\_update。
    -   CFB8：uapi\_drv\_cipher\_symc\_cfb8\_update。
    -   CFB128：uapi\_drv\_cipher\_symc\_cfb128\_update。

3.  销毁symc句柄，调用**uapi\_drv\_cipher\_symc\_teardown**接口完成。

>![](public_sys-resources/icon-note.gif) **说明：** 
>[1](#zh-cn_topic_0000002412401512_zh-cn_topic_0000002148438373_li1035715471201)支持多通道。
>[2](#zh-cn_topic_0000002412401512_zh-cn_topic_0000002148438373_li1429712311106)中update接口里的nc\_off、iv\_off参数，第一次调用接口时传入0，后续接口调用直接传入上次接口调用的输出值即可。

-   CCM工作流程：

1.  建一路symc，并获取symc句柄。调用**uapi\_drv\_cipher\_symc\_ccm\_setup**接口完成。
2.  设置附加数据数据长度、加解密数据长度和tag长度。调用**uapi\_drv\_cipher\_symc\_ccm\_set\_lengths**接口完成。
3.  调用零次或多次接口传递分段的附加数据。调用**uapi\_drv\_cipher\_symc\_ccm\_update\_ad**接口完成。
4.  调用零次或多次接口分段加密或解密数据。调用**uapi\_drv\_cipher\_symc\_ccm\_update**接口完成。
5.  获取最终的 tag 值。调用**uapi\_drv\_cipher\_symc\_ccm\_finish**接口完成。
6.  销毁 symc 句柄。调用**uapi\_drv\_cipher\_symc\_ccm\_teardown**接口完成。

>![](public_sys-resources/icon-note.gif) **说明：** 
>其中的key有两种传入方式，明文（软件） key传入建议使用方式二，硬件key传入只能使用方式一。
>-   方式一：传入key对应的 keyslot，keyslot 获取方式见“[密钥配置](#ZH-CN_TOPIC_0000002499581669)”。
>-   方式二：使用key和key\_len参数直接传入明文key。

-   GCM工作流程：

1.  建一路symc，并获取symc句柄。调用**uapi\_drv\_cipher\_symc\_gcm\_setup**接口完成。
2.  调用零次或多次接口传递分段的附加数据。调用**uapi\_drv\_cipher\_symc\_gcm\_update\_ad**接口完成。
3.  调用零次或多次接口分段加密或解密数据。调用**uapi\_drv\_cipher\_symc\_gcm\_update**接口完成。
4.  获取最终的tag值。调用**uapi\_drv\_cipher\_symc\_gcm\_finish**接口完成。
5.  销毁symc句柄。调用**uapi\_drv\_cipher\_symc\_gcm\_teardown**接口完成。

>![](public_sys-resources/icon-note.gif) **说明：** 
>其中的key有两种传入方式，明文（软件） key传入建议使用方式二，硬件key传入只能使用方式一。
>-   方式一：传入key对应的keyslot，keyslot获取方式见“[密钥配置](#ZH-CN_TOPIC_0000002499581669)”。
>-   方式二：使用key和key\_len参数直接传入明文key。

#### 注意事项<a name="ZH-CN_TOPIC_0000002499501525"></a>

-   支持AES ECB/CBC/CTR/OFB/CFB8/CFB128/CCM/GCM共8种模式。
-   CTR/CFB8/CFB128支持流加解密，单次加解密的数据长度无对齐要求；ECB/CBC/OFB加解密要求数据长度对齐到16Byte。
-   CCM/GCM支持流加解密，包括附加数据（aad）的长度和加解密数据长度均无对齐要求。
-   支持软件多通道。
    -   可同时进行多个数据加解密操作，即执行[2](#zh-cn_topic_0000002412401512_zh-cn_topic_0000002148438373_li1429712311106)进行数据加密运算时，在本次运算未完成（即未执行[3](#zh-cn_topic_0000002412401512_zh-cn_topic_0000002148438373_li1830519199620)）之前，可申请一个新通道，启动另一个数据的加解密运算，直到申请不到通道为止。

### MAC值获取<a name="ZH-CN_TOPIC_0000002499581605"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002499501581)**  

-   **[单包数据MAC值计算流程](#ZH-CN_TOPIC_0000002466542184)**  

-   **[多包数据MAC值计算流程](#ZH-CN_TOPIC_0000002466542220)**  

-   **[注意事项](#ZH-CN_TOPIC_0000002499501565)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002499501581"></a>

获取数据MAC（Message Authentication Code，消息认证码）值，支持以下场景：源数据所在位置为RAM、Flash。

#### 单包数据MAC值计算流程<a name="ZH-CN_TOPIC_0000002466542184"></a>

1.  调用**uapi\_drv\_cipher\_cbc\_mac**或**uapi\_drv\_cipher\_cmac**完成MAC值计算。

#### 多包数据MAC值计算流程<a name="ZH-CN_TOPIC_0000002466542220"></a>

CMAC 和 CBC-MAC算法调用流程一致，此处以CMAC为例进行说明。

1.  创建一路 mac 句柄。调用**uapi\_drv\_cipher\_cmac\_start**接口完成。
2.  多次调用**uapi\_drv\_cipher\_cmac\_update**接口对分段数据进行MAC计算，直至传入所有数据。
3.  获取MAC值，调用  **uapi\_drv\_cipher\_cmac\_finish**接口完成。
4.  销毁MAC句柄。调用**uapi\_drv\_cipher\_cmac\_destroy**接口完成。

#### 注意事项<a name="ZH-CN_TOPIC_0000002499501565"></a>

无论是单包MAC值计算流程还是多包MAC值计算流程，其out参数均为用户传入的buffer，用户需根据接口参数定义传入正确大小的buffer，以免导致数据访问越界。

### HASH计算<a name="ZH-CN_TOPIC_0000002466382632"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002466542236)**  

-   **[单包数据摘要计算工作流程](#ZH-CN_TOPIC_0000002466542216)**  

-   **[多包数据摘要计算工作流程](#ZH-CN_TOPIC_0000002499581597)**  

-   **[注意事项](#ZH-CN_TOPIC_0000002466542244)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002466542236"></a>

获取消息摘要，支持以下场景：源数据所在位置为RAM、Flash。

#### 单包数据摘要计算工作流程<a name="ZH-CN_TOPIC_0000002466542216"></a>

1.  根据要使用的摘要计算算法，选择下面接口中对应的摘要计算接口，完成消息的摘要计算。
    -   SHA1：uapi\_drv\_cipher\_sha1。
    -   SHA224：uapi\_drv\_cipher\_sha224。
    -   SHA256：uapi\_drv\_cipher\_sha256。
    -   SHA384：uapi\_drv\_cipher\_sha384。
    -   SHA512：uapi\_drv\_cipher\_sha512。
    -   SM3：uapi\_drv\_cipher\_sm3。

#### 多包数据摘要计算工作流程<a name="ZH-CN_TOPIC_0000002499581597"></a>

各摘要算法的调用流程一致，此处以SHA256为例进行说明。

1.  <a name="zh-cn_topic_0000002446040573_zh-cn_topic_0000002148438353_li10655629143515"></a>创建一路hash，获取hash句柄。调用**uapi\_drv\_cipher\_sha256\_start**接口完成。
2.  多次调用**uapi\_drv\_cipher\_sha256\_update**接口对消息进行摘要计算，直至传入整个消息数据。
3.  获取消息摘要，调用**uapi\_drv\_cipher\_sha256\_finish**接口完成。
4.  销毁hash句柄。调用**uapi\_drv\_cipher\_sha\_destroy**接口完成。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >[1](#zh-cn_topic_0000002446040573_zh-cn_topic_0000002148438353_li10655629143515)启动成功后，无论后续步骤成功或失败，均需调用**uapi\_drv\_cipher\_sha\_destroy**接口销毁hash句柄，否则会造成资源泄露。

#### 注意事项<a name="ZH-CN_TOPIC_0000002466542244"></a>

无论是单包摘要计算流程还是多包摘要计算流程，其out参数均为用户传入的buffer，用户需根据接口参数定义传入正确大小的buffer，以免导致数据访问越界。

### HMAC计算<a name="ZH-CN_TOPIC_0000002466382616"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002499501573)**  

-   **[单包数据摘要计算工作流程](#ZH-CN_TOPIC_0000002466542208)**  

-   **[多包数据摘要计算工作流程](#ZH-CN_TOPIC_0000002499581653)**  

-   **[注意事项](#ZH-CN_TOPIC_0000002466542260)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002499501573"></a>

依赖调用者传入的密钥数据，获取消息摘要，该密钥数据由消息交换双方提前协商好。支持以下场景：源数据所在位置为RAM、Flash。

#### 单包数据摘要计算工作流程<a name="ZH-CN_TOPIC_0000002466542208"></a>

1.  根据要使用的摘要计算算法，选择下面接口中对应的摘要计算接口，完成消息的摘要计算。
    -   HMAC-SHA1：uapi\_drv\_cipher\_hmac\_sha1。
    -   HMAC-SHA224：uapi\_drv\_cipher\_hmac\_sha224。
    -   HMAC-SHA256：uapi\_drv\_cipher\_hmac\_sha256。
    -   HMAC-SHA384：uapi\_drv\_cipher\_hmac\_sha384。
    -   HMAC-SHA512：uapi\_drv\_cipher\_hmac\_sha512。
    -   HMAC-SM3：uapi\_drv\_cipher\_hmac\_sm3。

>![](public_sys-resources/icon-note.gif) **说明：** 
>其中的key有两种传入方式，明文（软件） key传入建议使用方式二，硬件key传入只能使用方式一。
>-   方式一：传入key对应的keyslot，keyslot获取方式见“[密钥配置](#ZH-CN_TOPIC_0000002499581669)”。
>-   方式二：使用key和key\_len参数直接传入明文key。

#### 多包数据摘要计算工作流程<a name="ZH-CN_TOPIC_0000002499581653"></a>

各摘要算法的调用流程一致，此处以SHA256为例进行说明。

1.  <a name="zh-cn_topic_0000002412561412_zh-cn_topic_0000002112959736_li1049110437573"></a>创建一路hmac，获取hmac句柄。调用  **uapi\_drv\_cipher\_hmac\_sha256\_start**接口完成。
2.  多次调用**uapi\_drv\_cipher\_hmac\_sha256\_update**接口对消息进行摘要计算，直至传入整个消息数据。
3.  获取消息摘要，调用**uapi\_drv\_cipher\_hmac\_sha256\_finish**接口完成。
4.  销毁hmac句柄。调用**uapi\_drv\_cipher\_hmac\_destroy**接口完成。

>![](public_sys-resources/icon-note.gif) **说明：** 
>其中的key有两种传入方式，明文（软件） key传入建议使用方式二，硬件key传入只能使用方式一。
>-   方式一（暂不支持）：传入key对应的keyslot，keyslot获取方式见“[密钥配置](#ZH-CN_TOPIC_0000002499581669)”。
>-   方式二：使用key和key\_len 参数直接传入明文 key。
>[1](#zh-cn_topic_0000002412561412_zh-cn_topic_0000002112959736_li1049110437573)  启动成功后，无论后续步骤成功或失败，均需调用**uapi\_drv\_cipher\_hmac\_destroy**接口销毁hmac句柄，否则会造成资源泄露。

#### 注意事项<a name="ZH-CN_TOPIC_0000002466542260"></a>

无论是单包摘要计算流程还是多包摘要计算流程，其out参数均为用户传入的buffer，用户需根据接口参数定义传入正确大小的buffer，以免导致数据访问越界。

调用者传入的密钥数据，如果是明文密钥，则要求密钥长度不能大于当前hash类型的block\_size大小，如果密钥长度大于当前hash类型的block\_size大小，则需要调用者先对密钥进行hash计算，使得其长度小于block\_size。如果是密文密钥，则密钥长度只能为16Byte、24Byte或32Byte。

>![](public_sys-resources/icon-note.gif) **说明：** 
>SHA1/SHA224/SHA256/SM3 block size大小为64Byte，SHA384/SHA512 block size大小为128Byte。

### RSA加解密<a name="ZH-CN_TOPIC_0000002466382648"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002466542200)**  

-   **[工作流程](#ZH-CN_TOPIC_0000002499501609)**  

-   **[注意事项](#ZH-CN_TOPIC_0000002466542276)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002466542200"></a>

RSA加解密应用场景为公钥加密私钥解密。

#### 工作流程<a name="ZH-CN_TOPIC_0000002499501609"></a>

1.  公钥加密，调用uapi\_drv\_cipher\_pke\_rsa\_public\_encrypt接口完成。
2.  私钥解密，调用uapi\_drv\_cipher\_pke\_rsa\_private\_decrypt接口完成。

>![](public_sys-resources/icon-note.gif) **说明：** 
>调用加密接口时，存储加密结果的buffer，其缓冲区大小不能小于密钥N的位宽。

#### 注意事项<a name="ZH-CN_TOPIC_0000002466542276"></a>

支持PKCS\#1 v1.5 和 PKCS\#1 v2.1 OAEP两种模式，且用户标签数据（label）可选，该label仅在PKCS\#1 v2.1 OAEP模式下使用。PKCS\#1 v1.5为不安全算法，不建议使用。

### RSA签名验签<a name="ZH-CN_TOPIC_0000002466382640"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002466382572)**  

-   **[工作流程](#ZH-CN_TOPIC_0000002466382564)**  

-   **[注意事项](#ZH-CN_TOPIC_0000002466542292)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002466382572"></a>

RSA签名验签主要应用场景是私钥签名公钥验签。

#### 工作流程<a name="ZH-CN_TOPIC_0000002466382564"></a>

1.  私钥签名，调用uapi\_drv\_cipher\_pke\_rsa\_sign接口完成。
2.  公钥验签，调用uapi\_drv\_cipher\_pke\_rsa\_verify接口完成。

#### 注意事项<a name="ZH-CN_TOPIC_0000002466542292"></a>

支持PKCS\#1 v1.5和 PKCS\#1 v2.1 PSS两种模式的私钥签名和公钥验签。PKCS\#1 v1.5为不安全算法，不建议使用。

### SM2加解密<a name="ZH-CN_TOPIC_0000002466382688"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002499581521)**  

-   **[工作流程](#ZH-CN_TOPIC_0000002466542168)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002499581521"></a>

SM2加解密应用场景是公钥加密私钥解密。

#### 工作流程<a name="ZH-CN_TOPIC_0000002466542168"></a>

1.  公钥加密，调用uapi\_drv\_cipher\_pke\_sm2\_public\_encrypt接口完成。
2.  私钥解密，调用uapi\_drv\_cipher\_pke\_sm2\_private\_decrypt接口完成。

>![](public_sys-resources/icon-note.gif) **说明：** 
>加解密所需要的公私钥对可调用uapi\_drv\_cipher\_pke\_ecc\_gen\_key生成。

### SM2签名验签<a name="ZH-CN_TOPIC_0000002499581677"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002466542300)**  

-   **[工作流程](#ZH-CN_TOPIC_0000002499501633)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002466542300"></a>

SM2签名验签主要应用场景是私钥签名公钥验签。

#### 工作流程<a name="ZH-CN_TOPIC_0000002499501633"></a>

1.  生成消息对应的hash摘要，调用uapi\_drv\_cipher\_pke\_sm2\_dsa\_hash接口完成。
2.  私钥签名，调用uapi\_drv\_cipher\_pke\_sm2\_sign接口完成。
3.  公钥验签，调用uapi\_drv\_cipher\_pke\_sm2\_verify接口完成。

>![](public_sys-resources/icon-note.gif) **说明：** 
>签名验签所需要的公私钥对可调用uapi\_drv\_cipher\_pke\_ecc\_gen\_key接口生成，可以调用uapi\_drv\_cipher\_pke\_check\_dot\_on\_curve接口以确认公钥确实是曲线上的点。

### ECC签名验签<a name="ZH-CN_TOPIC_0000002499501485"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002466542308)**  

-   **[工作流程](#ZH-CN_TOPIC_0000002466542160)**  

-   **[注意事项](#ZH-CN_TOPIC_0000002499581645)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002466542308"></a>

ECC签名验签主要应用场景是私钥签名公钥验签。

#### 工作流程<a name="ZH-CN_TOPIC_0000002466542160"></a>

1.  生成消息对应的hash摘要，调用HASH相应接口完成。
2.  私钥签名，调用uapi\_drv\_cipher\_pke\_ecdsa\_sign接口完成。
3.  公钥验签，调用uapi\_drv\_cipher\_pke\_ecdsa\_verify接口完成。

>![](public_sys-resources/icon-note.gif) **说明：** 
>签名验签所需要的公私钥对可调用uapi\_drv\_cipher\_pke\_ecc\_gen\_key接口生成，可以调用uapi\_drv\_cipher\_pke\_check\_dot\_on\_curve接口以确认公钥确实是曲线上的点，签名需要先计算hash摘要，签名过程是对摘要进行签名。

#### 注意事项<a name="ZH-CN_TOPIC_0000002499581645"></a>

支持RFC 5639 - Brainpool P256/384/512r1、NIST FIPS 186 - 4 P192/224/256/384/521的私钥签名和公钥验签。

### EDDSA签名验签<a name="ZH-CN_TOPIC_0000002499581591"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002466382548)**  

-   **[工作流程](#ZH-CN_TOPIC_0000002499581545)**  

-   **[注意事项](#ZH-CN_TOPIC_0000002466382624)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002466382548"></a>

EDDSA签名验签主要应用场景是私钥签名公钥验签，是使用ed25519这种特殊曲线签名验签。

#### 工作流程<a name="ZH-CN_TOPIC_0000002499581545"></a>

1.  私钥签名，调用uapi\_drv\_cipher\_pke\_eddsa\_sign接口完成。
2.  公钥验签，调用uapi\_drv\_cipher\_pke\_eddsa\_verify接口完成。

>![](public_sys-resources/icon-note.gif) **说明：** 
>签名验签所需要的公私钥对可调用uapi\_drv\_cipher\_pke\_ecc\_gen\_key接口生成，可以调用uapi\_drv\_cipher\_pke\_check\_dot\_on\_curve接口以确认公钥确实是曲线上的点，EDDSA签名不需要提前计算hash，直接传入消息进行签名即可。

#### 注意事项<a name="ZH-CN_TOPIC_0000002466382624"></a>

支持RFC 8032 - ED25519的私钥签名和公钥验签。

### 大数运算<a name="ZH-CN_TOPIC_0000002466382604"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002466382580)**  

-   **[工作流程](#ZH-CN_TOPIC_0000002499501493)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002466382580"></a>

大数运算主要用于非对称加解密和签名验签的计算过程。

#### 工作流程<a name="ZH-CN_TOPIC_0000002499501493"></a>

直接调用大数计算接口完成相应类型的大数计算即可，例如模加可直接调用uapi\_drv\_cipher\_pke\_add\_mod接口完成。

支持的大数运算包括：

-   模加：uapi\_drv\_cipher\_pke\_add\_mod
-   模减：uapi\_drv\_cipher\_pke\_sub\_mod
-   模乘：uapi\_drv\_cipher\_pke\_mul\_mod
-   模逆：uapi\_drv\_cipher\_pke\_inv\_mod
-   取模：uapi\_drv\_cipher\_pke\_mod
-   大数乘：uapi\_drv\_cipher\_pke\_mul
-   模幂：uapi\_drv\_cipher\_pke\_exp\_mod
-   点乘：uapi\_drv\_cipher\_pke\_mul\_dot

### ECDH密钥协商<a name="ZH-CN_TOPIC_0000002499581613"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002499581629)**  

-   **[工作流程](#ZH-CN_TOPIC_0000002466542228)**  

-   **[注意事项](#ZH-CN_TOPIC_0000002499501533)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002499581629"></a>

密钥协商主要用于消息传递双方在生成各自的公私钥对之后进行密钥交换。

#### 工作流程<a name="ZH-CN_TOPIC_0000002466542228"></a>

1.  消息传递者a生成自己的公私钥对，调用uapi\_drv\_cipher\_pke\_ecc\_gen\_key接口完成。
2.  消息接收者b生成自己的公私钥对，调用uapi\_drv\_cipher\_pke\_ecc\_gen\_key接口完成。
3.  密钥协商，消息传递者a/消息接收者b使用自己的私钥和对方的公钥生成共享密钥，调用uapi\_drv\_cipher\_pke\_ecc\_gen\_ecdh\_key接口完成。

>![](public_sys-resources/icon-note.gif) **说明：** 
>-   对于给定的私钥uapi\_drv\_cipher\_pke\_ecc\_gen\_key生成的公钥总是固定的；对于未给定私钥的情况下，uapi\_drv\_cipher\_pke\_ecc\_gen\_key生成的公私钥对是随机的。
>-   对于RFC 5639 - Brainpool P256/384/512r1、NIST FIPS 186 - 4 P192/224/256/384/521曲线，在接收到对方公钥调用uapi\_drv\_cipher\_pke\_ecc\_gen\_ecdh\_key接口进行密钥协商前，需要先调用uapi\_drv\_cipher\_pke\_check\_dot\_on\_curve接口以确认公钥的合法性。

#### 注意事项<a name="ZH-CN_TOPIC_0000002499501533"></a>

-   支持RFC 5639 - Brainpool P256/384/512r1、NIST FIPS 186 - 4 P192/224/256/384/521、RFC 7748 - Curve448/Curve25519、RFC 8032 - ED25519（即EDDSA）和SM2的公私钥对生成，及由给定的私钥生成其对应的公钥。
-   支持RFC 5639 - Brainpool P256/384/512r1、NIST FIPS 186 - 4 P192/224/256/384/521、RFC 7748 - Curve448/Curve25519和SM2的密钥交换（ECDH），低于256bit的ECDH及NIST FIPS对应的曲线为不安全算法，不建议使用。

### DH密钥协商<a name="ZH-CN_TOPIC_0000002466382588"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002499581621)**  

-   **[工作流程](#ZH-CN_TOPIC_0000002499581585)**  

-   **[注意事项](#ZH-CN_TOPIC_0000002499501549)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002499581621"></a>

密钥协商主要用于消息传递双方在生成各自的公私钥对之后进行密钥交换，DH密钥协商用于非曲线类型。

#### 工作流程<a name="ZH-CN_TOPIC_0000002499581585"></a>

1.  消息传递者a生成自己的公私钥对，调用uapi\_drv\_cipher\_pke\_dh\_gen\_key接口完成。
2.  消息接收者b生成自己的公私钥对，调用uapi\_drv\_cipher\_pke\_dh\_gen\_key接口完成。
3.  密钥协商，消息传递者a/消息接收者b使用自己的私钥和对方的公钥生成共享密钥，调用uapi\_drv\_cipher\_pke\_dh\_compute\_key接口完成。

>![](public_sys-resources/icon-note.gif) **说明：** 
>对于给定的私钥uapi\_drv\_cipher\_pke\_dh\_gen\_key生成的公钥总是固定的；对于未给定私钥的情况下，uapi\_drv\_cipher\_pke\_dh\_gen\_key生成的公私钥对是随机的。

#### 注意事项<a name="ZH-CN_TOPIC_0000002499501549"></a>

支持192/224/256/384/512/521/1024/2048/3072/4096 bit DH密钥协商，低于3072bit的DH为不安全算法，不建议使用。

### TRNG随机数获取<a name="ZH-CN_TOPIC_0000002466382596"></a>

-   **[场景说明](#ZH-CN_TOPIC_0000002499501617)**  

-   **[工作流程](#ZH-CN_TOPIC_0000002499501589)**  

#### 场景说明<a name="ZH-CN_TOPIC_0000002499501617"></a>

主要用于获取真随机数。

#### 工作流程<a name="ZH-CN_TOPIC_0000002499501589"></a>

直接调用随机数获取接口完成真随机数获取。uapi\_drv\_cipher\_trng\_get\_random用于获取4字节的随机数，uapi\_drv\_cipher\_trng\_get\_random\_bytes用于获取指定字节长度的随机数。

### 安全相关eFuse烧写建议<a name="ZH-CN_TOPIC_0000002466542252"></a>

安全特性相关的eFuse位较多，涉及到安全特性的正确使用，此处将逐一列出相关eFuse及对应使用场景和烧写建议供参考，各项目根据具体需求及实际eFuse排布表单进行合理配置。[表1](#zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369__table1523256143518)是不同场景下使用的eFuse，及其建议烧写阶段和建议烧写值。

**表 1**  安全特性eFuse使用场景及烧写建议

<a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369__table1523256143518"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_row1130mcpsimp"><th class="cellrowborder" valign="top" width="16.48%" id="mcps1.2.7.1.1"><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1132mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1132mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1132mcpsimp"></a>使用场景</p>
</th>
<th class="cellrowborder" valign="top" width="16.48%" id="mcps1.2.7.1.2"><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1134mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1134mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1134mcpsimp"></a>关键项</p>
</th>
<th class="cellrowborder" valign="top" width="7.5200000000000005%" id="mcps1.2.7.1.3"><p id="zh-cn_topic_0000002446160701_p62301522111917"><a name="zh-cn_topic_0000002446160701_p62301522111917"></a><a name="zh-cn_topic_0000002446160701_p62301522111917"></a>bit长度</p>
</th>
<th class="cellrowborder" valign="top" width="10.23%" id="mcps1.2.7.1.4"><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1136mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1136mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1136mcpsimp"></a>烧写阶段</p>
</th>
<th class="cellrowborder" valign="top" width="16.259999999999998%" id="mcps1.2.7.1.5"><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1138mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1138mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1138mcpsimp"></a>烧写完是否需要lock</p>
</th>
<th class="cellrowborder" valign="top" width="33.03%" id="mcps1.2.7.1.6"><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1140mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1140mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1140mcpsimp"></a>烧写方式及烧写值</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_row1142mcpsimp"><td class="cellrowborder" rowspan="5" valign="top" width="16.48%" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1144mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1144mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1144mcpsimp"></a>安全启动</p>
</td>
<td class="cellrowborder" valign="top" width="16.48%" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1146mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1146mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1146mcpsimp"></a>secure_boot_enable_efuse</p>
</td>
<td class="cellrowborder" valign="top" width="7.5200000000000005%" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002446160701_p1230162213194"><a name="zh-cn_topic_0000002446160701_p1230162213194"></a><a name="zh-cn_topic_0000002446160701_p1230162213194"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="10.23%" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1148mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1148mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1148mcpsimp"></a>产线</p>
</td>
<td class="cellrowborder" valign="top" width="16.259999999999998%" headers="mcps1.2.7.1.5 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1150mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1150mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1150mcpsimp"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="33.03%" headers="mcps1.2.7.1.6 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1152mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1152mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1152mcpsimp"></a>Burntool烧写工具，根据是否开启安全启动功能确定。</p>
<p id="zh-cn_topic_0000002446160701_p135847673517"><a name="zh-cn_topic_0000002446160701_p135847673517"></a><a name="zh-cn_topic_0000002446160701_p135847673517"></a>0xA：未使能安全启动；</p>
<p id="zh-cn_topic_0000002446160701_p126171121143519"><a name="zh-cn_topic_0000002446160701_p126171121143519"></a><a name="zh-cn_topic_0000002446160701_p126171121143519"></a>others：使能安全启动。</p>
<p id="zh-cn_topic_0000002446160701_p129473773714"><a name="zh-cn_topic_0000002446160701_p129473773714"></a><a name="zh-cn_topic_0000002446160701_p129473773714"></a>建议使能安全启动。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_row1153mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1155mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1155mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1155mcpsimp"></a>tee_ver</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002446160701_p202301022151913"><a name="zh-cn_topic_0000002446160701_p202301022151913"></a><a name="zh-cn_topic_0000002446160701_p202301022151913"></a>16</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1157mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1157mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1157mcpsimp"></a>产线</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1159mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1159mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1159mcpsimp"></a>否</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.5 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1161mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1161mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1161mcpsimp"></a>若支持防回滚功能，镜像升级并启动成功后会根据镜像版本号信息自动烧写。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_row1162mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1164mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1164mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1164mcpsimp"></a>Flashboot_ver</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002446160701_p182301322181920"><a name="zh-cn_topic_0000002446160701_p182301322181920"></a><a name="zh-cn_topic_0000002446160701_p182301322181920"></a>8</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1166mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1166mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1166mcpsimp"></a>产线</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1168mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1168mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1168mcpsimp"></a>否</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.5 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1170mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1170mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1170mcpsimp"></a>若支持防回滚功能，镜像升级并启动成功后会根据镜像版本号信息自动烧写。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_row1180mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1182mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1182mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1182mcpsimp"></a>hash_oem_root_pub_key</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002446160701_p20230322111915"><a name="zh-cn_topic_0000002446160701_p20230322111915"></a><a name="zh-cn_topic_0000002446160701_p20230322111915"></a>256</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1184mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1184mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1184mcpsimp"></a>产线</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1186mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1186mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1186mcpsimp"></a>是</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.5 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1188mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1188mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1188mcpsimp"></a>Burntool烧写工具，根公钥镜像HASH值。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_row1189mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1191mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1191mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1191mcpsimp"></a>MSID</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002446160701_p10230142211198"><a name="zh-cn_topic_0000002446160701_p10230142211198"></a><a name="zh-cn_topic_0000002446160701_p10230142211198"></a>32</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1193mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1193mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1193mcpsimp"></a>产线</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1195mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1195mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1195mcpsimp"></a>是</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.5 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1197mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1197mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1197mcpsimp"></a>Burntool烧写工具，市场ID。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_row1198mcpsimp"><td class="cellrowborder" rowspan="3" valign="top" width="16.48%" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1200mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1200mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1200mcpsimp"></a>安全驱动</p>
</td>
<td class="cellrowborder" valign="top" width="16.48%" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1202mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1202mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1202mcpsimp"></a>otp_crc_rd_disable（KM）</p>
</td>
<td class="cellrowborder" valign="top" width="7.5200000000000005%" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002446160701_p1023072219196"><a name="zh-cn_topic_0000002446160701_p1023072219196"></a><a name="zh-cn_topic_0000002446160701_p1023072219196"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="10.23%" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1204mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1204mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1204mcpsimp"></a>产线</p>
</td>
<td class="cellrowborder" valign="top" width="16.259999999999998%" headers="mcps1.2.7.1.5 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1206mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1206mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1206mcpsimp"></a>否</p>
</td>
<td class="cellrowborder" valign="top" width="33.03%" headers="mcps1.2.7.1.6 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1208mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1208mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1208mcpsimp"></a>Burntool烧写工具，根据是否保留CRC debug功能确定烧写值。</p>
<p id="zh-cn_topic_0000002446160701_p1889421615317"><a name="zh-cn_topic_0000002446160701_p1889421615317"></a><a name="zh-cn_topic_0000002446160701_p1889421615317"></a>0：保留CRC debug；</p>
<p id="zh-cn_topic_0000002446160701_p9961102773115"><a name="zh-cn_topic_0000002446160701_p9961102773115"></a><a name="zh-cn_topic_0000002446160701_p9961102773115"></a>1：关闭CRC debug。</p>
<p id="zh-cn_topic_0000002446160701_p95398498374"><a name="zh-cn_topic_0000002446160701_p95398498374"></a><a name="zh-cn_topic_0000002446160701_p95398498374"></a>建议关闭。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_row1218mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1220mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1220mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1220mcpsimp"></a>rkp_deob_alg_sel</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002446160701_p1230022141920"><a name="zh-cn_topic_0000002446160701_p1230022141920"></a><a name="zh-cn_topic_0000002446160701_p1230022141920"></a>2</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1222mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1222mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1222mcpsimp"></a>产线</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1224mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1224mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1224mcpsimp"></a>是</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.5 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1226mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1226mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1226mcpsimp"></a>Burntool烧写工具，配置DEOB算法。</p>
<p id="zh-cn_topic_0000002446160701_p1238216361304"><a name="zh-cn_topic_0000002446160701_p1238216361304"></a><a name="zh-cn_topic_0000002446160701_p1238216361304"></a>2'b00：由软件配置deob算法，可选AES或者SM4；</p>
<p id="zh-cn_topic_0000002446160701_p9382113613014"><a name="zh-cn_topic_0000002446160701_p9382113613014"></a><a name="zh-cn_topic_0000002446160701_p9382113613014"></a>2'b01：固定选择AES作为deob算法；</p>
<p id="zh-cn_topic_0000002446160701_p19382836133015"><a name="zh-cn_topic_0000002446160701_p19382836133015"></a><a name="zh-cn_topic_0000002446160701_p19382836133015"></a>2'b10/2'b11：固定选择SM4作为deob算法。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_row1227mcpsimp"><td class="cellrowborder" valign="top" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1229mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1229mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1229mcpsimp"></a>sm_disable</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002446160701_p1123022211199"><a name="zh-cn_topic_0000002446160701_p1123022211199"></a><a name="zh-cn_topic_0000002446160701_p1123022211199"></a>1</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1231mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1231mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1231mcpsimp"></a>产线</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1233mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1233mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1233mcpsimp"></a>否</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.7.1.5 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1235mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1235mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1235mcpsimp"></a>Burntool烧写工具，根据是否关闭国密来确定。</p>
<p id="zh-cn_topic_0000002446160701_p553613412345"><a name="zh-cn_topic_0000002446160701_p553613412345"></a><a name="zh-cn_topic_0000002446160701_p553613412345"></a>0：允许使用国密；</p>
<p id="zh-cn_topic_0000002446160701_p26365113315"><a name="zh-cn_topic_0000002446160701_p26365113315"></a><a name="zh-cn_topic_0000002446160701_p26365113315"></a>1：禁止使用国密。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_row1236mcpsimp"><td class="cellrowborder" valign="top" width="16.48%" headers="mcps1.2.7.1.1 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1238mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1238mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1238mcpsimp"></a>AES/SM4加解密，HMAC，安全存储</p>
</td>
<td class="cellrowborder" valign="top" width="16.48%" headers="mcps1.2.7.1.2 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1240mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1240mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1240mcpsimp"></a>MRK (OEM)</p>
</td>
<td class="cellrowborder" valign="top" width="7.5200000000000005%" headers="mcps1.2.7.1.3 "><p id="zh-cn_topic_0000002446160701_p122301622171912"><a name="zh-cn_topic_0000002446160701_p122301622171912"></a><a name="zh-cn_topic_0000002446160701_p122301622171912"></a>128</p>
</td>
<td class="cellrowborder" valign="top" width="10.23%" headers="mcps1.2.7.1.4 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1242mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1242mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1242mcpsimp"></a>产线</p>
</td>
<td class="cellrowborder" valign="top" width="16.259999999999998%" headers="mcps1.2.7.1.5 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1244mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1244mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1244mcpsimp"></a>是</p>
</td>
<td class="cellrowborder" valign="top" width="33.03%" headers="mcps1.2.7.1.6 "><p id="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1246mcpsimp"><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1246mcpsimp"></a><a name="zh-cn_topic_0000002446160701_zh-cn_topic_0000002148438369_p1246mcpsimp"></a>OEM Global根密钥，Burntool烧写工具，128bit随机值。</p>
</td>
</tr>
</tbody>
</table>

# 错误码<a name="ZH-CN_TOPIC_0000002466542152"></a>

>![](public_sys-resources/icon-note.gif) **说明：** 
>本章描述了CIPHER DRIVER的错误码，便于开发人员理解与定位所发生的错误。

-   **[CIPHER DRIVER 错误码结构与说明](#ZH-CN_TOPIC_0000002499581553)**  

## CIPHER DRIVER 错误码结构与说明<a name="ZH-CN_TOPIC_0000002499581553"></a>

Cipher驱动返回值通常有成功和失败两种类型，如[表1](#zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936__d0e16563)所示。

**表 1**  Cipher驱动返回值

<a name="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936__d0e16563"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_row652mcpsimp"><th class="cellrowborder" valign="top" width="48%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p654mcpsimp"><a name="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p654mcpsimp"></a><a name="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p654mcpsimp"></a>成员名称</p>
</th>
<th class="cellrowborder" valign="top" width="52%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p656mcpsimp"><a name="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p656mcpsimp"></a><a name="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p656mcpsimp"></a>枚举值</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_row658mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p660mcpsimp"><a name="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p660mcpsimp"></a><a name="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p660mcpsimp"></a>CRYPTO_SUCCESS</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p662mcpsimp"><a name="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p662mcpsimp"></a><a name="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p662mcpsimp"></a>0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_row663mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p665mcpsimp"><a name="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p665mcpsimp"></a><a name="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p665mcpsimp"></a>CRYPTO_FAILURE</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p667mcpsimp"><a name="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p667mcpsimp"></a><a name="zh-cn_topic_0000002412561460_zh-cn_topic_0000002112799936_p667mcpsimp"></a>0xFFFFFFFF</p>
</td>
</tr>
</tbody>
</table>

为了通过错误码更加清晰的标识错误发生的具体信息，采用了层级结构信息拼接。其结构为：

Env\(4 bits\) | Layer\(4 bits\) | Modules\(4 bits\) | Reserved\(12 bits\) | Error Code\(8 bits\)

-   **[ENV](#ZH-CN_TOPIC_0000002499501625)**  

-   **[LAYER](#ZH-CN_TOPIC_0000002499501593)**  

-   **[MODULE](#ZH-CN_TOPIC_0000002466542284)**  

-   **[错误类型](#ZH-CN_TOPIC_0000002466382556)**  

### ENV<a name="ZH-CN_TOPIC_0000002499501625"></a>

【描述】

错误码发生的操作系统环境。

【定义】

```
#define ERROR_ENV_LINUX         0x1 
#define ERROR_ENV_ITRUSTEE      0x2 
#define ERROR_ENV_OPTEE         0x3 
#define ERROR_ENV_LITEOS        0x4 
#define ERROR_ENV_SELITEOS      0x5 
#define ERROR_ENV_NOOS          0x6 
#define ERROR_ENV_FREERTOS      0x7 
#define ERROR_ENV_ALIOS         0x8 

```

【成员】

<a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_table1292mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_row1297mcpsimp"><th class="cellrowborder" valign="top" width="48%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1299mcpsimp"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1299mcpsimp"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1299mcpsimp"></a>成员名称</p>
</th>
<th class="cellrowborder" valign="top" width="52%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1301mcpsimp"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1301mcpsimp"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1301mcpsimp"></a>枚举值</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_row1303mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1305mcpsimp"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1305mcpsimp"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1305mcpsimp"></a>ERROR_ENV_LINUX</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1307mcpsimp"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1307mcpsimp"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1307mcpsimp"></a>LINUX</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_row1308mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1310mcpsimp"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1310mcpsimp"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1310mcpsimp"></a>ERROR_ENV_ITRUSTEE</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1312mcpsimp"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1312mcpsimp"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1312mcpsimp"></a>ITRUSTEE</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_row1313mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1315mcpsimp"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1315mcpsimp"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1315mcpsimp"></a>ERROR_ENV_OPTEE</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1317mcpsimp"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1317mcpsimp"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1317mcpsimp"></a>OPTEE</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_row1318mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1320mcpsimp"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1320mcpsimp"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1320mcpsimp"></a>ERROR_ENV_LITEOS</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1322mcpsimp"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1322mcpsimp"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1322mcpsimp"></a>LITEOS</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_row1323mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1325mcpsimp"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1325mcpsimp"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1325mcpsimp"></a>ERROR_ENV_SELITEOS</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1327mcpsimp"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1327mcpsimp"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1327mcpsimp"></a>SELITEOS</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_row1328mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1330mcpsimp"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1330mcpsimp"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1330mcpsimp"></a>ERROR_ENV_NOOS</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1332mcpsimp"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1332mcpsimp"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1332mcpsimp"></a>无OS环境</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_row169672387436"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p952121017321"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p952121017321"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p952121017321"></a>ERROR_ENV_FREERTOS</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p13521010123215"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p13521010123215"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p13521010123215"></a>FreeRTOS</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_row1020810436438"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p773484913212"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p773484913212"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p773484913212"></a>ERROR_ENV_ALIOS</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1673412498325"><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1673412498325"></a><a name="zh-cn_topic_0000002412401580_zh-cn_topic_0000002112959668_p1673412498325"></a>阿里OS</p>
</td>
</tr>
</tbody>
</table>

### LAYER<a name="ZH-CN_TOPIC_0000002499501593"></a>

【描述】

错误码发生的代码层级。

【定义】

```
enum { 
    ERROR_LAYER_UAPI = 0x1, 
    ERROR_LAYER_DISPATCH, 
    ERROR_LAYER_KAPI, 
    ERROR_LAYER_DRV, 
    ERROR_LAYER_HAL, 
};
```

【成员】

<a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_table1043mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_row1048mcpsimp"><th class="cellrowborder" valign="top" width="48%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1050mcpsimp"><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1050mcpsimp"></a><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1050mcpsimp"></a>成员名称</p>
</th>
<th class="cellrowborder" valign="top" width="52%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1052mcpsimp"><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1052mcpsimp"></a><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1052mcpsimp"></a>枚举值</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_row1054mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1056mcpsimp"><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1056mcpsimp"></a><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1056mcpsimp"></a>ERROR_LAYER_UAPI</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1058mcpsimp"><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1058mcpsimp"></a><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1058mcpsimp"></a>用户接口层</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_row1059mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1061mcpsimp"><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1061mcpsimp"></a><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1061mcpsimp"></a>ERROR_LAYER_DISPATCH</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1063mcpsimp"><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1063mcpsimp"></a><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1063mcpsimp"></a>指令分发层</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_row1064mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1066mcpsimp"><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1066mcpsimp"></a><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1066mcpsimp"></a>ERROR_LAYER_KAPI</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1068mcpsimp"><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1068mcpsimp"></a><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1068mcpsimp"></a>内核接口层</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_row1069mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1071mcpsimp"><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1071mcpsimp"></a><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1071mcpsimp"></a>ERROR_LAYER_DRV</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1073mcpsimp"><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1073mcpsimp"></a><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1073mcpsimp"></a>驱动逻辑层</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_row1074mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1076mcpsimp"><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1076mcpsimp"></a><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1076mcpsimp"></a>ERROR_LAYER_HAL</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1078mcpsimp"><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1078mcpsimp"></a><a name="zh-cn_topic_0000002446160709_zh-cn_topic_0000002148558897_p1078mcpsimp"></a>硬件适配层</p>
</td>
</tr>
</tbody>
</table>

### MODULE<a name="ZH-CN_TOPIC_0000002466542284"></a>

【描述】

错误码发生的CIPHER模块。

【定义】

```
enum { 
    ERROR_MODULE_SYMC = 0x1, 
    ERROR_MODULE_HASH, 
    ERROR_MODULE_PKE, 
    ERROR_MODULE_TRNG, 
    ERROR_MODULE_KM, 
    ERROR_MODULE_OTP,
    ERROR_MODULE_OTHER 
};
```

【成员】

<a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_table228mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_row233mcpsimp"><th class="cellrowborder" valign="top" width="48%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p235mcpsimp"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p235mcpsimp"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p235mcpsimp"></a>成员名称</p>
</th>
<th class="cellrowborder" valign="top" width="52%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p237mcpsimp"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p237mcpsimp"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p237mcpsimp"></a>枚举值</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_row239mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p241mcpsimp"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p241mcpsimp"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p241mcpsimp"></a>ERROR_MODULE_SYMC</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p243mcpsimp"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p243mcpsimp"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p243mcpsimp"></a>对称加解密模块</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_row244mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p246mcpsimp"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p246mcpsimp"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p246mcpsimp"></a>ERROR_MODULE_HASH</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p248mcpsimp"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p248mcpsimp"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p248mcpsimp"></a>摘要算法模块</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_row249mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p251mcpsimp"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p251mcpsimp"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p251mcpsimp"></a>ERROR_MODULE_PKE</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p253mcpsimp"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p253mcpsimp"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p253mcpsimp"></a>非对称加解密模块</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_row254mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p256mcpsimp"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p256mcpsimp"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p256mcpsimp"></a>ERROR_MODULE_TRNG</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p258mcpsimp"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p258mcpsimp"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p258mcpsimp"></a>随机数模块</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_row1832012944419"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p19118537173412"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p19118537173412"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p19118537173412"></a>ERROR_MODULE_KM</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p71181537113412"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p71181537113412"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p71181537113412"></a>密钥管理模块</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_row137256326441"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p9634154133417"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p9634154133417"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p9634154133417"></a>ERROR_MODULE_OTP</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p96344412344"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p96344412344"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p96344412344"></a>otp烧写模块（当前产品不涉及）</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_row259mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p261mcpsimp"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p261mcpsimp"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p261mcpsimp"></a>ERROR_MODULE_OTHER</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p263mcpsimp"><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p263mcpsimp"></a><a name="zh-cn_topic_0000002446040633_zh-cn_topic_0000002148438345_p263mcpsimp"></a>其他模块</p>
</td>
</tr>
</tbody>
</table>

### 错误类型<a name="ZH-CN_TOPIC_0000002466382556"></a>

【描述】

错误码发生的具体错误类型。

【定义】

```
/* Common Error Code. 0x01 ~ 0x3F. */
ERROR_INVALID_PARAM = 0x1,      /* return when the input param's value is not in the valid range. */
ERROR_PARAM_IS_NULL,            /* return when the input param is NULL and required not NULL. */
ERROR_NOT_INIT,                 /* return when call other functions before call init function. */
ERROR_UNSUPPORT,                /* return when some configuration is unsupport. */
ERROR_UNEXPECTED,               /* reture when unexpected error occurs. */
ERROR_TRY_TIMES,                /* return when the number of try attempts failed. */
ERROR_CHN_BUSY,                 /* return when try to create one channel but all channels are busy. */
ERROR_CTX_CLOSED,               /* return when using one ctx to do something but has been closed. */
ERROR_NOT_SET_CONFIG,           /* return when not set_config but need for symc. */
ERROR_NOT_ATTACHED,             /* return when not attach but need for symc. */
ERROR_NOT_SET_SESSION_KEY,      /* return when not set_session_key but need for klad. */
ERROR_NOT_MAC_START,            /* return when not mac_start but need for symc. */
ERROR_INVALID_HANDLE,           /* return when pass one invalid handle. */
ERROR_GET_PHYS_ADDR,            /* return when transfer from virt_addr to phys_addr failed. */
ERROR_SYMC_LEN_NOT_ALIGNED,     /* return when length isn't aligned to 16-Byte except CTR/CCM/GCM.  */
ERROR_SYMC_ADDR_NOT_ALIGNED,    /* return when the phys_addr writing to register is not aligned to 4-Byte. */
ERROR_PKE_RSA_SAME_DATA,        /* return when rsa exp_mod, the input is equal to output. */
ERROR_PKE_RSA_CRYPTO_V15_CHECK, /* return when rsa crypto v15 padding check failed. */
ERROR_PKE_RSA_CRYPTO_OAEP_CHECK,    /* return when rsa crypto oaep padding check failed. */
ERROR_PKE_RSA_VERIFY_V15_CHECK,     /* return when rsa verify v15 padding check failed. */
ERROR_PKE_RSA_VERIFY_PSS_CHECK,     /* return when rsa verify pss padding check failed. */
ERROR_PKE_RSA_GEN_KEY,          /* return when rsa generate key failed. */
ERROR_PKE_SM2_DECRYPT,          /* return when sm2 decrypt failed. */
ERROR_ECDSA_SIGN_R_IS_ZERO,     /* return when ecdsa sign, the sig_r is zero. */
ERROR_ECDSA_SIGN_S_IS_ZERO,     /* return when ecdsa sign, the sig_s is zero. */
ERROR_PKE_ECDSA_VERIFY_CHECK,   /* return when ecdsa verify check failed. */
ERROR_OTP_ADDR_NOT_ALIGNED,     /* return when otp address is not aligned to 4. */
ERROR_HASH_ADDR_NOT_ALIGNED,    /* return when hash data's address is not aligned */
ERROR_HASH_LEN_NOT_ALIGNED,     /* return when hash length is not aligned to block size. */
ERROR_PKE_SECURITY_STRENGTH_NOT_MATCH,  /* return when the security strength is not match. */
/* Outer's Error Code. 0x40 ~ 0x5F. */
ERROR_MEMCPY_S      = 0x40,     /* return when call memcpy_s failed. */
ERROR_MALLOC,                   /* return when call xxx_malloc failed. */
ERROR_MUTEX_INIT,               /* return when call xxx_mutex_init failed. */
ERROR_MUTEX_LOCK,               /* return when call xxx_lock failed. */
ERROR_MEMSET_S,                 /* return when call memset_s failed. */
/* Specific Error Code for UAPI. 0x60 ~ 0x6F. */
ERROR_DEV_OPEN_FAILED  = 0x60,  /* return when open dev failed. */
ERROR_COUNT_OVERFLOW,           /* return when call init too many times. */
/* Specific Error Code for Dispatch. 0x70 ~ 0x7F. */
ERROR_CMD_DISMATCH  = 0x70,     /* return when cmd is dismatched. */
ERROR_COPY_FROM_USER,           /* return when call copy_from_user failed. */
ERROR_COPY_TO_USER,             /* return when call copy_to_user failed. */
ERROR_MEM_HANDLE_GET,           /* return when parse user's mem handle to kernel's mem handle failed. */
ERROR_GET_OWNER,                /* return when call crypto_get_owner failed. */
/* Specific Error Code for KAPI. 0x80 ~ 0x8F. */
ERROR_PROCESS_NOT_INIT = 0x80,  /* return when one process not call kapi_xxx_init first. */
ERROR_MAX_PROCESS,              /* return when process's num is over the limit. */
ERROR_MEMORY_ACCESS,            /* return when access the memory that does not belong to itself.  */
ERROR_INVALID_PROCESS,          /* return when the process accesses resources of other processes. */
/* Specific Error Code for DRV. 0x90 ~ 0x9F. */
/* Specific Error Code for HAL. 0xA0 ~ 0xAF. */
ERROR_HASH_LOGIC    = 0xA0,     /* return when hash logic's error occurs. */
ERROR_PKE_LOGIC,                /* return when pke logic's error occurs. */
ERROR_INVALID_CPU_TYPE,         /* return when logic get the invalid cpu type. */
ERROR_INVALID_REGISTER_VALUE,   /* return when value in register is invalid. */
ERROR_INVALID_PHYS_ADDR,        /* return when phys_addr is invalid. */
ERROR_OTP_PERMISSION,           /* return when otp access is forbidden. */
ERROR_KM_LOGIC,                  /* return when km logic's error occurs. */
ERROR_KM_KL_LOGIC,              /* return when km HKL logic's error occurs. */
ERROR_KM_KC_LOGIC,              /* return when km KEY CTRL logic's error occurs. */
ERROR_KLAD_ROOTKEY_NOT_READY,   /* return when klad rootkey is not ready. */
ERROR_KM_RKP_LOGIC,             /* return when km RKP logic's error occurs. */
/* Specific Error Code for Timeout. 0xB0 ~ 0xBF. */
ERROR_GET_TRNG_TIMEOUT = 0xB0,  /* return when logic get rnd timeout. */
ERROR_HASH_CLEAR_CHN_TIMEOUT,   /* return when clear hash channel timeout. */
ERROR_HASH_CALC_TIMEOUT,        /* return when hash calculation timeout. */
ERROR_SYMC_CLEAR_CHN_TIMEOUT,   /* return when clear symc channel timeout. */
ERROR_SYMC_CALC_TIMEOUT,        /* return when symc crypto timeout. */
ERROR_SYMC_GET_TAG_TIMEOUT,     /* return when symc get tag timeout. */
ERROR_PKE_LOCK_TIMEOUT,         /* return when pke lock timeout. */
ERROR_PKE_WAIT_DONE_TIMEOUT,    /* return when pke wait done timeout. */
ERROR_PKE_ROBUST_WARNING,    /* return when pke wait done timeout. */
ERROR_RKP_LOCK_TIMEOUT,      /* return when rkp lock timeout. */
ERROR_RKP_CALC_TIMEOUT,      /* return when rkp calculate timeout. */
ERROR_KLAD_COM_ROUTE_TIMEOUT,   /* return when klad com route timeout. */
ERROR_OTP_TIMEOUT,          /* return when otp wait timeout. */
ERROR_KEYSLOT_TIMEOUT,       /* return when keyslot wait timeout. */
ERROR_KEYSLOT_LOCK,          /* return when keyslot lock or unlock fail. */
ERROR_KLAD_TIMEOUT,          /* return when klad wait timeout. */
ERROR_KLAD_LOCK,             /* return when klad lock or unlock fail. */
ERROR_HASH_LOCK_TIMEOUT,     /* return when hash lock wait timeout. */
ERROR_PKE_ALG_TIMEOUT,     /* return when pke algorithm timeout. */
```

【成员】

<a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_table16718611064"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1269216339614"><th class="cellrowborder" valign="top" width="14.371437143714372%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p369212337618"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p369212337618"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p369212337618"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="37.28372837283728%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p86921433464"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p86921433464"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p86921433464"></a>错误码</p>
</th>
<th class="cellrowborder" valign="top" width="48.344834483448345%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p12692533562"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p12692533562"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p12692533562"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1290318119615"><td class="cellrowborder" rowspan="30" valign="top" width="14.371437143714372%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690313111369"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690313111369"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690313111369"></a>COMMON</p>
</td>
<td class="cellrowborder" valign="top" width="37.28372837283728%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p17903141117611"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p17903141117611"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p17903141117611"></a>ERROR_INVALID_PARAM</p>
</td>
<td class="cellrowborder" valign="top" width="48.344834483448345%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p139031011869"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p139031011869"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p139031011869"></a>输入参数在无效范围</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1790361119617"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p14903311163"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p14903311163"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p14903311163"></a>ERROR_PARAM_IS_NULL</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p18903511867"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p18903511867"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p18903511867"></a>输入非法空指针参数</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row0903011564"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p4903411769"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p4903411769"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p4903411769"></a>ERROR_NOT_INIT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p69039111261"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p69039111261"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p69039111261"></a>未调用初始化函数</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row13903171110612"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1890391116618"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1890391116618"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1890391116618"></a>ERROR_UNSUPPORT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p29034111169"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p29034111169"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p29034111169"></a>算法或配置未支持</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row99032112061"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1990391118615"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1990391118615"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1990391118615"></a>ERROR_UNEXPECTED</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189039114619"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189039114619"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189039114619"></a>预期之外的错误</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row390316119618"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p7903121112612"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p7903121112612"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p7903121112612"></a>ERROR_TRY_TIMES</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1590391114617"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1590391114617"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1590391114617"></a>TRY指定次数之后仍然失败</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row139035111161"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1590317111361"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1590317111361"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1590317111361"></a>ERROR_CHN_BUSY</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p690318111764"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p690318111764"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p690318111764"></a>通道均被占用时尝试创建通道</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1690331111611"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p109038111964"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p109038111964"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p109038111964"></a>ERROR_CTX_CLOSED</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p13903191115612"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p13903191115612"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p13903191115612"></a>尝试使用已关闭的CTX</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row790316111618"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p149035117616"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p149035117616"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p149035117616"></a>ERROR_NOT_SET_CONFIG</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2903131116612"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2903131116612"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2903131116612"></a>SYMC未调用set_config函数</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1890314116613"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p14903911465"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p14903911465"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p14903911465"></a>ERROR_NOT_ATTACHED</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p109039118618"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p109039118618"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p109039118618"></a>SYMC未绑定KEYSLOT</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1890316119618"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190317111869"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190317111869"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190317111869"></a>ERROR_NOT_SET_SESSION_KEY</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p159039115618"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p159039115618"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p159039115618"></a>kald派生key之前未配置seeesion_key</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row89038111768"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790318111069"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790318111069"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790318111069"></a>ERROR_NOT_MAC_START</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p79034116615"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p79034116615"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p79034116615"></a>SYMC未调用mac_start函数</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row8903131118620"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190310111265"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190310111265"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190310111265"></a>ERROR_INVALID_HANDLE</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190320111769"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190320111769"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190320111769"></a>传入了无效的handle</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row390310117616"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p14903111666"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p14903111666"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p14903111666"></a>ERROR_GET_PHYS_ADDR</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390312111613"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390312111613"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390312111613"></a>获取物理地址失败</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row139036111964"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1290413111464"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1290413111464"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1290413111464"></a>ERROR_SYMC_LEN_NOT_ALIGNED</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119045111268"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119045111268"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119045111268"></a>SYMC长度未对其到16Byte除了（CTR/CCM/GCM）模式</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1390401113619"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p149043111963"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p149043111963"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p149043111963"></a>ERROR_SYMC_ADDR_NOT_ALIGNED</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690451119616"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690451119616"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690451119616"></a>SYMC写到寄存器的物理地址没有按照4Byte对齐</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row690415111961"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p9904911661"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p9904911661"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p9904911661"></a>ERROR_PKE_RSA_SAME_DATA</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190419111668"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190419111668"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190419111668"></a>RSA模密运算时，输入和输出相等</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row7904411964"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p139042111663"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p139042111663"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p139042111663"></a>ERROR_PKE_RSA_CRYPTO_V15_CHECK</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119042111167"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119042111167"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119042111167"></a>RSA PKCSV15解密 Padding校验失败</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1990417117616"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790401120615"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790401120615"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790401120615"></a>ERROR_PKE_RSA_CRYPTO_OAEP_CHECK</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p19044111861"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p19044111861"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p19044111861"></a>RSA OAEP 解密Padding校验失败</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1904201113615"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190417117613"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190417117613"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190417117613"></a>ERROR_PKE_RSA_VERIFY_V15_CHECK</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p09042011368"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p09042011368"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p09042011368"></a>RSA PKCSV15验签 Padding校验失败</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row119045114615"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p0904201112613"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p0904201112613"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p0904201112613"></a>ERROR_PKE_RSA_VERIFY_PSS_CHECK</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190411111610"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190411111610"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190411111610"></a>RSA PSS验签Padding校验失败</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1290410111768"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390419116619"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390419116619"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390419116619"></a>ERROR_PKE_RSA_GEN_KEY</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39048118613"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39048118613"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39048118613"></a>RSA密钥生成失败，该功能暂不支持</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row129049119614"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p590471115615"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p590471115615"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p590471115615"></a>ERROR_PKE_SM2_DECRYPT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p14904161112619"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p14904161112619"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p14904161112619"></a>SM2解密失败</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row89041011369"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p11904101117619"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p11904101117619"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p11904101117619"></a>ERROR_ECDSA_SIGN_R_IS_ZERO</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119045111764"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119045111764"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119045111764"></a>签名计算结果中R为0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1990441112618"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p390418114620"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p390418114620"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p390418114620"></a>ERROR_ECDSA_SIGN_S_IS_ZERO</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p4904121110620"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p4904121110620"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p4904121110620"></a>签名计算结果中S为0</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row139041511169"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p49041211865"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p49041211865"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p49041211865"></a>ERROR_PKE_ECDSA_VERIFY_CHECK</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1990412111869"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1990412111869"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1990412111869"></a>ECDSA验签失败</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row15904151119610"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390417111163"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390417111163"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390417111163"></a>ERROR_OTP_ADDR_NOT_ALIGNED</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190418112068"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190418112068"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190418112068"></a>OTP地址没有按照4Byte对齐</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row99041111964"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1590417111867"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1590417111867"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1590417111867"></a>ERROR_HASH_ADDR_NOT_ALIGNED</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p199043118616"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p199043118616"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p199043118616"></a>HASH写到寄存器的物理地址没有按照4Byte对齐</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row49044115620"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p17904811262"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p17904811262"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p17904811262"></a>ERROR_HASH_LEN_NOT_ALIGNED</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p79049112618"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p79049112618"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p79049112618"></a>HASH计算长度没有按照block size对齐</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1904141111616"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190451120612"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190451120612"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1190451120612"></a>ERROR_PKE_SECURITY_STRENGTH_NOT_MATCH</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p3904101112620"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p3904101112620"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p3904101112620"></a>PKE安全强度不匹配</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row990412111163"><td class="cellrowborder" rowspan="5" valign="top" width="14.371437143714372%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p290418118616"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p290418118616"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p290418118616"></a>OUTER</p>
</td>
<td class="cellrowborder" valign="top" width="37.28372837283728%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p179041811467"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p179041811467"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p179041811467"></a>ERROR_MEMCPY_S</p>
</td>
<td class="cellrowborder" valign="top" width="48.344834483448345%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p79042111615"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p79042111615"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p79042111615"></a>调用memcpy_s失败</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row179040111969"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1490471113613"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1490471113613"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1490471113613"></a>ERROR_MALLOC</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119044111965"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119044111965"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119044111965"></a>调用xxx_malloc失败</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row169048111460"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1890415119618"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1890415119618"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1890415119618"></a>ERROR_MUTEX_INIT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189041811965"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189041811965"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189041811965"></a>调用xxx_mutex_init失败</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row89041211864"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1790418113612"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1790418113612"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1790418113612"></a>ERROR_MUTEX_LOCK</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1090414111964"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1090414111964"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1090414111964"></a>调用xxx_lock失败</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1890420114615"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1490412111469"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1490412111469"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1490412111469"></a>ERROR_MEMSET_S</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690410115618"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690410115618"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690410115618"></a>调用memset_s失败</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row14904181111617"><td class="cellrowborder" rowspan="2" valign="top" width="14.371437143714372%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p199041511068"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p199041511068"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p199041511068"></a>SPECIFIC UAPI</p>
</td>
<td class="cellrowborder" valign="top" width="37.28372837283728%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p129040111862"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p129040111862"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p129040111862"></a>ERROR_DEV_OPEN_FAILED</p>
</td>
<td class="cellrowborder" valign="top" width="48.344834483448345%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2904411869"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2904411869"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2904411869"></a>打开设备失败</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1790410111661"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p490416111616"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p490416111616"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p490416111616"></a>ERROR_COUNT_OVERFLOW</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p09049111360"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p09049111360"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p09049111360"></a>调用太多次初始化</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row139044115616"><td class="cellrowborder" rowspan="5" valign="top" width="14.371437143714372%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790410111569"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790410111569"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790410111569"></a>SPECIFIC DISPATCH</p>
</td>
<td class="cellrowborder" valign="top" width="37.28372837283728%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p109045118614"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p109045118614"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p109045118614"></a>ERROR_CMD_DISMATCH</p>
</td>
<td class="cellrowborder" valign="top" width="48.344834483448345%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1990411114615"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1990411114615"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1990411114615"></a>CMD无匹配项</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row290416119613"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390415110619"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390415110619"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390415110619"></a>ERROR_COPY_FROM_USER</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p149051611164"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p149051611164"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p149051611164"></a>从用户态拷贝发生错误</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row139055112618"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p17905191120619"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p17905191120619"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p17905191120619"></a>ERROR_COPY_TO_USER</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189050111763"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189050111763"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189050111763"></a>向用户态拷贝发生错误</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row69059111963"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p29051711662"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p29051711662"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p29051711662"></a>ERROR_MEM_HANDLE_GET</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p590515112615"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p590515112615"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p590515112615"></a>获取句柄发生错误</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row390510111616"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1890561117613"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1890561117613"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1890561117613"></a>ERROR_GET_OWNER</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p9905011362"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p9905011362"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p9905011362"></a>获取使用者发生错误</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row2905201116610"><td class="cellrowborder" rowspan="4" valign="top" width="14.371437143714372%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p9905101119614"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p9905101119614"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p9905101119614"></a>SPECIFIC KAPI</p>
</td>
<td class="cellrowborder" valign="top" width="37.28372837283728%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p4905171115618"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p4905171115618"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p4905171115618"></a>ERROR_PROCESS_NOT_INIT</p>
</td>
<td class="cellrowborder" valign="top" width="48.344834483448345%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790517113613"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790517113613"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790517113613"></a>进程未初始化</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row790511111616"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1090513117610"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1090513117610"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1090513117610"></a>ERROR_MAX_PROCESS</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p79056111962"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p79056111962"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p79056111962"></a>数值超过了最大值</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row129051511569"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p490519113619"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p490519113619"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p490519113619"></a>ERROR_MEMORY_ACCESS</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690513111064"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690513111064"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690513111064"></a>没有该内存的访问权限</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1390518112618"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p15905131110612"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p15905131110612"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p15905131110612"></a>ERROR_INVALID_PROCESS</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p139057112612"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p139057112612"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p139057112612"></a>没有访问资源权限的进程访问了该资源</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1590561119614"><td class="cellrowborder" rowspan="11" valign="top" width="14.371437143714372%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p20905111110617"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p20905111110617"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p20905111110617"></a>SPECIFIC HAL</p>
<p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p203371042145017"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p203371042145017"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p203371042145017"></a></p>
</td>
<td class="cellrowborder" valign="top" width="37.28372837283728%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119051211866"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119051211866"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119051211866"></a>ERROR_HASH_LOGIC = 0xA0</p>
</td>
<td class="cellrowborder" valign="top" width="48.344834483448345%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p129059110616"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p129059110616"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p129059110616"></a>HASH发生硬件逻辑错误</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row490591116613"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790515114610"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790515114610"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p790515114610"></a>ERROR_PKE_LOGIC</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1790518111468"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1790518111468"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1790518111468"></a>PKE发生硬件逻辑错误</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row19059111068"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p490561120618"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p490561120618"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p490561120618"></a>ERROR_INVALID_CPU_TYPE</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p49052011364"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p49052011364"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p49052011364"></a>逻辑拿到了无效的CPU类型</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row11905111114617"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119051811364"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119051811364"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119051811364"></a>ERROR_INVALID_REGISTER_VALUE</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p13905111112610"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p13905111112610"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p13905111112610"></a>寄存器中的值是无效的</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1090551118619"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p129057111468"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p129057111468"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p129057111468"></a>ERROR_INVALID_PHYS_ADDR</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p69052116617"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p69052116617"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p69052116617"></a>无效的物理地址</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1490541120613"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p8905131116617"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p8905131116617"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p8905131116617"></a>ERROR_OTP_PERMISSION</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189054112620"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189054112620"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189054112620"></a>没有访问对应OTP地址的权限</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row69051011365"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39053118617"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39053118617"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39053118617"></a>ERROR_KM_LOGIC</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189059113620"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189059113620"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189059113620"></a>KM发生硬件逻辑错误</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1890517115617"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39059111568"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39059111568"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39059111568"></a>ERROR_KM_KL_LOGIC</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p129058111668"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p129058111668"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p129058111668"></a>KLAD发生硬件逻辑错误</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row590511118619"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p109052011663"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p109052011663"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p109052011663"></a>ERROR_KM_KC_LOGIC</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p12905011167"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p12905011167"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p12905011167"></a>Keyslot发生硬件逻辑错误</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1890541111615"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p29056111966"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p29056111966"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p29056111966"></a>ERROR_KLAD_ROOTKEY_NOT_READY</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1490511111066"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1490511111066"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1490511111066"></a>KM执行硬件密钥派生时ROOT_KEY还没有准备好</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row103371429502"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1516101053616"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1516101053616"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1516101053616"></a>ERROR_KM_RKP_LOGIC</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1516910193615"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1516910193615"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1516910193615"></a>KM执行硬件密钥派生成发生硬件逻辑错误</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row10905411963"><td class="cellrowborder" rowspan="19" valign="top" width="14.371437143714372%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119058111569"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119058111569"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p119058111569"></a>TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" width="37.28372837283728%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p209057111068"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p209057111068"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p209057111068"></a>ERROR_GET_TRNG_TIMEOUT = 0xB0</p>
</td>
<td class="cellrowborder" valign="top" width="48.344834483448345%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p16906711666"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p16906711666"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p16906711666"></a>获取随机数超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1590617117616"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p890618119619"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p890618119619"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p890618119619"></a>ERROR_HASH_CLEAR_CHN_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1090618113610"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1090618113610"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1090618113610"></a>HASH清通道超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1090651118614"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p490619111363"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p490619111363"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p490619111363"></a>ERROR_HASH_CALC_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1990611118617"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1990611118617"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1990611118617"></a>HASH计算超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1390617119619"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p290615119619"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p290615119619"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p290615119619"></a>ERROR_SYMC_CLEAR_CHN_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p99067115616"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p99067115616"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p99067115616"></a>SYMC清通道超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row14906711564"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p390619111065"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p390619111065"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p390619111065"></a>ERROR_SYMC_CALC_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p09061611768"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p09061611768"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p09061611768"></a>SYMC计算超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1390615116613"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390612111161"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390612111161"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1390612111161"></a>ERROR_SYMC_GET_TAG_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p169065118610"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p169065118610"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p169065118610"></a>获取tag值超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row69063111562"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2906211667"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2906211667"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2906211667"></a>ERROR_PKE_LOCK_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p11906911868"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p11906911868"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p11906911868"></a>PKE锁超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row119061117616"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39061311561"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39061311561"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39061311561"></a>ERROR_PKE_WAIT_DONE_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p3906911661"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p3906911661"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p3906911661"></a>PKE计算超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row490617111062"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p169063114614"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p169063114614"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p169063114614"></a>ERROR_PKE_ROBUST_WARNING</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2090621111613"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2090621111613"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2090621111613"></a>PKE发生了鲁棒性告警</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1190671110618"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1906211767"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1906211767"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1906211767"></a>ERROR_RKP_LOCK_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p590691112617"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p590691112617"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p590691112617"></a>RKP锁超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row790691120617"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p890611110613"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p890611110613"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p890611110613"></a>ERROR_RKP_CALC_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190601113616"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190601113616"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190601113616"></a>RKP计算超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row209061111660"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1890615118615"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1890615118615"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1890615118615"></a>ERROR_KLAD_COM_ROUTE_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p149062115617"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p149062115617"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p149062115617"></a>KLAD COM计算超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row139064112619"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190617116616"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190617116616"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190617116616"></a>ERROR_OTP_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190617110612"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190617110612"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190617110612"></a>OTP计算超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row11907181116611"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1907141112612"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1907141112612"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1907141112612"></a>ERROR_KEYSLOT_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p19095111669"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p19095111669"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p19095111669"></a>Keyslot计算超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1590918111962"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p99099111361"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p99099111361"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p99099111361"></a>ERROR_KEYSLOT_LOCK</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1590991117616"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1590991117616"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1590991117616"></a>Keyslot锁超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row19909141115612"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p7909101117619"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p7909101117619"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p7909101117619"></a>ERROR_KLAD_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189094111666"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189094111666"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p189094111666"></a>KLAD计算超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row290917113617"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1490916116610"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1490916116610"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1490916116610"></a>ERROR_KLAD_LOCK</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39099111963"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39099111963"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p39099111963"></a>KLAD锁超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row3909121114614"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690920114611"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690920114611"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p1690920114611"></a>ERROR_HASH_LOCK_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p690961115612"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p690961115612"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p690961115612"></a>HASH锁超时</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_row1690915111568"><td class="cellrowborder" valign="top" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190917113618"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190917113618"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p190917113618"></a>ERROR_PKE_ALG_TIMEOUT</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2909611565"><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2909611565"></a><a name="zh-cn_topic_0000002412561468_zh-cn_topic_0000002112799928_p2909611565"></a>PKE标准算法计算超时</p>
</td>
</tr>
</tbody>
</table>

>![](public_sys-resources/icon-note.gif) **说明：** 
>CIPHER DRIVER对错误码进行了层级设计，方便debug时快速定位。用户调用service层接口时返回的错误的错误码不带层级信息，其错误含义与上述表格中的错误码一一对应。

# 网络安全注意事项<a name="ZH-CN_TOPIC_0000002499581537"></a>

-   **[安全驱动](#ZH-CN_TOPIC_0000002466382680)**  

## 安全驱动<a name="ZH-CN_TOPIC_0000002466382680"></a>

-   Cipher驱动实现了标准的对称加密AES/SM4算法，非对称RSA、ECDSA/SM2/ED25519算法，摘要算法SHA/SM3/HMAC\_SHA/HMAC\_SM3，密钥协商算法ECDH，密钥派生算法PBKDF2，没有使用任何私有算法。
-   对称算法使用建议：
    -   AES/SM4算法ECB模式为非安全算法，不建议使用。
    -   对称算法的密钥的长度越长，安全等级越高，建议用户使用AES 128bit及以上的密钥。

-   非对称算法使用建议：
    -   RSA算法建议使用密钥长度3072bit及以上的密钥。
    -   RSA签名算法padding方式建议使用PSS padding方式。
    -   RSA加解密算法padding方式建议使用OAEP padding方式。
    -   ECDSA算法建议使用密钥长度256bit及以上的密钥。
    -   ECDSA算法曲线不建议使用NIST P256/384/521曲线。

-   摘要算法使用建议：
    -   SHA1/SHA224算法安全性低，不建议用户使用。
    -   SHA/HMAC\_SHA算法建议使用SHA256/HMAC\_SHA256及以上的算法。

-   密钥协商算法使用建议：
    -   ECDH算法建议使用密钥长度256bit及以上的密钥。
    -   DH算法建议使用密钥长度3072bit及以上的密钥。

-   密钥派生算法使用建议：
    -   迭代次数和安全性成正比例，也与计算时间成正比例，迭代次数越大，意味着计算密钥花费时间越长，同时抗暴力破解能力越强。对于性能不敏感或高安全性要求场景推荐迭代次数至少需要10000000次，其他场景迭代次数默认推荐至少10000次；对于性能有特殊要求的产品最低可以迭代1000次（NIST SP 800 132）。
    -   哈希函数推荐选择SHA256或更安全的哈希算法。
    -   盐值至少16Byte，应使用安全的随机数。
    -   用于口令单向哈希时，其输出长度应该不小于256bit。

# 缩略语<a name="ZH-CN_TOPIC_0000002466382672"></a>

<a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_table1272116593117"></a>
<table><tbody><tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row1399313208408"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7993620174015"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7993620174015"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7993620174015"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1077033915459"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1077033915459"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1077033915459"></a>A</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row1999318201401"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p59934209405"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p59934209405"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p59934209405"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b13920635144516"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b13920635144516"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b13920635144516"></a>AES</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p119931320124015"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p119931320124015"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p119931320124015"></a>Advanced Encryption Standard</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p9993720134011"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p9993720134011"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p9993720134011"></a>高级加密标准</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row109931320104013"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row151175485547"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1211794865417"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1211794865417"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1211794865417"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1723718494598"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1723718494598"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1723718494598"></a>C</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row1911734815546"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p191171848105415"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p191171848105415"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p191171848105415"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b78431051155916"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b78431051155916"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b78431051155916"></a>CBC</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p5117154865417"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p5117154865417"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p5117154865417"></a>Cipher Block Chaining</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p18117154855420"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p18117154855420"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p18117154855420"></a>密码分组链接</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row163730395208"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p9373339182019"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p9373339182019"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p9373339182019"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b537519819211"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b537519819211"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b537519819211"></a>CBC_MAC</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p183731392208"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p183731392208"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p183731392208"></a>Cipher Block Chaining Message Authentication Code</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p83741139162019"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p83741139162019"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p83741139162019"></a>密文分组链接消息验证码</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row1054211426196"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p175426427194"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p175426427194"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p175426427194"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1041113418200"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1041113418200"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1041113418200"></a>CCM</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p654294241919"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p654294241919"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p654294241919"></a>Counter with CBC-MAC</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p4542194212198"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p4542194212198"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p4542194212198"></a>带有CBC-MAC 的CTR</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row43075511177"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1630716531719"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1630716531719"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1630716531719"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b960946181719"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b960946181719"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b960946181719"></a>CFB</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p12307855173"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p12307855173"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p12307855173"></a>Cipher Feedback</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7307205101716"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7307205101716"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7307205101716"></a>密码反馈</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row938117162206"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1938113162205"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1938113162205"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1938113162205"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b127141216219"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b127141216219"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b127141216219"></a>CMAC</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p2381171610208"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p2381171610208"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p2381171610208"></a>Cipher-based Message Authentication Code</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7381516122018"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7381516122018"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7381516122018"></a>加密消息认证码</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row0140711125514"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p71402118556"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p71402118556"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p71402118556"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b014311553593"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b014311553593"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b014311553593"></a>CTR</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p14140611185516"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p14140611185516"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p14140611185516"></a>Counter</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1014041115558"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1014041115558"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1014041115558"></a>计数器</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row1111704818548"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row91001655115310"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1910055595314"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1910055595314"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1910055595314"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1129614371802"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1129614371802"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1129614371802"></a>E</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row338510265542"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p338510266545"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p338510266545"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p338510266545"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b06512114019"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b06512114019"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b06512114019"></a>ECB</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p10385192685414"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p10385192685414"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p10385192685414"></a>Electronic Codebook</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p193851526175418"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p193851526175418"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p193851526175418"></a>电子密码本</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row38151134125317"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p13816123417538"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p13816123417538"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p13816123417538"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b731183711011"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b731183711011"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b731183711011"></a>ECC</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p38164340531"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p38164340531"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p38164340531"></a>Elliptic Curve Cryptography</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1781673435314"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1781673435314"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1781673435314"></a>椭圆曲线密码学</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row51001323194018"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row5413193875511"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p341353885516"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p341353885516"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p341353885516"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1630410395592"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1630410395592"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1630410395592"></a>G</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row1941318386555"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p94138384551"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p94138384551"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p94138384551"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b16810184319595"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b16810184319595"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b16810184319595"></a>GCM</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1241383815516"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1241383815516"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1241383815516"></a>Galois/Counter Mode</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1941311384554"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1941311384554"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1941311384554"></a>伽罗瓦/计数器模式</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row114131538195517"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row137114916457"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1771104919457"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1771104919457"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1771104919457"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b154111935194615"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b154111935194615"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b154111935194615"></a>H</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row8935154416532"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p893534415531"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p893534415531"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p893534415531"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b135253245418"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b135253245418"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b135253245418"></a>HASH</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1993515442538"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1993515442538"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1993515442538"></a>HASH function</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p20936184465320"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p20936184465320"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p20936184465320"></a>散列函数</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row155831229172213"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p9583112911225"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p9583112911225"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p9583112911225"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b7990119132315"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b7990119132315"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b7990119132315"></a>HKDF</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p11583122942217"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p11583122942217"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p11583122942217"></a>HMAC-based Key Derivation Function</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p10583122932211"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p10583122932211"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p10583122932211"></a>基于HMAC的密钥派生算法</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row129816104612"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p129812114613"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p129812114613"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p129812114613"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b193024074611"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b193024074611"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b193024074611"></a>HMAC</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1129811124617"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1129811124617"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1129811124617"></a>hash-based message authentication code</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p329814124618"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p329814124618"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p329814124618"></a>散列消息认证码</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row1635014712464"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row6591840105819"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1759124013585"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1759124013585"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1759124013585"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1432620372019"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1432620372019"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b1432620372019"></a>K</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row7489947175812"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p6489134710586"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p6489134710586"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p6489134710586"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b143269371104"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b143269371104"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b143269371104"></a>KM</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p44897478583"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p44897478583"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p44897478583"></a>Key Manage</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p4489124745811"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p4489124745811"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p4489124745811"></a>密钥管理</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row1510513243189"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row84361229171819"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7436129161815"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7436129161815"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7436129161815"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b422922481913"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b422922481913"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b422922481913"></a>O</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row14721534141815"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p19721434131819"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p19721434131819"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p19721434131819"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b55441520141919"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b55441520141919"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b55441520141919"></a>OFB</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p12721934191817"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p12721934191817"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p12721934191817"></a><span>Output Feedbac</span>k</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p13721734131819"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p13721734131819"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p13721734131819"></a>输出反馈</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row52052266405"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row14021346165219"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p94022466522"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p94022466522"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p94022466522"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b123271937402"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b123271937402"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b123271937402"></a>P</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row12721105921111"><td class="nocellnorowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p872105920119"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p872105920119"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p872105920119"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b9547134291213"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b9547134291213"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b9547134291213"></a>PKE</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p272195911118"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p272195911118"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p272195911118"></a>Public Key Encryption</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1972115931110"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1972115931110"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1972115931110"></a>公开密钥加密算法</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row20639112351018"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p14639102314107"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p14639102314107"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p14639102314107"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b3571054111"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b3571054111"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b3571054111"></a>PBKDF2</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1964052314102"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1964052314102"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1964052314102"></a>Password-Based Key Derivation Function 2</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p864032331017"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p864032331017"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p864032331017"></a>基于密码的密钥派生函数2</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row23821298406"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row7878536145117"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7878133635110"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7878133635110"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p7878133635110"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b10229123165219"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b10229123165219"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b10229123165219"></a>R</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row38781136145119"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p128781036145120"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p128781036145120"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p128781036145120"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b172432717528"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b172432717528"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b172432717528"></a>RSA</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p118781336105111"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p118781336105111"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p118781336105111"></a>Rivest–Shamir–Adleman</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p0878536185110"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p0878536185110"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p0878536185110"></a>Rivest–Shamir–Adleman算法</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row1987843605114"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row11925171135215"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1892571116526"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1892571116526"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1892571116526"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b18327437102"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b18327437102"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b18327437102"></a>S</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row13125296566"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p51251197565"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p51251197565"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p51251197565"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b232813371909"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b232813371909"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b232813371909"></a>SHA2</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p21254915566"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p21254915566"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p21254915566"></a>Secure Hash Algorithm 2</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p18125189125615"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p18125189125615"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p18125189125615"></a>安全散列算法2</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row2380162116410"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p538072113411"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p538072113411"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p538072113411"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b132812379010"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b132812379010"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b132812379010"></a>SM2</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p338042114418"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p338042114418"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p338042114418"></a>Shangyong Mima 2</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p83800218419"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p83800218419"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p83800218419"></a>商密2椭圆曲线公钥密码算法</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row12531209538"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p14533012534"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p14533012534"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p14533012534"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b2076633715536"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b2076633715536"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b2076633715536"></a>SM3</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p13896171465319"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p13896171465319"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p13896171465319"></a>Shangyong Mima 3</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p95330135310"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p95330135310"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p95330135310"></a>商密3密码杂凑算法</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row359023215432"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p95914329439"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p95914329439"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p95914329439"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b176941930164516"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b176941930164516"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b176941930164516"></a>SM4</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p3591173212433"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p3591173212433"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p3591173212433"></a>Shangyong Mima 4</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p17591432144310"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p17591432144310"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p17591432144310"></a>商密4分组密码算法</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row1968225155314"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p206829518532"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p206829518532"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p206829518532"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b86117279531"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b86117279531"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b86117279531"></a>SYMC</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1682458534"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1682458534"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1682458534"></a>Symmetric-key algorithm</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p19682125115311"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p19682125115311"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p19682125115311"></a>对称密钥算法</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row1592633224019"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row11771611174910"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p977141194910"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p977141194910"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p977141194910"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b232883718020"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b232883718020"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b232883718020"></a>T</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_row82821611125913"><td class="cellrowborder" style="border:none" valign="top" width="17.071707170717072%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1028214117597"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1028214117597"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1028214117597"></a><strong id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b123293371009"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b123293371009"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_b123293371009"></a>TRNG</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="49.594959495949595%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1628213118597"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1628213118597"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p1628213118597"></a>True Random Number Generator</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p19282101119594"><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p19282101119594"></a><a name="zh-cn_topic_0000002418276946_zh-cn_topic_0000002413233346_p19282101119594"></a>真随机数发生器</p>
</td>
</tr>
</tbody>
</table>

