#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D map_Kd;
//uniform sampler2D specularTexture;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{           
    vec3 color = texture(map_Kd, fs_in.TexCoords).rgb;
	vec4 color_b = texture(map_Kd, fs_in.TexCoords).rgba;
    // ambient
    vec3 ambient = 0.05 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    //float diff = 1.0f;
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    //vec3 specular = vec3(0.3) * spec * texture(specularTexture, fs_in.TexCoords).rgb; // assuming bright white light color
    vec3 specular = vec3(0.3) * spec * vec3(0.1); // assuming bright white light color
    FragColor = vec4(ambient + diffuse + specular, color_b.a);
}