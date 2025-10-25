#include <UI/Dynamic/DynamicCodeLayout.hpp>

namespace Pulsar {
namespace UI {

// Extremely small immediate mode rectangle draw. This relies on the UI pipeline's current projection.
void CodeCanvas::Draw(u32 curZIdx) {
    // Basic GX state for flat colored quads
    using namespace GX;

    // Configure vertex format (position + color)
    ClearVtxDesc();
    SetVtxDesc(GX_VA_POS, GX_DIRECT);
    SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

    SetNumTexGens(0);
    SetNumChans(1);
    SetChanCtrl(GX_COLOR0A0, false, GX_SRC_VTX, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

    SetNumTevStages(1);
    SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

    // Draw each rect as a quad
    for (int i = 0; i < rectCount; ++i) {
        const Rect& r = rects[i];

        // Immediate mode quad (XY only, Z assumed 0)
        Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(r.x,       r.y,        0.0f);
        GX_Color1u32(r.rgba);
        GX_Position3f32(r.x + r.w, r.y,        0.0f);
        GX_Color1u32(r.rgba);
        GX_Position3f32(r.x + r.w, r.y + r.h,  0.0f);
        GX_Color1u32(r.rgba);
        GX_Position3f32(r.x,       r.y + r.h,  0.0f);
        GX_Color1u32(r.rgba);
        GXEnd();
    }
}

} // namespace UI
} // namespace Pulsar
