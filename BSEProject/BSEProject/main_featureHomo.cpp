//
//  main.cpp
//  FeatureHomography
//
//  Created by John Anthony Jose on 12/17/14.
//  Copyright (c) 2014 Anthony. All rights reserved.
//

#include <iostream>
#include "estimateFeatureHomography.h"
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>

using namespace cv;
using namespace std;
int main(int argc, const char * argv[]) {
    // insert code here...
    
    Mat I1 = imread("tummyImage/tummiest2_1.png");
    Mat I2 = imread("tummyImage/tummiest2_40.png");
    Mat I1g,I2g;
    
    cvtColor(I1, I1g, COLOR_RGB2GRAY);
    cvtColor(I2, I2g, COLOR_RGB2GRAY);
    
    int maxCorners=1000, blockSize=3;
    double qualityLevel=0.01, minDistance=1., k=0.04;
    bool useHarrisDetector=false;
    
    Ptr<FeatureDetector> detector = GFTTDetector::create(maxCorners,qualityLevel,minDistance,blockSize,useHarrisDetector,k);
    Ptr<DescriptorExtractor> extractor = xfeatures2d::SIFT::create();
//    Ptr OppExtractor (new Opp)
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
    
    RobustMatcher rmatcher;
    
    rmatcher.setFeatureDetector(detector);
    rmatcher.setDescriptorExtractor(extractor);
    rmatcher.setDescriptorMatcher(matcher);
    
    vector<KeyPoint> points1, points2;
    Mat desc1, desc2;
    vector<DMatch> matches;
    Mat show;
    rmatcher.robustMatch(I2g, I1g, matches);
    rmatcher.showFeaturesDescriptors(points2, desc2, points1, desc1);
    drawMatches(I2g, points2, I1g, points1, matches, show);
    
    imshow("BSE", show);
    
    vector<Point2f> inliers1, inliers2;
    Mat H;
    
    for (auto i=matches.cbegin(); i!=matches.cend(); ++i) {
        inliers1.push_back(points1[i->trainIdx].pt);
        inliers2.push_back(points2[i->queryIdx].pt);
    }
    
    H = rmatcher.computeHomography(inliers1, inliers2);
    
    cout << H << endl;
    waitKey(0);
    
    
    return 0;   
    
    
    
    
    
    
}
