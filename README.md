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

- Release notes.
  
  For this implementation the image must be 8 bit monochromatic.
  Fortunately there are some other plugins (ColorConvert and Process)
  that allow you to convert to this format.

  There is a plethora of other functions in openCV that would likely
  be useful as areaDetector plugins.
