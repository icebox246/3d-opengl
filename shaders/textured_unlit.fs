#version 330 core

smooth in vec2 UV;
smooth in vec3 normal;

uniform sampler2D tex0;
uniform sampler2D tex1;

out vec4 outColor;

void main() {
    float depth = texture(tex1, UV).r;
    float zNear = 0.01f;
    float zFar = 1000.0f;

    float depth_scaled = 2.0 * depth - 1.0;
    float depth_z =
        2.0 * zNear * zFar / (zFar + zNear - depth_scaled * (zFar - zNear));

    float fog_perc = pow(clamp(depth_z / 25.0, 0.0, 1.0), 1.5);

    outColor = mix(texture(tex0, UV), vec4(vec3(0.7), 1.0), fog_perc);
}
