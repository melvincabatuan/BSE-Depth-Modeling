//
//  main.cpp
//  BSEProject
//
//  Created by John Anthony Jose on 12/14/14.
//  Copyright (c) 2014 Anthony. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include "Project.h"
#include <assert.h>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
using namespace std;
using namespace cv;
int main(int argc, const char * argv[]) {
    // insert code here...
    
    string file = "tummyNip_";
//    string RGBname = file + "RGB.avi";
//    string Depthname = file + "Depth.avi";
//    string fileDat = file + "Dat.xml";
//    
//    string pathDepthDir = file + "depth";
    string pathImgDir = file + "img";
//
    string fileName = pathImgDir + ".xml";
//    string pathDir = pathDepthDir + "/" + file + "?????.png";
//    int status = listCreator(fileName, pathDir);
//
    
    FileStorage fs;
    fs.open(fileName, FileStorage::READ);
    
    FileNode n = fs["images"];                         // Read string sequence - Get node
    if (n.type() != FileNode::SEQ)
    {
        cerr << "images is not a sequence! FAIL" << endl;
        return 1;
    }
    
    // Go through the node
    for (FileNodeIterator it = n.begin(); it != n.end(); ++it)
        cout << (string)*it << endl;
    
    
//    ostringstream ss;
//
//    for (int i=1;i<)
//    ss.str("");
//    ss << setw('5') << setfill('0') << ctr;
//    
//    
//    bool status = writeRGBv2();
    
    
    
    
    
    
    
//    listCreator("tummy1_dep/tummy1.yaml", "tummy1_dep/*.png");
//    
//    Ptr<FeatureDetector> detector = GFTTDetector::create();
//    Ptr<DescriptorExtractor> extractor = xfeatures2d::SIFT::create();
//    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
//    
//    RobustMatcher rmatcher;
//    rmatcher.setFeatureDetector(detector);
//    rmatcher.setDescriptorExtractor(extractor);
//    rmatcher.setDescriptorMatcher(matcher);
//    
//    
    
    
    
    return 0;
    
}
