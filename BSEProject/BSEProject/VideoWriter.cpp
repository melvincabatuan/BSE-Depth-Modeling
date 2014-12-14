//
//  VideoWriter.cpp
//  BSEprojectLocal
//
//  Created by John Anthony Jose on 12/13/14.
//  Copyright (c) 2014 Anthony. All rights reserved.
//

#include "Project.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

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


bool writeRGB(void){
    
    string file = "tummy1";
    string RGBname = file + "RGB.avi";
    string Depthname = file + "Depth.avi";
    string fileDat = file + "Dat.xml";
    string pathname = file + "dep";
    
    char * dir = getcwd(NULL, 0); // Platform-dependent, see reference link below
    printf("Current dir: %s\n", dir);
    
    if (!dirExists(pathname.c_str())) {
    
        int statDir = mkdir(pathname.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (statDir == 0)
            cout << "Created " << pathname << "Directory" << endl;
        else
            cerr << "cannot create " <<pathname <<"Directory" << endl;
    }
        
   
    cout << "Device opening ..." << endl;
    VideoCapture capture;
    VideoWriter rgbVid, depthVid;
    
    capture.open( CAP_OPENNI2 );
    
    cout << "done." << endl;
    
    if( !capture.isOpened() )
    {
        cout << "Can not open a capture object." << endl;
        return -1;
    }
    
    // Print some avalible device settings.
    cout << "\nDepth generator output mode:" << endl <<
    "FRAME_WIDTH      " << capture.get( CAP_PROP_FRAME_WIDTH ) << endl <<
    "FRAME_HEIGHT     " << capture.get( CAP_PROP_FRAME_HEIGHT ) << endl <<
    "FRAME_MAX_DEPTH  " << capture.get( CAP_PROP_OPENNI_FRAME_MAX_DEPTH ) << " mm" << endl <<
    "FPS              " << capture.get( CAP_PROP_FPS ) << endl <<
    "REGISTRATION     " << capture.get( CAP_PROP_OPENNI_REGISTRATION ) << endl;
    
    
    if( capture.get( CAP_OPENNI_IMAGE_GENERATOR_PRESENT ) )
    {
        cout <<
        "\nImage generator output mode:" << endl <<
        "FRAME_WIDTH   " << capture.get( CAP_OPENNI_IMAGE_GENERATOR+CAP_PROP_FRAME_WIDTH ) << endl <<
        "FRAME_HEIGHT  " << capture.get( CAP_OPENNI_IMAGE_GENERATOR+CAP_PROP_FRAME_HEIGHT ) << endl <<
        "FPS           " << capture.get( CAP_OPENNI_IMAGE_GENERATOR+CAP_PROP_FPS ) << endl;
    }
    else
    {
        cout << "\nDevice doesn't contain image generator." << endl;
    }
    Size sz = Size ( (int) capture.get( CAP_OPENNI_IMAGE_GENERATOR+CAP_PROP_FRAME_WIDTH ), (int)capture.get( CAP_OPENNI_IMAGE_GENERATOR+CAP_PROP_FRAME_HEIGHT ));
    rgbVid.open(RGBname, VideoWriter::fourcc('D','I','V','X'), 30, sz, true );
    Size szDep = Size ( (int) capture.get( CAP_OPENNI_DEPTH_GENERATOR+CAP_PROP_FRAME_WIDTH ),
                    (int) capture.get( CAP_OPENNI_DEPTH_GENERATOR+CAP_PROP_FRAME_HEIGHT ));
    depthVid.open(Depthname, VideoWriter::fourcc('F','F','V','1'), 30, szDep, false);
    int ctr = 0;
    for(;;)
    {
        Mat depthMap;
        Mat bgrImage,rgbImage;
        
        if( !capture.grab() )
        {
            cout << "Can not grab images." << endl;
            return false;
        }
        else
        {
                const float scaleFactor = 0.05f;
            capture.retrieve(depthMap, CAP_OPENNI_DEPTH_MAP);
                Mat show; depthMap.convertTo( show, CV_8UC1, scaleFactor );
            string fileImg = pathname + "/" + file + to_string(ctr) + ".png";
            imwrite(fileImg, depthMap);
//            depthVid << depthMap;
                imshow( "depth map", show );
            capture.retrieve( bgrImage, CAP_OPENNI_BGR_IMAGE );
                imshow( "rgb image", bgrImage );
            cvtColor(bgrImage, rgbImage, COLOR_BGR2RGB);
            rgbVid << rgbImage;
            
        }
        
        if( waitKey( 30 ) >= 0 )
            break;
        
        ctr++;
    }
    
    return true;
}


