#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "EditorAssetLibrary.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace UnrealMCPTest
{
    /**
     * Utility class for common test operations
     */
    class FTestUtils
    {
    public:
        /**
         * Generate a unique test name to avoid conflicts
         */
        static FString GenerateUniqueTestName(const FString& BaseName)
        {
            static int32 TestCounter = 0;
            return FString::Printf(TEXT("%s_%d_%f"), *BaseName, TestCounter++, FPlatformTime::Seconds());
        }

        /**
         * Get the standard test package path
         */
        static FString GetTestPackagePath()
        {
            return TEXT("/Game/AutomationTest/");
        }

        /**
         * Create a full asset path for testing
         */
        static FString GetTestAssetPath(const FString& AssetName)
        {
            return GetTestPackagePath() + AssetName;
        }

        /**
         * Clean up a test asset by path
         */
        static bool CleanupTestAsset(const FString& AssetPath)
        {
            if (UEditorAssetLibrary::DoesAssetExist(AssetPath))
            {
                return UEditorAssetLibrary::DeleteAsset(AssetPath);
            }
            return true; // Asset doesn't exist, so it's "clean"
        }

        /**
         * Test helper to verify two strings are equal with custom error message
         */
        static bool TestStringEqual(const FString& Expected, const FString& Actual,
                                   const FString& Context, FAutomationTestBase* Test)
        {
            if (Expected.Equals(Actual))
            {
                return true;
            }

            Test->AddError(FString::Printf(TEXT("%s: Expected '%s' but got '%s'"),
                                          *Context, *Expected, *Actual));
            return false;
        }

        /**
         * Test helper to verify a string contains a substring
         */
        static bool TestStringContains(const FString& Text, const FString& Substring,
                                      const FString& Context, FAutomationTestBase* Test)
        {
            if (Text.Contains(Substring))
            {
                return true;
            }

            Test->AddError(FString::Printf(TEXT("%s: Expected text to contain '%s' but it was '%s'"),
                                          *Context, *Substring, *Text));
            return false;
        }
    };
}

#endif // WITH_DEV_AUTOMATION_TESTS