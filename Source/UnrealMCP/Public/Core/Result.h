#pragma once

#include "CoreMinimal.h"
#include "ErrorTypes.h"

namespace UnrealMCP {
	/**
	 * Generic result type for operations that can fail
	 * Provides a type-safe way to return values or errors
	 */
	template <typename T>
	struct TResult {
		bool bSuccess;
		T Value;
		FError Error;

		/** Create a successful result */
		static auto Success(T&& InValue) -> TResult {
			return TResult{true, MoveTemp(InValue), FError()};
		}

		/** Create a successful result (copy) */
		static auto Success(const T& InValue) -> TResult {
			return TResult{true, InValue, FError()};
		}

		/** Create a failed result with error code */
		static auto Failure(EErrorCode ErrorCode) -> TResult {
			return TResult{false, T(), FError(ErrorCode)};
		}

		/** Create a failed result with error code and context */
		static auto Failure(EErrorCode ErrorCode, const FString& Context) -> TResult {
			return TResult{false, T(), FError(ErrorCode, Context)};
		}

		/** Create a failed result with error code, context, and details */
		static auto Failure(EErrorCode ErrorCode, const FString& Context, const FString& Details) -> TResult {
			return TResult{false, T(), FError(ErrorCode, Context, Details)};
		}

		/** Legacy: Create a failed result from string (for backward compatibility) */
		static auto Failure(const FString& InError) -> TResult {
			return TResult{false, T(), FError(EErrorCode::Unknown, InError)};
		}

		/** Create a failed result from an FError */
		static auto Failure(const FError& InError) -> TResult {
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

		/** Get the error (only valid if IsFailure()) */
		auto GetError() const -> const FError& {
			return Error;
		}

		/** Get the error code (only valid if IsFailure()) */
		auto GetErrorCode() const -> EErrorCode {
			return Error.Code;
		}

		/** Get the error message for logging/debugging (tests should use GetErrorCode()) */
		auto GetErrorMessage() const -> FString {
			return Error.GetMessage();
		}

		/** Legacy: Get error as string (for backward compatibility) */
		auto GetErrorString() const -> FString {
			return Error.GetMessage();
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
		FError Error;

		/** Create a successful result */
		static auto Success() -> FVoidResult {
			return FVoidResult{true, FError()};
		}

		/** Create a failed result with error code */
		static auto Failure(EErrorCode ErrorCode) -> FVoidResult {
			return FVoidResult{false, FError(ErrorCode)};
		}

		/** Create a failed result with error code and context */
		static auto Failure(EErrorCode ErrorCode, const FString& Context) -> FVoidResult {
			return FVoidResult{false, FError(ErrorCode, Context)};
		}

		/** Create a failed result with error code, context, and details */
		static auto Failure(EErrorCode ErrorCode, const FString& Context, const FString& Details) -> FVoidResult {
			return FVoidResult{false, FError(ErrorCode, Context, Details)};
		}

		/** Legacy: Create a failed result from string (for backward compatibility) */
		static auto Failure(const FString& InError) -> FVoidResult {
			return FVoidResult{false, FError(EErrorCode::Unknown, InError)};
		}

		/** Create a failed result from an FError */
		static auto Failure(const FError& InError) -> FVoidResult {
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

		/** Get the error (only valid if IsFailure()) */
		auto GetError() const -> const FError& {
			return Error;
		}

		/** Get the error code (only valid if IsFailure()) */
		auto GetErrorCode() const -> EErrorCode {
			return Error.Code;
		}

		/** Get the error message for logging/debugging (tests should use GetErrorCode()) */
		auto GetErrorMessage() const -> FString {
			return Error.GetMessage();
		}

		/** Legacy: Get error as string (for backward compatibility) */
		auto GetErrorString() const -> FString {
			return Error.GetMessage();
		}

		/** Convert to boolean (true if success) */
		explicit operator bool() const {
			return bSuccess;
		}
	};
}
