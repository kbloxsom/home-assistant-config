#include "esphome.h"

void apply_empty_string_patch() {
    ESP_LOGD("CUSTOM", "Applying emptyString patch...");

    // Get build directory
    std::string build_dir = esphome::App.get_build_path();

    // Find the header file
    std::vector<std::string> possible_paths = {
        build_dir + "/.piolibdeps/ESPAsyncWebServer/src/ESPAsyncWebServer.h",
        build_dir + "/.pio/libdeps/bedroom-fan/ESPAsyncWebServer/src/ESPAsyncWebServer.h"
    };

    std::string header_path;
    for (const auto& path : possible_paths) {
        if (esphome::App.fs_->exists(path.c_str())) {
            header_path = path;
            break;
        }
    }

    if (header_path.empty()) {
        ESP_LOGE("CUSTOM", "Header file not found!");
        return;
    }

    ESP_LOGD("CUSTOM", "Found header at: %s", header_path.c_str());

    // Read file content
    std::string content = esphome::App.fs_->read_file(header_path.c_str());

    // Check if patch is already applied
    if (content.find("static const arduino::String emptyString") != std::string::npos) {
        ESP_LOGD("CUSTOM", "Patch already applied");
        return;
    }

    // Find insertion point
    size_t pos = content.find("#include <vector>");
    if (pos == std::string::npos) {
        ESP_LOGE("CUSTOM", "Could not find insertion point");
        return;
    }

    // Insert the patch string
    size_t insert_pos = content.find('\n', pos) + 1;
    std::string patch_string = "static const arduino::String emptyString = String();\n\n";
    content.insert(insert_pos, patch_string);

    // Write modified content back to file
    esphome::App.fs_->write_file(header_path.c_str(), content);
    ESP_LOGI("CUSTOM", "Successfully applied emptyString patch");
}
