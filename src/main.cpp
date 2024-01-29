// warning: shitty code incoming
// good luck reading it, i should probably split it up into multiple files
#include <Geode/Geode.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

#include <random>
// setup random stuff
std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> distBG(1, 59);
std::uniform_int_distribution<std::mt19937::result_type> distGround(1, 22);
std::uniform_int_distribution<std::mt19937::result_type> distGroundLine(1, 3);

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
    static void onModify(auto & self)
    {
        self.setHookPriority("CCSprite::createWithSpriteFrameName", 1000);
    }

    static CCSprite* createWithSpriteFrameName(char const* frameName) {
        CCSprite* sprite = CCSprite::createWithSpriteFrameName(frameName);

        if (cornerSpriteNames.find(frameName) != cornerSpriteNames.end() && Mod::get()->getSettingValue<bool>("remove-corners"))
            sprite->setVisible(false); // remove corner

        if (strcmp(frameName, "GJ_topBar_001.png") == 0 && Mod::get()->getSettingValue<bool>("remove-level-select-top"))
            sprite->setVisible(false); // remove level select topper thingymabob

        return sprite;
    }
};

// "title-text" + "fix-main-menu-settings-gamepad" + "title-buttons" + "replace-more-games-w-texture" + "randomise-main-menu-bg"
class $modify(MenuLayer)
{
    static void onModify(auto & self)
    {
        // run after texture selector
        self.setHookPriority("MenuLayer::init", -100);
    }

    bool init()
    {
        if (!MenuLayer::init()) return false;

        if (Mod::get()->getSettingValue<bool>("title-text"))
        {
            // make title funny
            auto t = this->getChildByIDRecursive("main-title");

            t->setScale(1.025);
            t->setSkewX(52);
            t->setSkewY(10);
            t->setZOrder(-1);
            t->setPositionX(285);
            t->setPositionY(258);

            // put bg behind title
            this->getChildByIDRecursive("main-menu-bg")->setZOrder(-2);
        }

        if (Mod::get()->getSettingValue<bool>("fix-main-menu-settings-gamepad"))
        {
            //if (auto settingsButton = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("settings-button")))
            //{
            // this is added by geode, so should exist in theory, but might as well check for nullptr anyway
            if (auto controllerIcon = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("settings-gamepad-icon")))
            {
                // wowee magic numbers
                // I hope this isnt broken on widescreen
                controllerIcon->setPositionX(255.0f);
            }
            //}
        }

        if (Mod::get()->getSettingValue<bool>("title-buttons"))
        {
            // remove newgrounds button + move geode button to where it used to be
            auto ng = this->getChildByIDRecursive("newgrounds-button");
            ng->setVisible(false);
            this->getChildByIDRecursive("geode.loader/geode-button")->setPositionX(ng->getPositionX());
            auto btm = this->getChildByIDRecursive("bottom-menu");
            btm->setPositionX(btm->getPositionX() + 29); // shift over by 29
        }

        if (Mod::get()->getSettingValue<bool>("replace-more-games-w-texture"))
        {
            // replace more games button with texture selector
            // alright so I know that this is probably definitely bad practice to use objectAtIndex but
            // it's the only way to get this afaik and to be honest what other mods actually use the right side menu?
            // ok so basically there is a mod that uses the right side menu
            // and hopefully that's going to be the only one because I am NOT remaking these magic numbers again
            auto rsm = this->getChildByIDRecursive("right-side-menu");
            auto obj = rsm->getChildren()->objectAtIndex(rsm->getChildrenCount() == 2 ? 1 : 2); // 1 is normal, 2 is modified
            auto moregames = this->getChildByIDRecursive("more-games-menu");

            if (auto texSelector = typeinfo_cast<CCMenuItemSpriteExtra*>(obj))
            {
                moregames->setVisible(false);

                texSelector->setPositionX(26.25);
                // wow more magic numbers
                if (rsm->getChildrenCount() == 2)
                {
                    // normal
                    texSelector->setPositionY(-49.5);
                }
                else
                {
                    // modified
                    texSelector->setPositionY(-58.5);
                    rsm->setPositionY(189.25);
                }
                
            }
        }
        
        if (Mod::get()->getSettingValue<bool>("randomise-main-menu-bg"))
        {
            if (auto mainMenuBG = typeinfo_cast<MenuGameLayer*>(this->getChildByIDRecursive("main-menu-bg")))
            {
                if (auto tex = typeinfo_cast<CCTextureProtocol*>(mainMenuBG->getChildren()->objectAtIndex(0))) {
                    std::string rand = std::to_string(distBG(rng));
                    if (rand.length() == 1)
                        rand = "0" + rand;
                    std::string string = "game_bg_" + rand + "_001-uhd.png";

                    // literally had to follow a 10 YEAR OLD STACKOVERFLOW POST for this to work
                    // https://stackoverflow.com/a/21699549
                    auto newTexture = CCSprite::create(string.c_str());
                    ccTexParams tp = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
                    newTexture->getTexture()->setTexParameters(&tp);
                    tex->setTexture(newTexture->getTexture());
                }

                if (auto groundLayer = typeinfo_cast<GJGroundLayer*>(mainMenuBG->getChildren()->objectAtIndex(1)))
                {
                    int rand = distGround(rng);
                    int randL = distGroundLine(rng);

                    groundLayer = GJGroundLayer::create(rand, randL);
                }
            }
        }

