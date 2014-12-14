//
//  VideoWriterv2.cpp
//  BSEprojectLocal
//
//  Created by John Anthony Jose on 12/13/14.
//  Copyright (c) 2014 Anthony. All rights reserved.
//

#include <iostream>
#include "Project.h"
#include "cvni2.h"

using namespace std;

int writeRGBv2(void) {
    try {

        string file = "tummy1";
        string RGBname = file + "RGB.avi";
        string Depthname = file + "Depth.avi";
        string fileDat = file + "Dat.xml";

        CvNI2 camera;
        camera.open();
        cv::Mat color,depth,depth_color;

        bool stop=false;
        while ( camera.grab() && ! stop ) {
            camera.retrieve ( color,depth );
            
            showDepth ( depth,depth_color );
            cv::imshow ( "color",color );
            cv::imshow ( "depth",depth_color );
            
            stop= cv::waitKey ( 10 ) ==27;
            
        cout << "Hello World" << endl;
        };
        cerr<<"Finish"<<endl;
    }catch ( std::exception &ex ) {
        cerr<<ex.what() <<endl;
    }
    return 0;
}

void showDepth (cv::Mat &in_depth, cv::Mat &out_color) {
    double min,max;
    cv::minMaxIdx(in_depth,&min, &max);
    cv::convertScaleAbs(in_depth, out_color, 255 / max);
}


void colorizeDepth ( cv::Mat &in_depth,cv::Mat &out_color ) {
    double min,max;
    cv::minMaxLoc ( in_depth,&min,&max );
    out_color.create ( in_depth.size(),CV_8UC3 );
    assert ( in_depth.isContinuous() && out_color.isContinuous() );
    cv::Vec3b *out_ptr=out_color.ptr<cv::Vec3b> ( 0 );
    uint16_t *in_ptr=in_depth.ptr<uint16_t> ( 0 );
    for ( size_t x=0; x<out_color.size().area(); x++ ) {
        if ( in_ptr[x]==0 ) out_ptr[x]=cv::Vec3b ( 0,0,0 );
        else {
            float v=255.*double ( in_ptr[x] ) /max;
            out_ptr[x][0]=v;
            out_ptr[x][1]=v;
            out_ptr[x][2]=255-v;
        }
    }
}