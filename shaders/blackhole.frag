#version 450

layout(origin_upper_left) in vec4 gl_FragCoord;

layout (location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragUVW;
layout(location = 2) in vec4 MyfragCoords;

layout (location = 0) out vec4 outColor;

struct PointLight {
  vec4 position; // ignore w
  vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 invView;
  vec4 ambientLightColor; // w is intensity
  PointLight pointLights[10];
  int numLights;
  float time;
} ubo;

layout(set = 1, binding = 0) uniform samplerCube samplerCubeMap;
layout(set = 1, binding = 1) uniform sampler2D diskImage;

layout(push_constant) uniform Push {
  mat4 modelMatrix;
} push;

const vec3 bh=vec3(0, -10, -250);
const float bh_M=1.2;
const float r_bar=2.7*bh_M;
const float disc_r_orig=r_bar * 2.1;
const vec3 disc_n=vec3(0.,1.,0.);
const vec3 disc_s=vec3(1.,0.,0.);
const float PI = 3.14;

vec4 disc_color(vec3 p_disc, float disc_r, float time)
{
  vec3 v=p_disc-bh;
  float d=length(v);
  v/=d;
  if(d<disc_r)
  {
    //return vec4(1.0,0.,0.,1.);
    vec2 uv=vec2((atan(dot(v,disc_s),dot(v,cross(disc_s,disc_n)))/(2.*PI)*1.- time * 0.3),
    (d-r_bar)/(disc_r-r_bar));
//    return vec4(1, 0, 0, 0);
    return 3.*texture(diskImage,uv)*smoothstep(disc_r,r_bar,d);

  }
  else return vec4(0.);
}

void main() {
  mat4 rotView = ubo.view;
  rotView[3][0] = 0;
  rotView[3][1] = 0;
  rotView[3][2] = 0;

  vec3 eye = ubo.invView[3].xyz;
  vec3 up = ubo.invView[1].xyz;

  vec3 disc_n=vec3(0.,1.,0.);
  vec3 disc_s=vec3(1.,0.,0.);


  mat4 cam_mat = inverse(ubo.view) * inverse(ubo.projection);

  vec4 color=vec4(0);

  vec3 ray_dir = normalize((cam_mat*MyfragCoords).xyz - eye);

  vec3 h2e=eye-bh;
  float l2_h2e=dot(h2e,h2e);
  float rm=length(cross(ray_dir,h2e));
  float t_cp=sqrt(l2_h2e-rm*rm);

  float disc_r=disc_r_orig;

  float alpha=4.*bh_M/rm;
  disc_r=disc_r_orig;
  if(rm<r_bar){
    alpha*=(1.-abs(dot(disc_n, up))), disc_r*=1.25;
  }else{
    vec4 source = vec4(MyfragCoords);
    vec2 ratio = vec2(800.0 / 600.0, 1.0);

    vec4 blackHolePosScreen = ubo.projection * ubo.view * vec4(bh, 1);
    vec2 offset = MyfragCoords.xy / MyfragCoords.w - blackHolePosScreen.xy / blackHolePosScreen.w;
    float deformation = 2*r_bar*1/pow(length(offset * ratio) * pow(length(h2e),0.5),2);
    offset = offset*(1-deformation * ratio);
    source.xy = (blackHolePosScreen.xy / blackHolePosScreen.w + offset) * MyfragCoords.w;

    mat4 m = inverse(rotView) * inverse(ubo.projection);
    vec4 worldSpace = m * source * -1;
    color += texture(samplerCubeMap, worldSpace.xyz);
    outColor = color;
  }

  float tan_a_2=tan(alpha*0.5);

  vec3 cp=eye+ray_dir*t_cp;
  vec3 coord_origin=cp+ray_dir*(rm*tan_a_2);
  vec3 x_axis=normalize(bh-coord_origin);
  vec3 y_axis=normalize(ray_dir+tan_a_2*normalize(bh-cp));
  vec3 z_axis=cross(x_axis,y_axis);

  float c=length(bh-coord_origin);
  float k=tan_a_2;


  vec3 iline_r=normalize(cross(z_axis,disc_n));
  float x1=-1.,x2=-1.,y1,y2;



  float k2=k*k;
  float b2=c*c/(1.0+k2);
  float a2=k2*b2;
  float a=sqrt(a2);

  float denom=dot(x_axis,iline_r);
  if(denom==0.)
  {
    x1=x2=c;
    y1=-b2/sqrt(a2);
    y2=-y1;
  }
  else
  {
    float slope=dot(y_axis,iline_r)/denom;
    k2=slope*slope;
    float A=a2*k2-b2;
    float B=-2.*a2*k2*c;
    float C=a2*(k2*c*c+b2);

    float delta=sqrt(B*B-4.*A*C);
    x1=(-B-delta)/(2.*A);
    x2=(-B+delta)/(2.*A);
    y1=slope*(x1-c);
    y2=slope*(x2-c);
  }

  vec3 o2e=eye-coord_origin;
  float yeye=dot(o2e,y_axis);
  vec3 p1=coord_origin+x1*x_axis+y1*y_axis,
  p2=coord_origin+x2*x_axis+y2*y_axis;
  if(x1>=0.&&y1>=yeye && ((y1<0.&&length(p1-bh)>r_bar)||rm>r_bar))
  {
    color+=disc_color(p1, disc_r, ubo.time);
  }
  if(x2>=0.&&y2>=yeye && ((y2<0.&&length(p2-bh)>r_bar) ||rm>r_bar))
  {
    color+=disc_color(p2, disc_r, ubo.time);

  }

  outColor=color;
}
