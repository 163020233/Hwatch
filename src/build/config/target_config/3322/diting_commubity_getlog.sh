#!/bin/bash
# diting上社区自动化每日构建获取代码仓提交记录，执行方式：./diting_commubity_getlog.sh 2026-06-05 >> getlog.txt

if [ "$1" = "date" ]; then
    date +"%Y-%m-%d"
fi
TARGET_DATE="${1:-$(date +%Y-%m-%d)}"
# diting不涉及的仓库：完全忽略（精确路径匹配，以 ./ 开头）
DENY_LIST=(
    "./drivers/chips/ws53"
    "./drivers/chips/socmn2"
    "./drivers/chips/hi2132"
	"./build/config/target_config/bs27a"
	"./build/config/target_config/ws63"
	"./build/config/target_config/hi2132"
	"./build/config/target_config/ws53"
	"./application/hi2132"
	"./middleware/chips/sw21"
	"./middleware/chips/1a72"
	"./middleware/chips/socmn2"
	"./drivers/chips/1a72"
	"./drivers/chips/bs2x"
	"./libs_url/sw21"
	"./libs_url/bs21"
	"./libs_url/bs27a"
	"./libs_url/bs22"
	"./libs_url/ws63"
	"./libs_url/bs26"
	"./libs_url/bs21e./libs_url/bs21e"
	"./libs_url/socmn2"
	"./libs_url/bs21a"
	"./libs_url/bs25"
	"./libs_url/sw22"
	"./libs_url/ws53"
	"./bootloader/flashboot_bs27a"
	"./libs_url/bs20"
	"./libs_url/bs21e"
	"./middleware/chips"
	"./application/bs27a"
	"./test/hi2132"
	"./test/platform/testsuite"
	"./test/wifi"
	"./build"
	"./build/config/target_config/socmn2"
	"./ci/script"
	"./test"
	"./src/test"
	"./src/src"
)

# 关键词黑名单：单条记录包含任一关键词（不区分大小写）则丢弃该记录
FILTER_KEYWORDS=(
    "1A22"
    "sw72"
    "3372"
	"2331"
	"bs27a"
	"ws63"
	"ws53"
	"hi2132"
	"sw21"
	"1a72"
	"sw39"
	"bs2x"
	"test"
	"socmn2"
	"bs21"
	"bs22"
	"bs26"
	"bs21e"
	"bs21a"
	"bs25"
	"sw22"
	"flashboot_bs27a"
	"bs20"
	"bs21e"
	"md5 versions"
	"nfc"
	"2113"
	"WS75"
	"1a71"
	"2827"
	"1A21"
)

SINCE="${TARGET_DATE} 00:00:00"
UNTIL="${TARGET_DATE} 23:59:59"

is_denied() {
    local repo="$1"
    for denied in "${DENY_LIST[@]}"; do
        case "$repo" in
            "$denied"|"$denied"/*)
                return 0
                ;;
        esac
    done
    return 1
}

clean_subject() {
    local subject="$1"
    echo "$subject" | sed -E \
        -e 's/.*[Dd][Ee][Ss][Cc][Rr][Ii][Pp][Tt][Ii][Oo][Nn]:[[:space:]]*//' \
        -e 's/[[:space:]]*[Dd][Tt][Ss](\/[Aa][Rr])?:[[:space:]]*[^[:space:]].*//' \
        -e 's/[[:space:]]*[Tt][Ee][Aa][Mm]([^:]*)?:[[:space:]]*[^[:space:]].*//' \
        -e 's/[[:space:]]*[Tt][Ii][Cc][Kk][Ee][Tt][Nn][Oo]:[[:space:]]*[^[:space:]].*//' \
        -e 's/[[:space:]]*[Cc][Hh][Aa][Nn][Gg][Ee][Ii][Dd]:[[:space:]]*[^[:space:]].*//' \
        -e 's/[[:space:]]*[Ii][Ss][Ss][Uu][Ee]:[[:space:]]*[^[:space:]].*//' \
        -e 's/[[:space:]]*[Ff][Ee][Aa][Tt][Uu][Rr][Ee] or [Bb][Uu][Gg][Ff][Ii][Xx]:[[:space:]]*[^[:space:]].*//' \
        -e 's/^[[:space:]]*//' \
        -e 's/[[:space:]]*$//'
}

contains_keyword() {
    local text="$1"
    for kw in "${FILTER_KEYWORDS[@]}"; do
        echo "$text" | grep -qi "$kw" && return 0
    done
    return 1
}

first_repo=true
while IFS= read -r -d '' git_entry; do
    repo_path=$(dirname "$git_entry")
    is_denied "$repo_path" && continue

    if git -C "$repo_path" rev-parse --git-dir >/dev/null 2>&1; then
        commits=$(git -C "$repo_path" log --since="$SINCE" --until="$UNTIL" --pretty=format:"%s")
        [ -z "$commits" ] && continue

        cleaned_lines=$(echo "$commits" | while IFS= read -r line; do
            clean_subject "$line"
        done | awk '!seen[$0]++')
        [ -z "$cleaned_lines" ] && continue

        line_count=$(echo "$cleaned_lines" | wc -l)
        if [ "$line_count" -eq 1 ] && [ "$(echo "$cleaned_lines")" = "[Hisilicon_sh] 上社区Copyright整改" ]; then
            continue
        fi

        output_lines=""
        while IFS= read -r line; do
            if ! contains_keyword "$line"; then
                output_lines="${output_lines}${line}"$'\n'
            fi
        done <<< "$cleaned_lines"
        output_lines=$(echo -n "$output_lines")
        [ -z "$output_lines" ] && continue

        if [ "$first_repo" = true ]; then
            echo "Automated update: sync resources from jsapp, tools, software/sdk, docs"
            echo
            first_repo=false
        fi
        case "$repo_path" in
            .)
                echo "./src"
                ;;
            ./test/*)
                echo "$repo_path"
                ;;
            *)
                echo "$repo_path" | sed -e 's|^\./|./src/|'
                ;;
        esac
        echo "$output_lines"
        echo
    fi
done < <(find . -name ".git" -print0)