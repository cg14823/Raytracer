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
vec3 cameraPos(0, 0, -3); // Down and Left +
float yaw = 0;
mat3 R;


vec3 lightPos(0, -0.5, -0.7);
vec3 lightColor = 40.f * vec3(1, 1, 1);
vec3 indirectLight = 0.4f*vec3(1, 1, 1);

vec3 p(0.85, 0.85, 0.85);

float maxFloat = std::numeric_limits<float>::max();
vec3 frameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
float maskBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
float focusPoint = 4.0f;
float blurExp = 1.1f;

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

struct Intersection
{
	vec3 position;
	float distance;
	int triangleIndex;
};

void postBlur();
void Update();
void Draw();
bool ClosestIntersection(vec3& start,vec3& dir, const vector<Triangle>& triangles, Intersection& closestIntersection,int triIndex);
vec3 DirectLight(const Intersection& i);

int main(int argc, char* argv[])
{
	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();	// Set start value for timer.
	LoadTestModel(triangles);

	/*
	float xVal = (rand() % 100)/100.0f;
	float yVal = (rand() % 100)/100.0f;
	float zVal = (rand() % 100)/100.0f;
	vec3 randVec(xVal,yVal,zVal);
	cout << xVal << "," << yVal << "," << zVal << "\n";
	*/
	while (NoQuitMessageSDL())
	{
		Update();
		Draw();
	}

	SDL_SaveBMP(screen, "screenshot.bmp");
	return 0;
}

bool ClosestIntersection(vec3& start, vec3& dir, const vector<Triangle>& triangles, Intersection& closestIntersection,int triIndex)
{
	bool intersect = false;
	for (int i = 0; i<triangles.size(); i++) {

		Triangle triangle = triangles[i];
		vec3 e1 = triangle.v1 - triangle.v0;
		vec3 e2 = triangle.v2 - triangle.v0;
		vec3 p = glm::cross(dir, e2);
		vec3 q = glm::cross(start - triangle.v0, e1);
		vec3 x =(1/(glm::dot(p, e1)))* vec3(glm::dot(q,e2),glm::dot(p,start - triangle.v0), glm::dot(q,dir));
		if (x.x < closestIntersection.distance) {
			if (triIndex == i){
				return false;
			}
			if (x.y >= 0.0f && x.z >= 0.0f && (x.y + x.z) <= 1.0f && x.x >= 0.0f) {
				closestIntersection.distance = x.x;
				closestIntersection.position = start + x.x*dir;
				closestIntersection.triangleIndex = i;
				intersect = true;
			}
		}
	}
	return intersect;
}

vec3 DirectLight(const Intersection& i) {

	float light2point = pow(i.position.x - lightPos.x, 2) + pow(i.position.y - lightPos.y, 2) + pow(i.position.z - lightPos.z, 2);
	Intersection j;
	j.distance = maxFloat;
	vec3 d = glm::normalize(lightPos - i.position);
	vec3 newD = i.position+d*vec3(0.002f, 0.002f, 0.002f);

	if (ClosestIntersection(newD, d, triangles, j,-1)) {
		//cout << "j: " << abs(j.distance) << " light2pos: " << sqrt(light2point) << endl;
		if (abs(j.distance) < (sqrt(light2point))) {
			return vec3(0.0f, 0.0f, 0.0f);
		}
	}

	float A = (4.0f * 3.14159f * light2point);
	vec3 B = lightColor / A;
	float r = glm::dot(triangles[i.triangleIndex].normal, glm::normalize(lightPos - i.position));
	vec3 D = (r>0.0f) ? B*r : vec3(0.0f, 0.0f, 0.0f);
	return D;
}

