#version 330 core

smooth in vec2 UV;
smooth in vec3 position;
smooth in vec3 world_position;
smooth in vec4 sun_position;

uniform sampler2D tex1;

out vec4 outColor;

uniform mat4 model;

float calculate_shadows(vec4 sun_space_position) {
    vec3 projected_coords = sun_space_position.xyz / sun_space_position.w;
    projected_coords = projected_coords * 0.5 + 0.5;

    float closest_depth = texture(tex1, projected_coords.xy).r;
    float current_depth = projected_coords.z;

    float visible =
        smoothstep(current_depth - 0.00001, current_depth, closest_depth);

    return visible;
}

void main() {
    float sun_strength = calculate_shadows(sun_position) * 0.8;
	float ambient_strength = 0.2;

    outColor = vec4(vec3(0.2 * (ambient_strength + sun_strength)), 1.0);
}
