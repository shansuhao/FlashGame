#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics.hpp>
#include <iterator>
#include <vector>

using namespace sf;

bool runExample(tgui::Gui& gui){
	
	while (const std::optional event = gui.getWindow()->pollEvent())
	{
		Event temp = event.value();
		if (const auto* keyPressed = event->getIf<Event::KeyPressed>())
		{
			switch (keyPressed->scancode) {
			case Keyboard::Scancode::P:
				std::cout << "Hello TGUI!\n";
				break;
			default:
				break;
			}
		}
		gui.handleEvent(event.value());
	}
	return true;
}

int main(int argc, char* argv[])
{
	VideoMode desktop = VideoMode::getDesktopMode();
	RenderWindow window(VideoMode(Vector2u(1920, 1080), desktop.bitsPerPixel), "My Application");
	std::cout << "Desktop Size : { x=" << desktop.size.x << "; y=" << desktop.size.y << " }\n";
	window.setPosition({ (int)(desktop.size.x - window.getSize().x) / 2, (int)(desktop.size.y - window.getSize().y) / 2 });
	
	tgui::Gui gui(window);

	auto btn_close = tgui::Button::create();
	btn_close->setSize(96, 32);
	btn_close->setPosition("70%","70%");
	btn_close->setText("Close");
	btn_close->getRenderer();
	btn_close->onClick([&gui]() {
		gui.getWindow()->close();
	});
	gui.add(btn_close);

	auto btn_Create = tgui::Button::create();
	btn_Create->setSize(96, 32);
	btn_Create->setPosition("30%", "70%");
	btn_Create->setText("Create");
	btn_Create->onClick([&gui]() {
		gui.getWindow()->setVisible(false);
		auto childeWin = tgui::ChildWindow::create("Login");
		childeWin->setSize(640, 480);
		childeWin->setTitleAlignment(tgui::HorizontalAlignment::Left);
		childeWin->setPosition((int)(gui.getWindow()->getSize().x - childeWin->getSize().x) / 2, (int)(gui.getWindow()->getSize().y - childeWin->getSize().y) / 2);
		tgui::ChildWindowRenderer* childWindowRender =  childeWin->getRenderer();
		childWindowRender->setTitleBarHeight(0);
		childWindowRender->setBorderBelowTitleBar(0);
		childeWin->setTitleButtons(0);
		childeWin->setResizable(false);
		gui.add(childeWin, "Login");
	});
	gui.add(btn_Create);

	sf::VertexArray gradient(sf::PrimitiveType::TriangleFan, 4);
	gradient[0].position = sf::Vector2f(0, 0);
	gradient[1].position = sf::Vector2f(window.getSize().x, 0);
	gradient[2].position = sf::Vector2f(window.getSize().x, window.getSize().y);
	gradient[3].position = sf::Vector2f(0, window.getSize().y);
	// 设置四角颜色（从上到下渐变）
	gradient[0].color = sf::Color(52, 152, 219); // 顶部蓝色
	gradient[3].color = sf::Color(46, 204, 113); // 底部绿色

	while (window.isOpen())
	{
		if (runExample(gui)) {
			window.clear(Color(0x1E1E1E));
			window.draw(gradient);
			gui.draw();
			window.display();
		}
	}
	return 0;
}