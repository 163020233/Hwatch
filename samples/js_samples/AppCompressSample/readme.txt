当前工程是配置压缩示例工程，主要是将编译bin和hap压缩成zip文件，再将zip传入板端安装；
1.本地依赖python环境，支持zip，zlib，lz4压缩，其中lz4算法需要pip安装第三方模块
2..\compress_tool.py文件配置PC环境变量，模板例如：CompressTool：xxxxxx\compress_tool.py
3.修改.\entry\hvigorfile.ts编译文件，增加hvigor.buildFinished回调监听和executeCompressCommand压缩执行方法copy到新工程

示例：.\AppCompressSample\entry\hvigorfile.ts
