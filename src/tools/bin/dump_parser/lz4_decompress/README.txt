1. 文件lz4_decompress_freebsd_lz4.exe、lz4_decompress_freebsd_lz4是用于lz4压缩后，导出的挂死文件，的解压缩工具
2. lz4_decompress_freebsd_lz4.exe适用于Windows系统，lz4_decompress_freebsd_lz4适用于Linux系统
3. 使用方式为：
./lz4_decompress_freebsd_lz4 待解压文件 解压之后的文件 如：./lz4_decompress_freebsd_lz4 APP_DTCM_ORIGIN.bin_lz4.bin APP_DTCM_ORIGIN.bin
    解压成功将出现类似如下的打印
    Decompression completed successfully!
    Total blocks: 3
    Total decompressed size: 589824 bytes
    Output file: APP_DTCM_ORIGIN.bin

lz4_decompress_freebsd_lz4.exe 待解压文件 解压之后的文件 如：lz4_decompress_3322.exe APP_DTCM_ORIGIN.bin_lz4.bin APP_DTCM_ORIGIN.bin
    解压成功将出现类似如下的打印
    Input:  APP_DTCM_ORIGIN.bin_lz4.bin
    Output file: APP_DTCM_ORIGIN.bin

注意：此版本的lz4解压工具只适用于lz4版本：huawei/14.2（分支codehub_open/huawei/hisilicon_sh/hisi_trunk/14.2）