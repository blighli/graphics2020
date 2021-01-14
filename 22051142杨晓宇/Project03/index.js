import * as THREE from "three";
import { TDSLoader } from "three/examples/jsm/loaders/TDSLoader"
import { OBJLoader } from "three/examples/jsm/loaders/OBJLoader"
import { STLLoader } from "three/examples/jsm/loaders/STLLoader"
import { GLTFLoader } from "three/examples/jsm/loaders/GLTFLoader"
import { FirstPersonControls } from "three/examples/jsm/controls/FirstPersonControls"
import { OrbitControls } from "three/examples/jsm/controls/OrbitControls"
const scene = new THREE.Scene();
/* 渲染器 */
const renderer = new THREE.WebGLRenderer();
renderer.setClearColor(new THREE.Color(0x000000));
renderer.setSize(window.innerWidth, window.innerHeight);
/* 摄像机 */
const camera = new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.1, 1000);
camera.position.set(-30, 40, 30);
camera.lookAt(scene.position);
document.getElementById("webgl-output").appendChild(renderer.domElement);
/* 纹理加载器 */
const textureLoader = new THREE.TextureLoader();
/* 光源 */
const lights = [];
function createPointLight({ x, y, z, intensity = new THREE.Vector3(1., 1., 1.), ambient = null, diffuse = null, specular = null }) {
    lights.push({
        'lightPosition': new THREE.Vector4(x, y, z, 1),
        'ambientLightIntensity': ambient || new THREE.Vector3(0., 0., 0.),//环境光强度
        'diffuseLightIntensity': diffuse || intensity,//漫反射光强度
        'specularLightIntensity': specular || intensity //镜面反射光强度
    });
};
createPointLight({ x: 10, y: 0, z: 0, intensity: new THREE.Vector3(1., 0., 0.) });
createPointLight({ x: 0, y: 10, z: 0, intensity: new THREE.Vector3(0., 1., 0.) });
createPointLight({ x: 0, y: 0, z: 10, intensity: new THREE.Vector3(0., 0., 1.) });
/* 着色器 */
function createVertexShader() {
    return `out vec3 fColor;
varying vec2 fTexCoord;
uniform vec4 lightPosition[${lights.length}]; // 光源位置
uniform vec3 ambientLightIntensity[${lights.length}], diffuseLightIntensity[${lights.length}], specularLightIntensity[${lights.length}];
uniform vec3 ambientReflectivity, diffuseReflectivity, specularReflectivity;
uniform float shininess;
void main(void) {
    vec3 pColor = vec3(0.);
    for(int i=0;i<${lights.length};i++){
        vec4 light = viewMatrix * lightPosition[i];
        vec3 N = normalize( normalMatrix * normal);
        vec4 pos = modelViewMatrix * vec4(position,1.0);
        vec3 s = normalize(vec3(light - pos));
        vec3 v = normalize(-pos.xyz);
        vec3 r = reflect( -s, N );
        float sDotN = max( dot(s, N), 0.0 );

        vec3 ambient = ambientLightIntensity[i] * ambientReflectivity;
        vec3 diffuse = sDotN * diffuseLightIntensity[i] * diffuseReflectivity;
        vec3 specular = vec3(0.0);

        if(sDotN < 0.0){
            specular = specularLightIntensity[i] * specularReflectivity *
                  pow( max( dot(r,v), 0.0 ), shininess );
        }
        pColor = pColor + ambient + diffuse + specular;
    }
    
    fColor = pColor;
    fTexCoord = uv;

    gl_Position = projectionMatrix * modelViewMatrix * vec4( position, 1.0 );
}
`
}
function createFragmentShader(map = false) {
    return [
        "in vec3 fColor;",
        "varying vec2 fTexCoord;",
        map ? "uniform sampler2D map;" : "",
        "void main(void)",
        "{",
        map ? "vec4 texel = texture2D(map, fTexCoord);" : "",
        map ? "vec3 tc = texel.rgb;" : "",
        map ? "float ta = texel.a;" : "",
        map ? "gl_FragColor = vec4(fColor * tc, ta);" : "gl_FragColor = vec4(fColor, 1.);",
        "}"
    ].join("\n");
}
function createUniforms({ reflectivity = new THREE.Vector3(1., 1., 1.), ambient = null, diffuse = null, specular = null, shininess = 1, map = null }) {
    const positions = [];
    const ambients = [];
    const diffuses = [];
    const speculars = [];
    for (let light of lights) {
        positions.push(light['lightPosition']);
        ambients.push(light['ambientLightIntensity']);
        diffuses.push(light['diffuseLightIntensity']);
        speculars.push(light['specularLightIntensity']);
    }
    let uniforms = {
        'lightPosition': { value: positions },
        'ambientLightIntensity': { value: ambients },//环境光强度
        'diffuseLightIntensity': { value: diffuses },//漫反射光强度
        'specularLightIntensity': { value: speculars },//镜面反射光强度
        'ambientReflectivity': { value: ambient || reflectivity },//环境光反射系数
        'diffuseReflectivity': { value: diffuse || reflectivity },//漫反射系数
        'specularReflectivity': { value: specular || reflectivity },//镜面反射系数
        'shininess': { value: shininess }
    }
    if (map) uniforms['map'] = { value: map };
    return uniforms;
}
/* 坐标系 */
const axes = new THREE.AxesHelper(1000);
scene.add(axes);
/* 立方体以及球体 */
const cubeGeometry = new THREE.CubeGeometry(4, 4, 4);//立方体长宽高
const ballGrometry = new THREE.SphereGeometry(4, 20, 20);//球体半径

