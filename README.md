# ViveOpenCVPluginSample

UE4에서 OpenCV 기능을 사용 할 수 있게 해 주는 플러그인 샘플 프로젝트 (Camera Position Calibration, Camera Lens Calibration 기능)



## ViveOpenCV Plugin

OpenCV 기능을 Unreal Engine에서 사용할 수 있고 OpenCV를 이용하여 실제 Camera Transform을 구하고 Camera Lens Calibration 기능을 제공하는 플러그인.

## OpenCV로 Camera Position Calibration 하는 방법

Plugins\ViveOpenCVPlugin\Content\Blueprints 폴더에 BP_ViveDefaultDriver와 BP_ViveVideoSource 어셋이 있다. BP_ViveVideoSource 어셋은 OpenCV를 사용하여 Calibration하기 위한 카메라 및 패턴 이미지에 대한 정보를 담고 있는 어셋이다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/Video_Source_BP.png)

그리고 Calibration된 정보를 외부에 저장하기 위한 파일 이름을 설정하는 내용도 있다(현재 BP_ViveVideoSource에는 Camera Position과 Camera Lens에 대한 Calibration에 대한 예시 설정 정보가 입력되어져 있다. 사용하기전 사용하는 카메라에 대한 정보 및 기타 설정을 해야 함).

BP_ViveDefaultDriver 어셋은 BP_ViveVideoSource에 설정된 정보 중 어떤 것을 사용할 것인지 선택하고 또 다른 VideoSource 어셋이 있다면 어떤 VideoSource 어셋을 사용할 것인진 선택할 수 있다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/Default_Driver_BP.png)

설정 정보 입력이 완료 되었다면 CameraCalib 맵을 실행하면 다음 그림과 같이 카메라가 보는 영상이 화면에 나타난다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/Camera_Calibration.png)

Camera Calibration을 위한 패턴 이미지를 카메라로 촬영하면서 Calibrate 버튼을 누르게 되면 Camera Calibration 설정이 시작된다.

(패턴 이미지는 Plugins\ViveOpenCVPlugin\OpenCVImages 폴더에 있음).

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/Camera_Calibration2.png)

Continue 버튼을 누르면서 설정된 Captured Frames을 채운다. 한 번 누를때 마다 일정한 Frame이 캡처된다(노란색 박스로 현재 Capture되는 상태를 알수 있음).

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/Camera_Calibration_Result.png)

Calibration이 끝나면 로그에 Calibration된 결과 정보를 확인 할 수 있다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/Camera_Estimate_Pose.png)

Camera Calibration이 끝나면 결과로 만들어진 정보를 바탕으로 가상 카메라 위치를 계산 할 수 있는데 가상 카메라 위치를 계산하기 위해 EstimatePose 버튼을 누르면 위 그림처럼 Mark 이미지 위에 좌표 그림이 나타나게 된다. 이로써 가상 카메라 위치에 대한 결과를 만들어진다.

계산된 가상 카메라 위치에 대한 정보를 외부 파일로 저장하려면 Save Pose 버튼으로 저장한다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/Save_Pose.png)

저장된 내용은 다음 그림과 같다:

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/Save_Pose_Data.png)

이렇게 저장된 결과를 이용해서 가상 카메라 위치가 제대로 계산 되어졌는지 확인을 해보기 위해 테스트를 진행한다.

DummyMap으로 맵을 변경한다. 그리고 실행하게 되면 다음 그림처럼 Calibration 할 때의 카메라 위치가 가상 카메라로 나타나게 된다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/Camera_Position.png)

수직으로 세워져 있는 원뿔이 Estimate Pose일 때 좌표이고 화살축의 지점을 나타냄. 또 다른 원뿔은 Estimate Pose일 때 바라보는 카메라 위치를 나타낸다.



위 그림에서 가상 카메라를 나타내는 원뿔의 Transform 정보가 다음 그림처럼 로그에 존재한다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/Camera_Position_Data.png)



## API 정리

#### Vive Camera Calibrator

OpenCV를 통해 구한 Camera Calibration Information 데이터로 Unreal Engine 가상 카메라(예: ACineCameraActor)의 Transform을 계산해 주는 Function Node.

Out Transform: 계산된 가상 카메라 엑터의 Transform

Return Value: 정상적으로 계산되어 가상 카메라의 Transform 값을 얻었는지 여부.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림11.png)



