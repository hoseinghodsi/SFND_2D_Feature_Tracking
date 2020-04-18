# SFND 2D Feature Tracking

## The second project for Sensor Fusion nanodegree from Udacity.

This project is primarily focused on the following topics:

- Implementing a buffer to avoid exessive usage of memory wile processing a sequence of images; thus, preventing potential slowing down of the program 
- Utilizing various keypoint detectors such as: Shi-Tomasi, Harris, FAST, BRISK, ORB, AKAZE, and SIFT
- Utilizing differnt keypoint descriptors and ultimately matching the keypoints in succesive images using Brute Force/FLANN and NN/kNN

### Results

**Keypoints detected using FAST detector on the entire image frame**
![FAST keypoints detected](Results/FAST_entireFrame.jpg)

**Keypoints detected using FAST detector only on the front car**
![FAST keypoints detected](Results/FAST_onlyFrontCar.jpg)

**Matching keypoints detected (Shi Tomasi - BRIEF) in two consecutive images**
![Matched keypoints](Results/ShiTomasi-Brief matching keypoints.jpg)

#### MP.7 Performance evaluation 1
Count the number of keypoints on the preceding vehicle for all 10 images and take note of the distribution of their neighborhood size. Do this for all the detectors you have implemented.

**The average number of keypoints detected and the average processing time for 10 consecutive images**
![Number keypoints detected](Results/MP7-nKeypoint_vs_detectorType-Table.jpg)

As seen in table above, the number of keypoints detected varies considerably with different detector types. One comparison criteria could be simply looking at the highest number of keypoints detected; however, I think it is also important to compare the detector types performance based on the number of keypoints detected and the time it took for the detector to detect those keypoints. Therefore, I defined a metric in which the number of keypoints detected is normalized based on the processing time (last column in the table).
Following this metric, we can clearly conclude that FAST detector perfrom significantly faster than othe detectors while detecting acceptable number of keypoints. For example, BRISK detects almost two time more keypoints than FAST does; but at the cost of being almost 40 times slower. 

This conclusion can be seen in the following figure. 

**The average number of keypoints detected and num keypoints per time**
![keypoints_detected_performance](Results/MP7-nKeypoint_vs_detectorType.jpg)

#### MP.7 Performance evaluation 2

Count the number of matched keypoints for all 10 images using all possible combinations of detectors and descriptors. In the matching step, the BF approach is used with the descriptor distance ratio set to 0.8.

The raw data (SFND_PerformanceEvaluation.xlsx) for all 10 images can be found in Results folder. For simplicity purposes, below I provided the average number matched points over all the images.  


**The average number of matched keypoints**
![matched_keypoints_performance](Results/MP8-matched points-BruteForce-KNN-Table.jpg)


