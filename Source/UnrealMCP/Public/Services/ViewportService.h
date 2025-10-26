#pragma once

#include "CoreMinimal.h"
#include "Core/Result.h"

namespace UnrealMCP {
	/**
	 * Service for editor viewport operations
	 * Handles viewport focus, camera control, and screenshot capture
	 * All methods are static - this is a stateless utility service
	 */
	class UNREALMCP_API FViewportService {
	public:
		/**
		 * Focus the editor viewport on a specific actor or location
		 *
		 * @param ActorName Optional name of actor to focus on
		 * @param Location Optional world location to focus on
		 * @return Success if viewport was focused, Failure with error message
		 */
		static auto FocusViewport(
			const TOptional<FString>& ActorName,
			const TOptional<FVector>& Location
		) -> FVoidResult;

		/**
		 * Take a screenshot of the current viewport
		 *
		 * @param FilePath Path where screenshot should be saved
		 * @return Success with the file path, Failure with error message
		 */
		static auto TakeScreenshot(const FString& FilePath) -> TResult<FString>;
	};
}
