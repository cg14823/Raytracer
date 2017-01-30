#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 200;
const int SCREEN_HEIGHT = 200;
SDL_Surface* screen;
int t;
float focalLength = SCREEN_HEIGHT;
vec3 cameraPos(0,0,-3); // Down and Left +
vector<Triangle> triangles;

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
	for (u_int i = 0;i<triangles.size();i++){
		Triangle triangle = triangles[i];
		vec3 e1 = triangle.v1 - triangle.v0;
		vec3 e2 = triangle.v2 - triangle.v0;
		vec3 b = start - triangle.v0;
		mat3 A( -dir, e1, e2 );
		vec3 x = glm::inverse( A ) * b;
		if (x.x < closestIntersection.distance){
			if (x.y >= 0 && x.z >= 0 && (x.y+x.z) <= 1 && x.x >= 0){
				closestIntersection.distance = x.x;
				closestIntersection.position = start+x.x*dir;
				closestIntersection.triangleIndex = i;
				intersect = true;
			}
		}
	}
	return intersect;
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
}

void Draw()
{
	float m = std::numeric_limits<float>::max();
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);
	for( int y=0; y<SCREEN_HEIGHT; ++y )
	{
		for( int x=0; x<SCREEN_WIDTH; ++x )
		{
			vec3 color(0,0,0);
			vec3 d(x-SCREEN_WIDTH/2,y - SCREEN_HEIGHT/2,focalLength);
			Intersection closestIntersection;
			closestIntersection.distance = m;
			if (ClosestIntersection(cameraPos,d,triangles,closestIntersection)){
				color = triangles[closestIntersection.triangleIndex].color;
			}
			PutPixelSDL( screen, x, y, color );
		}
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}
