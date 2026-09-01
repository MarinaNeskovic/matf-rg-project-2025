//#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
out vec2 TexCoords;
void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
//#shader fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D scene_texture;
uniform sampler2D bloom_texture;
uniform float exposure;
void main() {
    vec3 hdr_color = texture(scene_texture, TexCoords).rgb;
    vec3 bloom_color = texture(bloom_texture, TexCoords).rgb;
    hdr_color += bloom_color;
    vec3 mapped = clamp(hdr_color * exposure, 0.0, 1.0);
    FragColor = vec4(mapped, 1.0);
}