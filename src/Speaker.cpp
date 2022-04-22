#include "Speaker.h"
#include <algorithm>
#include "Camera.h"
#include "Gameobject.h"
#include <ranges>
#include "Rect.h"
#include "Resources.h"
#include "Shape.h"
#include "soloud/soloud.h"
#include "soloud/soloud_wav.h"

using namespace Fastboi;

Speaker::Speaker(GORef&& go) : gameobject(std::move(go)), panner(nullptr) {
    SetPan<PanType::NONE>();
};

Speaker::Speaker(const Speaker& copy) : panning(copy.panning), panner(nullptr) {
    switch (panning) {
        case PanType::NONE:
            SetPan<PanType::NONE>();
            break;
        case PanType::CONSTANT:
            SetPan<PanType::CONSTANT>(static_cast<PanConstant*>(copy.panner.get())->panAmt);
            break;
        case PanType::POSITIONAL:
            SetPan<PanType::POSITIONAL>();
            break;
        case PanType::FUNCTION:
            SetPan<PanType::FUNCTION>(static_cast<PanFunction*>(copy.panner.get())->panFunc);
            break;
    }
}

void Speaker::Update() {
    if (soundsPlaying.size() == 0) return;

    PurgeInvalid();
    Soloud& soloud = Resources::GetSoloud();

    switch (panning) {
        case PanType::POSITIONAL: {
            float panVal = static_cast<PanPosition*>(panner.get())->GetPan(nullptr); // We can avoid a virtual call with static_cast

            for (const SoundData& sd : soundsPlaying) {
                soloud.setPan(sd.handle, panVal);
            }

            break;
        }
        case PanType::FUNCTION: {
            for (const SoundData& sd : soundsPlaying) {
                soloud.setPan(sd.handle, static_cast<PanFunction*>(panner.get())->GetPan(&sd));
            }

            break;
        }
        default:
            break;
    }
}

unsigned int Speaker::PlaySound(Wav& wav) {
    Soloud& soloud = Resources::GetSoloud();

    unsigned int handle = soloud.play(wav);
    const SoundData& sd = soundsPlaying.emplace_back(*this, handle, wav);

    soloud.setPan(handle, panner->GetPan(&sd));

    return handle;
}

void Speaker::PurgeInvalid() {
    soundsPlaying.erase(std::remove_if(soundsPlaying.begin(), soundsPlaying.end(), 
        [](const SoundData& sd) {
            return !Resources::GetSoloud().isValidVoiceHandle(sd.handle);
        })
    , soundsPlaying.end());
}

void Speaker::SetPanAllHandles(float pan) const {
    for (const auto& sd : soundsPlaying) {
        if (!Resources::GetSoloud().isValidVoiceHandle(sd.handle)) continue;

        Resources::GetSoloud().setPan(sd.handle, pan);
    }
}

float Speaker::PanPosition::GetPan(const SoundData* sd) const {
    const Position& pos = speaker->gameobject().transform->position;
    BoundingBox screen = Fastboi::GetCamera().GetVisibleBounds();

    const float screenCenter = (screen.upperBounds.x + screen.lowerBounds.x) / 2.f;
    // Get relative x where < -1 is past left edge, > 1 is past right edge
    const float relX = (pos.x - screenCenter) / (screen.upperBounds.x - screen.lowerBounds.x) * 2.f; 

    float pan = std::min(1.f, std::max(-1.f, relX));

    printf("Pan: %f\n", pan);

    return std::min(PanValues::RIGHT, std::max(PanValues::LEFT, relX)); // Clamp between [-1, 1]
}

