# 前言<a name="ZH-CN_TOPIC_0000002522474969"></a>

**概述<a name="section4537382116410"></a>**

本文档为HiDiTingV100星闪模块API接口说明文档，用来指导客户了解星闪模块对外提供的API，使客户能够准备理解接口的含义，更好的基于接口开发客户应用。

**读者对象<a name="section36711531112316"></a>**

本文档主要适用以下工程师：

-   技术支持工程
-   软件开发工程师

**符号约定<a name="section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="table2622507016410"></a>
<table><thead align="left"><tr id="row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="p6450074116410"><a name="p6450074116410"></a><a name="p6450074116410"></a><strong id="b2136615816410"><a name="b2136615816410"></a><a name="b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="p5435366816410"><a name="p5435366816410"></a><a name="p5435366816410"></a><strong id="b5941558116410"><a name="b5941558116410"></a><a name="b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p3734547016410"><a name="p3734547016410"></a><a name="p3734547016410"></a><a name="image2670064316410"></a><a name="image2670064316410"></a><span><img class="" id="image2670064316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002522475009.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p1757432116410"><a name="p1757432116410"></a><a name="p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1432579516410"><a name="p1432579516410"></a><a name="p1432579516410"></a><a name="image4895582316410"></a><a name="image4895582316410"></a><span><img class="" id="image4895582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002490275234.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p959197916410"><a name="p959197916410"></a><a name="p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1232579516410"><a name="p1232579516410"></a><a name="p1232579516410"></a><a name="image1235582316410"></a><a name="image1235582316410"></a><span><img class="" id="image1235582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002522315029.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p123197916410"><a name="p123197916410"></a><a name="p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p2204984716410"><a name="p2204984716410"></a><a name="p2204984716410"></a><a name="image4504446716410"></a><a name="image4504446716410"></a><span><img class="" id="image4504446716410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002522475005.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4388861916410"><a name="p4388861916410"></a><a name="p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="p1238861916410"><a name="p1238861916410"></a><a name="p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p5555360116410"><a name="p5555360116410"></a><a name="p5555360116410"></a><a name="image799324016410"></a><a name="image799324016410"></a><span><img class="" id="image799324016410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002490115254.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4612588116410"><a name="p4612588116410"></a><a name="p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="p1232588116410"><a name="p1232588116410"></a><a name="p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# 概述<a name="ZH-CN_TOPIC_0000002522318579"></a>

本文档主要描述穿戴解决方案星闪模块对外提供的接口。涉及以下方面：接口函数、数据结构。

# 星闪API说明<a name="ZH-CN_TOPIC_0000002490278784"></a>

