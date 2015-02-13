//shader version
#version 150 core

//mode of drawing
//if is true, then use Texture
//otherwise draw gradient
uniform int useTexture;

uniform vec3 lightPos;
uniform float lightPower_ambient;
uniform float lightPower_diffuse;
uniform float lightPower_specular;

uniform float k_ambient;
uniform float k_diffuse;
uniform float k_specular;
uniform float shininess;

//texture object
uniform sampler2D textureSampler;

//retrieve this data form vertex shader
in VertexData
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
} VertexIn;

out vec4 fragColor;

        
float
light_intensity()
{
    vec3 V = normalize(-VertexIn.position);
    vec3 N = normalize(VertexIn.normal);
    vec3 L = normalize(lightPos - VertexIn.position);

    float I_ambient = k_ambient * lightPower_ambient;

    float I_diffuse = k_diffuse * max(dot(N, L), 0.0) * lightPower_diffuse;

    float dot_R_V = 2.0 * dot(N, L) * dot(N,  V) - dot(L, V);
    float I_specular =
        k_specular * pow(max(dot_R_V, 0.0), shininess) * lightPower_specular;
 
    return I_ambient + I_diffuse + I_specular;
}

void
main()
{
	if (useTexture > 0)
		//take color from texture using texture2D
		fragColor = vec4(
            texture(textureSampler, VertexIn.texcoord.xy).rgb,
            length(VertexIn.normal) * length(VertexIn.position)
        );
	else
	{
		//use default color (brown)
		fragColor = vec4(0.5, 0.2, 0.1, 1);
	}
    
    fragColor *= light_intensity();
}
