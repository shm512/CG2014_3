//shader version
#version 150 core

uniform mat4 modelViewMatrix;

//inverse and transpose matrix for normals
uniform mat4 normalMatrix;

//projectionMatrix * modelViewMatrix
uniform mat4 modelViewProjectionMatrix;

//input vertex: position, normal, texture coordinates
in vec3 pos;
in vec3 nor;
in vec2 tex;

//output vertex to fragment shader
out VertexData
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
} VertexOut;

void main()
{
	gl_Position = modelViewProjectionMatrix * vec4(pos.xyz,1);

	VertexOut.position = vec3(modelViewMatrix * vec4(pos.xyz,1));
	VertexOut.normal = vec3(normalMatrix * vec4(nor.xyz,1));	
	VertexOut.texcoord = vec2(tex.xy);
}
