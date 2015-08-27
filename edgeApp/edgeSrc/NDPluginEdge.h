#ifndef NDPluginEdge_H
#define NDPluginEdge_H

#include "NDPluginDriver.h"

/* Output data type */
#define NDPluginEdgeLowThresholdString       "LOW_THRESHOLD"     /* (asynFloat32, r/w) Canny sensitivity                      */
#define NDPluginEdgeThresholdRatioString     "THRESHOLD_RATIO"   /* (asynFloat32, r/w) low threshold * ratio = high threshold   */
#define NDPluginEdgeVerticalFoundString      "VERTICAL_FOUND"    /* (asynInt32,   r/o) 1 we found it, 0 not                      */
#define NDPluginEdgeTopEdgeFoundString       "TOP_EDGE_FOUND"    /* (asynInt32,   r/o) 1 we found it, 0 not                      */
#define NDPluginEdgeTopPixelString           "TOP_PIXEL"         /* (asynInt32,   r/o) index of pixel or -1 if not found        */
#define NDPluginEdgeBottomEdgeFoundString    "BOTTOM_EDGE_FOUND" /* (asynInt32,   r/o) 1 we found it, 0 not                      */
#define NDPluginEdgeBottomPixelString        "BOTTOM_PIXEL"      /* (asynInt32,   r/o) index of pixel or -1 if not found        */
#define NDPluginEdgeVerticalCenterString     "VERTICAL_CENTER"   /* (asynFloat64, r/o) average of vertical positions            */
#define NDPluginEdgeVerticalSizeString       "VERTICAL_SIZE"     /* (asynInt32,   r/o) Differance between top and bottom        */
#define NDPluginEdgeHorizontalFoundString    "HORIZONTAL_FOUND"  /* (asynInt32,   r/o) 1 we found it, 0 we did not              */
#define NDPluginEdgeLeftEdgeFoundString      "LEFT_EDGE_FOUND"   /* (asynInt32,   r/o) 1 of we found it, 0 if not               */
#define NDPluginEdgeLeftPixelString          "LEFT_PIXEL"        /* (asynInt32,   r/o) index of pixel (-1 if not found          */
#define NDPluginEdgeRightEdgeFoundString     "RIGHT_EDGE_FOUND"  /* (asynInt32,   r/o) 1 of we found it, 0 if not               */
#define NDPluginEdgeRightPixelString         "RIGHT_PIXEL"       /* (asynInt32,   r/o) index of pixel (-1 if not found)         */
#define NDPluginEdgeHorizontalCenterString   "HORIZONTAL_CENTER" /* (asynFloat64, r/o) average of horizontal positions          */
#define NDPluginEdgeHorizontalSizeString     "HORIZONTAL_SIZE"   /* (asynInt32,   r/o) difference between left and right        */


/** Does image processing operations.
 */
class NDPluginEdge : public NDPluginDriver {
public:
    NDPluginEdge(const char *portName, int queueSize, int blockingCallbacks, 
                 const char *NDArrayPort, int NDArrayAddr,
                 int maxBuffers, size_t maxMemory,
                 int priority, int stackSize);
    /* These methods override the virtual methods in the base class */
    void processCallbacks(NDArray *pArray);
    
protected:
    
    /* edge sensitivity  */
    int NDPluginEdgeLowThreshold;
    #define FIRST_NDPLUGIN_EDGE_PARAM NDPluginEdgeLowThreshold

    /* threshold ratio (low * ratio = high, ratio is recommended to be 3)   */
    int NDPluginEdgeThresholdRatio;

    /* 1 if vertical edge found (top and bottom found and are different     */
    int NDPluginEdgeVerticalFound;

    /* if we found a top edge                                               */
    int NDPluginEdgeTopEdgeFound;

    /* first edge position from the top  (-1 for not found)                 */
    int NDPluginEdgeTopPixel;

    /* if we found a bottom edge                                            */
    int NDPluginEdgeBottomEdgeFound;

    /* first edge position from the bottom  (-1 for not found)              */
    int NDPluginEdgeBottomPixel;

    /* average of top and bottom                                            */
    int NDPluginEdgeVerticalCenter;

    /* difference between top and bottom                                    */
    int NDPluginEdgeVerticalSize;

    /* 1 if horizontal edit found (left and right found and are different   */
    int NDPluginEdgeHorizontalFound;

    /* 1 if left edge found                                                 */
    int NDPluginEdgeLeftEdgeFound;

    /* first edge position from left (-1 for not found)                     */
    int NDPluginEdgeLeftPixel;

    /* 1 if right edge found                                                */
    int NDPluginEdgeRightEdgeFound;

    /* first edge position from right (-1 for not found)                    */
    int NDPluginEdgeRightPixel;

    /* average of left and right positions                                  */
    int NDPluginEdgeHorizontalCenter;

    /* difference between left and right positions                          */
    int NDPluginEdgeHorizontalSize;

    #define LAST_NDPLUGIN_EDGE_PARAM NDPluginEdgeHorizontalSize

private:

};
#define NUM_NDPLUGIN_EDGE_PARAMS ((int)(&LAST_NDPLUGIN_EDGE_PARAM - &FIRST_NDPLUGIN_EDGE_PARAM + 1))
    
#endif
