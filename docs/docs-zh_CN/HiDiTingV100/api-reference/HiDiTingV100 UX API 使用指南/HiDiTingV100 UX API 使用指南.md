# 前言<a name="ZH-CN_TOPIC_0000002491178406"></a>

**概述<a name="zh-cn_topic_0000002518243973_section4537382116410"></a>**

本文档为穿戴产品UX API接口说明文档，用来指导客户了解UX模块对外提供的API，使客户能够准备理解UX接口的含义，更好的基于接口开发客户应用。

**读者对象<a name="zh-cn_topic_0000002518243973_title127mcpsimp"></a>**

本文档主要适用于以下工程师：

-   技术支持工程师
-   软件开发工程师

**符号约定<a name="zh-cn_topic_0000002518243973_section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="zh-cn_topic_0000002518243973_table2622507016410"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518243973_row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002518243973_p6450074116410"><a name="zh-cn_topic_0000002518243973_p6450074116410"></a><a name="zh-cn_topic_0000002518243973_p6450074116410"></a><strong id="zh-cn_topic_0000002518243973_b2136615816410"><a name="zh-cn_topic_0000002518243973_b2136615816410"></a><a name="zh-cn_topic_0000002518243973_b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002518243973_p5435366816410"><a name="zh-cn_topic_0000002518243973_p5435366816410"></a><a name="zh-cn_topic_0000002518243973_p5435366816410"></a><strong id="zh-cn_topic_0000002518243973_b5941558116410"><a name="zh-cn_topic_0000002518243973_b5941558116410"></a><a name="zh-cn_topic_0000002518243973_b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518243973_row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002518243973_p3734547016410"><a name="zh-cn_topic_0000002518243973_p3734547016410"></a><a name="zh-cn_topic_0000002518243973_p3734547016410"></a><a name="zh-cn_topic_0000002518243973_image2670064316410"></a><a name="zh-cn_topic_0000002518243973_image2670064316410"></a><span><img class="" id="zh-cn_topic_0000002518243973_image2670064316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002491018604.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002518243973_p1757432116410"><a name="zh-cn_topic_0000002518243973_p1757432116410"></a><a name="zh-cn_topic_0000002518243973_p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518243973_row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002518243973_p1432579516410"><a name="zh-cn_topic_0000002518243973_p1432579516410"></a><a name="zh-cn_topic_0000002518243973_p1432579516410"></a><a name="zh-cn_topic_0000002518243973_image4895582316410"></a><a name="zh-cn_topic_0000002518243973_image4895582316410"></a><span><img class="" id="zh-cn_topic_0000002518243973_image4895582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002491178594.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002518243973_p959197916410"><a name="zh-cn_topic_0000002518243973_p959197916410"></a><a name="zh-cn_topic_0000002518243973_p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518243973_row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002518243973_p1232579516410"><a name="zh-cn_topic_0000002518243973_p1232579516410"></a><a name="zh-cn_topic_0000002518243973_p1232579516410"></a><a name="zh-cn_topic_0000002518243973_image1235582316410"></a><a name="zh-cn_topic_0000002518243973_image1235582316410"></a><span><img class="" id="zh-cn_topic_0000002518243973_image1235582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002523178355.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002518243973_p123197916410"><a name="zh-cn_topic_0000002518243973_p123197916410"></a><a name="zh-cn_topic_0000002518243973_p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518243973_row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002518243973_p2204984716410"><a name="zh-cn_topic_0000002518243973_p2204984716410"></a><a name="zh-cn_topic_0000002518243973_p2204984716410"></a><a name="zh-cn_topic_0000002518243973_image4504446716410"></a><a name="zh-cn_topic_0000002518243973_image4504446716410"></a><span><img class="" id="zh-cn_topic_0000002518243973_image4504446716410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002523338317.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002518243973_p4388861916410"><a name="zh-cn_topic_0000002518243973_p4388861916410"></a><a name="zh-cn_topic_0000002518243973_p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="zh-cn_topic_0000002518243973_p1238861916410"><a name="zh-cn_topic_0000002518243973_p1238861916410"></a><a name="zh-cn_topic_0000002518243973_p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518243973_row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002518243973_p5555360116410"><a name="zh-cn_topic_0000002518243973_p5555360116410"></a><a name="zh-cn_topic_0000002518243973_p5555360116410"></a><a name="zh-cn_topic_0000002518243973_image799324016410"></a><a name="zh-cn_topic_0000002518243973_image799324016410"></a><span><img class="" id="zh-cn_topic_0000002518243973_image799324016410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002491018606.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002518243973_p4612588116410"><a name="zh-cn_topic_0000002518243973_p4612588116410"></a><a name="zh-cn_topic_0000002518243973_p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="zh-cn_topic_0000002518243973_p1232588116410"><a name="zh-cn_topic_0000002518243973_p1232588116410"></a><a name="zh-cn_topic_0000002518243973_p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# 概述<a name="ZH-CN_TOPIC_0000002491178386"></a>

本文档主要描述穿戴音频解决方案UX框架对外提供的接口。涉及以下方面：函数接口、数据结构；

UX中间层在架构层级上位于蓝牙、媒体、平台等组件的接口的上一层、在应用层app的下一层。UX中间层封装了蓝牙、媒体、平台等接口，形成应用开发框架，并进一步对应用层提供了更简化的接口。

此外，UX中间层在对蓝牙模块的处理上，除了进一步封装API接口，还预先注册了蓝牙模块所需的回调函数，并通过消息机制上报给应用层关键消息（如连接完成、断开连接），使得上层app模块在开发过程中与蓝牙等底层模块耦合度更小，开发复杂度更低。

**表 1**  UX版本和芯片对应关系

<a name="zh-cn_topic_0000002486164126_table226mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002486164126_row232mcpsimp"><th class="cellrowborder" valign="top" width="32%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002486164126_p234mcpsimp"><a name="zh-cn_topic_0000002486164126_p234mcpsimp"></a><a name="zh-cn_topic_0000002486164126_p234mcpsimp"></a>UX版本号</p>
</th>
<th class="cellrowborder" valign="top" width="68%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002486164126_p236mcpsimp"><a name="zh-cn_topic_0000002486164126_p236mcpsimp"></a><a name="zh-cn_topic_0000002486164126_p236mcpsimp"></a>产品版本</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002486164126_row238mcpsimp"><td class="cellrowborder" valign="top" width="32%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002486164126_p240mcpsimp"><a name="zh-cn_topic_0000002486164126_p240mcpsimp"></a><a name="zh-cn_topic_0000002486164126_p240mcpsimp"></a>UX 1.0.0</p>
</td>
<td class="cellrowborder" valign="top" width="68%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002486164126_p242mcpsimp"><a name="zh-cn_topic_0000002486164126_p242mcpsimp"></a><a name="zh-cn_topic_0000002486164126_p242mcpsimp"></a>HiDiTing</p>
</td>
</tr>
</tbody>
</table>

# UX API说明<a name="ZH-CN_TOPIC_0000002491018408"></a>

