#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include <cmath>
#include "BRDFRead.h"


using namespace std;
using glm::vec3;
using glm::mat3;

/*
 Arrows left, right , up and down
 a : camera rotate left
 d : camera rotate right
 w : move camera foward
 s : move camera backwards

 i: light foward
 k : light back
 j : light left
 l : light right


*/

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;
vector<Triangle> triangles;


float focalLength = SCREEN_HEIGHT;
vec3 cameraPos(0,0,-3); // Down and Left +
float yaw = 0;
mat3 R;


vec3 lightPos( 0, -0.5, -0.7 );
vec3 lightColor = 40.f * vec3( 1, 1, 1 );
vec3 indirectLight = 0.4f*vec3( 1, 1, 1 );

vec3 p(0.85,0.85,0.85);

float m = std::numeric_limits<float>::max();

double* brdf;

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

struct Intersection
{
	vec3 position;
	float distance;
	int triangleIndex;
};

void Update();
void Draw();
bool ClosestIntersection(
	vec3 start,
	vec3 dir,
	const vector<Triangle>& triangles,
	Intersection& closestIntersection );
vec3 DirectLight(const Intersection& i);

int main( int argc, char* argv[] )
{
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.
	LoadTestModel(triangles);
	if (!read_brdf("steel.binary", brdf))
	{
		fprintf(stderr, "Error reading %s\n", "red-specular-plastic.binary");
		exit(1);
	}
	while( NoQuitMessageSDL() )
	{
		Update();
		Draw();
	}

	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

bool ClosestIntersection(vec3 start,vec3 dir,const vector<Triangle>& triangles,Intersection& closestIntersection)
{
	bool intersect = false;
	for (int i = 0;i<triangles.size();i++){
		Triangle triangle = triangles[i];
		vec3 e1 = triangle.v1 - triangle.v0;
		vec3 e2 = triangle.v2 - triangle.v0;
		vec3 b = start - triangle.v0;
		mat3 A( -dir, e1, e2 );
		vec3 x = glm::inverse( A ) * b;
		if (x.x < closestIntersection.distance){
			if (x.y >= 0 && x.z >= 0 && (x.y+x.z) <= 1 && x.x >= 0){
				closestIntersection.distance = x.x;
				closestIntersection.position = start + x.x*dir;
				closestIntersection.triangleIndex = i;
				intersect = true;
			}
		}
	}
	return intersect;
}

vec3 DirectLight(const Intersection& i){

	float light2point = pow(i.position.x - lightPos.x,2) + pow(i.position.y - lightPos.y,2) + pow(i.position.z - lightPos.z,2);
	Intersection j;
	j.distance = m;
	vec3 d = glm::normalize(lightPos - i.position);
	if (ClosestIntersection(i.position + d*vec3(0.00001,0.00001,0.00001), d, triangles,j)) {
		//cout << "j: " << abs(j.distance) << " light2pos: " << sqrt(light2point) << endl;
		if (abs(j.distance) < (sqrt(light2point))) {
			return vec3(0.0, 0.0, 0.0);
		}
	}
	float A = ( 4 * 3.14159 * light2point);
	vec3 B = lightColor/A;
	float r  = glm::dot(triangles[i.triangleIndex].normal, glm::normalize(lightPos-i.position));
	vec3 D =  (r>0.0)? B*r: vec3(0,0,0);
	return D;
}

vec3 cartToSpherical(vec3 input, vec3 normal,vec3 tangent){
	vec3 spherical(0,0,0);
	float x = input.x;
	float y = input.y;
	float z = input.z;
	float xn = normal.x;
	float yn = normal.y;
	float zn = normal.z;
	float dot = x*xn + y*yn + z*zn;
	float lenSq1 = xn*xn + yn*yn + zn*zn;
	float lenSq2 = x*x + y*y + z*z;
	float angle = acos(dot/sqrt(lenSq1 * lenSq2));
	float tanSq = tangent.x*tangent.x + tangent.y*tangent.y + tangent.z*tangent.z;
	float len3;
	if (angle == 0) len3 = sqrt(lenSq2);
	else len3 = sqrt(lenSq2)*cos((M_PI/2)-angle);
	vec3 side4 = tangent - input;
	float lenSq4 = side4.x*side4.x + side4.y*side4.y + side4.z*side4.z;
	if (angle == 0) lenSq4 = sqrt(lenSq4);
	else lenSq4 = sqrt(lenSq4)*cos((M_PI/2)-angle);
	float val1 = (len3*len3) + tanSq - (lenSq4*lenSq4);
	float val2 = (2*len3*sqrt(tanSq));
	float cosvalue = val1/val2;

	spherical.x = sqrt(x*x+y*y+z*z); //rho
	spherical.y = angle;//acos(z/spherical.x); //theta
	if (angle == 0) spherical.z = 0;
	else spherical.z = acos(cosvalue); //phi
	return spherical;
}

void Update()
{

	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;
	Uint8* keystate = SDL_GetKeyState( 0 );
	if( keystate[SDLK_UP] )
	{
		cameraPos.y -= 0.1;
	// Move camera forward
	}
	if( keystate[SDLK_DOWN] )
	{
		cameraPos.y += 0.1;
	// Move camera backward
	}
	if( keystate[SDLK_LEFT] )
	{
		cameraPos.x -= 0.1;
	// Move camera to the left
	}
	if( keystate[SDLK_RIGHT] )
	{
		cameraPos.x += 0.1;
	// Move camera to the right
	}
	if (keystate[SDLK_w]) {
		cameraPos.z += 0.1;
	}
	if (keystate[SDLK_s]) {
		cameraPos.z -= 0.1;
	}
	if( keystate[SDLK_a]){
		yaw += 0.0174;
		R = mat3(vec3(1,0,yaw),vec3(0,1,0),vec3(-yaw,0,1));
	}
	if( keystate[SDLK_d]){
		yaw -= 0.0174;
		R = mat3(vec3(1,0,yaw),vec3(0,1,0),vec3(-yaw,0,1));
	}

	if (keystate[SDLK_i]) {
		lightPos.z += 0.1;
	}
	if (keystate[SDLK_k]) {
		lightPos.z -= 0.1;
	}
	if (keystate[SDLK_j]) {
		lightPos.x -= 0.1;
	}
	if (keystate[SDLK_l]) {
		lightPos.x += 0.1;
	}
	if (keystate[SDLK_y]) {
		lightPos.y -= 0.1;
	}
	if (keystate[SDLK_h]) {
		lightPos.y += 0.1;
	}
}

void Draw()
{

	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);
	for( int y=0; y<SCREEN_HEIGHT; ++y )
	{
		for( int x=0; x<SCREEN_WIDTH; ++x )
		{
			vec3 finalColor(0,0,0);
			/*
			for (float y2 = -0.5f; y2<0.5f;y2+=0.5f){
				for (float x2 = -0.5f; x2<0.5f;x2+=0.5f){
					vec3 color(0,0,0);
					vec3 d(x+(x2)-SCREEN_WIDTH/2,y+(y2) - SCREEN_HEIGHT/2,focalLength);
					d = R*glm::normalize(d);

					Intersection closestIntersection;
					closestIntersection.distance = m;
					closestIntersection.triangleIndex= -1;
					if (ClosestIntersection(cameraPos,d,triangles,closestIntersection)){
						color = triangles[closestIntersection.triangleIndex].color*(indirectLight + DirectLight(closestIntersection)*triangles[closestIntersection.triangleIndex].color);
					}
					finalColor = finalColor + color;
				}
			}
			*/
			vec3 d(x+-SCREEN_WIDTH/2,y+ - SCREEN_HEIGHT/2,focalLength);
			d = R*d;

			Intersection closestIntersection;
			closestIntersection.distance = m;
			closestIntersection.triangleIndex= -1;

			if (ClosestIntersection(cameraPos,d,triangles,closestIntersection)){
				finalColor = triangles[closestIntersection.triangleIndex].color*(indirectLight + DirectLight(closestIntersection));
				vec3 triangleTangent = glm::normalize(triangles[closestIntersection.triangleIndex].v0 - triangles[closestIntersection.triangleIndex].v1);
				vec3 dCamera = glm::normalize(cameraPos - closestIntersection.position);
				vec3 dSpher = cartToSpherical(dCamera,triangles[closestIntersection.triangleIndex].normal,triangleTangent); //point to camera spherical coords
				vec3 dLight = glm::normalize(lightPos - closestIntersection.position);
				vec3 dlSpher = cartToSpherical(dLight,triangles[closestIntersection.triangleIndex].normal,triangleTangent); //point to light spherical coords
				double red,green,blue;
				cout << dlSpher.y << "," << dlSpher.z << " :: " << dSpher.y << "," << dSpher.z << "\n";
				lookup_brdf_val(brdf,dlSpher.y,dlSpher.z,dSpher.y,dSpher.z,red,green,blue);
				finalColor.x = (float)red;
				finalColor.y = (float)green;
				finalColor.z = (float)blue;
			}


			PutPixelSDL( screen, x, y, finalColor );
		}
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}
