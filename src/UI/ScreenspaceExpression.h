#pragma once

#include <utility>
#include <type_traits>
#include <functional>
#include <concepts>
#include "Application.h"
#include "Transform.h"

namespace Fastboi {
    namespace UI {
        constexpr struct ScreenWidth_t {}   ScreenWidth;
        constexpr struct ScreenHeight_t {}  ScreenHeight;
 
        constexpr struct ScreenLeft_t {}    ScreenLeft;
        constexpr auto                      ScreenRight  = ScreenWidth;
        constexpr struct ScreenTop_t {}     ScreenTop;
        constexpr auto                      ScreenBottom = ScreenHeight;

        constexpr struct SelfWidth_t {}     SelfWidth;
        constexpr struct SelfHeight_t {}    SelfHeight;

        constexpr struct ParentWidth_t {}   ParentWidth;
        constexpr struct ParentHeight_t {}  ParentHeight;
 
        constexpr struct ParentLeft_t {}    ParentLeft;
        constexpr struct ParentRight_t {}   ParentRight;
        constexpr struct ParentTop_t {}     ParentTop;
        constexpr struct ParentBottom_t {}  ParentBottom;

        template<typename Check, typename... Args>
        constexpr bool is_same_list_v = std::disjunction_v<std::is_same<Check, Args>...>;

        template<typename T>
        concept ScreenspaceModifier = is_same_list_v<
            T
            , ScreenWidth_t
            , ScreenHeight_t
            , ScreenLeft_t
            , ScreenTop_t
            , SelfWidth_t
            , SelfHeight_t
            , ParentWidth_t
            , ParentHeight_t
            , ParentLeft_t
            , ParentRight_t
            , ParentTop_t
            , ParentBottom_t
        >;

        template<typename T>
        concept Arithmetic = std::is_arithmetic_v<T>;

        template<typename Callable, typename L, typename R>
        struct ScreenspaceExpression {
            template<typename T>
            struct Node {
                T value;

                template<typename X = int>
                requires std::invocable<T, Size, const Transform&>
                auto operator()(Size selfSize, const Transform& transform) const {
                    return value(selfSize, transform);
                }
            };

            template<ScreenspaceModifier T>
            struct Node<T> {
                T value;

                auto operator()(Size selfSize, const Transform& transform) const {
                    if constexpr (std::is_same_v<T, ScreenWidth_t>)
                        return Application::GetWindowSize().x;
                    else if constexpr (std::is_same_v<T, ScreenHeight_t>)
                        return Application::GetWindowSize().y;
                    else if constexpr (std::is_same_v<T, ScreenLeft_t>)
                        return 0.f;
                    else if constexpr (std::is_same_v<T, ScreenTop_t>)
                        return 0.f;
                    else if constexpr (std::is_same_v<T, SelfWidth_t>)
                        return selfSize.x;
                    else if constexpr (std::is_same_v<T, SelfHeight_t>)
                        return selfSize.y;
                    else if constexpr (std::is_same_v<T, ParentWidth_t>)
                        return transform.size.x;
                    else if constexpr (std::is_same_v<T, ParentHeight_t>)
                        return transform.size.y;
                    else if constexpr (std::is_same_v<T, ParentLeft_t>)
                        return transform.position.x - transform.size.x/2;
                    else if constexpr (std::is_same_v<T, ParentRight_t>)
                        return transform.position.x + transform.size.x/2;
                    else if constexpr (std::is_same_v<T, ParentTop_t>)
                        return transform.position.y - transform.size.y/2;
                    else if constexpr (std::is_same_v<T, ParentBottom_t>)
                        return transform.position.y + transform.size.y/2;
                }
            };

            template<Arithmetic T>
            struct Node<T> {
                T value;

                const T& operator()(Size selfSize, const Transform&) const {
                    return value;
                }
            };

            Node<L> left;
            Node<R> right;

            constexpr ScreenspaceExpression(const L& l, const R& r, Callable&&) : left{l}, right{r} { }

            auto operator()(Size selfSize, const Transform& parent) const {
                return Callable{}(left(selfSize, parent), right(selfSize, parent));
            }
        };

        template<typename Func, typename L, typename R>
        ScreenspaceExpression(const L&, const R&, Func&&) -> ScreenspaceExpression<Func, L, R>;

        template<typename T>
        constexpr auto ScreenConst(T c) {
            return ScreenspaceExpression(c, 0.f, std::plus{});
        }

        namespace detail {
            template<typename T>
            struct IsScreenspaceExpression : std::false_type {};

            template<typename... Args>
            struct IsScreenspaceExpression<ScreenspaceExpression<Args...>> : std::true_type {};
        }

        template<typename T>
        concept ScreenspaceExpressionOperand = ScreenspaceModifier<T> || detail::IsScreenspaceExpression<T>::value;

        template<typename L, typename R>
        concept ScreenspaceExpressionBinOper = 
            (ScreenspaceExpressionOperand<L> && Arithmetic<R>)
            || (Arithmetic<L> && ScreenspaceExpressionOperand<R>)
            || (ScreenspaceExpressionOperand<L> && ScreenspaceExpressionOperand<R>);

        template<typename L, typename R>
        requires ScreenspaceExpressionBinOper<L, R>
        constexpr auto operator+(const L& lhs, const R& rhs) {
            return ScreenspaceExpression(lhs, rhs, std::plus{});
        } 

        template<typename L, typename R>
        requires ScreenspaceExpressionBinOper<L, R>
        constexpr auto operator-(const L& lhs, const R& rhs) {
            return ScreenspaceExpression(lhs, rhs, std::minus{});
        } 

        template<typename L, typename R>
        requires ScreenspaceExpressionBinOper<L, R>
        constexpr auto operator*(const L& lhs, const R& rhs) {
            return ScreenspaceExpression(lhs, rhs, std::multiplies{});
        } 

        template<typename L, typename R>
        requires ScreenspaceExpressionBinOper<L, R>
        constexpr auto operator/(const L& lhs, const R& rhs) {
            return ScreenspaceExpression(lhs, rhs, std::divides{});
        } 
    }
}