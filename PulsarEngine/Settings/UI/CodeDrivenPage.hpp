#ifndef _PUL_CODEDRIVENPAGE_
#define _PUL_CODEDRIVENPAGE_
#include <kamek.hpp>
#include <MarioKartWii/UI/Page/Menu/Menu.hpp>
#include <MarioKartWii/UI/Ctrl/PushButton.hpp>
#include <UI/UI.hpp>
#include <UI/CodeLayout/CodeLayout.hpp>
#include <UI/CodeLayout/CodePane.hpp>

namespace Pulsar {
namespace UI {

class CodeDrivenPage : public Pages::MenuInteractable {
public:
    static const PulPageId id = PULPAGE_CODEDRIVEN;
    
    CodeDrivenPage();
    ~CodeDrivenPage() override;
    
    void OnInit() override;
    void OnActivate() override;
    void BeforeEntranceAnimations() override;
    void AfterEntranceAnimations() override;
    void BeforeControlUpdate() override;
    
    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override;
    void OnExternalButtonSelect(PushButton& button, u32 r5) override;
    int GetActivePlayerBitfield() const override;
    int GetPlayerBitfield() const override;
    ManipulatorManager& GetManipulatorManager() override;
    UIControl* CreateExternalControl(u32 id) override;
    UIControl* CreateControl(u32 id) override;
    void SetButtonHandlers(PushButton& pushButton) override;
    
private:
    void OnBackPress(u32 hudSlotId);
    void OnOkButtonClick(PushButton& button, u32 hudSlotId);
    void BuildCodeLayout();
    
    CodeLayout* codeLayout;
    CodePane* titlePane;
    CodePane* descPane;
    CodePane* item1Pane;
    CodePane* item2Pane;
    CodePane* item3Pane;
    
    PtmfHolder_2A<MenuInteractable, void, PushButton&, u32> onOkButtonClickHandler;
};

}
}

#endif
