/*
 * NDPluginEdge.cpp
 *
 * Image processing plugin
 * Author: Keith Brister
 *
 * Created November 10, 2014
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <epicsString.h>
#include <epicsMutex.h>
#include <iocsh.h>

#include "NDArray.h"
#include "NDPluginEdge.h"
#include <epicsExport.h>

#include <opencv2/opencv.hpp>

static const char *driverName="NDPluginEdge";


/** Color Mode to CV Matrix

    NDInt8,     Signed 8-bit integer        
    NDUInt8,    Unsigned 8-bit integer        
    NDInt16,    Signed 16-bit integer        
    NDUInt16,   Unsigned 16-bit integer        
    NDInt32,    Signed 32-bit integer        
    NDUInt32,   Unsigned 32-bit integer        
    NDFloat32,  32-bit float                
    NDFloat64   64-bit float                

    NDColorModeMono,    Monochromatic image                                                        
    NDColorModeBayer,   Bayer pattern image, 1 value per pixel but with color filter on detector
    NDColorModeRGB1,    RGB image with pixel color interleave, data array is [3, NX, NY]        
    NDColorModeRGB2,    RGB image with row color interleave, data array is [NX, 3, NY]                
    NDColorModeRGB3,    RGB image with plane color interleave, data array is [NX, NY, 3]        
    NDColorModeYUV444,  YUV image, 3 bytes encodes 1 RGB pixel                                        
    NDColorModeYUV422,  YUV image, 4 bytes encodes 2 RGB pixel                                        
    NDColorModeYUV411   YUV image, 6 bytes encodes 4 RGB pixels                                        


    NDArray         OpenCV
    =========       ==========
    NDInt8          CV_8S
    NDUInt8         CV_8U
    NDInt16         CV_16S
    NDUInt16        CV_16U
    NDInt32         CV_32S
    NDUInt32        CV_32U
    NDFloat32       CV_32F
    NDFloat64       CV_64F

    ND_BayerPatern  OpenCV
    ==============  ======
    NDBayer_RGGB    RG
    NDBayer_GBRG    GB
    NDBayer_GRGB    GR
    NDBayer_BGGR    BG
*/


/** Callback function that is called by the NDArray driver with new NDArray data.
  * Does image processing.
  * \param[in] pArray  The NDArray from the callback.
  */
