#pragma once

#include "CoreMinimal.h"
#include "Core/Result.h"
#include "Core/MCPTypes.h"

class UBlueprint;
class UClass;

namespace UnrealMCP
{
	/**
	 * Service for blueprint creation and compilation operations
	 * Handles the complete lifecycle of blueprint assets from creation to compilation
	 */
	class UNREALMCP_API FBlueprintCreationService
	{
	public:
		/**
		 * Create a new blueprint with the specified parent class
		 * @param Params Configuration for the new blueprint
		 * @return Result containing the created blueprint or an error message
		 */
		static auto CreateBlueprint(const FBlueprintCreationParams& Params) -> TResult<UBlueprint*>;

		/**
		 * Compile an existing blueprint by name
		 * @param BlueprintName The name of the blueprint to compile
		 * @return Result indicating success or compilation error
		 */
		static auto CompileBlueprint(const FString& BlueprintName) -> FVoidResult;

	private:
		/**
		 * Resolve a parent class name to a UClass instance
		 * Attempts to find the class in Engine or Game modules with fallback to AActor
		 * @param ParentClassName The name of the class (with or without 'A' prefix)
		 * @return Valid UClass pointer, defaults to AActor if not found
		 */
		static auto ResolveParentClass(const FString& ParentClassName) -> UClass*;
	};
} // namespace UnrealMCP
