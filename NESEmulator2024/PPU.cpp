#include "PPU.h"
#include "Bus.h"
#include "imgui/imgui.h"
#include <glad/glad.h>
uint8_t PPU::ReadRegister(uint8_t index) {
	return registers[index];
}
void PPU::WriteRegister(uint8_t index, uint8_t bits) {
	registers[index] = bits;
}

void PPU::DrawScreen() {
	if (texture != 0) {
		glDeleteTextures(1, &texture);
	}
	ImGui::Begin("PPU Screen");

	ImGui::SetWindowSize(ImVec2(350, 350));
	ImGui::SetWindowPos(ImVec2(580, 75));
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	int size = 300;
	ImVec2 base = ImVec2(590, 105);
	int width = 256;
	int height = 224;
	uint8_t* texture_data = new uint8_t[width * height * 3];
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			int pos = (y * width + x) * 3;
			texture_data[pos + 0] = (uint8_t)(((float)x / (float)width) * 255);
			texture_data[pos + 1] = (uint8_t)(((float)y / (float)height) * 255);
			texture_data[pos + 2] = 0;
		}
	}
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	//for (int x = 0; x < width; ++x) {
	//	for (int y = 0; y < height; ++y) {
	//		ImColor color = { (float)x / width, (float)y / height, 0.0f, 1.0f };

	//		draw_list->AddRectFilled(ImVec2(base.x + x * (size / width), base.y + y * (size / height)), 
	//								 ImVec2(base.x + x * (size / width) + size / width, base.y + y * (size / height) + size / height), 
	//								 color);
	//	}
	//}
	ImGui::Image((ImTextureID)texture, ImVec2(256, 224), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
	delete[] texture_data;

}
PPU::PPU(Bus * bus) {
	for (int i = 0; i < 8; ++i) {
		registers[i] = 0;
	}
	this->bus = bus;
	scrTex = 0;
}