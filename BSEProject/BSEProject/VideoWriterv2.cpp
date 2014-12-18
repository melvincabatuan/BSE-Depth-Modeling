//
//  VideoWriterv2.cpp
//  BSEprojectLocal
//
//  Created by John Anthony Jose on 12/13/14.
//  Copyright (c) 2014 Anthony. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include "Project.h"
#include "cvni2.h"
#include <sys/stat.h>

using namespace std;
using namespace cv;

bool dirExists(const char *path)
{
    struct stat info;
    
    if(stat( path, &info ) != 0)
        return false;
    else if(info.st_mode & S_IFDIR)
        return true;
    else
        return false;
}

bool writeCamParams(std::string fileName, CvNI2 & camera) {
    //Save Camera Settings:
    FileStorage fs(fileName, FileStorage::WRITE);
    
    fs << "depth_frame_res" << "[" << camera.get( CAP_OPENNI_DEPTH_GENERATOR+CAP_PROP_FRAME_WIDTH ) <<
                                      camera.get( CAP_OPENNI_DEPTH_GENERATOR+CAP_PROP_FRAME_HEIGHT ) << "]";
    fs << "depth_max_depth" << camera.get( CAP_OPENNI_DEPTH_GENERATOR+CAP_PROP_OPENNI_FRAME_MAX_DEPTH );
    fs << "depth_fps" << camera.get( CAP_OPENNI_DEPTH_GENERATOR+CAP_PROP_FPS );
    fs << "depth_registration" << camera.get( CAP_OPENNI_DEPTH_GENERATOR+CAP_PROP_OPENNI_REGISTRATION );
    fs << "depth_focal_length" << camera.get( CAP_OPENNI_DEPTH_GENERATOR_FOCAL_LENGTH);
    fs << "depth_base_line" << camera.get( CAP_OPENNI_DEPTH_GENERATOR_BASELINE);
    
    fs << "image_frame_res" << "[" << camera.get( CAP_OPENNI_IMAGE_GENERATOR+CAP_PROP_FRAME_WIDTH ) <<
                                      camera.get( CAP_OPENNI_IMAGE_GENERATOR+CAP_PROP_FRAME_HEIGHT ) << "]";
    fs << "image_fps" << camera.get( CAP_OPENNI_IMAGE_GENERATOR+CAP_PROP_FPS );
//    fs << "image_focal_length" << camera.get( CAP_OPENNI_IMAGE_GENERATOR+CAP_PROP_OPENNI_FOCAL_LENGTH);
    return true;
}




