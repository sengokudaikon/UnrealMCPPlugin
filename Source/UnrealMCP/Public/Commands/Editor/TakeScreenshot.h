#pragma once

#include "CoreMinimal.h"

/**
 * Handles taking screenshots of the editor viewport
 * Demonstrates proper delegation to ViewportService
 */
class UNREALMCP_API FTakeScreenshot {
public:
	FTakeScreenshot() = default;
	~FTakeScreenshot() = default;

	/**
	 * Processes the provided JSON parameters to take a screenshot and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters (filepath)
	 * @return A JSON object containing the screenshot filepath or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};
