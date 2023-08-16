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

		virtual void Active(int index) const = 0;
		virtual void Bind(uint32_t slot) const = 0;
		virtual void Unbind() const = 0;
		virtual void BindMultisample(unsigned int id) const = 0;
		virtual void UnbindMultisample() const = 0;

		virtual bool operator==(const Texture& other) const = 0;

		virtual std::string GetPath() const = 0;

		static void BindTexture(uint32_t rendererID, uint32_t index);
	};

	class Texture2D : public Texture
	{
	public:
		static std::shared_ptr<Texture2D> Create(const std::string& filepath, bool vertical, bool sRGB = false); // sRGB �����Ƿ񴴽�һ�� sRGB ����
		static std::shared_ptr<Texture2D> Create(uint32_t width, uint32_t height, const unsigned char* data, bool vertical, bool sRGB = false);
	};
}