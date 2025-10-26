#ifndef _PUL_CODEPANE_
#define _PUL_CODEPANE_
#include <kamek.hpp>
#include <core/nw4r/lyt/Pane.hpp>
#include <core/nw4r/lyt/TextBox.hpp>
#include <core/nw4r/lyt/Picture.hpp>
#include <core/nw4r/ut/Color.hpp>

namespace Pulsar {
namespace UI {

class CodePane {
public:
    enum PaneType {
        PANE_TYPE_BASIC,
        PANE_TYPE_TEXT,
        PANE_TYPE_PICTURE
    };

    CodePane();
    virtual ~CodePane();
    
    virtual nw4r::lyt::Pane* GetPane() = 0;
    virtual PaneType GetType() const = 0;
    
    void SetPosition(float x, float y, float z = 0.0f);
    void SetSize(float width, float height);
    void SetScale(float x, float y);
    void SetRotation(float x, float y, float z);
    void SetAlpha(u8 alpha);
    void SetVisible(bool visible);
    void SetName(const char* name);
    
    void AddChild(CodePane* child);
};

class CodeBasicPane : public CodePane {
public:
    CodeBasicPane(float width, float height);
    ~CodeBasicPane() override;
    
    nw4r::lyt::Pane* GetPane() override { return pane; }
    PaneType GetType() const override { return PANE_TYPE_BASIC; }
    
private:
    nw4r::lyt::Pane* pane;
};

class CodeTextPane : public CodePane {
public:
    CodeTextPane(float width, float height, const char* text = nullptr);
    ~CodeTextPane() override;
    
    nw4r::lyt::Pane* GetPane() override { return textBox; }
    PaneType GetType() const override { return PANE_TYPE_TEXT; }
    
    void SetText(const wchar_t* text);
    void SetFontSize(float width, float height);
    void SetTextColor(const nw4r::ut::Color& color);
    
private:
    nw4r::lyt::TextBox* textBox;
};

class CodePicturePane : public CodePane {
public:
    CodePicturePane(float width, float height);
    ~CodePicturePane() override;
    
    nw4r::lyt::Pane* GetPane() override { return picture; }
    PaneType GetType() const override { return PANE_TYPE_PICTURE; }
    
    void SetTextureColor(const nw4r::ut::Color& color);
    
private:
    nw4r::lyt::Picture* picture;
};

}
}

#endif
