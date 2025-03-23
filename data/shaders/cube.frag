#version 460

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;

// Light properties
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

vec3 Ka = vec3(0.2, 0.2, 0.2);        // Ambient reflectivity
vec3 Kd = vec3(0.8, 0.8, 0.8);        // Diffuse reflectivity
vec3 Ks = vec3(1.0, 1.0, 1.0);        // Specular reflectivity
float Shininess = 32.0;               // Specular exponent

uniform vec3 uniform_Color;

out vec4 frag_color;

void pointlight(out vec3 amb, out vec3 dif, out vec3 spec ) 
{
    vec3 N = normalize(frag_normal);
    vec3 L = normalize( lightPos - frag_pos );
    vec3 V = normalize(viewPos - frag_pos);
    vec3 R = reflect( -L, N );

    float dist = length(L);

    float c1 = 1.0;
    float c2 = 0.09;
    float c3 = 0.032;

    float att = min(1.0f / (c1 + c2 * dist + c3 * dist * dist), 1.0);
    
    int IntAmb = 1;
    int IntDif = 1;
    int IntSpec = 1;

    amb = att * IntAmb * Ka;
    dif = att * IntDif * Kd * max( dot(L, N), 0.0 );
    spec = att * IntSpec * Ks * pow( max( dot(R,V), 0.0 ), Shininess );
}

void main()
{
    vec3 amb = vec3(0,0,0);
    vec3 dif = vec3(0,0,0);
    vec3 spec = vec3(0,0,0);
    
    pointlight(amb, dif, spec);
    
    vec3 finalColor = (amb + dif) * uniform_Color + spec;

    frag_color = vec4(finalColor, 1.0);
}