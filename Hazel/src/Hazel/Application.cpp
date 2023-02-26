#include <iostream>

#include "Application.h"

namespace Hazel {

	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run() {
		std::cout << "Hello World!" << std::endl;
		std::cin.get();
	}

}