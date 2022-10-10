#include "Renderer.h"

#include "Walnut/Random.h"
#include <iostream>

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.f);
		uint8_t g = (uint8_t)(color.g * 255.f);
		uint8_t b = (uint8_t)(color.b * 255.f);
		uint8_t a = (uint8_t)(color.a * 255.f);
		
		// Sets format as 0xaabbggrr
		return (a << 24) | (b << 16) | (g << 8) | r;
	}
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

void Renderer::Render()
{
	// Render every pixel of viewport 
	for (uint32_t y{}; y < m_FinalImage->GetHeight(); ++y)
	{
		for (uint32_t x{}; x < m_FinalImage->GetWidth(); ++x)
		{
			// Convert pixel grid to [0-1] space.
			glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
 			
			// Remap coordinate space
			coord = coord * 2.f - 1.f;

			// Get color and clamp to avoid sudden color shifts
			glm::vec4 color = PerPixel(coord);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			// Set image data in pixel to color information.
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
	glm::vec3 rayOrigin{ 0.0f, 0.0f, 1.0f };
	glm::vec3 rayDirection{ coord.x, coord.y, -1.0f }; 

	float radius = 0.5f;

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - (radius * radius);

	// Discriminant
	float discriminant = (b * b) - 4.f * a * c;

	if (discriminant < 0.0f)
		return glm::vec4{ 0,0,0,1 };

	float t0 = (-b + glm::sqrt(discriminant)) / (2.f * a);
	float closestT = (-b - glm::sqrt(discriminant)) / (2.f * a);

	glm::vec3 hitPoint = rayOrigin + rayDirection * closestT;
	glm::vec3 normal = glm::normalize(hitPoint);
	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

	float light = glm::max(glm::dot(-lightDir, normal), 0.f);

	glm::vec3 sphereColor(1,0,1);
	sphereColor *= light;
	return glm::vec4{sphereColor, 1.f};
}
 