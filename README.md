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

.\
.\
.\

![Digit 9](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/cifra9.bmp)

And the image where the digits are detected

![Test image](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/test.bmp)

Output:

![Test image](https://github.com/JusticeBringer/TemplateMatching/blob/master/TemplateMatching/date/recunoasterePatternuri/imagineaGrayscale.bmp)
