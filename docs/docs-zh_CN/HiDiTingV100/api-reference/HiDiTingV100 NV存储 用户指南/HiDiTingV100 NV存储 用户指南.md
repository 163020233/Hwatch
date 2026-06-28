# 前言<a name="ZH-CN_TOPIC_0000002466541652"></a>

**概述<a name="zh-cn_topic_0000002411208236_section4537382116410"></a>**

本文档主要针对HiDiTingV100中NV存储模块使用进行介绍。用于指导工程人员能够快速使用NV模块进行二次开发。

**读者对象<a name="zh-cn_topic_0000002411208236_section4378592816410"></a>**

本文档主要适用于以下工程师：

-   技术支持工程师
-   软件工程师

**符号约定<a name="zh-cn_topic_0000002411208236_section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="zh-cn_topic_0000002411208236_table2622507016410"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002411208236_row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002411208236_p6450074116410"><a name="zh-cn_topic_0000002411208236_p6450074116410"></a><a name="zh-cn_topic_0000002411208236_p6450074116410"></a><strong id="zh-cn_topic_0000002411208236_b2136615816410"><a name="zh-cn_topic_0000002411208236_b2136615816410"></a><a name="zh-cn_topic_0000002411208236_b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002411208236_p5435366816410"><a name="zh-cn_topic_0000002411208236_p5435366816410"></a><a name="zh-cn_topic_0000002411208236_p5435366816410"></a><strong id="zh-cn_topic_0000002411208236_b5941558116410"><a name="zh-cn_topic_0000002411208236_b5941558116410"></a><a name="zh-cn_topic_0000002411208236_b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002411208236_row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002411208236_p3734547016410"><a name="zh-cn_topic_0000002411208236_p3734547016410"></a><a name="zh-cn_topic_0000002411208236_p3734547016410"></a><a name="zh-cn_topic_0000002411208236_image2670064316410"></a><a name="zh-cn_topic_0000002411208236_image2670064316410"></a><span><img class="" id="zh-cn_topic_0000002411208236_image2670064316410" src="figures/zh-cn_image_0000002469220110.png" width="55.9265" height="25.270000000000003"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002411208236_p1757432116410"><a name="zh-cn_topic_0000002411208236_p1757432116410"></a><a name="zh-cn_topic_0000002411208236_p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411208236_row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002411208236_p1432579516410"><a name="zh-cn_topic_0000002411208236_p1432579516410"></a><a name="zh-cn_topic_0000002411208236_p1432579516410"></a><a name="zh-cn_topic_0000002411208236_image4895582316410"></a><a name="zh-cn_topic_0000002411208236_image4895582316410"></a><span><img class="" id="zh-cn_topic_0000002411208236_image4895582316410" src="figures/zh-cn_image_0000002502220071.png" width="55.9265" height="25.270000000000003"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002411208236_p959197916410"><a name="zh-cn_topic_0000002411208236_p959197916410"></a><a name="zh-cn_topic_0000002411208236_p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411208236_row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002411208236_p1232579516410"><a name="zh-cn_topic_0000002411208236_p1232579516410"></a><a name="zh-cn_topic_0000002411208236_p1232579516410"></a><a name="zh-cn_topic_0000002411208236_image1235582316410"></a><a name="zh-cn_topic_0000002411208236_image1235582316410"></a><span><img class="" id="zh-cn_topic_0000002411208236_image1235582316410" src="figures/zh-cn_image_0000002469380094.png" width="55.9265" height="25.270000000000003"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002411208236_p123197916410"><a name="zh-cn_topic_0000002411208236_p123197916410"></a><a name="zh-cn_topic_0000002411208236_p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411208236_row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002411208236_p2204984716410"><a name="zh-cn_topic_0000002411208236_p2204984716410"></a><a name="zh-cn_topic_0000002411208236_p2204984716410"></a><a name="zh-cn_topic_0000002411208236_image4504446716410"></a><a name="zh-cn_topic_0000002411208236_image4504446716410"></a><span><img class="" id="zh-cn_topic_0000002411208236_image4504446716410" src="figures/zh-cn_image_0000002502100085.png" width="55.9265" height="25.270000000000003"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002411208236_p4388861916410"><a name="zh-cn_topic_0000002411208236_p4388861916410"></a><a name="zh-cn_topic_0000002411208236_p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="zh-cn_topic_0000002411208236_p1238861916410"><a name="zh-cn_topic_0000002411208236_p1238861916410"></a><a name="zh-cn_topic_0000002411208236_p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002411208236_row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002411208236_p5555360116410"><a name="zh-cn_topic_0000002411208236_p5555360116410"></a><a name="zh-cn_topic_0000002411208236_p5555360116410"></a><a name="zh-cn_topic_0000002411208236_image799324016410"></a><a name="zh-cn_topic_0000002411208236_image799324016410"></a><span><img class="" id="zh-cn_topic_0000002411208236_image799324016410" src="figures/zh-cn_image_0000002469220112.png" width="47.88" height="15.96"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002411208236_p4612588116410"><a name="zh-cn_topic_0000002411208236_p4612588116410"></a><a name="zh-cn_topic_0000002411208236_p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="zh-cn_topic_0000002411208236_p1232588116410"><a name="zh-cn_topic_0000002411208236_p1232588116410"></a><a name="zh-cn_topic_0000002411208236_p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# NV简介<a name="ZH-CN_TOPIC_0000002499581037"></a>

