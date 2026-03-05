//Facultad de Ingenieria UNAM
//práctica 3: Modelado Geométrico y Cámara Sintética.
//Computacion Grafica e Interaccion Humano-Computadora
//Jesus Alexis Perez Leon
//03-03-2026
//
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
#include "include/glew.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "include/glfw3.h"
#include "glm/gtc/type_ptr.hpp"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z


using std::vector;

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks


// =========================
// VÉRTICES DEL TETRAEDRO
// =========================
glm::vec3 v0(0.0f, 0.6f, 0.0f);
glm::vec3 v1(-0.5f, -0.3f, 0.5f);
glm::vec3 v2(0.5f, -0.3f, 0.5f);
glm::vec3 v3(0.0f, -0.3f, -0.6f);

float autoRotateAngle = 0.0f;



void CrearCarasTetraedro()
{
	// Cara frontal
	GLfloat cara1[] = {
		 0.0f,  0.6f,  0.0f,
		-0.5f, -0.3f,  0.5f,
		 0.5f, -0.3f,  0.5f
	};
	unsigned int ind1[] = { 0,1,2 };

	Mesh* c1 = new Mesh();
	c1->CreateMesh(cara1, ind1, 9, 3);
	meshList.push_back(c1);

	// Cara derecha
	GLfloat cara2[] = {
		 0.0f,  0.6f,  0.0f,
		 0.5f, -0.3f,  0.5f,
		 0.0f, -0.3f, -0.6f
	};
	unsigned int ind2[] = { 0,1,2 };

	Mesh* c2 = new Mesh();
	c2->CreateMesh(cara2, ind2, 9, 3);
	meshList.push_back(c2);


	// Cara izquierda
	GLfloat cara3[] = {
		 0.0f,  0.6f,  0.0f,
		 0.0f, -0.3f, -0.6f,
		-0.5f, -0.3f,  0.5f
	};
	unsigned int ind3[] = { 0,1,2 };

	Mesh* c3 = new Mesh();
	c3->CreateMesh(cara3, ind3, 9, 3);
	meshList.push_back(c3);


	// Base
	GLfloat cara4[] = {
		-0.5f, -0.3f,  0.5f,
		 0.0f, -0.3f, -0.6f,
		 0.5f, -0.3f,  0.5f
	};
	unsigned int ind4[] = { 0,1,2 };

	Mesh* c4 = new Mesh();
	c4->CreateMesh(cara4, ind4, 9, 3);
	meshList.push_back(c4);

	
}

void CrearMarcosTetraedro(std::vector<Mesh*>& meshList)
{
	float t = 0.07f;
	float offset = 0.0001f;

	// Función lambda interna para crear marco de una cara
	auto CrearMarcoCara = [&](glm::vec3 A, glm::vec3 B, glm::vec3 C)
		{
			glm::vec3 centro = (A + B + C) / 3.0f;

			glm::vec3 A2 = A + (centro - A) * t;
			glm::vec3 B2 = B + (centro - B) * t;
			glm::vec3 C2 = C + (centro - C) * t;

			glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));

			A += normal * offset;
			B += normal * offset;
			C += normal * offset;
			A2 += normal * offset;
			B2 += normal * offset;
			C2 += normal * offset;

			GLfloat marco[] = {

				// AB
				A.x, A.y, A.z,
				B.x, B.y, B.z,
				B2.x, B2.y, B2.z,

				A.x, A.y, A.z,
				B2.x, B2.y, B2.z,
				A2.x, A2.y, A2.z,

				// BC
				B.x, B.y, B.z,
				C.x, C.y, C.z,
				C2.x, C2.y, C2.z,

				B.x, B.y, B.z,
				C2.x, C2.y, C2.z,
				B2.x, B2.y, B2.z,

				// CA
				C.x, C.y, C.z,
				A.x, A.y, A.z,
				A2.x, A2.y, A2.z,

				C.x, C.y, C.z,
				A2.x, A2.y, A2.z,
				C2.x, C2.y, C2.z
			};

			unsigned int indices[] = {
				0,1,2, 3,4,5,
				6,7,8, 9,10,11,
				12,13,14, 15,16,17
			};

			Mesh* m = new Mesh();
			m->CreateMesh(marco, indices, 54, 18);
			meshList.push_back(m);
		};

	// =========================
	// CREAR LAS 4 CARAS
	// =========================

	// Frontal
	CrearMarcoCara(v0, v1, v2);

	// Derecha
	CrearMarcoCara(v0, v2, v3);

	// Izquierda
	CrearMarcoCara(v0, v3, v1);

	// Base
	CrearMarcoCara(v1, v3, v2);
}

