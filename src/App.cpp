#include "App.h"
#include "MainFrame.h"

// This macro tells wxWidgets to create the main() function
wxIMPLEMENT_APP(App);

bool App::OnInit()
{
    // Create the main application window
    MainFrame* frame = new MainFrame("C++ Script Combiner", wxPoint(50, 50), wxSize(1200, 800));
    frame->Show(true);
    return true;
}