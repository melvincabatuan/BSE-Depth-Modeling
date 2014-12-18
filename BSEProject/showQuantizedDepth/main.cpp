//
//  main.cpp
//  showQuantizedDepth
//
//  Created by John Anthony Jose on 12/18/14.
//  Copyright (c) 2014 Anthony. All rights reserved.
//

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void showDepth (cv::Mat &in_depth, cv::Mat &out_color) {
    double min,max;
    cv::Mat tempMap;
    cv::minMaxIdx(in_depth,&min, &max);
    cv::convertScaleAbs(in_depth, tempMap, 255 / (max-min),-min);
    cv::applyColorMap(tempMap, out_color, cv::COLORMAP_JET);
}

int main(int argc, const char * argv[]) {
    // insert code here...
    
    string file = "tummyNip_";
    string pathDepthDir = file + "depth";
    string pathImgDir = file + "img";
    string fileName = pathDepthDir + ".xml";
    
    FileStorage fs;
    fs.open(fileName, FileStorage::READ);
    
    FileNode n = fs["images"];                         // Read string sequence - Get node
    if (n.type() != FileNode::SEQ)
    {
        cerr << "images is not a sequence! FAIL" << endl;
        return 1;
    }
    
    Mat I = imread("tummyNip mask.png");
    Mat Ibw;
    //Threshold the mask
    threshold(I, Ibw, 100, 255, cv::THRESH_BINARY);
//    imshow("Mask",I);
    
    Mat colorized, show;
    // Go through the node
    for (FileNodeIterator it = n.begin(); it != n.end(); ++it) {
        Mat I1 = imread((string)*it,CV_16UC1);
        showDepth(I1, colorized);
        colorized.copyTo(show, Ibw);
        imshow("Masked Image", show);
        
        waitKey(10);
    }
    

    
    
    
    waitKey(0);
    
    
    
    
    return 0;
}
