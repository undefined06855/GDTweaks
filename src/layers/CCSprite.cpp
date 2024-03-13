#include <Geode/Geode.hpp>
#include <Geode/modify/CCSprite.hpp>
#include "../utils.hpp"

using namespace geode::prelude;

std::unordered_set<std::string> cornerSpriteNames = {
    "GJ_sideArt_001.png",
    "dailyLevelCorner_001.png",
    "gauntletCorner_001.png",
    "rewardCorner_001.png",
    "treasureRoomSpiderweb_001.png"
};

// I may or may not have ~~stolen~~ kindly borrowed some of this from NoControllerGlyphs
// it's pretty clever tbh
// "remove-corners" + "remove-level-select-top"
class $modify(CCSprite) {
    static void onModify(auto & self) { self.setHookPriority("CCSprite::createWithSpriteFrameName", 1000); }

    static CCSprite* createWithSpriteFrameName(char const* frameName) {
        CCSprite* sprite = CCSprite::createWithSpriteFrameName(frameName);

        if (cornerSpriteNames.find(frameName) != cornerSpriteNames.end() && Mod::get()->getSettingValue<bool>("remove-corners"))
            sprite->setVisible(false); // remove corner

        if (strcmp(frameName, "GJ_topBar_001.png") == 0 && Mod::get()->getSettingValue<bool>("remove-level-select-top"))
            sprite->setVisible(false); // remove level select topper thingymabob

        return sprite;
    }
};
