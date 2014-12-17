//
//  main.cpp
//  BSEProject
//
//  Created by John Anthony Jose on 12/14/14.
//  Copyright (c) 2014 Anthony. All rights reserved.
//

#include <iostream>
#include "Project.h"
#include <assert.h>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
using namespace std;
using namespace cv;
int main(int argc, const char * argv[]) {
    // insert code here...
    
    
    
    
//    bool status = writeRGBv2();
    
//    listCreator("tummy1_dep/tummy1.yaml", "tummy1_dep/*.png");
    
    Ptr<FeatureDetector> detector = GFTTDetector::create();
    Ptr<DescriptorExtractor> extractor = xfeatures2d::SIFT::create();
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
    
    RobustMatcher rmatcher;
    rmatcher.setFeatureDetector(detector);
    rmatcher.setDescriptorExtractor(extractor);
    rmatcher.setDescriptorMatcher(matcher);
    
    
    
    
    
    return 0;
    
}
