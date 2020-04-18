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

#### MP.7 Performance evaluation 1

**The average number of keypoints detected and the average processing time for 10 consecutive images**
![Number keypoints detected](Results/MP7-nKeypoint_vs_detectorType-Table.jpg)

