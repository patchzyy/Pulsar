#ifndef _PUL_CODELAYOUT_
#define _PUL_CODELAYOUT_
#include <kamek.hpp>
#include <MarioKartWii/UI/Ctrl/UIControl.hpp>
#include <MarioKartWii/UI/Layout/Layout.hpp>
#include <UI/CodeLayout/CodePane.hpp>
#include <core/nw4r/lyt/DrawInfo.hpp>

namespace Pulsar {
namespace UI {

class CodeLayout : public LayoutUIControl {
public:
    CodeLayout();
    ~CodeLayout() override;
    
    void Init() override;
    void Update() override;
    void Draw(u32 curZIdx) override;
    
    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override;
    const char* GetClassName() const override;
    
    void AddPane(CodePane* pane);
    void SetRootPane(CodePane* pane);
    void BuildLayout();
    
    void SetBackgroundColor(const nw4r::ut::Color& color);
    
protected:
    CodePane* rootCodePane;
    CodePane* panes[32];
    u32 paneCount;
    bool isBuilt;
};

}
}

#endif
