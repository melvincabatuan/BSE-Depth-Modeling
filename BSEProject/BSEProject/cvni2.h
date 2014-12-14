/**

Copyright 2014 Rafael Muñoz Salinas. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY Rafael Muñoz Salinas ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Rafael Muñoz Salinas OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Rafael Muñoz Salinas.
***/
#ifndef _CvNI2_Header_
#define _CvNI2_Header_
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "OpenNI.h"
#include "PS1080.h"


#include <iostream>
#include <cstring>
#include <boost/graph/graph_concepts.hpp>


class CvNI2 {

    public:
    inline CvNI2() ;
    inline ~CvNI2() ;
    //opens the device (or an oni file)
    inline void open ( std::string onifilepath="" ) throw ( cv::Exception );
    //grans an image
    inline  bool grab() throw ( cv::Exception );
    //retrieves data to user space
    inline  void retrieve ( cv::Mat  &color,cv::Mat &depth ) throw ( cv::Exception );
    //releases the device
    inline  void release() throw ( cv::Exception );
    //indicates if the device is opened
    inline bool isOpen() const {return _isOpen;}
    
    inline double get(int propIdx) { return (getProperty(propIdx));}
    
    static const int INVALID_PIXEL_VAL = 0;
    
    private:
    
    bool _isOpen;
    openni::Device _device;
    openni::VideoStream _depth_stream,_color_stream;
    bool _isColorReady,_isDepthReady;
    openni::VideoFrameRef _depth_frame , _color_frame;
    
    
    double getProperty(int propIdx);
    
    bool readCamerasParam();
    
    double getDepthGeneratorProperty(int propIdx);
    double getImageGeneratorProperty(int propIdx);
    double getCommonProperty(int propIdx);
    
    inline void getDepthMapFromMetaData(const openni::VideoFrameRef& depthMetaData,
                                        cv::Mat& depthMap, int noSampleValue, int shadowValue);
    
    // Distance between IR projector and IR camera (in meters)
    double baseline;
    // Focal length for the IR camera in VGA resolution (in pixels)
    int depthFocalLength_VGA;
    // The value for shadow (occluded pixels)
    int shadowValue;
    // The value for pixels without a valid disparity measurement
    int noSampleValue;
   
    

};
CvNI2::CvNI2() {
    _isOpen=false;
}
CvNI2::~CvNI2() {
    release();
}

bool CvNI2::readCamerasParam() {
    double pixelSize = 0;
    if (_depth_stream.getProperty<double>(XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE, &pixelSize) != openni::STATUS_OK)
    {
        CV_Error(cv::Error::StsError, cv::format("CvNI2::readCamerasParams : Could not read pixel size!\n"));
        return false;
    }
    
    // pixel size @ VGA = pixel size @ SXGA x 2
    pixelSize *= 2.0; // in mm
    
    // focal length of IR camera in pixels for VGA resolution
    int zeroPlanDistance; // in mm
    if (_depth_stream.getProperty(XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE, &zeroPlanDistance) != openni::STATUS_OK)
    {
        CV_Error(cv::Error::StsError, cv::format("CvNI2::readCamerasParams : Could not read virtual plane distance!\n"));
        return false;
    }
    
    if (_depth_stream.getProperty<double>(XN_STREAM_PROPERTY_EMITTER_DCMOS_DISTANCE, &baseline) != openni::STATUS_OK)
    {
        CV_Error(cv::Error::StsError, cv::format("CvNI2::readCamerasParams : Could not read base line!\n"));
        return false;
    }
    
    // baseline from cm -> mm
    baseline *= 10;
    
    // focal length from mm -> pixels (valid for 640x480)
    depthFocalLength_VGA = (int)((double)zeroPlanDistance / (double)pixelSize);
    
    return true;
}

double CvNI2::getProperty( int propIdx )
{
    double propValue = 0;
    
    if( isOpen() )
    {
        int purePropIdx = propIdx & ~cv::CAP_OPENNI_GENERATORS_MASK;
        
        if( (propIdx & cv::CAP_OPENNI_GENERATORS_MASK) == cv::CAP_OPENNI_IMAGE_GENERATOR )
        {
            propValue = getImageGeneratorProperty( purePropIdx );
        }
        else if( (propIdx & cv::CAP_OPENNI_GENERATORS_MASK) == cv::CAP_OPENNI_DEPTH_GENERATOR )
        {
            propValue = getDepthGeneratorProperty( purePropIdx );
        }
        else
        {
            propValue = getCommonProperty( purePropIdx );
        }
    }
    
    return propValue;
}

