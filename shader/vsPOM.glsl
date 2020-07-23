#version 330
layout( location = 0 ) in vec3 vtxCoord;
layout( location = 1 ) in vec2 vtxUv;
layout( location = 2 ) in vec3 vtxN;
layout( location = 3 ) in vec3 vtxT;
layout( location = 4 ) in vec3 vtxB;

out vec2 uv;
out vec3 worldPos;
out vec3 worldN;
out vec3 tanViewPos;
out vec3 tanFragPos;

uniform mat4 M, V, P;
uniform vec3 lightPosition;
uniform vec3 eyePoint;

void main(){
    //projection plane
    gl_Position = P * V * M * vec4( vtxCoord, 1.0 );

    uv = vtxUv;

    worldPos = (M * vec4(vtxCoord, 1.0)).xyz;

    worldN = (vec4(vtxN, 1.0) * inverse(M)).xyz;
    worldN = normalize(worldN);

    vec3 T = normalize(M * vec4(vtxT, 1.0)).xyz;
    vec3 B = normalize(M * vec4(vtxB, 1.0)).xyz;
    vec3 N = normalize(M * vec4(vtxN, 1.0)).xyz;
    mat3 TBN = transpose(mat3(T, B, N));

    tanViewPos = TBN * eyePoint;
    tanFragPos = TBN * worldPos;
}
