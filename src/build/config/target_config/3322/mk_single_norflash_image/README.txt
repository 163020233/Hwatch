本文档为生成单nor文件镜像的使用指导，target为3322-single-norflash

1.如果想要生成文件镜像，需要在target处添加配置:'fs_image': True
2.生成镜像前，需要编译好镜像工具（https://github.com/earlephilhower/mklittlefs），将编译好的mklittlefs工具放入本目录中
3.将要烧的资源文件放到fs目录中，镜像脚本还会拷贝一些必要的文件，如gnss_config.bin，请注意避免冲突。