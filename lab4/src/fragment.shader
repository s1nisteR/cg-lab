#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform sampler2D anotherTexture;

void main()
{
    FragColor = mix(texture(ourTexture, TexCoord), texture(anotherTexture, TexCoord), 0.4) * vec4(ourColor, 1.0);
}