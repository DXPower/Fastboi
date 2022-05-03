#include "Fastboi.h"
#include "FastboiComps.h"
#include "Game.h"
#include "Player.h"
#include "Block.h" 
#include "World.h"
#include "BlockHighlighter.h"

using namespace Fastboi;

void Minecraft::InitGame() {
    World::BuildWorld();

    Gameobject& player = Instantiate<Player::Inst>(Position(0, -300));

    Gameobject& highlighter = Instantiate<BlockHighlighter::Inst>();
}