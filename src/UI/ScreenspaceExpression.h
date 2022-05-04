#pragma once

#include <utility>
#include <type_traits>
#include <functional>
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

        constexpr struct ParentWidth_t {}   ParentWidth;
        constexpr struct ParentHeight_t {}  ParentHeight;
 
        constexpr struct ParentLeft_t {}    ParentLeft;
        constexpr struct ParentRight_t {}   ParentRight;
        constexpr struct ParentTop_t {}     ParentTop;
        constexpr struct ParentBottom_t {}  ParentBottom;

        constexpr struct ScreenCenter_t {
            struct ScreenCenterX_t {} x;
            struct ScreenCenterY_t {} y;
        } ScreenCenter;

        constexpr struct ParentCenter_t {
            struct ParentCenterX_t {} x;
            struct ParentCenterY_t {} y;
        } ParentCenter;

        template<typename Check, typename... Args>
        constexpr bool is_same_list_v = std::disjunction_v<std::is_same<Check, Args>...>;

        template<typename T>
        concept ScreenspaceModifier = is_same_list_v<
            T
            , ScreenWidth_t
            , ScreenHeight_t
            , ScreenLeft_t
            , ScreenTop_t
            , ParentWidth_t
            , ParentHeight_t
            , ParentLeft_t
            , ParentRight_t
            , ParentTop_t
            , ParentBottom_t
            , ScreenCenter_t::ScreenCenterX_t
            , ScreenCenter_t::ScreenCenterY_t
            , ParentCenter_t::ParentCenterX_t
            , ParentCenter_t::ParentCenterY_t
        >;

        template<typename T>
        concept Arithmetic = std::is_arithmetic_v<T>;

        template<typename Callable, typename L, typename R>
        struct ScreenspaceExpression {
            template<typename T>
            struct Node {
                T value;

                auto operator()(const Transform& transform) const {
                    return value(transform);
                }
            };

            template<ScreenspaceModifier T>
            struct Node<T> {
                T value;

                auto operator()(const Transform& transform) const {
                    if constexpr (std::is_same_v<T, ScreenWidth_t>)
                        return Application::GetWindowSize().x;
                    else if constexpr (std::is_same_v<T, ScreenHeight_t>)
                        return Application::GetWindowSize().y;
                    else if constexpr (std::is_same_v<T, ScreenLeft_t>)
                        return 0.f;
                    else if constexpr (std::is_same_v<T, ScreenTop_t>)
                        return 0.f;
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
                    else if constexpr (std::is_same_v<T, ScreenCenter_t::ScreenCenterX_t>)
                        return Application::GetWindowSize().x / 2;
                    else if constexpr (std::is_same_v<T, ScreenCenter_t::ScreenCenterY_t>)
                        return Application::GetWindowSize().y / 2;
                    else if constexpr (std::is_same_v<T, ParentCenter_t::ParentCenterX_t>)
                        return transform.position.x;
                    else if constexpr (std::is_same_v<T, ParentCenter_t::ParentCenterY_t>)
                        return transform.position.y;
                }
            };

            template<Arithmetic T>
            struct Node<T> {
                T value;

                const T& operator()(const Transform&) const {
                    return value;
                }
            };

            Node<L> left;
            Node<R> right;

            constexpr ScreenspaceExpression(const L& l, const R& r, Callable&&) : left{l}, right{r} { }

            auto operator()(const Transform& transform) const {
                return Callable{}(left(transform), right(transform));
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