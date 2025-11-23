#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include "gui.h"

pid_t pid = 0;
bool running = false;

int start_cli() {
    if (running) return -2;

    std::string cmd = std::string(CLI_PATH) + " "
                    + std::to_string(left::cps_limit) + " "
                    + std::to_string(left::cooldown) + " "
                    + std::to_string(right::cps_limit) + " "
                    + std::to_string(right::cooldown);

    pid = fork();
    if (pid < 0) {
        std::perror("fork");
        return -1;
    }

    if (pid == 0) {
        execlp("pkexec", "pkexec", "/bin/sh", "-c", cmd.c_str(), (char*)nullptr);
        _exit(127);
    }

    running = 1;
    return 0;
}

int stop_cli() {
    if (!running) return -2;

    if (running) {
        running = 0;
        std::string pidstr = std::to_string(pid);
        pid_t child = fork();
        if (child == 0) {
            execlp("pkexec", "pkexec", "/bin/kill", pidstr.c_str(), (char*)NULL);
            perror("execlp");
            _exit(1);
        }
        else {
            int status = 0;
            if (waitpid(child, &status, 0) < 0) {
                perror("waitpid");
                return -1;
            }
            if (WIFEXITED(status)) return WEXITSTATUS(status);
            return -1;
        }
    }
}

Window::Window() {
    mouse_left_click_pixmap = QPixmap(QPixmap(MOUSE_LEFT_PIXMAP_PATH).size());
    mouse_left_click_pixmap.fill(Qt::transparent);
    QPainter paint_left(&mouse_left_click_pixmap);
    paint_left.setCompositionMode(QPainter::CompositionMode_Source);
    paint_left.drawPixmap(0, 0, QPixmap(MOUSE_LEFT_PIXMAP_PATH));
    paint_left.setCompositionMode(QPainter::CompositionMode_SourceIn);
    paint_left.fillRect(mouse_left_click_pixmap.rect(), palette().color(QPalette::WindowText));
    paint_left.end();

    mouse_left_click_disabled_pixmap = QPixmap(QPixmap(MOUSE_LEFT_PIXMAP_PATH).size());
    mouse_left_click_disabled_pixmap.fill(Qt::transparent);
    QPainter paint_left_disabled(&mouse_left_click_disabled_pixmap);
    paint_left_disabled.setCompositionMode(QPainter::CompositionMode_Source);
    paint_left_disabled.drawPixmap(0, 0, QPixmap(MOUSE_LEFT_PIXMAP_PATH));
    paint_left_disabled.setCompositionMode(QPainter::CompositionMode_SourceIn);
    paint_left_disabled.fillRect(mouse_left_click_disabled_pixmap.rect(),
    palette().color(QPalette::Disabled, QPalette::WindowText));
    paint_left_disabled.end();

    mouse_right_click_pixmap = QPixmap(QPixmap(MOUSE_RIGHT_PIXMAP_PATH).size());
    mouse_right_click_pixmap.fill(Qt::transparent);
    QPainter paint_right(&mouse_right_click_pixmap);
    paint_right.setCompositionMode(QPainter::CompositionMode_Source);
    paint_right.drawPixmap(0, 0, QPixmap(MOUSE_RIGHT_PIXMAP_PATH));
    paint_right.setCompositionMode(QPainter::CompositionMode_SourceIn);
    paint_right.fillRect(mouse_right_click_pixmap.rect(), palette().color(QPalette::WindowText));
    paint_right.end();

    mouse_right_click_disabled_pixmap = QPixmap(QPixmap(MOUSE_RIGHT_PIXMAP_PATH).size());
    mouse_right_click_disabled_pixmap.fill(Qt::transparent);
    QPainter paint_right_disabled(&mouse_right_click_disabled_pixmap);
    paint_right_disabled.setCompositionMode(QPainter::CompositionMode_Source);
    paint_right_disabled.drawPixmap(0, 0, QPixmap(MOUSE_RIGHT_PIXMAP_PATH));
    paint_right_disabled.setCompositionMode(QPainter::CompositionMode_SourceIn);
    paint_right_disabled.fillRect(mouse_right_click_disabled_pixmap.rect(), palette().color(QPalette::Disabled, QPalette::WindowText)); paint_right_disabled.end();

    create_widgets();
}

