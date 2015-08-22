ADPluginEdge
============

An EPICS areaDetector plugin to perform edge detection using the
OpenCV Canny function.

Additional information:

- Documentation.

  This routine finds edges in an image using the Canny algorithm (John
  Canny. A computational approach to edge detection. Pattern Analysis
  and Machine Intelligence, IEEE Transactions on, (6):679–698, 1986.)
  The routine is cool if for no other reason than that the primary
  source shows an image of a Dalek before and after edge detection.

  There are two parameters: "LowThreshold" and "ThresholdRatio".  Use
  the low threshold to adjust the sensitivity.  Canny suggests that
  the threshold ratio be "two or three to one".

  To make use of this detector a vertical line is taken midway between
  the left and right edges of the image.  The first edge from the top
  (TopPixel) and the first edge from the bottom (BottomPixel) are
  found TopEdgeFound and BottomEdgeFound.  If the top and bottom edges
  are different then "VerticalFound" is set true.

  Likewise, a horizontal line is drawn midway between the top and
  bottom edges of the image.

- Input PVs

PV                 |  Comment
-------------------|---------
LowThreshold       | Lower values increase the sensitivity of the detector
ThresholdRatio     | Sets the size of the hysteresis of the edge detector (2 to 3 are reasonable)

- Output PVs

PV                 |  Comment
-------------------|---------
BottomEdgeFound    | Indicates that a bottom edge was found (0 = 0, 1 = Yes)
BottomPixel        | Location of the first edge from the bottom measured in pixels from the top edge
HorizontalCenter   | Average of the LeftPixel and RightPixel
HorizontalFound    | Indicates that both a left pixel and a right pixel were found and they are not the same
HorizontalSize     | Difference between RightPixel and LeftPixel
LeftEdgeFound      | Indicates that a left edge was found (0 = No, 1 = Yes)
LeftPixel          | Location of the first edge from the left measured in pixels from the left edge
LowThreshold_RBV   | Read back value for LowThreshold
RightEdgeFound     | Indicates that a right edge was found (0 = No, 1 = Yes)
RightPixel         | Location of the first edge from the right measured in pixels from the left edge
ThresholdRatio_RBV | Read back value for ThresholdRatio
TopEdgeFound       | Indicates that a top edge was found  (0 = No, 1 = Yes)
TopPixel           | Location of the first edge from the top measured in pixels from the top edge
VerticalCenter     | Average of TopPixel and BottomPixel
VerticalFound      | Indicates that both a top pixel and a bottom pixel were found and they are not the same
VerticalSize       | Difference between BottomPixel and TopPixel

- Release notes.
  
  For this implementation the image must be 8 bit monochromatic.
  Fortunately there are some other plugins (ColorConvert and Process)
  that allow you to convert to this format.

  There is a plethora of other functions in openCV that would likely
  be useful as areaDetector plugins.

  Needless to say, you'll need to install opencv (http://opencv.org).
  This code was built using verion 3.0 of the opencv API.