int writeRGBv2(void) {
    try {

        string file = "tummyNip_";
        string RGBname = file + "RGB.avi";
        string Depthname = file + "Depth.avi";
        string fileDat = file + "Dat.xml";
        
        string pathDepthDir = file + "depth";
        string pathImgDir = file + "img";
        
        
        char * dir = getcwd(NULL, 0); // Platform-dependent, see reference link below
        printf("Current dir: %s\n", dir);
        
        if (!dirExists(pathDepthDir.c_str())) {
            
            int statDir = mkdir(pathDepthDir.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (statDir == 0)
                cout << "Created " << pathDepthDir << " Directory " << endl;
            else
                cout << "cannot create " <<pathDepthDir <<" Directory " << endl;
        }
        
        if (!dirExists(pathImgDir.c_str())) {
            
            int statDir = mkdir(pathImgDir.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (statDir == 0)
                cout << "Created " << pathImgDir << " Directory " << endl;
            else
                cout << "cannot create " <<pathImgDir <<" Directory " << endl;
        }
        
        CvNI2 camera;
        camera.open();
        
        if( !camera.isOpen() )
        {
            cout << "Can not open a capture object." << endl;
            return -1;
        }
        else
            cout << "Camera Opened..." << endl;
        
        
        
        cv::VideoWriter rgbVid, depthVid;
        cv::Mat color,depth,depth_color;
        
        writeCamParams(fileDat,camera);
        
        // Print some avalible device settings.
        cout << "\nDepth generator output mode:" << endl <<
        "FRAME_WIDTH      " << camera.get( CAP_OPENNI_DEPTH_GENERATOR+CAP_PROP_FRAME_WIDTH ) << endl <<
        "FRAME_HEIGHT     " << camera.get( CAP_OPENNI_DEPTH_GENERATOR+CAP_PROP_FRAME_HEIGHT ) << endl <<
        "FRAME_MAX_DEPTH  " << camera.get( CAP_OPENNI_DEPTH_GENERATOR+CAP_PROP_OPENNI_FRAME_MAX_DEPTH ) << " mm" << endl <<
        "FPS              " << camera.get( CAP_OPENNI_DEPTH_GENERATOR+CAP_PROP_FPS ) << endl <<
        "REGISTRATION     " << camera.get( CAP_OPENNI_DEPTH_GENERATOR+CAP_PROP_OPENNI_REGISTRATION ) << endl <<
        "Focal Length     " << camera.get( CAP_OPENNI_DEPTH_GENERATOR_FOCAL_LENGTH) << "mm" << endl <<
        "Base Line        " << camera.get( CAP_OPENNI_DEPTH_GENERATOR_BASELINE) << "mm" << endl;
        
        if( camera.get( CAP_OPENNI_IMAGE_GENERATOR_PRESENT ) )
        {
            cout <<
            "\nImage generator output mode:" << endl <<
            "FRAME_WIDTH   " << camera.get( CAP_OPENNI_IMAGE_GENERATOR+CAP_PROP_FRAME_WIDTH ) << endl <<
            "FRAME_HEIGHT  " << camera.get( CAP_OPENNI_IMAGE_GENERATOR+CAP_PROP_FRAME_HEIGHT ) << endl <<
            "FPS           " << camera.get( CAP_OPENNI_IMAGE_GENERATOR+CAP_PROP_FPS ) << endl;
        }
        else
        {
            cout << "\nDevice doesn't contain image generator." << endl;
        }
        
        Size sz = Size ( (int) camera.get( CAP_OPENNI_IMAGE_GENERATOR+CAP_PROP_FRAME_WIDTH ), (int)camera.get( CAP_OPENNI_IMAGE_GENERATOR+CAP_PROP_FRAME_HEIGHT ));
//        rgbVid.open(RGBname, VideoWriter::fourcc('D','I','V','X'), 30, sz, true );
        Size szDep = Size ( (int) camera.get( CAP_OPENNI_DEPTH_GENERATOR+CAP_PROP_FRAME_WIDTH ),
                           (int) camera.get( CAP_OPENNI_DEPTH_GENERATOR+CAP_PROP_FRAME_HEIGHT ));
//        depthVid.open(Depthname, VideoWriter::fourcc('F','F','V','1'), 30, szDep, false);
        int ctr = 1;
        ostringstream ss,ssImg;
        bool stop=false;
        waitKey(3000);
        while ( camera.grab() && ! stop ) {
            Mat depth,color;
            Mat bgrImage,rgbImage;
            
            camera.retrieve ( color,depth );
            
            ss.str("");
            ssImg.str("");
            ss << setfill('0') << setw(5) << ctr;
            ssImg << setfill('0') << setw(5) << ctr;
            string fileDep = pathDepthDir + "/" + file + ss.str() + ".png";
            string fileImag = pathImgDir + "/" + file + ssImg.str() + ".png";
            imwrite(fileDep, depth);
            imwrite(fileImag,color);
            //            depthVid << depthMap;
//            cvtColor(bgrImage, rgbImage, COLOR_BGR2RGB);
//            rgbVid << color;
            
            showDepth ( depth,depth_color );
            cv::imshow ( "color",color );
            cv::imshow ( "depth",depth_color );
            
            stop= cv::waitKey ( 10 ) ==27;
            ctr++;
        };
        cerr<<"Finish"<<endl;
    }catch ( std::exception &ex ) {
        cerr<<ex.what() <<endl;
    }
    return 0;
}

void showDepth (cv::Mat &in_depth, cv::Mat &out_color) {
    double min,max;
    cv::Mat tempMap;
    cv::minMaxIdx(in_depth,&min, &max);
    cv::convertScaleAbs(in_depth, tempMap, 255 / (max-min),-min);
    cv::applyColorMap(tempMap, out_color, cv::COLORMAP_JET);
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