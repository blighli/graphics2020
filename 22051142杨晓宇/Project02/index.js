import * as THREE from "three";
import { OrbitControls } from "three/examples/jsm/controls/OrbitControls"
const scene = new THREE.Scene();
/* 渲染器 */
const renderer = new THREE.WebGLRenderer();
renderer.setClearColor(new THREE.Color(0x000000));
renderer.setSize(window.innerWidth, window.innerHeight);
renderer.shadowMap.enabled = true;
/* 摄像机 */
const camera = new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.1, 1000);
camera.position.set(0, 300, 0);
camera.lookAt(scene.position);
document.getElementById("webgl-output").appendChild(renderer.domElement);
/* 网状平面 */
const gridsize = 1000;
const divisions = 25;
const gridHelper = new THREE.GridHelper(gridsize, divisions, new THREE.Color(0x00ff00), new THREE.Color(0x00ff00));
scene.add(gridHelper);

/* 纹理加载器 */
const textureLoader = new THREE.TextureLoader();
/* 着色器 */
const vertexShader =
    `
out vec3 fColor;
varying vec2 fTexCoord;
uniform vec4 lightPosition; // 光源位置
uniform vec3 ambientLightIntensity, diffuseLightIntensity, specularLightIntensity;
uniform vec3 ambientReflectivity, diffuseReflectivity, specularReflectivity;
uniform float shininess;
void main(void) {
    vec4 light = viewMatrix * lightPosition;
    vec3 N = normalize( normalMatrix * normal);
    vec4 pos = modelViewMatrix * vec4(position,1.0);
    vec3 s = normalize(vec3(light - pos));
    vec3 v = normalize(-pos.xyz);
    vec3 r = reflect( -s, N );
    float sDotN = max( dot(s, N), 0.0 );

    vec3 ambient = ambientLightIntensity * ambientReflectivity;
    vec3 diffuse = sDotN * diffuseLightIntensity * diffuseReflectivity;
    vec3 specular = vec3(0.0);

    if(sDotN < 0.0){
        specular = specularLightIntensity * specularReflectivity *
              pow( max( dot(r,v), 0.0 ), shininess );
    }

    fColor = ambient + diffuse + specular;
    fTexCoord = uv;

    gl_Position = projectionMatrix * modelViewMatrix * vec4( position, 1.0 );
}`
const fragmentShader = `
in vec3 fColor;
varying vec2 fTexCoord;
uniform sampler2D map;
void main(void)
{
    vec4 texel = texture2D(map,fTexCoord);
    vec3 tc = texel.rgb;
    float ta = texel.a;
    gl_FragColor = vec4(fColor * tc,ta);
}`

/* 太阳 */
const sunGrometry = new THREE.SphereGeometry(20, 20, 20);//球体半径
let lightPosition = new THREE.Vector4(0., 0., 0., 1.);
const sunMaterial = new THREE.ShaderMaterial({

    uniforms: {
        'lightPosition': { value: lightPosition },
        'ambientLightIntensity': { value: new THREE.Vector3(1., 1., 1.) },//环境光强度
        'diffuseLightIntensity': { value: new THREE.Vector3(1., 1., 1.) },//漫反射光强度
        'specularLightIntensity': { value: new THREE.Vector3(1., 1., 1.) },//镜面反射光强度
        'ambientReflectivity': { value: new THREE.Vector3(1., 1., 1.) },//环境光反射系数
        'diffuseReflectivity': { value: new THREE.Vector3(1., 1., 1.) },//漫反射系数
        'specularReflectivity': { value: new THREE.Vector3(1., 1., 1.) },//镜面反射系数
        'map': { value: textureLoader.load("./img/sun.jpg") },
        'shininess': { value: 1.0 }
    },
    vertexShader: vertexShader,
    fragmentShader: fragmentShader,

});
const sun = new THREE.Mesh(sunGrometry, sunMaterial);
sun.position.set(0, 0, 0);

/* 地球 */
const earthRevolutionRadius = 100;//公转半径
const earthGrometry = new THREE.SphereGeometry(10, 20, 20);//球体半径
const earthMaterial = new THREE.ShaderMaterial({

    uniforms: {
        'lightPosition': { value: lightPosition },
        'ambientLightIntensity': { value: new THREE.Vector3(0., 0., 0.) },//环境光强度
        'diffuseLightIntensity': { value: new THREE.Vector3(1., 1., 1.) },//漫反射光强度
        'specularLightIntensity': { value: new THREE.Vector3(1., 1., 1.) },//镜面反射光强度
        'ambientReflectivity': { value: new THREE.Vector3(1., 1., 1.) },//环境光反射系数
        'diffuseReflectivity': { value: new THREE.Vector3(1., 1., 1.) },//漫反射系数
        'specularReflectivity': { value: new THREE.Vector3(1., 1., 1.) },//镜面反射系数
        'map': { value: textureLoader.load("./img/earth.jpg") },
        'shininess': { value: 10 }
    },
    vertexShader: vertexShader,
    fragmentShader: fragmentShader,

});
const earth = new THREE.Mesh(earthGrometry, earthMaterial);
earth.receiveShadow = true;
earth.castShadow = true;
earth.position.set(earthRevolutionRadius, 0, 0);

