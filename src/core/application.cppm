export module scint.core.application;

import std;
import <tracy/Tracy.hpp>;
import vulkan_hpp;

import scint.core.types;

export namespace scint {

inline void check_vulkan_import() {
    using std::println;

    println("[scint] Vulkan-Hpp import sanity check");

    // Basic sanity: use a couple of Vulkan-Hpp types.
    [[maybe_unused]] vk::Instance dummy_instance{};
    const auto success = vk::Result::eSuccess;

    println("  vk::Result::eSuccess = {} ({})",
            static_cast<int>(success),
            vk::to_string(success));

    // Query the runtime Vulkan API version via Hpp.
    std::uint32_t api_version_raw = 0;
    vk::Result version_result = vk::enumerateInstanceVersion(&api_version_raw);

    println("  vk::enumerateInstanceVersion(...) = {} ({})",
            static_cast<int>(version_result),
            vk::to_string(version_result));

    auto decode_version = [](std::uint32_t v) {
        const std::uint32_t major = v >> 22;
        const std::uint32_t minor = (v >> 12) & 0x3FFu;
        const std::uint32_t patch = v & 0xFFFu;
        return std::tuple{major, minor, patch};
    };

    if (version_result == vk::Result::eSuccess) {
        auto [maj, min, pat] = decode_version(api_version_raw);
        println("  Runtime Vulkan API version       = {}.{}.{} (0x{:08X})",
                maj, min, pat, api_version_raw);
    } else {
        println("  Failed to query Vulkan API version.");
    }

    // 3. Enumerate instance layers via Hpp.
    std::uint32_t layer_count = 0;
    vk::Result layer_result = vk::enumerateInstanceLayerProperties(&layer_count, nullptr);

    println("  vk::enumerateInstanceLayerProperties(nullptr) = {} ({})",
            static_cast<int>(layer_result),
            vk::to_string(layer_result));

    std::vector<vk::LayerProperties> layers;

    if (layer_result == vk::Result::eSuccess && layer_count > 0) {
        layers.resize(layer_count);
        layer_result = vk::enumerateInstanceLayerProperties(&layer_count, layers.data());
    }

    if (layer_result == vk::Result::eSuccess) {
        println("  Available instance layers        = {}", layer_count);

        // Print up to a few layers to keep output sane.
        const std::size_t max_print = std::min<std::size_t>(layers.size(), 8);
        for (std::size_t i = 0; i < max_print; ++i) {
            const auto& layer = layers[i];
            auto [maj, min, pat] = decode_version(layer.specVersion);

            println("    [{}] {:30} (spec {}.{}.{}), impl version {}",
                    i,
                    std::string(layer.layerName),    // null-terminated char[]
                    maj, min, pat,
                    layer.implementationVersion);
        }

        if (layers.size() > max_print) {
            println("    ... and {} more", layers.size() - max_print);
        }
    } else {
        println("  Failed to enumerate instance layers.");
    }

    println("[scint] Vulkan-Hpp check complete.");
}

struct Application {
    [[noreturn]] i32 run() {
        ZoneScoped;
        check_vulkan_import();  // run once at startup
    }
};

} // namespace scint
