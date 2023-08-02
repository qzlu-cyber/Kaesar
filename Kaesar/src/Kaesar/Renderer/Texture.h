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
		virtual void Bind() const = 0;
		virtual void Bind(unsigned int id) const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static std::shared_ptr<Texture2D> Create(const std::string& filepath, bool vertical);
	};
}