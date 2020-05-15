#version 330

in vec3 fragmentColor;
in vec2 uv;
in vec3 lightDir;

uniform sampler2D texBase, texNormal;
uniform float lightPower;
uniform vec3 lightColor;

out vec4 outputColor;

void main(){
    float alpha = 0.75;
    // outputColor = texture(texNormal, uv);
    // outputColor = (1 - alpha)*vec4( fragmentColor, 1.0 ) + alpha*texture(texBase, uv);
    //
    // outputColor -= vec4(0.1, 0.1, 0.1, 0.1);

    // [0, 1] to [-1, 1]
    vec3 normal = texture(texNormal, uv).xyz;
    normal = (normal - 0.5) * 2.0;

    vec3 tempColor = (1 - alpha) * dot(lightDir, normal) * lightPower * lightColor;
    tempColor += alpha * texture(texBase, uv).xyz;

    outputColor = vec4(tempColor, 1.0);

    // outputColor += 0.1;
}
