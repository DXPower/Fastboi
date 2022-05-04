#pragma once

#include "ScreenspaceExpression.h"
#include "Screen.h"

namespace Fastboi {
    namespace UI {
        namespace Layouts {
            namespace ScaleBy {
                inline Scale Parent(float scale) {
                    return Scale { ParentWidth * scale, ParentHeight * scale };
                }

                inline Scale Parent(float scaleX, float scaleY) {
                    return Scale { ParentWidth * scaleX, ParentHeight * scaleY };
                }
                
                inline Scale Screen(float scale) {
                    return Scale { ScreenWidth * scale, ScreenHeight * scale };
                }

                inline Scale Screen(float scaleX, float scaleY) {
                    return Scale { ScreenWidth * scaleX, ScreenHeight * scaleY };
                }
            }

            namespace Center {
                constexpr struct {
                        decltype(ParentLeft + ParentWidth / 2) x = ParentLeft + ParentWidth / 2;
                        decltype(ParentTop + ParentHeight / 2) y = ParentTop + ParentHeight / 2;
                } Parent;

                constexpr struct {
                    decltype(ScreenWidth  / 2) x = ScreenWidth  / 2;
                    decltype(ScreenHeight / 2) y = ScreenHeight / 2;
                } Screen;
            }

            namespace PushLeft {
                constexpr auto Parent = ParentLeft + SelfWidth / 2;
                constexpr auto Screen = ScreenLeft + SelfWidth / 2;
            };

            namespace PushRight {
                constexpr auto Parent = ParentRight - SelfWidth / 2;
                constexpr auto Screen = ScreenRight - SelfWidth / 2;
            };

            namespace PushTop {
                constexpr auto Parent = ParentTop + SelfHeight / 2;
                constexpr auto Screen = ScreenTop + SelfHeight / 2;
            };

            namespace PushBottom {
                constexpr auto Parent = ParentBottom - SelfHeight / 2;
                constexpr auto Screen = ScreenBottom - SelfHeight / 2;
            };

            template<typename T>
            concept PositivePusher = is_same_list_v<
                  T
                , std::remove_const_t<decltype(PushLeft::Parent)>,   std::remove_const_t<decltype(PushLeft::Screen)>
                , std::remove_const_t<decltype(PushTop::Parent)>,    std::remove_const_t<decltype(PushTop::Screen)>
            >;

            template<typename T>
            concept NegativePusher = is_same_list_v<
                  T
                , std::remove_const_t<decltype(PushRight::Parent)>,  std::remove_const_t<decltype(PushRight::Screen)>
                , std::remove_const_t<decltype(PushBottom::Parent)>, std::remove_const_t<decltype(PushBottom::Screen)>
            >;

            template<typename T>
            concept Pusher = PositivePusher<T> or NegativePusher<T>;

            template<Pusher T>
            constexpr auto PushWithMargin(const T& pusher, float margin) {
                if constexpr (PositivePusher<T>)
                    return pusher + margin;
                else
                    return pusher - margin;
            }

            namespace PushTopLeft { 
                inline const Anchor Parent { PushLeft::Parent,  PushTop::Parent };
                inline const Anchor Screen { PushLeft::Screen,  PushTop::Screen };
            }

            namespace PushTopRight { 
                inline const Anchor Parent { PushRight::Parent, PushTop::Parent };
                inline const Anchor Screen { PushRight::Screen, PushTop::Screen };
            }

            namespace PushBottomLeft { 
                inline const Anchor Parent { PushLeft::Parent,  PushBottom::Parent };
                inline const Anchor Screen { PushLeft::Screen,  PushBottom::Screen };
            }

            namespace PushBottomRight { 
                inline const Anchor Parent { PushRight::Parent, PushBottom::Parent };
                inline const Anchor Screen { PushRight::Screen, PushBottom::Screen };
            }

            namespace PushLeftMiddle {
                inline const Anchor Parent {PushLeft::Parent, Center::Parent.y };
                inline const Anchor Screen {PushLeft::Screen, Center::Screen.y };
            }

            namespace PushRightMiddle {
                inline const Anchor Parent {PushRight::Parent, Center::Parent.y };
                inline const Anchor Screen {PushRight::Screen, Center::Screen.y };
            }

            namespace PushTopMiddle {
                inline const Anchor Parent {Center::Parent.x, PushTop::Parent };
                inline const Anchor Screen {Center::Screen.x, PushTop::Screen };
            }

            namespace PushBottomMiddle {
                inline const Anchor Parent {Center::Parent.x, PushBottom::Parent };
                inline const Anchor Screen {Center::Screen.x, PushBottom::Screen };
            }

            namespace Centered {
                inline const Anchor Parent { Center::Parent.x, Center::Parent.y };
                inline const Anchor Screen { Center::Screen.x, Center::Screen.y };
            }
        }
    }
}