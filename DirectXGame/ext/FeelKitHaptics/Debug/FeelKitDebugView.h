#pragma once

#include "../core/FeelKitCore.h"

namespace FeelKit {

void ShowHapticTimeline();
void ShowEffectDebugger();
void DrawAISenses(const AISenseDebugDesc& debugDesc);
void ApplyOcclusion(const Vec3& listenerPosition, const Vec3& sourcePosition);

} // namespace FeelKit