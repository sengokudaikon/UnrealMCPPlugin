// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "UnrealMCPBridge.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeUnrealMCPBridge() {}

// ********** Begin Cross Module References ********************************************************
EDITORSUBSYSTEM_API UClass* Z_Construct_UClass_UEditorSubsystem();
UNREALMCP_API UClass* Z_Construct_UClass_UUnrealMCPBridge();
UNREALMCP_API UClass* Z_Construct_UClass_UUnrealMCPBridge_NoRegister();
UPackage* Z_Construct_UPackage__Script_UnrealMCP();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UUnrealMCPBridge *********************************************************
void UUnrealMCPBridge::StaticRegisterNativesUUnrealMCPBridge()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_UUnrealMCPBridge;
UClass* UUnrealMCPBridge::GetPrivateStaticClass()
{
	using TClass = UUnrealMCPBridge;
	if (!Z_Registration_Info_UClass_UUnrealMCPBridge.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("UnrealMCPBridge"),
			Z_Registration_Info_UClass_UUnrealMCPBridge.InnerSingleton,
			StaticRegisterNativesUUnrealMCPBridge,
			sizeof(TClass),
			alignof(TClass),
			TClass::StaticClassFlags,
			TClass::StaticClassCastFlags(),
			TClass::StaticConfigName(),
			(UClass::ClassConstructorType)InternalConstructor<TClass>,
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
			UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
			&TClass::Super::StaticClass,
			&TClass::WithinClass::StaticClass
		);
	}
	return Z_Registration_Info_UClass_UUnrealMCPBridge.InnerSingleton;
}
UClass* Z_Construct_UClass_UUnrealMCPBridge_NoRegister()
{
	return UUnrealMCPBridge::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UUnrealMCPBridge_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Editor subsystem for MCP Bridge\n * Handles communication between external tools and the Unreal Editor\n * through a TCP socket connection. Commands are received as JSON and\n * routed to appropriate command handlers.\n */" },
#endif
		{ "IncludePath", "UnrealMCPBridge.h" },
		{ "ModuleRelativePath", "Public/UnrealMCPBridge.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Editor subsystem for MCP Bridge\nHandles communication between external tools and the Unreal Editor\nthrough a TCP socket connection. Commands are received as JSON and\nrouted to appropriate command handlers." },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UUnrealMCPBridge>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UUnrealMCPBridge_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UEditorSubsystem,
	(UObject* (*)())Z_Construct_UPackage__Script_UnrealMCP,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UUnrealMCPBridge_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UUnrealMCPBridge_Statics::ClassParams = {
	&UUnrealMCPBridge::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UUnrealMCPBridge_Statics::Class_MetaDataParams), Z_Construct_UClass_UUnrealMCPBridge_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UUnrealMCPBridge()
{
	if (!Z_Registration_Info_UClass_UUnrealMCPBridge.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UUnrealMCPBridge.OuterSingleton, Z_Construct_UClass_UUnrealMCPBridge_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UUnrealMCPBridge.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UUnrealMCPBridge);
// ********** End Class UUnrealMCPBridge ***********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_d_Developer_unreal_mcp_MCPGameProject_Plugins_UnrealMCPPlugin_Source_UnrealMCP_Public_UnrealMCPBridge_h__Script_UnrealMCP_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UUnrealMCPBridge, UUnrealMCPBridge::StaticClass, TEXT("UUnrealMCPBridge"), &Z_Registration_Info_UClass_UUnrealMCPBridge, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UUnrealMCPBridge), 1510686656U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_d_Developer_unreal_mcp_MCPGameProject_Plugins_UnrealMCPPlugin_Source_UnrealMCP_Public_UnrealMCPBridge_h__Script_UnrealMCP_1097454756(TEXT("/Script/UnrealMCP"),
	Z_CompiledInDeferFile_FID_Users_d_Developer_unreal_mcp_MCPGameProject_Plugins_UnrealMCPPlugin_Source_UnrealMCP_Public_UnrealMCPBridge_h__Script_UnrealMCP_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_d_Developer_unreal_mcp_MCPGameProject_Plugins_UnrealMCPPlugin_Source_UnrealMCP_Public_UnrealMCPBridge_h__Script_UnrealMCP_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