double CvNI2::getCommonProperty( int propIdx )
{
    double propValue = 0;
    
    switch( propIdx )
    {
            // There is a set of properties that correspond to depth generator by default
            // (is they are pass without particular generator flag). Two reasons of this:
            // 1) We can assume that depth generator is the main one for depth sensor.
            // 2) In the initial vertions of OpenNI integration to OpenCV the value of
            //    flag cv::CAP_OPENNI_DEPTH_GENERATOR was 0 (it isn't zero now).
        case cv::CAP_PROP_OPENNI_GENERATOR_PRESENT :
        case cv::CAP_PROP_FRAME_WIDTH :
        case cv::CAP_PROP_FRAME_HEIGHT :
        case cv::CAP_PROP_FPS :
        case cv::CAP_PROP_OPENNI_FRAME_MAX_DEPTH :
        case cv::CAP_PROP_OPENNI_BASELINE :
        case cv::CAP_PROP_OPENNI_FOCAL_LENGTH :
        case cv::CAP_PROP_OPENNI_REGISTRATION :
            propValue = getDepthGeneratorProperty( propIdx );
            break;
        case cv::CAP_PROP_OPENNI2_SYNC :
            propValue = _device.getDepthColorSyncEnabled();
        case cv::CAP_PROP_OPENNI2_MIRROR:
        {
            bool isMirroring = _color_stream.getMirroringEnabled() && _depth_stream.getMirroringEnabled();
            propValue = isMirroring ? 1.0 : 0.0;
            break;
        }
        default :
            CV_Error( cv::Error::StsBadArg, cv::format("Such parameter (propIdx=%d) isn't supported for getting.\n", propIdx) );
    }
    
    return propValue;
}

double CvNI2::getDepthGeneratorProperty( int propIdx )
{
    double propValue = 0;
    if( !_depth_stream.isValid() )
        return propValue;
    
    openni::VideoMode mode;
    
    switch( propIdx )
    {
        case cv::CAP_PROP_OPENNI_GENERATOR_PRESENT :
            CV_DbgAssert(_depth_stream.isValid());
            propValue = 1.;
            break;
        case cv::CAP_PROP_FRAME_WIDTH :
            propValue = _depth_stream.getVideoMode().getResolutionX();
            break;
        case cv::CAP_PROP_FRAME_HEIGHT :
            propValue = _depth_stream.getVideoMode().getResolutionY();
            break;
        case cv::CAP_PROP_FPS :
            mode = _depth_stream.getVideoMode();
            propValue = mode.getFps();
            break;
        case cv::CAP_PROP_OPENNI_FRAME_MAX_DEPTH :
            propValue = _depth_stream.getMaxPixelValue();
            break;
        case cv::CAP_PROP_OPENNI_BASELINE :
            propValue = baseline;
            break;
        case cv::CAP_PROP_OPENNI_FOCAL_LENGTH :
            propValue = (double)depthFocalLength_VGA;
            break;
        case cv::CAP_PROP_OPENNI_REGISTRATION :
            propValue = _device.getImageRegistrationMode();
            break;
        case cv::CAP_PROP_POS_MSEC :
            propValue = (double)_depth_frame.getTimestamp();
            break;
        case cv::CAP_PROP_POS_FRAMES :
            propValue = _depth_frame.getFrameIndex();
            break;
        default :
            CV_Error( cv::Error::StsBadArg, cv::format("Depth generator does not support such parameter (propIdx=%d) for getting.\n", propIdx) );
    }
    
    return propValue;
}

double CvNI2::getImageGeneratorProperty( int propIdx )
{
    double propValue = 0.;
    if( !_color_stream.isValid() )
        return propValue;
    
    openni::VideoMode mode;
    switch( propIdx )
    {
        case cv::CAP_PROP_OPENNI_GENERATOR_PRESENT :
            CV_DbgAssert( _color_stream.isValid() );
            propValue = 1.;
            break;
        case cv::CAP_PROP_FRAME_WIDTH :
            propValue = _color_stream.getVideoMode().getResolutionX();
            break;
        case cv::CAP_PROP_FRAME_HEIGHT :
            propValue = _color_stream.getVideoMode().getResolutionY();
            break;
        case cv::CAP_PROP_FPS :
            propValue = _color_stream.getVideoMode().getFps();
            break;
        case cv::CAP_PROP_POS_MSEC :
            propValue = (double)_color_frame.getTimestamp();
            break;
        case cv::CAP_PROP_POS_FRAMES :
            propValue = (double)_color_frame.getFrameIndex();
            break;
        default :
            CV_Error( cv::Error::StsBadArg, cv::format("Image generator does not support such parameter (propIdx=%d) for getting.\n", propIdx) );
    }
    
    return propValue;
}

