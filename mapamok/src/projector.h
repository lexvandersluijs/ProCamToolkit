#pragma once


class projector  {
public:
	void initializeFromMesh(ofMesh& mesh);
	void loadCalibration();
	void saveCalibration();

	void updateCalibration(float aov, int flags);

	vector<cv::Point3f> objectPoints;
	vector<cv::Point2f> imagePoints;
	vector<bool> referencePoints;
	
	cv::Mat rvec, tvec;
	ofMatrix4x4 modelMatrix;
	ofxCv::Intrinsics intrinsics;
	bool calibrationReady;

	projector()
	{
		calibrationReady = false;
	}
};