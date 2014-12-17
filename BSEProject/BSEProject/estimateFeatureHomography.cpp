/*
 * RobustMatcher.cpp
 *
 *  Created on: Jun 4, 2014
 *      Author: eriba
 */

#include <iostream>
#include "estimateFeatureHomography.h"
#include <time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>

RobustMatcher::~RobustMatcher()
{
    // TODO Auto-generated destructor stub
}

void RobustMatcher::computeKeyPoints( const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints)
{
    detector_->detect(image, keypoints);
}

void RobustMatcher::computeDescriptors( const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors)
{
    extractor_->compute(image, keypoints, descriptors);
}

int RobustMatcher::ratioTest(std::vector<std::vector<cv::DMatch> > &matches)
{
    int removed = 0;
    // for all matches
    for ( std::vector<std::vector<cv::DMatch> >::iterator
         matchIterator= matches.begin(); matchIterator!= matches.end(); ++matchIterator)
    {
        // if 2 NN has been identified
        if (matchIterator->size() > 1)
        {
            // check distance ratio
            if ((*matchIterator)[0].distance / (*matchIterator)[1].distance > ratio_)
            {
                matchIterator->clear(); // remove match
                removed++;
            }
        }
        else
        { // does not have 2 neighbours
            matchIterator->clear(); // remove match
            removed++;
        }
    }
    return removed;
}

void RobustMatcher::symmetryTest( const std::vector<std::vector<cv::DMatch> >& matches1,
                                 const std::vector<std::vector<cv::DMatch> >& matches2,
                                 std::vector<cv::DMatch>& symMatches )
{
    
    // for all matches image 1 -> image 2
    for (std::vector<std::vector<cv::DMatch> >::const_iterator
         matchIterator1 = matches1.begin(); matchIterator1 != matches1.end(); ++matchIterator1)
    {
        
        // ignore deleted matches
        if (matchIterator1->empty() || matchIterator1->size() < 2)
            continue;
        
        // for all matches image 2 -> image 1
        for (std::vector<std::vector<cv::DMatch> >::const_iterator
             matchIterator2 = matches2.begin(); matchIterator2 != matches2.end(); ++matchIterator2)
        {
            // ignore deleted matches
            if (matchIterator2->empty() || matchIterator2->size() < 2)
                continue;
            
            // Match symmetry test
            if ((*matchIterator1)[0].queryIdx ==
                (*matchIterator2)[0].trainIdx &&
                (*matchIterator2)[0].queryIdx ==
                (*matchIterator1)[0].trainIdx)
            {
                // add symmetrical match
                symMatches.push_back(
                                     cv::DMatch((*matchIterator1)[0].queryIdx,
                                                (*matchIterator1)[0].trainIdx,
                                                (*matchIterator1)[0].distance));
                break; // next match in image 1 -> image 2
            }
        }
    }
    
}

void RobustMatcher::showFeaturesDescriptors (std::vector<cv::KeyPoint>& points_obj, cv::Mat& desc_obj, std::vector<cv::KeyPoint>& points_scene, cv::Mat& desc_scene) {
    
    points_obj = keypoints_object;
    desc_obj = descriptors_object;
    points_scene = keypoints_scene;
    desc_scene = descriptors_scene;
    
}


void RobustMatcher::robustMatch( const cv::Mat& object, const cv::Mat &scene, std::vector<cv::DMatch>& good_matches)
{
    // 1a. Detection of the ORB features
    this->computeKeyPoints(scene, keypoints_scene);
    this->computeKeyPoints(object, keypoints_object);
    
    // 1b. Extraction of the ORB descriptors
    this->computeDescriptors(scene, keypoints_scene, descriptors_scene);
    this->computeDescriptors(object, keypoints_object, descriptors_object);
    
    // 2. Match the two image descriptors
    std::vector<std::vector<cv::DMatch> > matches12, matches21;
    
    // 2a. From image 1 to image 2
    matcher_->knnMatch(descriptors_object, descriptors_scene, matches12, 2); // return 2 nearest neighbours
    
    // 2b. From image 2 to image 1
    matcher_->knnMatch(descriptors_scene, descriptors_object, matches21, 2); // return 2 nearest neighbours
    
    // 3. Remove matches for which NN ratio is > than threshold
    // clean image 1 -> image 2 matches
    ratioTest(matches12);
    // clean image 2 -> image 1 matches
    ratioTest(matches21);
    
    // 4. Remove non-symmetrical matches
    symmetryTest(matches12, matches21, good_matches);
    
    this->matchUsed = true;
}


void RobustMatcher::robustMatch( const cv::Mat& frame, std::vector<cv::DMatch>& good_matches,
                                std::vector<cv::KeyPoint>& keypoints_frame, const cv::Mat& descriptors_model )
{
    
    // 1a. Detection of the ORB features
    this->computeKeyPoints(frame, keypoints_frame);
    
    // 1b. Extraction of the ORB descriptors
    cv::Mat descriptors_frame;
    this->computeDescriptors(frame, keypoints_frame, descriptors_frame);
    
    // 2. Match the two image descriptors
    std::vector<std::vector<cv::DMatch> > matches12, matches21;
    
    // 2a. From image 1 to image 2
    matcher_->knnMatch(descriptors_frame, descriptors_model, matches12, 2); // return 2 nearest neighbours
    
    // 2b. From image 2 to image 1
    matcher_->knnMatch(descriptors_model, descriptors_frame, matches21, 2); // return 2 nearest neighbours
    
    // 3. Remove matches for which NN ratio is > than threshold
    // clean image 1 -> image 2 matches
    ratioTest(matches12);
    // clean image 2 -> image 1 matches
    ratioTest(matches21);
    
    // 4. Remove non-symmetrical matches
    symmetryTest(matches12, matches21, good_matches);
    
    this->matchUsed = true;
}

void RobustMatcher::fastRobustMatch( const cv::Mat& frame, std::vector<cv::DMatch>& good_matches,
                                    std::vector<cv::KeyPoint>& keypoints_frame,
                                    const cv::Mat& descriptors_model )
{
    good_matches.clear();
    
    // 1a. Detection of the ORB features
    this->computeKeyPoints(frame, keypoints_frame);
    
    // 1b. Extraction of the ORB descriptors
    cv::Mat descriptors_frame;
    this->computeDescriptors(frame, keypoints_frame, descriptors_frame);
    
    // 2. Match the two image descriptors
    std::vector<std::vector<cv::DMatch> > matches;
    matcher_->knnMatch(descriptors_frame, descriptors_model, matches, 2);
    
    // 3. Remove matches for which NN ratio is > than threshold
    ratioTest(matches);
    
    // 4. Fill good matches container
    for ( std::vector<std::vector<cv::DMatch> >::iterator
         matchIterator= matches.begin(); matchIterator!= matches.end(); ++matchIterator)
    {
        if (!matchIterator->empty()) good_matches.push_back((*matchIterator)[0]);
    }
    this->matchUsed = true;
    
}

cv::Mat RobustMatcher::computeHomography(std::vector<cv::Point2f> scene_points, std::vector<cv::Point2f> obj_points) {
    
    cv::Mat result;
    
    //Check whether RobustMatch was computed
    if (!matchUsed)
        std::cerr << "RMatcher Error: RobustMatch was not yet used" << std::endl;
    
    result = cv::findHomography(obj_points, scene_points, cv::FM_RANSAC, ransacReprojThreshold );
    
    
    return result;
}
