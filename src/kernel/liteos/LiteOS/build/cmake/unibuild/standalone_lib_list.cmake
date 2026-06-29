# 不打包在OS内核.a里的独立lib，名字需与对应lib一致
# 名字带有"test_"前缀的lib会被默认为是独立lib，不会打包到一起
set(STANDALONE_LIB_LIST
    "mxs"
    "ecat"
    "zlix"
    "littlefs"
    "sec"
)
