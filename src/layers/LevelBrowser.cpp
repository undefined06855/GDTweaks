#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include "../utils.hpp"

using namespace geode::prelude;

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