#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "../utils.hpp"

#include <random>
// setup random stuff 2
std::uniform_int_distribution<std::mt19937::result_type> distBG(1, 59);

using namespace geode::prelude;

// "title-text" + "fix-main-menu-settings-gamepad" + "title-buttons" + "replace-more-games-w-texture" + "randomise-main-menu-bg" (bg half) + "move-player-to-corner" + "hide-achievements-button" + "hide-stats-button"
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

#ifndef GEODE_IS_ANDROID
        if (Mod::get()->getSettingValue<bool>("randomise-main-menu-bg"))
        {
            // this only handles the background, not the ground
            if (auto mainMenuBG = typeinfo_cast<MenuGameLayer*>(this->getChildByIDRecursive("main-menu-bg")))
            {
                if (auto tex = typeinfo_cast<CCTextureProtocol*>(mainMenuBG->getChildren()->objectAtIndex(0))) {
                    std::string rand = std::to_string(distBG(rng));

                    /*
                     * 0: auto
                     * 1: sd
                     * 2: hd
                     * 3: uhd
                     */
                    int textureQuality = GameManager::sharedState()->m_texQuality;
                    std::string textureQualityString = "-uhd"; // textureQuality == 0 or 3
                    // auto uses the size of the window to determine the resolution
                    // but idc it's always uhd :)
                    // if someone can figure out how it does it then please submit a pr
                    // thanks

                    if      (textureQuality == 1) textureQualityString = ""; // "sd" quality doesnt have a suffix
                    else if (textureQuality == 2) textureQualityString = "-hd";

                    // pad number to be two digits
                    if (rand.length() == 1)
                        rand = "0" + rand;
                    
                    std::string string = "game_bg_" + rand + "_001" + textureQualityString + ".png";


                    // literally had to follow a 10 YEAR OLD STACKOVERFLOW POST for this to work
                    // https://stackoverflow.com/a/21699549
                    // (and it doesnt even work on android)
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
#endif
#ifndef GEODE_IS_MACOS:
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
#endif
        // bottom left
        if (Mod::get()->getSettingValue<int64_t>("move-player-to-corner") == 1)
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
        else if (Mod::get()->getSettingValue<int64_t>("move-player-to-corner") == 2)
        {
            // test if it can't move the player to the bottom right corner
            if (Mod::get()->getSettingValue<bool>("replace-more-games-w-texture"))
            {
                if (Mod::get()->getSettingValue<bool>("suppress-warnings")) goto endOfMovePlayerToCorner;

                auto alert = FLAlertLayer::create(
                    "Notice (from GDTweaks)",
                    "GDTweaks couldn't <cy>move the player to the bottom right corner</c> because <cj>\"Replace \"More Games\" with texture pack\"</c> is enabled. You can suppress these dialogs in the GDTweaks settings.",
                    "OK"
                );
                alert->m_scene = this;
                alert->show();

                goto endOfMovePlayerToCorner;
            }

            // it can! let's do the thing

            auto mgm = this->getChildByIDRecursive("more-games-menu"); // this might have been deleted
            auto mgb = this->getChildByIDRecursive("more-games-button");
            auto pfb = this->getChildByIDRecursive("profile-button");
            auto user = this->getChildByIDRecursive("player-username");

            if (!mgb || !mgm || !pfb || !user)
            {
                std::string label = "";
                if (!mgb) label = "more-games-button";
                else if (!mgm) label = "more-games-menu";
                else if (!pfb) label = "profile-button";
                else if (!user) label = "player-username";

                alert(label, "replace the more games button with the texture selector", this);
                goto endOfMovePlayerToCorner;
            }

            if (auto profileButton = typeinfo_cast<CCMenuItemSpriteExtra*>(pfb))
            {
                if (auto username = typeinfo_cast<CCLabelBMFont*>(user))
                {
                    profileButton->removeFromParent();
                    username->removeFromParent();

                    // create a ccmenu
                    auto ccmenu = CCMenu::create();
                    ccmenu->setLayout(AxisLayout::create());
                    if (auto rawLayout = ccmenu->getLayout())
                        if (auto layout = typeinfo_cast<AxisLayout*>(rawLayout))
                        {
                            layout->setAxis(Axis::Column);
                        }

                    ccmenu->addChild(username);
                    ccmenu->addChild(profileButton);
                    ccmenu->updateLayout();
                    username->setScale(0.625);
                    username->setPositionY(username->getPositionY() - 9);

                    // remove more games button
                    mgb->removeFromParent();
                    mgm->addChild(ccmenu);
                    mgm->updateLayout();

                    mgm->setPositionY(mgm->getPositionY() + 15);
                    mgm->setScale(1.35);
                }
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

        if (Mod::get()->getSettingValue<bool>("move-daily") && (Mod::get()->getSettingValue<int64_t>("move-player-to-corner") == 2))
        {
            auto alert = FLAlertLayer::create(
                "Notice (from GDTweaks)",
                "GDTweaks couldn't <cy>move the daily chest</c> because <cj>the account icon is in the same corner</c>. You can suppress these dialogs in the GDTweaks settings.",
                "OK"
            );
            alert->m_scene = this;
            alert->show();

            goto endOfMoveDaily;
        }

        if (Mod::get()->getSettingValue<bool>("move-daily"))
        {
            CCMenuItemSpriteExtra* dailyChest = nullptr;

            // move daily chest to bottom right
            if (auto mgb = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("more-games-button")))
                mgb->removeFromParent();
            else
            {
                alert("more-games-button", "move the daily chest to the bottom right", this);
                goto endOfMoveDaily;
            }

            if (auto _dailyChest = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("daily-chest-button")))
            {
                _dailyChest->removeFromParent();
                dailyChest = _dailyChest;
            }
            else
            {
                alert("daily-chest-button", "move the daily chest to the bottom right", this);
                goto endOfMoveDaily;
            }

            if (auto mgm = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("more-games-menu")))
            {
                mgm->addChild(dailyChest);
                mgm->updateLayout();
            }
            else
            {
                alert("more-games-menu", "move the daily chest to the bottom right", this);
                goto endOfMoveDaily;
            }
        }

    endOfMoveDaily:

        if (Mod::get()->getSettingValue<bool>("hide-achievements-button"))
        {
            // hide achievements button
            if (auto achb = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("achievements-button")))
            {
                achb->removeFromParent();
                if (auto btm = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("bottom-menu")))
                    btm->updateLayout();
                else
                {
                    alert("bottom-menu", "hide the achievements button", this);
                    goto endOfHideAchievementsButton;
                }
            }
            else
            {
                alert("achievements-button", "hide the achievements button", this);
                goto endOfHideAchievementsButton;
            }
        }

    endOfHideAchievementsButton:

        if (Mod::get()->getSettingValue<bool>("hide-stats-button"))
        {
            // hide stats button
            if (auto statb = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("stats-button")))
            {
                statb->removeFromParent();
                if (auto btm = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("bottom-menu")))
                    btm->updateLayout();
                else
                {
                    alert("bottom-menu", "hide the achievements button", this);
                    goto endOfHideStatsButton;
                }
            }
            else
            {
                alert("stats-button", "move the daily chest to the bottom right", this);
                goto endOfHideStatsButton;
            }
        }

    endOfHideStatsButton:

        return true;
    }

    void onPlay(CCObject * sender)
    {
        MenuLayer::onPlay(sender);

        if (!Mod::get()->getSettingValue<bool>("fire-in-the-hole")) return;

        FMODAudioEngine::sharedEngine()->playEffect("fire.ogg"_spr);
    }
};