void Window::hide_to_tray_action_clicked() {
    tray->show();
    this->hide();
}

void Window::tray_clicked(QSystemTrayIcon::ActivationReason reason) {
    this->show();
    tray->hide();
}

void Window::tray_quit_action_clicked() {
    stop_cli();
    QApplication::quit();
}

void Window::tray_run_stop_action_clicked() {
    if (!running) run_button_clicked();
    else stop_button_clicked();
}

void Window::create_widgets() {
    // font ---
    {
        font1 = QFont("Arial", 12 * window_multiply);
        font2 = QFont("Calibri", 14 * window_multiply, QFont::Bold);
    }
    // font ---


    // menu ---
    {
        QMenuBar* menuBar = this->menuBar();

        settings_menu = menuBar->addMenu("Settings");
        communication_menu = menuBar->addMenu("Communication");

        resize_menu = new QMenu("Resize", this);
        resize_1_0_action = new QAction("x1.0", this);
        resize_1_1_action = new QAction("x1.1", this);
        resize_1_2_action = new QAction("x1.2", this);
        resize_1_3_action = new QAction("x1.3", this);
        resize_1_4_action = new QAction("x1.4", this);
        resize_1_5_action = new QAction("x1.5", this);

        resize_1_0_action->setCheckable(true);
        resize_1_1_action->setCheckable(true);
        resize_1_2_action->setCheckable(true);
        resize_1_3_action->setCheckable(true);
        resize_1_4_action->setCheckable(true);
        resize_1_5_action->setCheckable(true);

        if (window_multiply == 1.0f) resize_1_0_action->setChecked(true);
        if (window_multiply == 1.1f) resize_1_1_action->setChecked(true);
        if (window_multiply == 1.2f) resize_1_2_action->setChecked(true);
        if (window_multiply == 1.3f) resize_1_3_action->setChecked(true);
        if (window_multiply == 1.4f) resize_1_4_action->setChecked(true);
        if (window_multiply == 1.5f) resize_1_5_action->setChecked(true);

        store_data_action = new QAction("store data", this);
        store_data_action->setCheckable(true);
        if (store_data) store_data_action->setChecked(true);

        hide_to_tray_action = new QAction("Hide To Tray", this);
        discord_action = new QAction("Discord", this);
        youtube_action = new QAction("YouTube", this);
        github_action = new QAction("Github", this);

        settings_menu->addMenu(resize_menu);
        settings_menu->addAction(store_data_action);
        settings_menu->addAction(hide_to_tray_action);

        resize_menu->addAction(resize_1_0_action);
        resize_menu->addAction(resize_1_1_action);
        resize_menu->addAction(resize_1_2_action);
        resize_menu->addAction(resize_1_3_action);
        resize_menu->addAction(resize_1_4_action);
        resize_menu->addAction(resize_1_5_action);

        communication_menu->addAction(discord_action);
        communication_menu->addAction(youtube_action);
        communication_menu->addAction(github_action);

        connect(resize_1_0_action, &QAction::triggered, this, &Window::resize_1_0_action_clicked);
        connect(resize_1_1_action, &QAction::triggered, this, &Window::resize_1_1_action_clicked);
        connect(resize_1_2_action, &QAction::triggered, this, &Window::resize_1_2_action_clicked);
        connect(resize_1_3_action, &QAction::triggered, this, &Window::resize_1_3_action_clicked);
        connect(resize_1_4_action, &QAction::triggered, this, &Window::resize_1_4_action_clicked);
        connect(resize_1_5_action, &QAction::triggered, this, &Window::resize_1_5_action_clicked);

        connect(hide_to_tray_action, &QAction::triggered, this, &Window::hide_to_tray_action_clicked);
        connect(store_data_action, &QAction::triggered, this, &Window::store_data_action_clicked);

        connect(discord_action, &QAction::triggered, this, &Window::discord_action_clicked);
        connect(youtube_action, &QAction::triggered, this, &Window::youtube_action_clicked);
        connect(github_action, &QAction::triggered, this, &Window::github_action_clicked);
    }
    // menu ---


    // tray ---
    {
        tray = new QSystemTrayIcon(this);
        tray->setIcon(QIcon(ICON_TRAY_PATH));
        trayMenu = new QMenu(this);

        tray_run_stop_action = new QAction("Run", this);
        tray_quit_action = new QAction("Quit", this);

        trayMenu->addAction(tray_run_stop_action);
        trayMenu->addAction(tray_quit_action);
        tray->setContextMenu(trayMenu);

        connect(tray, &QSystemTrayIcon::activated, this, &Window::tray_clicked);
        connect(tray_run_stop_action, &QAction::triggered, this, &Window::tray_run_stop_action_clicked);
        connect(tray_quit_action, &QAction::triggered, this, &Window::tray_quit_action_clicked);

        left_groupBox = new QGroupBox("", this);
        left_groupBox->setGeometry(5 * window_multiply, 35 * window_multiply, 300 * window_multiply, 75 * window_multiply);

        right_groupBox = new QGroupBox("", this);
        right_groupBox->setGeometry(5 * window_multiply, 115 * window_multiply, 300 * window_multiply, 75 * window_multiply);
    }
    // tray ---


    // info ---
    {
        info_label = new QLabel(CREDIT, this);
        info_label->setGeometry(5 * window_multiply, 190 * window_multiply, 300 * window_multiply, 30 * window_multiply);
        info_label->setFont(font1);
        info_label->setAlignment(Qt::AlignCenter);
    }
    // info ---


    // left ---
    {
        left_cps_limit_label = new QLabel("CPS Limit:", left_groupBox);
        left_cps_limit_label->setGeometry(50 * window_multiply, 5 * window_multiply, 75 * window_multiply, 30 * window_multiply);
        left_cps_limit_label->setFont(font1);

        left_cps_limit_edit = new QLineEdit("", left_groupBox);
        left_cps_limit_edit->setGeometry(130 * window_multiply, 5 * window_multiply, 55 * window_multiply, 30 * window_multiply);
        left_cps_limit_edit->setFont(font1);
        left_cps_limit_edit->setAlignment(Qt::AlignCenter);
        left_cps_limit_edit->setMaxLength(3);
        left_cps_limit_edit->setValidator(new QIntValidator(0, 999, left_cps_limit_edit));

        left_cooldown_label = new QLabel("Cooldown:", left_groupBox);
        left_cooldown_label->setGeometry(50 * window_multiply, 40 * window_multiply, 75 * window_multiply, 30 * window_multiply);
        left_cooldown_label->setFont(font1);

        left_cooldown_edit = new QLineEdit("", left_groupBox);
        left_cooldown_edit->setGeometry(130 * window_multiply, 40 * window_multiply, 55 * window_multiply, 30 * window_multiply);
        left_cooldown_edit->setFont(font1);
        left_cooldown_edit->setAlignment(Qt::AlignCenter);
        left_cooldown_edit->setMaxLength(3);
        left_cooldown_edit->setValidator(new QIntValidator(0, 999, left_cooldown_edit));

        left_cps_limit_button = new QPushButton("Set", left_groupBox);
        left_cps_limit_button->setGeometry(190 * window_multiply, 5 * window_multiply, 45 * window_multiply, 30 * window_multiply);
        left_cps_limit_button->setFont(font1);
        connect(left_cps_limit_button, &QPushButton::clicked, this, &Window::left_cps_limit_button_clicked);

        left_cooldown_button = new QPushButton("Set", left_groupBox);
        left_cooldown_button->setGeometry(190 * window_multiply, 40 * window_multiply, 45 * window_multiply, 30 * window_multiply);
        left_cooldown_button->setFont(font1);
        connect(left_cooldown_button, &QPushButton::clicked, this, &Window::left_cooldown_button_clicked);

        left_current_cps_limit_edit = new QLineEdit("", left_groupBox);
        left_current_cps_limit_edit->setGeometry(240 * window_multiply, 5 * window_multiply, 55 * window_multiply, 30 * window_multiply);
        left_current_cps_limit_edit->setFont(font1);
        left_current_cps_limit_edit->setAlignment(Qt::AlignCenter);
        left_current_cps_limit_edit->setEnabled(false);
        left_current_cps_limit_edit->setText(std::to_string(left::cps_limit).c_str());

        left_current_cooldown_edit = new QLineEdit("", left_groupBox);
        left_current_cooldown_edit->setGeometry(240 * window_multiply, 40 * window_multiply, 55 * window_multiply, 30 * window_multiply);
        left_current_cooldown_edit->setFont(font1);
        left_current_cooldown_edit->setAlignment(Qt::AlignCenter);
        left_current_cooldown_edit->setEnabled(false);
        left_current_cooldown_edit->setText(std::to_string(left::cooldown).c_str());
    }
    // left ---


    // right ---
    {
        right_cps_limit_label = new QLabel("CPS Limit:", right_groupBox);
        right_cps_limit_label->setGeometry(50 * window_multiply, 5 * window_multiply, 75 * window_multiply, 30 * window_multiply);
        right_cps_limit_label->setFont(font1);

        right_cps_limit_edit = new QLineEdit("", right_groupBox);
        right_cps_limit_edit->setGeometry(130 * window_multiply, 5 * window_multiply, 55 * window_multiply, 30 * window_multiply);
        right_cps_limit_edit->setFont(font1);
        right_cps_limit_edit->setAlignment(Qt::AlignCenter);
        right_cps_limit_edit->setMaxLength(3);
        right_cps_limit_edit->setValidator(new QIntValidator(0, 999, right_cps_limit_edit));

        right_cooldown_label = new QLabel("Cooldown:", right_groupBox);
        right_cooldown_label->setGeometry(50 * window_multiply, 40 * window_multiply, 75 * window_multiply, 30 * window_multiply);
        right_cooldown_label->setFont(font1);

        right_cooldown_edit = new QLineEdit("", right_groupBox);
        right_cooldown_edit->setGeometry(130 * window_multiply, 40 * window_multiply, 55 * window_multiply, 30 * window_multiply);
        right_cooldown_edit->setFont(font1);
        right_cooldown_edit->setAlignment(Qt::AlignCenter);
        right_cooldown_edit->setMaxLength(3);
        right_cooldown_edit->setValidator(new QIntValidator(0, 999, right_cooldown_edit));

        right_cps_limit_button = new QPushButton("Set", right_groupBox);
        right_cps_limit_button->setGeometry(190 * window_multiply, 5 * window_multiply, 45 * window_multiply, 30 * window_multiply);
        right_cps_limit_button->setFont(font1);
        connect(right_cps_limit_button, &QPushButton::clicked, this, &Window::right_cps_limit_button_clicked);

        right_cooldown_button = new QPushButton("Set", right_groupBox);
        right_cooldown_button->setGeometry(190 * window_multiply, 40 * window_multiply, 45 * window_multiply, 30 * window_multiply);
        right_cooldown_button->setFont(font1);
        connect(right_cooldown_button, &QPushButton::clicked, this, &Window::right_cooldown_button_clicked);

        right_current_cps_limit_edit = new QLineEdit("", right_groupBox);
        right_current_cps_limit_edit->setGeometry(240 * window_multiply, 5 * window_multiply, 55 * window_multiply, 30 * window_multiply);
        right_current_cps_limit_edit->setFont(font1);
        right_current_cps_limit_edit->setAlignment(Qt::AlignCenter);
        right_current_cps_limit_edit->setEnabled(false);
        right_current_cps_limit_edit->setText(std::to_string(right::cps_limit).c_str());

        right_current_cooldown_edit = new QLineEdit("", right_groupBox);
        right_current_cooldown_edit->setGeometry(240 * window_multiply, 40 * window_multiply, 55 * window_multiply, 30 * window_multiply);
        right_current_cooldown_edit->setFont(font1);
        right_current_cooldown_edit->setAlignment(Qt::AlignCenter);
        right_current_cooldown_edit->setEnabled(false);
        right_current_cooldown_edit->setText(std::to_string(right::cooldown).c_str());
    }
    // right ---


    // run-stop ---
    {
        run_button = new QPushButton("RUN", this);
        run_button->setGeometry(5 * window_multiply, 220 * window_multiply, 148 * window_multiply, 55 * window_multiply);
        run_button->setFont(font2);
        connect(run_button, &QPushButton::clicked, this, &Window::run_button_clicked);
        run_button->setFocus();

        stop_button = new QPushButton("STOP", this);
        stop_button->setGeometry(157 * window_multiply, 220 * window_multiply, 148 * window_multiply, 55 * window_multiply);
        stop_button->setFont(font2);
        stop_button->setEnabled(false);
        connect(stop_button, &QPushButton::clicked, this, &Window::stop_button_clicked);
    }
    // run-stop ---


    // mouse pixmap ---
    {
        mouse_left_click = new QLabel(left_groupBox);
        mouse_left_click->setPixmap(mouse_left_click_pixmap.scaled(55 * window_multiply, 55 * window_multiply, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        mouse_left_click->move(5 * window_multiply, 10 * window_multiply); // x = 100, y = 150
        mouse_left_click->setAttribute(Qt::WA_TranslucentBackground);

        mouse_right_click = new QLabel(right_groupBox);
        mouse_right_click->setPixmap(mouse_right_click_pixmap.scaled(55 * window_multiply, 55 * window_multiply, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        mouse_right_click->move(5 * window_multiply, 10 * window_multiply); // x = 100, y = 150
        mouse_right_click->setAttribute(Qt::WA_TranslucentBackground);
    }
    // mouse pixmap ---


    // show widgets ---
    {
        left_groupBox->show();
        right_groupBox->show();
        info_label->show();

        mouse_left_click->show();
        left_cps_limit_label->show();
        left_cooldown_label->show();
        left_cps_limit_edit->show();
        left_cooldown_edit->show();
        left_cps_limit_button->show();
        left_cooldown_button->show();
        left_current_cps_limit_edit->show();
        left_current_cooldown_edit->show();

        mouse_right_click->show();
        right_cps_limit_label->show();
        right_cooldown_label->show();
        right_cps_limit_edit->show();
        right_cooldown_edit->show();
        right_cps_limit_button->show();
        right_cooldown_button->show();
        right_current_cps_limit_edit->show();
        right_current_cooldown_edit->show();
        run_button->show();
        stop_button->show();
    }
    // show widgets ---
}

void Window::resize_1_0_action_clicked() {
    if (window_multiply != 1.0f) {
        window_multiply = 1.0f;
        while ( QWidget* w = findChild<QWidget*>() ) delete w;
        create_widgets();
        this->hide(); // to center the window
        this->show(); // to center the window
        this->setFixedSize(WINDOW_WIDTH * window_multiply, WINDOW_HEIGHT * window_multiply);
    }
    resize_1_0_action->setChecked(true);
}

void Window::resize_1_1_action_clicked() {
    if (window_multiply != 1.1f) {
        window_multiply = 1.1f;
        while ( QWidget* w = findChild<QWidget*>() ) delete w;
        create_widgets();
        this->hide(); // to center the window
        this->show(); // to center the window
        this->setFixedSize(WINDOW_WIDTH * window_multiply, WINDOW_HEIGHT * window_multiply);
    }
    resize_1_1_action->setChecked(true);
}

void Window::resize_1_2_action_clicked() {
    if (window_multiply != 1.2f) {
        window_multiply = 1.2f;
        while ( QWidget* w = findChild<QWidget*>() ) delete w;
        create_widgets();
        this->hide(); // to center the window
        this->show(); // to center the window
        this->setFixedSize(WINDOW_WIDTH * window_multiply, WINDOW_HEIGHT * window_multiply);
    }
    resize_1_2_action->setChecked(true);
}

void Window::resize_1_3_action_clicked() {
    if (window_multiply != 1.3f) {
        window_multiply = 1.3f;
        while ( QWidget* w = findChild<QWidget*>() ) delete w;
        create_widgets();
        this->hide(); // to center the window
        this->show(); // to center the window
        this->setFixedSize(WINDOW_WIDTH * window_multiply, WINDOW_HEIGHT * window_multiply);
    }
    resize_1_3_action->setChecked(true);
}

void Window::resize_1_4_action_clicked() {
    if (window_multiply != 1.4f) {
        window_multiply = 1.4f;
        while ( QWidget* w = findChild<QWidget*>() ) delete w;
        create_widgets();
        this->hide(); // to center the window
        this->show(); // to center the window
        this->setFixedSize(WINDOW_WIDTH * window_multiply, WINDOW_HEIGHT * window_multiply);
    }
    resize_1_4_action->setChecked(true);
}

void Window::resize_1_5_action_clicked() {
    if (window_multiply != 1.5f) {
        window_multiply = 1.5f;
        while ( QWidget* w = findChild<QWidget*>() ) delete w;
        create_widgets();
        this->hide(); // to center the window
        this->show(); // to center the window
        this->setFixedSize(WINDOW_WIDTH * window_multiply, WINDOW_HEIGHT * window_multiply);
    }
    resize_1_5_action->setChecked(true);
}

void Window::store_data_action_clicked() {
    store_data = !store_data;
    if (store_data) store_data_action->setChecked(true);
    else store_data_action->setChecked(false);
}

void Window::discord_action_clicked() {
    std::system("xdg-open " DISCORD_LINK);
}

void Window::youtube_action_clicked() {
    std::system("xdg-open " YOUTUBE_LINK);
}

void Window::github_action_clicked() {
    std::system("xdg-open " GITHUB_LINK);
}

void Window::left_cps_limit_button_clicked() {
    left::cps_limit = left_cps_limit_edit->text().toInt();
    left_current_cps_limit_edit->setText(std::to_string(left::cps_limit).c_str());
}

void Window::left_cooldown_button_clicked() {
    left::cooldown = left_cooldown_edit->text().toInt();
    left_current_cooldown_edit->setText(std::to_string(left::cooldown).c_str());
}

void Window::right_cps_limit_button_clicked() {
    right::cps_limit = right_cps_limit_edit->text().toInt();
    right_current_cps_limit_edit->setText(std::to_string(right::cps_limit).c_str());
}

void Window::right_cooldown_button_clicked() {
    right::cooldown = right_cooldown_edit->text().toInt();
    right_current_cooldown_edit->setText(std::to_string(right::cooldown).c_str());
}

void Window::setDefaultInfoMessage() {
    info_label->setText(CREDIT);
}

void Window::closeEvent(QCloseEvent *event) {
    stop_cli();
    if (store_data) {
        std::ofstream file(DATA_PATH);
        if (file.is_open()) {
            file << "left_cps_limit=" + std::to_string(left::cps_limit) + "\n";
            file << "left_cooldown=" + std::to_string(left::cooldown) + "\n";
            file << "right_cps_limit=" + std::to_string(right::cps_limit) + "\n";
            file << "right_cooldown=" + std::to_string(right::cooldown) + "\n";
            std::ostringstream oss;
            oss.imbue(std::locale::classic());   // Classic locale → nokta
            oss << std::fixed << std::setprecision(1) << window_multiply << "f";
            file << "window_multiply=" + oss.str() + "\n";
            file.close();
        }
    }
    else std::remove(DATA_PATH);
    event->accept();
}

bool Window::event(QEvent *event) {
    if (event->type() == QEvent::ApplicationPaletteChange) {
        qDebug() << "theme changed";

        menuBar()->setPalette(qApp->palette());

        mouse_left_click_pixmap = QPixmap(QPixmap(MOUSE_LEFT_PIXMAP_PATH).size());
        mouse_left_click_pixmap.fill(Qt::transparent);
        QPainter paint_left(&mouse_left_click_pixmap);
        paint_left.setCompositionMode(QPainter::CompositionMode_Source);
        paint_left.drawPixmap(0, 0, QPixmap(MOUSE_LEFT_PIXMAP_PATH));
        paint_left.setCompositionMode(QPainter::CompositionMode_SourceIn);
        paint_left.fillRect(mouse_left_click_pixmap.rect(), qApp->palette().color(QPalette::WindowText));
        paint_left.end();

        mouse_left_click_disabled_pixmap = QPixmap(QPixmap(MOUSE_LEFT_PIXMAP_PATH).size());
        mouse_left_click_disabled_pixmap.fill(Qt::transparent);
        QPainter paint_left_disabled(&mouse_left_click_disabled_pixmap);
        paint_left_disabled.setCompositionMode(QPainter::CompositionMode_Source);
        paint_left_disabled.drawPixmap(0, 0, QPixmap(MOUSE_LEFT_PIXMAP_PATH));
        paint_left_disabled.setCompositionMode(QPainter::CompositionMode_SourceIn);
        paint_left_disabled.fillRect(mouse_left_click_disabled_pixmap.rect(), qApp->palette().color(QPalette::Disabled, QPalette::WindowText));
        paint_left_disabled.end();

        mouse_right_click_pixmap = QPixmap(QPixmap(MOUSE_RIGHT_PIXMAP_PATH).size());
        mouse_right_click_pixmap.fill(Qt::transparent);
        QPainter paint_right(&mouse_right_click_pixmap);
        paint_right.setCompositionMode(QPainter::CompositionMode_Source);
        paint_right.drawPixmap(0, 0, QPixmap(MOUSE_RIGHT_PIXMAP_PATH));
        paint_right.setCompositionMode(QPainter::CompositionMode_SourceIn);
        paint_right.fillRect(mouse_right_click_pixmap.rect(), qApp->palette().color(QPalette::WindowText));
        paint_right.end();

        mouse_right_click_disabled_pixmap = QPixmap(QPixmap(MOUSE_RIGHT_PIXMAP_PATH).size());
        mouse_right_click_disabled_pixmap.fill(Qt::transparent);
        QPainter paint_right_disabled(&mouse_right_click_disabled_pixmap);
        paint_right_disabled.setCompositionMode(QPainter::CompositionMode_Source);
        paint_right_disabled.drawPixmap(0, 0, QPixmap(MOUSE_RIGHT_PIXMAP_PATH));
        paint_right_disabled.setCompositionMode(QPainter::CompositionMode_SourceIn);
        paint_right_disabled.fillRect(mouse_right_click_disabled_pixmap.rect(), qApp->palette().color(QPalette::Disabled, QPalette::WindowText)); paint_right_disabled.end();

        if (!running) mouse_left_click->setPixmap(mouse_left_click_pixmap.scaled(55 * window_multiply, 55 * window_multiply, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        else mouse_left_click->setPixmap(mouse_left_click_disabled_pixmap.scaled(55 * window_multiply, 55 * window_multiply, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        if (!running) mouse_right_click->setPixmap(mouse_right_click_pixmap.scaled(55 * window_multiply, 55 * window_multiply, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        else mouse_right_click->setPixmap(mouse_right_click_disabled_pixmap.scaled(55 * window_multiply, 55 * window_multiply, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    return QWidget::event(event);
}

void Window::run_button_clicked() {
    run_button->setEnabled(false);
    stop_button->setEnabled(true);

    mouse_left_click->setPixmap(mouse_left_click_disabled_pixmap.scaled(55 * window_multiply, 55 * window_multiply, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    mouse_right_click->setPixmap(mouse_right_click_disabled_pixmap.scaled(55 * window_multiply, 55 * window_multiply, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    left_cps_limit_button->setEnabled(false);
    left_cps_limit_edit->setEnabled(false);
    left_cooldown_button->setEnabled(false);
    left_cooldown_edit->setEnabled(false);
    right_cps_limit_button->setEnabled(false);
    right_cps_limit_edit->setEnabled(false);
    right_cooldown_button->setEnabled(false);
    right_cooldown_edit->setEnabled(false);
    left_cps_limit_label->setEnabled(false);
    left_cooldown_label->setEnabled(false);
    right_cps_limit_label->setEnabled(false);
    right_cooldown_label->setEnabled(false);
    this->setFocus();

    tray_run_stop_action->setText("Stop");
    start_cli();
    info_label->setText("CPS Limiter Is Running");
    QTimer::singleShot(3000, this, &Window::setDefaultInfoMessage);
}

void Window::stop_button_clicked() {
    run_button->setEnabled(true);
    stop_button->setEnabled(false);

    mouse_left_click->setPixmap(mouse_left_click_pixmap.scaled(55 * window_multiply, 55 * window_multiply, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    mouse_right_click->setPixmap(mouse_right_click_pixmap.scaled(55 * window_multiply, 55 * window_multiply, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    left_cps_limit_button->setEnabled(true);
    left_cps_limit_edit->setEnabled(true);
    left_cooldown_button->setEnabled(true);
    left_cooldown_edit->setEnabled(true);
    right_cps_limit_button->setEnabled(true);
    right_cps_limit_edit->setEnabled(true);
    right_cooldown_button->setEnabled(true);
    right_cooldown_edit->setEnabled(true);
    left_cps_limit_label->setEnabled(true);
    left_cooldown_label->setEnabled(true);
    right_cps_limit_label->setEnabled(true);
    right_cooldown_label->setEnabled(true);
    this->setFocus();

    tray_run_stop_action->setText("Run");
    stop_cli();
    info_label->setText("CPS Limiter Has Stopped");
    QTimer::singleShot(3000, this, &Window::setDefaultInfoMessage);
}