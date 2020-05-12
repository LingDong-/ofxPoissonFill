![ofxaddons_thumbnail](https://user-images.githubusercontent.com/7929704/81636183-296aa100-93e1-11ea-9356-d9302389b192.png)

# ofxPoissonFill

*Poisson filling shader for [OpenFrameworks](http://openframeworks.cc)*

- Uses GLSL shader
- Uses convolution pyramid algorithm
- Fast
- Header-only

![Snip20200511_9](https://user-images.githubusercontent.com/7929704/81635611-95e4a080-93df-11ea-836e-1323a33ffc92.png)

## Installation

Drop the folder into `OF_ROOT/addons`. Done!

Or even simpler, copy paste `src/ofxPoissonFill.hpp` directly to your own `src` folder.


## Usage

```cpp
PoissonFill pf;

ofTexture tex; 

// prepare your image here...

// allocate necessary datastructures (once)
pf.init(tex.getWidth(), tex.getHeight());

// process the image
pf.process(tex);

// draw processed image
pf.getTexture().draw(0,0);

```

See also [`example`](example)

## Reference

- [Convolution Pyramids, Farbman et al., 2011](https://www.cse.huji.ac.il/labs/cglab/projects/convpyr/data/convpyr-small.pdf)
- [Rendu](https://github.com/kosua20/Rendu)