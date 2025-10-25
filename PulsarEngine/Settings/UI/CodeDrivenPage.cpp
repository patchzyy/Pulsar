#include <Settings/UI/CodeDrivenPage.hpp>
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
#include <UI/UI.hpp>
#include <core/nw4r/ut/Color.hpp>

namespace Pulsar {
namespace UI {

CodeDrivenPage::CodeDrivenPage() : 
    codeLayout(nullptr),
    titlePane(nullptr), 
    descPane(nullptr),
    item1Pane(nullptr),
    item2Pane(nullptr),
    item3Pane(nullptr) {
    
    this->externControlCount = 1;
    this->internControlCount = 0;
    this->hasBackButton = false;
    this->nextPageId = PAGE_OPTIONS;
    this->prevPageId = PAGE_OPTIONS;
    this->activePlayerBitfield = 1;
    this->movieStartFrame = -1;
    this->extraControlNumber = 0;
    this->isLocked = false;
    this->controlCount = 0;
    this->nextSection = SECTION_NONE;
    this->controlSources = 2;
    
    onButtonSelectHandler.subject = this;
    onButtonSelectHandler.ptmf = &CodeDrivenPage::OnExternalButtonSelect;
    onButtonDeselectHandler.subject = this;
    onButtonDeselectHandler.ptmf = &Pages::VSSettings::OnButtonDeselect;
    onBackPressHandler.subject = this;
    onBackPressHandler.ptmf = &CodeDrivenPage::OnBackPress;
    onStartPressHandler.subject = this;
    onStartPressHandler.ptmf = &MenuInteractable::HandleStartPress;
    
    onOkButtonClickHandler.subject = this;
    onOkButtonClickHandler.ptmf = &CodeDrivenPage::OnOkButtonClick;
    
    this->controlsManipulatorManager.Init(1, false);
    this->SetManipulatorManager(controlsManipulatorManager);
    this->controlsManipulatorManager.SetGlobalHandler(START_PRESS, onStartPressHandler, false, false);
    this->controlsManipulatorManager.SetGlobalHandler(BACK_PRESS, onBackPressHandler, false, false);
}

CodeDrivenPage::~CodeDrivenPage() {
    if(this->codeLayout) {
        delete this->codeLayout;
    }
}

void CodeDrivenPage::OnInit() {
    this->BuildCodeLayout();
    MenuInteractable::OnInit();
    this->SetTransitionSound(0, 0);
}

void CodeDrivenPage::OnActivate() {
    this->titleBmg = BMG_SETTINGS_TITLE;
    this->externControls[0]->SelectInitial(0);
    this->bottomText->SetMessage(BMG_SETTINGS_BOTTOM);
    MenuInteractable::OnActivate();
}

void CodeDrivenPage::BeforeEntranceAnimations() {
    if(this->codeLayout) {
        this->codeLayout->animator.GetAnimationGroupById(0)->PlayAnimationAtFrame(0, 0.0f);
    }
}

void CodeDrivenPage::AfterEntranceAnimations() {}

void CodeDrivenPage::BeforeControlUpdate() {
    if(this->codeLayout) {
        this->codeLayout->Update();
    }
}

const ut::detail::RuntimeTypeInfo* CodeDrivenPage::GetRuntimeTypeInfo() const {
    return Pages::VSSettings::typeInfo;
}

void CodeDrivenPage::OnExternalButtonSelect(PushButton& button, u32 r5) {
    this->bottomText->SetMessage(BMG_SETTINGS_BOTTOM);
}

int CodeDrivenPage::GetActivePlayerBitfield() const {
    return this->activePlayerBitfield;
}

int CodeDrivenPage::GetPlayerBitfield() const {
    return this->playerBitfield;
}

ManipulatorManager& CodeDrivenPage::GetManipulatorManager() {
    return this->controlsManipulatorManager;
}

UIControl* CodeDrivenPage::CreateExternalControl(u32 id) {
    if(id == 0) {
        PushButton* button = new PushButton();
        this->AddControl(this->controlCount++, *button, 0);
        button->Load(UI::buttonFolder, "OKButton", "OK", this->activePlayerBitfield, 0, false);
        return button;
    }
    return nullptr;
}

UIControl* CodeDrivenPage::CreateControl(u32 id) {
    return nullptr;
}

void CodeDrivenPage::SetButtonHandlers(PushButton& button) {
    button.SetOnClickHandler(this->onOkButtonClickHandler, 0);
    button.SetOnSelectHandler(this->onButtonSelectHandler);
    button.SetOnDeselectHandler(this->onButtonDeselectHandler);
}

void CodeDrivenPage::OnBackPress(u32 hudSlotId) {
    PushButton& okButton = *this->externControls[0];
    okButton.SelectFocus();
    this->LoadPrevPage(okButton);
}

void CodeDrivenPage::OnOkButtonClick(PushButton& button, u32 hudSlotId) {
    this->LoadPrevPage(button);
}

void CodeDrivenPage::BuildCodeLayout() {
    this->codeLayout = new CodeLayout();
    
    CodeBasicPane* rootPane = new CodeBasicPane(608.0f, 456.0f);
    rootPane->SetPosition(0.0f, 0.0f, 0.0f);
    rootPane->SetName("root");
    this->codeLayout->AddPane(rootPane);
    this->codeLayout->SetRootPane(rootPane);
    
    this->titlePane = new CodeBasicPane(500.0f, 40.0f);
    this->titlePane->SetPosition(0.0f, -150.0f);
    this->titlePane->SetName("title");
    nw4r::ut::Color red = {255, 100, 100, 255};
    this->titlePane->GetPane()->SetVtxColor(0, red);
    this->titlePane->GetPane()->SetVtxColor(1, red);
    this->titlePane->GetPane()->SetVtxColor(2, red);
    this->titlePane->GetPane()->SetVtxColor(3, red);
    this->codeLayout->AddPane(this->titlePane);
    rootPane->AddChild(this->titlePane);
    
    this->descPane = new CodeBasicPane(500.0f, 30.0f);
    this->descPane->SetPosition(0.0f, -80.0f);
    this->descPane->SetName("desc");
    nw4r::ut::Color green = {100, 255, 100, 255};
    this->descPane->GetPane()->SetVtxColor(0, green);
    this->descPane->GetPane()->SetVtxColor(1, green);
    this->descPane->GetPane()->SetVtxColor(2, green);
    this->descPane->GetPane()->SetVtxColor(3, green);
    this->codeLayout->AddPane(this->descPane);
    rootPane->AddChild(this->descPane);
    
    this->item1Pane = new CodeBasicPane(450.0f, 25.0f);
    this->item1Pane->SetPosition(0.0f, -10.0f);
    this->item1Pane->SetName("item1");
    nw4r::ut::Color cyan = {100, 200, 255, 255};
    this->item1Pane->GetPane()->SetVtxColor(0, cyan);
    this->item1Pane->GetPane()->SetVtxColor(1, cyan);
    this->item1Pane->GetPane()->SetVtxColor(2, cyan);
    this->item1Pane->GetPane()->SetVtxColor(3, cyan);
    this->codeLayout->AddPane(this->item1Pane);
    rootPane->AddChild(this->item1Pane);
    
    this->item2Pane = new CodeBasicPane(450.0f, 25.0f);
    this->item2Pane->SetPosition(0.0f, 40.0f);
    this->item2Pane->SetName("item2");
    nw4r::ut::Color blue = {100, 100, 255, 255};
    this->item2Pane->GetPane()->SetVtxColor(0, blue);
    this->item2Pane->GetPane()->SetVtxColor(1, blue);
    this->item2Pane->GetPane()->SetVtxColor(2, blue);
    this->item2Pane->GetPane()->SetVtxColor(3, blue);
    this->codeLayout->AddPane(this->item2Pane);
    rootPane->AddChild(this->item2Pane);
    
    this->item3Pane = new CodeBasicPane(450.0f, 25.0f);
    this->item3Pane->SetPosition(0.0f, 90.0f);
    this->item3Pane->SetName("item3");
    nw4r::ut::Color yellow = {255, 255, 100, 255};
    this->item3Pane->GetPane()->SetVtxColor(0, yellow);
    this->item3Pane->GetPane()->SetVtxColor(1, yellow);
    this->item3Pane->GetPane()->SetVtxColor(2, yellow);
    this->item3Pane->GetPane()->SetVtxColor(3, yellow);
    this->codeLayout->AddPane(this->item3Pane);
    rootPane->AddChild(this->item3Pane);
    
    this->codeLayout->BuildLayout();
    
    this->AddControl(this->controlCount++, *this->codeLayout, 0);
}

}
}
