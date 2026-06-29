/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: intelligent_transform.cpp
 * Create: 2025-11-26
 */

#include <map>
#include <string>
#include "intelligent_transform.h"
namespace OHOS {

std::string findWords(int num)
{
    std::string targetString = "";
    static std::map<int, std::string> chineseNum = {
        {0, "零"}, {1, "一"}, {2, "二"}, {3, "三"}, {4, "四"}, {5, "五"}, {6, "六"}, {7, "七"}, {8, "八"}, {9, "九"}};
    auto iter = chineseNum.find(num);
    if (iter != chineseNum.end()) {
        targetString.append(iter->second);
    }
    return targetString;
}

std::string numberToChinese(int num)
{
    std::string targetString = "";
    if (num < 0 || num > 99) {
        return "";
    }
    if (num >= 0 && num <= 9) {
        return targetString.append(findWords(num));
    } else if (num % 10 == 0) {
        if (num / 10 == 1) {
            return targetString.append("十");
        } else {
            return targetString.append(findWords(num / 10)).append("十");
        }
    } else {
        return targetString.append(findWords(num / 10)).append("十").append(findWords(num % 10));
    }
}
}  // namespace OHOS