var material = new THREE.ShaderMaterial({

    uniforms: createUniforms({ reflectivity: new THREE.Vector3(1., 1., 1.), shininess: 1 }),
    vertexShader: createVertexShader(),
    fragmentShader: createFragmentShader(),

});//着色器材质
const cube = new THREE.Mesh(cubeGeometry, material);
cube.position.set(0, 20, 0);
const ball = new THREE.Mesh(ballGrometry, material);
ball.position.set(0, 0, 0);
scene.add(cube, ball);
/* 读取桌子模型 */
const loader = new TDSLoader();
loader.load("./model/table.3ds", tds => {
    tds.scale.set(0.001, 0.001, 0.001);
    tds.position.set(0, -9.5, 100);
    tds.rotateX(-Math.PI / 2);
    console.log(tds);
    scene.add(tds);
}, null, null, null);
/* 读取茶杯模型 */
const oloader = new OBJLoader();
oloader.load("./model/file.obj", obj => {
    obj.scale.set(0.1, 0.1, 0.1);
    obj.position.set(0, 0, 100);
    scene.add(obj);
    console.log(obj);
}, null, null, null);
let controls = null;
/* 光源 */
const light = new THREE.AmbientLight("#ffffff");
scene.add(light);
/* 轨道控制器 */
function OC() {
    controls = new OrbitControls(camera, renderer.domElement);
}
/* 第一人称控制器 */
function FPC() {
    controls = new FirstPersonControls(camera, renderer.domElement);
    controls.enabled = true;
    controls.lookSpeed = 0.1; //鼠标移动查看的速度
    controls.movementSpeed = 10; //相机移动速度
    controls.lon = 0; //进入初始视角x轴的角度
    controls.lat = 0; //初始视角进入后y轴的角度
}
OC();
document.getElementById("btn-oc").onclick = OC;
document.getElementById("btn-fpc").onclick = FPC;
/* 点击事件 */
const mouse = new THREE.Vector2();
const raycaster = new THREE.Raycaster();
function onMouseClick(event) {

    //将鼠标点击位置的屏幕坐标转换成threejs中的标准坐标

    mouse.x = (event.clientX / renderer.domElement.clientWidth) * 2 - 1
    mouse.y = -(event.clientY / renderer.domElement.clientHeight) * 2 + 1

    // 通过鼠标点的位置和当前相机的矩阵计算出raycaster
    raycaster.setFromCamera(mouse, camera);

    // 获取raycaster直线和所有模型相交的数组集合
    var intersects = raycaster.intersectObjects(scene.children);
    for (let it of intersects) {
        if (controls instanceof OrbitControls) {
            controls.target = it.object.position;
        }
    }
}
window.addEventListener('click', onMouseClick, false);
/* 数据更新 */
let clock = new THREE.Clock();
function update() {

    controls.update(clock.getDelta());

}
/* 循环渲染 */
function animate() {

    requestAnimationFrame(animate);
    renderer.render(scene, camera);
    update();

}
animate();