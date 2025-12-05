#pragma once
#include <cstdint>
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QMenuBar>
#include <QTimer>
#include <QAction>
#include <QFont>
#include <QPixmap>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QPainter>
#include <QGraphicsColorizeEffect>
#include <QSystemTrayIcon>
#include <fstream>
#include <QGuiApplication>

#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include <string>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

#define APP_NAME "CPS Limiter"
#define CREDIT "CPS Limiter By Deus"
#define DISCORD_LINK "https://discord.gg/zf25y8y3ug"
#define YOUTUBE_LINK "https://youtube.com/watch?v=dQw4w9WgXcQ"
#define GITHUB_LINK "https://github.com/DeusEge/CPS-Limiter/tree/linux"

#define HOME_DIR getenv("HOME")
#define CLI_PATH "/home/pro/Desktop/cpp/CPS-Limiter/cmake-build-release/cps-limiter-cli"
#define MOUSE_LEFT_PIXMAP_PATH "/usr/share/cps-limiter/mouse_left_click.png"
#define MOUSE_RIGHT_PIXMAP_PATH "/usr/share/cps-limiter/mouse_right_click.png"
#define ICON_PATH "/usr/share/cps-limiter/cps-limiter.png"
#define ICON_TRAY_PATH "/usr/share/cps-limiter/cps-limiter.png"
#define DATA_PATH (std::string(getenv("HOME")) + "/.config/cps-limiter/data.txt").c_str()

#define WINDOW_WIDTH 310
#define WINDOW_HEIGHT 280
#define DEFAULT_WINDOW_MULTIPLY 1.0f

#define DEFAULT_EVENT "/dev/input/event3"
#define DEFAULT_LEFT_LIMIT 16
#define DEFAULT_LEFT_COOLDOWN 50
#define DEFAULT_RIGHT_LIMIT 18
#define DEFAULT_RIGHT_COOLDOWN 0

struct InputDevice {
    int id;
    bool selected;
    std::string eventPath;
    std::string name;
};

extern float window_multiply;
extern bool store_data;
extern std::vector<InputDevice> devices;
extern std::string device;

namespace left {
    extern uint16_t cps_limit;
    extern uint16_t cooldown;
}

namespace right {
    extern uint16_t cps_limit;
    extern uint16_t cooldown;
}

class Window : public QMainWindow {
    Q_OBJECT
public:
    Window();

protected:
    void closeEvent(QCloseEvent *event) override;
    bool event(QEvent *event) override;

private slots:
    void create_widgets();

    void hide_to_tray_action_clicked();

    void tray_clicked(QSystemTrayIcon::ActivationReason reason);
    void tray_run_stop_action_clicked();
    void tray_quit_action_clicked();

    void resize_1_0_action_clicked();
    void resize_1_1_action_clicked();
    void resize_1_2_action_clicked();
    void resize_1_3_action_clicked();
    void resize_1_4_action_clicked();
    void resize_1_5_action_clicked();

    void store_data_action_clicked();

    void discord_action_clicked();
    void youtube_action_clicked();
    void github_action_clicked();

    void left_cps_limit_button_clicked();
    void left_cooldown_button_clicked();
    void right_cps_limit_button_clicked();
    void right_cooldown_button_clicked();

    void run_button_clicked();
    void stop_button_clicked();
    void setDefaultInfoMessage();

private:
    QFont font1;
    QFont font2;

    QSystemTrayIcon* tray;
    QMenu* trayMenu;
    QAction* tray_run_stop_action;
    QAction* tray_quit_action;

    QMenu* settings_menu;
    QMenu* communication_menu;

    QMenu* event_menu;
    QMenu* resize_menu;
    QAction* resize_1_0_action;
    QAction* resize_1_1_action;
    QAction* resize_1_2_action;
    QAction* resize_1_3_action;
    QAction* resize_1_4_action;
    QAction* resize_1_5_action;
    std::vector<QAction*> event_actions;
    QAction* store_data_action;
    QAction* hide_to_tray_action;
    QAction* discord_action;
    QAction* youtube_action;
    QAction* github_action;

    QGroupBox* left_groupBox;
    QGroupBox* right_groupBox;
    QLabel* info_label;

    QPixmap mouse_left_click_pixmap;
    QPixmap mouse_left_click_disabled_pixmap;
    QLabel* mouse_left_click;
    QLabel* left_cps_limit_label;
    QLabel* left_cooldown_label;
    QLineEdit* left_cps_limit_edit;
    QLineEdit* left_cooldown_edit;
    QPushButton* left_cps_limit_button;
    QPushButton* left_cooldown_button;
    QLineEdit* left_current_cps_limit_edit;
    QLineEdit* left_current_cooldown_edit;

    QPixmap mouse_right_click_pixmap;
    QPixmap mouse_right_click_disabled_pixmap;
    QLabel* mouse_right_click;
    QLabel* right_cps_limit_label;
    QLabel* right_cooldown_label;
    QLineEdit* right_cps_limit_edit;
    QLineEdit* right_cooldown_edit;
    QPushButton* right_cps_limit_button;
    QPushButton* right_cooldown_button;
    QLineEdit* right_current_cps_limit_edit;
    QLineEdit* right_current_cooldown_edit;
    QPushButton* run_button;
    QPushButton* stop_button;
};