//
//  Project.h
//  BSEprojectLocal
//
//  Created by John Anthony Jose on 12/13/14.
//  Copyright (c) 2014 Anthony. All rights reserved.
//

#ifndef BSEprojectLocal_Project_h
#define BSEprojectLocal_Project_h

#include <sys/stat.h>

#include "estimateFeatureHomography.h"

int listCreator(std::string fileName, std::string files);

bool writeRGB(void);
void showDepth (cv::Mat &in_depth, cv::Mat &out_color);
void colorizeDepth ( cv::Mat &in_depth,cv::Mat &out_color ) ;
int writeRGBv2(void);

bool dirExists(const char *path);

#endif
