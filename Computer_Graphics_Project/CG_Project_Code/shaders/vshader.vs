#version 330 core

in vec3 vVertex;
in vec3 vTangent;
in vec3 vBitangent;
in vec3 vNormal;
in vec2 vTexCoord;
in vec2 vTexCoord2;
in vec2 vTexCoord3;
in vec3 axisOfInterest;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;
uniform vec3 lpos_world;
uniform vec3 eye_normal;

out vec3 n;
out vec3 e;
out vec3 l;
out vec2 TexCoord;
out vec3 tangent;
out vec3 bitangent;

void main() {
	gl_Position = vProjection * vView * vModel * vec4(vVertex, 1.0);
	n = normalize(vNormal);
    l = normalize(lpos_world - vVertex);
    e = eye_normal;
    bool isXAxis = (axisOfInterest == vec3(1.0, 0.0, 0.0) || axisOfInterest == vec3(-1.0, 0.0, 0.0));
    bool isYAxis = (axisOfInterest == vec3(0.0, 1.0, 0.0) || axisOfInterest == vec3(0.0, -1.0, 0.0));
    bool isZAxis = (axisOfInterest == vec3(0.0, 0.0, 1.0) || axisOfInterest == vec3(0.0, 0.0, -1.0));

    TexCoord = isXAxis ? vec2(vTexCoord) : (isYAxis ? vec2(vTexCoord2) : (isZAxis ? vec2(vTexCoord3) : TexCoord));
}