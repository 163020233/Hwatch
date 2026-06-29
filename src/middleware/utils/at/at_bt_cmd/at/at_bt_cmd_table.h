/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: At cmd tab \n
 *
 */

#if ! defined (AT_BT_CMD_AT_CMD_TALBE_H)
#define AT_BT_CMD_AT_CMD_TALBE_H

#include "at.h"

/* HID服务 */
typedef struct {
    char                *para1;
} bt_args_t;
const at_para_parse_syntax_t bt_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(bt_args_t, para1)
    }
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
} hid_send_args_t;
const at_para_parse_syntax_t hid_send_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(hid_send_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(hid_send_args_t, para2)
    }
};

/* BLE管理 */
typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
} send_mouse_args_t;
const at_para_parse_syntax_t send_mouse_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(send_mouse_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 5,
        .offset = offsetof(send_mouse_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 5,
        .offset = offsetof(send_mouse_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(send_mouse_args_t, para4)
    }
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
} bgle_set_name_syntax_t;
const at_para_parse_syntax_t bgle_set_name_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(bgle_set_name_syntax_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 510,
        .offset = offsetof(bgle_set_name_syntax_t, para2)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
} ble_scan_args_t;
const at_para_parse_syntax_t ble_scan_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 5,
        .offset = offsetof(ble_scan_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 5,
        .offset = offsetof(ble_scan_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(ble_scan_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(ble_scan_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(ble_scan_args_t, para5)
    },
};

/* SLE管理 */
typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
} gle_connect_args_t;
const at_para_parse_syntax_t gle_connect_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(gle_connect_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(gle_connect_args_t, para2)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} gle_set_adv_args_t;
const at_para_parse_syntax_t gle_set_adv_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(gle_set_adv_args_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} gle_enable_adv_args_t;
const at_para_parse_syntax_t gle_enable_adv_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 1,
        .offset = offsetof(gle_enable_adv_args_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
    char                 *para7;
    char                 *para8;
    char                 *para9;
} sle_set_phy_t;
const at_para_parse_syntax_t sle_set_phy[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para8)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .last = true,
        .offset = offsetof(sle_set_phy_t, para9)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
} sle_conn_para_update_t;
const at_para_parse_syntax_t sle_conn_para_update[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_conn_para_update_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_conn_para_update_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_conn_para_update_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_conn_para_update_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .last = true,
        .offset = offsetof(sle_conn_para_update_t, para5)
    },
};