void NDPluginEdge::processCallbacks(NDArray *pArray)
{
  /* This function does array processing.
   * It is called with the mutex already locked.  It unlocks it during long calculations when private
   * structures don't need to be protected.
   */
  NDArray *pScratch=NULL;
  NDArrayInfo arrayInfo;

  int i, j;
  unsigned int numRows, rowSize;
  unsigned char *inData, *outData;
  int edge1;
  int edge1Found;
  int edge2;
  int edge2Found;
  double lowThreshold;
  double thresholdRatio;

  static const char* functionName = "processCallbacks";


  /* Call the base class method */
  NDPluginDriver::processCallbacks(pArray);

  getDoubleParam( NDPluginEdgeLowThreshold,   &lowThreshold);
  getDoubleParam( NDPluginEdgeThresholdRatio, &thresholdRatio);

  /* Do the computationally expensive code with the lock released */
  this->unlock();
  
  /* make the array something we understand */
  this->pNDArrayPool->convert( pArray, &pScratch, NDUInt8);

  pScratch->getInfo(&arrayInfo);

  rowSize = pScratch->dims[arrayInfo.xDim].size;
  numRows = pScratch->dims[arrayInfo.yDim].size;

  cv::Mat img = cv::Mat( numRows, rowSize, CV_8UC1);

  cv::Mat detected_edges;


  // Initialize the output data array
  //
  inData  = (unsigned char *)pScratch->pData;
  outData = (unsigned char *)img.data;
  memcpy( outData, inData, arrayInfo.nElements * sizeof( *inData));

  try {
    // As suggested in the openCV examples, first slightly blur the image
    //
    cv::blur( img, detected_edges, cv::Size(3,3));
  }
  catch( cv::Exception &e) {
    const char* err_msg = e.what();

    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s cv::blur exception:  %s\n", 
        driverName, functionName, err_msg);
    this->lock();
    return;
  }

  try {
    //
    // Here is the edge detection routine.
    //
    cv::Canny( detected_edges, detected_edges, lowThreshold, thresholdRatio * lowThreshold, 3);
  }
  catch( cv::Exception &e) {
    const char* err_msg = e.what();

    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s::%s cv::Canny exception:  %s\n", 
        driverName, functionName, err_msg);
    this->lock();
    return;
  }

  // Take the lock again since we are accessing the parameter library and 
  // these calculations are not time consuming
  this->lock();
  
  // Try to find the top pixel
  j = rowSize/2;
  edge1Found = 0;
  edge2Found = 0;
  outData = (unsigned char *)detected_edges.data;
  for( i=0; (unsigned int)i<numRows; i++) {
    if( *(outData + i*rowSize + j) != 0) {
      edge1Found = 1;
      break;
    }
  }
  edge1 = i;

  setIntegerParam( NDPluginEdgeTopEdgeFound, edge1Found);
  if( edge1Found)
    setIntegerParam( NDPluginEdgeTopPixel, edge1);

  // Maybe find the bottom pixel
  for( i=numRows - 1; i>=0; i--) {
    if( *(outData + i*rowSize + j) != 0) {
      edge2Found = 1;
      break;
    }
  }

  edge2 = i;
  setIntegerParam( NDPluginEdgeBottomEdgeFound, edge2Found);
  if( edge2Found)
    setIntegerParam( NDPluginEdgeBottomPixel, edge2);


  if( edge1Found && edge2Found && edge1 < edge2) {
    // Both edges found and they are not the same
    //
    setIntegerParam( NDPluginEdgeVerticalFound, 1);
    setDoubleParam( NDPluginEdgeVerticalCenter, (edge1 + edge2)/2.0);
    setIntegerParam(    NDPluginEdgeVerticalSize, edge2 - edge1);
  } else {
    // no edge found
    setIntegerParam( NDPluginEdgeVerticalFound, 0);
  }

  // Find Left pixel
  i = numRows/2;
  edge1Found = 0;
  edge2Found = 0;
  for( j=0; (unsigned int)j<rowSize; j++) {
    if( *(outData + i*rowSize + j) != 0) {
      edge1Found = 1;
      break;
    }
  }
  edge1 = j;

  setIntegerParam( NDPluginEdgeLeftEdgeFound, edge1Found);
  if( edge1Found)
    setIntegerParam( NDPluginEdgeLeftPixel, edge1);


  // Maybe find right pixel
  for( j=rowSize-1; j>=0; j--) {
    if( *(outData + i*rowSize + j) != 0) {
      edge2Found = 1;
      break;
    }
  }
  edge2 = j;

  setIntegerParam( NDPluginEdgeRightEdgeFound, edge2Found);
  if( edge2Found)
    setIntegerParam( NDPluginEdgeRightPixel, edge2);

  if( edge1Found && edge2Found && edge1 < edge2) {
    setIntegerParam( NDPluginEdgeHorizontalFound, 1);
    setDoubleParam( NDPluginEdgeHorizontalCenter, (edge1 + edge2)/2.0);
    setIntegerParam( NDPluginEdgeHorizontalSize, edge2 - edge1);
  } else {
    // no edge found
    setIntegerParam( NDPluginEdgeHorizontalFound, 0);
  }

  int arrayCallbacks = 0;
  getIntegerParam(NDArrayCallbacks, &arrayCallbacks);
  if (arrayCallbacks == 1) {
    inData  = (unsigned char *)detected_edges.data;
    outData = (unsigned char *)pScratch->pData;
    memcpy(outData, inData, arrayInfo.nElements * sizeof( *inData));
    this->getAttributes(pScratch->pAttributeList);
    this->unlock();
    doCallbacksGenericPointer(pScratch, NDArrayData, 0);
    this->lock();
  }

  if (NULL != pScratch)
    pScratch->release();

  callParamCallbacks();
}



/** Constructor for NDPluginEdge; most parameters are simply passed to NDPluginDriver::NDPluginDriver.
  * After calling the base class constructor this method sets reasonable default values for all of the
  * parameters.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] queueSize The number of NDArrays that the input queue for this plugin can hold when
  *            NDPluginDriverBlockingCallbacks=0.  Larger queues can decrease the number of dropped arrays,
  *            at the expense of more NDArray buffers being allocated from the underlying driver's NDArrayPool.
  * \param[in] blockingCallbacks Initial setting for the NDPluginDriverBlockingCallbacks flag.
  *            0=callbacks are queued and executed by the callback thread; 1 callbacks execute in the thread
  *            of the driver doing the callbacks.
  * \param[in] NDArrayPort Name of asyn port driver for initial source of NDArray callbacks.
  * \param[in] NDArrayAddr asyn port driver address for initial source of NDArray callbacks.
  * \param[in] maxBuffers The maximum number of NDArray buffers that the NDArrayPool for this driver is
  *            allowed to allocate. Set this to -1 to allow an unlimited number of buffers.
  * \param[in] maxMemory The maximum amount of memory that the NDArrayPool for this driver is
  *            allowed to allocate. Set this to -1 to allow an unlimited amount of memory.
  * \param[in] priority The thread priority for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  * \param[in] stackSize The stack size for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  */