void Update()
{

	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;
	Uint8* keystate = SDL_GetKeyState(0);
	if (keystate[SDLK_UP])
	{
		cameraPos.y -= 0.1;
		// Move camera forward
	}
	if (keystate[SDLK_DOWN])
	{
		cameraPos.y += 0.1;
		// Move camera backward
	}
	if (keystate[SDLK_LEFT])
	{
		cameraPos.x -= 0.1;
		// Move camera to the left
	}
	if (keystate[SDLK_RIGHT])
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
	if (keystate[SDLK_a]) {
		yaw += 0.0174;
		R = mat3(vec3(1, 0, yaw), vec3(0, 1, 0), vec3(-yaw, 0, 1));
	}
	if (keystate[SDLK_d]) {
		yaw -= 0.0174;
		R = mat3(vec3(1, 0, yaw), vec3(0, 1, 0), vec3(-yaw, 0, 1));
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
}

void Draw()
{

	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

	int N = 4;
	float val1 = PI/(2.0f*N);
	float val2 = 2*PI/N;

#pragma omp parallel for
	for (int y = 0; y<SCREEN_HEIGHT; ++y)
	{
		for (int x = 0; x<SCREEN_WIDTH; ++x)
		{
			vec3 finalColor(0.0f, 0.0f, 0.0f);
			for (float y2 = -0.5f; y2<0.5f; y2 += 0.5f) {
				for (float x2 = -0.5f; x2<0.5f; x2 += 0.5f) {
					vec3 color(0, 0, 0);
					vec3 d(x + (x2)-SCREEN_WIDTH / 2.0f, y + (y2)-SCREEN_HEIGHT / 2.0f, focalLength);
					d = R*d;

					Intersection closestIntersection;
					closestIntersection.distance = maxFloat;
					closestIntersection.triangleIndex = -1;
					if (ClosestIntersection(cameraPos, d, triangles, closestIntersection,-1)) {

						vec3 nm = triangles[closestIntersection.triangleIndex].normal;
						mat3 firstMat = mat3(vec3(nm.z,0,nm.x),vec3(0,nm.z,nm.y),vec3(-nm.x,-nm.y,nm.z));
						mat3 secMat = mat3(vec3(nm.y*nm.y,(-nm.x)*nm.y,0),vec3((-nm.x)*nm.y,nm.x*nm.x,0),vec3(0,0,0));
						mat3 rotationMat = firstMat + (1.0f/(1.0f + nm.z))*secMat;
						srand (time(NULL));
						float xVal = ((float)rand()/(RAND_MAX))*2 -1;
						float yVal = ((float)rand()/(RAND_MAX))*2 -1;
						float zVal = ((float)rand()/(RAND_MAX));

						vec3 randVec(xVal,yVal,zVal);
						randVec = rotationMat*randVec;
						//cout << randVec.x << "," << randVec.y << "," << randVec.z << "\n";
						color = triangles[closestIntersection.triangleIndex].color*(indirectLight + DirectLight(closestIntersection)*triangles[closestIntersection.triangleIndex].color);
						if (ClosestIntersection(closestIntersection.position, randVec, triangles,closestIntersection,closestIntersection.triangleIndex)){
							color += triangles[closestIntersection.triangleIndex].color*(DirectLight(closestIntersection));
						}
					}
					if(x2 == 0.0f && y2 == 0.0f)maskBuffer[y][x] = closestIntersection.distance * 1000;
					finalColor = finalColor + (color);
				}
			}

			frameBuffer[y][x] = finalColor*(1.0f / 9.0f);
		}
	}
	postBlur();

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void postBlur() {
#pragma omp parallel for
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		for (int x = 0; x < SCREEN_WIDTH; ++x)
		{
			float dist = abs(maskBuffer[y][x] -focusPoint);
			if (dist < 1 || dist > 100) PutPixelSDL(screen, x, y, frameBuffer[y][x]);
			else {
				float boxr = roundf(powf(blurExp, dist));
				if (boxr > SCREEN_HEIGHT) boxr = SCREEN_HEIGHT;
				int i = 0;
				vec3 currColor = vec3(0.0f, 0.0f, 0.0f);

				int yr = (y - boxr < 0) ? 0 : y - boxr;
				int xr = (x - boxr < 0) ? 0 : x - boxr;
				int stopy = (y + boxr < SCREEN_HEIGHT-1) ? y + boxr : SCREEN_HEIGHT-1;
				int stopx = (x + boxr < SCREEN_WIDTH-1) ? x + boxr : SCREEN_WIDTH-1;
				for (yr; yr <= stopy; yr++) {
					for (xr; xr <= stopx; xr++) {
						currColor += frameBuffer[yr][xr];
						i++;
					}
				}
				currColor = currColor / ((float)i);
				PutPixelSDL(screen, x, y, currColor);
			}
		}
	}

}
