#include "MainWindow.h"
#include "AudioGenerator.h"

int main() {
    MainWindow mainWindow;
    mainWindow.init();

    AudioGenerator audioGen;
    audioGen.init();

    mainWindow.run();

    return 0;
}
