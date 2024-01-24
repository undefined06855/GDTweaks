#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
using namespace geode::prelude;

float scale = 0;

class MyLayer : public CCLayer {
public:
    void onClick(CCObject* sender) {
        FLAlertLayer::create("your mum", "why did you press this", "who knows")->show();
    }
};

class $modify(MenuLayer) {
public:
    float i = 0;

    bool init()
    {
        if (!MenuLayer::init()) return false;

        // prompt player
        geode::createQuickPopup(
            "NOOOOOOOO",
            "WHY DID YOU LEAVE THIS MENU",
            "ok", "no",
            [](auto, auto) {}
        );

        // add test buton
        auto menu = this->getChildByID("main-menu");
        auto iconKitBtn = menu->getChildByID("icon-kit-button");

        auto btn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("test buton"),
            this,
            menu_selector(MyLayer::onClick)
        );

        btn->setPosition(iconKitBtn->getPosition() - CCPoint{ 50.f, 0.f });
        btn->setScaleY(3);
        menu->addChild(btn);

        // remove geode button hahaah
        auto geodeButton = this->getChildByIDRecursive("geode.loader/geode-button");
        geodeButton->setVisible(false);

        // remove buttons
        this->getChildByIDRecursive("achievements-button")->setVisible(false);
        this->getChildByIDRecursive("settings-button")->setVisible(false);
        this->getChildByIDRecursive("stats-button")->setVisible(false);
        this->getChildByIDRecursive("newgrounds-button")->setVisible(false);
        this->getChildByIDRecursive("profile-button")->setVisible(false);
        this->getChildByIDRecursive("daily-chest-button")->setVisible(false);
        this->getChildByIDRecursive("more-games-button")->setVisible(false);
        this->getChildByIDRecursive("icon-kit-button")->setVisible(false);
        this->getChildByIDRecursive("play-button")->setVisible(false);
        this->getChildByIDRecursive("profile-button")->setVisible(false);
        this->getChildByIDRecursive("social-media-menu")->setVisible(false);
        this->getChildByIDRecursive("main-title")->setScaleX(4);
        this->getChildByIDRecursive("main-title")->setScaleY(1.5);

        return true;
    }

    void draw()
    {
        scale += 0.05;
        this->getChildByIDRecursive("main-title")->setScaleX(scale);

        MenuLayer::draw();
    }
};

class $modify(CreatorLayer)
{
    bool init()
    {
        if (!CreatorLayer::init()) return false;

        auto mapPackBtn = this->getChildByIDRecursive("map-packs-button");
        mapPackBtn->setVisible(false);

        return true;
    }
};