OpenCV를 통해 구한 Camera Calibration Information 데이터로 Unreal Engine 가상 카메라(예: ACineCameraActor)의 Focal Length를 계산해 주는 Function Node.

Out Focal Length: 계산된 가상 카메라의 Focal Length 값.

Return Value: Focal Length 값을 얻었는지 여부.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림12.png)



사용 중인 실제 카메라의 종횡비(Aspect Ratio)를 구하는 Function Node.

Out Aspect Ratio: 실제 카메라 종횡비 값.

Return Value: 실제 카메라 종횡비를 얻었는지 여부.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림13.png)



OpenCV를 통해 구한 Camera Calibration Information 데이터로 Unreal Engine 가상 카메라의 X축 방향 Field Of View 값을 구하는 Function Node.

Out Fov X: X축 방향 Field Of View 값.

Return Value: X축 방향 Field Of View 값을 얻었는지 여부.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림14.png)



OpenCV를 통해 구한 Camera Calibration Information 데이터로 Unreal Engine 가상 카메라의 Y축 방향 Field Of View 값을 구하는 Function Node.

Out Fov Y: Y축 방향 Field Of View 값.

Return Value: Y축 방향 Field Of View 값을 얻었는지 여부.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림15.png)



#### Vive Camera Tracker Helper

**참고: ViveVR Tracker는 ViveVR 로고가 보이는 면이 가리키는 방향이 Forward 방향이고, 전원 램프가 있는 방향이 Up이 됨.**

**밑에 구현한 Function Node들은 ViveVR Tracker가 ViveVR 로고가 보이는 면이 가리키는 방향이 Up, 전원 램프가 있는 방향을 Forward가 되게 설치한 상태로 구현 함.**



ViveVR Tracker의 회전 값을 Unreal Engine의 회전 값으로 변환하는 Function Node.

In Rotation: ViveVR Tracker의 회전 값.

Out Rotation: Unreal Engine에서 사용 가능하도록 변환된 회전 값.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림16.png)



ViveVR Tracker의 이동 값을 Unreal Engine의 이동 값으로 변환하는 Function Node.

In Location: ViveVR Tracker의 이동 값.

Out Location: Unreal Engine에서 사용 가능하도록 변환된 이동 값.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림17.png)



ViveVR Tracker의 회전된 값과 Unreal Engine 가상 카메라의 회전된 값의 차이를 구하는 Function Node.

In Rotation A: Unreal Engine 가상 카메라 엑터의 현재 회전된 값.

In Rotation B: ViveVR Tracker의 현재 회전된 값.

Return Value: 두 회전 값의 차이 값.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림18.png)



ViveVR Tracker의 이동된 값과 Unreal Engine 가상 카메라의 이동된 값의 차이를 구하는 Function Node.

In Location A: Unreal Engine 가상 카메라 엑터의 현재 이동된 값.

In Location B: ViveVR Tracker의 현재 이동된 값.

Return Value: 두 이동 값의 차이 값.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림19.png)



초기 시점에 구해진 회전 Offset 값과 현재 ViveVR Tracker 회전 값이 합쳐진 회전 값을 구하는 Function Node.

In Rotation: ViveVR Tracker의 현재 회전 값.

In Offset: Compute Rotation Offset으로 구한 Offset 회전 값.

Return Value: 두 회전 값이 합쳐진 회전 값.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림20.png)



초기 시점에 구해진 이동 Offset 값과 현재 ViveVR Tracker 이동 값이 합쳐진 이동 값을 구하는 Function Node.

In Location: ViveVR Tracker의 현재 이동 값.

In Offset: Compte Location Offset으로 구한 Offset 이동 값.

Return Value: 두 이동 값이 합쳐진 이동 값.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림21.png)



#### Utility Function Node 사용예

##### Camera Calibration 설정 예제

Unreal Engine 가상 카메라 엑터 초기 설정(BeginPlay 이벤트)에서 Camera Calibration View Transform을 얻어와 가상 카메라 엑터의 Transform으로 설정 한다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림22.png)



Unreal Engine 가상 카메라 엑터 초기 설정(BeginPlay 이벤트)에서 가상 카메라의 Field Of View를 설정 한다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림23.png)



설정된 Unreal Engine 가상 카메라 엑터를 View Target으로 설정 한다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림24.png)



##### ViveVR Tracker와 Unreal Engine 가상 카메라 엑터간 Transform 관계 설정

ViveVR Tracker ID 얻기

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림25.png)



ViveVR Tracker의 Position과 Orientation 값 얻기.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림26.png)



