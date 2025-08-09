#include "Login.h"

Flash::Login::Login()
{
}

Flash::Login::Login(tgui::Gui& gui)
{
	LoginWin = tgui::ChildWindow::create();
	LoginWin->setPosition("50%","50%");
	LoginWin->setResizable(false);
	gui.add(LoginWin);
}

Flash::Login::~Login()
{
}
