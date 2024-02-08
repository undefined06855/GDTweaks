// warning: shitty code incoming
// good luck reading it, i should probably split it up into multiple files
#include <Geode/Geode.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/GJGroundLayer.hpp>

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

void alert(std::string labelName, std::string taskName, cocos2d::CCNode* _this)
{
    std::cout << "couldn't find " + labelName + " for " + taskName << std::endl;

    if (Mod::get()->getSettingValue<bool>("suppress-warnings")) return;

    auto alert = FLAlertLayer::create(
        "Notice (from GDTweaks)",
        "GDTweaks couldn't "+ taskName +" because of a mod incompatibility, (" + labelName + " was not found). You can suppress these dialogs in the GDTweaks settings.",
        "OK"
    );
    alert->m_scene = _this;
    alert->show();
}

bool loadingMainMenu = false;

// "randomise-main-menu-bg" (ground half)
class $modify(GJGroundLayer)
{
    static GJGroundLayer* create(int groundType, int lineType)
    {
        if (!loadingMainMenu) return GJGroundLayer::create(groundType, lineType);

        int64_t ground = Mod::get()->getSettingValue<int64_t>("force-ground");

        //std::cout << ground << std::endl;

        if (ground != 0)
        {
            //std::cout << "yah" << std::endl;
            groundType = std::stoi(std::to_string(ground));
            //std::cout << "did that" << std::endl;
        }
        else if (Mod::get()->getSettingValue<bool>("randomise-main-menu-bg"))
        {
            groundType = distGround(rng);
            lineType = distGroundLine(rng);
        }

        //std::cout << "abt to create" << std::endl;
        auto* groundLayer = GJGroundLayer::create(groundType, lineType);
        //std::cout << "created!" << std::endl;

        if (Mod::get()->getSettingValue<bool>("remove-ground"))
            groundLayer->setVisible(false);

        return groundLayer;
    }
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

// "title-text" + "fix-main-menu-settings-gamepad" + "title-buttons" + "replace-more-games-w-texture" + "randomise-main-menu-bg" (bg half) + "move-player-to-corner"
class $modify(MenuLayer)
{
    static void onModify(auto & self)
    {
        // run after texture selector
        self.setHookPriority("MenuLayer::init", -100);
    }

    bool init()
    {
        loadingMainMenu = true;
        if (!MenuLayer::init()) return false;
        loadingMainMenu = false;

        if (Mod::get()->getSettingValue<bool>("title-text"))
        {
            // make title funny
            auto t = this->getChildByIDRecursive("main-title");

            // ok but who in their right mind would remove the main menu title
            // might as well check for it anyway
            if (!t)
            {
                alert("main-title", "skew the title text", this);
                goto endOfTitleThing;
            }

            t->setScale(1.025);
            t->setSkewX(52);
            t->setSkewY(10);
            t->setZOrder(-1);
            t->setPositionX(285);
            t->setPositionY(258);

            // put bg behind title
            if (!this->getChildByIDRecursive("main-menu-bg"))
            {
                alert("main-menu-bg", "skew the title text", this);
                goto endOfTitleThing;
            }

            this->getChildByIDRecursive("main-menu-bg")->setZOrder(-2);
        }

    endOfTitleThing:

        if (Mod::get()->getSettingValue<bool>("remove-exit-button"))
        {
            if (auto exitBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("close-button")))
            {
                // would just setVisible(false) BUT then I wouldnt be able to update the layout because I know
                // that there's mods that update the `close-menu`
                exitBtn->removeFromParent();

                // speaking of close-menu:
                if (auto exitMenu = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("close-menu")))
                    exitMenu->updateLayout();
                else
                {
                    alert("close-menu", "remove the exit button", this);
                    goto endOfRemoveExitButton;
                }
            }
            else
            {
                alert("close-button", "remove the exit button", this);
                goto endOfRemoveExitButton;
            }
        }

    endOfRemoveExitButton:

