#pragma once

#include "CoreMinimal.h"

/**
 * Handles focusing the editor viewport
 * Moves the viewport camera to focus on an actor or location
 */
class UNREALMCP_API FFocusViewport {
public:
	FFocusViewport() = default;
	~FFocusViewport() = default;

	/**
	 * Focuses the viewport on a target actor or location
	 *
	 * @param Params The JSON object containing parameters (target, location, distance, orientation)
	 * @return A JSON object containing confirmation or an error response
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};