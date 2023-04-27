#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
out gl_PerVertex { vec4 gl_Position; };

void main()
{
   gl_Position = position;
};



#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color; // anything starting with u_ is uniform

void main()
{
   color = u_Color;
};
