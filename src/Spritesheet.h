#pragma once

#include "Vec.h"
#include "Fastboi.h"
#include "FastboiComps.h"
#include <map>

namespace Fastboi {
    namespace Components {
        template<typename AnimationKey = int>
        struct Spritesheet final {
            struct Animation {
                const Veci origin;
                const Vecb count;
                const uint32_t ticksNecessary;
                float ticksMultiplier;

                Rect window;
                uint8_t currentIteration;

                Animation(const Veci& origin, const Vec<int>& windowSize, const Vecb& count, uint32_t ticksNecessary)
                : origin(origin)
                , count(count)
                , ticksNecessary(ticksNecessary)
                , window(Rect(0, 0, windowSize.x, windowSize.y)) 
                , currentIteration(0)
                , ticksMultiplier(1.f)
                { };

                Animation() : Animation(Veci::zero(), Vec<int>::zero(), Vecb::zero(), 0) { };

                void Next() {
                    // Loop back to iteration 0 if we go over the number of tiles in the animation
                    if (++currentIteration >= count.x * count.y)
                        currentIteration = 0;

                    int curX = currentIteration % (count.x + 1);
                    int curY = currentIteration / (count.x + 1);

                    window.x = origin.x + (window.w * curX);
                    window.y = origin.y + (window.h * curY);
                }

                void Reset() {
                    currentIteration = 0;
                    ticksMultiplier = 1.f;
                    window.x = origin.x;
                    window.y = origin.y;
                }
            };

            Fastboi::GORef gameobject;
            SpriteRenderer* renderer;
            bool paused;

            private:
            std::map<AnimationKey, Animation> animations;
            AnimationKey currentAnimation;
            uint32_t count = 0;

            inline void UpdateWindow() const {
                if (renderer != nullptr)
                    renderer->cutout = GetCurrentAnimation().window;
            }

            public:
            Spritesheet(GORef&& gameobject)
            : gameobject(std::move(gameobject))
            , renderer(nullptr)
            , currentAnimation(AnimationKey())
            , paused(false) { };

            ~Spritesheet() = default;

            void Start() {
                using Reqs = RequiredComponents<SpriteRenderer>;

                if (!Reqs::HasRequiredComponents(gameobject())) {
                    Application::ThrowRuntimeException(
                        "Spritesheet missing reqs:", 
                        Application::REQUIREMENTS_NOT_FULFILLED,
                        Reqs::GetMissingNamesString(gameobject()).c_str());
                }

                renderer = &gameobject().template GetComponent<SpriteRenderer>();

                UpdateWindow();
            }

            void Update() {
                if (!paused) {
                    if (++count > (GetCurrentAnimation().ticksNecessary * GetCurrentAnimation().ticksMultiplier)) {
                        count = 0;
                        GetCurrentAnimation().Next();
                        UpdateWindow();
                    }
                }
            }

            void SetCurrentAnimation(const AnimationKey& key) {
                currentAnimation = key;

                GetCurrentAnimation().Reset();

                UpdateWindow();
                count = 0;
            }

            Animation& GetCurrentAnimation() {
                return animations.at(currentAnimation);
            }
            
            const Animation& GetCurrentAnimation() const {
                return animations.at(currentAnimation);
            }

            void AddAnimation(const AnimationKey& key, const Animation& animation) {
                animations.emplace(key, animation);
            }
        };
    };
};