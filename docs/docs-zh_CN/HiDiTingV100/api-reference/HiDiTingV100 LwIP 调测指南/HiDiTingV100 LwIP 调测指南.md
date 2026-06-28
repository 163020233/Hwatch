# 前言<a name="ZH-CN_TOPIC_0000002522510496"></a>

**概述<a name="zh-cn_topic_0000002548616071_section4537382116410"></a>**

本文档详细的描述了LwIP组件的适配调试，组件所用开源版本目前为v2.1.3，其中支持TCP、UDP、DHCP等协议。

**读者对象<a name="zh-cn_topic_0000002548616071_section4378592816410"></a>**

本文档主要适用于以下工程师：

-   软件开发工程师

-   技术支持工程师
-   软件测试工程师

**符号约定<a name="zh-cn_topic_0000002548616071_section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="zh-cn_topic_0000002548616071_table2622507016410"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002548616071_row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="zh-cn_topic_0000002548616071_p6450074116410"><a name="zh-cn_topic_0000002548616071_p6450074116410"></a><a name="zh-cn_topic_0000002548616071_p6450074116410"></a><strong id="zh-cn_topic_0000002548616071_b2136615816410"><a name="zh-cn_topic_0000002548616071_b2136615816410"></a><a name="zh-cn_topic_0000002548616071_b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="zh-cn_topic_0000002548616071_p5435366816410"><a name="zh-cn_topic_0000002548616071_p5435366816410"></a><a name="zh-cn_topic_0000002548616071_p5435366816410"></a><strong id="zh-cn_topic_0000002548616071_b5941558116410"><a name="zh-cn_topic_0000002548616071_b5941558116410"></a><a name="zh-cn_topic_0000002548616071_b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002548616071_row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002548616071_p3734547016410"><a name="zh-cn_topic_0000002548616071_p3734547016410"></a><a name="zh-cn_topic_0000002548616071_p3734547016410"></a><a name="zh-cn_topic_0000002548616071_image2670064316410"></a><a name="zh-cn_topic_0000002548616071_image2670064316410"></a><span><img class="" id="zh-cn_topic_0000002548616071_image2670064316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002553322167.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002548616071_p1757432116410"><a name="zh-cn_topic_0000002548616071_p1757432116410"></a><a name="zh-cn_topic_0000002548616071_p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002548616071_row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002548616071_p1432579516410"><a name="zh-cn_topic_0000002548616071_p1432579516410"></a><a name="zh-cn_topic_0000002548616071_p1432579516410"></a><a name="zh-cn_topic_0000002548616071_image4895582316410"></a><a name="zh-cn_topic_0000002548616071_image4895582316410"></a><span><img class="" id="zh-cn_topic_0000002548616071_image4895582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002522362218.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002548616071_p959197916410"><a name="zh-cn_topic_0000002548616071_p959197916410"></a><a name="zh-cn_topic_0000002548616071_p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002548616071_row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002548616071_p1232579516410"><a name="zh-cn_topic_0000002548616071_p1232579516410"></a><a name="zh-cn_topic_0000002548616071_p1232579516410"></a><a name="zh-cn_topic_0000002548616071_image1235582316410"></a><a name="zh-cn_topic_0000002548616071_image1235582316410"></a><span><img class="" id="zh-cn_topic_0000002548616071_image1235582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002553482123.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002548616071_p123197916410"><a name="zh-cn_topic_0000002548616071_p123197916410"></a><a name="zh-cn_topic_0000002548616071_p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002548616071_row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002548616071_p2204984716410"><a name="zh-cn_topic_0000002548616071_p2204984716410"></a><a name="zh-cn_topic_0000002548616071_p2204984716410"></a><a name="zh-cn_topic_0000002548616071_image4504446716410"></a><a name="zh-cn_topic_0000002548616071_image4504446716410"></a><span><img class="" id="zh-cn_topic_0000002548616071_image4504446716410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002522522216.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002548616071_p4388861916410"><a name="zh-cn_topic_0000002548616071_p4388861916410"></a><a name="zh-cn_topic_0000002548616071_p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="zh-cn_topic_0000002548616071_p1238861916410"><a name="zh-cn_topic_0000002548616071_p1238861916410"></a><a name="zh-cn_topic_0000002548616071_p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002548616071_row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="zh-cn_topic_0000002548616071_p5555360116410"><a name="zh-cn_topic_0000002548616071_p5555360116410"></a><a name="zh-cn_topic_0000002548616071_p5555360116410"></a><a name="zh-cn_topic_0000002548616071_image799324016410"></a><a name="zh-cn_topic_0000002548616071_image799324016410"></a><span><img class="" id="zh-cn_topic_0000002548616071_image799324016410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000002553322181.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="zh-cn_topic_0000002548616071_p4612588116410"><a name="zh-cn_topic_0000002548616071_p4612588116410"></a><a name="zh-cn_topic_0000002548616071_p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="zh-cn_topic_0000002548616071_p1232588116410"><a name="zh-cn_topic_0000002548616071_p1232588116410"></a><a name="zh-cn_topic_0000002548616071_p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

