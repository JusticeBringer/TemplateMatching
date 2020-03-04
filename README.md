# Template Matching

Discover and mark 0-9 digits from an image 

Target image is stored in internal memory \
Every digit image (in bmp format) is stored in internal memory\
Template matching algorithm is applied\
Results are stored in a dynamic array\
Results are sorted in descending order\
Non-maximal detections are deleted\
Remaining results are colored in another image\
The image is saved in external memory.

### Example used to test the source code

Input: 

All digits from 0 to 9 in .bmp format

![Digit 0](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/cifra0.bmp)
![Digit 1](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/cifra1.bmp)
![Digit 2](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/cifra2.bmp)
![Digit 3](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/cifra3.bmp)
![Digit 4](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/cifra4.bmp)
![Digit 5](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/cifra5.bmp)
![Digit 6](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/cifra6.bmp)
![Digit 7](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/cifra7.bmp)
![Digit 8](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/cifra8.bmp)
![Digit 9](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/cifra9.bmp)

And the image where the digits are detected

![Test image](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/test.bmp)

Output:

![Test image](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/imagineaGrayscale.bmp)

Legend:

Digit 0 – Red (255, 0, 0)\
Digit 1 – Yellow (255, 255, 0)\
Digit 2 – Green (0, 255, 0)\
Digit 3 – Cyan (0, 255, 255)\
Digit 4 – Magenta (255, 0, 255)\
Digit 5 – Blue (0, 0, 255)\
Digit 6 – Gray (192,192, 192)\
Digit 7 – Orange (255, 140, 0)\
Digit 8 – Dark magenta (128, 0, 128)\
Digit 9 – Dark red (128, 0, 0)\

Note that not every digit is colored to its color because of the used algorithm.
