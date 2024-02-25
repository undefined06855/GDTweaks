#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include "../alert.hpp"

using namespace geode::prelude;

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
            alert("icon kit background", "removing the icon kit background", this);
        }

    endOfGarageBGRemove:

        if (Mod::get()->getSettingValue<bool>("garage-lock-tap-remove"))
        {
            // remove tap for more info lock

            // lock->setVisible(false);
            // player icon -> setpositiony 222
            // icon type selector - > setposition y 171
            // weird ground line -> psoiton y 197

            if (auto lock = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("tap-more-hint")))
                lock->setVisible(false);
            else
            {
                alert("tap-more-hint", "removing the tap for more info lock", this);
                goto endOfGarageLockTapRemove;
            }

            if (auto player = typeinfo_cast<SimplePlayer*>(this->getChildByIDRecursive("player-icon")))
                player->setPositionY(222);
            else
            {
                alert("player-icon", "removing the tap for more info lock", this);
                goto endOfGarageLockTapRemove;
            }

            if (auto iconTypeSelector = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("category-menu")))
                iconTypeSelector->setPositionY(171);
            else
            {
                alert("category-menu", "removing the tap for more info lock", this);
                goto endOfGarageLockTapRemove;
            }

            if (auto line = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("floor-line")))
                line->setPositionY(197);
            else
            {
                alert("floor-line", "removing the tap for more info lock", this);
                goto endOfGarageLockTapRemove;
            }
        }

    endOfGarageLockTapRemove:

        return true;
    }
};