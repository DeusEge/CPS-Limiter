#include <iostream>

#include "gui.h"

float window_multiply = DEFAULT_WINDOW_MULTIPLY;
bool store_data = false;
std::string device = DEFAULT_EVENT;

std::vector<InputDevice> devices;

namespace left {
    uint16_t cooldown = DEFAULT_LEFT_COOLDOWN;
    uint16_t cps_limit = DEFAULT_LEFT_LIMIT;
}

namespace right {
    uint16_t cooldown = DEFAULT_RIGHT_COOLDOWN;
    uint16_t cps_limit = DEFAULT_RIGHT_LIMIT;
}

std::vector<InputDevice> getInputDevices() {
    std::vector<InputDevice> devices;
    const std::string basePath = "/sys/class/input";

    for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
        if (!entry.is_directory())
            continue;

        std::string dirName = entry.path().filename().string();

        // Only eventX folders
        if (dirName.rfind("event", 0) != 0)
            continue;

        std::string nameFile = entry.path().string() + "/device/name";
        std::ifstream file(nameFile);

        std::string name = "Unknown";
        if (file.good()) {
            std::getline(file, name);
        }

        devices.push_back({std::stoi(dirName.substr(5)), false, "/dev/input/" + dirName, name});
    }

    return devices;
}

int main(int argc, char** argv) {
    devices = getInputDevices();
    std::sort(devices.begin(), devices.end(), [](const InputDevice &a, const InputDevice &b){ return a.id < b.id; });

    // guessing mouse device ---
    for (int i = 0; i < devices.size(); i++) {
        if (devices[i].name.find("Mouse") != std::string::npos) {
            device = devices[i].eventPath;
            break;
        }
    }
    for (int i = 0; i < devices.size(); i++) {
        int j = 1;
        bool found = false;
        // keyboard stands for side clicks input
        while ((i + j < devices.size()) and (devices[i + j].name.substr(0, devices[i].name.length()) == devices[i].name)) {
            if (devices[i].name + " Keyboard" == devices[i + j].name) {
                device = devices[i].eventPath;
                found = true;
                break;
            }
            j++;
        }
        if (found) break;
    }
    // ---
    

    // reading stored data ---
    std::ifstream file(DATA_PATH);
    if (file.is_open()) {
        store_data = true;
        std::string line;
        for (int i = 1; getline(file, line); i++) {
            size_t pos = line.find('=');
            std::string value_str = line.substr(pos + 1);
            switch (i) {
                case 1:
                    left::cps_limit = std::stoi(value_str);
                    break;
                case 2:
                    left::cooldown = std::stoi(value_str);
                    break;
                case 3:
                    right::cps_limit = std::stoi(value_str);
                    break;
                case 4:
                    right::cooldown = std::stoi(value_str);
                    break;
                case 5:
                    window_multiply = std::stof(value_str);
                    break;
                case 6:
                    device = value_str;
                    break;
            }
        }
        file.close();
    }
    else store_data = false;
    // ---


    // marking selected device
    for (int i = 0; i < devices.size(); i++) if (devices[i].eventPath == device) devices[i].selected = true;


    // gui
    QApplication app(argc, argv);

    Window window;
    window.setWindowTitle(APP_NAME);
    window.setWindowIcon(QIcon(ICON_PATH));
    window.setFixedSize(WINDOW_WIDTH * window_multiply, WINDOW_HEIGHT * window_multiply);
    window.show();

    return app.exec();
}
