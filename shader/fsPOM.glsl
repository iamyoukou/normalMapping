#version 330

in vec2 uv;
in vec3 worldPos;
in vec3 worldN;
in vec3 tanViewPos;
in vec3 tanFragPos;
// in vec3 tanViewDir;

uniform sampler2D texBase, texNormal, texHeight;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 eyePoint;

out vec4 outputColor;

// compute fragment normal from a normal map
// i.e. transform it from tangent space to world space
// the code is from https://github.com/JoeyDeVries/LearnOpenGL
// check the theory at https://learnopengl.com/Advanced-Lighting/Normal-Mapping
//----------------------------------------------------------------
mat3 computeTBN(vec2 tempUv){
    vec3 Q1  = dFdx(worldPos);
    vec3 Q2  = dFdy(worldPos);
    vec2 st1 = dFdx(tempUv);
    vec2 st2 = dFdy(tempUv);

    vec3 n   = normalize(worldN);
    vec3 t  = normalize(Q1*st2.t - Q2*st1.t);

    // in the tutorial, they use vec3 b = -normalize(cross(n, t))
    // but it generates weird result
    // vec3 b  = normalize(cross(n, t));

    vec3 b = normalize(-Q1*st2.s + Q2*st1.s);

    mat3 tbn = mat3(t, b, n);

    return tbn;
}

vec3 getNormalFromMap(vec2 tempUv)
{
    vec3 tangentNormal = texture(texNormal, tempUv).xyz * 2.0 - 1.0;

    mat3 tbn = computeTBN(tempUv);

    return normalize(tbn * tangentNormal);
}
//----------------------------------------------------------------

// refer to https://learnopengl.com/Advanced-Lighting/Parallax-Mapping
// code: https://github.com/JoeyDeVries/LearnOpenGL/tree/master/src/5.advanced_lighting/5.3.parallax_occlusion_mapping
vec2 parallaxOcclusionMapping(vec2 texCoords, vec3 viewDir)
{
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float heightScale = 0.1;

    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale;
    vec2 deltaTexCoords = P / numLayers;

    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(texHeight, currentTexCoords).r;

    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(texHeight, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(texHeight, prevTexCoords).r - currentLayerDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main(){
    vec3 tanViewDir = normalize(computeTBN(uv) * (eyePoint - worldPos));
    vec2 distortedUv = parallaxOcclusionMapping(uv, tanViewDir);

    // if(distortedUv.x > 1.0 || distortedUv.y > 1.0 || distortedUv.x < 0.0 || distortedUv.y < 0.0)
    //     discard;

    vec4 texColor = texture(texBase, distortedUv) * 0.75;

    vec3 N = getNormalFromMap(distortedUv);
    vec3 L = normalize(lightPosition - worldPos);
    vec3 V = normalize(eyePoint - worldPos);
    vec3 H = normalize(L + V);

    float ka = 0.2, kd = 0.75, ks = 0.55;
    float alpha = 20;

    outputColor = vec4(0);

    vec4 ambient = texColor * ka;
    vec4 diffuse = texColor * kd;
    vec4 specular = vec4(lightColor * ks, 1.0);

    float dist = length(L);
    float attenuation = 1.0 / (dist * dist);
    float dc = max(dot(N, L), 0.0);
    float sc = pow(max(dot(H, N), 0.0), alpha);

    outputColor += ambient;
    outputColor += diffuse * dc * attenuation;
    outputColor += specular * sc * attenuation;
}
