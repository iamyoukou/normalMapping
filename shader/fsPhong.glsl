#version 330

in vec2 uv;
in vec3 worldPos;
in vec3 worldN;

uniform sampler2D texBase, texNormal;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 eyePoint;

out vec4 outputColor;

// compute fragment normal from a normal map
// i.e. transform it from tangent space to world space
// the code is from https://github.com/JoeyDeVries/LearnOpenGL
// check the theory at https://learnopengl.com/Advanced-Lighting/Normal-Mapping
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(texNormal, uv).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(worldPos);
    vec3 Q2  = dFdy(worldPos);
    vec2 st1 = dFdx(uv);
    vec2 st2 = dFdy(uv);

    vec3 n   = normalize(worldN);
    vec3 t  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 b  = -normalize(cross(n, t));
    mat3 tbn = mat3(t, b, n);

    return normalize(tbn * tangentNormal);
}


void main(){
    vec4 texColor = texture(texBase, uv) * 0.75;

    vec3 N = getNormalFromMap();

    float ka = 0.05, kd = 0.1, ks = 0.85;
    float alpha = 50;

    outputColor = vec4(0);

    vec4 ambient = vec4(lightColor * ka, 1.0);
    vec4 diffuse = vec4(lightColor * kd, 1.0);
    vec4 specular = vec4(lightColor * ks, 1.0);

    vec3 L = normalize(lightPosition - worldPos);
    vec3 V = normalize(eyePoint - worldPos);
    vec3 H = normalize(L + V);

    float dist = length(L);
    float attenuation = 1.0 / (dist * dist);

    outputColor += ambient;
    outputColor += diffuse * abs(dot(N, L)) * attenuation;
    outputColor += specular * pow(clamp(dot(H, N), 0, 1), alpha) * attenuation;

    outputColor += texColor;
}
