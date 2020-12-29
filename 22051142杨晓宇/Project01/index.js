import * as THREE from "three";
import {FirstPersonControls} from "three/examples/jsm/controls/FirstPersonControls"
import {GLTFLoader} from "three/examples/jsm/loaders/GLTFLoader"
const scene = new THREE.Scene();
/* 渲染器 */
const renderer = new THREE.WebGLRenderer();
renderer.setClearColor(new THREE.Color(0x000000));
renderer.setSize(window.innerWidth,window.innerHeight);
/* 坐标系 */
const axes = new THREE.AxesHelper(1000);
scene.add(axes);
/* 立方体以及球体 */
const cubeGeometry = new THREE.CubeGeometry(4,4,4);//立方体长宽高
const ballGrometry = new THREE.SphereGeometry(4,20,20);//球体半径
const material = new THREE.MeshBasicMaterial({color:0x00ffff,wireframe:true});//材质
const cube = new THREE.Mesh(cubeGeometry,material);
cube.position.set(0,20,0);
const ball = new THREE.Mesh(ballGrometry,material);
ball.position.set(50,0,0);
scene.add(cube,ball);
/* 读取茶壶模型 */
const loader = new GLTFLoader();
loader.load("./teapot.gltf",gltf=>{
    const obj = gltf.scene;
    obj.scale.set(0.1,0.1,0.1);
    obj.rotateX(-Math.PI/2);
    obj.position.set(0,0,15);
    scene.add(obj);
},null,null,null);
/* 光源 */
const light = new THREE.AmbientLight("#ffffff");
scene.add(light);
/* 摄像机 */
const camera = new THREE.PerspectiveCamera(45,window.innerWidth/window.innerHeight,0.1,1000);
camera.position.set(-30,40,30);
camera.lookAt(scene.position);
document.getElementById("webgl-output").appendChild(renderer.domElement);
 /* 第一人称控件 */
const controls = new FirstPersonControls(camera);
controls.enabled = true;
controls.lookSpeed = 0.02; //鼠标移动查看的速度
controls.movementSpeed = 10; //相机移动速度
controls.noFly = false;
//controls.constrainVertical = true; //约束垂直
controls.verticalMin = 1.0;
controls.verticalMax = 2.0;
controls.lon = 0; //进入初始视角x轴的角度
controls.lat = 0; //初始视角进入后y轴的角度
let clock = new THREE.Clock();
/* 数据更新 */
function update() {

    controls.update(clock.getDelta());

}
controls.handleResize();
/* 循环渲染 */
function animate() {

    requestAnimationFrame(animate);
    renderer.render(scene, camera);
    update();

}
animate();