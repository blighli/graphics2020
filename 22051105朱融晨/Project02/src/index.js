import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls';
import * as THREE from 'three';
import './styles/index.css';
import MERCURY from './assets/mercury.jpg';
import images from './assets/index';
import Track from './Track';

const clock = new THREE.Clock();
const DIS = 4;

const planets = [
  // 图片名称， 位置， 大小， 自转， 公转， 名字
  { path: 'mercury', pos: 0.38, scale: 0.8, rotation:0.2, revolution: 0.5, name: '水星' , track: [15,10]},
  { path: 'venus', pos: 0.72, scale: 0.9, rotation:0.18, revolution: 0.35, name: '金星' , track: [22,15]},
  { path: 'earth', pos: 1, scale: 0.8, rotation:0.15, revolution:0.3, name: '地球' , track: [28, 22]},
  { path: 'mars', pos: 1.52, scale: 1, rotation:0.2, revolution:0.2, name: '火星' , track: [35,28]},
  { path: 'jupiter', pos: 5.2, scale: 0.9, rotation:0.05, revolution:0.05, name: '木星' , track: [46,34]},
  { path: 'saturn', pos: 9.58, scale: 0.5, rotation:0.25, revolution:0.03, name: '土星' , track: [57, 42]},
  { path: 'uranus', pos: 19.14, scale: 1.7, rotation:0.25, revolution:0.02, name: '天王星' , track: [69, 49]},
  // { path: 'moon', pos: 42, scale: 3.5, rotation:0, revolution:1, name: '月球' , track: [,]},
  { path: 'neptune', pos: 30.20, scale: 2.9, rotation:0, revolution:1, name: '海王星' , track: [79, 59]},
  // { path: 'pluto', pos: 60, scale: 1.65, rotation:0.2, revolution:0.005, name: '冥王星' , track: [,]},
];

function init() {
  const camera = new THREE.PerspectiveCamera(
    45,
    window.innerWidth / window.innerHeight,
    1,
    5000
  );
  camera.position.set(0, 50, 50);

  const scene = new THREE.Scene();

  const renderer = new THREE.WebGLRenderer({
    antialias: true,
    alpha: true,
  });

  renderer.setClearColor('#000', 1);
  renderer.setSize(window.innerWidth, window.innerHeight);

  // 轨道
  const pathSegments = 128;
  const tubeRadius = 0.01;
  const radiusSegments = 3;
  const closed = true;

  const trackMaterial = new THREE.MeshPhongMaterial({color: '#e8e8e8'});

  let paths = planets.map(({track}) => {
    let trackClass = new Track(...track);
    let geometry = new THREE.TubeBufferGeometry(trackClass, pathSegments, tubeRadius, radiusSegments, closed);
    let trackMesh = new THREE.Mesh(geometry, trackMaterial);
    scene.add(trackMesh);
    
    return trackMesh;
  })

  // TEXTURES
  const loader = new THREE.TextureLoader();

  const textures = planets.map((planet) => loader.load(images[planet.path]));
  const materials = textures.map(
    (texture) =>
      new THREE.MeshStandardMaterial({
        map: texture,
      })
  );

  const meshGroup = new THREE.Group();

  const meshes = materials.map((material) => {
    let geometry = new THREE.SphereGeometry(1, 32, 32);
    return new THREE.Mesh(geometry, material);
  });

  meshes.forEach((mesh, i) => {
    mesh.position.set(planets[i].pos * DIS, 0, 0);
    mesh.scale.setScalar(planets[i].scale);
    // meshGroup.add(mesh);
    // scene.add(mesh)
  });

  const meshGroups = meshes.map((mesh) => {
    let meshGroup = new THREE.Group();
    meshGroup.add(mesh);
    scene.add(meshGroup);
    return meshGroup;
  });



  // let paths = A/

  // stars
  const starts = Array({length: 500}, () => ({
    x: Math.random(),
    y: Math.random()
  }))

  // LIGHTING
  const light = new THREE.PointLight('white', 1.25);
  light.position.set(0, 0, 0);
  scene.add(light);

  // HELPERS
  scene.add(new THREE.PointLightHelper(light, 1));
  scene.add(new THREE.GridHelper(50, 50));

  const controls = new OrbitControls(camera, renderer.domElement);
  controls.update();

  document.body.appendChild(renderer.domElement);

  renderer.render(scene, camera);

  return {
    render: function () {
      const delta = clock.getDelta();
      // meshGroups.forEach((group, i) => {
      //   group.rotation.y += delta * planets[i].revolution;
      // })
      // meshes.forEach((mesh, i) => {
      //   mesh.rotation.y += delta * planets[i].rotation;
      // })

      controls.update();
      renderer.render(scene, camera);
    },
  };
}

function animate() {
  requestAnimationFrame(animate);
  render();
}

const { render } = init();
animate();
