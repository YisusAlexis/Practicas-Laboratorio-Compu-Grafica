/*
* Facultad de Ingenieria UNAM
* Compitacion Grafica e Interaccion Humano Computadora
* 2026-2
* Jesus Alexis Perez Leon
* 314207850
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture dadoTexture8;
Texture peceraTexture;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
Model CarElRey_M;
Model CofreElRey_M;
Model LlantaElRey_M;
Model lamparaPublica;
Model antenaPez;
Model cuerpoPez;
Model focoPez;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void CrearDado()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,

		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,

		// right
		4, 5, 6,
		6, 7, 4,

	};	GLfloat cubo_vertices[] = {
			// front
			//x		y		z		S		T			NX		NY		NZ		//los valores (NX,NY,NZ) son contrarios a la normal
			-0.5f, -0.5f,  0.5f,	0.26f,  0.34f,		0.0f,	0.0f,	-1.0f,	//0
			0.5f, -0.5f,  0.5f,		0.49f,	0.34f,		0.0f,	0.0f,	-1.0f,	//1
			0.5f,  0.5f,  0.5f,		0.49f,	0.66f,		0.0f,	0.0f,	-1.0f,	//2
			-0.5f,  0.5f,  0.5f,	0.26f,	0.66f,		0.0f,	0.0f,	-1.0f,	//3
			// right
			//x		y		z		S		T
			0.5f, -0.5f,  0.5f,	    0.0f,  0.0f,		-1.0f,	0.0f,	0.0f,
			0.5f, -0.5f,  -0.5f,	1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,
			0.5f,  0.5f,  -0.5f,	1.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
			0.5f,  0.5f,  0.5f,	    0.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
			// back
			-0.5f, -0.5f, -0.5f,	0.0f,  0.0f,		0.0f,	0.0f,	1.0f,
			0.5f, -0.5f, -0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	1.0f,
			0.5f,  0.5f, -0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	1.0f,
			-0.5f,  0.5f, -0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	1.0f,

			// left
			//x		y		z		S		T
			-0.5f, -0.5f,  -0.5f,	0.0f,  0.0f,		1.0f,	0.0f,	0.0f,
			-0.5f, -0.5f,  0.5f,	1.0f,	0.0f,		1.0f,	0.0f,	0.0f,
			-0.5f,  0.5f,  0.5f,	1.0f,	1.0f,		1.0f,	0.0f,	0.0f,
			-0.5f,  0.5f,  -0.5f,	0.0f,	1.0f,		1.0f,	0.0f,	0.0f,

			// bottom
			//x		y		z		S		T
			-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	1.0f,	0.0f,
			0.5f,  -0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	1.0f,	0.0f,
			 0.5f,  -0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	1.0f,	0.0f,
			-0.5f, -0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	1.0f,	0.0f,

			//UP
			 //x		y		z		S		T
			 -0.5f, 0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	-1.0f,	0.0f,
			 0.5f,  0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	-1.0f,	0.0f,
			  0.5f, 0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	-1.0f,	0.0f,
			 -0.5f, 0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);



	GLfloat octaedro_vertices[] = {
		// Cara 1 (superior frontal) - Triángulo 1
		//x      y      z      S     T     NX    NY    NZ
		 0.0f,  1.0f,  0.0f,  0.29f,  0.74f,  -0.577,  -0.577,  -0.577,  // Vértice superior
		 1.0f,  0.0f,  0.0f,  0.43f,  0.5f,  -0.577,  -0.577,  -0.577,  // Vértice derecho
		 0.0f,  0.0f,  1.0f,  0.15f,  0.5f,  -0.577,  -0.577,  -0.577,  // Vértice frontal

		 // Cara 2 (superior izquierda) - Triángulo 2
		  0.0f,  1.0f,  0.0f,  0.01f,  0.74f,  0.577f, -0.577f, -0.577f,  // Vértice superior
		  0.0f,  0.0f,  1.0f,  0.29f,  0.74f,  0.577f, -0.577f, -0.577f,  // Vértice frontal
		 -1.0f,  0.0f,  0.01f,  0.15f,  0.5f,  0.577f, -0.577f, -0.577f,  // Vértice izquierdo

		 // Cara 3 (superior posterior) - Triángulo 3
		   0.0f,  1.0f,  0.0f,  0.44f,  0.99f,  0.577f, -0.577f,  0.577f,  // Vértice superior
		  -1.0f,  0.0f,  0.0f,  0.58f,  0.74f,  0.577f, -0.577f,  0.577f,  // Vértice izquierdo
		   0.0f,  0.0f, -1.0f,  0.29f,  0.74f,  0.577f, -0.577f,  0.577f,  // Vértice posterior

		   // Cara 4 (superior derecho) - Triángulo 4
			0.0f,  1.0f,  0.0f,  0.58f,  0.74f,  -0.577f, -0.577f,  0.577f,  // Vértice superior
			0.0f,  0.0f, -1.0f,  0.43f,  0.5f,	 -0.577f, -0.577f,  0.577f,  // Vértice posterior
			1.0f,  0.0f,  0.0f,  0.29f,  0.74f,  -0.577f, -0.577f,  0.577f,  // Vértice derecho

			// Cara 5 (inferior frontal) - Triángulo 5
			 0.0f, -1.0f,  0.0f,  0.29f,  0.26f,   -0.577f,  0.577f, -0.577f,  // Vértice inferior
			 0.0f,  0.0f,  1.0f,  0.15f,  0.5f,    -0.577f,  0.577f, -0.577f,  // Vértice frontal
			 1.0f,  0.0f,  0.0f,  0.43f,  0.5f,    -0.577f,  0.577f, -0.577f,  // Vértice derecho

			 // Cara 6 (inferior izquierdo) - Triángulo 6
			0.0f, -1.0f,  0.0f,  0.01f,  0.25f,  0.577f,  0.577f, -0.577f,  // Vértice inferior
		   -1.0f,  0.0f,  0.0f,  0.15f,  0.5f,  0.577f,  0.577f, -0.577f,  // Vértice izquierdo
			0.0f,  0.0f,  1.0f,  0.29f,  0.26f,   0.577f,  0.577f, -0.577f,  // Vértice frontal

			// Cara 7 (inferior posterior) - Triángulo 7
			 0.0f, -1.0f,  0.0f,  0.44f,  0.01f,  0.577f,  0.577f,  0.577f,  // Vértice inferior
			 0.0f,  0.0f, -1.0f,  0.29f,  0.26f,  0.577f,  0.577f,  0.577f,  // Vértice posterior
			-1.0f,  0.0f,  0.0f,  0.58f,  0.25f,  0.577f,  0.577f,  0.577f,  // Vértice izquierdo

			// Cara 8 (inferior deredcho) - Triángulo 8
			 0.0f, -1.0f,  0.0f,  0.58f,  0.25f,  -0.577f,  0.577f,  0.577f,  // Vértice inferior
			 1.0f,  0.0f,  0.0f,  0.29f,  0.26f,  -0.577f,  0.577f,  0.577f,  // Vértice derecho
			 0.0f,  0.0f, -1.0f,  0.43f,  0.5f,   -0.577f,  0.577f,  0.577f,   // Vértice posterior
	};
	// Índices para el octaedro (24 índices para 8 triángulos)
	GLuint octaedro_indices[] = {
		0, 1, 2,   // Cara 1
		3, 4, 5,   // Cara 2
		6, 7, 8,   // Cara 3
		9, 10, 11, // Cara 4
		12, 13, 14,// Cara 5
		15, 16, 17,// Cara 6
		18, 19, 20,// Cara 7
		21, 22, 23 // Cara 8
	};

	// Creación del mesh
	Mesh* dadoOctaedro = new Mesh();
	dadoOctaedro->CreateMesh(octaedro_vertices, octaedro_indices, 192, 24);
	meshList.push_back(dadoOctaedro);



}
void CrearPecera()
{
	unsigned int indices[] = {
		// En una pecera, queremos ver el interior, por lo que 
		// invertimos el orden de los índices (Clockwise) para que 
		// el culling no oculte las caras internas.

		// frontal (z = 0.5)
		0, 2, 1,
		2, 0, 3,

		// trasera (z = -0.5)
		8, 10, 9,
		10, 8, 11,

		// izquierda (x = -1.0)
		12, 14, 13,
		14, 12, 15,

		// derecha (x = 1.0)
		4, 6, 5,
		6, 4, 7,

		// abajo (y = -0.5)
		16, 18, 17,
		18, 16, 19,

		// arriba (y = 0.5)
		20, 22, 21,
		22, 20, 23
	};

	GLfloat vertices[] = {
		// x      y      z      S     T      NX     NY     NZ

		// Frontal (Normal hacia adentro: +Z -> -Z)
		-1.0f, -0.5f,  0.5f,  0.0f, 0.44f,   0.0f,  0.0f, 1.0f, // 0
		 1.0f, -0.5f,  0.5f,  0.56f, 0.44f,   0.0f,  0.0f, 1.0f, // 1
		 1.0f,  0.5f,  0.5f,  0.56f, 1.0f,   0.0f,  0.0f, 1.0f, // 2
		-1.0f,  0.5f,  0.5f,  0.0f, 1.0f,   0.0f,  0.0f, 1.0f, // 3

		// Derecha (Normal hacia adentro: +X -> -X)
		 1.0f, -0.5f,  0.5f,  0.0f, 0.44f,  1.0f,  0.0f,  0.0f, // 4
		 1.0f, -0.5f, -0.5f,  0.56f, 0.44f,  1.0f,  0.0f,  0.0f, // 5
		 1.0f,  0.5f, -0.5f,  0.56f, 1.0f,  1.0f,  0.0f,  0.0f, // 6
		 1.0f,  0.5f,  0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f, // 7

		 // Trasera (Normal hacia adentro: -Z -> +Z)
		 -1.0f, -0.5f, -0.5f,  0.0f, 0.44f,   0.0f,  0.0f,  -1.0f, // 8
		  1.0f, -0.5f, -0.5f,  0.56f, 0.44f,   0.0f,  0.0f,  -1.0f, // 9
		  1.0f,  0.5f, -0.5f,  0.56f, 1.0f,   0.0f,  0.0f,  -1.0f, // 10
		 -1.0f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  0.0f,  -1.0f, // 11

		 // Izquierda (Normal hacia adentro: -X -> +X)
		 -1.0f, -0.5f, -0.5f,  0.0f, 0.44f,   -1.0f,  0.0f,  0.0f, // 12
		 -1.0f, -0.5f,  0.5f,  0.56f, 0.44f,   -1.0f,  0.0f,  0.0f, // 13
		 -1.0f,  0.5f,  0.5f,  0.56f, 1.0f,   -1.0f,  0.0f,  0.0f, // 14
		 -1.0f,  0.5f, -0.5f,  0.0f, 1.0f,   -1.0f,  0.0f,  0.0f, // 15

		 // Abajo (Normal hacia adentro: -Y -> +Y)
		 -1.0f, -0.5f,  0.5f,  0.72f, 0.0f,   0.0f,  -1.0f,  0.0f, // 16
		  1.0f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  -1.0f,  0.0f, // 17
		  1.0f, -0.5f, -0.5f,  1.0f, 0.42f,   0.0f,  -1.0f,  0.0f, // 18
		 -1.0f, -0.5f, -0.5f,  0.72f, 0.42f,   0.0f,  -1.0f,  0.0f, // 19

		 // Arriba (Normal hacia adentro: +Y -> -Y)
		 -1.0f,  0.5f,  0.5f,  0.0f, 0.0f,   0.0f, 1.0f,  0.0f, // 20
		  1.0f,  0.5f,  0.5f,  0.58f, 0.0f,   0.0f, 1.0f,  0.0f, // 21
		  1.0f,  0.5f, -0.5f,  0.58f, 0.39f,   0.0f, 1.0f,  0.0f, // 22
		 -1.0f,  0.5f, -0.5f,  0.0f, 0.39f,   0.0f, 1.0f,  0.0f  // 23
	};

	Mesh* pecera = new Mesh();
	// 24 vértices * 8 componentes cada uno = 192 floats
	// 12 triángulos * 3 índices = 36 índices
	pecera->CreateMesh(vertices, indices, 192, 36);
	meshList.push_back(pecera);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CrearDado();
	CreateShaders();
	CrearPecera();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	dadoTexture8 = Texture("Textures/dado8.jpeg");
	dadoTexture8.LoadTextureA();
	peceraTexture = Texture("Textures/peceraTexture.png");
	peceraTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	CarElRey_M = Model();
	CarElRey_M.LoadModel("Models/autoElRey.obj");
	CofreElRey_M = Model();
	CofreElRey_M.LoadModel("Models/CofreElRey.obj");
	LlantaElRey_M = Model();
	LlantaElRey_M.LoadModel("Models/LlantaElRey.obj");
	lamparaPublica = Model();
	lamparaPublica.LoadModel("Models/lamparaPublica2.obj");
	antenaPez = Model();
	antenaPez.LoadModel("Models/antena_pez.obj");
	cuerpoPez = Model();
	cuerpoPez.LoadModel("Models/cuerpo_pez.obj");
	focoPez = Model();
	focoPez.LoadModel("Models/foco_pez.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.2f, 0.2f,
		0.0f, -1.0f, 0.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.2f, 0.2f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
	//segunda luz puntual para farola publica
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.5f,
		4.6f, 16.0f, -20.0f,
		1.0f, 0.045f, 0.0075f);
	pointLightCount++;
	//tercer luz puntual para el bulbo del pez
	pointLights[2] = PointLight(0.0f, 0.0f, 1.0f,
		0.5f, 0.5f,
		4.6f, 16.0f, -20.0f,
		1.0f, 0.045f, 0.0075f);
	pointLightCount++;



	unsigned int spotLightCount = 0;
	//linterna
	/*spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;*/

	//Luz naranja para spotlight bulbo pez direccional
	spotLights[0] = SpotLight(1.0f, 0.5f, 0.0f,
		0.5f, 0.5f,
		0.2f, 1.5f, 0.8f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		45.0f);
	spotLightCount++;

	//luz fija verde
	/*spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		0.5f, 0.5f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;*/
	//Luz amarilla para el helicoptero trasera
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.0f,
		1.0f, 0.5f,
		0.0f, 5.0f, 16.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	//Faro azul para el auto
	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 0.5f,
		-3.0f, 3.0f, 2.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//se crean mas luces puntuales y spotlight 
	//Luz amarilla para el helicoptero frontal
	spotLights[3] = SpotLight(1.0f, 1.0f, 0.0f,
		1.0f, 0.5f,
		0.0f, 5.0f, 16.0f,
		0.0f, 1.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	//arreglo de colores para el carro
	glm::vec3 cicloColores[] = {
	glm::vec3(1.0f, 0.0f, 0.0f), // Rojo
	glm::vec3(1.0f, 1.0f, 0.0f), // Amarillo
	glm::vec3(0.0f, 1.0f, 0.0f), // Verde
	glm::vec3(0.0f, 1.0f, 1.0f), // Cian
	glm::vec3(0.0f, 0.0f, 1.0f), // Azul
	glm::vec3(1.0f, 0.0f, 1.0f)  // Magenta
	};

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
	/*	glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());*/


		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		////////////////////
		// Lógica para enviar solo las luces puntuales activas al shader tanto para la farola como para el bulbo del pez.
		//arreglo temporal para enviar al shader
		PointLight lucesPuntualesActivas[3];
		int contadorActivas = 0;

		//agregamos la luz [0] (luz roja) es parte de practicas anteriores y siempre está activa, por lo que no necesita evaluación
		lucesPuntualesActivas[contadorActivas] = pointLights[0];
		contadorActivas++;

		// evakua Luz [1] (Lámpara Pública)
		if (mainWindow.getLuzPrendida()) {
			lucesPuntualesActivas[contadorActivas] = pointLights[1];
			contadorActivas++;
		}

		// Evaluar Luz [2] (Bulbo Pez)
		if (mainWindow.getBulboPrendido()) {
			lucesPuntualesActivas[contadorActivas] = pointLights[2];
			contadorActivas++;
		}

		//Enviar solo las que están activas
		// El shader solo hará el ciclo 'for' hasta 'contadorLucesActivas'
		shaderList[0].SetPointLights(lucesPuntualesActivas, contadorActivas);

		// --- LOGICA DE LUCES DINÁMICAS ---
// Índices originales: 0: Flash, 1: Trasera, 2: Faro Auto, 3: Frontal
// Queremos enviar un número variable de luces según el movimiento

		int lucesActivas = 0;

		if (mainWindow.getLuzFrontHelicoptero()) {
			// CASO: AVANCE FRONTAL (Encender Frontal [3], Apagar Trasera [1])
			// Intercambiamos la 1 con la 3 para que la Frontal esté en una posición visible (índice 1)
			std::swap(spotLights[1], spotLights[3]);

			// Enviamos 4 luces, pero como la trasera ahora está en el índice 3 
			// y solo queremos 3 luces totales (Flash, Frontal, Auto), mandamos 3:
			shaderList[0].SetSpotLights(spotLights, 3);

			// IMPORTANTE: Regresar al orden original para que SetFlash no se confunda el siguiente frame
			std::swap(spotLights[1], spotLights[3]);
		}
		else if (mainWindow.getLuzBackHelicoptero()) {
			// CASO: AVANCE TRASERO (Encender Trasera [1], Apagar Frontal [3])
			// Aquí no hay que swapear nada porque la Trasera ya está en el índice 1.
			// Simplemente enviamos 3 luces. El shader ignorará la luz 3 (Frontal).
			shaderList[0].SetSpotLights(spotLights, 3);
		}
		else {
			// CASO: ESTÁTICO (Ambas apagadas)
			// Movemos la 1 y la 3 al final del arreglo y enviamos solo las luces 0 y 2
			// Para simplificar, si no se mueve, enviamos solo la luz 0 (linterna)
			shaderList[0].SetSpotLights(spotLights, 1);
		}


		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 modelPez(1.0);
		glm::mat4 modelauxPez(1.0);
		glm::mat4 modelauxHelicoptero(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(8.0f, 0.1f, 8.0f));
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), 0.5f, -3.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CarElRey_M.RenderModel();

		//cambio del spotLight del faro del coche de colores
		glm::vec3 c = cicloColores[mainWindow.getColorIndex()];

		// Sobreescribimos la luz manteniendo sus propiedades físicas pero cambiando el color (RGB)
		spotLights[2] = SpotLight(c.x, c.y, c.z,          // el color cambia segun el indice del ciclo de colores
			1.0f, 0.5f,             // Intensidades (Ambiental y Difusa)
			-3.0f, 3.0f, 2.0f,      // Posición original
			-1.0f, 0.0f, 0.0f,      // Dirección original
			1.0f, 0.0f, 0.0f,       // Atenuación (con, lin, exp)
			15.0f);                 // Edge (ángulo)


		// Faro luz azul que sigue la jerarquia del auto
		glm::vec3 faroPos = glm::vec3(modelaux * glm::vec4(-0.4f, 0.0f, 1.35f, 1.0f)); // acomodo del spotlight faro azul (offset)
		glm::vec3 faroDir = glm::normalize(glm::vec3(modelaux * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
		spotLights[2].SetFlash(faroPos, faroDir);

		//Cofre
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.19f, 7.38f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CofreElRey_M.RenderModel();

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.2f, -0.16f, 0.86f));
		//model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaElRey_M.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.2f, -0.16f, 0.86f));
		model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaElRey_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.2f, -0.16f, -0.9f));
		//model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaElRey_M.RenderModel();


		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.2f, -0.16f, -0.9f));
		model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaElRey_M.RenderModel();


		// --- HELICÓPTERO BLACKHAWK ---
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 16.0f));
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevexHelicoptero(), 0.5f, -3.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		// Rotaciones que afectan la orientación local
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelauxHelicoptero = model; // Matriz con toda la transformación

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		//setFlash de la luz amartilla frontal para que semueva junto con el helicoptero
		glm::vec3 heliPos = glm::vec3(modelauxHelicoptero * glm::vec4(0.0f, 3.0f, 0.0f, 1.0f));
		glm::vec3 heliDir = glm::normalize(glm::vec3(modelauxHelicoptero * glm::vec4(0.0f, 1.0f, -1.0f, 0.0f)));
		spotLights[3].SetFlash(heliPos, heliDir);

		//setFlash de la luz amartilla trasera para que semueva junto con el helicoptero
		glm::vec3 heliPos2 = glm::vec3(modelauxHelicoptero * glm::vec4(0.0f, -5.0f, 0.0f, 1.0f));
		glm::vec3 heliDir2 = glm::normalize(glm::vec3(modelauxHelicoptero * glm::vec4(0.0f, -1.0f, -1.0f, 0.0f)));
		spotLights[1].SetFlash(heliPos2, heliDir2);




		//Lampara Publica texturizada
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -20.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.066f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lamparaPublica.RenderModel();



		//Dado Octaedro de Opengl
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture8.UseTexture();
		meshList[5]->RenderMesh();
		///////////////
		//PECERA
		// 
		// 1. Activar Blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// 2. IMPORTANTE: Desactivar la escritura en el Depth Buffer temporalmente
		// Esto permite que las caras de atrás se vean a través de las de adelante
		// sin que el Z-Buffer las descarte.
		glDepthMask(GL_FALSE);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30.0f, 10.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.5f, 5.5f, 5.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		peceraTexture.UseTexture();
		// Usa un material con poco brillo para que el vidrio no sea blanco puro
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[6]->RenderMesh();

		// 3. Limpieza: Reactivar escritura de profundidad y apagar blending
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		/////////////////////////////////////////////
		//Instancia del Pez
		modelPez = glm::mat4(1.0);
		modelPez = glm::translate(modelPez, glm::vec3(30.0f, 9.5f, 0.0f));
		modelPez = glm::translate(modelPez, glm::vec3(0.0f + mainWindow.getmuevexPez(), 0.0f + mainWindow.getmueveyPez(), 0.0f));
		modelPez = glm::scale(modelPez, glm::vec3(0.1f, 0.1f, 0.1f));
		modelPez = glm::rotate(modelPez, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec3 colorCuerpo(0.40f, 0.50f, 0.20f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorCuerpo));
		modelauxPez = modelPez;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelPez));
		cuerpoPez.RenderModel();

		//antena Pez
		modelauxPez = modelPez;
		modelPez = glm::translate(modelPez, glm::vec3(1.3f, 6.5f, -2.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelPez));
		antenaPez.RenderModel();

		//antena Pez
		modelauxPez = modelPez;
		modelPez = glm::translate(modelPez, glm::vec3(-0.2f, 8.9f, 6.7f));
		glm::vec3 colorBulbo(0.65f, 0.25f, 0.25f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorBulbo));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelPez));
		focoPez.RenderModel();

		// pointLight del bulbo del pez que sigue la jerarquía del pez
		glm::vec3 pezPos = glm::vec3(modelauxPez * glm::vec4(0.7f, 0.0f, 0.0f, 1.0f)); // acomodo del pointLight bulbo pez (offset)
		pointLights[2].setPos(glm::vec3(pezPos));

		// spotLight (direccional) del bulbo del pez que sigue la jerarquía del pez
		glm::vec3 spotLightPezPos = glm::vec3(modelPez * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)); // acomodo del spotlight bulbo pez (offset)
		glm::vec3 spotLightPezDir = glm::normalize(glm::vec3(modelauxPez * glm::vec4(mainWindow.getDirx(), 0.0f + mainWindow.getDiry(), 0.0f + mainWindow.getDirz(), 0.0f)));
		spotLights[0].SetFlash(spotLightPezPos, spotLightPezDir);


		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
