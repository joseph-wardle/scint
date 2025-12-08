export module scint.core.application;

import <tracy/Tracy.hpp>;

import std;

import scint.core.types;

export namespace scint {

struct Application {
    [[noreturn]] static i32 run() {
        ZoneScoped;
        std::println("Hello, Scint Application!");
        return 0;
    }
};

} // namespace scint