/* 月球 */
const moonRevolutionRadius = 25;//公转半径
const moonGrometry = new THREE.SphereGeometry(4, 20, 20);//球体半径
const moonMaterial = new THREE.ShaderMaterial({

    uniforms: {
        'lightPosition': { value: lightPosition },
        'ambientLightIntensity': { value: new THREE.Vector3(0., 0., 0.) },//环境光强度
        'diffuseLightIntensity': { value: new THREE.Vector3(1., 1., 1.) },//漫反射光强度
        'specularLightIntensity': { value: new THREE.Vector3(1., 1., 1.) },//镜面反射光强度
        'ambientReflectivity': { value: new THREE.Vector3(1., 1., 1.) },//环境光反射系数
        'diffuseReflectivity': { value: new THREE.Vector3(1., 1., 1.) },//漫反射系数
        'specularReflectivity': { value: new THREE.Vector3(1., 1., 1.) },//镜面反射系数
        'map': { value: textureLoader.load("./img/moon.jpg") },
        'shininess': { value: 0.1 }
    },
    vertexShader: vertexShader,
    fragmentShader: fragmentShader,

});
const moon = new THREE.Mesh(moonGrometry, moonMaterial);
moon.receiveShadow = true;
moon.castShadow = true;
moon.position.set(moonRevolutionRadius + earthRevolutionRadius, 0, 0);

scene.add(sun, earth, moon);

/* 旋转 */
let earthAngle = 0;//地球公转角度
let moonAngle = 0;//月球公转角度
let earthRotation = 0.05;//地球自转速度
let moonRotation = -0.6 * Math.PI / 180;//月球自转速度
function earthRevolution() {
    let angle = -earthAngle * Math.PI / 180;//逆时针
    let [x, y] = [earthRevolutionRadius * Math.cos(angle), earthRevolutionRadius * Math.sin(angle)];
    earth.position.set(x, 0, y);
    earth.rotateY(earthRotation);//地球自转
    earthAngle += 0.05;
    if (earthAngle >= 360) earthAngle -= 360;
}
function moonRevolution() {
    let angle = -moonAngle * Math.PI / 180;//逆时针
    let [x, y] = [moonRevolutionRadius * Math.cos(angle) + earth.position.x, moonRevolutionRadius * Math.sin(angle) + earth.position.z];
    moon.position.set(x, 0, y);
    moon.rotateY(-moonRotation);//月球自转
    moonAngle += 0.6;
    if (moonAngle >= 360) moonAngle -= 360;
}
/* 轨道控制器 */
const controls = new OrbitControls(camera, renderer.domElement);
/* 点击事件 */
const raycaster = new THREE.Raycaster();
const mouse = new THREE.Vector2();
const textarea = document.getElementById("textarea");
let updateText = () => { textarea.innerHTML = ''; }//设置更新文字

function onMouseClick(event) {

    //将鼠标点击位置的屏幕坐标转换成threejs中的标准坐标

    mouse.x = (event.clientX / renderer.domElement.clientWidth) * 2 - 1
    mouse.y = -(event.clientY / renderer.domElement.clientHeight) * 2 + 1

    // 通过鼠标点的位置和当前相机的矩阵计算出raycaster
    raycaster.setFromCamera(mouse, camera);

    // 获取raycaster直线和所有模型相交的数组集合
    var intersects = raycaster.intersectObjects(scene.children);
    for (let it of intersects) {
        if (it.object === sun) {
            controls.target = sun.position;
            updateText = () => {
                textarea.innerHTML = `
                    <p>太阳</p>
                    <p>x:${sun.position.x}</p>
                    <p>y:${sun.position.z}</p>
                `
            };
        }
        if (it.object === earth) {
            controls.target = earth.position;
            updateText = () => {
                textarea.innerHTML = `
                    <p>地球</p>
                    <p>x:${earth.position.x}</p>
                    <p>y:${earth.position.z}</p>
                    <p>公转角度:${earthAngle}</p>
                `
            };

        }
        if (it.object === moon) {
            controls.target = moon.position;
            updateText = () => {
                textarea.innerHTML = `
                    <p>月球</p>
                    <p>x:${earth.position.x}</p>
                    <p>y:${earth.position.z}</p>
                    <p>公转角度:${moonAngle}</p>
                `
            };
        }
    }
}
window.addEventListener('click', onMouseClick, false);
/* 数据更新 */

function update() {

    controls.update();
    moonRevolution();
    earthRevolution();
    updateText();

}
/* 循环渲染 */
function animate() {

    requestAnimationFrame(animate);
    renderer.render(scene, camera);
    update();

}
animate();