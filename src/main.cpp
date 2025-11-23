#include "gui.h"

float window_multiply = DEFAULT_WINDOW_MULTIPLY;
bool store_data = false;

namespace left {
    uint16_t cooldown = DEFAULT_LEFT_COOLDOWN;
    uint16_t cps_limit = DEFAULT_LEFT_LIMIT;
}

namespace right {
    uint16_t cooldown = DEFAULT_RIGHT_COOLDOWN;
    uint16_t cps_limit = DEFAULT_RIGHT_LIMIT;
}

int main(int argc, char** argv) {
    std::ifstream file(DATA_PATH);
    if (file.is_open()) {
        store_data = true;
        std::string line;
        for (int i = 1; getline(file, line); i++) {
            size_t pos = line.find('=');
            std::string value_str = line.substr(pos + 1);
            if (i == 1) left::cps_limit = std::stoi(value_str);
            if (i == 2) left::cooldown = std::stoi(value_str);
            if (i == 3) right::cps_limit = std::stoi(value_str);
            if (i == 4) right::cooldown = std::stoi(value_str);
            if (i == 5) window_multiply = std::stof(value_str);
        }
        file.close();
    }
    else store_data = false;

    QApplication app(argc, argv);

    Window window;
    window.setWindowTitle(APP_NAME);
    window.setWindowIcon(QIcon(ICON_PATH));
    window.setFixedSize(WINDOW_WIDTH * window_multiply, WINDOW_HEIGHT * window_multiply);
    window.show();

    return app.exec();
}
