#include "utils.hpp"

namespace gdtutils {

// setup random stuff for GJGround and MenuLayer
std::random_device dev;
std::mt19937 rng(dev());

// this is needed basically everywhere
void alert(std::string labelName, std::string taskName, cocos2d::CCNode* _this)
{
    log::info("couldn't find {} for {}", labelName, taskName);

    if (Mod::get()->getSettingValue<bool>("suppress-warnings")) return;

    auto alert = FLAlertLayer::create(
        "Notice (from GDTweaks)",
        "GDTweaks couldn't <cy>"+ taskName +"</c> because of a <cr>mod incompatibility</c>, (<cj>" + labelName + "</c> was not found). You can suppress these dialogs in the GDTweaks settings.",
        "OK"
    );
    alert->m_scene = _this;
    alert->show();
}

// this is here because it gets set in MenuLayer and is needed in GJGround
bool loadingMainMenu = false;

}
