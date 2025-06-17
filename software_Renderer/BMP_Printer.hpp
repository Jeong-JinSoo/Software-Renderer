/// GPT가 만들어준 이미지 출력 코드.
/// frameBuffer와 depthbuffer를 bmp파일로 출력함.
/// 소프트웨어 렌더어를 디버깅하면서 버퍼들에 값이 제대로 들어간건지 확인하기 어려워서 만듬
///
#pragma once
#include <vector>
#include <fstream>
#include <cstdint>
#include <algorithm>

struct BMP_Printer
{
	static void SaveFrameBufferAsBMP(const std::vector<uint32_t>& frameBuffer, int width, int height, const std::string& filename)
	{
		const int padding = (4 - (width * 3) % 4) % 4;
		const int rowSize = width * 3 + padding;
		const int dataSize = rowSize * height;
		const int fileSize = 54 + dataSize;

		std::ofstream file(filename, std::ios::binary);
		if (!file) return;

		// --- BMP Header ---
		uint8_t header[54] = {
			'B','M',
			static_cast<uint8_t>(fileSize), static_cast<uint8_t>(fileSize >> 8), static_cast<uint8_t>(fileSize >> 16), static_cast<uint8_t>(fileSize >> 24),
			0,0, 0,0, 54,0,0,0, // offset
			40,0,0,0, // DIB header size
			static_cast<uint8_t>(width), static_cast<uint8_t>(width >> 8), static_cast<uint8_t>(width >> 16), static_cast<uint8_t>(width >> 24),
			static_cast<uint8_t>(height), static_cast<uint8_t>(height >> 8), static_cast<uint8_t>(height >> 16), static_cast<uint8_t>(height >> 24),
			1,0, 24,0 // 1 plane, 24 bpp
		};
		file.write(reinterpret_cast<char*>(header), 54);

		// --- Pixel Data ---
		for (int y = height - 1; y >= 0; --y)
		{
			for (int x = 0; x < width; ++x)
			{
				uint32_t pixel = frameBuffer[y * width + x];
				uint8_t r = (pixel >> 16) & 0xFF;
				uint8_t g = (pixel >> 8) & 0xFF;
				uint8_t b = (pixel) & 0xFF;
				file.put(b); file.put(g); file.put(r);
			}
			for (int i = 0; i < padding; ++i) file.put(0);
		}
	}

	static void SaveDepthBufferAsBMP(const std::vector<float>& depthBuffer, int width, int height, const std::string& filename)
	{
		const int padding = (4 - (width * 3) % 4) % 4;
		const int rowSize = width * 3 + padding;
		const int dataSize = rowSize * height;
		const int fileSize = 54 + dataSize;

		std::ofstream file(filename, std::ios::binary);
		if (!file) return;

		// --- BMP Header ---
		uint8_t header[54] = {
			'B','M',
			static_cast<uint8_t>(fileSize), static_cast<uint8_t>(fileSize >> 8), static_cast<uint8_t>(fileSize >> 16), static_cast<uint8_t>(fileSize >> 24),
			0,0, 0,0, 54,0,0,0,
			40,0,0,0,
			static_cast<uint8_t>(width), static_cast<uint8_t>(width >> 8), static_cast<uint8_t>(width >> 16), static_cast<uint8_t>(width >> 24),
			static_cast<uint8_t>(height), static_cast<uint8_t>(height >> 8), static_cast<uint8_t>(height >> 16), static_cast<uint8_t>(height >> 24),
			1,0, 24,0
		};
		file.write(reinterpret_cast<char*>(header), 54);

		// --- Normalize depth buffer ---
		float minDepth = *std::min_element(depthBuffer.begin(), depthBuffer.end());
		float maxDepth = *std::max_element(depthBuffer.begin(), depthBuffer.end());
		float range = maxDepth - minDepth;
		if (range < 1e-6f) range = 1.0f; // avoid division by zero

		for (int y = height - 1; y >= 0; --y)
		{
			for (int x = 0; x < width; ++x)
			{
				float d = (depthBuffer[y * width + x] - minDepth) / range;
				d = std::clamp(d, 0.0f, 1.0f);

				uint8_t v = static_cast<uint8_t>((1.0f - d) * 255.0f); // 반전

				file.put(v); file.put(v); file.put(v); // grayscale
			}
			for (int i = 0; i < padding; ++i) file.put(0);
		}
	}

	// 그라데이션 뎁스 버퍼 생성 (왼쪽 0.0 → 오른쪽 1.0)
	static std::vector<float> GenerateGradientDepthBuffer(float width, float height)
	{
		std::vector<float> buffer(width * height);

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				float depth = static_cast<float>(x) / (width - 1); // 0.0 ~ 1.0
				buffer[y * width + x] = depth;
			}
		}

		return buffer;
	}

};