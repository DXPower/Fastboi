#pragma once

struct gjk_vec2 { float x; float y; };

struct gjk_vec2 tripleProduct (struct gjk_vec2 a, struct gjk_vec2 b, struct gjk_vec2 c);
struct gjk_vec2 averagePoint (const struct gjk_vec2 * vertices, size_t count);
size_t indexOfFurthestPoint (const struct gjk_vec2 * vertices, size_t count, struct gjk_vec2 d);
struct gjk_vec2 support (const struct gjk_vec2 * vertices1, size_t count1,
              const struct gjk_vec2 * vertices2, size_t count2, struct gjk_vec2 d);
int gjk (const struct gjk_vec2 * vertices1, size_t count1,
         const struct gjk_vec2 * vertices2, size_t count2,
         struct gjk_vec2 * finalSimplexOut);


