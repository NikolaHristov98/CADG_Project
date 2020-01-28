#include <glew.h>
#include <glfw3.h>

#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector> 


#include "src\Renderer.h"
#include "Assert.h"

#include "src\VertexBuffer.h"
#include "src\IndexBuffer.h"
#include "src\VertexArray.h"
#include "src\Shader.h"
#include "src\Texture.h"
#include "src\CADG\PolygonVertices.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

IndexBuffer ib;
VertexBuffer vb;
PolygonVertices pv;
VertexArray va;
VertexBufferLayout layout;

Shader shader;

const int UI_SIZE_WIDTH = 500,
		  MAX_SUB_POLYS = 10;


int steps = 10, pos_point= -1;
float point_size = 8.0f, line_size = 4.0f;
bool move = false;

glm::mat4 proj = glm::ortho(-250.0f, 250.0f, -250.0f, 250.0f, -1.0f, 1.0f);
glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
int width = 1600, height = 900;

float u = 0.25f, v = 0.25f;
bool seeOnlyResult = true, seeMainPoly = true;

void UpdatePoly() {
	if (pv.getIndices().size()) {
		ib.NewData(&(pv.getIndices()[0]), pv.getIndices().size());
		vb.NewData(pv.getArray(), pv.getUsedSize() * sizeof(float));
		va.AddBuffer(vb, layout);
	}
	else {
		ib.NewData(nullptr, 0);
		vb.NewData(nullptr, 0);
		va.AddBuffer(vb, layout);
	}
}

struct Polygon {
	Polygon() {
		color.x = 1.0f;
		color.y = 0.0f;
		color.z = 0.0f;
		color.w = 1.0f;	
	}

	ImVec4 color;
	float* vertices;
	unsigned int* indices;
	IndexBuffer pib;
	VertexBuffer pvb;
	VertexArray pva;
};
Polygon p[MAX_SUB_POLYS];


void calcNewArr(Polygon &p ,const float* arr, int size) {
	if (p.vertices != nullptr) {
		//std::cout << "d" << std::endl;
		delete p.vertices;
		p.vertices = new float[2 * size];
	}

	for (int i = 0; i < size /2; i ++) {

		float x1 = ((u * arr[2*i + 2]) + ((1.0f - u) * arr[2*i]));
		float y1 = ((u * arr[2*i + 3]) + ((1.0f - u) * arr[2*i + 1]));

		p.vertices[4 * i] = x1;
		p.vertices[4 * i + 1] = y1;

		float x2 = (((1.0f - v) * arr[2*i + 2]) + v * arr[2*i]);
		float y2 = (((1.0f - v) * arr[2*i + 3]) + v * arr[2*i + 1]);

		p.vertices[4 * i + 2] = x2;
		p.vertices[4 * i + 3] = y2;
	}

	float x1 = ((u * arr[0]) + ((1.0f - u) * arr[size - 2]));
	float y1 = ((u * arr[1]) + ((1.0f - u) * arr[size - 1]));

	p.vertices[2 * size - 4] = x1;
	p.vertices[2 * size - 3] = y1;

	float x2 = (((1.0f - v) * arr[0]) + ((v) * arr[size - 2]));
	float y2 = (((1.0f - v) * arr[1]) + ((v) * arr[size - 1]));

	p.vertices[2 * size - 2] = x2;
	p.vertices[2 * size - 1] = y2;
}

void genPoly(Polygon* p, const float* arr, int size, int count) {
	if (size == 0) return;
	if (count > steps) return;

	if (p->vertices != nullptr) {
		delete[] p->vertices;
	}
	if (p->indices != nullptr) {
		delete[] p->indices;
	}

	p->vertices = new float[size * 2];
	p->indices = new unsigned int[size];
	calcNewArr(*p, arr, size);

	genPoly(p + 1, p->vertices, 2 * size, count + 1);

	for (int i = 0; i < size; i++) {
		p->indices[i] = i;
	}

	p->pvb.NewData(p->vertices, 2*size * sizeof(float));
	p->pva.AddBuffer(p->pvb, layout);
	p->pib.NewData(&(p->indices[0]), size);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	

	if (action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwGetWindowSize(window, &width, &height);
		float normX = (xpos - ((float)width - UI_SIZE_WIDTH) / 2) / (((float)width - UI_SIZE_WIDTH) / 2),
			normY = (-ypos + (float)height / 2) / ((float)height / 2);

		if (button == GLFW_MOUSE_BUTTON_1 && xpos < width- UI_SIZE_WIDTH)
		{
			pv.addVertex(normX, normY);
			genPoly(p, pv.getArray(), pv.getUsedSize(), 1);
			UpdatePoly();
		}

		if (button == GLFW_MOUSE_BUTTON_2) {
			pv.removePoint(normX, normY, width);
			genPoly(p, pv.getArray(), pv.getUsedSize(), 1);
			UpdatePoly();
		}

		if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			pos_point = pv.getClosestIn(normX, normY, width);
			if(pos_point!=-1) move = true;
		}
	}

	if (action == GLFW_RELEASE) {
		if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			pos_point = -1;
			move = false;
		}
	}
	
}

void window_size_callback(GLFWwindow* window, int width_, int height_)
{
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width-UI_SIZE_WIDTH, height);
}

