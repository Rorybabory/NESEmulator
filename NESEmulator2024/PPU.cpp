#include "PPU.h"
#include "Bus.h"
#include "imgui/imgui.h"
#include <glad/glad.h>
#include "palette.h"



uint8_t PPU::ReadRegister(uint8_t index) {
	return registers[index];
}
void PPU::WriteRegister(uint8_t index, uint8_t bits) {
	registers[index] = bits;
}
void PPU::ClearScreen() {
	for (int x = 0; x < 256; ++x) {
		for (int y = 0; y < 224; ++y) {
			int pos = (y * 256 + x);
			screen_data[pos] = x % 64;
		}
	}
}
void PPU::DrawTile(unsigned int index, uint8_t x, uint8_t y, uint8_t colors[4]) {
	index += 0x8000;
	
	for (unsigned int scr_y = 0; scr_y < 8; ++scr_y) {
		uint8_t layer_1 = bus->Read(index + scr_y);
		uint8_t layer_2 = bus->Read(index + scr_y + 8);
		for (unsigned int scr_x = 0; scr_x < 8; ++scr_x) {
			int color = 0;
			if (layer_1 && (1 << scr_x) == (1 << scr_x)) {
				color++;
			}
			if (layer_2 && (1 << scr_x) == (1 << scr_x)) {
				color+=2;
			}

		}
	}
}
void PPU::DrawScreen() {
	ClearScreen();
	if (texture != 0) {
		glDeleteTextures(1, &texture);
	}
	ImGui::Begin("PPU Screen");

	ImGui::SetWindowSize(ImVec2(256 * 3, 224 * 3+40));
	ImGui::SetWindowPos(ImVec2(545, 0));
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	int size = 300;
	ImVec2 base = ImVec2(590, 105);
	int width = 256;
	int height = 224;
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			int pos = (y * width + x) * 3;
			int color_index = screen_data[pos / 3] * 3;
			texture_data[pos + 0] = palette[color_index + 0];
			texture_data[pos + 1] = palette[color_index + 1];
			texture_data[pos + 2] = palette[color_index + 2];
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
	
	ImGui::Image((ImTextureID)texture, ImVec2(256*3, 224*3), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
	//delete[] texture_data;

}
PPU::PPU(Bus * bus) {
	for (int i = 0; i < 8; ++i) {
		registers[i] = 0;
	}
	this->bus = bus;
	scrTex = 0;
	texture_data = new uint8_t[256 * 224 * 3];
	screen_data = new uint8_t[256 * 224];
	texture = 0;

}