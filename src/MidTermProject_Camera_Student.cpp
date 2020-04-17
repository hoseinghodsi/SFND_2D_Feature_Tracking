/* INCLUDES FOR THIS PROJECT */
#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <limits>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include "dataStructures.h"
#include "matching2D.hpp"

using namespace std;

/* MAIN PROGRAM */
int main(int argc, const char *argv[])
{

    /* INIT VARIABLES AND DATA STRUCTURES */

    // data location
    string dataPath = "../";

    // camera
    string imgBasePath = dataPath + "images/";
    string imgPrefix = "KITTI/2011_09_26/image_00/data/000000"; // left camera, color
    string imgFileType = ".png";
    int imgStartIndex = 0; // first file index to load (assumes Lidar and camera names have identical naming convention)
    int imgEndIndex = 9;   // last file index to load
    int imgFillWidth = 4;  // no. of digits which make up the file index (e.g. img-0001.png)

    // misc
    int dataBufferSize = 2;       // no. of images which are held in memory (ring buffer) at the same time
    vector<DataFrame> dataBuffer; // list of data frames which are held in memory at the same time
    bool bVis = false;            // visualize results

    vector<string> detector_type = {"SHITOMASI", "HARRIS", "FAST", "BRISK", "AKAZE", "SIFT"};
    vector<string> descriptor_type = {"BRISK", "BRIEF", "ORB", "FREAK", "AKAZE", "SIFT"};
    //vector<string> detector_type = {"SHITOMASI"};
    //vector<string> descriptor_type = {"BRISK"};

    // Defined these matching parameters earlier to crear proper filenames
    string matcherType = "MAT_BF";        // MAT_BF, MAT_FLANN
    string descriptorType = "DES_BINARY"; // DES_BINARY, DES_HOG
    string selectorType = "SEL_KNN";       // SEL_NN, SEL_KNN

    string fileBase = "../PerfEval";
    string fileType = ".csv";

    string filename = fileBase + "__" + matcherType + "__" + descriptorType + "__" + selectorType + fileType;
    cout << filename << endl;
    ofstream PerformanceEvaluation;
    PerformanceEvaluation.open (filename);

    int nImg = 0;
    // Preparing a header for the performance evaluation csv file
    PerformanceEvaluation << "Detector Type" << ", " << "Descriptor Type" << ", ";
    for (size_t imgIndex = 0; imgIndex <= imgEndIndex - imgStartIndex; imgIndex++)
    {
        PerformanceEvaluation << "Keypoints number" << ", " << "Detector Processing Time" << ", " << "Descriptor Processign Time" << ", " 
                              << "Total Elapsed Time" << ", " << "Matched Keypoints" << ", ";
        nImg++;
    }    
    PerformanceEvaluation << "Keypoints" << ", " << "DetDesTime" << "MatchingTime" << endl;



    // Loop over different detector types
    for (auto detectorType:detector_type)
    {
        // Loop over differnt descriptor types
        for (auto descriptorType:descriptor_type)
        {
            int sumNumKeypoints = 0;
            double sumElapsedTime = 0;
            double sumMatchingTime = 0;
            dataBuffer.clear();

            if (detectorType.compare("AKAZE") != 0 && descriptorType.compare("AKAZE") == 0) continue;
            if (detectorType.compare("AKAZE") == 0 && descriptorType.compare("AKAZE") == 0) continue;

            PerformanceEvaluation << detectorType << ", " << descriptorType << ", ";
            
            /* MAIN LOOP OVER ALL IMAGES */
            for (size_t imgIndex = 0; imgIndex <= imgEndIndex - imgStartIndex; imgIndex++)
            {
                /* LOAD IMAGE INTO BUFFER */
                cout << endl;
                cout << "----------------------------------" << endl;
                cout << "Detector  : " << detectorType << endl;
                cout << "Descriptor: " << descriptorType << endl;
                cout << "----------------------------------" << endl;
                cout << "Image     : " << imgIndex + 1 << endl;
                cout << endl;

                // assemble filenames for current index
                ostringstream imgNumber;
                imgNumber << setfill('0') << setw(imgFillWidth) << imgStartIndex + imgIndex;
                string imgFullFilename = imgBasePath + imgPrefix + imgNumber.str() + imgFileType;

                // load image from file and convert to grayscale
                cv::Mat img, imgGray; 
                img = cv::imread(imgFullFilename);
                cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

                //// STUDENT ASSIGNMENT
                //// TASK MP.1 -> replace the following code with ring buffer of size dataBufferSize

                // push image into data frame buffer
                DataFrame frame;
                frame.cameraImg = imgGray;
                dataBuffer.push_back(frame);

                if (dataBuffer.size() > dataBufferSize) 
                {
                    dataBuffer.erase(dataBuffer.begin());
                }
                //// EOF STUDENT ASSIGNMENT
                cout << "#1 : LOAD IMAGE INTO BUFFER done" << endl;

                /* DETECT IMAGE KEYPOINTS */

                // extract 2D keypoints from current image
                vector<cv::KeyPoint> keypoints; // create empty feature list for current image
                //string detectorType = "SHITOMASI";

                //// STUDENT ASSIGNMENT
                //// TASK MP.2 -> add the following keypoint detectors in file matching2D.cpp and enable string-based selection based on detectorType
                //// -> HARRIS, FAST, BRISK, ORB, AKAZE, SIFT
                pair<int, double> selectedDetector;
                
                if (detectorType.compare("SHITOMASI") == 0)
                {
                    selectedDetector = detKeypointsShiTomasi(keypoints, imgGray, false);
                }
                else if (detectorType.compare("HARRIS") == 0)
                {
                    selectedDetector = detKeypointsHarris(keypoints, imgGray, false);
                }
                else if (detectorType.compare("FAST") == 0 || detectorType.compare("BRISK") == 0 || 
                         detectorType.compare("ORG") == 0 || detectorType.compare("AKAZE") == 0 || detectorType.compare("SIFT") == 0)
                {
                    selectedDetector = detKeypointsModern(keypoints, imgGray, detectorType, false);
                }
                
                //// EOF STUDENT ASSIGNMENT

                //// STUDENT ASSIGNMENT
                //// TASK MP.3 -> only keep keypoints on the preceding vehicle

                // only keep keypoints on the preceding vehicle
                bool bFocusOnVehicle = true;
                cv::Rect vehicleRect(535, 180, 180, 150);

                vector<cv::KeyPoint>::iterator kp;
                vector<cv::KeyPoint> keypoints_ROI; 

                if (bFocusOnVehicle)
                {
                    for (kp = keypoints.begin(); kp != keypoints.end(); ++kp)
                    {
                        if (vehicleRect.contains(kp->pt))
                        {
                            cv::KeyPoint pickedKeyPoint;
                            pickedKeyPoint.pt = cv::Point2f(kp->pt);
                            pickedKeyPoint.size = 1;
                            keypoints_ROI.push_back(pickedKeyPoint);
                        }
                    }
                keypoints = keypoints_ROI;
                }
                //// EOF STUDENT ASSIGNMENT

                // optional : limit number of keypoints (helpful for debugging and learning)
                bool bLimitKpts = false;
                if (bLimitKpts)
                {
                    int maxKeypoints = 50;

                    if (detectorType.compare("SHITOMASI") == 0)
                    { // there is no response info, so keep the first 50 as they are sorted in descending quality order
                        keypoints.erase(keypoints.begin() + maxKeypoints, keypoints.end());
                    }
                    cv::KeyPointsFilter::retainBest(keypoints, maxKeypoints);
                    cout << " NOTE: Keypoints have been limited!" << endl;
                }

                // push keypoints and descriptor for current frame to end of data buffer
                (dataBuffer.end() - 1)->keypoints = keypoints;
                cout << "#2 : DETECT KEYPOINTS done" << endl;

                /* EXTRACT KEYPOINT DESCRIPTORS */

                //// STUDENT ASSIGNMENT
                //// TASK MP.4 -> add the following descriptors in file matching2D.cpp and enable string-based selection based on descriptorType
                //// -> BRIEF, ORB, FREAK, AKAZE, SIFT

                cv::Mat descriptors;
                //string descriptorType = "ORB"; // BRIEF, ORB, FREAK, AKAZE, SIFT
                double DescriptorElapsedTime;
                DescriptorElapsedTime = descKeypoints((dataBuffer.end() - 1)->keypoints, (dataBuffer.end() - 1)->cameraImg, descriptors, descriptorType);

                //// EOF STUDENT ASSIGNMENT

                // push descriptors for current frame to end of data buffer
                (dataBuffer.end() - 1)->descriptors = descriptors;

                cout << "#3 : EXTRACT DESCRIPTORS done" << endl;
                double match_time = 0;
                if (dataBuffer.size() > 1) // wait until at least two images have been processed
                {
                    
                    /* MATCH KEYPOINT DESCRIPTORS */

                    vector<cv::DMatch> matches;
                    //string matcherType = "MAT_BF";        // MAT_BF, MAT_FLANN
                    //string descriptorType = "DES_BINARY"; // DES_BINARY, DES_HOG
                    //string selectorType = "SEL_NN";       // SEL_NN, SEL_KNN

                    //// STUDENT ASSIGNMENT
                    //// TASK MP.5 -> add FLANN matching in file matching2D.cpp
                    //// TASK MP.6 -> add KNN match selection and perform descriptor distance ratio filtering with t=0.8 in file matching2D.cpp

                    match_time = matchDescriptors((dataBuffer.end() - 2)->keypoints, (dataBuffer.end() - 1)->keypoints,
                                                  (dataBuffer.end() - 2)->descriptors, (dataBuffer.end() - 1)->descriptors,
                                                   matches, descriptorType, matcherType, selectorType);                                    

                    //// EOF STUDENT ASSIGNMENT

                    // store matches in current data frame
                    (dataBuffer.end() - 1)->kptMatches = matches;

                    cout << "#4 : MATCH KEYPOINT DESCRIPTORS done in " << match_time << " ms" <<endl;

                    // visualize matches between current and previous image
                    bVis = false;
                    if (bVis)
                    {
                        cv::Mat matchImg = ((dataBuffer.end() - 1)->cameraImg).clone();
                        cv::drawMatches((dataBuffer.end() - 2)->cameraImg, (dataBuffer.end() - 2)->keypoints,
                                        (dataBuffer.end() - 1)->cameraImg, (dataBuffer.end() - 1)->keypoints,
                                        matches, matchImg,
                                        cv::Scalar::all(-1), cv::Scalar::all(-1),
                                        vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

                        string windowName = "Matching keypoints between two camera images";
                        cv::namedWindow(windowName, 7);
                        cv::imshow(windowName, matchImg);
                        cout << "Press key to continue to next image" << endl;
                        cv::waitKey(0); // wait for key to be pressed
                    }
                    bVis = false;
                }
                PerformanceEvaluation << keypoints.size() << ", " << selectedDetector.second << ", " << DescriptorElapsedTime << ", " << 
                                         selectedDetector.second + DescriptorElapsedTime << ", " << (dataBuffer.end() - 1) ->kptMatches.size() << ", ";
                sumMatchingTime = sumMatchingTime + match_time;
                sumNumKeypoints = sumNumKeypoints + keypoints.size();
                sumElapsedTime = sumElapsedTime + (selectedDetector.second + DescriptorElapsedTime);
            } // eof loop over all images

            // Taking the average number of keypoints and total elapsed time
            int meanNumKeypoints = (int)round(sumNumKeypoints/nImg);
            double meanElapsedTime = sumElapsedTime/nImg;
            double meanMatchTime = sumMatchingTime/(nImg-1);

            // Writing them in the csv file
            PerformanceEvaluation << meanNumKeypoints << ", " << meanElapsedTime << ", " << meanMatchTime << endl;

        } // eof loop over different descriptor types
    } // eof loop over different detector types
    PerformanceEvaluation.close();

    return 0;
}
