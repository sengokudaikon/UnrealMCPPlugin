#pragma once
#include <variant>

namespace PropertyHandlers {
	struct FBoolHandler {
		const FBoolProperty* Prop;
		void* Addr;

		auto operator()(const TSharedPtr<FJsonValue>& Value, FString& Error) const -> bool {
			Prop->SetPropertyValue(Addr, Value->AsBool());
			return true;
		}
	};

	struct FIntHandler {
		const FIntProperty* Prop;
		UObject* Object;

		auto operator()(const TSharedPtr<FJsonValue>& Value, FString& Error) const -> bool {
			Prop->SetPropertyValue_InContainer(Object, static_cast<int32>(Value->AsNumber()));
			return true;
		}
	};

	struct FFloatHandler {
		const FFloatProperty* Prop;
		void* Addr;

		auto operator()(const TSharedPtr<FJsonValue>& Value, FString& Error) const -> bool {
			Prop->SetPropertyValue(Addr, static_cast<float>(Value->AsNumber()));
			return true;
		}
	};

	struct FStringHandler {
		const FStrProperty* Prop;
		void* Addr;

		auto operator()(const TSharedPtr<FJsonValue>& Value, FString& Error) const -> bool {
			Prop->SetPropertyValue(Addr, Value->AsString());
			return true;
		}
	};

	struct FByteHandler {
		const FByteProperty* Prop;
		void* Addr;
		FString PropertyName;

		auto operator()(const TSharedPtr<FJsonValue>& Value, FString& Error) const -> bool {
			// Regular byte property (no enum)
			Prop->SetPropertyValue(Addr, static_cast<uint8>(Value->AsNumber()));
			return true;
		}
	};

	struct FEnumByteHandler {
		const FByteProperty* Prop;
		const UEnum* EnumDef;
		void* Addr;
		FString PropertyName;

		auto operator()(const TSharedPtr<FJsonValue>& Value, FString& Error) const -> bool {
			return HandleEnumValue(Value,
			                       Error,
			                       [this](int64 Val) {
				                       Prop->SetPropertyValue(Addr, static_cast<uint8>(Val));
			                       });
		}

	private:
		auto HandleEnumValue(const TSharedPtr<FJsonValue>& Value,
		                     FString& Error,
		                     const TFunction<void(int64)>& Setter) const -> bool {
			// Numeric value
			if (Value->Type == EJson::Number) {
				const int64 EnumValue = static_cast<int64>(Value->AsNumber());
				Setter(EnumValue);
				UE_LOG(LogTemp,
				       Display,
				       TEXT("Setting enum property %s to numeric value: %lld"),
				       *PropertyName,
				       EnumValue);
				return true;
			}

			// String value
			if (Value->Type == EJson::String) {
				FString EnumValueName = Value->AsString();

				// Handle numeric strings
				if (EnumValueName.IsNumeric()) {
					const int64 EnumValue = FCString::Atoi64(*EnumValueName);
					Setter(EnumValue);
					UE_LOG(LogTemp,
					       Display,
					       TEXT("Setting enum property %s to numeric string: %s -> %lld"),
					       *PropertyName,
					       *EnumValueName,
					       EnumValue);
					return true;
				}

				// Handle qualified enum names (e.g., "EAutoReceiveInput::Player0")
				if (EnumValueName.Contains(TEXT("::"))) {
					EnumValueName.Split(TEXT("::"), nullptr, &EnumValueName);
				}

				// Try to find enum by name
				int64 EnumValue = EnumDef->GetValueByNameString(EnumValueName);
				if (EnumValue == INDEX_NONE) {
					EnumValue = EnumDef->GetValueByNameString(Value->AsString());
				}

				if (EnumValue != INDEX_NONE) {
					Setter(EnumValue);
					UE_LOG(LogTemp,
					       Display,
					       TEXT("Setting enum property %s to name: %s -> %lld"),
					       *PropertyName,
					       *EnumValueName,
					       EnumValue);
					return true;
				}

				// Log available options
				UE_LOG(LogTemp,
				       Warning,
				       TEXT("Could not find enum value for '%s'. Available options:"),
				       *EnumValueName);
				for (int32 i = 0; i < EnumDef->NumEnums(); i++) {
					UE_LOG(LogTemp,
					       Warning,
					       TEXT("  - %s (value: %lld)"),
					       *EnumDef->GetNameStringByIndex(i),
					       EnumDef->GetValueByIndex(i));
				}

				Error = FString::Printf(TEXT("Could not find enum value for '%s'"), *EnumValueName);
				return false;
			}

			Error = TEXT("Enum value must be a number or string");
			return false;
		}
	};

