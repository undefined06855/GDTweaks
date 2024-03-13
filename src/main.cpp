// warning: shitty code incoming (nvm it's probably not too bad)
// good luck reading it, i should probably split it up into multiple files
// i got u - coop
#include <Geode/Geode.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/GJGroundLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/loader/SettingNode.hpp>
#include "./SectionSpacer.hpp"
#include "./utils.hpp"
#include <random>
// hehe
#include <ctime>
using namespace geode::prelude;

// -- stuff needed by other files  -----------------------------------------------------------------------

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

// getting ready for april fools!
std::time_t timeObject = std::time(0);
struct tm* timeStruct = localtime(&timeObject);

bool isAprilFools = ((timeStruct->tm_mon == 3) && (timeStruct->tm_mday == 1));
// -----------------------------------------------------------------------------------------------------

SettingNode* SectionSpacerSetting::createNode(float width) { return SectionSpacerNode::create(this, width); }
$on_mod(Loaded) {
    Mod::get()->addCustomSetting<SectionSpacerSetting>("header1", "");
    Mod::get()->addCustomSetting<SectionSpacerSetting>("header2", "");
    Mod::get()->addCustomSetting<SectionSpacerSetting>("header3", "");
    Mod::get()->addCustomSetting<SectionSpacerSetting>("header4", "");

    log::info("Today is {}th of {}", timeStruct->tm_mon, timeStruct->tm_mday);
    log::info("April Fools? {}", (isAprilFools ? "YEAH!" : "no :("));
}
