# 前言<a name="ZH-CN_TOPIC_0000002603222959"></a>

**概述<a name="zh-cn_topic_0000002540918428_section669mcpsimp"></a>**

本文档为HiDiTingV100蓝牙API接口说明文档，用于指导客户了解蓝牙模块对外提供的API，使客户能够准备理解接口的含义，更好的基于接口开发客户应用。

**读者对象<a name="zh-cn_topic_0000002540918428_section694mcpsimp"></a>**

本文档主要适用以下工程师：

-   技术支持工程
-   软件开发工程师

**符号约定<a name="zh-cn_topic_0000002540918428_section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="zh-cn_topic_0000002540918428_table2622507016410"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002540918428_row1530720816410"><th class="cellrowborder" valign="top" width="20.560000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002540918428_p6450074116410"><a name="zh-cn_topic_0000002540918428_p6450074116410"></a><a name="zh-cn_topic_0000002540918428_p6450074116410"></a><strong id="zh-cn_topic_0000002540918428_b2136615816410"><a name="zh-cn_topic_0000002540918428_b2136615816410"></a><a name="zh-cn_topic_0000002540918428_b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.44%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002540918428_p5435366816410"><a name="zh-cn_topic_0000002540918428_p5435366816410"></a><a name="zh-cn_topic_0000002540918428_p5435366816410"></a><strong id="zh-cn_topic_0000002540918428_b5941558116410"><a name="zh-cn_topic_0000002540918428_b5941558116410"></a><a name="zh-cn_topic_0000002540918428_b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002540918428_row1372280416410"><td class="cellrowborder" valign="top" width="20.560000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002540918428_p3734547016410"><a name="zh-cn_topic_0000002540918428_p3734547016410"></a><a name="zh-cn_topic_0000002540918428_p3734547016410"></a><a name="zh-cn_topic_0000002540918428_image2670064316410"></a><a name="zh-cn_topic_0000002540918428_image2670064316410"></a><span><img class="" id="zh-cn_topic_0000002540918428_image2670064316410" src="figures/zh-cn_image_0000002572627376.png" width="55.9265" height="25.270000000000003"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.44%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002540918428_p1757432116410"><a name="zh-cn_topic_0000002540918428_p1757432116410"></a><a name="zh-cn_topic_0000002540918428_p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002540918428_row466863216410"><td class="cellrowborder" valign="top" width="20.560000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002540918428_p1432579516410"><a name="zh-cn_topic_0000002540918428_p1432579516410"></a><a name="zh-cn_topic_0000002540918428_p1432579516410"></a><a name="zh-cn_topic_0000002540918428_image4895582316410"></a><a name="zh-cn_topic_0000002540918428_image4895582316410"></a><span><img class="" id="zh-cn_topic_0000002540918428_image4895582316410" src="figures/zh-cn_image_0000002572787024.png" width="55.9265" height="25.270000000000003"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.44%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002540918428_p959197916410"><a name="zh-cn_topic_0000002540918428_p959197916410"></a><a name="zh-cn_topic_0000002540918428_p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002540918428_row123863216410"><td class="cellrowborder" valign="top" width="20.560000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002540918428_p1232579516410"><a name="zh-cn_topic_0000002540918428_p1232579516410"></a><a name="zh-cn_topic_0000002540918428_p1232579516410"></a><a name="zh-cn_topic_0000002540918428_image1235582316410"></a><a name="zh-cn_topic_0000002540918428_image1235582316410"></a><span><img class="" id="zh-cn_topic_0000002540918428_image1235582316410" src="figures/zh-cn_image_0000002603346461.png" width="55.9265" height="25.270000000000003"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.44%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002540918428_p123197916410"><a name="zh-cn_topic_0000002540918428_p123197916410"></a><a name="zh-cn_topic_0000002540918428_p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002540918428_row5786682116410"><td class="cellrowborder" valign="top" width="20.560000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002540918428_p2204984716410"><a name="zh-cn_topic_0000002540918428_p2204984716410"></a><a name="zh-cn_topic_0000002540918428_p2204984716410"></a><a name="zh-cn_topic_0000002540918428_image4504446716410"></a><a name="zh-cn_topic_0000002540918428_image4504446716410"></a><span><img class="" id="zh-cn_topic_0000002540918428_image4504446716410" src="figures/zh-cn_image_0000002603226527.png" width="55.9265" height="25.270000000000003"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.44%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002540918428_p4388861916410"><a name="zh-cn_topic_0000002540918428_p4388861916410"></a><a name="zh-cn_topic_0000002540918428_p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="zh-cn_topic_0000002540918428_p1238861916410"><a name="zh-cn_topic_0000002540918428_p1238861916410"></a><a name="zh-cn_topic_0000002540918428_p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002540918428_row2856923116410"><td class="cellrowborder" valign="top" width="20.560000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002540918428_p5555360116410"><a name="zh-cn_topic_0000002540918428_p5555360116410"></a><a name="zh-cn_topic_0000002540918428_p5555360116410"></a><a name="zh-cn_topic_0000002540918428_image799324016410"></a><a name="zh-cn_topic_0000002540918428_image799324016410"></a><span><img class="" id="zh-cn_topic_0000002540918428_image799324016410" src="figures/zh-cn_image_0000002572627378.png" width="47.88" height="15.96"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.44%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002540918428_p4612588116410"><a name="zh-cn_topic_0000002540918428_p4612588116410"></a><a name="zh-cn_topic_0000002540918428_p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="zh-cn_topic_0000002540918428_p1232588116410"><a name="zh-cn_topic_0000002540918428_p1232588116410"></a><a name="zh-cn_topic_0000002540918428_p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# 设备与连接管理<a name="ZH-CN_TOPIC_0000002603223055"></a>

-   **[概述](#ZH-CN_TOPIC_0000002572783562)**  

-   **[接口说明](#ZH-CN_TOPIC_0000002572783550)**  

-   **[开发示例](#ZH-CN_TOPIC_0000002603223085)**  

## 概述<a name="ZH-CN_TOPIC_0000002572783562"></a>

设备与连接管理实现蓝牙设备开关控制、蓝牙设备信息管理、蓝牙设备的发现与被发现、连接与被连接、主动回连以及断开连接等功能。

## 接口说明<a name="ZH-CN_TOPIC_0000002572783550"></a>

-   **[初始化与参数配置](#ZH-CN_TOPIC_0000002572623876)**  

-   **[连接与配对](#ZH-CN_TOPIC_0000002572623826)**  

-   **[信息查询](#ZH-CN_TOPIC_0000002572623814)**  

### 初始化与参数配置<a name="ZH-CN_TOPIC_0000002572623876"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002603222981)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002603342911)**  

#### 数据结构<a name="ZH-CN_TOPIC_0000002603222981"></a>

-   **[bs\_ble\_config\_type\_t](#ZH-CN_TOPIC_0000002603223001)**  

-   **[bs\_ble\_callbacks\_t](#ZH-CN_TOPIC_0000002572783540)**  

-   **[bs\_ble\_dev\_manager\_callbacks\_t](#ZH-CN_TOPIC_0000002572783496)**  

-   **[bs\_br\_config\_type\_t](#ZH-CN_TOPIC_0000002603342971)**  

-   **[bs\_br\_callbacks\_t](#ZH-CN_TOPIC_0000002572783478)**  

##### bs\_ble\_config\_type\_t<a name="ZH-CN_TOPIC_0000002603223001"></a>

【结构体说明】

通过指定配置项来修改BLE配置。

【结构体定义】

```
typedef enum {
    BLE_CONFIG_LOCAL_ADDR            = BLE_CONFIG_LOCAL | 0x0,
    BLE_CONFIG_LOCAL_NAME            = BLE_CONFIG_LOCAL | 0x1,
    BLE_CONFIG_LOCAL_APPEARANCE      = BLE_CONFIG_LOCAL | 0x2, /* gap_ble_appearance_type_t */
    BLE_CONFIG_PAIR_INFO_AVAILABLE   = BLE_CONFIG_LOCAL | 0x3, /* gap_ble_set_pair_info_available */
    BLE_CONFIG_ACL_SWITCH_USE        = BLE_CONFIG_LOCAL | 0x4, /*!< @if Eng config acl switch.
                                                                {@ref bs_ble_config_enable_t}
                                                                @else 配置BLE链路无感切换开关
                                                                {@ref bs_ble_config_enable_t}
                                                                @endif */
    BLE_CONFIG_PEER_ADDR             = BLE_CONFIG_LOCAL | 0x5, /*!< @if Eng config tws peer le addr. {@ref char *}
                                                                @else 配置tws对耳BLE地址 {@ref char *} @endif */
    BLE_CONFIG_ADV_UNDIR_SLOW        = BLE_CONFIG_ADV | 0x1, /* !< @if Eng Use the slow gear to set the parameters.
                If preset advertising parameters have been added by using the BLE_CONFIG_ADV_OPR_CUSTOM_PARAM before the
                level configuration, and the mode ID of the added preset advertising parameters is the same as the macro
                value of BLE_CONFIG_ADV_UNDIR_SLOW, the preset advertising parameters take effect first. If the preset
                advertising parameters are not added, the parameters take effect by default of slow gear.
                 @else
                使用低速档位对广播参数进行设置。如果在进行档位配置前已使用 BLE_CONFIG_ADV_OPR_CUSTOM_PARAM 添加了用户预置的
                广播参数，且添加的预置广播参数的模式ID与 BLE_CONFIG_ADV_UNDIR_SLOW 的宏值相同，则对应的用户预置广播参数优先生效；
                若未添加对应的用户预置广播参数，则系统低速挡默认参数生效。
                 @endif */
    BLE_CONFIG_ADV_UNDIR_MIDDLE      = BLE_CONFIG_ADV | 0x2, /* !< @if Eng Use the middle gear to set the parameters.
                If preset advertising parameters have been added by using the BLE_CONFIG_ADV_OPR_CUSTOM_PARAM before the
                level configuration, and the mode ID of the added preset advertising parameters is the same as the macro
                value of BLE_CONFIG_ADV_UNDIR_MIDDLE, the preset advertising parameters take effect first. If the preset
                advertising parameters are not added, the parameters take effect by default of middle gear.
                 @else
                使用中速档位对广播参数进行设置。如果在进行档位配置前已使用 BLE_CONFIG_ADV_OPR_CUSTOM_PARAM 添加了用户预置的
                广播参数，且添加的预置广播参数的模式ID与 BLE_CONFIG_ADV_UNDIR_MIDDLE 的宏值相同，则对应用户预置广播参数优先生效；
                若未添加对应的用户预置广播参数，则系统中速档默认参数生效。
                 @endif */
    BLE_CONFIG_ADV_UNDIR_FAST        = BLE_CONFIG_ADV | 0x3, /* !< @if Eng Use the fast gear to set the parameters.
                If preset advertising parameters have been added by using the BLE_CONFIG_ADV_OPR_CUSTOM_PARAM before the
                level configuration, and the mode ID of the added preset advertising parameters is the same as the macro
                value of BLE_CONFIG_ADV_UNDIR_FAST, the preset advertising parameters take effect first. If the preset
                advertising parameters are not added, the parameters take effect by default of fast gear.
                 @else
                使用快速档位对广播参数进行设置。如果在进行档位配置前已使用 BLE_CONFIG_ADV_OPR_CUSTOM_PARAM 添加了用户预置的
                广播参数，且添加的预置广播参数的模式ID与 BLE_CONFIG_ADV_UNDIR_FAST 的宏值相同，则对应的用户预置广播参数优先生效；
                若未添加对应的用户预置广播参数，则系统高速档默认参数生效。
                 @endif */
    BLE_CONFIG_ADV_DIR_SLOW          = BLE_CONFIG_ADV | 0x4,
    BLE_CONFIG_ADV_DIR_FAST          = BLE_CONFIG_ADV | 0x5,
    BLE_CONFIG_ADV_TXPOWER           = BLE_CONFIG_ADV | 0x6,
    BLE_CONFIG_ADV_WHITE_LIST        = BLE_CONFIG_ADV | 0x7,
    BLE_CONFIG_ADV_DATA              = BLE_CONFIG_ADV | 0x8,
    BLE_CONFIG_ADV_SCAN_RSP          = BLE_CONFIG_ADV | 0x9,
    BLE_CONFIG_ADV_PEER_ADDR         = BLE_CONFIG_ADV | 0xA,
    BLE_CONFIG_ADV_DURATION          = BLE_CONFIG_ADV | 0xB, /* 只可用于配置回连广播的持续时间 */
    BLE_CONFIG_ADV_OPR_CUSTOM_PARAM  = BLE_CONFIG_ADV | 0xC, /* !< @if Eng Add preset advertising parameters
                                                                {@ref bs_ble_opr_pre_set_adv_param_t *}
                                                            @else 添加用户预制广播参数
                                                                {@ref bs_ble_opr_pre_set_adv_param_t *}
                                                            @endif */
    BLE_CONFIG_SCAN_MIDDLE           = BLE_CONFIG_SCAN | 0x0,/* !< @if Eng Use the middle gear to set the parameters.
                If preset scanning parameters have been added by using the BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM before the
                level configuration, and the mode ID of the added preset scanning parameters is the same as the macro
                value of BLE_CONFIG_SCAN_MIDDLE, the preset scanning parameters take effect first. If the preset
                scanning parameters are not added, the parameters take effect by default of middle gear.
                 @else
                使用中速档位对扫描参数进行设置。如果在进行档位配置前已使用 BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM 添加了用户预置的
                扫描参数，且添加的预置扫描参数的模式ID与 BLE_CONFIG_SCAN_MIDDLE 的宏值相同，则对应的用户预置扫描参数优先生效；
                若未添加对应的用户预置扫描参数，则系统中速档默认参数生效。
                 @endif */
    BLE_CONFIG_SCAN_SLOW             = BLE_CONFIG_SCAN | 0x1,/* !< @if Eng Use the slow gear to set the parameters.
                If preset scanning parameters have been added by using the BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM before the
                level configuration, and the mode ID of the added preset scanning parameters is the same as the macro
                value of BLE_CONFIG_SCAN_SLOW, the preset scanning parameters take effect first. If the preset
                scanning parameters are not added, the parameters take effect by default of slow gear.
                 @else
                使用低速档位对扫描参数进行设置。如果在进行档位配置前已使用 BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM 添加了用户预置的
                扫描参数，且添加的预置扫描参数的模式ID与 BLE_CONFIG_SCAN_SLOW 的宏值相同，则对应的用户预置扫描参数优先生效；
                若未添加对应的用户预置扫描参数，则系统低速档默认参数生效。
                 @endif */
    BLE_CONFIG_SCAN_FAST             = BLE_CONFIG_SCAN | 0x2,/* !< @if Eng Use the fast gear to set the parameters.
                If preset scanning parameters have been added by using the BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM before the
                level configuration, and the mode ID of the added preset scanning parameters is the same as the macro
                value of BLE_CONFIG_SCAN_FAST, the preset scanning parameters take effect first. If the preset
                scanning parameters are not added, the parameters take effect by default of fast gear.
                 @else
                使用高速档位对扫描参数进行设置。如果在进行档位配置前已使用 BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM 添加了用户预置的
                扫描参数，且添加的预置扫描参数的模式ID与 BLE_CONFIG_SCAN_FAST 的宏值相同，则对应的用户预置扫描参数优先生效；
                若未添加对应的用户预置扫描参数，则系统高速档默认参数生效。
                 @endif */
    BLE_CONFIG_SCAN_TYPE             = BLE_CONFIG_SCAN | 0x3,
    BLE_CONFIG_SCAN_WHITE_LIST       = BLE_CONFIG_SCAN | 0x4,
    BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM  = BLE_CONFIG_SCAN | 0x5, /* !< @if Eng Add preset scan parameters
                                                                {@ref bs_ble_opr_pre_set_scan_param_t *}
                                                            @else 添加用户预制扫描参数
                                                                {@ref bs_ble_opr_pre_set_scan_param_t *}
                                                            @endif */
    BLE_CONFIG_CONNECT_SLOW          = BLE_CONFIG_CONNECT | 0x0,
    BLE_CONFIG_CONNECT_HIGH          = BLE_CONFIG_CONNECT | 0x1,
    BLE_CONFIG_CONNECT_UPDATE        = BLE_CONFIG_CONNECT | 0x2, /* !< @if Eng config update.
                                                                      {@ref bs_ble_conn_param_update_t *}
                                                                 @else 更新连接参数
                                                                      {@ref bs_ble_conn_param_update_t *}
                                                                 @endif */
    BLE_CONFIG_CONNECT_PHY           = BLE_CONFIG_CONNECT | 0x3, /* !< @if Eng config PHY. {@ref bs_ble_set_phy_t *}
                                                                    @else 更新物理层参数 {@ref bs_ble_set_phy_t *} @endif */
    BLE_CONFIG_CONNECT_PRESET_CONN   = BLE_CONFIG_CONNECT | 0x4, /* !< @if Eng config pre-set connection params. {@ref bs_ble_opr_pre_set_conn_param_t *}
                                                                    @else 配置预定义的连接参数 {@ref bs_ble_opr_pre_set_conn_param_t *} @endif */
    BLE_CONFIG_CONNECT_PRESET_PHY    = BLE_CONFIG_CONNECT | 0x5, /* !< @if Eng config pre-set PHY. {@ref bs_ble_opr_pre_set_phy_param_t *}
                                                                    @else 配置预定义的连接参数 {@ref bs_ble_opr_pre_set_phy_param_t *} @endif */
    BLE_CONFIG_PAIR_USE_CTDK          = BLE_CONFIG_PAIR | 0x0,      /*!< @if Eng config ctdk.
                                                                            {@ref bs_ble_config_enable_t}
                                                                         @else 配置ctdk
                                                                            {@ref bs_ble_config_enable_t}
                                                                         @endif */
    BLE_CONFIG_PAIR_NOT_AUTO_PAIR     = BLE_CONFIG_PAIR | 0x1,  /*!< @if Eng This parameter is used to configure
                                                                    whether to disable automatic pair after the
                                                                    connection is complete.{@ref bs_ble_config_enable_t}
                                                                @else 用于配置在完成连接后是否不自动进行配对。
                                                                    {@ref bs_ble_config_enable_t}
                                                                @endif */
    BLE_CONFIG_RECONNECT_CENTRAL      = BLE_CONFIG_RECONNECT | 0x0,
    BLE_CONFIG_RECONNECT_PERIPHERAL   = BLE_CONFIG_RECONNECT | 0x1,
    BLE_CONFIG_RECONNECT_USE_WL       = BLE_CONFIG_RECONNECT | 0x2,
    BLE_CONFIG_RECONNECT_SCAN_RECONN  = BLE_CONFIG_RECONNECT | 0x3,
    BLE_CONFIG_RECONNECT_ADD_WL       = BLE_CONFIG_RECONNECT | 0x10,
    BLE_CONFIG_RECONNECT_RMV_WL       = BLE_CONFIG_RECONNECT | 0x11,
    BLE_CONFIG_RECONNECT_SCAN_RECONN_ADD_CHECK_RSSI = BLE_CONFIG_RECONNECT | 0x20,
    BLE_CONFIG_RECONNECT_SCAN_RECONN_ADD_CHECK_UUID = BLE_CONFIG_RECONNECT | 0x21,
    BLE_CONFIG_RECONNECT_SCAN_RECONN_ADD_CHECK_NAME = BLE_CONFIG_RECONNECT | 0x22,
    BLE_CONFIG_RECONNECT_SCAN_RECONN_RMV_CHECK_RSSI = BLE_CONFIG_RECONNECT | 0x30,
    BLE_CONFIG_RECONNECT_SCAN_RECONN_RMV_CHECK_UUID = BLE_CONFIG_RECONNECT | 0x31,
    BLE_CONFIG_RECONNECT_SCAN_RECONN_RMV_CHECK_NAME = BLE_CONFIG_RECONNECT | 0x32,
} bs_ble_config_type_t;
```

【注意事项】

无。

##### bs\_ble\_callbacks\_t<a name="ZH-CN_TOPIC_0000002572783540"></a>

【结构体说明】

BLE service回调函数接口定义。

【结构体定义】

```
typedef struct {
    bs_ble_stack_state_cbk          stack_state_cbk;        /*!< @if Eng ble stack state callback
                                                                 @else   ble协议栈状态回调函数 @endif */
    bs_ble_connect_state_cbk        connect_state_cbk;      /*!< @if Eng ble connect state callback
                                                                 @else   ble连接状态回调函数 @endif */
    bs_ble_pair_complete_cbk        pair_complete_cbk;        /*!< @if Eng ble pair result callback
                                                                 @else   ble配对结果回调函数 @endif */
    bs_ble_discover_result_cbk      discover_result_cbk;    /*!< @if Eng discover result callback
                                                                 @else   发现结果上报回调函数 @endif */
    bs_ble_pair_remove_cbk          pair_remove_cbk;        /*!< @if Eng pair remove result callback
                                                                 @else   移除配对信息结果回调函数 @endif */
    bs_ble_read_rssi_cbk            read_rssi_cbk;          /*!< @if Eng read rssi result callback
                                                                 @else   读取到的RSSI结果回调函数 @endif */
    bs_ble_advertising_state_cbk    adv_state_cbk;          /*!< @if Eng start or stop advertising state callback
                                                                 @else   启动或停止广播状态回调函数 @endif */
    bs_ble_connect_param_update_cbk conn_param_update_cbk;  /*!< @if Eng update connect parameters callback
                                                                 @else   连接参数更新结果回调函数 @endif */
    bs_ble_set_data_len_cbk         set_data_length_cbk;    /*!< @if Eng set data length callback
                                                                 @else   设置数据长度结果回调函数 @endif */
} bs_ble_callbacks_t;
```

【注意事项】

无。

##### bs\_ble\_dev\_manager\_callbacks\_t<a name="ZH-CN_TOPIC_0000002572783496"></a>

【结构体说明】

BLE service设备管理回调函数接口定义。

【结构体定义】

```
typedef struct {
    bs_ble_power_on_cbk power_on_cbk;                        /*!< @if Eng Device power on callback
                                                                   @else   设备上电回调函数 @endif */
} bs_ble_dev_manager_callbacks_t;
```

【注意事项】

无。

##### bs\_br\_config\_type\_t<a name="ZH-CN_TOPIC_0000002603342971"></a>

【结构体说明】

通过指定配置项来修改BR/EDR配置。

【结构体定义】

```
typedef enum {
    BR_CONFIG_LOCAL_ADDR                    = BR_CONFIG_LOCAL | 0x0, /*!< @if Eng config device addr. {@ref bs_addr_t *}
                                                                          @else 配置设备地址 {@ref bs_addr_t *} @endif */
    BR_CONFIG_LOCAL_NAME                    = BR_CONFIG_LOCAL | 0x1, /*!< @if Eng config device name. {@ref char *}
                                                                          @else 配置设备名称 {@ref char *} @endif */
    BR_CONFIG_LOCAL_DEV_CLASS               = BR_CONFIG_LOCAL | 0x2, /*!< @if Eng config device class.
                                                                                  {@ref bs_br_config_device_class_t *}
                                                                          @else 配置设备类型
                                                                                {@ref bs_br_config_device_class_t *}
                                                                          @endif */
    BR_CONFIG_INQUIRY_PARAM                 = BR_CONFIG_INQUIRY | 0x0, /*!< @if Eng config inquiry parameter.
                                                                                  {@ref bs_br_config_inquiry_param_t *}
                                                                          @else 配置设备inquiry参数
                                                                                {@ref bs_br_config_inquiry_param_t *}
                                                                          @endif */
    BR_CONFIG_CONNECT_FILTER_MODE           = BR_CONFIG_CONNECT | 0x0, /*!< @if Eng config filter mode.
                                                                                  {@ref bs_br_config_switch_t *}
                                                                          @else 配置过滤模式，只能连接一个手机、外设
                                                                                {@ref bs_br_config_switch_t *}
                                                                          @endif */
    BR_CONFIG_CONNECT_UPPER_DECIDE_CONNECT  = BR_CONFIG_CONNECT | 0x1, /*!< @if Eng config upper decides connect.
                                                                                  {@ref bs_br_config_switch_t *}
                                                                          @else 配置上层决定是否接受连接
                                                                                {@ref bs_br_config_switch_t *}
                                                                          @endif */
    BR_CONFIG_CONNECT_AUTO_RECONNECT        = BR_CONFIG_CONNECT | 0x2, /*!< @if Eng config auto reconnect.
                                                                                  {@ref bs_br_config_switch_t *}
                                                                          @else 配置是否自动回连
                                                                                {@ref bs_br_config_switch_t *}
                                                                          @endif */
    BR_CONFIG_CONNECT_RECONNECT_MASTER      = BR_CONFIG_CONNECT | 0x3, /*!< @if Eng config reconnect master device.
                                                                                  {@ref bs_br_config_switch_t *}
                                                                          @else 配置是否回连主设备
                                                                                {@ref bs_br_config_switch_t *}
                                                                          @endif */
    BR_CONFIG_CONNECT_RECONNECT_SLAVE       = BR_CONFIG_CONNECT | 0x4, /*!< @if Eng config reconnect slave device.
                                                                                  {@ref bs_br_config_switch_t *}
                                                                          @else 配置是否回连从设备
                                                                                {@ref bs_br_config_switch_t *}
                                                                          @endif */
    BR_CONFIG_CONNECT_CONN_MAX              = BR_CONFIG_CONNECT | 0x5, /*!< @if Eng config max connection number.
                                                                                  {@ref bs_br_config_connect_max_t *}
                                                                          @else 配置最大连接数
                                                                                {@ref bs_br_config_connect_max_t *}
                                                                          @endif */
    BR_CONFIG_PROFILE_SUPPORT               = BR_CONFIG_PROFILE | 0x0, /*!< @if Eng config supprot prifiles.
                                                                                {@ref bs_br_config_support_profile_t *}
                                                                          @else 配置支持的profiles
                                                                                {@ref bs_br_config_support_profile_t *}
                                                                          @endif */
    BR_CONFIG_PROFILE_CONNECT               = BR_CONFIG_PROFILE | 0x1, /*!< @if Eng config auto connect prifiles.
                                                                                {@ref bs_br_config_support_profile_t *}
                                                                          @else 配置自动连接的profiles
                                                                                {@ref bs_br_config_support_profile_t *}
                                                                          @endif */
    BR_CONFIG_PORT_CONNECT_MODE             = BR_CONFIG_PORT_CONNECT | 0x0, /*!< @if Eng Configure the port server to
                                                                                 automatically reply to the connection.
                                                                                {@ref bs_br_config_switch_t *}
                                                                          @else 配置 port 服务端自动回复连接
                                                                                {@ref bs_br_config_switch_t *}
                                                                          @endif */
    BR_CONFIG_PROFILE_MEDIA_ATTRIBUTE       = BR_CONFIG_PROFILE | 0x2, /*!< @if Eng config media attribute.
                                                                                {@ref bs_br_config_music_media_attr_t *}
                                                                          @else 配置媒体歌词属性信息
                                                                                {@ref bs_br_config_music_media_attr_t *}
                                                                          @endif */
} bs_br_config_type_t;
```

【注意事项】

无。

##### bs\_br\_callbacks\_t<a name="ZH-CN_TOPIC_0000002572783478"></a>

【结构体说明】

BR/EDR service回调函数接口定义。

【结构体定义】

```
typedef struct {
    bs_br_stack_state_cbk stack_state_cb;
    bs_br_connect_request_cbk connect_request_cb;
    bs_br_connect_state_cbk connect_state_cb;
    bs_br_pair_complete_cbk pair_complete_cb;
    bs_br_pair_remove_cbk pair_remove_cb;
    bs_br_discovery_result_cbk discovery_result_cb;
    bs_br_remote_name_changed_cbk remote_name_changed_cb;
} bs_br_callbacks_t;
```

【注意事项】

无。

#### 函数接口<a name="ZH-CN_TOPIC_0000002603342911"></a>

-   **[bs\_config](#ZH-CN_TOPIC_0000002572623828)**  

-   **[bs\_ble\_enable](#ZH-CN_TOPIC_0000002572623874)**  

-   **[bs\_ble\_disable](#ZH-CN_TOPIC_0000002603342909)**  

-   **[bs\_ble\_register\_callbacks](#ZH-CN_TOPIC_0000002572623870)**  

-   **[bs\_ble\_unregister\_callbacks](#ZH-CN_TOPIC_0000002603222995)**  

-   **[bs\_ble\_dev\_manager\_register\_callbacks](#ZH-CN_TOPIC_0000002572783578)**  

-   **[bs\_ble\_set\_connect\_param](#ZH-CN_TOPIC_0000002603342983)**  

-   **[bs\_ble\_set\_phy\_param](#ZH-CN_TOPIC_0000002572623926)**  

-   **[bs\_ble\_set\_data\_length](#ZH-CN_TOPIC_0000002603342967)**  

-   **[bs\_br\_enable](#ZH-CN_TOPIC_0000002603342953)**  

-   **[bs\_br\_disable](#ZH-CN_TOPIC_0000002572783466)**  

-   **[bs\_br\_register\_callbacks](#ZH-CN_TOPIC_0000002572783482)**  

##### bs\_config<a name="ZH-CN_TOPIC_0000002572623828"></a>

【描述】

配置参数。

【接口定义】

```
errcode_t bs_config(uint16_t type, void *param, uint16_t *len);
```

【参数】

<a name="zh-cn_topic_0000002543254728_table567mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254728_row573mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254728_p575mcpsimp"><a name="zh-cn_topic_0000002543254728_p575mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p575mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.13999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254728_p577mcpsimp"><a name="zh-cn_topic_0000002543254728_p577mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p577mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.86%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254728_p579mcpsimp"><a name="zh-cn_topic_0000002543254728_p579mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p579mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254728_row580mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254728_p582mcpsimp"><a name="zh-cn_topic_0000002543254728_p582mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p582mcpsimp"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254728_p584mcpsimp"><a name="zh-cn_topic_0000002543254728_p584mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p584mcpsimp"></a>配置项。BLE配置项详见“<a href="#ZH-CN_TOPIC_0000002603223001">bs_ble_config_type_t</a>”，BR/EDR配置项详见“<a href="#ZH-CN_TOPIC_0000002603342971">bs_br_config_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254728_p586mcpsimp"><a name="zh-cn_topic_0000002543254728_p586mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p586mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254728_row587mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254728_p589mcpsimp"><a name="zh-cn_topic_0000002543254728_p589mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p589mcpsimp"></a>param</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254728_p591mcpsimp"><a name="zh-cn_topic_0000002543254728_p591mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p591mcpsimp"></a>配置值，具体类型根据配置项而定。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254728_p593mcpsimp"><a name="zh-cn_topic_0000002543254728_p593mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p593mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254728_row594mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254728_p596mcpsimp"><a name="zh-cn_topic_0000002543254728_p596mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p596mcpsimp"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254728_p598mcpsimp"><a name="zh-cn_topic_0000002543254728_p598mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p598mcpsimp"></a>配置值长度，具体大小根据对应配置项而定。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254728_p600mcpsimp"><a name="zh-cn_topic_0000002543254728_p600mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p600mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543254728_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254728_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254728_p610mcpsimp"><a name="zh-cn_topic_0000002543254728_p610mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254728_p612mcpsimp"><a name="zh-cn_topic_0000002543254728_p612mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254728_p614mcpsimp"><a name="zh-cn_topic_0000002543254728_p614mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254728_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254728_p1931854142112"><a name="zh-cn_topic_0000002543254728_p1931854142112"></a><a name="zh-cn_topic_0000002543254728_p1931854142112"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254728_p18397105811319"><a name="zh-cn_topic_0000002543254728_p18397105811319"></a><a name="zh-cn_topic_0000002543254728_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543254728_p620mcpsimp"><a name="zh-cn_topic_0000002543254728_p620mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254728_p622mcpsimp"><a name="zh-cn_topic_0000002543254728_p622mcpsimp"></a><a name="zh-cn_topic_0000002543254728_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_ble\_enable<a name="ZH-CN_TOPIC_0000002572623874"></a>

【描述】

使能BLE协议栈。

【接口定义】

```
errcode_t bs_ble_enable(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002573894641_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894641_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894641_p610mcpsimp"><a name="zh-cn_topic_0000002573894641_p610mcpsimp"></a><a name="zh-cn_topic_0000002573894641_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894641_p612mcpsimp"><a name="zh-cn_topic_0000002573894641_p612mcpsimp"></a><a name="zh-cn_topic_0000002573894641_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894641_p614mcpsimp"><a name="zh-cn_topic_0000002573894641_p614mcpsimp"></a><a name="zh-cn_topic_0000002573894641_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894641_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894641_p6394174991318"><a name="zh-cn_topic_0000002573894641_p6394174991318"></a><a name="zh-cn_topic_0000002573894641_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894641_p18397105811319"><a name="zh-cn_topic_0000002573894641_p18397105811319"></a><a name="zh-cn_topic_0000002573894641_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002573894641_p620mcpsimp"><a name="zh-cn_topic_0000002573894641_p620mcpsimp"></a><a name="zh-cn_topic_0000002573894641_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894641_p622mcpsimp"><a name="zh-cn_topic_0000002573894641_p622mcpsimp"></a><a name="zh-cn_topic_0000002573894641_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_ble\_disable<a name="ZH-CN_TOPIC_0000002603342909"></a>

【描述】

去使能BLE协议栈。

【接口定义】

```
errcode_t bs_ble_disable(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002574014615_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014615_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014615_p610mcpsimp"><a name="zh-cn_topic_0000002574014615_p610mcpsimp"></a><a name="zh-cn_topic_0000002574014615_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014615_p612mcpsimp"><a name="zh-cn_topic_0000002574014615_p612mcpsimp"></a><a name="zh-cn_topic_0000002574014615_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014615_p614mcpsimp"><a name="zh-cn_topic_0000002574014615_p614mcpsimp"></a><a name="zh-cn_topic_0000002574014615_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014615_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014615_p6394174991318"><a name="zh-cn_topic_0000002574014615_p6394174991318"></a><a name="zh-cn_topic_0000002574014615_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014615_p18397105811319"><a name="zh-cn_topic_0000002574014615_p18397105811319"></a><a name="zh-cn_topic_0000002574014615_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002574014615_p620mcpsimp"><a name="zh-cn_topic_0000002574014615_p620mcpsimp"></a><a name="zh-cn_topic_0000002574014615_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014615_p622mcpsimp"><a name="zh-cn_topic_0000002574014615_p622mcpsimp"></a><a name="zh-cn_topic_0000002574014615_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_ble\_register\_callbacks<a name="ZH-CN_TOPIC_0000002572623870"></a>

【描述】

BLE service回调函数注册。

【接口定义】

```
errcode_t bs_ble_register_callbacks(uint8_t app_id, const bs_ble_callbacks_t *cbks);
```

【参数】

<a name="zh-cn_topic_0000002573894647_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894647_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894647_p702mcpsimp"><a name="zh-cn_topic_0000002573894647_p702mcpsimp"></a><a name="zh-cn_topic_0000002573894647_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894647_p704mcpsimp"><a name="zh-cn_topic_0000002573894647_p704mcpsimp"></a><a name="zh-cn_topic_0000002573894647_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894647_p706mcpsimp"><a name="zh-cn_topic_0000002573894647_p706mcpsimp"></a><a name="zh-cn_topic_0000002573894647_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894647_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894647_p20551103653315"><a name="zh-cn_topic_0000002573894647_p20551103653315"></a><a name="zh-cn_topic_0000002573894647_p20551103653315"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894647_p77056403218"><a name="zh-cn_topic_0000002573894647_p77056403218"></a><a name="zh-cn_topic_0000002573894647_p77056403218"></a>应用ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894647_p713mcpsimp"><a name="zh-cn_topic_0000002573894647_p713mcpsimp"></a><a name="zh-cn_topic_0000002573894647_p713mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002573894647_row52061771342"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894647_p120747113415"><a name="zh-cn_topic_0000002573894647_p120747113415"></a><a name="zh-cn_topic_0000002573894647_p120747113415"></a>cbks</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894647_p14207376347"><a name="zh-cn_topic_0000002573894647_p14207376347"></a><a name="zh-cn_topic_0000002573894647_p14207376347"></a>被注册的回调函数。支持注册的回调函数详见“<a href="#ZH-CN_TOPIC_0000002572783540">bs_ble_callbacks_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894647_p146135433610"><a name="zh-cn_topic_0000002573894647_p146135433610"></a><a name="zh-cn_topic_0000002573894647_p146135433610"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002573894647_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894647_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894647_p610mcpsimp"><a name="zh-cn_topic_0000002573894647_p610mcpsimp"></a><a name="zh-cn_topic_0000002573894647_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894647_p612mcpsimp"><a name="zh-cn_topic_0000002573894647_p612mcpsimp"></a><a name="zh-cn_topic_0000002573894647_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894647_p614mcpsimp"><a name="zh-cn_topic_0000002573894647_p614mcpsimp"></a><a name="zh-cn_topic_0000002573894647_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894647_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894647_p6394174991318"><a name="zh-cn_topic_0000002573894647_p6394174991318"></a><a name="zh-cn_topic_0000002573894647_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894647_p18397105811319"><a name="zh-cn_topic_0000002573894647_p18397105811319"></a><a name="zh-cn_topic_0000002573894647_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002573894647_p620mcpsimp"><a name="zh-cn_topic_0000002573894647_p620mcpsimp"></a><a name="zh-cn_topic_0000002573894647_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894647_p622mcpsimp"><a name="zh-cn_topic_0000002573894647_p622mcpsimp"></a><a name="zh-cn_topic_0000002573894647_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

该接口仅使用传入指针访问目标数据，不会获取该指针所指向内存的所有权，亦不负责其生命周期管理。

BLE service回调函数的注册操作应在BLE协议栈使能之前调用。

##### bs\_ble\_unregister\_callbacks<a name="ZH-CN_TOPIC_0000002603222995"></a>

【描述】

BLE service回调函数去注册。

【接口定义】

```
errcode_t bs_ble_unregister_callbacks(uint8_t app_id);
```

【参数】

<a name="zh-cn_topic_0000002574014621_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014621_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014621_p702mcpsimp"><a name="zh-cn_topic_0000002574014621_p702mcpsimp"></a><a name="zh-cn_topic_0000002574014621_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014621_p704mcpsimp"><a name="zh-cn_topic_0000002574014621_p704mcpsimp"></a><a name="zh-cn_topic_0000002574014621_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014621_p706mcpsimp"><a name="zh-cn_topic_0000002574014621_p706mcpsimp"></a><a name="zh-cn_topic_0000002574014621_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014621_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014621_p20551103653315"><a name="zh-cn_topic_0000002574014621_p20551103653315"></a><a name="zh-cn_topic_0000002574014621_p20551103653315"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014621_p77056403218"><a name="zh-cn_topic_0000002574014621_p77056403218"></a><a name="zh-cn_topic_0000002574014621_p77056403218"></a>应用ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014621_p713mcpsimp"><a name="zh-cn_topic_0000002574014621_p713mcpsimp"></a><a name="zh-cn_topic_0000002574014621_p713mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002574014621_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014621_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014621_p610mcpsimp"><a name="zh-cn_topic_0000002574014621_p610mcpsimp"></a><a name="zh-cn_topic_0000002574014621_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014621_p612mcpsimp"><a name="zh-cn_topic_0000002574014621_p612mcpsimp"></a><a name="zh-cn_topic_0000002574014621_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014621_p614mcpsimp"><a name="zh-cn_topic_0000002574014621_p614mcpsimp"></a><a name="zh-cn_topic_0000002574014621_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014621_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014621_p6394174991318"><a name="zh-cn_topic_0000002574014621_p6394174991318"></a><a name="zh-cn_topic_0000002574014621_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014621_p18397105811319"><a name="zh-cn_topic_0000002574014621_p18397105811319"></a><a name="zh-cn_topic_0000002574014621_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002574014621_p620mcpsimp"><a name="zh-cn_topic_0000002574014621_p620mcpsimp"></a><a name="zh-cn_topic_0000002574014621_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014621_p622mcpsimp"><a name="zh-cn_topic_0000002574014621_p622mcpsimp"></a><a name="zh-cn_topic_0000002574014621_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

BLE service回调函数的去注册流程应在BLE协议栈去使能之后。

##### bs\_ble\_dev\_manager\_register\_callbacks<a name="ZH-CN_TOPIC_0000002572783578"></a>

【描述】

BLE设备管理回调函数。

【接口定义】

```
errcode_t bs_ble_dev_manager_register_callbacks(uint8_t app_id, const bs_ble_dev_manager_callbacks_t *cbks);
```

【参数】

<a name="zh-cn_topic_0000002543254738_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254738_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254738_p702mcpsimp"><a name="zh-cn_topic_0000002543254738_p702mcpsimp"></a><a name="zh-cn_topic_0000002543254738_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254738_p704mcpsimp"><a name="zh-cn_topic_0000002543254738_p704mcpsimp"></a><a name="zh-cn_topic_0000002543254738_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254738_p706mcpsimp"><a name="zh-cn_topic_0000002543254738_p706mcpsimp"></a><a name="zh-cn_topic_0000002543254738_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254738_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254738_p20551103653315"><a name="zh-cn_topic_0000002543254738_p20551103653315"></a><a name="zh-cn_topic_0000002543254738_p20551103653315"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254738_p77056403218"><a name="zh-cn_topic_0000002543254738_p77056403218"></a><a name="zh-cn_topic_0000002543254738_p77056403218"></a>应用ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254738_p713mcpsimp"><a name="zh-cn_topic_0000002543254738_p713mcpsimp"></a><a name="zh-cn_topic_0000002543254738_p713mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254738_row52061771342"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254738_p120747113415"><a name="zh-cn_topic_0000002543254738_p120747113415"></a><a name="zh-cn_topic_0000002543254738_p120747113415"></a>cbks</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254738_p14207376347"><a name="zh-cn_topic_0000002543254738_p14207376347"></a><a name="zh-cn_topic_0000002543254738_p14207376347"></a>被注册的回调函数。支持注册的回调函数详见“<a href="#ZH-CN_TOPIC_0000002572783496">bs_ble_dev_manager_callbacks_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254738_p146135433610"><a name="zh-cn_topic_0000002543254738_p146135433610"></a><a name="zh-cn_topic_0000002543254738_p146135433610"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543254738_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254738_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254738_p610mcpsimp"><a name="zh-cn_topic_0000002543254738_p610mcpsimp"></a><a name="zh-cn_topic_0000002543254738_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254738_p612mcpsimp"><a name="zh-cn_topic_0000002543254738_p612mcpsimp"></a><a name="zh-cn_topic_0000002543254738_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254738_p614mcpsimp"><a name="zh-cn_topic_0000002543254738_p614mcpsimp"></a><a name="zh-cn_topic_0000002543254738_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254738_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254738_p6394174991318"><a name="zh-cn_topic_0000002543254738_p6394174991318"></a><a name="zh-cn_topic_0000002543254738_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254738_p18397105811319"><a name="zh-cn_topic_0000002543254738_p18397105811319"></a><a name="zh-cn_topic_0000002543254738_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543254738_p620mcpsimp"><a name="zh-cn_topic_0000002543254738_p620mcpsimp"></a><a name="zh-cn_topic_0000002543254738_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254738_p622mcpsimp"><a name="zh-cn_topic_0000002543254738_p622mcpsimp"></a><a name="zh-cn_topic_0000002543254738_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

该接口仅使用传入指针访问目标数据，不会获取该指针所指向内存的所有权，亦不负责其生命周期管理。

BLE service设备管理回调函数的注册操作应在服务线程启动前调用。

##### bs\_ble\_set\_connect\_param<a name="ZH-CN_TOPIC_0000002603342983"></a>

【描述】

使用预置参数对BLE连接参数进行设置。

【接口定义】

```
errcode_t bs_ble_set_connect_param(uint16_t dev_id, uint16_t mode_id);
```

【参数】

<a name="zh-cn_topic_0000002543414402_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414402_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414402_p702mcpsimp"><a name="zh-cn_topic_0000002543414402_p702mcpsimp"></a><a name="zh-cn_topic_0000002543414402_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414402_p704mcpsimp"><a name="zh-cn_topic_0000002543414402_p704mcpsimp"></a><a name="zh-cn_topic_0000002543414402_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414402_p706mcpsimp"><a name="zh-cn_topic_0000002543414402_p706mcpsimp"></a><a name="zh-cn_topic_0000002543414402_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414402_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414402_p20551103653315"><a name="zh-cn_topic_0000002543414402_p20551103653315"></a><a name="zh-cn_topic_0000002543414402_p20551103653315"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414402_p77056403218"><a name="zh-cn_topic_0000002543414402_p77056403218"></a><a name="zh-cn_topic_0000002543414402_p77056403218"></a>设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414402_p713mcpsimp"><a name="zh-cn_topic_0000002543414402_p713mcpsimp"></a><a name="zh-cn_topic_0000002543414402_p713mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543414402_row52061771342"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414402_p120747113415"><a name="zh-cn_topic_0000002543414402_p120747113415"></a><a name="zh-cn_topic_0000002543414402_p120747113415"></a>mode_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414402_p135991244164615"><a name="zh-cn_topic_0000002543414402_p135991244164615"></a><a name="zh-cn_topic_0000002543414402_p135991244164615"></a>模式ID，对应一组已配置的预置连接参数。预置连接参数可通过接口bs_config进行配置。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414402_p146135433610"><a name="zh-cn_topic_0000002543414402_p146135433610"></a><a name="zh-cn_topic_0000002543414402_p146135433610"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543414402_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414402_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414402_p610mcpsimp"><a name="zh-cn_topic_0000002543414402_p610mcpsimp"></a><a name="zh-cn_topic_0000002543414402_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414402_p612mcpsimp"><a name="zh-cn_topic_0000002543414402_p612mcpsimp"></a><a name="zh-cn_topic_0000002543414402_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414402_p614mcpsimp"><a name="zh-cn_topic_0000002543414402_p614mcpsimp"></a><a name="zh-cn_topic_0000002543414402_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414402_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414402_p6394174991318"><a name="zh-cn_topic_0000002543414402_p6394174991318"></a><a name="zh-cn_topic_0000002543414402_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414402_p18397105811319"><a name="zh-cn_topic_0000002543414402_p18397105811319"></a><a name="zh-cn_topic_0000002543414402_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543414402_p620mcpsimp"><a name="zh-cn_topic_0000002543414402_p620mcpsimp"></a><a name="zh-cn_topic_0000002543414402_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414402_p622mcpsimp"><a name="zh-cn_topic_0000002543414402_p622mcpsimp"></a><a name="zh-cn_topic_0000002543414402_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

使用该函数前需已配置对应的预置参数。

##### bs\_ble\_set\_phy\_param<a name="ZH-CN_TOPIC_0000002572623926"></a>

【描述】

使用预置参数对BLE PHY参数进行设置。

【接口定义】

```
errcode_t bs_ble_set_phy_param(uint16_t dev_id, uint16_t mode_id);
```

【参数】

<a name="zh-cn_topic_0000002543254736_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254736_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254736_p702mcpsimp"><a name="zh-cn_topic_0000002543254736_p702mcpsimp"></a><a name="zh-cn_topic_0000002543254736_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254736_p704mcpsimp"><a name="zh-cn_topic_0000002543254736_p704mcpsimp"></a><a name="zh-cn_topic_0000002543254736_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254736_p706mcpsimp"><a name="zh-cn_topic_0000002543254736_p706mcpsimp"></a><a name="zh-cn_topic_0000002543254736_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254736_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254736_p20551103653315"><a name="zh-cn_topic_0000002543254736_p20551103653315"></a><a name="zh-cn_topic_0000002543254736_p20551103653315"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254736_p77056403218"><a name="zh-cn_topic_0000002543254736_p77056403218"></a><a name="zh-cn_topic_0000002543254736_p77056403218"></a>设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254736_p713mcpsimp"><a name="zh-cn_topic_0000002543254736_p713mcpsimp"></a><a name="zh-cn_topic_0000002543254736_p713mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254736_row52061771342"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254736_p120747113415"><a name="zh-cn_topic_0000002543254736_p120747113415"></a><a name="zh-cn_topic_0000002543254736_p120747113415"></a>mode_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254736_p135991244164615"><a name="zh-cn_topic_0000002543254736_p135991244164615"></a><a name="zh-cn_topic_0000002543254736_p135991244164615"></a>模式ID，对应一组已配置的预置PHY参数。预置PHY参数可通过接口bs_config进行配置。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254736_p146135433610"><a name="zh-cn_topic_0000002543254736_p146135433610"></a><a name="zh-cn_topic_0000002543254736_p146135433610"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543254736_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254736_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254736_p610mcpsimp"><a name="zh-cn_topic_0000002543254736_p610mcpsimp"></a><a name="zh-cn_topic_0000002543254736_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254736_p612mcpsimp"><a name="zh-cn_topic_0000002543254736_p612mcpsimp"></a><a name="zh-cn_topic_0000002543254736_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254736_p614mcpsimp"><a name="zh-cn_topic_0000002543254736_p614mcpsimp"></a><a name="zh-cn_topic_0000002543254736_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254736_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254736_p6394174991318"><a name="zh-cn_topic_0000002543254736_p6394174991318"></a><a name="zh-cn_topic_0000002543254736_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254736_p18397105811319"><a name="zh-cn_topic_0000002543254736_p18397105811319"></a><a name="zh-cn_topic_0000002543254736_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543254736_p620mcpsimp"><a name="zh-cn_topic_0000002543254736_p620mcpsimp"></a><a name="zh-cn_topic_0000002543254736_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254736_p622mcpsimp"><a name="zh-cn_topic_0000002543254736_p622mcpsimp"></a><a name="zh-cn_topic_0000002543254736_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

使用该函数前需已配置对应的预置参数。

##### bs\_ble\_set\_data\_length<a name="ZH-CN_TOPIC_0000002603342967"></a>

【描述】

设置BLE发包参数。

【接口定义】

```
errcode_t bs_ble_set_data_length(bs_ble_set_data_length_t *param);
```

【参数】

<a name="zh-cn_topic_0000002573894649_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894649_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894649_p702mcpsimp"><a name="zh-cn_topic_0000002573894649_p702mcpsimp"></a><a name="zh-cn_topic_0000002573894649_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894649_p704mcpsimp"><a name="zh-cn_topic_0000002573894649_p704mcpsimp"></a><a name="zh-cn_topic_0000002573894649_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894649_p706mcpsimp"><a name="zh-cn_topic_0000002573894649_p706mcpsimp"></a><a name="zh-cn_topic_0000002573894649_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894649_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894649_p10539133408"><a name="zh-cn_topic_0000002573894649_p10539133408"></a><a name="zh-cn_topic_0000002573894649_p10539133408"></a>param</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894649_p77056403218"><a name="zh-cn_topic_0000002573894649_p77056403218"></a><a name="zh-cn_topic_0000002573894649_p77056403218"></a>发包参数。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894649_p713mcpsimp"><a name="zh-cn_topic_0000002573894649_p713mcpsimp"></a><a name="zh-cn_topic_0000002573894649_p713mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002573894649_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894649_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894649_p610mcpsimp"><a name="zh-cn_topic_0000002573894649_p610mcpsimp"></a><a name="zh-cn_topic_0000002573894649_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894649_p612mcpsimp"><a name="zh-cn_topic_0000002573894649_p612mcpsimp"></a><a name="zh-cn_topic_0000002573894649_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894649_p614mcpsimp"><a name="zh-cn_topic_0000002573894649_p614mcpsimp"></a><a name="zh-cn_topic_0000002573894649_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894649_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894649_p6394174991318"><a name="zh-cn_topic_0000002573894649_p6394174991318"></a><a name="zh-cn_topic_0000002573894649_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894649_p18397105811319"><a name="zh-cn_topic_0000002573894649_p18397105811319"></a><a name="zh-cn_topic_0000002573894649_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002573894649_p620mcpsimp"><a name="zh-cn_topic_0000002573894649_p620mcpsimp"></a><a name="zh-cn_topic_0000002573894649_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894649_p622mcpsimp"><a name="zh-cn_topic_0000002573894649_p622mcpsimp"></a><a name="zh-cn_topic_0000002573894649_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

该接口仅使用传入指针访问目标数据，不会获取该指针所指向内存的所有权，亦不负责其生命周期管理。

##### bs\_br\_enable<a name="ZH-CN_TOPIC_0000002603342953"></a>

【描述】

使能BR协议栈。

【接口定义】

```
errcode_t bs_br_enable(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002553134984_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002553134984_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002553134984_p610mcpsimp"><a name="zh-cn_topic_0000002553134984_p610mcpsimp"></a><a name="zh-cn_topic_0000002553134984_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002553134984_p612mcpsimp"><a name="zh-cn_topic_0000002553134984_p612mcpsimp"></a><a name="zh-cn_topic_0000002553134984_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002553134984_p614mcpsimp"><a name="zh-cn_topic_0000002553134984_p614mcpsimp"></a><a name="zh-cn_topic_0000002553134984_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002553134984_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553134984_p6394174991318"><a name="zh-cn_topic_0000002553134984_p6394174991318"></a><a name="zh-cn_topic_0000002553134984_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553134984_p18397105811319"><a name="zh-cn_topic_0000002553134984_p18397105811319"></a><a name="zh-cn_topic_0000002553134984_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002553134984_p620mcpsimp"><a name="zh-cn_topic_0000002553134984_p620mcpsimp"></a><a name="zh-cn_topic_0000002553134984_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553134984_p622mcpsimp"><a name="zh-cn_topic_0000002553134984_p622mcpsimp"></a><a name="zh-cn_topic_0000002553134984_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_disable<a name="ZH-CN_TOPIC_0000002572783466"></a>

【描述】

去使能BR/EDR协议栈。

【接口定义】

```
errcode_t bs_br_disable(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002583774633_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002583774633_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002583774633_p610mcpsimp"><a name="zh-cn_topic_0000002583774633_p610mcpsimp"></a><a name="zh-cn_topic_0000002583774633_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002583774633_p612mcpsimp"><a name="zh-cn_topic_0000002583774633_p612mcpsimp"></a><a name="zh-cn_topic_0000002583774633_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002583774633_p614mcpsimp"><a name="zh-cn_topic_0000002583774633_p614mcpsimp"></a><a name="zh-cn_topic_0000002583774633_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002583774633_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583774633_p6394174991318"><a name="zh-cn_topic_0000002583774633_p6394174991318"></a><a name="zh-cn_topic_0000002583774633_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583774633_p18397105811319"><a name="zh-cn_topic_0000002583774633_p18397105811319"></a><a name="zh-cn_topic_0000002583774633_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002583774633_p620mcpsimp"><a name="zh-cn_topic_0000002583774633_p620mcpsimp"></a><a name="zh-cn_topic_0000002583774633_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583774633_p622mcpsimp"><a name="zh-cn_topic_0000002583774633_p622mcpsimp"></a><a name="zh-cn_topic_0000002583774633_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_register\_callbacks<a name="ZH-CN_TOPIC_0000002572783482"></a>

【描述】

注册BR/EDR回调。

【接口定义】

```
errcode_t bs_br_register_callbacks(uint8_t app_id, const bs_br_callbacks_t *cbks);
```

【参数】

<a name="zh-cn_topic_0000002583854655_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002583854655_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002583854655_p702mcpsimp"><a name="zh-cn_topic_0000002583854655_p702mcpsimp"></a><a name="zh-cn_topic_0000002583854655_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002583854655_p704mcpsimp"><a name="zh-cn_topic_0000002583854655_p704mcpsimp"></a><a name="zh-cn_topic_0000002583854655_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002583854655_p706mcpsimp"><a name="zh-cn_topic_0000002583854655_p706mcpsimp"></a><a name="zh-cn_topic_0000002583854655_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002583854655_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583854655_p17510161565314"><a name="zh-cn_topic_0000002583854655_p17510161565314"></a><a name="zh-cn_topic_0000002583854655_p17510161565314"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583854655_p0803104817537"><a name="zh-cn_topic_0000002583854655_p0803104817537"></a><a name="zh-cn_topic_0000002583854655_p0803104817537"></a>上层应用ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583854655_p713mcpsimp"><a name="zh-cn_topic_0000002583854655_p713mcpsimp"></a><a name="zh-cn_topic_0000002583854655_p713mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002583854655_row1515055105312"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583854655_p1130285816533"><a name="zh-cn_topic_0000002583854655_p1130285816533"></a><a name="zh-cn_topic_0000002583854655_p1130285816533"></a>cbks</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583854655_p01618556535"><a name="zh-cn_topic_0000002583854655_p01618556535"></a><a name="zh-cn_topic_0000002583854655_p01618556535"></a>被注册的回调函数。支持注册的回调函数详见”<a href="#ZH-CN_TOPIC_0000002572783478">bs_br_callbacks_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583854655_p131613558535"><a name="zh-cn_topic_0000002583854655_p131613558535"></a><a name="zh-cn_topic_0000002583854655_p131613558535"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002583854655_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002583854655_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002583854655_p610mcpsimp"><a name="zh-cn_topic_0000002583854655_p610mcpsimp"></a><a name="zh-cn_topic_0000002583854655_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002583854655_p612mcpsimp"><a name="zh-cn_topic_0000002583854655_p612mcpsimp"></a><a name="zh-cn_topic_0000002583854655_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002583854655_p614mcpsimp"><a name="zh-cn_topic_0000002583854655_p614mcpsimp"></a><a name="zh-cn_topic_0000002583854655_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002583854655_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583854655_p6394174991318"><a name="zh-cn_topic_0000002583854655_p6394174991318"></a><a name="zh-cn_topic_0000002583854655_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583854655_p18397105811319"><a name="zh-cn_topic_0000002583854655_p18397105811319"></a><a name="zh-cn_topic_0000002583854655_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002583854655_p620mcpsimp"><a name="zh-cn_topic_0000002583854655_p620mcpsimp"></a><a name="zh-cn_topic_0000002583854655_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583854655_p622mcpsimp"><a name="zh-cn_topic_0000002583854655_p622mcpsimp"></a><a name="zh-cn_topic_0000002583854655_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

该接口仅使用传入指针访问目标数据，不会获取该指针所指向内存的所有权，亦不负责其生命周期管理。

BR/EDR service回调函数的注册操作应在BR/EDR协议栈使能之前调用。

### 连接与配对<a name="ZH-CN_TOPIC_0000002572623826"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002572623906)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002603223029)**  

#### 数据结构<a name="ZH-CN_TOPIC_0000002572623906"></a>

-   **[bs\_ble\_discovery\_type\_t](#ZH-CN_TOPIC_0000002572623818)**  

-   **[bs\_br\_discovery\_type\_t](#ZH-CN_TOPIC_0000002572623872)**  

-   **[bs\_br\_connect\_reject\_reason\_t](#ZH-CN_TOPIC_0000002572623908)**  

##### bs\_ble\_discovery\_type\_t<a name="ZH-CN_TOPIC_0000002572623818"></a>

【结构体说明】

BLE发现模式类型枚举。

【结构体定义】

```
typedef enum {
    BLE_DISCOVERY_CENTRAL,       /*!< @if Eng enter discovery mode as central device, aka start scan.
                                     @else   作为中央设备进入发现态，即扫描。 @endif */
    BLE_DISCOVERY_PERIPHERAL,    /*!< @if Eng enter discovery mode as peripheral device, aka start advertise.
                                     @else   作为外围设备进入发现态，即广播。 @endif */
} bs_ble_discovery_type_t;
```

【注意事项】

无。

##### bs\_br\_discovery\_type\_t<a name="ZH-CN_TOPIC_0000002572623872"></a>

【结构体说明】

BR/EDR发现模式类型枚举。

【结构体定义】

```
typedef enum {
    BR_DISCOVERY_CENTRAL,    /*!< @if Eng enter discovery mode as central device, start scan.
                                       @else   作为中央设备进入发现态，即查询外围设备。 @endif */
    BR_DISCOVERY_PERIPHERAL, /*!< @if Eng enter discovery mode as peripheral device, start advertise.
                                       @else   作为外围设备进入发现态，即进入广播状态。 @endif */
} bs_br_discovery_type_t;
```

【注意事项】

无。

##### bs\_br\_connect\_reject\_reason\_t<a name="ZH-CN_TOPIC_0000002572623908"></a>

【结构体说明】

BR/EDR拒绝连接原因值枚举。

【结构体定义】

```
typedef enum {
    BR_CONNECT_REJECT_REASON_LIMITED_RESOURCES ,   /*!< @if Eng Rejected due to limited resources.
                                                        @else 资源有限。 @endif */
    BR_CONNECT_REJECT_REASON_SECURITY_REASONS,     /*!< @if Eng Security requirements not being fulfilled.
                                                        @else 未满足安全要求。 @endif */
    BR_CONNECT_REJECT_REASON_UNACCEPTABLE_BD_ADDR, /*!< @if Eng This device does not accept the BD_ADDR.
                                                                This may be because the device will only
                                                                accept connections from specific BD_ADDRs.
                                                        @else 不接受该地址，只接受特定地址连接。 @endif */
} bs_br_connect_reject_reason_t;
```

【注意事项】

无。

#### 函数接口<a name="ZH-CN_TOPIC_0000002603223029"></a>

-   **[bs\_alloc\_device\_id\_by\_addr](#ZH-CN_TOPIC_0000002572623848)**  

-   **[bs\_ble\_start\_discovery](#ZH-CN_TOPIC_0000002603223083)**  

-   **[bs\_ble\_stop\_discovery](#ZH-CN_TOPIC_0000002572623924)**  

-   **[bs\_ble\_start\_pair](#ZH-CN_TOPIC_0000002572783528)**  

-   **[bs\_ble\_stop\_pair](#ZH-CN_TOPIC_0000002603222963)**  

-   **[bs\_ble\_start\_auto\_conn](#ZH-CN_TOPIC_0000002572783586)**  

-   **[bs\_ble\_stop\_auto\_conn](#ZH-CN_TOPIC_0000002603343021)**  

-   **[bs\_ble\_disconnect](#ZH-CN_TOPIC_0000002603342963)**  

-   **[bs\_ble\_disconnect\_all](#ZH-CN_TOPIC_0000002572783468)**  

-   **[bs\_ble\_remove\_pair](#ZH-CN_TOPIC_0000002572623888)**  

-   **[bs\_ble\_remove\_all\_pair](#ZH-CN_TOPIC_0000002603223075)**  

-   **[bs\_br\_start\_discovery](#ZH-CN_TOPIC_0000002572783572)**  

-   **[bs\_br\_stop\_discovery](#ZH-CN_TOPIC_0000002572623832)**  

-   **[bs\_br\_accept\_connect](#ZH-CN_TOPIC_0000002603342933)**  

-   **[bs\_br\_disconnect](#ZH-CN_TOPIC_0000002572783464)**  

-   **[bs\_br\_start\_pair](#ZH-CN_TOPIC_0000002603223007)**  

-   **[bs\_br\_remove\_pair](#ZH-CN_TOPIC_0000002572623812)**  

-   **[bs\_br\_start\_auto\_conn](#ZH-CN_TOPIC_0000002603223063)**  

-   **[bs\_br\_stop\_auto\_conn](#ZH-CN_TOPIC_0000002572623838)**  

##### bs\_alloc\_device\_id\_by\_addr<a name="ZH-CN_TOPIC_0000002572623848"></a>

【描述】

通过地址分配设备ID。

【接口定义】

```
uint16_t bs_alloc_device_id_by_addr(bs_protocol_type_t protocol, uint8_t type, const uint8_t *addr);
```

【参数】

<a name="zh-cn_topic_0000002583791447_table567mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002583791447_row573mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002583791447_p575mcpsimp"><a name="zh-cn_topic_0000002583791447_p575mcpsimp"></a><a name="zh-cn_topic_0000002583791447_p575mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.13999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002583791447_p577mcpsimp"><a name="zh-cn_topic_0000002583791447_p577mcpsimp"></a><a name="zh-cn_topic_0000002583791447_p577mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.86%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002583791447_p579mcpsimp"><a name="zh-cn_topic_0000002583791447_p579mcpsimp"></a><a name="zh-cn_topic_0000002583791447_p579mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002583791447_row580mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583791447_p2264115717419"><a name="zh-cn_topic_0000002583791447_p2264115717419"></a><a name="zh-cn_topic_0000002583791447_p2264115717419"></a>protocol</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583791447_p584mcpsimp"><a name="zh-cn_topic_0000002583791447_p584mcpsimp"></a><a name="zh-cn_topic_0000002583791447_p584mcpsimp"></a>协议类型。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583791447_p586mcpsimp"><a name="zh-cn_topic_0000002583791447_p586mcpsimp"></a><a name="zh-cn_topic_0000002583791447_p586mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002583791447_row587mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583791447_p589mcpsimp"><a name="zh-cn_topic_0000002583791447_p589mcpsimp"></a><a name="zh-cn_topic_0000002583791447_p589mcpsimp"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583791447_p591mcpsimp"><a name="zh-cn_topic_0000002583791447_p591mcpsimp"></a><a name="zh-cn_topic_0000002583791447_p591mcpsimp"></a>地址类型。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583791447_p593mcpsimp"><a name="zh-cn_topic_0000002583791447_p593mcpsimp"></a><a name="zh-cn_topic_0000002583791447_p593mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002583791447_row594mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583791447_p596mcpsimp"><a name="zh-cn_topic_0000002583791447_p596mcpsimp"></a><a name="zh-cn_topic_0000002583791447_p596mcpsimp"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583791447_p4621173415420"><a name="zh-cn_topic_0000002583791447_p4621173415420"></a><a name="zh-cn_topic_0000002583791447_p4621173415420"></a>设备地址。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583791447_p419315004311"><a name="zh-cn_topic_0000002583791447_p419315004311"></a><a name="zh-cn_topic_0000002583791447_p419315004311"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002583791447_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002583791447_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002583791447_p610mcpsimp"><a name="zh-cn_topic_0000002583791447_p610mcpsimp"></a><a name="zh-cn_topic_0000002583791447_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002583791447_p612mcpsimp"><a name="zh-cn_topic_0000002583791447_p612mcpsimp"></a><a name="zh-cn_topic_0000002583791447_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002583791447_p614mcpsimp"><a name="zh-cn_topic_0000002583791447_p614mcpsimp"></a><a name="zh-cn_topic_0000002583791447_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002583791447_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583791447_p19410171112439"><a name="zh-cn_topic_0000002583791447_p19410171112439"></a><a name="zh-cn_topic_0000002583791447_p19410171112439"></a>uint16_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583791447_p1151433164318"><a name="zh-cn_topic_0000002583791447_p1151433164318"></a><a name="zh-cn_topic_0000002583791447_p1151433164318"></a>BS_CONNECTION_UPPER_LIMIT：分配失败；</p>
<p id="zh-cn_topic_0000002583791447_p620mcpsimp"><a name="zh-cn_topic_0000002583791447_p620mcpsimp"></a><a name="zh-cn_topic_0000002583791447_p620mcpsimp"></a>其他：分配的设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583791447_p622mcpsimp"><a name="zh-cn_topic_0000002583791447_p622mcpsimp"></a><a name="zh-cn_topic_0000002583791447_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_ble\_start\_discovery<a name="ZH-CN_TOPIC_0000002603223083"></a>

【描述】

基于BLE启动发现流程。

【接口定义】

```
errcode_t bs_ble_start_discovery(uint8_t type);
```

【参数】

<a name="zh-cn_topic_0000002543414396_table567mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414396_row573mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414396_p575mcpsimp"><a name="zh-cn_topic_0000002543414396_p575mcpsimp"></a><a name="zh-cn_topic_0000002543414396_p575mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.13999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414396_p577mcpsimp"><a name="zh-cn_topic_0000002543414396_p577mcpsimp"></a><a name="zh-cn_topic_0000002543414396_p577mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.86%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414396_p579mcpsimp"><a name="zh-cn_topic_0000002543414396_p579mcpsimp"></a><a name="zh-cn_topic_0000002543414396_p579mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414396_row580mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414396_p582mcpsimp"><a name="zh-cn_topic_0000002543414396_p582mcpsimp"></a><a name="zh-cn_topic_0000002543414396_p582mcpsimp"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414396_p1391310357195"><a name="zh-cn_topic_0000002543414396_p1391310357195"></a><a name="zh-cn_topic_0000002543414396_p1391310357195"></a>发现模式，指示作为中央设备进入发现态或作为外围设备进入发现态。详见“<a href="#ZH-CN_TOPIC_0000002572623818">bs_ble_discovery_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414396_p586mcpsimp"><a name="zh-cn_topic_0000002543414396_p586mcpsimp"></a><a name="zh-cn_topic_0000002543414396_p586mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543414396_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414396_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414396_p610mcpsimp"><a name="zh-cn_topic_0000002543414396_p610mcpsimp"></a><a name="zh-cn_topic_0000002543414396_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414396_p612mcpsimp"><a name="zh-cn_topic_0000002543414396_p612mcpsimp"></a><a name="zh-cn_topic_0000002543414396_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414396_p614mcpsimp"><a name="zh-cn_topic_0000002543414396_p614mcpsimp"></a><a name="zh-cn_topic_0000002543414396_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414396_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414396_p6394174991318"><a name="zh-cn_topic_0000002543414396_p6394174991318"></a><a name="zh-cn_topic_0000002543414396_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414396_p18397105811319"><a name="zh-cn_topic_0000002543414396_p18397105811319"></a><a name="zh-cn_topic_0000002543414396_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543414396_p620mcpsimp"><a name="zh-cn_topic_0000002543414396_p620mcpsimp"></a><a name="zh-cn_topic_0000002543414396_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414396_p622mcpsimp"><a name="zh-cn_topic_0000002543414396_p622mcpsimp"></a><a name="zh-cn_topic_0000002543414396_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_ble\_stop\_discovery<a name="ZH-CN_TOPIC_0000002572623924"></a>

【描述】

基于BLE停止发现流程。

【接口定义】

```
errcode_t bs_ble_stop_discovery(uint8_t type);
```

【参数】

<a name="zh-cn_topic_0000002543254730_table567mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254730_row573mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254730_p575mcpsimp"><a name="zh-cn_topic_0000002543254730_p575mcpsimp"></a><a name="zh-cn_topic_0000002543254730_p575mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.13999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254730_p577mcpsimp"><a name="zh-cn_topic_0000002543254730_p577mcpsimp"></a><a name="zh-cn_topic_0000002543254730_p577mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.86%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254730_p579mcpsimp"><a name="zh-cn_topic_0000002543254730_p579mcpsimp"></a><a name="zh-cn_topic_0000002543254730_p579mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254730_row580mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254730_p582mcpsimp"><a name="zh-cn_topic_0000002543254730_p582mcpsimp"></a><a name="zh-cn_topic_0000002543254730_p582mcpsimp"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254730_p1391310357195"><a name="zh-cn_topic_0000002543254730_p1391310357195"></a><a name="zh-cn_topic_0000002543254730_p1391310357195"></a>发现模式，指示作为中央设备退出发现态或作为外围设备退出发现态。详见“<a href="#ZH-CN_TOPIC_0000002572623818">bs_ble_discovery_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254730_p586mcpsimp"><a name="zh-cn_topic_0000002543254730_p586mcpsimp"></a><a name="zh-cn_topic_0000002543254730_p586mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543254730_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254730_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254730_p610mcpsimp"><a name="zh-cn_topic_0000002543254730_p610mcpsimp"></a><a name="zh-cn_topic_0000002543254730_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254730_p612mcpsimp"><a name="zh-cn_topic_0000002543254730_p612mcpsimp"></a><a name="zh-cn_topic_0000002543254730_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254730_p614mcpsimp"><a name="zh-cn_topic_0000002543254730_p614mcpsimp"></a><a name="zh-cn_topic_0000002543254730_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254730_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254730_p6394174991318"><a name="zh-cn_topic_0000002543254730_p6394174991318"></a><a name="zh-cn_topic_0000002543254730_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254730_p18397105811319"><a name="zh-cn_topic_0000002543254730_p18397105811319"></a><a name="zh-cn_topic_0000002543254730_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543254730_p620mcpsimp"><a name="zh-cn_topic_0000002543254730_p620mcpsimp"></a><a name="zh-cn_topic_0000002543254730_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254730_p622mcpsimp"><a name="zh-cn_topic_0000002543254730_p622mcpsimp"></a><a name="zh-cn_topic_0000002543254730_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_ble\_start\_pair<a name="ZH-CN_TOPIC_0000002572783528"></a>

【描述】

基于BLE进行设备连接并配对对端设备。

【接口定义】

```
errcode_t bs_ble_start_pair(const bs_addr_t *addr);
```

【参数】

<a name="zh-cn_topic_0000002573894643_table567mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894643_row573mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894643_p575mcpsimp"><a name="zh-cn_topic_0000002573894643_p575mcpsimp"></a><a name="zh-cn_topic_0000002573894643_p575mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.13999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894643_p577mcpsimp"><a name="zh-cn_topic_0000002573894643_p577mcpsimp"></a><a name="zh-cn_topic_0000002573894643_p577mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.86%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894643_p579mcpsimp"><a name="zh-cn_topic_0000002573894643_p579mcpsimp"></a><a name="zh-cn_topic_0000002573894643_p579mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894643_row580mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894643_p582mcpsimp"><a name="zh-cn_topic_0000002573894643_p582mcpsimp"></a><a name="zh-cn_topic_0000002573894643_p582mcpsimp"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894643_p1391310357195"><a name="zh-cn_topic_0000002573894643_p1391310357195"></a><a name="zh-cn_topic_0000002573894643_p1391310357195"></a>对端设备地址。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894643_p586mcpsimp"><a name="zh-cn_topic_0000002573894643_p586mcpsimp"></a><a name="zh-cn_topic_0000002573894643_p586mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002573894643_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894643_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894643_p610mcpsimp"><a name="zh-cn_topic_0000002573894643_p610mcpsimp"></a><a name="zh-cn_topic_0000002573894643_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894643_p612mcpsimp"><a name="zh-cn_topic_0000002573894643_p612mcpsimp"></a><a name="zh-cn_topic_0000002573894643_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894643_p614mcpsimp"><a name="zh-cn_topic_0000002573894643_p614mcpsimp"></a><a name="zh-cn_topic_0000002573894643_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894643_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894643_p6394174991318"><a name="zh-cn_topic_0000002573894643_p6394174991318"></a><a name="zh-cn_topic_0000002573894643_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894643_p18397105811319"><a name="zh-cn_topic_0000002573894643_p18397105811319"></a><a name="zh-cn_topic_0000002573894643_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002573894643_p620mcpsimp"><a name="zh-cn_topic_0000002573894643_p620mcpsimp"></a><a name="zh-cn_topic_0000002573894643_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894643_p622mcpsimp"><a name="zh-cn_topic_0000002573894643_p622mcpsimp"></a><a name="zh-cn_topic_0000002573894643_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

该接口仅使用传入指针访问目标数据，不会获取该指针所指向内存的所有权，亦不负责其生命周期管理。

##### bs\_ble\_stop\_pair<a name="ZH-CN_TOPIC_0000002603222963"></a>

【描述】

BLE取消连接对端设备。

【接口定义】

```
errcode_t bs_ble_stop_pair(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002574014617_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014617_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014617_p610mcpsimp"><a name="zh-cn_topic_0000002574014617_p610mcpsimp"></a><a name="zh-cn_topic_0000002574014617_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014617_p612mcpsimp"><a name="zh-cn_topic_0000002574014617_p612mcpsimp"></a><a name="zh-cn_topic_0000002574014617_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014617_p614mcpsimp"><a name="zh-cn_topic_0000002574014617_p614mcpsimp"></a><a name="zh-cn_topic_0000002574014617_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014617_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014617_p6394174991318"><a name="zh-cn_topic_0000002574014617_p6394174991318"></a><a name="zh-cn_topic_0000002574014617_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014617_p18397105811319"><a name="zh-cn_topic_0000002574014617_p18397105811319"></a><a name="zh-cn_topic_0000002574014617_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002574014617_p620mcpsimp"><a name="zh-cn_topic_0000002574014617_p620mcpsimp"></a><a name="zh-cn_topic_0000002574014617_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014617_p622mcpsimp"><a name="zh-cn_topic_0000002574014617_p622mcpsimp"></a><a name="zh-cn_topic_0000002574014617_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_ble\_start\_auto\_conn<a name="ZH-CN_TOPIC_0000002572783586"></a>

【描述】

BLE启动设备回连。

【接口定义】

```
errcode_t bs_ble_start_auto_conn(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002543414398_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414398_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414398_p610mcpsimp"><a name="zh-cn_topic_0000002543414398_p610mcpsimp"></a><a name="zh-cn_topic_0000002543414398_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414398_p612mcpsimp"><a name="zh-cn_topic_0000002543414398_p612mcpsimp"></a><a name="zh-cn_topic_0000002543414398_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414398_p614mcpsimp"><a name="zh-cn_topic_0000002543414398_p614mcpsimp"></a><a name="zh-cn_topic_0000002543414398_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414398_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414398_p6394174991318"><a name="zh-cn_topic_0000002543414398_p6394174991318"></a><a name="zh-cn_topic_0000002543414398_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414398_p18397105811319"><a name="zh-cn_topic_0000002543414398_p18397105811319"></a><a name="zh-cn_topic_0000002543414398_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543414398_p620mcpsimp"><a name="zh-cn_topic_0000002543414398_p620mcpsimp"></a><a name="zh-cn_topic_0000002543414398_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414398_p622mcpsimp"><a name="zh-cn_topic_0000002543414398_p622mcpsimp"></a><a name="zh-cn_topic_0000002543414398_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_ble\_stop\_auto\_conn<a name="ZH-CN_TOPIC_0000002603343021"></a>

【描述】

取消设备回连。

【接口定义】

```
errcode_t bs_ble_stop_auto_conn(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002543254732_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254732_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254732_p610mcpsimp"><a name="zh-cn_topic_0000002543254732_p610mcpsimp"></a><a name="zh-cn_topic_0000002543254732_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254732_p612mcpsimp"><a name="zh-cn_topic_0000002543254732_p612mcpsimp"></a><a name="zh-cn_topic_0000002543254732_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254732_p614mcpsimp"><a name="zh-cn_topic_0000002543254732_p614mcpsimp"></a><a name="zh-cn_topic_0000002543254732_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254732_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254732_p6394174991318"><a name="zh-cn_topic_0000002543254732_p6394174991318"></a><a name="zh-cn_topic_0000002543254732_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254732_p18397105811319"><a name="zh-cn_topic_0000002543254732_p18397105811319"></a><a name="zh-cn_topic_0000002543254732_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543254732_p620mcpsimp"><a name="zh-cn_topic_0000002543254732_p620mcpsimp"></a><a name="zh-cn_topic_0000002543254732_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254732_p622mcpsimp"><a name="zh-cn_topic_0000002543254732_p622mcpsimp"></a><a name="zh-cn_topic_0000002543254732_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_ble\_disconnect<a name="ZH-CN_TOPIC_0000002603342963"></a>

【描述】

基于BLE与指定的对端设备断连。

【接口定义】

```
errcode_t bs_ble_disconnect(uint16_t dev_id);
```

【参数】

<a name="zh-cn_topic_0000002573894645_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894645_row700mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894645_p702mcpsimp"><a name="zh-cn_topic_0000002573894645_p702mcpsimp"></a><a name="zh-cn_topic_0000002573894645_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.25000000000001%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894645_p704mcpsimp"><a name="zh-cn_topic_0000002573894645_p704mcpsimp"></a><a name="zh-cn_topic_0000002573894645_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894645_p706mcpsimp"><a name="zh-cn_topic_0000002573894645_p706mcpsimp"></a><a name="zh-cn_topic_0000002573894645_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894645_row707mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894645_p709mcpsimp"><a name="zh-cn_topic_0000002573894645_p709mcpsimp"></a><a name="zh-cn_topic_0000002573894645_p709mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.25000000000001%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894645_p711mcpsimp"><a name="zh-cn_topic_0000002573894645_p711mcpsimp"></a><a name="zh-cn_topic_0000002573894645_p711mcpsimp"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894645_p713mcpsimp"><a name="zh-cn_topic_0000002573894645_p713mcpsimp"></a><a name="zh-cn_topic_0000002573894645_p713mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002573894645_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894645_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894645_p610mcpsimp"><a name="zh-cn_topic_0000002573894645_p610mcpsimp"></a><a name="zh-cn_topic_0000002573894645_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894645_p612mcpsimp"><a name="zh-cn_topic_0000002573894645_p612mcpsimp"></a><a name="zh-cn_topic_0000002573894645_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894645_p614mcpsimp"><a name="zh-cn_topic_0000002573894645_p614mcpsimp"></a><a name="zh-cn_topic_0000002573894645_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894645_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894645_p6394174991318"><a name="zh-cn_topic_0000002573894645_p6394174991318"></a><a name="zh-cn_topic_0000002573894645_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894645_p18397105811319"><a name="zh-cn_topic_0000002573894645_p18397105811319"></a><a name="zh-cn_topic_0000002573894645_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002573894645_p620mcpsimp"><a name="zh-cn_topic_0000002573894645_p620mcpsimp"></a><a name="zh-cn_topic_0000002573894645_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894645_p622mcpsimp"><a name="zh-cn_topic_0000002573894645_p622mcpsimp"></a><a name="zh-cn_topic_0000002573894645_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_ble\_disconnect\_all<a name="ZH-CN_TOPIC_0000002572783468"></a>

【描述】

基于BLE与对端所有设备断连。

【接口定义】

```
errcode_t bs_ble_disconnect_all(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002574014619_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014619_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014619_p610mcpsimp"><a name="zh-cn_topic_0000002574014619_p610mcpsimp"></a><a name="zh-cn_topic_0000002574014619_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014619_p612mcpsimp"><a name="zh-cn_topic_0000002574014619_p612mcpsimp"></a><a name="zh-cn_topic_0000002574014619_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014619_p614mcpsimp"><a name="zh-cn_topic_0000002574014619_p614mcpsimp"></a><a name="zh-cn_topic_0000002574014619_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014619_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014619_p6394174991318"><a name="zh-cn_topic_0000002574014619_p6394174991318"></a><a name="zh-cn_topic_0000002574014619_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014619_p18397105811319"><a name="zh-cn_topic_0000002574014619_p18397105811319"></a><a name="zh-cn_topic_0000002574014619_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002574014619_p620mcpsimp"><a name="zh-cn_topic_0000002574014619_p620mcpsimp"></a><a name="zh-cn_topic_0000002574014619_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014619_p622mcpsimp"><a name="zh-cn_topic_0000002574014619_p622mcpsimp"></a><a name="zh-cn_topic_0000002574014619_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_ble\_remove\_pair<a name="ZH-CN_TOPIC_0000002572623888"></a>

【描述】

BLE基于设备ID移除指定配对。

【接口定义】

```
errcode_t bs_ble_remove_pair(uint16_t dev_id);
```

【参数】

<a name="zh-cn_topic_0000002543414400_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414400_row700mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414400_p702mcpsimp"><a name="zh-cn_topic_0000002543414400_p702mcpsimp"></a><a name="zh-cn_topic_0000002543414400_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.25000000000001%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414400_p704mcpsimp"><a name="zh-cn_topic_0000002543414400_p704mcpsimp"></a><a name="zh-cn_topic_0000002543414400_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414400_p706mcpsimp"><a name="zh-cn_topic_0000002543414400_p706mcpsimp"></a><a name="zh-cn_topic_0000002543414400_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414400_row707mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414400_p709mcpsimp"><a name="zh-cn_topic_0000002543414400_p709mcpsimp"></a><a name="zh-cn_topic_0000002543414400_p709mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.25000000000001%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414400_p711mcpsimp"><a name="zh-cn_topic_0000002543414400_p711mcpsimp"></a><a name="zh-cn_topic_0000002543414400_p711mcpsimp"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414400_p713mcpsimp"><a name="zh-cn_topic_0000002543414400_p713mcpsimp"></a><a name="zh-cn_topic_0000002543414400_p713mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543414400_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414400_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414400_p610mcpsimp"><a name="zh-cn_topic_0000002543414400_p610mcpsimp"></a><a name="zh-cn_topic_0000002543414400_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414400_p612mcpsimp"><a name="zh-cn_topic_0000002543414400_p612mcpsimp"></a><a name="zh-cn_topic_0000002543414400_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414400_p614mcpsimp"><a name="zh-cn_topic_0000002543414400_p614mcpsimp"></a><a name="zh-cn_topic_0000002543414400_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414400_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414400_p6394174991318"><a name="zh-cn_topic_0000002543414400_p6394174991318"></a><a name="zh-cn_topic_0000002543414400_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414400_p18397105811319"><a name="zh-cn_topic_0000002543414400_p18397105811319"></a><a name="zh-cn_topic_0000002543414400_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543414400_p620mcpsimp"><a name="zh-cn_topic_0000002543414400_p620mcpsimp"></a><a name="zh-cn_topic_0000002543414400_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414400_p622mcpsimp"><a name="zh-cn_topic_0000002543414400_p622mcpsimp"></a><a name="zh-cn_topic_0000002543414400_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_ble\_remove\_all\_pair<a name="ZH-CN_TOPIC_0000002603223075"></a>

【描述】

BLE移除所有配对设备。

【接口定义】

```
errcode_t bs_ble_remove_all_pair(uint16_t *removed_num);
```

【参数】

<a name="zh-cn_topic_0000002543254734_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254734_row700mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254734_p702mcpsimp"><a name="zh-cn_topic_0000002543254734_p702mcpsimp"></a><a name="zh-cn_topic_0000002543254734_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.25000000000001%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254734_p704mcpsimp"><a name="zh-cn_topic_0000002543254734_p704mcpsimp"></a><a name="zh-cn_topic_0000002543254734_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254734_p706mcpsimp"><a name="zh-cn_topic_0000002543254734_p706mcpsimp"></a><a name="zh-cn_topic_0000002543254734_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254734_row707mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254734_p476744010315"><a name="zh-cn_topic_0000002543254734_p476744010315"></a><a name="zh-cn_topic_0000002543254734_p476744010315"></a>removed_num</p>
</td>
<td class="cellrowborder" valign="top" width="61.25000000000001%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254734_p77056403218"><a name="zh-cn_topic_0000002543254734_p77056403218"></a><a name="zh-cn_topic_0000002543254734_p77056403218"></a>被移除设备的数量。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254734_p713mcpsimp"><a name="zh-cn_topic_0000002543254734_p713mcpsimp"></a><a name="zh-cn_topic_0000002543254734_p713mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543254734_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254734_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254734_p610mcpsimp"><a name="zh-cn_topic_0000002543254734_p610mcpsimp"></a><a name="zh-cn_topic_0000002543254734_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254734_p612mcpsimp"><a name="zh-cn_topic_0000002543254734_p612mcpsimp"></a><a name="zh-cn_topic_0000002543254734_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254734_p614mcpsimp"><a name="zh-cn_topic_0000002543254734_p614mcpsimp"></a><a name="zh-cn_topic_0000002543254734_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254734_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254734_p6394174991318"><a name="zh-cn_topic_0000002543254734_p6394174991318"></a><a name="zh-cn_topic_0000002543254734_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254734_p18397105811319"><a name="zh-cn_topic_0000002543254734_p18397105811319"></a><a name="zh-cn_topic_0000002543254734_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543254734_p620mcpsimp"><a name="zh-cn_topic_0000002543254734_p620mcpsimp"></a><a name="zh-cn_topic_0000002543254734_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254734_p622mcpsimp"><a name="zh-cn_topic_0000002543254734_p622mcpsimp"></a><a name="zh-cn_topic_0000002543254734_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_start\_discovery<a name="ZH-CN_TOPIC_0000002572783572"></a>

【描述】

基于BR/EDR进入设备发现模式。

【接口定义】

```
errcode_t bs_br_start_discovery(bs_br_discovery_type_t type);
```

【参数】

<a name="zh-cn_topic_0000002553294632_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002553294632_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002553294632_p702mcpsimp"><a name="zh-cn_topic_0000002553294632_p702mcpsimp"></a><a name="zh-cn_topic_0000002553294632_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002553294632_p704mcpsimp"><a name="zh-cn_topic_0000002553294632_p704mcpsimp"></a><a name="zh-cn_topic_0000002553294632_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002553294632_p706mcpsimp"><a name="zh-cn_topic_0000002553294632_p706mcpsimp"></a><a name="zh-cn_topic_0000002553294632_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002553294632_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553294632_p19978165531013"><a name="zh-cn_topic_0000002553294632_p19978165531013"></a><a name="zh-cn_topic_0000002553294632_p19978165531013"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553294632_p77056403218"><a name="zh-cn_topic_0000002553294632_p77056403218"></a><a name="zh-cn_topic_0000002553294632_p77056403218"></a>设备发现模式类型。详见“<a href="#ZH-CN_TOPIC_0000002572623872">bs_br_discovery_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553294632_p713mcpsimp"><a name="zh-cn_topic_0000002553294632_p713mcpsimp"></a><a name="zh-cn_topic_0000002553294632_p713mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002553294632_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002553294632_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002553294632_p610mcpsimp"><a name="zh-cn_topic_0000002553294632_p610mcpsimp"></a><a name="zh-cn_topic_0000002553294632_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002553294632_p612mcpsimp"><a name="zh-cn_topic_0000002553294632_p612mcpsimp"></a><a name="zh-cn_topic_0000002553294632_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002553294632_p614mcpsimp"><a name="zh-cn_topic_0000002553294632_p614mcpsimp"></a><a name="zh-cn_topic_0000002553294632_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002553294632_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553294632_p6394174991318"><a name="zh-cn_topic_0000002553294632_p6394174991318"></a><a name="zh-cn_topic_0000002553294632_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553294632_p18397105811319"><a name="zh-cn_topic_0000002553294632_p18397105811319"></a><a name="zh-cn_topic_0000002553294632_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002553294632_p620mcpsimp"><a name="zh-cn_topic_0000002553294632_p620mcpsimp"></a><a name="zh-cn_topic_0000002553294632_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553294632_p622mcpsimp"><a name="zh-cn_topic_0000002553294632_p622mcpsimp"></a><a name="zh-cn_topic_0000002553294632_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_stop\_discovery<a name="ZH-CN_TOPIC_0000002572623832"></a>

【描述】

基于BR/EDR退出设备发现模式。

【接口定义】

```
errcode_t bs_br_stop_discovery(bs_br_discovery_type_t type);
```

【参数】

<a name="zh-cn_topic_0000002583854659_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002583854659_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002583854659_p702mcpsimp"><a name="zh-cn_topic_0000002583854659_p702mcpsimp"></a><a name="zh-cn_topic_0000002583854659_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002583854659_p704mcpsimp"><a name="zh-cn_topic_0000002583854659_p704mcpsimp"></a><a name="zh-cn_topic_0000002583854659_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002583854659_p706mcpsimp"><a name="zh-cn_topic_0000002583854659_p706mcpsimp"></a><a name="zh-cn_topic_0000002583854659_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002583854659_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583854659_p19978165531013"><a name="zh-cn_topic_0000002583854659_p19978165531013"></a><a name="zh-cn_topic_0000002583854659_p19978165531013"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583854659_p77056403218"><a name="zh-cn_topic_0000002583854659_p77056403218"></a><a name="zh-cn_topic_0000002583854659_p77056403218"></a>设备发现模式类型。详见“<a href="#ZH-CN_TOPIC_0000002572623872">bs_br_discovery_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583854659_p713mcpsimp"><a name="zh-cn_topic_0000002583854659_p713mcpsimp"></a><a name="zh-cn_topic_0000002583854659_p713mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002583854659_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002583854659_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002583854659_p610mcpsimp"><a name="zh-cn_topic_0000002583854659_p610mcpsimp"></a><a name="zh-cn_topic_0000002583854659_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002583854659_p612mcpsimp"><a name="zh-cn_topic_0000002583854659_p612mcpsimp"></a><a name="zh-cn_topic_0000002583854659_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002583854659_p614mcpsimp"><a name="zh-cn_topic_0000002583854659_p614mcpsimp"></a><a name="zh-cn_topic_0000002583854659_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002583854659_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583854659_p6394174991318"><a name="zh-cn_topic_0000002583854659_p6394174991318"></a><a name="zh-cn_topic_0000002583854659_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583854659_p18397105811319"><a name="zh-cn_topic_0000002583854659_p18397105811319"></a><a name="zh-cn_topic_0000002583854659_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002583854659_p620mcpsimp"><a name="zh-cn_topic_0000002583854659_p620mcpsimp"></a><a name="zh-cn_topic_0000002583854659_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583854659_p622mcpsimp"><a name="zh-cn_topic_0000002583854659_p622mcpsimp"></a><a name="zh-cn_topic_0000002583854659_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_accept\_connect<a name="ZH-CN_TOPIC_0000002603342933"></a>

【描述】

基于BR/EDR与远端设备连接。

【接口定义】

```
errcode_t bs_br_accept_connect(uint16_t dev_id, bool accept, uint8_t reason);
```

【参数】

<a name="zh-cn_topic_0000002553134992_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002553134992_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002553134992_p702mcpsimp"><a name="zh-cn_topic_0000002553134992_p702mcpsimp"></a><a name="zh-cn_topic_0000002553134992_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002553134992_p704mcpsimp"><a name="zh-cn_topic_0000002553134992_p704mcpsimp"></a><a name="zh-cn_topic_0000002553134992_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002553134992_p706mcpsimp"><a name="zh-cn_topic_0000002553134992_p706mcpsimp"></a><a name="zh-cn_topic_0000002553134992_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002553134992_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553134992_p138411416101612"><a name="zh-cn_topic_0000002553134992_p138411416101612"></a><a name="zh-cn_topic_0000002553134992_p138411416101612"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553134992_p77056403218"><a name="zh-cn_topic_0000002553134992_p77056403218"></a><a name="zh-cn_topic_0000002553134992_p77056403218"></a>远端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553134992_p713mcpsimp"><a name="zh-cn_topic_0000002553134992_p713mcpsimp"></a><a name="zh-cn_topic_0000002553134992_p713mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002553134992_row235711219161"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553134992_p8357202116169"><a name="zh-cn_topic_0000002553134992_p8357202116169"></a><a name="zh-cn_topic_0000002553134992_p8357202116169"></a>accept</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553134992_p535716211168"><a name="zh-cn_topic_0000002553134992_p535716211168"></a><a name="zh-cn_topic_0000002553134992_p535716211168"></a>是否接受连接。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553134992_p5357122181615"><a name="zh-cn_topic_0000002553134992_p5357122181615"></a><a name="zh-cn_topic_0000002553134992_p5357122181615"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002553134992_row33312194161"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553134992_p834519151619"><a name="zh-cn_topic_0000002553134992_p834519151619"></a><a name="zh-cn_topic_0000002553134992_p834519151619"></a>reason</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553134992_p434131961620"><a name="zh-cn_topic_0000002553134992_p434131961620"></a><a name="zh-cn_topic_0000002553134992_p434131961620"></a>拒绝原因。详见“<a href="#ZH-CN_TOPIC_0000002572623908">bs_br_connect_reject_reason_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553134992_p63481991614"><a name="zh-cn_topic_0000002553134992_p63481991614"></a><a name="zh-cn_topic_0000002553134992_p63481991614"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002553134992_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002553134992_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002553134992_p610mcpsimp"><a name="zh-cn_topic_0000002553134992_p610mcpsimp"></a><a name="zh-cn_topic_0000002553134992_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002553134992_p612mcpsimp"><a name="zh-cn_topic_0000002553134992_p612mcpsimp"></a><a name="zh-cn_topic_0000002553134992_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002553134992_p614mcpsimp"><a name="zh-cn_topic_0000002553134992_p614mcpsimp"></a><a name="zh-cn_topic_0000002553134992_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002553134992_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553134992_p6394174991318"><a name="zh-cn_topic_0000002553134992_p6394174991318"></a><a name="zh-cn_topic_0000002553134992_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553134992_p18397105811319"><a name="zh-cn_topic_0000002553134992_p18397105811319"></a><a name="zh-cn_topic_0000002553134992_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002553134992_p620mcpsimp"><a name="zh-cn_topic_0000002553134992_p620mcpsimp"></a><a name="zh-cn_topic_0000002553134992_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553134992_p622mcpsimp"><a name="zh-cn_topic_0000002553134992_p622mcpsimp"></a><a name="zh-cn_topic_0000002553134992_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_disconnect<a name="ZH-CN_TOPIC_0000002572783464"></a>

【描述】

基于BR/EDR与远端指定设备断开连接。

【接口定义】

```
errcode_t bs_br_disconnect(uint16_t dev_id);
```

【参数】

<a name="zh-cn_topic_0000002583774639_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002583774639_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002583774639_p702mcpsimp"><a name="zh-cn_topic_0000002583774639_p702mcpsimp"></a><a name="zh-cn_topic_0000002583774639_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002583774639_p704mcpsimp"><a name="zh-cn_topic_0000002583774639_p704mcpsimp"></a><a name="zh-cn_topic_0000002583774639_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002583774639_p706mcpsimp"><a name="zh-cn_topic_0000002583774639_p706mcpsimp"></a><a name="zh-cn_topic_0000002583774639_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002583774639_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583774639_p787792161810"><a name="zh-cn_topic_0000002583774639_p787792161810"></a><a name="zh-cn_topic_0000002583774639_p787792161810"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583774639_p1037418298185"><a name="zh-cn_topic_0000002583774639_p1037418298185"></a><a name="zh-cn_topic_0000002583774639_p1037418298185"></a>远端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583774639_p713mcpsimp"><a name="zh-cn_topic_0000002583774639_p713mcpsimp"></a><a name="zh-cn_topic_0000002583774639_p713mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002583774639_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002583774639_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002583774639_p610mcpsimp"><a name="zh-cn_topic_0000002583774639_p610mcpsimp"></a><a name="zh-cn_topic_0000002583774639_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002583774639_p612mcpsimp"><a name="zh-cn_topic_0000002583774639_p612mcpsimp"></a><a name="zh-cn_topic_0000002583774639_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002583774639_p614mcpsimp"><a name="zh-cn_topic_0000002583774639_p614mcpsimp"></a><a name="zh-cn_topic_0000002583774639_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002583774639_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583774639_p6394174991318"><a name="zh-cn_topic_0000002583774639_p6394174991318"></a><a name="zh-cn_topic_0000002583774639_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583774639_p18397105811319"><a name="zh-cn_topic_0000002583774639_p18397105811319"></a><a name="zh-cn_topic_0000002583774639_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002583774639_p620mcpsimp"><a name="zh-cn_topic_0000002583774639_p620mcpsimp"></a><a name="zh-cn_topic_0000002583774639_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583774639_p622mcpsimp"><a name="zh-cn_topic_0000002583774639_p622mcpsimp"></a><a name="zh-cn_topic_0000002583774639_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_start\_pair<a name="ZH-CN_TOPIC_0000002603223007"></a>

【描述】

基于BR/EDR进行配对设备流程。

【接口定义】

```
errcode_t bs_br_start_pair(const bs_addr_t *addr);
```

【参数】

<a name="zh-cn_topic_0000002553134988_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002553134988_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002553134988_p702mcpsimp"><a name="zh-cn_topic_0000002553134988_p702mcpsimp"></a><a name="zh-cn_topic_0000002553134988_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002553134988_p704mcpsimp"><a name="zh-cn_topic_0000002553134988_p704mcpsimp"></a><a name="zh-cn_topic_0000002553134988_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002553134988_p706mcpsimp"><a name="zh-cn_topic_0000002553134988_p706mcpsimp"></a><a name="zh-cn_topic_0000002553134988_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002553134988_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553134988_p10539133408"><a name="zh-cn_topic_0000002553134988_p10539133408"></a><a name="zh-cn_topic_0000002553134988_p10539133408"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553134988_p77056403218"><a name="zh-cn_topic_0000002553134988_p77056403218"></a><a name="zh-cn_topic_0000002553134988_p77056403218"></a>设备地址。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553134988_p713mcpsimp"><a name="zh-cn_topic_0000002553134988_p713mcpsimp"></a><a name="zh-cn_topic_0000002553134988_p713mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002553134988_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002553134988_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002553134988_p610mcpsimp"><a name="zh-cn_topic_0000002553134988_p610mcpsimp"></a><a name="zh-cn_topic_0000002553134988_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002553134988_p612mcpsimp"><a name="zh-cn_topic_0000002553134988_p612mcpsimp"></a><a name="zh-cn_topic_0000002553134988_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002553134988_p614mcpsimp"><a name="zh-cn_topic_0000002553134988_p614mcpsimp"></a><a name="zh-cn_topic_0000002553134988_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002553134988_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553134988_p6394174991318"><a name="zh-cn_topic_0000002553134988_p6394174991318"></a><a name="zh-cn_topic_0000002553134988_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553134988_p18397105811319"><a name="zh-cn_topic_0000002553134988_p18397105811319"></a><a name="zh-cn_topic_0000002553134988_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002553134988_p620mcpsimp"><a name="zh-cn_topic_0000002553134988_p620mcpsimp"></a><a name="zh-cn_topic_0000002553134988_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553134988_p622mcpsimp"><a name="zh-cn_topic_0000002553134988_p622mcpsimp"></a><a name="zh-cn_topic_0000002553134988_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_remove\_pair<a name="ZH-CN_TOPIC_0000002572623812"></a>

【描述】

BR/DER移除指定配对设备。

【接口定义】

```
errcode_t bs_br_remove_pair(uint16_t dev_id);
```

【参数】

<a name="zh-cn_topic_0000002583774637_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002583774637_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002583774637_p702mcpsimp"><a name="zh-cn_topic_0000002583774637_p702mcpsimp"></a><a name="zh-cn_topic_0000002583774637_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002583774637_p704mcpsimp"><a name="zh-cn_topic_0000002583774637_p704mcpsimp"></a><a name="zh-cn_topic_0000002583774637_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002583774637_p706mcpsimp"><a name="zh-cn_topic_0000002583774637_p706mcpsimp"></a><a name="zh-cn_topic_0000002583774637_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002583774637_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583774637_p10539133408"><a name="zh-cn_topic_0000002583774637_p10539133408"></a><a name="zh-cn_topic_0000002583774637_p10539133408"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583774637_p77056403218"><a name="zh-cn_topic_0000002583774637_p77056403218"></a><a name="zh-cn_topic_0000002583774637_p77056403218"></a>远端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583774637_p713mcpsimp"><a name="zh-cn_topic_0000002583774637_p713mcpsimp"></a><a name="zh-cn_topic_0000002583774637_p713mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002583774637_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002583774637_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002583774637_p610mcpsimp"><a name="zh-cn_topic_0000002583774637_p610mcpsimp"></a><a name="zh-cn_topic_0000002583774637_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002583774637_p612mcpsimp"><a name="zh-cn_topic_0000002583774637_p612mcpsimp"></a><a name="zh-cn_topic_0000002583774637_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002583774637_p614mcpsimp"><a name="zh-cn_topic_0000002583774637_p614mcpsimp"></a><a name="zh-cn_topic_0000002583774637_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002583774637_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583774637_p6394174991318"><a name="zh-cn_topic_0000002583774637_p6394174991318"></a><a name="zh-cn_topic_0000002583774637_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583774637_p18397105811319"><a name="zh-cn_topic_0000002583774637_p18397105811319"></a><a name="zh-cn_topic_0000002583774637_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002583774637_p620mcpsimp"><a name="zh-cn_topic_0000002583774637_p620mcpsimp"></a><a name="zh-cn_topic_0000002583774637_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583774637_p622mcpsimp"><a name="zh-cn_topic_0000002583774637_p622mcpsimp"></a><a name="zh-cn_topic_0000002583774637_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_start\_auto\_conn<a name="ZH-CN_TOPIC_0000002603223063"></a>

【描述】

BR/DER启动设备回连。

【接口定义】

```
errcode_t bs_br_start_auto_conn(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002553294636_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002553294636_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002553294636_p610mcpsimp"><a name="zh-cn_topic_0000002553294636_p610mcpsimp"></a><a name="zh-cn_topic_0000002553294636_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002553294636_p612mcpsimp"><a name="zh-cn_topic_0000002553294636_p612mcpsimp"></a><a name="zh-cn_topic_0000002553294636_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002553294636_p614mcpsimp"><a name="zh-cn_topic_0000002553294636_p614mcpsimp"></a><a name="zh-cn_topic_0000002553294636_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002553294636_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553294636_p6394174991318"><a name="zh-cn_topic_0000002553294636_p6394174991318"></a><a name="zh-cn_topic_0000002553294636_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553294636_p18397105811319"><a name="zh-cn_topic_0000002553294636_p18397105811319"></a><a name="zh-cn_topic_0000002553294636_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002553294636_p620mcpsimp"><a name="zh-cn_topic_0000002553294636_p620mcpsimp"></a><a name="zh-cn_topic_0000002553294636_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553294636_p622mcpsimp"><a name="zh-cn_topic_0000002553294636_p622mcpsimp"></a><a name="zh-cn_topic_0000002553294636_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_stop\_auto\_conn<a name="ZH-CN_TOPIC_0000002572623838"></a>

【描述】

BR/DER停止设备回连。

【接口定义】

```
errcode_t bs_br_stop_auto_conn(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002583854661_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002583854661_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002583854661_p610mcpsimp"><a name="zh-cn_topic_0000002583854661_p610mcpsimp"></a><a name="zh-cn_topic_0000002583854661_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002583854661_p612mcpsimp"><a name="zh-cn_topic_0000002583854661_p612mcpsimp"></a><a name="zh-cn_topic_0000002583854661_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002583854661_p614mcpsimp"><a name="zh-cn_topic_0000002583854661_p614mcpsimp"></a><a name="zh-cn_topic_0000002583854661_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002583854661_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583854661_p6394174991318"><a name="zh-cn_topic_0000002583854661_p6394174991318"></a><a name="zh-cn_topic_0000002583854661_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583854661_p18397105811319"><a name="zh-cn_topic_0000002583854661_p18397105811319"></a><a name="zh-cn_topic_0000002583854661_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002583854661_p620mcpsimp"><a name="zh-cn_topic_0000002583854661_p620mcpsimp"></a><a name="zh-cn_topic_0000002583854661_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583854661_p622mcpsimp"><a name="zh-cn_topic_0000002583854661_p622mcpsimp"></a><a name="zh-cn_topic_0000002583854661_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

### 信息查询<a name="ZH-CN_TOPIC_0000002572623814"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002603222967)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002603342941)**  

#### 数据结构<a name="ZH-CN_TOPIC_0000002603222967"></a>

-   **[bs\_ble\_acquire\_type\_t](#ZH-CN_TOPIC_0000002603223067)**  

-   **[bs\_ble\_acquire\_t](#ZH-CN_TOPIC_0000002603223015)**  

-   **[bs\_br\_acquire\_type\_t](#ZH-CN_TOPIC_0000002603223077)**  

-   **[bs\_br\_acquire\_t](#ZH-CN_TOPIC_0000002603342921)**  

##### bs\_ble\_acquire\_type\_t<a name="ZH-CN_TOPIC_0000002603223067"></a>

【结构体说明】

BLE支持查询的参数类型。

【结构体定义】

```
typedef enum {
    BLE_ACQUIRE_LOCAL_ADDR           = BLE_ACQUIRE_LOCAL | 0x0,
} bs_ble_acquire_type_t;
```

【注意事项】

无。

##### bs\_ble\_acquire\_t<a name="ZH-CN_TOPIC_0000002603223015"></a>

【结构体说明】

BLE查询参数对应的结构体。

【结构体定义】

```
typedef union {
    /**
     * @brief BLE_ACQUIRE_LOCAL_ADDR
     */
    bs_addr_t local_addr;
} bs_ble_acquire_t;
```

【注意事项】

无。

##### bs\_br\_acquire\_type\_t<a name="ZH-CN_TOPIC_0000002603223077"></a>

【结构体说明】

BR/EDR支持查询的参数类型。

【结构体定义】

```
typedef enum {
    BR_ACQUIRE_REMOTE_BOND_LIST           = BR_ACQUIRE_REMOTE | 0x0,
} bs_br_acquire_type_t;
```

【注意事项】

无。

##### bs\_br\_acquire\_t<a name="ZH-CN_TOPIC_0000002603342921"></a>

【结构体说明】

BR/EDR查询参数对应的结构体。

【结构体定义】

```
typedef union {
    /**
     * @brief BR_ACQUIRE_REMOTE_BOND_LIST
     */
    struct bs_br_bond_device_info {
        uint8_t     addr[BS_ADDR_LEN];
        uint16_t    dev_id;
        uint32_t    class_of_device;
        uint8_t     name[BR_DEVICE_NAME_LEN];
    } bond_device_info;
} bs_br_acquire_t;
```

【注意事项】

无。

#### 函数接口<a name="ZH-CN_TOPIC_0000002603342941"></a>

-   **[bs\_acquire](#ZH-CN_TOPIC_0000002572783474)**  

-   **[bs\_get\_addr\_by\_device\_id](#ZH-CN_TOPIC_0000002603342991)**  

-   **[bs\_find\_device\_id\_by\_addr](#ZH-CN_TOPIC_0000002572623938)**  

-   **[bs\_ble\_read\_remote\_device\_rssi](#ZH-CN_TOPIC_0000002603222989)**  

-   **[bs\_ble\_get\_paired\_dev\_infos](#ZH-CN_TOPIC_0000002572623890)**  

-   **[bs\_br\_get\_remote\_device\_info](#ZH-CN_TOPIC_0000002572783568)**  

##### bs\_acquire<a name="ZH-CN_TOPIC_0000002572783474"></a>

【描述】

获取参数。

【接口定义】

```
errcode_t bs_acquire(uint16_t type, void *param, uint16_t *len);
```

【参数】

<a name="zh-cn_topic_0000002553149468_table567mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002553149468_row573mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002553149468_p575mcpsimp"><a name="zh-cn_topic_0000002553149468_p575mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p575mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.13999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002553149468_p577mcpsimp"><a name="zh-cn_topic_0000002553149468_p577mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p577mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.86%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002553149468_p579mcpsimp"><a name="zh-cn_topic_0000002553149468_p579mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p579mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002553149468_row580mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553149468_p582mcpsimp"><a name="zh-cn_topic_0000002553149468_p582mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p582mcpsimp"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553149468_p584mcpsimp"><a name="zh-cn_topic_0000002553149468_p584mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p584mcpsimp"></a>需要查询的参数类型。BLE支持查询的参数类型详见“<a href="#ZH-CN_TOPIC_0000002603223067">bs_ble_acquire_type_t</a>”，BR/EDR支持查询的参数类型详见“<a href="#ZH-CN_TOPIC_0000002603223077">bs_br_acquire_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553149468_p586mcpsimp"><a name="zh-cn_topic_0000002553149468_p586mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p586mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002553149468_row587mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553149468_p589mcpsimp"><a name="zh-cn_topic_0000002553149468_p589mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p589mcpsimp"></a>param</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553149468_p591mcpsimp"><a name="zh-cn_topic_0000002553149468_p591mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p591mcpsimp"></a>用于保存查询返回的参数值。BLE查询参数结构体详见“<a href="#ZH-CN_TOPIC_0000002603223015">bs_ble_acquire_t</a>”，BR/EDR查询参数结构体详见“<a href="#ZH-CN_TOPIC_0000002603342921">bs_br_acquire_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553149468_p593mcpsimp"><a name="zh-cn_topic_0000002553149468_p593mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p593mcpsimp"></a>输出</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002553149468_row594mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553149468_p596mcpsimp"><a name="zh-cn_topic_0000002553149468_p596mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p596mcpsimp"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553149468_p598mcpsimp"><a name="zh-cn_topic_0000002553149468_p598mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p598mcpsimp"></a>用于保存查询返回的参数长度。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553149468_p600mcpsimp"><a name="zh-cn_topic_0000002553149468_p600mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p600mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002553149468_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002553149468_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002553149468_p610mcpsimp"><a name="zh-cn_topic_0000002553149468_p610mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002553149468_p612mcpsimp"><a name="zh-cn_topic_0000002553149468_p612mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002553149468_p614mcpsimp"><a name="zh-cn_topic_0000002553149468_p614mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002553149468_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553149468_p1931854142112"><a name="zh-cn_topic_0000002553149468_p1931854142112"></a><a name="zh-cn_topic_0000002553149468_p1931854142112"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553149468_p18397105811319"><a name="zh-cn_topic_0000002553149468_p18397105811319"></a><a name="zh-cn_topic_0000002553149468_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002553149468_p620mcpsimp"><a name="zh-cn_topic_0000002553149468_p620mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553149468_p622mcpsimp"><a name="zh-cn_topic_0000002553149468_p622mcpsimp"></a><a name="zh-cn_topic_0000002553149468_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_get\_addr\_by\_device\_id<a name="ZH-CN_TOPIC_0000002603342991"></a>

【描述】

通过设备ID获取地址。

【接口定义】

```
bs_addr_t *bs_get_addr_by_device_id(uint16_t dev_id);
```

【参数】

<a name="zh-cn_topic_0000002553151808_table567mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002553151808_row573mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002553151808_p575mcpsimp"><a name="zh-cn_topic_0000002553151808_p575mcpsimp"></a><a name="zh-cn_topic_0000002553151808_p575mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.13999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002553151808_p577mcpsimp"><a name="zh-cn_topic_0000002553151808_p577mcpsimp"></a><a name="zh-cn_topic_0000002553151808_p577mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.86%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002553151808_p579mcpsimp"><a name="zh-cn_topic_0000002553151808_p579mcpsimp"></a><a name="zh-cn_topic_0000002553151808_p579mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002553151808_row580mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553151808_p102154744615"><a name="zh-cn_topic_0000002553151808_p102154744615"></a><a name="zh-cn_topic_0000002553151808_p102154744615"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553151808_p584mcpsimp"><a name="zh-cn_topic_0000002553151808_p584mcpsimp"></a><a name="zh-cn_topic_0000002553151808_p584mcpsimp"></a>设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553151808_p586mcpsimp"><a name="zh-cn_topic_0000002553151808_p586mcpsimp"></a><a name="zh-cn_topic_0000002553151808_p586mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002553151808_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002553151808_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002553151808_p610mcpsimp"><a name="zh-cn_topic_0000002553151808_p610mcpsimp"></a><a name="zh-cn_topic_0000002553151808_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002553151808_p612mcpsimp"><a name="zh-cn_topic_0000002553151808_p612mcpsimp"></a><a name="zh-cn_topic_0000002553151808_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002553151808_p614mcpsimp"><a name="zh-cn_topic_0000002553151808_p614mcpsimp"></a><a name="zh-cn_topic_0000002553151808_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002553151808_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553151808_p6413144713462"><a name="zh-cn_topic_0000002553151808_p6413144713462"></a><a name="zh-cn_topic_0000002553151808_p6413144713462"></a>bs_addr_t *</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553151808_p620mcpsimp"><a name="zh-cn_topic_0000002553151808_p620mcpsimp"></a><a name="zh-cn_topic_0000002553151808_p620mcpsimp"></a>成功返回有效地址，失败返回NULL。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553151808_p622mcpsimp"><a name="zh-cn_topic_0000002553151808_p622mcpsimp"></a><a name="zh-cn_topic_0000002553151808_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_find\_device\_id\_by\_addr<a name="ZH-CN_TOPIC_0000002572623938"></a>

【描述】

通过地址查找设备ID。

【接口定义】

```
uint16_t bs_find_device_id_by_addr(bs_protocol_type_t protocol, uint8_t type, const uint8_t *addr);
```

【参数】

<a name="zh-cn_topic_0000002583871473_table567mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002583871473_row573mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002583871473_p575mcpsimp"><a name="zh-cn_topic_0000002583871473_p575mcpsimp"></a><a name="zh-cn_topic_0000002583871473_p575mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.13999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002583871473_p577mcpsimp"><a name="zh-cn_topic_0000002583871473_p577mcpsimp"></a><a name="zh-cn_topic_0000002583871473_p577mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.86%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002583871473_p579mcpsimp"><a name="zh-cn_topic_0000002583871473_p579mcpsimp"></a><a name="zh-cn_topic_0000002583871473_p579mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002583871473_row580mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583871473_p2264115717419"><a name="zh-cn_topic_0000002583871473_p2264115717419"></a><a name="zh-cn_topic_0000002583871473_p2264115717419"></a>protocol</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583871473_p584mcpsimp"><a name="zh-cn_topic_0000002583871473_p584mcpsimp"></a><a name="zh-cn_topic_0000002583871473_p584mcpsimp"></a>协议类型。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583871473_p586mcpsimp"><a name="zh-cn_topic_0000002583871473_p586mcpsimp"></a><a name="zh-cn_topic_0000002583871473_p586mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002583871473_row587mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583871473_p589mcpsimp"><a name="zh-cn_topic_0000002583871473_p589mcpsimp"></a><a name="zh-cn_topic_0000002583871473_p589mcpsimp"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583871473_p1274612539212"><a name="zh-cn_topic_0000002583871473_p1274612539212"></a><a name="zh-cn_topic_0000002583871473_p1274612539212"></a>地址类型。</p>
<p id="zh-cn_topic_0000002583871473_p591mcpsimp"><a name="zh-cn_topic_0000002583871473_p591mcpsimp"></a><a name="zh-cn_topic_0000002583871473_p591mcpsimp"></a>0：公共地址，1：随机地址。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583871473_p593mcpsimp"><a name="zh-cn_topic_0000002583871473_p593mcpsimp"></a><a name="zh-cn_topic_0000002583871473_p593mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002583871473_row594mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583871473_p596mcpsimp"><a name="zh-cn_topic_0000002583871473_p596mcpsimp"></a><a name="zh-cn_topic_0000002583871473_p596mcpsimp"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583871473_p4621173415420"><a name="zh-cn_topic_0000002583871473_p4621173415420"></a><a name="zh-cn_topic_0000002583871473_p4621173415420"></a>设备地址。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583871473_p419315004311"><a name="zh-cn_topic_0000002583871473_p419315004311"></a><a name="zh-cn_topic_0000002583871473_p419315004311"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002583871473_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002583871473_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002583871473_p610mcpsimp"><a name="zh-cn_topic_0000002583871473_p610mcpsimp"></a><a name="zh-cn_topic_0000002583871473_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002583871473_p612mcpsimp"><a name="zh-cn_topic_0000002583871473_p612mcpsimp"></a><a name="zh-cn_topic_0000002583871473_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002583871473_p614mcpsimp"><a name="zh-cn_topic_0000002583871473_p614mcpsimp"></a><a name="zh-cn_topic_0000002583871473_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002583871473_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002583871473_p1931854142112"><a name="zh-cn_topic_0000002583871473_p1931854142112"></a><a name="zh-cn_topic_0000002583871473_p1931854142112"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002583871473_p1151433164318"><a name="zh-cn_topic_0000002583871473_p1151433164318"></a><a name="zh-cn_topic_0000002583871473_p1151433164318"></a>BS_CONNECTION_UPPER_LIMIT：查询失败；</p>
<p id="zh-cn_topic_0000002583871473_p620mcpsimp"><a name="zh-cn_topic_0000002583871473_p620mcpsimp"></a><a name="zh-cn_topic_0000002583871473_p620mcpsimp"></a>其他：分配的设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002583871473_p622mcpsimp"><a name="zh-cn_topic_0000002583871473_p622mcpsimp"></a><a name="zh-cn_topic_0000002583871473_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_ble\_read\_remote\_device\_rssi<a name="ZH-CN_TOPIC_0000002603222989"></a>

【描述】

BLE读取对端设备的RSSI信息。

【接口定义】

```
errcode_t bs_ble_read_remote_device_rssi(uint16_t dev_id);
```

【参数】

<a name="zh-cn_topic_0000002574014623_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014623_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014623_p702mcpsimp"><a name="zh-cn_topic_0000002574014623_p702mcpsimp"></a><a name="zh-cn_topic_0000002574014623_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014623_p704mcpsimp"><a name="zh-cn_topic_0000002574014623_p704mcpsimp"></a><a name="zh-cn_topic_0000002574014623_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014623_p706mcpsimp"><a name="zh-cn_topic_0000002574014623_p706mcpsimp"></a><a name="zh-cn_topic_0000002574014623_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014623_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014623_p10539133408"><a name="zh-cn_topic_0000002574014623_p10539133408"></a><a name="zh-cn_topic_0000002574014623_p10539133408"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014623_p77056403218"><a name="zh-cn_topic_0000002574014623_p77056403218"></a><a name="zh-cn_topic_0000002574014623_p77056403218"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014623_p713mcpsimp"><a name="zh-cn_topic_0000002574014623_p713mcpsimp"></a><a name="zh-cn_topic_0000002574014623_p713mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002574014623_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014623_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014623_p610mcpsimp"><a name="zh-cn_topic_0000002574014623_p610mcpsimp"></a><a name="zh-cn_topic_0000002574014623_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014623_p612mcpsimp"><a name="zh-cn_topic_0000002574014623_p612mcpsimp"></a><a name="zh-cn_topic_0000002574014623_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014623_p614mcpsimp"><a name="zh-cn_topic_0000002574014623_p614mcpsimp"></a><a name="zh-cn_topic_0000002574014623_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014623_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014623_p6394174991318"><a name="zh-cn_topic_0000002574014623_p6394174991318"></a><a name="zh-cn_topic_0000002574014623_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014623_p18397105811319"><a name="zh-cn_topic_0000002574014623_p18397105811319"></a><a name="zh-cn_topic_0000002574014623_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002574014623_p620mcpsimp"><a name="zh-cn_topic_0000002574014623_p620mcpsimp"></a><a name="zh-cn_topic_0000002574014623_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014623_p622mcpsimp"><a name="zh-cn_topic_0000002574014623_p622mcpsimp"></a><a name="zh-cn_topic_0000002574014623_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

该接口为异步接口，调用该接口读取对端设备的RSSI信息之前需要先注册读取RSSI的回调函数。

##### bs\_ble\_get\_paired\_dev\_infos<a name="ZH-CN_TOPIC_0000002572623890"></a>

【描述】

BLE查询所有配对设备信息。

【接口定义】

```
errcode_t bs_ble_get_paired_dev_infos(bs_ble_paired_dev_info *paired_infos, const uint16_t paired_infos_len, uint16_t *queried_num);
```

【参数】

<a name="zh-cn_topic_0000002543414404_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414404_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414404_p702mcpsimp"><a name="zh-cn_topic_0000002543414404_p702mcpsimp"></a><a name="zh-cn_topic_0000002543414404_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414404_p704mcpsimp"><a name="zh-cn_topic_0000002543414404_p704mcpsimp"></a><a name="zh-cn_topic_0000002543414404_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414404_p706mcpsimp"><a name="zh-cn_topic_0000002543414404_p706mcpsimp"></a><a name="zh-cn_topic_0000002543414404_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414404_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414404_p10539133408"><a name="zh-cn_topic_0000002543414404_p10539133408"></a><a name="zh-cn_topic_0000002543414404_p10539133408"></a>paired_infos</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414404_p19326123717515"><a name="zh-cn_topic_0000002543414404_p19326123717515"></a><a name="zh-cn_topic_0000002543414404_p19326123717515"></a>一个用于保存所有配对设备信息的数组。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414404_p713mcpsimp"><a name="zh-cn_topic_0000002543414404_p713mcpsimp"></a><a name="zh-cn_topic_0000002543414404_p713mcpsimp"></a>输出</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543414404_row26018471448"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414404_p15602134711412"><a name="zh-cn_topic_0000002543414404_p15602134711412"></a><a name="zh-cn_topic_0000002543414404_p15602134711412"></a>paired_infos_len</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414404_p960216471947"><a name="zh-cn_topic_0000002543414404_p960216471947"></a><a name="zh-cn_topic_0000002543414404_p960216471947"></a>数组长度。用于标识数组所能容纳的最大元素个数。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414404_p86025471345"><a name="zh-cn_topic_0000002543414404_p86025471345"></a><a name="zh-cn_topic_0000002543414404_p86025471345"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543414404_row9823204915412"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414404_p1582416495418"><a name="zh-cn_topic_0000002543414404_p1582416495418"></a><a name="zh-cn_topic_0000002543414404_p1582416495418"></a>queried_num</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414404_p1514013448912"><a name="zh-cn_topic_0000002543414404_p1514013448912"></a><a name="zh-cn_topic_0000002543414404_p1514013448912"></a>查询到的配对设备信息的数量。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414404_p7824164915419"><a name="zh-cn_topic_0000002543414404_p7824164915419"></a><a name="zh-cn_topic_0000002543414404_p7824164915419"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543414404_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414404_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414404_p610mcpsimp"><a name="zh-cn_topic_0000002543414404_p610mcpsimp"></a><a name="zh-cn_topic_0000002543414404_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414404_p612mcpsimp"><a name="zh-cn_topic_0000002543414404_p612mcpsimp"></a><a name="zh-cn_topic_0000002543414404_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414404_p614mcpsimp"><a name="zh-cn_topic_0000002543414404_p614mcpsimp"></a><a name="zh-cn_topic_0000002543414404_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414404_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414404_p6394174991318"><a name="zh-cn_topic_0000002543414404_p6394174991318"></a><a name="zh-cn_topic_0000002543414404_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414404_p18397105811319"><a name="zh-cn_topic_0000002543414404_p18397105811319"></a><a name="zh-cn_topic_0000002543414404_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543414404_p620mcpsimp"><a name="zh-cn_topic_0000002543414404_p620mcpsimp"></a><a name="zh-cn_topic_0000002543414404_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414404_p622mcpsimp"><a name="zh-cn_topic_0000002543414404_p622mcpsimp"></a><a name="zh-cn_topic_0000002543414404_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_get\_remote\_device\_info<a name="ZH-CN_TOPIC_0000002572783568"></a>

【描述】

BR/EDR基于设备ID获取对端设备信息。

【接口定义】

```
errcode_t bs_br_get_remote_device_info(uint16_t dev_id, bs_br_remote_device_info_t *device_info);
```

【参数】

<a name="zh-cn_topic_0000002553294638_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002553294638_row700mcpsimp"><th class="cellrowborder" valign="top" width="16.99%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002553294638_p702mcpsimp"><a name="zh-cn_topic_0000002553294638_p702mcpsimp"></a><a name="zh-cn_topic_0000002553294638_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.260000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002553294638_p704mcpsimp"><a name="zh-cn_topic_0000002553294638_p704mcpsimp"></a><a name="zh-cn_topic_0000002553294638_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002553294638_p706mcpsimp"><a name="zh-cn_topic_0000002553294638_p706mcpsimp"></a><a name="zh-cn_topic_0000002553294638_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002553294638_row707mcpsimp"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553294638_p787792161810"><a name="zh-cn_topic_0000002553294638_p787792161810"></a><a name="zh-cn_topic_0000002553294638_p787792161810"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553294638_p1037418298185"><a name="zh-cn_topic_0000002553294638_p1037418298185"></a><a name="zh-cn_topic_0000002553294638_p1037418298185"></a>远端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553294638_p713mcpsimp"><a name="zh-cn_topic_0000002553294638_p713mcpsimp"></a><a name="zh-cn_topic_0000002553294638_p713mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002553294638_row6908911161913"><td class="cellrowborder" valign="top" width="16.99%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553294638_p790841151919"><a name="zh-cn_topic_0000002553294638_p790841151919"></a><a name="zh-cn_topic_0000002553294638_p790841151919"></a>device_info</p>
</td>
<td class="cellrowborder" valign="top" width="61.260000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553294638_p169091911181919"><a name="zh-cn_topic_0000002553294638_p169091911181919"></a><a name="zh-cn_topic_0000002553294638_p169091911181919"></a>远端设备信息。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553294638_p490991161917"><a name="zh-cn_topic_0000002553294638_p490991161917"></a><a name="zh-cn_topic_0000002553294638_p490991161917"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002553294638_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002553294638_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002553294638_p610mcpsimp"><a name="zh-cn_topic_0000002553294638_p610mcpsimp"></a><a name="zh-cn_topic_0000002553294638_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002553294638_p612mcpsimp"><a name="zh-cn_topic_0000002553294638_p612mcpsimp"></a><a name="zh-cn_topic_0000002553294638_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002553294638_p614mcpsimp"><a name="zh-cn_topic_0000002553294638_p614mcpsimp"></a><a name="zh-cn_topic_0000002553294638_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002553294638_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002553294638_p6394174991318"><a name="zh-cn_topic_0000002553294638_p6394174991318"></a><a name="zh-cn_topic_0000002553294638_p6394174991318"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002553294638_p18397105811319"><a name="zh-cn_topic_0000002553294638_p18397105811319"></a><a name="zh-cn_topic_0000002553294638_p18397105811319"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002553294638_p620mcpsimp"><a name="zh-cn_topic_0000002553294638_p620mcpsimp"></a><a name="zh-cn_topic_0000002553294638_p620mcpsimp"></a>其他：失败，错误码见errcode_bt_t，定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002553294638_p622mcpsimp"><a name="zh-cn_topic_0000002553294638_p622mcpsimp"></a><a name="zh-cn_topic_0000002553294638_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

## 开发示例<a name="ZH-CN_TOPIC_0000002603223085"></a>

>![](public_sys-resources/icon-notice.gif) **须知：** 
>示例代码仅用于展示基本处理流程，请勿直接部署至生产环境使用。

-   **[开启BLE广播](#ZH-CN_TOPIC_0000002603343001)**  

-   **[开启BLE扫描](#ZH-CN_TOPIC_0000002572783544)**  

-   **[BLE配对与取消配对](#ZH-CN_TOPIC_0000002603222997)**  

-   **[开启BR/EDR广播](#ZH-CN_TOPIC_0000002603222965)**  

-   **[开启BR/EDR扫描](#ZH-CN_TOPIC_0000002603223013)**  

-   **[BR/EDR配对与取消配对](#ZH-CN_TOPIC_0000002603223041)**  

### 开启BLE广播<a name="ZH-CN_TOPIC_0000002603343001"></a>

1.  注册BLE service回调函数。

    ```
    void demo_stack_state_cbk(uint8_t state)
    {
        // do your things here
    }
    
    void demo_connect_state_cbk(const bs_ble_discovery_result_t *discovery_result, const uint8_t *data, uint16_t len)
    {
        // do your things here
    }
    
    bs_ble_callbacks_t demo_cbk = {
        .stack_state_cbk = demo_stack_state_cbk,
        .connect_state_cbk = demo_connect_state_cbk,
    };
    uint8_t demo_app_id = 0;
    errcode_t ret = bs_ble_register_callbacks(demo_app_id, &demo_cbk);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

2.  使能BLE协议栈。

    ```
    errcode_t ret = bs_ble_enable();
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

3.  协议栈使能后，配置设备信息。

    ```
    uint8_t demo_addr[6] = {0x02, 0x02, 0x02, 0x02, 0x02, 0x02};
    errcode_t ret = bs_config(BLE_CONFIG_LOCAL_ADDR, demo_addr, sizeof(demo_addr));
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    
    char *demo_name = "demo_device_2";
    ret = bs_config(BLE_CONFIG_LOCAL_NAME, (void *)demo_name , strlen(demo_name));
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

4.  开启广播。

    ```
    errcode_t ret = bs_ble_start_discovery(BLE_DISCOVERY_PERIPHERAL);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

### 开启BLE扫描<a name="ZH-CN_TOPIC_0000002572783544"></a>

1.  注册BLE service回调函数。

    ```
    void demo_stack_state_cbk(uint8_t state)
    {
        // do your things here
    }
    
    void demo_connect_state_cbk(const bs_ble_discovery_result_t *discovery_result, const uint8_t *data, uint16_t len)
    {
        // do your things here
    }
    
    void demo_discover_result_cbk(bs_ble_connection_result_t *result)
    {
        // do your things here
    }
    
    bs_ble_callbacks_t demo_cbk = {
        .stack_state_cbk = demo_stack_state_cbk,
        .discover_result_cbk = demo_discover_result_cbk,
        .connect_state_cbk = demo_connect_state_cbk,
    };
    uint8_t demo_app_id = 0;
    errcode_t ret = bs_ble_register_callbacks(demo_app_id, &demo_cbk);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

2.  使能BLE协议栈。

    ```
    errcode_t ret = bs_ble_enable();
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

3.  协议栈使能后，配置设备信息。

    ```
    uint8_t demo_addr[6] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
    errcode_t ret = bs_config(BLE_CONFIG_LOCAL_ADDR, demo_addr, sizeof(demo_addr));
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    
    char *demo_name = "demo_device_1";
    ret = bs_config(BLE_CONFIG_LOCAL_NAME, (void *)demo_name , strlen(demo_name));
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

4.  开启扫描。

    ```
    errcode_t ret = bs_ble_start_discovery(BLE_DISCOVERY_CENTRAL);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

### BLE配对与取消配对<a name="ZH-CN_TOPIC_0000002603222997"></a>

1.  外围设备参考“[开启BLE广播](#ZH-CN_TOPIC_0000002603343001)”步骤开启广播。
2.  中央设备参考“[开启BLE扫描](#ZH-CN_TOPIC_0000002572783544)”步骤开启扫描。
3.  中央设备扫描到外围设备后会通过发现结果上报回调函数进行设备信息上报，中央设备可通过该接口获取设备地址，进行配对。

    ```
    bs_addr_t demo_peer_device_addr = {
        .type = 0,
        .addr = {0x02, 0x02, 0x02, 0x02, 0x02, 0x02},
    };
    errcode_t ret = bs_ble_start_pair(&demo_peer_device_addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

4.  中央设备与外围设备连接成功或配对成功时，会通过连接状态变化回调函数或配对结果回调函数上报对应信息，可据此获取到对应的对端设备ID dev\_id，并可通过该设备ID与对端设备取消配对。

    ```
    errcode_t ret = bs_ble_remove_pair(dev_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

### 开启BR/EDR广播<a name="ZH-CN_TOPIC_0000002603222965"></a>

1.  注册BR/EDR service回调函数。

    ```
    static void demo_stack_state_cb(uint8_t state)
    {
        // do your things here
    }
    
    static void demo_connect_state_cb(uint16_t dev_id, uint8_t conn_state, uint8_t encrypt_state, uint32_t reason)
    {
        // do your things here
    }
    
    static void demo_pair_complete_cb(uint16_t dev_id, const bs_br_pair_info_t *pair_info)
    {
        // do your things here
    }
    
    static void demo_discovery_result_cb(const bs_br_discovery_result_t *discover_result)
    {
        // do your things here
    }
    
    bs_br_callbacks_t demo_cbk = {
        stack_state_cb = demo_stack_state_cb,
        connect_state_cb = demo_connect_state_cb,
        pair_complete_cb = demo_pair_complete_cb,
        discovery_result_cb = demo_discovery_result_cb,
    };
    uint8_t demo_app_id = 0;
    errcode_t ret = bs_br_register_callbacks(demo_app_id, &demo_cbk);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

2.  配置设备信息。

    ```
    bs_addr_t demo_addr = {
        .type = 0,
        .addr = {0x02, 0x02, 0x02, 0x02, 0x02, 0x02},
    };
    
    errcode_t ret = bs_config(BR_CONFIG_LOCAL_ADDR, &demo_addr, sizeof(bs_addrdemo_addr_t));
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    
    char *demo_name = "demo_device_2";
    errcode_t ret = bs_config(BR_CONFIG_LOCAL_NAME, (void *)demo_name, strlen(demo_name));
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    
    bs_br_config_device_class_t demo_device_class = {
        .device_class = 0x600704, // 智能手表
    };
    errcode_t ret = bs_config(BR_CONFIG_LOCAL_DEV_CLASS, &demo_device_class, sizeof(demo_device_class));
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

3.  使能BR/EDR协议栈。

    ```
    errcode_t ret = bs_br_enable();
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

4.  开启广播。

    ```
    errcode_t ret = bs_br_start_discovery(BR_DISCOVERY_PERIPHERAL);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

### 开启BR/EDR扫描<a name="ZH-CN_TOPIC_0000002603223013"></a>

1.  注册BR/EDR service回调函数。

    ```
    static void demo_stack_state_cb(uint8_t state)
    {
        // do your things here
    }
    
    static void demo_connect_state_cb(uint16_t dev_id, uint8_t conn_state, uint8_t encrypt_state, uint32_t reason)
    {
        // do your things here
    }
    
    static void demo_pair_complete_cb(uint16_t dev_id, const bs_br_pair_info_t *pair_info)
    {
        // do your things here
    }
    
    static void demo_discovery_result_cb(const bs_br_discovery_result_t *discover_result)
    {
        // do your things here
    }
    
    bs_br_callbacks_t demo_cbk = {
        stack_state_cb = demo_stack_state_cb,
        connect_state_cb = demo_connect_state_cb,
        pair_complete_cb = demo_pair_complete_cb,
        discovery_result_cb = demo_discovery_result_cb,
    };
    uint8_t demo_app_id = 0;
    errcode_t ret = bs_br_register_callbacks(demo_app_id, &demo_cbk);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

2.  配置设备信息。

    ```
    bs_addr_t demo_addr = {
        .type = 0,
        .addr = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    };
    
    errcode_t ret = bs_config(BR_CONFIG_LOCAL_ADDR, &demo_addr, sizeof(bs_addrdemo_addr_t));
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    
    char *demo_name = "demo_device_1";
    errcode_t ret = bs_config(BR_CONFIG_LOCAL_NAME, (void *)demo_name, strlen(demo_name));
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    
    bs_br_config_device_class_t demo_device_class = {
        .device_class = 0x600704, // 智能手表
    };
    errcode_t ret = bs_config(BR_CONFIG_LOCAL_DEV_CLASS, &demo_device_class, sizeof(demo_device_class));
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

3.  使能BR/EDR协议栈。

    ```
    errcode_t ret = bs_br_enable();
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

4.  开启扫描。

    ```
    errcode_t ret = bs_br_start_discovery(BR_DISCOVERY_CENTRAL);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

### BR/EDR配对与取消配对<a name="ZH-CN_TOPIC_0000002603223041"></a>

1.  外围设备参考[开启BR/EDR广播](#ZH-CN_TOPIC_0000002603222965)步骤开启广播。
2.  中央设备参考[开启BR/EDR扫描](#ZH-CN_TOPIC_0000002603223013)步骤开启扫描。
3.  中央设备扫描到外围设备后会通过发现结果上报回调函数进行设备信息上报，中央设备可通过该接口获取设备地址，进行配对。

    ```
    bs_addr_t demo_peer_device_addr = {
        .type = 0,
        .addr = {0x02, 0x02, 0x02, 0x02, 0x02, 0x02},
    };
    errcode_t ret = bs_br_start_pair(&demo_peer_device_addr);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

4.  中央设备与外围设备连接成功或配对成功时，会通过连接状态变化回调函数或配对结果回调函数上报对应信息，可据此获取到对应的对端设备ID dev\_id，并可通过该设备ID与对端设备取消配对。

    ```
    errcode_t ret = bs_br_remove_pair(dev_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

# 数据通信管理<a name="ZH-CN_TOPIC_0000002603343031"></a>

-   **[概述](#ZH-CN_TOPIC_0000002603343003)**  

-   **[接口说明](#ZH-CN_TOPIC_0000002572783486)**  

-   **[开发示例](#ZH-CN_TOPIC_0000002572623904)**  

## 概述<a name="ZH-CN_TOPIC_0000002603343003"></a>

数据通信管理实现蓝牙设备之间的数据传输与属性交互等功能，接口覆盖客户端与服务端角色开发。

## 接口说明<a name="ZH-CN_TOPIC_0000002572783486"></a>

-   **[SPP服务管理](#ZH-CN_TOPIC_0000002603223005)**  

-   **[GATT服务管理](#ZH-CN_TOPIC_0000002603342989)**  

### SPP服务管理<a name="ZH-CN_TOPIC_0000002603223005"></a>

-   **[初始化与服务配置](#ZH-CN_TOPIC_0000002572623868)**  

-   **[SPP客户端管理](#ZH-CN_TOPIC_0000002603342979)**  

-   **[SPP服务端管理](#ZH-CN_TOPIC_0000002603223033)**  

#### 初始化与服务配置<a name="ZH-CN_TOPIC_0000002572623868"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002572783514)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002572623816)**  

##### 数据结构<a name="ZH-CN_TOPIC_0000002572783514"></a>

-   **[bs\_br\_port\_callbacks\_t](#ZH-CN_TOPIC_0000002572623892)**  

###### bs\_br\_port\_callbacks\_t<a name="ZH-CN_TOPIC_0000002572623892"></a>

【结构体说明】

BR/EDR PORT回调函数注册。

【结构体定义】

```
typedef struct {
    bs_br_port_conn_state_changed_callback conn_state_cbk;         /*!< @if Eng Connection state change callback.
                                                                        @else bs port 连接状态回调函数。@endif */
    bs_br_port_receive_data_callback receive_data_cbk;             /*!< @if Eng Receive data callback.
                                                                        @else bs port 数据接收回调函数。@endif */
    bs_br_port_connection_request_callback conn_req_cbk;           /*!< @if Eng Connection request callback function.
                                                                        @else bs port 连接请求回调函数。@endif */
} bs_br_port_callbacks_t;
```

【注意事项】

无。

##### 函数接口<a name="ZH-CN_TOPIC_0000002572623816"></a>

-   **[bs\_br\_port\_enable](#ZH-CN_TOPIC_0000002572623884)**  

-   **[bs\_br\_port\_disable](#ZH-CN_TOPIC_0000002603223035)**  

-   **[bs\_br\_port\_register\_callbacks](#ZH-CN_TOPIC_0000002572783492)**  

-   **[bs\_br\_port\_unregister\_callbacks](#ZH-CN_TOPIC_0000002572783542)**  

-   **[bs\_br\_port\_write\_data](#ZH-CN_TOPIC_0000002572783582)**  

###### bs\_br\_port\_enable<a name="ZH-CN_TOPIC_0000002572623884"></a>

【描述】

BR/EDR使能SPP PORT port。

【接口定义】

```
errcode_t bs_br_port_enable(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002573894651_table454mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894651_row460mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894651_p462mcpsimp"><a name="zh-cn_topic_0000002573894651_p462mcpsimp"></a><a name="zh-cn_topic_0000002573894651_p462mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894651_p464mcpsimp"><a name="zh-cn_topic_0000002573894651_p464mcpsimp"></a><a name="zh-cn_topic_0000002573894651_p464mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894651_p466mcpsimp"><a name="zh-cn_topic_0000002573894651_p466mcpsimp"></a><a name="zh-cn_topic_0000002573894651_p466mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894651_row467mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894651_p469mcpsimp"><a name="zh-cn_topic_0000002573894651_p469mcpsimp"></a><a name="zh-cn_topic_0000002573894651_p469mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894651_p471mcpsimp"><a name="zh-cn_topic_0000002573894651_p471mcpsimp"></a><a name="zh-cn_topic_0000002573894651_p471mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002573894651_p472mcpsimp"><a name="zh-cn_topic_0000002573894651_p472mcpsimp"></a><a name="zh-cn_topic_0000002573894651_p472mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894651_p474mcpsimp"><a name="zh-cn_topic_0000002573894651_p474mcpsimp"></a><a name="zh-cn_topic_0000002573894651_p474mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_br\_port\_disable<a name="ZH-CN_TOPIC_0000002603223035"></a>

【描述】

BR/EDR去使能SPP port。

【接口定义】

```
errcode_t bs_br_port_disable(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002585180405_table454mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585180405_row460mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585180405_p462mcpsimp"><a name="zh-cn_topic_0000002585180405_p462mcpsimp"></a><a name="zh-cn_topic_0000002585180405_p462mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585180405_p464mcpsimp"><a name="zh-cn_topic_0000002585180405_p464mcpsimp"></a><a name="zh-cn_topic_0000002585180405_p464mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585180405_p466mcpsimp"><a name="zh-cn_topic_0000002585180405_p466mcpsimp"></a><a name="zh-cn_topic_0000002585180405_p466mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585180405_row467mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585180405_p469mcpsimp"><a name="zh-cn_topic_0000002585180405_p469mcpsimp"></a><a name="zh-cn_topic_0000002585180405_p469mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585180405_p471mcpsimp"><a name="zh-cn_topic_0000002585180405_p471mcpsimp"></a><a name="zh-cn_topic_0000002585180405_p471mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585180405_p472mcpsimp"><a name="zh-cn_topic_0000002585180405_p472mcpsimp"></a><a name="zh-cn_topic_0000002585180405_p472mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585180405_p474mcpsimp"><a name="zh-cn_topic_0000002585180405_p474mcpsimp"></a><a name="zh-cn_topic_0000002585180405_p474mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_br\_port\_register\_callbacks<a name="ZH-CN_TOPIC_0000002572783492"></a>

【描述】

BR/EDR SPP注册本端port相关回调。

【接口定义】

```
errcode_t bs_br_port_register_callbacks(bt_uuid_t *uuid, bs_br_port_callbacks_t *func);
```

【参数】

<a name="zh-cn_topic_0000002574014625_table1299mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014625_row1305mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014625_p1307mcpsimp"><a name="zh-cn_topic_0000002574014625_p1307mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1307mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.88%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014625_p1309mcpsimp"><a name="zh-cn_topic_0000002574014625_p1309mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1309mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.12%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014625_p1311mcpsimp"><a name="zh-cn_topic_0000002574014625_p1311mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1311mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014625_row1312mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014625_p1314mcpsimp"><a name="zh-cn_topic_0000002574014625_p1314mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1314mcpsimp"></a>uuid</p>
</td>
<td class="cellrowborder" valign="top" width="61.88%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014625_p1316mcpsimp"><a name="zh-cn_topic_0000002574014625_p1316mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1316mcpsimp"></a>应用的UUID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.12%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014625_p1318mcpsimp"><a name="zh-cn_topic_0000002574014625_p1318mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1318mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002574014625_row1319mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014625_p1321mcpsimp"><a name="zh-cn_topic_0000002574014625_p1321mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1321mcpsimp"></a>func</p>
</td>
<td class="cellrowborder" valign="top" width="61.88%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014625_p1323mcpsimp"><a name="zh-cn_topic_0000002574014625_p1323mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1323mcpsimp"></a>需要注册的回调函数。支持注册的函数详见“<a href="#ZH-CN_TOPIC_0000002572623892">bs_br_port_callbacks_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.12%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014625_p1325mcpsimp"><a name="zh-cn_topic_0000002574014625_p1325mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1325mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002574014625_table1327mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014625_row1333mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014625_p1335mcpsimp"><a name="zh-cn_topic_0000002574014625_p1335mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1335mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014625_p1337mcpsimp"><a name="zh-cn_topic_0000002574014625_p1337mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1337mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014625_p1339mcpsimp"><a name="zh-cn_topic_0000002574014625_p1339mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1339mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014625_row1340mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014625_p1342mcpsimp"><a name="zh-cn_topic_0000002574014625_p1342mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1342mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014625_p1344mcpsimp"><a name="zh-cn_topic_0000002574014625_p1344mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1344mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002574014625_p1345mcpsimp"><a name="zh-cn_topic_0000002574014625_p1345mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1345mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014625_p1347mcpsimp"><a name="zh-cn_topic_0000002574014625_p1347mcpsimp"></a><a name="zh-cn_topic_0000002574014625_p1347mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_br\_port\_unregister\_callbacks<a name="ZH-CN_TOPIC_0000002572783542"></a>

【描述】

BR/EDR SPP本端port相关回调去注册。

【接口定义】

```
errcode_t bs_br_port_unregister_callbacks(bt_uuid_t *uuid);
```

【参数】

<a name="zh-cn_topic_0000002543414406_table1736mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414406_row1742mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414406_p1744mcpsimp"><a name="zh-cn_topic_0000002543414406_p1744mcpsimp"></a><a name="zh-cn_topic_0000002543414406_p1744mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="63.190000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414406_p1746mcpsimp"><a name="zh-cn_topic_0000002543414406_p1746mcpsimp"></a><a name="zh-cn_topic_0000002543414406_p1746mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="19.81%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414406_p1748mcpsimp"><a name="zh-cn_topic_0000002543414406_p1748mcpsimp"></a><a name="zh-cn_topic_0000002543414406_p1748mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414406_row1749mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414406_p1751mcpsimp"><a name="zh-cn_topic_0000002543414406_p1751mcpsimp"></a><a name="zh-cn_topic_0000002543414406_p1751mcpsimp"></a>uuid</p>
</td>
<td class="cellrowborder" valign="top" width="63.190000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414406_p1753mcpsimp"><a name="zh-cn_topic_0000002543414406_p1753mcpsimp"></a><a name="zh-cn_topic_0000002543414406_p1753mcpsimp"></a>应用的UUID。</p>
</td>
<td class="cellrowborder" valign="top" width="19.81%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414406_p1755mcpsimp"><a name="zh-cn_topic_0000002543414406_p1755mcpsimp"></a><a name="zh-cn_topic_0000002543414406_p1755mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543414406_table1757mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414406_row1763mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414406_p1765mcpsimp"><a name="zh-cn_topic_0000002543414406_p1765mcpsimp"></a><a name="zh-cn_topic_0000002543414406_p1765mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="65.42%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414406_p1767mcpsimp"><a name="zh-cn_topic_0000002543414406_p1767mcpsimp"></a><a name="zh-cn_topic_0000002543414406_p1767mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="19.43%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414406_p1769mcpsimp"><a name="zh-cn_topic_0000002543414406_p1769mcpsimp"></a><a name="zh-cn_topic_0000002543414406_p1769mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414406_row1770mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414406_p1772mcpsimp"><a name="zh-cn_topic_0000002543414406_p1772mcpsimp"></a><a name="zh-cn_topic_0000002543414406_p1772mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="65.42%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414406_p1774mcpsimp"><a name="zh-cn_topic_0000002543414406_p1774mcpsimp"></a><a name="zh-cn_topic_0000002543414406_p1774mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543414406_p1775mcpsimp"><a name="zh-cn_topic_0000002543414406_p1775mcpsimp"></a><a name="zh-cn_topic_0000002543414406_p1775mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="19.43%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414406_p1777mcpsimp"><a name="zh-cn_topic_0000002543414406_p1777mcpsimp"></a><a name="zh-cn_topic_0000002543414406_p1777mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_br\_port\_write\_data<a name="ZH-CN_TOPIC_0000002572783582"></a>

【描述】

BR/EDR SPP发送数据接口。

【接口定义】

```
errcode_t bs_br_port_write_data(uint16_t dev_id, uint16_t port, const uint8_t *data, const uint16_t len);
```

【参数】

<a name="zh-cn_topic_0000002543254740_table1212mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254740_row1218mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254740_p1220mcpsimp"><a name="zh-cn_topic_0000002543254740_p1220mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1220mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.839999999999996%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254740_p1222mcpsimp"><a name="zh-cn_topic_0000002543254740_p1222mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1222mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.16%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254740_p1224mcpsimp"><a name="zh-cn_topic_0000002543254740_p1224mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1224mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254740_row1225mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254740_p1227mcpsimp"><a name="zh-cn_topic_0000002543254740_p1227mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1227mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.839999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254740_p1229mcpsimp"><a name="zh-cn_topic_0000002543254740_p1229mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1229mcpsimp"></a>用于多连接情况下指定接收端设备，单连接场景下无效。</p>
</td>
<td class="cellrowborder" valign="top" width="21.16%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254740_p1231mcpsimp"><a name="zh-cn_topic_0000002543254740_p1231mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1231mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254740_row1232mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254740_p1234mcpsimp"><a name="zh-cn_topic_0000002543254740_p1234mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1234mcpsimp"></a>port</p>
</td>
<td class="cellrowborder" valign="top" width="61.839999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254740_p1236mcpsimp"><a name="zh-cn_topic_0000002543254740_p1236mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1236mcpsimp"></a>当前操作port。</p>
</td>
<td class="cellrowborder" valign="top" width="21.16%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254740_p1238mcpsimp"><a name="zh-cn_topic_0000002543254740_p1238mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1238mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254740_row1239mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254740_p1241mcpsimp"><a name="zh-cn_topic_0000002543254740_p1241mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1241mcpsimp"></a>data</p>
</td>
<td class="cellrowborder" valign="top" width="61.839999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254740_p1243mcpsimp"><a name="zh-cn_topic_0000002543254740_p1243mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1243mcpsimp"></a>接收到的数据。</p>
</td>
<td class="cellrowborder" valign="top" width="21.16%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254740_p1245mcpsimp"><a name="zh-cn_topic_0000002543254740_p1245mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1245mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254740_row1246mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254740_p1248mcpsimp"><a name="zh-cn_topic_0000002543254740_p1248mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1248mcpsimp"></a>lend</p>
</td>
<td class="cellrowborder" valign="top" width="61.839999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254740_p1250mcpsimp"><a name="zh-cn_topic_0000002543254740_p1250mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1250mcpsimp"></a>数据长度，单位：Byte。</p>
</td>
<td class="cellrowborder" valign="top" width="21.16%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254740_p1252mcpsimp"><a name="zh-cn_topic_0000002543254740_p1252mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1252mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543254740_table1254mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254740_row1260mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254740_p1262mcpsimp"><a name="zh-cn_topic_0000002543254740_p1262mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1262mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254740_p1264mcpsimp"><a name="zh-cn_topic_0000002543254740_p1264mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1264mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254740_p1266mcpsimp"><a name="zh-cn_topic_0000002543254740_p1266mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1266mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254740_row1267mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254740_p1269mcpsimp"><a name="zh-cn_topic_0000002543254740_p1269mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1269mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254740_p1271mcpsimp"><a name="zh-cn_topic_0000002543254740_p1271mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1271mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543254740_p1272mcpsimp"><a name="zh-cn_topic_0000002543254740_p1272mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1272mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254740_p1274mcpsimp"><a name="zh-cn_topic_0000002543254740_p1274mcpsimp"></a><a name="zh-cn_topic_0000002543254740_p1274mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

客户端、服务端都使用该接口发送数据，发送的业务数据长度最大为990Byte，超过最大长度会发送失败。

#### SPP客户端管理<a name="ZH-CN_TOPIC_0000002603342979"></a>

-   **[函数接口](#ZH-CN_TOPIC_0000002603342995)**  

##### 函数接口<a name="ZH-CN_TOPIC_0000002603342995"></a>

-   **[bs\_br\_port\_create\_remote\_port](#ZH-CN_TOPIC_0000002603222991)**  

-   **[bs\_br\_port\_connect](#ZH-CN_TOPIC_0000002603342985)**  

-   **[bs\_br\_port\_disconnect](#ZH-CN_TOPIC_0000002603223081)**  

###### bs\_br\_port\_create\_remote\_port<a name="ZH-CN_TOPIC_0000002603222991"></a>

【描述】

BR/EDR SPP设置客户端固定port。

【接口定义】

```
errcode_t bs_br_port_create_remote_port(bt_uuid_t *uuid, uint8_t channel, uint16_t port);
```

【参数】

<a name="zh-cn_topic_0000002574014627_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014627_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014627_p1551mcpsimp"><a name="zh-cn_topic_0000002574014627_p1551mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014627_p1553mcpsimp"><a name="zh-cn_topic_0000002574014627_p1553mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014627_p1555mcpsimp"><a name="zh-cn_topic_0000002574014627_p1555mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014627_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014627_p1558mcpsimp"><a name="zh-cn_topic_0000002574014627_p1558mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1558mcpsimp"></a>uuid</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014627_p1560mcpsimp"><a name="zh-cn_topic_0000002574014627_p1560mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1560mcpsimp"></a>应用UUID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014627_p1562mcpsimp"><a name="zh-cn_topic_0000002574014627_p1562mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002574014627_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014627_p1565mcpsimp"><a name="zh-cn_topic_0000002574014627_p1565mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1565mcpsimp"></a>channel</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014627_p1567mcpsimp"><a name="zh-cn_topic_0000002574014627_p1567mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1567mcpsimp"></a>固定通道标识。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014627_p1569mcpsimp"><a name="zh-cn_topic_0000002574014627_p1569mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1569mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002574014627_row1570mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014627_p1572mcpsimp"><a name="zh-cn_topic_0000002574014627_p1572mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1572mcpsimp"></a>port</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014627_p1574mcpsimp"><a name="zh-cn_topic_0000002574014627_p1574mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1574mcpsimp"></a>对端固定的port号。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014627_p1576mcpsimp"><a name="zh-cn_topic_0000002574014627_p1576mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1576mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002574014627_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014627_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014627_p1586mcpsimp"><a name="zh-cn_topic_0000002574014627_p1586mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014627_p1588mcpsimp"><a name="zh-cn_topic_0000002574014627_p1588mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014627_p1590mcpsimp"><a name="zh-cn_topic_0000002574014627_p1590mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014627_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014627_p1593mcpsimp"><a name="zh-cn_topic_0000002574014627_p1593mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014627_p1595mcpsimp"><a name="zh-cn_topic_0000002574014627_p1595mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002574014627_p1596mcpsimp"><a name="zh-cn_topic_0000002574014627_p1596mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014627_p1598mcpsimp"><a name="zh-cn_topic_0000002574014627_p1598mcpsimp"></a><a name="zh-cn_topic_0000002574014627_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

两端使用固定port时，客户端发起连接时，不需要进行服务发现来获取对端port，详见“[bs\_br\_port\_connect](#ZH-CN_TOPIC_0000002603342985)”接口。

###### bs\_br\_port\_connect<a name="ZH-CN_TOPIC_0000002603342985"></a>

【描述】

BR/EDR SPP port连接，服务发现并创建客户端port，收到响应后在bs\_br\_port\_conn\_state\_changed\_callback中返回本端port和连接状态。

【接口定义】

```
errcode_t bs_br_port_connect(uint16_t dev_id, bt_uuid_t *uuid, uint8_t find_remote_flag);
```

【参数】

<a name="zh-cn_topic_0000002543414408_table897mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414408_row903mcpsimp"><th class="cellrowborder" valign="top" width="20.000000000000004%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414408_p905mcpsimp"><a name="zh-cn_topic_0000002543414408_p905mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p905mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="58.510000000000005%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414408_p907mcpsimp"><a name="zh-cn_topic_0000002543414408_p907mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p907mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.490000000000002%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414408_p909mcpsimp"><a name="zh-cn_topic_0000002543414408_p909mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p909mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414408_row910mcpsimp"><td class="cellrowborder" valign="top" width="20.000000000000004%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414408_p912mcpsimp"><a name="zh-cn_topic_0000002543414408_p912mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p912mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="58.510000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414408_p914mcpsimp"><a name="zh-cn_topic_0000002543414408_p914mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p914mcpsimp"></a>设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.490000000000002%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414408_p916mcpsimp"><a name="zh-cn_topic_0000002543414408_p916mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p916mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543414408_row917mcpsimp"><td class="cellrowborder" valign="top" width="20.000000000000004%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414408_p919mcpsimp"><a name="zh-cn_topic_0000002543414408_p919mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p919mcpsimp"></a>uuid</p>
</td>
<td class="cellrowborder" valign="top" width="58.510000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414408_p921mcpsimp"><a name="zh-cn_topic_0000002543414408_p921mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p921mcpsimp"></a>应用UUID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.490000000000002%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414408_p923mcpsimp"><a name="zh-cn_topic_0000002543414408_p923mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p923mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543414408_row924mcpsimp"><td class="cellrowborder" valign="top" width="20.000000000000004%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414408_p926mcpsimp"><a name="zh-cn_topic_0000002543414408_p926mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p926mcpsimp"></a>find_remote_flag</p>
</td>
<td class="cellrowborder" valign="top" width="58.510000000000005%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414408_p133671655132518"><a name="zh-cn_topic_0000002543414408_p133671655132518"></a><a name="zh-cn_topic_0000002543414408_p133671655132518"></a>是否需要服务发现对端port。</p>
<p id="zh-cn_topic_0000002543414408_p26971558102514"><a name="zh-cn_topic_0000002543414408_p26971558102514"></a><a name="zh-cn_topic_0000002543414408_p26971558102514"></a>0：不做服务发现，1：需要做服务发现。</p>
<p id="zh-cn_topic_0000002543414408_p928mcpsimp"><a name="zh-cn_topic_0000002543414408_p928mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p928mcpsimp"></a>若为固定port则不需要做服务发现。</p>
</td>
<td class="cellrowborder" valign="top" width="21.490000000000002%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414408_p930mcpsimp"><a name="zh-cn_topic_0000002543414408_p930mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p930mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543414408_table932mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414408_row938mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414408_p940mcpsimp"><a name="zh-cn_topic_0000002543414408_p940mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p940mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414408_p942mcpsimp"><a name="zh-cn_topic_0000002543414408_p942mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p942mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414408_p944mcpsimp"><a name="zh-cn_topic_0000002543414408_p944mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p944mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414408_row945mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414408_p947mcpsimp"><a name="zh-cn_topic_0000002543414408_p947mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p947mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414408_p949mcpsimp"><a name="zh-cn_topic_0000002543414408_p949mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p949mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543414408_p950mcpsimp"><a name="zh-cn_topic_0000002543414408_p950mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p950mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414408_p952mcpsimp"><a name="zh-cn_topic_0000002543414408_p952mcpsimp"></a><a name="zh-cn_topic_0000002543414408_p952mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

使用固定port时，不需要进行服务发现。

###### bs\_br\_port\_disconnect<a name="ZH-CN_TOPIC_0000002603223081"></a>

【描述】

BR/EDR SPP客户端断开连接。

【接口定义】

```
errcode_t bs_br_port_disconnect(uint16_t dev_id, uint16_t port);
```

【参数】

<a name="zh-cn_topic_0000002543254742_table1033mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254742_row1039mcpsimp"><th class="cellrowborder" valign="top" width="20%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254742_p1041mcpsimp"><a name="zh-cn_topic_0000002543254742_p1041mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1041mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="59.160000000000004%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254742_p1043mcpsimp"><a name="zh-cn_topic_0000002543254742_p1043mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1043mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.84%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254742_p1045mcpsimp"><a name="zh-cn_topic_0000002543254742_p1045mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1045mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254742_row1046mcpsimp"><td class="cellrowborder" valign="top" width="20%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254742_p1048mcpsimp"><a name="zh-cn_topic_0000002543254742_p1048mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1048mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="59.160000000000004%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254742_p1050mcpsimp"><a name="zh-cn_topic_0000002543254742_p1050mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1050mcpsimp"></a>设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.84%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254742_p1052mcpsimp"><a name="zh-cn_topic_0000002543254742_p1052mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1052mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254742_row1053mcpsimp"><td class="cellrowborder" valign="top" width="20%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254742_p1055mcpsimp"><a name="zh-cn_topic_0000002543254742_p1055mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1055mcpsimp"></a>port</p>
</td>
<td class="cellrowborder" valign="top" width="59.160000000000004%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254742_p1057mcpsimp"><a name="zh-cn_topic_0000002543254742_p1057mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1057mcpsimp"></a>客户端port。</p>
</td>
<td class="cellrowborder" valign="top" width="20.84%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254742_p1059mcpsimp"><a name="zh-cn_topic_0000002543254742_p1059mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1059mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543254742_table1061mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254742_row1067mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254742_p1069mcpsimp"><a name="zh-cn_topic_0000002543254742_p1069mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1069mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254742_p1071mcpsimp"><a name="zh-cn_topic_0000002543254742_p1071mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1071mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254742_p1073mcpsimp"><a name="zh-cn_topic_0000002543254742_p1073mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1073mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254742_row1074mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254742_p1076mcpsimp"><a name="zh-cn_topic_0000002543254742_p1076mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1076mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254742_p1078mcpsimp"><a name="zh-cn_topic_0000002543254742_p1078mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1078mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543254742_p1079mcpsimp"><a name="zh-cn_topic_0000002543254742_p1079mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1079mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254742_p1081mcpsimp"><a name="zh-cn_topic_0000002543254742_p1081mcpsimp"></a><a name="zh-cn_topic_0000002543254742_p1081mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

#### SPP服务端管理<a name="ZH-CN_TOPIC_0000002603223033"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002603342951)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002603342959)**  

##### 数据结构<a name="ZH-CN_TOPIC_0000002603342951"></a>

-   **[bs\_br\_port\_create\_info\_t](#ZH-CN_TOPIC_0000002603222977)**  

###### bs\_br\_port\_create\_info\_t<a name="ZH-CN_TOPIC_0000002603222977"></a>

【结构体说明】

BR/EDR SPP服务端创建port参数信息。

【结构体定义】

```
typedef struct {
    bt_uuid_t uuid;         /*!< @if Eng Application UUID. Supports 2 or 16 bits.
                                    @else 应用uuid。支持2位或16位 @endif */
    char *name;             /*!< @if Eng port server name.
                                    @else  port Server名称。 @endif */
    uint8_t name_len;      /*!< @if Eng Source port server name length.
                                    @else  port Server名称长度。 @endif */
} bs_br_port_create_info_t;
```

【注意事项】

无。

##### 函数接口<a name="ZH-CN_TOPIC_0000002603342959"></a>

-   **[bs\_br\_port\_create\_server\_port](#ZH-CN_TOPIC_0000002572783494)**  

-   **[bs\_br\_port\_destroy](#ZH-CN_TOPIC_0000002572783530)**  

-   **[bs\_br\_port\_send\_conn\_rsp](#ZH-CN_TOPIC_0000002572623934)**  

###### bs\_br\_port\_create\_server\_port<a name="ZH-CN_TOPIC_0000002572783494"></a>

【描述】

BR/EDR SPP创建服务端port。

【接口定义】

```
errcode_t bs_br_port_create_server_port(bs_br_port_create_info_t *port_create_info, uint8_t channel, uint8_t auto_generate_port_flag, uint16_t *port);
```

【参数】

<a name="zh-cn_topic_0000002574014629_table1471mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014629_row1477mcpsimp"><th class="cellrowborder" valign="top" width="24.47%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014629_p1479mcpsimp"><a name="zh-cn_topic_0000002574014629_p1479mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1479mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="55.089999999999996%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014629_p1481mcpsimp"><a name="zh-cn_topic_0000002574014629_p1481mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1481mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014629_p1483mcpsimp"><a name="zh-cn_topic_0000002574014629_p1483mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1483mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014629_row1484mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014629_p1486mcpsimp"><a name="zh-cn_topic_0000002574014629_p1486mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1486mcpsimp"></a>port_create_info</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014629_p1488mcpsimp"><a name="zh-cn_topic_0000002574014629_p1488mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1488mcpsimp"></a>创建server端port相关参数：uuid、name、name_len。详见“<a href="#ZH-CN_TOPIC_0000002603222977">bs_br_port_create_info_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014629_p1490mcpsimp"><a name="zh-cn_topic_0000002574014629_p1490mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1490mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002574014629_row1491mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014629_p1493mcpsimp"><a name="zh-cn_topic_0000002574014629_p1493mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1493mcpsimp"></a>channel</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014629_p1495mcpsimp"><a name="zh-cn_topic_0000002574014629_p1495mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1495mcpsimp"></a>固定通道标识，和auto_generate_port_flag配合使用。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014629_p1497mcpsimp"><a name="zh-cn_topic_0000002574014629_p1497mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1497mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002574014629_row1498mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014629_p1500mcpsimp"><a name="zh-cn_topic_0000002574014629_p1500mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1500mcpsimp"></a>auto_generate_port_flag</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014629_p95957525491"><a name="zh-cn_topic_0000002574014629_p95957525491"></a><a name="zh-cn_topic_0000002574014629_p95957525491"></a>是否由profile创建port。</p>
<p id="zh-cn_topic_0000002574014629_p224895554910"><a name="zh-cn_topic_0000002574014629_p224895554910"></a><a name="zh-cn_topic_0000002574014629_p224895554910"></a>0：由profile分配；</p>
<p id="zh-cn_topic_0000002574014629_p1502mcpsimp"><a name="zh-cn_topic_0000002574014629_p1502mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1502mcpsimp"></a>1：指定port。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014629_p1504mcpsimp"><a name="zh-cn_topic_0000002574014629_p1504mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1504mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002574014629_row1505mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014629_p1507mcpsimp"><a name="zh-cn_topic_0000002574014629_p1507mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1507mcpsimp"></a>port</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014629_p1509mcpsimp"><a name="zh-cn_topic_0000002574014629_p1509mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1509mcpsimp"></a>自动分配：分配的port号；</p>
<p id="zh-cn_topic_0000002574014629_p1510mcpsimp"><a name="zh-cn_topic_0000002574014629_p1510mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1510mcpsimp"></a>指定：固定的port号。</p>
<p id="zh-cn_topic_0000002574014629_p1164193252713"><a name="zh-cn_topic_0000002574014629_p1164193252713"></a><a name="zh-cn_topic_0000002574014629_p1164193252713"></a>注：自动分配是协议栈的profile去分配，指定port是client和server约定一个固定port号进行使用。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014629_p1512mcpsimp"><a name="zh-cn_topic_0000002574014629_p1512mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1512mcpsimp"></a>输入/输出</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002574014629_table1514mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014629_row1520mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014629_p1522mcpsimp"><a name="zh-cn_topic_0000002574014629_p1522mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1522mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014629_p1524mcpsimp"><a name="zh-cn_topic_0000002574014629_p1524mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1524mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014629_p1526mcpsimp"><a name="zh-cn_topic_0000002574014629_p1526mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1526mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014629_row1527mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014629_p1529mcpsimp"><a name="zh-cn_topic_0000002574014629_p1529mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1529mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014629_p1531mcpsimp"><a name="zh-cn_topic_0000002574014629_p1531mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1531mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002574014629_p1532mcpsimp"><a name="zh-cn_topic_0000002574014629_p1532mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1532mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014629_p1534mcpsimp"><a name="zh-cn_topic_0000002574014629_p1534mcpsimp"></a><a name="zh-cn_topic_0000002574014629_p1534mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_br\_port\_destroy<a name="ZH-CN_TOPIC_0000002572783530"></a>

【描述】

BR/EDR SPP服务端删除port。

【接口定义】

```
errcode_t bs_br_port_destroy(uint16_t port);
```

【参数】

<a name="zh-cn_topic_0000002543414410_table1356mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414410_row1362mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414410_p1364mcpsimp"><a name="zh-cn_topic_0000002543414410_p1364mcpsimp"></a><a name="zh-cn_topic_0000002543414410_p1364mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.529999999999994%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414410_p1366mcpsimp"><a name="zh-cn_topic_0000002543414410_p1366mcpsimp"></a><a name="zh-cn_topic_0000002543414410_p1366mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.47%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414410_p1368mcpsimp"><a name="zh-cn_topic_0000002543414410_p1368mcpsimp"></a><a name="zh-cn_topic_0000002543414410_p1368mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414410_row1369mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414410_p1371mcpsimp"><a name="zh-cn_topic_0000002543414410_p1371mcpsimp"></a><a name="zh-cn_topic_0000002543414410_p1371mcpsimp"></a>port</p>
</td>
<td class="cellrowborder" valign="top" width="61.529999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414410_p1373mcpsimp"><a name="zh-cn_topic_0000002543414410_p1373mcpsimp"></a><a name="zh-cn_topic_0000002543414410_p1373mcpsimp"></a>待删除的的port。</p>
</td>
<td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414410_p1375mcpsimp"><a name="zh-cn_topic_0000002543414410_p1375mcpsimp"></a><a name="zh-cn_topic_0000002543414410_p1375mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543414410_table1377mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414410_row1383mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414410_p1385mcpsimp"><a name="zh-cn_topic_0000002543414410_p1385mcpsimp"></a><a name="zh-cn_topic_0000002543414410_p1385mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414410_p1387mcpsimp"><a name="zh-cn_topic_0000002543414410_p1387mcpsimp"></a><a name="zh-cn_topic_0000002543414410_p1387mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414410_p1389mcpsimp"><a name="zh-cn_topic_0000002543414410_p1389mcpsimp"></a><a name="zh-cn_topic_0000002543414410_p1389mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414410_row1390mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414410_p1392mcpsimp"><a name="zh-cn_topic_0000002543414410_p1392mcpsimp"></a><a name="zh-cn_topic_0000002543414410_p1392mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414410_p1394mcpsimp"><a name="zh-cn_topic_0000002543414410_p1394mcpsimp"></a><a name="zh-cn_topic_0000002543414410_p1394mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543414410_p1395mcpsimp"><a name="zh-cn_topic_0000002543414410_p1395mcpsimp"></a><a name="zh-cn_topic_0000002543414410_p1395mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414410_p1397mcpsimp"><a name="zh-cn_topic_0000002543414410_p1397mcpsimp"></a><a name="zh-cn_topic_0000002543414410_p1397mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_br\_port\_send\_conn\_rsp<a name="ZH-CN_TOPIC_0000002572623934"></a>

【描述】

BR/EDR SPP向远程设备发送连接响应，指示是否接受或拒绝连接请求。

【接口定义】

```
errcode_t bs_br_port_send_conn_rsp(uint16_t port, uint8_t is_accept);
```

【参数】

<a name="zh-cn_topic_0000002543254744_table1787mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254744_row1793mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254744_p1795mcpsimp"><a name="zh-cn_topic_0000002543254744_p1795mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1795mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.029999999999994%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254744_p1797mcpsimp"><a name="zh-cn_topic_0000002543254744_p1797mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1797mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.97%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254744_p1799mcpsimp"><a name="zh-cn_topic_0000002543254744_p1799mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1799mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254744_row1800mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254744_p1802mcpsimp"><a name="zh-cn_topic_0000002543254744_p1802mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1802mcpsimp"></a>port</p>
</td>
<td class="cellrowborder" valign="top" width="62.029999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254744_p1804mcpsimp"><a name="zh-cn_topic_0000002543254744_p1804mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1804mcpsimp"></a>要回复的port连接的端口号。</p>
</td>
<td class="cellrowborder" valign="top" width="20.97%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254744_p1806mcpsimp"><a name="zh-cn_topic_0000002543254744_p1806mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1806mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254744_row1807mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254744_p1809mcpsimp"><a name="zh-cn_topic_0000002543254744_p1809mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1809mcpsimp"></a>is_accept</p>
</td>
<td class="cellrowborder" valign="top" width="62.029999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254744_p97265215018"><a name="zh-cn_topic_0000002543254744_p97265215018"></a><a name="zh-cn_topic_0000002543254744_p97265215018"></a>指示是否接受连接请求。</p>
<p id="zh-cn_topic_0000002543254744_p13735655017"><a name="zh-cn_topic_0000002543254744_p13735655017"></a><a name="zh-cn_topic_0000002543254744_p13735655017"></a>0：拒绝；</p>
<p id="zh-cn_topic_0000002543254744_p1811mcpsimp"><a name="zh-cn_topic_0000002543254744_p1811mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1811mcpsimp"></a>1：接受。</p>
</td>
<td class="cellrowborder" valign="top" width="20.97%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254744_p1813mcpsimp"><a name="zh-cn_topic_0000002543254744_p1813mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1813mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543254744_table1815mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254744_row1821mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254744_p1823mcpsimp"><a name="zh-cn_topic_0000002543254744_p1823mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1823mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254744_p1825mcpsimp"><a name="zh-cn_topic_0000002543254744_p1825mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1825mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254744_p1827mcpsimp"><a name="zh-cn_topic_0000002543254744_p1827mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1827mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254744_row1828mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254744_p1830mcpsimp"><a name="zh-cn_topic_0000002543254744_p1830mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1830mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254744_p1832mcpsimp"><a name="zh-cn_topic_0000002543254744_p1832mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1832mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543254744_p1833mcpsimp"><a name="zh-cn_topic_0000002543254744_p1833mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1833mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254744_p1835mcpsimp"><a name="zh-cn_topic_0000002543254744_p1835mcpsimp"></a><a name="zh-cn_topic_0000002543254744_p1835mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

### GATT服务管理<a name="ZH-CN_TOPIC_0000002603342989"></a>

-   **[GATT客户端管理](#ZH-CN_TOPIC_0000002603223027)**  

-   **[GATT服务端管理](#ZH-CN_TOPIC_0000002603342977)**  

#### GATT客户端管理<a name="ZH-CN_TOPIC_0000002603223027"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002603223053)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002572623854)**  

##### 数据结构<a name="ZH-CN_TOPIC_0000002603223053"></a>

-   **[bs\_gattc\_operation\_cbk\_t](#ZH-CN_TOPIC_0000002603342919)**  

-   **[bs\_gattc\_write\_value\_t](#ZH-CN_TOPIC_0000002603223079)**  

###### bs\_gattc\_operation\_cbk\_t<a name="ZH-CN_TOPIC_0000002603342919"></a>

【结构体说明】

BLE GATT客户端操作类接口回调函数接口定义。

【结构体说明】

```
typedef struct {
    bs_gattc_read_write_callbacks_t read_write_cbk; /*!< @if Eng Struct of gatt client read and write callback function.
                                                      @else gatt client读写回调函数接口定义。
                                                      @endif */
    bs_gattc_notication_indication_callbacks_t notication_indication_cbk;
    /*!< @if Eng Struct of gatt client notification and indication callback function.
         @else gatt client notification和indication回调函数接口定义。
         @endif */
    bs_gattc_mtu_changed_callback mtu_changed_cb; /*!< @if Eng Mtu changed callback,
                                                    @else mtu改变回调函数 @endif */
} bs_gattc_operation_cbk_t;
```

【注意事项】

无。

###### bs\_gattc\_write\_value\_t<a name="ZH-CN_TOPIC_0000002603223079"></a>

【结构体说明】

BLE GATT客户端写信息。

【结构体说明】

```
typedef struct {
    uint16_t handle;   /*!< @if Eng attribute handle.
                            @else   属性句柄。@endif */
    bool is_need_confirm; /*!< @if WWhether to require peer confirmation: false: no peer response needed, true: the
                           result is returned in the bs_gattc_write_cfm_callback callback after peer confirmation.
                           @else   是否需要对端确认, false：无需对端回复,
                           true:对端确认后结果在bs_gattc_write_cfm_callback回调中返回。@endif */
    uint16_t data_len; /*!< @if Eng data length.
                            @else   数据长度。@endif */
    uint8_t *data;     /*!< @if Eng data.
                            @else   数据。@endif */
} bs_gattc_write_value_t;
```

【注意事项】

无。

##### 函数接口<a name="ZH-CN_TOPIC_0000002572623854"></a>

-   **[bs\_gattc\_register](#ZH-CN_TOPIC_0000002603342931)**  

-   **[bs\_gattc\_unregister](#ZH-CN_TOPIC_0000002572623882)**  

-   **[bs\_gattc\_find\_all\_service](#ZH-CN_TOPIC_0000002572783584)**  

-   **[bs\_gattc\_find\_service\_by\_uuid](#ZH-CN_TOPIC_0000002603342975)**  

-   **[bs\_gattc\_register\_callbacks](#ZH-CN_TOPIC_0000002603342925)**  

-   **[bs\_gattc\_unregister\_callbacks](#ZH-CN_TOPIC_0000002572783532)**  

-   **[bs\_gattc\_read](#ZH-CN_TOPIC_0000002572623936)**  

-   **[bs\_gattc\_write](#ZH-CN_TOPIC_0000002572783518)**  

-   **[bs\_gattc\_exchange\_mtu\_req](#ZH-CN_TOPIC_0000002603222979)**  

###### bs\_gattc\_register<a name="ZH-CN_TOPIC_0000002603342931"></a>

【描述】

BLE注册GATT客户端。

【接口定义】

```
errcode_t bs_gattc_register(bt_uuid_t *app_uuid,uint8_t *client_id);
```

【参数】

<a name="zh-cn_topic_0000002574014631_table483mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014631_row489mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014631_p491mcpsimp"><a name="zh-cn_topic_0000002574014631_p491mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p491mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.46000000000001%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014631_p493mcpsimp"><a name="zh-cn_topic_0000002574014631_p493mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p493mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.54%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014631_p495mcpsimp"><a name="zh-cn_topic_0000002574014631_p495mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p495mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014631_row496mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014631_p498mcpsimp"><a name="zh-cn_topic_0000002574014631_p498mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p498mcpsimp"></a>app_uuid</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014631_p500mcpsimp"><a name="zh-cn_topic_0000002574014631_p500mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p500mcpsimp"></a>上层应用UUID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.54%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014631_p502mcpsimp"><a name="zh-cn_topic_0000002574014631_p502mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p502mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002574014631_row503mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014631_p505mcpsimp"><a name="zh-cn_topic_0000002574014631_p505mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p505mcpsimp"></a>client_id</p>
</td>
<td class="cellrowborder" valign="top" width="62.46000000000001%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014631_p507mcpsimp"><a name="zh-cn_topic_0000002574014631_p507mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p507mcpsimp"></a>客户端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.54%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014631_p509mcpsimp"><a name="zh-cn_topic_0000002574014631_p509mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p509mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002574014631_table511mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014631_row517mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014631_p519mcpsimp"><a name="zh-cn_topic_0000002574014631_p519mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p519mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014631_p521mcpsimp"><a name="zh-cn_topic_0000002574014631_p521mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p521mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014631_p523mcpsimp"><a name="zh-cn_topic_0000002574014631_p523mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p523mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014631_row524mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014631_p526mcpsimp"><a name="zh-cn_topic_0000002574014631_p526mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p526mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014631_p528mcpsimp"><a name="zh-cn_topic_0000002574014631_p528mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p528mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002574014631_p529mcpsimp"><a name="zh-cn_topic_0000002574014631_p529mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p529mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014631_p531mcpsimp"><a name="zh-cn_topic_0000002574014631_p531mcpsimp"></a><a name="zh-cn_topic_0000002574014631_p531mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

支持多次调用，注册多个客户端，目前可注册的客户端最大数量为8，当注册数量超过最大数量时，接口返回失败。

###### bs\_gattc\_unregister<a name="ZH-CN_TOPIC_0000002572623882"></a>

【描述】

BLE注销GATT客户端。

【接口定义】

```
errcode_t bs_gattc_unregister(uint8_t client_id);
```

【参数】

<a name="zh-cn_topic_0000002543414412_table1607mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414412_row1613mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414412_p1615mcpsimp"><a name="zh-cn_topic_0000002543414412_p1615mcpsimp"></a><a name="zh-cn_topic_0000002543414412_p1615mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.9%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414412_p1617mcpsimp"><a name="zh-cn_topic_0000002543414412_p1617mcpsimp"></a><a name="zh-cn_topic_0000002543414412_p1617mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.099999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414412_p1619mcpsimp"><a name="zh-cn_topic_0000002543414412_p1619mcpsimp"></a><a name="zh-cn_topic_0000002543414412_p1619mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414412_row1620mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414412_p1622mcpsimp"><a name="zh-cn_topic_0000002543414412_p1622mcpsimp"></a><a name="zh-cn_topic_0000002543414412_p1622mcpsimp"></a>client_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.9%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414412_p1624mcpsimp"><a name="zh-cn_topic_0000002543414412_p1624mcpsimp"></a><a name="zh-cn_topic_0000002543414412_p1624mcpsimp"></a>客户端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.099999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414412_p1626mcpsimp"><a name="zh-cn_topic_0000002543414412_p1626mcpsimp"></a><a name="zh-cn_topic_0000002543414412_p1626mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543414412_table1628mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414412_row1634mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414412_p1636mcpsimp"><a name="zh-cn_topic_0000002543414412_p1636mcpsimp"></a><a name="zh-cn_topic_0000002543414412_p1636mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414412_p1638mcpsimp"><a name="zh-cn_topic_0000002543414412_p1638mcpsimp"></a><a name="zh-cn_topic_0000002543414412_p1638mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414412_p1640mcpsimp"><a name="zh-cn_topic_0000002543414412_p1640mcpsimp"></a><a name="zh-cn_topic_0000002543414412_p1640mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414412_row1641mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414412_p1643mcpsimp"><a name="zh-cn_topic_0000002543414412_p1643mcpsimp"></a><a name="zh-cn_topic_0000002543414412_p1643mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414412_p1645mcpsimp"><a name="zh-cn_topic_0000002543414412_p1645mcpsimp"></a><a name="zh-cn_topic_0000002543414412_p1645mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543414412_p1646mcpsimp"><a name="zh-cn_topic_0000002543414412_p1646mcpsimp"></a><a name="zh-cn_topic_0000002543414412_p1646mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414412_p1648mcpsimp"><a name="zh-cn_topic_0000002543414412_p1648mcpsimp"></a><a name="zh-cn_topic_0000002543414412_p1648mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_gattc\_find\_all\_service<a name="ZH-CN_TOPIC_0000002572783584"></a>

【描述】

BLE GATT客户端发现全部服务结构，包括服务、特征、描述符。

【接口定义】

```
errcode_t bs_gattc_find_all_service(uint16_t dev_id, uint8_t client_id, bs_gattc_find_service_callback find_structure_cbk, bool is_remote);
```

【参数】

<a name="zh-cn_topic_0000002543254746_table549mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254746_row555mcpsimp"><th class="cellrowborder" valign="top" width="18.45%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254746_p557mcpsimp"><a name="zh-cn_topic_0000002543254746_p557mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p557mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="60.709999999999994%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254746_p559mcpsimp"><a name="zh-cn_topic_0000002543254746_p559mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p559mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.84%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254746_p561mcpsimp"><a name="zh-cn_topic_0000002543254746_p561mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p561mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254746_row562mcpsimp"><td class="cellrowborder" valign="top" width="18.45%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254746_p564mcpsimp"><a name="zh-cn_topic_0000002543254746_p564mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p564mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="60.709999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254746_p566mcpsimp"><a name="zh-cn_topic_0000002543254746_p566mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p566mcpsimp"></a>设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.84%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254746_p568mcpsimp"><a name="zh-cn_topic_0000002543254746_p568mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p568mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254746_row569mcpsimp"><td class="cellrowborder" valign="top" width="18.45%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254746_p571mcpsimp"><a name="zh-cn_topic_0000002543254746_p571mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p571mcpsimp"></a>client_id</p>
</td>
<td class="cellrowborder" valign="top" width="60.709999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254746_p573mcpsimp"><a name="zh-cn_topic_0000002543254746_p573mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p573mcpsimp"></a>客户端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.84%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254746_p575mcpsimp"><a name="zh-cn_topic_0000002543254746_p575mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p575mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254746_row576mcpsimp"><td class="cellrowborder" valign="top" width="18.45%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254746_p578mcpsimp"><a name="zh-cn_topic_0000002543254746_p578mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p578mcpsimp"></a>find_structure_cbk</p>
</td>
<td class="cellrowborder" valign="top" width="60.709999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254746_p580mcpsimp"><a name="zh-cn_topic_0000002543254746_p580mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p580mcpsimp"></a>查找服务完成回调，上报服务发现结果。</p>
</td>
<td class="cellrowborder" valign="top" width="20.84%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254746_p582mcpsimp"><a name="zh-cn_topic_0000002543254746_p582mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p582mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254746_row583mcpsimp"><td class="cellrowborder" valign="top" width="18.45%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254746_p585mcpsimp"><a name="zh-cn_topic_0000002543254746_p585mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p585mcpsimp"></a>is_remote</p>
</td>
<td class="cellrowborder" valign="top" width="60.709999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254746_p32867834520"><a name="zh-cn_topic_0000002543254746_p32867834520"></a><a name="zh-cn_topic_0000002543254746_p32867834520"></a>是否强制从远端设备获取服务。</p>
<p id="zh-cn_topic_0000002543254746_p8426629105114"><a name="zh-cn_topic_0000002543254746_p8426629105114"></a><a name="zh-cn_topic_0000002543254746_p8426629105114"></a>true：强制从远端设备获取服务；</p>
<p id="zh-cn_topic_0000002543254746_p587mcpsimp"><a name="zh-cn_topic_0000002543254746_p587mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p587mcpsimp"></a>false：如果本地有缓存则从缓存中获取所有服务。</p>
</td>
<td class="cellrowborder" valign="top" width="20.84%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254746_p589mcpsimp"><a name="zh-cn_topic_0000002543254746_p589mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p589mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543254746_table591mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254746_row597mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254746_p599mcpsimp"><a name="zh-cn_topic_0000002543254746_p599mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p599mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254746_p601mcpsimp"><a name="zh-cn_topic_0000002543254746_p601mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p601mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254746_p603mcpsimp"><a name="zh-cn_topic_0000002543254746_p603mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p603mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254746_row604mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254746_p606mcpsimp"><a name="zh-cn_topic_0000002543254746_p606mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p606mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254746_p608mcpsimp"><a name="zh-cn_topic_0000002543254746_p608mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p608mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543254746_p609mcpsimp"><a name="zh-cn_topic_0000002543254746_p609mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p609mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254746_p611mcpsimp"><a name="zh-cn_topic_0000002543254746_p611mcpsimp"></a><a name="zh-cn_topic_0000002543254746_p611mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_gattc\_find\_service\_by\_uuid<a name="ZH-CN_TOPIC_0000002603342975"></a>

【描述】

BLE GATT客户端发现特定UUID的服务的结构，包括其特征和描述符。

【接口定义】

```
errcode_t bs_gattc_find_service_by_uuid(uint16_t dev_id, uint8_t client_id, uint8_t uuid_num, bt_uuid_t *uuid_list, bs_gattc_find_service_callback find_structure_cbk);
```

【参数】

<a name="zh-cn_topic_0000002573894659_table708mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894659_row714mcpsimp"><th class="cellrowborder" valign="top" width="22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894659_p716mcpsimp"><a name="zh-cn_topic_0000002573894659_p716mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p716mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="58.46%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894659_p718mcpsimp"><a name="zh-cn_topic_0000002573894659_p718mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p718mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="19.54%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894659_p720mcpsimp"><a name="zh-cn_topic_0000002573894659_p720mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p720mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894659_row721mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894659_p723mcpsimp"><a name="zh-cn_topic_0000002573894659_p723mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p723mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="58.46%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894659_p725mcpsimp"><a name="zh-cn_topic_0000002573894659_p725mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p725mcpsimp"></a>设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="19.54%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894659_p727mcpsimp"><a name="zh-cn_topic_0000002573894659_p727mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p727mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002573894659_row728mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894659_p730mcpsimp"><a name="zh-cn_topic_0000002573894659_p730mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p730mcpsimp"></a>client_id</p>
</td>
<td class="cellrowborder" valign="top" width="58.46%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894659_p732mcpsimp"><a name="zh-cn_topic_0000002573894659_p732mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p732mcpsimp"></a>客户端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="19.54%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894659_p734mcpsimp"><a name="zh-cn_topic_0000002573894659_p734mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p734mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002573894659_row735mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894659_p737mcpsimp"><a name="zh-cn_topic_0000002573894659_p737mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p737mcpsimp"></a>uuid_num</p>
</td>
<td class="cellrowborder" valign="top" width="58.46%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894659_p739mcpsimp"><a name="zh-cn_topic_0000002573894659_p739mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p739mcpsimp"></a>待发现的UUID数量。</p>
</td>
<td class="cellrowborder" valign="top" width="19.54%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894659_p741mcpsimp"><a name="zh-cn_topic_0000002573894659_p741mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p741mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002573894659_row742mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894659_p744mcpsimp"><a name="zh-cn_topic_0000002573894659_p744mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p744mcpsimp"></a>uuid_list</p>
</td>
<td class="cellrowborder" valign="top" width="58.46%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894659_p746mcpsimp"><a name="zh-cn_topic_0000002573894659_p746mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p746mcpsimp"></a>待发现的UUID。</p>
</td>
<td class="cellrowborder" valign="top" width="19.54%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894659_p748mcpsimp"><a name="zh-cn_topic_0000002573894659_p748mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p748mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002573894659_row749mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894659_p751mcpsimp"><a name="zh-cn_topic_0000002573894659_p751mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p751mcpsimp"></a>find_structure_cbk</p>
</td>
<td class="cellrowborder" valign="top" width="58.46%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894659_p753mcpsimp"><a name="zh-cn_topic_0000002573894659_p753mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p753mcpsimp"></a>查找服务完成回调，上报服务发现结果。</p>
</td>
<td class="cellrowborder" valign="top" width="19.54%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894659_p755mcpsimp"><a name="zh-cn_topic_0000002573894659_p755mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p755mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002573894659_table757mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894659_row763mcpsimp"><th class="cellrowborder" valign="top" width="15.150000000000002%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894659_p765mcpsimp"><a name="zh-cn_topic_0000002573894659_p765mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p765mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="65.66000000000001%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894659_p767mcpsimp"><a name="zh-cn_topic_0000002573894659_p767mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p767mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="19.19%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894659_p769mcpsimp"><a name="zh-cn_topic_0000002573894659_p769mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p769mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894659_row770mcpsimp"><td class="cellrowborder" valign="top" width="15.150000000000002%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894659_p772mcpsimp"><a name="zh-cn_topic_0000002573894659_p772mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p772mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="65.66000000000001%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894659_p774mcpsimp"><a name="zh-cn_topic_0000002573894659_p774mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p774mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002573894659_p775mcpsimp"><a name="zh-cn_topic_0000002573894659_p775mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p775mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="19.19%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894659_p777mcpsimp"><a name="zh-cn_topic_0000002573894659_p777mcpsimp"></a><a name="zh-cn_topic_0000002573894659_p777mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_gattc\_register\_callbacks<a name="ZH-CN_TOPIC_0000002603342925"></a>

【描述】

BLE注册GATT客户端多服务回调函数。

【接口定义】

```
errcode_t bs_gattc_register_callbacks(uint16_t dev_id, uint8_t client_id, bt_uuid_t *service_uuid, bs_gattc_operation_cbk_t *func);
```

【参数】

<a name="zh-cn_topic_0000002574014633_table962mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014633_row968mcpsimp"><th class="cellrowborder" valign="top" width="22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014633_p970mcpsimp"><a name="zh-cn_topic_0000002574014633_p970mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p970mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="59.58%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014633_p972mcpsimp"><a name="zh-cn_topic_0000002574014633_p972mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p972mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="18.42%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014633_p974mcpsimp"><a name="zh-cn_topic_0000002574014633_p974mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p974mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014633_row975mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014633_p977mcpsimp"><a name="zh-cn_topic_0000002574014633_p977mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p977mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="59.58%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014633_p979mcpsimp"><a name="zh-cn_topic_0000002574014633_p979mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p979mcpsimp"></a>设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="18.42%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014633_p981mcpsimp"><a name="zh-cn_topic_0000002574014633_p981mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p981mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002574014633_row982mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014633_p984mcpsimp"><a name="zh-cn_topic_0000002574014633_p984mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p984mcpsimp"></a>client_id</p>
</td>
<td class="cellrowborder" valign="top" width="59.58%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014633_p986mcpsimp"><a name="zh-cn_topic_0000002574014633_p986mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p986mcpsimp"></a>客户端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="18.42%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014633_p988mcpsimp"><a name="zh-cn_topic_0000002574014633_p988mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p988mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002574014633_row989mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014633_p991mcpsimp"><a name="zh-cn_topic_0000002574014633_p991mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p991mcpsimp"></a>service_uuid</p>
</td>
<td class="cellrowborder" valign="top" width="59.58%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014633_p993mcpsimp"><a name="zh-cn_topic_0000002574014633_p993mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p993mcpsimp"></a>当前客户端订阅的服务的唯一标识。</p>
</td>
<td class="cellrowborder" valign="top" width="18.42%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014633_p995mcpsimp"><a name="zh-cn_topic_0000002574014633_p995mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p995mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002574014633_row996mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014633_p998mcpsimp"><a name="zh-cn_topic_0000002574014633_p998mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p998mcpsimp"></a>func</p>
</td>
<td class="cellrowborder" valign="top" width="59.58%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014633_p1000mcpsimp"><a name="zh-cn_topic_0000002574014633_p1000mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p1000mcpsimp"></a>客户端操作类回调函数。详见“<a href="#ZH-CN_TOPIC_0000002603342919">bs_gattc_operation_cbk_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="18.42%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014633_p1002mcpsimp"><a name="zh-cn_topic_0000002574014633_p1002mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p1002mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002574014633_table1004mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014633_row1010mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014633_p1012mcpsimp"><a name="zh-cn_topic_0000002574014633_p1012mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p1012mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="65.93%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014633_p1014mcpsimp"><a name="zh-cn_topic_0000002574014633_p1014mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p1014mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="18.92%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014633_p1016mcpsimp"><a name="zh-cn_topic_0000002574014633_p1016mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p1016mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014633_row1017mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014633_p1019mcpsimp"><a name="zh-cn_topic_0000002574014633_p1019mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p1019mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="65.93%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014633_p1021mcpsimp"><a name="zh-cn_topic_0000002574014633_p1021mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p1021mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002574014633_p1022mcpsimp"><a name="zh-cn_topic_0000002574014633_p1022mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p1022mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="18.92%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014633_p1024mcpsimp"><a name="zh-cn_topic_0000002574014633_p1024mcpsimp"></a><a name="zh-cn_topic_0000002574014633_p1024mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

当前接口需要在服务发现完成之后调用，如果需要订阅多个服务，则需要多次调用该接口，传入不同UUID。

###### bs\_gattc\_unregister\_callbacks<a name="ZH-CN_TOPIC_0000002572783532"></a>

【描述】

BLE去注册GATT客户端多服务回调函数。

【接口定义】

```
errcode_t bs_gattc_unregister_callbacks(uint16_t dev_id, uint8_t client_id, bt_uuid_t *service_uuid);
```

【参数】

<a name="zh-cn_topic_0000002543414414_table220mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414414_row226mcpsimp"><th class="cellrowborder" valign="top" width="22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414414_p228mcpsimp"><a name="zh-cn_topic_0000002543414414_p228mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p228mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="58.650000000000006%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414414_p230mcpsimp"><a name="zh-cn_topic_0000002543414414_p230mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p230mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="19.35%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414414_p232mcpsimp"><a name="zh-cn_topic_0000002543414414_p232mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p232mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414414_row233mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414414_p235mcpsimp"><a name="zh-cn_topic_0000002543414414_p235mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p235mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="58.650000000000006%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414414_p237mcpsimp"><a name="zh-cn_topic_0000002543414414_p237mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p237mcpsimp"></a>设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="19.35%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414414_p239mcpsimp"><a name="zh-cn_topic_0000002543414414_p239mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p239mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543414414_row240mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414414_p242mcpsimp"><a name="zh-cn_topic_0000002543414414_p242mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p242mcpsimp"></a>client_id</p>
</td>
<td class="cellrowborder" valign="top" width="58.650000000000006%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414414_p244mcpsimp"><a name="zh-cn_topic_0000002543414414_p244mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p244mcpsimp"></a>客户端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="19.35%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414414_p246mcpsimp"><a name="zh-cn_topic_0000002543414414_p246mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p246mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543414414_row247mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414414_p249mcpsimp"><a name="zh-cn_topic_0000002543414414_p249mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p249mcpsimp"></a>service_uuid</p>
</td>
<td class="cellrowborder" valign="top" width="58.650000000000006%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414414_p251mcpsimp"><a name="zh-cn_topic_0000002543414414_p251mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p251mcpsimp"></a>当前客户端订阅的服务的唯一标识。</p>
</td>
<td class="cellrowborder" valign="top" width="19.35%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414414_p253mcpsimp"><a name="zh-cn_topic_0000002543414414_p253mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p253mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543414414_table255mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414414_row261mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414414_p263mcpsimp"><a name="zh-cn_topic_0000002543414414_p263mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p263mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="64.79%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414414_p265mcpsimp"><a name="zh-cn_topic_0000002543414414_p265mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p265mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.06%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414414_p267mcpsimp"><a name="zh-cn_topic_0000002543414414_p267mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p267mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414414_row268mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414414_p270mcpsimp"><a name="zh-cn_topic_0000002543414414_p270mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p270mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="64.79%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414414_p272mcpsimp"><a name="zh-cn_topic_0000002543414414_p272mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p272mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543414414_p273mcpsimp"><a name="zh-cn_topic_0000002543414414_p273mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p273mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="20.06%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414414_p275mcpsimp"><a name="zh-cn_topic_0000002543414414_p275mcpsimp"></a><a name="zh-cn_topic_0000002543414414_p275mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_gattc\_read<a name="ZH-CN_TOPIC_0000002572623936"></a>

【描述】

BLE GATT客户端发起按照句柄读取请求。

【接口定义】

```
errcode_t bs_gattc_read(uint16_t dev_id, uint8_t client_id, uint16_t handle);
```

【参数】

<a name="zh-cn_topic_0000002543254748_table1148mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254748_row1154mcpsimp"><th class="cellrowborder" valign="top" width="22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254748_p1156mcpsimp"><a name="zh-cn_topic_0000002543254748_p1156mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1156mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="59.199999999999996%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254748_p1158mcpsimp"><a name="zh-cn_topic_0000002543254748_p1158mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1158mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="18.8%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254748_p1160mcpsimp"><a name="zh-cn_topic_0000002543254748_p1160mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1160mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254748_row1161mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254748_p1163mcpsimp"><a name="zh-cn_topic_0000002543254748_p1163mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1163mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="59.199999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254748_p1165mcpsimp"><a name="zh-cn_topic_0000002543254748_p1165mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1165mcpsimp"></a>设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="18.8%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254748_p1167mcpsimp"><a name="zh-cn_topic_0000002543254748_p1167mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1167mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254748_row1168mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254748_p1170mcpsimp"><a name="zh-cn_topic_0000002543254748_p1170mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1170mcpsimp"></a>client_id</p>
</td>
<td class="cellrowborder" valign="top" width="59.199999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254748_p1172mcpsimp"><a name="zh-cn_topic_0000002543254748_p1172mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1172mcpsimp"></a>客户端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="18.8%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254748_p1174mcpsimp"><a name="zh-cn_topic_0000002543254748_p1174mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1174mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254748_row1175mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254748_p1177mcpsimp"><a name="zh-cn_topic_0000002543254748_p1177mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1177mcpsimp"></a>handle</p>
</td>
<td class="cellrowborder" valign="top" width="59.199999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254748_p1179mcpsimp"><a name="zh-cn_topic_0000002543254748_p1179mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1179mcpsimp"></a>需要读取的属性/描述符句柄，读取结果在客户端注册的操作类回调中返回。</p>
</td>
<td class="cellrowborder" valign="top" width="18.8%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254748_p1181mcpsimp"><a name="zh-cn_topic_0000002543254748_p1181mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1181mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543254748_table1183mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254748_row1189mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254748_p1191mcpsimp"><a name="zh-cn_topic_0000002543254748_p1191mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1191mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="65.9%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254748_p1193mcpsimp"><a name="zh-cn_topic_0000002543254748_p1193mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1193mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="18.95%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254748_p1195mcpsimp"><a name="zh-cn_topic_0000002543254748_p1195mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1195mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254748_row1196mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254748_p1198mcpsimp"><a name="zh-cn_topic_0000002543254748_p1198mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1198mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="65.9%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254748_p1200mcpsimp"><a name="zh-cn_topic_0000002543254748_p1200mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1200mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543254748_p1201mcpsimp"><a name="zh-cn_topic_0000002543254748_p1201mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1201mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="18.95%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254748_p1203mcpsimp"><a name="zh-cn_topic_0000002543254748_p1203mcpsimp"></a><a name="zh-cn_topic_0000002543254748_p1203mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_gattc\_write<a name="ZH-CN_TOPIC_0000002572783518"></a>

【描述】

BLE GATT客户端发起写请求。

【接口定义】

```
errcode_t bs_gattc_write(uint16_t dev_id, uint8_t client_id, bs_gattc_write_value_t *param);
```

【参数】

<a name="zh-cn_topic_0000002573894661_table1672mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894661_row1678mcpsimp"><th class="cellrowborder" valign="top" width="18.45%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894661_p1680mcpsimp"><a name="zh-cn_topic_0000002573894661_p1680mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1680mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.74999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894661_p1682mcpsimp"><a name="zh-cn_topic_0000002573894661_p1682mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1682mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="18.8%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894661_p1684mcpsimp"><a name="zh-cn_topic_0000002573894661_p1684mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1684mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894661_row1685mcpsimp"><td class="cellrowborder" valign="top" width="18.45%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894661_p1687mcpsimp"><a name="zh-cn_topic_0000002573894661_p1687mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1687mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="62.74999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894661_p1689mcpsimp"><a name="zh-cn_topic_0000002573894661_p1689mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1689mcpsimp"></a>设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="18.8%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894661_p1691mcpsimp"><a name="zh-cn_topic_0000002573894661_p1691mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1691mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002573894661_row1692mcpsimp"><td class="cellrowborder" valign="top" width="18.45%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894661_p1694mcpsimp"><a name="zh-cn_topic_0000002573894661_p1694mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1694mcpsimp"></a>client_id</p>
</td>
<td class="cellrowborder" valign="top" width="62.74999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894661_p1696mcpsimp"><a name="zh-cn_topic_0000002573894661_p1696mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1696mcpsimp"></a>客户端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="18.8%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894661_p1698mcpsimp"><a name="zh-cn_topic_0000002573894661_p1698mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1698mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002573894661_row1699mcpsimp"><td class="cellrowborder" valign="top" width="18.45%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894661_p1701mcpsimp"><a name="zh-cn_topic_0000002573894661_p1701mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1701mcpsimp"></a>param</p>
</td>
<td class="cellrowborder" valign="top" width="62.74999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894661_p970715117586"><a name="zh-cn_topic_0000002573894661_p970715117586"></a><a name="zh-cn_topic_0000002573894661_p970715117586"></a>写信息参数，详见“<a href="#ZH-CN_TOPIC_0000002603223079">bs_gattc_write_value_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="18.8%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894661_p1706mcpsimp"><a name="zh-cn_topic_0000002573894661_p1706mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1706mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002573894661_table1708mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894661_row1714mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894661_p1716mcpsimp"><a name="zh-cn_topic_0000002573894661_p1716mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1716mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="65.9%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894661_p1718mcpsimp"><a name="zh-cn_topic_0000002573894661_p1718mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1718mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="18.95%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894661_p1720mcpsimp"><a name="zh-cn_topic_0000002573894661_p1720mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1720mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894661_row1721mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894661_p1723mcpsimp"><a name="zh-cn_topic_0000002573894661_p1723mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1723mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="65.9%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894661_p1725mcpsimp"><a name="zh-cn_topic_0000002573894661_p1725mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1725mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002573894661_p1726mcpsimp"><a name="zh-cn_topic_0000002573894661_p1726mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1726mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="18.95%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894661_p1728mcpsimp"><a name="zh-cn_topic_0000002573894661_p1728mcpsimp"></a><a name="zh-cn_topic_0000002573894661_p1728mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_gattc\_exchange\_mtu\_req<a name="ZH-CN_TOPIC_0000002603222979"></a>

【描述】

BLE GATT客户端发送交换MTU请求。

【接口定义】

```
errcode_t bs_gattc_exchange_mtu_req(uint16_t dev_id, uint8_t client_id, uint16_t mtu_size);
```

【参数】

<a name="zh-cn_topic_0000002574014635_table786mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014635_row792mcpsimp"><th class="cellrowborder" valign="top" width="22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014635_p794mcpsimp"><a name="zh-cn_topic_0000002574014635_p794mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p794mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="58.96%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014635_p796mcpsimp"><a name="zh-cn_topic_0000002574014635_p796mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p796mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="19.040000000000003%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014635_p798mcpsimp"><a name="zh-cn_topic_0000002574014635_p798mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p798mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014635_row799mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014635_p801mcpsimp"><a name="zh-cn_topic_0000002574014635_p801mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p801mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="58.96%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014635_p803mcpsimp"><a name="zh-cn_topic_0000002574014635_p803mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p803mcpsimp"></a>设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="19.040000000000003%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014635_p805mcpsimp"><a name="zh-cn_topic_0000002574014635_p805mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p805mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002574014635_row806mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014635_p808mcpsimp"><a name="zh-cn_topic_0000002574014635_p808mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p808mcpsimp"></a>client_id</p>
</td>
<td class="cellrowborder" valign="top" width="58.96%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014635_p810mcpsimp"><a name="zh-cn_topic_0000002574014635_p810mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p810mcpsimp"></a>客户端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="19.040000000000003%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014635_p812mcpsimp"><a name="zh-cn_topic_0000002574014635_p812mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p812mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002574014635_row813mcpsimp"><td class="cellrowborder" valign="top" width="22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014635_p815mcpsimp"><a name="zh-cn_topic_0000002574014635_p815mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p815mcpsimp"></a>mtu_size</p>
</td>
<td class="cellrowborder" valign="top" width="58.96%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014635_p817mcpsimp"><a name="zh-cn_topic_0000002574014635_p817mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p817mcpsimp"></a>客户端接收MTU，MTU改变结果在回调中返回，MTU值范围23Byte～512Byte。</p>
</td>
<td class="cellrowborder" valign="top" width="19.040000000000003%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014635_p819mcpsimp"><a name="zh-cn_topic_0000002574014635_p819mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p819mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002574014635_table821mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014635_row827mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014635_p829mcpsimp"><a name="zh-cn_topic_0000002574014635_p829mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p829mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="65.9%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014635_p831mcpsimp"><a name="zh-cn_topic_0000002574014635_p831mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p831mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="18.95%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014635_p833mcpsimp"><a name="zh-cn_topic_0000002574014635_p833mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p833mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014635_row834mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014635_p836mcpsimp"><a name="zh-cn_topic_0000002574014635_p836mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p836mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="65.9%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014635_p838mcpsimp"><a name="zh-cn_topic_0000002574014635_p838mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p838mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002574014635_p839mcpsimp"><a name="zh-cn_topic_0000002574014635_p839mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p839mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="18.95%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014635_p841mcpsimp"><a name="zh-cn_topic_0000002574014635_p841mcpsimp"></a><a name="zh-cn_topic_0000002574014635_p841mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

客户端调用该接口之前，服务端需要先通过bs\_gatts\_set\_mtu\_size接口设置服务端接收MTU大小，协商后的MTU值为客户端请求交换的客户端接收MTU值与服务端设置的服务端接收MTU值两值中的较小值。

#### GATT服务端管理<a name="ZH-CN_TOPIC_0000002603342977"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002603223011)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002603342999)**  

##### 数据结构<a name="ZH-CN_TOPIC_0000002603223011"></a>

-   **[bs\_gatt\_service\_template\_t](#ZH-CN_TOPIC_0000002603223051)**  

-   **[bs\_gatts\_send\_rsp\_t](#ZH-CN_TOPIC_0000002603223039)**  

-   **[bs\_gatts\_ntf\_ind\_t](#ZH-CN_TOPIC_0000002603223031)**  

###### bs\_gatt\_service\_template\_t<a name="ZH-CN_TOPIC_0000002603223051"></a>

【结构体说明】

BLE GATT服务端配置结构。

【结构体定义】

```
typedef struct {
    bt_uuid_t uuid;                                        /*!< @if Eng uuid of this service.
                                                                @else   该服务uuid。 @endif */
    uint8_t is_primary;                                    /*!< @if Eng is primary service tag
                                                                @else   是否是主服务标记 @endif */
    uint8_t attribute_count;                               /*!< @if Eng size of attribute_array.
                                                                @else   attribute_array中存储的属性（包括特征和描述符）个数 @endif */
    bs_gatt_service_callbacks_t cbks;                      /*!< @if Eng server callback function.
                                                                @else   服务端回调函数 @endif */
    bs_gatt_service_extend_param_t extend_param;           /*!< @if Eng server structure extend param.
                                                                @else   服务端扩展参数 @endif */
    bs_gatts_attribute_info attribute_array[0];            /*!< @if Eng point array of attribute config.
                                                                @else 服务中的属性（包括特征和描述符）数组。@endif */
} bs_gatt_service_template_t;
```

【注意事项】

无。

###### bs\_gatts\_send\_rsp\_t<a name="ZH-CN_TOPIC_0000002603223039"></a>

【结构体说明】

BLE GATT服务端发送响应信息结构体。

【结构体定义】

```
typedef struct {
    uint16_t request_id; /*!< @if Eng Request ID.
                              @else   请求 ID。 @endif */
    uint8_t status;      /*!< @if Eng Status code of read/write, { @ref bs_gatt_status_t }.
                              @else   读写结果的状态， { @ref bs_gatt_status_t }。 @endif */
    uint16_t value_len;  /*!< @if Eng Length of reponse data.
                              @else   响应的数据长度。 @endif */
    uint8_t *value;      /*!< @if Eng Reponse data.
                              @else   响应的数据。 @endif */
} bs_gatts_send_rsp_t;
```

【注意事项】

无。

###### bs\_gatts\_ntf\_ind\_t<a name="ZH-CN_TOPIC_0000002603223031"></a>

【结构体说明】

BLE GATT服务端发送通知、指示信息结构体。

【结构体定义】

```
typedef struct {
    uint8_t instance_id;             /*!< @if Eng Service Instance id.
                                           @else  服务实例ID。 @endif */
    uint8_t attribute_id;            /*!< @if Eng attribute ID.
                                          @else   属性ID。 @endif */
    uint16_t value_len;              /*!< @if Eng Length of notification/indication data.
                                           @else   通知/指示数据长度。 @endif */
    uint8_t *value;                  /*!< @if Eng Notification/indication data.
                                           @else   发送的通知/指示数据。 @endif */
} bs_gatts_ntf_ind_t;
```

【注意事项】

无。

##### 函数接口<a name="ZH-CN_TOPIC_0000002603342999"></a>

-   **[bs\_gatts\_register](#ZH-CN_TOPIC_0000002572623900)**  

-   **[bs\_gatts\_unregister](#ZH-CN_TOPIC_0000002603223061)**  

-   **[bs\_gatts\_service\_instance\_init](#ZH-CN_TOPIC_0000002572783484)**  

-   **[bs\_gatts\_send\_rsp](#ZH-CN_TOPIC_0000002572783534)**  

-   **[bs\_gatts\_notify\_indicate](#ZH-CN_TOPIC_0000002572783552)**  

-   **[bs\_gatts\_set\_mtu\_size](#ZH-CN_TOPIC_0000002572623914)**  

###### bs\_gatts\_register<a name="ZH-CN_TOPIC_0000002572623900"></a>

【描述】

注册GATT服务端。

【接口定义】

```
errcode_t bs_gatts_register(bt_uuid_t *app_uuid, uint8_t *server_id);
```

【参数】

<a name="zh-cn_topic_0000002543254750_table1091mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254750_row1097mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254750_p1099mcpsimp"><a name="zh-cn_topic_0000002543254750_p1099mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1099mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="63.480000000000004%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254750_p1101mcpsimp"><a name="zh-cn_topic_0000002543254750_p1101mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1101mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="19.52%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254750_p1103mcpsimp"><a name="zh-cn_topic_0000002543254750_p1103mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1103mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254750_row1104mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254750_p1106mcpsimp"><a name="zh-cn_topic_0000002543254750_p1106mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1106mcpsimp"></a>app_uuid</p>
</td>
<td class="cellrowborder" valign="top" width="63.480000000000004%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254750_p1108mcpsimp"><a name="zh-cn_topic_0000002543254750_p1108mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1108mcpsimp"></a>上层应用UUID。</p>
</td>
<td class="cellrowborder" valign="top" width="19.52%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254750_p1110mcpsimp"><a name="zh-cn_topic_0000002543254750_p1110mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1110mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254750_row1111mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254750_p1113mcpsimp"><a name="zh-cn_topic_0000002543254750_p1113mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1113mcpsimp"></a>server_id</p>
</td>
<td class="cellrowborder" valign="top" width="63.480000000000004%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254750_p1115mcpsimp"><a name="zh-cn_topic_0000002543254750_p1115mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1115mcpsimp"></a>服务端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="19.52%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254750_p1117mcpsimp"><a name="zh-cn_topic_0000002543254750_p1117mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1117mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543254750_table1119mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254750_row1125mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254750_p1127mcpsimp"><a name="zh-cn_topic_0000002543254750_p1127mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1127mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="65.34%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254750_p1129mcpsimp"><a name="zh-cn_topic_0000002543254750_p1129mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1129mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="19.509999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254750_p1131mcpsimp"><a name="zh-cn_topic_0000002543254750_p1131mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1131mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254750_row1132mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254750_p1134mcpsimp"><a name="zh-cn_topic_0000002543254750_p1134mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1134mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="65.34%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254750_p1136mcpsimp"><a name="zh-cn_topic_0000002543254750_p1136mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1136mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543254750_p1137mcpsimp"><a name="zh-cn_topic_0000002543254750_p1137mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1137mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="19.509999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254750_p1139mcpsimp"><a name="zh-cn_topic_0000002543254750_p1139mcpsimp"></a><a name="zh-cn_topic_0000002543254750_p1139mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

支持多次调用，注册多个服务端，目前可注册的服务端最大数量为5。

###### bs\_gatts\_unregister<a name="ZH-CN_TOPIC_0000002603223061"></a>

【描述】

注销GATT服务端。

【接口定义】

```
errcode_t bs_gatts_unregister(uint8_t server_id);
```

【参数】

<a name="zh-cn_topic_0000002573894663_table657mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894663_row663mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894663_p665mcpsimp"><a name="zh-cn_topic_0000002573894663_p665mcpsimp"></a><a name="zh-cn_topic_0000002573894663_p665mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="64.41%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894663_p667mcpsimp"><a name="zh-cn_topic_0000002573894663_p667mcpsimp"></a><a name="zh-cn_topic_0000002573894663_p667mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="18.59%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894663_p669mcpsimp"><a name="zh-cn_topic_0000002573894663_p669mcpsimp"></a><a name="zh-cn_topic_0000002573894663_p669mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894663_row670mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894663_p672mcpsimp"><a name="zh-cn_topic_0000002573894663_p672mcpsimp"></a><a name="zh-cn_topic_0000002573894663_p672mcpsimp"></a>server_id</p>
</td>
<td class="cellrowborder" valign="top" width="64.41%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894663_p674mcpsimp"><a name="zh-cn_topic_0000002573894663_p674mcpsimp"></a><a name="zh-cn_topic_0000002573894663_p674mcpsimp"></a>服务端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="18.59%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894663_p676mcpsimp"><a name="zh-cn_topic_0000002573894663_p676mcpsimp"></a><a name="zh-cn_topic_0000002573894663_p676mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002573894663_table678mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894663_row684mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894663_p686mcpsimp"><a name="zh-cn_topic_0000002573894663_p686mcpsimp"></a><a name="zh-cn_topic_0000002573894663_p686mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="66.18%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894663_p688mcpsimp"><a name="zh-cn_topic_0000002573894663_p688mcpsimp"></a><a name="zh-cn_topic_0000002573894663_p688mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="18.67%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894663_p690mcpsimp"><a name="zh-cn_topic_0000002573894663_p690mcpsimp"></a><a name="zh-cn_topic_0000002573894663_p690mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894663_row691mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894663_p693mcpsimp"><a name="zh-cn_topic_0000002573894663_p693mcpsimp"></a><a name="zh-cn_topic_0000002573894663_p693mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="66.18%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894663_p695mcpsimp"><a name="zh-cn_topic_0000002573894663_p695mcpsimp"></a><a name="zh-cn_topic_0000002573894663_p695mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002573894663_p696mcpsimp"><a name="zh-cn_topic_0000002573894663_p696mcpsimp"></a><a name="zh-cn_topic_0000002573894663_p696mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="18.67%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894663_p698mcpsimp"><a name="zh-cn_topic_0000002573894663_p698mcpsimp"></a><a name="zh-cn_topic_0000002573894663_p698mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_gatts\_service\_instance\_init<a name="ZH-CN_TOPIC_0000002572783484"></a>

【描述】

添加一个GATT服务实例。

【接口定义】

```
errcode_t bs_gatts_service_instance_init(uint8_t server_id, bs_gatt_service_template_t *service, uint8_t *inst_id);
```

【参数】

<a name="zh-cn_topic_0000002574014637_table284mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014637_row290mcpsimp"><th class="cellrowborder" valign="top" width="18.28%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014637_p292mcpsimp"><a name="zh-cn_topic_0000002574014637_p292mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p292mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="64.15%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014637_p294mcpsimp"><a name="zh-cn_topic_0000002574014637_p294mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p294mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="17.570000000000004%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014637_p296mcpsimp"><a name="zh-cn_topic_0000002574014637_p296mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p296mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014637_row297mcpsimp"><td class="cellrowborder" valign="top" width="18.28%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014637_p299mcpsimp"><a name="zh-cn_topic_0000002574014637_p299mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p299mcpsimp"></a>server_id</p>
</td>
<td class="cellrowborder" valign="top" width="64.15%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014637_p301mcpsimp"><a name="zh-cn_topic_0000002574014637_p301mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p301mcpsimp"></a>服务端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="17.570000000000004%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014637_p303mcpsimp"><a name="zh-cn_topic_0000002574014637_p303mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p303mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002574014637_row304mcpsimp"><td class="cellrowborder" valign="top" width="18.28%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014637_p306mcpsimp"><a name="zh-cn_topic_0000002574014637_p306mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p306mcpsimp"></a>service</p>
</td>
<td class="cellrowborder" valign="top" width="64.15%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014637_p308mcpsimp"><a name="zh-cn_topic_0000002574014637_p308mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p308mcpsimp"></a>服务配置模板。详见“<a href="#ZH-CN_TOPIC_0000002603223051">bs_gatt_service_template_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="17.570000000000004%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014637_p310mcpsimp"><a name="zh-cn_topic_0000002574014637_p310mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p310mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002574014637_row311mcpsimp"><td class="cellrowborder" valign="top" width="18.28%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014637_p313mcpsimp"><a name="zh-cn_topic_0000002574014637_p313mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p313mcpsimp"></a>inst_id</p>
</td>
<td class="cellrowborder" valign="top" width="64.15%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014637_p315mcpsimp"><a name="zh-cn_topic_0000002574014637_p315mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p315mcpsimp"></a>返回的服务实例ID。</p>
</td>
<td class="cellrowborder" valign="top" width="17.570000000000004%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014637_p317mcpsimp"><a name="zh-cn_topic_0000002574014637_p317mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p317mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002574014637_table342mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002574014637_row348mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002574014637_p350mcpsimp"><a name="zh-cn_topic_0000002574014637_p350mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p350mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002574014637_p352mcpsimp"><a name="zh-cn_topic_0000002574014637_p352mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p352mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002574014637_p354mcpsimp"><a name="zh-cn_topic_0000002574014637_p354mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p354mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002574014637_row355mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002574014637_p357mcpsimp"><a name="zh-cn_topic_0000002574014637_p357mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p357mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002574014637_p359mcpsimp"><a name="zh-cn_topic_0000002574014637_p359mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p359mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002574014637_p360mcpsimp"><a name="zh-cn_topic_0000002574014637_p360mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p360mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h；</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002574014637_p362mcpsimp"><a name="zh-cn_topic_0000002574014637_p362mcpsimp"></a><a name="zh-cn_topic_0000002574014637_p362mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

定义服务配置模板时，特征和描述符的配置数组大小需要与character\_desc\_count一致。

###### bs\_gatts\_send\_rsp<a name="ZH-CN_TOPIC_0000002572783534"></a>

【描述】

当收到需要用户回复响应的请求时发送响应。

【接口定义】

```
errcode_t bs_gatts_send_rsp(uint16_t dev_id, uint8_t server_id, bs_gatts_send_rsp_t *param);
```

【参数】

<a name="zh-cn_topic_0000002543414418_table156mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414418_row162mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414418_p164mcpsimp"><a name="zh-cn_topic_0000002543414418_p164mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p164mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.92%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414418_p166mcpsimp"><a name="zh-cn_topic_0000002543414418_p166mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p166mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.080000000000002%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414418_p168mcpsimp"><a name="zh-cn_topic_0000002543414418_p168mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p168mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414418_row169mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414418_p171mcpsimp"><a name="zh-cn_topic_0000002543414418_p171mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p171mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="62.92%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414418_p173mcpsimp"><a name="zh-cn_topic_0000002543414418_p173mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p173mcpsimp"></a>设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.080000000000002%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414418_p175mcpsimp"><a name="zh-cn_topic_0000002543414418_p175mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p175mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543414418_row176mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414418_p178mcpsimp"><a name="zh-cn_topic_0000002543414418_p178mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p178mcpsimp"></a>server_id</p>
</td>
<td class="cellrowborder" valign="top" width="62.92%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414418_p180mcpsimp"><a name="zh-cn_topic_0000002543414418_p180mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p180mcpsimp"></a>服务端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.080000000000002%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414418_p182mcpsimp"><a name="zh-cn_topic_0000002543414418_p182mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p182mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543414418_row183mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414418_p185mcpsimp"><a name="zh-cn_topic_0000002543414418_p185mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p185mcpsimp"></a>param</p>
</td>
<td class="cellrowborder" valign="top" width="62.92%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414418_p187mcpsimp"><a name="zh-cn_topic_0000002543414418_p187mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p187mcpsimp"></a>响应参数。详见“<a href="#ZH-CN_TOPIC_0000002603223039">bs_gatts_send_rsp_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.080000000000002%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414418_p189mcpsimp"><a name="zh-cn_topic_0000002543414418_p189mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p189mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543414418_table191mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543414418_row197mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543414418_p199mcpsimp"><a name="zh-cn_topic_0000002543414418_p199mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p199mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="64.42%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543414418_p201mcpsimp"><a name="zh-cn_topic_0000002543414418_p201mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p201mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.43%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543414418_p203mcpsimp"><a name="zh-cn_topic_0000002543414418_p203mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p203mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543414418_row204mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543414418_p206mcpsimp"><a name="zh-cn_topic_0000002543414418_p206mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p206mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="64.42%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543414418_p208mcpsimp"><a name="zh-cn_topic_0000002543414418_p208mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p208mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543414418_p209mcpsimp"><a name="zh-cn_topic_0000002543414418_p209mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p209mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="20.43%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543414418_p211mcpsimp"><a name="zh-cn_topic_0000002543414418_p211mcpsimp"></a><a name="zh-cn_topic_0000002543414418_p211mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_gatts\_notify\_indicate<a name="ZH-CN_TOPIC_0000002572783552"></a>

【描述】

向对端发送通知或指示。

【接口定义】

```
errcode_t bs_gatts_notify_indicate(uint16_t dev_id, uint8_t server_id, bs_gatts_ntf_ind_t *param);
```

【参数】

<a name="zh-cn_topic_0000002543254752_table1406mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254752_row1412mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254752_p1414mcpsimp"><a name="zh-cn_topic_0000002543254752_p1414mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1414mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="63.849999999999994%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254752_p1416mcpsimp"><a name="zh-cn_topic_0000002543254752_p1416mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1416mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="19.15%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254752_p1418mcpsimp"><a name="zh-cn_topic_0000002543254752_p1418mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1418mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254752_row1419mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254752_p1421mcpsimp"><a name="zh-cn_topic_0000002543254752_p1421mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1421mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="63.849999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254752_p1423mcpsimp"><a name="zh-cn_topic_0000002543254752_p1423mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1423mcpsimp"></a>设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="19.15%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254752_p1425mcpsimp"><a name="zh-cn_topic_0000002543254752_p1425mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1425mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254752_row1426mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254752_p1428mcpsimp"><a name="zh-cn_topic_0000002543254752_p1428mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1428mcpsimp"></a>server_id</p>
</td>
<td class="cellrowborder" valign="top" width="63.849999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254752_p1430mcpsimp"><a name="zh-cn_topic_0000002543254752_p1430mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1430mcpsimp"></a>服务端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="19.15%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254752_p1432mcpsimp"><a name="zh-cn_topic_0000002543254752_p1432mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1432mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002543254752_row1433mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254752_p1435mcpsimp"><a name="zh-cn_topic_0000002543254752_p1435mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1435mcpsimp"></a>param</p>
</td>
<td class="cellrowborder" valign="top" width="63.849999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254752_p1437mcpsimp"><a name="zh-cn_topic_0000002543254752_p1437mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1437mcpsimp"></a>通知或指示参数。详见“<a href="#ZH-CN_TOPIC_0000002603223031">bs_gatts_ntf_ind_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="19.15%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254752_p1439mcpsimp"><a name="zh-cn_topic_0000002543254752_p1439mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1439mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002543254752_table1441mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002543254752_row1447mcpsimp"><th class="cellrowborder" valign="top" width="15.150000000000002%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002543254752_p1449mcpsimp"><a name="zh-cn_topic_0000002543254752_p1449mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1449mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="65.56%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002543254752_p1451mcpsimp"><a name="zh-cn_topic_0000002543254752_p1451mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1451mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="19.290000000000003%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002543254752_p1453mcpsimp"><a name="zh-cn_topic_0000002543254752_p1453mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1453mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002543254752_row1454mcpsimp"><td class="cellrowborder" valign="top" width="15.150000000000002%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002543254752_p1456mcpsimp"><a name="zh-cn_topic_0000002543254752_p1456mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1456mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="65.56%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002543254752_p1458mcpsimp"><a name="zh-cn_topic_0000002543254752_p1458mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1458mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002543254752_p1459mcpsimp"><a name="zh-cn_topic_0000002543254752_p1459mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1459mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="19.290000000000003%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002543254752_p1461mcpsimp"><a name="zh-cn_topic_0000002543254752_p1461mcpsimp"></a><a name="zh-cn_topic_0000002543254752_p1461mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

###### bs\_gatts\_set\_mtu\_size<a name="ZH-CN_TOPIC_0000002572623914"></a>

【描述】

在连接之前设置服务端接收MTU。

【接口定义】

```
errcode_t bs_gatts_set_mtu_size(uint8_t server_id, uint16_t mtu_size);
```

【参数】

<a name="zh-cn_topic_0000002573894665_table371mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894665_row377mcpsimp"><th class="cellrowborder" valign="top" width="22.000000000000004%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894665_p379mcpsimp"><a name="zh-cn_topic_0000002573894665_p379mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p379mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="59.02%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894665_p381mcpsimp"><a name="zh-cn_topic_0000002573894665_p381mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p381mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="18.980000000000004%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894665_p383mcpsimp"><a name="zh-cn_topic_0000002573894665_p383mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p383mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894665_row384mcpsimp"><td class="cellrowborder" valign="top" width="22.000000000000004%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894665_p386mcpsimp"><a name="zh-cn_topic_0000002573894665_p386mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p386mcpsimp"></a>server_id</p>
</td>
<td class="cellrowborder" valign="top" width="59.02%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894665_p388mcpsimp"><a name="zh-cn_topic_0000002573894665_p388mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p388mcpsimp"></a>服务端ID。</p>
</td>
<td class="cellrowborder" valign="top" width="18.980000000000004%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894665_p390mcpsimp"><a name="zh-cn_topic_0000002573894665_p390mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p390mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002573894665_row391mcpsimp"><td class="cellrowborder" valign="top" width="22.000000000000004%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894665_p393mcpsimp"><a name="zh-cn_topic_0000002573894665_p393mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p393mcpsimp"></a>mtu_size</p>
</td>
<td class="cellrowborder" valign="top" width="59.02%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894665_p395mcpsimp"><a name="zh-cn_topic_0000002573894665_p395mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p395mcpsimp"></a>服务端接收MTU，MTU值范围23Byte～512Byte。</p>
</td>
<td class="cellrowborder" valign="top" width="18.980000000000004%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894665_p397mcpsimp"><a name="zh-cn_topic_0000002573894665_p397mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p397mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002573894665_table399mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002573894665_row405mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002573894665_p407mcpsimp"><a name="zh-cn_topic_0000002573894665_p407mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p407mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="65.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002573894665_p409mcpsimp"><a name="zh-cn_topic_0000002573894665_p409mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p409mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="19.13%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002573894665_p411mcpsimp"><a name="zh-cn_topic_0000002573894665_p411mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p411mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002573894665_row412mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002573894665_p414mcpsimp"><a name="zh-cn_topic_0000002573894665_p414mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p414mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="65.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002573894665_p416mcpsimp"><a name="zh-cn_topic_0000002573894665_p416mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p416mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002573894665_p417mcpsimp"><a name="zh-cn_topic_0000002573894665_p417mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p417mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="19.13%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002573894665_p419mcpsimp"><a name="zh-cn_topic_0000002573894665_p419mcpsimp"></a><a name="zh-cn_topic_0000002573894665_p419mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

## 开发示例<a name="ZH-CN_TOPIC_0000002572623904"></a>

>![](public_sys-resources/icon-note.gif) **说明：** 
>示例代码仅用于展示基本处理流程，请勿直接部署至生产环境使用。

-   **[SPP服务端注册](#ZH-CN_TOPIC_0000002572623930)**  

-   **[SPP客户端注册](#ZH-CN_TOPIC_0000002603342913)**  

-   **[SPP数据传输](#ZH-CN_TOPIC_0000002603343005)**  

-   **[GATT服务注册](#ZH-CN_TOPIC_0000002572623916)**  

-   **[GATT服务发现](#ZH-CN_TOPIC_0000002603343013)**  

### SPP服务端注册<a name="ZH-CN_TOPIC_0000002572623930"></a>

1.  服务端参考“[开启BR/EDR广播](#ZH-CN_TOPIC_0000002603222965)”中的步骤完成BR/EDR service回调注册、设备信息配置、BR/EDR协议栈使能。
2.  服务端使能SPP服务。

    ```
    errcode_t ret = bs_br_port_enable();
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    };
    ```

3.  服务端创建固定port。

    ```
    bs_br_port_create_info_t demo_port_create_info = {
        .uuid = {
            .uuid_len = 0x02,
            .uuid = {0x11, 0x22},
        },
        .name = "fixed_server",
        .name_len = 12,
    };
    uint8_t demo_spp_channel = 10;
    uint16_t demo_port_id = 6414;
    errcode_t ret = bs_br_port_create_server_port(&demo_port_create_info, demo_spp_channel, 1, &demo_port_id );
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

4.  服务端注册数传回调函数。

    ```
    static void demo_conn_req_cbk(uint16_t dev_id, uint16_t port)
    {
        // do your things here
    }
    
    static void demo_receive_data_cbk(uint16_t dev_id, uint16_t port, uint8_t *data, uint16_t data_len)
    {
        // do your things here
    }
    
    static void demo_conn_state_cbk(uint16_t dev_id, uint16_t port, uint8_t state)
    {
        // do your things here
    }
    
    bs_br_port_callbacks_t demo_func = {
        .conn_req_cbk = demo_conn_req_cbk,
        .receive_data_cbk = demo_receive_data_cbk,
        .conn_state_cbk = demo_conn_state_cbk,
    };
    errcode_t ret = bs_br_port_register_callbacks(&demo_port_create_info.uuid, &demo_func);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

5.  服务端开启广播。

### SPP客户端注册<a name="ZH-CN_TOPIC_0000002603342913"></a>

1.  客户端参考“[开启BR/EDR扫描](#ZH-CN_TOPIC_0000002603223013)”中的步骤完成BR/EDR service回调注册、设备信息配置、BR/EDR协议栈使能。
2.  客户端使能SPP服务。

    ```
    errcode_t ret = bs_br_port_enable();
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    };
    ```

3.  客户端创建远端固定port。

    ```
    bt_uuid_t demo_uuid = {
        .uuid_len = 0x02,
        .uuid = {0x11, 0x22},
    };
    uint8_t demo_spp_channel = 10;
    uint16_t demo_port_id = 6414;
    errcode_t ret = bs_br_port_create_remote_port(&demo_uuid, demo_spp_channel, demo_port_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

4.  客户端注册数传回调函数。

    ```
    static void demo_conn_req_cbk(uint16_t dev_id, uint16_t port)
    {
        // do your things here
    }
    
    static void demo_receive_data_cbk(uint16_t dev_id, uint16_t port, uint8_t *data, uint16_t data_len)
    {
        // do your things here
    }
    
    static void demo_conn_state_cbk(uint16_t dev_id, uint16_t port, uint8_t state)
    {
        // do your things here
    }
    
    bs_br_port_callbacks_t demo_func = {
        .conn_req_cbk = demo_conn_req_cbk,
        .receive_data_cbk = demo_receive_data_cbk,
        .conn_state_cbk = demo_conn_state_cbk,
    };
    errcode_t ret = bs_br_port_register_callbacks(&demo_uuid, &demo_func);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

5.  客户端开启扫描。

### SPP数据传输<a name="ZH-CN_TOPIC_0000002603343005"></a>

1.  服务端参考“[SPP服务端注册](#ZH-CN_TOPIC_0000002572623930)”进行SPP服务端注册。
2.  客户端参考“[SPP客户端注册](#ZH-CN_TOPIC_0000002603342913)”进行SPP客户端注册。
3.  客户端参考“[BR/EDR配对与取消配对](#ZH-CN_TOPIC_0000002603223041)”中配对的步骤与对端设备完成配对，获取对端设备ID dev\_id。
4.  客户端发起与对端port的连接。连接结果通过注册的bs\_br\_port\_conn\_state\_changed\_callback回调函数进行上报。

    ```
    errcode_t ret = bs_br_port_connect(dev_id, &demo_uuid, BS_BR_PORT_NOT_DISCOVER_REMOTE);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

5.  服务端接收到连接请求后通过注册的bs\_br\_port\_connection\_request\_callback回调函数进行上报。客户端调用发送连接响应接口通知客户端接受连接。

    ```
    errcode_t ret = bs_br_port_send_conn_rsp(demo_port_id, true);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

6.  客户端接收到连接响应消息后通过注册的bs\_br\_port\_conn\_state\_changed\_callback回调函数上报连接状态。
7.  客户端向服务端发送数据。

    ```
    uint8_t demo_data[] = "This is a test message.";
    uint8_t demo_data_len = sizeof(data) - 1;
    errcode_t ret = bs_br_port_write_data(dev_id, demo_port_id, demo_data, demo_data_len);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

8.  服务端接收到数据后通过注册的bs\_br\_port\_receive\_data\_callback回调函数进行上报。
9.  完成数据传输后，客户端断开SPP连接。

    ```
    errcode_t ret = bs_br_port_disconnect(dev_id, demo_port_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

10. SPP连接断开后，客户端与服务端可根据需要去注册SPP服务。

    ```
    errcode_t ret = bs_br_port_disable();
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

### GATT服务注册<a name="ZH-CN_TOPIC_0000002572623916"></a>

1.  服务端参考“[开启BLE广播](#ZH-CN_TOPIC_0000002603343001)”中的步骤完成BLE service回调注册、BLE协议栈使能与设备信息配置。
2.  服务端进行服务注册，获取对应的服务ID server\_id。

    ```
    bt_uuid_t demo_app_uuid = {
        .uuid_len = 0x02,
        .uuid = {0x11, 0x22},
    };
    uint8_t server_id= 0;
    errcode_t ret = bs_gatts_register(&demo_app_uuid, &server_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    };
    ```

3.  服务端根据配置的服务模板进行服务注册，获取对应服务实例ID inst\_id。

    ```
    uint32_t demo_profile_size = sizeof(bs_gatt_service_template_t) + 2 * sizeof(bs_gatts_attribute_info);
    bs_gatt_service_template_t *demo_profile = (bs_gatt_service_template_t *)malloc(demo_profile_size);
    memset(demo_profile, 0, demo_profile_size);
    *demo_profile = {
        .uuid = {4, {0xAA, 0x00}},
        .is_primary = 1,
        .attribute_count = 2,
        .cbks.start_service_cb = NULL,
        .cbks.delete_service_cb = NULL,
        .cbks.read_request_cb = NULL,
        .cbks.write_request_cb = NULL,
        .cbks.indicate_cfm_cb = NULL,
        .cbks.mtu_changed_cb = NULL,
        .attribute_array = {
            [0] = {
                .type = BS_GATTS_CHARAC_TYPE,
                .character = {
                    .uuid = {16, {0xFF, 0x01}},
                    .permissions = BS_GATT_ATTRIBUTE_PERMISSION_READ | BS_GATT_ATTRIBUTE_PERMISSION_WRITE,
                    .properties = BS_GATT_CHARACTER_PROPERTY_BIT_NOTIFY | BS_GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP |
                                  BS_GATT_CHARACTER_PROPERTY_BIT_READ,
                    .value_len = 517,
                    .value = {0x00, 0x00},
                },
            },
            [1] = {
                .type = BS_GATTS_DESC_TYPE,
                .descriptor = {
                    .uuid = {16, {0x29, 0x02}},
                    .permissions = BS_GATT_ATTRIBUTE_PERMISSION_READ | BS_GATT_ATTRIBUTE_PERMISSION_WRITE,
                    .value_len = 1,
                    .value = {0x01, 0x00},
                },
            },
        },
    };
    uint8_t inst_id= 0;
    errcode_t ret = bs_gatts_service_instance_init(demo_service_id, server_id, &inst_id);
    free(demo_profile);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    };
    ```

4.  服务端开启广播。

### GATT服务发现<a name="ZH-CN_TOPIC_0000002603343013"></a>

1.  客户端参考“[开启BLE扫描](#ZH-CN_TOPIC_0000002572783544)”中的步骤完成BLE service回调注册、BLE协议栈使能与设备信息配置。
2.  客户端进行服务注册，获取对应的客户端ID client\_id。

    ```
    bt_uuid_t demo_app_uuid = {
        .uuid_len = 0x02,
        .uuid = {0x22, 0x33},
    };
    uint8_t client_id= 0;
    errcode_t ret = bs_gattc_register(&demo_app_uuid, &client_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    };
    ```

3.  客户端开启扫描。
4.  客户端参考“[BLE配对与取消配对](#ZH-CN_TOPIC_0000002603222997)”中的配对的步骤完成配对，获取对端设备ID dev\_id。
5.  客户端根据客户端ID client\_id与对端设备ID dev\_id进行服务发现，通过注册服务发现回调函数获取上报的发现结果。

    ```
    void demo_bs_gattc_find_service_callback(uint16_t dev_id, uint8_t client_id, uint8_t service_count,
        bs_gattc_find_service_result_node_t *structure_list, errcode_t status)
    {
        // do your things here
    }
    
    errcode_t ret = bs_gattc_find_all_service(dev_id, client_id, demo_bs_gattc_find_service_callback, true);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

6.  客户端通过服务发现回调函数获取发现结果，据此可获取对应服务的UUID service\_uuid，并对对应UUID服务的回调函数进行注册。

    ```
    void demo_mtu_changed_cb(uint16_t dev_id, uint8_t client_id, uint16_t mtu_size, errcode_t status)
    {
        // do your things here
    }
    
    void demo_read_cb(uint16_t dev_id, uint8_t client_id, bs_gattc_handle_value_t *read_result,
        uint16_t status)
    {
        // do your things here
    }
    
    void demo_write_cb(uint16_t dev_id, uint8_t client_id, uint16_t handle, uint16_t status)
    {
        // do your things here
    }
    
    void demo_notification_cb(uint16_t dev_id, uint8_t client_id, bs_gattc_handle_value_t *data,
        errcode_t status)
    {
        // do your things here
    }
    
    void demo_indication_cb(uint16_t dev_id, uint8_t client_id, bs_gattc_handle_value_t *data,
        errcode_t status)
    {
        // do your things here
    }
    
    bs_gattc_operation_cbk_t demo_bs_gattc_operation_cbk= { 0 };
    demo_bs_gattc_operation_cbk.mtu_changed_cb = demo_mtu_changed_cb;
    demo_bs_gattc_operation_cbk.read_write_cbk.read_cb = demo_read_cb;
    demo_bs_gattc_operation_cbk.read_write_cbk.write_cb = demo_write_cb;
    demo_bs_gattc_operation_cbk.notication_indication_cbk.notification_cb = demo_notification_cb;
    demo_bs_gattc_operation_cbk.notication_indication_cbk.indication_cb = demo_indication_cb;
    errcode_t ret = bs_gattc_register_callbacks(dev_id, client_id, &service_uuid, &demo_bs_gattc_operation_cbk);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

# 通讯管理<a name="ZH-CN_TOPIC_0000002603223059"></a>

-   **[概述](#ZH-CN_TOPIC_0000002603343023)**  

-   **[接口说明](#ZH-CN_TOPIC_0000002603223049)**  

-   **[开发示例](#ZH-CN_TOPIC_0000002603342939)**  

## 概述<a name="ZH-CN_TOPIC_0000002603343023"></a>

通讯管理实现蓝牙设备的电话簿与通话功能，支持连接、断开、状态通知等操作，提供完整的蓝牙通话控制能力。

## 接口说明<a name="ZH-CN_TOPIC_0000002603223049"></a>

-   **[电话簿管理](#ZH-CN_TOPIC_0000002572783510)**  

-   **[通话管理](#ZH-CN_TOPIC_0000002603223037)**  

### 电话簿管理<a name="ZH-CN_TOPIC_0000002572783510"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002603342993)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002603222975)**  

#### 数据结构<a name="ZH-CN_TOPIC_0000002603342993"></a>

-   **[bs\_br\_phone\_book\_vcard\_version\_t](#ZH-CN_TOPIC_0000002572783548)**  

-   **[bs\_br\_call\_log\_type\_t](#ZH-CN_TOPIC_0000002572783480)**  

-   **[bs\_br\_phone\_book\_callbacks\_t](#ZH-CN_TOPIC_0000002603223047)**  

##### bs\_br\_phone\_book\_vcard\_version\_t<a name="ZH-CN_TOPIC_0000002572783548"></a>

【结构体说明】

电话薄vCard版本。

【结构体定义】  

```
typedef enum {
    BR_PHONE_BOOK_VCARD21 = 0,              /*!< @if Eng vCard version is 2.1
                                                @else vCard 2.1 版本 @endif */
    BR_PHONE_BOOK_VCARD30,                  /*!< @if Eng vCard version is 3.0
                                                @else vCard 3.0 版本 @endif */
} bs_br_phone_book_vcard_version_t;
```

【注意事项】

无。

##### bs\_br\_call\_log\_type\_t<a name="ZH-CN_TOPIC_0000002572783480"></a>

【结构体说明】

通话记录类型。

【结构体定义】

```
typedef enum {
    BR_CALL_LOG_ICH = 0x1,          /*!< @if Eng incoming call log
                                        @else 来电通话记录 @endif */
    BR_CALL_LOG_OCH,                /*!< @if Eng outgoing call log
                                        @else 去电通话记录 @endif */
    BR_CALL_LOG_MCH,                /*!< @if Eng missed call log
                                        @else 未接来电通话记录 @endif */
    BR_CALL_LOG_CCH,                /*!< @if Eng all call log
                                        @else 所有的通话记录 @endif */
} bs_br_call_log_type_t;
```

【注意事项】

无。

##### bs\_br\_phone\_book\_callbacks\_t<a name="ZH-CN_TOPIC_0000002603223047"></a>

【结构体说明】

电话薄回调函数接口定义。

【结构体定义】

```
typedef struct {
    bs_br_phone_book_conn_state_changed_cbk connect_state_cbk;          /*!< @if Eng Connection state callback.
                                                                             @else 连接状态上报回调函数。 @endif */
    bs_br_phone_book_recevice_cbk           recevice_phone_book_cbk;    /*!< @if Eng Pull phone book data callback.
                                                                             @else 拉取电话薄数据的回调函数。 @endif */
    bs_br_phone_book_result_cbk             phone_book_result_cbk;      /*!< @if Eng Pull phone book result callback.
                                                                             @else 拉取电话薄结果的回调函数。 @endif */
    bs_br_call_log_recevice_cbk             recevice_call_log_cbk;      /*!< @if Eng Pull call log data callback.
                                                                             @else 拉取通话记录数据的回调函数。 @endif */
    bs_br_call_log_result_cbk               call_log_result_cbk;        /*!< @if Eng Pull call log result callback.
                                                                             @else 拉取通话记录结果的回调函数。 @endif */
} bs_br_phone_book_callbacks_t;
```

【注意事项】

无。

#### 函数接口<a name="ZH-CN_TOPIC_0000002603222975"></a>

-   **[bs\_br\_phone\_book\_connect](#ZH-CN_TOPIC_0000002603223043)**  

-   **[bs\_br\_phone\_book\_disconnect](#ZH-CN_TOPIC_0000002572623856)**  

-   **[bs\_br\_pull\_phone\_book](#ZH-CN_TOPIC_0000002572783536)**  

-   **[bs\_br\_pull\_call\_log](#ZH-CN_TOPIC_0000002572623864)**  

-   **[bs\_br\_phone\_book\_register\_callbacks](#ZH-CN_TOPIC_0000002572783546)**  

-   **[bs\_br\_phone\_book\_unregister\_callbacks](#ZH-CN_TOPIC_0000002603222971)**  

##### bs\_br\_phone\_book\_connect<a name="ZH-CN_TOPIC_0000002603223043"></a>

【描述】

BR/EDR连接电话薄。

【接口定义】

```
errcode_t bs_br_phone_book_connect(uint16_t dev_id);
```

【参数】

<a name="zh-cn_topic_0000002585584675_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585584675_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585584675_p1551mcpsimp"><a name="zh-cn_topic_0000002585584675_p1551mcpsimp"></a><a name="zh-cn_topic_0000002585584675_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585584675_p1553mcpsimp"><a name="zh-cn_topic_0000002585584675_p1553mcpsimp"></a><a name="zh-cn_topic_0000002585584675_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585584675_p1555mcpsimp"><a name="zh-cn_topic_0000002585584675_p1555mcpsimp"></a><a name="zh-cn_topic_0000002585584675_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585584675_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585584675_p6147106163216"><a name="zh-cn_topic_0000002585584675_p6147106163216"></a><a name="zh-cn_topic_0000002585584675_p6147106163216"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585584675_p8879161614328"><a name="zh-cn_topic_0000002585584675_p8879161614328"></a><a name="zh-cn_topic_0000002585584675_p8879161614328"></a>远端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585584675_p1562mcpsimp"><a name="zh-cn_topic_0000002585584675_p1562mcpsimp"></a><a name="zh-cn_topic_0000002585584675_p1562mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585584675_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585584675_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585584675_p1586mcpsimp"><a name="zh-cn_topic_0000002585584675_p1586mcpsimp"></a><a name="zh-cn_topic_0000002585584675_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585584675_p1588mcpsimp"><a name="zh-cn_topic_0000002585584675_p1588mcpsimp"></a><a name="zh-cn_topic_0000002585584675_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585584675_p1590mcpsimp"><a name="zh-cn_topic_0000002585584675_p1590mcpsimp"></a><a name="zh-cn_topic_0000002585584675_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585584675_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585584675_p1593mcpsimp"><a name="zh-cn_topic_0000002585584675_p1593mcpsimp"></a><a name="zh-cn_topic_0000002585584675_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585584675_p1595mcpsimp"><a name="zh-cn_topic_0000002585584675_p1595mcpsimp"></a><a name="zh-cn_topic_0000002585584675_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585584675_p1596mcpsimp"><a name="zh-cn_topic_0000002585584675_p1596mcpsimp"></a><a name="zh-cn_topic_0000002585584675_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585584675_p1598mcpsimp"><a name="zh-cn_topic_0000002585584675_p1598mcpsimp"></a><a name="zh-cn_topic_0000002585584675_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_phone\_book\_disconnect<a name="ZH-CN_TOPIC_0000002572623856"></a>

【描述】

BR/EDR断连电话薄。

【接口定义】

```
errcode_t bs_br_phone_book_disconnect(uint16_t dev_id);
```

【参数】

<a name="zh-cn_topic_0000002555144726_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555144726_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555144726_p1551mcpsimp"><a name="zh-cn_topic_0000002555144726_p1551mcpsimp"></a><a name="zh-cn_topic_0000002555144726_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555144726_p1553mcpsimp"><a name="zh-cn_topic_0000002555144726_p1553mcpsimp"></a><a name="zh-cn_topic_0000002555144726_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555144726_p1555mcpsimp"><a name="zh-cn_topic_0000002555144726_p1555mcpsimp"></a><a name="zh-cn_topic_0000002555144726_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555144726_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555144726_p6147106163216"><a name="zh-cn_topic_0000002555144726_p6147106163216"></a><a name="zh-cn_topic_0000002555144726_p6147106163216"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555144726_p8879161614328"><a name="zh-cn_topic_0000002555144726_p8879161614328"></a><a name="zh-cn_topic_0000002555144726_p8879161614328"></a>远端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555144726_p1562mcpsimp"><a name="zh-cn_topic_0000002555144726_p1562mcpsimp"></a><a name="zh-cn_topic_0000002555144726_p1562mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555144726_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555144726_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555144726_p1586mcpsimp"><a name="zh-cn_topic_0000002555144726_p1586mcpsimp"></a><a name="zh-cn_topic_0000002555144726_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555144726_p1588mcpsimp"><a name="zh-cn_topic_0000002555144726_p1588mcpsimp"></a><a name="zh-cn_topic_0000002555144726_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555144726_p1590mcpsimp"><a name="zh-cn_topic_0000002555144726_p1590mcpsimp"></a><a name="zh-cn_topic_0000002555144726_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555144726_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555144726_p1593mcpsimp"><a name="zh-cn_topic_0000002555144726_p1593mcpsimp"></a><a name="zh-cn_topic_0000002555144726_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555144726_p1595mcpsimp"><a name="zh-cn_topic_0000002555144726_p1595mcpsimp"></a><a name="zh-cn_topic_0000002555144726_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555144726_p1596mcpsimp"><a name="zh-cn_topic_0000002555144726_p1596mcpsimp"></a><a name="zh-cn_topic_0000002555144726_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555144726_p1598mcpsimp"><a name="zh-cn_topic_0000002555144726_p1598mcpsimp"></a><a name="zh-cn_topic_0000002555144726_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_pull\_phone\_book<a name="ZH-CN_TOPIC_0000002572783536"></a>

【描述】

BR/EDR下载电话薄。

【接口定义】

```
errcode_t bs_br_pull_phone_book(uint16_t dev_id, bs_br_phone_book_vcard_version_t version);
```

【参数】

<a name="zh-cn_topic_0000002554985098_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002554985098_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002554985098_p1551mcpsimp"><a name="zh-cn_topic_0000002554985098_p1551mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002554985098_p1553mcpsimp"><a name="zh-cn_topic_0000002554985098_p1553mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002554985098_p1555mcpsimp"><a name="zh-cn_topic_0000002554985098_p1555mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002554985098_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985098_p1558mcpsimp"><a name="zh-cn_topic_0000002554985098_p1558mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1558mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985098_p17461137133418"><a name="zh-cn_topic_0000002554985098_p17461137133418"></a><a name="zh-cn_topic_0000002554985098_p17461137133418"></a>远端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985098_p1562mcpsimp"><a name="zh-cn_topic_0000002554985098_p1562mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002554985098_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985098_p1565mcpsimp"><a name="zh-cn_topic_0000002554985098_p1565mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1565mcpsimp"></a>version</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985098_p1567mcpsimp"><a name="zh-cn_topic_0000002554985098_p1567mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1567mcpsimp"></a>vCard版本。详见“<a href="#ZH-CN_TOPIC_0000002572783548">bs_br_phone_book_vcard_version_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985098_p1569mcpsimp"><a name="zh-cn_topic_0000002554985098_p1569mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1569mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002554985098_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002554985098_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002554985098_p1586mcpsimp"><a name="zh-cn_topic_0000002554985098_p1586mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002554985098_p1588mcpsimp"><a name="zh-cn_topic_0000002554985098_p1588mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002554985098_p1590mcpsimp"><a name="zh-cn_topic_0000002554985098_p1590mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002554985098_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985098_p1593mcpsimp"><a name="zh-cn_topic_0000002554985098_p1593mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985098_p1595mcpsimp"><a name="zh-cn_topic_0000002554985098_p1595mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002554985098_p1596mcpsimp"><a name="zh-cn_topic_0000002554985098_p1596mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985098_p1598mcpsimp"><a name="zh-cn_topic_0000002554985098_p1598mcpsimp"></a><a name="zh-cn_topic_0000002554985098_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_pull\_call\_log<a name="ZH-CN_TOPIC_0000002572623864"></a>

【描述】

BR/EDR下载通话记录。

【接口定义】

```
errcode_t bs_br_pull_call_log(uint16_t dev_id, bs_br_call_log_type_t type, bs_br_phone_book_vcard_version_t version);
```

【参数】

<a name="zh-cn_topic_0000002585704721_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585704721_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585704721_p1551mcpsimp"><a name="zh-cn_topic_0000002585704721_p1551mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585704721_p1553mcpsimp"><a name="zh-cn_topic_0000002585704721_p1553mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585704721_p1555mcpsimp"><a name="zh-cn_topic_0000002585704721_p1555mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585704721_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704721_p1558mcpsimp"><a name="zh-cn_topic_0000002585704721_p1558mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1558mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704721_p17461137133418"><a name="zh-cn_topic_0000002585704721_p17461137133418"></a><a name="zh-cn_topic_0000002585704721_p17461137133418"></a>远端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704721_p1562mcpsimp"><a name="zh-cn_topic_0000002585704721_p1562mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585704721_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704721_p1565mcpsimp"><a name="zh-cn_topic_0000002585704721_p1565mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1565mcpsimp"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704721_p1567mcpsimp"><a name="zh-cn_topic_0000002585704721_p1567mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1567mcpsimp"></a>通话记录类型。详见“<a href="#ZH-CN_TOPIC_0000002572783480">bs_br_call_log_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704721_p1569mcpsimp"><a name="zh-cn_topic_0000002585704721_p1569mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1569mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585704721_row1570mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704721_p868722918366"><a name="zh-cn_topic_0000002585704721_p868722918366"></a><a name="zh-cn_topic_0000002585704721_p868722918366"></a>version</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704721_p16871429143613"><a name="zh-cn_topic_0000002585704721_p16871429143613"></a><a name="zh-cn_topic_0000002585704721_p16871429143613"></a>vCard版本。详见“<a href="#ZH-CN_TOPIC_0000002572783548">bs_br_phone_book_vcard_version_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704721_p1687729203620"><a name="zh-cn_topic_0000002585704721_p1687729203620"></a><a name="zh-cn_topic_0000002585704721_p1687729203620"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585704721_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585704721_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585704721_p1586mcpsimp"><a name="zh-cn_topic_0000002585704721_p1586mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585704721_p1588mcpsimp"><a name="zh-cn_topic_0000002585704721_p1588mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585704721_p1590mcpsimp"><a name="zh-cn_topic_0000002585704721_p1590mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585704721_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704721_p1593mcpsimp"><a name="zh-cn_topic_0000002585704721_p1593mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704721_p1595mcpsimp"><a name="zh-cn_topic_0000002585704721_p1595mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585704721_p1596mcpsimp"><a name="zh-cn_topic_0000002585704721_p1596mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704721_p1598mcpsimp"><a name="zh-cn_topic_0000002585704721_p1598mcpsimp"></a><a name="zh-cn_topic_0000002585704721_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_phone\_book\_register\_callbacks<a name="ZH-CN_TOPIC_0000002572783546"></a>

【描述】

BR/EDR注册电话薄回调。

【接口定义】

```
errcode_t bs_br_phone_book_register_callbacks(uint8_t app_id, const bs_br_phone_book_callbacks_t *funs);
```

【参数】

<a name="zh-cn_topic_0000002585584677_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585584677_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585584677_p1551mcpsimp"><a name="zh-cn_topic_0000002585584677_p1551mcpsimp"></a><a name="zh-cn_topic_0000002585584677_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585584677_p1553mcpsimp"><a name="zh-cn_topic_0000002585584677_p1553mcpsimp"></a><a name="zh-cn_topic_0000002585584677_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585584677_p1555mcpsimp"><a name="zh-cn_topic_0000002585584677_p1555mcpsimp"></a><a name="zh-cn_topic_0000002585584677_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585584677_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585584677_p1983815423385"><a name="zh-cn_topic_0000002585584677_p1983815423385"></a><a name="zh-cn_topic_0000002585584677_p1983815423385"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585584677_p137585420383"><a name="zh-cn_topic_0000002585584677_p137585420383"></a><a name="zh-cn_topic_0000002585584677_p137585420383"></a>应用ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585584677_p1562mcpsimp"><a name="zh-cn_topic_0000002585584677_p1562mcpsimp"></a><a name="zh-cn_topic_0000002585584677_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585584677_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585584677_p107782573915"><a name="zh-cn_topic_0000002585584677_p107782573915"></a><a name="zh-cn_topic_0000002585584677_p107782573915"></a>cbks</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585584677_p1567mcpsimp"><a name="zh-cn_topic_0000002585584677_p1567mcpsimp"></a><a name="zh-cn_topic_0000002585584677_p1567mcpsimp"></a>回调函数。详见“<a href="#ZH-CN_TOPIC_0000002603223047">bs_br_phone_book_callbacks_t</a>”</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585584677_p1569mcpsimp"><a name="zh-cn_topic_0000002585584677_p1569mcpsimp"></a><a name="zh-cn_topic_0000002585584677_p1569mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585584677_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585584677_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585584677_p1586mcpsimp"><a name="zh-cn_topic_0000002585584677_p1586mcpsimp"></a><a name="zh-cn_topic_0000002585584677_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585584677_p1588mcpsimp"><a name="zh-cn_topic_0000002585584677_p1588mcpsimp"></a><a name="zh-cn_topic_0000002585584677_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585584677_p1590mcpsimp"><a name="zh-cn_topic_0000002585584677_p1590mcpsimp"></a><a name="zh-cn_topic_0000002585584677_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585584677_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585584677_p1593mcpsimp"><a name="zh-cn_topic_0000002585584677_p1593mcpsimp"></a><a name="zh-cn_topic_0000002585584677_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585584677_p1595mcpsimp"><a name="zh-cn_topic_0000002585584677_p1595mcpsimp"></a><a name="zh-cn_topic_0000002585584677_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585584677_p1596mcpsimp"><a name="zh-cn_topic_0000002585584677_p1596mcpsimp"></a><a name="zh-cn_topic_0000002585584677_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585584677_p1598mcpsimp"><a name="zh-cn_topic_0000002585584677_p1598mcpsimp"></a><a name="zh-cn_topic_0000002585584677_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_phone\_book\_unregister\_callbacks<a name="ZH-CN_TOPIC_0000002603222971"></a>

【描述】

BR/EDR去注册电话薄回调。

【接口定义】

```
errcode_t bs_br_phone_book_unregister_callbacks(uint8_t app_id);
```

【参数】

<a name="zh-cn_topic_0000002554985100_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002554985100_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002554985100_p1551mcpsimp"><a name="zh-cn_topic_0000002554985100_p1551mcpsimp"></a><a name="zh-cn_topic_0000002554985100_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002554985100_p1553mcpsimp"><a name="zh-cn_topic_0000002554985100_p1553mcpsimp"></a><a name="zh-cn_topic_0000002554985100_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002554985100_p1555mcpsimp"><a name="zh-cn_topic_0000002554985100_p1555mcpsimp"></a><a name="zh-cn_topic_0000002554985100_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002554985100_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985100_p1983815423385"><a name="zh-cn_topic_0000002554985100_p1983815423385"></a><a name="zh-cn_topic_0000002554985100_p1983815423385"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985100_p137585420383"><a name="zh-cn_topic_0000002554985100_p137585420383"></a><a name="zh-cn_topic_0000002554985100_p137585420383"></a>应用ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985100_p1562mcpsimp"><a name="zh-cn_topic_0000002554985100_p1562mcpsimp"></a><a name="zh-cn_topic_0000002554985100_p1562mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002554985100_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002554985100_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002554985100_p1586mcpsimp"><a name="zh-cn_topic_0000002554985100_p1586mcpsimp"></a><a name="zh-cn_topic_0000002554985100_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002554985100_p1588mcpsimp"><a name="zh-cn_topic_0000002554985100_p1588mcpsimp"></a><a name="zh-cn_topic_0000002554985100_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002554985100_p1590mcpsimp"><a name="zh-cn_topic_0000002554985100_p1590mcpsimp"></a><a name="zh-cn_topic_0000002554985100_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002554985100_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985100_p1593mcpsimp"><a name="zh-cn_topic_0000002554985100_p1593mcpsimp"></a><a name="zh-cn_topic_0000002554985100_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985100_p1595mcpsimp"><a name="zh-cn_topic_0000002554985100_p1595mcpsimp"></a><a name="zh-cn_topic_0000002554985100_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002554985100_p1596mcpsimp"><a name="zh-cn_topic_0000002554985100_p1596mcpsimp"></a><a name="zh-cn_topic_0000002554985100_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985100_p1598mcpsimp"><a name="zh-cn_topic_0000002554985100_p1598mcpsimp"></a><a name="zh-cn_topic_0000002554985100_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

### 通话管理<a name="ZH-CN_TOPIC_0000002603223037"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002572783502)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002572623898)**  

#### 数据结构<a name="ZH-CN_TOPIC_0000002572783502"></a>

-   **[bs\_br\_call\_hfp\_role\_t](#ZH-CN_TOPIC_0000002572623896)**  

-   **[bs\_br\_call\_opt\_type\_t](#ZH-CN_TOPIC_0000002572783566)**  

-   **[bs\_br\_call\_volume\_type\_t](#ZH-CN_TOPIC_0000002603342915)**  

-   **[bs\_br\_call\_notify\_type\_t](#ZH-CN_TOPIC_0000002572623866)**  

-   **[bs\_br\_call\_callbacks\_t](#ZH-CN_TOPIC_0000002603342973)**  

-   **[bs\_br\_sco\_callbacks\_t](#ZH-CN_TOPIC_0000002603343011)**  

##### bs\_br\_call\_hfp\_role\_t<a name="ZH-CN_TOPIC_0000002572623896"></a>

【结构体说明】

BR/EDR SPP服务端创建port参数信息。

【结构体定义】

```
typedef struct {
    bt_uuid_t uuid;         /*!< @if Eng Application UUID. Supports 2 or 16 bits.
                                    @else 应用uuid。支持2位或16位 @endif */
    char *name;             /*!< @if Eng port server name.
                                    @else  port Server名称。 @endif */
    uint8_t name_len;      /*!< @if Eng Source port server name length.
                                    @else  port Server名称长度。 @endif */
} bs_br_port_create_info_t;
```

【注意事项】

无。

##### bs\_br\_call\_opt\_type\_t<a name="ZH-CN_TOPIC_0000002572783566"></a>

【结构体说明】

BR/EDR SPP服务端创建port参数信息。

【结构体定义】

```
typedef struct {
    bt_uuid_t uuid;         /*!< @if Eng Application UUID. Supports 2 or 16 bits.
                                    @else 应用uuid。支持2位或16位 @endif */
    char *name;             /*!< @if Eng port server name.
                                    @else  port Server名称。 @endif */
    uint8_t name_len;      /*!< @if Eng Source port server name length.
                                    @else  port Server名称长度。 @endif */
} bs_br_port_create_info_t;
```

【注意事项】

无。

##### bs\_br\_call\_volume\_type\_t<a name="ZH-CN_TOPIC_0000002603342915"></a>

【结构体说明】

BR/EDR SPP服务端创建port参数信息。

【结构体定义】

```
typedef struct {
    bt_uuid_t uuid;         /*!< @if Eng Application UUID. Supports 2 or 16 bits.
                                    @else 应用uuid。支持2位或16位 @endif */
    char *name;             /*!< @if Eng port server name.
                                    @else  port Server名称。 @endif */
    uint8_t name_len;      /*!< @if Eng Source port server name length.
                                    @else  port Server名称长度。 @endif */
} bs_br_port_create_info_t;
```

【注意事项】

无。

##### bs\_br\_call\_notify\_type\_t<a name="ZH-CN_TOPIC_0000002572623866"></a>

【结构体说明】

BR/EDR SPP服务端创建port参数信息。

【结构体定义】

```
typedef struct {
    bt_uuid_t uuid;         /*!< @if Eng Application UUID. Supports 2 or 16 bits.
                                    @else 应用uuid。支持2位或16位 @endif */
    char *name;             /*!< @if Eng port server name.
                                    @else  port Server名称。 @endif */
    uint8_t name_len;      /*!< @if Eng Source port server name length.
                                    @else  port Server名称长度。 @endif */
} bs_br_port_create_info_t;
```

【注意事项】

无。

##### bs\_br\_call\_callbacks\_t<a name="ZH-CN_TOPIC_0000002603342973"></a>

【结构体说明】

通话回调函数结构体。

【结构体定义】

```
typedef struct {
    bs_br_call_conn_state_cbk                connect_state_cbk;
    bs_br_call_voice_recognition_status_cbk  voice_reco_status_cbk;
    bs_br_call_set_volume_request_cbk        set_volume_request_cbk;
    bs_br_call_status_notify_cbk             call_status_notify_cbk;
    bs_br_call_band_ring_tone_changed_cbk    band_ring_tone_changed_cbk;
    bs_br_call_operate_request_cbk           call_opt_req_cbk;
    bs_br_call_vendor_at_cbk                 vendor_at_cbk;
} bs_br_call_callbacks_t;
```

【注意事项】

无。

##### bs\_br\_sco\_callbacks\_t<a name="ZH-CN_TOPIC_0000002603343011"></a>

【结构体说明】

BR/EDR SCO回调函数结构体。

【结构体定义】

```
typedef struct {
    bs_br_sco_state_changed_cbk sco_state_cbk;
    bs_br_sco_codec_report_cbk sco_codec_cbk;
} bs_br_sco_callbacks_t;
```

【注意事项】

无。

#### 函数接口<a name="ZH-CN_TOPIC_0000002572623898"></a>

-   **[bs\_br\_call\_hfp\_register](#ZH-CN_TOPIC_0000002572623822)**  

-   **[bs\_br\_call\_hfp\_unregister](#ZH-CN_TOPIC_0000002603342961)**  

-   **[bs\_br\_call\_hfp\_connect](#ZH-CN_TOPIC_0000002572783526)**  

-   **[bs\_br\_call\_hfp\_disconnect](#ZH-CN_TOPIC_0000002572783574)**  

-   **[bs\_br\_call\_operate\_request](#ZH-CN_TOPIC_0000002603222973)**  

-   **[bs\_br\_call\_voice\_recognition\_request](#ZH-CN_TOPIC_0000002603342957)**  

-   **[bs\_br\_call\_set\_volume\_request](#ZH-CN_TOPIC_0000002603222999)**  

-   **[bs\_br\_call\_send\_dtmf\_tone\_request](#ZH-CN_TOPIC_0000002572623878)**  

-   **[bs\_br\_call\_status\_notify](#ZH-CN_TOPIC_0000002572783556)**  

-   **[bs\_br\_call\_send\_vendor\_at\_request](#ZH-CN_TOPIC_0000002603342997)**  

-   **[bs\_br\_call\_register\_callbacks](#ZH-CN_TOPIC_0000002603223069)**  

-   **[bs\_br\_call\_unregister\_callbacks](#ZH-CN_TOPIC_0000002572783520)**  

-   **[bs\_br\_create\_sco\_connection](#ZH-CN_TOPIC_0000002572623928)**  

-   **[bs\_br\_release\_sco\_connection](#ZH-CN_TOPIC_0000002572783560)**  

-   **[bs\_br\_create\_sco\_result\_reply](#ZH-CN_TOPIC_0000002603343017)**  

-   **[bs\_br\_sco\_register\_callbacks](#ZH-CN_TOPIC_0000002572783522)**  

-   **[bs\_br\_sco\_unregister\_callbacks](#ZH-CN_TOPIC_0000002603343009)**  

##### bs\_br\_call\_hfp\_register<a name="ZH-CN_TOPIC_0000002572623822"></a>

【描述】

BR/EDR注册HFP服务。

【接口定义】

```
errcode_t bs_br_call_hfp_register(uint8_t hfp_role);
```

【参数】

<a name="zh-cn_topic_0000002554985104_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002554985104_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002554985104_p1551mcpsimp"><a name="zh-cn_topic_0000002554985104_p1551mcpsimp"></a><a name="zh-cn_topic_0000002554985104_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002554985104_p1553mcpsimp"><a name="zh-cn_topic_0000002554985104_p1553mcpsimp"></a><a name="zh-cn_topic_0000002554985104_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002554985104_p1555mcpsimp"><a name="zh-cn_topic_0000002554985104_p1555mcpsimp"></a><a name="zh-cn_topic_0000002554985104_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002554985104_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985104_p11853124611587"><a name="zh-cn_topic_0000002554985104_p11853124611587"></a><a name="zh-cn_topic_0000002554985104_p11853124611587"></a>hfp_role</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985104_p1560mcpsimp"><a name="zh-cn_topic_0000002554985104_p1560mcpsimp"></a><a name="zh-cn_topic_0000002554985104_p1560mcpsimp"></a>服务角色。详见“<a href="#ZH-CN_TOPIC_0000002572623896">bs_br_call_hfp_role_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985104_p1562mcpsimp"><a name="zh-cn_topic_0000002554985104_p1562mcpsimp"></a><a name="zh-cn_topic_0000002554985104_p1562mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002554985104_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002554985104_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002554985104_p1586mcpsimp"><a name="zh-cn_topic_0000002554985104_p1586mcpsimp"></a><a name="zh-cn_topic_0000002554985104_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002554985104_p1588mcpsimp"><a name="zh-cn_topic_0000002554985104_p1588mcpsimp"></a><a name="zh-cn_topic_0000002554985104_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002554985104_p1590mcpsimp"><a name="zh-cn_topic_0000002554985104_p1590mcpsimp"></a><a name="zh-cn_topic_0000002554985104_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002554985104_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985104_p1593mcpsimp"><a name="zh-cn_topic_0000002554985104_p1593mcpsimp"></a><a name="zh-cn_topic_0000002554985104_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985104_p1595mcpsimp"><a name="zh-cn_topic_0000002554985104_p1595mcpsimp"></a><a name="zh-cn_topic_0000002554985104_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002554985104_p1596mcpsimp"><a name="zh-cn_topic_0000002554985104_p1596mcpsimp"></a><a name="zh-cn_topic_0000002554985104_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985104_p1598mcpsimp"><a name="zh-cn_topic_0000002554985104_p1598mcpsimp"></a><a name="zh-cn_topic_0000002554985104_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_call\_hfp\_unregister<a name="ZH-CN_TOPIC_0000002603342961"></a>

【描述】

BR/EDR去注册HFP服务。

【接口定义】

```
errcode_t bs_br_call_hfp_unregister(uint8_t hfp_role);
```

【参数】

<a name="zh-cn_topic_0000002585704729_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585704729_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585704729_p1551mcpsimp"><a name="zh-cn_topic_0000002585704729_p1551mcpsimp"></a><a name="zh-cn_topic_0000002585704729_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585704729_p1553mcpsimp"><a name="zh-cn_topic_0000002585704729_p1553mcpsimp"></a><a name="zh-cn_topic_0000002585704729_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585704729_p1555mcpsimp"><a name="zh-cn_topic_0000002585704729_p1555mcpsimp"></a><a name="zh-cn_topic_0000002585704729_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585704729_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704729_p11853124611587"><a name="zh-cn_topic_0000002585704729_p11853124611587"></a><a name="zh-cn_topic_0000002585704729_p11853124611587"></a>hfp_role</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704729_p1560mcpsimp"><a name="zh-cn_topic_0000002585704729_p1560mcpsimp"></a><a name="zh-cn_topic_0000002585704729_p1560mcpsimp"></a>服务角色。详见“<a href="#ZH-CN_TOPIC_0000002572623896">bs_br_call_hfp_role_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704729_p1562mcpsimp"><a name="zh-cn_topic_0000002585704729_p1562mcpsimp"></a><a name="zh-cn_topic_0000002585704729_p1562mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585704729_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585704729_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585704729_p1586mcpsimp"><a name="zh-cn_topic_0000002585704729_p1586mcpsimp"></a><a name="zh-cn_topic_0000002585704729_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585704729_p1588mcpsimp"><a name="zh-cn_topic_0000002585704729_p1588mcpsimp"></a><a name="zh-cn_topic_0000002585704729_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585704729_p1590mcpsimp"><a name="zh-cn_topic_0000002585704729_p1590mcpsimp"></a><a name="zh-cn_topic_0000002585704729_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585704729_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704729_p1593mcpsimp"><a name="zh-cn_topic_0000002585704729_p1593mcpsimp"></a><a name="zh-cn_topic_0000002585704729_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704729_p1595mcpsimp"><a name="zh-cn_topic_0000002585704729_p1595mcpsimp"></a><a name="zh-cn_topic_0000002585704729_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585704729_p1596mcpsimp"><a name="zh-cn_topic_0000002585704729_p1596mcpsimp"></a><a name="zh-cn_topic_0000002585704729_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704729_p1598mcpsimp"><a name="zh-cn_topic_0000002585704729_p1598mcpsimp"></a><a name="zh-cn_topic_0000002585704729_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_call\_hfp\_connect<a name="ZH-CN_TOPIC_0000002572783526"></a>

【描述】

BR/EDR建立蓝牙通话服务连接。

【接口定义】

```
errcode_t bs_br_call_hfp_connect(uint16_t dev_id, uint8_t hfp_role);
```

【参数】

<a name="zh-cn_topic_0000002585584685_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585584685_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585584685_p1551mcpsimp"><a name="zh-cn_topic_0000002585584685_p1551mcpsimp"></a><a name="zh-cn_topic_0000002585584685_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585584685_p1553mcpsimp"><a name="zh-cn_topic_0000002585584685_p1553mcpsimp"></a><a name="zh-cn_topic_0000002585584685_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585584685_p1555mcpsimp"><a name="zh-cn_topic_0000002585584685_p1555mcpsimp"></a><a name="zh-cn_topic_0000002585584685_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585584685_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585584685_p318745519118"><a name="zh-cn_topic_0000002585584685_p318745519118"></a><a name="zh-cn_topic_0000002585584685_p318745519118"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585584685_p38462723"><a name="zh-cn_topic_0000002585584685_p38462723"></a><a name="zh-cn_topic_0000002585584685_p38462723"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585584685_p1562mcpsimp"><a name="zh-cn_topic_0000002585584685_p1562mcpsimp"></a><a name="zh-cn_topic_0000002585584685_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585584685_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585584685_p11853124611587"><a name="zh-cn_topic_0000002585584685_p11853124611587"></a><a name="zh-cn_topic_0000002585584685_p11853124611587"></a>hfp_role</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585584685_p1560mcpsimp"><a name="zh-cn_topic_0000002585584685_p1560mcpsimp"></a><a name="zh-cn_topic_0000002585584685_p1560mcpsimp"></a>服务角色。详见“<a href="#ZH-CN_TOPIC_0000002572623896">bs_br_call_hfp_role_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585584685_p1569mcpsimp"><a name="zh-cn_topic_0000002585584685_p1569mcpsimp"></a><a name="zh-cn_topic_0000002585584685_p1569mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585584685_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585584685_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585584685_p1586mcpsimp"><a name="zh-cn_topic_0000002585584685_p1586mcpsimp"></a><a name="zh-cn_topic_0000002585584685_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585584685_p1588mcpsimp"><a name="zh-cn_topic_0000002585584685_p1588mcpsimp"></a><a name="zh-cn_topic_0000002585584685_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585584685_p1590mcpsimp"><a name="zh-cn_topic_0000002585584685_p1590mcpsimp"></a><a name="zh-cn_topic_0000002585584685_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585584685_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585584685_p1593mcpsimp"><a name="zh-cn_topic_0000002585584685_p1593mcpsimp"></a><a name="zh-cn_topic_0000002585584685_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585584685_p1595mcpsimp"><a name="zh-cn_topic_0000002585584685_p1595mcpsimp"></a><a name="zh-cn_topic_0000002585584685_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585584685_p1596mcpsimp"><a name="zh-cn_topic_0000002585584685_p1596mcpsimp"></a><a name="zh-cn_topic_0000002585584685_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585584685_p1598mcpsimp"><a name="zh-cn_topic_0000002585584685_p1598mcpsimp"></a><a name="zh-cn_topic_0000002585584685_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_call\_hfp\_disconnect<a name="ZH-CN_TOPIC_0000002572783574"></a>

【描述】

BR/EDR断开蓝牙通话服务连接。

【接口定义】

```
errcode_t bs_br_call_hfp_disconnect(uint16_t dev_id, uint8_t hfp_role);
```

【参数】

<a name="zh-cn_topic_0000002555144736_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555144736_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555144736_p1551mcpsimp"><a name="zh-cn_topic_0000002555144736_p1551mcpsimp"></a><a name="zh-cn_topic_0000002555144736_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555144736_p1553mcpsimp"><a name="zh-cn_topic_0000002555144736_p1553mcpsimp"></a><a name="zh-cn_topic_0000002555144736_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555144736_p1555mcpsimp"><a name="zh-cn_topic_0000002555144736_p1555mcpsimp"></a><a name="zh-cn_topic_0000002555144736_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555144736_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555144736_p318745519118"><a name="zh-cn_topic_0000002555144736_p318745519118"></a><a name="zh-cn_topic_0000002555144736_p318745519118"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555144736_p38462723"><a name="zh-cn_topic_0000002555144736_p38462723"></a><a name="zh-cn_topic_0000002555144736_p38462723"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555144736_p1562mcpsimp"><a name="zh-cn_topic_0000002555144736_p1562mcpsimp"></a><a name="zh-cn_topic_0000002555144736_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002555144736_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555144736_p11853124611587"><a name="zh-cn_topic_0000002555144736_p11853124611587"></a><a name="zh-cn_topic_0000002555144736_p11853124611587"></a>hfp_role</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555144736_p1560mcpsimp"><a name="zh-cn_topic_0000002555144736_p1560mcpsimp"></a><a name="zh-cn_topic_0000002555144736_p1560mcpsimp"></a>服务角色。详见“<a href="#ZH-CN_TOPIC_0000002572623896">bs_br_call_hfp_role_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555144736_p1569mcpsimp"><a name="zh-cn_topic_0000002555144736_p1569mcpsimp"></a><a name="zh-cn_topic_0000002555144736_p1569mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555144736_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555144736_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555144736_p1586mcpsimp"><a name="zh-cn_topic_0000002555144736_p1586mcpsimp"></a><a name="zh-cn_topic_0000002555144736_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555144736_p1588mcpsimp"><a name="zh-cn_topic_0000002555144736_p1588mcpsimp"></a><a name="zh-cn_topic_0000002555144736_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555144736_p1590mcpsimp"><a name="zh-cn_topic_0000002555144736_p1590mcpsimp"></a><a name="zh-cn_topic_0000002555144736_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555144736_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555144736_p1593mcpsimp"><a name="zh-cn_topic_0000002555144736_p1593mcpsimp"></a><a name="zh-cn_topic_0000002555144736_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555144736_p1595mcpsimp"><a name="zh-cn_topic_0000002555144736_p1595mcpsimp"></a><a name="zh-cn_topic_0000002555144736_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555144736_p1596mcpsimp"><a name="zh-cn_topic_0000002555144736_p1596mcpsimp"></a><a name="zh-cn_topic_0000002555144736_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555144736_p1598mcpsimp"><a name="zh-cn_topic_0000002555144736_p1598mcpsimp"></a><a name="zh-cn_topic_0000002555144736_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_call\_operate\_request<a name="ZH-CN_TOPIC_0000002603222973"></a>

【描述】

BR/EDR SPP设置客户端固定port。

【接口定义】

```
errcode_t bs_br_call_operate_request(uint16_t dev_id, uint8_t opt_type, uint8_t *param, uint16_t param_len);
```

【参数】

<a name="zh-cn_topic_0000002554985106_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002554985106_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002554985106_p1551mcpsimp"><a name="zh-cn_topic_0000002554985106_p1551mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002554985106_p1553mcpsimp"><a name="zh-cn_topic_0000002554985106_p1553mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002554985106_p1555mcpsimp"><a name="zh-cn_topic_0000002554985106_p1555mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002554985106_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985106_p18901457734"><a name="zh-cn_topic_0000002554985106_p18901457734"></a><a name="zh-cn_topic_0000002554985106_p18901457734"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985106_p38462723"><a name="zh-cn_topic_0000002554985106_p38462723"></a><a name="zh-cn_topic_0000002554985106_p38462723"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985106_p1562mcpsimp"><a name="zh-cn_topic_0000002554985106_p1562mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002554985106_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985106_p1565mcpsimp"><a name="zh-cn_topic_0000002554985106_p1565mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1565mcpsimp"></a>opt_type</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985106_p1567mcpsimp"><a name="zh-cn_topic_0000002554985106_p1567mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1567mcpsimp"></a>通话操作类型。详见“<a href="#ZH-CN_TOPIC_0000002572783566">bs_br_call_opt_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985106_p1569mcpsimp"><a name="zh-cn_topic_0000002554985106_p1569mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1569mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002554985106_row1570mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985106_p951751748"><a name="zh-cn_topic_0000002554985106_p951751748"></a><a name="zh-cn_topic_0000002554985106_p951751748"></a>param</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985106_p1574mcpsimp"><a name="zh-cn_topic_0000002554985106_p1574mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1574mcpsimp"></a>通话操作参数。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985106_p1576mcpsimp"><a name="zh-cn_topic_0000002554985106_p1576mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1576mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002554985106_row16444677420"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985106_p14445376411"><a name="zh-cn_topic_0000002554985106_p14445376411"></a><a name="zh-cn_topic_0000002554985106_p14445376411"></a>param_len</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985106_p24451571414"><a name="zh-cn_topic_0000002554985106_p24451571414"></a><a name="zh-cn_topic_0000002554985106_p24451571414"></a>通话操作参数长度。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985106_p1644577745"><a name="zh-cn_topic_0000002554985106_p1644577745"></a><a name="zh-cn_topic_0000002554985106_p1644577745"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002554985106_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002554985106_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002554985106_p1586mcpsimp"><a name="zh-cn_topic_0000002554985106_p1586mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002554985106_p1588mcpsimp"><a name="zh-cn_topic_0000002554985106_p1588mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002554985106_p1590mcpsimp"><a name="zh-cn_topic_0000002554985106_p1590mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002554985106_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985106_p1593mcpsimp"><a name="zh-cn_topic_0000002554985106_p1593mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985106_p1595mcpsimp"><a name="zh-cn_topic_0000002554985106_p1595mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002554985106_p1596mcpsimp"><a name="zh-cn_topic_0000002554985106_p1596mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985106_p1598mcpsimp"><a name="zh-cn_topic_0000002554985106_p1598mcpsimp"></a><a name="zh-cn_topic_0000002554985106_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_call\_voice\_recognition\_request<a name="ZH-CN_TOPIC_0000002603342957"></a>

【描述】

BR/EDR语音识别请求。

【接口定义】

```
errcode_t bs_br_call_voice_recognition_request(uint16_t dev_id, bool is_open);
```

【参数】

<a name="zh-cn_topic_0000002585704731_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585704731_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585704731_p1551mcpsimp"><a name="zh-cn_topic_0000002585704731_p1551mcpsimp"></a><a name="zh-cn_topic_0000002585704731_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585704731_p1553mcpsimp"><a name="zh-cn_topic_0000002585704731_p1553mcpsimp"></a><a name="zh-cn_topic_0000002585704731_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585704731_p1555mcpsimp"><a name="zh-cn_topic_0000002585704731_p1555mcpsimp"></a><a name="zh-cn_topic_0000002585704731_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585704731_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704731_p1573433012510"><a name="zh-cn_topic_0000002585704731_p1573433012510"></a><a name="zh-cn_topic_0000002585704731_p1573433012510"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704731_p38462723"><a name="zh-cn_topic_0000002585704731_p38462723"></a><a name="zh-cn_topic_0000002585704731_p38462723"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704731_p1562mcpsimp"><a name="zh-cn_topic_0000002585704731_p1562mcpsimp"></a><a name="zh-cn_topic_0000002585704731_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585704731_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704731_p1565mcpsimp"><a name="zh-cn_topic_0000002585704731_p1565mcpsimp"></a><a name="zh-cn_topic_0000002585704731_p1565mcpsimp"></a>is_open</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704731_p1567mcpsimp"><a name="zh-cn_topic_0000002585704731_p1567mcpsimp"></a><a name="zh-cn_topic_0000002585704731_p1567mcpsimp"></a>是否开启。true：打开，false：关闭。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704731_p1569mcpsimp"><a name="zh-cn_topic_0000002585704731_p1569mcpsimp"></a><a name="zh-cn_topic_0000002585704731_p1569mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585704731_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585704731_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585704731_p1586mcpsimp"><a name="zh-cn_topic_0000002585704731_p1586mcpsimp"></a><a name="zh-cn_topic_0000002585704731_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585704731_p1588mcpsimp"><a name="zh-cn_topic_0000002585704731_p1588mcpsimp"></a><a name="zh-cn_topic_0000002585704731_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585704731_p1590mcpsimp"><a name="zh-cn_topic_0000002585704731_p1590mcpsimp"></a><a name="zh-cn_topic_0000002585704731_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585704731_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704731_p1593mcpsimp"><a name="zh-cn_topic_0000002585704731_p1593mcpsimp"></a><a name="zh-cn_topic_0000002585704731_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704731_p1595mcpsimp"><a name="zh-cn_topic_0000002585704731_p1595mcpsimp"></a><a name="zh-cn_topic_0000002585704731_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585704731_p1596mcpsimp"><a name="zh-cn_topic_0000002585704731_p1596mcpsimp"></a><a name="zh-cn_topic_0000002585704731_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704731_p1598mcpsimp"><a name="zh-cn_topic_0000002585704731_p1598mcpsimp"></a><a name="zh-cn_topic_0000002585704731_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_call\_set\_volume\_request<a name="ZH-CN_TOPIC_0000002603222999"></a>

【描述】

BR/EDR通话音量配置请求。

【接口定义】

```
errcode_t bs_br_call_set_volume_request(uint16_t dev_id, uint8_t volume_type, uint8_t volume);
```

【参数】

<a name="zh-cn_topic_0000002555003448_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555003448_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555003448_p1551mcpsimp"><a name="zh-cn_topic_0000002555003448_p1551mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555003448_p1553mcpsimp"><a name="zh-cn_topic_0000002555003448_p1553mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555003448_p1555mcpsimp"><a name="zh-cn_topic_0000002555003448_p1555mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555003448_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555003448_p1573433012510"><a name="zh-cn_topic_0000002555003448_p1573433012510"></a><a name="zh-cn_topic_0000002555003448_p1573433012510"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555003448_p38462723"><a name="zh-cn_topic_0000002555003448_p38462723"></a><a name="zh-cn_topic_0000002555003448_p38462723"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555003448_p1562mcpsimp"><a name="zh-cn_topic_0000002555003448_p1562mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002555003448_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555003448_p1565mcpsimp"><a name="zh-cn_topic_0000002555003448_p1565mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1565mcpsimp"></a>volume_type</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555003448_p1567mcpsimp"><a name="zh-cn_topic_0000002555003448_p1567mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1567mcpsimp"></a>音量类型。详见“<a href="#ZH-CN_TOPIC_0000002572783566">bs_br_call_opt_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555003448_p1569mcpsimp"><a name="zh-cn_topic_0000002555003448_p1569mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1569mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002555003448_row1570mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555003448_p1572mcpsimp"><a name="zh-cn_topic_0000002555003448_p1572mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1572mcpsimp"></a>volume</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555003448_p1574mcpsimp"><a name="zh-cn_topic_0000002555003448_p1574mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1574mcpsimp"></a>音量值。取值范围为0～15的整数。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555003448_p1576mcpsimp"><a name="zh-cn_topic_0000002555003448_p1576mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1576mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555003448_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555003448_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555003448_p1586mcpsimp"><a name="zh-cn_topic_0000002555003448_p1586mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555003448_p1588mcpsimp"><a name="zh-cn_topic_0000002555003448_p1588mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555003448_p1590mcpsimp"><a name="zh-cn_topic_0000002555003448_p1590mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555003448_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555003448_p1593mcpsimp"><a name="zh-cn_topic_0000002555003448_p1593mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555003448_p1595mcpsimp"><a name="zh-cn_topic_0000002555003448_p1595mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555003448_p1596mcpsimp"><a name="zh-cn_topic_0000002555003448_p1596mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555003448_p1598mcpsimp"><a name="zh-cn_topic_0000002555003448_p1598mcpsimp"></a><a name="zh-cn_topic_0000002555003448_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_call\_send\_dtmf\_tone\_request<a name="ZH-CN_TOPIC_0000002572623878"></a>

【描述】

BR/EDR发送DTMF识别码请求。

【接口定义】

```
errcode_t bs_br_call_send_dtmf_tone_request(uint16_t dev_id, uint8_t code);
```

【参数】

<a name="zh-cn_topic_0000002585723067_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585723067_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585723067_p1551mcpsimp"><a name="zh-cn_topic_0000002585723067_p1551mcpsimp"></a><a name="zh-cn_topic_0000002585723067_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585723067_p1553mcpsimp"><a name="zh-cn_topic_0000002585723067_p1553mcpsimp"></a><a name="zh-cn_topic_0000002585723067_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585723067_p1555mcpsimp"><a name="zh-cn_topic_0000002585723067_p1555mcpsimp"></a><a name="zh-cn_topic_0000002585723067_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585723067_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585723067_p1573433012510"><a name="zh-cn_topic_0000002585723067_p1573433012510"></a><a name="zh-cn_topic_0000002585723067_p1573433012510"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585723067_p38462723"><a name="zh-cn_topic_0000002585723067_p38462723"></a><a name="zh-cn_topic_0000002585723067_p38462723"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585723067_p1562mcpsimp"><a name="zh-cn_topic_0000002585723067_p1562mcpsimp"></a><a name="zh-cn_topic_0000002585723067_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585723067_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585723067_p1565mcpsimp"><a name="zh-cn_topic_0000002585723067_p1565mcpsimp"></a><a name="zh-cn_topic_0000002585723067_p1565mcpsimp"></a>code</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585723067_p1567mcpsimp"><a name="zh-cn_topic_0000002585723067_p1567mcpsimp"></a><a name="zh-cn_topic_0000002585723067_p1567mcpsimp"></a>DTMF识别码。值为字符“0”到“9”、“A”到“D”、“*”、“#”的ASCII码。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585723067_p1569mcpsimp"><a name="zh-cn_topic_0000002585723067_p1569mcpsimp"></a><a name="zh-cn_topic_0000002585723067_p1569mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585723067_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585723067_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585723067_p1586mcpsimp"><a name="zh-cn_topic_0000002585723067_p1586mcpsimp"></a><a name="zh-cn_topic_0000002585723067_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585723067_p1588mcpsimp"><a name="zh-cn_topic_0000002585723067_p1588mcpsimp"></a><a name="zh-cn_topic_0000002585723067_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585723067_p1590mcpsimp"><a name="zh-cn_topic_0000002585723067_p1590mcpsimp"></a><a name="zh-cn_topic_0000002585723067_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585723067_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585723067_p1593mcpsimp"><a name="zh-cn_topic_0000002585723067_p1593mcpsimp"></a><a name="zh-cn_topic_0000002585723067_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585723067_p1595mcpsimp"><a name="zh-cn_topic_0000002585723067_p1595mcpsimp"></a><a name="zh-cn_topic_0000002585723067_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585723067_p1596mcpsimp"><a name="zh-cn_topic_0000002585723067_p1596mcpsimp"></a><a name="zh-cn_topic_0000002585723067_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585723067_p1598mcpsimp"><a name="zh-cn_topic_0000002585723067_p1598mcpsimp"></a><a name="zh-cn_topic_0000002585723067_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_call\_status\_notify<a name="ZH-CN_TOPIC_0000002572783556"></a>

【描述】

BR/EDR通话操作请求。

【接口定义】

```
errcode_t bs_br_call_status_notify(uint16_t dev_id, uint8_t notify_type, uint8_t *param, uint16_t param_len);
```

【参数】

<a name="zh-cn_topic_0000002585603025_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585603025_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585603025_p1551mcpsimp"><a name="zh-cn_topic_0000002585603025_p1551mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585603025_p1553mcpsimp"><a name="zh-cn_topic_0000002585603025_p1553mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585603025_p1555mcpsimp"><a name="zh-cn_topic_0000002585603025_p1555mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585603025_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585603025_p1558mcpsimp"><a name="zh-cn_topic_0000002585603025_p1558mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1558mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585603025_p1560mcpsimp"><a name="zh-cn_topic_0000002585603025_p1560mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1560mcpsimp"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585603025_p1562mcpsimp"><a name="zh-cn_topic_0000002585603025_p1562mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585603025_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585603025_p1565mcpsimp"><a name="zh-cn_topic_0000002585603025_p1565mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1565mcpsimp"></a>notify_type</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585603025_p1567mcpsimp"><a name="zh-cn_topic_0000002585603025_p1567mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1567mcpsimp"></a>通知类型。详见“<a href="#ZH-CN_TOPIC_0000002572623866">bs_br_call_notify_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585603025_p1569mcpsimp"><a name="zh-cn_topic_0000002585603025_p1569mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1569mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585603025_row1570mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585603025_p1572mcpsimp"><a name="zh-cn_topic_0000002585603025_p1572mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1572mcpsimp"></a>param</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585603025_p1574mcpsimp"><a name="zh-cn_topic_0000002585603025_p1574mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1574mcpsimp"></a>通知参数。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585603025_p1576mcpsimp"><a name="zh-cn_topic_0000002585603025_p1576mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1576mcpsimp"></a>输出</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585603025_row3661767427"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585603025_p76623634217"><a name="zh-cn_topic_0000002585603025_p76623634217"></a><a name="zh-cn_topic_0000002585603025_p76623634217"></a>param_len</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585603025_p1766212644215"><a name="zh-cn_topic_0000002585603025_p1766212644215"></a><a name="zh-cn_topic_0000002585603025_p1766212644215"></a>通知参数长度。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585603025_p1234265215434"><a name="zh-cn_topic_0000002585603025_p1234265215434"></a><a name="zh-cn_topic_0000002585603025_p1234265215434"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585603025_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585603025_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585603025_p1586mcpsimp"><a name="zh-cn_topic_0000002585603025_p1586mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585603025_p1588mcpsimp"><a name="zh-cn_topic_0000002585603025_p1588mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585603025_p1590mcpsimp"><a name="zh-cn_topic_0000002585603025_p1590mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585603025_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585603025_p1593mcpsimp"><a name="zh-cn_topic_0000002585603025_p1593mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585603025_p1595mcpsimp"><a name="zh-cn_topic_0000002585603025_p1595mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585603025_p1596mcpsimp"><a name="zh-cn_topic_0000002585603025_p1596mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585603025_p1598mcpsimp"><a name="zh-cn_topic_0000002585603025_p1598mcpsimp"></a><a name="zh-cn_topic_0000002585603025_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_call\_send\_vendor\_at\_request<a name="ZH-CN_TOPIC_0000002603342997"></a>

【描述】

BR/EDR发送私有命令请求。

【接口定义】

```
errcode_t bs_br_call_send_vendor_at_request(uint16_t dev_id, uint8_t *at_cmd, uint16_t at_len);
```

【参数】

<a name="zh-cn_topic_0000002555163088_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555163088_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555163088_p1551mcpsimp"><a name="zh-cn_topic_0000002555163088_p1551mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555163088_p1553mcpsimp"><a name="zh-cn_topic_0000002555163088_p1553mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555163088_p1555mcpsimp"><a name="zh-cn_topic_0000002555163088_p1555mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555163088_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555163088_p1558mcpsimp"><a name="zh-cn_topic_0000002555163088_p1558mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1558mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555163088_p1560mcpsimp"><a name="zh-cn_topic_0000002555163088_p1560mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1560mcpsimp"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555163088_p1562mcpsimp"><a name="zh-cn_topic_0000002555163088_p1562mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002555163088_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555163088_p194095132449"><a name="zh-cn_topic_0000002555163088_p194095132449"></a><a name="zh-cn_topic_0000002555163088_p194095132449"></a>at_cmd</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555163088_p1567mcpsimp"><a name="zh-cn_topic_0000002555163088_p1567mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1567mcpsimp"></a>AT命令参数。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555163088_p1569mcpsimp"><a name="zh-cn_topic_0000002555163088_p1569mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1569mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002555163088_row1570mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555163088_p3970143084419"><a name="zh-cn_topic_0000002555163088_p3970143084419"></a><a name="zh-cn_topic_0000002555163088_p3970143084419"></a>at_len</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555163088_p12705192784418"><a name="zh-cn_topic_0000002555163088_p12705192784418"></a><a name="zh-cn_topic_0000002555163088_p12705192784418"></a>AT命令参数长度。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555163088_p1576mcpsimp"><a name="zh-cn_topic_0000002555163088_p1576mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1576mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555163088_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555163088_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555163088_p1586mcpsimp"><a name="zh-cn_topic_0000002555163088_p1586mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555163088_p1588mcpsimp"><a name="zh-cn_topic_0000002555163088_p1588mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555163088_p1590mcpsimp"><a name="zh-cn_topic_0000002555163088_p1590mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555163088_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555163088_p1593mcpsimp"><a name="zh-cn_topic_0000002555163088_p1593mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555163088_p1595mcpsimp"><a name="zh-cn_topic_0000002555163088_p1595mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555163088_p1596mcpsimp"><a name="zh-cn_topic_0000002555163088_p1596mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555163088_p1598mcpsimp"><a name="zh-cn_topic_0000002555163088_p1598mcpsimp"></a><a name="zh-cn_topic_0000002555163088_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_call\_register\_callbacks<a name="ZH-CN_TOPIC_0000002603223069"></a>

【描述】

BR/EDR注册蓝牙通话回调函数。

【接口定义】

```
errcode_t bs_br_call_register_callbacks(uint8_t app_id, bs_br_call_callbacks_t *func);
```

【参数】

<a name="zh-cn_topic_0000002555003450_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555003450_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555003450_p1551mcpsimp"><a name="zh-cn_topic_0000002555003450_p1551mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555003450_p1553mcpsimp"><a name="zh-cn_topic_0000002555003450_p1553mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555003450_p1555mcpsimp"><a name="zh-cn_topic_0000002555003450_p1555mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555003450_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555003450_p1558mcpsimp"><a name="zh-cn_topic_0000002555003450_p1558mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1558mcpsimp"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555003450_p1560mcpsimp"><a name="zh-cn_topic_0000002555003450_p1560mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1560mcpsimp"></a>应用ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555003450_p1562mcpsimp"><a name="zh-cn_topic_0000002555003450_p1562mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002555003450_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555003450_p1565mcpsimp"><a name="zh-cn_topic_0000002555003450_p1565mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1565mcpsimp"></a>func</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555003450_p1567mcpsimp"><a name="zh-cn_topic_0000002555003450_p1567mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1567mcpsimp"></a>回调函数。支持的回调函数详见“<a href="#ZH-CN_TOPIC_0000002603342973">bs_br_call_callbacks_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555003450_p1569mcpsimp"><a name="zh-cn_topic_0000002555003450_p1569mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1569mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555003450_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555003450_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555003450_p1586mcpsimp"><a name="zh-cn_topic_0000002555003450_p1586mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555003450_p1588mcpsimp"><a name="zh-cn_topic_0000002555003450_p1588mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555003450_p1590mcpsimp"><a name="zh-cn_topic_0000002555003450_p1590mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555003450_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555003450_p1593mcpsimp"><a name="zh-cn_topic_0000002555003450_p1593mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555003450_p1595mcpsimp"><a name="zh-cn_topic_0000002555003450_p1595mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555003450_p1596mcpsimp"><a name="zh-cn_topic_0000002555003450_p1596mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555003450_p1598mcpsimp"><a name="zh-cn_topic_0000002555003450_p1598mcpsimp"></a><a name="zh-cn_topic_0000002555003450_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_call\_unregister\_callbacks<a name="ZH-CN_TOPIC_0000002572783520"></a>

【描述】

BR/EDR去注册蓝牙通话回调函数。

【接口定义】

```
errcode_t bs_br_call_unregister_callbacks(uint8_t app_id);
```

【参数】

<a name="zh-cn_topic_0000002585723069_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585723069_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585723069_p1551mcpsimp"><a name="zh-cn_topic_0000002585723069_p1551mcpsimp"></a><a name="zh-cn_topic_0000002585723069_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585723069_p1553mcpsimp"><a name="zh-cn_topic_0000002585723069_p1553mcpsimp"></a><a name="zh-cn_topic_0000002585723069_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585723069_p1555mcpsimp"><a name="zh-cn_topic_0000002585723069_p1555mcpsimp"></a><a name="zh-cn_topic_0000002585723069_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585723069_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585723069_p1558mcpsimp"><a name="zh-cn_topic_0000002585723069_p1558mcpsimp"></a><a name="zh-cn_topic_0000002585723069_p1558mcpsimp"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585723069_p1560mcpsimp"><a name="zh-cn_topic_0000002585723069_p1560mcpsimp"></a><a name="zh-cn_topic_0000002585723069_p1560mcpsimp"></a>应用ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585723069_p1562mcpsimp"><a name="zh-cn_topic_0000002585723069_p1562mcpsimp"></a><a name="zh-cn_topic_0000002585723069_p1562mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585723069_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585723069_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585723069_p1586mcpsimp"><a name="zh-cn_topic_0000002585723069_p1586mcpsimp"></a><a name="zh-cn_topic_0000002585723069_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585723069_p1588mcpsimp"><a name="zh-cn_topic_0000002585723069_p1588mcpsimp"></a><a name="zh-cn_topic_0000002585723069_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585723069_p1590mcpsimp"><a name="zh-cn_topic_0000002585723069_p1590mcpsimp"></a><a name="zh-cn_topic_0000002585723069_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585723069_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585723069_p1593mcpsimp"><a name="zh-cn_topic_0000002585723069_p1593mcpsimp"></a><a name="zh-cn_topic_0000002585723069_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585723069_p1595mcpsimp"><a name="zh-cn_topic_0000002585723069_p1595mcpsimp"></a><a name="zh-cn_topic_0000002585723069_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585723069_p1596mcpsimp"><a name="zh-cn_topic_0000002585723069_p1596mcpsimp"></a><a name="zh-cn_topic_0000002585723069_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585723069_p1598mcpsimp"><a name="zh-cn_topic_0000002585723069_p1598mcpsimp"></a><a name="zh-cn_topic_0000002585723069_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_create\_sco\_connection<a name="ZH-CN_TOPIC_0000002572623928"></a>

【描述】

BR/EDR建立SCO音频流连接。

【接口定义】

```
errcode_t bs_br_create_sco_connection(uint16_t dev_id);
```

【参数】

<a name="zh-cn_topic_0000002585603027_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585603027_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585603027_p1551mcpsimp"><a name="zh-cn_topic_0000002585603027_p1551mcpsimp"></a><a name="zh-cn_topic_0000002585603027_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585603027_p1553mcpsimp"><a name="zh-cn_topic_0000002585603027_p1553mcpsimp"></a><a name="zh-cn_topic_0000002585603027_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585603027_p1555mcpsimp"><a name="zh-cn_topic_0000002585603027_p1555mcpsimp"></a><a name="zh-cn_topic_0000002585603027_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585603027_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585603027_p1558mcpsimp"><a name="zh-cn_topic_0000002585603027_p1558mcpsimp"></a><a name="zh-cn_topic_0000002585603027_p1558mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585603027_p1560mcpsimp"><a name="zh-cn_topic_0000002585603027_p1560mcpsimp"></a><a name="zh-cn_topic_0000002585603027_p1560mcpsimp"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585603027_p1562mcpsimp"><a name="zh-cn_topic_0000002585603027_p1562mcpsimp"></a><a name="zh-cn_topic_0000002585603027_p1562mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585603027_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585603027_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585603027_p1586mcpsimp"><a name="zh-cn_topic_0000002585603027_p1586mcpsimp"></a><a name="zh-cn_topic_0000002585603027_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585603027_p1588mcpsimp"><a name="zh-cn_topic_0000002585603027_p1588mcpsimp"></a><a name="zh-cn_topic_0000002585603027_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585603027_p1590mcpsimp"><a name="zh-cn_topic_0000002585603027_p1590mcpsimp"></a><a name="zh-cn_topic_0000002585603027_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585603027_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585603027_p1593mcpsimp"><a name="zh-cn_topic_0000002585603027_p1593mcpsimp"></a><a name="zh-cn_topic_0000002585603027_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585603027_p1595mcpsimp"><a name="zh-cn_topic_0000002585603027_p1595mcpsimp"></a><a name="zh-cn_topic_0000002585603027_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585603027_p1596mcpsimp"><a name="zh-cn_topic_0000002585603027_p1596mcpsimp"></a><a name="zh-cn_topic_0000002585603027_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585603027_p1598mcpsimp"><a name="zh-cn_topic_0000002585603027_p1598mcpsimp"></a><a name="zh-cn_topic_0000002585603027_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_release\_sco\_connection<a name="ZH-CN_TOPIC_0000002572783560"></a>

【描述】

BR/EDR断开SCO音频流连接。

【接口定义】

```
errcode_t bs_br_release_sco_connection(uint16_t dev_id);
```

【参数】

<a name="zh-cn_topic_0000002555163090_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555163090_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555163090_p1551mcpsimp"><a name="zh-cn_topic_0000002555163090_p1551mcpsimp"></a><a name="zh-cn_topic_0000002555163090_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555163090_p1553mcpsimp"><a name="zh-cn_topic_0000002555163090_p1553mcpsimp"></a><a name="zh-cn_topic_0000002555163090_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555163090_p1555mcpsimp"><a name="zh-cn_topic_0000002555163090_p1555mcpsimp"></a><a name="zh-cn_topic_0000002555163090_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555163090_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555163090_p1558mcpsimp"><a name="zh-cn_topic_0000002555163090_p1558mcpsimp"></a><a name="zh-cn_topic_0000002555163090_p1558mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555163090_p1560mcpsimp"><a name="zh-cn_topic_0000002555163090_p1560mcpsimp"></a><a name="zh-cn_topic_0000002555163090_p1560mcpsimp"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555163090_p1562mcpsimp"><a name="zh-cn_topic_0000002555163090_p1562mcpsimp"></a><a name="zh-cn_topic_0000002555163090_p1562mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555163090_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555163090_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555163090_p1586mcpsimp"><a name="zh-cn_topic_0000002555163090_p1586mcpsimp"></a><a name="zh-cn_topic_0000002555163090_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555163090_p1588mcpsimp"><a name="zh-cn_topic_0000002555163090_p1588mcpsimp"></a><a name="zh-cn_topic_0000002555163090_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555163090_p1590mcpsimp"><a name="zh-cn_topic_0000002555163090_p1590mcpsimp"></a><a name="zh-cn_topic_0000002555163090_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555163090_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555163090_p1593mcpsimp"><a name="zh-cn_topic_0000002555163090_p1593mcpsimp"></a><a name="zh-cn_topic_0000002555163090_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555163090_p1595mcpsimp"><a name="zh-cn_topic_0000002555163090_p1595mcpsimp"></a><a name="zh-cn_topic_0000002555163090_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555163090_p1596mcpsimp"><a name="zh-cn_topic_0000002555163090_p1596mcpsimp"></a><a name="zh-cn_topic_0000002555163090_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555163090_p1598mcpsimp"><a name="zh-cn_topic_0000002555163090_p1598mcpsimp"></a><a name="zh-cn_topic_0000002555163090_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_create\_sco\_result\_reply<a name="ZH-CN_TOPIC_0000002603343017"></a>

【描述】

BR/EDR接受或拒绝SCO连接。

【接口定义】

```
errcode_t bs_br_create_sco_result_reply(uint16_t dev_id, bool is_accept);
```

【参数】

<a name="zh-cn_topic_0000002555003456_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555003456_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555003456_p1551mcpsimp"><a name="zh-cn_topic_0000002555003456_p1551mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555003456_p1553mcpsimp"><a name="zh-cn_topic_0000002555003456_p1553mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555003456_p1555mcpsimp"><a name="zh-cn_topic_0000002555003456_p1555mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555003456_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555003456_p1558mcpsimp"><a name="zh-cn_topic_0000002555003456_p1558mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1558mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555003456_p1560mcpsimp"><a name="zh-cn_topic_0000002555003456_p1560mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1560mcpsimp"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555003456_p1562mcpsimp"><a name="zh-cn_topic_0000002555003456_p1562mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002555003456_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555003456_p1565mcpsimp"><a name="zh-cn_topic_0000002555003456_p1565mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1565mcpsimp"></a>is_accept</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555003456_p1567mcpsimp"><a name="zh-cn_topic_0000002555003456_p1567mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1567mcpsimp"></a>是否接受连接。true：接受，false：拒绝。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555003456_p1569mcpsimp"><a name="zh-cn_topic_0000002555003456_p1569mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1569mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555003456_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555003456_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555003456_p1586mcpsimp"><a name="zh-cn_topic_0000002555003456_p1586mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555003456_p1588mcpsimp"><a name="zh-cn_topic_0000002555003456_p1588mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555003456_p1590mcpsimp"><a name="zh-cn_topic_0000002555003456_p1590mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555003456_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555003456_p1593mcpsimp"><a name="zh-cn_topic_0000002555003456_p1593mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555003456_p1595mcpsimp"><a name="zh-cn_topic_0000002555003456_p1595mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555003456_p1596mcpsimp"><a name="zh-cn_topic_0000002555003456_p1596mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555003456_p1598mcpsimp"><a name="zh-cn_topic_0000002555003456_p1598mcpsimp"></a><a name="zh-cn_topic_0000002555003456_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_sco\_register\_callbacks<a name="ZH-CN_TOPIC_0000002572783522"></a>

【描述】

BR/EDR注册SCO回调函数。

【接口定义】

```
errcode_t bs_br_sco_register_callbacks(uint8_t app_id, bs_br_sco_callbacks_t *func);
```

【参数】

<a name="zh-cn_topic_0000002585723075_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585723075_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585723075_p1551mcpsimp"><a name="zh-cn_topic_0000002585723075_p1551mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585723075_p1553mcpsimp"><a name="zh-cn_topic_0000002585723075_p1553mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585723075_p1555mcpsimp"><a name="zh-cn_topic_0000002585723075_p1555mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585723075_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585723075_p1558mcpsimp"><a name="zh-cn_topic_0000002585723075_p1558mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1558mcpsimp"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585723075_p1560mcpsimp"><a name="zh-cn_topic_0000002585723075_p1560mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1560mcpsimp"></a>应用ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585723075_p1562mcpsimp"><a name="zh-cn_topic_0000002585723075_p1562mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1562mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585723075_row1563mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585723075_p1565mcpsimp"><a name="zh-cn_topic_0000002585723075_p1565mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1565mcpsimp"></a>func</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585723075_p1567mcpsimp"><a name="zh-cn_topic_0000002585723075_p1567mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1567mcpsimp"></a>回调函数。支持的回调函数详见“<a href="#ZH-CN_TOPIC_0000002603343011">bs_br_sco_callbacks_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585723075_p1569mcpsimp"><a name="zh-cn_topic_0000002585723075_p1569mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1569mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585723075_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585723075_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585723075_p1586mcpsimp"><a name="zh-cn_topic_0000002585723075_p1586mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585723075_p1588mcpsimp"><a name="zh-cn_topic_0000002585723075_p1588mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585723075_p1590mcpsimp"><a name="zh-cn_topic_0000002585723075_p1590mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585723075_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585723075_p1593mcpsimp"><a name="zh-cn_topic_0000002585723075_p1593mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585723075_p1595mcpsimp"><a name="zh-cn_topic_0000002585723075_p1595mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585723075_p1596mcpsimp"><a name="zh-cn_topic_0000002585723075_p1596mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585723075_p1598mcpsimp"><a name="zh-cn_topic_0000002585723075_p1598mcpsimp"></a><a name="zh-cn_topic_0000002585723075_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_sco\_unregister\_callbacks<a name="ZH-CN_TOPIC_0000002603343009"></a>

【描述】

BR/EDR去注册SCO回调函数。

【接口定义】

```
errcode_t bs_br_sco_unregister_callbacks(uint8_t app_id);
```

【参数】

<a name="zh-cn_topic_0000002585603033_table1543mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585603033_row1549mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585603033_p1551mcpsimp"><a name="zh-cn_topic_0000002585603033_p1551mcpsimp"></a><a name="zh-cn_topic_0000002585603033_p1551mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.72%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585603033_p1553mcpsimp"><a name="zh-cn_topic_0000002585603033_p1553mcpsimp"></a><a name="zh-cn_topic_0000002585603033_p1553mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.279999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585603033_p1555mcpsimp"><a name="zh-cn_topic_0000002585603033_p1555mcpsimp"></a><a name="zh-cn_topic_0000002585603033_p1555mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585603033_row1556mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585603033_p1558mcpsimp"><a name="zh-cn_topic_0000002585603033_p1558mcpsimp"></a><a name="zh-cn_topic_0000002585603033_p1558mcpsimp"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.72%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585603033_p1560mcpsimp"><a name="zh-cn_topic_0000002585603033_p1560mcpsimp"></a><a name="zh-cn_topic_0000002585603033_p1560mcpsimp"></a>应用ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.279999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585603033_p1562mcpsimp"><a name="zh-cn_topic_0000002585603033_p1562mcpsimp"></a><a name="zh-cn_topic_0000002585603033_p1562mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585603033_table1578mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585603033_row1584mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585603033_p1586mcpsimp"><a name="zh-cn_topic_0000002585603033_p1586mcpsimp"></a><a name="zh-cn_topic_0000002585603033_p1586mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585603033_p1588mcpsimp"><a name="zh-cn_topic_0000002585603033_p1588mcpsimp"></a><a name="zh-cn_topic_0000002585603033_p1588mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585603033_p1590mcpsimp"><a name="zh-cn_topic_0000002585603033_p1590mcpsimp"></a><a name="zh-cn_topic_0000002585603033_p1590mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585603033_row1591mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585603033_p1593mcpsimp"><a name="zh-cn_topic_0000002585603033_p1593mcpsimp"></a><a name="zh-cn_topic_0000002585603033_p1593mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585603033_p1595mcpsimp"><a name="zh-cn_topic_0000002585603033_p1595mcpsimp"></a><a name="zh-cn_topic_0000002585603033_p1595mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585603033_p1596mcpsimp"><a name="zh-cn_topic_0000002585603033_p1596mcpsimp"></a><a name="zh-cn_topic_0000002585603033_p1596mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585603033_p1598mcpsimp"><a name="zh-cn_topic_0000002585603033_p1598mcpsimp"></a><a name="zh-cn_topic_0000002585603033_p1598mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

## 开发示例<a name="ZH-CN_TOPIC_0000002603342939"></a>

>![](public_sys-resources/icon-notice.gif) **须知：** 
>示例代码仅用于展示基本处理流程，请勿直接部署至生产环境使用。

-   **[手表客户端拉取电话簿](#ZH-CN_TOPIC_0000002572623910)**  

### 手表客户端拉取电话簿<a name="ZH-CN_TOPIC_0000002572623910"></a>

1.  客户端参考“[开启BR/EDR扫描](#ZH-CN_TOPIC_0000002603223013)”中的步骤完成BR/EDR service回调注册、设备信息配置、BR/EDR协议栈使能。
2.  客户端注册PBAP服务。

    ```
    void demo_connect_state_cbk(uint16_t dev_id, uint8_t conn_state)
    {
        // do your things here
    }
    
    void demo_recevice_phone_book_cbk(uint16_t dev_id, const uint8_t *data, uint16_t len)
    {
        // do your things here
    }
    
    void demo_phone_book_result_cbk(uint16_t dev_id, errcode_t ret)
    {
        // do your things here
    }
    
    void demo_recevice_call_log_cbk(uint16_t dev_id, const uint8_t *data, uint16_t len)
    {
        // do your things here
    }
    
    void demo_call_log_result_cbk(uint16_t dev_id, errcode_t ret)
    {
        // do your things here
    }
    
    bs_br_phone_book_callbacks_t demo_phone_book_cbks = {
        .connect_state_cbk = demo_connect_state_cbk,
        .recevice_phone_book_cbk = demo_recevice_phone_book_cbk,
        .phone_book_result_cbk = demo_phone_book_result_cbk,
        .recevice_call_log_cbk = demo_recevice_call_log_cbk,
        .call_log_result_cbk = demo_call_log_result_cbk,
    };
    uint8_t demo_app_id = 0;
    errcode_t ret = bs_br_phone_book_register_callbacks(demo_app_id, &demo_phone_book_cbks);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

3.  客户端与服务端对应设备（如：手机）进行配对，获取对应的设备ID dev\_id。
4.  客户端与服务端进行PBAP连接，连接结果通过注册的bs\_br\_phone\_book\_conn\_state\_changed\_cbk回调函数进行上报。

    ```
    errcode_t ret = bs_br_phone_book_connect(dev_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

5.  客户端与服务端连接成功后，拉取电话簿信息。拉取结果状态通过注册的bs\_br\_phone\_book\_result\_cbk回调函数进行上报，拉取电话簿数据通过注册的bs\_br\_phone\_book\_recevice\_cbk回调函数进行上报。

    ```
    errcode_t ret = bs_br_pull_phone_book(dev_id, BR_PHONE_BOOK_VCARD21);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

6.  电话簿数据传输完毕后，可根据需要断开PBAP连接。

    ```
    errcode_t ret = bs_br_phone_book_disconnect(dev_id);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

# 媒体传输与媒体控制<a name="ZH-CN_TOPIC_0000002603222993"></a>

-   **[概述](#ZH-CN_TOPIC_0000002603223057)**  

-   **[接口说明](#ZH-CN_TOPIC_0000002603223071)**  

-   **[开发示例](#ZH-CN_TOPIC_0000002572783576)**  

## 概述<a name="ZH-CN_TOPIC_0000002603223057"></a>

媒体传输与媒体控制实现蓝牙设备的AVRCP和A2DP协议的注册、连接、断开及媒体流操作，支持音量控制、回调注册等功能，用于实现蓝牙媒体控制与数据传输。

## 接口说明<a name="ZH-CN_TOPIC_0000002603223071"></a>

-   **[媒体控制](#ZH-CN_TOPIC_0000002603342923)**  

-   **[媒体传输](#ZH-CN_TOPIC_0000002572623850)**  

### 媒体控制<a name="ZH-CN_TOPIC_0000002603342923"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002603223017)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002572623886)**  

#### 数据结构<a name="ZH-CN_TOPIC_0000002603223017"></a>

-   **[bs\_br\_media\_avrcp\_role\_t](#ZH-CN_TOPIC_0000002603342955)**  

-   **[bs\_br\_media\_element\_attr\_id\_t](#ZH-CN_TOPIC_0000002572623836)**  

-   **[bs\_br\_media\_operate\_type\_t](#ZH-CN_TOPIC_0000002603223019)**  

-   **[bs\_br\_media\_element\_attr\_value\_t](#ZH-CN_TOPIC_0000002572783590)**  

-   **[bs\_br\_media\_element\_attribute\_t](#ZH-CN_TOPIC_0000002572623830)**  

-   **[br\_media\_play\_status\_t](#ZH-CN_TOPIC_0000002572783490)**  

-   **[br\_media\_info\_type\_t](#ZH-CN_TOPIC_0000002572783524)**  

-   **[bs\_br\_media\_volume\_opt\_type\_t](#ZH-CN_TOPIC_0000002603343033)**  

-   **[bs\_br\_media\_callbacks\_t](#ZH-CN_TOPIC_0000002572623912)**  

##### bs\_br\_media\_avrcp\_role\_t<a name="ZH-CN_TOPIC_0000002603342955"></a>

【结构体说明】

BR/EDR蓝牙媒体AVRCP服务角色。

【结构体定义】

```
typedef enum {
    BR_MEDIA_AVRCP_CT = 0x01,
    BR_MEDIA_AVRCP_TG = 0x02,
} bs_br_media_avrcp_role_t;
```

【注意事项】

无。

##### bs\_br\_media\_element\_attr\_id\_t<a name="ZH-CN_TOPIC_0000002572623836"></a>

【结构体说明】

BR/EDR媒体元素属性ID。

【结构体定义】

```
typedef enum {
    BR_MEDIA_ELEMENT_ATTR_TITLE = 0x01,       /*!< @if Eng Title, song name or content description
                                                   @else 标题，歌曲名或者内容描述 @endif */
    BR_MEDIA_ELEMENT_ATTR_ARTIST_NAME = 0x02, /*!< @if Eng Artist(s), performer(s) or group
                                                   @else 艺人、表演者或团体 @endif */
    BR_MEDIA_ELEMENT_ATTR_ALBUM_NAME = 0x03,   /*!< @if Eng Album @else 唱片，专辑 @endif */
    BR_MEDIA_ELEMENT_ATTR_TRACK_NUMBER = 0x04, /*!< @if Eng The order number of the audio-file on its original recording
                                                    @else 音频文件的原始编号 @endif */
    BR_MEDIA_ELEMENT_ATTR_TOTAL_NUMBER_OF_TRACKS = 0x05, /*!< @if Eng The total number of tracks/elements on the
                                                        original recording @else tracks/elements的总个数 @endif */
    BR_MEDIA_ELEMENT_ATTR_TRACK_GENRE = 0x06,            /*!< @if Eng Genre @else 风格，流派 @endif */
    BR_MEDIA_ELEMENT_ATTR_TRACK_PLAYING_TIME = 0x07      /*!< @if Eng The length of the audio file in milliseconds
                                                              @else 音频文件的播放时间 @endif */
} bs_br_media_element_attr_id_t;
```

【注意事项】

无。

##### bs\_br\_media\_operate\_type\_t<a name="ZH-CN_TOPIC_0000002603223019"></a>

【结构体说明】

BR/EDR媒体操作类型。

【结构体定义】

```
typedef enum {
    BR_MEDIA_OPT_PLAY = 0x01,         /*!< @if Eng play operation, param is NULL, param_len is 0.
                                           @else play操作,  入参是空, 入参长度是0. @endif */
    BR_MEDIA_OPT_STOP = 0x02,         /*!< @if Eng stop operation, param is NULL, param_len is 0.
                                           @else stop操作,  入参是空, 入参长度是0.@endif */
    BR_MEDIA_OPT_PAUSE = 0x03,        /*!< @if Eng pause operation, param is NULL, param_len is 0.
                                           @else pause操作,  入参是空, 入参长度是0. @endif */
    BR_MEDIA_OPT_FORWARD = 0x04,      /*!< @if Eng Switches to the forward one, param is NULL, param_len is 0.
                                           @else 下一首,  入参是空, 入参长度是0. @endif */
    BR_MEDIA_OPT_BACKWARD = 0x05,     /*!< @if Eng Switches to the backward one, param is NULL, param_len is 0.
                                           @else 上一首,  入参是空, 入参长度是0. @endif */
    BR_MEDIA_OPT_REWIND = 0x06,       /*!< @if Eng Moves the position toward the beginning of the medium,
                                                   param is NULL, param_len is 0.
                                             @else 将媒体设置为从起始位置开始, param is NULL, param_len is 0. @endif */
    BR_MEDIA_OPT_FAST_FORWARD = 0x07, /*!< @if Eng Fast forward, param is NULL, param_len is 0.
                                             @else 设置媒体快进, 入参是空, 入参长度是0. @endif */
    BR_MEDIA_OPT_REGISTER_ATTRIBUTE_NOTIFICATION = 0x08, /*!< @if Eng Register media attribute notification,
                                                                      param is NULL, param_len is 0,
                                                              @else 注册歌词属性通知,入参是空, 入参长度是0. @endif */
    BR_MEDIA_OPT_UNREGISTER_ATTRIBUTE_NOTIFICATION = 0x09, /*!< @if Eng Unregister media attribute notification,
                                                                      param is NULL, param_len is 0,
                                                              @else 去注册歌词属性通知,入参是空, 入参长度是0. @endif */
    BR_MEDIA_OPT_REGISTER_POSITION_NOTIFICATION = 0x0A, /*!< @if Eng Register playback position notification,
                                                                      param is interval, param_len is 4 byte,
                                                              @else 注册已播放时长变化通知,入参是通知间隔,
                                                                    入参长度是4字节. @endif */
    BR_MEDIA_OPT_UNREGISTER_POSITION_NOTIFICATION = 0x0B, /*!< @if Eng Unregister playback position notification,
                                                                      param is NULL, param_len is 0,
                                                              @else 去注册已播放时长变化通知,入参是空,入参长度是0. @endif */
} bs_br_media_operate_type_t;
```

【注意事项】

无。

##### bs\_br\_media\_element\_attr\_value\_t<a name="ZH-CN_TOPIC_0000002572783590"></a>

【结构体说明】

BR/EDR TG返回的attribute值。

【结构体定义】

```
typedef struct {
    uint32_t attr_id;            /*!< @if Eng Specifies the attribute ID to be written,
                                          See @ref {bs_br_media_element_attr_id_t}
                                      @else 属性ID 参考 {bs_br_media_element_attr_id_t} @endif */
    uint16_t character_set_id; /*!< @if Eng Specifies the character set ID to be displayed on CT
                                    @else 字符集ID @endif */
    uint16_t value_len; /*!< @if Eng Length of the value of the attribute. 0-65535 (0, if no name is provided)
                             @else 属性名称的长度, 缺省默认0值 @endif */
    uint8_t *value;            /*!< @if Eng Name of the value of the attribute
                                    @else 属性名称 @endif */
} bs_br_media_element_attr_value_t;
```

【注意事项】

无。

##### bs\_br\_media\_element\_attribute\_t<a name="ZH-CN_TOPIC_0000002572623830"></a>

【结构体说明】

BR/EDR TG返回的GetElementAttributes响应参数。

【结构体定义】

```
typedef struct {
    uint8_t attr_num;                       /*!< @if Eng Specifies the attribute number, in range(1, 255)
                                                       @else TG返回的attribute数量，1-255 @endif */
    bs_br_media_element_attr_value_t *attr_value; /*!< @if Eng Specifies the attribute msg
                                                       @else 属性信息，根据attrNum大小动态分配空间 @endif */
} bs_br_media_element_attribute_t;
```

【注意事项】

无。

##### br\_media\_play\_status\_t<a name="ZH-CN_TOPIC_0000002572783490"></a>

【结构体说明】

BR/EDR媒体播放状态。

【结构体定义】

```
typedef enum {
    BR_MEDIA_PLAY_STATUS_STOPPED = 0x00,  /*!< @if Eng Playing status of stop @else AVRCP媒体stopped状态 @endif */
    BR_MEDIA_PLAY_STATUS_PLAYING = 0x01,  /*!< @if Eng Playing status of playing @else AVRCP媒体playing状态 @endif */
    BR_MEDIA_PLAY_STATUS_PAUSED = 0x02,   /*!< @if Eng Playing status of pause @else AVRCP媒体Paused状态 @endif */
    BR_MEDIA_PLAY_STATUS_FWD_SEEK = 0x03, /*!< @if Eng Playing status of FWD_SEEK @else AVRCP媒体快进状态 @endif */
    BR_MEDIA_PLAY_STATUS_REV_SEEK = 0x04, /*!< @if Eng Playing status of REV_SEEK @else AVRCP媒体快退状态 @endif */
} br_media_play_status_t;
```

【注意事项】

无。

##### br\_media\_info\_type\_t<a name="ZH-CN_TOPIC_0000002572783524"></a>

【结构体说明】

BR/EDR媒体信息类型。

【结构体定义】

```
typedef enum {
    BR_MEDIA_INFO_PLAY_STATUS = 0x01, /*!< @if Eng media info play status, param is {br_media_play_status_t},
                                               param_len is 1.
                                           @else 媒体信息-播放状态, 入参是枚举{br_media_play_status_t},
                                                 入参长度是1. @endif */
    BR_MEDIA_INFO_ELEMENT_ATTR = 0x02, /*!< @if Eng media info element attribut,
                                                param is the pointer of {bs_br_media_element_attribute_t},
                                                param_len is the length of struct {bs_br_media_element_attribute_t}.
                                            @else 媒体信息-媒体元素属性, 入参是结构体指针{bs_br_media_element_attribute_t},
                                                 入参长度是结构体{bs_br_media_element_attribute_t}的长度. @endif */
    BR_MEDIA_INFO_PLAYBACK_POSITION = 0x03, /*!< @if Eng Current playback position in millisecond,param_len is 4.
                                                 @else 媒体信息-当前播放时长,单位是毫秒,入参长度是4. @endif */
} br_media_info_type_t;
```

【注意事项】

无。

##### bs\_br\_media\_volume\_opt\_type\_t<a name="ZH-CN_TOPIC_0000002603343033"></a>

【结构体说明】

BR/EDR媒体音量操作类型。

【结构体定义】

```
typedef enum {
    BR_MEDIA_VOLUME_OPT_UP = 0x01, /*!< @if Eng Turn up the volume. @else 调高音量 @endif */
    BR_MEDIA_VOLUME_OPT_DOWN = 0x02, /*!< @if Eng Turn down the volume. @else 调低音量 @endif */
    BR_MEDIA_VOLUME_OPT_MUTE = 0x03, /*!< @if Eng Mute the volume. @else 静音 @endif */
    BR_MEDIA_VOLUME_OPT_SET_ABSOLUTE = 0x04, /*!< @if Eng Set the absolute volume,volume value is 0x00-0x7F
                                                  @else 配置绝对音量,音量值范围是0x00-0x7F @endif */
} bs_br_media_volume_opt_type_t;
```

【注意事项】

无。

##### bs\_br\_media\_callbacks\_t<a name="ZH-CN_TOPIC_0000002572623912"></a>

【结构体说明】

BR/EDR媒体控制回调函数结构体。

【结构体定义】

```
typedef struct {
    bs_br_media_avrcp_conn_state_cbk connect_state_cbk;
    bs_br_media_operate_request_cbk media_opt_req_cbk;
    bs_br_media_info_update_cbk media_update_cbk;
    bs_br_media_volume_operate_cbk media_volume_operate_cbk;
    bs_br_media_volume_info_report_cbk media_volume_report_cbk;
} bs_br_media_callbacks_t;
```

【注意事项】

无。

#### 函数接口<a name="ZH-CN_TOPIC_0000002572623886"></a>

-   **[bs\_br\_media\_avrcp\_register](#ZH-CN_TOPIC_0000002572783512)**  

-   **[bs\_br\_media\_avrcp\_unregister](#ZH-CN_TOPIC_0000002572783554)**  

-   **[bs\_br\_media\_avrcp\_connect](#ZH-CN_TOPIC_0000002603343025)**  

-   **[bs\_br\_media\_avrcp\_disconnect](#ZH-CN_TOPIC_0000002572623922)**  

-   **[bs\_br\_media\_operate\_request](#ZH-CN_TOPIC_0000002603342943)**  

-   **[bs\_br\_media\_update\_info](#ZH-CN_TOPIC_0000002572623894)**  

-   **[bs\_br\_media\_volume\_operate](#ZH-CN_TOPIC_0000002572783580)**  

-   **[bs\_br\_media\_volume\_notify](#ZH-CN_TOPIC_0000002603343019)**  

-   **[bs\_br\_media\_register\_callbacks](#ZH-CN_TOPIC_0000002572623858)**  

-   **[bs\_br\_media\_unregister\_callbacks](#ZH-CN_TOPIC_0000002603223009)**  

##### bs\_br\_media\_avrcp\_register<a name="ZH-CN_TOPIC_0000002572783512"></a>

【描述】

BR/EDR注册蓝牙AVRCP服务。

【接口定义】

```
errcode_t bs_br_media_avrcp_register(uint8_t avrcp_role);
```

【参数】

<a name="zh-cn_topic_0000002555144744_table1471mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555144744_row1477mcpsimp"><th class="cellrowborder" valign="top" width="24.47%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555144744_p1479mcpsimp"><a name="zh-cn_topic_0000002555144744_p1479mcpsimp"></a><a name="zh-cn_topic_0000002555144744_p1479mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="55.089999999999996%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555144744_p1481mcpsimp"><a name="zh-cn_topic_0000002555144744_p1481mcpsimp"></a><a name="zh-cn_topic_0000002555144744_p1481mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555144744_p1483mcpsimp"><a name="zh-cn_topic_0000002555144744_p1483mcpsimp"></a><a name="zh-cn_topic_0000002555144744_p1483mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555144744_row1484mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555144744_p1486mcpsimp"><a name="zh-cn_topic_0000002555144744_p1486mcpsimp"></a><a name="zh-cn_topic_0000002555144744_p1486mcpsimp"></a>avrcp_role</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555144744_p1488mcpsimp"><a name="zh-cn_topic_0000002555144744_p1488mcpsimp"></a><a name="zh-cn_topic_0000002555144744_p1488mcpsimp"></a>本地服务角色。详见“<a href="#ZH-CN_TOPIC_0000002603342955">bs_br_media_avrcp_role_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555144744_p1490mcpsimp"><a name="zh-cn_topic_0000002555144744_p1490mcpsimp"></a><a name="zh-cn_topic_0000002555144744_p1490mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555144744_table1514mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555144744_row1520mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555144744_p1522mcpsimp"><a name="zh-cn_topic_0000002555144744_p1522mcpsimp"></a><a name="zh-cn_topic_0000002555144744_p1522mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555144744_p1524mcpsimp"><a name="zh-cn_topic_0000002555144744_p1524mcpsimp"></a><a name="zh-cn_topic_0000002555144744_p1524mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555144744_p1526mcpsimp"><a name="zh-cn_topic_0000002555144744_p1526mcpsimp"></a><a name="zh-cn_topic_0000002555144744_p1526mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555144744_row1527mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555144744_p1529mcpsimp"><a name="zh-cn_topic_0000002555144744_p1529mcpsimp"></a><a name="zh-cn_topic_0000002555144744_p1529mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555144744_p1531mcpsimp"><a name="zh-cn_topic_0000002555144744_p1531mcpsimp"></a><a name="zh-cn_topic_0000002555144744_p1531mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555144744_p1532mcpsimp"><a name="zh-cn_topic_0000002555144744_p1532mcpsimp"></a><a name="zh-cn_topic_0000002555144744_p1532mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555144744_p1534mcpsimp"><a name="zh-cn_topic_0000002555144744_p1534mcpsimp"></a><a name="zh-cn_topic_0000002555144744_p1534mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_avrcp\_unregister<a name="ZH-CN_TOPIC_0000002572783554"></a>

【描述】

BR/EDR去注册蓝牙AVRCP服务。

【接口定义】

```
errcode_t bs_br_media_avrcp_unregister(uint8_t avrcp_role);
```

【参数】

<a name="zh-cn_topic_0000002555467192_table1471mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555467192_row1477mcpsimp"><th class="cellrowborder" valign="top" width="24.47%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555467192_p1479mcpsimp"><a name="zh-cn_topic_0000002555467192_p1479mcpsimp"></a><a name="zh-cn_topic_0000002555467192_p1479mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="55.089999999999996%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555467192_p1481mcpsimp"><a name="zh-cn_topic_0000002555467192_p1481mcpsimp"></a><a name="zh-cn_topic_0000002555467192_p1481mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555467192_p1483mcpsimp"><a name="zh-cn_topic_0000002555467192_p1483mcpsimp"></a><a name="zh-cn_topic_0000002555467192_p1483mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555467192_row1484mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555467192_p1486mcpsimp"><a name="zh-cn_topic_0000002555467192_p1486mcpsimp"></a><a name="zh-cn_topic_0000002555467192_p1486mcpsimp"></a>avrcp_role</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555467192_p1488mcpsimp"><a name="zh-cn_topic_0000002555467192_p1488mcpsimp"></a><a name="zh-cn_topic_0000002555467192_p1488mcpsimp"></a>本地服务角色。详见“<a href="#ZH-CN_TOPIC_0000002603342955">bs_br_media_avrcp_role_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555467192_p1490mcpsimp"><a name="zh-cn_topic_0000002555467192_p1490mcpsimp"></a><a name="zh-cn_topic_0000002555467192_p1490mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555467192_table1514mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555467192_row1520mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555467192_p1522mcpsimp"><a name="zh-cn_topic_0000002555467192_p1522mcpsimp"></a><a name="zh-cn_topic_0000002555467192_p1522mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555467192_p1524mcpsimp"><a name="zh-cn_topic_0000002555467192_p1524mcpsimp"></a><a name="zh-cn_topic_0000002555467192_p1524mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555467192_p1526mcpsimp"><a name="zh-cn_topic_0000002555467192_p1526mcpsimp"></a><a name="zh-cn_topic_0000002555467192_p1526mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555467192_row1527mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555467192_p1529mcpsimp"><a name="zh-cn_topic_0000002555467192_p1529mcpsimp"></a><a name="zh-cn_topic_0000002555467192_p1529mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555467192_p1531mcpsimp"><a name="zh-cn_topic_0000002555467192_p1531mcpsimp"></a><a name="zh-cn_topic_0000002555467192_p1531mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555467192_p1532mcpsimp"><a name="zh-cn_topic_0000002555467192_p1532mcpsimp"></a><a name="zh-cn_topic_0000002555467192_p1532mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555467192_p1534mcpsimp"><a name="zh-cn_topic_0000002555467192_p1534mcpsimp"></a><a name="zh-cn_topic_0000002555467192_p1534mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_avrcp\_connect<a name="ZH-CN_TOPIC_0000002603343025"></a>

【描述】

BR/EDR建立蓝牙媒体AVRCP连接。

【接口定义】

```
errcode_t bs_br_media_avrcp_connect(uint16_t dev_id, uint8_t avrcp_role);
```

【参数】

<a name="zh-cn_topic_0000002585907125_table1471mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585907125_row1477mcpsimp"><th class="cellrowborder" valign="top" width="24.47%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585907125_p1479mcpsimp"><a name="zh-cn_topic_0000002585907125_p1479mcpsimp"></a><a name="zh-cn_topic_0000002585907125_p1479mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="55.089999999999996%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585907125_p1481mcpsimp"><a name="zh-cn_topic_0000002585907125_p1481mcpsimp"></a><a name="zh-cn_topic_0000002585907125_p1481mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585907125_p1483mcpsimp"><a name="zh-cn_topic_0000002585907125_p1483mcpsimp"></a><a name="zh-cn_topic_0000002585907125_p1483mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585907125_row1484mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585907125_p973212251289"><a name="zh-cn_topic_0000002585907125_p973212251289"></a><a name="zh-cn_topic_0000002585907125_p973212251289"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585907125_p1929830192810"><a name="zh-cn_topic_0000002585907125_p1929830192810"></a><a name="zh-cn_topic_0000002585907125_p1929830192810"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585907125_p1490mcpsimp"><a name="zh-cn_topic_0000002585907125_p1490mcpsimp"></a><a name="zh-cn_topic_0000002585907125_p1490mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585907125_row1491mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585907125_p1486mcpsimp"><a name="zh-cn_topic_0000002585907125_p1486mcpsimp"></a><a name="zh-cn_topic_0000002585907125_p1486mcpsimp"></a>avrcp_role</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585907125_p1488mcpsimp"><a name="zh-cn_topic_0000002585907125_p1488mcpsimp"></a><a name="zh-cn_topic_0000002585907125_p1488mcpsimp"></a>本地服务角色。详见“<a href="#ZH-CN_TOPIC_0000002603342955">bs_br_media_avrcp_role_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585907125_p4544243132814"><a name="zh-cn_topic_0000002585907125_p4544243132814"></a><a name="zh-cn_topic_0000002585907125_p4544243132814"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585907125_table1514mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585907125_row1520mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585907125_p1522mcpsimp"><a name="zh-cn_topic_0000002585907125_p1522mcpsimp"></a><a name="zh-cn_topic_0000002585907125_p1522mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585907125_p1524mcpsimp"><a name="zh-cn_topic_0000002585907125_p1524mcpsimp"></a><a name="zh-cn_topic_0000002585907125_p1524mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585907125_p1526mcpsimp"><a name="zh-cn_topic_0000002585907125_p1526mcpsimp"></a><a name="zh-cn_topic_0000002585907125_p1526mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585907125_row1527mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585907125_p1529mcpsimp"><a name="zh-cn_topic_0000002585907125_p1529mcpsimp"></a><a name="zh-cn_topic_0000002585907125_p1529mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585907125_p1531mcpsimp"><a name="zh-cn_topic_0000002585907125_p1531mcpsimp"></a><a name="zh-cn_topic_0000002585907125_p1531mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585907125_p1532mcpsimp"><a name="zh-cn_topic_0000002585907125_p1532mcpsimp"></a><a name="zh-cn_topic_0000002585907125_p1532mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585907125_p1534mcpsimp"><a name="zh-cn_topic_0000002585907125_p1534mcpsimp"></a><a name="zh-cn_topic_0000002585907125_p1534mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_avrcp\_disconnect<a name="ZH-CN_TOPIC_0000002572623922"></a>

【描述】

BR/EDR断开蓝牙媒体AVRCP连接。

【接口定义】

```
errcode_t bs_br_media_avrcp_disconnect(uint16_t dev_id, uint8_t avrcp_role);
```

【参数】

<a name="zh-cn_topic_0000002586027171_table1471mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002586027171_row1477mcpsimp"><th class="cellrowborder" valign="top" width="24.47%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002586027171_p1479mcpsimp"><a name="zh-cn_topic_0000002586027171_p1479mcpsimp"></a><a name="zh-cn_topic_0000002586027171_p1479mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="55.089999999999996%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002586027171_p1481mcpsimp"><a name="zh-cn_topic_0000002586027171_p1481mcpsimp"></a><a name="zh-cn_topic_0000002586027171_p1481mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002586027171_p1483mcpsimp"><a name="zh-cn_topic_0000002586027171_p1483mcpsimp"></a><a name="zh-cn_topic_0000002586027171_p1483mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002586027171_row1484mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002586027171_p973212251289"><a name="zh-cn_topic_0000002586027171_p973212251289"></a><a name="zh-cn_topic_0000002586027171_p973212251289"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002586027171_p1929830192810"><a name="zh-cn_topic_0000002586027171_p1929830192810"></a><a name="zh-cn_topic_0000002586027171_p1929830192810"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002586027171_p1490mcpsimp"><a name="zh-cn_topic_0000002586027171_p1490mcpsimp"></a><a name="zh-cn_topic_0000002586027171_p1490mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002586027171_row1491mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002586027171_p1486mcpsimp"><a name="zh-cn_topic_0000002586027171_p1486mcpsimp"></a><a name="zh-cn_topic_0000002586027171_p1486mcpsimp"></a>avrcp_role</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002586027171_p1488mcpsimp"><a name="zh-cn_topic_0000002586027171_p1488mcpsimp"></a><a name="zh-cn_topic_0000002586027171_p1488mcpsimp"></a>本地服务角色。详见“<a href="#ZH-CN_TOPIC_0000002603342955">bs_br_media_avrcp_role_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002586027171_p4544243132814"><a name="zh-cn_topic_0000002586027171_p4544243132814"></a><a name="zh-cn_topic_0000002586027171_p4544243132814"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002586027171_table1514mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002586027171_row1520mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002586027171_p1522mcpsimp"><a name="zh-cn_topic_0000002586027171_p1522mcpsimp"></a><a name="zh-cn_topic_0000002586027171_p1522mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002586027171_p1524mcpsimp"><a name="zh-cn_topic_0000002586027171_p1524mcpsimp"></a><a name="zh-cn_topic_0000002586027171_p1524mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002586027171_p1526mcpsimp"><a name="zh-cn_topic_0000002586027171_p1526mcpsimp"></a><a name="zh-cn_topic_0000002586027171_p1526mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002586027171_row1527mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002586027171_p1529mcpsimp"><a name="zh-cn_topic_0000002586027171_p1529mcpsimp"></a><a name="zh-cn_topic_0000002586027171_p1529mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002586027171_p1531mcpsimp"><a name="zh-cn_topic_0000002586027171_p1531mcpsimp"></a><a name="zh-cn_topic_0000002586027171_p1531mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002586027171_p1532mcpsimp"><a name="zh-cn_topic_0000002586027171_p1532mcpsimp"></a><a name="zh-cn_topic_0000002586027171_p1532mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002586027171_p1534mcpsimp"><a name="zh-cn_topic_0000002586027171_p1534mcpsimp"></a><a name="zh-cn_topic_0000002586027171_p1534mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_operate\_request<a name="ZH-CN_TOPIC_0000002603342943"></a>

【描述】

BR/EDR蓝牙媒体操作请求。

【接口定义】

```
errcode_t bs_br_media_operate_request(uint16_t dev_id, uint8_t opt_type, uint8_t *param, uint16_t param_len);

```

【参数】

<a name="zh-cn_topic_0000002555307550_table1471mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555307550_row1477mcpsimp"><th class="cellrowborder" valign="top" width="24.47%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555307550_p1479mcpsimp"><a name="zh-cn_topic_0000002555307550_p1479mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1479mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="55.089999999999996%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555307550_p1481mcpsimp"><a name="zh-cn_topic_0000002555307550_p1481mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1481mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555307550_p1483mcpsimp"><a name="zh-cn_topic_0000002555307550_p1483mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1483mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555307550_row1484mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555307550_p973212251289"><a name="zh-cn_topic_0000002555307550_p973212251289"></a><a name="zh-cn_topic_0000002555307550_p973212251289"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555307550_p1929830192810"><a name="zh-cn_topic_0000002555307550_p1929830192810"></a><a name="zh-cn_topic_0000002555307550_p1929830192810"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555307550_p1490mcpsimp"><a name="zh-cn_topic_0000002555307550_p1490mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1490mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002555307550_row1491mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555307550_p1493mcpsimp"><a name="zh-cn_topic_0000002555307550_p1493mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1493mcpsimp"></a>opt_type</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555307550_p1495mcpsimp"><a name="zh-cn_topic_0000002555307550_p1495mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1495mcpsimp"></a>媒体操作类型。详见“<a href="#ZH-CN_TOPIC_0000002603223019">bs_br_media_operate_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555307550_p1497mcpsimp"><a name="zh-cn_topic_0000002555307550_p1497mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1497mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002555307550_row1498mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555307550_p1500mcpsimp"><a name="zh-cn_topic_0000002555307550_p1500mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1500mcpsimp"></a>param</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555307550_p111213712319"><a name="zh-cn_topic_0000002555307550_p111213712319"></a><a name="zh-cn_topic_0000002555307550_p111213712319"></a>媒体操作参数。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555307550_p1504mcpsimp"><a name="zh-cn_topic_0000002555307550_p1504mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1504mcpsimp"></a>输出</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002555307550_row1505mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555307550_p1507mcpsimp"><a name="zh-cn_topic_0000002555307550_p1507mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1507mcpsimp"></a>param_len</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555307550_p49075116312"><a name="zh-cn_topic_0000002555307550_p49075116312"></a><a name="zh-cn_topic_0000002555307550_p49075116312"></a>媒体操作参数长度</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555307550_p1512mcpsimp"><a name="zh-cn_topic_0000002555307550_p1512mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1512mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555307550_table1514mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555307550_row1520mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555307550_p1522mcpsimp"><a name="zh-cn_topic_0000002555307550_p1522mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1522mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555307550_p1524mcpsimp"><a name="zh-cn_topic_0000002555307550_p1524mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1524mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555307550_p1526mcpsimp"><a name="zh-cn_topic_0000002555307550_p1526mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1526mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555307550_row1527mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555307550_p1529mcpsimp"><a name="zh-cn_topic_0000002555307550_p1529mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1529mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555307550_p1531mcpsimp"><a name="zh-cn_topic_0000002555307550_p1531mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1531mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555307550_p1532mcpsimp"><a name="zh-cn_topic_0000002555307550_p1532mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1532mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555307550_p1534mcpsimp"><a name="zh-cn_topic_0000002555307550_p1534mcpsimp"></a><a name="zh-cn_topic_0000002555307550_p1534mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_update\_info<a name="ZH-CN_TOPIC_0000002572623894"></a>

【描述】

BR/EDR蓝牙媒体信息更新。

【接口定义】

```
errcode_t bs_br_media_update_info(uint16_t dev_id, uint8_t type, uint8_t *param, uint16_t param_len);
```

【参数】

<a name="zh-cn_topic_0000002555467198_table1471mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555467198_row1477mcpsimp"><th class="cellrowborder" valign="top" width="24.47%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555467198_p1479mcpsimp"><a name="zh-cn_topic_0000002555467198_p1479mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1479mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="55.089999999999996%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555467198_p1481mcpsimp"><a name="zh-cn_topic_0000002555467198_p1481mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1481mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555467198_p1483mcpsimp"><a name="zh-cn_topic_0000002555467198_p1483mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1483mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555467198_row1484mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555467198_p973212251289"><a name="zh-cn_topic_0000002555467198_p973212251289"></a><a name="zh-cn_topic_0000002555467198_p973212251289"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555467198_p1929830192810"><a name="zh-cn_topic_0000002555467198_p1929830192810"></a><a name="zh-cn_topic_0000002555467198_p1929830192810"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555467198_p1490mcpsimp"><a name="zh-cn_topic_0000002555467198_p1490mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1490mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002555467198_row1491mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555467198_p1493mcpsimp"><a name="zh-cn_topic_0000002555467198_p1493mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1493mcpsimp"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555467198_p1495mcpsimp"><a name="zh-cn_topic_0000002555467198_p1495mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1495mcpsimp"></a>媒体操作类型。详见“<a href="#ZH-CN_TOPIC_0000002572783524">br_media_info_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555467198_p1497mcpsimp"><a name="zh-cn_topic_0000002555467198_p1497mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1497mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002555467198_row1498mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555467198_p1500mcpsimp"><a name="zh-cn_topic_0000002555467198_p1500mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1500mcpsimp"></a>param</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555467198_p111213712319"><a name="zh-cn_topic_0000002555467198_p111213712319"></a><a name="zh-cn_topic_0000002555467198_p111213712319"></a>媒体操作参数。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555467198_p1504mcpsimp"><a name="zh-cn_topic_0000002555467198_p1504mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1504mcpsimp"></a>输出</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002555467198_row1505mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555467198_p1507mcpsimp"><a name="zh-cn_topic_0000002555467198_p1507mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1507mcpsimp"></a>param_len</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555467198_p49075116312"><a name="zh-cn_topic_0000002555467198_p49075116312"></a><a name="zh-cn_topic_0000002555467198_p49075116312"></a>媒体操作参数长度。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555467198_p1512mcpsimp"><a name="zh-cn_topic_0000002555467198_p1512mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1512mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555467198_table1514mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555467198_row1520mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555467198_p1522mcpsimp"><a name="zh-cn_topic_0000002555467198_p1522mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1522mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555467198_p1524mcpsimp"><a name="zh-cn_topic_0000002555467198_p1524mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1524mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555467198_p1526mcpsimp"><a name="zh-cn_topic_0000002555467198_p1526mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1526mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555467198_row1527mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555467198_p1529mcpsimp"><a name="zh-cn_topic_0000002555467198_p1529mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1529mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555467198_p1531mcpsimp"><a name="zh-cn_topic_0000002555467198_p1531mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1531mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555467198_p1532mcpsimp"><a name="zh-cn_topic_0000002555467198_p1532mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1532mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555467198_p1534mcpsimp"><a name="zh-cn_topic_0000002555467198_p1534mcpsimp"></a><a name="zh-cn_topic_0000002555467198_p1534mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_volume\_operate<a name="ZH-CN_TOPIC_0000002572783580"></a>

【描述】

BR/EDR蓝牙媒体音量操作。

【接口定义】

```
errcode_t bs_br_media_volume_operate(uint16_t dev_id, uint8_t type, uint8_t volume);
```

【参数】

<a name="zh-cn_topic_0000002585907129_table1471mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585907129_row1477mcpsimp"><th class="cellrowborder" valign="top" width="24.47%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585907129_p1479mcpsimp"><a name="zh-cn_topic_0000002585907129_p1479mcpsimp"></a><a name="zh-cn_topic_0000002585907129_p1479mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="55.089999999999996%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585907129_p1481mcpsimp"><a name="zh-cn_topic_0000002585907129_p1481mcpsimp"></a><a name="zh-cn_topic_0000002585907129_p1481mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585907129_p1483mcpsimp"><a name="zh-cn_topic_0000002585907129_p1483mcpsimp"></a><a name="zh-cn_topic_0000002585907129_p1483mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585907129_row1484mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585907129_p973212251289"><a name="zh-cn_topic_0000002585907129_p973212251289"></a><a name="zh-cn_topic_0000002585907129_p973212251289"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585907129_p1488mcpsimp"><a name="zh-cn_topic_0000002585907129_p1488mcpsimp"></a><a name="zh-cn_topic_0000002585907129_p1488mcpsimp"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585907129_p1490mcpsimp"><a name="zh-cn_topic_0000002585907129_p1490mcpsimp"></a><a name="zh-cn_topic_0000002585907129_p1490mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585907129_row1579923212340"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585907129_p87991332133419"><a name="zh-cn_topic_0000002585907129_p87991332133419"></a><a name="zh-cn_topic_0000002585907129_p87991332133419"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585907129_p979914324341"><a name="zh-cn_topic_0000002585907129_p979914324341"></a><a name="zh-cn_topic_0000002585907129_p979914324341"></a>操作类型。详见“<a href="#ZH-CN_TOPIC_0000002603343033">bs_br_media_volume_opt_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585907129_p15799133243416"><a name="zh-cn_topic_0000002585907129_p15799133243416"></a><a name="zh-cn_topic_0000002585907129_p15799133243416"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585907129_row1491mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585907129_p164713493332"><a name="zh-cn_topic_0000002585907129_p164713493332"></a><a name="zh-cn_topic_0000002585907129_p164713493332"></a>volume</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585907129_p1495mcpsimp"><a name="zh-cn_topic_0000002585907129_p1495mcpsimp"></a><a name="zh-cn_topic_0000002585907129_p1495mcpsimp"></a>音量值。操作类型为配置绝对音量时的入参。取值范围为0x00～0x7F。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585907129_p1497mcpsimp"><a name="zh-cn_topic_0000002585907129_p1497mcpsimp"></a><a name="zh-cn_topic_0000002585907129_p1497mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585907129_table1514mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585907129_row1520mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585907129_p1522mcpsimp"><a name="zh-cn_topic_0000002585907129_p1522mcpsimp"></a><a name="zh-cn_topic_0000002585907129_p1522mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585907129_p1524mcpsimp"><a name="zh-cn_topic_0000002585907129_p1524mcpsimp"></a><a name="zh-cn_topic_0000002585907129_p1524mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585907129_p1526mcpsimp"><a name="zh-cn_topic_0000002585907129_p1526mcpsimp"></a><a name="zh-cn_topic_0000002585907129_p1526mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585907129_row1527mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585907129_p1529mcpsimp"><a name="zh-cn_topic_0000002585907129_p1529mcpsimp"></a><a name="zh-cn_topic_0000002585907129_p1529mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585907129_p1531mcpsimp"><a name="zh-cn_topic_0000002585907129_p1531mcpsimp"></a><a name="zh-cn_topic_0000002585907129_p1531mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585907129_p1532mcpsimp"><a name="zh-cn_topic_0000002585907129_p1532mcpsimp"></a><a name="zh-cn_topic_0000002585907129_p1532mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585907129_p1534mcpsimp"><a name="zh-cn_topic_0000002585907129_p1534mcpsimp"></a><a name="zh-cn_topic_0000002585907129_p1534mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_volume\_notify<a name="ZH-CN_TOPIC_0000002603343019"></a>

【描述】

BR/EDR蓝牙媒体音量通知。

【接口定义】

```
errcode_t bs_br_media_volume_notify(uint16_t dev_id, uint8_t volume);
```

【参数】

<a name="zh-cn_topic_0000002586027173_table1471mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002586027173_row1477mcpsimp"><th class="cellrowborder" valign="top" width="24.47%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002586027173_p1479mcpsimp"><a name="zh-cn_topic_0000002586027173_p1479mcpsimp"></a><a name="zh-cn_topic_0000002586027173_p1479mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="55.089999999999996%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002586027173_p1481mcpsimp"><a name="zh-cn_topic_0000002586027173_p1481mcpsimp"></a><a name="zh-cn_topic_0000002586027173_p1481mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002586027173_p1483mcpsimp"><a name="zh-cn_topic_0000002586027173_p1483mcpsimp"></a><a name="zh-cn_topic_0000002586027173_p1483mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002586027173_row1484mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002586027173_p973212251289"><a name="zh-cn_topic_0000002586027173_p973212251289"></a><a name="zh-cn_topic_0000002586027173_p973212251289"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002586027173_p1488mcpsimp"><a name="zh-cn_topic_0000002586027173_p1488mcpsimp"></a><a name="zh-cn_topic_0000002586027173_p1488mcpsimp"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002586027173_p1490mcpsimp"><a name="zh-cn_topic_0000002586027173_p1490mcpsimp"></a><a name="zh-cn_topic_0000002586027173_p1490mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002586027173_row1491mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002586027173_p164713493332"><a name="zh-cn_topic_0000002586027173_p164713493332"></a><a name="zh-cn_topic_0000002586027173_p164713493332"></a>volume</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002586027173_p1495mcpsimp"><a name="zh-cn_topic_0000002586027173_p1495mcpsimp"></a><a name="zh-cn_topic_0000002586027173_p1495mcpsimp"></a>音量值。取值范围0x00～0x7F。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002586027173_p1497mcpsimp"><a name="zh-cn_topic_0000002586027173_p1497mcpsimp"></a><a name="zh-cn_topic_0000002586027173_p1497mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002586027173_table1514mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002586027173_row1520mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002586027173_p1522mcpsimp"><a name="zh-cn_topic_0000002586027173_p1522mcpsimp"></a><a name="zh-cn_topic_0000002586027173_p1522mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002586027173_p1524mcpsimp"><a name="zh-cn_topic_0000002586027173_p1524mcpsimp"></a><a name="zh-cn_topic_0000002586027173_p1524mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002586027173_p1526mcpsimp"><a name="zh-cn_topic_0000002586027173_p1526mcpsimp"></a><a name="zh-cn_topic_0000002586027173_p1526mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002586027173_row1527mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002586027173_p1529mcpsimp"><a name="zh-cn_topic_0000002586027173_p1529mcpsimp"></a><a name="zh-cn_topic_0000002586027173_p1529mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002586027173_p1531mcpsimp"><a name="zh-cn_topic_0000002586027173_p1531mcpsimp"></a><a name="zh-cn_topic_0000002586027173_p1531mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002586027173_p1532mcpsimp"><a name="zh-cn_topic_0000002586027173_p1532mcpsimp"></a><a name="zh-cn_topic_0000002586027173_p1532mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002586027173_p1534mcpsimp"><a name="zh-cn_topic_0000002586027173_p1534mcpsimp"></a><a name="zh-cn_topic_0000002586027173_p1534mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_register\_callbacks<a name="ZH-CN_TOPIC_0000002572623858"></a>

【描述】

BR/EDR注册媒体控制回调函数。

【接口定义】

```
errcode_t bs_br_media_register_callbacks(uint8_t app_id, bs_br_media_callbacks_t *func);
```

【参数】

<a name="zh-cn_topic_0000002555307552_table1471mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555307552_row1477mcpsimp"><th class="cellrowborder" valign="top" width="24.47%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555307552_p1479mcpsimp"><a name="zh-cn_topic_0000002555307552_p1479mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1479mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="55.089999999999996%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555307552_p1481mcpsimp"><a name="zh-cn_topic_0000002555307552_p1481mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1481mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555307552_p1483mcpsimp"><a name="zh-cn_topic_0000002555307552_p1483mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1483mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555307552_row1484mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555307552_p1486mcpsimp"><a name="zh-cn_topic_0000002555307552_p1486mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1486mcpsimp"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555307552_p1488mcpsimp"><a name="zh-cn_topic_0000002555307552_p1488mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1488mcpsimp"></a>应用ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555307552_p1490mcpsimp"><a name="zh-cn_topic_0000002555307552_p1490mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1490mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002555307552_row1491mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555307552_p1493mcpsimp"><a name="zh-cn_topic_0000002555307552_p1493mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1493mcpsimp"></a>func</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555307552_p1495mcpsimp"><a name="zh-cn_topic_0000002555307552_p1495mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1495mcpsimp"></a>被注册的回调函数。详见“<a href="#ZH-CN_TOPIC_0000002572623912">bs_br_media_callbacks_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555307552_p1497mcpsimp"><a name="zh-cn_topic_0000002555307552_p1497mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1497mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555307552_table1514mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555307552_row1520mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555307552_p1522mcpsimp"><a name="zh-cn_topic_0000002555307552_p1522mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1522mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555307552_p1524mcpsimp"><a name="zh-cn_topic_0000002555307552_p1524mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1524mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555307552_p1526mcpsimp"><a name="zh-cn_topic_0000002555307552_p1526mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1526mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555307552_row1527mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555307552_p1529mcpsimp"><a name="zh-cn_topic_0000002555307552_p1529mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1529mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555307552_p1531mcpsimp"><a name="zh-cn_topic_0000002555307552_p1531mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1531mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555307552_p1532mcpsimp"><a name="zh-cn_topic_0000002555307552_p1532mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1532mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555307552_p1534mcpsimp"><a name="zh-cn_topic_0000002555307552_p1534mcpsimp"></a><a name="zh-cn_topic_0000002555307552_p1534mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_unregister\_callbacks<a name="ZH-CN_TOPIC_0000002603223009"></a>

【描述】

BR/EDR去注册媒体控制回调函数。

【接口定义】

```
errcode_t bs_br_media_unregister_callbacks(uint8_t app_id);
```

【参数】

<a name="zh-cn_topic_0000002555467200_table1471mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555467200_row1477mcpsimp"><th class="cellrowborder" valign="top" width="24.47%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555467200_p1479mcpsimp"><a name="zh-cn_topic_0000002555467200_p1479mcpsimp"></a><a name="zh-cn_topic_0000002555467200_p1479mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="55.089999999999996%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555467200_p1481mcpsimp"><a name="zh-cn_topic_0000002555467200_p1481mcpsimp"></a><a name="zh-cn_topic_0000002555467200_p1481mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555467200_p1483mcpsimp"><a name="zh-cn_topic_0000002555467200_p1483mcpsimp"></a><a name="zh-cn_topic_0000002555467200_p1483mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555467200_row1484mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555467200_p1486mcpsimp"><a name="zh-cn_topic_0000002555467200_p1486mcpsimp"></a><a name="zh-cn_topic_0000002555467200_p1486mcpsimp"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555467200_p1488mcpsimp"><a name="zh-cn_topic_0000002555467200_p1488mcpsimp"></a><a name="zh-cn_topic_0000002555467200_p1488mcpsimp"></a>应用ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555467200_p1490mcpsimp"><a name="zh-cn_topic_0000002555467200_p1490mcpsimp"></a><a name="zh-cn_topic_0000002555467200_p1490mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555467200_table1514mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555467200_row1520mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555467200_p1522mcpsimp"><a name="zh-cn_topic_0000002555467200_p1522mcpsimp"></a><a name="zh-cn_topic_0000002555467200_p1522mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555467200_p1524mcpsimp"><a name="zh-cn_topic_0000002555467200_p1524mcpsimp"></a><a name="zh-cn_topic_0000002555467200_p1524mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555467200_p1526mcpsimp"><a name="zh-cn_topic_0000002555467200_p1526mcpsimp"></a><a name="zh-cn_topic_0000002555467200_p1526mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555467200_row1527mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555467200_p1529mcpsimp"><a name="zh-cn_topic_0000002555467200_p1529mcpsimp"></a><a name="zh-cn_topic_0000002555467200_p1529mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555467200_p1531mcpsimp"><a name="zh-cn_topic_0000002555467200_p1531mcpsimp"></a><a name="zh-cn_topic_0000002555467200_p1531mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555467200_p1532mcpsimp"><a name="zh-cn_topic_0000002555467200_p1532mcpsimp"></a><a name="zh-cn_topic_0000002555467200_p1532mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555467200_p1534mcpsimp"><a name="zh-cn_topic_0000002555467200_p1534mcpsimp"></a><a name="zh-cn_topic_0000002555467200_p1534mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

### 媒体传输<a name="ZH-CN_TOPIC_0000002572623850"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002572783472)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002572623834)**  

#### 数据结构<a name="ZH-CN_TOPIC_0000002572783472"></a>

-   **[bs\_br\_media\_a2dp\_role\_t](#ZH-CN_TOPIC_0000002572623844)**  

-   **[bs\_br\_media\_stream\_opt\_type\_t](#ZH-CN_TOPIC_0000002572623852)**  

-   **[bs\_br\_media\_stream\_state\_t](#ZH-CN_TOPIC_0000002572783488)**  

-   **[bs\_br\_media\_sbc\_sf\_t](#ZH-CN_TOPIC_0000002603222969)**  

-   **[bs\_br\_media\_sbc\_chmode\_t](#ZH-CN_TOPIC_0000002603342935)**  

-   **[bs\_br\_media\_sbc\_subband\_t](#ZH-CN_TOPIC_0000002572783506)**  

-   **[bs\_br\_media\_sbc\_allocation\_t](#ZH-CN_TOPIC_0000002572783564)**  

-   **[bs\_br\_media\_aac\_object\_type\_t](#ZH-CN_TOPIC_0000002603223025)**  

-   **[bs\_br\_media\_aac\_sf\_t](#ZH-CN_TOPIC_0000002572623920)**  

-   **[bs\_br\_media\_aac\_channel\_t](#ZH-CN_TOPIC_0000002603342937)**  

-   **[bs\_br\_media\_aac\_vbr\_t](#ZH-CN_TOPIC_0000002603343027)**  

-   **[bs\_br\_media\_aac\_codec\_t](#ZH-CN_TOPIC_0000002603342969)**  

-   **[bs\_br\_media\_sbc\_codec\_t](#ZH-CN_TOPIC_0000002572623862)**  

-   **[bs\_br\_media\_codec\_type\_t](#ZH-CN_TOPIC_0000002572783470)**  

-   **[bs\_br\_media\_codec\_info\_t](#ZH-CN_TOPIC_0000002603343029)**  

-   **[bs\_br\_media\_stream\_callbacks\_t](#ZH-CN_TOPIC_0000002603223021)**  

##### bs\_br\_media\_a2dp\_role\_t<a name="ZH-CN_TOPIC_0000002572623844"></a>

【结构体说明】

BR/EDR蓝牙媒体A2DP服务角色。

【结构体定义】

```
typedef enum {
    BR_MEDIA_A2DP_SNK = 0x01,
    BR_MEDIA_A2DP_SRC = 0x02,
} bs_br_media_a2dp_role_t;
```

【注意事项】

无。

##### bs\_br\_media\_stream\_opt\_type\_t<a name="ZH-CN_TOPIC_0000002572623852"></a>

【结构体说明】

BR/EDR蓝牙媒体流操作类型。

【结构体定义】

```
typedef enum {
    BR_MEDIA_STREAM_OPT_START = 0x01,
} bs_br_media_stream_opt_type_t;
```

【注意事项】

无。

##### bs\_br\_media\_stream\_state\_t<a name="ZH-CN_TOPIC_0000002572783488"></a>

【结构体说明】

BR/EDR媒体A2DP流状态。

【结构体定义】

```
typedef enum {
    BR_MEDIA_STREAM_STARTING = 1,               /*!< @if Eng when start request is reported,stream is starting state.
                                                 @else   启播请求上报时，流状态是启播请求状态。 @endif */
    BR_MEDIA_STREAM_STREAMING,              /*!< @if Eng when start request is accepted,stream is streaming state.
                                                 @else   启播请求接受后，流状态是播放状态。 @endif */
    BR_MEDIA_STREAM_STOPED,                 /*!< @if Eng when suspend request is accepted,stream is stoped state.
                                                 @else   停止请求接受后，流状态是停止状态。 @endif */
    BR_MEDIA_STREAM_CLOSED,                 /*!< @if Eng when service is disconnected,stream is closed state.
                                                 @else   服务断连时，流状态是关闭状态。 @endif */
} bs_br_media_stream_state_t;
```

【注意事项】

无。

##### bs\_br\_media\_sbc\_sf\_t<a name="ZH-CN_TOPIC_0000002603222969"></a>

【结构体说明】

BR/EDR SBC采样率类型。

【结构体定义】

```
typedef enum {
    BR_MEDIA_SBC_SF_16000 = 0x80,        /*!< @if Eng 16KHz Sampling Frequency.
                                            @else   16KHz采样率类型。 @endif */
    BR_MEDIA_SBC_SF_32000 = 0x40,        /*!< @if Eng 32KHz Sampling Frequency.
                                            @else   32KHz采样率类型。 @endif */
    BR_MEDIA_SBC_SF_44100 = 0x20,        /*!< @if Eng 44.1KHz Sampling Frequency.
                                            @else   44.1KHz采样率类型。 @endif */
    BR_MEDIA_SBC_SF_48000 = 0x10,        /*!< @if Eng 48KHz Sampling Frequency.
                                            @else   48KHz采样率类型。 @endif */
    BR_MEDIA_SBC_SF_ALL   = 0xF0,        /*!< @if Eng All of the above Sampling Frequency.
                                            @else   以上全部采样率类型。 @endif */
} bs_br_media_sbc_sf_t;
```

【注意事项】

无。

##### bs\_br\_media\_sbc\_chmode\_t<a name="ZH-CN_TOPIC_0000002603342935"></a>

【结构体说明】

BR/EDR SBC通道模式类型。

【结构体定义】

```
typedef enum {
    BR_MEDIA_SBC_CHMODE_MONO         = 0x08,     /*!< @if Eng MONO channel mode.
                                                    @else   单声道。 @endif */
    BR_MEDIA_SBC_CHMODE_DUAL_CHANNEL = 0x04,     /*!< @if Eng dual channel channel mode.
                                                    @else   多通道。 @endif */
    BR_MEDIA_SBC_CHMODE_STEREO       = 0x02,     /*!< @if Eng stereo channel mode.
                                                    @else   立体声。 @endif */
    BR_MEDIA_SBC_CHMODE_JOINT_STEREO = 0x01,     /*!< @if Eng joint stereo channel mode.
                                                    @else   联合立体声。 @endif */
    BR_MEDIA_SBC_CHMODE_ALL          = 0x0F,     /*!< @if Eng All of the above channel mode.
                                                    @else   以上全部通道模式。 @endif */
} bs_br_media_sbc_chmode_t;
```

【注意事项】

无。

##### bs\_br\_media\_sbc\_subband\_t<a name="ZH-CN_TOPIC_0000002572783506"></a>

【结构体说明】

BR/EDR SBC子带个数类型。

【结构体定义】

```
typedef enum {
    BR_MEDIA_SBC_SUBBAND_4   = 0x08,     /*!< @if Eng The number of subbands is 4.
                                            @else   子带个数为4。 @endif */
    BR_MEDIA_SBC_SUBBAND_8   = 0x04,     /*!< @if Eng The number of subbands is 8.
                                            @else   子带个数为8。 @endif */
    BR_MEDIA_SBC_SUBBAND_ALL = 0x0C,     /*!< @if Eng All of the above subands number.
                                            @else   以上全部子带个数类型。 @endif */
} bs_br_media_sbc_subband_t;
```

【注意事项】

无。

##### bs\_br\_media\_sbc\_allocation\_t<a name="ZH-CN_TOPIC_0000002572783564"></a>

【结构体说明】

BR/EDR SBC分配方式类型。

【结构体定义】

```
typedef enum {
    BR_MEDIA_SBC_SNR       = 0x02,   /*!< @if Eng SNR allocation method.
                                        @else   信噪比优化。 @endif */
    BR_MEDIA_SBC_LOUDNESS  = 0x01,   /*!< @if Eng Loundness allocation method.
                                        @else   响度优化。 @endif */
    BR_MEDIA_SBC_ALLOC_ALL = 0x03,   /*!< @if Eng All of the above allocation method.
                                        @else   以上全部分配方式。 @endif */
} bs_br_media_sbc_allocation_t;
```

【注意事项】

无。

##### bs\_br\_media\_aac\_object\_type\_t<a name="ZH-CN_TOPIC_0000002603223025"></a>

【结构体说明】

BR/EDR MPEG-2、MPEG-4 AAC对象类型。

【结构体定义】

```
typedef enum {
    BR_MEDIA_AAC_OBJECT_TYPE_2_LC       = 0x80,  /*!< @if Eng MPEG-2 AAC LC object type.
                                                    @else   MPEG-2 AAC低复杂度。 @endif */
    BR_MEDIA_AAC_OBJECT_TYPE_4_LC       = 0x40,  /*!< @if Eng MPEG-4 AAC LC object type.
                                                    @else   MPEG-4 AAC低复杂度。 @endif */
    BR_MEDIA_AAC_OBJECT_TYPE_4_LTP      = 0x20,  /*!< @if Eng MPEG-4 AAC LTP object type.
                                                    @else   MPEG-4 AAC LTP。 @endif */
    BR_MEDIA_AAC_OBJECT_TYPE_4_SCALABLE = 0x10,  /*!< @if Eng MPEG-4 AAC Scalable object type.
                                                    @else   MPEG-4 AAC Scalable。 @endif */
    BR_MEDIA_AAC_OBJECT_TYPE_ALL        = 0xF0,  /*!< @if Eng All of the above object type.
                                                    @else   以上全部对象类型。 @endif */
} bs_br_media_aac_object_type_t;
```

【注意事项】

无。

##### bs\_br\_media\_aac\_sf\_t<a name="ZH-CN_TOPIC_0000002572623920"></a>

【结构体说明】

BR/EDR MPEG-2、MPEG-4 AAC采样率类型。

【结构体定义】

```
typedef enum {
    BR_MEDIA_AAC_SF_8000  = 0x0800,      /*!< @if Eng 8KHz Sampling Frequency.
                                            @else 8KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_11025 = 0x0400,      /*!< @if Eng 11.025KHz Sampling Frequency.
                                            @else 11.025KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_12000 = 0x0200,      /*!< @if Eng 12KHz Sampling Frequency.
                                            @else 12KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_16000 = 0x0100,      /*!< @if Eng 16KHz Sampling Frequency.
                                            @else 16KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_22050 = 0x0080,      /*!< @if Eng 22.05KHz Sampling Frequency.
                                            @else 22.05KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_24000 = 0x0040,      /*!< @if Eng 24KHz Sampling Frequency.
                                            @else 24KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_32000 = 0x0020,      /*!< @if Eng 32KHz Sampling Frequency.
                                            @else 32KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_44100 = 0x0010,      /*!< @if Eng 44.1KHz Sampling Frequency.
                                            @else 44.1KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_48000 = 0x0008,      /*!< @if Eng 48KHz Sampling Frequency.
                                            @else 48KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_64000 = 0x0004,      /*!< @if Eng 64KHz Sampling Frequency.
                                            @else 64KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_88200 = 0x0002,      /*!< @if Eng 88.2KHz Sampling Frequency.
                                            @else 88.2KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_96000 = 0x0001,      /*!< @if Eng 96KHz Sampling Frequency.
                                            @else 96KHz采样率类型。 @endif */
} bs_br_media_aac_sf_t;
```

【注意事项】

无。

##### bs\_br\_media\_aac\_channel\_t<a name="ZH-CN_TOPIC_0000002603342937"></a>

【结构体说明】

BR/EDR MPEG-2、MPEG-4 AAC通道类型。

【结构体定义】

```
typedef enum {
    BR_MEDIA_AAC_CHANNEL_1   = 0x08,     /*!< @if Eng 1 channel.
                                            @else  1通道。 @endif */
    BR_MEDIA_AAC_CHANNEL_2   = 0x04,     /*!< @if Eng 2 channel.
                                            @else  2通道。 @endif */
    BR_MEDIA_AAC_CHANNEL_ALL = 0x0C,     /*!< @if Eng All of the above channels.
                                            @else   以上全部通道类型。 @endif */
} bs_br_media_aac_channel_t;
```

【注意事项】

无。

##### bs\_br\_media\_aac\_vbr\_t<a name="ZH-CN_TOPIC_0000002603343027"></a>

【结构体说明】

BR/EDR MPEG-2、MPEG-4 AAC可变比特率类型。

【结构体定义】

```
typedef enum {
    BR_MEDIA_AAC_VBR_SUPPORTED = 0x80,    /*!< @if Eng Variable Bit Rate supported.
                                            @else  支持可变比特率类型。 @endif */
} bs_br_media_aac_vbr_t;
```

【注意事项】

无。

##### bs\_br\_media\_aac\_codec\_t<a name="ZH-CN_TOPIC_0000002603342969"></a>

【结构体说明】

BR/EDR MPEG4 AAC ISO 14496-3编码格式信息。

【结构体定义】

```
typedef struct {
    uint16_t sample_rate;  /*!< @if Eng Sample frequency, @See bs_br_media_aac_sf_t.
                                    @else 采样频率，@参考 bs_br_media_aac_sf_t @endif */
    uint32_t bit_rate;          /*!< @if Eng bit rate.
                                    @else 比特率。 @endif */
    uint8_t channel;           /*!< @if Eng channels, @See bs_br_media_aac_channel_t.
                                    @else 通道，@参考 bs_br_media_aac_channel_t @endif */
    uint8_t object_type;        /*!< @if Eng object type, @See bs_br_media_aac_object_type_t.
                                    @else 对象类型，@参考 bs_br_media_aac_object_type_t @endif */
    uint8_t rfa;                /*!< @if Eng Reserved for future use.
                                    @else 预留 @endif */
    uint8_t vbr;                /*!< @if Eng Variable Bit Rate supported, @See bs_br_media_aac_vbr_t.
                                    @else 支持动态比特率, @参考 bs_br_media_aac_vbr_t @endif */
} bs_br_media_aac_codec_t;
```

【注意事项】

无。

##### bs\_br\_media\_sbc\_codec\_t<a name="ZH-CN_TOPIC_0000002572623862"></a>

【结构体说明】

BR/EDR SBC编码格式信息。

【结构体定义】

```
typedef struct {
    uint8_t sample_rate;    /*!< @if Eng Sample frequency, @See bs_br_media_sbc_sf_t.
                                     @else   采样频率，@参考 bs_br_media_sbc_sf_t @endif */
    uint8_t channel;            /*!< @if Eng Channel mode, @See bs_br_media_sbc_chmode_t.
                                     @else 通道模式  @参考 bs_br_media_sbc_chmode_t @endif */
    uint8_t block_length;         /*!< @if Eng Block length, @See bs_br_media_sbc_block_t.
                                     @else   块长度, @参考 bs_br_media_sbc_block_t @endif */
    uint8_t subband;              /*!< @if Eng Num of subband, @See bs_br_media_sbc_subband_t.
                                     @else   子带个数, @参考 bs_br_media_sbc_subband_t @endif */
    uint8_t alloc_method;         /*!< @if Eng Allocation Method,@See bs_br_media_sbc_allocation_t.
                                     @else Allocation Method, @参考 bs_br_media_sbc_allocation_t @endif */
    uint8_t min_bitpool;          /*!< @if Eng Min bitpool value 2~250.
                                     @else   最位池，取值 2~250。 @endif */
    uint8_t max_bitpool;          /*!< @if Eng Max bitpool value 2~250.
                                     @else   最位池，值 2~250。 @endif */
} bs_br_media_sbc_codec_t;
```

【注意事项】

无。

##### bs\_br\_media\_codec\_type\_t<a name="ZH-CN_TOPIC_0000002572783470"></a>

【结构体说明】

BR/EDR媒体流编码格式类型。

【结构体定义】

```
typedef enum {
    BR_MEDIA_STREAM_CODEC_SBC = 0x00,          /*!< @if Eng SBC codec type. @else SBC编码格式类型。 @endif */
    BR_MEDIA_STREAM_CODEC_AAC = 0x01,       /*!< @if Eng MPEG24/AAC codec type.
                                                    @else MPEG24/AAC编码格式类型。 @endif */
} bs_br_media_codec_type_t;
```

【注意事项】

无。

##### bs\_br\_media\_codec\_info\_t<a name="ZH-CN_TOPIC_0000002603343029"></a>

【结构体说明】

BR/EDR A2DP编码格式参数定义。

【结构体定义】

```
typedef struct {
    uint8_t codec_type;             /*!< @if Eng The stream codec type, @See bs_br_media_codec_type_t.
                                         @else 音频流编解码类型, @参考 bs_br_media_codec_type_t @endif */
    union {
        bs_br_media_sbc_codec_t sbc;   /*!< @if Eng SBC codec info.
                                        @else SBC编解码信息。 @endif */
        bs_br_media_aac_codec_t aac;   /*!< @if Eng AAC codec info.
                                        @else AAC编解码信息。 @endif */
    } info;
} bs_br_media_codec_info_t;
```

【注意事项】

无。

##### bs\_br\_media\_stream\_callbacks\_t<a name="ZH-CN_TOPIC_0000002603223021"></a>

【结构体说明】

BR/EDR媒体流回调函数接口定义结构体。

【结构体定义】

```
typedef struct {
    bs_br_media_a2dp_conn_state_cbk connect_state_cbk;
    bs_br_media_stream_state_changed_cbk stream_state_cbk;
    bs_br_media_stream_codec_report_cbk stream_codec_cbk;
} bs_br_media_stream_callbacks_t;
```

【注意事项】

无。

#### 函数接口<a name="ZH-CN_TOPIC_0000002572623834"></a>

-   **[bs\_br\_media\_a2dp\_register](#ZH-CN_TOPIC_0000002572623824)**  

-   **[bs\_br\_media\_a2dp\_unregister](#ZH-CN_TOPIC_0000002572623840)**  

-   **[bs\_br\_media\_a2dp\_connect](#ZH-CN_TOPIC_0000002603342947)**  

-   **[bs\_br\_media\_a2dp\_disconnect](#ZH-CN_TOPIC_0000002603342929)**  

-   **[bs\_br\_start\_media\_stream](#ZH-CN_TOPIC_0000002603342917)**  

-   **[bs\_br\_stop\_media\_stream](#ZH-CN_TOPIC_0000002572623846)**  

-   **[bs\_br\_media\_stream\_operate\_result\_reply](#ZH-CN_TOPIC_0000002603342945)**  

-   **[bs\_br\_media\_stream\_register\_callbacks](#ZH-CN_TOPIC_0000002603222983)**  

-   **[bs\_br\_media\_stream\_unregister\_callbacks](#ZH-CN_TOPIC_0000002572783498)**  

##### bs\_br\_media\_a2dp\_register<a name="ZH-CN_TOPIC_0000002572623824"></a>

【描述】

BR/EDR注册蓝牙A2DP服务。

【接口定义】

```
errcode_t bs_br_media_a2dp_register(uint8_t a2dp_role);
```

【参数】

<a name="zh-cn_topic_0000002555144754_table1471mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555144754_row1477mcpsimp"><th class="cellrowborder" valign="top" width="24.47%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555144754_p1479mcpsimp"><a name="zh-cn_topic_0000002555144754_p1479mcpsimp"></a><a name="zh-cn_topic_0000002555144754_p1479mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="55.089999999999996%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555144754_p1481mcpsimp"><a name="zh-cn_topic_0000002555144754_p1481mcpsimp"></a><a name="zh-cn_topic_0000002555144754_p1481mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555144754_p1483mcpsimp"><a name="zh-cn_topic_0000002555144754_p1483mcpsimp"></a><a name="zh-cn_topic_0000002555144754_p1483mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555144754_row1484mcpsimp"><td class="cellrowborder" valign="top" width="24.47%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555144754_p1486mcpsimp"><a name="zh-cn_topic_0000002555144754_p1486mcpsimp"></a><a name="zh-cn_topic_0000002555144754_p1486mcpsimp"></a>a2dp_role</p>
</td>
<td class="cellrowborder" valign="top" width="55.089999999999996%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555144754_p1488mcpsimp"><a name="zh-cn_topic_0000002555144754_p1488mcpsimp"></a><a name="zh-cn_topic_0000002555144754_p1488mcpsimp"></a>本地服务角色。详见“<a href="#ZH-CN_TOPIC_0000002572623844">bs_br_media_a2dp_role_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555144754_p1490mcpsimp"><a name="zh-cn_topic_0000002555144754_p1490mcpsimp"></a><a name="zh-cn_topic_0000002555144754_p1490mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555144754_table1514mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555144754_row1520mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555144754_p1522mcpsimp"><a name="zh-cn_topic_0000002555144754_p1522mcpsimp"></a><a name="zh-cn_topic_0000002555144754_p1522mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555144754_p1524mcpsimp"><a name="zh-cn_topic_0000002555144754_p1524mcpsimp"></a><a name="zh-cn_topic_0000002555144754_p1524mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555144754_p1526mcpsimp"><a name="zh-cn_topic_0000002555144754_p1526mcpsimp"></a><a name="zh-cn_topic_0000002555144754_p1526mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555144754_row1527mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555144754_p1529mcpsimp"><a name="zh-cn_topic_0000002555144754_p1529mcpsimp"></a><a name="zh-cn_topic_0000002555144754_p1529mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555144754_p1531mcpsimp"><a name="zh-cn_topic_0000002555144754_p1531mcpsimp"></a><a name="zh-cn_topic_0000002555144754_p1531mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555144754_p1532mcpsimp"><a name="zh-cn_topic_0000002555144754_p1532mcpsimp"></a><a name="zh-cn_topic_0000002555144754_p1532mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555144754_p1534mcpsimp"><a name="zh-cn_topic_0000002555144754_p1534mcpsimp"></a><a name="zh-cn_topic_0000002555144754_p1534mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_a2dp\_unregister<a name="ZH-CN_TOPIC_0000002572623840"></a>

【描述】

BR/EDR去注册蓝牙A2DP服务。

【接口定义】

```
errcode_t bs_br_media_a2dp_unregister(uint8_t a2dp_role);
```

【参数】

<a name="zh-cn_topic_0000002554985126_table1356mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002554985126_row1362mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002554985126_p1364mcpsimp"><a name="zh-cn_topic_0000002554985126_p1364mcpsimp"></a><a name="zh-cn_topic_0000002554985126_p1364mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.529999999999994%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002554985126_p1366mcpsimp"><a name="zh-cn_topic_0000002554985126_p1366mcpsimp"></a><a name="zh-cn_topic_0000002554985126_p1366mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.47%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002554985126_p1368mcpsimp"><a name="zh-cn_topic_0000002554985126_p1368mcpsimp"></a><a name="zh-cn_topic_0000002554985126_p1368mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002554985126_row1369mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985126_p1486mcpsimp"><a name="zh-cn_topic_0000002554985126_p1486mcpsimp"></a><a name="zh-cn_topic_0000002554985126_p1486mcpsimp"></a>a2dp_role</p>
</td>
<td class="cellrowborder" valign="top" width="61.529999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985126_p1488mcpsimp"><a name="zh-cn_topic_0000002554985126_p1488mcpsimp"></a><a name="zh-cn_topic_0000002554985126_p1488mcpsimp"></a>本地服务角色。详见“<a href="#ZH-CN_TOPIC_0000002572623844">bs_br_media_a2dp_role_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985126_p1490mcpsimp"><a name="zh-cn_topic_0000002554985126_p1490mcpsimp"></a><a name="zh-cn_topic_0000002554985126_p1490mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002554985126_table1377mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002554985126_row1383mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002554985126_p1385mcpsimp"><a name="zh-cn_topic_0000002554985126_p1385mcpsimp"></a><a name="zh-cn_topic_0000002554985126_p1385mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002554985126_p1387mcpsimp"><a name="zh-cn_topic_0000002554985126_p1387mcpsimp"></a><a name="zh-cn_topic_0000002554985126_p1387mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002554985126_p1389mcpsimp"><a name="zh-cn_topic_0000002554985126_p1389mcpsimp"></a><a name="zh-cn_topic_0000002554985126_p1389mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002554985126_row1390mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985126_p1392mcpsimp"><a name="zh-cn_topic_0000002554985126_p1392mcpsimp"></a><a name="zh-cn_topic_0000002554985126_p1392mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985126_p1394mcpsimp"><a name="zh-cn_topic_0000002554985126_p1394mcpsimp"></a><a name="zh-cn_topic_0000002554985126_p1394mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002554985126_p1395mcpsimp"><a name="zh-cn_topic_0000002554985126_p1395mcpsimp"></a><a name="zh-cn_topic_0000002554985126_p1395mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985126_p1397mcpsimp"><a name="zh-cn_topic_0000002554985126_p1397mcpsimp"></a><a name="zh-cn_topic_0000002554985126_p1397mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_a2dp\_connect<a name="ZH-CN_TOPIC_0000002603342947"></a>

【描述】

BR/EDR建立蓝牙媒体A2DP连接。

【接口定义】

```
errcode_t bs_br_media_a2dp_connect(uint16_t dev_id, uint8_t a2dp_role);
```

【参数】

<a name="zh-cn_topic_0000002585704749_table1356mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585704749_row1362mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585704749_p1364mcpsimp"><a name="zh-cn_topic_0000002585704749_p1364mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1364mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.529999999999994%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585704749_p1366mcpsimp"><a name="zh-cn_topic_0000002585704749_p1366mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1366mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.47%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585704749_p1368mcpsimp"><a name="zh-cn_topic_0000002585704749_p1368mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1368mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585704749_row1369mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704749_p1371mcpsimp"><a name="zh-cn_topic_0000002585704749_p1371mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1371mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.529999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704749_p1373mcpsimp"><a name="zh-cn_topic_0000002585704749_p1373mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1373mcpsimp"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704749_p1375mcpsimp"><a name="zh-cn_topic_0000002585704749_p1375mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1375mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585704749_row3256212164519"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704749_p1486mcpsimp"><a name="zh-cn_topic_0000002585704749_p1486mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1486mcpsimp"></a>a2dp_role</p>
</td>
<td class="cellrowborder" valign="top" width="61.529999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704749_p1488mcpsimp"><a name="zh-cn_topic_0000002585704749_p1488mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1488mcpsimp"></a>本地服务角色。详见“<a href="#ZH-CN_TOPIC_0000002572623844">bs_br_media_a2dp_role_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704749_p1490mcpsimp"><a name="zh-cn_topic_0000002585704749_p1490mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1490mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585704749_table1377mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585704749_row1383mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585704749_p1385mcpsimp"><a name="zh-cn_topic_0000002585704749_p1385mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1385mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585704749_p1387mcpsimp"><a name="zh-cn_topic_0000002585704749_p1387mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1387mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585704749_p1389mcpsimp"><a name="zh-cn_topic_0000002585704749_p1389mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1389mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585704749_row1390mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704749_p1392mcpsimp"><a name="zh-cn_topic_0000002585704749_p1392mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1392mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704749_p1394mcpsimp"><a name="zh-cn_topic_0000002585704749_p1394mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1394mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585704749_p1395mcpsimp"><a name="zh-cn_topic_0000002585704749_p1395mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1395mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704749_p1397mcpsimp"><a name="zh-cn_topic_0000002585704749_p1397mcpsimp"></a><a name="zh-cn_topic_0000002585704749_p1397mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_a2dp\_disconnect<a name="ZH-CN_TOPIC_0000002603342929"></a>

【描述】

BR/EDR断开蓝牙媒体A2DP连接。

【接口定义】

```
errcode_t bs_br_media_a2dp_disconnect(uint16_t dev_id, uint8_t a2dp_role);
```

【参数】

<a name="zh-cn_topic_0000002585584707_table1356mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585584707_row1362mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585584707_p1364mcpsimp"><a name="zh-cn_topic_0000002585584707_p1364mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1364mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.529999999999994%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585584707_p1366mcpsimp"><a name="zh-cn_topic_0000002585584707_p1366mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1366mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.47%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585584707_p1368mcpsimp"><a name="zh-cn_topic_0000002585584707_p1368mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1368mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585584707_row1369mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585584707_p1371mcpsimp"><a name="zh-cn_topic_0000002585584707_p1371mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1371mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.529999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585584707_p1373mcpsimp"><a name="zh-cn_topic_0000002585584707_p1373mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1373mcpsimp"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585584707_p1375mcpsimp"><a name="zh-cn_topic_0000002585584707_p1375mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1375mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585584707_row3256212164519"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585584707_p1486mcpsimp"><a name="zh-cn_topic_0000002585584707_p1486mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1486mcpsimp"></a>a2dp_role</p>
</td>
<td class="cellrowborder" valign="top" width="61.529999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585584707_p1488mcpsimp"><a name="zh-cn_topic_0000002585584707_p1488mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1488mcpsimp"></a>本地服务角色。详见“<a href="#ZH-CN_TOPIC_0000002572623844">bs_br_media_a2dp_role_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585584707_p1490mcpsimp"><a name="zh-cn_topic_0000002585584707_p1490mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1490mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585584707_table1377mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585584707_row1383mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585584707_p1385mcpsimp"><a name="zh-cn_topic_0000002585584707_p1385mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1385mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585584707_p1387mcpsimp"><a name="zh-cn_topic_0000002585584707_p1387mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1387mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585584707_p1389mcpsimp"><a name="zh-cn_topic_0000002585584707_p1389mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1389mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585584707_row1390mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585584707_p1392mcpsimp"><a name="zh-cn_topic_0000002585584707_p1392mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1392mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585584707_p1394mcpsimp"><a name="zh-cn_topic_0000002585584707_p1394mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1394mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585584707_p1395mcpsimp"><a name="zh-cn_topic_0000002585584707_p1395mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1395mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585584707_p1397mcpsimp"><a name="zh-cn_topic_0000002585584707_p1397mcpsimp"></a><a name="zh-cn_topic_0000002585584707_p1397mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_start\_media\_stream<a name="ZH-CN_TOPIC_0000002603342917"></a>

【描述】

BR/EDR蓝牙媒体音频流启播请求。

【接口定义】

```
errcode_t bs_br_start_media_stream(uint16_t dev_id);
```

【参数】

<a name="zh-cn_topic_0000002555144756_table1356mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555144756_row1362mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555144756_p1364mcpsimp"><a name="zh-cn_topic_0000002555144756_p1364mcpsimp"></a><a name="zh-cn_topic_0000002555144756_p1364mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.529999999999994%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555144756_p1366mcpsimp"><a name="zh-cn_topic_0000002555144756_p1366mcpsimp"></a><a name="zh-cn_topic_0000002555144756_p1366mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.47%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555144756_p1368mcpsimp"><a name="zh-cn_topic_0000002555144756_p1368mcpsimp"></a><a name="zh-cn_topic_0000002555144756_p1368mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555144756_row1369mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555144756_p1371mcpsimp"><a name="zh-cn_topic_0000002555144756_p1371mcpsimp"></a><a name="zh-cn_topic_0000002555144756_p1371mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.529999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555144756_p1373mcpsimp"><a name="zh-cn_topic_0000002555144756_p1373mcpsimp"></a><a name="zh-cn_topic_0000002555144756_p1373mcpsimp"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555144756_p1375mcpsimp"><a name="zh-cn_topic_0000002555144756_p1375mcpsimp"></a><a name="zh-cn_topic_0000002555144756_p1375mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555144756_table1377mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555144756_row1383mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555144756_p1385mcpsimp"><a name="zh-cn_topic_0000002555144756_p1385mcpsimp"></a><a name="zh-cn_topic_0000002555144756_p1385mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555144756_p1387mcpsimp"><a name="zh-cn_topic_0000002555144756_p1387mcpsimp"></a><a name="zh-cn_topic_0000002555144756_p1387mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555144756_p1389mcpsimp"><a name="zh-cn_topic_0000002555144756_p1389mcpsimp"></a><a name="zh-cn_topic_0000002555144756_p1389mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555144756_row1390mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555144756_p1392mcpsimp"><a name="zh-cn_topic_0000002555144756_p1392mcpsimp"></a><a name="zh-cn_topic_0000002555144756_p1392mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555144756_p1394mcpsimp"><a name="zh-cn_topic_0000002555144756_p1394mcpsimp"></a><a name="zh-cn_topic_0000002555144756_p1394mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555144756_p1395mcpsimp"><a name="zh-cn_topic_0000002555144756_p1395mcpsimp"></a><a name="zh-cn_topic_0000002555144756_p1395mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555144756_p1397mcpsimp"><a name="zh-cn_topic_0000002555144756_p1397mcpsimp"></a><a name="zh-cn_topic_0000002555144756_p1397mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_stop\_media\_stream<a name="ZH-CN_TOPIC_0000002572623846"></a>

【描述】

BR/EDR蓝牙媒体音频流停止请求。

【接口定义】

```
errcode_t bs_br_stop_media_stream(uint16_t dev_id);
```

【参数】

<a name="zh-cn_topic_0000002554985128_table1356mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002554985128_row1362mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002554985128_p1364mcpsimp"><a name="zh-cn_topic_0000002554985128_p1364mcpsimp"></a><a name="zh-cn_topic_0000002554985128_p1364mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.529999999999994%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002554985128_p1366mcpsimp"><a name="zh-cn_topic_0000002554985128_p1366mcpsimp"></a><a name="zh-cn_topic_0000002554985128_p1366mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.47%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002554985128_p1368mcpsimp"><a name="zh-cn_topic_0000002554985128_p1368mcpsimp"></a><a name="zh-cn_topic_0000002554985128_p1368mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002554985128_row1369mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985128_p1371mcpsimp"><a name="zh-cn_topic_0000002554985128_p1371mcpsimp"></a><a name="zh-cn_topic_0000002554985128_p1371mcpsimp"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.529999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985128_p1373mcpsimp"><a name="zh-cn_topic_0000002554985128_p1373mcpsimp"></a><a name="zh-cn_topic_0000002554985128_p1373mcpsimp"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985128_p1375mcpsimp"><a name="zh-cn_topic_0000002554985128_p1375mcpsimp"></a><a name="zh-cn_topic_0000002554985128_p1375mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002554985128_table1377mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002554985128_row1383mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002554985128_p1385mcpsimp"><a name="zh-cn_topic_0000002554985128_p1385mcpsimp"></a><a name="zh-cn_topic_0000002554985128_p1385mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002554985128_p1387mcpsimp"><a name="zh-cn_topic_0000002554985128_p1387mcpsimp"></a><a name="zh-cn_topic_0000002554985128_p1387mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002554985128_p1389mcpsimp"><a name="zh-cn_topic_0000002554985128_p1389mcpsimp"></a><a name="zh-cn_topic_0000002554985128_p1389mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002554985128_row1390mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002554985128_p1392mcpsimp"><a name="zh-cn_topic_0000002554985128_p1392mcpsimp"></a><a name="zh-cn_topic_0000002554985128_p1392mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002554985128_p1394mcpsimp"><a name="zh-cn_topic_0000002554985128_p1394mcpsimp"></a><a name="zh-cn_topic_0000002554985128_p1394mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002554985128_p1395mcpsimp"><a name="zh-cn_topic_0000002554985128_p1395mcpsimp"></a><a name="zh-cn_topic_0000002554985128_p1395mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002554985128_p1397mcpsimp"><a name="zh-cn_topic_0000002554985128_p1397mcpsimp"></a><a name="zh-cn_topic_0000002554985128_p1397mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_stream\_operate\_result\_reply<a name="ZH-CN_TOPIC_0000002603342945"></a>

【描述】

BR/EDR蓝牙媒体音频流操作响应。

【接口定义】

```
errcode_t bs_br_media_stream_operate_result_reply(uint16_t dev_id, uint8_t opt_type, bool is_accept);
```

【参数】

<a name="zh-cn_topic_0000002585704751_table1356mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585704751_row1362mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585704751_p1364mcpsimp"><a name="zh-cn_topic_0000002585704751_p1364mcpsimp"></a><a name="zh-cn_topic_0000002585704751_p1364mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.529999999999994%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585704751_p1366mcpsimp"><a name="zh-cn_topic_0000002585704751_p1366mcpsimp"></a><a name="zh-cn_topic_0000002585704751_p1366mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.47%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585704751_p1368mcpsimp"><a name="zh-cn_topic_0000002585704751_p1368mcpsimp"></a><a name="zh-cn_topic_0000002585704751_p1368mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585704751_row1369mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704751_p1234824212479"><a name="zh-cn_topic_0000002585704751_p1234824212479"></a><a name="zh-cn_topic_0000002585704751_p1234824212479"></a>dev_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.529999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704751_p53481642154714"><a name="zh-cn_topic_0000002585704751_p53481642154714"></a><a name="zh-cn_topic_0000002585704751_p53481642154714"></a>对端设备ID。</p>
</td>
<td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704751_p5348144218474"><a name="zh-cn_topic_0000002585704751_p5348144218474"></a><a name="zh-cn_topic_0000002585704751_p5348144218474"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585704751_row1032453616479"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704751_p13324113654713"><a name="zh-cn_topic_0000002585704751_p13324113654713"></a><a name="zh-cn_topic_0000002585704751_p13324113654713"></a>opt_type</p>
</td>
<td class="cellrowborder" valign="top" width="61.529999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704751_p173241236154710"><a name="zh-cn_topic_0000002585704751_p173241236154710"></a><a name="zh-cn_topic_0000002585704751_p173241236154710"></a>操作类型。详见“<a href="#ZH-CN_TOPIC_0000002572623852">bs_br_media_stream_opt_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704751_p144288369483"><a name="zh-cn_topic_0000002585704751_p144288369483"></a><a name="zh-cn_topic_0000002585704751_p144288369483"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585704751_row1987833318477"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704751_p38781533124717"><a name="zh-cn_topic_0000002585704751_p38781533124717"></a><a name="zh-cn_topic_0000002585704751_p38781533124717"></a>is_accept</p>
</td>
<td class="cellrowborder" valign="top" width="61.529999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704751_p687823314713"><a name="zh-cn_topic_0000002585704751_p687823314713"></a><a name="zh-cn_topic_0000002585704751_p687823314713"></a>是否接受。true：接受，false：拒绝。</p>
</td>
<td class="cellrowborder" valign="top" width="21.47%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704751_p578416360489"><a name="zh-cn_topic_0000002585704751_p578416360489"></a><a name="zh-cn_topic_0000002585704751_p578416360489"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585704751_table1377mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585704751_row1383mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585704751_p1385mcpsimp"><a name="zh-cn_topic_0000002585704751_p1385mcpsimp"></a><a name="zh-cn_topic_0000002585704751_p1385mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585704751_p1387mcpsimp"><a name="zh-cn_topic_0000002585704751_p1387mcpsimp"></a><a name="zh-cn_topic_0000002585704751_p1387mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585704751_p1389mcpsimp"><a name="zh-cn_topic_0000002585704751_p1389mcpsimp"></a><a name="zh-cn_topic_0000002585704751_p1389mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585704751_row1390mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585704751_p1392mcpsimp"><a name="zh-cn_topic_0000002585704751_p1392mcpsimp"></a><a name="zh-cn_topic_0000002585704751_p1392mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585704751_p1394mcpsimp"><a name="zh-cn_topic_0000002585704751_p1394mcpsimp"></a><a name="zh-cn_topic_0000002585704751_p1394mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585704751_p1395mcpsimp"><a name="zh-cn_topic_0000002585704751_p1395mcpsimp"></a><a name="zh-cn_topic_0000002585704751_p1395mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585704751_p1397mcpsimp"><a name="zh-cn_topic_0000002585704751_p1397mcpsimp"></a><a name="zh-cn_topic_0000002585704751_p1397mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_stream\_register\_callbacks<a name="ZH-CN_TOPIC_0000002603222983"></a>

【描述】

BR/EDR注册媒体流回调函数。

【接口定义】

```
errcode_t bs_br_media_stream_register_callbacks(uint8_t app_id, bs_br_media_stream_callbacks_t *func);
```

【参数】

<a name="zh-cn_topic_0000002585584709_table1787mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585584709_row1793mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585584709_p1795mcpsimp"><a name="zh-cn_topic_0000002585584709_p1795mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1795mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.029999999999994%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585584709_p1797mcpsimp"><a name="zh-cn_topic_0000002585584709_p1797mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1797mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.97%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585584709_p1799mcpsimp"><a name="zh-cn_topic_0000002585584709_p1799mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1799mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585584709_row1800mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585584709_p1802mcpsimp"><a name="zh-cn_topic_0000002585584709_p1802mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1802mcpsimp"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="62.029999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585584709_p1804mcpsimp"><a name="zh-cn_topic_0000002585584709_p1804mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1804mcpsimp"></a>应用ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.97%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585584709_p1806mcpsimp"><a name="zh-cn_topic_0000002585584709_p1806mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1806mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002585584709_row1807mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585584709_p1809mcpsimp"><a name="zh-cn_topic_0000002585584709_p1809mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1809mcpsimp"></a>func</p>
</td>
<td class="cellrowborder" valign="top" width="62.029999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585584709_p1811mcpsimp"><a name="zh-cn_topic_0000002585584709_p1811mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1811mcpsimp"></a>注册的回调函数。详见“<a href="#ZH-CN_TOPIC_0000002603223021">bs_br_media_stream_callbacks_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.97%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585584709_p1813mcpsimp"><a name="zh-cn_topic_0000002585584709_p1813mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1813mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002585584709_table1815mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002585584709_row1821mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002585584709_p1823mcpsimp"><a name="zh-cn_topic_0000002585584709_p1823mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1823mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002585584709_p1825mcpsimp"><a name="zh-cn_topic_0000002585584709_p1825mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1825mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002585584709_p1827mcpsimp"><a name="zh-cn_topic_0000002585584709_p1827mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1827mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002585584709_row1828mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002585584709_p1830mcpsimp"><a name="zh-cn_topic_0000002585584709_p1830mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1830mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002585584709_p1832mcpsimp"><a name="zh-cn_topic_0000002585584709_p1832mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1832mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002585584709_p1833mcpsimp"><a name="zh-cn_topic_0000002585584709_p1833mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1833mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002585584709_p1835mcpsimp"><a name="zh-cn_topic_0000002585584709_p1835mcpsimp"></a><a name="zh-cn_topic_0000002585584709_p1835mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

##### bs\_br\_media\_stream\_unregister\_callbacks<a name="ZH-CN_TOPIC_0000002572783498"></a>

【描述】

BR/EDR去注册媒体流回调函数。

【接口定义】

```
errcode_t bs_br_media_stream_unregister_callbacks(uint8_t app_id);
```

【参数】

<a name="zh-cn_topic_0000002555451222_table1787mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555451222_row1793mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555451222_p1795mcpsimp"><a name="zh-cn_topic_0000002555451222_p1795mcpsimp"></a><a name="zh-cn_topic_0000002555451222_p1795mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.029999999999994%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555451222_p1797mcpsimp"><a name="zh-cn_topic_0000002555451222_p1797mcpsimp"></a><a name="zh-cn_topic_0000002555451222_p1797mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.97%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555451222_p1799mcpsimp"><a name="zh-cn_topic_0000002555451222_p1799mcpsimp"></a><a name="zh-cn_topic_0000002555451222_p1799mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555451222_row1800mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555451222_p1802mcpsimp"><a name="zh-cn_topic_0000002555451222_p1802mcpsimp"></a><a name="zh-cn_topic_0000002555451222_p1802mcpsimp"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="62.029999999999994%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555451222_p1804mcpsimp"><a name="zh-cn_topic_0000002555451222_p1804mcpsimp"></a><a name="zh-cn_topic_0000002555451222_p1804mcpsimp"></a>应用ID。</p>
</td>
<td class="cellrowborder" valign="top" width="20.97%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555451222_p1806mcpsimp"><a name="zh-cn_topic_0000002555451222_p1806mcpsimp"></a><a name="zh-cn_topic_0000002555451222_p1806mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002555451222_table1815mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002555451222_row1821mcpsimp"><th class="cellrowborder" valign="top" width="15.15%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002555451222_p1823mcpsimp"><a name="zh-cn_topic_0000002555451222_p1823mcpsimp"></a><a name="zh-cn_topic_0000002555451222_p1823mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63.63999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002555451222_p1825mcpsimp"><a name="zh-cn_topic_0000002555451222_p1825mcpsimp"></a><a name="zh-cn_topic_0000002555451222_p1825mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002555451222_p1827mcpsimp"><a name="zh-cn_topic_0000002555451222_p1827mcpsimp"></a><a name="zh-cn_topic_0000002555451222_p1827mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002555451222_row1828mcpsimp"><td class="cellrowborder" valign="top" width="15.15%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002555451222_p1830mcpsimp"><a name="zh-cn_topic_0000002555451222_p1830mcpsimp"></a><a name="zh-cn_topic_0000002555451222_p1830mcpsimp"></a>errcode_t</p>
</td>
<td class="cellrowborder" valign="top" width="63.63999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002555451222_p1832mcpsimp"><a name="zh-cn_topic_0000002555451222_p1832mcpsimp"></a><a name="zh-cn_topic_0000002555451222_p1832mcpsimp"></a>ERRCODE_BT_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002555451222_p1833mcpsimp"><a name="zh-cn_topic_0000002555451222_p1833mcpsimp"></a><a name="zh-cn_topic_0000002555451222_p1833mcpsimp"></a>其他：失败，错误码见errcode_bt_t，其定义在bts_def.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21.21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002555451222_p1835mcpsimp"><a name="zh-cn_topic_0000002555451222_p1835mcpsimp"></a><a name="zh-cn_topic_0000002555451222_p1835mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

## 开发示例<a name="ZH-CN_TOPIC_0000002572783576"></a>

>![](public_sys-resources/icon-notice.gif) **须知：** 
>示例代码仅用于展示基本处理流程，请勿直接部署至生产环境使用。

-   **[注册媒体控制服务](#ZH-CN_TOPIC_0000002572783508)**  

-   **[手表控制手机音乐播放](#ZH-CN_TOPIC_0000002603342981)**  

### 注册媒体控制服务<a name="ZH-CN_TOPIC_0000002572783508"></a>

1.  客户端参考“[开启BR/EDR扫描](#ZH-CN_TOPIC_0000002603223013)”中的步骤完成BR/EDR service回调注册、设备信息配置、BR/EDR协议栈使能。其中，设备信息配置时参考如下示例对设备的profile信息进行配置。

    ```
    bs_br_config_support_profile_t demo_profile = {0};
    demo_profile.a2dp_snk = true;
    demo_profile.a2dp_src = true;
    demo_profile.avrcp_ct = true;
    demo_profile.avrcp_tg = true;
    demo_profile.hfp_hf = true;
    demo_profile.hfp_ag = true;
    errcode_t ret = bs_config(BR_CONFIG_PROFILE_SUPPORT, &demo_profile, sizeof(demo_profile));
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

2.  注册媒体传输回调函数。

    ```
    void demo_connect_state_cbk(uint16_t dev_id, uint8_t a2dp_role, uint8_t conn_state)
    {
        // do your things here
    }
    
    void demo_stream_state_cbk(uint16_t dev_id, uint8_t a2dp_role, uint8_t stream_state)
    {
        // do your things here
    }
    
    void demo_stream_codec_cbk(uint16_t dev_id, uint8_t a2dp_role,
        bs_br_media_codec_info_t *codec)
    {
        // do your things here
    }
    
    bs_br_media_stream_callbacks_t demo_bs_br_media_stream_callbacks = {
        .connect_state_cbk = demo_connect_state_cbk,
        .stream_state_cbk = demo_stream_state_cbk,
        .stream_codec_cbk = demo_stream_codec_cbk,
    };
    
    uint8_t demo_app_id = 0;
    errcode_t ret = bs_br_media_stream_register_callbacks(demo_app_id, &demo_bs_br_media_stream_callbacks);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

3.  媒体传输服务初始化。

    ```
    bs_br_media_stream_init();
    ```

4.  注册媒体控制回调函数。

    ```
    void demo_connect_state_cbk(uint16_t dev_id, uint8_t avrcp_role, uint8_t conn_state)
    {
        // do your things here
    }
    
    void demo_media_opt_req_cbk(uint16_t dev_id, uint8_t opt_type, uint8_t *param, uint16_t param_len)
    {
        // do your things here
    }
    
    void demo_media_update_cbk(uint16_t dev_id, uint8_t type, uint8_t *param, uint16_t param_len)
    {
        // do your things here
    }
    
    void demo_media_volume_operate_cbk(uint16_t dev_id, uint8_t type, uint8_t volume)
    {
        // do your things here
    }
    
    void demo_media_volume_report_cbk(uint16_t dev_id, uint8_t volume)
    {
        // do your things here
    }
    
    bs_br_media_callbacks_t demo_bs_br_media_callbacks = {
            .connect_state_cbk = demo_connect_state_cbk,
            .media_opt_req_cbk = demo_media_opt_req_cbk,
            .media_update_cbk = demo_media_update_cbk,
            .media_volume_operate_cbk = demo_media_volume_operate_cbk,
            .media_volume_report_cbk = demo_media_volume_report_cbk,
    };
    
    uint8_t demo_app_id = 0;
    errcode_t ret = bs_br_media_register_callbacks(demo_app_id, &demo_bs_br_media_callbacks);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

5.  媒体控制服务初始化。

    ```
    bs_br_media_control_init();
    ```

6.  注册AVRCP CT服务。

    ```
    errcode_t result = bs_br_media_avrcp_register(BR_MEDIA_AVRCP_CT);
    if (result != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

7.  注册AVRCP TG服务。

    ```
    errcode_t result = bs_br_media_avrcp_register(BR_MEDIA_AVRCP_TG);
    if (result != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

### 手表控制手机音乐播放<a name="ZH-CN_TOPIC_0000002603342981"></a>

1.  手表参考“[注册媒体控制服务](#ZH-CN_TOPIC_0000002572783508)”进行配置。
2.  手表开启广播。

    ```
    errcode_t ret = bs_br_start_discovery(BR_DISCOVERY_PERIPHERAL);
    if (ret != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

3.  手机端打开蓝牙，扫描到该设备后点击进行连接。
4.  手表端根据注册的连接状态回调或配对结果回调获取对端设备ID dev\_id。
5.  手机端播放音乐。
6.  手表端暂停音乐。

    ```
    errcode_t result = bs_br_media_operate_request(dev_id, BR_MEDIA_OPT_PAUSE, NULL, 0);
    if (result != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

7.  手表端播放音乐。

    ```
    errcode_t result = bs_br_media_operate_request(dev_id, BR_MEDIA_OPT_PLAY, NULL, 0);
    if (result != ERRCODE_BT_SUCCESS) {
        // error handling
    }
    ```

# 缩略语<a name="ZH-CN_TOPIC_0000002603222987"></a>

<a name="zh-cn_topic_0000002599426193_table84264501102"></a>
<table><tbody><tr id="zh-cn_topic_0000002599426193_row18426450161018"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p10426550111011"><a name="zh-cn_topic_0000002599426193_p10426550111011"></a><a name="zh-cn_topic_0000002599426193_p10426550111011"></a><strong id="zh-cn_topic_0000002599426193_b335313578226"><a name="zh-cn_topic_0000002599426193_b335313578226"></a><a name="zh-cn_topic_0000002599426193_b335313578226"></a>A</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row22651459585"><td class="cellrowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p15265125115812"><a name="zh-cn_topic_0000002599426193_p15265125115812"></a><a name="zh-cn_topic_0000002599426193_p15265125115812"></a><strong id="zh-cn_topic_0000002599426193_b4171113385218"><a name="zh-cn_topic_0000002599426193_b4171113385218"></a><a name="zh-cn_topic_0000002599426193_b4171113385218"></a>AAC</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p18265165195819"><a name="zh-cn_topic_0000002599426193_p18265165195819"></a><a name="zh-cn_topic_0000002599426193_p18265165195819"></a>Advanced Audio Coding</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p1426510517584"><a name="zh-cn_topic_0000002599426193_p1426510517584"></a><a name="zh-cn_topic_0000002599426193_p1426510517584"></a>高级音频编码</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row20426205017101"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p1426155012107"><a name="zh-cn_topic_0000002599426193_p1426155012107"></a><a name="zh-cn_topic_0000002599426193_p1426155012107"></a><strong id="zh-cn_topic_0000002599426193_b6355185713221"><a name="zh-cn_topic_0000002599426193_b6355185713221"></a><a name="zh-cn_topic_0000002599426193_b6355185713221"></a>A2DP</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p13211416121417"><a name="zh-cn_topic_0000002599426193_p13211416121417"></a><a name="zh-cn_topic_0000002599426193_p13211416121417"></a>Advanced Audio Distribution Profile</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p9321151615149"><a name="zh-cn_topic_0000002599426193_p9321151615149"></a><a name="zh-cn_topic_0000002599426193_p9321151615149"></a>高级音频分发协议</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row4426145017105"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p7426105010106"><a name="zh-cn_topic_0000002599426193_p7426105010106"></a><a name="zh-cn_topic_0000002599426193_p7426105010106"></a><strong id="zh-cn_topic_0000002599426193_b3356155715225"><a name="zh-cn_topic_0000002599426193_b3356155715225"></a><a name="zh-cn_topic_0000002599426193_b3356155715225"></a>AVRCP</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p710811202145"><a name="zh-cn_topic_0000002599426193_p710811202145"></a><a name="zh-cn_topic_0000002599426193_p710811202145"></a>Audio/Video Remote Control Profile</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p12108182041417"><a name="zh-cn_topic_0000002599426193_p12108182041417"></a><a name="zh-cn_topic_0000002599426193_p12108182041417"></a>音视频远程控制协议</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row1842735014104"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row6427750141018"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p124271050111019"><a name="zh-cn_topic_0000002599426193_p124271050111019"></a><a name="zh-cn_topic_0000002599426193_p124271050111019"></a><strong id="zh-cn_topic_0000002599426193_b1335715782218"><a name="zh-cn_topic_0000002599426193_b1335715782218"></a><a name="zh-cn_topic_0000002599426193_b1335715782218"></a>B</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row64273501101"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p1342745014102"><a name="zh-cn_topic_0000002599426193_p1342745014102"></a><a name="zh-cn_topic_0000002599426193_p1342745014102"></a><strong id="zh-cn_topic_0000002599426193_b5357157142220"><a name="zh-cn_topic_0000002599426193_b5357157142220"></a><a name="zh-cn_topic_0000002599426193_b5357157142220"></a>BLE</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p1219545018128"><a name="zh-cn_topic_0000002599426193_p1219545018128"></a><a name="zh-cn_topic_0000002599426193_p1219545018128"></a>Bluetooth Low Energy</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p2195550121219"><a name="zh-cn_topic_0000002599426193_p2195550121219"></a><a name="zh-cn_topic_0000002599426193_p2195550121219"></a>低功耗蓝牙</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row18876191215125"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p178771012141216"><a name="zh-cn_topic_0000002599426193_p178771012141216"></a><a name="zh-cn_topic_0000002599426193_p178771012141216"></a><strong id="zh-cn_topic_0000002599426193_b835845712228"><a name="zh-cn_topic_0000002599426193_b835845712228"></a><a name="zh-cn_topic_0000002599426193_b835845712228"></a>BR</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p20277125151415"><a name="zh-cn_topic_0000002599426193_p20277125151415"></a><a name="zh-cn_topic_0000002599426193_p20277125151415"></a>Bluetooth Basic Rate</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p6277162501413"><a name="zh-cn_topic_0000002599426193_p6277162501413"></a><a name="zh-cn_topic_0000002599426193_p6277162501413"></a>蓝牙基本速率</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row6427175001012"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p9427115015103"><a name="zh-cn_topic_0000002599426193_p9427115015103"></a><a name="zh-cn_topic_0000002599426193_p9427115015103"></a><strong id="zh-cn_topic_0000002599426193_b133585578226"><a name="zh-cn_topic_0000002599426193_b133585578226"></a><a name="zh-cn_topic_0000002599426193_b133585578226"></a>BT</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p1523160161319"><a name="zh-cn_topic_0000002599426193_p1523160161319"></a><a name="zh-cn_topic_0000002599426193_p1523160161319"></a>Bluetooth</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p1524705138"><a name="zh-cn_topic_0000002599426193_p1524705138"></a><a name="zh-cn_topic_0000002599426193_p1524705138"></a>蓝牙</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row6625174180"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row1553717818218"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p75372083218"><a name="zh-cn_topic_0000002599426193_p75372083218"></a><a name="zh-cn_topic_0000002599426193_p75372083218"></a><strong id="zh-cn_topic_0000002599426193_b163599571225"><a name="zh-cn_topic_0000002599426193_b163599571225"></a><a name="zh-cn_topic_0000002599426193_b163599571225"></a>C</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row85385811215"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p953813810214"><a name="zh-cn_topic_0000002599426193_p953813810214"></a><a name="zh-cn_topic_0000002599426193_p953813810214"></a><strong id="zh-cn_topic_0000002599426193_b1236025713222"><a name="zh-cn_topic_0000002599426193_b1236025713222"></a><a name="zh-cn_topic_0000002599426193_b1236025713222"></a>CT</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p1253878162117"><a name="zh-cn_topic_0000002599426193_p1253878162117"></a><a name="zh-cn_topic_0000002599426193_p1253878162117"></a>Controller</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p25382819214"><a name="zh-cn_topic_0000002599426193_p25382819214"></a><a name="zh-cn_topic_0000002599426193_p25382819214"></a>控制器</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row1453811852112"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row116218176182"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p36271714183"><a name="zh-cn_topic_0000002599426193_p36271714183"></a><a name="zh-cn_topic_0000002599426193_p36271714183"></a><strong id="zh-cn_topic_0000002599426193_b1736018571225"><a name="zh-cn_topic_0000002599426193_b1736018571225"></a><a name="zh-cn_topic_0000002599426193_b1736018571225"></a>D</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row46311712187"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p763111771817"><a name="zh-cn_topic_0000002599426193_p763111771817"></a><a name="zh-cn_topic_0000002599426193_p763111771817"></a><strong id="zh-cn_topic_0000002599426193_b2361157112216"><a name="zh-cn_topic_0000002599426193_b2361157112216"></a><a name="zh-cn_topic_0000002599426193_b2361157112216"></a>DTMF</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p1863131791819"><a name="zh-cn_topic_0000002599426193_p1863131791819"></a><a name="zh-cn_topic_0000002599426193_p1863131791819"></a>Dual-Tone Multi-Frequency</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p4631417121815"><a name="zh-cn_topic_0000002599426193_p4631417121815"></a><a name="zh-cn_topic_0000002599426193_p4631417121815"></a>双音多频</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row542716505102"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row44276502108"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p04271350131018"><a name="zh-cn_topic_0000002599426193_p04271350131018"></a><a name="zh-cn_topic_0000002599426193_p04271350131018"></a><strong id="zh-cn_topic_0000002599426193_b1136235762214"><a name="zh-cn_topic_0000002599426193_b1136235762214"></a><a name="zh-cn_topic_0000002599426193_b1136235762214"></a>E</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row34271450181012"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p1042785012103"><a name="zh-cn_topic_0000002599426193_p1042785012103"></a><a name="zh-cn_topic_0000002599426193_p1042785012103"></a><strong id="zh-cn_topic_0000002599426193_b13621576222"><a name="zh-cn_topic_0000002599426193_b13621576222"></a><a name="zh-cn_topic_0000002599426193_b13621576222"></a>EDR</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p74271550191017"><a name="zh-cn_topic_0000002599426193_p74271550191017"></a><a name="zh-cn_topic_0000002599426193_p74271550191017"></a>Enhanced Data Rate</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p24274505103"><a name="zh-cn_topic_0000002599426193_p24274505103"></a><a name="zh-cn_topic_0000002599426193_p24274505103"></a>增强数据速率</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row1342717502102"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row2427155020104"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p16428250191011"><a name="zh-cn_topic_0000002599426193_p16428250191011"></a><a name="zh-cn_topic_0000002599426193_p16428250191011"></a><strong id="zh-cn_topic_0000002599426193_b133638578220"><a name="zh-cn_topic_0000002599426193_b133638578220"></a><a name="zh-cn_topic_0000002599426193_b133638578220"></a>G</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row24281450141013"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p7428135011106"><a name="zh-cn_topic_0000002599426193_p7428135011106"></a><a name="zh-cn_topic_0000002599426193_p7428135011106"></a><strong id="zh-cn_topic_0000002599426193_b23631257142215"><a name="zh-cn_topic_0000002599426193_b23631257142215"></a><a name="zh-cn_topic_0000002599426193_b23631257142215"></a>GATT</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p1677977161311"><a name="zh-cn_topic_0000002599426193_p1677977161311"></a><a name="zh-cn_topic_0000002599426193_p1677977161311"></a>General Attribute Protocol</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p477937171314"><a name="zh-cn_topic_0000002599426193_p477937171314"></a><a name="zh-cn_topic_0000002599426193_p477937171314"></a>通用属性协议</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row550915501175"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row0509750141719"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p13509135013178"><a name="zh-cn_topic_0000002599426193_p13509135013178"></a><a name="zh-cn_topic_0000002599426193_p13509135013178"></a><strong id="zh-cn_topic_0000002599426193_b5364457122220"><a name="zh-cn_topic_0000002599426193_b5364457122220"></a><a name="zh-cn_topic_0000002599426193_b5364457122220"></a>H</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row65091550161712"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p450975018174"><a name="zh-cn_topic_0000002599426193_p450975018174"></a><a name="zh-cn_topic_0000002599426193_p450975018174"></a><strong id="zh-cn_topic_0000002599426193_b93652057132212"><a name="zh-cn_topic_0000002599426193_b93652057132212"></a><a name="zh-cn_topic_0000002599426193_b93652057132212"></a>HFP</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p550919503177"><a name="zh-cn_topic_0000002599426193_p550919503177"></a><a name="zh-cn_topic_0000002599426193_p550919503177"></a>Hands-Free Profile</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p1050925013171"><a name="zh-cn_topic_0000002599426193_p1050925013171"></a><a name="zh-cn_topic_0000002599426193_p1050925013171"></a>免提规格</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row442815019104"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row257091216168"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p14570112101612"><a name="zh-cn_topic_0000002599426193_p14570112101612"></a><a name="zh-cn_topic_0000002599426193_p14570112101612"></a><strong id="zh-cn_topic_0000002599426193_b136575792214"><a name="zh-cn_topic_0000002599426193_b136575792214"></a><a name="zh-cn_topic_0000002599426193_b136575792214"></a>M</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row171539111216"><td class="cellrowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p31545112110"><a name="zh-cn_topic_0000002599426193_p31545112110"></a><a name="zh-cn_topic_0000002599426193_p31545112110"></a><strong id="zh-cn_topic_0000002599426193_b438542320119"><a name="zh-cn_topic_0000002599426193_b438542320119"></a><a name="zh-cn_topic_0000002599426193_b438542320119"></a>MPEG-2</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p21540117113"><a name="zh-cn_topic_0000002599426193_p21540117113"></a><a name="zh-cn_topic_0000002599426193_p21540117113"></a>Moving Picture Experts Group Phase 2</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p5154181116115"><a name="zh-cn_topic_0000002599426193_p5154181116115"></a><a name="zh-cn_topic_0000002599426193_p5154181116115"></a>运动图像专家组第二代编码标准</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row19874801325"><td class="cellrowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p28746017216"><a name="zh-cn_topic_0000002599426193_p28746017216"></a><a name="zh-cn_topic_0000002599426193_p28746017216"></a><strong id="zh-cn_topic_0000002599426193_b8869144219210"><a name="zh-cn_topic_0000002599426193_b8869144219210"></a><a name="zh-cn_topic_0000002599426193_b8869144219210"></a>MPEG-4</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p287400129"><a name="zh-cn_topic_0000002599426193_p287400129"></a><a name="zh-cn_topic_0000002599426193_p287400129"></a>Moving Picture Experts Group Phase 4</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p68746017213"><a name="zh-cn_topic_0000002599426193_p68746017213"></a><a name="zh-cn_topic_0000002599426193_p68746017213"></a>运动图像专家组第四代编码标准</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row1757013129169"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p2057141251610"><a name="zh-cn_topic_0000002599426193_p2057141251610"></a><a name="zh-cn_topic_0000002599426193_p2057141251610"></a><strong id="zh-cn_topic_0000002599426193_b436615575224"><a name="zh-cn_topic_0000002599426193_b436615575224"></a><a name="zh-cn_topic_0000002599426193_b436615575224"></a>MTU</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p428213981320"><a name="zh-cn_topic_0000002599426193_p428213981320"></a><a name="zh-cn_topic_0000002599426193_p428213981320"></a>Maximum Transmission Unit</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p12282439131316"><a name="zh-cn_topic_0000002599426193_p12282439131316"></a><a name="zh-cn_topic_0000002599426193_p12282439131316"></a>最大传输单元</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row5571912141610"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row193533119190"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p53533141911"><a name="zh-cn_topic_0000002599426193_p53533141911"></a><a name="zh-cn_topic_0000002599426193_p53533141911"></a><strong id="zh-cn_topic_0000002599426193_b2367175713224"><a name="zh-cn_topic_0000002599426193_b2367175713224"></a><a name="zh-cn_topic_0000002599426193_b2367175713224"></a>P</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row93533141916"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p53533161915"><a name="zh-cn_topic_0000002599426193_p53533161915"></a><a name="zh-cn_topic_0000002599426193_p53533161915"></a><strong id="zh-cn_topic_0000002599426193_b19367557172215"><a name="zh-cn_topic_0000002599426193_b19367557172215"></a><a name="zh-cn_topic_0000002599426193_b19367557172215"></a>PBAP</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p1435331171918"><a name="zh-cn_topic_0000002599426193_p1435331171918"></a><a name="zh-cn_topic_0000002599426193_p1435331171918"></a>Phone Book Access Profile</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p173533119196"><a name="zh-cn_topic_0000002599426193_p173533119196"></a><a name="zh-cn_topic_0000002599426193_p173533119196"></a>电话本存取规范</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row11353117199"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row477711521284"><td class="cellrowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p17781252192814"><a name="zh-cn_topic_0000002599426193_p17781252192814"></a><a name="zh-cn_topic_0000002599426193_p17781252192814"></a><strong id="zh-cn_topic_0000002599426193_b1159159202816"><a name="zh-cn_topic_0000002599426193_b1159159202816"></a><a name="zh-cn_topic_0000002599426193_b1159159202816"></a>R</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row2778252112810"><td class="cellrowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p16778175242816"><a name="zh-cn_topic_0000002599426193_p16778175242816"></a><a name="zh-cn_topic_0000002599426193_p16778175242816"></a><strong id="zh-cn_topic_0000002599426193_b101601959182812"><a name="zh-cn_topic_0000002599426193_b101601959182812"></a><a name="zh-cn_topic_0000002599426193_b101601959182812"></a>RSSI</strong></p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p5778352132818"><a name="zh-cn_topic_0000002599426193_p5778352132818"></a><a name="zh-cn_topic_0000002599426193_p5778352132818"></a>Received Signal Strength Indicator</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p9778125214282"><a name="zh-cn_topic_0000002599426193_p9778125214282"></a><a name="zh-cn_topic_0000002599426193_p9778125214282"></a>接收信号强度指示</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row177895212818"><td class="cellrowborder" style="border:none" valign="top" width="22.23222322232223%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cellrowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row1742825051019"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p8428135091015"><a name="zh-cn_topic_0000002599426193_p8428135091015"></a><a name="zh-cn_topic_0000002599426193_p8428135091015"></a><strong id="zh-cn_topic_0000002599426193_b1236855752211"><a name="zh-cn_topic_0000002599426193_b1236855752211"></a><a name="zh-cn_topic_0000002599426193_b1236855752211"></a>S</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row665115108204"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p206511710142017"><a name="zh-cn_topic_0000002599426193_p206511710142017"></a><a name="zh-cn_topic_0000002599426193_p206511710142017"></a><strong id="zh-cn_topic_0000002599426193_b18369657132217"><a name="zh-cn_topic_0000002599426193_b18369657132217"></a><a name="zh-cn_topic_0000002599426193_b18369657132217"></a>SBC</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p5651810142015"><a name="zh-cn_topic_0000002599426193_p5651810142015"></a><a name="zh-cn_topic_0000002599426193_p5651810142015"></a>Sub-Band Coding</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p12651310182020"><a name="zh-cn_topic_0000002599426193_p12651310182020"></a><a name="zh-cn_topic_0000002599426193_p12651310182020"></a>子带编码</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row12906538191712"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p19906938181710"><a name="zh-cn_topic_0000002599426193_p19906938181710"></a><a name="zh-cn_topic_0000002599426193_p19906938181710"></a><strong id="zh-cn_topic_0000002599426193_b1369157142213"><a name="zh-cn_topic_0000002599426193_b1369157142213"></a><a name="zh-cn_topic_0000002599426193_b1369157142213"></a>SCO</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p12902113501414"><a name="zh-cn_topic_0000002599426193_p12902113501414"></a><a name="zh-cn_topic_0000002599426193_p12902113501414"></a>Synchronous Connection-Oriented Link</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p69021135181419"><a name="zh-cn_topic_0000002599426193_p69021135181419"></a><a name="zh-cn_topic_0000002599426193_p69021135181419"></a>同步面向连接链路</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row84284507109"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p44282502108"><a name="zh-cn_topic_0000002599426193_p44282502108"></a><a name="zh-cn_topic_0000002599426193_p44282502108"></a><strong id="zh-cn_topic_0000002599426193_b1737075714227"><a name="zh-cn_topic_0000002599426193_b1737075714227"></a><a name="zh-cn_topic_0000002599426193_b1737075714227"></a>SPP</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p2629105114134"><a name="zh-cn_topic_0000002599426193_p2629105114134"></a><a name="zh-cn_topic_0000002599426193_p2629105114134"></a>Serial Port Profile</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p1629195119135"><a name="zh-cn_topic_0000002599426193_p1629195119135"></a><a name="zh-cn_topic_0000002599426193_p1629195119135"></a>串行端口配置</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row1136112160212"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row3361121682112"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p5361141682115"><a name="zh-cn_topic_0000002599426193_p5361141682115"></a><a name="zh-cn_topic_0000002599426193_p5361141682115"></a><strong id="zh-cn_topic_0000002599426193_b17371757182210"><a name="zh-cn_topic_0000002599426193_b17371757182210"></a><a name="zh-cn_topic_0000002599426193_b17371757182210"></a>T</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row1736251610216"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p12362161662115"><a name="zh-cn_topic_0000002599426193_p12362161662115"></a><a name="zh-cn_topic_0000002599426193_p12362161662115"></a><strong id="zh-cn_topic_0000002599426193_b1037185716228"><a name="zh-cn_topic_0000002599426193_b1037185716228"></a><a name="zh-cn_topic_0000002599426193_b1037185716228"></a>TG</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p183621516192116"><a name="zh-cn_topic_0000002599426193_p183621516192116"></a><a name="zh-cn_topic_0000002599426193_p183621516192116"></a>Target</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p63627163215"><a name="zh-cn_topic_0000002599426193_p63627163215"></a><a name="zh-cn_topic_0000002599426193_p63627163215"></a>目标设备</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row94287509105"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row04281450181019"><td class="nocellnorowborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p1842855012102"><a name="zh-cn_topic_0000002599426193_p1842855012102"></a><a name="zh-cn_topic_0000002599426193_p1842855012102"></a><strong id="zh-cn_topic_0000002599426193_b1537295710229"><a name="zh-cn_topic_0000002599426193_b1537295710229"></a><a name="zh-cn_topic_0000002599426193_b1537295710229"></a>U</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="39.73397339733973%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="38.033803380338036%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002599426193_row1142815505102"><td class="row-nocellborder" style="border:none" valign="top" width="22.23222322232223%"><p id="zh-cn_topic_0000002599426193_p114282508108"><a name="zh-cn_topic_0000002599426193_p114282508108"></a><a name="zh-cn_topic_0000002599426193_p114282508108"></a><strong id="zh-cn_topic_0000002599426193_b1737275710227"><a name="zh-cn_topic_0000002599426193_b1737275710227"></a><a name="zh-cn_topic_0000002599426193_b1737275710227"></a>UUID</strong></p>
</td>
<td class="row-nocellborder" style="border:none" valign="top" width="39.73397339733973%"><p id="zh-cn_topic_0000002599426193_p188285551319"><a name="zh-cn_topic_0000002599426193_p188285551319"></a><a name="zh-cn_topic_0000002599426193_p188285551319"></a>Universal Unique Identifier</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="38.033803380338036%"><p id="zh-cn_topic_0000002599426193_p1188245519133"><a name="zh-cn_topic_0000002599426193_p1188245519133"></a><a name="zh-cn_topic_0000002599426193_p1188245519133"></a>通用的唯一标识</p>
</td>
</tr>
</tbody>
</table>

