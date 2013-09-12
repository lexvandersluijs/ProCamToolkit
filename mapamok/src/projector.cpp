#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void projector::initializeFromMesh(ofMesh& objectMesh)
{
	int n = objectMesh.getNumVertices();
	objectPoints.resize(n);
	imagePoints.resize(n);
	referencePoints.resize(n, false);
	for(int i = 0; i < n; i++) {
		objectPoints[i] = toCv(objectMesh.getVertex(i));
	}

}

void projector::updateCalibration(float aov, int flags)
{
	// generate camera matrix given aov guess

	// LS: NOTE: this gets the WINDOW size, not the VIEWPORT size. Needs to be adapted for multi-projector operation
	//Size2i imageSize(ofGetWidth(), ofGetHeight());

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	Size2i imageSize(viewport[2], viewport[3]);

	float f = imageSize.width * ofDegToRad(aov); // i think this is wrong, but it's optimized out anyway
	Point2f c = Point2f(imageSize) * (1. / 2);
	Mat1d cameraMatrix = (Mat1d(3, 3) <<
		f, 0, c.x,
		0, f, c.y,
		0, 0, 1);
		

	
	vector<Mat> rvecs, tvecs;
	Mat distCoeffs;
	vector<vector<Point3f> > referenceObjectPoints(1);
	vector<vector<Point2f> > referenceImagePoints(1);
	int n = referencePoints.size();
	for(int i = 0; i < n; i++) {
		if(referencePoints[i]) {
			referenceObjectPoints[0].push_back(objectPoints[i]);
			referenceImagePoints[0].push_back(imagePoints[i]);
		}
	}
	const static int minPoints = 6;
	if(referenceObjectPoints[0].size() >= minPoints) {
		calibrateCamera(referenceObjectPoints, referenceImagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, flags);
		rvec = rvecs[0];
		tvec = tvecs[0];
		intrinsics.setup(cameraMatrix, imageSize);
		modelMatrix = makeMatrix(rvec, tvec);
		calibrationReady = true;
	} else {
		calibrationReady = false;
	}
}

void projector::saveCalibration() {
	string dirName = "calibration-" + ofGetTimestampString() + "/";
	ofDirectory dir(dirName);
	dir.create();
	
	FileStorage fs(ofToDataPath(dirName + "calibration-advanced.yml"), FileStorage::WRITE);	
	
	Mat cameraMatrix = intrinsics.getCameraMatrix();
	fs << "cameraMatrix" << cameraMatrix;
	
	double focalLength = intrinsics.getFocalLength();
	fs << "focalLength" << focalLength;
	
	Point2d fov = intrinsics.getFov();
	fs << "fov" << fov;
	
	Point2d principalPoint = intrinsics.getPrincipalPoint();
	fs << "principalPoint" << principalPoint;
	
	cv::Size imageSize = intrinsics.getImageSize();
	fs << "imageSize" << imageSize;
	
	fs << "translationVector" << tvec;
	fs << "rotationVector" << rvec;

	Mat rotationMatrix;
	Rodrigues(rvec, rotationMatrix);
	fs << "rotationMatrix" << rotationMatrix;
	
	double rotationAngleRadians = norm(rvec, NORM_L2);
	double rotationAngleDegrees = ofRadToDeg(rotationAngleRadians);
	Mat rotationAxis = rvec / rotationAngleRadians;
	fs << "rotationAngleRadians" << rotationAngleRadians;
	fs << "rotationAngleDegrees" << rotationAngleDegrees;
	fs << "rotationAxis" << rotationAxis;
	
	ofVec3f axis(rotationAxis.at<double>(0), rotationAxis.at<double>(1), rotationAxis.at<double>(2));
	ofVec3f euler = ofQuaternion(rotationAngleDegrees, axis).getEuler();
	Mat eulerMat = (Mat_<double>(3,1) << euler.x, euler.y, euler.z);
	fs << "euler" << eulerMat;
	
	ofFile basic("calibration-basic.txt", ofFile::WriteOnly);
	ofVec3f position( tvec.at<double>(1), tvec.at<double>(2));
	basic << "position (in world units):" << endl;
	basic << "\tx: " << ofToString(tvec.at<double>(0), 2) << endl;
	basic << "\ty: " << ofToString(tvec.at<double>(1), 2) << endl;
	basic << "\tz: " << ofToString(tvec.at<double>(2), 2) << endl;
	basic << "axis-angle rotation (in degrees):" << endl;
	basic << "\taxis x: " << ofToString(axis.x, 2) << endl;
	basic << "\taxis y: " << ofToString(axis.y, 2) << endl;
	basic << "\taxis z: " << ofToString(axis.z, 2) << endl;
	basic << "\tangle: " << ofToString(rotationAngleDegrees, 2) << endl;
	basic << "euler rotation (in degrees):" << endl;
	basic << "\tx: " << ofToString(euler.x, 2) << endl;
	basic << "\ty: " << ofToString(euler.y, 2) << endl;
	basic << "\tz: " << ofToString(euler.z, 2) << endl;
	basic << "fov (in degrees):" << endl;
	basic << "\thorizontal: " << ofToString(fov.x, 2) << endl;
	basic << "\tvertical: " << ofToString(fov.y, 2) << endl;
	basic << "principal point (in screen units):" << endl;
	basic << "\tx: " << ofToString(principalPoint.x, 2) << endl;
	basic << "\ty: " << ofToString(principalPoint.y, 2) << endl;
	basic << "image size (in pixels):" << endl;
	basic << "\tx: " << ofToString(principalPoint.x, 2) << endl;
	basic << "\ty: " << ofToString(principalPoint.y, 2) << endl;
	
	saveMat(Mat(objectPoints), dirName + "objectPoints.yml");
	saveMat(Mat(imagePoints), dirName + "imagePoints.yml");
}

