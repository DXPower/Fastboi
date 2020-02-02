#include "Fastboi.h"

using namespace Fastboi;

namespace Slowboi {
    struct Button final : public Gameobject {
        Input::TargetedClickListener clickListener;

        Button(const Position& pos, const Size& size);

        private:
        void Click(const TargetedClickEvent& e) const;
    };
}