#include <UI/DynamicUI/DynamicSettingsPage.hpp>
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
#include <MarioKartWii/Input/Controller.hpp>
#include <Settings/Settings.hpp>
#include <Settings/UI/SettingsPanel.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {
namespace UI {

DynamicSettingsPage::DynamicSettingsPage() 
    : selectedOption(0) {
    
    externControlCount = 0;
    internControlCount = 0; // controls are managed manually via dynamicLayout
    hasBackButton = false;
    nextPageId = PAGE_NONE;
    activePlayerBitfield = 1;
    movieStartFrame = -1;
    extraControlNumber = 0;
    isLocked = false;
    controlCount = 0;
    nextSection = SECTION_NONE;
    controlSources = 1;
    
    prevPageId = static_cast<PageId>(SettingsPanel::id);
    
    onBackPressHandler.subject = this;
    onBackPressHandler.ptmf = &DynamicSettingsPage::OnBackPress;
    
    onOptionButtonClickHandler.subject = this;
    onOptionButtonClickHandler.ptmf = &DynamicSettingsPage::OnOptionButtonClick;
    
    onButtonSelectHandler.subject = this;
    onButtonSelectHandler.ptmf = &DynamicSettingsPage::OnButtonSelect;
    
    onButtonDeselectHandler.subject = this;
    onButtonDeselectHandler.ptmf = &DynamicSettingsPage::OnButtonDeselect;
    
    // Use a PageManipulatorManager because this page doesn't add interactive ControlManipulators
    this->pageManipulatorManager.Init(1, false);
    this->SetManipulatorManager(pageManipulatorManager);
    this->pageManipulatorManager.SetGlobalHandler(BACK_PRESS, onBackPressHandler, false);
}

DynamicSettingsPage::~DynamicSettingsPage() {
}

void DynamicSettingsPage::OnInit() {
    MenuInteractable::OnInit();
    
    // Build the dynamic UI layout
    BuildDynamicLayout();
    
    // Create option buttons dynamically
    // Since we're building everything from code, these would normally be interactive elements
    // For now, we'll use the manipulator system without requiring brctr files
    
    this->AddControl(this->controlCount++, dynamicLayout, 0);
}

void DynamicSettingsPage::BuildDynamicLayout() {
    // Create root pane for our dynamic layout
    nw4r::lyt::Pane* root = DynamicPaneBuilder::CreatePane("root", 0.0f, 0.0f, 640.0f, 480.0f);
    dynamicLayout.InitWithRootPane(root);
    
    // Create a UI builder to help us construct the layout
    DynamicUIBuilder builder(root);
    
    // Title bar at the top
    nw4r::lyt::Pane* titleBar = builder.AddTitleBar("title_bar", -300.0f, -200.0f, 600.0f, 50.0f);
    nw4r::lyt::Pane* titleText = builder.AddText("title_text", -250.0f, -185.0f, 500.0f, 30.0f, L"Dynamic Settings Demo");
    
    // Main content panel
    nw4r::lyt::Pane* mainPanel = builder.AddPanel("main_panel", -280.0f, -140.0f, 560.0f, 320.0f, 0x222222FF);
    
    // Add some informational text
    builder.AddText("info_text", -260.0f, -120.0f, 520.0f, 20.0f, L"This page is created entirely from code!");
    builder.AddText("info_text2", -260.0f, -90.0f, 520.0f, 20.0f, L"No brlyt or brctr files were used.");
    builder.AddText("info_text3", -260.0f, -60.0f, 520.0f, 20.0f, L"Use the arrows in the Settings Panel to reach this demo.");
    
    // Add a separator
    builder.AddSeparator("separator1", -260.0f, -30.0f, 520.0f, 2.0f);
    
    // Add list items for different settings options
    builder.AddListItem("option", -260.0f, -10.0f, 520.0f, 40.0f, L"Visual Settings", 0x444488FF);
    builder.AddListItem("option", -260.0f, 40.0f, 520.0f, 40.0f, L"Audio Settings", 0x444488FF);
    builder.AddListItem("option", -260.0f, 90.0f, 520.0f, 40.0f, L"Gameplay Settings", 0x444488FF);
    builder.AddListItem("option", -260.0f, 140.0f, 520.0f, 40.0f, L"Network Settings", 0x444488FF);
    
    // Add some decorative elements
    nw4r::lyt::Pane* leftBorder = builder.AddPanel("left_border", -320.0f, -200.0f, 5.0f, 400.0f, 0x4488FFFF);
    nw4r::lyt::Pane* rightBorder = builder.AddPanel("right_border", 315.0f, -200.0f, 5.0f, 400.0f, 0x4488FFFF);
    
    // Bottom info bar
    nw4r::lyt::Pane* bottomBar = builder.AddPanel("bottom_bar", -300.0f, 190.0f, 600.0f, 40.0f, 0x333333FF);
    builder.AddText("bottom_text", -250.0f, 200.0f, 500.0f, 20.0f, L"Press Back to return");
}

void DynamicSettingsPage::OnActivate() {
    MenuInteractable::OnActivate();
    
    // In a real implementation, you'd select the first interactable element here
    // For this demo, we're just showing that the layout can be built dynamically
}

const ut::detail::RuntimeTypeInfo* DynamicSettingsPage::GetRuntimeTypeInfo() const {
    return Pages::Menu::GetRuntimeTypeInfo();
}

int DynamicSettingsPage::GetActivePlayerBitfield() const {
    return this->activePlayerBitfield;
}

int DynamicSettingsPage::GetPlayerBitfield() const {
    return this->playerBitfield;
}

ManipulatorManager& DynamicSettingsPage::GetManipulatorManager() {
    // Return the active manager for this page (page-level handlers only)
    return this->pageManipulatorManager;
}

void DynamicSettingsPage::BeforeControlUpdate() {
    MenuInteractable::BeforeControlUpdate();
}

UIControl* DynamicSettingsPage::CreateControl(u32 id) {
    // This dynamic page doesn't create standard controls via factory; everything is built in code.
    return nullptr;
}

void DynamicSettingsPage::OnBackPress(u32 hudSlotId) {
    // Return to previous page
    this->nextPageId = this->prevPageId;
    this->EndStateAnimated(0, 0.0f);
}

void DynamicSettingsPage::OnOptionButtonClick(PushButton& button, u32 hudSlotId) {
    // Handle option button clicks
    selectedOption = button.buttonId;
    
    // In a real implementation, you would navigate to a sub-page or change settings here
}

void DynamicSettingsPage::OnButtonSelect(PushButton& button, u32 hudSlotId) {
    // Button selection handler - can be used to update UI when hovering over buttons
}

void DynamicSettingsPage::OnButtonDeselect(PushButton& button, u32 hudSlotId) {
    // Button deselection handler
}

} // namespace UI
} // namespace Pulsar
