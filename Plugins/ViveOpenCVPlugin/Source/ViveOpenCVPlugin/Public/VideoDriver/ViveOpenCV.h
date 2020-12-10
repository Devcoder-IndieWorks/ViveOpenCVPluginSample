// Copyright UserHabit io. All Rights Reserved.
#pragma once
// 이 헤더파일은 UE4 관련 헤더파일보다 우선 선언해야 됨.

// OpenCV 라이브러리에서는 check를 template 함수로 정의하고,
// UE4에서는 매크로로 정의하고 있어 서로 충돌이 발생 함.
// 그래서 매크로 정의를 없애고, OpenCV에서 template 함수로 정의 되도록 하고,
// 이후 UE4에서 매크로로 정의 되도록 함.
#pragma push_macro( "check" )
#undef check

#pragma warning( push )
__pragma(warning(disable: 4946))
__pragma(warning(disable: 4190))
__pragma(warning(disable: 6297))
__pragma(warning(disable: 6294))
__pragma(warning(disable: 6201))
#include <opencv2/video.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/vive_studios.hpp>
#pragma warning( pop )

#pragma pop_macro( "check" )