# LwIP的适配<a name="ZH-CN_TOPIC_0000002522510504"></a>

LwIP的适配主要是南北向接口的适配，南向接口主要是驱动的注册；北向接口LwIP提供了三种编程接口，分别为RAW/Callback API、NETCONN API、SOCKET API，它们的易用性从左到右依次提高，而执行效率从左到右依次降低，本文主要基于SOCKET API进行适配，流程如[图1](#zh-cn_topic_0000002516976272_fig720564116222)所示。

**图 1**  SOCKET API整体流程<a name="zh-cn_topic_0000002516976272_fig720564116222"></a>  
![](figures/SOCKET-API整体流程.png "SOCKET-API整体流程")

# 线程创建<a name="ZH-CN_TOPIC_0000002553310461"></a>

主线程为app\_main线程，LwIP另起一个子线程如[图1](#zh-cn_topic_0000002517136188_fig510264010175)所示中的lwIP\_main。

**图 1**  LwIP线程<a name="zh-cn_topic_0000002517136188_fig510264010175"></a>  
![](figures/LwIP线程.png "LwIP线程")

在LwIP线程中调用tcpip\_init初始化网络协议栈，并在其中创建一个tcpip\_mbox邮箱，邮箱的大小是TCPIP\_MBOX\_SIZE，用于接收从底层或者上层传递过来的消息，还会创建tcpip\_thread线程并调用协议栈初始化操作（创建邮箱、请求互斥锁等），如[图2](#zh-cn_topic_0000002517136188_fig19253122645710)所示。

**图 2**  tcpip\_thread线程<a name="zh-cn_topic_0000002517136188_fig19253122645710"></a>  
![](figures/tcpip_thread线程.png "tcpip_thread线程")

# 南向接口适配<a name="ZH-CN_TOPIC_0000002522350514"></a>

1.  构建网卡驱动注册环境：构建IP地址、网关和网络掩码。

    **图 1**  IP地址、网关和网络掩码<a name="zh-cn_topic_0000002517136186_fig1360514715196"></a>  
    ![](figures/IP地址-网关和网络掩码.png "IP地址-网关和网络掩码")

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >此外还需配置MAC地址。

2.  初始化LwIP协议栈，调用netif\_add接口（LwIP社区开源框架）将其加入netif链表（挂载到链表上是使用的前提），根据IP协议IPv4/IPv6解析地址，初始化netif网卡结构，配置网卡各项基础参数，更改网络接口的IP地址配置（包括网络掩码和默认网关），在0～254范围内分配一个唯一的netif号（接口索引为num+1，假如分配的号为254，那么接口索引就是0），最后在接口上启动IGMP处理。

1.  调用netif\_set\_default（LwIP社区开源框架）切换默认路由。

    将网络接口设置为默认网络接口，输出所有找不到具体路由的报文。

2.  询问链路正常状态。
3.  调用netif\_set\_up函数（LwIP社区开源框架）开始运行。

    设置回调参数、发布报文、协议分流等。

总结就是开发板上eth接收完数据后产生一个中断，然后释放一个信号量通知网卡接收线程去处理这些接收的数据，然后将这些数据封装成消息，投递到tcpip\_mbox邮箱中，LwIP内核线程得到此消息，就对消息进行解析，根据消息中数据包类型进行处理。

>![](public_sys-resources/icon-note.gif) **说明：** 
>在使用4G CAT1访问公网时，LwIP使用默认的208.67.222.222 （OpenDNS ）DNS服务IP地址。

# 北向接口适配<a name="ZH-CN_TOPIC_0000002522350508"></a>

创建client和server作为系统的起始和结果终端收发报文。

**图 1**  客户端和服务端线程<a name="zh-cn_topic_0000002548736061_fig2251112913585"></a>  
![](figures/客户端和服务端线程.png "客户端和服务端线程")

>![](public_sys-resources/icon-note.gif) **说明：** 
>线程空间要给充足，最少为4KB。

以下以Socket API为例。

**客户端正常进行数据发送（以UDP为例）<a name="zh-cn_topic_0000002548736061_section1371014575253"></a>**

创建UDP套接字，也就是调用LwIP socket函数（开源框架，本质上其实就是对netconn的封装）向内核申请一个套接字，如[图2](#zh-cn_topic_0000002548736061_fig73512327195)所示。

**图 2**  申请套接字<a name="zh-cn_topic_0000002548736061_fig73512327195"></a>  
![](figures/申请套接字.png "申请套接字")

设置服务器地址信息如[图3](#zh-cn_topic_0000002548736061_fig10271541141916)所示，调用lwIP\_sendto（LwIP社区开源框架，如[图4](#zh-cn_topic_0000002548736061_fig20900448131916)）接口发送数据，进入LwIP协议栈。

>![](public_sys-resources/icon-note.gif) **说明：** 
>lwIP\_sendto主要是用于UDP协议传输数据中，它向另一端的UDP主机发送一个UDP报文，本质上是对netconn\_send\(\)函数的封装。

**图 3**  设置服务器地址信息<a name="zh-cn_topic_0000002548736061_fig10271541141916"></a>  
![](figures/设置服务器地址信息.png "设置服务器地址信息")

**图 4**  发送数据<a name="zh-cn_topic_0000002548736061_fig20900448131916"></a>  
![](figures/发送数据.png "发送数据")

**服务端接收数据<a name="zh-cn_topic_0000002548736061_section6518191802616"></a>**

创建UDP套接字，设置服务器地址信息，绑定套接字（用于服务器端绑定套接字与网卡信息，如[图5](#zh-cn_topic_0000002548736061_fig209271808201)所示），循环调用recvfrom（开源框架，如[图6](#zh-cn_topic_0000002548736061_fig126971314172015)所示）接口接收数据。

**图 5**  绑定socket<a name="zh-cn_topic_0000002548736061_fig209271808201"></a>  
![](figures/绑定socket.png "绑定socket")

**图 6**  接收数据<a name="zh-cn_topic_0000002548736061_fig126971314172015"></a>  
![](figures/接收数据.png "接收数据")

# 目录说明<a name="ZH-CN_TOPIC_0000002553310453"></a>

**图 1**  适配层目录<a name="zh-cn_topic_0000002548736063_fig544223913213"></a>  
![](figures/适配层目录.png "适配层目录")

-   sys\_arch.c为邮箱、线程创建、互斥锁、信号量、内核时间获取等功能接口实现。

    sys\_arch.h展示其接口。

-   fs.h文件系统适配文件。
-   ethernetif.c提供了netif访问各种不同的网卡，每个网卡有不同的实现方式，初始化硬件和数据包传输等操作。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >ethernetif.c文件中的函数为与硬件打交道的底层函数，当有数据需要通过网卡接收或者发送数据的时候就会被调用，经过LwIP协议栈内部进行处理后，从应用层就能得到数据或者可以发送数据。

    以下就是具体实现：

    -   low\_level\_init\(\)为网卡初始化函数，它主要完成网卡的复位及参数初始化，根据实际的网卡属性配置netif中与网卡相关的字段，例如网卡的MAC地址、长度，最大发送单元等。
    -   low\_level\_output\(\)函数为网卡的发送函数，它主要将内核的数据包发送出去，数据包采用pbuf数据结构进行描述，该数据结构是一个比较复杂的数据结构。
    -   low\_level\_input\(\)函数为网卡的数据接收函数，该函数会接收一个数据包，为了内核易于对数据包的管理，该函数必须将接收的数据封装成pbuf的形式。

    ethernetif.h展示网卡实现接口。

-   lwipopts\_default.h为默认配置信息，一切外部未定义或重复定义宏或变量等在此定义或进行覆盖，比如与社区源码（如lwIP\_v2.1.3）相关宏的配置选项取舍和设定，常用见[表1](#zh-cn_topic_0000002548736063_table10562637564)。

    **表 1**  常用配置宏

    <a name="zh-cn_topic_0000002548736063_table10562637564"></a>
    <table><thead align="left"><tr id="zh-cn_topic_0000002548736063_row858610371760"><th class="cellrowborder" valign="top" width="39.160000000000004%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000002548736063_p1258683719615"><a name="zh-cn_topic_0000002548736063_p1258683719615"></a><a name="zh-cn_topic_0000002548736063_p1258683719615"></a>配置项</p>
    </th>
    <th class="cellrowborder" valign="top" width="60.84%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000002548736063_p175861237365"><a name="zh-cn_topic_0000002548736063_p175861237365"></a><a name="zh-cn_topic_0000002548736063_p175861237365"></a>说明</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="zh-cn_topic_0000002548736063_row10586103718611"><td class="cellrowborder" valign="top" width="39.160000000000004%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002548736063_p1158616371569"><a name="zh-cn_topic_0000002548736063_p1158616371569"></a><a name="zh-cn_topic_0000002548736063_p1158616371569"></a>NO_SYS</p>
    </td>
    <td class="cellrowborder" valign="top" width="60.84%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002548736063_p458613719617"><a name="zh-cn_topic_0000002548736063_p458613719617"></a><a name="zh-cn_topic_0000002548736063_p458613719617"></a>有无操作系统，有则置为0。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002548736063_row125867373619"><td class="cellrowborder" valign="top" width="39.160000000000004%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002548736063_p358613712610"><a name="zh-cn_topic_0000002548736063_p358613712610"></a><a name="zh-cn_topic_0000002548736063_p358613712610"></a>LWIP_SOCKET</p>
    </td>
    <td class="cellrowborder" valign="top" width="60.84%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002548736063_p1758783714615"><a name="zh-cn_topic_0000002548736063_p1758783714615"></a><a name="zh-cn_topic_0000002548736063_p1758783714615"></a>支持socket接口（需有操作系统）。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002548736063_row45871637565"><td class="cellrowborder" valign="top" width="39.160000000000004%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002548736063_p258718374617"><a name="zh-cn_topic_0000002548736063_p258718374617"></a><a name="zh-cn_topic_0000002548736063_p258718374617"></a>LWIP_CONN</p>
    </td>
    <td class="cellrowborder" valign="top" width="60.84%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002548736063_p35871337667"><a name="zh-cn_topic_0000002548736063_p35871337667"></a><a name="zh-cn_topic_0000002548736063_p35871337667"></a>支持NETCONN接口，（需有操作系统）。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002548736063_row35871037163"><td class="cellrowborder" valign="top" width="39.160000000000004%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002548736063_p155874376617"><a name="zh-cn_topic_0000002548736063_p155874376617"></a><a name="zh-cn_topic_0000002548736063_p155874376617"></a>LWIP_NETIF_API</p>
    </td>
    <td class="cellrowborder" valign="top" width="60.84%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002548736063_p115876371264"><a name="zh-cn_topic_0000002548736063_p115876371264"></a><a name="zh-cn_topic_0000002548736063_p115876371264"></a>支持netif API。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002548736063_row5587737368"><td class="cellrowborder" valign="top" width="39.160000000000004%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002548736063_p858710371663"><a name="zh-cn_topic_0000002548736063_p858710371663"></a><a name="zh-cn_topic_0000002548736063_p858710371663"></a>TCPIP_MBOX_SIZE</p>
    </td>
    <td class="cellrowborder" valign="top" width="60.84%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002548736063_p15876371468"><a name="zh-cn_topic_0000002548736063_p15876371468"></a><a name="zh-cn_topic_0000002548736063_p15876371468"></a>定义邮箱大小。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002548736063_row25871537767"><td class="cellrowborder" valign="top" width="39.160000000000004%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002548736063_p1558718371366"><a name="zh-cn_topic_0000002548736063_p1558718371366"></a><a name="zh-cn_topic_0000002548736063_p1558718371366"></a>LWIP_NUM_SOCKETS_MAX</p>
    </td>
    <td class="cellrowborder" valign="top" width="60.84%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002548736063_p05873371065"><a name="zh-cn_topic_0000002548736063_p05873371065"></a><a name="zh-cn_topic_0000002548736063_p05873371065"></a>最大支持套接字数。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0000002548736063_row1458713374613"><td class="cellrowborder" valign="top" width="39.160000000000004%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000002548736063_p1158719371362"><a name="zh-cn_topic_0000002548736063_p1158719371362"></a><a name="zh-cn_topic_0000002548736063_p1158719371362"></a>MEM_LIBC_MALLOC</p>
    </td>
    <td class="cellrowborder" valign="top" width="60.84%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000002548736063_p11587437368"><a name="zh-cn_topic_0000002548736063_p11587437368"></a><a name="zh-cn_topic_0000002548736063_p11587437368"></a>分配内存方式，置为1，则使用此库函数（malloc、free）等。</p>
    </td>
    </tr>
    </tbody>
    </table>

-   与lwip\_adapter同级的lwIP\_v2.1.3则为开源中心框架。

-   test目录内容则为示例程序，仅作参考。

# 必要操作<a name="ZH-CN_TOPIC_0000002522350512"></a>

根据目标组件，适配LiteOS系统API，不同系统在线程、锁、邮箱、互斥体等方面实现不同，确认是否存在相应的目标组件库。例如[图1](#zh-cn_topic_0000002517136190_fig12566251135815)所示适配不同系统。

**图 1**  组件库选项<a name="zh-cn_topic_0000002517136190_fig12566251135815"></a>  
![](figures/组件库选项.png "组件库选项")

# 调试及优化<a name="ZH-CN_TOPIC_0000002553310457"></a>

LwIP协议栈相关功能默认为打开状态。

-   tcpip\_init函数初始化LwIP各模块（包括定时器模块），开启tcpip\_thread线程（核心业务为不断循环获取并处理邮箱消息）。

-   lwip\_cat1\_open/lwip\_cat1\_set\_ip函数为CAT1南向适配层初始化，可开启cat1netif等相关功能【示例，仅供参考】。

>![](public_sys-resources/icon-notice.gif) **须知：** 
>4G CAT1默认使用了公网DNS服务器：119.29.29.29 （DNSPod）、223.5.5.5 （AliDNS）、1.2.4.8 （CNNIC SDNS）。
>客户也可根据实际情况选择私有的DNS服务器。

**图 1**  lwip\_main任务<a name="zh-cn_topic_0000002516976274_fig56822661918"></a>  
![](figures/lwip_main任务.png "lwip_main任务")

在lwipopts.h文件中配置LwIP的参数，具体见[图2](#zh-cn_topic_0000002516976274_fig8834314125911)。LwIP管理的内存建议稍多分配，而当发送数据是存储在ROM或者静态存储区时，还要将MEMP\_NUM\_PBUF宏定义数据做进一步提升，发送缓冲区大小和发送缓冲区队列长度决定了发送速度的大小，根据不同需求进行配置，并且需要不断调试；而对于接收数据的配置，应该配置TCP缓冲队列中的报文段数量与TCP接收窗口大小，特别是接收窗口的大小，会直接影响数据的接收速度。

**图 2**  配置项<a name="zh-cn_topic_0000002516976274_fig8834314125911"></a>  
![](figures/配置项.png "配置项")

# LwIP日志开启说明<a name="ZH-CN_TOPIC_0000002522510500"></a>

针对HiDiTing使用LiteOS操作系统，协议栈使用LwIP。开启日志调试打印方法如下：

日志总开关路径：ohos/third\_party/lwip/lwip\_adapter/liteos/src/include/lwip/lwipopts\_default.h。

1.  在上述文件路径中定义“LWIP\_DEBUG”宏，表示开启LwIP日志调试功能。
2.  “LWIP\_DBG\_TYPES\_ON”宏定义修改为“LWIP\_DBG\_ON”，表示开启debug日志。

    **图 1**  LwIP日志宏LWIP\_DEBUG<a name="zh-cn_topic_0000002548616073_fig1546384694919"></a>  
    ![](figures/LwIP日志宏LWIP_DEBUG.png "LwIP日志宏LWIP_DEBUG")

3.  开启相关模块日志功能。路径：ohos/third\_party/lwip/lwip\_v2.1.3/src/include/lwip/opt.h。

    例如开启TCP模块日志，需要修改如[图2](#zh-cn_topic_0000002548616073_fig43770965418)所示，将相关宏开启后重洗编译版本，运行相关程序，则会打印TCP相关日志。

    **图 2**  TCP模块相关宏定义开启<a name="zh-cn_topic_0000002548616073_fig43770965418"></a>  
    ![](figures/TCP模块相关宏定义开启.png "TCP模块相关宏定义开启")

# 缩略语<a name="ZH-CN_TOPIC_0000002553470419"></a>

<a name="zh-cn_topic_0000002517207984_table1572112082619"></a>
<table><tbody><tr id="zh-cn_topic_0000002517207984_row14573720182619"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p857372052619"><a name="zh-cn_topic_0000002517207984_p857372052619"></a><a name="zh-cn_topic_0000002517207984_p857372052619"></a><strong id="zh-cn_topic_0000002517207984_b15659232183512"><a name="zh-cn_topic_0000002517207984_b15659232183512"></a><a name="zh-cn_topic_0000002517207984_b15659232183512"></a>A</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row6573020182620"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p17573122018264"><a name="zh-cn_topic_0000002517207984_p17573122018264"></a><a name="zh-cn_topic_0000002517207984_p17573122018264"></a><strong id="zh-cn_topic_0000002517207984_b13660193218355"><a name="zh-cn_topic_0000002517207984_b13660193218355"></a><a name="zh-cn_topic_0000002517207984_b13660193218355"></a>API</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%"><p id="zh-cn_topic_0000002517207984_p15968259143313"><a name="zh-cn_topic_0000002517207984_p15968259143313"></a><a name="zh-cn_topic_0000002517207984_p15968259143313"></a>Application Programming Interface</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002517207984_p9968195923310"><a name="zh-cn_topic_0000002517207984_p9968195923310"></a><a name="zh-cn_topic_0000002517207984_p9968195923310"></a>应用编程接口</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row557313206265"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row1573620182616"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p3625848153110"><a name="zh-cn_topic_0000002517207984_p3625848153110"></a><a name="zh-cn_topic_0000002517207984_p3625848153110"></a><strong id="zh-cn_topic_0000002517207984_b26623320359"><a name="zh-cn_topic_0000002517207984_b26623320359"></a><a name="zh-cn_topic_0000002517207984_b26623320359"></a>C</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row15731620192618"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p10623348163115"><a name="zh-cn_topic_0000002517207984_p10623348163115"></a><a name="zh-cn_topic_0000002517207984_p10623348163115"></a><strong id="zh-cn_topic_0000002517207984_b166637324351"><a name="zh-cn_topic_0000002517207984_b166637324351"></a><a name="zh-cn_topic_0000002517207984_b166637324351"></a>CAT1</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%"><p id="zh-cn_topic_0000002517207984_p115941210814"><a name="zh-cn_topic_0000002517207984_p115941210814"></a><a name="zh-cn_topic_0000002517207984_p115941210814"></a>Category 1</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002517207984_p5594421689"><a name="zh-cn_topic_0000002517207984_p5594421689"></a><a name="zh-cn_topic_0000002517207984_p5594421689"></a>4G物联网终端</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row2574102002617"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row8574182020261"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p1457382082610"><a name="zh-cn_topic_0000002517207984_p1457382082610"></a><a name="zh-cn_topic_0000002517207984_p1457382082610"></a><strong id="zh-cn_topic_0000002517207984_b566419329352"><a name="zh-cn_topic_0000002517207984_b566419329352"></a><a name="zh-cn_topic_0000002517207984_b566419329352"></a>D</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row1957482011261"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p185731020192613"><a name="zh-cn_topic_0000002517207984_p185731020192613"></a><a name="zh-cn_topic_0000002517207984_p185731020192613"></a><strong id="zh-cn_topic_0000002517207984_b2664183293513"><a name="zh-cn_topic_0000002517207984_b2664183293513"></a><a name="zh-cn_topic_0000002517207984_b2664183293513"></a>DHCP</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%"><p id="zh-cn_topic_0000002517207984_p75738203268"><a name="zh-cn_topic_0000002517207984_p75738203268"></a><a name="zh-cn_topic_0000002517207984_p75738203268"></a>Dynamic Host Configuration Protocol</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002517207984_p1757413202261"><a name="zh-cn_topic_0000002517207984_p1757413202261"></a><a name="zh-cn_topic_0000002517207984_p1757413202261"></a>动态主机配置协议</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row125741320202620"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p25741820152610"><a name="zh-cn_topic_0000002517207984_p25741820152610"></a><a name="zh-cn_topic_0000002517207984_p25741820152610"></a><strong id="zh-cn_topic_0000002517207984_b2665153243510"><a name="zh-cn_topic_0000002517207984_b2665153243510"></a><a name="zh-cn_topic_0000002517207984_b2665153243510"></a>DNS</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%"><p id="zh-cn_topic_0000002517207984_p125742209264"><a name="zh-cn_topic_0000002517207984_p125742209264"></a><a name="zh-cn_topic_0000002517207984_p125742209264"></a>Domain Name Server</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002517207984_p4574112022610"><a name="zh-cn_topic_0000002517207984_p4574112022610"></a><a name="zh-cn_topic_0000002517207984_p4574112022610"></a>域名服务器</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row1557412204267"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row4574122012614"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p657416205266"><a name="zh-cn_topic_0000002517207984_p657416205266"></a><a name="zh-cn_topic_0000002517207984_p657416205266"></a><strong id="zh-cn_topic_0000002517207984_b116668328353"><a name="zh-cn_topic_0000002517207984_b116668328353"></a><a name="zh-cn_topic_0000002517207984_b116668328353"></a>I</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row257514200268"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p5575420142613"><a name="zh-cn_topic_0000002517207984_p5575420142613"></a><a name="zh-cn_topic_0000002517207984_p5575420142613"></a><strong id="zh-cn_topic_0000002517207984_b156671632153514"><a name="zh-cn_topic_0000002517207984_b156671632153514"></a><a name="zh-cn_topic_0000002517207984_b156671632153514"></a>IGMP</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%"><p id="zh-cn_topic_0000002517207984_p7575142020268"><a name="zh-cn_topic_0000002517207984_p7575142020268"></a><a name="zh-cn_topic_0000002517207984_p7575142020268"></a>Internet Group Management Protocol</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002517207984_p157572015268"><a name="zh-cn_topic_0000002517207984_p157572015268"></a><a name="zh-cn_topic_0000002517207984_p157572015268"></a>互联网组管理协议</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row4575102017267"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row257532052612"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p257510203263"><a name="zh-cn_topic_0000002517207984_p257510203263"></a><a name="zh-cn_topic_0000002517207984_p257510203263"></a><strong id="zh-cn_topic_0000002517207984_b7668532163519"><a name="zh-cn_topic_0000002517207984_b7668532163519"></a><a name="zh-cn_topic_0000002517207984_b7668532163519"></a>M</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row1657582052611"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p8575162012264"><a name="zh-cn_topic_0000002517207984_p8575162012264"></a><a name="zh-cn_topic_0000002517207984_p8575162012264"></a><strong id="zh-cn_topic_0000002517207984_b196687323359"><a name="zh-cn_topic_0000002517207984_b196687323359"></a><a name="zh-cn_topic_0000002517207984_b196687323359"></a>MAC</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%"><p id="zh-cn_topic_0000002517207984_p1657510209268"><a name="zh-cn_topic_0000002517207984_p1657510209268"></a><a name="zh-cn_topic_0000002517207984_p1657510209268"></a>Media Access Control Address</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002517207984_p10575142013262"><a name="zh-cn_topic_0000002517207984_p10575142013262"></a><a name="zh-cn_topic_0000002517207984_p10575142013262"></a>媒体接入控制</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row85751220142619"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row7575142018263"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p557662016267"><a name="zh-cn_topic_0000002517207984_p557662016267"></a><a name="zh-cn_topic_0000002517207984_p557662016267"></a><strong id="zh-cn_topic_0000002517207984_b1566919321356"><a name="zh-cn_topic_0000002517207984_b1566919321356"></a><a name="zh-cn_topic_0000002517207984_b1566919321356"></a>T</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row857615209267"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p18576132017260"><a name="zh-cn_topic_0000002517207984_p18576132017260"></a><a name="zh-cn_topic_0000002517207984_p18576132017260"></a><strong id="zh-cn_topic_0000002517207984_b66702321356"><a name="zh-cn_topic_0000002517207984_b66702321356"></a><a name="zh-cn_topic_0000002517207984_b66702321356"></a>TCP</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%"><p id="zh-cn_topic_0000002517207984_p35761420192615"><a name="zh-cn_topic_0000002517207984_p35761420192615"></a><a name="zh-cn_topic_0000002517207984_p35761420192615"></a>Transmission Control Protocol</p>
</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002517207984_p3576182016263"><a name="zh-cn_topic_0000002517207984_p3576182016263"></a><a name="zh-cn_topic_0000002517207984_p3576182016263"></a>传输控制协议</p>
</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row35761220172618"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%">&nbsp;&nbsp;</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row5576120152613"><td class="nocellnorowborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p125761020122613"><a name="zh-cn_topic_0000002517207984_p125761020122613"></a><a name="zh-cn_topic_0000002517207984_p125761020122613"></a><strong id="zh-cn_topic_0000002517207984_b1967183213352"><a name="zh-cn_topic_0000002517207984_b1967183213352"></a><a name="zh-cn_topic_0000002517207984_b1967183213352"></a>U</strong></p>
</td>
<td class="nocellnorowborder" style="border:none" valign="top" width="45.77457745774577%">&nbsp;&nbsp;</td>
<td class="cell-norowborder" style="border:none" valign="top" width="33.33333333333333%">&nbsp;&nbsp;</td>
</tr>
<tr id="zh-cn_topic_0000002517207984_row95762201265"><td class="row-nocellborder" style="border:none" valign="top" width="20.89208920892089%"><p id="zh-cn_topic_0000002517207984_p5576152019267"><a name="zh-cn_topic_0000002517207984_p5576152019267"></a><a name="zh-cn_topic_0000002517207984_p5576152019267"></a><strong id="zh-cn_topic_0000002517207984_b146728320352"><a name="zh-cn_topic_0000002517207984_b146728320352"></a><a name="zh-cn_topic_0000002517207984_b146728320352"></a>UDP</strong></p>
</td>
<td class="row-nocellborder" style="border:none" valign="top" width="45.77457745774577%"><p id="zh-cn_topic_0000002517207984_p1757662002616"><a name="zh-cn_topic_0000002517207984_p1757662002616"></a><a name="zh-cn_topic_0000002517207984_p1757662002616"></a>User Datagram Protocol</p>
</td>
<td class="cellrowborder" style="border:none" valign="top" width="33.33333333333333%"><p id="zh-cn_topic_0000002517207984_p1257612010267"><a name="zh-cn_topic_0000002517207984_p1257612010267"></a><a name="zh-cn_topic_0000002517207984_p1257612010267"></a>用户数据报协议</p>
</td>
</tr>
</tbody>
</table>

