# 综述

## 预装环境

* Windows 10
* Visual Studio 2017或2019
* CMake

## 本实验使用的框架

* GLFW和配套的GLAD（OpenGL 3.3 Loader）。
* Luna（自己开发的一个图形应用程序框架，这里只使用了数学运算的部分）。

## 编译方法

1. 在所有预装环境安装完毕以后，双击`gen_vs2017.bat`或者`gen_vs2019.bat`。

2. 使用Visual Studio打开`Solution/HW.sln`。

3. 编译`Exp1`，依赖库`Runtime`、`glfw`和`glad`会一起编译。

# 编程作业1：OpenGL基本程序 (Code/Exp1)

## 要求

- 使用glfw和glew搭建OpenGL基本框架
  - 也可以使用glut或者Qt等其他框架
- 实现基本模型绘制
  - 绘制立方体、圆球或者茶壶
- 实现相机控制
  - 支持从不同视角和距离观察模型

## 完成情况

- 使用glfw搭建了OpenGL基本框架

- 实现了立方体的绘制

- 实现了相机控制

  - WSADQE控制相机的前后左右上下移动。
  - 鼠标左键按住以后可以旋转相机，以看向不同的位置。

  