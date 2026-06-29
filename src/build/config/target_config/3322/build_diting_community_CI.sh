#!/bin/bash
set -e

echo "替换3322社区门禁配置文件"
cp build/config/target_config/3322/config.py.community build/config/target_config/3322/config.py

echo "编译谛听社区门禁 release 版本"
./build.py pack_diting_community -release

echo "社区门禁固件编译完成"
