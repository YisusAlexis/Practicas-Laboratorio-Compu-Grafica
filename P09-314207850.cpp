/*
* Facultad de Ingenieria UNAM
* Computacion Grafica e Interaccion Humano Computadora
* Practica 09: Animacion Basica
* 2026-2
* Jesus Alexis Perez Leon
* 314207850
* 22-abril-2026
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

//variables para animación

float rotllanta;
float rotAla;
float dirAla;
float rothelice;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;

// Datos para manejo de puntos de la pista y movimiento del auto y nave
int i = 0;           // Índice del punto actual
float t = 0.0f;      // Progreso entre punto i e i+1 (0.0 a 1.0)
float velocidad = 0.5f;
float distancia;

// Variables para el Translate y Rotate del auto
glm::vec3 posAuto;
float angleY = 0.0f; // YAW (giro horizontal)
float angleX = 0.0f; // PITCH (si sube o baja una rampa)
float angleZ = 0.0f; // ROLL (giro lateral)

//variables para el moviminto del dado
bool estaAnimando = false;
float tiempoInicioAnim = 0.0f;
float anguloX = 0.0f, anguloY = 0.0f, anguloZ = 0.0f;

////////////////////////////////////////////////////
//Rotaciones especificas para cada cara del dado D8 (en grados)
struct ConfigCara {
	float x;
	float z;
};

ConfigCara configsD8[8] = {
	{ 35.26f,   45.0f  }, // Cara 1
	{ 35.26f,  -45.0f  }, // Cara 2
	{-35.26f,  -45.0f  }, // Cara 3
	{-35.26f,   45.0f  }, // Cara 4
	{ 144.74f,  45.0f  }, // Cara 5
	{ 144.74f, -45.0f  }, // Cara 6
	{-144.74f, -45.0f  }, // Cara 7
	{-144.74f,  45.0f  }  // Cara 8
};

//Puntos de la superficie de la pista (obtenidos en Blender)
std::vector<glm::vec3> puntosPista = {
	//glm::vec3(-5.3678f, -1.6908f, 0.6023f),
	//glm::vec3(-5.3678f, -0.0373f, 0.6023f),
	glm::vec3(-5.3737f, 1.5927f, 0.5995f),
	glm::vec3(-5.3845f, 3.2439f, 0.5945f),
	glm::vec3(-5.3971f, 4.8881f, 0.5893f),
	glm::vec3(-5.4151f, 6.5241f, 0.5843f),
	glm::vec3(-5.4411f, 8.1549f, 0.5798f),
	glm::vec3(-5.4763f, 9.7823f, 0.5756f),
	glm::vec3(-5.5233f, 11.4062f, 0.5720f),
	glm::vec3(-5.5818f, 13.0272f, 0.5689f),
	glm::vec3(-5.6531f, 14.6447f, 0.5665f),
	glm::vec3(-5.7385f, 16.2586f, 0.5647f),
	glm::vec3(-5.8390f, 17.8680f, 0.5637f),
	glm::vec3(-5.9559f, 19.4723f, 0.5634f),
	glm::vec3(-6.0904f, 21.0707f, 0.5640f),
	glm::vec3(-6.2439f, 22.6621f, 0.5655f),
	glm::vec3(-6.4210f, 24.2439f, 0.5682f),
	glm::vec3(-6.6242f, 25.8146f, 0.5722f),
	glm::vec3(-6.8541f, 27.3742f, 0.5775f),
	glm::vec3(-7.1127f, 28.9224f, 0.5843f),
	glm::vec3(-7.4022f, 30.4584f, 0.5927f),
	glm::vec3(-7.7246f, 31.9815f, 0.6031f),
	glm::vec3(-8.0822f, 33.4907f, 0.6156f),
	glm::vec3(-8.4791f, 34.9843f, 0.6308f),
	glm::vec3(-8.9175f, 36.4614f, 0.6493f),
	glm::vec3(-9.3954f, 37.9231f, 0.6712f),
	glm::vec3(-9.9138f, 39.3702f, 0.6974f),
	glm::vec3(-10.4732f, 40.8049f, 0.7286f),
	glm::vec3(-11.0735f, 42.2317f, 0.7660f),
	glm::vec3(-11.7139f, 43.6592f, 0.8113f),
	glm::vec3(-12.3964f, 45.1074f, 0.8683f),
	glm::vec3(-13.1128f, 46.6091f, 0.9411f),
	glm::vec3(-13.8517f, 48.2203f, 1.0366f),
	glm::vec3(-14.5747f, 50.0909f, 1.1763f),
	glm::vec3(-15.1398f, 52.4159f, 1.0939f),
	glm::vec3(-15.1108f, 55.1533f, 1.7036f),
	glm::vec3(-14.2608f, 57.7829f, 2.0617f),
	glm::vec3(-12.9710f, 59.8287f, 2.3812f),
	glm::vec3(-11.5699f, 61.4249f, 2.6567f),
	glm::vec3(-10.1634f, 62.7398f, 2.9021f),
	glm::vec3(-8.7754f, 63.8913f, 3.1311f),
	glm::vec3(-7.4088f, 64.9537f, 3.3529f),
	glm::vec3(-6.0661f, 65.9613f, 3.5724f),
	glm::vec3(-4.7507f, 66.9361f, 3.7930f),
	glm::vec3(-3.4669f, 67.8933f, 4.0173f),
	glm::vec3(-2.2207f, 68.8432f, 4.2472f),
	glm::vec3(-1.0263f, 69.7945f, 4.4856f),
	glm::vec3(0.1046f, 70.7546f, 4.7343f),
	glm::vec3(1.1627f, 71.7281f, 4.9944f),
	glm::vec3(2.1352f, 72.7178f, 5.2675f),
	glm::vec3(3.0085f, 73.7251f, 5.5545f),
	glm::vec3(3.7693f, 74.7498f, 5.8565f),
	glm::vec3(4.4072f, 75.7922f, 6.1745f),
	glm::vec3(4.9169f, 76.8550f, 6.5103f),
	glm::vec3(5.2983f, 77.9450f, 6.8673f),
	glm::vec3(5.5546f, 79.0720f, 7.2496f),
	glm::vec3(5.6911f, 80.2444f, 7.6613f),
	glm::vec3(5.7147f, 81.4649f, 8.1042f),
	glm::vec3(5.6347f, 82.7307f, 8.5777f),
	glm::vec3(5.4426f, 84.0420f, 9.0861f),
	glm::vec3(5.1720f, 85.3852f, 9.6222f),
	glm::vec3(4.8424f, 86.7527f, 10.1819f),
	glm::vec3(4.4663f, 88.1401f, 10.7623f),
	glm::vec3(4.0438f, 89.5495f, 11.3688f),
	glm::vec3(3.5971f, 90.9783f, 11.9993f),
	glm::vec3(3.1401f, 92.4201f, 12.6461f),
	glm::vec3(2.6770f, 93.8818f, 13.3180f),
	glm::vec3(2.2252f, 95.3625f, 14.0132f),
	glm::vec3(1.7805f, 96.8532f, 14.7209f),
	glm::vec3(1.3764f, 98.3729f, 15.4597f),
	glm::vec3(0.9937f, 99.9023f, 16.2058f),
	glm::vec3(0.6568f, 101.4533f, 16.9677f),
	glm::vec3(0.3655f, 103.0202f, 17.7331f),
	glm::vec3(0.1175f, 104.6024f, 18.5014f),
	glm::vec3(-0.0649f, 106.2023f, 19.2604f),
	glm::vec3(-0.2076f, 107.8129f, 20.0132f),
	glm::vec3(-0.2783f, 109.4374f, 20.7408f),
	glm::vec3(-0.3067f, 111.0703f, 21.4531f),
	glm::vec3(-0.2770f, 112.7102f, 22.1331f),
	glm::vec3(-0.1904f, 114.3569f, 22.7809f),
	glm::vec3(-0.0814f, 116.0049f, 23.4115f),
	glm::vec3(0.1131f, 117.6562f, 23.9749f),
	glm::vec3(0.3099f, 119.3079f, 24.5363f),
	glm::vec3(0.5673f, 120.9572f, 25.0349f),
	glm::vec3(0.8583f, 122.6051f, 25.4985f),
	glm::vec3(1.1509f, 124.2533f, 25.9605f),
	glm::vec3(1.5064f, 125.8934f, 26.3380f),
	glm::vec3(1.8767f, 127.5317f, 26.6959f),
	glm::vec3(2.2478f, 129.1701f, 27.0525f),
	glm::vec3(2.6575f, 130.8000f, 27.3427f),
	glm::vec3(3.0895f, 132.4250f, 27.5945f),
	glm::vec3(3.5219f, 134.0500f, 27.8456f),
	glm::vec3(3.9557f, 135.6747f, 28.0936f),
	glm::vec3(4.4334f, 137.2849f, 28.2433f),
	glm::vec3(4.9165f, 138.9212f, 28.3926f),
	glm::vec3(5.3934f, 140.5274f, 28.5414f),
	glm::vec3(5.8674f, 142.1154f, 28.6897f),
	glm::vec3(6.3736f, 143.7122f, 28.7534f),
	glm::vec3(6.8831f, 145.3074f, 28.8070f),
	glm::vec3(7.3927f, 146.9027f, 28.8603f),
	glm::vec3(7.9024f, 148.4979f, 28.9134f),
	glm::vec3(8.4154f, 150.0907f, 28.9522f),
	glm::vec3(8.9450f, 151.6717f, 28.9181f),
	glm::vec3(10.0042f, 154.8336f, 28.8494f),
	glm::vec3(11.0637f, 157.9956f, 28.7802f),
	glm::vec3(12.1415f, 161.1349f, 28.5751f),
	glm::vec3(13.2132f, 164.2513f, 28.3476f),
	glm::vec3(14.2927f, 167.3828f, 28.1199f),
	glm::vec3(15.3896f, 170.5241f, 27.7883f),
	glm::vec3(16.4777f, 173.6358f, 27.4185f),
	glm::vec3(17.5658f, 176.7475f, 27.0485f),
	glm::vec3(18.6534f, 179.8562f, 26.6611f),
	//glm::vec3(19.7378f, 182.9467f, 26.1642f),
	//glm::vec3(20.8222f, 186.0372f, 25.6674f)
};

// Variables para el movimiento de la nave
int iNave = puntosPista.size() - 2; // Inicia en el penúltimo punto
float tNave = 0.0f;
bool naveAterrizada = false;
float velocidadNave = 0.15f;
glm::vec3 posNave;
float angleYNave, angleZNave;
int estadoNave = 0;
float tManiobra = 0.0f; // Para controlar el tiempo del aterrizaje
float desplazamientoLateralFinal = 0.0f;
float alturaVueloFinal = 5.0f;


void movimientoAuto(float deltaTime) {
	if (puntosPista.empty() || i >= puntosPista.size() - 1) return;
	//Definir puntos de origen y destino
	glm::vec3 p0 = puntosPista[i];
	glm::vec3 p1 = puntosPista[i + 1];
	// 3. Control de progreso (t)
	// Calculamos la distancia real entre los puntos de Blender
	distancia = glm::distance(p0, p1);
	t += (velocidad * deltaTime) / distancia;
	// Si llegamos al destino, pasamos al siguiente tramo
	if (t >= 1.0f) {
		t = 0.0f;
		i++;
		return;
	}

	// Posición interpolada
	glm::vec3 posInterp = p0 + t * (p1 - p0);
	posAuto.x = posInterp.x;
	posAuto.y = posInterp.z; // La Z de Blender es la altura en OpenGL
	posAuto.z = posInterp.y; // La Y de Blender es la profundidad en OpenGL
	rotllanta -= (velocidad * 30.0f) * deltaTime;

	//CÁLCULO DE DIRECCIÓN (Vectores)
	glm::vec3 direccion;
	direccion.x = p1.x - p0.x;
	direccion.y = p1.z - p0.z; // Diferencia de altura real
	direccion.z = p1.y - p0.y; // Diferencia de profundidad real
	direccion = glm::normalize(direccion);

	//CÁLCULO DE ÁNGULOS (En grados para glm::rotate)
	// angleY: Giro horizontal (Yaw)
	angleY = glm::degrees(atan2(direccion.x, direccion.z));
	// angleX: Inclinación de rampa (Pitch)
	angleX = glm::degrees(-asin(direccion.y));

}

void movimientoNave(float deltaTime) {

	// ESTADO 0: SIGUIENDO LA RUTA
	if (estadoNave == 0) {
		if (puntosPista.size() < 2 || iNave < 0) {
			estadoNave = 1; // Cuando se acaban los puntos, pasamos a maniobra lateral
			return;
		}

		glm::vec3 pOrigen = puntosPista[iNave + 1];
		glm::vec3 pDestino = puntosPista[iNave];

		float dist = glm::distance(pOrigen, pDestino);

		// Evitamos división por cero para evitar saltos/parpadeos
		if (dist > 0.0001f) {
			tNave += (velocidadNave * deltaTime) / dist;
		}
		else {
			tNave = 1.0f;
		}

		if (tNave >= 1.0f) {
			tNave = 0.0f;
			iNave--;
			return;
		}

		glm::vec3 posInterp = pOrigen + tNave * (pDestino - pOrigen);
		posNave.x = posInterp.x;
		posNave.y = posInterp.z + 5.0f;
		posNave.z = posInterp.y;

		// Dirección y Ángulos (Para que no parpadee, solo calculamos si hay movimiento)
		glm::vec3 dir = pDestino - pOrigen;
		if (glm::length(dir) > 0.0001f) {
			dir = glm::normalize(glm::vec3(dir.x, dir.z, dir.y));
			angleYNave = glm::degrees(atan2(dir.x, dir.z));
			angleZNave = glm::degrees(-asin(dir.y));
		}
	}

	// ESTADO 1: MOVERSE A UN LADO (IZQUIERDA/DERECHA)
	else if (estadoNave == 1) {
		tManiobra += deltaTime * (velocidadNave / 10.0f); // Velocidad de la maniobra lateral
		desplazamientoLateralFinal = glm::mix(0.0f, 15.0f, glm::min(tManiobra, 1.0f));
		if (tManiobra >= 1.0f) {
			tManiobra = 0.0f;
			estadoNave = 2; // Siguiente paso: bajar
		}
	}

	// ESTADO 2: BAJAR AL PISO
	else if (estadoNave == 2) {
		tManiobra += deltaTime * (velocidadNave / 10.0f); // Velocidad de descenso
		// Bajamos la altura de 5.0 a 0.0 (o lo que midan tus llantas)
		alturaVueloFinal = glm::mix(5.0f, 1.0f, glm::min(tManiobra, 1.0f));

		if (tManiobra >= 1.0f) {
			estadoNave = 3; // Nave aterrizada completamente
		}
	}
	//Movimento de las alas, siempre aleteando
	rotAla += (velocidadNave * 30.0f) * deltaTime * dirAla;
	// Si llega a 35 grados, empieza a CERRARSE EL ALA. Si llega a -45, empieza a ABRIRSE EL ALA.
	if (rotAla > 35.0f) {
		rotAla = 35.0f;
		dirAla = -1.0f;
	}
	else if (rotAla < -45.0f) {
		rotAla = -45.0f;
		dirAla = 1.0f;
	}
	rothelice += (velocidadNave * 50.0f) * deltaTime;
}



Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;
Texture dadoTexture8;



Model auto_M;
Model Llanta_M;
Model Cofre_M;
Model Pista_M;
Model Nave_M;
Model AlaDerecha_M;
Model AlaIzquierda_M;
Model Helice_M;

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



//cálculo del promedio de las normales para sombreado de Phong
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


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); // todos los números

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un número


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


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}






int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	//CrearDado();
	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

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
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();
	dadoTexture8 = Texture("Textures/dado8.jpeg");
	dadoTexture8.LoadTextureA();


	auto_M = Model();
	auto_M.LoadModel("Models/autoElRey.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llantaElRey.obj");
	Cofre_M = Model();
	Cofre_M.LoadModel("Models/cofreElRey.obj");
	Pista_M = Model();
	Pista_M.LoadModel("Models/pista.obj");
	Nave_M = Model();
	Nave_M.LoadModel("Models/nave.obj");
	AlaDerecha_M = Model();
	AlaDerecha_M.LoadModel("Models/alaDerecha.obj");
	AlaIzquierda_M = Model();
	AlaIzquierda_M.LoadModel("Models/alaIzquierda.obj");
	Helice_M = Model();
	Helice_M.LoadModel("Models/heliceNave.obj");

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
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;

	//luz faro del auto
	spotLights[1] = SpotLight(1.0f, 0.5f, 0.0f, // Color
		1.0f, 1.0f,                             // Intensidad
		-3.0f, 3.0f, 2.0f,                      // Posición
		1.0f, 0.0f, 0.0f,                   // Dirección
		1.0f, 0.0f, 0.001f,                    // Atenuación
		15.0f);                                 // Ángulo
	spotLightCount++;

	//Luz frontal nave
	spotLights[2] = SpotLight(0.31f, 1.00f, 0.39f, // Color
		1.0f, 1.0f,                             // Intensidad
		0.0f, 5.0f, 16.0f,						// Posición
		-1.0f, -1.0f, 0.0f,						// Dirección
		1.0f, 0.0f, 0.001f,                    // Atenuación
		15.0f);                                 // Ángulo
	spotLightCount++;

	//spotlight para el dado de 8 caras, para que se vea bien iluminado
	spotLights[3] = SpotLight(0.20f, 0.86f, 0.78f, // Color
		1.0f, 1.0f,                             // Intensidad
		30.0f, 20.0f, 0.0f,                      // Posición
		0.0f, -1.0f, 0.0f,                   // Dirección
		1.0f, 0.0f, 0.0f,                    // Atenuación
		15.0f);                                 // Ángulo
	spotLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	// VARIABLES DE CONTROL DE ANIMACIONES
	rotllanta = 0.0f;
	rotAla = 0.0f;
	dirAla = 1.0f; // 1 para subir, -1 para bajar
	rothelice = 0.0f;

	// OFFSETS DEL AUTO
	// Ajusta  (positivo o negativo) para mover el auto al medio  de la pista
	float desplazamientoCentro = 4.5f;
	// Ajusta la altura sobre la pista
	float alturaSobrePista = 1.2f;

	// OFFSET DE LA NAVE
	// Ajusta  (positivo o negativo) para mover la nave al medio  de la pista
	float desplazamientoCentroNave = 5.0f;
	// Ajusta la altura sobre la pista
	float alturaVueloNave = 8.0f;



	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelNave(1.0);
	glm::mat4 modelauxNave(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);




	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// Resetear el auto a su posición inicial si se ha presionado la tecla "X"
		if (mainWindow.getResetAuto()) {
			//Reiniciamos variables de la pista
			i = 0;
			t = 0.0f;

			//Reiniciamos rotaciones
			angleZ = 0.0f;
			rotllanta = 0.0f;

			// 3. APAGAMOS la bandera para que no se quede loopeado
			mainWindow.getSetResetAuto(false);
		}

		movimientoAuto(deltaTime);	//funcion de la animacion del auto
		bool resetPresionado = false; //Reset del auto para reiniciar recorrido sobre la pista
		movimientoNave(deltaTime);	//logica de la animacion de la nave

		//Logica animacion del dado de 8 caras
		float tiempoActual = glfwGetTime();

		if (mainWindow.lanzarDado && !estaAnimando) {
			estaAnimando = true;
			tiempoInicioAnim = tiempoActual;
		}

		if (estaAnimando) {
			float tiempoTranscurrido = tiempoActual - tiempoInicioAnim;

			if (tiempoTranscurrido < 2.0f) {
				//ROTACIÓN ALEATORIA (GIRO LOCO)
				anguloX += 500.0f * deltaTime; 
				anguloY += 800.0f * deltaTime;
				anguloZ += 300.0f * deltaTime;
			}
			else {
				// SELECCION DE UNA CARA
				int resultado = mainWindow.caraD8;
				anguloX = configsD8[resultado].x;
				anguloZ = configsD8[resultado].z;
				anguloY = 0.0f; // El ajuste de cara ya se hace con X y Z

				estaAnimando = false;
				mainWindow.lanzarDado = false;
			}
		}

		

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
		uniformTextureOffset = shaderList[0].getOffsetLocation(); // para la textura con movimiento

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Reinicializando variables cada ciclo de reloj
		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//////////////////////////////////////////////////////////////////////////
		//PISTA
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.1f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 modelPista = model; // Base para el auto

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelPista));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pista_M.RenderModel();

		//////////////////////////////////////////////////////////////////////////
		//AUTO
		model = modelPista;
		model = glm::translate(model, glm::vec3(posAuto.x + desplazamientoCentro,
			posAuto.y + alturaSobrePista,
			-posAuto.z));

		//Rotación de trayectoria
		model = glm::rotate(model, glm::radians(-angleY), glm::vec3(0.0f, 1.0f, 0.0f));
		// Ajuste de orientación local
		model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//Rotación de pendiente
		model = glm::rotate(model, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(angleZ), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		auto_M.RenderModel();

		// Faro del auto (Spotlight)
		glm::vec3 faroPos = glm::vec3(modelaux * glm::vec4(-0.4f, 0.0f, 1.35f, 1.0f));
		glm::vec3 faroDir = glm::normalize(glm::vec3(modelaux * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
		spotLights[1].SetFlash(faroPos, faroDir);

		// COFRE
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.19f, 7.38f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cofre_M.RenderModel();

		//LLANTAS
		// Delantera Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.2f, -0.16f, 0.86f));
		model = glm::rotate(model, -(rotllanta)*toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		// Trasera Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.2f, -0.16f, -0.9f));
		model = glm::rotate(model, -(rotllanta)*toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		// Delantera Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.2f, -0.16f, 0.86f));
		model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (rotllanta)*toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		// Trasera Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.2f, -0.16f, -0.9f));
		model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (rotllanta)*toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();


		//////////////////////////////////////////////////////////////////////////
		//NAVE
		model = modelPista;
		model = glm::translate(model, glm::vec3(posNave.x + desplazamientoCentroNave + desplazamientoLateralFinal,
			posNave.y + alturaVueloFinal,
			-posNave.z));
		model = glm::rotate(model, glm::radians(-angleYNave), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(angleZNave), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		modelauxNave = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Nave_M.RenderModel();

		// luz frontal de la nave (Spotlight)
		glm::vec3 navePos = glm::vec3(modelauxNave * glm::vec4(-0.5f, -0.1f, 0.0f, 1.0f));
		glm::vec3 naveDir = glm::normalize(glm::vec3(modelauxNave * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f)));
		spotLights[2].SetFlash(navePos, naveDir);

		//ALA DERECHA
		modelNave = modelauxNave;
		modelNave = glm::translate(modelNave, glm::vec3(-0.06f, -0.04f, -0.23f));
		modelNave = glm::rotate(modelNave, -rotAla * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelNave));
		AlaDerecha_M.RenderModel();

		//ALA IZQUIERDA
		modelNave = modelauxNave;
		modelNave = glm::translate(modelNave, glm::vec3(-0.07f, -0.01f, 0.24f));
		modelNave = glm::rotate(modelNave, rotAla * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelNave));
		AlaIzquierda_M.RenderModel();

		//HELICE IZQUIERDA
		modelNave = modelauxNave;
		modelNave = glm::translate(modelNave, glm::vec3(-0.37f, -0.29f, 0.36f));
		modelNave = glm::rotate(modelNave, rothelice * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelNave));
		Helice_M.RenderModel();

		//HELICE DERECHA
		modelNave = modelauxNave;
		modelNave = glm::translate(modelNave, glm::vec3(-0.37f, -0.29f, -0.36f));
		modelNave = glm::rotate(modelNave, -rothelice * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelNave));
		Helice_M.RenderModel();


		//DADO 8 CARAS (OCTAEDRO)
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(30.0f, 1.0f, 0.0f)); 
		model = glm::rotate(model, glm::radians(anguloX), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(anguloY), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(anguloZ), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture8.UseTexture();
		meshList[7]->RenderMesh();


		//Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
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
