/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: DialBinParserV3_1
 * Author:
 * Create: 2025-09-13
 */

#ifndef DIAL_BIN_PARSER_V3_1_H
#define DIAL_BIN_PARSER_V3_1_H
#include "DialBinParserV3.h"
#include "main/dial/DialSphereView.h"

namespace OHOS {
class DialBinParserV3_1 : public DialBinParserV3 {
public:
    DialBinParserV3_1();
    ~DialBinParserV3_1() override;

protected:
    bool ParseDialLightAttr(ContainerInterface &parent);
    bool ParseDialRain(ContainerInterface &parent);
    bool ParseDialRipple(ContainerInterface &parent);
    bool ParseDialEarth(ContainerInterface &parent);
    bool ParseDialMoon(ContainerInterface &parent);

    bool IsTypeValid(uint8_t type) override;

private:
    DialSphereView *ParseSphereInternal(uint16_t texture, uint32_t offset, uint32_t len);
}; // class DialBinParserV3_1
} // namespace OHOS
#endif // DIAL_BIN_PARSER_V3_1_H
