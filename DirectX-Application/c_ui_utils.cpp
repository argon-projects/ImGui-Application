#include "c_ui_utils.hpp"

ImFont* c_ui_utils::load_font(ImFontAtlas& atlas, const char* name, float size, const ImVec2& displayOffset)
{
    char* windir = nullptr;
    if (_dupenv_s(&windir, nullptr, "WINDIR") || windir == nullptr)
        return nullptr;

    static const ImWchar ranges[] =
    {
        0x1, 0x1FFFF, //All Ranges
        0
    };

    ImFontConfig config;
    config.OversampleH = 4;
    config.OversampleV = 4;
    config.PixelSnapH = false;

    auto path = std::string(windir) + "\\Fonts\\" + name;
    auto font = atlas.AddFontFromFileTTF(path.c_str(), size, &config, ranges);


    free(windir);

    return font;
}
