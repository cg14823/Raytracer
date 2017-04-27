#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include <cmath>
#define PI 3.14159

extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }

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




vec3 p(0.85, 0.85, 0.85);

float maxFloat = std::numeric_limits<float>::max();
vec3 frameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
float maskBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
float focusPoint = 4.0f;
float blurExp = 1.1f;

// Light parameters
float Kd = 0.8;
float Ks = 0.1;
float specularExponent = 10;

float sc = 1.0f;
float sl = 1.0f;
float sq = 1.0f;

vec3 lightPos(0, -0.5, -0.7);
vec3 lightS = 20.f * vec3(1, 1, 1);
vec3 lightD = 10.f * vec3(1, 1, 1);
vec3 indirectLight = 1.0f*vec3(1, 1, 1);

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
bool ClosestIntersection(vec3& start,vec3& dir, const vector<Triangle>& triangles, Intersection& closestIntersection);
vec3 DirectLight(const Intersection& i);

int main(int argc, char* argv[])
{
	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();	// Set start value for timer.
	LoadTestModel(triangles);
	while (NoQuitMessageSDL())
	{
		Update();
		Draw();
	}

	SDL_SaveBMP(screen, "screenshot.bmp");
	return 0;
}

vec3 reflectD(vec3 input, vec3 normal) {
	return input - 2 * glm::dot(input, normal)*normal;
}

