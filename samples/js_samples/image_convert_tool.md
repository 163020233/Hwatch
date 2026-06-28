### image convert tool简单配置

1. 设置图像转换工具环境变量，本示例中设置如下，路径变量名为ImageTool：

   ![image_convert_tool_path](misc/image_convert_tool_path.png)


2. 在新建工程entry目录下的build.gradle脚本中添加任务如下：

  ![deveco_config](misc/deveco_config.png)

```groovy
project.afterEvaluate {
    project.getTasks().getByName("convertImage").dependsOn(project.getTasks().getByName("generateDebugBinSource"))
    project.getTasks().getByName("assembleDebug").dependsOn(project.getTasks().getByName("convertImage"))
}

task convertImage(type: Exec) {
    workingDir System.getenv("ImageTool")
    buildDir = getBuildDir()
    commandLine "python3","images_tool.py","-p","file","-c","config.xml","-i","${buildDir}/intermediates/","-o","${buildDir}/intermediates/"
}
```

  project.afterEvaluate指定任务添加位置； 
  convertImage为添加的任务；
  ImageTool为第1步设置的图片转换工具所在目录。

