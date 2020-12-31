# 编程作业 1
本次作业代码基于我的 3D 渲染引擎 [Horizon](https://github.com/harutemp/Horizon)

## 目录
- Source：源代码
- Samples：示例 gif、运行截图 
- Assets：资源文件

## 特性
* 渲染：
    PBR + IBL
	
* 场景载入：
    支持 [glTF 2.0](https://github.com/KhronosGroup/glTF)
	
* 相机控制（按 T 切换 CameraController）：
    * OrbiterCameraController：
    > 按住鼠标右键或者中键然后移动鼠标，控制相机旋转、移动
    > 鼠标滚轮调整相机距离
    * FirstPersonCameraController：
    > 按住鼠标右键移动鼠标，控制相机以第一人称视角旋转
    > W、S、A、D、Q、E 控制相机前后左右上下移动

## 示例
![GIF](/Samples/sample_teapot.gif?raw=true)
![GIF](/Samples/sample_spheres.gif?raw=true)

## 参考
https://learnopengl.com/

## Licenses and acknowledgments
本次作业使用的资源文件来源于：
- HDR文件来自：http://www.hdrlabs.com/sibl/archive
- 球和茶壶模型文件来自：https://github.com/SaschaWillems/Vulkan
- glTF文件来自：https://github.com/KhronosGroup/glTF-Sample-Models
