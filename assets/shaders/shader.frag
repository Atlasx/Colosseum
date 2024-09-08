#version 330 core

in vec3 vertColor;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixPercent;

void main()
{
	vec4 sampleTex1 = texture(texture1, texCoord);
	vec4 sampleTex2 = texture(texture2, texCoord);
	//fragColor = mix(sampleTex1, vec4(vertColor.x, vertColor.y, vertColor.z, 1.0f), mixPercent);
	fragColor = mix(sampleTex2, sampleTex1, mixPercent);
}
