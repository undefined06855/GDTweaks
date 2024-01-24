#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/GauntletSelectLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/SecretRewardsLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
using namespace geode::prelude;

static void removeCorners(auto _this)
{
    // if remove corners is off, skip everything
    if (!Mod::get()->getSettingValue<bool>("remove-corners")) return;

    // self-explanatory what this does
    auto nodes = _this->getChildren();
    for (int i = 0; i < _this->getChildrenCount(); ++i)
    {
        if (auto node = nodes->objectAtIndex(i))
        {
            if (auto spriteNode = typeinfo_cast<CCSprite*>(node))
            {
                if ((spriteNode->getZOrder() == 1 || spriteNode->getZOrder() == -1) && spriteNode->getPositionY() != 321)
                {
                    spriteNode->setVisible(false);
                }
            }
        }
    }
}

class $modify(MenuLayer)
{
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

        if (Mod::get()->getSettingValue<bool>("title-buttons"))
        {
            // remove newgrounds button + move geode button to where it used to be
            this->getChildByIDRecursive("newgrounds-button")->setVisible(false);
            this->getChildByIDRecursive("geode.loader/geode-button")->setPositionX(229.25);
            this->getChildByIDRecursive("bottom-menu")->setPositionX(313.5);
        }

        return true;
    }
};

class $modify(CreatorLayer)
{
    static void onModify(auto& self)
    {
        self.setHookPriority("CreatorLayer::init", -100);
    }

    bool init()
    {
        if (!CreatorLayer::init()) return false;
        if (Mod::get()->getSettingValue<bool>("map-packs"))
        {
            // remove map packs
            this->getChildByIDRecursive("map-packs-button")->setVisible(false);

            // move the map to where map packs used to be
            this->getChildByIDRecursive("map-button")->setPositionX(314.6);
            this->getChildByIDRecursive("map-button")->setPositionY(65.4);

            // then shift all the rest over
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
        else if (auto biButton = this->getChildByID("cvolton.betterinfo/center-right-menu")) biButton->setZOrder(1);

        // then remove the corners
        removeCorners(this);

        return true;
    }
};

class $modify(LevelInfoLayer)
{
    bool init(GJGameLevel * p0, bool p1)
    {
        if (!LevelInfoLayer::init(p0, p1)) return false;

        if (Mod::get()->getSettingValue<bool>("remove-corners"))
        {
            // remove corners
            this->getChildByIDRecursive("bottom-left-art")->setVisible(false);
            this->getChildByIDRecursive("bottom-right-art")->setVisible(false);
        }

        return true;
    }
};

// remove corners
class $modify(GauntletSelectLayer)
{
    bool init(int p0)
    {
        if (!GauntletSelectLayer::init(p0)) return false;

        removeCorners(this);

        return true;
    }
};

// remove corners
class $modify(GJGarageLayer)
{
    bool init()
    {
        if (!GJGarageLayer::init()) return false;

        removeCorners(this);

        return true;
    }
};

// remove corners
class $modify(SecretRewardsLayer)
{
    bool init(bool p0)
    {
        if (!SecretRewardsLayer::init(p0)) return false;

        removeCorners(this);

        return true;
    }
};

// remove corners + fix joystick icon pos
class $modify(LevelBrowserLayer)
{
    bool init(GJSearchObject* p0)
    {
        if (!LevelBrowserLayer::init(p0)) return false;

        if (Mod::get()->getSettingValue<bool>("fix-joystick-scroll"))
        {
            // put joystick controller stuff in correct position
            auto nodes = this->getChildren();
            for (int i = 0; i < this->getChildrenCount(); ++i)
            {
                if (auto node = nodes->objectAtIndex(i))
                {
                    if (auto spriteNode = typeinfo_cast<CCSprite*>(node))
                    {
                        // test if it's in the correct position to be the controller buttons
                        if (spriteNode->getPositionX() == 545)
                        {
                            // if so then fix it
                            spriteNode->setPositionY(spriteNode->getPositionY() - 144);
                            spriteNode->setPositionX(30);
                        }
                    }
                }
            }
        }

        // and obviously remove corners
        removeCorners(this);

        return true;
    }
};

// remove corners and other stuf
class $modify(LevelSelectLayer)
{
    bool init(int p0)
    {
        if (!LevelSelectLayer::init(p0)) return false;

        if (Mod::get()->getSettingValue<bool>("remove-level-select-top"))
        {
            auto nodes = this->getChildren();
            for (int i = 0; i < this->getChildrenCount(); ++i)
            {
                if (auto node = nodes->objectAtIndex(i))
                {
                    if (auto spriteNode = typeinfo_cast<CCSprite*>(node))
                    {
                        if (spriteNode->getPositionY() == 321)
                        {
                            spriteNode->setVisible(false);
                        }
                    }
                }
            }
        }

        removeCorners(this);

        return true;
    }
};