inline void CvNI2::getDepthMapFromMetaData(const openni::VideoFrameRef& depthMetaData, cv::Mat& depthMap, int noSampleValue, int shadowValue)
{
    depthMap.create(depthMetaData.getHeight(), depthMetaData.getWidth(), CV_16UC1);
    depthMap.data = (uchar*)depthMetaData.getData();
    
    cv::Mat badMask = (depthMap == (double)noSampleValue) | (depthMap == (double)shadowValue) | (depthMap == 0);
    
    // mask the pixels with invalid depth
    depthMap.setTo( cv::Scalar::all( CvNI2::INVALID_PIXEL_VAL ), badMask );
}

void CvNI2::open ( std::string onifilepath ) throw ( cv::Exception ) {
    //INIT
    if ( openni::OpenNI::initialize() != openni::STATUS_OK )
        throw cv::Exception ( -1,"OpenNI initilization error",__func__,__FILE__,__LINE__ );
    //OPEN DEVICE
    if ( onifilepath.empty() ) {
        if ( _device.open ( openni::ANY_DEVICE ) != openni::STATUS_OK )
            throw cv::Exception ( -1,"OpenNI could not open device ",__func__,__FILE__,__LINE__ );
	if ( _device.setDepthColorSyncEnabled ( true ) != openni::STATUS_OK )
	  std::cerr<<__func__<<" : WARN: Can't set setDepthColorSyncEnabled:\n";
    } else {
        if ( _device.open ( onifilepath.c_str() ) != openni::STATUS_OK )
            throw cv::Exception ( -1,"OpenNI could not open path :"+onifilepath,__func__,__FILE__,__LINE__ );
    }


    


    //CHECK THAT THERE IS DEPTH SENSOR
    if ( _device.getSensorInfo ( openni::SENSOR_DEPTH ) == NULL )
        throw cv::Exception ( -1,"OpenNI Couldn't find depth sensor" ,__func__,__FILE__,__LINE__ );

    //OPEN DEPTH SENSOR
    if ( _depth_stream.create ( _device, openni::SENSOR_DEPTH ) != openni::STATUS_OK )
        throw cv::Exception ( -1,std::string ( "OpenNI Couldn't create depth stream:" ) +openni::OpenNI::getExtendedError() ,__func__,__FILE__,__LINE__ );

    //CHECK THAT THERE IS COLOR SENSOR
    if ( _device.getSensorInfo ( openni::SENSOR_COLOR ) == NULL )
        throw cv::Exception ( -1,"OpenNI Couldn't find color sensor" ,__func__,__FILE__,__LINE__ );
    //OPEN COLOR SENSOR
    if ( _color_stream.create ( _device, openni::SENSOR_COLOR ) != openni::STATUS_OK )
        throw cv::Exception ( -1,std::string ( "OpenNI Couldn't create color stream:" ) +openni::OpenNI::getExtendedError() ,__func__,__FILE__,__LINE__ );


    //Set depth registration, this is optional in many applications
    if ( onifilepath.empty() ) {
	_depth_stream.setMirroringEnabled(false);
	_color_stream.setMirroringEnabled(false);
        if ( ! _device.isImageRegistrationModeSupported ( openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR ) )
            throw cv::Exception ( -1, "Device does not support registration " ,__func__,__FILE__,__LINE__ );
        if ( _device.setImageRegistrationMode ( openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR ) != openni::STATUS_OK )
            throw cv::Exception ( -1, "Could not set depth registration " ,__func__,__FILE__,__LINE__ );

    }

    //SET DEPTH SENSOR MODE 640x480 1mm
    const openni::SensorInfo& si = _depth_stream.getSensorInfo();
    int idx=-1;
    for ( int i=0; i<si.getSupportedVideoModes().getSize(); i++ )
        if ( si.getSupportedVideoModes() [i].getResolutionX() ==640 &&
                si.getSupportedVideoModes() [i].getResolutionY() ==480 &&
                si.getSupportedVideoModes() [i].getPixelFormat() == openni::PIXEL_FORMAT_DEPTH_1_MM ) {
            idx=i;
            break;

        }
    if ( idx==-1 )
        throw cv::Exception ( -1,"OpenNI Couldn't find mode 640x480:1mm ",__func__,__FILE__,__LINE__ );
    _depth_stream.setVideoMode ( si.getSupportedVideoModes() [idx] );

    //SET COLOR SENSOR MODE 640x480 RGB888
    idx=-1;
    const openni::SensorInfo& si_color= _color_stream.getSensorInfo();
    for ( int i=0; i<si_color.getSupportedVideoModes().getSize(); i++ )
        if ( si_color.getSupportedVideoModes() [i].getResolutionX() ==640 &&
                si_color.getSupportedVideoModes() [i].getResolutionY() ==480 &&
                si_color.getSupportedVideoModes() [i].getPixelFormat() == openni::PIXEL_FORMAT_RGB888 ) {
            idx=i;
            break;

        }
    if ( idx==-1 )
        throw cv::Exception ( -1,"OpenNI Couldn't find mode 640x480:RGB888 ",__func__,__FILE__,__LINE__ );

    _color_stream.setVideoMode ( si_color.getSupportedVideoModes() [idx] );



    //set frame by frame    if reading from file
    if ( !onifilepath.empty() ) {
        if ( _device.getPlaybackControl()->setSpeed ( -1 ) != openni::STATUS_OK )
            throw cv::Exception ( -1,std::string ( "OpenNI Couldn't set -1 speed in playback " ) +openni::OpenNI::getExtendedError() ,__func__,__FILE__,__LINE__ );
    }


    if ( _depth_stream.start() != openni::STATUS_OK )
        throw cv::Exception ( -1,std::string ( "OpenNI Couldn't start depth stream:" ) +openni::OpenNI::getExtendedError() ,__func__,__FILE__,__LINE__ );

    if ( _color_stream.start() != openni::STATUS_OK )
        throw cv::Exception ( -1,std::string ( "OpenNI Couldn't start depth stream:" ) +openni::OpenNI::getExtendedError() ,__func__,__FILE__,__LINE__ );

    
    if( !readCamerasParam() )
    {
        CV_Error(cv::Error::StsError, cv::format("CvCapture_OpenNI2::CvCapture_OpenNI2 : Could not read cameras parameters\n"));
        return;
    }

    _isOpen=true;
}
bool CvNI2::grab() throw ( cv::Exception ) {
    if ( !_isOpen )
        throw cv::Exception ( -1,"Not opened ",__func__,__FILE__,__LINE__ );
    
    

    int changedStreamDummy;
    openni::VideoStream  *pStream [2]= { &_depth_stream,&_color_stream};
    _isColorReady=_isDepthReady=false;
    //wait for two streams to arrive
    while ( ( _isColorReady & _isDepthReady ) == false ) {
        if ( openni::OpenNI::waitForAnyStream ( pStream, 2, &changedStreamDummy, 2000 ) !=openni::STATUS_OK ) {
            std::cerr<< "Wait failed! (timeout is 2000 ms) "<<  openni::OpenNI::getExtendedError() <<std::endl;
            return false;
        }
        //is depth frame ready??
        if ( _depth_stream.readFrame ( &_depth_frame ) ==openni::STATUS_OK )
            _isDepthReady=true;
        if ( _color_stream.readFrame ( &_color_frame ) ==openni::STATUS_OK )
            _isColorReady=true;
    }
    return _isDepthReady&_isColorReady;
}


