#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
# 一站式代码修复工具合集
# 功能1：ISO-8859 转 UTF-8
# 功能2：替换版权标签 HiSilicon (Shanghai) Technologies Co., Ltd./HiSilicon (Shanghai) Technologies Co., Ltd.
# 功能3：批量设置文件权限为 644
# 功能4：GBK/GB2312/GB18030 转 UTF-8
# 使用方式：
#    ./diting_commubity_release.sh 目标路径          # 执行全部4个功能
#    ./diting_commubity_release.sh 目标路径 [iso|tag|perm|gbk]  # 执行单个功能

# 注意：暂时关闭严格退出，避免 grep 无匹配导致脚本退出
set +e

# ====================== 颜色定义 ======================
RED='\033[31m'
GREEN='\033[32m'
YELLOW='\033[33m'
BLUE='\033[34m'
CYAN='\033[36m'
NC='\033[0m'

# ====================== 参数处理 ======================
if [ $# -lt 1 ]; then
    echo -e "${CYAN}===============================================${NC}"
    echo -e "${GREEN}        一站式代码修复脚本（全能版）${NC}"
    echo -e "${CYAN}===============================================${NC}"
    echo "用法："
    echo "  $0 <目标路径>                # 执行全部修复"
    echo "  $0 <目标路径> iso            # 仅修复 ISO-8859 编码"
    echo "  $0 <目标路径> tag            # 仅替换版权标签"
    echo "  $0 <目标路径> perm           # 仅修复文件权限 644"
    echo "  $0 <目标路径> gbk            # 仅修复 GBK 编码"
    exit 1
fi

TARGET_DIR="$1"
MODE="${2:-all}"

if [ ! -d "${TARGET_DIR}" ]; then
    echo -e "${RED}❌ 错误：路径 ${TARGET_DIR} 不存在或不是目录！${NC}"
    exit 1
fi

echo -e "${GREEN}🚀 目标路径：${TARGET_DIR}${NC}"
echo -e "${GREEN}🚀 运行模式：${MODE}${NC}"
echo -e "========================================================"

# ====================== 功能1：ISO-8859 转 UTF-8 ======================
fix_iso8859() {
    echo -e "${BLUE}======== 1. ISO-8859 → UTF-8 转换 ========${NC}"
    CHARSET_FILE="${TARGET_DIR}/charset.txt"
    ISO_LIST_FILE="${TARGET_DIR}/iso.list"

    # 查找代码文件，检测编码（屏蔽错误，防止退出）
    find "${TARGET_DIR}" -type f \
        -name '*.c' -o \
        -name '*.h' -o \
        -name '*.cpp' -o \
        -name '*.hpp' -o \
        -name '*.java' -o \
        -name '*.js' -o \
        -name '*.go' -o \
        -name '*.rs' -o \
        -name '*.css' -o \
        -name '*.html' -o \
        -name '*.sh' -o \
        -name '*.py' \
    | xargs file 2>/dev/null \
    | grep -v -E "UTF-8 text|ASCII text|PNG image|with BOM|empty|Unicode text" \
    > "${CHARSET_FILE}"

    # 提取ISO文件列表
    grep "ISO-8859" "${CHARSET_FILE}" | awk -F: '{print $1}' | sort -u | grep -v "^$" > "${ISO_LIST_FILE}" 2>/dev/null

    if [ ! -s "${ISO_LIST_FILE}" ]; then
        echo -e "${YELLOW}⚠️ 无 ISO-8859 文件，跳过${NC}"
        rm -f "${CHARSET_FILE}" "${ISO_LIST_FILE}"
        return
    fi

    # 转换
    while IFS= read -r FILE_PATH; do
        [ -z "${FILE_PATH}" ] && continue
        [ ! -f "${FILE_PATH}" ] && continue

        BACKUP="${FILE_PATH}.iso8859.bak"
        TMP="${FILE_PATH}.utf8.tmp"
        cp -pf "${FILE_PATH}" "${BACKUP}"

        if iconv -f ISO-8859-1 -t UTF-8 "${FILE_PATH}" -o "${TMP}" 2>/dev/null; then
            mv -f "${TMP}" "${FILE_PATH}"
            rm -f "${BACKUP}"
            echo -e "${GREEN}✅ 转换成功：${FILE_PATH}${NC}"
        else
            mv -f "${BACKUP}" "${FILE_PATH}"
            echo -e "${RED}❌ 转换失败：${FILE_PATH}${NC}"
        fi
    done < "${ISO_LIST_FILE}"

    rm -f "${CHARSET_FILE}" "${ISO_LIST_FILE}"
}

# ====================== 功能2：替换版权标签 ======================
replace_company_tag() {
    echo -e "${BLUE}======== 2. 替换版权标签 HiSilicon (Shanghai) Technologies Co., Ltd. ========${NC}"
    find "${TARGET_DIR}" -type f \( \
        -name "*.c" -o -name "*.h" -o -name "*.cpp" -o \
        -name "CMakeLists.txt" -o -name "*.cmake" -o \
        -name "*.s" -o -name "*.sh" -o -name "Kconfig" \) -print0 | \
    xargs -0 grep -lZ -E "@?HiSilicon (Shanghai) Technologies Co., Ltd." 2>/dev/null | \
    xargs -0 -r sed -i.bak -E 's/@?HiSilicon (Shanghai) Technologies Co., Ltd./HiSilicon (Shanghai) Technologies Co., Ltd./g' 2>/dev/null

    find "${TARGET_DIR}" -name "*.bak" -delete 2>/dev/null
    echo -e "${GREEN}✅ 版权标签替换完成${NC}"
}

# ====================== 功能3：设置文件权限 644 ======================
fix_file_perms() {
    echo -e "${BLUE}======== 3. 设置文件权限为 644 ========${NC}"
    find "${TARGET_DIR}" -type f \( \
        -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o \
        -name "README*" -o -name "LICENSE*" -o -name "*.txt" -o -name "*.md" -o \
        -name "*.json" -o -name "*.xml" -o -name "*.cfg" -o -name "*.conf" \) -exec chmod 644 {} \; 2>/dev/null
    echo -e "${GREEN}✅ 权限 644 设置完成${NC}"
}

# ====================== 功能4：GBK 转 UTF-8 ======================
fix_gbk() {
    echo -e "${BLUE}======== 4. GBK/GB2312/GB18030 → UTF-8 ========${NC}"
    find "${TARGET_DIR}" -type f \( \
        -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" -o \
        -name "*.java" -o -name "*.js" -o -name "*.py" -o -name "*.sh" -o \
        -name "*.txt" -o -name "*.md" \) | while read -r file; do

        encoding=$(file -i "$file" 2>/dev/null | awk -F= '{print $2}')
        if [[ "$encoding" == *gb2312* || "$encoding" == *gbk* || "$encoding" == *gb18030* ]]; then
            cp -af "$file" "$file.bak"
            if iconv -f GB18030 -t UTF-8 "$file" -o "$file.tmp" 2>/dev/null; then
                mv -f "$file.tmp" "$file"
                rm -f "$file.bak"
                echo -e "${GREEN}✅ GBK 转 UTF-8：$file${NC}"
            else
                mv -f "$file.bak" "$file"
                echo -e "${RED}❌ 失败：$file${NC}"
            fi
        fi
    done
}

# ====================== 执行模式 ======================
case "${MODE}" in
    iso) fix_iso8859 ;;
    tag) replace_company_tag ;;
    perm) fix_file_perms ;;
    gbk) fix_gbk ;;
    all)
        fix_iso8859
        replace_company_tag
        fix_file_perms
        fix_gbk
        ;;
    *)
        echo -e "${RED}❌ 错误模式：${MODE}${NC}"
        exit 1
        ;;
esac

echo -e "\n${GREEN}🎉 所有任务执行完成！${NC}"