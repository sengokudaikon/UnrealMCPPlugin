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

	struct FStructHandler {
		const FStructProperty* Prop;
		void* Addr;
		FString PropertyName;

		auto operator()(const TSharedPtr<FJsonValue>& Value, FString& Error) const -> bool {
			// Handle FVector, FRotator, FTransform, etc.
			if (Value->Type == EJson::Array) {
				const TArray<TSharedPtr<FJsonValue>>& Array = Value->AsArray();

				// Vector/Rotator handling (3 components)
				if (Array.Num() == 3) {
					if (Prop->Struct == TBaseStructure<FVector>::Get()) {
						FVector VectorValue(
							static_cast<float>(Array[0]->AsNumber()),
							static_cast<float>(Array[1]->AsNumber()),
							static_cast<float>(Array[2]->AsNumber())
						);
						Prop->InitializeValue_InContainer(Addr);
						*static_cast<FVector*>(Addr) = VectorValue;
						UE_LOG(LogTemp, Display, TEXT("Setting FVector property %s to [%f, %f, %f]"),
						       *PropertyName, VectorValue.X, VectorValue.Y, VectorValue.Z);
						return true;
					}

					if (Prop->Struct == TBaseStructure<FRotator>::Get()) {
						FRotator RotatorValue(
							static_cast<float>(Array[0]->AsNumber()),
							static_cast<float>(Array[1]->AsNumber()),
							static_cast<float>(Array[2]->AsNumber())
						);
						Prop->InitializeValue_InContainer(Addr);
						*static_cast<FRotator*>(Addr) = RotatorValue;
						UE_LOG(LogTemp, Display, TEXT("Setting FRotator property %s to [%f, %f, %f]"),
						       *PropertyName, RotatorValue.Pitch, RotatorValue.Yaw, RotatorValue.Roll);
						return true;
					}

					if (Prop->Struct == TBaseStructure<FColor>::Get()) {
						FColor ColorValue(
							static_cast<uint8>(Array[0]->AsNumber()),
							static_cast<uint8>(Array[1]->AsNumber()),
							static_cast<uint8>(Array[2]->AsNumber()),
							255 // Default alpha
						);
						Prop->InitializeValue_InContainer(Addr);
						*static_cast<FColor*>(Addr) = ColorValue;
						UE_LOG(LogTemp, Display, TEXT("Setting FColor property %s to [%d, %d, %d, %d]"),
						       *PropertyName, ColorValue.R, ColorValue.G, ColorValue.B, ColorValue.A);
						return true;
					}
				}

				// Transform handling (9 components for rotation matrix + 3 for translation + 3 for scale)
				if (Array.Num() == 9) {
					if (Prop->Struct == TBaseStructure<FTransform>::Get()) {
						// Assuming format: [Roll, Pitch, Yaw, TranslateX, TranslateY, TranslateZ, ScaleX, ScaleY, ScaleZ]
						FRotator Rotation(
							static_cast<float>(Array[1]->AsNumber()), // Pitch
							static_cast<float>(Array[0]->AsNumber()), // Yaw
							static_cast<float>(Array[2]->AsNumber())  // Roll
						);
						FVector Translation(
							static_cast<float>(Array[3]->AsNumber()),
							static_cast<float>(Array[4]->AsNumber()),
							static_cast<float>(Array[5]->AsNumber())
						);
						FVector Scale3D(
							static_cast<float>(Array[6]->AsNumber()),
							static_cast<float>(Array[7]->AsNumber()),
							static_cast<float>(Array[8]->AsNumber())
						);

						FTransform TransformValue(Rotation, Translation, Scale3D);
						Prop->InitializeValue_InContainer(Addr);
						*static_cast<FTransform*>(Addr) = TransformValue;
						UE_LOG(LogTemp, Display, TEXT("Setting FTransform property %s"), *PropertyName);
						return true;
					}
				}

				// Color handling (4 components with alpha)
				if (Array.Num() == 4) {
					if (Prop->Struct == TBaseStructure<FColor>::Get()) {
						FColor ColorValue(
							static_cast<uint8>(Array[0]->AsNumber()),
							static_cast<uint8>(Array[1]->AsNumber()),
							static_cast<uint8>(Array[2]->AsNumber()),
							static_cast<uint8>(Array[3]->AsNumber())
						);
						Prop->InitializeValue_InContainer(Addr);
						*static_cast<FColor*>(Addr) = ColorValue;
						UE_LOG(LogTemp, Display, TEXT("Setting FColor property %s to [%d, %d, %d, %d]"),
						       *PropertyName, ColorValue.R, ColorValue.G, ColorValue.B, ColorValue.A);
						return true;
					}
				}

				// LinearColor handling (4 components)
				if (Array.Num() == 4) {
					if (Prop->Struct == TBaseStructure<FLinearColor>::Get()) {
						FLinearColor ColorValue(
							static_cast<float>(Array[0]->AsNumber()),
							static_cast<float>(Array[1]->AsNumber()),
							static_cast<float>(Array[2]->AsNumber()),
							static_cast<float>(Array[3]->AsNumber())
						);
						Prop->InitializeValue_InContainer(Addr);
						*static_cast<FLinearColor*>(Addr) = ColorValue;
						UE_LOG(LogTemp, Display, TEXT("Setting FLinearColor property %s to [%f, %f, %f, %f]"),
						       *PropertyName, ColorValue.R, ColorValue.G, ColorValue.B, ColorValue.A);
						return true;
					}
				}

				// Vector2D handling (2 components)
				if (Array.Num() == 2) {
					if (Prop->Struct == TBaseStructure<FVector2D>::Get()) {
						FVector2D Vector2DValue(
							static_cast<float>(Array[0]->AsNumber()),
							static_cast<float>(Array[1]->AsNumber())
						);
						Prop->InitializeValue_InContainer(Addr);
						*static_cast<FVector2D*>(Addr) = Vector2DValue;
						UE_LOG(LogTemp, Display, TEXT("Setting FVector2D property %s to [%f, %f]"),
						       *PropertyName, Vector2DValue.X, Vector2DValue.Y);
						return true;
					}

					if (Prop->Struct == TBaseStructure<FVector4>::Get()) {
						// Handle Vector4 with only 2 components (set Z and W to 0)
						FVector4 Vector4Value(
							static_cast<float>(Array[0]->AsNumber()),
							static_cast<float>(Array[1]->AsNumber()),
							0.0f,
							0.0f
						);
						Prop->InitializeValue_InContainer(Addr);
						*static_cast<FVector4*>(Addr) = Vector4Value;
						UE_LOG(LogTemp, Display, TEXT("Setting FVector4 property %s to [%f, %f, 0, 0]"),
						       *PropertyName, Vector4Value.X, Vector4Value.Y);
						return true;
					}
				}

				// Vector4 handling (4 components)
				if (Array.Num() == 4) {
					if (Prop->Struct == TBaseStructure<FVector4>::Get()) {
						FVector4 Vector4Value(
							static_cast<float>(Array[0]->AsNumber()),
							static_cast<float>(Array[1]->AsNumber()),
							static_cast<float>(Array[2]->AsNumber()),
							static_cast<float>(Array[3]->AsNumber())
						);
						Prop->InitializeValue_InContainer(Addr);
						*static_cast<FVector4*>(Addr) = Vector4Value;
						UE_LOG(LogTemp, Display, TEXT("Setting FVector4 property %s to [%f, %f, %f, %f]"),
						       *PropertyName, Vector4Value.X, Vector4Value.Y, Vector4Value.Z, Vector4Value.W);
						return true;
					}
				}
			}

			// Handle object format for structs
			else if (Value->Type == EJson::Object) {
				const TSharedPtr<FJsonObject>& Object = Value->AsObject();

				// Handle Transform with object format
				if (Prop->Struct == TBaseStructure<FTransform>::Get()) {
					FRotator Rotation(0.0f, 0.0f, 0.0f);
					FVector Translation(0.0f, 0.0f, 0.0f);
					FVector Scale3D(1.0f, 1.0f, 1.0f);

					// Extract rotation
					if (Object->HasTypedField<EJson::Array>(TEXT("rotation"))) {
						const TArray<TSharedPtr<FJsonValue>>& RotArray = Object->GetArrayField(TEXT("rotation"));
						if (RotArray.Num() >= 3) {
							Rotation = FRotator(
								static_cast<float>(RotArray[1]->AsNumber()), // Pitch
								static_cast<float>(RotArray[0]->AsNumber()), // Yaw
								static_cast<float>(RotArray[2]->AsNumber())  // Roll
							);
						}
					}

					// Extract translation
					if (Object->HasTypedField<EJson::Array>(TEXT("location"))) {
						const TArray<TSharedPtr<FJsonValue>>& LocArray = Object->GetArrayField(TEXT("location"));
						if (LocArray.Num() >= 3) {
							Translation = FVector(
								static_cast<float>(LocArray[0]->AsNumber()),
								static_cast<float>(LocArray[1]->AsNumber()),
								static_cast<float>(LocArray[2]->AsNumber())
							);
						}
					}

					// Extract scale
					if (Object->HasTypedField<EJson::Array>(TEXT("scale"))) {
						const TArray<TSharedPtr<FJsonValue>>& ScaleArray = Object->GetArrayField(TEXT("scale"));
						if (ScaleArray.Num() >= 3) {
							Scale3D = FVector(
								static_cast<float>(ScaleArray[0]->AsNumber()),
								static_cast<float>(ScaleArray[1]->AsNumber()),
								static_cast<float>(ScaleArray[2]->AsNumber())
							);
						}
					}

					FTransform TransformValue(Rotation, Translation, Scale3D);
					Prop->InitializeValue_InContainer(Addr);
						*static_cast<FTransform*>(Addr) = TransformValue;
					UE_LOG(LogTemp, Display, TEXT("Setting FTransform property %s from object format"), *PropertyName);
					return true;
				}
			}

			// Log supported struct types for debugging
			FString SupportedTypes = TEXT("Supported struct types: FVector, FRotator, FTransform, FColor, FLinearColor, FVector2D, FVector4");
			UE_LOG(LogTemp, Warning, TEXT("Unsupported struct format for %s. %s"), *PropertyName, *SupportedTypes);

			Error = FString::Printf(TEXT("Unsupported struct format for %s. Expected array format with 2-4 or 9 elements, or object format for Transform"), *PropertyName);
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
		FStructHandler,
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
