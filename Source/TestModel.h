#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H

// Defines a simple test model: The Cornel Box

#include <glm/glm.hpp>
#include <vector>

// Used to describe a triangular surface:
class Triangle
{
public:
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float reflectance;

	Triangle( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color )
		: v0(v0), v1(v1), v2(v2), color(color)
	{
		ComputeNormal();
		specular = glm::vec3(0.0f, 0.0f, 0.0f);
		ambient = glm::vec3(0.0f, 0.0f, 0.0f);
		diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float reflectance)
	: v0(v0), v1(v1), v2(v2), color(color), ambient(ambient), diffuse(diffuse), specular(specular), reflectance(reflectance) {
		ComputeNormal();
	}

	void ComputeNormal()
	{
		glm::vec3 e1 = v1-v0;
		glm::vec3 e2 = v2-v0;
		normal = glm::normalize( glm::cross( e2, e1 ) );
	}
};

// Loads the Cornell Box. It is scaled to fill the volume:
// -1 <= x <= +1
// -1 <= y <= +1
// -1 <= z <= +1
void LoadTestModel( std::vector<Triangle>& triangles )
{
	using glm::vec3;

	// Defines colors:
	vec3 red(    0.75f, 0.15f, 0.15f );
	vec3 yellow( 0.75f, 0.75f, 0.15f );
	vec3 green(  0.15f, 0.75f, 0.15f );
	vec3 cyan(   0.15f, 0.75f, 0.75f );
	vec3 blue(   0.15f, 0.15f, 0.75f );
	vec3 purple( 0.75f, 0.15f, 0.75f );
	vec3 white(  0.75f, 0.75f, 0.75f );

	triangles.clear();
	triangles.reserve( 5*2*3 );

	// ---------------------------------------------------------------------------
	// Room

	float L = 555;			// Length of Cornell Box side.

	vec3 A(L,0,0);
	vec3 B(0,0,0);
	vec3 C(L,0,L);
	vec3 D(0,0,L);

	vec3 E(L,L,0);
	vec3 F(0,L,0);
	vec3 G(L,L,L);
	vec3 H(0,L,L);


	vec3 tspecular = vec3(0.0f, 0.5f, 0.0f);
	vec3 tdiffuse = vec3(0.1f, 0.5f, 0.1f);
	vec3 tambient = vec3(0.1f, 0.5f, 0.1f);
	float reflectance = 0.2f;

	// Floor:
	triangles.push_back( Triangle( C, B, A, green, tambient, tdiffuse, tspecular, 0.05f));
	triangles.push_back( Triangle( C, D, B, green, tambient, tdiffuse, tspecular, 0.05f));

	tspecular = vec3(0.4f, 0.0f, 0.4f);
	tdiffuse = vec3(0.4f, 0.05f, 0.4f);
	tambient = vec3(0.4f, 0.01f, 0.4f);

	// Left wall
	triangles.push_back( Triangle( A, E, C, purple, tambient, tdiffuse, tspecular, reflectance) );
	triangles.push_back( Triangle( C, E, G, purple, tambient, tdiffuse, tspecular, reflectance) );

	tspecular = vec3(0.4f, 0.4f, 0.0f);
	tdiffuse = vec3(0.4f, 0.4f, 0.1f);
	tambient = vec3(0.4f, 0.2f, 0.01f);

	// Right wall
	triangles.push_back( Triangle( F, B, D, yellow, tambient, tdiffuse, tspecular, reflectance) );
	triangles.push_back( Triangle( H, F, D, yellow, tambient, tdiffuse, tspecular, reflectance) );

	tspecular = vec3(0.0f, 0.4f, 0.4f);
	tdiffuse = vec3(0.1f, 0.4f, 0.4f);
	tambient = vec3(0.01f, 0.4f, 0.4f);

	// Ceiling
	triangles.push_back( Triangle( E, F, G, cyan, tambient, tdiffuse, tspecular, 0.0f) );
	triangles.push_back( Triangle( F, H, G, cyan, tambient, tdiffuse, tspecular, 0.0f) );

	tspecular = vec3(0.4f, 0.4f, 0.4f);
	tdiffuse = vec3(0.3f, 0.3f, 0.3f);
	tambient = vec3(0.3f, 0.3f, 0.3f);

	// Back wall
	triangles.push_back( Triangle( G, D, C, white, tambient, tdiffuse, tspecular, 0.0f) );
	triangles.push_back( Triangle( G, H, D, white, tambient, tdiffuse, tspecular, 0.0f) );

	// ---------------------------------------------------------------------------
	// Short block

	A = vec3(290,0,114);
	B = vec3(130,0, 65);
	C = vec3(240,0,272);
	D = vec3( 82,0,225);

	E = vec3(290,165,114);
	F = vec3(130,165, 65);
	G = vec3(240,165,272);
	H = vec3( 82,165,225);

	tspecular = vec3(0.7f, 0.05f, 0.05f);
	tdiffuse = vec3(0.7f, 0.1f, 0.1f);
	tambient = vec3(0.5f, 0.1f, 0.1f);
	reflectance = 0.01f;


	// Front
	triangles.push_back( Triangle(E,B,A,red, tambient, tdiffuse,tspecular, reflectance) );
	triangles.push_back( Triangle(E,F,B,red, tambient, tdiffuse, tspecular, reflectance) );

	// Front
	triangles.push_back( Triangle(F,D,B,red, tambient, tdiffuse, tspecular, reflectance) );
	triangles.push_back( Triangle(F,H,D,red, tambient, tdiffuse, tspecular, reflectance) );

	// BACK
	triangles.push_back( Triangle(H,C,D,red, tambient, tdiffuse, tspecular, reflectance) );
	triangles.push_back( Triangle(H,G,C,red, tambient, tdiffuse, tspecular, reflectance) );

	// LEFT
	triangles.push_back( Triangle(G,E,C,red, tambient, tdiffuse, tspecular, reflectance) );
	triangles.push_back( Triangle(E,A,C,red, tambient, tdiffuse, tspecular, reflectance) );

	// TOP
	triangles.push_back( Triangle(G,F,E,red, tambient, tdiffuse, tspecular, reflectance) );
	triangles.push_back( Triangle(G,H,F,red, tambient, tdiffuse, tspecular, reflectance) );

	// ---------------------------------------------------------------------------
	// Tall block

	A = vec3(423,0,247);
	B = vec3(265,0,296);
	C = vec3(472,0,406);
	D = vec3(314,0,456);

	E = vec3(423,330,247);
	F = vec3(265,330,296);
	G = vec3(472,330,406);
	H = vec3(314,330,456);

	tspecular = vec3(0.1f, 0.1f, 0.75f);
	tdiffuse = vec3(0.15f, 0.15f, 0.75f);
	tambient = vec3(0.1f, 0.1f, 0.5f);

	// Front
	triangles.push_back( Triangle(E,B,A,blue, tambient, tdiffuse, tspecular, reflectance));
	triangles.push_back( Triangle(E,F,B,blue, tambient, tdiffuse, tspecular, reflectance));

	// Front
	triangles.push_back( Triangle(F,D,B,blue, tambient, tdiffuse, tspecular, reflectance));
	triangles.push_back( Triangle(F,H,D,blue, tambient, tdiffuse, tspecular, reflectance));

	// BACK
	triangles.push_back( Triangle(H,C,D,blue, tambient, tdiffuse, tspecular, reflectance));
	triangles.push_back( Triangle(H,G,C,blue, tambient, tdiffuse, tspecular, reflectance));

	// LEFT
	triangles.push_back( Triangle(G,E,C,blue, tambient, tdiffuse, tspecular, reflectance));
	triangles.push_back( Triangle(E,A,C,blue, tambient, tdiffuse, tspecular, reflectance));

	// TOP
	triangles.push_back( Triangle(G,F,E,blue, tambient, tdiffuse, tspecular, reflectance));
	triangles.push_back( Triangle(G,H,F,blue, tambient, tdiffuse, tspecular, reflectance));


	// ----------------------------------------------
	// Scale to the volume [-1,1]^3

	for( size_t i=0; i<triangles.size(); ++i )
	{
		triangles[i].v0 *= 2/L;
		triangles[i].v1 *= 2/L;
		triangles[i].v2 *= 2/L;

		triangles[i].v0 -= vec3(1,1,1);
		triangles[i].v1 -= vec3(1,1,1);
		triangles[i].v2 -= vec3(1,1,1);

		triangles[i].v0.x *= -1;
		triangles[i].v1.x *= -1;
		triangles[i].v2.x *= -1;

		triangles[i].v0.y *= -1;
		triangles[i].v1.y *= -1;
		triangles[i].v2.y *= -1;

		triangles[i].ComputeNormal();
	}
}

#endif