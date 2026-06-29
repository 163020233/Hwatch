/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: UnzipHelper - public interface for extracting a zip archive into a target directory
 * Created: 2026-05-09
 */

#ifndef UNZIP_HELPER_H
#define UNZIP_HELPER_H

namespace OHOS {

enum UnzipResult {
    UNZIP_OK = 0,
    UNZIP_ERR_OPEN_ZIP = -1,
    UNZIP_ERR_GET_INFO = -2,
    UNZIP_ERR_OPEN_ENTRY = -3,
    UNZIP_ERR_MAKE_DIR = -4,
    UNZIP_ERR_OPEN_OUT = -5,
    UNZIP_ERR_WRITE = -6,
    UNZIP_ERR_READ = -7,
    UNZIP_ERR_NOMEM = -8,
    UNZIP_ERR_PATH = -9,
};

class UnzipHelper {
public:
    static bool Extract(const char *zipPath, const char *destDir);

private:
    static int ExtractToDir(const char *zipPath, const char *destDir);

    UnzipHelper() = delete;
    ~UnzipHelper() = delete;
};

} // namespace OHOS

#endif // UNZIP_HELPER_H
