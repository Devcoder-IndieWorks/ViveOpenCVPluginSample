#include "VideoSource/ViveVideoConfiguration.h"
#include "ViveStudiosUtilsPCH.h"

void FViveVideoConfiguration::SetupIdentifierAndDisplayName( const FString& InIdentifier, const FString& InSourceName, 
    const FString& InVariant )
{
    Identifier = FString::Printf( TEXT( "%s_%s" ), *InIdentifier, *InVariant );
    DisplayName = FText::Format( NSLOCTEXT( "VIVE", "VideoConfigurationDisplayName", "{0}: {1}" ), FText::FromString( InSourceName ), 
        FText::FromString( InVariant ) );
}
