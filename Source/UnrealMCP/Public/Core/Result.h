#pragma once

#include "CoreMinimal.h"

namespace UnrealMCP {
	/**
	 * Generic result type for operations that can fail
	 * Provides a type-safe way to return values or errors
	 */
	template <typename T>
	struct TResult {
		bool bSuccess;
		T Value;
		FString Error;

		/** Create a successful result */
		static auto Success(T&& InValue) -> TResult {
			return TResult{true, MoveTemp(InValue), FString()};
		}

		/** Create a successful result (copy) */
		static auto Success(const T& InValue) -> TResult {
			return TResult{true, InValue, FString()};
		}

		/** Create a failed result */
		static auto Failure(const FString& InError) -> TResult {
			return TResult{false, T(), InError};
		}

		/** Check if the operation succeeded */
		auto IsSuccess() const -> bool {
			return bSuccess;
		}

		/** Check if the operation failed */
		auto IsFailure() const -> bool {
			return !bSuccess;
		}

		/** Get the value (only valid if IsSuccess()) */
		auto GetValue() const -> const T& {
			return Value;
		}

		auto GetValue() -> T& {
			return Value;
		}

		/** Get the error message (only valid if IsFailure()) */
		auto GetError() const -> const FString& {
			return Error;
		}

		/** Convert to boolean (true if success) */
		explicit operator bool() const {
			return bSuccess;
		}
	};

	/**
	 * Specialized result for void operations (operations that don't return a value)
	 */
	struct FVoidResult {
		bool bSuccess;
		FString Error;

		/** Create a successful result */
		static auto Success() -> FVoidResult {
			return FVoidResult{true, FString()};
		}

		/** Create a failed result */
		static auto Failure(const FString& InError) -> FVoidResult {
			return FVoidResult{false, InError};
		}

		/** Check if the operation succeeded */
		auto IsSuccess() const -> bool {
			return bSuccess;
		}

		/** Check if the operation failed */
		auto IsFailure() const -> bool {
			return !bSuccess;
		}

		/** Get the error message (only valid if IsFailure()) */
		auto GetError() const -> const FString& {
			return Error;
		}

		/** Convert to boolean (true if success) */
		explicit operator bool() const {
			return bSuccess;
		}
	};
}
