#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <thread>
#include <chrono>
#include <iostream>

#define DEFAULT_LEFT_LIMIT 16
#define DEFAULT_LEFT_COOLDOWN 50
#define DEFAULT_RIGHT_LIMIT 18
#define DEFAULT_RIGHT_COOLDOWN 0

namespace left {
    uint16_t cps = 0;
    uint16_t cps_limit = DEFAULT_LEFT_LIMIT; // maximum cps limit
    uint16_t cooldown = DEFAULT_LEFT_COOLDOWN; // if time between last click and previous click smaller than cooldown last click will be blocked
    std::chrono::steady_clock::time_point arr[256]; // the time when you click will be stored in this array
    std::chrono::steady_clock::time_point now; // its declared so it will not declared while algorithm working
    uint8_t start = 0; // index of first value larger than now - 1 ms in arr
    uint8_t end = 0; // index of last value in arr
    bool isDownBlocked = 0; // for block click up message if previous click down blocked
}

namespace right {
    uint16_t cps = 0;
    uint16_t cps_limit = DEFAULT_RIGHT_LIMIT; // maximum cps limit
    uint16_t cooldown = DEFAULT_RIGHT_COOLDOWN; // if time between last click and previous click smaller than cooldown last click will be blocked
    std::chrono::steady_clock::time_point arr[256]; // the time when you click will be stored in this array
    std::chrono::steady_clock::time_point now; // its declared so it will not declared while algorithm working
    uint8_t start = 0; // index of first value larger than now - 1 ms in arr
    uint8_t end = 0; // index of last value in arr
    bool isDownBlocked = 0; // for block click up message if previous click down blocked
}

static volatile bool running = 0;
int sfd;
int ufd;
ssize_t n;

void handleSigint(int signal) {
    std::cout << "\nrunning = 0\n";
    running = 0;
}

