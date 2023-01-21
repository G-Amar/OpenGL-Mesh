#version 140

out vec3 N;
out vec3 v;

//this program is a slightly modified version of one of the examples posted by the professor

void main(){
    v = vec3(gl_ModelViewMatrix * gl_Vertex);       
    N = normalize(gl_NormalMatrix * gl_Normal);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
