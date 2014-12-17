//
//  listCreator.cpp
//  BSEProject
//
//  Created by John Anthony Jose on 12/17/14.
//  Copyright (c) 2014 Anthony. All rights reserved.
//

#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

using namespace cv;
using namespace std;

std::string exec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

std::vector<std::string> getStrings (std::string strSerial) {
    
    size_t pos;
    vector<string> fileList;
   for(auto i=0; pos!=string::npos; i=pos+1) {
        pos = strSerial.find(" ",i);
       std::string fileCur = strSerial.substr(i,pos-i);
       if (fileCur.back() == '\n') fileCur.pop_back();
        fileList.push_back(fileCur);
    }
    return fileList;
}

// listCreator(tummy1_dep/tummy1.yaml, tummy1_dep/*.png)
int listCreator(std::string fileName, std::string files)
{
    
    string outputname = fileName;
    
    size_t pos = fileName.find('/');
    if (pos!=string::npos) {
        outputname = fileName.substr(pos+1,string::npos);
    }
    
//    Mat m = imread(outputname); //check if the output is an image - prevent overwrites!
    
    string cmd = "echo " + files;
    
    vector<string> imagelist = getStrings(exec(cmd.c_str()));

    FileStorage fs(outputname, FileStorage::WRITE);
    fs << "images" << "[";
    for(auto i = imagelist.cbegin(); i != imagelist.cend(); ++i){
        fs << *i;
    }
    fs << "]";
    return 0;
}
