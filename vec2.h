//
// My vec2 class!
//

typedef struct vec2_Struct
{
    float x;
    float y;
} vec2;

vec2 vec2_add(vec2 a, vec2 b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

vec2 vec2_subtract(vec2 a, vec2 b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

vec2 vec2_scalar_multiply(vec2 a, float b)
{
    a.x *= b;
    a.y *= b;
    return a;
}

float vec2_dot_product(vec2 a, vec2 b)
{
    return a.x * b.x + a.y * b.y; 
}

vec2 vec2_normalize(vec2 a)
{
    float scale = sqrt( a.x * a.x + a.y * a.y);
    a.x /= scale;
    a.y /= scale;

    return a;
}