void mousehook() {
    struct input_event ev;
    while (running) {
        n = read(sfd, &ev, sizeof(ev));
        if (n == sizeof(ev)) {
            if (ev.type == EV_KEY) { // key event
                if (ev.code == BTN_LEFT) { // left
                    if (ev.value == 1) { // left down
                        left::now = std::chrono::steady_clock::now();
                        if (std::chrono::duration_cast<std::chrono::milliseconds>(left::now - left::arr[left::end]) < std::chrono::milliseconds(left::cooldown)) {
                            left::isDownBlocked = 1;
                            continue; // block the click
                        }
                        if (left::end >= left::start) {
                            left::start = std::upper_bound(left::arr + left::start, left::arr + left::end + 1, left::now - std::chrono::seconds(1)) - left::arr; // find the index of value that larger than now-1
                            left::cps = left::end - left::start + 1;
                        }
                        else {
                            if (left::now - left::arr[255] < std::chrono::seconds(1)) { // index of value that larger than now-1 is between left::arr[left::start] and left::arr[255]
                                left::start = std::upper_bound(left::arr + left::start, left::arr + 256, left::now - std::chrono::seconds(1)) - left::arr;
                                left::cps = left::end - left::start + 256;
                            }
                            else { // index of value that larger than now-1 is between left::arr[0] and left::arr[left::end]
                                left::start = std::upper_bound(left::arr, left::arr + left::end + 1, left::now - std::chrono::seconds(1)) - left::arr;
                                left::cps = left::end - left::start + 1;
                            }
                        }
                        if (left::cps == left::cps_limit) {
                            left::isDownBlocked = 1;
                            continue; // block the click
                        }
                        left::isDownBlocked = 0;
                        left::end++;
                        left::arr[left::end] = left::now;
                    }
                    if (ev.value == 0) { // left up
                        if (left::isDownBlocked) continue; // block click if mouse left click down blocked
                    }
                }

                if (ev.code == BTN_RIGHT) { // right
                    if (ev.value == 1) { // right down
                        right::now = std::chrono::steady_clock::now();
                        if (std::chrono::duration_cast<std::chrono::milliseconds>(right::now - right::arr[right::end]) < std::chrono::milliseconds(right::cooldown)) {
                            right::isDownBlocked = 1;
                            continue; // block the click
                        }
                        if (right::end >= right::start) {
                            right::start = std::upper_bound(right::arr + right::start, right::arr + right::end + 1, right::now - std::chrono::seconds(1)) - right::arr; // find the index of value that larger than now-1
                            right::cps = right::end - right::start + 1;
                        }
                        else {
                            if (right::now - right::arr[255] < std::chrono::seconds(1)) { // index of value that larger than now-1 is between right::arr[right::start] and right::arr[255]
                                right::start = std::upper_bound(right::arr + right::start, right::arr + 256, right::now - std::chrono::seconds(1)) - right::arr;
                                right::cps = right::end - right::start + 256;
                            }
                            else { // index of value that larger than now-1 is between right::arr[0] and right::arr[right::end]
                                right::start = std::upper_bound(right::arr, right::arr + right::end + 1, right::now - std::chrono::seconds(1)) - right::arr;
                                right::cps = right::end - right::start + 1;
                            }
                        }
                        if (right::cps == right::cps_limit) {
                            right::isDownBlocked = 1;
                            continue; // block the click
                        }
                        right::isDownBlocked = 0;
                        right::end++;
                        right::arr[right::end] = right::now;
                    }
                    if (ev.value == 0) { // right up
                        if (right::isDownBlocked) continue; // block click if mouse right click down blocked
                    }
                }
            }
            write(ufd, &ev, sizeof(ev));

            struct input_event sync = { 0 };
            gettimeofday(&sync.time, NULL);
            sync.type = EV_SYN; sync.code = SYN_REPORT; sync.value = 0;
            write(ufd, &sync, sizeof(sync));

        } else if (n < 0) { // read başarısız olabilir; kısa uyku
            std::cout << "sleeping" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    // removing mousehook
    std::cout << "remove mouse hook" << std::endl;
    ioctl(sfd, EVIOCGRAB, 0);
    ioctl(ufd, UI_DEV_DESTROY);
    close(ufd);
    close(sfd);
}

int create_uinput_device(const char* name) {
    int ufd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (ufd < 0) {
        perror("open /dev/uinput");
        return -1;
    }

    // Enable EV_KEY and EV_REL and EV_SYN
    ioctl(ufd, UI_SET_EVBIT, EV_KEY);
    ioctl(ufd, UI_SET_EVBIT, EV_REL);
    ioctl(ufd, UI_SET_RELBIT, REL_X);
    ioctl(ufd, UI_SET_RELBIT, REL_Y);
    ioctl(ufd, UI_SET_RELBIT, REL_WHEEL);

    // Enable common buttons
    ioctl(ufd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(ufd, UI_SET_KEYBIT, BTN_RIGHT);
    ioctl(ufd, UI_SET_KEYBIT, BTN_MIDDLE);
    ioctl(ufd, UI_SET_KEYBIT, BTN_SIDE);
    ioctl(ufd, UI_SET_KEYBIT, BTN_EXTRA);

    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "%s", name);
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1234;
    uidev.id.product = 0x5678;
    uidev.id.version = 1;

    if (write(ufd, &uidev, sizeof(uidev)) < 0) {
        perror("write uidev");
        close(ufd);
        return -1;
    }

    if (ioctl(ufd, UI_DEV_CREATE) < 0) {
        perror("UI_DEV_CREATE");
        close(ufd);
        return -1;
    }

    // small pause to let device appear
    sleep(1);
    return ufd;
}

void remove_mousehook() { // removing mouse hook if exist
    if (running) {
        std::cout << "Remove mouse hook" << std::endl;
        running = 0;
    }
}

int create_mousehook() {
    if (running) {
        std::cout << "mousehook is already running" << std::endl;
    }
    else {
        std::cout << "create mousehook" << std::endl;

        running = 1;

        int error = 0;

        sfd = open("/dev/input/event3", O_RDONLY);
        if (sfd < 0) {
            error = 2;
            std::cout << "Failed to create mousehook. Error code: " << error << std::endl;
        }

        if (ioctl(sfd, EVIOCGRAB, 1) < 0) {
            error = 3;
            std::cout << "Failed to create mousehook. Error code: " << error << std::endl;
            perror("EVIOCGRAB (grab)");
        }

        ufd = create_uinput_device("cps-limiter");
        if (ufd < 0) {
            error = 4;
            std::cout << "Failed to create mousehook. Error code: " << error << std::endl;
            ioctl(sfd, EVIOCGRAB, 0);
        }

        if (error == 0) mousehook();
        return error;
    }
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, handleSigint);

    std::cout << "\ncli log start ---"<< std::endl;

    if (geteuid() != 0) {
        std::cout << "you must run as root" << std::endl;
        return -1;
    }

    if (argc == 1) {
        std::cout << "usage: ./CPS-Limiter-Cli <left-limit> <left-cooldown> <right-limit> <right-cooldown>" << std::endl;
        std::cout << "running with default settings" << std::endl;
    }
    else if (argc == 5) {
        left::cps_limit = std::stoi(argv[1]);
        left::cooldown = std::stoi(argv[2]);
        right::cps_limit = std::stoi(argv[3]);
        right::cooldown = std::stoi(argv[4]);
    }
    else {
        std::cout << "usage: ./CPS-Limiter-Cli <left-limit> <left-cooldown> <right-limit> <right-cooldown>" << std::endl;
        return -1;
    }

    std::cout << "left limit: " << left::cps_limit << std::endl;
    std::cout << "left cooldown: " << left::cooldown << std::endl;
    std::cout << "right limit: " << right::cps_limit << std::endl;
    std::cout << "right cooldown: " << right::cooldown << std::endl;

    std::cout << "pid: " << getpid() << std::endl;

    std::cout << "cli log end ---\n"<< std::endl;

    create_mousehook();

    return 0;
}