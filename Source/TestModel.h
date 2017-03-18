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

	Triangle( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color )
		: v0(v0), v1(v1), v2(v2), color(color)
	{
		ComputeNormal();
	}

	void ComputeNormal()
	{
		glm::vec3 e1 = v1-v0;
		glm::vec3 e2 = v2-v0;
		normal = glm::normalize( glm::cross( e2, e1 ) );
	}
};

class DynamicBoundingBox{
public:
	glm::vec3 max;
	glm::vec3 min;

	DynamicBoundingBox()
	{

	}

	DynamicBoundingBox(float maxX,float minX,float maxY,float minY,float maxZ,float minZ)
	{
		max = glm::vec3(maxX,maxY,maxZ);
		min = glm::vec3(minX,minY,minZ);
	}
};

struct BoundingBox
{
	std::vector<glm::vec3> myVertices;
	std::vector<Triangle> myTriangles;
	DynamicBoundingBox dynamicBox;
};

// Loads the Cornell Box. It is scaled to fill the volume:
// -1 <= x <= +1
// -1 <= y <= +1
// -1 <= z <= +1
void LoadTestModel( std::vector<Triangle>& triangles , std::vector<BoundingBox>& boxes)
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
	BoundingBox current;

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

	// Floor:
	triangles.push_back( Triangle( C, B, A, green ) );
	triangles.push_back( Triangle( C, D, B, green ) );
	current.myTriangles.push_back(Triangle( C, B, A, green ));
	current.myTriangles.push_back(Triangle( C, D, B, green ));
	current.myVertices.push_back(C);
	current.myVertices.push_back(B);
	current.myVertices.push_back(A);
	current.myVertices.push_back(D);
	boxes.push_back(current);
	current.myTriangles.clear();
	current.myVertices.clear();

	// Left wall
	triangles.push_back( Triangle( A, E, C, purple ) );
	triangles.push_back( Triangle( C, E, G, purple ) );
	current.myTriangles.push_back(Triangle( A, E, C, purple ));
	current.myTriangles.push_back(Triangle( C, E, G, purple ));
	current.myVertices.push_back(A);
	current.myVertices.push_back(E);
	current.myVertices.push_back(C);
	current.myVertices.push_back(G);
	boxes.push_back(current);
	current.myTriangles.clear();
	current.myVertices.clear();

	// Right wall
	triangles.push_back( Triangle( F, B, D, yellow ) );
	triangles.push_back( Triangle( H, F, D, yellow ) );
	current.myTriangles.push_back(Triangle( F, B, D, yellow ));
	current.myTriangles.push_back(Triangle( H, F, D, yellow ));
	current.myVertices.push_back(F);
	current.myVertices.push_back(B);
	current.myVertices.push_back(D);
	current.myVertices.push_back(H);
	boxes.push_back(current);
	current.myTriangles.clear();
	current.myVertices.clear();

	// Ceiling
	triangles.push_back( Triangle( E, F, G, cyan ) );
	triangles.push_back( Triangle( F, H, G, cyan ) );
	current.myTriangles.push_back(Triangle( E, F, G, cyan ));
	current.myTriangles.push_back(Triangle( F, H, G, cyan ));
	current.myVertices.push_back(E);
	current.myVertices.push_back(F);
	current.myVertices.push_back(G);
	current.myVertices.push_back(H);
	boxes.push_back(current);
	current.myTriangles.clear();
	current.myVertices.clear();

	// Back wall
	triangles.push_back( Triangle( G, D, C, white ) );
	triangles.push_back( Triangle( G, H, D, white ) );
	current.myTriangles.push_back(Triangle( G, D, C, white ));
	current.myTriangles.push_back(Triangle( G, H, D, white ));
	current.myVertices.push_back(G);
	current.myVertices.push_back(D);
	current.myVertices.push_back(C);
	current.myVertices.push_back(H);
	boxes.push_back(current);
	current.myTriangles.clear();
	current.myVertices.clear();

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

	// Front
	triangles.push_back( Triangle(E,B,A,red) );
	triangles.push_back( Triangle(E,F,B,red) );

	// Front
	triangles.push_back( Triangle(F,D,B,red) );
	triangles.push_back( Triangle(F,H,D,red) );

	// BACK
	triangles.push_back( Triangle(H,C,D,red) );
	triangles.push_back( Triangle(H,G,C,red) );

	// LEFT
	triangles.push_back( Triangle(G,E,C,red) );
	triangles.push_back( Triangle(E,A,C,red) );

	// TOP
	triangles.push_back( Triangle(G,F,E,red) );
	triangles.push_back( Triangle(G,H,F,red) );


	current.myTriangles.push_back( Triangle(E,B,A,red) );
	current.myTriangles.push_back( Triangle(E,F,B,red) );
	current.myTriangles.push_back( Triangle(F,D,B,red) );
	current.myTriangles.push_back( Triangle(F,H,D,red) );
	current.myTriangles.push_back( Triangle(H,C,D,red) );
	current.myTriangles.push_back( Triangle(H,G,C,red) );
	current.myTriangles.push_back( Triangle(G,E,C,red) );
	current.myTriangles.push_back( Triangle(E,A,C,red) );
	current.myTriangles.push_back( Triangle(G,F,E,red) );
	current.myTriangles.push_back( Triangle(G,H,F,red) );

	current.myVertices.push_back(E);
	current.myVertices.push_back(B);
	current.myVertices.push_back(A);
	current.myVertices.push_back(F);
	current.myVertices.push_back(D);
	current.myVertices.push_back(H);
	current.myVertices.push_back(G);
	current.myVertices.push_back(C);
	boxes.push_back(current);
	current.myTriangles.clear();
	current.myVertices.clear();


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

	// Front
	triangles.push_back( Triangle(E,B,A,blue) );
	triangles.push_back( Triangle(E,F,B,blue) );

	// Front
	triangles.push_back( Triangle(F,D,B,blue) );
	triangles.push_back( Triangle(F,H,D,blue) );

	// BACK
	triangles.push_back( Triangle(H,C,D,blue) );
	triangles.push_back( Triangle(H,G,C,blue) );

	// LEFT
	triangles.push_back( Triangle(G,E,C,blue) );
	triangles.push_back( Triangle(E,A,C,blue) );

	// TOP
	triangles.push_back( Triangle(G,F,E,blue) );
	triangles.push_back( Triangle(G,H,F,blue) );

	current.myTriangles.push_back( Triangle(E,B,A,blue) );
	current.myTriangles.push_back( Triangle(E,F,B,blue) );
	current.myTriangles.push_back( Triangle(F,D,B,blue) );
	current.myTriangles.push_back( Triangle(F,H,D,blue) );
	current.myTriangles.push_back( Triangle(H,C,D,blue) );
	current.myTriangles.push_back( Triangle(H,G,C,blue) );
	current.myTriangles.push_back( Triangle(G,E,C,blue) );
	current.myTriangles.push_back( Triangle(E,A,C,blue) );
	current.myTriangles.push_back( Triangle(G,F,E,blue) );
	current.myTriangles.push_back( Triangle(G,H,F,blue) );

	current.myVertices.push_back(E);
	current.myVertices.push_back(B);
	current.myVertices.push_back(A);
	current.myVertices.push_back(F);
	current.myVertices.push_back(D);
	current.myVertices.push_back(H);
	current.myVertices.push_back(G);
	current.myVertices.push_back(C);
	boxes.push_back(current);
	current.myTriangles.clear();
	current.myVertices.clear();


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
	for( size_t i=0; i<boxes.size(); ++i ){
		for (size_t j=0; j<boxes[i].myTriangles.size();j++){
			boxes[i].myTriangles[j].v0 *= 2/L;
			boxes[i].myTriangles[j].v1 *= 2/L;
			boxes[i].myTriangles[j].v2 *= 2/L;

			boxes[i].myTriangles[j].v0 -= vec3(1,1,1);
			boxes[i].myTriangles[j].v1 -= vec3(1,1,1);
			boxes[i].myTriangles[j].v2 -= vec3(1,1,1);

			boxes[i].myTriangles[j].v0.x *= -1;
			boxes[i].myTriangles[j].v1.x *= -1;
			boxes[i].myTriangles[j].v2.x *= -1;

			boxes[i].myTriangles[j].v0.y *= -1;
			boxes[i].myTriangles[j].v1.y *= -1;
			boxes[i].myTriangles[j].v2.y *= -1;

			boxes[i].myTriangles[j].ComputeNormal();
		}
		for (size_t j=0; j<boxes[i].myVertices.size();j++){
			boxes[i].myVertices[j] *= 2/L;
			boxes[i].myVertices[j] *= 2/L;
			boxes[i].myVertices[j] *= 2/L;

			boxes[i].myVertices[j] -= vec3(1,1,1);
			boxes[i].myVertices[j] -= vec3(1,1,1);
			boxes[i].myVertices[j] -= vec3(1,1,1);

			boxes[i].myVertices[j].x *= -1;
			boxes[i].myVertices[j].x *= -1;
			boxes[i].myVertices[j].x *= -1;

			boxes[i].myVertices[j].y *= -1;
			boxes[i].myVertices[j].y *= -1;
			boxes[i].myVertices[j].y *= -1;
		}
		float xMax = -10000.0f;
		float xMin = 10000.0f;
		float yMax = -10000.0f;
		float yMin = 10000.0f;
		float zMax = -10000.0f;
		float zMin = 10000.0f;
		for (size_t j=0; j<boxes[i].myVertices.size();j++){
			if(boxes[i].myVertices[j].x > xMax) xMax = boxes[i].myVertices[j].x;
			if(boxes[i].myVertices[j].x < xMin) xMin = boxes[i].myVertices[j].x;
			if(boxes[i].myVertices[j].y > yMax) yMax = boxes[i].myVertices[j].y;
			if(boxes[i].myVertices[j].y < yMin) yMin = boxes[i].myVertices[j].y;
			if(boxes[i].myVertices[j].z > zMax) zMax = boxes[i].myVertices[j].z;
			if(boxes[i].myVertices[j].z < zMin) zMin = boxes[i].myVertices[j].z;
		}
		DynamicBoundingBox dyBBox = DynamicBoundingBox(xMax,yMax,xMin,yMin,zMax,zMin);
		boxes[i].dynamicBox = dyBBox;
	}

}

#endif
