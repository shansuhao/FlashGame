#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

using namespace sf;

bool runExample(tgui::BackendGui& gui) {
	return true;
}

int main(int argc, char* argv[])
{
	RenderWindow window(sf::VideoMode(sf::Vector2u(1920, 1080)), "My Application");
	window.setPosition({10,10});
	tgui::Gui gui(window);

	if (runExample(gui))
		gui.mainLoop();
	
	return 0;
}