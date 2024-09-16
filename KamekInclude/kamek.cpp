#include <kamek.hpp>


//SectionLoadHook* SectionLoadHook::sHooks = nullptr;
DoFuncsHook* RaceLoadHook::raceLoadHooks = nullptr;
DoFuncsHook* PageLoadHook::pageLoadHooks = nullptr;
DoFuncsHook* RaceFrameHook::raceFrameHooks = nullptr;
DoFuncsHook* SectionLoadHook::sectionLoadHooks = nullptr;
RacePostLoadHook *RacePostLoadHook::sHooks = nullptr;
DoFuncsHook::DoFuncsHook(Func& f, DoFuncsHook** prev) : func(f) {
    next = *prev;
    *prev = this;
}

void DoFuncsHook::exec(DoFuncsHook* first) {
    for(DoFuncsHook* p = first; p; p = p->next) {
        p->func();
    }
}

nw4r::ut::List BootHook::list ={ nullptr, nullptr, 0, offsetof(BootHook, link) };

//kmBranch(0x80001500, BootHook2::exec);
kmBranch(0x800074d4, BootHook::exec);
kmBranch(0x80554728, RaceLoadHook::exec);
kmBranch(0x8083822C, RaceLoadHook::exec);
kmBranch(0x80601C5C, PageLoadHook::exec);
kmBranch(0x8053369c, RaceFrameHook::exec); //RaceInfo::Update()
kmBranch(0x80855D90, RaceFrameHook::exec); //RaceInfo::Update()

kmBranch(0x8063507c, SectionLoadHook::exec);