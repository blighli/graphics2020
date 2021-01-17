import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls';
import { TransformControls } from 'three/examples/jsm/controls/TransformControls.js';
import * as THREE from 'three';
import './styles/index.css';
import VERTEX_SHADER from './shaders/vertex.glsl';
import FRAG_SHADER from './shaders/fragment.glsl';
import { STLLoader } from 'three/examples/jsm/loaders/STLLoader';
import ROOF from './assets/roof_03.stl';
import STUMP from './assets/stump_01.stl';
import Stats from 'three/examples/jsm/libs/stats.module';

// 默认旋转视图
let flag = false;

// 是否要移动物体了
let moveFlag = false;

function init() {
  const camera = new THREE.PerspectiveCamera(
    60,
    window.innerWidth / window.innerHeight,
    1,
    3000
  );
  camera.position.set(0, 40, 100);
  camera.lookAt(0, 0, 5);

  camera.target = new THREE.Vector3(0, 0, 0); //设置相机所看的位置

  const scene = new THREE.Scene();

  const material = new THREE.ShaderMaterial({
    uniforms: {
      uShininess: { value: 200.0 },
      pointLights: {
        value: [
          {
            position: new THREE.Vector3(-20, 10, 25),
            ambient: new THREE.Vector3(0.05, 0.05, 0.05),
            diffuse: new THREE.Vector3(0.8, 0.8, 0.8),
            specular: new THREE.Vector3(1.0, 1.0, 1.0),
            constant: 1.0,
            linear: 0.009,
            quadratic: 0.032,
            lightColor: new THREE.Vector3(1.0, 0.0, 0.0),
          },
          {
            position: new THREE.Vector3(20, 10, 25),
            ambient: new THREE.Vector3(0.05, 0.05, 0.05),
            diffuse: new THREE.Vector3(0.8, 0.8, 0.8),
            specular: new THREE.Vector3(1.0, 1.0, 1.0),
            constant: 1.0,
            linear: 0.009,
            quadratic: 0.032,
            lightColor: new THREE.Vector3(0.0, 0.0, 1.0),
          },
        ],
      },
    },
    vertexShader: VERTEX_SHADER,
    fragmentShader: FRAG_SHADER,
  });

  let group = new THREE.Group();
  // load
  const loader = new STLLoader();
  loader.load(ROOF, function (geometry) {
    const mesh = new THREE.Mesh(geometry, material);

    mesh.position.set(-5.5, 10, 6);
    // mesh.rotation.set( -Math.PI / 2, -Math.PI / 2, Math.PI/2 );
    mesh.rotation.set(-Math.PI / 2, 0, 0);
    mesh.scale.set(0.2, 0.2, 0.2);

    // mesh.castShadow = true;
    // mesh.receiveShadow = true;
    group.add(mesh);
    // scene.add(mesh);
  });

  loader.load(STUMP, function (geometry) {
    const mesh = new THREE.Mesh(geometry, material);

    mesh.position.set(1.5, 6, 15);
    mesh.rotation.set(-Math.PI / 2, 0, 0);
    mesh.scale.set(0.2, 0.2, 0.2);

    // mesh.castShadow = true;
    // mesh.receiveShadow = true;
    group.add(mesh);
    // scene.add(mesh);
  });

  scene.add(group);

  const start = new THREE.Vector3(20, 25, 10);
  const middle = new THREE.Vector3(0, 18, 10);
  const end = new THREE.Vector3(-20, 25, 10);

  const curveQuad = new THREE.QuadraticBezierCurve3(start, middle, end);

  const tube = new THREE.TubeGeometry(curveQuad, 50, 0.1, 10, false);
  const mesh = new THREE.Mesh(
    tube,
    new THREE.MeshNormalMaterial({
      opacity: 0.6,
      transparent: true,
    })
  );
  scene.add(mesh);

  const geometry = new THREE.SphereGeometry(1, 32, 32);
  const lightMesh = new THREE.Mesh(
    geometry,
    new THREE.MeshBasicMaterial({
      color: 0x0000ff,
    })
  );
  lightMesh.position.set(20, 25, 10);

  scene.add(lightMesh);

  const lightMesh2 = new THREE.Mesh(
    geometry,
    new THREE.MeshBasicMaterial({
      color: 0xff0000,
    })
  );
  lightMesh2.position.set(-20, 25, 10);
  scene.add(lightMesh2);

  const renderer = new THREE.WebGLRenderer({
    antialias: true,
    alpha: true,
  });

  const stats = new Stats();
  document.body.appendChild(stats.dom);

  renderer.setClearColor('#000 ', 1);
  renderer.setSize(window.innerWidth, window.innerHeight);

  renderer.shadowMapSoft = true;

  // HELPERS
  // scene.add(new THREE.PointLightHelper(light, 1));
  scene.add(new THREE.GridHelper(65, 65));
  const axesHelper = new THREE.AxesHelper(5);
  scene.add(axesHelper);

  const transformControl = new TransformControls(camera, renderer.domElement);
  transformControl.attach(group);
  scene.add(transformControl);

  transformControl.addEventListener('dragging-changed', function (event) {
    moveFlag = !event.value;
    // console.log(event.value, 'moveflag: ', moveFlag);
  });

  let targetRotationX = 0;
  let targetRotationOnPointerDownX = 0;
  let targetRotationY = 0;
  let targetRotationOnPointerDownY = 0;

  let pointerX = 0;
  let pointerY = 0;
  let pointerXOnPointerDown = 0;
  let pointerYOnPointerDown = 0;

  let windowHalfX = window.innerWidth / 2;
  let windowHalfY = window.innerHeight / 2;

  let finalRotationY;

  document.addEventListener(
    'pointerdown',
    function (event) {
      if (event.isPrimary === false) return;
      if(moveFlag === false || flag) return;
      pointerXOnPointerDown = event.clientX - windowHalfX;
      targetRotationOnPointerDownX = targetRotationX;

      pointerYOnPointerDown = event.clientY - windowHalfY;
      targetRotationOnPointerDownY = targetRotationY;

      document.addEventListener('pointermove', onPointerMove, false);
      document.addEventListener('pointerup', onPointerUp, false);
    },
    false
  );

  function onPointerMove(event) {
    if (event.isPrimary === false) return;

    pointerX = event.clientX - windowHalfX;
    pointerY = event.clientY - windowHalfY;

    targetRotationX =
      targetRotationOnPointerDownX + (pointerX - pointerXOnPointerDown) * 0.02;
    targetRotationY =
      targetRotationOnPointerDownY + (pointerY - pointerYOnPointerDown) * 0.02;
  }

  function onPointerUp(event) {
    if (event.isPrimary === false) return;

    document.removeEventListener('pointermove', onPointerMove);
    document.removeEventListener('pointerup', onPointerUp);
  }

  document.addEventListener('wheel', onDocumentMouseWheel, false);

  document.body.appendChild(renderer.domElement);

  renderer.render(scene, camera);

  const controls = new OrbitControls(camera, renderer.domElement);

  function onDocumentMouseWheel(e) {
    let d = typeof e.wheelDelta != 'undefined' ? -e.wheelDelta : e.detail;
    d = 0.1 * (d > 0 ? 1 : -1);

    let cPos = group.scale;
    console.log(cPos);
    if (isNaN(cPos.x) || isNaN(cPos.y) || isNaN(cPos.z)) return;

    let r = cPos.x * cPos.x + cPos.y * cPos.y;
    let sqr = Math.sqrt(r);
    let sqrZ = Math.sqrt(cPos.z * cPos.z + r);

    let nx = cPos.x + (r === 0 ? 0 : (d * cPos.x) / sqr);
    let ny = cPos.y + (r === 0 ? 0 : (d * cPos.y) / sqr);
    let nz = cPos.z + (sqrZ === 0 ? 0 : (d * cPos.z) / sqrZ);

    if (isNaN(nx) || isNaN(ny) || isNaN(nz)) return;

    group.scale.set(nx, ny, nz);
  }

  return {
    render: function () {
      controls.enabled = flag;

      if (flag) {
        controls.update();
      } else {
        if (moveFlag) {
          group.rotation.y += (targetRotationX - group.rotation.y) * 0.05;
          finalRotationY = targetRotationY - group.rotation.x;
          group.rotation.x += finalRotationY * 0.1;
        }
      }

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

const $info = document.querySelector('#header');
const $text = document.querySelector('.box');

const toggle = () => {
  flag = !flag;
  $text.innerHTML = (flag ? '移动视图': '移动模型');
};

$info.addEventListener('click', toggle, false);