        if (Mod::get()->getSettingValue<bool>("fix-main-menu-settings-gamepad"))
        {
            // this is added by geode, so should exist in theory, but might as well check for nullptr anyway
            if (auto controllerIcon = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("settings-gamepad-icon")))
            {
                // wowee magic numbers
                // Ic hope this isnt broken on widescreen
                controllerIcon->setPositionX(255.0f);
            }
        }

        if (Mod::get()->getSettingValue<bool>("title-buttons"))
        {
            // remove newgrounds button + move geode button to where it used to be
            auto ng = this->getChildByIDRecursive("newgrounds-button");
            if (!ng)
            {
                alert("newgrounds-button", "move geode button to where the newgrounds button was", this);
                goto endOfTitleButtonThing;
            }
            this->getChildByIDRecursive("geode.loader/geode-button")->setPositionX(ng->getPositionX());
            ng->removeFromParent();
            auto btm = this->getChildByIDRecursive("bottom-menu");
            if (!btm)
            {
                alert("bottom-menu", "move geode button to where the newgrounds button was", this);
                goto endOfTitleButtonThing;
            }
            btm->updateLayout();
        }

    endOfTitleButtonThing:

        if (Mod::get()->getSettingValue<bool>("replace-more-games-w-texture"))
        {
            if (!Loader::get()->isModLoaded("geode.texture-loader")) goto endOfReplaceMoreGamesWithTexture;

            // replace more games button with texture selector
            // alright so I know that this is probably definitely bad practice to use objectAtIndex but
            // it's the only way to get this afaik and to be honest what other mods actually use the right side menu?
            // ok so basically there is a mod that uses the right side menu
            // and hopefully that's going to be the only one because I am NOT remaking these magic numbers again
            auto rsm = this->getChildByIDRecursive("right-side-menu");
            auto obj = rsm->getChildren()->objectAtIndex(rsm->getChildrenCount() == 2 ? 1 : 2); // 1 is normal, 2 is modified
            auto mgm = this->getChildByIDRecursive("more-games-menu"); // this might have been deleted
            auto mgb = this->getChildByIDRecursive("more-games-button");

            if (!mgb || !mgm || !rsm || !obj)
            {
                std::string label = "";
                if (!mgb) label = "more-games-button";
                else if (!mgm) label = "more-games-menu";
                else if (!rsm) label = "right-side-menu";
                else if (!obj) label = "(the texture selector menuitem)";

                alert(label, "replace the more games button with the texture selector", this);
                goto endOfReplaceMoreGamesWithTexture;
            }

            if (auto texSelector = typeinfo_cast<CCMenuItemSpriteExtra*>(obj))
            {
                // remove texture selector from right side menu
                texSelector->removeFromParent();

                // remove more games button + add texture selector to the same menu
                mgb->removeFromParent();
                mgm->addChild(texSelector);

                // refresh everything
                rsm->updateLayout();
                mgm->updateLayout();

                // and move the menu to the left a smidge
                mgm->setPositionX(mgm->getPositionX() - 16);
            }
        }

    endOfReplaceMoreGamesWithTexture:

        if (Mod::get()->getSettingValue<bool>("randomise-main-menu-bg"))
        {
            // this only handles the background, not the ground
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
                else
                {
                    alert("(first child of) main-menu-bg", "randomise the main menu background", this);
                    goto endOfRandomiseMainMenuBG;
                }
            }
            else
            {
                alert("main-menu-bg", "randomise the main menu background", this);
                goto endOfRandomiseMainMenuBG;
            }
        }

    endOfRandomiseMainMenuBG:

        int64_t bg = Mod::get()->getSettingValue<int64_t>("force-bg");
        if (bg != 0)
        {
            if (auto mainMenuBG = typeinfo_cast<MenuGameLayer*>(this->getChildByIDRecursive("main-menu-bg")))
            {
                if (auto tex = typeinfo_cast<CCTextureProtocol*>(mainMenuBG->getChildren()->objectAtIndex(0))) {
                    std::string bgIndexStr = std::to_string(bg);
                    if (bgIndexStr.length() == 1)
                        bgIndexStr = "0" + bgIndexStr;
                    std::string string = "game_bg_" + bgIndexStr + "_001-uhd.png";
                    //std::cout << string << std::endl;
                    auto newTexture = CCSprite::create(string.c_str());
                    ccTexParams tp = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
                    newTexture->getTexture()->setTexParameters(&tp);
                    tex->setTexture(newTexture->getTexture());
                }
                else
                {
                    alert("(first child of) main-menu-bg", "force the main menu background to id " + bg, this);
                    goto endOfForceBGId;
                }
            }
            else
            {
                alert("main-menu-bg", "force the main menu background to id " + bg, this);
                goto endOfForceBGId;
            }
        }
        
    endOfForceBGId:

        if (Mod::get()->getSettingValue<bool>("remove-player"))
        {
            if (auto mainMenuBG = typeinfo_cast<MenuGameLayer*>(this->getChildByIDRecursive("main-menu-bg")))
            {
                auto children = mainMenuBG->getChildren();
                for (int i = 0; i < mainMenuBG->getChildrenCount(); i++)
                {
                    if (auto node = typeinfo_cast<HardStreak*>          (mainMenuBG->getChildren()->objectAtIndex(i))) { node->setVisible(false); continue; }
                    if (auto node = typeinfo_cast<CCMotionStreak*>      (mainMenuBG->getChildren()->objectAtIndex(i))) { node->setVisible(false); continue; }
                    if (auto node = typeinfo_cast<CCParticleSystemQuad*>(mainMenuBG->getChildren()->objectAtIndex(i))) { node->setVisible(false); continue; }
                    if (auto node = typeinfo_cast<PlayerObject*>        (mainMenuBG->getChildren()->objectAtIndex(i))) { node->setVisible(false); continue; }
                }
            }
            else
            {
                alert("main-menu-bg", "remove the player", this);
                goto endOfRemovePlayer;
            }
        }

    endOfRemovePlayer:

        if (Mod::get()->getSettingValue<bool>("move-player-to-corner"))
        {
            if (auto socialMediaMenu = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("social-media-menu")))
                socialMediaMenu->setVisible(false); // a mod already does this, but have to hide it anyway in case the player doesn't have it

            if (auto profileMenu = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("profile-menu")))
                profileMenu->setPositionY(profileMenu->getPositionY() - 60);
            else
            {
                alert("profile-menu", "move the icon to the corner", this);
                goto endOfMovePlayerToCorner;
            }

            if (auto username = typeinfo_cast<CCLabelBMFont*>(this->getChildByIDRecursive("player-username")))
                username->setPositionY(username->getPositionY() - 60);
            else
            {
                alert("player-username", "move the icon to the corner", this);
                goto endOfMovePlayerToCorner;
            }
        }

    endOfMovePlayerToCorner:

        if (Mod::get()->getSettingValue<bool>("align-daily"))
        {
            if (auto rsm = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("right-side-menu")))
                rsm->setPositionY(rsm->getPositionY() - 10);
            else
            {
                alert("right-side-menu", "align the daily chest", this);
                goto endOfAlignDaily;
            }
        }

    endOfAlignDaily:

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

            // TODO: see if this works etc

            std::string buttonChecks[6] = {
                "daily-button",
                "weekly-button",
                "event-button",
                "gauntlets-button",
                "map-button",
                "map-packs-button"
            };

            // firstly we have to check every single thing is here - if not then a mod
            // will have incompatibilities
            for (int i = 0; i < 6; i++)
            {
                std::string labelName = buttonChecks[i];
                if (!this->getChildByIDRecursive(labelName))
                {
                    alert(labelName, "remove the map pack button", this);
                    goto endOfMapPacks;
                }
            }

            // guaranteed to exist bc of the check above
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


    endOfMapPacks:
        if (Mod::get()->getSettingValue<bool>("move-betterinfo"))
        {
            if (!Loader::get()->isModLoaded("cvolton.betterinfo")) goto endOfBetterInfo;

            // move betterinfo button to bottom left (if it exists)
            // for some reason setPosition doesn't seem to work for me
            // so two setPositionX/Ys will have to do
            if (auto biButton = this->getChildByID("cvolton.betterinfo/center-right-menu"))
            {
                biButton->setPositionX(28);
                biButton->setPositionY(28);
            }
            else
            {
                alert("cvolton.betterinfo/center-right-menu", "move the betterinfo button to the corner", this);
                goto endOfBetterInfo; // not needed but might as well in case expansion is needed
            }
        }

    endOfBetterInfo:

        // this is just a nice thing to do
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
            for (int i = 0; i < this->getChildrenCount(); i++)
            {
                if (auto bg = typeinfo_cast<CCScale9Sprite*>(this->getChildren()->objectAtIndex(i)))
                {
                    bg->setVisible(false);
                    goto endOfGarageBGRemove;
                }
            }

            //std::cout << "couldnt find ccscale9sprite for icon kit" << std::endl;
            if (!Mod::get()->getSettingValue<bool>("suppress-warnings"))
            {
                auto alert = FLAlertLayer::create(
                    "Notice (from GDTweaks)",
                    "GDTweaks couldn't find the icon kit background because of a mod incompatibility. You can suppress these dialogs in the GDTweaks settings.",
                    "OK"
                );
                alert->m_scene = this;
                alert->show();
            }
        }

    endOfGarageBGRemove:

        if (Mod::get()->getSettingValue<bool>("garage-lock-tap-remove"))
        {
            // remove tap for more info lock

            // it's easier to use the content size here
            auto lockSpriteContentSize = CCSprite::createWithSpriteFrameName("GJ_unlockTxt_001.png")->getContentSize();
            auto lineSpriteContentSize = CCSprite::createWithSpriteFrameName("floorLine_001.png")->getContentSize();

            bool foundLock = false;
            bool foundPlayer = false;
            bool foundWeirdGroundLineThing = false;
            bool foundIconTypeSelector = false;

            for (int i = 0; i < this->getChildrenCount(); i++)
            {
                //std::cout << i << std::endl;
                auto obj = this->getChildren()->objectAtIndex(i);

                // brah why cant i just use && :despair:
                // python ass syntax my god
                if (!foundLock)
                    if (auto lock = typeinfo_cast<CCSprite*>(obj))
                        if (lock->getContentSize()  == lockSpriteContentSize)
                        {
                            //std::cout << "found lock" << std::endl;
                            lock->setVisible(false);
                            foundLock = true;
                        }

                if (!foundPlayer)
                    if (auto playerIconPreview = typeinfo_cast<SimplePlayer*>(obj))
                    {
                        //std::cout << "found piconp" << std::endl;

                        playerIconPreview->setPositionY(222);
                        foundPlayer = true;
                    }

                if (!foundIconTypeSelector)
                    if (auto iconTypeSelector = typeinfo_cast<CCMenu*>(obj))
                        // before this check it could potentially be the only other ccmenu in garagelayer
                        if (iconTypeSelector->getChildrenCount() >= 13)
                        {
                            //std::cout << "found itypeselec" << std::endl;
                            foundIconTypeSelector = true;

                            iconTypeSelector->setPositionY(171);
                            // for whatever reason the left and right arrows are in the icon type selector thingymabob
                            if (auto leftArrow = typeinfo_cast<CCMenuItemSpriteExtra*>(iconTypeSelector->getChildren()->objectAtIndex(11)))
                                leftArrow->setPositionY(-75);
                            if (auto rightArrow = typeinfo_cast<CCMenuItemSpriteExtra*>(iconTypeSelector->getChildren()->objectAtIndex(12)))
                                rightArrow->setPositionY(-75);
                        }

                if (!foundWeirdGroundLineThing)
                    if (auto weirdGroundLineThing = typeinfo_cast<CCSprite*>(obj))
                        if (weirdGroundLineThing->getContentSize() == lineSpriteContentSize)
                        {
                            //std::cout << "found line" << std::endl;

                            weirdGroundLineThing->setPositionY(197);
                            foundWeirdGroundLineThing = true;
                        }
            }

            if (!foundLock)
            {
                alert("the lock", "removing the tap for more info lock", this);
                goto endOfGarageLockTapRemove;
            }

            if (!foundPlayer)
            {
                alert("the player sprite", "removing the tap for more info lock", this);
                goto endOfGarageLockTapRemove;
            }

            if (!foundIconTypeSelector)
            {
                alert("the icon type selector", "removing the tap for more info lock", this);
                goto endOfGarageLockTapRemove;
            }

            if (!foundWeirdGroundLineThing)
            {
                alert("the ground line thingymabob", "removing the tap for more info lock", this);
                goto endOfGarageLockTapRemove;
            }
        }

    endOfGarageLockTapRemove:

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
