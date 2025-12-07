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
            if (frame_number % 60 == 0) {
                std::println("Running frame {}", frame_number);
            }
            frame_number++;
        }
    }
};

} // namespace scint
