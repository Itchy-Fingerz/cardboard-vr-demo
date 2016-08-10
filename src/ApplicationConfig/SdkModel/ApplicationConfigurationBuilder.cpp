// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "ApplicationConfigurationBuilder.h"

namespace Examples
{
    namespace ApplicationConfig
    {
        namespace SdkModel
        {
            ApplicationConfigurationBuilder::ApplicationConfigurationBuilder()
            : m_name("")
            , m_eegeoApiKey("")
            , m_interestLocation(0.0, 0.0, 0.0)
            , m_orientationDegrees(0.f)
            , m_coverageTreeManifestURL("")
            , m_themeManifestURL("")
            , m_productVersion("")
            , m_buildNumber("")
            , m_combinedVersionString("")
            , m_jumpPointsSpriteSheet("")
            , m_jumpPointsSpriteSheetSize(1)
            {

            }

            IApplicationConfigurationBuilder& ApplicationConfigurationBuilder::SetApplicationName(const std::string& name)
            {
                m_name = name;
                return *this;
            }

            IApplicationConfigurationBuilder& ApplicationConfigurationBuilder::SetEegeoApiKey(const std::string& eegeoApiKey)
            {
                m_eegeoApiKey = eegeoApiKey;
                return *this;
            }

            IApplicationConfigurationBuilder& ApplicationConfigurationBuilder::SetStartInterestPointLocation(const Eegeo::Space::LatLongAltitude& location)
            {
                m_interestLocation = location;
                return *this;
            }

            IApplicationConfigurationBuilder& ApplicationConfigurationBuilder::SetStartOrientationAboutInterestPoint(float degrees)
            {
                m_orientationDegrees = degrees;
                return *this;
            }

            IApplicationConfigurationBuilder& ApplicationConfigurationBuilder::SetCoverageTreeManifestURL(const std::string& coverageTreeManifestURL)
            {
                m_coverageTreeManifestURL = coverageTreeManifestURL;
                return *this;
            }

            IApplicationConfigurationBuilder& ApplicationConfigurationBuilder::SetThemeManifestURL(const std::string& themeManifestURL)
            {
                m_themeManifestURL = themeManifestURL;
                return *this;
            }

            IApplicationConfigurationBuilder& ApplicationConfigurationBuilder::SetProductVersion(const std::string& productVersion)
            {
                m_productVersion = productVersion;
                return *this;
            }

            IApplicationConfigurationBuilder& ApplicationConfigurationBuilder::SetBuildNumber(const std::string& buildNumber)
            {
                m_buildNumber = buildNumber;
                return *this;
            }

            IApplicationConfigurationBuilder& ApplicationConfigurationBuilder::SetCombinedVersionString(const std::string& combinedVersionString)
            {
                m_combinedVersionString = combinedVersionString;
                return *this;
            }

            IApplicationConfigurationBuilder& ApplicationConfigurationBuilder::SetJumpPointSpriteSheet(const std::string& spriteSheet)
            {
                m_jumpPointsSpriteSheet = spriteSheet;
                return *this;
            }

            IApplicationConfigurationBuilder& ApplicationConfigurationBuilder::SetJumpPointSpriteSheetSize(const int spriteSheetSize)
            {
                m_jumpPointsSpriteSheetSize = spriteSheetSize;
                return *this;
            }

            IApplicationConfigurationBuilder& ApplicationConfigurationBuilder::SetExteriorLocations(const TWorldLocations& exteriorLocations)
            {
                m_exteriorLocations = exteriorLocations;
                return *this;
            }

            IApplicationConfigurationBuilder& ApplicationConfigurationBuilder::AddExteriorJumpPoints(const std::string& locationName, const TJumpPointVector& jumpPoints)
            {
                m_exteriorJumpPoints[locationName] = jumpPoints;
                return *this;
            }

            IApplicationConfigurationBuilder& ApplicationConfigurationBuilder::AddInteriorJumpPoints(const std::string& locationName, const TInteriorFloorJumpPoints& jumpPoints)
            {
                m_interiorJumpPoints[locationName] = jumpPoints;
                return *this;
            }

            ApplicationConfiguration ApplicationConfigurationBuilder::Build()
            {
                return ApplicationConfiguration(m_name,
                                                m_eegeoApiKey,
                                                m_interestLocation,
                                                m_orientationDegrees,
                                                m_coverageTreeManifestURL,
                                                m_themeManifestURL,
                                                m_productVersion,
                                                m_buildNumber,
                                                m_combinedVersionString,
                                                m_jumpPointsSpriteSheet,
                                                m_jumpPointsSpriteSheetSize,
                                                m_exteriorLocations,
                                                m_exteriorJumpPoints,
                                                m_interiorJumpPoints);
            }
        }
    }
}
