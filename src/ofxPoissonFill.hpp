//
// ofxPoissonFill.hpp
// Poisson filling shader for OpenFrameworks
// Lingdong Huang 2020
//
// Reference:
// Convolution Pyramids, Farbman et al., 2011
//   (https://www.cse.huji.ac.il/labs/cglab/projects/convpyr/data/convpyr-small.pdf)
// Rendu (https://github.com/kosua20/Rendu)
//

#ifndef poissonfill_h
#define poissonfill_h

#define PF_MAX_LAYERS 12
class PoissonFill{public:
  ofFbo downs[PF_MAX_LAYERS];
  ofFbo ups  [PF_MAX_LAYERS];
  
  ofShader shader;
  int w;
  int h;
  int depth;
  
  /// \brief Initialize Poisson filler and allocate necessary datastructures
  ///
  /// \param _w     input texture width
  /// \param _h     input texture width
  /// \param _depth depth of the image pyramid
  void init(int _w, int _h, int _depth){
    w = _w;
    h = _h;
    depth = min(PF_MAX_LAYERS,_depth);
    
    for (int i = 0; i < depth; i++){
      _w /= 2;
      _h /= 2;
      downs[i].allocate(_w,_h,GL_RGBA);
    }
    for (int i = 0; i < depth; i++){
      _w *= 2;
      _h *= 2;
      ups[i].allocate(_w,_h,GL_RGBA);
    }
    shader = shader2way();
  }
  
  /// \brief Initialize Poisson filler and allocate necessary datastructures
  ///
  /// Depth of the image pyramid is inferred using log2
  ///
  /// \param _w input texture width
  /// \param _h input texture width
  void init(int _w, int _h){
    int _depth = log2(min(_w,_h))-1;
    init(_w,_h,_depth);
  }
  
  /// \brief Process a texture (RGBA)
  ///
  /// \param tex the texture to be processed
  void process(ofTexture& tex){
    int i;
    pass(downs[0],&tex,NULL);
    for (i = 1; i < depth; i++){
      pass(downs[i],&(downs[i-1].getTexture()),NULL);
    }
    pass(ups[0],&(downs[depth-2].getTexture()),&(downs[depth-1].getTexture()));
    
    for (i = 1; i < depth-1; i++){
      pass(ups[i],&(downs[depth-i-2].getTexture()),&(ups[i-1].getTexture()));
    }
    pass(ups[depth-1],&tex,&(ups[depth-2].getTexture()));
  }
  
  /// \brief Get the processed output
  ///
  /// \return the processed texture (reference)
  ofTexture& getTexture(){
    return ups[depth-1].getTexture();
  }
  
  void pass(ofFbo& p, ofTexture* tex1, ofTexture* tex2){
    p.begin();
    ofClear(0,0,0,0);
    shader.begin();
    
    // <!> theoratically we can do boundry testing with w/h
    // but the result looks identical without it,
    // so for the sake of speed this is commented out
    // same with the w/h uniforms/if statement in the shader
    //
    // shader.setUniform1i("w",tex1->getWidth() );
    // shader.setUniform1i("h",tex1->getHeight());
    
    shader.setUniformTexture("unf",*tex1,1);
    if (tex2 != NULL){
      shader.setUniformTexture("fil",*tex2,2);
    }
    shader.setUniform1i("isup",tex2 != NULL);
    ofSetColor(255);
    ofDrawRectangle(0,0,p.getWidth(),p.getHeight());
    shader.end();
    p.end();
  }
  
  
  ofShader shader2way(){
    ofShader sh;
    sh.setupShaderFromSource(GL_FRAGMENT_SHADER, R"(
      #version 120
      uniform sampler2DRect unf;
      uniform sampler2DRect fil;
      // uniform int w;
      // uniform int h;
      uniform bool isup;
                             
      float h1(int i){
        if (i == 0 || i == 4){
          return 0.1507;
        }
        if (i == 1 || i == 3){
          return 0.6836;
        }
        return 1.0334;
      }
      
      float G(int i){
        if (i == 0 || i == 2){
          return 0.0312;
        }
        return 0.7753;
      }
      
      void main(){

        int i = int(gl_FragCoord.y-0.5);
        int j = int(gl_FragCoord.x-0.5);

        if (!isup){

          int x = j * 2;
          int y = i * 2;

          vec4 acc = vec4(0.0,0.0,0.0,0.0);
          for (int dy = -2; dy <= 2; dy++) {
            for (int dx = -2; dx <= 2; dx++) {
              int nx = x + dx;
              int ny = y + dy;

              // if (nx < 0 || nx >= w || ny < 0 || ny >= h){
              //   continue;
              // }

              vec4 col = texture2DRect(unf, vec2(float(nx)+1.0, float(ny)+1.0));

              acc.r += h1(dx+2) * h1(dy+2) * col.r;
              acc.g += h1(dx+2) * h1(dy+2) * col.g;
              acc.b += h1(dx+2) * h1(dy+2) * col.b;
              acc.a += h1(dx+2) * h1(dy+2) * col.a;
            }
          }
          if (acc.a == 0.0){
            gl_FragColor = acc;
          }else{
            gl_FragColor = vec4(acc.r/acc.a,acc.g/acc.a,acc.b/acc.a,1.0);
          }
          
        }else{
          float h2 = 0.0270;

          vec4 acc = vec4(0.0,0.0,0.0,0.0);
          for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
              int nx = j + dx;
              int ny = i + dy;

              // if (nx < 0 || nx >= w || ny < 0 || ny >= h){
              //   continue;
              // }

              vec4 col = texture2DRect(unf, vec2(float(nx)+1.0, float(ny)+1.0));

              acc.r += G(dx+1) * G(dy+1) * col.r;
              acc.g += G(dx+1) * G(dy+1) * col.g;
              acc.b += G(dx+1) * G(dy+1) * col.b;
              acc.a += G(dx+1) * G(dy+1) * col.a;
            }
          }
          for (int dy = -2; dy <= 2; dy++) {
            for (int dx = -2; dx <= 2; dx++) {
              int nx = j + dx;
              int ny = i + dy;
              nx /= 2;
              ny /= 2;
              // if (nx < 0 || nx >= w/2 || ny < 0 || ny >= h/2){
              //   continue;
              // }
              vec4 col = texture2DRect(fil, vec2(float(nx), float(ny)));

              acc.r += h2 * h1(dx+2) * h1(dy+2) * col.r;
              acc.g += h2 * h1(dx+2) * h1(dy+2) * col.g;
              acc.b += h2 * h1(dx+2) * h1(dy+2) * col.b;
              acc.a += h2 * h1(dx+2) * h1(dy+2) * col.a;
            }
          }
          if (acc.a == 0.0){
            gl_FragColor = acc;
          }else{
            gl_FragColor = vec4(acc.r/acc.a,acc.g/acc.a,acc.b/acc.a,1.0);
          }
        }
      }
    )");
    sh.linkProgram();
    return sh;
  }
  
  
};

#endif /* poissonfill_h */
