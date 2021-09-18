#pragma once
#include "pch.hpp"
class c_ui_utils
{
public:
    static ImFont* load_font(ImFontAtlas& atlas, const char* name, float size, const ImVec2& displayOffset = ImVec2(0, 0));
};

