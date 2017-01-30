#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include <cmath>
#define PI 3.14159

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
vec3 lightColor = 30.f * vec3( 1, 1, 1 );

vec3 p(0.85,0.85,0.85);

float m = std::numeric_limits<float>::max();

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
				if (closestIntersection.triangleIndex != i) {
					closestIntersection.distance = x.x;
					closestIntersection.position = start + x.x*dir;
					closestIntersection.triangleIndex = i;
					intersect = true;
				}
			}
		}
	}
	return intersect;
}

vec3 DirectLight(const Intersection& i){
	float light2point = pow(i.position.x - lightPos.x,2) + pow(i.position.y - lightPos.y,2) + pow(i.position.z - lightPos.z,2);
	Intersection j;
	j.distance = m;
	j.triangleIndex = i.triangleIndex;
	if (ClosestIntersection(i.position, lightPos-i.position, triangles,j)) {
		if (j.distance < sqrt(light2point)) {
			return vec3(0.0, 0.0, 0.0);
		}
	}
	//cout << "h: " << j.distance << " light2pos: " << sqrt(light2point) << endl;
	float A = ( 4 * 3.14159 * light2point);
	vec3 B = lightColor/A;
	float r  = glm::dot(triangles[i.triangleIndex].normal, glm::normalize(lightPos-i.position));
	vec3 D =  (r>0.0)? B*r: vec3(0,0,0);
	return D;
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
	if (keystate[SDLK_j]) {
		yaw -= 0.0174;
		R = mat3(vec3(1, 0, yaw), vec3(0, 1, 0), vec3(-yaw, 0, 1));
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
			vec3 color(0,0,0);
			vec3 d(x-SCREEN_WIDTH/2,y - SCREEN_HEIGHT/2,focalLength);
			d = R*d;
			Intersection closestIntersection;
			closestIntersection.distance = m;
			closestIntersection.triangleIndex= -1;
			if (ClosestIntersection(cameraPos,d,triangles,closestIntersection)){
				color = DirectLight(closestIntersection)*triangles[closestIntersection.triangleIndex].color;
			}

			PutPixelSDL( screen, x, y, color );
		}
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}