/* SLE数传 */
typedef struct {
    uint32_t             para_map;
    char                 *para1;
} gle_add_server_args_t;
const at_para_parse_syntax_t gle_add_server_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(gle_add_server_args_t, para1)
    },
};
typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
} gle_add_service_args_t;
const at_para_parse_syntax_t gle_add_service_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(gle_add_service_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(gle_add_service_args_t, para2)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
} gle_add_property_args_t;
const at_para_parse_syntax_t gle_add_property_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(gle_add_property_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(gle_add_property_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(gle_add_property_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(gle_add_property_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(gle_add_property_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(gle_add_property_args_t, para6)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
    char                 *para7;
    char                 *para8;
} gle_add_descriptor_args_t;
const at_para_parse_syntax_t gle_add_descriptor_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_add_descriptor_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_add_descriptor_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(gle_add_descriptor_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_add_descriptor_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_add_descriptor_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(gle_add_descriptor_args_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_add_descriptor_args_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 512,
        .offset = offsetof(gle_add_descriptor_args_t, para8)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} gle_start_service_args_t;
const at_para_parse_syntax_t gle_start_service_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_start_service_args_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
} gle_discovery_services_args_t;
const at_para_parse_syntax_t gle_discovery_services_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(gle_discovery_services_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_discovery_services_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_discovery_services_args_t, para3)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
} gle_write_value_args_t;
const at_para_parse_syntax_t gle_write_value_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(gle_write_value_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_write_value_args_t, para2)
    },    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_write_value_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(gle_write_value_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_write_value_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 521,
        .offset = offsetof(gle_write_value_args_t, para6)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
} gle_read_value_args_t;
const at_para_parse_syntax_t gle_read_value_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(gle_read_value_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_read_value_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_read_value_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(gle_read_value_args_t, para4)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
} sle_sample_server_send_data_t;
const at_para_parse_syntax_t sle_sample_server_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_sample_server_send_data_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 800,
        .last = true,
        .offset = offsetof(sle_sample_server_send_data_t, para2)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
    char                 *para7;
    char                 *para8;
    char                 *para9;
    char                 *para10;
    char                 *para11;
} ble_set_adv_para_t;
const at_para_parse_syntax_t ble_set_adv_para[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(ble_set_adv_para_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(ble_set_adv_para_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(ble_set_adv_para_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(ble_set_adv_para_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(ble_set_adv_para_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(ble_set_adv_para_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(ble_set_adv_para_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(ble_set_adv_para_t, para8)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(ble_set_adv_para_t, para9)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(ble_set_adv_para_t, para10)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .last = true,
        .offset = offsetof(ble_set_adv_para_t, para11)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
} at_ble_set_adv_data_t;
const at_para_parse_syntax_t at_ble_set_adv_data[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(at_ble_set_adv_data_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 128,
        .offset = offsetof(at_ble_set_adv_data_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(at_ble_set_adv_data_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 128,
        .offset = offsetof(at_ble_set_adv_data_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .last = true,
        .offset = offsetof(at_ble_set_adv_data_t, para5)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} ble_enable_adv_by_param_t;
const at_para_parse_syntax_t ble_enable_adv_by_param[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .last = true,
        .offset = offsetof(ble_enable_adv_by_param_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
    char                 *para7;
    char                 *para8;
} sle_set_adv_par_t;
const at_para_parse_syntax_t sle_set_adv_par[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_adv_par_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_adv_par_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_set_adv_par_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_set_adv_par_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_adv_par_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(sle_set_adv_par_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_adv_par_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .last = true,
        .offset = offsetof(sle_set_adv_par_t, para8)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
} sle_set_adv_data_t;
const at_para_parse_syntax_t sle_set_adv_data[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_adv_data_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_set_adv_data_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_set_adv_data_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 521,
        .offset = offsetof(sle_set_adv_data_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 521,
        .last = true,
        .offset = offsetof(sle_set_adv_data_t, para5)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} sle_enable_adv_t;
const at_para_parse_syntax_t sle_enable_adv[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .last = true,
        .offset = offsetof(sle_enable_adv_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} ble_read_name_t;
const at_para_parse_syntax_t ble_read_name[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .last = true,
        .offset = offsetof(ble_read_name_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} ble_set_local_addr_t;
const at_para_parse_syntax_t ble_set_local_addr[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 12,
        .last = true,
        .offset = offsetof(ble_set_local_addr_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} bgle_connect_args_t;
const at_para_parse_syntax_t bgle_connect_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 14,
        .last = true,
        .offset = offsetof(bgle_connect_args_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
} chba_start_args_t;
const at_para_parse_syntax_t bgle_chba_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 1,
        .offset = offsetof(chba_start_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 1,
        .offset = offsetof(chba_start_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(chba_start_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 1,
        .offset = offsetof(chba_start_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 2,
        .last = true,
        .offset = offsetof(chba_start_args_t, para5)
    },
};

/* SLE管理 */
at_ret_t bt_at_sle_enable(void);
at_ret_t bt_at_gle_at_cmd_set_adv(void);
at_ret_t bt_at_gle_at_cmd_enable_adv(const gle_enable_adv_args_t *args);
at_ret_t bt_at_sle_set_adv_param(const sle_set_adv_par_t *args);
at_ret_t bt_at_sle_set_adv_data(const sle_set_adv_data_t *args);
at_ret_t bt_at_sle_enable_adv_by_param(const sle_enable_adv_t *args);
at_ret_t bt_at_sle_disable_adv_by_param(const sle_enable_adv_t *args);
at_ret_t bt_at_gle_at_cmd_connect(const gle_connect_args_t *args);
at_ret_t bt_at_gle_at_cmd_disconnect(const gle_connect_args_t *args);
at_ret_t bt_at_gle_pair_remote(const gle_connect_args_t *args);
at_ret_t bt_at_gle_remove_pair_remote(const gle_connect_args_t *args);
at_ret_t bt_at_gle_sdk_get_paired_devices_num(void);
at_ret_t bt_at_gle_sdk_get_paired_devices(void);
at_ret_t bt_at_gle_sdk_get_pair_state(void);
at_ret_t bt_at_sle_set_phy(const sle_set_phy_t *arg);
at_ret_t bt_at_sle_conn_para_update(const sle_conn_para_update_t *arg);
/* SLE数传 */
at_ret_t bt_at_gle_at_cmd_add_server(const gle_add_server_args_t *args);
at_ret_t bt_at_gle_at_cmd_add_service(const gle_add_service_args_t *args);
at_ret_t bt_at_gle_at_cmd_add_property(const gle_add_property_args_t *args);
at_ret_t bt_at_gle_at_cmd_add_descriptor(const gle_add_descriptor_args_t *args);
at_ret_t bt_at_gle_at_cmd_start_service(const gle_start_service_args_t *args);
at_ret_t bt_at_gle_at_cmd_ssaps_register_cbks(void);
at_ret_t bt_at_gle_at_cmd_dd_register_cbks(void);
at_ret_t bt_at_gle_at_cmd_ssapc_register_cbks(void);
at_ret_t bt_at_gle_at_cmd_cm_register_cbks(void);
at_ret_t bt_at_gle_at_cmd_discovery_services(const gle_discovery_services_args_t *args);
at_ret_t bt_at_gle_at_cmd_ssapc_write_req(const gle_write_value_args_t *args);
at_ret_t bt_at_gle_at_cmd_ssapc_read_req(const gle_read_value_args_t *args);

/* BLE管理 */
at_ret_t bt_at_ble_adv_enable(void);
at_ret_t bt_at_ble_adv_disable(void);
at_ret_t bt_at_ble_disconnect(void);
at_ret_t bt_at_ble_conn_par_upd(const sle_conn_para_update_t *arg);
at_ret_t bt_at_ble_set_adv_param(const ble_set_adv_para_t *arg);
at_ret_t bt_at_ble_set_adv_data(const at_ble_set_adv_data_t *arg);
at_ret_t bt_at_ble_enable_adv_by_param(const ble_enable_adv_by_param_t *arg);
at_ret_t bt_at_ble_disable_adv_by_param(const ble_enable_adv_by_param_t *arg);
at_ret_t bt_at_ble_enter_scan(const ble_scan_args_t *args);
at_ret_t bt_at_send_cmd_high_mouse_input(const send_mouse_args_t *args);
at_ret_t bt_at_send_cmd_ble_connect_peer(const bgle_connect_args_t *args);
at_ret_t bt_at_cmd_gap_enable_ble(void);
at_ret_t bt_at_cmd_hid_mouse_init(void);

/* HID服务 */
at_ret_t bt_at_ble_at_cmd_gap_ble_set_scan_parameters(const ble_scan_args_t *args);
at_ret_t bt_at_ble_at_cmd_gap_start_scan(void);
at_ret_t bt_at_ble_at_cmd_gap_stop_scan(void);

/* HID服务 */
at_ret_t bt_at_hid_init(void);
at_ret_t bt_at_hid_mouse_server_send_input_report(const send_mouse_args_t *args);
at_ret_t bt_at_hid_mouse_server_send_input_report_by_uuid(const send_mouse_args_t *args);
at_ret_t bt_at_hid_server_send_input_report_down(const hid_send_args_t *args);
at_ret_t bt_at_hid_server_send_input_report_up(const hid_send_args_t *args);
at_ret_t bt_at_hid_server_send_input_report_down_by_uuid(const hid_send_args_t *args);
at_ret_t bt_at_hid_server_send_input_report_up_by_uuid(const hid_send_args_t *args);
at_ret_t bt_at_query_paired_device_info(void);
at_ret_t bt_at_set_product_type(void);
at_ret_t bt_at_read_bdaddr(void);
at_ret_t bt_at_read_name(const ble_read_name_t *args);
at_ret_t bt_at_write_addr(const ble_set_local_addr_t *args);
at_ret_t bt_at_write_name(const bgle_set_name_syntax_t *args);

// sle service sample
at_ret_t bt_at_sle_sample_client_init(void);
at_ret_t bt_at_sle_sample_client_scan(void);
at_ret_t bt_at_sle_sample_server_init(void);
at_ret_t bt_at_sle_sample_server_send_data_by_uuid(const sle_sample_server_send_data_t *arg);
at_ret_t bt_at_sle_sample_server_send_data_by_handle(const sle_sample_server_send_data_t *arg);

#ifdef CHBA_SUPPORT
// chba
at_ret_t bt_at_chba_start(const chba_start_args_t *args);
#endif


const at_cmd_entry_t at_bt_cmd_parse_table[] = {
    {
        .name = "BLECONNPARUPD",
        .cmd_id = 1,
        .attribute = 0,
        .syntax = sle_conn_para_update,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_ble_conn_par_upd,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "BTENABLE",    // 起广播
        .cmd_id = 1,
        .attribute = 0,
        .syntax = NULL,   // 参数类型
        .cmd = bt_at_ble_adv_enable,        // 无参数时调用的函数
        .set = (at_set_func_t)NULL,   // 有参数时调用的函数
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "BTDISABLE",    // 停广播
        .cmd_id = 1,
        .attribute = 0,
        .syntax = NULL,   // 参数类型
        .cmd = bt_at_ble_adv_disable,        // 无参数时调用的函数
        .set = (at_set_func_t)NULL,   // 有参数时调用的函数
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "BLESETADVPAR",
        .cmd_id = 1,
        .attribute = 0,
        .syntax = ble_set_adv_para,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_ble_set_adv_param,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "BLESETADVDATA",
        .cmd_id = 1,
        .attribute = 0,
        .syntax = at_ble_set_adv_data,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_ble_set_adv_data,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "BLEENADVBYPAR",
        .cmd_id = 1,
        .attribute = 0,
        .syntax = ble_enable_adv_by_param,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_ble_enable_adv_by_param,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "BLEDISADVBYPAR",
        .cmd_id = 1,
        .attribute = 0,
        .syntax = ble_enable_adv_by_param,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_ble_disable_adv_by_param,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "GETBTNAME",    // 查名称
        .cmd_id = 2,
        .attribute = 0,
        .syntax = ble_read_name,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_read_name,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "GETPHYNUM",    // 查地址
        .cmd_id = 2,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_read_bdaddr,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "BTNAME",   // 更改蓝牙名称
        .cmd_id = 3,
        .attribute = 0,
        .syntax = bgle_set_name_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_write_name,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "PHYNUM",   // 更改蓝牙地址
        .cmd_id = 4,
        .attribute = 0,
        .syntax = ble_set_local_addr,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_write_addr,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "BTDISCONN",    // BLE 断链
        .cmd_id = 3,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_ble_disconnect,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "GAPENABLEBLE",
        .cmd_id = 15,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_cmd_gap_enable_ble,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SETPRODUCTTYPE",
        .cmd_id = 4,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_set_product_type,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
#ifdef CONFIG_ENABLE_BT_SAMPLE
    {
        .name = "HIDMOUSEINIT",
        .cmd_id = 15,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_cmd_hid_mouse_init,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "HIDKEYBOARDINIT",
        .cmd_id = 5,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_hid_init,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "KEYBOARDINPUTPDOWN",
        .cmd_id = 6,
        .attribute = 0,
        .syntax = hid_send_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_hid_server_send_input_report_down,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "KEYBOARDINPUTPUP",
        .cmd_id = 7,
        .attribute = 0,
        .syntax = hid_send_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_hid_server_send_input_report_up,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "KEYBOARDINPUTPDOWNBYUUID",
        .cmd_id = 6,
        .attribute = 0,
        .syntax = hid_send_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_hid_server_send_input_report_down_by_uuid,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "KEYBOARDINPUTPUPBYUUID",
        .cmd_id = 7,
        .attribute = 0,
        .syntax = hid_send_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_hid_server_send_input_report_up_by_uuid,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "HIDMOUSEINPUT",
        .cmd_id = 8,
        .attribute = 0,
        .syntax = send_mouse_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_hid_mouse_server_send_input_report,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "HIDMOUSEINPUTBYUUID",
        .cmd_id = 8,
        .attribute = 0,
        .syntax = send_mouse_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_hid_mouse_server_send_input_report_by_uuid,
        .read = NULL,
        .test = NULL,
    },
#endif
    {
        .name = "MOUSEINPUT",
        .cmd_id = 8,
        .attribute = 0,
        .syntax = send_mouse_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_send_cmd_high_mouse_input,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "BTCONNADDR",
        .cmd_id = 9,
        .attribute = 0,
        .syntax = bgle_connect_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_send_cmd_ble_connect_peer,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "BLEENTERSCAN",
        .cmd_id = 9,
        .attribute = 0,
        .syntax = ble_scan_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_ble_enter_scan,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "BLESETSCANPARAM",
        .cmd_id = 9,
        .attribute = 0,
        .syntax = ble_scan_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_ble_at_cmd_gap_ble_set_scan_parameters,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "BLESCANOPEN",
        .cmd_id = 12,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_ble_at_cmd_gap_start_scan,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "BLESCANCLOSE",
        .cmd_id = 12,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_ble_at_cmd_gap_stop_scan,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "BTPAIRINFO?",
        .cmd_id = 10,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_query_paired_device_info,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEENABLE",    // 使能SLE
        .cmd_id = 1,
        .attribute = 0,
        .syntax = NULL,   // 参数类型
        .cmd = bt_at_sle_enable,        // 无参数时调用的函数
        .set = (at_set_func_t)NULL,   // 有参数时调用的函数
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLESETADV",
        .cmd_id = 12,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_gle_at_cmd_set_adv,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEENABLEADV",
        .cmd_id = 13,
        .attribute = 0,
        .syntax = gle_enable_adv_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_gle_at_cmd_enable_adv,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLESETADVPAR",
        .cmd_id = 13,
        .attribute = 0,
        .syntax = sle_set_adv_par,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_sle_set_adv_param,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLESETADVDATA",
        .cmd_id = 13,
        .attribute = 0,
        .syntax = sle_set_adv_data,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_sle_set_adv_data,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEENADVBYPAR",
        .cmd_id = 13,
        .attribute = 0,
        .syntax = sle_enable_adv,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_sle_enable_adv_by_param,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEDISADVBYPAR",
        .cmd_id = 13,
        .attribute = 0,
        .syntax = sle_enable_adv,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_sle_disable_adv_by_param,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLECONNADDR",
        .cmd_id = 11,
        .attribute = 0,
        .syntax = gle_connect_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_gle_at_cmd_connect,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEDISCONN",
        .cmd_id = 14,
        .attribute = 0,
        .syntax = gle_connect_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_gle_at_cmd_disconnect,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEPAIRREMOTE",
        .cmd_id = 14,
        .attribute = 0,
        .syntax = gle_connect_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_gle_pair_remote,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEREMOVEPAIR",
        .cmd_id = 14,
        .attribute = 0,
        .syntax = gle_connect_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_gle_remove_pair_remote,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLESETPHY",
        .cmd_id = 14,
        .attribute = 0,
        .syntax = sle_set_phy,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_sle_set_phy,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLECONNPARUPD",
        .cmd_id = 14,
        .attribute = 0,
        .syntax = sle_conn_para_update,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_sle_conn_para_update,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEGETPAIRDEVICESNUM",
        .cmd_id = 14,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_gle_sdk_get_paired_devices_num,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEGETPAIRDEVICES",
        .cmd_id = 14,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_gle_sdk_get_paired_devices,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEGETPAIRSTATE",
        .cmd_id = 14,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_gle_sdk_get_pair_state,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEADDSERVER",
        .cmd_id = 16,
        .attribute = 0,
        .syntax = gle_add_server_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_gle_at_cmd_add_server,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEADDSERVICE",
        .cmd_id = 17,
        .attribute = 0,
        .syntax = gle_add_service_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_gle_at_cmd_add_service,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEADDPROPERTY",
        .cmd_id = 18,
        .attribute = 0,
        .syntax = gle_add_property_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_gle_at_cmd_add_property,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEADDDESCRIPTOR",
        .cmd_id = 19,
        .attribute = 0,
        .syntax = gle_add_descriptor_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_gle_at_cmd_add_descriptor,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLESTARTSERVICE",
        .cmd_id = 20,
        .attribute = 0,
        .syntax = gle_start_service_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_gle_at_cmd_start_service,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLESSAPSERREGISTER",
        .cmd_id = 21,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_gle_at_cmd_ssaps_register_cbks,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLESSAPCENREGISTER",
        .cmd_id = 23,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_gle_at_cmd_ssapc_register_cbks,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEDDREGISTER",
        .cmd_id = 22,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_gle_at_cmd_dd_register_cbks,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLECMREGISTER",
        .cmd_id = 24,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_gle_at_cmd_cm_register_cbks,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLEDISCOVERYSERVICES",
        .cmd_id = 25,
        .attribute = 0,
        .syntax = gle_discovery_services_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_gle_at_cmd_discovery_services,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLESSAPCENWRITE",
        .cmd_id = 26,
        .attribute = 0,
        .syntax = gle_write_value_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_gle_at_cmd_ssapc_write_req,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLESSAPCENREAD",
        .cmd_id = 27,
        .attribute = 0,
        .syntax = gle_read_value_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_gle_at_cmd_ssapc_read_req,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLECLIENTINIT",
        .cmd_id = 28,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_sle_sample_client_init,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLECLIENTSCAN",
        .cmd_id = 28,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_sle_sample_client_scan,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLESERVERINIT",
        .cmd_id = 28,
        .attribute = 0,
        .syntax = NULL,
        .cmd = bt_at_sle_sample_server_init,
        .set = (at_set_func_t)NULL,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLESSENDBYUUID",
        .cmd_id = 28,
        .attribute = 0,
        .syntax = sle_sample_server_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_sle_sample_server_send_data_by_uuid,
        .read = NULL,
        .test = NULL,
    },
    {
        .name = "SLESSENDBYHANDLE",
        .cmd_id = 28,
        .attribute = 0,
        .syntax = sle_sample_server_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_sle_sample_server_send_data_by_handle,
        .read = NULL,
        .test = NULL,
    },
#ifdef CHBA_SUPPORT
    {
        .name = "CHBASTART",
        .cmd_id = 29,
        .attribute = 0,
        .syntax = bgle_chba_syntax,
        .cmd = NULL,
        .set = (at_set_func_t)bt_at_chba_start,
        .read = NULL,
        .test = NULL,
    },
#endif
};

#endif  /* AT_BT_CMD_TALBE_H */