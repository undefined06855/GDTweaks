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
#include "./utils.hpp"

#include <random>
// setup random stuff
std::random_device dev;
std::mt19937 rng(dev());

using namespace geode::prelude;

void alert(std::string labelName, std::string taskName, cocos2d::CCNode* _this)
{
    std::cout << "couldn't find " + labelName + " for " + taskName << std::endl;

    if (Mod::get()->getSettingValue<bool>("suppress-warnings")) return;

    auto alert = FLAlertLayer::create(
        "Notice (from GDTweaks)",
        "GDTweaks couldn't <cy>"+ taskName +"</c> because of a <cr>mod incompatibility</c>, (<cj>" + labelName + "</c> was not found). You can suppress these dialogs in the GDTweaks settings.",
        "OK"
    );
    alert->m_scene = _this;
    alert->show();
}

bool loadingMainMenu = false;
