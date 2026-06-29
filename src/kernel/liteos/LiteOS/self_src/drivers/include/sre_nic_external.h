/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: NIC模块头文件
 * Author: LibCK项目组
 * Create: 2023-09-16
 * History: 2023-09-16 LibCK项目组  EtherCAT网卡驱动模块头文件
 */
#ifndef SRE_NIC_EXTERNAL_H
#define SRE_NIC_EXTERNAL_H

#include "sre_nic.h"
#include "sre_ecat_external.h"

/* 报文L1/L2异常标识 */
#define OS_NIC_FRAME_INVALID_STATUS     0x1U
/* 报文drop标识 */
#define OS_NIC_FRAME_DROP_STATUS        0x8U

/*
 * L5ID
 * 0x0：DHCP/BOOTP报文；
 * 0x1：IPPM报文；
 * 0x2：UDP封装的1588报文；
 * 0x3：GTPU报文；
 * 0x4：保留；
 * 0x5：VxLAN报文
 * 0x6~0xE：保留；
 * 0xF：未知报文
 * L4ID
 * 0x0：UDP报文；
 * 0x1：TCP报文；
 * 0x2：GRE报文；
 * 0x3：SCTP报文；
 * 0x4：IGMP报文；
 * 0x5：ICMP报文；
 * 0x6：ESP报文；
 * 0x7：AH报文；
 * 0x8~0xE：保留；
 * 0xF：未知报文
 * L3ID
 * 0x0：IPv4报文；
 * 0x1：IPv6报文；
 * 0x2：保留；
 * 0x3：保留；
 * 0x4：Q.922报文；
 * 0x5：MAC封装的1588报文
 * 0x6：ARP；
 * 0x7：RARP；
 * 0x8：LLDP；
 * 0x9：802.1ag/Y.1731；
 * 0xA~C：保留；
 * 0xD：PPPoE；
 * 0xE：BPDU（含Slow Protocols OAM（802.3AH OAM）, Slow Protocols Port Trunk）；
 * 0xF：未知报文
 * VLAN
 * 0：无VLAN封装；
 * 1：一层VLAN封装报文；
 * 2：二层VLAN封装的报文；
 * 3：保留。
 * DMAC
 * 0：单播
 * 1：多播
 * 2：广播
 */
struct OsNicRxHeadDescriptorLe {
    /* 【注意】该部分为软件或其他加速器预留，PPE只实现单向链表，固定填充为0。 */
    U32 prevDataAddrLow;
    U32 prevDataAddrHigh;

    /* 下一个分片数据描述指针，最后分片填写NULL。（高28bit，1213不关心） */
    U32 nextDataAddrLow;
    U32 nextDataAddrHigh;

    /* 该分片报文数据BUFF块基地址；（高28bit，1213不关心） */
    U32 refItselfAddrLow;
    U32 refItselfAddrHigh;

    /* 表示该分片报文数据的长度，单位：字节。注：0表示非法 */
    U32 dataLength : 16;
    /* 报文数据首地址相对于DataBuffBaseAddr的偏移，单位：字节。 */
    U32 dataOffset : 16;

    /* 描述符扩展指示。为1表示存在SEC BD，为2表示存在SA BD，其它值；保留；【注意】该部分为软件预留，PPE固定填充为0。 */
    U32 appendType : 2;
    /* 保留域 */
    U32 reserved : 14;
    /* 该片报文数据相对于整报文的偏移，单位：字节。（该域为软件预留部分） */
    U32 chainOffset : 16;

