//
//  My 3d vector class!
//

typedef struct vec3_Struct 
{
    float x;
    float y;
    float z;
} vec3;

vec3 vec3_add(vec3 a, vec3 b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

vec3 vec3_subtract(vec3 a, vec3 b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

vec3 vec3_scalar_multiply(vec3 a, float b)
{
    a.x *= b;
    a.y *= b;
    a.z *= b;
    return a;
}

vec3 vec3_cross_product(vec3 a, vec3 b)
{
    vec3 temp;
    temp.x = a.y * b.z - a.z * b.y;  
    temp.y = a.z * b.x - a.x * b.z;
    temp.z = a.x * b.y - a.y * b.x;
    return temp;
}

vec3 vec3_dot_product(vec3 a, vec3 b)
{
    vec3 temp;
    temp.x = a.x * b.x;
    temp.y = a.y * b.y;
    temp.z = a.z * b.z;
    return temp;
}

vec3 vec3_normalize(vec3 a)
{
    float scale;
    scale = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);

    a.x /= scale;
    a.y /= scale;
    a.z /= scale;

    return a;
}
