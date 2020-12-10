#include "VideoScreen/ViveVideoScreenComponent.h"
#include "ViveStudiosUtilsPCH.h"
#include "VideoDriver/ViveVideoDriver.h"
#include "ViveLog.h"

UViveVideoScreenComponent::UViveVideoScreenComponent( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    UseGlobalDriver = true;
    ReplacementMaterial = nullptr;
    VideoDriver = nullptr;
    VideoMaterial = nullptr;

    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;

    bTickInEditor = false;
    bAutoRegister = true;
    bAutoActivate = true;

    Resolution = FIntPoint( 1920, 1080 );

    SetEnableGravity( false );
    SetCollisionEnabled( ECollisionEnabled::NoCollision );
    SetGenerateOverlapEvents( false );
}

void UViveVideoScreenComponent::BeginPlay()
{
    Super::BeginPlay();

    InitVideoMaterial();

    if ( UseGlobalDriver ) {
        if ( UViveVideoDriver::GetGlobalDriver() != nullptr )
            UseDriver( UViveVideoDriver::GetGlobalDriver() );

        UViveVideoDriver::OnVideoDriverInstanceChange.AddUniqueDynamic( this, &UViveVideoScreenComponent::UseDriver );
    }
}

void UViveVideoScreenComponent::InitVideoMaterial()
{
    if ( VideoMaterial != nullptr )
        return;

    VIVELOG( Log, TEXT( "#### Initialize video material. ####" ) );
    const int32 MATERIAL_INDEX = 0;
    auto material = GetMaterial( MATERIAL_INDEX );

    UTexture* textureParamValue = nullptr;
    if ( material->GetTextureParameterValue( FMaterialParameterInfo( TEXT( "VideoTexture" ) ), textureParamValue ) ) {

        auto dynamicMaterailInstance = Cast<UMaterialInstanceDynamic>( material );
        if ( dynamicMaterailInstance == nullptr ) {
            VIVELOG( Log, TEXT( "#### Create dynamic material instance. ####" ) );
            dynamicMaterailInstance = UMaterialInstanceDynamic::Create( material, this );
            SetMaterial( MATERIAL_INDEX, dynamicMaterailInstance );
        }

        VideoMaterial = dynamicMaterailInstance;
    }
}

void UViveVideoScreenComponent::UseDriver( UViveVideoDriver* InNewDriver )
{
    if ( VideoMaterial == nullptr )
        InitVideoMaterial();

    if ( VideoDriver != nullptr )
        VideoDriver->OnVideoPropertiesChagne.RemoveAll( this );

    VideoDriver = InNewDriver;

    if ( VideoDriver != nullptr ) {
        OnCameraPropertiesChange( InNewDriver );
        VideoDriver->OnVideoPropertiesChagne.AddUniqueDynamic( this, &UViveVideoScreenComponent::OnCameraPropertiesChange );
    }
    else {
        SetVideoMaterialActive( false );
    }
}

void UViveVideoScreenComponent::EndPlay( const EEndPlayReason::Type InEndPlayReason )
{
    UViveVideoDriver::OnVideoDriverInstanceChange.RemoveAll( this );

    Super::EndPlay( InEndPlayReason );
}

void UViveVideoScreenComponent::OnCameraPropertiesChange( UViveVideoDriver* InVideoDriver )
{
    bool newActive = InVideoDriver != nullptr && InVideoDriver->IsConnected();
    if ( newActive ) {
        SetResolution( VideoDriver->GetResolution() );
        SetSizeForFOV( VideoDriver->GetFieldOfView().X );
    }

    SetVideoMaterialActive( newActive );

    if ( (InVideoDriver != nullptr) && (InVideoDriver->GetOutputTexture() != nullptr) && (VideoMaterial != nullptr) ) {
        VideoMaterial->SetTextureParameterValue( FName( TEXT( "VideoTexture" ) ), InVideoDriver->GetOutputTexture() );
        VIVELOG( Log, TEXT( "#### Bind material texture parameter value. ####" ) );
    }
}

void UViveVideoScreenComponent::SetResolution( const FIntPoint& InResolution )
{
    if ( ensure( InResolution.GetMin() > 0 ) )
        Resolution = InResolution;
}

void UViveVideoScreenComponent::SetSizeForFOV( float InHorizontalFOV )
{
    InHorizontalFOV = FMath::IsNearlyZero( InHorizontalFOV ) ? 50.0f : InHorizontalFOV;

    auto distanceToOrigin = GetRelativeTransform().GetLocation().Size();

    float width = distanceToOrigin * 2.0f * FMath::Tan( FMath::DegreesToRadians( 0.5f * InHorizontalFOV ) );
    float height = width * float(Resolution.Y) / float(Resolution.X);
    SetRelativeScale3D( FVector( width/100.0f, height/100.0f, 1.0f ) );

    auto log = FString::Printf( TEXT( "FieldOfView Horizontal=%f -> Scale=%f X %f" ), 
        InHorizontalFOV, GetRelativeScale3D().X, GetRelativeScale3D().Y );
    VIVELOG( Log, TEXT( "#### Video screen resize info: %s ####" ), *log );
}

void UViveVideoScreenComponent::SetVideoMaterialActive( bool InNewActive )
{
    const int32 MATERIAL_INDEX = 0;
    if ( InNewActive && (VideoMaterial != nullptr) ) {
        SetMaterial( MATERIAL_INDEX, VideoMaterial );
        VIVELOG( Log, TEXT( "#### Set video material. ####" ) );
    }
    else if ( ReplacementMaterial != nullptr ) {
        SetMaterial( MATERIAL_INDEX, ReplacementMaterial );
        VIVELOG( Log, TEXT( "#### Set replacement material. ####" ) );
    }
}