-   **[数据结构](#ZH-CN_TOPIC_0000002522478567)**  

-   **[函数接口](#ZH-CN_TOPIC_0000002490118814)**  

-   **[开发指导](#ZH-CN_TOPIC_0000002522318593)**  

## 数据结构<a name="ZH-CN_TOPIC_0000002522478567"></a>

-   **[bs\_sle\_config\_type\_t](#ZH-CN_TOPIC_0000002490118812)**  

-   **[bs\_sle\_stack\_state\_callbacks\_t](#ZH-CN_TOPIC_0000002522318583)**  

-   **[bs\_sle\_callbacks\_t](#ZH-CN_TOPIC_0000002490278786)**  

-   **[bs\_sle\_device\_callbacks\_t](#ZH-CN_TOPIC_0000002522478569)**  

### bs\_sle\_config\_type\_t<a name="ZH-CN_TOPIC_0000002490118812"></a>

【头文件】

protocol/bt/host/include/sle/sle\_service/ssf\_config.h

【结构体说明】

用于调整星闪的默认配置，通过指定配置项来修改配置。

【结构体定义】

```
typedef enum {
    SLE_CONFIG_COMMON = 0x0000 | (BS_PROTOCOL_SLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    SLE_CONFIG_ADV = 0x0100 | (BS_PROTOCOL_SLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    SLE_CONFIG_SCAN = 0x0200 | (BS_PROTOCOL_SLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    SLE_CONFIG_AS = 0x0300 | (BS_PROTOCOL_SLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
} bs_sle_config_group_t;

typedef enum {
    SLE_CONFIG_COMMON_ROLE = SLE_CONFIG_COMMON | 0x01,      /* sle_config_role_t */
    SLE_CONFIG_COMMON_PEER_ADDR = SLE_CONFIG_COMMON | 0x02, /* 6 bytes */
    SLE_CONFIG_COMMON_SUB_ROLE = SLE_CONFIG_COMMON | 0x03,  /* sle_config_sub_role_t */
    SLE_CONFIG_COMMON_LOCAL_ADDR = SLE_CONFIG_COMMON | 0x04, /* sle_config_local_addr_t */
    SLE_CONFIG_ADV_SHORT_NAME = SLE_CONFIG_ADV | 0x01, /* uint8_t */
    SLE_CONFIG_ADV_FULL_NAME = SLE_CONFIG_ADV | 0x02,  /* uint8_t */
    SLE_CONFIG_ADV_VENDOR = SLE_CONFIG_ADV | 0x03,     /* uint8_t */
    SLE_CONFIG_ADV_APPEARANCE = SLE_CONFIG_ADV | 0x04, /* sle_config_appearence_t */
    SLE_CONFIG_ADV_PRESET_PARAM = SLE_CONFIG_ADV | 0x05, /* !< @if Eng Add preset advertising parameters.The configuration item does not support the interface bs_sle_get_config.{bs_sle_opr_pre_set_adv_param_t}.This parameter applies only to the bs_config interface, but not to the bs_sle_get_config interface.
                                                      @else 添加用户预制广播参数。{bs_sle_opr_pre_set_adv_param_t}。该配置项仅支持bs_config接口，不支持bs_sle_get_config接口。
                                                      @endif */
    SLE_CONFIG_ADV_DISCOVERING_SPEED = SLE_CONFIG_ADV | 0x06, /* !< @if Eng Use the preset parameters for device advertising. Type is uint16_t. The input value 0 indicates that preset parameters are not used. Other input value must in range of {@ref bs_sle_preset_adv_speed_mode_id_t *}. If the preset parameter gear level (mode_id) is specified by this configuration but the corresponding preset parameter is not be added, the default parameter will take effect.
                            @else 使用用户预置的广播参数作为广播参数，参数数据类型为uint16_t。取值为0时表示不使用预置参数。其他有效值应满足枚举值范围 {@ref bs_sle_preset_adv_speed_mode_id_t *}。若使用该配置项指定了预置参数档位（即mode_id）但对应的预置参数并未添加，则在使用时默认参数生效。
                            @endif */
    SLE_CONFIG_SCAN_FILTER_DUPLICATE = SLE_CONFIG_SCAN | 0x01,
    SLE_CONFIG_SCAN_SEEK_FILTER_POLICY = SLE_CONFIG_SCAN | 0x02,
    SLE_CONFIG_SCAN_SEEK_TYPE = SLE_CONFIG_SCAN | 0x03,
    SLE_CONFIG_SCAN_SEEK_PRESET_PARAM = SLE_CONFIG_SCAN | 0x04, /* !< @if Eng Add preset scan parameters {@ref bs_sle_opr_pre_set_seek_param_t *}.This parameter applies only to the bs_config interface, but not to the bs_sle_get_config interface.
                                                          @else 添加用户预制扫描参数。{@ref bs_sle_opr_pre_set_seek_param_t *}。该配置项仅支持bs_config接口，不支持bs_sle_get_config接口。
                                                          @endif */
    SLE_CONFIG_SCAN_SEEK_SPEED = SLE_CONFIG_SCAN | 0x05, /* !< @if Eng Use the preset parameters for device discovery. Type is uint16_t. The input value 0 indicates that preset parameters are not used. Other input value must in range of {@ref bs_sle_preset_seek_speed_mode_id_t *} corresponding preset parameter is not be added, the default parameter will take effect.
                            @else 使用用户预置的扫描参数作为扫描参数，参数数据类型为uint16_t。取值为0时表示不使用预置参数。其他有效值应满足枚举值范围 {@ref bs_sle_preset_seek_speed_mode_id_t *}若使用该配置项指定了预置参数档位（即mode_id）但对应的预置参数并未添加，则在使用时默认参数生效。
                            @endif */
    SLE_CONFIG_AS_CODEC_CAPABILITY = SLE_CONFIG_AS | 0x01, /*!< @if Eng the config of codec capability.
                                                           @else 编解码能力配置，查询编解码能力配置时，需要先查询编解码能力个数，出参结构体指针
                                                          {sle_config_as_codec_capability_set_t}
                                                           @endif */
    SLE_CONFIG_AS_LOCATION_BITMAP = SLE_CONFIG_AS | 0x02,
    SLE_CONFIG_AS_CODEC_CAPABILITY_COUNT = SLE_CONFIG_AS | 0x03, /*!< @if Eng the count of codec     capability.
                                                                  @else 编解码能力个数，查询编解码能力配置时，需要先查询编解码能力个数 @endif */
    SLE_CONFIG_TYPE_MAX,
} bs_sle_config_type_t;
```

【注意事项】

无。

### bs\_sle\_stack\_state\_callbacks\_t<a name="ZH-CN_TOPIC_0000002522318583"></a>

【头文件】

protocol/bt/host/include/sle/sle\_service/ssf\_init.h

【结构体说明】

SLE状态回调函数接口定义。

【结构体定义】

```
typedef struct {
    bs_sle_stack_state_cbk stack_state_cbk;
} bs_sle_stack_state_callbacks_t;
```

【注意事项】

使能协议栈之前调用。

### bs\_sle\_callbacks\_t<a name="ZH-CN_TOPIC_0000002490278786"></a>

【头文件】

protocol/bt/host/include/sle/sle\_service/ssf\_connection.h

【结构体说明】

SLE service回调函数接口。

【结构体定义】

```
typedef struct {
    bs_sle_pair_request_cbk pair_request_cb;            /*!< @if Eng SLE pair request callback. If the function for reporting
                                                                     pairing requests is enabled, when a pairing request from
                                                                     the peer is received, this callback is reported. It is
                                                                     not enabled by default.
                                                             @else   星闪配对请求的回调函数。打开配对请求上报时，收到对端配对
                                                                     请求会上报该回调，默认不打开。@endif */
    bs_sle_connect_state_cbk connect_state_cb;          /*!< @if Eng SLE device connect state callback. This callback
                                                                     is reported when the connection status and encryption
                                                                     status of devices changed.
                                                             @else   星闪（非合作设备集内设备）连接状态的回调函数。
                                                                     当连接状态以及加密状态发生改变时上报该回调。 @endif */
    bs_sle_connect_state_cbk member_connect_state_cb;   /*!< @if Eng SLE device in cooperation device set connect state
                                                                     callback. This callback is reported when the connection
                                                                     status and encryption status of devices within the SLE
                                                                     cooperation device set change.
                                                             @else   星闪合作设备集内的设备间连接状态的回调函数。
                                                                     当星闪合作设备集内设备连接状态以及加密状态发生改变时
                                                                     上报该回调。 @endif */
    bs_sle_pair_complete_cbk pair_complete_cb;          /*!< @if Eng SLE pair complete callback. When the pairing
                                                                     process is finished, this callback will report
                                                                     and notify the pairing information.
                                                             @else   星闪配对完成的回调函数。当配对流程成功后，
                                                                     该回调会上报并通知配对信息。 @endif */
    bs_sle_pair_remove_cbk pair_remove_cb;              /*!< @if Eng SLE remove pair callback. When the device pairing
                                                                     information is removed, this callback will report
                                                                     and notify the result.
                                                             @else   星闪移除配对完成的回调函数。当移除设备配对信息后，
                                                                     该回调会上报并通知结果。 @endif */
    bs_sle_discovery_result_cbk discovery_result_cb;    /*!< @if Eng SLE remove pair callback. When the device pairing
                                                                     information is removed, this callback will report
                                                                     and notify the result.
                                                             @else   星闪发现结果上报的回调函数。当移除设备配对信息后，
                                                                     该回调会上报并通知结果。 @endif */
    bs_sle_discovery_state_cbk discovery_state_cb;      /*!< @if Eng SLE discovery state report callback. This callback
                                                                     is reported when entering or exiting the discovery
                                                                     state.
                                                             @else   星闪发现状态上报的回调函数。当进入或退出发现状态后上报
                                                                     该回调。@endif */
    bs_sle_auto_conn_state_cbk auto_conn_state_cb;      /*!< @if Eng SLE automatic connect state report callback. This
                                                                     callback is reported when entering or exiting the
                                                                     automatic reconnection state.
                                                             @else   星闪自动回连状态上报的回调函数。当进入或退出自动回连状态
                                                                     后上报该回调。@endif */
} bs_sle_callbacks_t;
```

【注意事项】

connect\_state\_cb为连接状态回调函数，而member\_connect\_state\_cb为合作设备集合设备之间的链路（如耳机、音箱），手表无需关注该回调。

### bs\_sle\_device\_callbacks\_t<a name="ZH-CN_TOPIC_0000002522478569"></a>

【头文件】

protocol/bt/host/include/sle/sle\_service/ssf\_device.h

【结构体说明】

SLE设备回调函数结构体，该结构体用于通过bs\_sle\_register\_device\_callbacks接口注册设备回调函数，用于获取对端设备的星闪对象编号（Object，Obj）。

【结构体定义】

```
typedef struct {
    bs_sle_obj_alloc_cbk obj_alloc_cbk;
    bs_sle_obj_recover_cbk obj_recover_cbk;
} bs_sle_device_callbacks_t;
```

【注意事项】

星闪Object的概念是来自于合作设备集合，由于存在耳机、音箱场景，多个设备属于一个设备集合内，因此属于同一个Obj，一个Obj内包含多个设备ID，即dev\_id，但对于手表来说，一个Obj包含一个dev\_id。

## 函数接口<a name="ZH-CN_TOPIC_0000002490118814"></a>

-   **[sle\_service\_init](#ZH-CN_TOPIC_0000002522318585)**  

-   **[bs\_config](#ZH-CN_TOPIC_0000002490278788)**  

-   **[bs\_sle\_get\_config](#ZH-CN_TOPIC_0000002522478571)**  

-   **[bs\_sle\_register\_stack\_state\_callbacks](#ZH-CN_TOPIC_0000002490118816)**  

-   **[bs\_sle\_enable/bs\_sle\_disable](#ZH-CN_TOPIC_0000002522318587)**  

-   **[bs\_sle\_register\_callbacks](#ZH-CN_TOPIC_0000002490278790)**  

-   **[bs\_sle\_unregister\_callbacks](#ZH-CN_TOPIC_0000002522478573)**  

-   **[bs\_sle\_start\_discovery](#ZH-CN_TOPIC_0000002490118818)**  

-   **[bs\_sle\_stop\_discovery](#ZH-CN_TOPIC_0000002522318589)**  

-   **[bs\_sle\_start\_pair](#ZH-CN_TOPIC_0000002490278792)**  

-   **[bs\_sle\_disconnect](#ZH-CN_TOPIC_0000002522478575)**  

-   **[bs\_sle\_remove\_pair](#ZH-CN_TOPIC_0000002490118820)**  

-   **[bs\_sle\_start\_auto\_conn](#ZH-CN_TOPIC_0000002522318591)**  

-   **[bs\_sle\_stop\_auto\_conn](#ZH-CN_TOPIC_0000002490278794)**  

-   **[bs\_sle\_register\_device\_callbacks](#ZH-CN_TOPIC_0000002522478577)**  

-   **[bs\_sle\_unregister\_device\_callbacks](#ZH-CN_TOPIC_0000002490118822)**  

### sle\_service\_init<a name="ZH-CN_TOPIC_0000002522318585"></a>

【描述】

SLE service资源初始化。

【接口定义】

```
void sle_service_init(void);
```

【参数】

无。

【返回值】

无。

【注意事项】

上电后，bs\_config配置资源前调用该接口，以初始化service层相关资源。

### bs\_config<a name="ZH-CN_TOPIC_0000002490278788"></a>

【描述】

配置星闪参数。

【接口定义】

```
errcode_t bs_config(uint16_t type, void *param, uint16_t len);
```

【参数】

<a name="zh-cn_topic_0000002521732933_table567mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002521732933_row573mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002521732933_p575mcpsimp"><a name="zh-cn_topic_0000002521732933_p575mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p575mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.13999999999999%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002521732933_p577mcpsimp"><a name="zh-cn_topic_0000002521732933_p577mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p577mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.86%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002521732933_p579mcpsimp"><a name="zh-cn_topic_0000002521732933_p579mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p579mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002521732933_row580mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002521732933_p582mcpsimp"><a name="zh-cn_topic_0000002521732933_p582mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p582mcpsimp"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002521732933_p584mcpsimp"><a name="zh-cn_topic_0000002521732933_p584mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p584mcpsimp"></a>配置项，详见“<a href="#ZH-CN_TOPIC_0000002490118812">bs_sle_config_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002521732933_p586mcpsimp"><a name="zh-cn_topic_0000002521732933_p586mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p586mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002521732933_row587mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002521732933_p589mcpsimp"><a name="zh-cn_topic_0000002521732933_p589mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p589mcpsimp"></a>param</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002521732933_p591mcpsimp"><a name="zh-cn_topic_0000002521732933_p591mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p591mcpsimp"></a>配置值，具体类型根据配置项而定。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002521732933_p593mcpsimp"><a name="zh-cn_topic_0000002521732933_p593mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p593mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002521732933_row594mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002521732933_p596mcpsimp"><a name="zh-cn_topic_0000002521732933_p596mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p596mcpsimp"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="62.13999999999999%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002521732933_p598mcpsimp"><a name="zh-cn_topic_0000002521732933_p598mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p598mcpsimp"></a>配置值大小。</p>
</td>
<td class="cellrowborder" valign="top" width="20.86%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002521732933_p600mcpsimp"><a name="zh-cn_topic_0000002521732933_p600mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p600mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002521732933_table602mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002521732933_row608mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002521732933_p610mcpsimp"><a name="zh-cn_topic_0000002521732933_p610mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p610mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002521732933_p612mcpsimp"><a name="zh-cn_topic_0000002521732933_p612mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p612mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002521732933_p614mcpsimp"><a name="zh-cn_topic_0000002521732933_p614mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p614mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002521732933_row615mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002521732933_p617mcpsimp"><a name="zh-cn_topic_0000002521732933_p617mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p617mcpsimp"></a>errcode_sle_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002521732933_p619mcpsimp"><a name="zh-cn_topic_0000002521732933_p619mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p619mcpsimp"></a>ERRCODE_SLE_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002521732933_p620mcpsimp"><a name="zh-cn_topic_0000002521732933_p620mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p620mcpsimp"></a>其他：失败，错误码见errcode_sle_t，定义在sle_errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002521732933_p622mcpsimp"><a name="zh-cn_topic_0000002521732933_p622mcpsimp"></a><a name="zh-cn_topic_0000002521732933_p622mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

### bs\_sle\_get\_config<a name="ZH-CN_TOPIC_0000002522478571"></a>

【描述】

该函数用于查询星闪的当前配置，通过指定配置命令和子项来获取配置值。

【接口定义】

```
errcode_sle_t bs_sle_get_config(uint16_t type, void *param, uint16_t *len);
```

【参数】

<a name="zh-cn_topic_0000002489613116_table751mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002489613116_row757mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002489613116_p759mcpsimp"><a name="zh-cn_topic_0000002489613116_p759mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p759mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.78%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002489613116_p761mcpsimp"><a name="zh-cn_topic_0000002489613116_p761mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p761mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.22%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002489613116_p763mcpsimp"><a name="zh-cn_topic_0000002489613116_p763mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p763mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002489613116_row764mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489613116_p766mcpsimp"><a name="zh-cn_topic_0000002489613116_p766mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p766mcpsimp"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="61.78%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489613116_p768mcpsimp"><a name="zh-cn_topic_0000002489613116_p768mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p768mcpsimp"></a>配置项，详见“<a href="#ZH-CN_TOPIC_0000002490118812">bs_sle_config_type_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.22%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489613116_p770mcpsimp"><a name="zh-cn_topic_0000002489613116_p770mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p770mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002489613116_row771mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489613116_p773mcpsimp"><a name="zh-cn_topic_0000002489613116_p773mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p773mcpsimp"></a>param</p>
</td>
<td class="cellrowborder" valign="top" width="61.78%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489613116_p775mcpsimp"><a name="zh-cn_topic_0000002489613116_p775mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p775mcpsimp"></a>当前配置值的值。</p>
</td>
<td class="cellrowborder" valign="top" width="21.22%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489613116_p777mcpsimp"><a name="zh-cn_topic_0000002489613116_p777mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p777mcpsimp"></a>输出</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002489613116_row778mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489613116_p780mcpsimp"><a name="zh-cn_topic_0000002489613116_p780mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p780mcpsimp"></a>len</p>
</td>
<td class="cellrowborder" valign="top" width="61.78%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489613116_p782mcpsimp"><a name="zh-cn_topic_0000002489613116_p782mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p782mcpsimp"></a>配置值大小。</p>
</td>
<td class="cellrowborder" valign="top" width="21.22%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489613116_p784mcpsimp"><a name="zh-cn_topic_0000002489613116_p784mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p784mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002489613116_table786mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002489613116_row792mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002489613116_p794mcpsimp"><a name="zh-cn_topic_0000002489613116_p794mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p794mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002489613116_p796mcpsimp"><a name="zh-cn_topic_0000002489613116_p796mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p796mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002489613116_p798mcpsimp"><a name="zh-cn_topic_0000002489613116_p798mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p798mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002489613116_row799mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489613116_p801mcpsimp"><a name="zh-cn_topic_0000002489613116_p801mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p801mcpsimp"></a>errcode_sle_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489613116_p803mcpsimp"><a name="zh-cn_topic_0000002489613116_p803mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p803mcpsimp"></a>ERRCODE_SLE_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002489613116_p804mcpsimp"><a name="zh-cn_topic_0000002489613116_p804mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p804mcpsimp"></a>其他：失败，错误码见errcode_sle_t，定义在sle_errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489613116_p806mcpsimp"><a name="zh-cn_topic_0000002489613116_p806mcpsimp"></a><a name="zh-cn_topic_0000002489613116_p806mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

### bs\_sle\_register\_stack\_state\_callbacks<a name="ZH-CN_TOPIC_0000002490118816"></a>

【描述】

注册SLE协议栈状态上报回调函数。

【接口定义】

```
errcode_sle_t bs_sle_register_stack_state_callbacks(uint32_t app_id, bs_sle_stack_state_callbacks_t *cbks);
```

【参数】

<a name="zh-cn_topic_0000002489453150_table694mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002489453150_row700mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002489453150_p702mcpsimp"><a name="zh-cn_topic_0000002489453150_p702mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p702mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.25000000000001%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002489453150_p704mcpsimp"><a name="zh-cn_topic_0000002489453150_p704mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p704mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.75%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002489453150_p706mcpsimp"><a name="zh-cn_topic_0000002489453150_p706mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p706mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002489453150_row707mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489453150_p709mcpsimp"><a name="zh-cn_topic_0000002489453150_p709mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p709mcpsimp"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.25000000000001%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489453150_p711mcpsimp"><a name="zh-cn_topic_0000002489453150_p711mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p711mcpsimp"></a>区分不同应用的标识。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489453150_p713mcpsimp"><a name="zh-cn_topic_0000002489453150_p713mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p713mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002489453150_row714mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489453150_p716mcpsimp"><a name="zh-cn_topic_0000002489453150_p716mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p716mcpsimp"></a>cbks</p>
</td>
<td class="cellrowborder" valign="top" width="61.25000000000001%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489453150_p718mcpsimp"><a name="zh-cn_topic_0000002489453150_p718mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p718mcpsimp"></a>要注册的回调函数，详见“<a href="#ZH-CN_TOPIC_0000002522318583">bs_sle_stack_state_callbacks_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.75%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489453150_p720mcpsimp"><a name="zh-cn_topic_0000002489453150_p720mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p720mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002489453150_table722mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002489453150_row728mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002489453150_p730mcpsimp"><a name="zh-cn_topic_0000002489453150_p730mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p730mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002489453150_p732mcpsimp"><a name="zh-cn_topic_0000002489453150_p732mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p732mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002489453150_p734mcpsimp"><a name="zh-cn_topic_0000002489453150_p734mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p734mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002489453150_row735mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489453150_p737mcpsimp"><a name="zh-cn_topic_0000002489453150_p737mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p737mcpsimp"></a>errcode_sle_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489453150_p739mcpsimp"><a name="zh-cn_topic_0000002489453150_p739mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p739mcpsimp"></a>ERRCODE_SLE_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002489453150_p740mcpsimp"><a name="zh-cn_topic_0000002489453150_p740mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p740mcpsimp"></a>其他：失败，错误码见errcode_sle_t，定义在sle_errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489453150_p742mcpsimp"><a name="zh-cn_topic_0000002489453150_p742mcpsimp"></a><a name="zh-cn_topic_0000002489453150_p742mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

使能协议栈之前调用。

### bs\_sle\_enable/bs\_sle\_disable<a name="ZH-CN_TOPIC_0000002522318587"></a>

【描述】

使能/去使能SLE协议栈。

【接口定义】

```
errcode_sle_t bs_sle_enable(void);
```

```
errcode_sle_t bs_sle_disable(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002521732935_table665mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002521732935_row671mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002521732935_p673mcpsimp"><a name="zh-cn_topic_0000002521732935_p673mcpsimp"></a><a name="zh-cn_topic_0000002521732935_p673mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002521732935_p675mcpsimp"><a name="zh-cn_topic_0000002521732935_p675mcpsimp"></a><a name="zh-cn_topic_0000002521732935_p675mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002521732935_p677mcpsimp"><a name="zh-cn_topic_0000002521732935_p677mcpsimp"></a><a name="zh-cn_topic_0000002521732935_p677mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002521732935_row678mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002521732935_p680mcpsimp"><a name="zh-cn_topic_0000002521732935_p680mcpsimp"></a><a name="zh-cn_topic_0000002521732935_p680mcpsimp"></a>errcode_sle_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002521732935_p682mcpsimp"><a name="zh-cn_topic_0000002521732935_p682mcpsimp"></a><a name="zh-cn_topic_0000002521732935_p682mcpsimp"></a>ERRCODE_SLE_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002521732935_p683mcpsimp"><a name="zh-cn_topic_0000002521732935_p683mcpsimp"></a><a name="zh-cn_topic_0000002521732935_p683mcpsimp"></a>其他：失败，错误码见errcode_sle_t，定义在sle_errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002521732935_p685mcpsimp"><a name="zh-cn_topic_0000002521732935_p685mcpsimp"></a><a name="zh-cn_topic_0000002521732935_p685mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

### bs\_sle\_register\_callbacks<a name="ZH-CN_TOPIC_0000002490278790"></a>

【描述】

注册配对状态上报回调函数，在设备配对状态发生变化后上报。

【接口定义】

```
errcode_t bs_sle_register_callbacks(uint32_t app_id, bs_sle_callbacks_t *cbks);
```

【参数】

<a name="zh-cn_topic_0000002489613118_table452mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002489613118_row458mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002489613118_p460mcpsimp"><a name="zh-cn_topic_0000002489613118_p460mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p460mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.5%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002489613118_p462mcpsimp"><a name="zh-cn_topic_0000002489613118_p462mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p462mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.5%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002489613118_p464mcpsimp"><a name="zh-cn_topic_0000002489613118_p464mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p464mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002489613118_row465mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489613118_p467mcpsimp"><a name="zh-cn_topic_0000002489613118_p467mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p467mcpsimp"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.5%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489613118_p469mcpsimp"><a name="zh-cn_topic_0000002489613118_p469mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p469mcpsimp"></a>区分不同应用的标识。</p>
</td>
<td class="cellrowborder" valign="top" width="21.5%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489613118_p471mcpsimp"><a name="zh-cn_topic_0000002489613118_p471mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p471mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002489613118_row472mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489613118_p474mcpsimp"><a name="zh-cn_topic_0000002489613118_p474mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p474mcpsimp"></a>cbks</p>
</td>
<td class="cellrowborder" valign="top" width="61.5%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489613118_p476mcpsimp"><a name="zh-cn_topic_0000002489613118_p476mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p476mcpsimp"></a>要注册的回调函数，详见“<a href="#ZH-CN_TOPIC_0000002490278786">bs_sle_callbacks_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.5%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489613118_p478mcpsimp"><a name="zh-cn_topic_0000002489613118_p478mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p478mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002489613118_table480mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002489613118_row486mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002489613118_p488mcpsimp"><a name="zh-cn_topic_0000002489613118_p488mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p488mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002489613118_p490mcpsimp"><a name="zh-cn_topic_0000002489613118_p490mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p490mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002489613118_p492mcpsimp"><a name="zh-cn_topic_0000002489613118_p492mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p492mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002489613118_row493mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489613118_p495mcpsimp"><a name="zh-cn_topic_0000002489613118_p495mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p495mcpsimp"></a>errcode_sle_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489613118_p497mcpsimp"><a name="zh-cn_topic_0000002489613118_p497mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p497mcpsimp"></a>ERRCODE_SLE_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002489613118_p498mcpsimp"><a name="zh-cn_topic_0000002489613118_p498mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p498mcpsimp"></a>其他：失败，错误码见errcode_sle_t，定义在sle_errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489613118_p500mcpsimp"><a name="zh-cn_topic_0000002489613118_p500mcpsimp"></a><a name="zh-cn_topic_0000002489613118_p500mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

### bs\_sle\_unregister\_callbacks<a name="ZH-CN_TOPIC_0000002522478573"></a>

【描述】

去注册配对状态上报回调函数。

【接口定义】

```
void bs_sle_unregister_callbacks(uint32_t app_id);
```

【参数】

<a name="zh-cn_topic_0000002521612951_table632mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002521612951_row638mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002521612951_p640mcpsimp"><a name="zh-cn_topic_0000002521612951_p640mcpsimp"></a><a name="zh-cn_topic_0000002521612951_p640mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="67%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002521612951_p642mcpsimp"><a name="zh-cn_topic_0000002521612951_p642mcpsimp"></a><a name="zh-cn_topic_0000002521612951_p642mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002521612951_p644mcpsimp"><a name="zh-cn_topic_0000002521612951_p644mcpsimp"></a><a name="zh-cn_topic_0000002521612951_p644mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002521612951_row645mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002521612951_p647mcpsimp"><a name="zh-cn_topic_0000002521612951_p647mcpsimp"></a><a name="zh-cn_topic_0000002521612951_p647mcpsimp"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="67%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002521612951_p649mcpsimp"><a name="zh-cn_topic_0000002521612951_p649mcpsimp"></a><a name="zh-cn_topic_0000002521612951_p649mcpsimp"></a>区分不同应用的标识。</p>
</td>
<td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002521612951_p651mcpsimp"><a name="zh-cn_topic_0000002521612951_p651mcpsimp"></a><a name="zh-cn_topic_0000002521612951_p651mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

无。

【注意事项】

无。

### bs\_sle\_start\_discovery<a name="ZH-CN_TOPIC_0000002490118818"></a>

【描述】

启动设备发现过程，根据传入的type，会开启本节点的广播或者扫描逻辑，如果本端为主端，如手表连接耳机场景，起扫描；如果本端为从端，如手机连接手表场景，起广播。发现过程完成后，会通过回调函数通知结果。

【接口定义】

```
errcode_t bs_sle_start_discovery(bs_sle_discovery_type_t type);
```

【参数】

<a name="zh-cn_topic_0000002489453152_table402mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002489453152_row408mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002489453152_p410mcpsimp"><a name="zh-cn_topic_0000002489453152_p410mcpsimp"></a><a name="zh-cn_topic_0000002489453152_p410mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.629999999999995%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002489453152_p412mcpsimp"><a name="zh-cn_topic_0000002489453152_p412mcpsimp"></a><a name="zh-cn_topic_0000002489453152_p412mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.37%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002489453152_p414mcpsimp"><a name="zh-cn_topic_0000002489453152_p414mcpsimp"></a><a name="zh-cn_topic_0000002489453152_p414mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002489453152_row415mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489453152_p417mcpsimp"><a name="zh-cn_topic_0000002489453152_p417mcpsimp"></a><a name="zh-cn_topic_0000002489453152_p417mcpsimp"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="61.629999999999995%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489453152_p419mcpsimp"><a name="zh-cn_topic_0000002489453152_p419mcpsimp"></a><a name="zh-cn_topic_0000002489453152_p419mcpsimp"></a>连接类型，指定在发现过程中本端的角色（中心节点或外围节点）。</p>
</td>
<td class="cellrowborder" valign="top" width="21.37%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489453152_p421mcpsimp"><a name="zh-cn_topic_0000002489453152_p421mcpsimp"></a><a name="zh-cn_topic_0000002489453152_p421mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002489453152_table423mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002489453152_row429mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002489453152_p431mcpsimp"><a name="zh-cn_topic_0000002489453152_p431mcpsimp"></a><a name="zh-cn_topic_0000002489453152_p431mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002489453152_p433mcpsimp"><a name="zh-cn_topic_0000002489453152_p433mcpsimp"></a><a name="zh-cn_topic_0000002489453152_p433mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002489453152_p435mcpsimp"><a name="zh-cn_topic_0000002489453152_p435mcpsimp"></a><a name="zh-cn_topic_0000002489453152_p435mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002489453152_row436mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489453152_p438mcpsimp"><a name="zh-cn_topic_0000002489453152_p438mcpsimp"></a><a name="zh-cn_topic_0000002489453152_p438mcpsimp"></a>errcode_sle_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489453152_p440mcpsimp"><a name="zh-cn_topic_0000002489453152_p440mcpsimp"></a><a name="zh-cn_topic_0000002489453152_p440mcpsimp"></a>ERRCODE_SLE_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002489453152_p441mcpsimp"><a name="zh-cn_topic_0000002489453152_p441mcpsimp"></a><a name="zh-cn_topic_0000002489453152_p441mcpsimp"></a>其他：失败，错误码见errcode_sle_t，定义在sle_errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489453152_p443mcpsimp"><a name="zh-cn_topic_0000002489453152_p443mcpsimp"></a><a name="zh-cn_topic_0000002489453152_p443mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

### bs\_sle\_stop\_discovery<a name="ZH-CN_TOPIC_0000002522318589"></a>

【描述】

停止当前正在运行的设备发现过程。

【接口定义】

```
errcode_t bs_sle_stop_discovery(bs_sle_discovery_type_t type);
```

【参数】

<a name="zh-cn_topic_0000002521732937_table154mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002521732937_row160mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002521732937_p162mcpsimp"><a name="zh-cn_topic_0000002521732937_p162mcpsimp"></a><a name="zh-cn_topic_0000002521732937_p162mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.150000000000006%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002521732937_p164mcpsimp"><a name="zh-cn_topic_0000002521732937_p164mcpsimp"></a><a name="zh-cn_topic_0000002521732937_p164mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.849999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002521732937_p166mcpsimp"><a name="zh-cn_topic_0000002521732937_p166mcpsimp"></a><a name="zh-cn_topic_0000002521732937_p166mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002521732937_row167mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002521732937_p169mcpsimp"><a name="zh-cn_topic_0000002521732937_p169mcpsimp"></a><a name="zh-cn_topic_0000002521732937_p169mcpsimp"></a>type</p>
</td>
<td class="cellrowborder" valign="top" width="62.150000000000006%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002521732937_p171mcpsimp"><a name="zh-cn_topic_0000002521732937_p171mcpsimp"></a><a name="zh-cn_topic_0000002521732937_p171mcpsimp"></a>连接类型，指定在发现过程中本端的角色（中心节点或外围节点）。</p>
</td>
<td class="cellrowborder" valign="top" width="20.849999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002521732937_p173mcpsimp"><a name="zh-cn_topic_0000002521732937_p173mcpsimp"></a><a name="zh-cn_topic_0000002521732937_p173mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002521732937_table175mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002521732937_row181mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002521732937_p183mcpsimp"><a name="zh-cn_topic_0000002521732937_p183mcpsimp"></a><a name="zh-cn_topic_0000002521732937_p183mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002521732937_p185mcpsimp"><a name="zh-cn_topic_0000002521732937_p185mcpsimp"></a><a name="zh-cn_topic_0000002521732937_p185mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002521732937_p187mcpsimp"><a name="zh-cn_topic_0000002521732937_p187mcpsimp"></a><a name="zh-cn_topic_0000002521732937_p187mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002521732937_row188mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002521732937_p190mcpsimp"><a name="zh-cn_topic_0000002521732937_p190mcpsimp"></a><a name="zh-cn_topic_0000002521732937_p190mcpsimp"></a>errcode_sle_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002521732937_p192mcpsimp"><a name="zh-cn_topic_0000002521732937_p192mcpsimp"></a><a name="zh-cn_topic_0000002521732937_p192mcpsimp"></a>ERRCODE_SLE_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002521732937_p193mcpsimp"><a name="zh-cn_topic_0000002521732937_p193mcpsimp"></a><a name="zh-cn_topic_0000002521732937_p193mcpsimp"></a>其他：失败，错误码见errcode_sle_t，定义在sle_errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002521732937_p195mcpsimp"><a name="zh-cn_topic_0000002521732937_p195mcpsimp"></a><a name="zh-cn_topic_0000002521732937_p195mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

### bs\_sle\_start\_pair<a name="ZH-CN_TOPIC_0000002490278792"></a>

【描述】

启动配对函数，用于启动已发现设备的配对逻辑。

【接口定义】

```
errcode_t bs_sle_start_pair(const sle_addr_t *addr);
```

【参数】

<a name="zh-cn_topic_0000002489613120_table216mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002489613120_row222mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002489613120_p224mcpsimp"><a name="zh-cn_topic_0000002489613120_p224mcpsimp"></a><a name="zh-cn_topic_0000002489613120_p224mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.629999999999995%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002489613120_p226mcpsimp"><a name="zh-cn_topic_0000002489613120_p226mcpsimp"></a><a name="zh-cn_topic_0000002489613120_p226mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.37%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002489613120_p228mcpsimp"><a name="zh-cn_topic_0000002489613120_p228mcpsimp"></a><a name="zh-cn_topic_0000002489613120_p228mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002489613120_row229mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489613120_p231mcpsimp"><a name="zh-cn_topic_0000002489613120_p231mcpsimp"></a><a name="zh-cn_topic_0000002489613120_p231mcpsimp"></a>addr</p>
</td>
<td class="cellrowborder" valign="top" width="61.629999999999995%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489613120_p233mcpsimp"><a name="zh-cn_topic_0000002489613120_p233mcpsimp"></a><a name="zh-cn_topic_0000002489613120_p233mcpsimp"></a>要配对的设备地址信息。</p>
</td>
<td class="cellrowborder" valign="top" width="21.37%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489613120_p235mcpsimp"><a name="zh-cn_topic_0000002489613120_p235mcpsimp"></a><a name="zh-cn_topic_0000002489613120_p235mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002489613120_table237mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002489613120_row243mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002489613120_p245mcpsimp"><a name="zh-cn_topic_0000002489613120_p245mcpsimp"></a><a name="zh-cn_topic_0000002489613120_p245mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002489613120_p247mcpsimp"><a name="zh-cn_topic_0000002489613120_p247mcpsimp"></a><a name="zh-cn_topic_0000002489613120_p247mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002489613120_p249mcpsimp"><a name="zh-cn_topic_0000002489613120_p249mcpsimp"></a><a name="zh-cn_topic_0000002489613120_p249mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002489613120_row250mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489613120_p252mcpsimp"><a name="zh-cn_topic_0000002489613120_p252mcpsimp"></a><a name="zh-cn_topic_0000002489613120_p252mcpsimp"></a>errcode_sle_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489613120_p254mcpsimp"><a name="zh-cn_topic_0000002489613120_p254mcpsimp"></a><a name="zh-cn_topic_0000002489613120_p254mcpsimp"></a>ERRCODE_SLE_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002489613120_p255mcpsimp"><a name="zh-cn_topic_0000002489613120_p255mcpsimp"></a><a name="zh-cn_topic_0000002489613120_p255mcpsimp"></a>其他：失败，错误码见errcode_sle_t，定义在sle_errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489613120_p257mcpsimp"><a name="zh-cn_topic_0000002489613120_p257mcpsimp"></a><a name="zh-cn_topic_0000002489613120_p257mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

该接口完成星闪的连接及配对逻辑，有两次回调上报，即连接完成回调connect\_state\_cb以及配对完成回调pair\_complete\_cb。

### bs\_sle\_disconnect<a name="ZH-CN_TOPIC_0000002522478575"></a>

【描述】

用于断开与某个配对设备对象的连接。

【接口定义】

```
errcode_t bs_sle_disconnect(sle_obj_t obj);
```

【参数】

<a name="zh-cn_topic_0000002521612953_table915mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002521612953_row921mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002521612953_p923mcpsimp"><a name="zh-cn_topic_0000002521612953_p923mcpsimp"></a><a name="zh-cn_topic_0000002521612953_p923mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.4%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002521612953_p925mcpsimp"><a name="zh-cn_topic_0000002521612953_p925mcpsimp"></a><a name="zh-cn_topic_0000002521612953_p925mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.599999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002521612953_p927mcpsimp"><a name="zh-cn_topic_0000002521612953_p927mcpsimp"></a><a name="zh-cn_topic_0000002521612953_p927mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002521612953_row928mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002521612953_p930mcpsimp"><a name="zh-cn_topic_0000002521612953_p930mcpsimp"></a><a name="zh-cn_topic_0000002521612953_p930mcpsimp"></a>obj</p>
</td>
<td class="cellrowborder" valign="top" width="62.4%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002521612953_p932mcpsimp"><a name="zh-cn_topic_0000002521612953_p932mcpsimp"></a><a name="zh-cn_topic_0000002521612953_p932mcpsimp"></a>要断开连接的配对设备对象。</p>
</td>
<td class="cellrowborder" valign="top" width="20.599999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002521612953_p934mcpsimp"><a name="zh-cn_topic_0000002521612953_p934mcpsimp"></a><a name="zh-cn_topic_0000002521612953_p934mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002521612953_table936mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002521612953_row942mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002521612953_p944mcpsimp"><a name="zh-cn_topic_0000002521612953_p944mcpsimp"></a><a name="zh-cn_topic_0000002521612953_p944mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002521612953_p946mcpsimp"><a name="zh-cn_topic_0000002521612953_p946mcpsimp"></a><a name="zh-cn_topic_0000002521612953_p946mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002521612953_p948mcpsimp"><a name="zh-cn_topic_0000002521612953_p948mcpsimp"></a><a name="zh-cn_topic_0000002521612953_p948mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002521612953_row949mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002521612953_p951mcpsimp"><a name="zh-cn_topic_0000002521612953_p951mcpsimp"></a><a name="zh-cn_topic_0000002521612953_p951mcpsimp"></a>errcode_sle_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002521612953_p953mcpsimp"><a name="zh-cn_topic_0000002521612953_p953mcpsimp"></a><a name="zh-cn_topic_0000002521612953_p953mcpsimp"></a>ERRCODE_SLE_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002521612953_p954mcpsimp"><a name="zh-cn_topic_0000002521612953_p954mcpsimp"></a><a name="zh-cn_topic_0000002521612953_p954mcpsimp"></a>其他：失败，错误码见errcode_sle_t，定义在sle_errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002521612953_p956mcpsimp"><a name="zh-cn_topic_0000002521612953_p956mcpsimp"></a><a name="zh-cn_topic_0000002521612953_p956mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

SLE Obj（星闪设备对象编号）与设备dev\_id是一对多的关系，在设备回调中上报，手表场景下1个Obj包含一个设备，Obj与Dev\_id是一对一关系。

### bs\_sle\_remove\_pair<a name="ZH-CN_TOPIC_0000002490118820"></a>

【描述】

删除某个设备的配对数据。

【接口定义】

```
errcode_t bs_sle_remove_pair(sle_obj_t obj);
```

【参数】

<a name="zh-cn_topic_0000002489453154_table296mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002489453154_row302mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002489453154_p304mcpsimp"><a name="zh-cn_topic_0000002489453154_p304mcpsimp"></a><a name="zh-cn_topic_0000002489453154_p304mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.5%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002489453154_p306mcpsimp"><a name="zh-cn_topic_0000002489453154_p306mcpsimp"></a><a name="zh-cn_topic_0000002489453154_p306mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.5%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002489453154_p308mcpsimp"><a name="zh-cn_topic_0000002489453154_p308mcpsimp"></a><a name="zh-cn_topic_0000002489453154_p308mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002489453154_row309mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489453154_p311mcpsimp"><a name="zh-cn_topic_0000002489453154_p311mcpsimp"></a><a name="zh-cn_topic_0000002489453154_p311mcpsimp"></a>obj</p>
</td>
<td class="cellrowborder" valign="top" width="61.5%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489453154_p313mcpsimp"><a name="zh-cn_topic_0000002489453154_p313mcpsimp"></a><a name="zh-cn_topic_0000002489453154_p313mcpsimp"></a>要断开连接的配对设备对象。</p>
</td>
<td class="cellrowborder" valign="top" width="21.5%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489453154_p315mcpsimp"><a name="zh-cn_topic_0000002489453154_p315mcpsimp"></a><a name="zh-cn_topic_0000002489453154_p315mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002489453154_table317mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002489453154_row323mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002489453154_p325mcpsimp"><a name="zh-cn_topic_0000002489453154_p325mcpsimp"></a><a name="zh-cn_topic_0000002489453154_p325mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002489453154_p327mcpsimp"><a name="zh-cn_topic_0000002489453154_p327mcpsimp"></a><a name="zh-cn_topic_0000002489453154_p327mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002489453154_p329mcpsimp"><a name="zh-cn_topic_0000002489453154_p329mcpsimp"></a><a name="zh-cn_topic_0000002489453154_p329mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002489453154_row330mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489453154_p332mcpsimp"><a name="zh-cn_topic_0000002489453154_p332mcpsimp"></a><a name="zh-cn_topic_0000002489453154_p332mcpsimp"></a>errcode_sle_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489453154_p334mcpsimp"><a name="zh-cn_topic_0000002489453154_p334mcpsimp"></a><a name="zh-cn_topic_0000002489453154_p334mcpsimp"></a>ERRCODE_SLE_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002489453154_p335mcpsimp"><a name="zh-cn_topic_0000002489453154_p335mcpsimp"></a><a name="zh-cn_topic_0000002489453154_p335mcpsimp"></a>其他：失败，错误码见errcode_sle_t，定义在sle_errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489453154_p337mcpsimp"><a name="zh-cn_topic_0000002489453154_p337mcpsimp"></a><a name="zh-cn_topic_0000002489453154_p337mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

### bs\_sle\_start\_auto\_conn<a name="ZH-CN_TOPIC_0000002522318591"></a>

【描述】

开启背景回连功能。

【接口定义】

```
errcode_t bs_sle_start_auto_conn(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002521732939_table886mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002521732939_row892mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002521732939_p894mcpsimp"><a name="zh-cn_topic_0000002521732939_p894mcpsimp"></a><a name="zh-cn_topic_0000002521732939_p894mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002521732939_p896mcpsimp"><a name="zh-cn_topic_0000002521732939_p896mcpsimp"></a><a name="zh-cn_topic_0000002521732939_p896mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002521732939_p898mcpsimp"><a name="zh-cn_topic_0000002521732939_p898mcpsimp"></a><a name="zh-cn_topic_0000002521732939_p898mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002521732939_row899mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002521732939_p901mcpsimp"><a name="zh-cn_topic_0000002521732939_p901mcpsimp"></a><a name="zh-cn_topic_0000002521732939_p901mcpsimp"></a>errcode_sle_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002521732939_p903mcpsimp"><a name="zh-cn_topic_0000002521732939_p903mcpsimp"></a><a name="zh-cn_topic_0000002521732939_p903mcpsimp"></a>ERRCODE_SLE_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002521732939_p904mcpsimp"><a name="zh-cn_topic_0000002521732939_p904mcpsimp"></a><a name="zh-cn_topic_0000002521732939_p904mcpsimp"></a>其他：失败，错误码见errcode_sle_t，定义在sle_errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002521732939_p906mcpsimp"><a name="zh-cn_topic_0000002521732939_p906mcpsimp"></a><a name="zh-cn_topic_0000002521732939_p906mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

在首次配对之后，会进行持久化，开启之后，会自动根据持久化的设备信息和认证信息进行连接，不需要进行主动配对，当前暂不支持持久化，该接口暂不适用。

### bs\_sle\_stop\_auto\_conn<a name="ZH-CN_TOPIC_0000002490278794"></a>

【描述】

停止背景回连功能。

【接口定义】

```
errcode_t bs_sle_stop_auto_conn(void);
```

【参数】

无。

【返回值】

<a name="zh-cn_topic_0000002489613122_table364mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002489613122_row370mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002489613122_p372mcpsimp"><a name="zh-cn_topic_0000002489613122_p372mcpsimp"></a><a name="zh-cn_topic_0000002489613122_p372mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002489613122_p374mcpsimp"><a name="zh-cn_topic_0000002489613122_p374mcpsimp"></a><a name="zh-cn_topic_0000002489613122_p374mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002489613122_p376mcpsimp"><a name="zh-cn_topic_0000002489613122_p376mcpsimp"></a><a name="zh-cn_topic_0000002489613122_p376mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002489613122_row377mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489613122_p379mcpsimp"><a name="zh-cn_topic_0000002489613122_p379mcpsimp"></a><a name="zh-cn_topic_0000002489613122_p379mcpsimp"></a>errcode_sle_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489613122_p381mcpsimp"><a name="zh-cn_topic_0000002489613122_p381mcpsimp"></a><a name="zh-cn_topic_0000002489613122_p381mcpsimp"></a>ERRCODE_SLE_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002489613122_p382mcpsimp"><a name="zh-cn_topic_0000002489613122_p382mcpsimp"></a><a name="zh-cn_topic_0000002489613122_p382mcpsimp"></a>其他：失败，错误码见errcode_sle_t，定义在sle_errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489613122_p384mcpsimp"><a name="zh-cn_topic_0000002489613122_p384mcpsimp"></a><a name="zh-cn_topic_0000002489613122_p384mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

无。

### bs\_sle\_register\_device\_callbacks<a name="ZH-CN_TOPIC_0000002522478577"></a>

【描述】

星闪对于注册设备的回调函数。

【接口定义】

```
errcode_t bs_sle_register_device_callbacks(uint32_t app_id, bs_sle_device_callbacks_t *cbks);
```

【参数】

<a name="zh-cn_topic_0000002521612955_table510mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002521612955_row516mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002521612955_p518mcpsimp"><a name="zh-cn_topic_0000002521612955_p518mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p518mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="61.89%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002521612955_p520mcpsimp"><a name="zh-cn_topic_0000002521612955_p520mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p520mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21.11%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002521612955_p522mcpsimp"><a name="zh-cn_topic_0000002521612955_p522mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p522mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002521612955_row523mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002521612955_p525mcpsimp"><a name="zh-cn_topic_0000002521612955_p525mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p525mcpsimp"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="61.89%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002521612955_p527mcpsimp"><a name="zh-cn_topic_0000002521612955_p527mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p527mcpsimp"></a>区分不同应用的标识。</p>
</td>
<td class="cellrowborder" valign="top" width="21.11%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002521612955_p529mcpsimp"><a name="zh-cn_topic_0000002521612955_p529mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p529mcpsimp"></a>输入</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002521612955_row530mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002521612955_p532mcpsimp"><a name="zh-cn_topic_0000002521612955_p532mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p532mcpsimp"></a>cbks</p>
</td>
<td class="cellrowborder" valign="top" width="61.89%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002521612955_p534mcpsimp"><a name="zh-cn_topic_0000002521612955_p534mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p534mcpsimp"></a>要注册的回调函数，详见“<a href="#ZH-CN_TOPIC_0000002490278786">bs_sle_callbacks_t</a>”。</p>
</td>
<td class="cellrowborder" valign="top" width="21.11%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002521612955_p536mcpsimp"><a name="zh-cn_topic_0000002521612955_p536mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p536mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

<a name="zh-cn_topic_0000002521612955_table538mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002521612955_row544mcpsimp"><th class="cellrowborder" valign="top" width="16%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002521612955_p546mcpsimp"><a name="zh-cn_topic_0000002521612955_p546mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p546mcpsimp"></a>返回值</p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002521612955_p548mcpsimp"><a name="zh-cn_topic_0000002521612955_p548mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p548mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="21%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002521612955_p550mcpsimp"><a name="zh-cn_topic_0000002521612955_p550mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p550mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002521612955_row551mcpsimp"><td class="cellrowborder" valign="top" width="16%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002521612955_p553mcpsimp"><a name="zh-cn_topic_0000002521612955_p553mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p553mcpsimp"></a>errcode_sle_t</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002521612955_p555mcpsimp"><a name="zh-cn_topic_0000002521612955_p555mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p555mcpsimp"></a>ERRCODE_SLE_SUCCESS：成功；</p>
<p id="zh-cn_topic_0000002521612955_p556mcpsimp"><a name="zh-cn_topic_0000002521612955_p556mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p556mcpsimp"></a>其他：失败，错误码见errcode_sle_t，定义在sle_errcode.h。</p>
</td>
<td class="cellrowborder" valign="top" width="21%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002521612955_p558mcpsimp"><a name="zh-cn_topic_0000002521612955_p558mcpsimp"></a><a name="zh-cn_topic_0000002521612955_p558mcpsimp"></a>输出</p>
</td>
</tr>
</tbody>
</table>

【注意事项】

此回调会将星闪Obj与dev id的对应关系上报。

### bs\_sle\_unregister\_device\_callbacks<a name="ZH-CN_TOPIC_0000002490118822"></a>

【描述】

星闪取消注册设备的回调函数。

【接口定义】

```
void bs_sle_unregister_device_callbacks(uint32_t app_id);
```

【参数】

<a name="zh-cn_topic_0000002489453156_table266mcpsimp"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002489453156_row272mcpsimp"><th class="cellrowborder" valign="top" width="17%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002489453156_p274mcpsimp"><a name="zh-cn_topic_0000002489453156_p274mcpsimp"></a><a name="zh-cn_topic_0000002489453156_p274mcpsimp"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.4%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002489453156_p276mcpsimp"><a name="zh-cn_topic_0000002489453156_p276mcpsimp"></a><a name="zh-cn_topic_0000002489453156_p276mcpsimp"></a>描述</p>
</th>
<th class="cellrowborder" valign="top" width="20.599999999999998%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002489453156_p278mcpsimp"><a name="zh-cn_topic_0000002489453156_p278mcpsimp"></a><a name="zh-cn_topic_0000002489453156_p278mcpsimp"></a>输入/输出</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002489453156_row279mcpsimp"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002489453156_p281mcpsimp"><a name="zh-cn_topic_0000002489453156_p281mcpsimp"></a><a name="zh-cn_topic_0000002489453156_p281mcpsimp"></a>app_id</p>
</td>
<td class="cellrowborder" valign="top" width="62.4%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002489453156_p283mcpsimp"><a name="zh-cn_topic_0000002489453156_p283mcpsimp"></a><a name="zh-cn_topic_0000002489453156_p283mcpsimp"></a>区分不同应用的标识。</p>
</td>
<td class="cellrowborder" valign="top" width="20.599999999999998%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002489453156_p285mcpsimp"><a name="zh-cn_topic_0000002489453156_p285mcpsimp"></a><a name="zh-cn_topic_0000002489453156_p285mcpsimp"></a>输入</p>
</td>
</tr>
</tbody>
</table>

【返回值】

无。

【注意事项】

无。

## 开发指导<a name="ZH-CN_TOPIC_0000002522318593"></a>

1.  星闪Service资源初始化：调用sle\_service\_init接口初始化SLE Service资源。
2.  星闪设备信息配置：通过bs\_config接口以及“protocol/bt/host/include/sle/sle\_service/ssf\_config.h”头文件中定义的各种配置项完成星闪设备信息及资源配置，如设备角色、地址、名称及设备外观等。
3.  星闪设备回调注册：调用bs\_sle\_register\_device\_callbacks注册星闪对于注册设备的回调函数。
4.  协议栈状态回调注册：调用bs\_sle\_register\_stack\_state\_callbacks接口注册协议栈回调，通过bs\_sle\_enable接口使能协议栈。
5.  设备连接、配对相关回调注册：通过bs\_sle\_register\_callbacks接口注册sle service回调函数。
6.  设备进入发现态：协议栈使能回调返回正常使能状态后，调用bs\_sle\_start\_discovery接口进入发现模式，如果当前设备是中央设备，则进入发现态，起扫描，查询外围设备；如果当前设备是外围设备，则进入发现态，起广播。
7.  设备连接配对：中央设备扫描到对端设备后，调用bs\_sle\_start\_pair接口与对端设备进行连接配对；外围设备接收对端连接请求，连接状态、dev\_id等通过connect\_state\_cb回调上报；星闪对象编号obj通过bs\_sle\_obj\_alloc\_cbk回调上报。
8.  设备断开连接、取消配对设备：中央设备通过bs\_sle\_disconnect接口与对端设备断链；通过bs\_sle\_remove\_pair接口删除对端设备的配对数据。

# 缩略语<a name="ZH-CN_TOPIC_0000002490278796"></a>

<a name="zh-cn_topic_0000002521979203_table183211341529"></a>
<table><tbody><tr id="zh-cn_topic_0000002521979203_row93831646213"><td class="nocellnorowborder" style="border:none" valign="top" width="20.808080808080806%"><p id="zh-cn_topic_0000002521979203_p73831841824"><a name="zh-cn_topic_0000002521979203_p73831841824"></a><a name="zh-cn_topic_0000002521979203_p73831841824"></a><strong id="zh-cn_topic_0000002521979203_b9383941427"><a name="zh-cn_topic_0000002521979203_b9383941427"></a><a name="zh-cn_topic_0000002521979203_b9383941427"></a>A</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="36.38383838383839%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="42.80808080808081%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002521979203_row1238334828"><td class="nocellnorowborder" style="border:none" valign="top" width="20.808080808080806%"><p id="zh-cn_topic_0000002521979203_p4383184226"><a name="zh-cn_topic_0000002521979203_p4383184226"></a><a name="zh-cn_topic_0000002521979203_p4383184226"></a><strong id="zh-cn_topic_0000002521979203_b1738384929"><a name="zh-cn_topic_0000002521979203_b1738384929"></a><a name="zh-cn_topic_0000002521979203_b1738384929"></a>API</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="36.38383838383839%"><p id="zh-cn_topic_0000002521979203_p1738310413218"><a name="zh-cn_topic_0000002521979203_p1738310413218"></a><a name="zh-cn_topic_0000002521979203_p1738310413218"></a>Application Programming Interface</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="42.80808080808081%"><p id="zh-cn_topic_0000002521979203_p133832041625"><a name="zh-cn_topic_0000002521979203_p133832041625"></a><a name="zh-cn_topic_0000002521979203_p133832041625"></a>应用编程接口</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002521979203_row7383245217"><td class="nocellnorowborder" style="border:none" valign="top" width="20.808080808080806%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="36.38383838383839%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="42.80808080808081%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002521979203_row838518417210"><td class="nocellnorowborder" style="border:none" valign="top" width="20.808080808080806%"><p id="zh-cn_topic_0000002521979203_p1238534125"><a name="zh-cn_topic_0000002521979203_p1238534125"></a><a name="zh-cn_topic_0000002521979203_p1238534125"></a><strong id="zh-cn_topic_0000002521979203_b1345735119316"><a name="zh-cn_topic_0000002521979203_b1345735119316"></a><a name="zh-cn_topic_0000002521979203_b1345735119316"></a>O</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="36.38383838383839%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="42.80808080808081%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002521979203_row18348213314"><td class="nocellnorowborder" style="border:none" valign="top" width="20.808080808080806%"><p id="zh-cn_topic_0000002521979203_p535132110311"><a name="zh-cn_topic_0000002521979203_p535132110311"></a><a name="zh-cn_topic_0000002521979203_p535132110311"></a><strong id="zh-cn_topic_0000002521979203_b74585511933"><a name="zh-cn_topic_0000002521979203_b74585511933"></a><a name="zh-cn_topic_0000002521979203_b74585511933"></a>OBJ</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="36.38383838383839%"><p id="zh-cn_topic_0000002521979203_p235162112315"><a name="zh-cn_topic_0000002521979203_p235162112315"></a><a name="zh-cn_topic_0000002521979203_p235162112315"></a>Object</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="42.80808080808081%"><p id="zh-cn_topic_0000002521979203_p61934381324"><a name="zh-cn_topic_0000002521979203_p61934381324"></a><a name="zh-cn_topic_0000002521979203_p61934381324"></a>星闪合作设备集对象；一个星闪Object包含1个或多个星闪设备dev_id</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002521979203_row1735192113318"><td class="nocellnorowborder" style="border:none" valign="top" width="20.808080808080806%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="36.38383838383839%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="42.80808080808081%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002521979203_row838514924"><td class="nocellnorowborder" style="border:none" valign="top" width="20.808080808080806%"><p id="zh-cn_topic_0000002521979203_p1938514414212"><a name="zh-cn_topic_0000002521979203_p1938514414212"></a><a name="zh-cn_topic_0000002521979203_p1938514414212"></a><strong id="zh-cn_topic_0000002521979203_b15385154123"><a name="zh-cn_topic_0000002521979203_b15385154123"></a><a name="zh-cn_topic_0000002521979203_b15385154123"></a>S</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="36.38383838383839%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="42.80808080808081%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002521979203_row10385841127"><td class="row-nocellborder" style="border:none" valign="top" width="20.808080808080806%"><p id="zh-cn_topic_0000002521979203_p19843154510318"><a name="zh-cn_topic_0000002521979203_p19843154510318"></a><a name="zh-cn_topic_0000002521979203_p19843154510318"></a><strong id="zh-cn_topic_0000002521979203_b9843194518314"><a name="zh-cn_topic_0000002521979203_b9843194518314"></a><a name="zh-cn_topic_0000002521979203_b9843194518314"></a>SLE</strong></p>
</td>
<td class="row-nocellborder" style="border:none" valign="top" width="36.38383838383839%"><p id="zh-cn_topic_0000002521979203_p9843745538"><a name="zh-cn_topic_0000002521979203_p9843745538"></a><a name="zh-cn_topic_0000002521979203_p9843745538"></a>SparkLink Low Energy</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="42.80808080808081%"><p id="zh-cn_topic_0000002521979203_p13843104511317"><a name="zh-cn_topic_0000002521979203_p13843104511317"></a><a name="zh-cn_topic_0000002521979203_p13843104511317"></a>星闪低功耗接入技术</p>
</td>
</tr>
</tbody>
</table>