int main()
{
	GLFWwindow* window;
	std::cout << "HI" << std::endl;
	layout.Push<float>(2);

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_SAMPLES, 16);
	glEnable(GL_MULTISAMPLE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Subdivision", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		std::cout << "Error";
		return 0;
	}

	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	
	for (int i = 0; i < MAX_SUB_POLYS; i++) {
		p[i].pva.Create();
		p[i].pib.CreateNew(nullptr, 0);
		p[i].pvb.CreateNew(nullptr, 0);
	}
	
	ImVec4 main_poly_color = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
	ImVec4 bg_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

	genPoly(p, pv.getArray(), pv.getUsedSize(), 1);
	va.Create();
	ib.CreateNew(nullptr, 0);

	float bg_vert[] = { -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, 1.0f ,-1.0f, 1.0f};
	unsigned int bg_ind[] = { 0, 1, 2, 2, 3, 0 };

	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glEnable(GL_BLEND));

	{
		///Background
		VertexBuffer vb_bg;
		VertexArray va_bg;
		IndexBuffer ib_bg;

		va_bg.Create();
		vb_bg.CreateNew((const void*)bg_vert, 8 * sizeof(float));
		va_bg.AddBuffer(vb_bg, layout);
		ib_bg.CreateNew(bg_ind, 6);

		vb.CreateNew(nullptr, 0);
	
		proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
		glm::mat4 mvp = proj;

		shader.CreateSahder("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniformMat4f("u_MVP", mvp);
		shader.SetUniform1i("u_Texture", 0);

		Renderer renderer;
		glEnable(GL_POINT_SMOOTH);
		glPointSize(point_size);
		glLineWidth(line_size);

		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, false);
		ImGui::StyleColorsDark();

		glViewport(0, 0, width - UI_SIZE_WIDTH, height);

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			shader.Bind();
			
			renderer.Clear();

			ImGui_ImplGlfwGL3_NewFrame();
			ImGui::Begin("Options");
			ImGui::SetWindowPos(ImVec2(width - UI_SIZE_WIDTH, 0));
			ImGui::SetWindowSize(ImVec2(UI_SIZE_WIDTH, 400));

			shader.SetUniform4f("u_Color", bg_color.x, bg_color.y, bg_color.z, bg_color.w);
			renderer.Draw(va_bg, ib_bg, shader, GL_TRIANGLES);
			
			if (pv.getUsedSize()) {
				if (move) {
					double xpos, ypos;
					glfwGetCursorPos(window, &xpos, &ypos);
					glfwGetWindowSize(window, &width, &height);
					float normX = (xpos - ((float)width - UI_SIZE_WIDTH) / 2) / (((float)width - UI_SIZE_WIDTH) / 2),
						normY = (-ypos + (float)height / 2) / ((float)height / 2);

					pv.movePoint(normX, normY, pos_point);
					genPoly(p, pv.getArray(), pv.getUsedSize(), 1);
					UpdatePoly();
					
				}

				for (int i = (seeOnlyResult ? steps -1: 0); i>=0&&i < steps; i++) {
					shader.SetUniform4f("u_Color", p[i].color.x, p[i].color.y, p[i].color.z, p[i].color.w);
					//renderer.Draw(p[i].pva, p[i].pib, shader, GL_POINTS);
					renderer.Draw(p[i].pva, p[i].pib, shader, GL_LINE_LOOP);
				}

				if (seeMainPoly) {
					shader.SetUniform4f("u_Color", main_poly_color.x, main_poly_color.y, main_poly_color.z, main_poly_color.w);
					renderer.Draw(va, ib, shader, GL_POINTS);
					renderer.Draw(va, ib, shader, GL_LINE_LOOP);
				}
		}

			
			{
				ImGui::SliderInt("Iterations: ", &steps, 0, MAX_SUB_POLYS);// Display some text (you can use a format string too)    
				if (ImGui::IsItemActive()) {
					genPoly(p, pv.getArray(), pv.getUsedSize(), 1);
				}

				ImGui::SliderFloat("u ", &u, -0.5, 1.5f);
				if (ImGui::IsItemActive()) {
					genPoly(p, pv.getArray(), pv.getUsedSize(), 1);
				}
				ImGui::SliderFloat("v ", &v, -0.5, 1.5f);
				if (ImGui::IsItemActive()) {
					genPoly(p, pv.getArray(), pv.getUsedSize(), 1);
				}
				
				ImGui::Text("u + 2v = %.1f", u + 2 * v);
				ImGui::Text("2u + v = %.1f", 2 * u + v);

				ImGui::ColorEdit3("Main polygon", (float*)&main_poly_color); // Edit 3 floats representing a color
				
				for (int i = (seeOnlyResult ? steps - 1 : 0); i>=0&&i < steps; i++) {
					std::string text = "Sub polygon " + std::to_string(i);
					ImGui::ColorEdit3(&text[0], (float*)&p[i].color);
				}
				
				ImGui::ColorEdit3("Background", (float*)&bg_color);
				ImGui::Checkbox("Show last polygon", &seeOnlyResult);
				ImGui::Checkbox("Show main polygon", &seeMainPoly);

				ImGui::SliderFloat("Point size", &point_size, 1.0f, 8.0f);
				if (ImGui::IsItemActive()) {
					glPointSize(point_size);
				}

				ImGui::SliderFloat("Line size", &line_size, 1.0f, 8.0f);
				if (ImGui::IsItemActive()) {
					glLineWidth(line_size);
				}

				ImGui::Button("Clear All", ImVec2(100, 20));
				if (ImGui::IsItemClicked()) {
					pv.removeAll();
					genPoly(p, pv.getArray(), pv.getUsedSize(), 1);
				}

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				
			}
			ImGui::End();
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}