void CvNI2::retrieve ( cv::Mat  &color,cv::Mat &depth ) throw ( cv::Exception ) {
    if ( !_isOpen )
        throw cv::Exception ( -1,"Not opened ",__func__,__FILE__,__LINE__ );
    if ( ! ( _isDepthReady&_isColorReady ) )
        throw cv::Exception ( -1,"Data is not ready  ",__func__,__FILE__,__LINE__ );
    cv::Mat colorImage ( _color_frame.getHeight() ,_color_frame.getWidth() ,CV_8UC3, ( void* ) _color_frame.getData() );
    cv::cvtColor ( colorImage,color,cv::COLOR_RGB2BGR );
    depth.create ( _depth_frame.getHeight(),_depth_frame.getWidth(),CV_16UC1 );
    getDepthMapFromMetaData(_depth_frame, depth, noSampleValue, shadowValue);
//    memcpy ( depth.ptr<char> ( 0 ),_depth_frame.getData(),_depth_frame.getWidth() *_depth_frame.getHeight() *2 );

}

void CvNI2::release() throw ( cv::Exception ) {

    _depth_stream.stop();
    _depth_stream.destroy();
    _color_stream.stop();
    _color_stream.destroy();
    _device.close();
    openni::OpenNI::shutdown();
    _isOpen=false;
}

#endif
