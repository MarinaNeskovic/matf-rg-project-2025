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
uniform vec3 spot_position;
uniform vec3 spot_direction;
uniform vec3 spot_color;
uniform float spot_cutoff;
uniform float spot_outer_cutoff;
uniform vec3 fill_position;
uniform vec3 fill_color;
void main() {
    vec3 norm = normalize(Normal);
    vec3 base_color = texture(texture_diffuse1, TexCoords).rgb;

    // Mesecina / ambijent (directional light)
    vec3 moon_dir = normalize(-moon_direction);
    float moon_diff = max(dot(norm, moon_dir), 0.0);
    vec3 moon_result = moon_diff * moon_color * base_color;

    // Svecice (point light)
    vec3 lamp_dir = normalize(lamp_position - FragPos);
    float lamp_diff = max(dot(norm, lamp_dir), 0.0);
    float lamp_dist = length(lamp_position - FragPos);
    float lamp_attenuation = 1.0 / (1.0 + 0.09 * lamp_dist + 0.032 * lamp_dist * lamp_dist);
    vec3 lamp_result = lamp_diff * lamp_color * base_color * lamp_attenuation;

    // Reflektor (spot light)
    vec3 spot_dir = normalize(spot_position - FragPos);
    float theta = dot(spot_dir, normalize(-spot_direction));
    float epsilon = spot_cutoff - spot_outer_cutoff;
    float spot_intensity = clamp((theta - spot_outer_cutoff) / epsilon, 0.0, 1.0);
    float spot_diff = max(dot(norm, spot_dir), 0.0);
    float spot_dist = length(spot_position - FragPos);
    float spot_attenuation = 1.0 / (1.0 + 0.09 * spot_dist + 0.032 * spot_dist * spot_dist);
    vec3 spot_result = spot_diff * spot_color * base_color * spot_attenuation * spot_intensity;

    // Fill light (popunjava senke, blago, sa suprotne strane)
    vec3 fill_dir = normalize(fill_position - FragPos);
    float fill_diff = max(dot(norm, fill_dir), 0.0);
    float fill_dist = length(fill_position - FragPos);
    float fill_attenuation = 1.0 / (1.0 + 0.09 * fill_dist + 0.032 * fill_dist * fill_dist);
    vec3 fill_result = fill_diff * fill_color * base_color * fill_attenuation;

    vec3 ambient = 0.35 * base_color;

    FragColor = vec4(ambient + moon_result + lamp_result + spot_result + fill_result, 1.0);
}