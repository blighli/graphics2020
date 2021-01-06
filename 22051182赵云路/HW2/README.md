# 编程作业 2
本次作业代码基于我的 3D 渲染引擎 [Horizon](https://github.com/harutemp/Horizon)

## 目录
- Source：源代码
- Samples：示例 gif、运行截图
- Assets：资源文件

## 特性
* 光照模型：
	- Cook-Torrance BRDF 光照模型

* 材质贴图（本次作业仅使用了颜色贴图和发光贴图）：
    - 支持 Albedo 贴图
	- 支持 Metallic Roughness 贴图
	- 支持 Normal 贴图
	- 支持 Emissive 贴图
	- 支持 AO 贴图

* 光源：
	- 使用 IBL 模拟环境光
	- 在太阳中心位置放置一个点光源模拟太阳光照

* 拾取（Picking）：
	- 拾取算法大体上可分为基于 CPU 的方法（如 Ray casting）和基于 GPU 的方法。本次作业中实现了一种基于 GPU 的方法：在帧缓存中额外添加一个颜色附件；绘制时，将所绘制物体的 ID 写入该颜色附件中；当发生鼠标左键单击事件时，读取该颜色附件对应像素的物体 ID，从而获取被拾取的物体。

## 示例
![GIF](./Samples/sample_picking.gif)
![GIF](./Samples/sample_lighting.gif)
![image](./Samples/screenshot01.gif)

## Licenses and acknowledgments
本次作业使用的资源文件来源于：
- HDR文件来自：http://www.hdrlabs.com/sibl/archive
- 球模型文件来自：https://github.com/SaschaWillems/Vulkan
