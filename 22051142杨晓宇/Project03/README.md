# Project03

## 基于 WebGL 和 Three.js 的复杂三维模型显示

本项目已打包，不需要使用 Node，直接进入 index.html 即可访问。  
可能存在资源跨域访问问题，可调整浏览器安全策略或使用 Node 启动服务：  
安装 Node 并执行命令 `npm install` 来下载所需要的包。  
下载完成后，执行 `npm start` 并打开浏览器访问 http://localhost:8081/ 即可浏览效果。

## 已完成的特性

1. 载入 OBJ (file.obj) 和 3DS (table.3ds) 模型文件，并使用环境光源进行展示。
2. 自己编写着色器实现三个点光源的光照效果，光源分别在 x、y、z 轴提供红、绿、蓝三色光。

```javascript
function createPointLight({ x, y, z, intensity = new THREE.Vector3(1., 1., 1.), ambient = null, diffuse = null, specular = null })
```

该方法建立一个以 xyz 为坐标的光源，需要给定全局 RGB 光照强度，也可按照环境光、漫反射、镜面反射分别给出对应的光照强度。

```javascript
createPointLight({ x: 10, y: 0, z: 0, intensity: new THREE.Vector3(1, 0, 0) });
createPointLight({ x: 0, y: 10, z: 0, intensity: new THREE.Vector3(0, 1, 0) });
createPointLight({ x: 0, y: 0, z: 10, intensity: new THREE.Vector3(0, 0, 1) });
```

以上三行分别在 x、y、z 轴各自建立了一个红色、绿色、蓝色光源。

```javascript
function createVertexShader()
```

该方法获取顶点着色器代码。

```javascript
function createFragmentShader(map = false)
```

该方法获取片段着色器代码，map 值用来决定是否需要使用纹理图像。

```javascript
function createUniforms({ reflectivity = new THREE.Vector3(1., 1., 1.), ambient = null, diffuse = null, specular = null, shininess = 1, map = null })
```

该方法得到一个有关材质的 Uniforms 对象，给定材质的 RGB 反射系数和亮度，也可以按照环境光、漫反射、镜面反射分别给出对应的反射系数，如果需要使用纹理映射时则需要使用 map 给出一个二维图像。
最终生成的

```javascript
const material = new THREE.ShaderMaterial({
  uniforms: createUniforms({
    reflectivity: new THREE.Vector3(1, 1, 1),
    shininess: 1,
  }),
  vertexShader: createVertexShader(),
  fragmentShader: createFragmentShader(),
});
const mesh = new THREE.Mesh(geometry, material);
scene.add(mesh);
```

通过以上代码就可以建立一个着色器渲染的网格。

3. 支持多种视点浏览方式，点击“轨道控制器”后以模型为中心，拖动屏幕即可旋转，点击“第一人称控制器”后以视角为中心，使用 WASD 或者 ←↑↓→ 按键来控制摄像机位置，移动鼠标在浏览器窗口内的位置来控制摄像机的视角。

## 效果预览

![三色点光源]("example1.jpg")
![载入模型]("example2.jpg")
