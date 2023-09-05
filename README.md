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
