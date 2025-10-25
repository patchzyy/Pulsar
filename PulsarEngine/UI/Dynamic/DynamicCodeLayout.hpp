#ifndef _PUL_DYNAMIC_CODE_LAYOUT_
#define _PUL_DYNAMIC_CODE_LAYOUT_

#include <kamek.hpp>
#include <core/rvl/gx/GX.hpp>
#include <MarioKartWii/UI/Ctrl/UIControl.hpp>

namespace Pulsar {
namespace UI {

// A very lightweight, code-driven canvas control.
// This control is meant to be used without any BRCTR/BRLYT resources.
// For now it exposes a minimal API and draws using GX directly.
class CodeCanvas : public UIControl {
public:
    CodeCanvas() : UIControl() {}
    ~CodeCanvas() override {}

    // Simple filled rectangle (screen space in arbitrary units; the current UI projection is used)
    struct Rect { float x, y, w, h; u32 rgba; };

    void Clear() { rectCount = 0; }
    void AddRect(float x, float y, float w, float h, u32 rgba) {
        if (rectCount >= MaxRects) return;
        rects[rectCount++] = {x, y, w, h, rgba};
    }

    void Draw(u32 curZIdx) override;

private:
    static const int MaxRects = 32;
    Rect rects[MaxRects];
    int rectCount = 0;
};

} // namespace UI
} // namespace Pulsar

#endif
