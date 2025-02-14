#include "application/application.h"
#include "ofMain.h"

int main()
{
    ofGLFWWindowSettings windowSettings;

    windowSettings.resizable = true;

    windowSettings.setGLVersion(3, 3);

    ofCreateWindow(windowSettings);

    ofRunApp(new Application());
}
