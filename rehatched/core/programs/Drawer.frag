// Fragment func for GraphicsDrawer objects
#version 410

in vec4 oColor;
in vec2 oUV;

out vec4 fragColor;

uniform sampler2D image;

void main() {
	fragColor = texture(image, oUV) * oColor;
}
