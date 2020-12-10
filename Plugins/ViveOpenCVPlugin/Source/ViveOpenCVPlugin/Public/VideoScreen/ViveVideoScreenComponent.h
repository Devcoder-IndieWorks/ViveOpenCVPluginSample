// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "Components/StaticMeshComponent.h"
#include "ViveVideoScreenComponent.generated.h"

UCLASS( Blueprintable, BlueprintType, ClassGroup=(ViveStudiosUtils) ) 
class VIVEOPENCVPLUGIN_API UViveVideoScreenComponent : public UStaticMeshComponent
{
    GENERATED_UCLASS_BODY()
public:
    UFUNCTION( BlueprintCallable, Category="VideoScreen|Functions" )
    void SetSizeForFOV( float InHorizontalFOV );

    UFUNCTION()
    void OnCameraPropertiesChange( class UViveVideoDriver* InVideoDriver );

    //-------------------------------------------------------------------------

    void BeginPlay() override;
    void EndPlay( const EEndPlayReason::Type InEndPlayReason ) override;

protected:
    UFUNCTION()
    void UseDriver( class UViveVideoDriver* InVideoDriver );

    void InitVideoMaterial();
    void SetVideoMaterialActive( bool InNewActive );

    void SetResolution( const FIntPoint& InResolution );

public:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category="VideoScreen|Settings" )
    bool UseGlobalDriver;
    UPROPERTY( EditAnywhere, BlueprintReadOnly, Category="VideoScreen|Settings" )
    UMaterial* ReplacementMaterial;

    //-------------------------------------------------------------------------

    UPROPERTY( Transient, BlueprintReadOnly, Category="VideoScreen|FieldData" )
    class UViveVideoDriver* VideoDriver;
    UPROPERTY( Transient, BlueprintReadOnly, Category="VideoScreen|FieldData" )
    UMaterialInstanceDynamic* VideoMaterial;

private:
    FIntPoint Resolution;
};