bool ClosestIntersection(vec3& start, vec3& dir, const vector<Triangle>& triangles, Intersection& closestIntersection)
{
	bool intersect = false;
	for (int i = 0; i<triangles.size(); i++) {

		Triangle triangle = triangles[i];
		vec3 e1 = triangle.v1 - triangle.v0;
		vec3 e2 = triangle.v2 - triangle.v0;
		vec3 p = glm::cross(dir, e2);
		vec3 q = glm::cross(start - triangle.v0, e1);
		vec3 x = (1 / (glm::dot(p, e1)))* vec3(glm::dot(q, e2), glm::dot(p, start - triangle.v0), glm::dot(q, dir));
		if (x.x < closestIntersection.distance) {
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

bool ClosestIntersection2(vec3& start, vec3& dir, const vector<Triangle>& triangles, Intersection& closestIntersection, Intersection previousI)
{
	bool intersect = false;
	for (int i = 0; i<triangles.size(); i++) {
		if (previousI.triangleIndex != i) {
			Triangle triangle = triangles[i];
			vec3 e1 = triangle.v1 - triangle.v0;
			vec3 e2 = triangle.v2 - triangle.v0;
			vec3 p = glm::cross(dir, e2);
			vec3 q = glm::cross(start - triangle.v0, e1);
			vec3 x = (1 / (glm::dot(p, e1)))* vec3(glm::dot(q, e2), glm::dot(p, start - triangle.v0), glm::dot(q, dir));
			if (x.x < closestIntersection.distance) {
				if (x.y >= 0.0f && x.z >= 0.0f && (x.y + x.z) <= 1.0f && x.x >= 0.0f) {
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

vec3 DirectLight(const Intersection& i) {

	float light2point = pow(i.position.x - lightPos.x, 2) + pow(i.position.y - lightPos.y, 2) + pow(i.position.z - lightPos.z, 2);
	Intersection j;
	j.distance = maxFloat;
	vec3 d = glm::normalize(lightPos - i.position);
	if (ClosestIntersection(i.position + d*vec3(0.002f, 0.002f, 0.002f), d, triangles, j)) {
		if (abs(j.distance) < (sqrt(light2point))) {
			return vec3(0.0, 0.0, 0.0);
		}
	}
	float A = (4.0f * 3.14159f * light2point);
	vec3 B = lightS / A;
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

float max(float x, float y) {
	if (x > y) return x;
	else return y;
}

vec3 phongShading(Intersection& i,Triangle& triangle, const vector<Triangle>&triangles) {

	Intersection j;
	j.distance = maxFloat;
	vec3 direction2light = glm::normalize(lightPos - i.position);
	float disLp = sqrt(glm::dot((lightPos - i.position), (lightPos - i.position)));
	

	if (ClosestIntersection(i.position + direction2light*vec3(0.002f, 0.002f, 0.002f), direction2light, triangles, j)) {
		if (abs(j.distance) < (sqrt(disLp))) {
			return indirectLight* triangle.ambient;
		}
	}

	vec3 normal = glm::normalize(triangle.normal);
	vec3 viewDirection = glm::normalize(cameraPos - i.position);
	vec3 R = 2.0f * glm::dot(normal, direction2light) * normal - direction2light;
	float x = glm::dot(viewDirection, R);
	vec3 specular = pow(max(x,0), specularExponent) *triangle.specular;
	vec3 diffuse = max(glm::dot(normal, direction2light), 0)* triangle.diffuse;
	float A = (4.0f * 3.14159f * disLp);


	vec3 ligth = indirectLight*triangle.ambient + (lightS*specular + lightD*diffuse) / (sc +sl*disLp + sq * disLp*disLp);

	return ligth;
	
}

vec3 blingShadding(Intersection& i, Triangle& triangle, const vector<Triangle>&triangles) {

	Intersection j;
	j.distance = maxFloat;
	vec3 direction2light = glm::normalize(lightPos - i.position);
	float disLp = sqrt(glm::dot((lightPos - i.position), (lightPos - i.position)));


	if (ClosestIntersection(i.position + direction2light*vec3(0.002f, 0.002f, 0.002f), direction2light, triangles, j)) {
		if (abs(j.distance) < (sqrt(disLp))) {
			return indirectLight* triangle.ambient;
		}
	}

	vec3 normal = glm::normalize(triangle.normal);
	vec3 viewDirection = glm::normalize(cameraPos - i.position);
	vec3 h = (direction2light + viewDirection) / (sqrt(glm::dot((direction2light + viewDirection), (direction2light + viewDirection))));
	float x = glm::dot(normal,h);
	vec3 specular = pow(max(x, 0), specularExponent) *triangle.specular;
	vec3 diffuse = max(glm::dot(normal, direction2light), 0)* triangle.diffuse;
	float A = (4.0f * 3.14159f * disLp);

	vec3 ligth = indirectLight*triangle.ambient + (lightS*specular + lightD*diffuse) / (sc + sl*disLp + sq * disLp*disLp);

	return ligth;
}

void Draw()
{

	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);


#pragma omp parallel for
	for (int y = 0; y<SCREEN_HEIGHT; ++y)
	{
		for (int x = 0; x<SCREEN_WIDTH; ++x)
		{
			vec3 finalColor(0.0f, 0.0f, 0.0f);
			for (float y2 = -0.5f; y2<0.5f; y2 += 0.5f) {
				for (float x2 = -0.5f; x2<0.5f; x2 += 0.5f) {
					vec3 color1(0, 0, 0);
					vec3 color2(0, 0, 0);
					vec3 d(x + (x2)-SCREEN_WIDTH / 2.0f, y + (y2)-SCREEN_HEIGHT / 2.0f, focalLength);
					d = R*d;

					Intersection closestIntersection;
					closestIntersection.distance = maxFloat;
					closestIntersection.triangleIndex = -1;
					if (ClosestIntersection(cameraPos, d, triangles, closestIntersection)) {
						color1 = blingShadding(closestIntersection, triangles[closestIntersection.triangleIndex], triangles);
						vec3 bounceD = reflectD(d, triangles[closestIntersection.triangleIndex].normal);
						//vec3 newpos = closestIntersection.position - bounceD*vec3(0.002f, 0.002f, 0.002f);
						Intersection closestIntersection2;
						closestIntersection2.distance = maxFloat;
						closestIntersection2.triangleIndex = -1;
						if (ClosestIntersection2(closestIntersection.position + bounceD*vec3(0.00000002f, 0.00000002f, 0.00000002f), bounceD, triangles, closestIntersection2, closestIntersection)) {
							color2 = blingShadding(closestIntersection2, triangles[closestIntersection2.triangleIndex], triangles);
						}

					}
					if(x2 == 0.0f && y2 == 0.0f)maskBuffer[y][x] = closestIntersection.distance * 1000;
					finalColor = finalColor + color1 +color2*triangles[closestIntersection.triangleIndex].reflectance;
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