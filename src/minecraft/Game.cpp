#include "ColorComp.h"
#include "Events.h"
#include "Fastboi.h"
#include "FastboiComps.h"
#include "Game.h"
#include "Input.h"
#include "Player.h"
#include "Block.h" 
#include "Resources.h"
#include "SpriteRenderer.h"
#include "UI/CommonLayouts.h"
#include "World.h"
#include "BlockHighlighter.h"
#include "UI/Button.h"
#include "ClickTarget.h"

#include <random>
#include <iostream>

using namespace Fastboi;
using namespace Components;

static std::random_device rd;  //Will be used to obtain a seed for the random number engine
static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

struct RandomColorChanger {
    inline static std::uniform_int_distribution<> distrib{0, 255};

    Components::ColorComp* color = nullptr;
    Input::TargetedClickListener clickListener;

    void Start(const Gameobject& go) {
        color = &go.GetComponent<Components::ColorComp>();

        clickListener.Init(*go.transform, *go.renderer);
        clickListener.signal.connect<&RandomColorChanger::Click>(*this);
    }

    void Click(const TargetedClickEvent& e) {
        std::cout << "Clicked!\n";

        color->r = distrib(gen);
        color->g = distrib(gen);
        color->b = distrib(gen);
    }
};

void BtnClick(const TargetedClickEvent& e);
void TBtnClick(const TargetedClickEvent& e);

void Minecraft::LoadResources() {
    Resources::LoadImage("Blocks", "mc_blocks.png");
}

void Minecraft::InitGame() {
    LoadResources();
    
    World::BuildWorld();

    Gameobject& player = Instantiate<Player::Inst>(Position(0, -300));

    Gameobject& highlighter = Instantiate<BlockHighlighter::Inst>();

    using namespace UI;
    using namespace Layouts;

    // Gameobject& uiThingy = Instantiate<Gameobject>();
    // Gameobject& uiChild = Instantiate<Gameobject>("UI Thingy");

    // uiThingy.AddComponent<Components::ColorComp>(0, 0, 255, 255);
    // uiChild.AddComponent<Components::ColorComp>(255, 0, 0, 255);

    // auto& renderer1 = uiThingy.AddComponent<Components::BoxColorRenderer>(RenderData(RenderOrder::UI, 0));
    // auto& renderer2 = uiChild.AddComponent<Components::BoxColorRenderer>(RenderData(RenderOrder::UI, 10));
    
    // uiThingy.AddComponent<Transform>(Position(0, 0), Size(1, 1), 0_deg);

    // renderer1.screen = Screen(
    //     Anchor{ Center::Screen.x, PushWithMargin(PushTop::Screen, 25) },
    //     Scale{ ScreenWidth * 0.75f, ScreenConst(100.f) }
    // );

    // uiChild.AddComponent<Transform>(Position(0, 0), Size(1, 1), 0_deg);
    // uiChild.transform->Parent(*uiThingy.transform);

    // renderer2.screen = Screen(
    //     PushBottomMiddle::Parent,
    //     ScaleBy::Parent(0.9f, 0.5f)
    // );

    // uiChild.AddComponent<RandomColorChanger>();


    // Gameobject& btn = Instantiate<ColorButton>(ColorComp(209, 66, 245, 255));
    // btn.GetComponent<ClickTarget>().Signal().connect<&BtnClick>();

    // Gameobject& btn2 = Instantiate<TextureButton>(Resources::GetTexture("Blocks"));
    // btn2.transform->position += 100.f;

    // btn2.GetComponent<ClickTarget>().Signal().connect<&TBtnClick>();
    // btn2.GetComponent<SpriteRenderer>().cutout = Rect(0, 0, 16, 16);
}

void BtnClick(const TargetedClickEvent& e) {
    e.transform.gameobject().GetComponent<ColorComp>().r += 64;
}

void TBtnClick(const TargetedClickEvent& e) {
    e.transform.gameobject().GetComponent<SpriteRenderer>().cutout.x += 16;
}


