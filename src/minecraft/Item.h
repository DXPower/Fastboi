#include "Fastboi.h"
#include "Block.h"

namespace Minecraft {
    struct ItemSpriteInfo {
        Texture texture;
        Rect cutout;
    };

    struct Stack {
        int curSize;
        int maxSize;

        virtual ~Stack() = default;

        bool AddToStack();

        virtual void UseItem() = 0;
        virtual ItemSpriteInfo GetTexture() const = 0;
    };

    struct BlockStack final : Stack {
        BlockType type;
        ItemSpriteInfo sprite;

        void UseItem() override;
        ItemSpriteInfo GetTexture() const override;
    };
}