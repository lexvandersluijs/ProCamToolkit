#pragma once


class projector  {
public:
	void initializeFromMesh(ofMesh& mesh);
	void loadCalibration_interactive();
	void loadCalibration_fromConfigFolder(string configFolder);
	void loadCalibration(string calibFolder);
	void saveCalibration(string configFolder);

	void updateCalibration(float aov, int flags, ofRectangle viewport);

	vector<cv::Point3f> objectPoints;
	vector<cv::Point2f> imagePoints;
	vector<bool> referencePoints;
	
	string name;
	cv::Mat rvec, tvec;
	ofMatrix4x4 modelMatrix;
	ofxCv::Intrinsics intrinsics;
	bool calibrationReady;

	projector()
	{
		calibrationReady = false;
	}
};