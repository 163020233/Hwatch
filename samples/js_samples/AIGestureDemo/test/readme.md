# 手势模拟数据与测试说明

## 模拟数据文件
- `clench_once.csv` —— 握拳一次
- `clench_twice.csv` —— 握拳两次
- `rotate_once.csv` —— 翻腕一次
- `rotate_twice.csv` —— 翻腕两次

## GestureDemo 使用注意事项

1. **上传手势数据**  
   将本目录下配套的四个手势数据文件上传到板端目录：  
   `/user/ai/gesture/test_data/gesture_signal_type_data`

2. **上传模型文件**  
   将 `/application/wearable/res/ai/gesture/model/lstm.exeom` 模型文件上传到板端目录：  
   `/user/ai/gesture/model`

## AT 命令触发手势事件

使用 `AT+GESTURESINGLETYPE` 命令触发对应手势事件，JS 端接收的 `eventcode` 如下：

| AT 命令 | 手势动作 | JS 接收 eventcode |
|---------|---------|-------------------|
| `AT+GESTURESINGLETYPE=1` | 握拳一次 | `2` |
| `AT+GESTURESINGLETYPE=2` | 握拳两次 | `3` |
| `AT+GESTURESINGLETYPE=3` | 翻腕一次 | `0` |
| `AT+GESTURESINGLETYPE=4` | 翻腕两次 | `1` |
