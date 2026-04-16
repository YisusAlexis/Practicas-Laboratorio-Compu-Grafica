#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	GLfloat getmuevexHelicoptero() { return muevexHelicoptero; }
	GLfloat getmuevexPez() { return muevexPez; }
	GLfloat getmueveyPez() { return mueveyPez; }
	GLfloat getDirx() { return Dirx; }
	GLfloat getDiry() { return Diry; }
	GLfloat getDirz() { return Dirz; }
	bool getLuzPrendida() { return luzPrendida; }
	bool getBulboPrendido() { return bulboPrendido; }
	bool getLuzFrontHelicoptero() { return luzFrontHelicoptero; }
	bool getLuzBackHelicoptero() { return luzBackHelicoptero; }
	int getColorIndex() { return colorIndex; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat muevexHelicoptero;
	GLfloat muevexPez;
	GLfloat mueveyPez;	
	GLfloat Dirx;
	GLfloat Diry;
	GLfloat Dirz;
	bool luzPrendida = true;
	bool bulboPrendido = true;
	bool luzFrontHelicoptero = false;
	bool luzBackHelicoptero = false;
	int colorIndex = 4; // Empezamos en 4 (Azul), que es el color original de tu código
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

