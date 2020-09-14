#pragma once

#include <cstring>
#include <cstdio>

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
      printf("cstring hash: %lu\n", hash_cstring(p));
      return hash_cstring(p);
   };
};

struct cstring_eq {
   bool operator()(const char* const& lhs, const char* const& rhs) const {
      return std::strcmp(lhs, rhs) == 0;
   }
};

struct SDL_Rect;
struct SDL_FRect;

void print_rect(const SDL_Rect& rect);
void print_rect(const SDL_FRect& rect);