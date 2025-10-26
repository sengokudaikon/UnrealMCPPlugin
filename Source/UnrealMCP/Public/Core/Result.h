#pragma once

#include "CoreMinimal.h"

namespace UnrealMCP
{
	/**
	 * Generic result type for operations that can fail
	 * Provides a type-safe way to return values or errors
	 */
	template<typename T>
	struct TResult
	{
		bool bSuccess;
		T Value;
		FString Error;

		/** Create a successful result */
		static TResult Success(T&& InValue)
		{
			return TResult{true, MoveTemp(InValue), FString()};
		}

		/** Create a successful result (copy) */
		static TResult Success(const T& InValue)
		{
			return TResult{true, InValue, FString()};
		}

		/** Create a failed result */
		static TResult Failure(const FString& InError)
		{
			return TResult{false, T(), InError};
		}

		/** Check if the operation succeeded */
		bool IsSuccess() const { return bSuccess; }

		/** Check if the operation failed */
		bool IsFailure() const { return !bSuccess; }

		/** Get the value (only valid if IsSuccess()) */
		const T& GetValue() const { return Value; }
		T& GetValue() { return Value; }

		/** Get the error message (only valid if IsFailure()) */
		const FString& GetError() const { return Error; }

		/** Convert to boolean (true if success) */
		explicit operator bool() const { return bSuccess; }
	};

	/**
	 * Specialized result for void operations (operations that don't return a value)
	 */
	struct FVoidResult
	{
		bool bSuccess;
		FString Error;

		/** Create a successful result */
		static FVoidResult Success()
		{
			return FVoidResult{true, FString()};
		}

		/** Create a failed result */
		static FVoidResult Failure(const FString& InError)
		{
			return FVoidResult{false, InError};
		}

		/** Check if the operation succeeded */
		bool IsSuccess() const { return bSuccess; }

		/** Check if the operation failed */
		bool IsFailure() const { return !bSuccess; }

		/** Get the error message (only valid if IsFailure()) */
		const FString& GetError() const { return Error; }

		/** Convert to boolean (true if success) */
		explicit operator bool() const { return bSuccess; }
	};
}
