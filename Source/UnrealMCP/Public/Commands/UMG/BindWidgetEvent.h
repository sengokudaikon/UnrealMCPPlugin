#pragma once

#include "CoreMinimal.h"

/**
 * Handles operations for binding events to widgets in UMG Widget Blueprints.
 */
class UNREALMCP_API FBindWidgetEvent {
public:
	FBindWidgetEvent() = default;
	~FBindWidgetEvent() = default;

	/**
	 * Processes the provided JSON parameters to bind an event to a widget and returns a JSON response.
	 *
	 * @param Params The JSON object containing parameters required for binding the widget event.
	 * @return A JSON object containing details of the event binding or an error response if the operation fails.
	 */
	static auto Handle(const TSharedPtr<FJsonObject>& Params) -> TSharedPtr<FJsonObject>;
};