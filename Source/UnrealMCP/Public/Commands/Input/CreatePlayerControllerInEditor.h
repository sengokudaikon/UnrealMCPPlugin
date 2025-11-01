 , #pragma once

#include "CoreMinimal.h"

namespace UnrealMCP {
	/**
	 * Handles creating a PlayerController in the editor world for testing input mappings
	 */
	class UNREALMCP_API FCreatePlayerControllerInEditor {
	public:
		FCreatePlayerControllerInEditor() = default;

		~FCreatePlayerControllerInEditor() = default;

		/**
		 * Creates a PlayerController in the editor world and returns a JSON response.
		 *
		 * @param Params The JSON parameters (no parameters required for this command)
		 * @return A JSON object confirming the PlayerController was created or an error response
		 */
		static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
	};
}