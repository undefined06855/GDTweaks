#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include "../utils.hpp"

using namespace geode::prelude;

// "map-packs" + "move-betterinfo" + "map-packs-hof"
class $modify(MyCreatorLayer, CreatorLayer)
{
    static void onModify(auto & self)
    {
        // run after betterinfo
        self.setHookPriority("CreatorLayer::init", -100);
    }

    // definitely not taken from BetterInfo :) (thanks cvolton)
    void onFame(CCObject* object)
    {
        auto searchObject = GJSearchObject::create(SearchType::HallOfFame);
        auto browserLayer = LevelBrowserLayer::scene(searchObject);

        auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

        CCDirector::sharedDirector()->pushScene(transitionFade);
    }

    bool init()
    {
        if (!CreatorLayer::init()) return false;

        if (Mod::get()->getSettingValue<bool>("map-packs"))
        {
            if (Mod::get()->getSettingValue<bool>("map-packs-hof"))
            {
                if (Mod::get()->getSettingValue<bool>("suppress-warnings")) goto endOfMapPacks;

                auto alert = FLAlertLayer::create(
                    "Notice (from GDTweaks)",
                    "GDTweaks couldn't <cy>remove the map pack button</c> because <cj>\"Replace \"Map Packs\" with Hall Of Fame\"</c> is enabled. You can suppress these dialogs in the GDTweaks settings.",
                    "OK"
                );
                alert->m_scene = this;
                alert->show();

                goto endOfMapPacks;
            }

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

            // move the map to where map packs used to be
            this->getChildByIDRecursive("map-button")->setPosition(mpb->getPosition());

            // then shift all the rest over
            // somehow these magic numbers work fine on every aspect ratio I've tested (4:3 (actually 3:2 but whatever), 16:9 and 21:9 (actually 20:9 but ignore my weird aspect ratio issues ok?))
            this->getChildByIDRecursive("daily-button")->setPositionX(79.35);
            this->getChildByIDRecursive("weekly-button")->setPositionX(176.45);
            this->getChildByIDRecursive("event-button")->setPositionX(273.55);
            this->getChildByIDRecursive("gauntlets-button")->setPositionX(370.65);

            // and remove map pack button
            mpb->removeFromParent();
        }

    endOfMapPacks:

        if (Mod::get()->getSettingValue<bool>("move-betterinfo"))
        {
            if (!Loader::get()->isModLoaded("cvolton.betterinfo")) goto endOfBetterInfo;

            // move betterinfo button to bottom left (if it exists)
            // for some reason setPosition doesn't seem to work for me
            // so two setPositionX/Ys will have to do
            if (auto biButton = this->getChildByIDRecursive("cvolton.betterinfo/main-button"))
            {
                auto* bottomLeftMenu = static_cast<CCMenu*>(this->getChildByID("bottom-left-menu"));
                
                biButton->removeFromParent();
                bottomLeftMenu->addChild(biButton);
                bottomLeftMenu->updateLayout();
            }
            else
            {
                alert("cvolton.betterinfo/main-button", "move the betterinfo button to the corner", this);
                goto endOfBetterInfo; // not needed but might as well in case expansion is needed
            }
        }

    endOfBetterInfo:

        if (Mod::get()->getSettingValue<bool>("map-packs-hof"))
        {
            auto mpb = this->getChildByIDRecursive("map-packs-button");
            if (!mpb)
            {
                alert("map-packs-button", "replacing the Map Packs button with the Hall Of Fame", this);
                goto endOfHallOfFame;
            }
            mpb->setVisible(false);
            auto mapSprite = typeinfo_cast<CCSprite*>(mpb->getChildren()->objectAtIndex(0));

            // taken from BetterInfo :) thanks cvolton
            auto* spr = CCSprite::create("GJ_fameBtn_001.png"_spr);
            auto fameBtn = CCMenuItemSpriteExtra::create(
                spr,
                this,
                menu_selector(MyCreatorLayer::onFame)
            );
            // i could just set it to "fame-button" but im nice :)
            fameBtn->setID("undefined0.gdtweaks/fame-button");

            auto cbm = this->getChildByIDRecursive("creator-buttons-menu");
            // this should exist in theory but might as well check
            if (!cbm)
            {
                alert("creator-buttons-menu", "replacing the Map Packs button with the Hall Of Fame", this);
                goto endOfHallOfFame;
            }

            // add the fame button
            cbm->addChild(fameBtn);
            spr->setScale(0.8); // for some reason it's just so much bigger than everything else without this

            // for some reason i cant get the positioning or anything right so ill just set everything to what
            // the map packs had it at
            auto mapSpriteContentSize = mapSprite->getContentSize();
            mapSpriteContentSize.width = 93.5;
            spr->setContentSize(mapSpriteContentSize);
            fameBtn->setContentSize(mpb->getContentSize());
            spr->setPosition(mapSprite->getPosition());
            fameBtn->setPosition(mpb->getPosition());
        }

    endOfHallOfFame:

        // this is just a nice thing to do
        if (auto biButton = this->getChildByID("cvolton.betterinfo/center-right-menu")) biButton->setZOrder(1);

        return true;
    }
};
