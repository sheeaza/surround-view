#version 300 es
#ifdef GL_FRAGMENT_PRECISION_HIGH
  precision highp float;
#else
  precision mediump float;
#endif

//layout (std140, row_major) uniform transformBlock {
//    mat3 intrinsic;
//    vec4 discoeffs;
//    mat3 H;
//};

uniform mat3 intrinsic;
uniform vec4 distCoeffs;
uniform mat4x3 extrinsic;

uniform sampler2D s_texture;
uniform sampler2D alpha_mask;

in vec2 v_texCoord;
in vec3 vertexPos;
out vec4 color;

vec2 fisheye(vec3 pos_i)
{
    float _x = pos_i.x;
    float _y = pos_i.y;
    float r = sqrt(_x*_x + _y*_y);
    float theta = atan(r);
    float theta2 = theta*theta, theta4=theta2*theta2,
          theta6 = theta4*theta2, theta8=theta4*theta4;
    float theta_d = theta*(1.0 + distCoeffs[0]*theta2 + distCoeffs[1]*theta4
                           + distCoeffs[2]*theta6 + distCoeffs[3]*theta8);
    float scale = (r == 0.0) ? 1.0 : theta_d / r;
    pos_i.xy *= scale;
    pos_i = intrinsic * pos_i;

    return pos_i.xy;
}
vec3 transformProj(vec4 pos)
{
    vec3 ret = extrinsic * pos;
    return ret/ret.z;
}

void main()
{
//    float alpha = texture(alpha_mask, v_texCoord).a;
//    color = texture(s_texture, transform(v_texCoord));
//    color *= alpha;
    vec3 iPos = transformProj(vec4(vertexPos, 1.0));
    color = texture(s_texture, fisheye(iPos));
}
