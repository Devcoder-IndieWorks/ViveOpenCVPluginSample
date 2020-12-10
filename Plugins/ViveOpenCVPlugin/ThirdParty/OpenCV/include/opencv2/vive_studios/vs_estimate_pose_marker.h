#pragma once
#include <opencv2/aruco.hpp>

namespace cv
{
namespace vive_studios
{
CV_EXPORTS_W void SetupPredefinedDictionary( aruco::PREDEFINED_DICTIONARY_NAME InName );
CV_EXPORTS_W void ResetPredefinedDictionary();

CV_EXPORTS_W bool EstimagePoseSingleMarker( float InMarkerSize, Mat& InCameraMatrix, Mat& InDistCoeff, bool InDrawMarker, bool InDrawAxis, 
    Mat& InOutFrame, Vec3d& OutRot, Vec3d& OutTrans );
}
}
