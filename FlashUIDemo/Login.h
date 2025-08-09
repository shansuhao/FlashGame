#pragma once
#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"

using namespace tgui;
namespace Flash {
    class Login : public ChildWindow
    {
    public:
        Login();
        Login(tgui::Gui&);
        ~Login();
    private:
        tgui::ChildWindow::Ptr LoginWin = nullptr;
    };
}