void CrearDivisionTrapecio(
	glm::vec3 a,
	glm::vec3 b,
	glm::vec3 normal,
	float t,
	float offset,
	std::vector<Mesh*>& meshList)
{
	glm::vec3 dir = glm::normalize(b - a);

	// ACORTAR SEGMENTO
	float shrink = t * 1.2f;
	a = a + dir * shrink;
	b = b - dir * shrink;

	glm::vec3 perp = glm::normalize(glm::cross(normal, dir));
	glm::vec3 despl = normal * offset;

	glm::vec3 p1 = a - perp * t + despl;
	glm::vec3 p2 = a + perp * t + despl;
	glm::vec3 p3 = b + perp * t + despl;
	glm::vec3 p4 = b - perp * t + despl;

	GLfloat vertices[] = {
		p1.x, p1.y, p1.z,
		p2.x, p2.y, p2.z,
		p3.x, p3.y, p3.z,
		p4.x, p4.y, p4.z
	};

	unsigned int indices[] = { 0,1,2, 0,2,3 };

	Mesh* division = new Mesh();
	division->CreateMesh(
		vertices,
		indices,
		sizeof(vertices) / sizeof(GLfloat),
		sizeof(indices) / sizeof(unsigned int)
	);

	meshList.push_back(division);
}

void CrearDivisionesCara(
	glm::vec3 v0,
	glm::vec3 v1,
	glm::vec3 v2,
	std::vector<Mesh*>& meshList)
{
	float t = 0.009f;
	float offset = 0.001f;

	glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

	// Dividimos en 3 partes
	for (int i = 1; i <= 2; i++)
	{
		float alpha = i / 3.0f;

		// Puntos sobre lado v0-v1
		glm::vec3 a1 = glm::mix(v0, v1, alpha);

		// Puntos sobre lado v0-v2
		glm::vec3 a2 = glm::mix(v0, v2, alpha);

		// Línea paralela a base (v1-v2)
		CrearDivisionTrapecio(a1, a2, normal, t, offset, meshList);
	}

	// Ahora las líneas paralelas a v0-v1
	for (int i = 1; i <= 2; i++)
	{
		float alpha = i / 3.0f;

		glm::vec3 b1 = glm::mix(v1, v0, alpha);
		glm::vec3 b2 = glm::mix(v1, v2, alpha);

		CrearDivisionTrapecio(b1, b2, normal, t, offset, meshList);
	}

	// Líneas paralelas a v0-v2
	for (int i = 1; i <= 2; i++)
	{
		float alpha = i / 3.0f;

		glm::vec3 c1 = glm::mix(v2, v0, alpha);
		glm::vec3 c2 = glm::mix(v2, v1, alpha);

		CrearDivisionTrapecio(c1, c2, normal, t, offset, meshList);
	}
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCarasTetraedro();
	CrearMarcosTetraedro(meshList);
	CrearDivisionesCara(v0, v1, v2, meshList);
	CrearDivisionesCara(v0, v2, v3, meshList);
	CrearDivisionesCara(v0, v3, v1, meshList);
	CrearDivisionesCara(v1, v3, v2, meshList);
	CreateShaders();

	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);


	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);

	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f); //inicializar Color para enviar a variable Uniform;

	glEnable(GL_DEPTH_TEST);

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiamos pantalla
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glm::mat4 viewMat = camera.calculateViewMatrix();
		glm::mat4 currentModel;
		glm::mat4 model;
		glm::vec3 color;
		
		

		autoRotateAngle += deltaTime * 2.0f;  // velocidad 

		model = glm::mat4(1.0f);

		// Transformaciones base
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		model = glm::scale(model, glm::vec3(2.0f));

		//  ROTACIÓN AUTOMÁTICA
		model = glm::rotate(model,
			glm::radians(autoRotateAngle),
			glm::vec3(0.0f, 1.0f, 0.0f));

		model = glm::rotate(model,
			glm::radians(autoRotateAngle * 0.5f),
			glm::vec3(1.0f, 0.0f, 0.0f));

		// cámara
		currentModel = viewMat * model;

		// Enviar al shader
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(currentModel));

		//CARAS TETRAEDRO

		color = glm::vec3(1, 0, 0);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		color = glm::vec3(0, 1, 0);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		color = glm::vec3(0, 0, 1);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMesh();

		color = glm::vec3(1, 1, 0);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMesh();

		//MARCOS PARA LAS CARAS DEL TETRAEDRO
		color = glm::vec3(0, 0, 0);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh();

		color = glm::vec3(0, 0, 0);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh();

		color = glm::vec3(0, 0, 0);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[6]->RenderMesh();

		color = glm::vec3(0, 0, 0);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[7]->RenderMesh();

		//DIVISIONES POR CARA (9 TRIANGULOS)
		color = glm::vec3(0, 0, 0);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		for (size_t i = 7; i < meshList.size(); i++)
		{
			
			meshList[i]->RenderMesh();
		}

		glUseProgram(0);
		mainWindow.swapBuffers();

		
	}
	return 0;
}
