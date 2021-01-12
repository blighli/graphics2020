# 编程作业 3
本次作业代码基于我的 3D 渲染引擎 [Horizon](https://github.com/harutemp/Horizon)

## 目录
- Source：源代码
- Samples：示例 gif、运行截图
- Assets：资源文件

## 特性
* 模型载入：
    - 支持 obj
	- 支持 [glTF 2.0](https://github.com/KhronosGroup/glTF)

* 延迟着色：
	- 本次作业实现了基础的延迟着色渲染，支持 Multiple subpasses， 第一个 subpass 将 position、normal、color 渲染到 GBuffer，第二个 subpass 为 full screen quad pass，使用 GBuffer 中的数据进行延迟照明和颜色合成。

* 摄像机控制方式：
	- 使用 OrbiterCameraController 控制相机，以相机的目标点为中心进行旋转、平移和缩放
	- 使用 FirstPersonCameraController 控制相机，以第一视角为中心进行场景漫游

## 示例
![GIF](./Samples/sample_deferred_01.gif)
![GIF](./Samples/sample_deferred_02.gif)

## 参考
https://github.com/SaschaWillems/Vulkan

## Licenses and acknowledgments
本次作业使用的资源文件来源于：
- armored knight 模型文件来自：http://opengameart.org/content/knight-2
- deferred floor 模型文件来自：https://github.com/SaschaWillems/Vulkan
