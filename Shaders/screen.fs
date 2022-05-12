#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

const float offset = 1.0 / 300.0;

vec4 gaussianBlur(vec2 stCoord, float stStep)
{
    const float scaleFactor = 1.0/273.0;
    vec2 offsets[25]=vec2[25]
    (
      vec2(-2.0,-2.0),vec2(-1.0,-2.0),vec2(0.0,-2.0),vec2(1.0,-2.0),vec2(2.0,-2.0),
      vec2(-2.0,-1.0),vec2(-1.0,-1.0),vec2(0.0,-1.0),vec2(1.0,-1.0),vec2(2.0,-1.0),
      vec2(-2.0,0.0),vec2(-1.0,0.0),vec2(0.0,0.0),vec2(1.0,0.0),vec2(2.0,0.0),
      vec2(-2.0,1.0),vec2(-1.0,1.0),vec2(0.0,1.0),vec2(1.0,1.0),vec2(2.0,1.0),
      vec2(-2.0,2.0),vec2(-1.0,2.0),vec2(0.0,2.0),vec2(1.0,2.0),vec2(2.0,2.0)
    );
    float kernelValues[25]=float[25]
    (
        1.0,4.0,7.0,4.0,1.0,
        4.0,16.0,26.0,16.0,4.0,
        7.0,26.0,41.0,26.0,7.0,
        4.0,16.0,26.0,16.0,4.0,
        1.0,4.0,7.0,4.0,1.0
    );
    
    vec4 sum=vec4(0,0,0,0);
    // sum
    for(int i=0;i<25;i++)
    {
        sum=sum+kernelValues[i]*texture(screenTexture, stCoord+offsets[i]/stStep);
    }
    return sum*scaleFactor;
}

void main()
{ 
   vec4 blurred;
    float blur = 0;
    float depthValue = pow(texture(depthTexture, TexCoords).r, 16.0);
    if (depthValue >= 0.4 && depthValue <= 0.60) blur = 0;
    else if (depthValue > 0.6) blur = smoothstep(0.6, 0.95, depthValue);
    else if (depthValue < 0.4) blur = smoothstep(-0.40, 0.0, -depthValue);
    else blur = 1.0;
    vec4 color = texture(screenTexture, TexCoords);
    if (length(color) > 1.999) {
    	blurred = gaussianBlur(TexCoords, 256);
          blur = 1.0;
   }
   else {
  	  blurred = gaussianBlur(TexCoords, 128);
    }
    //FragColor = vec4(vec3(blur), 1);
    FragColor = mix(texture(screenTexture, TexCoords), blurred, blur);
    //FragColor = blurred;
}