# 跟手粒子效果表盘

## 需求

- 点击表盘后，粒子向触点汇聚。
- 手指移动时，粒子跟随触点移动。
- 松手后，粒子向外散开并保持显示，不消失。
- 粒子最终回到环境漂浮状态。

## 当前状态

代码已完成初版集成，等待 Windows 环境编译和真机验证。

## 实现方案

- 新增 `ParticleDialView`，作为独立的全屏透明粒子图层。
- 粒子层叠加在现有表盘背景之上、步数和心率等交互控件之下。
- 使用 `UICanvasExt` 绘制粒子和 `Animator` 驱动动画。
- 触摸按下、拖动和释放分别切换汇聚、跟随和散开状态。
- 粒子使用弹簧、阻尼和速度限制，松手后保留并回到环境漂浮状态。
- 默认粒子数量为 48，便于先验证真机性能和视觉效果。

## 代码位置

- `src/application/wearable/nativeapp/nativeui/include/main/dial/ParticleDialView.h`
- `src/application/wearable/nativeapp/nativeui/main/src/dial/ParticleDialView.cpp`
- `src/application/wearable/nativeapp/nativeui/include/clock/MainClockView.h`
- `src/application/wearable/nativeapp/nativeui/clock/src/MainClockView.cpp`

## 生命周期

- 表盘激活：启动或恢复粒子动画。
- 表盘切换离开：暂停粒子动画。
- 主界面暂停：停止粒子动画。
- 表盘销毁：停止动画并释放粒子视图。

## 交互兼容

- 粒子视图不消费触摸事件，左右滑动切卡仍交给上层容器处理。
- 粒子视图位于现有按钮和标签下方，不遮挡步数点击等原有交互。

## 编译验证

状态：待用户执行。

建议目标：

```text
pack_diting_community
```

编译要求：

- 自研代码零编译错误。
- 自研代码零新增告警。
- 如第三方库内部告警导致失败，仅对该组件临时关闭 `-Werror`，并记录 TODO。

## 真机验收

- [ ] 开机后表盘正常显示，粒子处于环境漂浮状态。
- [ ] 点击表盘后粒子向触点汇聚。
- [ ] 拖动时粒子跟随触点移动。
- [ ] 松手后粒子向外散开且不消失。
- [ ] 松手后粒子能自然回到环境漂浮状态。
- [ ] 左右滑动切卡、步数点击等原有交互正常。
- [ ] 快速连续点击和拖动不崩溃。
- [ ] 页面切换离开后粒子动画停止，返回后恢复。
- [ ] 连续操作后无明显内存增长。
- [ ] 真机动画无明显卡顿；如卡顿，优先降低粒子数量。

## 性能降级

如真机出现明显掉帧，按以下顺序调整：

1. 将粒子数量从 48 降低到 32。
2. 关闭每个粒子的抗锯齿。
3. 降低环境漂浮幅度。
4. 降低拖动跟随弹簧强度。

## 回退方式

当前功能位于独立 `feature/particle-dial` 分支。未通过真机验收前不合并到 `develop`；需要回退时切回 `develop` 即可。