ViveVR Tracker의 Offset 이동 값 얻기

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림27.png)



ViveVR Tracker의 Offset 회전 값 얻기.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림28.png)



매 프레임 ViveVR Tracker의 이동 값과 회전 값을 Unreal Engine 가상 카메라 엑터에 적용 하기.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림29.png)



## 구현 내용

OpenCV API를 활용하여 Unreal Engine의 Camera Actor의 위치를 설정하는 기능에 대한 구현 내용을 정리한다.

ViveOpenCV 플러그인의 메인이 되는 코드는 UViveOpenCVDriver::FWorkerRunnable::Run() 함수이다. OpenCV API로 카메라가 촬영한 영상 데이터를 처리할 때 처리 시간이 다소 소요된다. 대부분 OpenCV API 함수들이 블로킹 함수들로써 함수를 실행하게 되면 함수가 종료되기전에는 다른 처리를 할 수가 없다.

그래서 Unreal Engine의 Thread System을 이용하여 OpenCV API가 영상 데이터를 처리 하는 부분을 병렬 처리 될 수 있도록 구현 하였다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림30.png)

[그림1] UViveOpenCVDriver::FWorkerRunnable::Run() 함수 도입 부분



![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림31.png)

[그림2] UViveOpenCVDriver::FWorkerRunnable::Run() 함수 중간 부분



![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림32.png)

[그림3]UViveOpenCVDriver::FWorkerRunnable::Run() 함수 끝 부분



위 코드를 보면 루프문 초기에 OpenCV를 사용하여 카메라 제어를 위한 초기 설정을 하고 설정이 끝나면 Chessboard 이미지를 사용하여 실제 카메라 Calibration을 진행하게 된다.

카메라 Calibration이 끝나게 되면 Marker 이미지를 사용하여 실제 카메라가 위치한 지점을 계산한다.

이러한 과정에서 촬용되는 영상 이미지를 Unreal Engine으로 전달하는 코드가 그림3에서 ParallelFor() 함수를 이용하여 전달하기 위한 이미지 버퍼에 픽셀 데이터를 옮기고 있으며, StoreWorkerFrame() 함수를 통해 Unreal Engine으로 전달된다.

그림2에서 calibrator->ProcessFrame() 함수에서 OpenCV API로 카메라 Calibration을 하기 위해 Chessboard 코너 검출을 하고 여러 위치에서의 Chessboard 코너 검출 데이터를 수집하여 수집된 데이터로 카메라 Calibration을 하게 된다.

그림4에서 빨간 박스 부분이 Chessboard 코너를 검출하여 수집하는 부분이다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림33.png)

[그림4] UViveCameraCalibrator::ProcessFrame() 함수



수집된 Chessboard 코너 검출 데이터들을 가지고 실제 카메라 Calibration 처리를 담당하는 함수를 호출하는 부분이 그림5에서 빨간색 박스 부분이다.

노란색 박스 부분은 정해진 갯수만큼의 Chessboard 코너 검출 데이터가 수집되지 않았었음 화면에 알려주는 코드이다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림34.png)

[그림5] UViveCameraCalibrator::ProcessFrame() 함수



카메라 Calibration 처리를 하는 부분은 그림6과 같다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림35.png)

[그림6] UViveCameraCalibrator::CalculateCalibration() 함수



카메라 Calibration 정보를 가지고 실제 카메라 위치를 계산하기 위해서는 Marker 이미지를 사용한다. Calibartion 정보를 입력 값으로 하고 Marker 이미지 영상을 분석해서 실제 카메라 위치를 계산한다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림36.png)

[그림7] UViveCameraCalibrator::EstimatePoseSingleMarker() 함수



계산된 실제 카메라 위치는 OpenCV 좌표 시스템 기반이기 때문에 Unreal Engine에서 바로 사용할 수가 없다. 그래서 OpenCV 좌표 시스템에서 Unreal Engine 좌표 시스템으로 변환하여 외부 파일로 저장 한다.

![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림37.png)

[그림8] FViveVideoCameraProperties::SavePoseCalibToFile() 함수 도입 부분



![](https://github.com/Devcoder-IndieWorks/ViveOpenCVPluginSample/blob/master/Images/그림38.png)

[그림9] FViveVideoCameraProperties::SavePoseCalibToFile() 함수 끝 부분



저장되는 파일은 텍스트 기반 파일로 속성 ID와 속성 값으로 구성되어 있다.
