import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls';
import * as THREE from 'three';
import img from './ground.jpg';
import './styles/index.css';

let camera, scene, renderer; 
let cubeCameras;
let pivots;
let balls;

let controls;

const clock = new THREE.Clock();

const size = 2;

function init() {
  camera = new THREE.PerspectiveCamera(45, window.innerWidth/ window.innerHeight, 1, 10000);

  scene = new THREE.Scene();
  scene.fog = new THREE.Fog(0x292a2b, 1000, 10000);

  // 大地
  let texture = new THREE.TextureLoader().load(img);
  texture.minFilter = THREE.LinearFilter;
  texture.needsUpdate = true;

  let groundMaterial = new THREE.MeshPhongMaterial({
    shininess: 80,
    color: 0xffffff,
    specular: 0xffffff,
    map: texture
  });

  let planeGeometry = new THREE.PlaneBufferGeometry(300, 300);
  let ground = new THREE.Mesh(planeGeometry, groundMaterial);
  ground.rotation.x = -Math.PI / 2;
  ground.scale.set(10, 5, 20);
  ground.receiveShadow = true;
  scene.add(ground);

  const option = {
    format: THREE.RGBFormat,
    generateMipmaps: true,
    minFilter:THREE.LinearMipmapLinearFilter
  };

  const cubes = Array.from({length: size}, () => new THREE.WebGLCubeRenderTarget(64, option))
  cubeCameras = cubes.map(cube => new THREE.CubeCamera(1, 1000, cube));

  pivots = Array.from({length: size}, () => new THREE.Object3D())
  pivots.forEach(pivot => scene.add(pivot));

  balls = Array.from({length: size}, (_, i) => {
    let material = new THREE.MeshPhongMaterial({
      shininess: 100,
      color: 0xffffff,
      specular: 0xffffff,
      envMap: cubes[i].texture,
      transparent: true,
      side: THREE.BackSide,
      blending: THREE.AdditiveBlending,
      depthWrite: false
    })

    let geometry = new THREE.SphereGeometry(32, 16, 16);
    let ball = new THREE.Mesh(geometry, material);
    ball.position.set(50 * i + 100, 100, 0);
    ball.castShadow = true;
    ball.receiveShadow = true;
    ball.add(cubeCameras[i]);
    pivots[i].add(ball);

    return ball;
  })

  // 光
  let ambientLight = new THREE.AmbientLight(0x000000);
  scene.add(ambientLight);

  let pointLight = new THREE.PointLight(0xffaa00, 1, 5000);
  pointLight.position.set(0, 0, 0);
  scene.add(pointLight);

  let sunLight = new THREE.SpotLight(0xffffff, 0.3, 0, Math.PI / 2);
  sunLight.position.set(1000, 2000, 1000);
  sunLight.castShadow = true;
  sunLight.shadow.bias = -0.0003;
  sunLight.shadow.camera.far = 1000;
  sunLight.shadow.camera.near = 100;
  sunLight.shadow.camera.fov = 30;
  scene.add(sunLight);

  renderer = new THREE.WebGLRenderer({
    antialias: true
  });

  renderer.setClearColor(scene.fog.color);
  renderer.setSize(window.innerWidth, window.innerHeight);

  controls = new OrbitControls( camera, renderer.domElement );

  camera.position.set( 100, 100, 100 );
  controls.update();

  document.body.appendChild(renderer.domElement);
}

function animate() {
  requestAnimationFrame(animate);
  const delta = clock.getDelta();
  pivots.forEach(pivot=> {
    pivot.rotateY(0.1 * delta);
  })
  balls.forEach(ball => {
    ball.rotateY(-0.1 * delta)
  })
  cubeCameras.forEach(cubeCamera => {
    cubeCamera.update(renderer, scene);
  })
  controls.update();
  renderer.render(scene, camera)
}

init();
animate();