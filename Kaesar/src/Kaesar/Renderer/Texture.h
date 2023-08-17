#pragma once

#include <string>
#include <memory>

namespace Kaesar {
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void Bind(uint32_t slot) const = 0;
		virtual void UnBind() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual bool operator==(const Texture& other) const = 0;

		virtual std::string GetPath() const = 0;

		static void BindTexture(uint32_t rendererID, uint32_t index);
	};

	class Texture1D : public Texture
	{
	public:
		static std::shared_ptr<Texture1D> Create(uint32_t size);
		static std::shared_ptr<Texture1D> Create(uint32_t size, void* data);
	};

	class Texture2D : public Texture
	{
	public:
		static std::shared_ptr<Texture2D> Create(const std::string& filepath, bool vertical, bool sRGB = false); // sRGB 标明是否创建一个 sRGB 纹理
		static std::shared_ptr<Texture2D> Create(uint32_t width, uint32_t height, const unsigned char* data, bool vertical, bool sRGB = false);
	};
}