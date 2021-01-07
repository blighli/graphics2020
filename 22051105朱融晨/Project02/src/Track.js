import * as THREE from 'three';

class Track extends THREE.Curve {
  constructor(xRadius, yRadius) {
    super();
    this.xRadius = xRadius;
    this.yRadius = yRadius;
  }

  getPoint(t) {
    let radians = 2 * Math.PI * t;
    return new THREE.Vector3(this.xRadius * Math.cos(radians), 0 , this.yRadius * Math.sin(radians))
  }  
}

export default Track;