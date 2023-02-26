#include <Hazel.h>

class Sandbox : public Hazel::Application
{
public:
	Sandbox();
	~Sandbox();

private:

};

Sandbox::Sandbox()
{
}

Sandbox::~Sandbox()
{
}

// 将引擎的入口点移动到引擎的代码中，Sandbox 只聚焦实际应用
Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox();
}