//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
//#shader fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

uniform vec3 moon_direction;
uniform vec3 moon_color;

uniform vec3 lamp_position;
uniform vec3 lamp_color;

void main() {
    vec3 norm = normalize(Normal);
    vec3 base_color = texture(texture_diffuse1, TexCoords).rgb;

    // Mesecina (directional light)
    vec3 moon_dir = normalize(-moon_direction);
    float moon_diff = max(dot(norm, moon_dir), 0.0);
    vec3 moon_result = moon_diff * moon_color * base_color;

    // Svetlo iz svetionika (point light)
    vec3 lamp_dir = normalize(lamp_position - FragPos);
    float lamp_diff = max(dot(norm, lamp_dir), 0.0);
    float dist = length(lamp_position - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
    vec3 lamp_result = lamp_diff * lamp_color * base_color * attenuation;

    vec3 ambient = 0.08 * base_color;

    FragColor = vec4(ambient + moon_result + lamp_result, 1.0);
}