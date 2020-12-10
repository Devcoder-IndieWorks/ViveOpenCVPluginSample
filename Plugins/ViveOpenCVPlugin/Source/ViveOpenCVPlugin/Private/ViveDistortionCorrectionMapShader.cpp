#include "ViveDistortionCorrectionMapShader.h"
#include "ViveStudiosUtilsPCH.h"
#include "RHIStaticStates.h"

IMPLEMENT_SHADER_TYPE(, FViveDistortionCorrectionMapShaderVS, TEXT( "/ViveOpenCVPluginShaders/Private/DistortionCorrectionMapGeneration.usf" ), TEXT( "MainVS" ), SF_Vertex );
IMPLEMENT_SHADER_TYPE(, FViveDistortionCorrectionMapShaderPS, TEXT( "/ViveOpenCVPluginShaders/Private/DistortionCorrectionMapGeneration.usf" ), TEXT( "MainPS" ), SF_Pixel );

//-----------------------------------------------------------------------------

FViveDistortionCorrectionMapShaderVS::FViveDistortionCorrectionMapShaderVS( const ShaderMetaType::CompiledShaderInitializerType& InInitializer )
    : FGlobalShader( InInitializer )
{
}

bool FViveDistortionCorrectionMapShaderVS::ShouldCompilePermutation( const FGlobalShaderPermutationParameters& InParameters )
{
    return true;
}

template<typename TShaderRHIParamRef> 
void FViveDistortionCorrectionMapShaderVS::SetParameters( FRHICommandList& InRHICmdList, const TShaderRHIParamRef InShaderRHI, const FGlobalShaderPermutationParameters& InShaderInputData )
{
}

//-----------------------------------------------------------------------------

FViveDistortionCorrectionMapShaderPS::FViveDistortionCorrectionMapShaderPS( const ShaderMetaType::CompiledShaderInitializerType& InInitializer )
    : FGlobalShader( InInitializer )
{
    K1Parameter.Bind( InInitializer.ParameterMap, TEXT( "K1" ) );
    K2Parameter.Bind( InInitializer.ParameterMap, TEXT( "K2" ) );
    P1Parameter.Bind( InInitializer.ParameterMap, TEXT( "P1" ) );
    P2Parameter.Bind( InInitializer.ParameterMap, TEXT( "P2" ) );
    K3Parameter.Bind( InInitializer.ParameterMap, TEXT( "K3" ) );
    NormalizedPrincipalPointParameter.Bind( InInitializer.ParameterMap, TEXT( "NormalizedPrincipalPoint" ) );
}

bool FViveDistortionCorrectionMapShaderPS::ShouldCompilePermutation( const FGlobalShaderPermutationParameters& InParameters )
{
    return IsFeatureLevelSupported( InParameters.Platform, ERHIFeatureLevel::SM5 );
}