        return true;
    }

    void onPlay(CCObject * sender)
    {
        MenuLayer::onPlay(sender);

        if (!Mod::get()->getSettingValue<bool>("fire-in-the-hole")) return;

        FMODAudioEngine::sharedEngine()->playEffect("fire.ogg"_spr);
    }
};

// "map-packs" + "move-betterinfo"
class $modify(CreatorLayer)
{
    static void onModify(auto & self)
    {
        // run after betterinfo
        self.setHookPriority("CreatorLayer::init", -100);
    }

    bool init()
    {
        if (!CreatorLayer::init()) return false;

        if (Mod::get()->getSettingValue<bool>("map-packs"))
        {
            // remove map packs
            auto mpb = this->getChildByIDRecursive("map-packs-button");
            mpb->setVisible(false);

            // move the map to where map packs used to be
            this->getChildByIDRecursive("map-button")->setPosition(mpb->getPosition());

            // then shift all the rest over
            // somehow these magic numbers work fine on every aspect ratio I've tested (4:3 (actually 3:2 but whatever), 16:9 and 21:9 (actually 20:9 but ignore my weird aspect ratio issues ok?))
            this->getChildByIDRecursive("daily-button")->setPositionX(79.35);
            this->getChildByIDRecursive("weekly-button")->setPositionX(176.45);
            this->getChildByIDRecursive("event-button")->setPositionX(273.55);
            this->getChildByIDRecursive("gauntlets-button")->setPositionX(370.65);
        }

        if (Mod::get()->getSettingValue<bool>("move-betterinfo"))
        {
            // move betterinfo button to bottom left (if it exists)
            // for some reason setPosition doesn't seem to work for me
            // so two setPositionX/Ys will have to do
            if (auto biButton = this->getChildByID("cvolton.betterinfo/center-right-menu"))
            {
                biButton->setPositionX(28);
                biButton->setPositionY(28);
            }
        }

        if (auto biButton = this->getChildByID("cvolton.betterinfo/center-right-menu")) biButton->setZOrder(1);

        return true;
    }
};

// "garage-bg-remove" + "garage-lock-tap-remove" + 
class $modify(GJGarageLayer)
{
    static void onModify(auto & self)
    {
        self.setHookPriority("GJGarageLayer::init", 100);
    }

    bool init()
    {
        if (!GJGarageLayer::init()) return false;

        if (Mod::get()->getSettingValue<bool>("garage-bg-remove"))
        {
            // remove the background from the icon kit
            if (auto bg = typeinfo_cast<CCScale9Sprite*>(this->getChildren()->objectAtIndex(8)))
                bg->setVisible(false);
        }

        if (Mod::get()->getSettingValue<bool>("garage-lock-tap-remove"))
        {
            // remove tap for more info lock
            if (auto lock = typeinfo_cast<CCSprite*>(this->getChildren()->objectAtIndex(9)))
                lock->setVisible(false);

            // shuffle everything around
            // my excuse for the overuse of objectAtIndex is that literally nobody edits the icon kit
            // also how else am I meant to grab them (without looping through every node) there's no node ids
            // also the option literally says there may be a lot of mod incompatiblities using this

            if (auto playerIconPreview = typeinfo_cast<SimplePlayer*>(this->getChildren()->objectAtIndex(7)))
                playerIconPreview->setPositionY(222);

            if (auto iconTypeSelector = typeinfo_cast<CCMenu*>(this->getChildren()->objectAtIndex(10)))
            {
                iconTypeSelector->setPositionY(171);
                // for whatever reason the left and right arrows are in the icon type selector thingymabob
                if (auto leftArrow = typeinfo_cast<CCMenuItemSpriteExtra*>(iconTypeSelector->getChildren()->objectAtIndex(11)))
                    leftArrow->setPositionY(-75);
                if (auto rightArrow = typeinfo_cast<CCMenuItemSpriteExtra*>(iconTypeSelector->getChildren()->objectAtIndex(12)))
                    rightArrow->setPositionY(-75);
            }

            if (auto weirdGroundLineThing = typeinfo_cast<CCSprite*>(this->getChildren()->objectAtIndex(6)))
                weirdGroundLineThing->setPositionY(197);
        }

        return true;
    }
};

// "fix-joystick-scroll"
class $modify(LevelBrowserLayer)
{
    bool init(GJSearchObject * p0)
    {
        if (!LevelBrowserLayer::init(p0)) return false;

        // put joystick controller stuff in correct position
        if (Mod::get()->getSettingValue<bool>("fix-joystick-scroll"))
        {
            auto nodes = this->getChildren();
            for (int i = 0; i < this->getChildrenCount(); ++i)
            {
                auto node = nodes->objectAtIndex(i);

                if (auto spriteNode = typeinfo_cast<CCSprite*>(node))
                {
                    // test if it's in the correct position to be the controller buttons
                    // there may be a better way to test this but not that I know of
                    if (spriteNode->getPositionX() == 545)
                    {
                        // if so then fix it
                        spriteNode->setPositionY(spriteNode->getPositionY() - 140);
                        spriteNode->setPositionX(30);
                    }
                }
            }
        }

        return true;
    }
};