NDPluginEdge::NDPluginEdge(const char *portName, int queueSize, int blockingCallbacks,
                         const char *NDArrayPort, int NDArrayAddr,
                         int maxBuffers, size_t maxMemory,
                         int priority, int stackSize)
    /* Invoke the base class constructor */
    : NDPluginDriver(portName, queueSize, blockingCallbacks,
                   NDArrayPort, NDArrayAddr, 1, NUM_NDPLUGIN_EDGE_PARAMS, maxBuffers, maxMemory,
                   asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask,
                   asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask,
                   ASYN_MULTIDEVICE, 1, priority, stackSize)
{
   //static const char *functionName = "NDPluginEdge";
    
  createParam( NDPluginEdgeLowThresholdString,     asynParamFloat64,  &NDPluginEdgeLowThreshold);
  createParam( NDPluginEdgeThresholdRatioString,   asynParamFloat64,  &NDPluginEdgeThresholdRatio);
  createParam( NDPluginEdgeVerticalFoundString,    asynParamInt32,    &NDPluginEdgeVerticalFound);
  createParam( NDPluginEdgeTopEdgeFoundString,     asynParamInt32,    &NDPluginEdgeTopEdgeFound);
  createParam( NDPluginEdgeTopPixelString,         asynParamInt32,    &NDPluginEdgeTopPixel);
  createParam( NDPluginEdgeBottomEdgeFoundString,  asynParamInt32,    &NDPluginEdgeBottomEdgeFound);
  createParam( NDPluginEdgeBottomPixelString,      asynParamInt32,    &NDPluginEdgeBottomPixel);
  createParam( NDPluginEdgeVerticalCenterString,   asynParamFloat64,  &NDPluginEdgeVerticalCenter);
  createParam( NDPluginEdgeVerticalSizeString,     asynParamInt32,    &NDPluginEdgeVerticalSize);
  createParam( NDPluginEdgeHorizontalFoundString,  asynParamInt32,    &NDPluginEdgeHorizontalFound);
  createParam( NDPluginEdgeLeftEdgeFoundString,    asynParamInt32,    &NDPluginEdgeLeftEdgeFound);
  createParam( NDPluginEdgeLeftPixelString,        asynParamInt32,    &NDPluginEdgeLeftPixel);
  createParam( NDPluginEdgeRightEdgeFoundString,   asynParamInt32,    &NDPluginEdgeRightEdgeFound);
  createParam( NDPluginEdgeRightPixelString,       asynParamInt32,    &NDPluginEdgeRightPixel);
  createParam( NDPluginEdgeHorizontalCenterString, asynParamFloat64,  &NDPluginEdgeHorizontalCenter);
  createParam( NDPluginEdgeHorizontalSizeString,   asynParamInt32,    &NDPluginEdgeHorizontalSize);
  
  
  /* Set the plugin type string */
  setStringParam(NDPluginDriverPluginType, "NDPluginEdge");
  
  /* Try to connect to the array port */
  connectToArrayPort();
}

/** Configuration command */
extern "C" int NDEdgeConfigure(const char *portName, int queueSize, int blockingCallbacks,
                                 const char *NDArrayPort, int NDArrayAddr,
                                 int maxBuffers, size_t maxMemory,
                                 int priority, int stackSize)
{
    new NDPluginEdge(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr,
                        maxBuffers, maxMemory, priority, stackSize);
    return(asynSuccess);
}

/* EPICS iocsh shell commands */
static const iocshArg initArg0 = { "portName",iocshArgString};
static const iocshArg initArg1 = { "frame queue size",iocshArgInt};
static const iocshArg initArg2 = { "blocking callbacks",iocshArgInt};
static const iocshArg initArg3 = { "NDArrayPort",iocshArgString};
static const iocshArg initArg4 = { "NDArrayAddr",iocshArgInt};
static const iocshArg initArg5 = { "maxBuffers",iocshArgInt};
static const iocshArg initArg6 = { "maxMemory",iocshArgInt};
static const iocshArg initArg7 = { "priority",iocshArgInt};
static const iocshArg initArg8 = { "stackSize",iocshArgInt};
static const iocshArg * const initArgs[] = {&initArg0,
                                            &initArg1,
                                            &initArg2,
                                            &initArg3,
                                            &initArg4,
                                            &initArg5,
                                            &initArg6,
                                            &initArg7,
                                            &initArg8};
static const iocshFuncDef initFuncDef = {"NDEdgeConfigure",9,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
    NDEdgeConfigure(args[0].sval, args[1].ival, args[2].ival,
                       args[3].sval, args[4].ival, args[5].ival,
                       args[6].ival, args[7].ival, args[8].ival);
}

extern "C" void NDEdgeRegister(void)
{
    iocshRegister(&initFuncDef,initCallFunc);
}

extern "C" {
epicsExportRegistrar(NDEdgeRegister);
}
