import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls';
import * as THREE from 'three';
import './styles/index.css';
import images from './assets/index';
import Track from './Track';
import { vec4 } from 'gl-matrix';
import VERTEX_SHADER from './shaders/vertex.glsl';
import FRAG_SHADER from './shaders/fragment.glsl';
import { Vector4 } from 'three';

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

function createSpotlights(scene) {
  var color = 0xFFFFFF;
  var intensity = 5;
  var distance = 10;
  var angle = Math.PI/7;

  new Array(6).fill('').forEach((item, i) => {
    var spotlight = new THREE.SpotLight(color, intensity, distance, angle);
    var value = i % 2 === 0 ? 10:-10

    spotlight.position.set(
      i < 2 ? value : 0,
      i >= 2 && i < 4 ? value : 0,
      i >= 4 ? value : 0
    );
    scene.add( spotlight );

    // scene.add( new THREE.SpotLightHelper( spotlight ));
  });
}

function init() {
  let planetsT = Array(planets.length).fill(0);

  let up = new THREE.Vector3();
  let axis = new THREE.Vector3();

  let controlConfig = {
    isAnimationStopped: false,
    animationSpeed: 0.05,
    planetFocused: null
  };
  let inverseSpeed = 1 / (controlConfig.animationSpeed);

  const camera = new THREE.PerspectiveCamera(
    45,
    window.innerWidth / window.innerHeight,
    1,
    5000
  );
  camera.position.set(-10, 30, 50);

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

  const paths = planets.map(({track} )=> {
    return new Track(...track);
  })

  const trackMeshes = paths.map((trackClass) => {
    let geometry = new THREE.TubeBufferGeometry(trackClass, pathSegments, tubeRadius, radiusSegments, closed);
    let trackMesh = new THREE.Mesh(geometry, trackMaterial);
    scene.add(trackMesh);
    
    return trackMesh;
  })

  // TEXTURES
  const loader = new THREE.TextureLoader();

  const sunTexture = loader.load(images['sun']);
  const sunMaterial = new THREE.MeshStandardMaterial({map: sunTexture})

  let geometry = new THREE.SphereGeometry(1, 32, 32);
  const sunMesh = new THREE.Mesh(geometry, sunMaterial);
  sunMesh.position.set(0, 0, 0);
  sunMesh.scale.setScalar(4);
  scene.add(sunMesh);

  const textures = planets.map((planet) => loader.load(images[planet.path]));
  
  // uniform
  const lightPosition = new vec4.fromValues(0.5, 0.4, 1.0, 0.0);

  const lightAmbient = new vec4.fromValues(1.0, 1.0, 1.0, 1.0);
  const lightDiffuse = new vec4.fromValues(0.8, 0.8, 0.8, 1.0);
  const lightSpecular = new vec4.fromValues(0.8, 0.8, 0.8, 1.0);

  const materialAmbient = new vec4.fromValues(1.0, 1.0, 1.0, 1.0);
  const materialDiffuse = new vec4.fromValues(0.3, 0.3, 0.3, 1.0);
  const materialSpecular = new vec4.fromValues(0.3, 0.3, 0.3, 1.0);
  const materialShininess = 80.0;

  let ambientProduct = vec4.create();
  let diffuseProduct = vec4.create();
  let specularProduct = vec4.create();

  vec4.mul(ambientProduct, lightAmbient, materialAmbient);
  vec4.mul(diffuseProduct, lightDiffuse, materialDiffuse);
  vec4.mul(specularProduct, lightSpecular, materialSpecular);

  // const materials = textures.map(
  //   (texture) =>
  //     new THREE.MeshStandardMaterial({
  //       map: texture,
  //     })
  // );

  // console.log(THREE.ShaderLib['basic'])

  const materials = textures.map(
    (texture) => {
      let material = new THREE.ShaderMaterial({
        uniforms: {
          Ka: { value: new THREE.Vector4(1.0, 1.0, 1.0, 1.0)},
          Kd: { value: new THREE.Vector4(1.0, 1.0, 1.0, 1.0)},
          Ks: { value: new THREE.Vector4(1.0, 1.0, 1.0, 1.0)},
          LightIntensity: { value: new THREE.Vector4(0.8, 0.8, 0.8, 1.0) },
          LightPosition: { value: lightPosition },
          Shininess: { value: materialShininess },

          'map': { type: 't', value: null },
          'time': { value: 0.0 }
        },
        vertexShader: VERTEX_SHADER,
        fragmentShader: FRAG_SHADER,
      })

      material.uniforms.map.value = texture;

      return material;
    }
  );

  console.log(materials[0].uniforms)

  let mesh = new THREE.Mesh(new THREE.BoxGeometry(1, 1, 1),materials[0])
  mesh.position.x = 2
  scene.add(mesh)

  const meshes = materials.map((material, i) => {
    let geometry = new THREE.SphereGeometry(2, 32, 32);
    let mesh = new THREE.Mesh(geometry, material);
    mesh.name = planets[i].name

    let t = planetsT[i]
    const {x, y, z} = paths[i].getPoint(t);
    mesh.position.set(x, y, z);
    mesh.scale.setScalar(planets[i].scale);

    scene.add(mesh)
    return mesh;
  });

  // const meshGroups = meshes.map((mesh, i) => {
  //   let meshGroup = new THREE.Group();
  //   meshGroup.name = planets[i].name
  //   meshGroup.add(mesh);
  //   scene.add(meshGroup);
  //   return meshGroup;
  // });

  // stars
  const starts = Array({length: 500}, () => ({
    x: Math.random(),
    y: Math.random()
  }))

  // LIGHTING
  const light = new THREE.PointLight('white', 1.25);
  light.position.set(0, 0, 0);
  scene.add(light);

  createSpotlights(scene);

  // Lighting
	var luzAmbiente = new THREE.AmbientLight(0x404040);
	scene.add(luzAmbiente)

	renderer.shadowMapSoft = true;


  // HELPERS
  // scene.add(new THREE.PointLightHelper(light, 1));
  // scene.add(new THREE.GridHelper(65, 65));

  const controls = new OrbitControls(camera, renderer.domElement);
  controls.update();

  document.body.appendChild(renderer.domElement);

  let raycaster = new THREE.Raycaster();
  let mouse = new THREE.Vector2();

  function onDocumentClick(event) {
    const rect = renderer.domElement.getBoundingClientRect();
    mouse.x = ((event.clientX - rect.left) / (rect.width - rect.left)) * 2 - 1;
    mouse.y = - ((event.clientY - rect.top) / (rect.bottom - rect.top)) * 2 + 1;

    raycaster.setFromCamera(mouse, camera);
    
    // for(let i = 0; i < meshGroups.length; ++i) {
    //   let obj = meshes[i];
    //   let intersects = raycaster.intersectObjects([obj]);

    //   console.log(intersects)
    //   if(intersects.length > 0) {
    //     console.log(planets[i].name)
    //   }
    // }
    let intersects = raycaster.intersectObjects(meshes);
    if(intersects.length > 0) {
      let picked = intersects[0].object;

      console.log(picked.name);
      updateInfo(picked.name);
    } else {
      intersects = raycaster.intersectObject(sunMesh)
      // console.log(intersects)
      updateInfo(intersects.length ? '太阳': '');
    }

    // console.log('click');
  }

  document.addEventListener('click', onDocumentClick, false);

  renderer.render(scene, camera);

  return {
    render: function () {
      const delta = clock.getDelta();
      // meshGroups.forEach((group, i) => {
      //   group.rotation.y += delta * planets[i].revolution;
      // })

      sunMesh.rotation.y += delta * 0.05;

      // 公转
      paths.forEach((path, i) => {
        let mt = planetsT[i];
        let pt = path.getPoint(mt);
        let tangent = path.getTangent(mt).normalize();
        meshes[i].position.set(pt.x, pt.y-0.2, pt.z);

        // calculate the axis to rotate around
        axis.crossVectors(up, tangent).normalize();

        // calcluate the angle between the up vector and the tangent
        let radians = Math.acos(up.dot(tangent));

        mt = (mt >= 1) ? 0 : mt += (0.05 * planets[i].revolution / inverseSpeed);
        planetsT[i] = mt;

        // materials[i].uniforms.time.value = radians;
      })
      
      
      // 自转
      meshes.forEach((mesh, i) => {
        let addValue = delta * planets[i].rotation;
        // materials[i].uniforms.time.value = THREE.Math.radToDeg(mesh.rotation.y) % 360;
        mesh.rotation.y += addValue;
      })

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
const $info = document.querySelector('.info-box');
const $header = $info.querySelector('h1');

function updateInfo(message) {
  if(message === '') {
    $info.classList.remove('show');
  } else {
    $info.classList.add('show');
  }
  $header.innerHTML = message;
}


// setTimeout(() => {
//   updateInfo('HHH')
// }, 1000);

// setTimeout(() => {
//   updateInfo('HHH')
// }, 2000);