#pragma once

#include "ofMain.h"
#include "ofxPoissonFill.hpp"

class ofApp : public ofBaseApp{
public:
  void setup();
  void update();
  void draw();
  
  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y);
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);
  
  ofFbo fbo;        // input graphics
  PoissonFill pf;   // the poisson filler
  
  ofShader shNorm;  // normal shader to draw fancy input graphics for demo
  ofImage img;      // draw some image to input
  ofIcoSpherePrimitive sphere; // draw a sphere to input
};
