The component-graph structure is the natural extension of the component-tree structure developed in the framework of mathematical morphology.
Component-graph stores the inclusion relation between connected-components of all the threshold sets of an image, when the image values are not totally-ordered.

This project presents source code associated to the paper "Colour image filtering with component-graphs" submitted to the ICPR'2014 conference.

Example:
|<img src='http://component-graph.googlecode.com/svn/wiki/images/image.jpeg' width='100' height='100' />|<img src='http://component-graph.googlecode.com/svn/wiki/images/image1.jpeg' width='100' height='100' />|<img src='http://component-graph.googlecode.com/svn/wiki/images/image2.jpeg' width='100' height='100' />|<img src='http://component-graph.googlecode.com/svn/wiki/images/image3.jpeg' width='100' height='100' />| <img src='http://component-graph.googlecode.com/svn/wiki/images/image4.jpeg' width='100' height='100' />|
|:------------------------------------------------------------------------------------------------------|:-------------------------------------------------------------------------------------------------------|:-------------------------------------------------------------------------------------------------------|:-------------------------------------------------------------------------------------------------------|:--------------------------------------------------------------------------------------------------------|
|Image I                                                                                                |<img src='http://latex.codecogs.com/png.latex?X_a%.png' title='X_a' />                                  |<img src='http://latex.codecogs.com/png.latex?X_b%.png' title='X_b' />                                  |<img src='http://latex.codecogs.com/png.latex?X_c%.png' title='X_c' />                                  |<img src='http://latex.codecogs.com/png.latex?X_d=X_f%.png' title='X_d=X_f' />                           |

Hasse diagram of partially ordered set V and final <img src='http://latex.codecogs.com/png.latex?\ddot\Theta%.png' title='\ddot\Theta' />-component-graph:

|<img src='http://component-graph.googlecode.com/svn/wiki/images/algo_V.jpeg' width='100' />|<img src='http://component-graph.googlecode.com/svn/wiki/images/graph.jpeg' width='100' />|
|:------------------------------------------------------------------------------------------|:-----------------------------------------------------------------------------------------|