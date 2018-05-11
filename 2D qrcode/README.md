# Computervision

Assisment 2

Read color 2D QR code from images

2D barcodes are becoming very popular. A well-known one is the QR code, which is common in mobile apps. There is a new type of barcode that uses colours to increase the depth of the encoding.  The 2D barcodes may be rotated, scaled and slightly distorted.
This new 2D barcode uses a simple encoding with 8 colours. The colours are black, blue, green, cyan, red, magenta, yellow, and white. These colours can represent three bit words. Notice that there is a relationship with the RGB colour space (e.g., black is 0 (0,0,0), blue is 1 (0,0,255), and green is 2 (0,255,0) etc).
There are 64 characters that can be encoded: the digits 0~9, the lower case a~z, the upper case A~Z, a full stop '.' and space. The encoding table can be found on Stream. The encoding of text is done through the 2D bar using the order Top-left towards Right-bottom (just like scanning an image). Every two squares of colours represents one of the 64 characters (3bits + 3bits). The maximum number of characters in one barcode is 1050. The last square (bottom right) is left unused.
An example of encoding: char 'a' is 1. Therefore, its bit representation is 000001. Dividing this 6 bit group into two groups, we have 000 and 001. The first square should be black (0,0,0) for R=0, G=0 and B=0 and the second square should be blue (0,0,1) for R=0, G=0 and B=255. Each square block of colour represents 3 bits, and a pair of squares represent a single character.
An example of decoding: two squares red and yellow. Red is represented by (255,0,0) and yellow is (255,255,0) in RGB. So red is 100 and yellow is 110. Putting the two together, 100110 (38 in decimal) is 'L' in the table. Therefore, red/yellow is decoded as 'L'.
a) an empty 2D bar showing the concentric circles. b) an example of encoding all possible characters in order.
Two examples of the 2D bar are shown. The concentric circles can be used for aligning the 2D bar when the image is rotated. The centres of the concentric circles can also be used to locate the squares properly when the image is scaled and rotated. The lines between the squares can also be used to rotate the 2D bar to an horizontal position. Both the lines and the concentric circles can be used concurrently to get a better accuracy, so the centre of each squares where the colours will be determined can be scanned properly.
Notes:
1) Use a combination of Hough transforms for straight lines and circles to re-orient the image in case the image is rotated or scaled.
2) the colours present in the barcode are:
black (0,0,0), red (255,0,0), green (0,255,0), blue (0,0,255), white (255,255,255), cyan (0,255,255), magenta (255,0,255) and yellow (255,255,0). Notice that the eight colours give all the possible states for a 3 bit word.
