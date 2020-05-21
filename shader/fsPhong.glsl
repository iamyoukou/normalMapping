#version 330

in vec2 uv;
in vec3 lightDir;
in vec3 viewDir;

uniform sampler2D texBase, texNormal;
uniform float lightPower;
uniform vec3 lightColor;

out vec4 outputColor;

void main(){
    vec4 texColor = texture(texBase, uv);

    // [0, 1] to [-1, 1]
    vec3 normal = texture(texNormal, uv).xyz;
    normal = normalize((normal - 0.5) * 2.0);

    vec3 reflectDir = normalize(reflect(-lightDir, normal));

    float ka = 0.01, kd = 0.25, ks = 0.8;
    float alpha = 10;

    float scale = 1;
    vec4 ambient = vec4(lightColor * ka, 1.0) * scale;
    vec4 diffuse = vec4(lightColor * kd, 1.0) * scale;
    vec4 specular = vec4(lightColor * ks, 1.0) * scale;

    outputColor = ambient;
    outputColor += diffuse * clamp(dot(normal, lightDir), 0, 1);
    outputColor += specular * pow(clamp(dot(reflectDir, viewDir), 0, 1), alpha);

    outputColor += texColor;
}
