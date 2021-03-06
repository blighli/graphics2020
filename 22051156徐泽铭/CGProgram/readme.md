# 编程作业

## 实验环境

- MacOS Mojave 10.14.6
- CMake Version: 3.15.3
- (Apple)Clang 10.0.1


## Build方法：
```bash
mkdir build && cd build
cmake .. && make 
```

## project01 OpenGL基本程序

### 实验要求

- 使用glfw和glew搭建OpenGL基本框架
    - 也可以使用glut或者Qt等其他框架
- 实现基本模型绘制
    - 绘制立方体、圆球或茶壶
- 实现相机控制
    - 支持从不同视角和距离观察模型
    
### 完成情况
- 使用GLFW和GLAD搭建OpenGL基本框架几环境
- 实现立方体的绘制
- 实现相机控制
    - 使用WSAD控制相机移动
    - 使用鼠标进行视角的旋转

### 运行方法
```bash
cd build/Project01
./Project01
```



## project02 Simple SolarSystem

### 实验要求
- 必选特性
   - 1 光照
        - 太阳为光源
   - 2 纹理
        - 使用图片进行纹理映射

- 可选特性
   - 3 使用顶点着色器和片段着色器
     - 自己实现光照效果
   - 4 鼠标选择
     - 点击不同球体显示不同名称
     

### 完成情况
- 太阳为光源：简单在太阳中心作为点光源位置，着色时太阳的表面法线求反。
- 纹理：实现球体的绘制，并在生成求表面顶点及法线信息时进行纹理映射生成纹理坐标
- 使用顶点着色器和片段着色器： 地球和月球使用相同的着色器，因太阳为光源，需要对太阳表面的法线逆操作，太阳使用单独的着色器。
- 光照：实现Phong光照模型
- 视角变换仍可以按Project01中的方法进行操作。

### 运行方法
```bash
# 使用上面的Build方法rebuild项目生成Project02的执行目录
cd build/Projecto02
./Project02

# 适当的使用键盘（如S键进行视角的后移调整）
```





    
## project03 复杂三维模型显示

### 实验要求
- 支持以下模型文件格式中至少1种：
    - OBJ
    - 3DS
    - STL
- 支持多光源的光照效果，使用着色器渲染
- 支持多种视点浏览方式
    - 以模型为中心的平移旋转和缩放
    - 以视点为中心的场景漫游
 
 
 ### 完成情况
 - 借助ASSIMP完成支持OBJ文件的读取
 - 支持多光源的光照效果，使用着色器渲染，场景中包含光源如下：
    - 4个点光源，位置固定
    - 1个方向光
    - 1个聚光灯，绑定在Camera位置
 - 支持多种视点浏览方式
    - 以模型为中心的平移旋转和缩放,操作如下（使用Z键切换至Model变换）：
        1. WSADQE 6个键控制模型的沿3个轴的移动
        2. 鼠标左右上下移动控制模型沿Model坐标系中的Y、X轴旋转，鼠标滚轮控制模型沿Model坐标系中的Z轴旋转
        3. 使用I键控制Zoom In， O键盘控制Zoom Out
    
    - 以视点为中心的场景漫游,操作如下（使用X键切换至相机视角）：
        1.  WSADQE 6个键控制相机沿3个轴移动
        2. 鼠标左右移动控制Camera视点的朝向，鼠标滚轮控制视点的拉近与远离
 
 