#pragma once

#include "CoreMinimal.h"

/**
 * Handles operations for creating UMG Widget Blueprints dynamically.
 */
class UNREALMCP_API FCreateUMGWidgetBlueprint {
public:
	FCreateUMGWidgetBlueprint() = default;
	~FCreateUMGWidgetBlueprint() = default;

	/**
	 * Processes the provided JSON parameters to create a UMG Widget Blueprint and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters required for widget blueprint creation.
	 * @return A JSON object containing details of the created widget blueprint or an error response if the operation fails.
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};