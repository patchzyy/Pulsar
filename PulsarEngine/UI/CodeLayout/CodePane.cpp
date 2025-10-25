#include <UI/CodeLayout/CodePane.hpp>
#include <core/nw4r/lyt/Pane.hpp>
#include <core/nw4r/lyt/resources.hpp>
#include <cstring>

namespace Pulsar {
namespace UI {

CodePane::CodePane() {}
CodePane::~CodePane() {}

void CodePane::SetPosition(float x, float y, float z) {
    nw4r::lyt::Pane* pane = this->GetPane();
    if(pane) {
        pane->trans.x = x;
        pane->trans.y = y;
        pane->trans.z = z;
    }
}

void CodePane::SetSize(float width, float height) {
    nw4r::lyt::Pane* pane = this->GetPane();
    if(pane) {
        pane->size.x = width;
        pane->size.y = height;
    }
}

void CodePane::SetScale(float x, float y) {
    nw4r::lyt::Pane* pane = this->GetPane();
    if(pane) {
        pane->scale.x = x;
        pane->scale.y = y;
    }
}

void CodePane::SetRotation(float x, float y, float z) {
    nw4r::lyt::Pane* pane = this->GetPane();
    if(pane) {
        pane->rotate.x = x;
        pane->rotate.y = y;
        pane->rotate.z = z;
    }
}

void CodePane::SetAlpha(u8 alpha) {
    nw4r::lyt::Pane* pane = this->GetPane();
    if(pane) {
        pane->alpha = alpha;
    }
}

void CodePane::SetVisible(bool visible) {
    nw4r::lyt::Pane* pane = this->GetPane();
    if(pane) {
        if(visible) {
            pane->flag |= 0x1;
        } else {
            pane->flag &= ~0x1;
        }
    }
}

void CodePane::SetName(const char* name) {
    nw4r::lyt::Pane* pane = this->GetPane();
    if(pane && name) {
        strncpy(pane->name, name, 0x10);
        pane->name[0x10] = '\0';
    }
}

void CodePane::AddChild(CodePane* child) {
    nw4r::lyt::Pane* pane = this->GetPane();
    nw4r::lyt::Pane* childPane = child ? child->GetPane() : nullptr;
    if(pane && childPane) {
        pane->AppendChild(childPane);
    }
}

CodeBasicPane::CodeBasicPane(float width, float height) {
    nw4r::lyt::res::Pane paneRes;
    memset(&paneRes, 0, sizeof(paneRes));
    strncpy(paneRes.header.magic, "pan1", 4);
    paneRes.header.blockSize = sizeof(nw4r::lyt::res::Pane);
    paneRes.basePosition = 1;
    paneRes.originType = 4;
    paneRes.alpha = 255;
    strncpy(paneRes.name, "code", 4);
    paneRes.translation.x = 0.0f;
    paneRes.translation.y = 0.0f;
    paneRes.translation.z = 0.0f;
    paneRes.rotation.x = 0.0f;
    paneRes.rotation.y = 0.0f;
    paneRes.rotation.z = 0.0f;
    paneRes.scale.x = 1.0f;
    paneRes.scale.y = 1.0f;
    paneRes.width = width;
    paneRes.height = height;
    
    this->pane = new nw4r::lyt::Pane(&paneRes);
    this->pane->isUserAllocated = true;
}

CodeBasicPane::~CodeBasicPane() {
    if(this->pane && this->pane->isUserAllocated) {
        delete this->pane;
    }
}

CodeTextPane::CodeTextPane(float width, float height, const char* text) {
    nw4r::lyt::res::Pane paneRes;
    memset(&paneRes, 0, sizeof(paneRes));
    strncpy(paneRes.name, "text", 4);
    paneRes.width = width;
    paneRes.height = height;
    paneRes.alpha = 255;
    
    this->textBox = reinterpret_cast<nw4r::lyt::TextBox*>(new nw4r::lyt::Pane(&paneRes));
    this->textBox->isUserAllocated = true;
}

CodeTextPane::~CodeTextPane() {
    if(this->textBox && this->textBox->isUserAllocated) {
        delete this->textBox;
    }
}

void CodeTextPane::SetText(const wchar_t* text) {
}

void CodeTextPane::SetFontSize(float width, float height) {
}

void CodeTextPane::SetTextColor(const nw4r::ut::Color& color) {
    if(this->textBox) {
        this->textBox->SetVtxColor(0, color);
        this->textBox->SetVtxColor(1, color);
        this->textBox->SetVtxColor(2, color);
        this->textBox->SetVtxColor(3, color);
    }
}

CodePicturePane::CodePicturePane(float width, float height) {
    nw4r::lyt::res::Pane paneRes;
    memset(&paneRes, 0, sizeof(paneRes));
    strncpy(paneRes.name, "pic", 4);
    paneRes.width = width;
    paneRes.height = height;
    paneRes.alpha = 255;
    
    this->picture = reinterpret_cast<nw4r::lyt::Picture*>(new nw4r::lyt::Pane(&paneRes));
    this->picture->isUserAllocated = true;
}

CodePicturePane::~CodePicturePane() {
    if(this->picture && this->picture->isUserAllocated) {
        delete this->picture;
    }
}

void CodePicturePane::SetTextureColor(const nw4r::ut::Color& color) {
    if(this->picture) {
        this->picture->SetVtxColor(0, color);
        this->picture->SetVtxColor(1, color);
        this->picture->SetVtxColor(2, color);
        this->picture->SetVtxColor(3, color);
    }
}

}
}