-   **[连接管理](#ZH-CN_TOPIC_0000002523338119)**  

-   **[数传通道](#ZH-CN_TOPIC_0000002491178384)**  

-   **[蓝牙音频](#ZH-CN_TOPIC_0000002523178165)**  

## 连接管理<a name="ZH-CN_TOPIC_0000002523338119"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002491178388)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002491018412)**  

-   **[开发指导](#ZH-CN_TOPIC_0000002491178402)**  

### 数据结构<a name="ZH-CN_TOPIC_0000002491178388"></a>

-   **[ux\_api\_bt\_addr\_t](#ZH-CN_TOPIC_0000002523178175)**  

-   **[ux\_api\_bt\_protocol\_t](#ZH-CN_TOPIC_0000002523338143)**  

-   **[ux\_api\_bt\_profile\_t](#ZH-CN_TOPIC_0000002491178408)**  

-   **[ux\_api\_bt\_profile\_state\_t](#ZH-CN_TOPIC_0000002491018420)**  

-   **[ux\_api\_bt\_role\_t](#ZH-CN_TOPIC_0000002523178177)**  

-   **[ux\_api\_bt\_config\_t](#ZH-CN_TOPIC_0000002523178171)**  

-   **[ux\_api\_bt\_conn\_param\_t](#ZH-CN_TOPIC_0000002491018416)**  

-   **[ux\_api\_bt\_phy\_t](#ZH-CN_TOPIC_0000002523338131)**  

-   **[ux\_api\_bt\_auto\_connect\_profile\_t](#ZH-CN_TOPIC_0000002523338139)**  

#### ux\_api\_bt\_addr\_t<a name="ZH-CN_TOPIC_0000002523178175"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt.h

【结构体说明】

蓝牙地址类型的结构体。

【结构体定义】

```
typedef struct {
    uint8_t addr[UX_API_BT_ADDR_LEN];   // 蓝牙地址，6位MAC地址
    uint8_t type;                       // 蓝牙地址类型, 0: 公共地址, 1: 随机地址
} ux_api_bt_addr_t;
```

#### ux\_api\_bt\_protocol\_t<a name="ZH-CN_TOPIC_0000002523338143"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt.h

【枚举说明】

用于标识协议类型的枚举。

【枚举定义】

```
typedef enum {
    UX_API_BT_PROTOCOL_BR,   // BR协议
    UX_API_BT_PROTOCOL_BLE,  // BLE协议
    UX_API_BT_PROTOCOL_MAX,
} ux_api_bt_protocol_t;
```

#### ux\_api\_bt\_profile\_t<a name="ZH-CN_TOPIC_0000002491178408"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt.h

【枚举说明】

指代链路类型的枚举。

【枚举定义】

```
typedef enum {
    UX_API_BT_PROFILE_BR = 0,   // BR链路
    UX_API_BT_PROFILE_BLE,      // BLE链路
    UX_API_BT_PROFILE_SCO,      // SCO链路
    UX_API_BT_PROFILE_A2DP,     // A2DP链路
    UX_API_BT_PROFILE_HFP,      // HFP链路
    UX_API_BT_PROFILE_AVRCP,    // AVRCP链路
    UX_API_BT_PROFILE_MAX,
} ux_api_bt_profile_t;
```

#### ux\_api\_bt\_profile\_state\_t<a name="ZH-CN_TOPIC_0000002491018420"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt.h

【枚举说明】

指代链路统一连接状态的枚举。

【枚举定义】

```
typedef enum {
    UX_API_BT_PROFILE_STATE_CONNECTING = 0x01, 
    UX_API_BT_PROFILE_STATE_CONNECTED = 0x02,      
    UX_API_BT_PROFILE_STATE_DISCONNECTING = 0x03,    
    UX_API_BT_PROFILE_STATE_DISCONNECTED = 0x04,
    UX_API_BT_PROFILE_STATE_MAX,
} ux_api_bt_profile_state_t;
```

#### ux\_api\_bt\_role\_t<a name="ZH-CN_TOPIC_0000002523178177"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt.h

【枚举说明】

用于标识蓝牙设备角色的枚举，包括中心设备和外围设备。

【枚举定义】

```
typedef enum {
    UX_API_BT_AS_CENTRAL,
    UX_API_BT_AS_PERIPHERAL,
    UX_API_BT_AS_MAX,
} ux_api_bt_role_t;
```

#### ux\_api\_bt\_config\_t<a name="ZH-CN_TOPIC_0000002523178171"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt.h

【枚举说明】

用于标识蓝牙配置项的枚举。

【枚举定义】

```
typedef enum {
    UX_API_BT_CONFIG_LOCAL_ADDR = 0,
    UX_API_BT_CONFIG_LOCAL_NAME,    
    UX_API_BT_CONFIG_ADV_DATA,
    UX_API_BT_CONFIG_ADV_SCAN_RSP,
    UX_API_BT_CONFIG_CONNECT_PARAM,
    UX_API_BT_CONFIG_CONNECT_PHY,
    UX_API_BT_CONFIG_PAIR_USE_CTKD,
    UX_API_BT_CONFIG_PROFILE_CONNECT,
    UX_API_BT_CONFIG_MAX,
} ux_api_bt_config_t;
```

#### ux\_api\_bt\_conn\_param\_t<a name="ZH-CN_TOPIC_0000002491018416"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt.h

【结构体说明】

设置蓝牙连接参数时候的参数结构体。

【结构体定义】

```
typedef struct {
    ux_api_bt_addr_t addr;      
    uint16_t interval_min;       
    uint16_t interval_max;  
    uint16_t slave_latency; 
    uint16_t timeout_multiplier; 
} ux_api_bt_conn_param_t;
```

#### ux\_api\_bt\_phy\_t<a name="ZH-CN_TOPIC_0000002523338131"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt.h

【结构体说明】

设置蓝牙PHY参数的结构体。

【结构体定义】

```
typedef struct {
    ux_api_bt_addr_t addr;      // 地址 参考 @ref ux_api_bt_addr_t
    uint8_t all_phys;           // 所有PHY
    uint8_t tx_phys;            // 发送PHY  
    uint8_t rx_phys;            // 接收PHY
    uint16_t phy_options;       // PHY选项
} ux_api_bt_phy_t;
```

#### ux\_api\_bt\_auto\_connect\_profile\_t<a name="ZH-CN_TOPIC_0000002523338139"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt.h

【结构体说明】

用于设置蓝牙自动连接项的结构体。

【结构体定义】

```
typedef struct {
    bool a2dp_snk : 1; // 支持a2dp sink profile。
    bool a2dp_src : 1; // 支持a2dp source profile。
    bool avrcp_ct : 1; // 支持avrcp control profile。
    bool avrcp_tg : 1; // 支持avrcp target profile。 
    bool hfp_hf : 1;   // 支持hfp hands free profile。
    bool hfp_ag : 1;   // 支持hfp audio gate profile。
} ux_api_bt_auto_connect_profile_t;
```

### 函数接口<a name="ZH-CN_TOPIC_0000002491018412"></a>

-   **[ux\_api\_bt\_enable](#ZH-CN_TOPIC_0000002523178161)**  

-   **[ux\_api\_bt\_disable](#ZH-CN_TOPIC_0000002523338127)**  

-   **[ux\_api\_bt\_start\_discovery](#ZH-CN_TOPIC_0000002523338135)**  

-   **[ux\_api\_bt\_stop\_discovery](#ZH-CN_TOPIC_0000002523178167)**  

-   **[ux\_api\_bt\_pair\_device](#ZH-CN_TOPIC_0000002491018430)**  

-   **[ux\_api\_bt\_connect](#ZH-CN_TOPIC_0000002491018434)**  

-   **[ux\_api\_bt\_remove\_device](#ZH-CN_TOPIC_0000002491178410)**  

-   **[ux\_api\_bt\_disconnect](#ZH-CN_TOPIC_0000002491018436)**  

-   **[ux\_api\_bt\_get\_profile\_state](#ZH-CN_TOPIC_0000002523338133)**  

-   **[ux\_api\_bt\_config](#ZH-CN_TOPIC_0000002491018424)**  

-   **[ux\_api\_bt\_get\_name](#ZH-CN_TOPIC_0000002523178183)**  

#### ux\_api\_bt\_enable<a name="ZH-CN_TOPIC_0000002523178161"></a>

【描述】

启用指定协议的蓝牙功能。

【接口定义】

```
errcode_t ux_api_bt_enable(ux_api_bt_protocol_t protocol);
```

【参数】

<a name="zh-cn_topic_0000002486004166_table196273704812"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002486004166_row179711373486"><th class="cellrowborder" valign="top" width="15.761576157615762%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002486004166_p15971123716486"><a name="zh-cn_topic_0000002486004166_p15971123716486"></a><a name="zh-cn_topic_0000002486004166_p15971123716486"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="63.08630863086309%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002486004166_p179711374484"><a name="zh-cn_topic_0000002486004166_p179711374484"></a><a name="zh-cn_topic_0000002486004166_p179711374484"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.152115211521153%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002486004166_p1097113377486"><a name="zh-cn_topic_0000002486004166_p1097113377486"></a><a name="zh-cn_topic_0000002486004166_p1097113377486"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002486004166_row169711379483"><td class="cellrowborder" valign="top" width="15.761576157615762%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002486004166_p09712371488"><a name="zh-cn_topic_0000002486004166_p09712371488"></a><a name="zh-cn_topic_0000002486004166_p09712371488"></a>protocol</p>
</td>
<td class="cellrowborder" valign="top" width="63.08630863086309%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002486004166_p5971113744818"><a name="zh-cn_topic_0000002486004166_p5971113744818"></a><a name="zh-cn_topic_0000002486004166_p5971113744818"></a>需启用的蓝牙协议类型，取值范围：</p>
<p id="zh-cn_topic_0000002486004166_p81213369498"><a name="zh-cn_topic_0000002486004166_p81213369498"></a><a name="zh-cn_topic_0000002486004166_p81213369498"></a>UX_API_BT_PROTOCOL_BR（经典蓝牙）；</p>
<p id="zh-cn_topic_0000002486004166_p2971113794819"><a name="zh-cn_topic_0000002486004166_p2971113794819"></a><a name="zh-cn_topic_0000002486004166_p2971113794819"></a>UX_API_BT_PROTOCOL_BLE（低功耗蓝牙）。</p>
</td>
<td class="cellrowborder" valign="top" width="21.152115211521153%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002486004166_p20971133794819"><a name="zh-cn_topic_0000002486004166_p20971133794819"></a><a name="zh-cn_topic_0000002486004166_p20971133794819"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002486004166_table1950818234503"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002486004166_row10515123105016"><th class="cellrowborder" valign="top" width="16.535353535353536%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002486004166_p1516122315502"><a name="zh-cn_topic_0000002486004166_p1516122315502"></a><a name="zh-cn_topic_0000002486004166_p1516122315502"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="62.282828282828284%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002486004166_p10516623145013"><a name="zh-cn_topic_0000002486004166_p10516623145013"></a><a name="zh-cn_topic_0000002486004166_p10516623145013"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.181818181818183%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002486004166_p251692365012"><a name="zh-cn_topic_0000002486004166_p251692365012"></a><a name="zh-cn_topic_0000002486004166_p251692365012"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002486004166_row1851622314504"><td class="cellrowborder" valign="top" width="16.535353535353536%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002486004166_p12516192311501"><a name="zh-cn_topic_0000002486004166_p12516192311501"></a><a name="zh-cn_topic_0000002486004166_p12516192311501"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="62.282828282828284%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002486004166_p84714551501"><a name="zh-cn_topic_0000002486004166_p84714551501"></a><a name="zh-cn_topic_0000002486004166_p84714551501"></a>0：启用成功。</p>
<p id="zh-cn_topic_0000002486004166_p451642320503"><a name="zh-cn_topic_0000002486004166_p451642320503"></a><a name="zh-cn_topic_0000002486004166_p451642320503"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.181818181818183%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002486004166_p2051610232509"><a name="zh-cn_topic_0000002486004166_p2051610232509"></a><a name="zh-cn_topic_0000002486004166_p2051610232509"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【消息上报】

ux\_api\_bt\_enable是异步接口，调用成功的瞬间并不代表指定的BR或者BLE协议已经成功开启。BR或者BLE真正开启成功的信息需要等待蓝牙协议栈通过回调函数上报，UX模块在启动时已经注册了回调函数（BR对应于process\_br\_stack\_state\_change\_result，BLE对应于process\_ble\_stack\_state\_change\_result，位于“application/ux/watch/bg/gap”目录），并且会通过发送软件消息的方式通知上层native-app。

在调用ux\_api\_bt\_enable成功、并且BR或BLE协议栈也确实开启成功后，UX模块将通过SendMsgToUIService函数将BT\_GAP\_ENABLE\_SUCC消息发送到ui\_service（见位于“application/ux/adapter/bg/watch/”目录的ux\_adapter\_bt\_enable\_process函数），ui\_service中会通过ServiceMsgProcess函数初步处理该消息，在该函数的BT\_GAP\_ENABLE\_SUCC case中，会通过抛事件的方式抛到GraphicService处理函数。

最终在GraphicService中，会使用SettingEnableMsgProcess函数来详细处理BT\_GAP\_ENABLE\_SUCC消息。如果需要定制修改BR/BLE的协议栈启动成功消息的处理，请修改SettingEnableMsgProcess函数。

<a name="zh-cn_topic_0000002486004166_table1742711478425"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002486004166_row20428847134219"><th class="cellrowborder" valign="top" width="20.162016201620162%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002486004166_p1442854704216"><a name="zh-cn_topic_0000002486004166_p1442854704216"></a><a name="zh-cn_topic_0000002486004166_p1442854704216"></a>消息定义</p>
</th>
<th class="cellrowborder" valign="top" width="39.74397439743974%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002486004166_p842820471422"><a name="zh-cn_topic_0000002486004166_p842820471422"></a><a name="zh-cn_topic_0000002486004166_p842820471422"></a>消息处理函数</p>
</th>
<th class="cellrowborder" valign="top" width="40.09400940094009%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002486004166_p1468631774314"><a name="zh-cn_topic_0000002486004166_p1468631774314"></a><a name="zh-cn_topic_0000002486004166_p1468631774314"></a>消息处理函数参数</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002486004166_row1242874724213"><td class="cellrowborder" valign="top" width="20.162016201620162%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002486004166_p144281447174212"><a name="zh-cn_topic_0000002486004166_p144281447174212"></a><a name="zh-cn_topic_0000002486004166_p144281447174212"></a>BT_GAP_ENABLE_SUCC</p>
</td>
<td class="cellrowborder" valign="top" width="39.74397439743974%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002486004166_p1440017380433"><a name="zh-cn_topic_0000002486004166_p1440017380433"></a><a name="zh-cn_topic_0000002486004166_p1440017380433"></a>SettingEnableMsgProcess：在BR或者BLE协议启动成功后进行处理。</p>
</td>
<td class="cellrowborder" valign="top" width="40.09400940094009%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002486004166_p23647094416"><a name="zh-cn_topic_0000002486004166_p23647094416"></a><a name="zh-cn_topic_0000002486004166_p23647094416"></a>bs_protocol_type_t protocal：协议类型</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

重复启用同一协议会返回错误。

#### ux\_api\_bt\_disable<a name="ZH-CN_TOPIC_0000002523338127"></a>

【描述】

关闭指定协议的蓝牙功能。

【接口定义】

```
errcode_t ux_api_bt_disable(ux_api_bt_protocol_t protocol);
```

【参数】

<a name="zh-cn_topic_0000002518243979_table20624840135119"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518243979_row18637104015515"><th class="cellrowborder" valign="top" width="22.57%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518243979_p563715407518"><a name="zh-cn_topic_0000002518243979_p563715407518"></a><a name="zh-cn_topic_0000002518243979_p563715407518"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="57.8%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518243979_p663711403512"><a name="zh-cn_topic_0000002518243979_p663711403512"></a><a name="zh-cn_topic_0000002518243979_p663711403512"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="19.63%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518243979_p86372408512"><a name="zh-cn_topic_0000002518243979_p86372408512"></a><a name="zh-cn_topic_0000002518243979_p86372408512"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518243979_row106374402518"><td class="cellrowborder" valign="top" width="22.57%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518243979_p16637640155110"><a name="zh-cn_topic_0000002518243979_p16637640155110"></a><a name="zh-cn_topic_0000002518243979_p16637640155110"></a>protocol</p>
</td>
<td class="cellrowborder" valign="top" width="57.8%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518243979_p156371940155118"><a name="zh-cn_topic_0000002518243979_p156371940155118"></a><a name="zh-cn_topic_0000002518243979_p156371940155118"></a>需禁用的蓝牙协议类型：</p>
<p id="zh-cn_topic_0000002518243979_p15121220165218"><a name="zh-cn_topic_0000002518243979_p15121220165218"></a><a name="zh-cn_topic_0000002518243979_p15121220165218"></a>UX_API_BT_PROTOCOL_BR（经典蓝牙）；</p>
<p id="zh-cn_topic_0000002518243979_p563714012518"><a name="zh-cn_topic_0000002518243979_p563714012518"></a><a name="zh-cn_topic_0000002518243979_p563714012518"></a>UX_API_BT_PROTOCOL_BLE（低功耗蓝牙）。</p>
</td>
<td class="cellrowborder" valign="top" width="19.63%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518243979_p3637140185110"><a name="zh-cn_topic_0000002518243979_p3637140185110"></a><a name="zh-cn_topic_0000002518243979_p3637140185110"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002518243979_table4626174012512"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518243979_row116379409510"><th class="cellrowborder" valign="top" width="23.22222222222222%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518243979_p12637340145116"><a name="zh-cn_topic_0000002518243979_p12637340145116"></a><a name="zh-cn_topic_0000002518243979_p12637340145116"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="58.14141414141414%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518243979_p12637040165120"><a name="zh-cn_topic_0000002518243979_p12637040165120"></a><a name="zh-cn_topic_0000002518243979_p12637040165120"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="18.636363636363637%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518243979_p3637140155110"><a name="zh-cn_topic_0000002518243979_p3637140155110"></a><a name="zh-cn_topic_0000002518243979_p3637140155110"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518243979_row1663784045110"><td class="cellrowborder" valign="top" width="23.22222222222222%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518243979_p10637124011511"><a name="zh-cn_topic_0000002518243979_p10637124011511"></a><a name="zh-cn_topic_0000002518243979_p10637124011511"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="58.14141414141414%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518243979_p19771143117525"><a name="zh-cn_topic_0000002518243979_p19771143117525"></a><a name="zh-cn_topic_0000002518243979_p19771143117525"></a>0：禁用成功。</p>
<p id="zh-cn_topic_0000002518243979_p06378409511"><a name="zh-cn_topic_0000002518243979_p06378409511"></a><a name="zh-cn_topic_0000002518243979_p06378409511"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="18.636363636363637%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518243979_p1963724075118"><a name="zh-cn_topic_0000002518243979_p1963724075118"></a><a name="zh-cn_topic_0000002518243979_p1963724075118"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

-   禁用后该协议的所有连接将被终止。
-   协议未启用时调用会返回错误。

#### ux\_api\_bt\_start\_discovery<a name="ZH-CN_TOPIC_0000002523338135"></a>

【描述】

启动蓝牙设备发现流程（包含广播和扫描）。

【接口定义】

```
errcode_t ux_api_bt_start_discovery(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol);
```

【参数】

<a name="zh-cn_topic_0000002518244059_table1040415075213"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518244059_row16421205014521"><th class="cellrowborder" valign="top" width="19.891989198919894%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518244059_p6421750105214"><a name="zh-cn_topic_0000002518244059_p6421750105214"></a><a name="zh-cn_topic_0000002518244059_p6421750105214"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="54.37543754375438%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518244059_p124215502521"><a name="zh-cn_topic_0000002518244059_p124215502521"></a><a name="zh-cn_topic_0000002518244059_p124215502521"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="25.73257325732573%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518244059_p442175011527"><a name="zh-cn_topic_0000002518244059_p442175011527"></a><a name="zh-cn_topic_0000002518244059_p442175011527"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518244059_row1242112502522"><td class="cellrowborder" valign="top" width="19.891989198919894%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518244059_p1742117505521"><a name="zh-cn_topic_0000002518244059_p1742117505521"></a><a name="zh-cn_topic_0000002518244059_p1742117505521"></a>role</p>
</td>
<td class="cellrowborder" valign="top" width="54.37543754375438%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518244059_p6421155019529"><a name="zh-cn_topic_0000002518244059_p6421155019529"></a><a name="zh-cn_topic_0000002518244059_p6421155019529"></a>设备在扫描中的角色：</p>
<p id="zh-cn_topic_0000002518244059_p5193316545"><a name="zh-cn_topic_0000002518244059_p5193316545"></a><a name="zh-cn_topic_0000002518244059_p5193316545"></a>UX_API_BT_AS_CENTRAL（中心设备）；</p>
<p id="zh-cn_topic_0000002518244059_p16421550125213"><a name="zh-cn_topic_0000002518244059_p16421550125213"></a><a name="zh-cn_topic_0000002518244059_p16421550125213"></a>UX_API_BT_AS_PERIPHERAL（外围设备）。</p>
</td>
<td class="cellrowborder" valign="top" width="25.73257325732573%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518244059_p242105035213"><a name="zh-cn_topic_0000002518244059_p242105035213"></a><a name="zh-cn_topic_0000002518244059_p242105035213"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518244059_row1242120501524"><td class="cellrowborder" valign="top" width="19.891989198919894%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518244059_p1421175011522"><a name="zh-cn_topic_0000002518244059_p1421175011522"></a><a name="zh-cn_topic_0000002518244059_p1421175011522"></a>protocol</p>
</td>
<td class="cellrowborder" valign="top" width="54.37543754375438%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518244059_p54211450105220"><a name="zh-cn_topic_0000002518244059_p54211450105220"></a><a name="zh-cn_topic_0000002518244059_p54211450105220"></a>使用的蓝牙协议类型：</p>
<p id="zh-cn_topic_0000002518244059_p1016516795419"><a name="zh-cn_topic_0000002518244059_p1016516795419"></a><a name="zh-cn_topic_0000002518244059_p1016516795419"></a>UX_API_BT_PROTOCOL_BR；</p>
<p id="zh-cn_topic_0000002518244059_p21517502537"><a name="zh-cn_topic_0000002518244059_p21517502537"></a><a name="zh-cn_topic_0000002518244059_p21517502537"></a>UX_API_BT_PROTOCOL_BLE。</p>
</td>
<td class="cellrowborder" valign="top" width="25.73257325732573%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518244059_p742115015521"><a name="zh-cn_topic_0000002518244059_p742115015521"></a><a name="zh-cn_topic_0000002518244059_p742115015521"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002518244059_table1840613502527"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518244059_row9421165055218"><th class="cellrowborder" valign="top" width="20.858585858585858%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518244059_p154216508525"><a name="zh-cn_topic_0000002518244059_p154216508525"></a><a name="zh-cn_topic_0000002518244059_p154216508525"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="53.36363636363637%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518244059_p2042195005213"><a name="zh-cn_topic_0000002518244059_p2042195005213"></a><a name="zh-cn_topic_0000002518244059_p2042195005213"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="25.77777777777778%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518244059_p14421150205213"><a name="zh-cn_topic_0000002518244059_p14421150205213"></a><a name="zh-cn_topic_0000002518244059_p14421150205213"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518244059_row1642112509527"><td class="cellrowborder" valign="top" width="20.858585858585858%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518244059_p184211750175218"><a name="zh-cn_topic_0000002518244059_p184211750175218"></a><a name="zh-cn_topic_0000002518244059_p184211750175218"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="53.36363636363637%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518244059_p14159152162813"><a name="zh-cn_topic_0000002518244059_p14159152162813"></a><a name="zh-cn_topic_0000002518244059_p14159152162813"></a>0：启动成功；</p>
<p id="zh-cn_topic_0000002518244059_p742165095216"><a name="zh-cn_topic_0000002518244059_p742165095216"></a><a name="zh-cn_topic_0000002518244059_p742165095216"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="25.77777777777778%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518244059_p84211350165212"><a name="zh-cn_topic_0000002518244059_p84211350165212"></a><a name="zh-cn_topic_0000002518244059_p84211350165212"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

-   需先通过ux\_api\_bt\_enable\(\)启用协议。
-   角色选择：

    CENTRAL：主动扫描其他设备（如手机扫描耳机）。

    PERIPHERAL：广播自身信息（如耳机广播自身存在）。

-   发现结果通过事件回调返回（需提前注册回调函数）。

【消息上报】

在调用ux\_api\_bt\_start\_discovery并指定role为CENTRAL后，蓝牙协议栈会根据protocol参数指定的协议类型启动BR或者BLE的设备发现流程。蓝牙协议栈会通过回调函数上报设备发现结果，UX模块在启动时已注册了回调函数，BR与BLE的扫描回调函数分别对应为process\_br\_discover\_result和process\_ble\_discover\_result。

对于BR的扫描结果，UX模块会通过发送软件消息的方式通知上层native-app，具体是通过SendMsgToUIService函数将BT\_SCAN\_COMPLETE消息发送到ui\_service（见位于“application/ux/adapter/bg/watch/”目录的ux\_adapter\_bt\_scan\_complete\_process函数），ui\_service中会通过ServiceMsgProcess函数初步处理该消息，在该函数的BT\_SCAN\_COMPLETE case中，会通过抛事件的方式抛到GraphicService处理函数。（注：当前仅针对BR的扫描结果提供上层消息通知，BLE扫描结果暂不转发至native-app处理）

最终在GraphicService中，会使用SettingScanCompleteMsgProcess函数来详细处理BT\_SCAN\_COMPLETE消息。如果需要定制修改BR扫描结果消息的处理，请修改SettingScanCompleteMsgProcess函数。

<a name="zh-cn_topic_0000002518244059_table1742711478425"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518244059_row20428847134219"><th class="cellrowborder" valign="top" width="18.3018301830183%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518244059_p1442854704216"><a name="zh-cn_topic_0000002518244059_p1442854704216"></a><a name="zh-cn_topic_0000002518244059_p1442854704216"></a>消息定义</p>
</th>
<th class="cellrowborder" valign="top" width="36.86368636863686%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518244059_p842820471422"><a name="zh-cn_topic_0000002518244059_p842820471422"></a><a name="zh-cn_topic_0000002518244059_p842820471422"></a>消息处理函数</p>
</th>
<th class="cellrowborder" valign="top" width="44.834483448344834%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518244059_p1468631774314"><a name="zh-cn_topic_0000002518244059_p1468631774314"></a><a name="zh-cn_topic_0000002518244059_p1468631774314"></a>消息处理函数参数</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518244059_row1242874724213"><td class="cellrowborder" valign="top" width="18.3018301830183%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518244059_p144281447174212"><a name="zh-cn_topic_0000002518244059_p144281447174212"></a><a name="zh-cn_topic_0000002518244059_p144281447174212"></a>BT_SCAN_COMPLETE</p>
</td>
<td class="cellrowborder" valign="top" width="36.86368636863686%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518244059_p1440017380433"><a name="zh-cn_topic_0000002518244059_p1440017380433"></a><a name="zh-cn_topic_0000002518244059_p1440017380433"></a>SettingScanCompleteMsgProcess：在作为中心设备发现BR设备（如：耳机）后进行处理。</p>
</td>
<td class="cellrowborder" valign="top" width="44.834483448344834%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518244059_p129795531239"><a name="zh-cn_topic_0000002518244059_p129795531239"></a><a name="zh-cn_topic_0000002518244059_p129795531239"></a>BtCaseInfo scan_msg：发现到的设备，成员包括设备名称、地址、RSSI信号强度、设备类型；</p>
<p id="zh-cn_topic_0000002518244059_p1643119211149"><a name="zh-cn_topic_0000002518244059_p1643119211149"></a><a name="zh-cn_topic_0000002518244059_p1643119211149"></a>bool need_to_del：是否需要从扫描列表中删除该设备（当RSSI强度低于扫描列表中的其它设备时）。</p>
</td>
</tr>
</tbody>
</table>

#### ux\_api\_bt\_stop\_discovery<a name="ZH-CN_TOPIC_0000002523178167"></a>

【描述】

停止设备发现流程（包含广播和扫描）。

【接口定义】

```
errcode_t ux_api_bt_stop_discovery(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol);
```

【参数】

<a name="zh-cn_topic_0000002486004138_table467654175813"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002486004138_row1069364175817"><th class="cellrowborder" valign="top" width="20.7020702070207%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002486004138_p26932416584"><a name="zh-cn_topic_0000002486004138_p26932416584"></a><a name="zh-cn_topic_0000002486004138_p26932416584"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="53.8953895389539%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002486004138_p1069394135810"><a name="zh-cn_topic_0000002486004138_p1069394135810"></a><a name="zh-cn_topic_0000002486004138_p1069394135810"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="25.402540254025403%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002486004138_p156935412580"><a name="zh-cn_topic_0000002486004138_p156935412580"></a><a name="zh-cn_topic_0000002486004138_p156935412580"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002486004138_row569344110587"><td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002486004138_p169312419585"><a name="zh-cn_topic_0000002486004138_p169312419585"></a><a name="zh-cn_topic_0000002486004138_p169312419585"></a>role</p>
</td>
<td class="cellrowborder" valign="top" width="53.8953895389539%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002486004138_p136935413581"><a name="zh-cn_topic_0000002486004138_p136935413581"></a><a name="zh-cn_topic_0000002486004138_p136935413581"></a>要停止的设备角色：</p>
<p id="zh-cn_topic_0000002486004138_p289416593586"><a name="zh-cn_topic_0000002486004138_p289416593586"></a><a name="zh-cn_topic_0000002486004138_p289416593586"></a>UX_API_BT_AS_CENTRAL；</p>
<p id="zh-cn_topic_0000002486004138_p15860195616581"><a name="zh-cn_topic_0000002486004138_p15860195616581"></a><a name="zh-cn_topic_0000002486004138_p15860195616581"></a>UX_API_BT_AS_PERIPHERAL。</p>
</td>
<td class="cellrowborder" valign="top" width="25.402540254025403%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002486004138_p46939412586"><a name="zh-cn_topic_0000002486004138_p46939412586"></a><a name="zh-cn_topic_0000002486004138_p46939412586"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002486004138_row869354119584"><td class="cellrowborder" valign="top" width="20.7020702070207%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002486004138_p36938417587"><a name="zh-cn_topic_0000002486004138_p36938417587"></a><a name="zh-cn_topic_0000002486004138_p36938417587"></a>protocol</p>
</td>
<td class="cellrowborder" valign="top" width="53.8953895389539%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002486004138_p7693141175816"><a name="zh-cn_topic_0000002486004138_p7693141175816"></a><a name="zh-cn_topic_0000002486004138_p7693141175816"></a>蓝牙协议类型：</p>
<p id="zh-cn_topic_0000002486004138_p1584310695919"><a name="zh-cn_topic_0000002486004138_p1584310695919"></a><a name="zh-cn_topic_0000002486004138_p1584310695919"></a>UX_API_BT_PROTOCOL_BR；</p>
<p id="zh-cn_topic_0000002486004138_p569316416583"><a name="zh-cn_topic_0000002486004138_p569316416583"></a><a name="zh-cn_topic_0000002486004138_p569316416583"></a>UX_API_BT_PROTOCOL_BLE。</p>
</td>
<td class="cellrowborder" valign="top" width="25.402540254025403%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002486004138_p156931941165820"><a name="zh-cn_topic_0000002486004138_p156931941165820"></a><a name="zh-cn_topic_0000002486004138_p156931941165820"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002486004138_table86783411587"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002486004138_row116933414584"><th class="cellrowborder" valign="top" width="20.162016201620162%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002486004138_p3693134118582"><a name="zh-cn_topic_0000002486004138_p3693134118582"></a><a name="zh-cn_topic_0000002486004138_p3693134118582"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="54.825482548254826%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002486004138_p12693174116582"><a name="zh-cn_topic_0000002486004138_p12693174116582"></a><a name="zh-cn_topic_0000002486004138_p12693174116582"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="25.012501250125013%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002486004138_p26931241185813"><a name="zh-cn_topic_0000002486004138_p26931241185813"></a><a name="zh-cn_topic_0000002486004138_p26931241185813"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002486004138_row9693184112587"><td class="cellrowborder" valign="top" width="20.162016201620162%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002486004138_p13693174145810"><a name="zh-cn_topic_0000002486004138_p13693174145810"></a><a name="zh-cn_topic_0000002486004138_p13693174145810"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="54.825482548254826%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002486004138_p15775111805911"><a name="zh-cn_topic_0000002486004138_p15775111805911"></a><a name="zh-cn_topic_0000002486004138_p15775111805911"></a>0：停止成功；</p>
<p id="zh-cn_topic_0000002486004138_p3693184125813"><a name="zh-cn_topic_0000002486004138_p3693184125813"></a><a name="zh-cn_topic_0000002486004138_p3693184125813"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="25.012501250125013%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002486004138_p0693164175817"><a name="zh-cn_topic_0000002486004138_p0693164175817"></a><a name="zh-cn_topic_0000002486004138_p0693164175817"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

-   必须与ux\_api\_bt\_start\_discovery配对使用。
-   停止不存在的发现流程会返回错误。

#### ux\_api\_bt\_pair\_device<a name="ZH-CN_TOPIC_0000002491018430"></a>

【描述】

与指定设备启动配对流程。

【接口定义】

```
errcode_t ux_api_bt_pair_device(const ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol);
```

【参数】

<a name="zh-cn_topic_0000002518243975_table7572156185910"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518243975_row2587105614595"><th class="cellrowborder" valign="top" width="25.56%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518243975_p9587195685918"><a name="zh-cn_topic_0000002518243975_p9587195685918"></a><a name="zh-cn_topic_0000002518243975_p9587195685918"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="49.4%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518243975_p105871356105915"><a name="zh-cn_topic_0000002518243975_p105871356105915"></a><a name="zh-cn_topic_0000002518243975_p105871356105915"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="25.040000000000003%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518243975_p158745625919"><a name="zh-cn_topic_0000002518243975_p158745625919"></a><a name="zh-cn_topic_0000002518243975_p158745625919"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518243975_row175879562595"><td class="cellrowborder" valign="top" width="25.56%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518243975_p1358705695910"><a name="zh-cn_topic_0000002518243975_p1358705695910"></a><a name="zh-cn_topic_0000002518243975_p1358705695910"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="49.4%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518243975_p158717568591"><a name="zh-cn_topic_0000002518243975_p158717568591"></a><a name="zh-cn_topic_0000002518243975_p158717568591"></a>目标设备蓝牙地址，6字节数组为小端序。</p>
</td>
<td class="cellrowborder" valign="top" width="25.040000000000003%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518243975_p1587185695910"><a name="zh-cn_topic_0000002518243975_p1587185695910"></a><a name="zh-cn_topic_0000002518243975_p1587185695910"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518243975_row858714560597"><td class="cellrowborder" valign="top" width="25.56%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518243975_p11587115617593"><a name="zh-cn_topic_0000002518243975_p11587115617593"></a><a name="zh-cn_topic_0000002518243975_p11587115617593"></a>protocol</p>
</td>
<td class="cellrowborder" valign="top" width="49.4%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518243975_p258711569599"><a name="zh-cn_topic_0000002518243975_p258711569599"></a><a name="zh-cn_topic_0000002518243975_p258711569599"></a>使用的蓝牙协议类型：</p>
<p id="zh-cn_topic_0000002518243975_p1093772419017"><a name="zh-cn_topic_0000002518243975_p1093772419017"></a><a name="zh-cn_topic_0000002518243975_p1093772419017"></a>UX_API_BT_PROTOCOL_BR；</p>
<p id="zh-cn_topic_0000002518243975_p45871556135916"><a name="zh-cn_topic_0000002518243975_p45871556135916"></a><a name="zh-cn_topic_0000002518243975_p45871556135916"></a>UX_API_BT_PROTOCOL_BLE。</p>
</td>
<td class="cellrowborder" valign="top" width="25.040000000000003%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518243975_p258765605918"><a name="zh-cn_topic_0000002518243975_p258765605918"></a><a name="zh-cn_topic_0000002518243975_p258765605918"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002518243975_table15574175635912"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518243975_row35871256125912"><th class="cellrowborder" valign="top" width="25.012501250125013%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518243975_p1858795619591"><a name="zh-cn_topic_0000002518243975_p1858795619591"></a><a name="zh-cn_topic_0000002518243975_p1858795619591"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="51.13511351135113%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518243975_p16587105620594"><a name="zh-cn_topic_0000002518243975_p16587105620594"></a><a name="zh-cn_topic_0000002518243975_p16587105620594"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.85238523852385%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518243975_p115871856155919"><a name="zh-cn_topic_0000002518243975_p115871856155919"></a><a name="zh-cn_topic_0000002518243975_p115871856155919"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518243975_row3587155611599"><td class="cellrowborder" valign="top" width="25.012501250125013%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518243975_p205871456165910"><a name="zh-cn_topic_0000002518243975_p205871456165910"></a><a name="zh-cn_topic_0000002518243975_p205871456165910"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="51.13511351135113%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518243975_p5323195311016"><a name="zh-cn_topic_0000002518243975_p5323195311016"></a><a name="zh-cn_topic_0000002518243975_p5323195311016"></a>0：配对流程启动成功；</p>
<p id="zh-cn_topic_0000002518243975_p14587115615598"><a name="zh-cn_topic_0000002518243975_p14587115615598"></a><a name="zh-cn_topic_0000002518243975_p14587115615598"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="23.85238523852385%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518243975_p358735613591"><a name="zh-cn_topic_0000002518243975_p358735613591"></a><a name="zh-cn_topic_0000002518243975_p358735613591"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

-   配对结果通过异步事件通知。
-   BLE配对需设备处于可连接状态。
-   经典蓝牙配对需先停止扫描。

#### ux\_api\_bt\_connect<a name="ZH-CN_TOPIC_0000002491018434"></a>

【描述】

连接已配对设备的指定协议链路。

【接口定义】

```
errcode_t ux_api_bt_connect(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr);
```

【参数】

<a name="zh-cn_topic_0000002518244071_table1486123112111"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518244071_row195046313118"><th class="cellrowborder" valign="top" width="15.901590159015901%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518244071_p15042031112"><a name="zh-cn_topic_0000002518244071_p15042031112"></a><a name="zh-cn_topic_0000002518244071_p15042031112"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="59.97599759975998%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518244071_p55044317115"><a name="zh-cn_topic_0000002518244071_p55044317115"></a><a name="zh-cn_topic_0000002518244071_p55044317115"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="24.122412241224122%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518244071_p6504183117111"><a name="zh-cn_topic_0000002518244071_p6504183117111"></a><a name="zh-cn_topic_0000002518244071_p6504183117111"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518244071_row16504123111110"><td class="cellrowborder" valign="top" width="15.901590159015901%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518244071_p115041731617"><a name="zh-cn_topic_0000002518244071_p115041731617"></a><a name="zh-cn_topic_0000002518244071_p115041731617"></a>profile</p>
</td>
<td class="cellrowborder" valign="top" width="59.97599759975998%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518244071_p155046315112"><a name="zh-cn_topic_0000002518244071_p155046315112"></a><a name="zh-cn_topic_0000002518244071_p155046315112"></a>要连接的协议链路，具体请参考“<a href="#ZH-CN_TOPIC_0000002491178408">ux_api_bt_profile_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="24.122412241224122%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518244071_p125041331614"><a name="zh-cn_topic_0000002518244071_p125041331614"></a><a name="zh-cn_topic_0000002518244071_p125041331614"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518244071_row450473114116"><td class="cellrowborder" valign="top" width="15.901590159015901%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518244071_p25041031714"><a name="zh-cn_topic_0000002518244071_p25041031714"></a><a name="zh-cn_topic_0000002518244071_p25041031714"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="59.97599759975998%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518244071_p20504231510"><a name="zh-cn_topic_0000002518244071_p20504231510"></a><a name="zh-cn_topic_0000002518244071_p20504231510"></a>目标设备蓝牙地址，地址成员为小端序。</p>
</td>
<td class="cellrowborder" valign="top" width="24.122412241224122%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518244071_p1850443118114"><a name="zh-cn_topic_0000002518244071_p1850443118114"></a><a name="zh-cn_topic_0000002518244071_p1850443118114"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002518244071_table948712310112"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518244071_row12504831418"><th class="cellrowborder" valign="top" width="23.63636363636364%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518244071_p35044312115"><a name="zh-cn_topic_0000002518244071_p35044312115"></a><a name="zh-cn_topic_0000002518244071_p35044312115"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="52.04040404040404%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518244071_p55041131412"><a name="zh-cn_topic_0000002518244071_p55041131412"></a><a name="zh-cn_topic_0000002518244071_p55041131412"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="24.32323232323232%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518244071_p10504153117116"><a name="zh-cn_topic_0000002518244071_p10504153117116"></a><a name="zh-cn_topic_0000002518244071_p10504153117116"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518244071_row1750414312116"><td class="cellrowborder" valign="top" width="23.63636363636364%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518244071_p19504193116119"><a name="zh-cn_topic_0000002518244071_p19504193116119"></a><a name="zh-cn_topic_0000002518244071_p19504193116119"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="52.04040404040404%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518244071_p840116221021"><a name="zh-cn_topic_0000002518244071_p840116221021"></a><a name="zh-cn_topic_0000002518244071_p840116221021"></a>0：请求发送成功；</p>
<p id="zh-cn_topic_0000002518244071_p742165095216"><a name="zh-cn_topic_0000002518244071_p742165095216"></a><a name="zh-cn_topic_0000002518244071_p742165095216"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="24.32323232323232%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518244071_p17504173119113"><a name="zh-cn_topic_0000002518244071_p17504173119113"></a><a name="zh-cn_topic_0000002518244071_p17504173119113"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

-   连接状态通过ux\_api\_bt\_get\_profile\_state查询。
-   A2DP和HFP通常需要分别连接。

【消息上报】

请参见“[蓝牙连接完成消息](#ZH-CN_TOPIC_0000002539998613)～[SCO连接状态变化消息](#ZH-CN_TOPIC_0000002539878637)”章节。

-   **[蓝牙连接完成消息](#ZH-CN_TOPIC_0000002539998613)**  

-   **[A2DP src连接状态变化消息](#ZH-CN_TOPIC_0000002508198778)**  

-   **[HFP连接状态变化消息](#ZH-CN_TOPIC_0000002508038934)**  

-   **[SCO连接状态变化消息](#ZH-CN_TOPIC_0000002539878637)**  

##### 蓝牙连接完成消息<a name="ZH-CN_TOPIC_0000002539998613"></a>

在调用ux\_api\_bt\_connect并指定profile为UX\_API\_BT\_PROFILE\_BR或者UX\_API\_BT\_PROFILE\_BLE后，蓝牙协议栈会根据参数指定的协议类型启动BR或者BLE的设备连接。蓝牙协议栈会通过回调函数上报连接状态变化事件，UX模块在启动时已注册了回调函数，BR与BLE的连接状态变化回调函数分别对应为process\_br\_connect\_state\_changed\_result和process\_ble\_connect\_state\_changed\_result。

对于BR或者BLE的连接成功事件，UX模块会通过发送软件消息的方式通知上层native-app，具体是通过BroadcastService将TOPIC\_EVENT\_BT\_CONNECTED消息进行广播（其他服务可订阅此消息），在当前的参考代码中msg\_center定阅了此消息并使用MsgCenterNotifyConnectProc来初步处理此消息，然后通过抛事件的方式抛到GraphicService处理函数。

最终在GraphicService中，会使用SettingConnectMsgProcess函数来详细处理TOPIC\_EVENT\_BT\_CONNECTED消息。如果需要定制修改BR/BLE连接完成消息的处理，请修改SettingConnectMsgProcess函数。

<a name="zh-cn_topic_0000002537984193_table1742711478425"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002537984193_row20428847134219"><th class="cellrowborder" valign="top" width="18.3018301830183%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002537984193_p1442854704216"><a name="zh-cn_topic_0000002537984193_p1442854704216"></a><a name="zh-cn_topic_0000002537984193_p1442854704216"></a>消息定义</p>
</th>
<th class="cellrowborder" valign="top" width="25.982598259825977%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002537984193_p842820471422"><a name="zh-cn_topic_0000002537984193_p842820471422"></a><a name="zh-cn_topic_0000002537984193_p842820471422"></a>消息处理函数</p>
</th>
<th class="cellrowborder" valign="top" width="55.71557155715572%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002537984193_p1468631774314"><a name="zh-cn_topic_0000002537984193_p1468631774314"></a><a name="zh-cn_topic_0000002537984193_p1468631774314"></a>消息处理函数参数</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002537984193_row1242874724213"><td class="cellrowborder" valign="top" width="18.3018301830183%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002537984193_p144281447174212"><a name="zh-cn_topic_0000002537984193_p144281447174212"></a><a name="zh-cn_topic_0000002537984193_p144281447174212"></a>TOPIC_EVENT_BT_CONNECTED</p>
</td>
<td class="cellrowborder" valign="top" width="25.982598259825977%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002537984193_p326294633319"><a name="zh-cn_topic_0000002537984193_p326294633319"></a><a name="zh-cn_topic_0000002537984193_p326294633319"></a>SettingConnectMsgProcess：在BR或者BLE连接成功时进行处理。</p>
</td>
<td class="cellrowborder" valign="top" width="55.71557155715572%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002537984193_p1478472416316"><a name="zh-cn_topic_0000002537984193_p1478472416316"></a><a name="zh-cn_topic_0000002537984193_p1478472416316"></a>const bd_addr_t& bdAddr：设备地址。</p>
<p id="zh-cn_topic_0000002537984193_p15170136123212"><a name="zh-cn_topic_0000002537984193_p15170136123212"></a><a name="zh-cn_topic_0000002537984193_p15170136123212"></a>uint8_t state：UX_BT_BR_CONNECTED和UX_BT_BLE_CONNECTED分别指代BR和BLE连接成功。</p>
<p id="zh-cn_topic_0000002537984193_p1493511143326"><a name="zh-cn_topic_0000002537984193_p1493511143326"></a><a name="zh-cn_topic_0000002537984193_p1493511143326"></a>uint32_t device_class：设备类型。</p>
</td>
</tr>
</tbody>
</table>

##### A2DP src连接状态变化消息<a name="ZH-CN_TOPIC_0000002508198778"></a>

在调用ux\_api\_bt\_connect并指定profile为UX\_API\_BT\_PROFILE\_A2DP后，蓝牙协议栈会根据开始连接addr参数指定设备的A2DP协议。蓝牙协议栈会通过回调函数上报A2DP状态变化事件，UX模块在启动时已注册了回调函数。

对于A2DP的连接状态变化事件，UX模块会通过发送软件消息的方式通知上层native-app，具体是通过SendMsgToUIService将BT\_A2DP\_SRC\_STATE\_CHANGE消息发送到ui\_service（见见位于“application/ux/adapter/bg/watch/”目录的ux\_adapter\_bt\_a2dp\_src\_state\_process函数），ui\_service中会通过ServiceMsgProcess函数初步处理该消息，在该函数的BT\_A2DP\_SRC\_STATE\_CHANGE case中，会通过抛事件的方式抛到GraphicService处理函数。

最终在GraphicService中，会使用A2dpSrcStateChangedProcess函数来详细处理BT\_A2DP\_SRC\_STATE\_CHANGE 消息。如果需要定制修改A2DP连接状态变化的处理，请修改A2dpSrcStateChangedProcess函数。

<a name="zh-cn_topic_0000002538144161_table10639183281218"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002538144161_row19639163211125"><th class="cellrowborder" valign="top" width="18.3018301830183%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002538144161_p263911325125"><a name="zh-cn_topic_0000002538144161_p263911325125"></a><a name="zh-cn_topic_0000002538144161_p263911325125"></a>消息定义</p>
</th>
<th class="cellrowborder" valign="top" width="25.892589258925895%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002538144161_p2639332141210"><a name="zh-cn_topic_0000002538144161_p2639332141210"></a><a name="zh-cn_topic_0000002538144161_p2639332141210"></a>消息处理函数</p>
</th>
<th class="cellrowborder" valign="top" width="55.80558055805581%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002538144161_p16639532131214"><a name="zh-cn_topic_0000002538144161_p16639532131214"></a><a name="zh-cn_topic_0000002538144161_p16639532131214"></a>消息处理函数参数</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002538144161_row463923219128"><td class="cellrowborder" valign="top" width="18.3018301830183%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002538144161_p3639113291211"><a name="zh-cn_topic_0000002538144161_p3639113291211"></a><a name="zh-cn_topic_0000002538144161_p3639113291211"></a>BT_A2DP_SRC_STATE_CHANGE</p>
</td>
<td class="cellrowborder" valign="top" width="25.892589258925895%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002538144161_p1725514582244"><a name="zh-cn_topic_0000002538144161_p1725514582244"></a><a name="zh-cn_topic_0000002538144161_p1725514582244"></a>A2dpSrcStateChangedProcess：A2DP状态变化后处理。</p>
</td>
<td class="cellrowborder" valign="top" width="55.80558055805581%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002538144161_p278235418248"><a name="zh-cn_topic_0000002538144161_p278235418248"></a><a name="zh-cn_topic_0000002538144161_p278235418248"></a>const bd_addr_t& bdAddr：设备地址。</p>
<p id="zh-cn_topic_0000002538144161_p13782454102413"><a name="zh-cn_topic_0000002538144161_p13782454102413"></a><a name="zh-cn_topic_0000002538144161_p13782454102413"></a>uint8_t state：A2DP当前的状态，请参见“<a href="#ZH-CN_TOPIC_0000002491018420">ux_api_bt_profile_state_t</a>”。</p>
</td>
</tr>
</tbody>
</table>

##### HFP连接状态变化消息<a name="ZH-CN_TOPIC_0000002508038934"></a>

在调用ux\_api\_bt\_connect并指定profile为UX\_API\_BT\_PROFILE\_HFP后，蓝牙协议栈会根据开始连接addr参数指定设备的HFP协议。蓝牙协议栈会通过回调函数上报HFP状态变化事件，UX模块在启动时已注册了回调函数。

对于HFP的连接状态变化事件，UX模块会通过发送软件消息的方式通知上层native-app，具体是通过SendMsgToUIService将BT\_HFP\_STATE\_CHANGE消息发送到ui\_service（见位于“application/ux/adapter/bg/watch/”目录的ux\_adapter\_bt\_hfp\_state\_process函数），ui\_service中会通过ServiceMsgProcess函数初步处理该消息，在该函数的BT\_HFP\_STATE\_CHANGE case中，会通过抛事件的方式抛到GraphicService处理函数。

最终在GraphicService中，会使用ConnStateChangedProcess函数来详细处理BT\_HFP\_STATE\_CHANGE消息。如果您需要定制修改HFP连接状态变化的处理，请修改ConnStateChangedProcess函数。

<a name="zh-cn_topic_0000002506264270_table019231151312"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002506264270_row1919221121316"><th class="cellrowborder" valign="top" width="18.3018301830183%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002506264270_p1219211117134"><a name="zh-cn_topic_0000002506264270_p1219211117134"></a><a name="zh-cn_topic_0000002506264270_p1219211117134"></a>消息定义</p>
</th>
<th class="cellrowborder" valign="top" width="26.292629262926294%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002506264270_p719201111320"><a name="zh-cn_topic_0000002506264270_p719201111320"></a><a name="zh-cn_topic_0000002506264270_p719201111320"></a>消息处理函数</p>
</th>
<th class="cellrowborder" valign="top" width="55.40554055405541%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002506264270_p12192191114135"><a name="zh-cn_topic_0000002506264270_p12192191114135"></a><a name="zh-cn_topic_0000002506264270_p12192191114135"></a>消息处理函数参数</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002506264270_row719210116137"><td class="cellrowborder" valign="top" width="18.3018301830183%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002506264270_p319261116132"><a name="zh-cn_topic_0000002506264270_p319261116132"></a><a name="zh-cn_topic_0000002506264270_p319261116132"></a>BT_HFP_STATE_CHANGE</p>
</td>
<td class="cellrowborder" valign="top" width="26.292629262926294%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002506264270_p141374919401"><a name="zh-cn_topic_0000002506264270_p141374919401"></a><a name="zh-cn_topic_0000002506264270_p141374919401"></a>ConnStateChangedProcess：HFP状态变化后处理。</p>
</td>
<td class="cellrowborder" valign="top" width="55.40554055405541%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002506264270_p198351618204110"><a name="zh-cn_topic_0000002506264270_p198351618204110"></a><a name="zh-cn_topic_0000002506264270_p198351618204110"></a>const bd_addr_t& bdAddr：设备地址。</p>
<p id="zh-cn_topic_0000002506264270_p183561814412"><a name="zh-cn_topic_0000002506264270_p183561814412"></a><a name="zh-cn_topic_0000002506264270_p183561814412"></a>uint8_t state：HFP当前的状态，请参见“<a href="#ZH-CN_TOPIC_0000002491018420">ux_api_bt_profile_state_t</a>”。</p>
</td>
</tr>
</tbody>
</table>

##### SCO连接状态变化消息<a name="ZH-CN_TOPIC_0000002539878637"></a>

在调用ux\_api\_bt\_connect并指定profile为UX\_API\_BT\_PROFILE\_SCO后，蓝牙协议栈会根据开始连接addr参数指定设备的SCO协议。蓝牙协议栈会通过回调函数上报SCO状态变化事件，UX模块在启动时已注册了回调函数。

对于SCO的连接状态变化事件，UX模块会通过发送软件消息的方式通知上层native-app，具体是通过SendMsgToUIService将BT\_SCO\_STATE\_CHANGE消息发送到ui\_service（见位于“application/ux/adapter/bg/watch/”目录的ux\_adapter\_bt\_sco\_state\_process函数），ui\_service中会通过ServiceMsgProcess函数初步处理该消息，在该函数的BT\_SCO\_STATE\_CHANGE case中，会通过抛事件的方式抛到GraphicService处理函数。

最终在GraphicService中，会使用ScoStateChangedProcess函数来详细处理BT\_SCO\_STATE\_CHANGE消息。如果您需要定制修改SCO连接状态变化的处理，请修改ScoStateChangedProcess函数。

<a name="zh-cn_topic_0000002506104438_table2804151204110"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002506104438_row5804105114120"><th class="cellrowborder" valign="top" width="18.291829182918292%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002506104438_p11804251194117"><a name="zh-cn_topic_0000002506104438_p11804251194117"></a><a name="zh-cn_topic_0000002506104438_p11804251194117"></a>消息定义</p>
</th>
<th class="cellrowborder" valign="top" width="26.302630263026305%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002506104438_p0804125110410"><a name="zh-cn_topic_0000002506104438_p0804125110410"></a><a name="zh-cn_topic_0000002506104438_p0804125110410"></a>消息处理函数</p>
</th>
<th class="cellrowborder" valign="top" width="55.40554055405541%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002506104438_p18804351144115"><a name="zh-cn_topic_0000002506104438_p18804351144115"></a><a name="zh-cn_topic_0000002506104438_p18804351144115"></a>消息处理函数参数</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002506104438_row12804145119419"><td class="cellrowborder" valign="top" width="18.291829182918292%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002506104438_p193447194314"><a name="zh-cn_topic_0000002506104438_p193447194314"></a><a name="zh-cn_topic_0000002506104438_p193447194314"></a>BT_SCO_STATE_CHANGE</p>
</td>
<td class="cellrowborder" valign="top" width="26.302630263026305%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002506104438_p780411512416"><a name="zh-cn_topic_0000002506104438_p780411512416"></a><a name="zh-cn_topic_0000002506104438_p780411512416"></a>ScoStateChangedProcess：SCO状态变化后处理。</p>
</td>
<td class="cellrowborder" valign="top" width="55.40554055405541%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002506104438_p880465114413"><a name="zh-cn_topic_0000002506104438_p880465114413"></a><a name="zh-cn_topic_0000002506104438_p880465114413"></a>const bd_addr_t& bdAddr：设备地址。</p>
<p id="zh-cn_topic_0000002506104438_p20804135112412"><a name="zh-cn_topic_0000002506104438_p20804135112412"></a><a name="zh-cn_topic_0000002506104438_p20804135112412"></a>uint8_t state：SCO当前的状态，请参见“<a href="#ZH-CN_TOPIC_0000002491018420">ux_api_bt_profile_state_t</a>”。</p>
</td>
</tr>
</tbody>
</table>

#### ux\_api\_bt\_remove\_device<a name="ZH-CN_TOPIC_0000002491178410"></a>

【描述】

删除已配对的设备信息。

【接口定义】

```
errcode_t ux_api_bt_remove_device(const ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol);
```

【参数】

<a name="zh-cn_topic_0000002519986207_table139204942"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002519986207_row3406041417"><th class="cellrowborder" valign="top" width="23.82238223822382%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002519986207_p124060415413"><a name="zh-cn_topic_0000002519986207_p124060415413"></a><a name="zh-cn_topic_0000002519986207_p124060415413"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="51.81518151815182%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002519986207_p20406241949"><a name="zh-cn_topic_0000002519986207_p20406241949"></a><a name="zh-cn_topic_0000002519986207_p20406241949"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="24.362436243624362%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002519986207_p6406174843"><a name="zh-cn_topic_0000002519986207_p6406174843"></a><a name="zh-cn_topic_0000002519986207_p6406174843"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002519986207_row17406341949"><td class="cellrowborder" valign="top" width="23.82238223822382%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002519986207_p114066410414"><a name="zh-cn_topic_0000002519986207_p114066410414"></a><a name="zh-cn_topic_0000002519986207_p114066410414"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="51.81518151815182%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002519986207_p44061741411"><a name="zh-cn_topic_0000002519986207_p44061741411"></a><a name="zh-cn_topic_0000002519986207_p44061741411"></a>目标设备蓝牙地址，地址成员为小端序。</p>
</td>
<td class="cellrowborder" valign="top" width="24.362436243624362%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002519986207_p64063410412"><a name="zh-cn_topic_0000002519986207_p64063410412"></a><a name="zh-cn_topic_0000002519986207_p64063410412"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002519986207_row340610420415"><td class="cellrowborder" valign="top" width="23.82238223822382%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002519986207_p44061348416"><a name="zh-cn_topic_0000002519986207_p44061348416"></a><a name="zh-cn_topic_0000002519986207_p44061348416"></a>protocol</p>
</td>
<td class="cellrowborder" valign="top" width="51.81518151815182%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002519986207_p340610415412"><a name="zh-cn_topic_0000002519986207_p340610415412"></a><a name="zh-cn_topic_0000002519986207_p340610415412"></a>蓝牙协议类型，决定删除BR或BLE的配对信息。</p>
</td>
<td class="cellrowborder" valign="top" width="24.362436243624362%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002519986207_p64062418420"><a name="zh-cn_topic_0000002519986207_p64062418420"></a><a name="zh-cn_topic_0000002519986207_p64062418420"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002519986207_table1039320415415"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002519986207_row44062417419"><th class="cellrowborder" valign="top" width="23.99239923992399%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002519986207_p204061445418"><a name="zh-cn_topic_0000002519986207_p204061445418"></a><a name="zh-cn_topic_0000002519986207_p204061445418"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="51.58515851585159%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002519986207_p24062041944"><a name="zh-cn_topic_0000002519986207_p24062041944"></a><a name="zh-cn_topic_0000002519986207_p24062041944"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="24.42244224422442%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002519986207_p440613417417"><a name="zh-cn_topic_0000002519986207_p440613417417"></a><a name="zh-cn_topic_0000002519986207_p440613417417"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002519986207_row4406124144"><td class="cellrowborder" valign="top" width="23.99239923992399%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002519986207_p174061941449"><a name="zh-cn_topic_0000002519986207_p174061941449"></a><a name="zh-cn_topic_0000002519986207_p174061941449"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="51.58515851585159%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002519986207_p1267413439418"><a name="zh-cn_topic_0000002519986207_p1267413439418"></a><a name="zh-cn_topic_0000002519986207_p1267413439418"></a>0：删除成功；</p>
<p id="zh-cn_topic_0000002519986207_p742165095216"><a name="zh-cn_topic_0000002519986207_p742165095216"></a><a name="zh-cn_topic_0000002519986207_p742165095216"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="24.42244224422442%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002519986207_p114061340417"><a name="zh-cn_topic_0000002519986207_p114061340417"></a><a name="zh-cn_topic_0000002519986207_p114061340417"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

删除不存在的设备会返回错误。

#### ux\_api\_bt\_disconnect<a name="ZH-CN_TOPIC_0000002491018436"></a>

【描述】

断开指定协议链路。

【接口定义】

```
errcode_t ux_api_bt_disconnect(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr);
```

【参数】

<a name="zh-cn_topic_0000002487826360_table1764318251650"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002487826360_row565822516518"><th class="cellrowborder" valign="top" width="22.642264226422643%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002487826360_p96586251451"><a name="zh-cn_topic_0000002487826360_p96586251451"></a><a name="zh-cn_topic_0000002487826360_p96586251451"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="53.635363536353644%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002487826360_p146588251952"><a name="zh-cn_topic_0000002487826360_p146588251952"></a><a name="zh-cn_topic_0000002487826360_p146588251952"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.722372237223723%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002487826360_p1565852510511"><a name="zh-cn_topic_0000002487826360_p1565852510511"></a><a name="zh-cn_topic_0000002487826360_p1565852510511"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002487826360_row36581425256"><td class="cellrowborder" valign="top" width="22.642264226422643%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487826360_p10658172510517"><a name="zh-cn_topic_0000002487826360_p10658172510517"></a><a name="zh-cn_topic_0000002487826360_p10658172510517"></a>profile</p>
</td>
<td class="cellrowborder" valign="top" width="53.635363536353644%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487826360_p04905211010"><a name="zh-cn_topic_0000002487826360_p04905211010"></a><a name="zh-cn_topic_0000002487826360_p04905211010"></a>要断开的协议链路，具体请参考“<a href="#ZH-CN_TOPIC_0000002491178408">ux_api_bt_profile_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="23.722372237223723%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487826360_p136581425559"><a name="zh-cn_topic_0000002487826360_p136581425559"></a><a name="zh-cn_topic_0000002487826360_p136581425559"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002487826360_row17658225456"><td class="cellrowborder" valign="top" width="22.642264226422643%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487826360_p36581325256"><a name="zh-cn_topic_0000002487826360_p36581325256"></a><a name="zh-cn_topic_0000002487826360_p36581325256"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="53.635363536353644%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487826360_p14658122514515"><a name="zh-cn_topic_0000002487826360_p14658122514515"></a><a name="zh-cn_topic_0000002487826360_p14658122514515"></a>目标设备蓝牙地址，地址成员为小端序。</p>
</td>
<td class="cellrowborder" valign="top" width="23.722372237223723%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487826360_p1965872510512"><a name="zh-cn_topic_0000002487826360_p1965872510512"></a><a name="zh-cn_topic_0000002487826360_p1965872510512"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002487826360_table7645325859"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002487826360_row56581925557"><th class="cellrowborder" valign="top" width="22.972297229722972%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002487826360_p136589255513"><a name="zh-cn_topic_0000002487826360_p136589255513"></a><a name="zh-cn_topic_0000002487826360_p136589255513"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="53.815381538153815%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002487826360_p12658825253"><a name="zh-cn_topic_0000002487826360_p12658825253"></a><a name="zh-cn_topic_0000002487826360_p12658825253"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.212321232123212%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002487826360_p146580256512"><a name="zh-cn_topic_0000002487826360_p146580256512"></a><a name="zh-cn_topic_0000002487826360_p146580256512"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002487826360_row86581725554"><td class="cellrowborder" valign="top" width="22.972297229722972%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487826360_p1665810259512"><a name="zh-cn_topic_0000002487826360_p1665810259512"></a><a name="zh-cn_topic_0000002487826360_p1665810259512"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="53.815381538153815%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487826360_p184981419191611"><a name="zh-cn_topic_0000002487826360_p184981419191611"></a><a name="zh-cn_topic_0000002487826360_p184981419191611"></a>0：断开请求发送成功；</p>
<p id="zh-cn_topic_0000002487826360_p15658112519514"><a name="zh-cn_topic_0000002487826360_p15658112519514"></a><a name="zh-cn_topic_0000002487826360_p15658112519514"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="23.212321232123212%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487826360_p16581251456"><a name="zh-cn_topic_0000002487826360_p16581251456"></a><a name="zh-cn_topic_0000002487826360_p16581251456"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

断开未连接的链路会返回错误。

【消息上报】

**蓝牙连接完成消息**

在调用ux\_api\_bt\_disconnect并指定profile为UX\_API\_BT\_PROFILE\_BR或者UX\_API\_BT\_PROFILE\_BLE后，蓝牙协议栈会根据参数指定的协议类型启动BR或者BLE的设备断连。蓝牙协议栈会通过回调函数上报连接状态变化事件，UX模块在启动时已注册了回调函数，BR与BLE的连接状态变化回调函数分别对应为process\_br\_connect\_state\_changed\_result和process\_ble\_connect\_state\_changed\_result。

对于BR或者BLE的断连完成事件，UX模块会通过发送软件消息的方式通知上层native-app，具体是通过BroadcastService将TOPIC\_EVENT\_BT\_DISCONNECTED消息进行广播（其他服务可订阅此消息），在当前的参考代码中msg\_center定阅了此消息并使用MsgCenterNotifyDisconnectProc来初步处理此消息，然后通过抛事件的方式抛到GraphicService处理函数。

最终在GraphicService中，会使用SettingDisconnectMsgProcess函数来详细处理TOPIC\_EVENT\_BT\_DISCONNECTED消息。如果需要定制修改BLE/BR断连消息的处理，请修改SettingDisconnectMsgProcess函数。

<a name="zh-cn_topic_0000002487826360_table18592122017355"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002487826360_row959218200355"><th class="cellrowborder" valign="top" width="18.3018301830183%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002487826360_p459217203356"><a name="zh-cn_topic_0000002487826360_p459217203356"></a><a name="zh-cn_topic_0000002487826360_p459217203356"></a>消息定义</p>
</th>
<th class="cellrowborder" valign="top" width="25.982598259825977%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002487826360_p559262083510"><a name="zh-cn_topic_0000002487826360_p559262083510"></a><a name="zh-cn_topic_0000002487826360_p559262083510"></a>消息处理函数</p>
</th>
<th class="cellrowborder" valign="top" width="55.71557155715572%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002487826360_p2592192016353"><a name="zh-cn_topic_0000002487826360_p2592192016353"></a><a name="zh-cn_topic_0000002487826360_p2592192016353"></a>消息处理函数参数</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002487826360_row3592112020352"><td class="cellrowborder" valign="top" width="18.3018301830183%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487826360_p55921620153517"><a name="zh-cn_topic_0000002487826360_p55921620153517"></a><a name="zh-cn_topic_0000002487826360_p55921620153517"></a>TOPIC_EVENT_BT_DISCONNECTED</p>
</td>
<td class="cellrowborder" valign="top" width="25.982598259825977%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487826360_p326294633319"><a name="zh-cn_topic_0000002487826360_p326294633319"></a><a name="zh-cn_topic_0000002487826360_p326294633319"></a>SettingDisconnectMsgProcess：在BR或者BLE断连时进行处理。</p>
</td>
<td class="cellrowborder" valign="top" width="55.71557155715572%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487826360_p1478472416316"><a name="zh-cn_topic_0000002487826360_p1478472416316"></a><a name="zh-cn_topic_0000002487826360_p1478472416316"></a>const bd_addr_t& bdAddr：设备地址；</p>
<p id="zh-cn_topic_0000002487826360_p5377205118385"><a name="zh-cn_topic_0000002487826360_p5377205118385"></a><a name="zh-cn_topic_0000002487826360_p5377205118385"></a>uint8_t reason：断连原因；</p>
<p id="zh-cn_topic_0000002487826360_p15170136123212"><a name="zh-cn_topic_0000002487826360_p15170136123212"></a><a name="zh-cn_topic_0000002487826360_p15170136123212"></a>uint8_t state：UX_BT_BR_DISCONNECTED和UX_BT_BLE_DISCONNECTED分别指代BR和BLE断连；</p>
<p id="zh-cn_topic_0000002487826360_p1493511143326"><a name="zh-cn_topic_0000002487826360_p1493511143326"></a><a name="zh-cn_topic_0000002487826360_p1493511143326"></a>uint32_t device_class：设备类型。</p>
</td>
</tr>
</tbody>
</table>

其他的HFP、A2DP、SCO的连接状态变化消息与消息处理方式，请参考“[ux\_api\_bt\_connect](#ZH-CN_TOPIC_0000002491018434)”章节的消息上报部分描述。

#### ux\_api\_bt\_get\_profile\_state<a name="ZH-CN_TOPIC_0000002523338133"></a>

【描述】

获取指定协议链路的状态。

【接口定义】

```
errcode_t ux_api_bt_get_profile_state(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr, ux_api_bt_profile_state_t *profile_state);
```

<a name="zh-cn_topic_0000002520066193_table093434710127"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002520066193_row294624718126"><th class="cellrowborder" valign="top" width="21.16211621162116%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002520066193_p194615476126"><a name="zh-cn_topic_0000002520066193_p194615476126"></a><a name="zh-cn_topic_0000002520066193_p194615476126"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="54.595459545954604%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002520066193_p29467478124"><a name="zh-cn_topic_0000002520066193_p29467478124"></a><a name="zh-cn_topic_0000002520066193_p29467478124"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="24.242424242424242%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002520066193_p99461947101219"><a name="zh-cn_topic_0000002520066193_p99461947101219"></a><a name="zh-cn_topic_0000002520066193_p99461947101219"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002520066193_row169461647141212"><td class="cellrowborder" valign="top" width="21.16211621162116%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520066193_p4613641185320"><a name="zh-cn_topic_0000002520066193_p4613641185320"></a><a name="zh-cn_topic_0000002520066193_p4613641185320"></a>profile</p>
</td>
<td class="cellrowborder" valign="top" width="54.595459545954604%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520066193_p19946154741214"><a name="zh-cn_topic_0000002520066193_p19946154741214"></a><a name="zh-cn_topic_0000002520066193_p19946154741214"></a>需查询的蓝牙协议类型。</p>
</td>
<td class="cellrowborder" valign="top" width="24.242424242424242%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520066193_p7946947181220"><a name="zh-cn_topic_0000002520066193_p7946947181220"></a><a name="zh-cn_topic_0000002520066193_p7946947181220"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002520066193_row9946194731215"><td class="cellrowborder" valign="top" width="21.16211621162116%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520066193_p96501044105315"><a name="zh-cn_topic_0000002520066193_p96501044105315"></a><a name="zh-cn_topic_0000002520066193_p96501044105315"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="54.595459545954604%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520066193_p9946164718126"><a name="zh-cn_topic_0000002520066193_p9946164718126"></a><a name="zh-cn_topic_0000002520066193_p9946164718126"></a>目标设备的蓝牙地址指针。</p>
</td>
<td class="cellrowborder" valign="top" width="24.242424242424242%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520066193_p11946447151216"><a name="zh-cn_topic_0000002520066193_p11946447151216"></a><a name="zh-cn_topic_0000002520066193_p11946447151216"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002520066193_row17946947161219"><td class="cellrowborder" valign="top" width="21.16211621162116%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520066193_p1171194919534"><a name="zh-cn_topic_0000002520066193_p1171194919534"></a><a name="zh-cn_topic_0000002520066193_p1171194919534"></a>profile_state</p>
</td>
<td class="cellrowborder" valign="top" width="54.595459545954604%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520066193_p11946147131212"><a name="zh-cn_topic_0000002520066193_p11946147131212"></a><a name="zh-cn_topic_0000002520066193_p11946147131212"></a>输出协议状态的缓冲区指针。</p>
</td>
<td class="cellrowborder" valign="top" width="24.242424242424242%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520066193_p6946114711219"><a name="zh-cn_topic_0000002520066193_p6946114711219"></a><a name="zh-cn_topic_0000002520066193_p6946114711219"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002520066193_table126118411962"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002520066193_row1262214115619"><th class="cellrowborder" valign="top" width="20.01200120012001%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002520066193_p13622141168"><a name="zh-cn_topic_0000002520066193_p13622141168"></a><a name="zh-cn_topic_0000002520066193_p13622141168"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="56.645664566456645%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002520066193_p15622194116620"><a name="zh-cn_topic_0000002520066193_p15622194116620"></a><a name="zh-cn_topic_0000002520066193_p15622194116620"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.342334233423344%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002520066193_p2062214411264"><a name="zh-cn_topic_0000002520066193_p2062214411264"></a><a name="zh-cn_topic_0000002520066193_p2062214411264"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002520066193_row146221411366"><td class="cellrowborder" valign="top" width="20.01200120012001%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520066193_p4622164111614"><a name="zh-cn_topic_0000002520066193_p4622164111614"></a><a name="zh-cn_topic_0000002520066193_p4622164111614"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="56.645664566456645%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520066193_p5176889718"><a name="zh-cn_topic_0000002520066193_p5176889718"></a><a name="zh-cn_topic_0000002520066193_p5176889718"></a>0：查询状态成功；</p>
<p id="zh-cn_topic_0000002520066193_p1662214416615"><a name="zh-cn_topic_0000002520066193_p1662214416615"></a><a name="zh-cn_topic_0000002520066193_p1662214416615"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="23.342334233423344%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520066193_p13622204110618"><a name="zh-cn_topic_0000002520066193_p13622204110618"></a><a name="zh-cn_topic_0000002520066193_p13622204110618"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

可查询连接状态（连接中/已连接/断开中等）。

#### ux\_api\_bt\_config<a name="ZH-CN_TOPIC_0000002491018424"></a>

【描述】

动态配置蓝牙底层参数。

【接口定义】

```
errcode_t ux_api_bt_config(ux_api_bt_config_t config, void *param, uint16_t len);
```

【参数】

<a name="zh-cn_topic_0000002487946342_table143031835145115"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002487946342_row15320735105115"><th class="cellrowborder" valign="top" width="23.03030303030303%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002487946342_p4320335105113"><a name="zh-cn_topic_0000002487946342_p4320335105113"></a><a name="zh-cn_topic_0000002487946342_p4320335105113"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="54.515151515151516%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002487946342_p163201235125113"><a name="zh-cn_topic_0000002487946342_p163201235125113"></a><a name="zh-cn_topic_0000002487946342_p163201235125113"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="22.454545454545453%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002487946342_p33209351515"><a name="zh-cn_topic_0000002487946342_p33209351515"></a><a name="zh-cn_topic_0000002487946342_p33209351515"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002487946342_row032023585112"><td class="cellrowborder" valign="top" width="23.03030303030303%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487946342_p16591194545219"><a name="zh-cn_topic_0000002487946342_p16591194545219"></a><a name="zh-cn_topic_0000002487946342_p16591194545219"></a>config</p>
</td>
<td class="cellrowborder" valign="top" width="54.515151515151516%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487946342_p3320143565117"><a name="zh-cn_topic_0000002487946342_p3320143565117"></a><a name="zh-cn_topic_0000002487946342_p3320143565117"></a>配置项类型，请参见“<a href="#ZH-CN_TOPIC_0000002523178171">ux_api_bt_config_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="22.454545454545453%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487946342_p1332073565119"><a name="zh-cn_topic_0000002487946342_p1332073565119"></a><a name="zh-cn_topic_0000002487946342_p1332073565119"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002487946342_row153201635125118"><td class="cellrowborder" valign="top" width="23.03030303030303%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487946342_p19027185311"><a name="zh-cn_topic_0000002487946342_p19027185311"></a><a name="zh-cn_topic_0000002487946342_p19027185311"></a>param</p>
</td>
<td class="cellrowborder" valign="top" width="54.515151515151516%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487946342_p33201435165112"><a name="zh-cn_topic_0000002487946342_p33201435165112"></a><a name="zh-cn_topic_0000002487946342_p33201435165112"></a>配置参数指针。</p>
</td>
<td class="cellrowborder" valign="top" width="22.454545454545453%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487946342_p133201635135116"><a name="zh-cn_topic_0000002487946342_p133201635135116"></a><a name="zh-cn_topic_0000002487946342_p133201635135116"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002487946342_row132043518512"><td class="cellrowborder" valign="top" width="23.03030303030303%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487946342_p1012811405314"><a name="zh-cn_topic_0000002487946342_p1012811405314"></a><a name="zh-cn_topic_0000002487946342_p1012811405314"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="54.515151515151516%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487946342_p10320535115112"><a name="zh-cn_topic_0000002487946342_p10320535115112"></a><a name="zh-cn_topic_0000002487946342_p10320535115112"></a>配置参数长度（单位：Byte）。</p>
</td>
<td class="cellrowborder" valign="top" width="22.454545454545453%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487946342_p1732093512515"><a name="zh-cn_topic_0000002487946342_p1732093512515"></a><a name="zh-cn_topic_0000002487946342_p1732093512515"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002487946342_table126118411962"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002487946342_row1262214115619"><th class="cellrowborder" valign="top" width="22.74227422742274%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002487946342_p13622141168"><a name="zh-cn_topic_0000002487946342_p13622141168"></a><a name="zh-cn_topic_0000002487946342_p13622141168"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="53.91539153915391%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002487946342_p15622194116620"><a name="zh-cn_topic_0000002487946342_p15622194116620"></a><a name="zh-cn_topic_0000002487946342_p15622194116620"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.342334233423344%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002487946342_p2062214411264"><a name="zh-cn_topic_0000002487946342_p2062214411264"></a><a name="zh-cn_topic_0000002487946342_p2062214411264"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002487946342_row146221411366"><td class="cellrowborder" valign="top" width="22.74227422742274%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487946342_p4622164111614"><a name="zh-cn_topic_0000002487946342_p4622164111614"></a><a name="zh-cn_topic_0000002487946342_p4622164111614"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="53.91539153915391%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487946342_p5176889718"><a name="zh-cn_topic_0000002487946342_p5176889718"></a><a name="zh-cn_topic_0000002487946342_p5176889718"></a>0：成功；</p>
<p id="zh-cn_topic_0000002487946342_p1662214416615"><a name="zh-cn_topic_0000002487946342_p1662214416615"></a><a name="zh-cn_topic_0000002487946342_p1662214416615"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="23.342334233423344%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487946342_p13622204110618"><a name="zh-cn_topic_0000002487946342_p13622204110618"></a><a name="zh-cn_topic_0000002487946342_p13622204110618"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

len必须等于目标配置项的数据类型大小。

#### ux\_api\_bt\_get\_name<a name="ZH-CN_TOPIC_0000002523178183"></a>

【描述】

获取本机蓝牙设备名称。

【接口定义】

```
errcode_t ux_api_bt_get_name(uint8_t *name, uint16_t len);
```

【参数】

<a name="zh-cn_topic_0000002520009363_table861187115616"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002520009363_row378273568"><th class="cellrowborder" valign="top" width="25.53535353535354%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002520009363_p0782725612"><a name="zh-cn_topic_0000002520009363_p0782725612"></a><a name="zh-cn_topic_0000002520009363_p0782725612"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="51.262626262626256%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002520009363_p4786716567"><a name="zh-cn_topic_0000002520009363_p4786716567"></a><a name="zh-cn_topic_0000002520009363_p4786716567"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.2020202020202%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002520009363_p4782795610"><a name="zh-cn_topic_0000002520009363_p4782795610"></a><a name="zh-cn_topic_0000002520009363_p4782795610"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002520009363_row8781773563"><td class="cellrowborder" valign="top" width="25.53535353535354%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520009363_p4280164017564"><a name="zh-cn_topic_0000002520009363_p4280164017564"></a><a name="zh-cn_topic_0000002520009363_p4280164017564"></a>name</p>
</td>
<td class="cellrowborder" valign="top" width="51.262626262626256%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520009363_p117811775614"><a name="zh-cn_topic_0000002520009363_p117811775614"></a><a name="zh-cn_topic_0000002520009363_p117811775614"></a>名称输出缓冲区指针。</p>
</td>
<td class="cellrowborder" valign="top" width="23.2020202020202%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520009363_p13785712565"><a name="zh-cn_topic_0000002520009363_p13785712565"></a><a name="zh-cn_topic_0000002520009363_p13785712565"></a>输出</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002520009363_row1778672566"><td class="cellrowborder" valign="top" width="25.53535353535354%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520009363_p1183624465620"><a name="zh-cn_topic_0000002520009363_p1183624465620"></a><a name="zh-cn_topic_0000002520009363_p1183624465620"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="51.262626262626256%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520009363_p137827175617"><a name="zh-cn_topic_0000002520009363_p137827175617"></a><a name="zh-cn_topic_0000002520009363_p137827175617"></a>缓冲区长度（单位：Byte）。</p>
</td>
<td class="cellrowborder" valign="top" width="23.2020202020202%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520009363_p378973566"><a name="zh-cn_topic_0000002520009363_p378973566"></a><a name="zh-cn_topic_0000002520009363_p378973566"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002520009363_table126118411962"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002520009363_row1262214115619"><th class="cellrowborder" valign="top" width="25.422542254225423%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002520009363_p13622141168"><a name="zh-cn_topic_0000002520009363_p13622141168"></a><a name="zh-cn_topic_0000002520009363_p13622141168"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="51.23512351235123%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002520009363_p15622194116620"><a name="zh-cn_topic_0000002520009363_p15622194116620"></a><a name="zh-cn_topic_0000002520009363_p15622194116620"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.342334233423344%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002520009363_p2062214411264"><a name="zh-cn_topic_0000002520009363_p2062214411264"></a><a name="zh-cn_topic_0000002520009363_p2062214411264"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002520009363_row146221411366"><td class="cellrowborder" valign="top" width="25.422542254225423%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520009363_p4622164111614"><a name="zh-cn_topic_0000002520009363_p4622164111614"></a><a name="zh-cn_topic_0000002520009363_p4622164111614"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="51.23512351235123%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520009363_p5176889718"><a name="zh-cn_topic_0000002520009363_p5176889718"></a><a name="zh-cn_topic_0000002520009363_p5176889718"></a>0：成功；</p>
<p id="zh-cn_topic_0000002520009363_p1662214416615"><a name="zh-cn_topic_0000002520009363_p1662214416615"></a><a name="zh-cn_topic_0000002520009363_p1662214416615"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="23.342334233423344%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520009363_p13622204110618"><a name="zh-cn_topic_0000002520009363_p13622204110618"></a><a name="zh-cn_topic_0000002520009363_p13622204110618"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

1.  蓝牙名称最大长度为32Byte（含结束符）。
2.  名称格式为UTF-8编码字符串，以“\\0”结尾。

### 开发指导<a name="ZH-CN_TOPIC_0000002491178402"></a>

打开/关闭蓝牙协议栈：

1.  调用ux\_api\_bt\_enable启动蓝牙协议栈，传参为UX\_API\_BT\_PROTOCOL\_BR表示启动BR，UX\_API\_BT\_PROTOCOL\_BLE表示启动BLE。
2.  在蓝牙协议栈打开的前提下，调用ux\_api\_bt\_disable，传参为UX\_API\_BT\_PROTOCOL\_BR表示关闭BR，UX\_API\_BT\_PROTOCOL\_BLE表示关闭BLE。

作为外围设备等待连接：

1.  在蓝牙协议栈启动成功后，调用ux\_api\_bt\_start\_discovery，role指定为UX\_API\_BT\_AS\_PERIPHERAL，protocol指定为UX\_API\_BT\_PROTOCOL\_BR或者UX\_API\_BT\_PROTOCOL\_BLE。
2.  等待中心设备（如：手机）主动发起连接。

作为中心设备连接外围设备：

1.  在蓝牙协议栈启动成功后，调用ux\_api\_bt\_start\_discovery，role指定为UX\_API\_BT\_AS\_CENTRAL，protocol指定为UX\_API\_BT\_PROTOCOL\_BR或者UX\_API\_BT\_PROTOCOL\_BLE。
2.  发现外围设备（如：耳机）后，调用ux\_api\_bt\_connect，指定profile为UX\_API\_BT\_PROFILE\_BR，并填写设备的地址到addr参数中。
3.  如果要断开设备（如：耳机），调用ux\_api\_bt\_disconnect，指定profile为UX\_API\_BT\_PROFILE\_BR，并填写设备的地址到addr参数中。
4.  如果要移除设备，调用ux\_api\_bt\_remove\_device，指定profile与addr。

## 数传通道<a name="ZH-CN_TOPIC_0000002491178384"></a>

-   **[SPP数传接口](#ZH-CN_TOPIC_0000002523338147)**  

-   **[BLE数传接口](#ZH-CN_TOPIC_0000002523338121)**  

### SPP数传接口<a name="ZH-CN_TOPIC_0000002523338147"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002523338123)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002491018414)**  

-   **[开发指导](#ZH-CN_TOPIC_0000002523178163)**  

#### 数据结构<a name="ZH-CN_TOPIC_0000002523338123"></a>

-   **[ux\_api\_spp\_msg\_handler](#ZH-CN_TOPIC_0000002491018422)**  

##### ux\_api\_spp\_msg\_handler<a name="ZH-CN_TOPIC_0000002491018422"></a>

【描述】

注册处理SPP数据的回调函数类型。

【接口定义】

```
typedef errcode_t (*ux_api_spp_msg_handler)(uint8_t *data, uint16_t data_len);
```

【参数】

<a name="zh-cn_topic_0000002518243963_table3701939318"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518243963_row1989631139"><th class="cellrowborder" valign="top" width="25%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518243963_p108983434"><a name="zh-cn_topic_0000002518243963_p108983434"></a><a name="zh-cn_topic_0000002518243963_p108983434"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="51%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518243963_p198910310311"><a name="zh-cn_topic_0000002518243963_p198910310311"></a><a name="zh-cn_topic_0000002518243963_p198910310311"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="24%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518243963_p789143533"><a name="zh-cn_topic_0000002518243963_p789143533"></a><a name="zh-cn_topic_0000002518243963_p789143533"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518243963_row17891531634"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518243963_p0729152013258"><a name="zh-cn_topic_0000002518243963_p0729152013258"></a><a name="zh-cn_topic_0000002518243963_p0729152013258"></a>data</p>
</td>
<td class="cellrowborder" valign="top" width="51%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518243963_p15643103592518"><a name="zh-cn_topic_0000002518243963_p15643103592518"></a><a name="zh-cn_topic_0000002518243963_p15643103592518"></a>接收数据缓冲区。</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518243963_p17891336312"><a name="zh-cn_topic_0000002518243963_p17891336312"></a><a name="zh-cn_topic_0000002518243963_p17891336312"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518243963_row2018834417121"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518243963_p318854411217"><a name="zh-cn_topic_0000002518243963_p318854411217"></a><a name="zh-cn_topic_0000002518243963_p318854411217"></a>data_len</p>
</td>
<td class="cellrowborder" valign="top" width="51%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518243963_p17188184418126"><a name="zh-cn_topic_0000002518243963_p17188184418126"></a><a name="zh-cn_topic_0000002518243963_p17188184418126"></a>接收数据长度，单位：Byte。</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518243963_p181896441126"><a name="zh-cn_topic_0000002518243963_p181896441126"></a><a name="zh-cn_topic_0000002518243963_p181896441126"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002518243963_table126118411962"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518243963_row1262214115619"><th class="cellrowborder" valign="top" width="22.74227422742274%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518243963_p13622141168"><a name="zh-cn_topic_0000002518243963_p13622141168"></a><a name="zh-cn_topic_0000002518243963_p13622141168"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="53.91539153915391%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518243963_p15622194116620"><a name="zh-cn_topic_0000002518243963_p15622194116620"></a><a name="zh-cn_topic_0000002518243963_p15622194116620"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.342334233423344%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518243963_p2062214411264"><a name="zh-cn_topic_0000002518243963_p2062214411264"></a><a name="zh-cn_topic_0000002518243963_p2062214411264"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518243963_row146221411366"><td class="cellrowborder" valign="top" width="22.74227422742274%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518243963_p4622164111614"><a name="zh-cn_topic_0000002518243963_p4622164111614"></a><a name="zh-cn_topic_0000002518243963_p4622164111614"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="53.91539153915391%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518243963_p5176889718"><a name="zh-cn_topic_0000002518243963_p5176889718"></a><a name="zh-cn_topic_0000002518243963_p5176889718"></a>0：成功；</p>
<p id="zh-cn_topic_0000002518243963_p1662214416615"><a name="zh-cn_topic_0000002518243963_p1662214416615"></a><a name="zh-cn_topic_0000002518243963_p1662214416615"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="23.342334233423344%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518243963_p13622204110618"><a name="zh-cn_topic_0000002518243963_p13622204110618"></a><a name="zh-cn_topic_0000002518243963_p13622204110618"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

#### 函数接口<a name="ZH-CN_TOPIC_0000002491018414"></a>

-   **[ux\_api\_spp\_service\_send\_data](#ZH-CN_TOPIC_0000002523338145)**  

-   **[ux\_api\_spp\_service\_register\_msg\_handler](#ZH-CN_TOPIC_0000002523178179)**  

##### ux\_api\_spp\_service\_send\_data<a name="ZH-CN_TOPIC_0000002523338145"></a>

【描述】

通过SPP服务发送数据。

【接口定义】

```
errcode_t ux_api_spp_service_send_data(uint16_t dev_id, uint8_t *data, uint16_t data_len);
```

【参数】

<a name="zh-cn_topic_0000002486164152_table3701939318"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002486164152_row1989631139"><th class="cellrowborder" valign="top" width="25%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002486164152_p108983434"><a name="zh-cn_topic_0000002486164152_p108983434"></a><a name="zh-cn_topic_0000002486164152_p108983434"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="51%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002486164152_p198910310311"><a name="zh-cn_topic_0000002486164152_p198910310311"></a><a name="zh-cn_topic_0000002486164152_p198910310311"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="24%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002486164152_p789143533"><a name="zh-cn_topic_0000002486164152_p789143533"></a><a name="zh-cn_topic_0000002486164152_p789143533"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002486164152_row17891531634"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002486164152_p4933203952919"><a name="zh-cn_topic_0000002486164152_p4933203952919"></a><a name="zh-cn_topic_0000002486164152_p4933203952919"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="51%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002486164152_p1691918134278"><a name="zh-cn_topic_0000002486164152_p1691918134278"></a><a name="zh-cn_topic_0000002486164152_p1691918134278"></a>用于多连接情况下指定接收端设备，单连接情况下dev_id值无效。</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002486164152_p17891336312"><a name="zh-cn_topic_0000002486164152_p17891336312"></a><a name="zh-cn_topic_0000002486164152_p17891336312"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002486164152_row2018834417121"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002486164152_p318854411217"><a name="zh-cn_topic_0000002486164152_p318854411217"></a><a name="zh-cn_topic_0000002486164152_p318854411217"></a>data</p>
</td>
<td class="cellrowborder" valign="top" width="51%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002486164152_p17188184418126"><a name="zh-cn_topic_0000002486164152_p17188184418126"></a><a name="zh-cn_topic_0000002486164152_p17188184418126"></a>发送数据缓冲区。</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002486164152_p181896441126"><a name="zh-cn_topic_0000002486164152_p181896441126"></a><a name="zh-cn_topic_0000002486164152_p181896441126"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002486164152_row192841347121218"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002486164152_p52841847131219"><a name="zh-cn_topic_0000002486164152_p52841847131219"></a><a name="zh-cn_topic_0000002486164152_p52841847131219"></a>data_len</p>
</td>
<td class="cellrowborder" valign="top" width="51%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002486164152_p11284204701218"><a name="zh-cn_topic_0000002486164152_p11284204701218"></a><a name="zh-cn_topic_0000002486164152_p11284204701218"></a>发送数据长度，单位：Byte。</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002486164152_p277616254283"><a name="zh-cn_topic_0000002486164152_p277616254283"></a><a name="zh-cn_topic_0000002486164152_p277616254283"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002486164152_table126118411962"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002486164152_row1262214115619"><th class="cellrowborder" valign="top" width="22.74227422742274%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002486164152_p13622141168"><a name="zh-cn_topic_0000002486164152_p13622141168"></a><a name="zh-cn_topic_0000002486164152_p13622141168"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="53.91539153915391%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002486164152_p15622194116620"><a name="zh-cn_topic_0000002486164152_p15622194116620"></a><a name="zh-cn_topic_0000002486164152_p15622194116620"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.342334233423344%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002486164152_p2062214411264"><a name="zh-cn_topic_0000002486164152_p2062214411264"></a><a name="zh-cn_topic_0000002486164152_p2062214411264"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002486164152_row146221411366"><td class="cellrowborder" valign="top" width="22.74227422742274%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002486164152_p4622164111614"><a name="zh-cn_topic_0000002486164152_p4622164111614"></a><a name="zh-cn_topic_0000002486164152_p4622164111614"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="53.91539153915391%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002486164152_p5176889718"><a name="zh-cn_topic_0000002486164152_p5176889718"></a><a name="zh-cn_topic_0000002486164152_p5176889718"></a>0：成功；</p>
<p id="zh-cn_topic_0000002486164152_p1662214416615"><a name="zh-cn_topic_0000002486164152_p1662214416615"></a><a name="zh-cn_topic_0000002486164152_p1662214416615"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="23.342334233423344%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002486164152_p13622204110618"><a name="zh-cn_topic_0000002486164152_p13622204110618"></a><a name="zh-cn_topic_0000002486164152_p13622204110618"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### ux\_api\_spp\_service\_register\_msg\_handler<a name="ZH-CN_TOPIC_0000002523178179"></a>

【描述】

注册处理接收到的SPP数据。

【接口定义】

```
void ux_api_spp_service_register_msg_handler(uint16_t dev_id, ux_api_spp_msg_handler handler);
```

【参数】

<a name="zh-cn_topic_0000002518364079_table3701939318"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518364079_row1989631139"><th class="cellrowborder" valign="top" width="25%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518364079_p108983434"><a name="zh-cn_topic_0000002518364079_p108983434"></a><a name="zh-cn_topic_0000002518364079_p108983434"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="51%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518364079_p198910310311"><a name="zh-cn_topic_0000002518364079_p198910310311"></a><a name="zh-cn_topic_0000002518364079_p198910310311"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="24%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518364079_p789143533"><a name="zh-cn_topic_0000002518364079_p789143533"></a><a name="zh-cn_topic_0000002518364079_p789143533"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518364079_row17891531634"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518364079_p51171856122911"><a name="zh-cn_topic_0000002518364079_p51171856122911"></a><a name="zh-cn_topic_0000002518364079_p51171856122911"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="51%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518364079_p8749213301"><a name="zh-cn_topic_0000002518364079_p8749213301"></a><a name="zh-cn_topic_0000002518364079_p8749213301"></a>双通情况下指定数据的发送端设备。</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518364079_p1111617286312"><a name="zh-cn_topic_0000002518364079_p1111617286312"></a><a name="zh-cn_topic_0000002518364079_p1111617286312"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518364079_row0768150172913"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518364079_p1276995012296"><a name="zh-cn_topic_0000002518364079_p1276995012296"></a><a name="zh-cn_topic_0000002518364079_p1276995012296"></a>handler</p>
</td>
<td class="cellrowborder" valign="top" width="51%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518364079_p167691750142914"><a name="zh-cn_topic_0000002518364079_p167691750142914"></a><a name="zh-cn_topic_0000002518364079_p167691750142914"></a>处理数据的函数指针类型，详见“<a href="#ZH-CN_TOPIC_0000002491018422">ux_api_spp_msg_handler</a>”章节。</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518364079_p476925010299"><a name="zh-cn_topic_0000002518364079_p476925010299"></a><a name="zh-cn_topic_0000002518364079_p476925010299"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002518364079_table874337315"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518364079_row48910318318"><th class="cellrowborder" valign="top" width="24.240000000000002%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518364079_p13622141168"><a name="zh-cn_topic_0000002518364079_p13622141168"></a><a name="zh-cn_topic_0000002518364079_p13622141168"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="52.53%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518364079_p15622194116620"><a name="zh-cn_topic_0000002518364079_p15622194116620"></a><a name="zh-cn_topic_0000002518364079_p15622194116620"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.23%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518364079_p2062214411264"><a name="zh-cn_topic_0000002518364079_p2062214411264"></a><a name="zh-cn_topic_0000002518364079_p2062214411264"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518364079_row1389173634"><td class="cellrowborder" valign="top" width="24.240000000000002%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518364079_p4622164111614"><a name="zh-cn_topic_0000002518364079_p4622164111614"></a><a name="zh-cn_topic_0000002518364079_p4622164111614"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="52.53%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518364079_p5176889718"><a name="zh-cn_topic_0000002518364079_p5176889718"></a><a name="zh-cn_topic_0000002518364079_p5176889718"></a>0：成功；</p>
<p id="zh-cn_topic_0000002518364079_p1662214416615"><a name="zh-cn_topic_0000002518364079_p1662214416615"></a><a name="zh-cn_topic_0000002518364079_p1662214416615"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="23.23%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518364079_p13622204110618"><a name="zh-cn_topic_0000002518364079_p13622204110618"></a><a name="zh-cn_topic_0000002518364079_p13622204110618"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

#### 开发指导<a name="ZH-CN_TOPIC_0000002523178163"></a>

1.  调用ux\_api\_spp\_service\_register\_msg\_handler接口，客户可以注册特定的函数，用于处理其他设备通过数传通道发来的数据。
2.  调用ux\_api\_spp\_service\_send\_data接口，客户可向数传通道的对端发送数据。

### BLE数传接口<a name="ZH-CN_TOPIC_0000002523338121"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002523178159)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002523338125)**  

-   **[开发指导](#ZH-CN_TOPIC_0000002491178390)**  

#### 数据结构<a name="ZH-CN_TOPIC_0000002523178159"></a>

-   **[ux\_api\_ble\_msg\_handler](#ZH-CN_TOPIC_0000002491178396)**  

##### ux\_api\_ble\_msg\_handler<a name="ZH-CN_TOPIC_0000002491178396"></a>

【描述】

注册处理host端BLE数据的回调函数类型。

【接口定义】

```
typedef errcode_t (*ux_api_ble_msg_handler)(uint8_t *data, uint16_t data_len);
```

【参数】

<a name="zh-cn_topic_0000002518363973_table3701939318"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518363973_row1989631139"><th class="cellrowborder" valign="top" width="25%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518363973_p108983434"><a name="zh-cn_topic_0000002518363973_p108983434"></a><a name="zh-cn_topic_0000002518363973_p108983434"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="51%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518363973_p198910310311"><a name="zh-cn_topic_0000002518363973_p198910310311"></a><a name="zh-cn_topic_0000002518363973_p198910310311"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="24%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518363973_p789143533"><a name="zh-cn_topic_0000002518363973_p789143533"></a><a name="zh-cn_topic_0000002518363973_p789143533"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518363973_row17891531634"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518363973_p0729152013258"><a name="zh-cn_topic_0000002518363973_p0729152013258"></a><a name="zh-cn_topic_0000002518363973_p0729152013258"></a>data</p>
</td>
<td class="cellrowborder" valign="top" width="51%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518363973_p15643103592518"><a name="zh-cn_topic_0000002518363973_p15643103592518"></a><a name="zh-cn_topic_0000002518363973_p15643103592518"></a>接收数据缓冲区。</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518363973_p17891336312"><a name="zh-cn_topic_0000002518363973_p17891336312"></a><a name="zh-cn_topic_0000002518363973_p17891336312"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518363973_row2018834417121"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518363973_p318854411217"><a name="zh-cn_topic_0000002518363973_p318854411217"></a><a name="zh-cn_topic_0000002518363973_p318854411217"></a>data_len</p>
</td>
<td class="cellrowborder" valign="top" width="51%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518363973_p17188184418126"><a name="zh-cn_topic_0000002518363973_p17188184418126"></a><a name="zh-cn_topic_0000002518363973_p17188184418126"></a>接收数据长度，单位：Byte。</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518363973_p181896441126"><a name="zh-cn_topic_0000002518363973_p181896441126"></a><a name="zh-cn_topic_0000002518363973_p181896441126"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002518363973_table126118411962"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518363973_row1262214115619"><th class="cellrowborder" valign="top" width="22.74227422742274%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518363973_p13622141168"><a name="zh-cn_topic_0000002518363973_p13622141168"></a><a name="zh-cn_topic_0000002518363973_p13622141168"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="53.91539153915391%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518363973_p15622194116620"><a name="zh-cn_topic_0000002518363973_p15622194116620"></a><a name="zh-cn_topic_0000002518363973_p15622194116620"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.342334233423344%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518363973_p2062214411264"><a name="zh-cn_topic_0000002518363973_p2062214411264"></a><a name="zh-cn_topic_0000002518363973_p2062214411264"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518363973_row146221411366"><td class="cellrowborder" valign="top" width="22.74227422742274%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518363973_p4622164111614"><a name="zh-cn_topic_0000002518363973_p4622164111614"></a><a name="zh-cn_topic_0000002518363973_p4622164111614"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="53.91539153915391%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518363973_p5176889718"><a name="zh-cn_topic_0000002518363973_p5176889718"></a><a name="zh-cn_topic_0000002518363973_p5176889718"></a>0：成功；</p>
<p id="zh-cn_topic_0000002518363973_p1662214416615"><a name="zh-cn_topic_0000002518363973_p1662214416615"></a><a name="zh-cn_topic_0000002518363973_p1662214416615"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="23.342334233423344%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518363973_p13622204110618"><a name="zh-cn_topic_0000002518363973_p13622204110618"></a><a name="zh-cn_topic_0000002518363973_p13622204110618"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

#### 函数接口<a name="ZH-CN_TOPIC_0000002523338125"></a>

-   **[ux\_api\_ble\_service\_send\_data](#ZH-CN_TOPIC_0000002523178185)**  

-   **[ux\_api\_ble\_service\_register\_msg\_handler](#ZH-CN_TOPIC_0000002491018428)**  

##### ux\_api\_ble\_service\_send\_data<a name="ZH-CN_TOPIC_0000002523178185"></a>

【描述】

通过BLE服务发送数据。

【接口定义】

```
errcode_t ux_api_ble_service_send_data(uint16_t dev_id, uint8_t *data, uint16_t data_len);
```

【参数】

<a name="zh-cn_topic_0000002518364007_table3701939318"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518364007_row1989631139"><th class="cellrowborder" valign="top" width="22.5%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518364007_p108983434"><a name="zh-cn_topic_0000002518364007_p108983434"></a><a name="zh-cn_topic_0000002518364007_p108983434"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="53.5%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518364007_p198910310311"><a name="zh-cn_topic_0000002518364007_p198910310311"></a><a name="zh-cn_topic_0000002518364007_p198910310311"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="24%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518364007_p789143533"><a name="zh-cn_topic_0000002518364007_p789143533"></a><a name="zh-cn_topic_0000002518364007_p789143533"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518364007_row17891531634"><td class="cellrowborder" valign="top" width="22.5%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518364007_p4933203952919"><a name="zh-cn_topic_0000002518364007_p4933203952919"></a><a name="zh-cn_topic_0000002518364007_p4933203952919"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="53.5%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518364007_p1691918134278"><a name="zh-cn_topic_0000002518364007_p1691918134278"></a><a name="zh-cn_topic_0000002518364007_p1691918134278"></a>用于多连接情况下指定接收端设备，单连接情况下dev_id值无效。</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518364007_p17891336312"><a name="zh-cn_topic_0000002518364007_p17891336312"></a><a name="zh-cn_topic_0000002518364007_p17891336312"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518364007_row2018834417121"><td class="cellrowborder" valign="top" width="22.5%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518364007_p318854411217"><a name="zh-cn_topic_0000002518364007_p318854411217"></a><a name="zh-cn_topic_0000002518364007_p318854411217"></a>data</p>
</td>
<td class="cellrowborder" valign="top" width="53.5%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518364007_p17188184418126"><a name="zh-cn_topic_0000002518364007_p17188184418126"></a><a name="zh-cn_topic_0000002518364007_p17188184418126"></a>发送数据缓冲区。</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518364007_p181896441126"><a name="zh-cn_topic_0000002518364007_p181896441126"></a><a name="zh-cn_topic_0000002518364007_p181896441126"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518364007_row192841347121218"><td class="cellrowborder" valign="top" width="22.5%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518364007_p52841847131219"><a name="zh-cn_topic_0000002518364007_p52841847131219"></a><a name="zh-cn_topic_0000002518364007_p52841847131219"></a>data_len</p>
</td>
<td class="cellrowborder" valign="top" width="53.5%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518364007_p11284204701218"><a name="zh-cn_topic_0000002518364007_p11284204701218"></a><a name="zh-cn_topic_0000002518364007_p11284204701218"></a>发送数据长度，单位：Byte。</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518364007_p277616254283"><a name="zh-cn_topic_0000002518364007_p277616254283"></a><a name="zh-cn_topic_0000002518364007_p277616254283"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002518364007_table126118411962"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518364007_row1262214115619"><th class="cellrowborder" valign="top" width="22.74227422742274%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518364007_p13622141168"><a name="zh-cn_topic_0000002518364007_p13622141168"></a><a name="zh-cn_topic_0000002518364007_p13622141168"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="53.91539153915391%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518364007_p15622194116620"><a name="zh-cn_topic_0000002518364007_p15622194116620"></a><a name="zh-cn_topic_0000002518364007_p15622194116620"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.342334233423344%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518364007_p2062214411264"><a name="zh-cn_topic_0000002518364007_p2062214411264"></a><a name="zh-cn_topic_0000002518364007_p2062214411264"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518364007_row146221411366"><td class="cellrowborder" valign="top" width="22.74227422742274%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518364007_p4622164111614"><a name="zh-cn_topic_0000002518364007_p4622164111614"></a><a name="zh-cn_topic_0000002518364007_p4622164111614"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="53.91539153915391%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518364007_p5176889718"><a name="zh-cn_topic_0000002518364007_p5176889718"></a><a name="zh-cn_topic_0000002518364007_p5176889718"></a>0：成功；</p>
<p id="zh-cn_topic_0000002518364007_p1662214416615"><a name="zh-cn_topic_0000002518364007_p1662214416615"></a><a name="zh-cn_topic_0000002518364007_p1662214416615"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="23.342334233423344%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518364007_p13622204110618"><a name="zh-cn_topic_0000002518364007_p13622204110618"></a><a name="zh-cn_topic_0000002518364007_p13622204110618"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### ux\_api\_ble\_service\_register\_msg\_handler<a name="ZH-CN_TOPIC_0000002491018428"></a>

【描述】

注册处理接收到的BLE数据。

【接口定义】

```
void ux_api_ble_service_register_msg_handler(uint16_t dev_id, ux_api_spp_msg_handler handler);
```

【参数】

<a name="zh-cn_topic_0000002518364037_table3701939318"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518364037_row1989631139"><th class="cellrowborder" valign="top" width="25%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518364037_p108983434"><a name="zh-cn_topic_0000002518364037_p108983434"></a><a name="zh-cn_topic_0000002518364037_p108983434"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="51%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518364037_p198910310311"><a name="zh-cn_topic_0000002518364037_p198910310311"></a><a name="zh-cn_topic_0000002518364037_p198910310311"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="24%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518364037_p789143533"><a name="zh-cn_topic_0000002518364037_p789143533"></a><a name="zh-cn_topic_0000002518364037_p789143533"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518364037_row17891531634"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518364037_p51171856122911"><a name="zh-cn_topic_0000002518364037_p51171856122911"></a><a name="zh-cn_topic_0000002518364037_p51171856122911"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="51%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518364037_p8749213301"><a name="zh-cn_topic_0000002518364037_p8749213301"></a><a name="zh-cn_topic_0000002518364037_p8749213301"></a>双通情况下指定数据的发送端设备。</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518364037_p1111617286312"><a name="zh-cn_topic_0000002518364037_p1111617286312"></a><a name="zh-cn_topic_0000002518364037_p1111617286312"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518364037_row0768150172913"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518364037_p1276995012296"><a name="zh-cn_topic_0000002518364037_p1276995012296"></a><a name="zh-cn_topic_0000002518364037_p1276995012296"></a>handler</p>
</td>
<td class="cellrowborder" valign="top" width="51%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518364037_p167691750142914"><a name="zh-cn_topic_0000002518364037_p167691750142914"></a><a name="zh-cn_topic_0000002518364037_p167691750142914"></a>处理数据的函数指针类型，详见“<a href="#ZH-CN_TOPIC_0000002491178396">ux_api_ble_msg_handler</a>”章节。</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518364037_p476925010299"><a name="zh-cn_topic_0000002518364037_p476925010299"></a><a name="zh-cn_topic_0000002518364037_p476925010299"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002518364037_table126118411962"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002518364037_row1262214115619"><th class="cellrowborder" valign="top" width="22.74227422742274%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002518364037_p13622141168"><a name="zh-cn_topic_0000002518364037_p13622141168"></a><a name="zh-cn_topic_0000002518364037_p13622141168"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="53.91539153915391%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002518364037_p15622194116620"><a name="zh-cn_topic_0000002518364037_p15622194116620"></a><a name="zh-cn_topic_0000002518364037_p15622194116620"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.342334233423344%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002518364037_p2062214411264"><a name="zh-cn_topic_0000002518364037_p2062214411264"></a><a name="zh-cn_topic_0000002518364037_p2062214411264"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002518364037_row146221411366"><td class="cellrowborder" valign="top" width="22.74227422742274%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002518364037_p4622164111614"><a name="zh-cn_topic_0000002518364037_p4622164111614"></a><a name="zh-cn_topic_0000002518364037_p4622164111614"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="53.91539153915391%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002518364037_p5176889718"><a name="zh-cn_topic_0000002518364037_p5176889718"></a><a name="zh-cn_topic_0000002518364037_p5176889718"></a>0：成功；</p>
<p id="zh-cn_topic_0000002518364037_p1662214416615"><a name="zh-cn_topic_0000002518364037_p1662214416615"></a><a name="zh-cn_topic_0000002518364037_p1662214416615"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="23.342334233423344%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002518364037_p13622204110618"><a name="zh-cn_topic_0000002518364037_p13622204110618"></a><a name="zh-cn_topic_0000002518364037_p13622204110618"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

#### 开发指导<a name="ZH-CN_TOPIC_0000002491178390"></a>

1.  调用ux\_api\_ble\_service\_register\_msg\_handler接口，客户可以注册特定的函数，用于处理其他设备通过数传通道发来的数据。
2.  调用ux\_api\_ble\_service\_send\_data接口，客户可向数传通道的对端发送数据。

## 蓝牙音频<a name="ZH-CN_TOPIC_0000002523178165"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002491178394)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002491178400)**  

### 数据结构<a name="ZH-CN_TOPIC_0000002491178394"></a>

-   **[ux\_api\_bt\_control\_volume\_t](#ZH-CN_TOPIC_0000002491178392)**  

-   **[ux\_api\_bt\_control\_play\_t](#ZH-CN_TOPIC_0000002491018432)**  

-   **[ux\_api\_bt\_volume\_type\_t](#ZH-CN_TOPIC_0000002523178181)**  

-   **[ux\_api\_bt\_control\_volume\_info\_t](#ZH-CN_TOPIC_0000002523178169)**  

-   **[ux\_api\_bt\_control\_call\_t](#ZH-CN_TOPIC_0000002523178173)**  

-   **[ux\_api\_bt\_control\_call\_info\_t](#ZH-CN_TOPIC_0000002491178404)**  

#### ux\_api\_bt\_control\_volume\_t<a name="ZH-CN_TOPIC_0000002491178392"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt\_control.h

【枚举说明】

音量控制的枚举定义。

【枚举定义】

```
typedef enum {   
    UX_API_BT_CONTROL_VOLUME_UP = 0,       /* 音量增加一档 */      
    UX_API_BT_CONTROL_VOLUME_DOWN,         /* 音量降低一档 */  
    UX_API_BT_CONTROL_VOLUME_MUTE,         /* 静音 */     
    UX_API_BT_CONTROL_VOLUME_SET_ABSOLUTE, /* 设置绝对音量 */  
    UX_API_BT_CONTROL_VOLUME_MAX,          /* 无效枚举 */ 
} ux_api_bt_control_volume_t;
```

#### ux\_api\_bt\_control\_play\_t<a name="ZH-CN_TOPIC_0000002491018432"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt\_control.h

【枚举说明】

音乐播放控制的枚举定义。

【枚举定义】

```
typedef enum {    
    UX_API_BT_CONTROL_PLAY_START = 0,    /* 继续播放 */     
    UX_API_BT_CONTROL_PLAY_STOP,         /* 停止播放 */     
    UX_API_BT_CONTROL_PLAY_PAUSE,        /* 暂停播放 */    
    UX_API_BT_CONTROL_PLAY_NEXT,         /* 播放下一首 */     
    UX_API_BT_CONTROL_PLAY_PRE,          /* 播放上一首 */     
    UX_API_BT_CONTROL_PLAY_REWIND,       /* 从头开始播放 */
    UX_API_BT_CONTROL_PLAY_FAST_FORWARD, /* 快进 */    
    UX_API_BT_CONTROL_PLAY_MAX,          /* 无效枚举 */
} ux_api_bt_control_play_t;
```

#### ux\_api\_bt\_volume\_type\_t<a name="ZH-CN_TOPIC_0000002523178181"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt\_control.h

【枚举说明】

音量控制类型的枚举定义。

【枚举定义】

```
typedef enum {    
    UX_API_BT_VOLUME_TYPE_MIC = 0, /* 调整麦克风增益 */    
    UX_API_BT_VOLUME_TYPE_SPEAKER, /* 调整扬声器增益 */
    UX_API_BT_VOLUME_TYPE_MAX,
} ux_api_bt_volume_type_t;
```

#### ux\_api\_bt\_control\_volume\_info\_t<a name="ZH-CN_TOPIC_0000002523178169"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt\_control.h

【结构体说明】

音量控制请求参数的结构体。

【结构体定义】

```
typedef struct {    
    ux_api_bt_volume_type_t volume_type; /* 音频设备类型 */       
    ux_api_bt_control_volume_t control;  /* 音量控制请求 */    
    uint8_t volume;                      /* 音量, ux_api_bt_control_volume_t为UX_API_BT_CONTROL_VOLUME_SET_ABSOLUTE时生效,avrcp范围为[0,0x7F], hfp范围为[0,15] */
} ux_api_bt_control_volume_info_t;
```

#### ux\_api\_bt\_control\_call\_t<a name="ZH-CN_TOPIC_0000002523178173"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt\_control.h

【枚举说明】

通话控制类型的枚举定义。

【枚举定义】

```
typedef enum {
    UX_API_BT_CONTROL_CALL_DAILING = 0, /* 呼出请求，入参param为手机号,param_len为手机号长度 */
    UX_API_BT_CONTROL_CALL_ANSWER,  /* 接听请求，入参param为空，param_len为0 */
    UX_API_BT_CONTROL_CALL_HANGUP,  /* 挂断请求，入参param为空，param_len为0 */
    UX_API_BT_CONTROL_CALL_REJECT,  /* 拒接请求，入参param为空,param_len为0 */
    UX_API_BT_CONTROL_CALL_HOLD,    /* 保持当前通话请求，入参param为空，param_len为0 */
    UX_API_BT_CONTROL_CALL_RECORD,  /*!< 通话信息请求，入参param为空,param_len为0  */
    UX_API_BT_CONTROL_CALL_MAX,
} ux_api_bt_control_call_t;
```

#### ux\_api\_bt\_control\_call\_info\_t<a name="ZH-CN_TOPIC_0000002491178404"></a>

【头文件】

application/ux/interfaces/solution/bg/ux\_api\_bt\_control.h

【结构体说明】

通话控制参数的结构体定义。

【结构体定义】

```
typedef struct {
    uint8_t call_id;                    // 通话id, 为三方通话预留, 默认填0
    ux_api_bt_control_call_t request;   // 通话控制请求
    uint8_t *param;                     // 通话控制参数, 参考 @ref ux_api_bt_control_call_t
    uint16_t param_len;                 // 通话控制参数长度
} ux_api_bt_control_call_info_t;
```

### 函数接口<a name="ZH-CN_TOPIC_0000002491178400"></a>

-   **[ux\_api\_bt\_audio\_start](#ZH-CN_TOPIC_0000002491018418)**  

-   **[ux\_api\_bt\_audio\_stop](#ZH-CN_TOPIC_0000002491178398)**  

-   **[ux\_api\_bt\_control\_volume](#ZH-CN_TOPIC_0000002523338141)**  

-   **[ux\_api\_bt\_control\_play](#ZH-CN_TOPIC_0000002491018426)**  

-   **[ux\_api\_bt\_control\_call](#ZH-CN_TOPIC_0000002523338129)**  

#### ux\_api\_bt\_audio\_start<a name="ZH-CN_TOPIC_0000002491018418"></a>

【描述】

主动启动蓝牙音频流传输。

【接口定义】

```
errcode_t ux_api_bt_audio_start(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr);
```

【参数】

<a name="zh-cn_topic_0000002487931866_table480911141240"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002487931866_row684551415413"><th class="cellrowborder" valign="top" width="23.482348234823483%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002487931866_p184571413416"><a name="zh-cn_topic_0000002487931866_p184571413416"></a><a name="zh-cn_topic_0000002487931866_p184571413416"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="43.18431843184319%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002487931866_p084518141645"><a name="zh-cn_topic_0000002487931866_p084518141645"></a><a name="zh-cn_topic_0000002487931866_p084518141645"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002487931866_p4845111414417"><a name="zh-cn_topic_0000002487931866_p4845111414417"></a><a name="zh-cn_topic_0000002487931866_p4845111414417"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002487931866_row198451314641"><td class="cellrowborder" valign="top" width="23.482348234823483%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487931866_p13845314649"><a name="zh-cn_topic_0000002487931866_p13845314649"></a><a name="zh-cn_topic_0000002487931866_p13845314649"></a>profile</p>
</td>
<td class="cellrowborder" valign="top" width="43.18431843184319%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487931866_p84552281349"><a name="zh-cn_topic_0000002487931866_p84552281349"></a><a name="zh-cn_topic_0000002487931866_p84552281349"></a>蓝牙音频服务类型：</p>
<p id="zh-cn_topic_0000002487931866_p126539544372"><a name="zh-cn_topic_0000002487931866_p126539544372"></a><a name="zh-cn_topic_0000002487931866_p126539544372"></a>UX_API_BT_PROFILE_A2DP（音频流）；</p>
<p id="zh-cn_topic_0000002487931866_p198459141441"><a name="zh-cn_topic_0000002487931866_p198459141441"></a><a name="zh-cn_topic_0000002487931866_p198459141441"></a>UX_API_BT_PROFILE_SCO（语音通话）。</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487931866_p1984501418417"><a name="zh-cn_topic_0000002487931866_p1984501418417"></a><a name="zh-cn_topic_0000002487931866_p1984501418417"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002487931866_row884513141743"><td class="cellrowborder" valign="top" width="23.482348234823483%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487931866_p9845181420411"><a name="zh-cn_topic_0000002487931866_p9845181420411"></a><a name="zh-cn_topic_0000002487931866_p9845181420411"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="43.18431843184319%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487931866_p9845101412418"><a name="zh-cn_topic_0000002487931866_p9845101412418"></a><a name="zh-cn_topic_0000002487931866_p9845101412418"></a>目标设备地址（地址成员为小端序）。</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487931866_p11845121410413"><a name="zh-cn_topic_0000002487931866_p11845121410413"></a><a name="zh-cn_topic_0000002487931866_p11845121410413"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002487931866_table126118411962"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002487931866_row1262214115619"><th class="cellrowborder" valign="top" width="22.74227422742274%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002487931866_p13622141168"><a name="zh-cn_topic_0000002487931866_p13622141168"></a><a name="zh-cn_topic_0000002487931866_p13622141168"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="53.91539153915391%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002487931866_p15622194116620"><a name="zh-cn_topic_0000002487931866_p15622194116620"></a><a name="zh-cn_topic_0000002487931866_p15622194116620"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.342334233423344%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002487931866_p2062214411264"><a name="zh-cn_topic_0000002487931866_p2062214411264"></a><a name="zh-cn_topic_0000002487931866_p2062214411264"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002487931866_row146221411366"><td class="cellrowborder" valign="top" width="22.74227422742274%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487931866_p4622164111614"><a name="zh-cn_topic_0000002487931866_p4622164111614"></a><a name="zh-cn_topic_0000002487931866_p4622164111614"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="53.91539153915391%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487931866_p5176889718"><a name="zh-cn_topic_0000002487931866_p5176889718"></a><a name="zh-cn_topic_0000002487931866_p5176889718"></a>0：成功；</p>
<p id="zh-cn_topic_0000002487931866_p1662214416615"><a name="zh-cn_topic_0000002487931866_p1662214416615"></a><a name="zh-cn_topic_0000002487931866_p1662214416615"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="23.342334233423344%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487931866_p13622204110618"><a name="zh-cn_topic_0000002487931866_p13622204110618"></a><a name="zh-cn_topic_0000002487931866_p13622204110618"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

需确保目标设备已连接且支持指定的profile。

#### ux\_api\_bt\_audio\_stop<a name="ZH-CN_TOPIC_0000002491178398"></a>

【描述】

主动停止蓝牙音频流传输。

【接口定义】

```
errcode_t ux_api_bt_audio_stop(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr);
```

【参数】

<a name="zh-cn_topic_0000002520211715_table2527516513"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002520211715_row1862551957"><th class="cellrowborder" valign="top" width="23.482348234823483%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002520211715_p10622511859"><a name="zh-cn_topic_0000002520211715_p10622511859"></a><a name="zh-cn_topic_0000002520211715_p10622511859"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="43.18431843184319%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002520211715_p26215511553"><a name="zh-cn_topic_0000002520211715_p26215511553"></a><a name="zh-cn_topic_0000002520211715_p26215511553"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002520211715_p146210511652"><a name="zh-cn_topic_0000002520211715_p146210511652"></a><a name="zh-cn_topic_0000002520211715_p146210511652"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002520211715_row13631151752"><td class="cellrowborder" valign="top" width="23.482348234823483%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520211715_p16345110515"><a name="zh-cn_topic_0000002520211715_p16345110515"></a><a name="zh-cn_topic_0000002520211715_p16345110515"></a>profile</p>
</td>
<td class="cellrowborder" valign="top" width="43.18431843184319%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520211715_p1075401567"><a name="zh-cn_topic_0000002520211715_p1075401567"></a><a name="zh-cn_topic_0000002520211715_p1075401567"></a>蓝牙音频服务类型：</p>
<p id="zh-cn_topic_0000002520211715_p11138612143812"><a name="zh-cn_topic_0000002520211715_p11138612143812"></a><a name="zh-cn_topic_0000002520211715_p11138612143812"></a>UX_API_BT_PROFILE_A2DP（音频流）；</p>
<p id="zh-cn_topic_0000002520211715_p1563551259"><a name="zh-cn_topic_0000002520211715_p1563551259"></a><a name="zh-cn_topic_0000002520211715_p1563551259"></a>UX_API_BT_PROFILE_SCO（语音通话）。</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520211715_p763145111518"><a name="zh-cn_topic_0000002520211715_p763145111518"></a><a name="zh-cn_topic_0000002520211715_p763145111518"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002520211715_row206395115513"><td class="cellrowborder" valign="top" width="23.482348234823483%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520211715_p13631651951"><a name="zh-cn_topic_0000002520211715_p13631651951"></a><a name="zh-cn_topic_0000002520211715_p13631651951"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="43.18431843184319%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520211715_p17636515518"><a name="zh-cn_topic_0000002520211715_p17636515518"></a><a name="zh-cn_topic_0000002520211715_p17636515518"></a>目标设备地址（地址为小端序）。</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520211715_p563155114519"><a name="zh-cn_topic_0000002520211715_p563155114519"></a><a name="zh-cn_topic_0000002520211715_p563155114519"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002520211715_table2535511054"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002520211715_row46316511852"><th class="cellrowborder" valign="top" width="23.112311231123112%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002520211715_p136311515511"><a name="zh-cn_topic_0000002520211715_p136311515511"></a><a name="zh-cn_topic_0000002520211715_p136311515511"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="43.55435543554355%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002520211715_p12634511050"><a name="zh-cn_topic_0000002520211715_p12634511050"></a><a name="zh-cn_topic_0000002520211715_p12634511050"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002520211715_p363165114516"><a name="zh-cn_topic_0000002520211715_p363165114516"></a><a name="zh-cn_topic_0000002520211715_p363165114516"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002520211715_row663051551"><td class="cellrowborder" valign="top" width="23.112311231123112%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520211715_p66319511956"><a name="zh-cn_topic_0000002520211715_p66319511956"></a><a name="zh-cn_topic_0000002520211715_p66319511956"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="43.55435543554355%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520211715_p4676143120613"><a name="zh-cn_topic_0000002520211715_p4676143120613"></a><a name="zh-cn_topic_0000002520211715_p4676143120613"></a>0：停止成功；</p>
<p id="zh-cn_topic_0000002520211715_p1963155110512"><a name="zh-cn_topic_0000002520211715_p1963155110512"></a><a name="zh-cn_topic_0000002520211715_p1963155110512"></a>非0：失败，错误码参见 errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520211715_p46316518515"><a name="zh-cn_topic_0000002520211715_p46316518515"></a><a name="zh-cn_topic_0000002520211715_p46316518515"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

若音频流未启动，调用此接口会返回错误。

#### ux\_api\_bt\_control\_volume<a name="ZH-CN_TOPIC_0000002523338141"></a>

【描述】

通过蓝牙控制服务链路调节对端设备音量。

【接口定义】

```
errcode_t ux_api_bt_control_volume(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr, const ux_api_bt_control_volume_info_t *volume_control_info);
```

【参数】

<a name="zh-cn_topic_0000002520131685_table27521421669"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002520131685_row4800134212620"><th class="cellrowborder" valign="top" width="19.561956195619562%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002520131685_p13800442765"><a name="zh-cn_topic_0000002520131685_p13800442765"></a><a name="zh-cn_topic_0000002520131685_p13800442765"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="59.565956595659564%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002520131685_p12813104210617"><a name="zh-cn_topic_0000002520131685_p12813104210617"></a><a name="zh-cn_topic_0000002520131685_p12813104210617"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.87208720872087%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002520131685_p18132421362"><a name="zh-cn_topic_0000002520131685_p18132421362"></a><a name="zh-cn_topic_0000002520131685_p18132421362"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002520131685_row081318421866"><td class="cellrowborder" valign="top" width="19.561956195619562%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520131685_p178131842367"><a name="zh-cn_topic_0000002520131685_p178131842367"></a><a name="zh-cn_topic_0000002520131685_p178131842367"></a>profile</p>
</td>
<td class="cellrowborder" valign="top" width="59.565956595659564%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520131685_p1185513418102"><a name="zh-cn_topic_0000002520131685_p1185513418102"></a><a name="zh-cn_topic_0000002520131685_p1185513418102"></a>蓝牙控制服务类型：</p>
<p id="zh-cn_topic_0000002520131685_p8201183812106"><a name="zh-cn_topic_0000002520131685_p8201183812106"></a><a name="zh-cn_topic_0000002520131685_p8201183812106"></a>UX_API_BT_PROFILE_AVRCP（媒体音量）；</p>
<p id="zh-cn_topic_0000002520131685_p158132425619"><a name="zh-cn_topic_0000002520131685_p158132425619"></a><a name="zh-cn_topic_0000002520131685_p158132425619"></a>UX_API_BT_PROFILE_HFP（通话音量）。</p>
</td>
<td class="cellrowborder" valign="top" width="20.87208720872087%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520131685_p3813194217615"><a name="zh-cn_topic_0000002520131685_p3813194217615"></a><a name="zh-cn_topic_0000002520131685_p3813194217615"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002520131685_row581313421563"><td class="cellrowborder" valign="top" width="19.561956195619562%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520131685_p58131942868"><a name="zh-cn_topic_0000002520131685_p58131942868"></a><a name="zh-cn_topic_0000002520131685_p58131942868"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="59.565956595659564%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520131685_p28136421360"><a name="zh-cn_topic_0000002520131685_p28136421360"></a><a name="zh-cn_topic_0000002520131685_p28136421360"></a>目标设备地址（地址成员为小端序）。</p>
</td>
<td class="cellrowborder" valign="top" width="20.87208720872087%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520131685_p14813184220615"><a name="zh-cn_topic_0000002520131685_p14813184220615"></a><a name="zh-cn_topic_0000002520131685_p14813184220615"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002520131685_row1981354220619"><td class="cellrowborder" valign="top" width="19.561956195619562%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520131685_p10813144217616"><a name="zh-cn_topic_0000002520131685_p10813144217616"></a><a name="zh-cn_topic_0000002520131685_p10813144217616"></a>volume_control_info</p>
</td>
<td class="cellrowborder" valign="top" width="59.565956595659564%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520131685_p84191757867"><a name="zh-cn_topic_0000002520131685_p84191757867"></a><a name="zh-cn_topic_0000002520131685_p84191757867"></a>音量控制信息（结构体指针），详见“<a href="#ZH-CN_TOPIC_0000002523178169">ux_api_bt_control_volume_info_t</a>”：</p>
<p id="zh-cn_topic_0000002520131685_p11631411175"><a name="zh-cn_topic_0000002520131685_p11631411175"></a><a name="zh-cn_topic_0000002520131685_p11631411175"></a>volume_type：音频设备类型（麦克风/扬声器）；</p>
<p id="zh-cn_topic_0000002520131685_p16408173375"><a name="zh-cn_topic_0000002520131685_p16408173375"></a><a name="zh-cn_topic_0000002520131685_p16408173375"></a>control：音量操作类型（增减/静音/设绝对值）；</p>
<p id="zh-cn_topic_0000002520131685_p14813742862"><a name="zh-cn_topic_0000002520131685_p14813742862"></a><a name="zh-cn_topic_0000002520131685_p14813742862"></a>volume：绝对音量值（仅当control = UX_API_BT_CONTROL_VOLUME_SET_ABSOLUTE时生效）。</p>
</td>
<td class="cellrowborder" valign="top" width="20.87208720872087%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520131685_p68131420610"><a name="zh-cn_topic_0000002520131685_p68131420610"></a><a name="zh-cn_topic_0000002520131685_p68131420610"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002520131685_table275434217619"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002520131685_row11813134216611"><th class="cellrowborder" valign="top" width="19.701970197019705%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002520131685_p28136429616"><a name="zh-cn_topic_0000002520131685_p28136429616"></a><a name="zh-cn_topic_0000002520131685_p28136429616"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="59.405940594059416%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002520131685_p381317429617"><a name="zh-cn_topic_0000002520131685_p381317429617"></a><a name="zh-cn_topic_0000002520131685_p381317429617"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.892089208920893%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002520131685_p1481315427610"><a name="zh-cn_topic_0000002520131685_p1481315427610"></a><a name="zh-cn_topic_0000002520131685_p1481315427610"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002520131685_row188133421610"><td class="cellrowborder" valign="top" width="19.701970197019705%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520131685_p481317421762"><a name="zh-cn_topic_0000002520131685_p481317421762"></a><a name="zh-cn_topic_0000002520131685_p481317421762"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="59.405940594059416%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520131685_p350919343153"><a name="zh-cn_topic_0000002520131685_p350919343153"></a><a name="zh-cn_topic_0000002520131685_p350919343153"></a>0：操作成功；</p>
<p id="zh-cn_topic_0000002520131685_p38139421368"><a name="zh-cn_topic_0000002520131685_p38139421368"></a><a name="zh-cn_topic_0000002520131685_p38139421368"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="20.892089208920893%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520131685_p18131442467"><a name="zh-cn_topic_0000002520131685_p18131442467"></a><a name="zh-cn_topic_0000002520131685_p18131442467"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

-   AVRCP音量范围：0～0x7F，HFP音量范围：0～15。
-   静音操作（UX\_API\_BT\_CONTROL\_VOLUME\_MUTE）会覆盖当前音量状态。

【消息上报】

**音量控制消息**

在AVRCP链路已经建立的前提下（以手机连接蓝牙手表为例，AVRCP的连接通常由手机主动发起），当对端（以手机侧为例）发起音量控制时，蓝牙协议栈会通过回调函数的方式上报音量控制事件（音量+、音量-、调节绝对音量）。

UX模块在启动时已注册了回调函数，对于AVRCP的音量控制相关事件，UX模块会通过发送软件消息的方式通知上层native-app，具体是通过SendMsgToUIService将BT\_AVRCP\_TG\_VOLUME\_CONTROL消息发送到ui\_service（见位于“application/ux/adapter/bg/watch/”目录的ux\_adapter\_bt\_avrcp\_tg\_volume\_control\_process函数），ui\_service中会通过ServiceMsgProcess函数初步处理该消息，在该函数的BT\_AVRCP\_TG\_VOLUME\_CONTROL case中，会通过抛事件的方式抛到GraphicService处理函数。

最终在GraphicService中，会使用AvrcpTgVolumeControlProcess函数来详细处理BT\_AVRCP\_TG\_VOLUME\_CONTROL消息。如果需要定制修改AVRCP音量控制的处理，请修改AvrcpTgVolumeControlProcess函数。

<a name="zh-cn_topic_0000002520131685_table019231151312"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002520131685_row1919221121316"><th class="cellrowborder" valign="top" width="18.3018301830183%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002520131685_p1219211117134"><a name="zh-cn_topic_0000002520131685_p1219211117134"></a><a name="zh-cn_topic_0000002520131685_p1219211117134"></a>消息定义</p>
</th>
<th class="cellrowborder" valign="top" width="26.292629262926294%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002520131685_p719201111320"><a name="zh-cn_topic_0000002520131685_p719201111320"></a><a name="zh-cn_topic_0000002520131685_p719201111320"></a>消息处理函数</p>
</th>
<th class="cellrowborder" valign="top" width="55.40554055405541%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002520131685_p12192191114135"><a name="zh-cn_topic_0000002520131685_p12192191114135"></a><a name="zh-cn_topic_0000002520131685_p12192191114135"></a>消息处理函数参数</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002520131685_row719210116137"><td class="cellrowborder" valign="top" width="18.3018301830183%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002520131685_p319261116132"><a name="zh-cn_topic_0000002520131685_p319261116132"></a><a name="zh-cn_topic_0000002520131685_p319261116132"></a>BT_AVRCP_TG_VOLUME_CONTROL</p>
</td>
<td class="cellrowborder" valign="top" width="26.292629262926294%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002520131685_p9546165215530"><a name="zh-cn_topic_0000002520131685_p9546165215530"></a><a name="zh-cn_topic_0000002520131685_p9546165215530"></a>AvrcpTgVolumeControlProcess：音量控制处理。</p>
</td>
<td class="cellrowborder" valign="top" width="55.40554055405541%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002520131685_p187314217531"><a name="zh-cn_topic_0000002520131685_p187314217531"></a><a name="zh-cn_topic_0000002520131685_p187314217531"></a>uint8_t avrcp_volume：需要调节的音量，按协议标准，范围0～127（AVRCP协议指定的音量等级档位）。</p>
</td>
</tr>
</tbody>
</table>

#### ux\_api\_bt\_control\_play<a name="ZH-CN_TOPIC_0000002491018426"></a>

【描述】

通过蓝牙控制服务链路控制对端媒体播放。

【语法】

```
errcode_t ux_api_bt_control_play(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr, ux_api_bt_control_play_t play_control);
```

【参数】

<a name="zh-cn_topic_0000002488051844_table1881612417116"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002488051844_row782824116113"><th class="cellrowborder" valign="top" width="22.132213221322132%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002488051844_p158281541101111"><a name="zh-cn_topic_0000002488051844_p158281541101111"></a><a name="zh-cn_topic_0000002488051844_p158281541101111"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="57.175717571757176%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002488051844_p382854121119"><a name="zh-cn_topic_0000002488051844_p382854121119"></a><a name="zh-cn_topic_0000002488051844_p382854121119"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.692069206920692%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002488051844_p98281741141112"><a name="zh-cn_topic_0000002488051844_p98281741141112"></a><a name="zh-cn_topic_0000002488051844_p98281741141112"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002488051844_row1882810414111"><td class="cellrowborder" valign="top" width="22.132213221322132%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002488051844_p2828114111118"><a name="zh-cn_topic_0000002488051844_p2828114111118"></a><a name="zh-cn_topic_0000002488051844_p2828114111118"></a>profile</p>
</td>
<td class="cellrowborder" valign="top" width="57.175717571757176%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002488051844_p2022014543111"><a name="zh-cn_topic_0000002488051844_p2022014543111"></a><a name="zh-cn_topic_0000002488051844_p2022014543111"></a>蓝牙控制服务类型：</p>
<p id="zh-cn_topic_0000002488051844_p48281841111110"><a name="zh-cn_topic_0000002488051844_p48281841111110"></a><a name="zh-cn_topic_0000002488051844_p48281841111110"></a>UX_API_BT_PROFILE_AVRCP（媒体控制）</p>
</td>
<td class="cellrowborder" valign="top" width="20.692069206920692%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002488051844_p0828241131114"><a name="zh-cn_topic_0000002488051844_p0828241131114"></a><a name="zh-cn_topic_0000002488051844_p0828241131114"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002488051844_row198284419116"><td class="cellrowborder" valign="top" width="22.132213221322132%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002488051844_p18828441131110"><a name="zh-cn_topic_0000002488051844_p18828441131110"></a><a name="zh-cn_topic_0000002488051844_p18828441131110"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="57.175717571757176%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002488051844_p13828124116115"><a name="zh-cn_topic_0000002488051844_p13828124116115"></a><a name="zh-cn_topic_0000002488051844_p13828124116115"></a>目标设备地址（地址成员为小端序）。</p>
</td>
<td class="cellrowborder" valign="top" width="20.692069206920692%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002488051844_p108281841161113"><a name="zh-cn_topic_0000002488051844_p108281841161113"></a><a name="zh-cn_topic_0000002488051844_p108281841161113"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002488051844_row1682819410114"><td class="cellrowborder" valign="top" width="22.132213221322132%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002488051844_p138281541121117"><a name="zh-cn_topic_0000002488051844_p138281541121117"></a><a name="zh-cn_topic_0000002488051844_p138281541121117"></a>play_control</p>
</td>
<td class="cellrowborder" valign="top" width="57.175717571757176%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002488051844_p1592212814129"><a name="zh-cn_topic_0000002488051844_p1592212814129"></a><a name="zh-cn_topic_0000002488051844_p1592212814129"></a>播放控制类型：UX_API_BT_CONTROL_PLAY_START（播放）、UX_API_BT_CONTROL_PLAY_STOP（停止）、UX_API_BT_CONTROL_PLAY_PAUSE（暂停）、UX_API_BT_CONTROL_PLAY_NEXT（下一首）</p>
<p id="zh-cn_topic_0000002488051844_p4828184110116"><a name="zh-cn_topic_0000002488051844_p4828184110116"></a><a name="zh-cn_topic_0000002488051844_p4828184110116"></a>等（详见“<a href="#ZH-CN_TOPIC_0000002491018432">ux_api_bt_control_play_t</a>”）。</p>
</td>
<td class="cellrowborder" valign="top" width="20.692069206920692%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002488051844_p28281341101110"><a name="zh-cn_topic_0000002488051844_p28281341101110"></a><a name="zh-cn_topic_0000002488051844_p28281341101110"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002488051844_table9817441171111"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002488051844_row2828104101116"><th class="cellrowborder" valign="top" width="22.742274227422744%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002488051844_p1782894111116"><a name="zh-cn_topic_0000002488051844_p1782894111116"></a><a name="zh-cn_topic_0000002488051844_p1782894111116"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="57.78577857785779%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002488051844_p982844101112"><a name="zh-cn_topic_0000002488051844_p982844101112"></a><a name="zh-cn_topic_0000002488051844_p982844101112"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="19.471947194719476%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002488051844_p1882819413115"><a name="zh-cn_topic_0000002488051844_p1882819413115"></a><a name="zh-cn_topic_0000002488051844_p1882819413115"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002488051844_row10828641131116"><td class="cellrowborder" valign="top" width="22.742274227422744%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002488051844_p108281641121110"><a name="zh-cn_topic_0000002488051844_p108281641121110"></a><a name="zh-cn_topic_0000002488051844_p108281641121110"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="57.78577857785779%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002488051844_p1221874961110"><a name="zh-cn_topic_0000002488051844_p1221874961110"></a><a name="zh-cn_topic_0000002488051844_p1221874961110"></a>0：操作成功；</p>
<p id="zh-cn_topic_0000002488051844_p5828204191118"><a name="zh-cn_topic_0000002488051844_p5828204191118"></a><a name="zh-cn_topic_0000002488051844_p5828204191118"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="19.471947194719476%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002488051844_p1882814418111"><a name="zh-cn_topic_0000002488051844_p1882814418111"></a><a name="zh-cn_topic_0000002488051844_p1882814418111"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

仅当对端设备处于可控制状态（如播放中/暂停）时操作有效。

【消息上报】

**媒体控制消息**

在AVRCP链路已经建立的前提下（以手机连接蓝牙手表为例，AVRCP的连接通常由手机主动发起），当对端（以手机侧为例）发起媒体控制请求（播放、暂停、上一首、下一首）时，蓝牙协议栈会通过回调函数的方式上报媒体控制事件。

UX模块在启动时已注册了回调函数，对于AVRCP的媒体控制相关事件，UX模块会通过发送软件消息的方式通知上层native-app，具体是通过SendMsgToUIService将BT\_AVRCP\_TG\_PLAY\_CONTROL消息发送到ui\_service（见位于“application/ux/adapter/bg/watch/”目录的ux\_adapter\_bt\_avrcp\_tg\_play\_control\_process函数），ui\_service中会通过ServiceMsgProcess函数初步处理该消息，在该函数的BT\_AVRCP\_TG\_PLAY\_CONTROL case中，会通过抛事件的方式抛到GraphicService处理函数。

最终在GraphicService中，会使用AvrcpTgPlayControlProcess函数来详细处理BT\_AVRCP\_TG\_PLAY\_CONTROL消息。如果您需要定制修改AVRCP媒体控制的处理，请修改AvrcpTgPlayControlProcess函数。

<a name="zh-cn_topic_0000002488051844_table019231151312"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002488051844_row1919221121316"><th class="cellrowborder" valign="top" width="18.3018301830183%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002488051844_p1219211117134"><a name="zh-cn_topic_0000002488051844_p1219211117134"></a><a name="zh-cn_topic_0000002488051844_p1219211117134"></a>消息定义</p>
</th>
<th class="cellrowborder" valign="top" width="26.292629262926294%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002488051844_p719201111320"><a name="zh-cn_topic_0000002488051844_p719201111320"></a><a name="zh-cn_topic_0000002488051844_p719201111320"></a>消息处理函数</p>
</th>
<th class="cellrowborder" valign="top" width="55.40554055405541%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002488051844_p12192191114135"><a name="zh-cn_topic_0000002488051844_p12192191114135"></a><a name="zh-cn_topic_0000002488051844_p12192191114135"></a>消息处理函数参数</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002488051844_row719210116137"><td class="cellrowborder" valign="top" width="18.3018301830183%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002488051844_p3871172312588"><a name="zh-cn_topic_0000002488051844_p3871172312588"></a><a name="zh-cn_topic_0000002488051844_p3871172312588"></a>BT_AVRCP_TG_PLAY_CONTROL</p>
</td>
<td class="cellrowborder" valign="top" width="26.292629262926294%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002488051844_p2077973855815"><a name="zh-cn_topic_0000002488051844_p2077973855815"></a><a name="zh-cn_topic_0000002488051844_p2077973855815"></a>AvrcpTgPlayControlProcess：媒体控制处理。</p>
</td>
<td class="cellrowborder" valign="top" width="55.40554055405541%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002488051844_p353154635819"><a name="zh-cn_topic_0000002488051844_p353154635819"></a><a name="zh-cn_topic_0000002488051844_p353154635819"></a>const bd_addr_t&<strong id="zh-cn_topic_0000002488051844_b13864194424716"><a name="zh-cn_topic_0000002488051844_b13864194424716"></a><a name="zh-cn_topic_0000002488051844_b13864194424716"></a> </strong>bdAddr：设备地址。</p>
<p id="zh-cn_topic_0000002488051844_p1653184695814"><a name="zh-cn_topic_0000002488051844_p1653184695814"></a><a name="zh-cn_topic_0000002488051844_p1653184695814"></a>ux_api_bt_control_play_t play_control：媒体控制消息，该结构请参见“<a href="#ZH-CN_TOPIC_0000002491018432">ux_api_bt_control_play_t</a>”的描述。</p>
</td>
</tr>
</tbody>
</table>

#### ux\_api\_bt\_control\_call<a name="ZH-CN_TOPIC_0000002523338129"></a>

【描述】

通过蓝牙控制服务链路控制对端（带CAT1卡）的通话。

【语法】

```
errcode_t ux_api_bt_control_call(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr, const ux_api_bt_control_call_info_t *call_control);
```

【参数】

<a name="zh-cn_topic_0000002487931868_table7964110131316"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002487931868_row0976121081317"><th class="cellrowborder" valign="top" width="21.52215221522152%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002487931868_p3976151071312"><a name="zh-cn_topic_0000002487931868_p3976151071312"></a><a name="zh-cn_topic_0000002487931868_p3976151071312"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="57.71577157715772%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002487931868_p129761106136"><a name="zh-cn_topic_0000002487931868_p129761106136"></a><a name="zh-cn_topic_0000002487931868_p129761106136"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.762076207620765%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002487931868_p69761310131311"><a name="zh-cn_topic_0000002487931868_p69761310131311"></a><a name="zh-cn_topic_0000002487931868_p69761310131311"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002487931868_row197671071318"><td class="cellrowborder" valign="top" width="21.52215221522152%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487931868_p3976181031315"><a name="zh-cn_topic_0000002487931868_p3976181031315"></a><a name="zh-cn_topic_0000002487931868_p3976181031315"></a>profile</p>
</td>
<td class="cellrowborder" valign="top" width="57.71577157715772%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487931868_p9412251201312"><a name="zh-cn_topic_0000002487931868_p9412251201312"></a><a name="zh-cn_topic_0000002487931868_p9412251201312"></a>蓝牙控制服务类型，UX_API_BT_PROFILE_HFP。</p>
</td>
<td class="cellrowborder" valign="top" width="20.762076207620765%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487931868_p1197614107132"><a name="zh-cn_topic_0000002487931868_p1197614107132"></a><a name="zh-cn_topic_0000002487931868_p1197614107132"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002487931868_row10976151012132"><td class="cellrowborder" valign="top" width="21.52215221522152%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487931868_p89768107137"><a name="zh-cn_topic_0000002487931868_p89768107137"></a><a name="zh-cn_topic_0000002487931868_p89768107137"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="57.71577157715772%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487931868_p1197641011313"><a name="zh-cn_topic_0000002487931868_p1197641011313"></a><a name="zh-cn_topic_0000002487931868_p1197641011313"></a>目标设备地址（地址成员为小端序）。</p>
</td>
<td class="cellrowborder" valign="top" width="20.762076207620765%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487931868_p7976111031315"><a name="zh-cn_topic_0000002487931868_p7976111031315"></a><a name="zh-cn_topic_0000002487931868_p7976111031315"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002487931868_row10976191013136"><td class="cellrowborder" valign="top" width="21.52215221522152%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487931868_p11977410171311"><a name="zh-cn_topic_0000002487931868_p11977410171311"></a><a name="zh-cn_topic_0000002487931868_p11977410171311"></a>call_control</p>
</td>
<td class="cellrowborder" valign="top" width="57.71577157715772%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487931868_p1593462714135"><a name="zh-cn_topic_0000002487931868_p1593462714135"></a><a name="zh-cn_topic_0000002487931868_p1593462714135"></a>通话控制信息（结构体指针），详见 “<a href="#ZH-CN_TOPIC_0000002491178404">ux_api_bt_control_call_info_t</a>”：</p>
<p id="zh-cn_topic_0000002487931868_p9821331111312"><a name="zh-cn_topic_0000002487931868_p9821331111312"></a><a name="zh-cn_topic_0000002487931868_p9821331111312"></a>call_id：通话ID（默认填0）；</p>
<p id="zh-cn_topic_0000002487931868_p1582817338134"><a name="zh-cn_topic_0000002487931868_p1582817338134"></a><a name="zh-cn_topic_0000002487931868_p1582817338134"></a>request：操作类型（拨号/接听/挂断等）；</p>
<p id="zh-cn_topic_0000002487931868_p1454793601312"><a name="zh-cn_topic_0000002487931868_p1454793601312"></a><a name="zh-cn_topic_0000002487931868_p1454793601312"></a>param：附加参数（如拨号时传入电话号码）；</p>
<p id="zh-cn_topic_0000002487931868_p14977101031313"><a name="zh-cn_topic_0000002487931868_p14977101031313"></a><a name="zh-cn_topic_0000002487931868_p14977101031313"></a>param_len：参数长度（单位：Byte）。</p>
</td>
<td class="cellrowborder" valign="top" width="20.762076207620765%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487931868_p18977151031319"><a name="zh-cn_topic_0000002487931868_p18977151031319"></a><a name="zh-cn_topic_0000002487931868_p18977151031319"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002487931868_table126118411962"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002487931868_row1262214115619"><th class="cellrowborder" valign="top" width="22.74227422742274%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002487931868_p13622141168"><a name="zh-cn_topic_0000002487931868_p13622141168"></a><a name="zh-cn_topic_0000002487931868_p13622141168"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="53.91539153915391%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002487931868_p15622194116620"><a name="zh-cn_topic_0000002487931868_p15622194116620"></a><a name="zh-cn_topic_0000002487931868_p15622194116620"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="23.342334233423344%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002487931868_p2062214411264"><a name="zh-cn_topic_0000002487931868_p2062214411264"></a><a name="zh-cn_topic_0000002487931868_p2062214411264"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002487931868_row146221411366"><td class="cellrowborder" valign="top" width="22.74227422742274%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487931868_p4622164111614"><a name="zh-cn_topic_0000002487931868_p4622164111614"></a><a name="zh-cn_topic_0000002487931868_p4622164111614"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="53.91539153915391%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487931868_p5176889718"><a name="zh-cn_topic_0000002487931868_p5176889718"></a><a name="zh-cn_topic_0000002487931868_p5176889718"></a>0：成功；</p>
<p id="zh-cn_topic_0000002487931868_p1662214416615"><a name="zh-cn_topic_0000002487931868_p1662214416615"></a><a name="zh-cn_topic_0000002487931868_p1662214416615"></a>非0：失败，错误码参见errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="23.342334233423344%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487931868_p13622204110618"><a name="zh-cn_topic_0000002487931868_p13622204110618"></a><a name="zh-cn_topic_0000002487931868_p13622204110618"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

拨号操作（UX\_API\_BT\_CONTROL\_CALL\_DAILING）需在param传入电话号码（如 "13800138000"）。

【消息上报】

**通话状态消息**

在HFP链路建立的前提下，当通话状态发生变化时（例如来电），蓝牙协议栈会通过回调函数上报通话状态变化事件，UX模块在启动时已注册了回调函数，因此，UX模块会在感知到通话状态发生变化时，通过发送软件消息的方式通知上层native-app，具体是通过SendMsgToUIService将BT\_CALL\_STATE\_CHANGE消息发送到ui\_service（见位于“application/ux/adapter/bg/watch/”目录的ux\_adapter\_bt\_call\_state\_process函数）。

ui\_service中会通过ServiceMsgProcess函数初步处理该消息，在该函数的BT\_CALL\_STATE\_CHANGE case中，会通过抛事件的方式抛到GraphicService处理函数。

最终在GraphicService中，会使用CallStateChangedProcess函数来详细处理BT\_CALL\_STATE\_CHANGE消息。如果您需要定制修改通话状态变化的处理，请修改CallStateChangedProcess函数。

<a name="zh-cn_topic_0000002487931868_table019231151312"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002487931868_row1919221121316"><th class="cellrowborder" valign="top" width="18.3018301830183%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002487931868_p1219211117134"><a name="zh-cn_topic_0000002487931868_p1219211117134"></a><a name="zh-cn_topic_0000002487931868_p1219211117134"></a>消息定义</p>
</th>
<th class="cellrowborder" valign="top" width="26.292629262926294%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002487931868_p719201111320"><a name="zh-cn_topic_0000002487931868_p719201111320"></a><a name="zh-cn_topic_0000002487931868_p719201111320"></a>消息处理函数</p>
</th>
<th class="cellrowborder" valign="top" width="55.40554055405541%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002487931868_p12192191114135"><a name="zh-cn_topic_0000002487931868_p12192191114135"></a><a name="zh-cn_topic_0000002487931868_p12192191114135"></a>消息处理函数参数</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002487931868_row719210116137"><td class="cellrowborder" valign="top" width="18.3018301830183%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002487931868_p115412565716"><a name="zh-cn_topic_0000002487931868_p115412565716"></a><a name="zh-cn_topic_0000002487931868_p115412565716"></a>BT_CALL_STATE_CHANGE</p>
</td>
<td class="cellrowborder" valign="top" width="26.292629262926294%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002487931868_p141374919401"><a name="zh-cn_topic_0000002487931868_p141374919401"></a><a name="zh-cn_topic_0000002487931868_p141374919401"></a>CallStateChangedProcess：通话状态变化后处理。</p>
</td>
<td class="cellrowborder" valign="top" width="55.40554055405541%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002487931868_p198351618204110"><a name="zh-cn_topic_0000002487931868_p198351618204110"></a><a name="zh-cn_topic_0000002487931868_p198351618204110"></a>const bd_addr_t& bdAddr：设备地址。</p>
<p id="zh-cn_topic_0000002487931868_p69772464113"><a name="zh-cn_topic_0000002487931868_p69772464113"></a><a name="zh-cn_topic_0000002487931868_p69772464113"></a>const PhoneHfpCallInfo &call<strong id="zh-cn_topic_0000002487931868_b125916512488"><a name="zh-cn_topic_0000002487931868_b125916512488"></a><a name="zh-cn_topic_0000002487931868_b125916512488"></a>：</strong>通话状态变化内容，详细见如下结构体，ServiceMsgProcess中会根据UX模块上报的消息填充该结构体。</p>
</td>
</tr>
</tbody>
</table>

const PhoneHfpCallInfo &call参数对应的结构体类型如下所示：

```
typedef struct {
    bd_addr_t bd_addr;   // 蓝牙地址
    int id;              // 通话编号
    int state;           // 通话状态
    char number[MAX_PHONE_NUM];  // 电话号码
    unsigned char number_len;    // 电话号码长度
    bt_uuid_t uuid;              // 保留兼容SDK旧版本
    bool multi_party;            // 是否多方电话
    bool outgoing;               // 是否呼出
    bool in_band_ring;           // 带内铃声或者带外铃声
    long creation_time;          // 保留兼容SDK旧版本
} PhoneHfpCallInfo;
```

# 缩略语<a name="ZH-CN_TOPIC_0000002523338137"></a>

<a name="zh-cn_topic_0000002518363947_table116918720118"></a>
<table><tbody><tr id="zh-cn_topic_0000002518363947_row3100127131118"><td class="nocellnorowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p15100207161112"><a name="zh-cn_topic_0000002518363947_p15100207161112"></a><a name="zh-cn_topic_0000002518363947_p15100207161112"></a><strong id="zh-cn_topic_0000002518363947_b131002751119"><a name="zh-cn_topic_0000002518363947_b131002751119"></a><a name="zh-cn_topic_0000002518363947_b131002751119"></a>A</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.57455745574558%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row68191117448"><td class="cellrowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p586113442"><a name="zh-cn_topic_0000002518363947_p586113442"></a><a name="zh-cn_topic_0000002518363947_p586113442"></a><strong id="zh-cn_topic_0000002518363947_b17873111174514"><a name="zh-cn_topic_0000002518363947_b17873111174514"></a><a name="zh-cn_topic_0000002518363947_b17873111174514"></a>A2DP</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.57455745574558%"><p id="zh-cn_topic_0000002518363947_p4101157191110"><a name="zh-cn_topic_0000002518363947_p4101157191110"></a><a name="zh-cn_topic_0000002518363947_p4101157191110"></a>Advanced Audio Distribution Profile</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002518363947_p118111194418"><a name="zh-cn_topic_0000002518363947_p118111194418"></a><a name="zh-cn_topic_0000002518363947_p118111194418"></a>高级音频分发协议</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row1210112719115"><td class="nocellnorowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p51012761118"><a name="zh-cn_topic_0000002518363947_p51012761118"></a><a name="zh-cn_topic_0000002518363947_p51012761118"></a><strong id="zh-cn_topic_0000002518363947_b310197131120"><a name="zh-cn_topic_0000002518363947_b310197131120"></a><a name="zh-cn_topic_0000002518363947_b310197131120"></a>API</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.57455745574558%"><p id="zh-cn_topic_0000002518363947_p41011973118"><a name="zh-cn_topic_0000002518363947_p41011973118"></a><a name="zh-cn_topic_0000002518363947_p41011973118"></a>Application Programming Interface</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002518363947_p210107201117"><a name="zh-cn_topic_0000002518363947_p210107201117"></a><a name="zh-cn_topic_0000002518363947_p210107201117"></a>应用编程接口</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row13101157111112"><td class="nocellnorowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p136841217148"><a name="zh-cn_topic_0000002518363947_p136841217148"></a><a name="zh-cn_topic_0000002518363947_p136841217148"></a><strong id="zh-cn_topic_0000002518363947_b1787415174513"><a name="zh-cn_topic_0000002518363947_b1787415174513"></a><a name="zh-cn_topic_0000002518363947_b1787415174513"></a>AVRCP</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.57455745574558%"><p id="zh-cn_topic_0000002518363947_p1570918815413"><a name="zh-cn_topic_0000002518363947_p1570918815413"></a><a name="zh-cn_topic_0000002518363947_p1570918815413"></a>Audio/Video Remote Control Profile</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002518363947_p1510197111113"><a name="zh-cn_topic_0000002518363947_p1510197111113"></a><a name="zh-cn_topic_0000002518363947_p1510197111113"></a>音视频远程控制协议</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row7362150307"><td class="cellrowborder" style="border:none" valign="top" width="21.09210921092109%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.57455745574558%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row310110791117"><td class="nocellnorowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p910110711118"><a name="zh-cn_topic_0000002518363947_p910110711118"></a><a name="zh-cn_topic_0000002518363947_p910110711118"></a><strong id="zh-cn_topic_0000002518363947_b31016751113"><a name="zh-cn_topic_0000002518363947_b31016751113"></a><a name="zh-cn_topic_0000002518363947_b31016751113"></a>B</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.57455745574558%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row1210119712114"><td class="nocellnorowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p31010721112"><a name="zh-cn_topic_0000002518363947_p31010721112"></a><a name="zh-cn_topic_0000002518363947_p31010721112"></a><strong id="zh-cn_topic_0000002518363947_b51012761118"><a name="zh-cn_topic_0000002518363947_b51012761118"></a><a name="zh-cn_topic_0000002518363947_b51012761118"></a>BLE</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.57455745574558%"><p id="zh-cn_topic_0000002518363947_p1510127121120"><a name="zh-cn_topic_0000002518363947_p1510127121120"></a><a name="zh-cn_topic_0000002518363947_p1510127121120"></a>Bluetooth Low Energy</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002518363947_p191013711110"><a name="zh-cn_topic_0000002518363947_p191013711110"></a><a name="zh-cn_topic_0000002518363947_p191013711110"></a>低功耗蓝牙</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row14784114317212"><td class="cellrowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p16784174312214"><a name="zh-cn_topic_0000002518363947_p16784174312214"></a><a name="zh-cn_topic_0000002518363947_p16784174312214"></a><strong id="zh-cn_topic_0000002518363947_b15872135062116"><a name="zh-cn_topic_0000002518363947_b15872135062116"></a><a name="zh-cn_topic_0000002518363947_b15872135062116"></a>BR</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.57455745574558%"><p id="zh-cn_topic_0000002518363947_p1578484312214"><a name="zh-cn_topic_0000002518363947_p1578484312214"></a><a name="zh-cn_topic_0000002518363947_p1578484312214"></a>Basic Rate</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002518363947_p77845438217"><a name="zh-cn_topic_0000002518363947_p77845438217"></a><a name="zh-cn_topic_0000002518363947_p77845438217"></a>经典蓝牙</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row14101167131115"><td class="nocellnorowborder" style="border:none" valign="top" width="21.09210921092109%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.57455745574558%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row11206741119"><td class="cellrowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p22061248114"><a name="zh-cn_topic_0000002518363947_p22061248114"></a><a name="zh-cn_topic_0000002518363947_p22061248114"></a><strong id="zh-cn_topic_0000002518363947_b88768114452"><a name="zh-cn_topic_0000002518363947_b88768114452"></a><a name="zh-cn_topic_0000002518363947_b88768114452"></a>H</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.57455745574558%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row3687771716"><td class="cellrowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p768747112"><a name="zh-cn_topic_0000002518363947_p768747112"></a><a name="zh-cn_topic_0000002518363947_p768747112"></a><strong id="zh-cn_topic_0000002518363947_b1487711114516"><a name="zh-cn_topic_0000002518363947_b1487711114516"></a><a name="zh-cn_topic_0000002518363947_b1487711114516"></a>HFP</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.57455745574558%"><p id="zh-cn_topic_0000002518363947_p66876715116"><a name="zh-cn_topic_0000002518363947_p66876715116"></a><a name="zh-cn_topic_0000002518363947_p66876715116"></a>Hands-Free Profile</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002518363947_p1968718713114"><a name="zh-cn_topic_0000002518363947_p1968718713114"></a><a name="zh-cn_topic_0000002518363947_p1968718713114"></a>免提协议</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row14121717718"><td class="cellrowborder" style="border:none" valign="top" width="21.09210921092109%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.57455745574558%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row198024916221"><td class="cellrowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p1780316922211"><a name="zh-cn_topic_0000002518363947_p1780316922211"></a><a name="zh-cn_topic_0000002518363947_p1780316922211"></a><strong id="zh-cn_topic_0000002518363947_b714211652219"><a name="zh-cn_topic_0000002518363947_b714211652219"></a><a name="zh-cn_topic_0000002518363947_b714211652219"></a>R</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.57455745574558%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row198031591227"><td class="cellrowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p13803159172210"><a name="zh-cn_topic_0000002518363947_p13803159172210"></a><a name="zh-cn_topic_0000002518363947_p13803159172210"></a><strong id="zh-cn_topic_0000002518363947_b14144131692213"><a name="zh-cn_topic_0000002518363947_b14144131692213"></a><a name="zh-cn_topic_0000002518363947_b14144131692213"></a>RSSI</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.57455745574558%"><p id="zh-cn_topic_0000002518363947_p168031796226"><a name="zh-cn_topic_0000002518363947_p168031796226"></a><a name="zh-cn_topic_0000002518363947_p168031796226"></a>Received Signal Strength Indicator</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002518363947_p0803999227"><a name="zh-cn_topic_0000002518363947_p0803999227"></a><a name="zh-cn_topic_0000002518363947_p0803999227"></a>接收信号强度指示</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row580310962214"><td class="cellrowborder" style="border:none" valign="top" width="21.09210921092109%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.57455745574558%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row5102137141112"><td class="nocellnorowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p410210771111"><a name="zh-cn_topic_0000002518363947_p410210771111"></a><a name="zh-cn_topic_0000002518363947_p410210771111"></a><strong id="zh-cn_topic_0000002518363947_b2010212718118"><a name="zh-cn_topic_0000002518363947_b2010212718118"></a><a name="zh-cn_topic_0000002518363947_b2010212718118"></a>S</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.57455745574558%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row410257151116"><td class="nocellnorowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p171021715118"><a name="zh-cn_topic_0000002518363947_p171021715118"></a><a name="zh-cn_topic_0000002518363947_p171021715118"></a><strong id="zh-cn_topic_0000002518363947_b61037714112"><a name="zh-cn_topic_0000002518363947_b61037714112"></a><a name="zh-cn_topic_0000002518363947_b61037714112"></a>SPP</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.57455745574558%"><p id="zh-cn_topic_0000002518363947_p31032771116"><a name="zh-cn_topic_0000002518363947_p31032771116"></a><a name="zh-cn_topic_0000002518363947_p31032771116"></a>Serial Port Profile</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002518363947_p31035741113"><a name="zh-cn_topic_0000002518363947_p31035741113"></a><a name="zh-cn_topic_0000002518363947_p31035741113"></a>串行端口配置</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row1729718387112"><td class="nocellnorowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p1629833815116"><a name="zh-cn_topic_0000002518363947_p1629833815116"></a><a name="zh-cn_topic_0000002518363947_p1629833815116"></a><strong id="zh-cn_topic_0000002518363947_b18791011453"><a name="zh-cn_topic_0000002518363947_b18791011453"></a><a name="zh-cn_topic_0000002518363947_b18791011453"></a>SCO</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.57455745574558%"><p id="zh-cn_topic_0000002518363947_p1129853841113"><a name="zh-cn_topic_0000002518363947_p1129853841113"></a><a name="zh-cn_topic_0000002518363947_p1129853841113"></a>Synchronous Connection-Oriented Link</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002518363947_p9298163818117"><a name="zh-cn_topic_0000002518363947_p9298163818117"></a><a name="zh-cn_topic_0000002518363947_p9298163818117"></a>同步面向连接链路</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row2541543909"><td class="cellrowborder" style="border:none" valign="top" width="21.09210921092109%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="45.57455745574558%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row329811385116"><td class="nocellnorowborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p9829941181114"><a name="zh-cn_topic_0000002518363947_p9829941181114"></a><a name="zh-cn_topic_0000002518363947_p9829941181114"></a><strong id="zh-cn_topic_0000002518363947_b1982954119116"><a name="zh-cn_topic_0000002518363947_b1982954119116"></a><a name="zh-cn_topic_0000002518363947_b1982954119116"></a>U</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.57455745574558%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002518363947_row13298163816113"><td class="row-nocellborder" style="border:none" valign="top" width="21.09210921092109%"><p id="zh-cn_topic_0000002518363947_p682974110115"><a name="zh-cn_topic_0000002518363947_p682974110115"></a><a name="zh-cn_topic_0000002518363947_p682974110115"></a><strong id="zh-cn_topic_0000002518363947_b13829241101113"><a name="zh-cn_topic_0000002518363947_b13829241101113"></a><a name="zh-cn_topic_0000002518363947_b13829241101113"></a>UX</strong></p>
</td>
<td class="row-nocellborder" style="border:none" valign="top" width="45.57455745574558%"><p id="zh-cn_topic_0000002518363947_p14829641191120"><a name="zh-cn_topic_0000002518363947_p14829641191120"></a><a name="zh-cn_topic_0000002518363947_p14829641191120"></a>User Experience</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002518363947_p682920413116"><a name="zh-cn_topic_0000002518363947_p682920413116"></a><a name="zh-cn_topic_0000002518363947_p682920413116"></a>用户体验</p>
</td>
</tr>
</tbody>
</table>

