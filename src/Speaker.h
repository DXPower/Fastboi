#pragma once

#include <functional>
#include "GORef.h"
#include <memory>
#include "Resources.h"
#include <vector>

namespace Fastboi {
    enum class PanType {
        NONE,
        CONSTANT, // Panning is always a constant value
        POSITIONAL, // Panning is based on position on the screen
        FUNCTION // Panning is based on given lambda
    };

    struct PanValues {
        constexpr static float LEFT = -1.f;
        constexpr static float CENTER = 0.f;
        constexpr static float RIGHT = 1.f;
    };

    struct Speaker;

    struct SoundData {
        const Speaker* speaker; 
        unsigned int handle;
        Wav* wave;

        SoundData(const Speaker& s, const unsigned int h, Wav& w) : speaker(&s), handle(h), wave(&w) { };
    };

    struct Speaker {
        private:

        struct PanBase {
            const Speaker* speaker;
            
            PanBase(const Speaker* speaker) : speaker(speaker) { };
            virtual ~PanBase() { };
            virtual float GetPan(const SoundData* sd) const = 0;
        };

        struct PanNone : PanBase {
            PanNone(const Speaker* speaker) : PanBase(speaker) { };
            float GetPan(const SoundData*) const override { return 0.f; };
        };

        struct PanConstant : PanBase {
            float panAmt;

            PanConstant(const Speaker* speaker, float panAmt) : PanBase(speaker), panAmt(panAmt) { };
            float GetPan(const SoundData*) const override { return panAmt; };
        };

        struct PanPosition : PanBase {
            PanPosition(const Speaker* speaker) : PanBase(speaker) { };
            float GetPan(const SoundData* sd) const override;
        };

        using PanFunction_t = float(const SoundData& sd);

        struct PanFunction : PanBase {
            std::function<PanFunction_t> panFunc;

            PanFunction(const Speaker* speaker, const std::function<PanFunction_t> func) : PanBase(speaker), panFunc(func) { };
            float GetPan(const SoundData* sd) const override { return panFunc(*sd); };
        };

        public:
        GORef gameobject;

        private:
        std::unique_ptr<PanBase> panner;
        PanType panning;

        std::vector<SoundData> soundsPlaying;

        public:
        Speaker(GORef&& go);
        Speaker(const Speaker& copy);

        void Update();
        unsigned int PlaySound(Wav& wav);

        private:
        void SetPanAllHandles(float pan) const;
        void PurgeInvalid(); // Purge all invalid handles

        public:
        template<PanType Pan>
        requires (Pan == PanType::NONE)
        void SetPan() {
            panner = std::make_unique<PanNone>(this);
            panning = Pan;

            SetPanAllHandles(0);
        }

        template<PanType Pan>
        requires (Pan == PanType::CONSTANT)
        void SetPan(float panAmt) {
            panner = std::make_unique<PanConstant>(this, panAmt);
            panning = Pan;

            SetPanAllHandles(panAmt);
        }

        template<PanType Pan>
        requires (Pan == PanType::POSITIONAL)
        void SetPan() {
            panner = std::make_unique<PanPosition>(this);
            panning = Pan;

            SetPanAllHandles(panner->GetPan(nullptr));
        }

        template<PanType Pan>
        requires (Pan == PanType::FUNCTION)
        void SetPan(const std::function<PanFunction_t>& panFunc) {
            panner = std::make_unique<PanFunction>(this, panFunc);
            panning = Pan;

            // Don't set pan handle here because we can't fake it for the function expecting us, it will have to wait until Update().
        }
    };
}