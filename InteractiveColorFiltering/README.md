# Interactive connected filtering for color images based on component-graphs

This folder provides a graphical application based on Qt for color image filtering based on component-graphs.
This work is described in [1].

## Installation - compilation
This applications has been compiled with Qt 5. To compile, you can either import the file cgraph.pro in QtCreator, or execute *qmake* on this file in order to generate the Makefile, then type *make* to compile.

## Usage

First open a color image (all formats supported by Qt).
#### Patch definition interface
This interfce enables to subdivise the original image in overlapping patches as described in the paper.
You can chose to compute the component-graph on the whole image (division in width and weight=1), but be warned that this may lead to a long computation time depending on the size of the input image.
In order to accelerate the computation, you can subdivise the image into overlapping patches. This way, component-graphs are computed only on these patches, using multi-threading capacities of your CPUs.

#### Color space
The component-graph can be computed on:
-RGB space, using the component-wise ordering.
-HSV space, using component-wise ordering.
-SV space + H, using component-wise ordering on the S (saturation) and V (value) channels, leaving the H (hue) channel unchanged.

#### Ordering 
Component-wise ordering (0,0,0) min -> (255,255,255) max or inverse component-wise ordering (255,255,255) min -> (0,0,0) max

#### Area and contrast
Once the component-graphs are computed, you can interactively perform area and contrast filtering, keeping only components having an area between *area min* and *area max* and contrast (L1 difference between the minimal element and maximal element of the node) between *contrast min* and *contrast max*. If the patch division has been used (i.e. component-graphs have been computed for each patch) border effect may appear, since the absolute value of the filters parameters are applied individually in each patch.

The two sliders below : *adaptive filtering: area* and *adaptive filtering: contrast* enable to perform adaptive filtering, in which a fraction of components defined by the slider (x %) are kept based on the statistic distribution of attributes.
This method is more robust when multiple patches are used, enable to hide the border effects. 

Note that the buttons *adaptive filtering: area* and *adaptive filtering: contrast* are inoperant.

[1] B. Naegel et N. Passat. « Colour Image Filtering with Component-Graphs ». In : 2014 22nd International Conference on Pattern Recognition. Août 2014, p. 1621–1626.
