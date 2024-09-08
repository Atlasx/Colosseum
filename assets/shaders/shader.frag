#version 330 core

in vec3 vertColor;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D ourTexture;
uniform float mixPercent;

void main()
{
	vec4 sample = texture(ourTexture, texCoord);
	fragColor = mix(sample, vec4(vertColor.x, vertColor.y, vertColor.z, 1.0f), mixPercent);
}
