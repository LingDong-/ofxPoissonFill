#include "ofApp.h"
#define WIDTH 512
#define HEIGHT 512
//--------------------------------------------------------------
void ofApp::setup(){
  
  img.load("un_poisson.png"); // load the test image
  
  fbo.allocate(WIDTH,HEIGHT,GL_RGBA);
  
  // initialize the poisson filler
  // depth of pyramid is inferred, alternatively specify
  // it with a third argument, usually you can get away with
  // less than log2 levels
  pf.init(WIDTH,HEIGHT);

  // initialize the normal shader
  // not really related to poisson filling, just for fancy demo
  shNorm.setupShaderFromSource(GL_VERTEX_SHADER, R"(
    #version 120
    varying vec3 vNormal;
    void main(){
      gl_Position = ftransform();
      vNormal = normalize(gl_Normal);
    }
  )");
  shNorm.setupShaderFromSource(GL_FRAGMENT_SHADER, R"(
    #version 120
    varying vec3 vNormal;
    void main(){
      gl_FragColor = vec4(abs(vNormal),1.0);
    }
  )");
  shNorm.linkProgram();
  
}

//--------------------------------------------------------------
void ofApp::update(){
  
}

//--------------------------------------------------------------
void ofApp::draw(){
  
  // draw some fancy graphics to input to poisson fill
  fbo.begin();
  ofPushStyle();
  ofClear(0,0,0,0);
  ofNoFill();
  
  img.draw((float)WIDTH-(float)(ofGetFrameNum()%WIDTH*2),0);
  
  ofPushMatrix();
  ofTranslate(WIDTH/2,HEIGHT/2);
  sphere.rotateDeg( 0.2,0.0,1.0,0.0);
  sphere.rotateDeg(-0.1,1.0,0.0,0.0);
  sphere.setRadius(WIDTH/3);
  sphere.setResolution(0.5);

  ofEnableDepthTest();
  shNorm.begin();
  sphere.drawWireframe();
  shNorm.end();

  ofFill();
  ofSetColor(255,0,0);
  ofPushMatrix();
  ofRotateXDeg( 0.2*(float)ofGetFrameNum());
  ofRotateYDeg(-0.1*(float)ofGetFrameNum());
  ofDrawSphere(
      cos((float)ofGetFrameNum()*0.01)*(float)WIDTH/2,
      sin((float)ofGetFrameNum()*0.01)*(float)WIDTH/2,
      0, WIDTH/20);
  ofPopMatrix();

  ofDisableDepthTest();
  ofPopMatrix();
  ofPopStyle();
  fbo.end();
  // done drawing input

  // process the frame
  pf.process(fbo.getTexture());

  // draw the original image
  fbo.draw(0,0);

  // visualize the image pyramid
  ofPushMatrix();
  ofTranslate(0,HEIGHT);
  for (int i = 0; i < pf.depth; i++){
    pf.downs[i].draw(0,0);
    ofTranslate(pf.downs[i].getWidth(),0);
  }
  ofPopMatrix();
  ofPushMatrix();
  ofTranslate(WIDTH*2,HEIGHT);
  for (int i = pf.depth-2; i >= 0; i--){
    ofTranslate(-pf.ups[i].getWidth(),0);
    pf.ups[i].draw(0,0);
  }
  ofPopMatrix();
  // done visualizing the image pyramid
  
  // draw the output
  pf.getTexture().draw(WIDTH,0);
  
  // track frame/sec
  ofDrawBitmapStringHighlight("FPS: "+ofToString(ofGetFrameRate(),2),10,20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
  
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
  
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
  
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
  
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
  
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
  
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
  
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
  
}
