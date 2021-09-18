#include "c_ui_utils.hpp"

ImFont* c_ui_utils::load_font(ImFontAtlas& atlas, const char* name, float size, const ImVec2& displayOffset)
{
    char* windir = nullptr;
    if (_dupenv_s(&windir, nullptr, "WINDIR") || windir == nullptr)
        return nullptr;

    static const ImWchar ranges[] =
    {
        //0x0020, 0x00FF, // LATEINUS SCHWANZIUS
        //0x0104, 0x017C, // Die leute die alles klauen Char ranges
        //0x400, 0x4FF,  // Vodka Char ranges
        //0,
        0x1, 0x1FFFF, 
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
