# Component-graphs

Component-graphs [1] are an extension of the concept of component-trees developed in the image processing domain [1].
Component-trees enable to filter an image by means of the connected components of its *cuts* or *thresholdings*. Component-trees are defined on images having their values in a totally ordered space. Component-graphs extend the component-tree concept and are defined on images having their values in a space which is not totally ordered (i.e. partially ordered). 

This repository contains two folders, each one corresponding to a stand-alone application based on component-graphs.
- The folder [InteractiveColorFiltering](https://github.com/bnaegel/component-graph/tree/master/InteractiveColorFiltering) proposes a graphical interface for experimenting connected based filtering on color images with component-graphs, implementing the ideas exposed in [3]
- The folder [ShapingFiltering](https://github.com/bnaegel/component-graph/tree/master/ShapingFiltering) contains the code implementing the notion on shaping on component-graphs proposed in [4]

The data structures of this code are based on the LibTIM library https://github.com/bnaegel/libtim

[1] P. Salembier, A. Oliveras and L. Garrido. « Anti-Extensive Connected Operators for Image and Sequence Processing ». In : IEEE Transactions on Image Processing 7.4 (1998), p. 555–570.

[2] N. Passat and B. Naegel. « Component-Trees and Multivalued Images: Structural Properties ». In : Journal of Mathematical Imaging and Vision 49.1 (avr. 2014), p. 37–50.

[3] B. Naegel et N. Passat. « Colour Image Filtering with Component-Graphs ». In : 2014 22nd International Conference on Pattern Recognition. Août 2014, p. 1621–1626.

[4] E. Grossiord, B. Naegel, H. Talbot, N. Passat and L. Najman. « Shape-based analysis on component-graphs for multivalued image processing ». In : International Symposium on Mathematical Morphology and Its Applications to Signal and Image Processing. Springer. 2015, p. 446–457.