void projector::loadCalibration() {
    
    // retrieve advanced calibration folder
    
    string calibPath;
    ofFileDialogResult result = ofSystemLoadDialog("Select a calibration folder", true, ofToDataPath("", true));
    calibPath = result.getPath();
    
    // load objectPoints and imagePoints
    
    Mat objPointsMat, imgPointsMat;
    loadMat( objPointsMat, calibPath + "/objectPoints.yml");
    loadMat( imgPointsMat, calibPath + "/imagePoints.yml");
    
    int numVals;
    float x, y, z;
    cv::Point3f oP;
    
    const float* objVals = objPointsMat.ptr<float>(0);
    numVals = objPointsMat.cols * objPointsMat.rows;
    
    for(int i = 0; i < numVals; i+=3) {
        oP.x = objVals[i];
        oP.y = objVals[i+1];
        oP.z = objVals[i+2];
        objectPoints[i/3] = oP;
    }
    
    cv::Point2f iP;
    
    referencePoints.resize( (imgPointsMat.cols * imgPointsMat.rows ) / 2, false);
    
    const float* imgVals = imgPointsMat.ptr<float>(0);
    numVals = objPointsMat.cols * objPointsMat.rows;
    
    for(int i = 0; i < numVals; i+=2) {
        iP.x = imgVals[i];
        iP.y = imgVals[i+1];
        if(iP.x != 0 && iP.y != 0) {
            referencePoints[i/2] = true;
        }
        imagePoints[i/2] = iP;
    }
    
    
    // load the calibration-advanced yml
    
    FileStorage fs(ofToDataPath(calibPath + "/calibration-advanced.yml", true), FileStorage::READ);
    
    Mat cameraMatrix;
    Size2i imageSize;
    fs["cameraMatrix"] >> cameraMatrix;
    fs["imageSize"][0] >> imageSize.width;
    fs["imageSize"][1] >> imageSize.height;
    fs["rotationVector"] >> rvec;
    fs["translationVector"] >> tvec;
    
    intrinsics.setup(cameraMatrix, imageSize);
    modelMatrix = makeMatrix(rvec, tvec);
    
    calibrationReady = true;
}
