#pragma once

#include <cstring>

#define GenHasFunction(CheckerName, CheckFunction) \
template <typename _T> \
class CheckerName \
{ \
    private: \
    struct yes { }; \
    struct no { }; \
 \
    template <typename C> static yes test(decltype(&C::CheckFunction)) ; \
    template <typename C> static no test(...); \
 \
    public: \
    enum { value = static_cast<int>(std::is_same_v<decltype(test<_T>(0)), yes>) }; \
}

#define HasFunction(CheckerName, CheckType) CheckerName<CheckType>::value

inline size_t hash_cstring(const char* p) {
   size_t result = 0;
   constexpr size_t prime = 31;

   for (size_t i = 0; p[i] != '\0'; i++) {
      result = p[i] + (result * prime);
   }

   return result;
}

struct cstring_hasher {
   std::size_t __attribute__((flatten)) operator()(const char* const& p) const {
      return hash_cstring(p);
   };
};

struct SDL_Rect;
struct SDL_FRect;

void print_rect(const SDL_Rect& rect);
void print_rect(const SDL_FRect& rect);