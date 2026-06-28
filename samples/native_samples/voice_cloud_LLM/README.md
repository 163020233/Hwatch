# voice_cloud_LLM Sample

`voice_cloud_LLM` 是一个端侧直连云端语音大模型的可运行 sample。它复用本地唤醒能力，在唤醒后通过 WebSocket 连接云端服务，把麦克风 PCM 音频流上传到云端，由云端完成语音检测、理解和回复生成，再把云端下发的 PCM 音频在设备侧播放出来。

当前实现定位为端云语音对话链路打通 sample，已实现：

- 本地唤醒触发云端语音对话。
- 端侧直接连接 Coze WebSocket 语音会话接口。
- 麦克风 PCM 上行、云端音频下行、本地播放的完整闭环。
- 异常和结束后释放音频、WebSocket、ASR 资源并回到待唤醒状态。
