#include "FeelKitDebugView.h"

#include <cmath>
#include <cstdio>
#include <string>

namespace FeelKit {

static void debugLog(const char* msg) {
	if (Internal::g_debugLogCallback) {
		Internal::g_debugLogCallback(msg);
	} else {
		std::printf("%s", msg);
	}
}

void ShowHapticTimeline() {
	char buf[128];
	std::snprintf(buf, sizeof(buf), "--- Haptic Timeline (%zu entries) ---\n",
		Internal::g_hapticTimelineLog.size());
	debugLog(buf);
	for (size_t i = 0; i < Internal::g_hapticTimelineLog.size(); i++) {
		std::snprintf(buf, sizeof(buf), "  [%zu] %s\n", i, Internal::g_hapticTimelineLog[i].c_str());
		debugLog(buf);
	}
}

void ShowEffectDebugger() {
	auto log = [](const char* msg) { debugLog(msg); };
	char buf[256];
	log("--- Effect Debugger ---\n");
	std::snprintf(buf, sizeof(buf), "  Active Effects: %zu\n", Internal::g_activeEffects.size()); log(buf);
	std::snprintf(buf, sizeof(buf), "  Registered Effects: %zu\n", Internal::g_effectRegistry.size()); log(buf);
	std::snprintf(buf, sizeof(buf), "  Shake: power=%.2f remaining=%.3f\n",
		Internal::g_shakePower, Internal::g_shakeRemaining); log(buf);
	std::snprintf(buf, sizeof(buf), "  Flash: color=0x%08X remaining=%.3f\n",
		Internal::g_flashColor, Internal::g_flashRemaining); log(buf);
	std::snprintf(buf, sizeof(buf), "  HitStop: remaining=%.3f\n", Internal::g_hitStopRemaining); log(buf);
	std::snprintf(buf, sizeof(buf), "  SlowMotion: scale=%.2f remaining=%.3f\n",
		Internal::g_slowMoScale, Internal::g_slowMoRemaining); log(buf);
	std::snprintf(buf, sizeof(buf), "  MasterVolume: %.2f\n", Internal::g_masterVolume); log(buf);
	std::snprintf(buf, sizeof(buf), "  Decals: %zu\n", Internal::g_decalPool.size()); log(buf);
	std::snprintf(buf, sizeof(buf), "  Timelines: %zu\n", Internal::g_timelines.size()); log(buf);
	std::snprintf(buf, sizeof(buf), "  Waveforms: %zu\n", Internal::g_waveformRegistry.size()); log(buf);
	std::snprintf(buf, sizeof(buf), "  Emitters: %zu\n", Internal::g_emitters.size()); log(buf);
	std::snprintf(buf, sizeof(buf), "  Sequences: %zu\n", Internal::g_sequenceRegistry.size()); log(buf);
	std::snprintf(buf, sizeof(buf), "  Recording: %s\n",
		Internal::g_recordingBuffer.recording ? "active" : "inactive"); log(buf);
	std::snprintf(buf, sizeof(buf), "  OcclusionFactor: %.2f\n", Internal::g_occlusionFactor); log(buf);
}

void DrawAISenses(const AISenseDebugDesc& debugDesc) {
	char buf[256];
	std::snprintf(buf, sizeof(buf),
		"[FeelKit] DrawAISenses: pos=(%.1f,%.1f,%.1f) sight=%.1f hearing=%.1f alert=%d\n",
		debugDesc.position.x, debugDesc.position.y, debugDesc.position.z,
		debugDesc.sightRange, debugDesc.hearingRange, debugDesc.alertLevel);
	debugLog(buf);
}

void ApplyOcclusion(const Vec3& listenerPosition, const Vec3& sourcePosition) {
	float dx = sourcePosition.x - listenerPosition.x;
	float dy = sourcePosition.y - listenerPosition.y;
	float dz = sourcePosition.z - listenerPosition.z;
	float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
	float occlusionFactor = Internal::clampFloat(distance * 0.01f, 0.0f, 0.9f);

	Internal::g_occlusionFactor = 1.0f - occlusionFactor;

	char buf[256];
	std::snprintf(buf, sizeof(buf),
		"[FeelKit] ApplyOcclusion: distance=%.1f occlusion=%.2f factor=%.2f\n",
		distance, occlusionFactor, Internal::g_occlusionFactor);
	debugLog(buf);
}

} // namespace FeelKit