NV模块用于本地存储器中存储非易失性数据。NV中的每项数据以类似key-value的方式进行定义，数据项中包含唯一的索引key和自定义数据类型的value。

NV项可通过两种方式进行存储：编译预置和API写入。

-   编译预置是指开发者可在代码编译阶段，通过修改NV头文件和NV配置文件的方式生成客制化的NV镜像，在镜像烧录的过程中统一烧录到存储介质中。预置的NV在代码运行阶段可通过API接口进行读取和更新。
-   API写入是用户可直接在代码中调用API接口写入新的NV项，具体使用方法请参见“[NV API指南](#ZH-CN_TOPIC_0000002466382024)”章节。

以Flash为例，NV项存储在Flash中时，以Flash器件的sector（扇区，存储空间的基本逻辑单元）为单位进行管理，NV的每个页为一个sector大小（4096Byte），NV页的数量默认配置为2页。除去管理结构，单个非加密NV项的有效数据最大不应超过4060Byte。

# NV编译预置<a name="ZH-CN_TOPIC_0000002466382040"></a>

-   **[新增NV项](#ZH-CN_TOPIC_0000002499501013)**  

-   **[编译生成NV镜像](#ZH-CN_TOPIC_0000002499500997)**  

## 新增NV项<a name="ZH-CN_TOPIC_0000002499501013"></a>

-   **[新增NV项流程](#ZH-CN_TOPIC_0000002499581053)**  

-   **[新增NV项示例](#ZH-CN_TOPIC_0000002499500977)**  

### 新增NV项流程<a name="ZH-CN_TOPIC_0000002499581053"></a>

新增NV项流程如下：

1.  在头文件中新增kvalue的数据类型定义（非必须，如果是通用类型数据可忽略此步骤）。
2.  在json文件中新增NV描述项。

**新增kvalue数据类型<a name="zh-cn_topic_0000002411048372_section206861047121115"></a>**

-   通用数据类型：uint8\_t、uint16\_t、uint32\_t、bool。
-   自定义数据类型：支持自定义枚举（enum）类型和结构体（struct）类型。
-   自定义数据类型存放路径：middleware/chips/3322/nv/nv\_config/nv\_default/include/common.h
-   当用户使用通用或已定义的数据类型时，不涉及本小节的修改；当用户要新增枚举或结构体类型时，需在上述文件中定义。

**新增NV描述项<a name="zh-cn_topic_0000002411048372_section13134513141310"></a>**

-   NV描述项文件路径：middleware/chips/3322/nv/nv\_config/nv\_default/cfg/acore/app.json
-   定义说明：

    **表 1**  NV配置选项说明

    <a name="zh-cn_topic_0000002411048372_table8802115017551"></a>
    <table><thead align="left"><tr id="zh-cn_topic_0000002411048372_row3802550105510"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002411048372_p680265010550"><a name="zh-cn_topic_0000002411048372_p680265010550"></a><a name="zh-cn_topic_0000002411048372_p680265010550"></a>NV配置选项</p>
    </th>
    <th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002411048372_p7802115012558"><a name="zh-cn_topic_0000002411048372_p7802115012558"></a><a name="zh-cn_topic_0000002411048372_p7802115012558"></a>说明</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="zh-cn_topic_0000002411048372_row1780215504558"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002411048372_p188021550125516"><a name="zh-cn_topic_0000002411048372_p188021550125516"></a><a name="zh-cn_topic_0000002411048372_p188021550125516"></a>key_id</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002411048372_p198021950105511"><a name="zh-cn_topic_0000002411048372_p198021950105511"></a><a name="zh-cn_topic_0000002411048372_p198021950105511"></a>NV项的ID。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002411048372_row198025507556"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002411048372_p14802115012556"><a name="zh-cn_topic_0000002411048372_p14802115012556"></a><a name="zh-cn_topic_0000002411048372_p14802115012556"></a>key_status</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002411048372_p1780235085512"><a name="zh-cn_topic_0000002411048372_p1780235085512"></a><a name="zh-cn_topic_0000002411048372_p1780235085512"></a>NV项的状态。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002411048372_row480295035518"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002411048372_p18021450165513"><a name="zh-cn_topic_0000002411048372_p18021450165513"></a><a name="zh-cn_topic_0000002411048372_p18021450165513"></a>structure_type</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002411048372_p198021450155516"><a name="zh-cn_topic_0000002411048372_p198021450155516"></a><a name="zh-cn_topic_0000002411048372_p198021450155516"></a>NV项的数据结构类型。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002411048372_row1580225015551"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002411048372_p880245035512"><a name="zh-cn_topic_0000002411048372_p880245035512"></a><a name="zh-cn_topic_0000002411048372_p880245035512"></a>attributions</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002411048372_p1280225015519"><a name="zh-cn_topic_0000002411048372_p1280225015519"></a><a name="zh-cn_topic_0000002411048372_p1280225015519"></a>NV项的属性值。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002411048372_row10802125014557"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002411048372_p20802450155516"><a name="zh-cn_topic_0000002411048372_p20802450155516"></a><a name="zh-cn_topic_0000002411048372_p20802450155516"></a>value</p>
    </td>
    <td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002411048372_p08025503552"><a name="zh-cn_topic_0000002411048372_p08025503552"></a><a name="zh-cn_topic_0000002411048372_p08025503552"></a>NV项的数据。</p>
    </td>
    </tr>
    </tbody>
    </table>

    NV配置文件示例：

    ```
    "common":{
        "module_id": "0x0",
    
        "unused": {
            "key_id": "0x0",
            "key_status": "reserve",
            "structure_type": "uint8_t",
            "attributions:": 1,
            "value": [0]
        },
        "sample": {
            "key_id": "0x1",
            "key_status": "alive",
            "structure_type": "sample_type_t",
            "attributions": 1,
            "value": [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17]
        }
    }
    ```

    在NV的配置中，每个字段的详细描述如下：

    -   key\_id：

        以十六进制形式给出的NV项ID。key\_id必须唯一，不能重复，因此建议用户对key\_id的取值进行管理，如：key\_id由16bit组成，可规定高8bit为所在模块内的module\_id，低8bit在模块内取值，避免不同模块使用NV互相影响。

    -   key\_status：

        用于标记是否将该项的NV值编到生成的bin文件中。该字段为“alive”，表示NV项生效，当前固件版本正在使用此key；若为其他的字段或空，则不生效。

    -   structure\_type：

        NV项的数据类型。已在“[新增kvalue数据类型](#zh-cn_topic_0000002411048372_section206861047121115)”中详细描述。

    -   attributions：NV项属性值。1 、2、4为互斥关系，三选一。

        1：Normal nv（普通NV）。

        2：Permanent nv（不可修改）。

        4：Un-upgrade nv（不随版本升级而修改）。

    -   value：

        如果value不是上述通用数据类型，任何结构都必须以列表的形式书写，有如下两种情况：

        -   列表所有成员全部赋值。
        -   只对列表前面若干个成员赋值。这表明对末尾未赋值的成员缺省赋值为0。

### 新增NV项示例<a name="ZH-CN_TOPIC_0000002499500977"></a>

-   在“middleware/chips/3322/nv/nv\_config/nv\_default/include/common.h”文件中新增自定义结构体。新增自定义数据类型示例如下：
    -   新增结构体类型且结构体内都为基础类型：

        ```
        typedef struct {
            int8_t param1;
            int8_t param2;
            int8_t param3;
            int8_t param4;
            int8_t param5;
            uint32_t param6;
            uint32_t param7;
            int32_t param8;
            uint32_t param9;
            uint32_t param10;
            uint32_t param11;
            uint32_t param12;
            uint32_t param13;
            uint32_t param14;
            uint32_t param15;
            uint32_t param16;
            uint32_t param17;
        } sample_type_t;
        ```

    -   新增结构体类型且结构体内有数组类型：

        ```
        typedef struct {
            uint16_t param1; 
            uint16_t param2; 
            uint16_t param3;
            uint16_t param4[2];
        } sample_two;
        ```

    -   新增枚举类型：

        ```
        typedef enum {
            PARAM1,
            PARAM2,
            PARAM3,
            PARAM4
        } sample_three;
        ```

-   在“middleware/chips/3322/nv/nv\_config/nv\_default/cfg/acore/app.json”文件中添加新的NV项。
    -   当kvalue预置值类型是基础类型时，添加kvalue预置值如下所示，可在“app.json”配置文件直接添加，无需在头文件中新增。

        添加kvalue预置值为基础类型：

        ```
        "sample1":{
            "key_id": "0x1",
            "key_status": "alive",
            "structure_type": "uint8_t",
            "attributions": 1,
            "value": 0
        }
        ```

    -   当kvalue预置值类型是结构体类型时，添加kvalue预置值如下所示，该kvalue值要对应头文件中已有的结构体，如果没有需手动添加自定义结构体。

        添加kvalue预置值为结构体类型：

        ```
        "sample2":{
            "key_id": "0x2",
            "key_status": "alive",
            "structure_type": "sample_type_t",
            "attributions": 1,
            "value": [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17]
        }
        ```

        >![](public_sys-resources/icon-note.gif) **说明：** 
        >当添加了结构体并如上所示调用后，需先编译任意A核版本生成必要中间文件后再执行“[编译生成NV镜像](#ZH-CN_TOPIC_0000002499500997)”中编译命令，才能生成NV镜像。

## 编译生成NV镜像<a name="ZH-CN_TOPIC_0000002499500997"></a>

重新执行工程编译命令，将会自动生成NV镜像，并且打包到用于烧写的fwpkg文件中。

# NV API指南<a name="ZH-CN_TOPIC_0000002466382024"></a>

-   **[功能描述](#ZH-CN_TOPIC_0000002466541664)**  

-   **[接口说明](#ZH-CN_TOPIC_0000002499581025)**  

-   **[开发指引](#ZH-CN_TOPIC_0000002499501029)**  

-   **[注意事项](#ZH-CN_TOPIC_0000002466382076)**  

-   **[常见问题](#ZH-CN_TOPIC_0000002466541688)**  

## 功能描述<a name="ZH-CN_TOPIC_0000002466541664"></a>

NV API主要提供以下几种功能：

-   NV项写入：

    保存需要存储的格式化数据，同时可以定义写入数据的三种属性，分别为是否永久存储、是否加密存储和是否不可升级。

-   NV项读取：

    从本地存储器读取NV数据。

-   NV信息查询：
    -   查询NV是否已存储于本地存储器中。
    -   查询NV空间的使用状态。

-   NV刷新缓存中数据到flash：

    仅NV支持异步存储时有效，在下电、关机、重启等场景调用，保证NV数据不丢失。

NV项的写入接口可设置NV项的属性，对于通过API动态添加的NV项，可在其写入的接口中传入其所拥有的特殊属性。

## 接口说明<a name="ZH-CN_TOPIC_0000002499581025"></a>

使用NV接口需要引用NV接口头文件，路径：include/middleware/utils/nv.h

NV模块主要提供以下API：

【函数接口】

```
errcode_t uapi_nv_write(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length)
```

【函数功能】写入NV数据项，默认属性Normal，无回调函数。

【参数说明】

<a name="zh-cn_topic_0000002444727517_table6707155564719"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002444727517_row4707135514715"><th class="cellrowborder" valign="top" width="23.150000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002444727517_p770717558479"><a name="zh-cn_topic_0000002444727517_p770717558479"></a><a name="zh-cn_topic_0000002444727517_p770717558479"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="76.85%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002444727517_p11707755204711"><a name="zh-cn_topic_0000002444727517_p11707755204711"></a><a name="zh-cn_topic_0000002444727517_p11707755204711"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002444727517_row67071055184710"><td class="cellrowborder" valign="top" width="23.150000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p10707115514471"><a name="zh-cn_topic_0000002444727517_p10707115514471"></a><a name="zh-cn_topic_0000002444727517_p10707115514471"></a>key</p>
</td>
<td class="cellrowborder" valign="top" width="76.85%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p27071855154716"><a name="zh-cn_topic_0000002444727517_p27071855154716"></a><a name="zh-cn_topic_0000002444727517_p27071855154716"></a>要写入的NV项的key ID，用于索引。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444727517_row1707165554718"><td class="cellrowborder" valign="top" width="23.150000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p13707185512478"><a name="zh-cn_topic_0000002444727517_p13707185512478"></a><a name="zh-cn_topic_0000002444727517_p13707185512478"></a>*kvalue</p>
</td>
<td class="cellrowborder" valign="top" width="76.85%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p137072555474"><a name="zh-cn_topic_0000002444727517_p137072555474"></a><a name="zh-cn_topic_0000002444727517_p137072555474"></a>指向要写入的NV项的值的指针。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444727517_row1370775524718"><td class="cellrowborder" valign="top" width="23.150000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p1870725518470"><a name="zh-cn_topic_0000002444727517_p1870725518470"></a><a name="zh-cn_topic_0000002444727517_p1870725518470"></a>kvalue_length</p>
</td>
<td class="cellrowborder" valign="top" width="76.85%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p370714556478"><a name="zh-cn_topic_0000002444727517_p370714556478"></a><a name="zh-cn_topic_0000002444727517_p370714556478"></a>写入数据的长度，单位：Byte。</p>
</td>
</tr>
</tbody>
</table>

【函数接口】

```
errcode_t uapi_nv_write_with_attr(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length,nv_key_attr_t *attr, nv_storage_completed_callback func)
```

【函数功能】写入NV数据项，并根据业务需求配置属性及回调函数。

【参数说明】

<a name="zh-cn_topic_0000002444727517_table7346114574915"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002444727517_row23461045114920"><th class="cellrowborder" valign="top" width="23.39%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002444727517_p1634612458495"><a name="zh-cn_topic_0000002444727517_p1634612458495"></a><a name="zh-cn_topic_0000002444727517_p1634612458495"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="76.61%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002444727517_p5346114514496"><a name="zh-cn_topic_0000002444727517_p5346114514496"></a><a name="zh-cn_topic_0000002444727517_p5346114514496"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002444727517_row234634518498"><td class="cellrowborder" valign="top" width="23.39%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p1734644517499"><a name="zh-cn_topic_0000002444727517_p1734644517499"></a><a name="zh-cn_topic_0000002444727517_p1734644517499"></a>key</p>
</td>
<td class="cellrowborder" valign="top" width="76.61%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p10346174515497"><a name="zh-cn_topic_0000002444727517_p10346174515497"></a><a name="zh-cn_topic_0000002444727517_p10346174515497"></a>要写入的NV项的key ID，用于索引。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444727517_row13346154584913"><td class="cellrowborder" valign="top" width="23.39%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p3346114524910"><a name="zh-cn_topic_0000002444727517_p3346114524910"></a><a name="zh-cn_topic_0000002444727517_p3346114524910"></a>*kvalue</p>
</td>
<td class="cellrowborder" valign="top" width="76.61%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p434694554919"><a name="zh-cn_topic_0000002444727517_p434694554919"></a><a name="zh-cn_topic_0000002444727517_p434694554919"></a>指向要写入的NV项的值的指针。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444727517_row134615453495"><td class="cellrowborder" valign="top" width="23.39%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p5346645194914"><a name="zh-cn_topic_0000002444727517_p5346645194914"></a><a name="zh-cn_topic_0000002444727517_p5346645194914"></a>kvalue_length</p>
</td>
<td class="cellrowborder" valign="top" width="76.61%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p134604511495"><a name="zh-cn_topic_0000002444727517_p134604511495"></a><a name="zh-cn_topic_0000002444727517_p134604511495"></a>写入数据的长度，单位：Byte。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444727517_row027852165114"><td class="cellrowborder" valign="top" width="23.39%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p162835235120"><a name="zh-cn_topic_0000002444727517_p162835235120"></a><a name="zh-cn_topic_0000002444727517_p162835235120"></a>*attr</p>
</td>
<td class="cellrowborder" valign="top" width="76.61%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p4281152165118"><a name="zh-cn_topic_0000002444727517_p4281152165118"></a><a name="zh-cn_topic_0000002444727517_p4281152165118"></a>要配置的NV项的属性。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444727517_row446465925112"><td class="cellrowborder" valign="top" width="23.39%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p44641659175110"><a name="zh-cn_topic_0000002444727517_p44641659175110"></a><a name="zh-cn_topic_0000002444727517_p44641659175110"></a>func</p>
</td>
<td class="cellrowborder" valign="top" width="76.61%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p1446413592510"><a name="zh-cn_topic_0000002444727517_p1446413592510"></a><a name="zh-cn_topic_0000002444727517_p1446413592510"></a>kvalue写入Flash后调用的回调函数。</p>
</td>
</tr>
</tbody>
</table>

【函数接口】

```
errcode_t uapi_nv_read(uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue)
```

【函数功能】读取指定NV数据项的值，默认不获取key的属性值。

【参数说明】

<a name="zh-cn_topic_0000002444727517_table525394610498"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002444727517_row72539466499"><th class="cellrowborder" valign="top" width="23.62%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002444727517_p11253164654919"><a name="zh-cn_topic_0000002444727517_p11253164654919"></a><a name="zh-cn_topic_0000002444727517_p11253164654919"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="76.38000000000001%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002444727517_p125318465490"><a name="zh-cn_topic_0000002444727517_p125318465490"></a><a name="zh-cn_topic_0000002444727517_p125318465490"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002444727517_row4253194634912"><td class="cellrowborder" valign="top" width="23.62%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p4253114612499"><a name="zh-cn_topic_0000002444727517_p4253114612499"></a><a name="zh-cn_topic_0000002444727517_p4253114612499"></a>key</p>
</td>
<td class="cellrowborder" valign="top" width="76.38000000000001%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p12531346194919"><a name="zh-cn_topic_0000002444727517_p12531346194919"></a><a name="zh-cn_topic_0000002444727517_p12531346194919"></a>要读取的NV项的key ID，用于索引。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444727517_row925324664918"><td class="cellrowborder" valign="top" width="23.62%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p164554315543"><a name="zh-cn_topic_0000002444727517_p164554315543"></a><a name="zh-cn_topic_0000002444727517_p164554315543"></a>kvalue_max_length</p>
</td>
<td class="cellrowborder" valign="top" width="76.38000000000001%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p9253134611495"><a name="zh-cn_topic_0000002444727517_p9253134611495"></a><a name="zh-cn_topic_0000002444727517_p9253134611495"></a>允许存储数据的最大长度，单位：Byte。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444727517_row11253164644913"><td class="cellrowborder" valign="top" width="23.62%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p20253246104919"><a name="zh-cn_topic_0000002444727517_p20253246104919"></a><a name="zh-cn_topic_0000002444727517_p20253246104919"></a>*kvalue_length</p>
</td>
<td class="cellrowborder" valign="top" width="76.38000000000001%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p225317462493"><a name="zh-cn_topic_0000002444727517_p225317462493"></a><a name="zh-cn_topic_0000002444727517_p225317462493"></a>实际读取到的数据长度。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444727517_row6895182265417"><td class="cellrowborder" valign="top" width="23.62%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p15895102285417"><a name="zh-cn_topic_0000002444727517_p15895102285417"></a><a name="zh-cn_topic_0000002444727517_p15895102285417"></a>*kvalue</p>
</td>
<td class="cellrowborder" valign="top" width="76.38000000000001%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p989519221544"><a name="zh-cn_topic_0000002444727517_p989519221544"></a><a name="zh-cn_topic_0000002444727517_p989519221544"></a>指向保存读取数据的buffer的指针。</p>
</td>
</tr>
</tbody>
</table>

【函数接口】

```
errcode_t uapi_nv_read_with_attr(uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length,uint8_t *kvalue, nv_key_attr_t *attr)
```

【函数功能】读取指定NV数据项的值，同时获取key的属性值。

【参数说明】

<a name="zh-cn_topic_0000002444727517_table71638477494"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002444727517_row15163147174917"><th class="cellrowborder" valign="top" width="23.82%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002444727517_p516344754914"><a name="zh-cn_topic_0000002444727517_p516344754914"></a><a name="zh-cn_topic_0000002444727517_p516344754914"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="76.18%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002444727517_p8163047174919"><a name="zh-cn_topic_0000002444727517_p8163047174919"></a><a name="zh-cn_topic_0000002444727517_p8163047174919"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002444727517_row1716310471498"><td class="cellrowborder" valign="top" width="23.82%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p7163174724914"><a name="zh-cn_topic_0000002444727517_p7163174724914"></a><a name="zh-cn_topic_0000002444727517_p7163174724914"></a>key</p>
</td>
<td class="cellrowborder" valign="top" width="76.18%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p516384784917"><a name="zh-cn_topic_0000002444727517_p516384784917"></a><a name="zh-cn_topic_0000002444727517_p516384784917"></a>要读取的NV项的key ID，用于索引。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444727517_row9163184712499"><td class="cellrowborder" valign="top" width="23.82%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p1093420275819"><a name="zh-cn_topic_0000002444727517_p1093420275819"></a><a name="zh-cn_topic_0000002444727517_p1093420275819"></a>kvalue_max_length</p>
</td>
<td class="cellrowborder" valign="top" width="76.18%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p1579439155819"><a name="zh-cn_topic_0000002444727517_p1579439155819"></a><a name="zh-cn_topic_0000002444727517_p1579439155819"></a>允许存储数据的最大长度，单位：Byte。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444727517_row11163204714915"><td class="cellrowborder" valign="top" width="23.82%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p21631947194917"><a name="zh-cn_topic_0000002444727517_p21631947194917"></a><a name="zh-cn_topic_0000002444727517_p21631947194917"></a>*kvalue_length</p>
</td>
<td class="cellrowborder" valign="top" width="76.18%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p167953925815"><a name="zh-cn_topic_0000002444727517_p167953925815"></a><a name="zh-cn_topic_0000002444727517_p167953925815"></a>实际读取到的数据长度。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444727517_row8993789585"><td class="cellrowborder" valign="top" width="23.82%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p5993168125819"><a name="zh-cn_topic_0000002444727517_p5993168125819"></a><a name="zh-cn_topic_0000002444727517_p5993168125819"></a>*kvalue</p>
</td>
<td class="cellrowborder" valign="top" width="76.18%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p779639155813"><a name="zh-cn_topic_0000002444727517_p779639155813"></a><a name="zh-cn_topic_0000002444727517_p779639155813"></a>指向保存读取数据的buffer的指针。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002444727517_row137151455813"><td class="cellrowborder" valign="top" width="23.82%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p17371111455810"><a name="zh-cn_topic_0000002444727517_p17371111455810"></a><a name="zh-cn_topic_0000002444727517_p17371111455810"></a>*attr</p>
</td>
<td class="cellrowborder" valign="top" width="76.18%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p337171485810"><a name="zh-cn_topic_0000002444727517_p337171485810"></a><a name="zh-cn_topic_0000002444727517_p337171485810"></a>获取到的NV项的属性。</p>
</td>
</tr>
</tbody>
</table>

【函数接口】

```
errcode_t uapi_nv_get_store_status(nv_store_status_t *status)
```

【函数功能】获取NV存储空间使用情况。

【参数说明】

<a name="zh-cn_topic_0000002444727517_table11171155012496"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002444727517_row81717509491"><th class="cellrowborder" valign="top" width="23.97%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002444727517_p1717116503494"><a name="zh-cn_topic_0000002444727517_p1717116503494"></a><a name="zh-cn_topic_0000002444727517_p1717116503494"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="76.03%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002444727517_p1517115509496"><a name="zh-cn_topic_0000002444727517_p1517115509496"></a><a name="zh-cn_topic_0000002444727517_p1517115509496"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002444727517_row01711950114911"><td class="cellrowborder" valign="top" width="23.97%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p1543219281924"><a name="zh-cn_topic_0000002444727517_p1543219281924"></a><a name="zh-cn_topic_0000002444727517_p1543219281924"></a>*status</p>
</td>
<td class="cellrowborder" valign="top" width="76.03%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p1417135074915"><a name="zh-cn_topic_0000002444727517_p1417135074915"></a><a name="zh-cn_topic_0000002444727517_p1417135074915"></a>指向保存NV状态数据的指针。</p>
</td>
</tr>
</tbody>
</table>

【函数接口】

```
errcode_t errcode_t uapi_nv_flush(void)
```

【函数功能】NV支持异步存储时，刷新缓存NV数据到flash。

【参数说明】

<a name="zh-cn_topic_0000002444727517_table4570730165016"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002444727517_row15570163075010"><th class="cellrowborder" valign="top" width="23.97%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002444727517_p7695172995120"><a name="zh-cn_topic_0000002444727517_p7695172995120"></a><a name="zh-cn_topic_0000002444727517_p7695172995120"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="76.03%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002444727517_p657014306502"><a name="zh-cn_topic_0000002444727517_p657014306502"></a><a name="zh-cn_topic_0000002444727517_p657014306502"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002444727517_row157033014506"><td class="cellrowborder" valign="top" width="23.97%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002444727517_p0802174145320"><a name="zh-cn_topic_0000002444727517_p0802174145320"></a><a name="zh-cn_topic_0000002444727517_p0802174145320"></a>void</p>
</td>
<td class="cellrowborder" valign="top" width="76.03%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002444727517_p52388236545"><a name="zh-cn_topic_0000002444727517_p52388236545"></a><a name="zh-cn_topic_0000002444727517_p52388236545"></a>该函数入参为空，下电前调用。</p>
</td>
</tr>
</tbody>
</table>

>![](public_sys-resources/icon-notice.gif) **须知：** 
>uapi\_nv\_flush：该接口为NV支持异步存储时新增接口，在关机、下电、重启等流程要主动增加该接口的调用，保证数据不丢失。

## 开发指引<a name="ZH-CN_TOPIC_0000002499501029"></a>

使用NV接口需要引用NV接口头文件，路径：include/middleware/utils/nv.h

下述步骤为NV读写接口的使用指引。API调用处以加粗突出。

1.  写入默认Normal类型NV。

    ```
    uint8_t *test_nv_value; /* 要写入的NV value保存在test_nv_value中 */
    uint32_t test_len = 15; /* 长度为test_len ，例中为15*/
    
    uint16_t key = TEST_KEY; /* TEST_KEY 为该Key的ID*/
    uint16_t key_len= test_len;
    uint8_t *write_value = uapi_malloc(key_len);
    (void)memcpy_s(write_value, key_len, test_nv_value, key_len);
    errcode_t nv_ret_value = uapi_nv_write(key, write_value, key_len);
    if (nv_ret_value != ERRCODE_SUCC) {
        /* ERROR PROCESS */
        uapi_free(wrt_value);
        return ERRCODE_FAIL;
    }
    /* APP PROCESS */
    uapi_free(wrt_value);
    return ERRCODE_SUCC;
    ```

2.  写入带属性NV（配置加密属性，其他略）。

    ```
    uint8_t *test_nv_value; /* 要写入的NV value保存在test_nv_value中 */
    uint32_t test_len = 15; /* 长度为test_len，例中为15 */
    
    uint16_t key = TEST_KEY;
    uint16_t key_len= test_len;
    
    nv_key_attr_t attr = {0};
    attr.permanent = false;
    attr.encrypted = true; /* 加密属性设为true */
    attr.non_upgrade = false
    uint8_t *write_value= uapi_malloc(key_len);
    (void)memcpy_s(write_value, key_len, test_nv_value, key_len);
    errcode_t nv_ret_value = uapi_nv_write_with_attr(key, write_value, key_len, &attr, NULL);
    if (nv_ret_value != ERRCODE_SUCC) {
        /* ERROR PROCESS */
        uapi_free(write_value);
        return ERRCODE_FAIL;
    }
    /* APP PROCESS */
    uapi_free(write_value);
    return ERRCODE_SUCC;
    ```

3.  读取NV。

    ```
    uint16_t key = TEST_KEY;
    uint16_t key_len= test_len;
    uint16_t real_len= 0;
    uint8_t *read_value = uapi_malloc(key_len);
    if (uapi_nv_read(key, key_len, &real_len, read_value) != ERRCODE_SUCC) {
        /* ERROR PROCESS */
        uapi_free(read_value);
        return ERRCODE_FAIL;
    }
    /* APP PROCESS */
    uapi_free(read_value);
    return ERRCODE_SUCC;
    ```

4.  读取NV及属性。

    ```
    uint16_t key = TEST_KEY;
    uint16_t key_len = test_len;
    uint16_t real_len = 0;
    uint8_t *read_value = uapi_malloc(key_len);
    nv_key_attr_t attr = {false, false, false, 0};
    ext_errno nv_ret = uapi_nv_read_with_attr(key, key_len, &real_len, read_value, &attr);
    if (nv_ret != ERRCODE_SUCC ) {
        /* ERROR PROCESS */
        uapi_free(read_value);
        return ERRCODE_FAIL;
    } 
    /* APP PROCESS */
    uapi_free(read_value);
    return ERRCODE_SUCC;
    ```

5.  查询NV空间状态。

    ```
    nv_store_status status;
    if (uapi_nv_get_store_status(&status) == ERRCODE_SUCC) {
        /* APP PROCESS */
        printf("Total:      %d Bytes\r\n", status.total_space);
        printf("used:       %d Bytes\r\n", status.used_space);
        printf("reclaimable:%d Bytes\r\n", status.reclaimable_space);
        printf("corrupted:  %d Bytes\r\n", status.corrupted_space);
        printf("max_key:    %d Bytes\r\n", status.max_key_space);
    }
    ```

>![](public_sys-resources/icon-note.gif) **说明：** 
>开发指引只是API接口的测试用例，为用户提供简单的sample参考，sample中省略了宏、部分变量、回调函数的定义过程和业务处理过程。

## 注意事项<a name="ZH-CN_TOPIC_0000002466382076"></a>

-   uapi\_nv\_write：默认不对所存储的key添加额外属性（是否永久存储、是否加密存储等）。
-   HiDiTingV100支持加密NV。
-   uapi\_nv\_write\_with\_attr：可同时配置key属性和注册回调函数。在HiDiTingV100中回调函数可以忽略，传NULL即可。
-   NV属性结构体和NV空间状态结构体说明详见“nv.h”文件。
-   Flash擦写次数存在阈值（一般为十万次左右）；在适配NV流程中，忌频繁擦写Flash，以免器件过快损耗。

## 常见问题<a name="ZH-CN_TOPIC_0000002466541688"></a>

问题描述：NV数据读取失败。

解决方法：请排查uapi\_nv\_flush接口是否未在下电前调用，导致数据丢失。

