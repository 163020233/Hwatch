本文档为生成emmc文件镜像的使用指导

Windows环境制作

1.当前目录适配的是emmc的FAT文件镜像。
2.生成emmc的FAT文件镜像前，请打开mkfatimgtool.py脚本进行相关配置，详细见脚本说明
3.将要烧录的资源文件放到fs目录中，可以有子目录，此目录对应A核系统挂载第一分区的目录，默认为system目录。
4.因为生成FAT镜像的工具是windows平台的工具，生成过程需要在windows平台上进行。
5.执行脚本是python脚本，所以需要预先安装python3软件，建议python3.7及以上的版本。
6.执行python mkfatimgtool.py即可在当前目录生成FAT镜像，即file.bin文件。

Linux环境制作

工具依赖：
    sudo apt-get update
    sudo apt-get install dosfstools
制作过程：

    #创建镜像
    dd if=/dev/zero of=file.bin bs=512 count=8386



    #格式化为 FAT32
    mkfs.vfat -F 32 -S 512 -s 8 file.bin

    

    #挂载和卸载
    mkdir ~/fat_mount_point
    sudo mount -o loop,uid=$(id -u),gid=$(id -g) file.bin ~/fat_mount_point/
    #拷贝资源文件到fat_mount_point目录
    sudo umount ~/fat_mount_point/



    #检查簇大小
    fsck.fat -v file.bin | grep "bytes per cluster"