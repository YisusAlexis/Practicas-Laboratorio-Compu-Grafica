/*
Facultad de Ingeniria UNAM
Computaicon Gracfica e Interaccion Humano-Computadora
2026-2
Practica 9
Sesión 2
Compleja: Por medio de funciones y algoritmos.
Textura Animada
Jesus Alexis Perez Leon
29-abril-2026
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
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float toffsetflechau = 0.0f;	//ofset para los parametros de la textura (S,T)
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;
//variables animacion aeolipile
float rotAeolipile = 0.0f;

//variables animacion fuego
float tiempoAnimacion = 0.0f;
float velocidadEscala = 0.5f; // Qué tan rápido "vibra" el fuego
float amplitudEscala = 0.1f;   // Qué tanto varía el tamaño 

//variables para controlar la velocidad en que se muestra los numeros del tablero (velocidad del shader)
float cronometro = 0.0f;
float velocidadNumeros;

//variables para la animacion del humo
float cronometro2 = 0.0f;
float creceHumo = 0.0f;
float velocidadAeolipile = 0.0f;
float aeolipileCaliente;

//variables movimiento brazo catapulta
float rotBrazoCatapulta = 0.0f;
int estadoLanzarBola=0; 
// Variables físicas para el lanzamiento de la pelota/bola
glm::vec3 posPelotaActual;    // Para la posicion de la pelota durante la animacion
glm::vec3 velPelotaActual;    // Velocidad durante la animacion de la pelota (no es la inicial)
float gravedad = -9.81f; // 
float factorRebote = 0.6f;    // Cuánta energía conserva al rebotar (0 a 1)
glm::vec3 posPortal;	//se posiciona en el mismo punto cuando la pelota se detiene
float bolaDesaparece;	//permite desaparecer la bola cuando se detiene en el portal
bool portalPosicionFijada = false;	//desvincula la posicion del portal con la de la pelota

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
Texture FuegoTexture;
Texture HumoTexture;



Model Kitt_M;
Model Llanta_M;
Model Pista_M;
Model Nave_M;
Model Ala_M;
Model Aeolipile_base_M;
Model Aeolipile_M;
Model CatapultaCuerpo;
Model CatapultaBrazo;
Model CatapultaBola;
Model CarreteAeolipile;
Model Portal;

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

	GLfloat planoFuego[] = {
		// Posición (x,y,z)    // Textura (S,T)    // Normal (nx,ny,nz)
		-0.5f, 0.0f, 0.5f,     0.0f, 0.0f,       0.0f, -1.0f, 0.0f,  
		 0.5f, 0.0f, 0.5f,     1.0f, 0.0f,       0.0f, -1.0f, 0.0f,  
		 0.5f, 0.0f,-0.5f,     1.0f, 1.0f,       0.0f, -1.0f, 0.0f,  
		-0.5f, 0.0f,-0.5f,     0.0f, 1.0f,       0.0f, -1.0f, 0.0f   
	};


	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
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
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

	Mesh* obj8 = new Mesh();
	obj8->CreateMesh(planoFuego, numeroIndices, 32, 6);
	meshList.push_back(obj8);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

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
	FuegoTexture = Texture("Textures/fuegoTexture.png");
	FuegoTexture.LoadTextureA();
	HumoTexture = Texture("Textures/humoTexture.png");
	HumoTexture.LoadTextureA();


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Pista_M = Model();
	Pista_M.LoadModel("Models/pista.obj");
	Nave_M = Model();
	Nave_M.LoadModel("Models/nave.obj");
	Ala_M = Model();
	Ala_M.LoadModel("Models/ala.obj");
	Aeolipile_base_M = Model();
	Aeolipile_base_M.LoadModel("Models/Aeolipile_base.obj");
	Aeolipile_M = Model();
	Aeolipile_M.LoadModel("Models/Aeolipile.obj");
	CatapultaCuerpo = Model();
	CatapultaCuerpo.LoadModel("Models/CatapultaCuerpo.obj");
	CatapultaBrazo = Model();
	CatapultaBrazo.LoadModel("Models/CatapultaBrazo.obj");
	CatapultaBola = Model();
	CatapultaBola.LoadModel("Models/CatapultaBola.obj");
	CarreteAeolipile = Model();
	CarreteAeolipile.LoadModel("Models/CarreteAeolipile.obj");
	Portal = Model();
	Portal.LoadModel("Models/portal.obj");

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
		0.5f, 0.5f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//PointLight para la luz del portal
	pointLights[0] = PointLight(0.8f, 0.2f, 0.9f, 
		2.5f, 2.0f,                    
		0.0f, -100.0f, 0.0f,                     
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

	//PointLight 2
	/*spotLights[1] = SpotLight(0.8f, 0.2f, 0.9f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;*/



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;

	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelCatapulta(1.0);
	glm::mat4 modelAuxCatapulta(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		aeolipileCaliente = 100.00f;//valor segun lo que queramos que tarde en calentarse el aeolipile
		//condicion para que gire el aelipile 
		if (mainWindow.getMostrarFuego()) {
			//animacion fuego
			tiempoAnimacion += deltaTime * velocidadEscala;
			//printf("Estado: %d\n", estadoLanzarBola);
			//printf("Angulo brazo: %f\n", rotBrazoCatapulta);
			// /////////////////////////////////////////////////////////////////////////////////////////////////
			//inicio de calentar aeolipile (animacion giro y humo) y movimiento brazo catapulta
			//por estados
			//0: calentar aeolipile (animacion de girar con vapor)
			//1: preparar bola (llevarla a posicion de lanzamiento)
			//2: lanzar bola
			//3: Animacion bola lazada
			//4: Animacion bola cayendo a portal
			//5: Desaparecer el portal junto con su pointlight y posicionar la pelota en el brazo de la catapulta 
			//ESTADO 0: calentando aeolipile
			//////////////////////////////////////////////////////////////////////////////////////////////////////
			cronometro2 += deltaTime;
			if (cronometro2 >= aeolipileCaliente) {
				if (velocidadAeolipile >= 1.5f) { velocidadAeolipile = 1.5f; }
				velocidadAeolipile += 0.001f;
				rotAeolipile += velocidadAeolipile * deltaTime;

				if (creceHumo >= 0.5) { creceHumo = 0.5f; }
				else { creceHumo += 0.001 * deltaTime; }
				if (creceHumo == 0.5f && estadoLanzarBola == 0) { estadoLanzarBola = 1; }//se infiere que ya se calento bien el aeolipile cuando el vapor ya esta en su tamaño máximo desseado, entonces se pasa al siguiente estado de preparar bola
			}
			//Estado 1: preparar bola, llevarla a posicion de lanzamiento
			if (estadoLanzarBola == 1) {
				if (rotBrazoCatapulta <= -90.0f) {
					rotBrazoCatapulta = -90.0f;
					estadoLanzarBola = 2;
				}
				else { rotBrazoCatapulta += -0.5f * deltaTime; }
			}
			////Estado 2: lanzando bola, movimiento de lanzar hasta antes de que salga la pelota del brazo
			if (estadoLanzarBola == 2) {
				if (rotBrazoCatapulta >= 45.0f) {
					// Extraer posición final de la matriz de la bola
					// La cuarta columna de una mat4 contiene la traslación [3][0], [3][1], [3][2]
					posPelotaActual = glm::vec3(modelCatapulta[3][0], modelCatapulta[3][1], modelCatapulta[3][2]);

					
					float fuerzaLanzamiento = 2.0f; //(v0) Fuerza con la que se lanza la pelota
					float anguloRadianes = (rotBrazoCatapulta + 100.0f) * toRadians; // Ajuste del angulo segun la posicion del brazo (direccion de lanzaminto)
					// Ecuaciones de velocidad inicial (aunqe no es inicial ya que tecnicamente lleva movimiento previo pero ese es controlado empiricamente, y no calculado)
					// Velocidad inicial (v0) en tiro parabólico:
					// v0 = velocidad inicial 
					// v0x = v0 * cos(angulo)
					// v0y = v0 * sin(angulo)
					velPelotaActual.x = -cos(anguloRadianes) * fuerzaLanzamiento;
					velPelotaActual.y = sin(anguloRadianes) * fuerzaLanzamiento;
					velPelotaActual.z = 0.0f;

					rotBrazoCatapulta = 45.0f;
					estadoLanzarBola = 3;
				}
				else { rotBrazoCatapulta += 2.0f * deltaTime; }
			}
			//Estado 3: animacion de la bola en el aire
			if (estadoLanzarBola == 3) {
				
				if(rotBrazoCatapulta<=0.0){//regresamos el brazo a su posicion original
					rotBrazoCatapulta = 0.0f;
				}
				else { rotBrazoCatapulta += -0.5f * deltaTime; }
				// Efecto de la gravedad en la velocidad vertical (vy) durante el vuelo de la pelota
				// vy = v0 * sin(angulo) - g * t
				velPelotaActual.y += gravedad * deltaTime * 0.01f;// el factor que multiplica a deltaTIme permit econtrola la velocidad en la escena

				// Actualizacion de la posicion de la pelota en funcion de la velocidad y el tiempo
				posPelotaActual += velPelotaActual * deltaTime;

				// Rebote de la pelota (Suelo en Y = 0)
				// vx = vx_antes  (horizontal NO cambia, sin fricción)
				// vy = -e * vy(anterior)  (vertical se invierte y reduce por e)
				if (posPelotaActual.y <= 0.0f) {
					posPelotaActual.y = 0.0f; // Resetear al suelo
					velPelotaActual.y = -velPelotaActual.y * factorRebote; // Invertir y amortiguar

					// Fricción en el suelo (opcional)
					velPelotaActual.x *= 0.9f;

					// Detener si el rebote es muy pequeño
					if (abs(velPelotaActual.y) < 0.1f) {
						velPelotaActual.y = 0;
						cronometro2 = 0.0f;
						estadoLanzarBola = 4;
					}
				}
			}
			//Estado:4 animacion de la bola cayendo al portal, se asume que el portal esta en el mismo lugar donde la pelota se detiene, 
			// entonces se desvincula la posicion del portal con la de la pelota para que esta pueda caer y desaparecer mientras el portal permanece fijo
			if (estadoLanzarBola == 4) {
				cronometro2 += deltaTime;
				if (cronometro2 <= 300.0f) {
					bolaDesaparece += 0.0005f * deltaTime;
					posPelotaActual.y -= bolaDesaparece;
				}
				else{ 
					estadoLanzarBola = 5; //ESTADO 5: desaparecer el portal y posicionar la pelota en el brazo de la catapulta
				}
			}
		}

		else {
			//SI EL FUEGO ESTA APAGADO REINICIAMOS VARIABLES 
			cronometro2 = 0.0f;
			estadoLanzarBola = 0;
			rotBrazoCatapulta = 0.0f;
	
			//SIMULACION QUE SE ENFRIA EL AEOLIPILE Y DISMINIYE SU GIRO POCO A POCO
			if (velocidadAeolipile <= 0.0f)velocidadAeolipile = 0.0f;
			velocidadAeolipile -= 0.001f;
			rotAeolipile += velocidadAeolipile * deltaTime;
			//SIMULACION DE QUE EL VAPOR DISMINUTE POCO A POCO
			if (creceHumo <= 0.0f)creceHumo = 0.0f;
			else creceHumo -= 0.001 * deltaTime;
		}
		

		angulovaria += 0.5f * deltaTime;
		if (movCoche < 30.0f)
		{
			movCoche -= movOffset * deltaTime;
			//printf("avanza%f \n ",movCoche);
			rotllanta += rotllantaOffset * deltaTime;
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
		uniformTextureOffset = shaderList[0].getOffsetLocation();

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


		//reiniciar variables antes de que sean enviadas al shader

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f); 
		
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		modelCatapulta = glm::mat4(1.0);
		modelAuxCatapulta = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCoche-50.0f, 0.5f, -2.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Aquí va la nave con jerarquía de modelos, completar
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Nave_M.RenderModel();

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala_M.RenderModel();


		//AEOLIPILE CUERPO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aeolipile_base_M.RenderModel();


		//FUEGO
		if (mainWindow.getMostrarFuego()) {
			float factorOscilacion = 1.0f + (sinf(tiempoAnimacion) * amplitudEscala);
			model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(2.0f * factorOscilacion, 2.0f, 3.0f * factorOscilacion));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			FuegoTexture.UseTexture();
			meshList[7]->RenderMesh();
			glDisable(GL_BLEND); // Es buena práctica desactivar el blending después de usarlo
		}

		//AEOLIPILE ESFERA GIRATORIA
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.1f, 5.0f, -0.1f));
		model = glm::rotate(model, (rotAeolipile) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Aeolipile_M.RenderModel();

		//carrete unido al aeolipile simulando que se enrreda la cuerda de la catapulta
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 2.0f));
		model = glm::rotate(model, (rotAeolipile)*toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CarreteAeolipile.RenderModel();


		//HUMO 1 PARA TUBO 1 DE LA ESFERA DEL AEOLIPILE
		model = modelaux;
			model = glm::translate(model, glm::vec3(-0.3f, -0.6f, 0.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(creceHumo, creceHumo*2.0f, creceHumo));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			HumoTexture.UseTexture();
			meshList[7]->RenderMesh();
			glDisable(GL_BLEND); // Es buena práctica desactivar el blending después de usarlo

		//HUMO 2 PARA TUBO 2 DE LA ESFERA DEL AEOLIPILE
		model = modelaux;
			model = glm::translate(model, glm::vec3(0.3f, 0.6f, 0.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(creceHumo, creceHumo * 2.0f, creceHumo));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			HumoTexture.UseTexture();
			meshList[7]->RenderMesh();
			glDisable(GL_BLEND); // Es buena práctica desactivar el blending después de usarlo
		
	//catapulta cuerpo
			modelCatapulta = glm::mat4(1.0);
			modelCatapulta = glm::translate(modelCatapulta, glm::vec3(28.0f, 0.0f, -5.0f));
			modelCatapulta = glm::scale(modelCatapulta, glm::vec3(0.5f, 0.5f, 0.5f));
			modelCatapulta = glm::rotate(modelCatapulta, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			modelAuxCatapulta = modelCatapulta;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCatapulta));
			CatapultaCuerpo.RenderModel();

			//catapulta brazo
			modelCatapulta = modelAuxCatapulta;
			modelCatapulta = glm::translate(modelCatapulta, glm::vec3(0.0f, 35.0f, 0.0f));
			modelCatapulta = glm::rotate(modelCatapulta, rotBrazoCatapulta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
			modelAuxCatapulta = modelCatapulta;
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCatapulta));
			CatapultaBrazo.RenderModel();
			modelCatapulta = modelAuxCatapulta;

			
			//bola catapulta
			if (estadoLanzarBola == 0 || estadoLanzarBola == 1 || estadoLanzarBola == 2) {
				modelCatapulta = modelAuxCatapulta;
				modelCatapulta = glm::translate(modelCatapulta, glm::vec3(29.0f, 11.0f, 1.0f));
				modelCatapulta = glm::scale(modelCatapulta, glm::vec3(2.0f, 2.0f, 2.0f));
				modelAuxCatapulta = modelCatapulta;
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCatapulta));
				CatapultaBola.RenderModel();
			}

			else {
				// instancia de otra pelota que tendra efecto del tiro parabolico pero hereda la posicion inicial del brazo de la catapulta para que parezca que es la misma pelota la que se lanza, pero realmente es otra que se instancia al momento de lanzar y se le aplica la física del tiro parabólico
				glm::mat4 modelBola = glm::mat4(1.0f);
				modelBola = glm::translate(modelBola, glm::vec3(0.0f, 1.0f, 0.0f));
				modelBola = glm::translate(modelBola, posPelotaActual);
				//modelBola = glm::scale(modelBola, glm::vec3(2.0f, 2.0f, 2.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelBola));
				CatapultaBola.RenderModel();

				// Estado 4: animación bola cayendo a portal
				if (estadoLanzarBola == 4) {
					// fijamos la posicion del portal en este estado para que evite segui a la pelota despues de que esta se detiene
					if (!portalPosicionFijada) {
						posPortal = posPelotaActual; //copiamos la posicion de lapelota para que en ese punto aparesca el portal 
						portalPosicionFijada = true;
					}

					glm::mat4 modelPortal = glm::mat4(1.0f);
					modelPortal = glm::translate(modelPortal, glm::vec3(0.0f, -1.5f, 0.0f));
					modelPortal = glm::translate(modelPortal, posPortal);  // Usa posición fija
					modelPortal = glm::rotate(modelPortal, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelPortal));
					Portal.RenderModel();

					// Luz sigue al portal
					pointLights[0].setPos(glm::vec3(posPortal));
				}
				if (estadoLanzarBola == 5) {
					//portalPosicionFijada = false; //Reiniciamos la posicion del portal para el próximo lanzamiento
					modelCatapulta = modelAuxCatapulta;
					modelCatapulta = glm::translate(modelCatapulta, glm::vec3(29.0f, 11.0f, 1.0f));
					modelCatapulta = glm::scale(modelCatapulta, glm::vec3(2.0f, 2.0f, 2.0f));
					modelAuxCatapulta = modelCatapulta;
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCatapulta));
					CatapultaBola.RenderModel();
					pointLights[0].setPos(glm::vec3(0.0f, -100.0f, 0.0f));
				}
			}


		//Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		
		//textura con movimiento
		//Importantes porque la variable uniform no podemos modificarla directamente
		toffsetflechau += 0.001;
		toffsetflechav = 0.000;
		//para que no se desborde la variable
		if (toffsetflechau > 1.0)
			toffsetflechau = 0.0;
		//if (toffsetv > 1.0)
		//	toffsetv = 0;
		//printf("\ntfosset %f \n", toffsetu);
		//pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		FlechaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		//plano con todos los números
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-6.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		//número 1
		//toffsetnumerou = 0.0;
		//toffsetnumerov = 0.0;
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		//glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		for (int i = 1; i<4; i++)
		{
			//números 2-4
			toffsetnumerou += 0.25;
			toffsetnumerov = 0.0;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-10.0f - (i * 3.0), 2.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMesh();

		 }

		for (int j = 1; j < 5; j++)
		{
			//números 5-8
			toffsetnumerou += 0.25;
			toffsetnumerov = -0.33;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-7.0f - (j * 3.0), 5.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMesh();
		}

		//número cambiante 
		/*
		¿Cómo hacer para que sea a una velocidad visible?
		*/
		velocidadNumeros = 20.0f;
		//acumulamos el deltaTime a nuestro cronometro
		cronometro += deltaTime;
		//determinamos si ya pasó el tiempo necesario para cambiar el número
		if (cronometro >= velocidadNumeros) {
			toffsetnumerocambiau += 0.25;	//1/4 porque hay cuatro numeros a lo ancho en la textura
			if (toffsetnumerocambiau >= 1.0)
				toffsetnumerocambiau = 0.0;
			cronometro = 0.0f;	//reiniciamos el cronometro para la siguiente cuenta
		}
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerocambiau, toffsetnumerov);	//mandamos el valor actualizado del numero a mostrar al shader
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();

		//cambiar automáticamente entre textura número 1 y número 2
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Numero1Texture.UseTexture();
		/*if(numero1<19){
			Numero1Texture.UseTexture();
			numero1++;
			
		}
		if (numero1 >= 19) {
			Numero2Texture.UseTexture();
			numero1++;
		}
		if(numero1>=40) {
			
			numero1 = 0;

		}*/
		
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();


		glDisable(GL_BLEND);
		
		



		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
