#include <UI/CodeLayout/CodeLayout.hpp>
#include <core/nw4r/lyt/DrawInfo.hpp>

namespace Pulsar {
namespace UI {

static const ut::detail::RuntimeTypeInfo typeInfo = {0, nullptr};

CodeLayout::CodeLayout() : rootCodePane(nullptr), paneCount(0), isBuilt(false) {
    for(u32 i = 0; i < 32; ++i) {
        this->panes[i] = nullptr;
    }
}

CodeLayout::~CodeLayout() {
    for(u32 i = 0; i < this->paneCount; ++i) {
        if(this->panes[i]) {
            delete this->panes[i];
            this->panes[i] = nullptr;
        }
    }
}

void CodeLayout::Init() {
    if(!this->isBuilt) {
        this->BuildLayout();
    }
    LayoutUIControl::Init();
}

void CodeLayout::Update() {
    LayoutUIControl::Update();
}

void CodeLayout::Draw(u32 curZIdx) {
    if(this->isBuilt && this->rootPane) {
        nw4r::lyt::DrawInfo drawInfo;
        this->layout.layout.SetupDrawInfo(&drawInfo);
        
        if(this->rootCodePane && this->rootCodePane->GetPane()) {
            nw4r::lyt::Pane* pane = this->rootCodePane->GetPane();
            pane->CalculateMtx(drawInfo);
            pane->Draw(drawInfo);
        }
    }
    LayoutUIControl::Draw(curZIdx);
}

const ut::detail::RuntimeTypeInfo* CodeLayout::GetRuntimeTypeInfo() const {
    return &typeInfo;
}

const char* CodeLayout::GetClassName() const {
    return "pulsar::UI::CodeLayout";
}

void CodeLayout::AddPane(CodePane* pane) {
    if(this->paneCount < 32 && pane) {
        this->panes[this->paneCount++] = pane;
    }
}

void CodeLayout::SetRootPane(CodePane* pane) {
    this->rootCodePane = pane;
    if(pane && pane->GetPane()) {
        this->layout.layout.rootPane = pane->GetPane();
        this->rootPane = pane->GetPane();
    }
}

void CodeLayout::BuildLayout() {
    if(this->rootCodePane && this->rootCodePane->GetPane()) {
        this->layout.layout.rootPane = this->rootCodePane->GetPane();
        this->rootPane = this->rootCodePane->GetPane();
        this->isBuilt = true;
    }
}

void CodeLayout::SetBackgroundColor(const nw4r::ut::Color& color) {
    if(this->rootCodePane) {
        nw4r::lyt::Pane* pane = this->rootCodePane->GetPane();
        if(pane) {
            pane->SetVtxColor(0, color);
            pane->SetVtxColor(1, color);
            pane->SetVtxColor(2, color);
            pane->SetVtxColor(3, color);
        }
    }
}

}
}
