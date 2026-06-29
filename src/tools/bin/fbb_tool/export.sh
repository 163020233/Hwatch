#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
# 脚本功能：导出环境变量
get_script_path() {
    if [ -n "${BASH_SOURCE-}" ]; then
        echo "${BASH_SOURCE}"
    elif [ -n "${ZSH_VERSION-}" ]; then
        echo "${(%):-%x}"
    else
        # 对于不支持的shell，回退到当前脚本
        echo "$0"
    fi
}

self_path="$(get_script_path)"

# 兼容的方式获取绝对路径
if command -v realpath >/dev/null 2>&1; then
    script_name="$(realpath "${self_path}")"
elif command -v readlink >/dev/null 2>&1; then
    script_name="$(readlink -f "${self_path}" 2>/dev/null || readlink "${self_path}")"
else
    # 如果没有realpath或readlink，使用Python作为后备
    script_name="$(python3 -c "import os; print(os.path.abspath('${self_path}'))" 2>/dev/null)"
fi

# 如果以上方法都失败，使用保守方法
if [ -z "$script_name" ] || [ ! -e "$script_name" ]; then
    script_name="$self_path"
    if [ ! -e "$script_name" ]; then
        echo "Warning: Cannot determine script path reliably" >&2
        script_name="$(pwd)/$(basename "$self_path")"
    fi
fi

script_dir="$(dirname "$script_name")"

# 检查fbb.py是否存在于脚本目录中
if [ ! -f "$script_dir/fbb.py" ]; then
    echo "Error: fbb.py not found in $script_dir" >&2
    exit 1
fi

# 确保脚本有执行权限
if [ ! -x "$script_dir/fbb.py" ]; then
    echo "Setting execute permission for fbb.py..."
    chmod +x "$script_dir/fbb.py" 2>/dev/null || {
        echo "Warning: Cannot set execute permission for fbb.py" >&2
    }
fi

# 将目录添加到PATH，但避免重复添加
case ":${PATH}:" in
    *:"${script_dir}":*)
        echo "fbb.py directory already in PATH"
        ;;
    *)
        export PATH="${script_dir}:$PATH"
        echo "Added $script_dir to PATH"
        ;;
esac

# 验证fbb.py是否可以执行
if command -v fbb.py >/dev/null 2>&1; then
    echo "fbb.py is now available for execution"
else
    echo "Warning: fbb.py may not be executable from PATH" >&2
fi