    /* 硬件处理固定写0x05； */
    U32 own : 4;
    /*
     * 报文接收状态异常类型：
     * bit[0]：为1表示L1、L2错误；
     * bit[1]：为1表示L3错误；
     * bit[2]：为1表示L4错误；
     * bit[3]：为1表示报文有drop，内部mem存不下出现截断；
     * bit[4]：为1表示链式buf申请时发生丢弃，报文出现截断。
     * 各种协议异常说明
     * 报文L1异常
     *      报文EOP丢失；
     *      报文SOP丢失；
     *      报文总长度大于9600B；
     * 报文L3异常
     *      IPv4_Version≠4；
     *      IPv4的实际Payload（含IPv4头）小于20B；
     *      IPv4_IHL<5；
     *      IPv4_Total Length<4*IHL；
     *      单分片，或多分片的首分片TCP报文的IPv4_Total Length减去4*IHL小于20；
     *      单分片，或多分片的首分片UDP报文的IPv4_Total Length减去4*IHL小于8；
     *      单分片，或多分片的首分片SCTP报文的IPv4_Total Length减去4*IHL小于12；
     *      IPv4_Total Length不等于IP头长度+实际IP的Payload长度；
     *      IPv4的Header Checksum不正确；
     * 报文L4异常
     *      UDP_Length小于8；
     *      UDP的实际Payload（含UDP头）长度小于8B；
     *      TCP的实际Payload（含TCP头）长度小于20B；
     */
    U32 status : 8;
    /* 报文的总长度，单位：字节，有效范围：1~65535。1213最大支持10K包长，只用【15:0】比特；注：0表示非法。 */
    U32 pktLen : 20;
    /* 保留域 */
    U32 reserved1 : 5;
    /*
     * 分片模式
     * 0：不分片
     * 1：保留；
     * 2：IP分片；
     * 3：保留
     */
    U32 splitType : 2;
    U32 reserved2 : 4;    /* 保留域 */
    U32 mplsNum : 2;      /* MPLS的层数 */
    U32 reserved3 : 3;    /* 保留域 */
    U32 dmac : 2;         /* 0: 单播 1: 多播 2: 广播 */
    U32 vlan : 2;         /* VLAN报文标识 */
    U32 l3Id : 4;         /* 三层报文标识 */
    U32 l4Id : 4;         /* 四层报文标识 */
    U32 l5Id : 4;         /* 五层报文标识 */

    U32 reserved4 : 18;   /* 保留域 */
    /* 二层头长度，包括mac地址、VLAN、ETHERTYPE,即 IP头在首分片的偏移位置（Mac头的长度），单位：word16 (2Bytes) */
    U32 l2len : 7;
    /* IP头的长度，  单位： word32（4B），对于IPV4有效取值5~15； */
    U32 l3len : 7;

    U32 reserved5 : 16;   /* 保留域，填0 */
    U32 vlanId : 12;      /* VLAN报文的VLAN_ID域 */
    U32 cfi : 1;          /* VLAN报文的CFI域 */
    U32 priority : 3;     /* VLAN报文的Priority域 */

    U32 reserved6;        /* 保留域 */
    U32 reserved7;        /* 保留域 */
    U32 reserved8;        /* 保留域 */
    /* 压缩的时间戳: Bit[31:22]表示秒值，单位秒。Bit[21:0]表示纳秒值，单位1024纳秒。 */
    U32 timestamp;
};

struct OsNicRxFragDescriptorLe {
    /* 【注意】该部分为软件或其他加速器预留，PPE只实现单向链表，固定填充为0。 */
    U32 prevDataAddrLow;
    U32 prevDataAddrHigh;

    /* 下一个分片数据描述指针，最后分片填写NULL。（高28bit，1213不关心） */
    U32 nextDataAddrLow;
    U32 nextDataAddrHigh;

    /* 该分片报文数据BUFF块基地址；（高28bit，1213不关心） */
    U32 refItselfAddrLow;
    U32 refItselfAddrHigh;

    /* 表示该分片报文数据的长度，单位：字节。注：0表示非法 */
    U32 dataLength : 16;
    /* 报文数据首地址相对于DataBuffBaseAddr的偏移，单位：字节。 */
    U32 dataOffset : 16;

    /* 保留域 */
    U32 reserved : 16;
    /* 该片报文数据相对于整报文的偏移，单位：字节。（该域为软件预留部分） */
    U32 chainOffset : 16;
    /* 保留域 */
    U32 reserved1[8];
};

typedef union {
    struct OsNicRxHeadDescriptorLe head;
    struct OsNicRxFragDescriptorLe frag;
    U32 word[16];
} OsNicRxDescriptor;

enum OsNicType {
    /* ge驱动暴露的收发包网口 */
    OS_NIC_NORMAL = 0,
    /* ethercat 主网口 */
    OS_NIC_ECAT_PRIM,
    /* ethercat 副网口 */
    OS_NIC_ECAT_SEC,
};

extern U32 OsNicRxPoll(U32 nicIndex);
extern U32 OsNicTxPacket(U32 nicIndex, const U8 *packet, U32 length);
extern U32 OsNicRxPollEcat(OsNicRxDescriptor *rxd, enum OsNicType nicType);
extern U32 OsNicSetEcatNic(U32 nicIndex);
extern U32 OsNicSetEcatRedNic(U32 nicIndex);
extern void OsNicUnSetEcatNic(U32 nicIndex);

/*
 * 描述: nic网卡收包钩子
 */
typedef U32 (*NicRxPollFunc)(OsNicRxDescriptor *rxd, enum OsNicType nicType);

#endif /* SRE_NIC_EXTERNAL_H */