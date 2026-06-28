@echo off

set DUMP_PATH=%~1
set LST_PATH=%~2

if "%DUMP_PATH%"=="" (
    echo Usage: run.bat ^<last word bin^> [lst file]
    goto end
)

echo Available last word types:
echo 1. hi3322-a-liteos
echo 2. hi3322-bt
set /p num=Input number for last word or q for quit (num/q):
set header=
set struct=
if "%num%"=="1" (
    set header=hi3322_a_liteos.h.pre
    set struct=crash_data_save_t
) else if "%num%"=="2" (
    set header=hi3322_bt.h.pre
    set struct=crash_data_save_t
) else if "%num%"=="q" (
    goto end
) else (
    echo The number is invalid!
    goto end
)

if not "%LST_PATH%"=="" (
    set extra_opt=-l "%LST_PATH%"
) else (
    set extra_opt=
)

python dump_parser.py -d headers\preprocess\%header% -s %struct% -i "%DUMP_PATH%" %extra_opt%

:end
