export module scint.core.application;

import <tracy/Tracy.hpp>;

import std;
import scint.core.types;

export namespace scint {

struct Application {
    [[noreturn]] i32 run() {
        i32 frame_number = 0;
        while (true) {
            ZoneScoped;
            std::println("Frame number: {}", frame_number++);
        }
    }
};

} // namespace scint
