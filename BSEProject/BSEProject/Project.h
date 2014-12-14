//
//  Project.h
//  BSEprojectLocal
//
//  Created by John Anthony Jose on 12/13/14.
//  Copyright (c) 2014 Anthony. All rights reserved.
//

#ifndef BSEprojectLocal_Project_h
#define BSEprojectLocal_Project_h

#include <opencv2/core.hpp>

bool writeRGB(void);
void showDepth (cv::Mat &in_depth, cv::Mat &out_color);
void colorizeDepth ( cv::Mat &in_depth,cv::Mat &out_color ) ;
int writeRGBv2(void);

#endif