	struct FEnumHandler {
		const FEnumProperty* Prop;
		const UEnum* EnumDef;
		const FNumericProperty* NumericProp;
		void* Addr;
		FString PropertyName;

		auto operator()(const TSharedPtr<FJsonValue>& Value, FString& Error) const -> bool {
			return HandleEnumValue(Value,
			                       Error,
			                       [this](int64 Val) {
				                       NumericProp->SetIntPropertyValue(Addr, Val);
			                       });
		}

	private:
		auto HandleEnumValue(const TSharedPtr<FJsonValue>& Value,
		                     FString& Error,
		                     const TFunction<void(int64)>& Setter) const -> bool {
			// Numeric value
			if (Value->Type == EJson::Number) {
				const int64 EnumValue = static_cast<int64>(Value->AsNumber());
				Setter(EnumValue);
				UE_LOG(LogTemp,
				       Display,
				       TEXT("Setting enum property %s to numeric value: %lld"),
				       *PropertyName,
				       EnumValue);
				return true;
			}

			// String value
			if (Value->Type == EJson::String) {
				FString EnumValueName = Value->AsString();

				// Handle numeric strings
				if (EnumValueName.IsNumeric()) {
					const int64 EnumValue = FCString::Atoi64(*EnumValueName);
					Setter(EnumValue);
					UE_LOG(LogTemp,
					       Display,
					       TEXT("Setting enum property %s to numeric string: %s -> %lld"),
					       *PropertyName,
					       *EnumValueName,
					       EnumValue);
					return true;
				}

				// Handle qualified enum names
				if (EnumValueName.Contains(TEXT("::"))) {
					EnumValueName.Split(TEXT("::"), nullptr, &EnumValueName);
				}

				// Try to find enum by name
				int64 EnumValue = EnumDef->GetValueByNameString(EnumValueName);
				if (EnumValue == INDEX_NONE) {
					EnumValue = EnumDef->GetValueByNameString(Value->AsString());
				}

				if (EnumValue != INDEX_NONE) {
					Setter(EnumValue);
					UE_LOG(LogTemp,
					       Display,
					       TEXT("Setting enum property %s to name: %s -> %lld"),
					       *PropertyName,
					       *EnumValueName,
					       EnumValue);
					return true;
				}

				// Log available options
				UE_LOG(LogTemp,
				       Warning,
				       TEXT("Could not find enum value for '%s'. Available options:"),
				       *EnumValueName);
				for (int32 i = 0; i < EnumDef->NumEnums(); i++) {
					UE_LOG(LogTemp,
					       Warning,
					       TEXT("  - %s (value: %lld)"),
					       *EnumDef->GetNameStringByIndex(i),
					       EnumDef->GetValueByIndex(i));
				}

				Error = FString::Printf(TEXT("Could not find enum value for '%s'"), *EnumValueName);
				return false;
			}

			Error = TEXT("Enum value must be a number or string");
			return false;
		}
	};

	struct FUnsupportedHandler {
		FString PropertyTypeName;
		FString PropertyName;

		auto operator()(const TSharedPtr<FJsonValue>& Value, FString& Error) const -> bool {
			Error = FString::Printf(TEXT("Unsupported property type: %s for property %s"),
			                        *PropertyTypeName,
			                        *PropertyName);
			return false;
		}
	};

	using FPropertyHandler = std::variant<
		FBoolHandler,
		FIntHandler,
		FFloatHandler,
		FStringHandler,
		FByteHandler,
		FEnumByteHandler,
		FEnumHandler,
		FUnsupportedHandler
	>;

	struct FExecuteHandler {
		const TSharedPtr<FJsonValue>& Value;
		FString& Error;

		auto operator()(const auto& Handler) const -> bool {
			return Handler(Value, Error);
		}
	};
}
