// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "RHI.h"
#include "RHICommandList.h"
#include "RHIStaticStates.h"
#include "RenderResource.h"
#include "PipelineStateCache.h"
#include "ShaderParameters.h"
#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"

class FViveDistortionCorrectionMapShaderVS : public FGlobalShader
{
    DECLARE_SHADER_TYPE( FViveDistortionCorrectionMapShaderVS, Global );
public:
    FViveDistortionCorrectionMapShaderVS() = default;
    FViveDistortionCorrectionMapShaderVS( const ShaderMetaType::CompiledShaderInitializerType& InInitializer );

    static bool ShouldCompilePermutation( const FGlobalShaderPermutationParameters& InParameters );
    template<typename TShaderRHIParamRef> 
    void SetParameters( FRHICommandList& InRHICmdList, const TShaderRHIParamRef InShaderRHI, const FGlobalShaderPermutationParameters& InShaderInputData );
};

//-----------------------------------------------------------------------------

class FViveDistortionCorrectionMapShaderPS : public FGlobalShader
{
    DECLARE_SHADER_TYPE( FViveDistortionCorrectionMapShaderPS, Global );
public:
    LAYOUT_FIELD( FShaderParameter, NormalizedPrincipalPointParameter );
    LAYOUT_FIELD( FShaderParameter, K1Parameter );
    LAYOUT_FIELD( FShaderParameter, K2Parameter );
    LAYOUT_FIELD( FShaderParameter, P1Parameter );
    LAYOUT_FIELD( FShaderParameter, P2Parameter );
    LAYOUT_FIELD( FShaderParameter, K3Parameter );

public:
    FViveDistortionCorrectionMapShaderPS() = default;
    FViveDistortionCorrectionMapShaderPS( const ShaderMetaType::CompiledShaderInitializerType& InInitializer );

    static bool ShouldCompilePermutation( const FGlobalShaderPermutationParameters& InParameters );

    template<typename TShaderRHIParamRef> 
    void SetParameters( FRHICommandListImmediate& InRHICmdList, const TShaderRHIParamRef InShaderRHI, 
        FVector2D InNormalizedPrincipalPointParameter, float InK1, float InK2, float InP1, float InP2, float InK3 )
    {
        SetShaderValue( InRHICmdList, InShaderRHI, K1Parameter, InK1 );
        SetShaderValue( InRHICmdList, InShaderRHI, K2Parameter, InK2 );
        SetShaderValue( InRHICmdList, InShaderRHI, P1Parameter, InP1 );
        SetShaderValue( InRHICmdList, InShaderRHI, P2Parameter, InP2 );
        SetShaderValue( InRHICmdList, InShaderRHI, K3Parameter, InK3 );
        SetShaderValue( InRHICmdList, InShaderRHI, NormalizedPrincipalPointParameter, InNormalizedPrincipalPointParameter );
    }
};
