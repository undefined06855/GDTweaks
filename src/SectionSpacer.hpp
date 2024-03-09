#pragma once
// this could potentially may or may not be taken from hide+ (maybe)

#include <Geode/loader/SettingNode.hpp>

using namespace geode::prelude;

class SectionSpacerSetting;

class SectionSpacerSetting : public SettingValue {
protected:
    std::string m_placeholder;
public:
    // lines 5, 8, 11, and 12 are copied from GDUtils
    SectionSpacerSetting(std::string const& key, std::string const& modID, std::string const& placeholder)
        : SettingValue(key, modID), m_placeholder(placeholder) {}
    bool load(matjson::Value const& json) override { return true; }
    bool save(matjson::Value& json) const override { return true; }
    SettingNode* createNode(float width) override;
};

class SectionSpacerNode : public SettingNode {
protected:
    bool init(SectionSpacerSetting* value, float width) {
        if (!SettingNode::init(value)) return false;

        this->setContentSize({ width, 25.0});
        // why is this SO long
        std::string name = Mod::get()->getSettingDefinition(value->getKey())->get<CustomSetting>()->json->get<std::string>("name");

        auto menu = CCMenu::create();
        auto label = CCLabelBMFont::create(name.c_str(), "bigFont.fnt");

        // streakb is the only solid square texture - square02-001 has rounded corners
        auto lineLeft = CCSprite::create("streakb_01_001.png");
        auto lineRight = CCSprite::create("streakb_01_001.png");

        label->setScale(.5);
        label->setPositionX(0);

        lineLeft->setPosition(CCPoint{ -115, 0 });
        lineLeft->setScaleY(0.3);
        lineLeft->setScaleX(11);
        lineLeft->setOpacity(111);
        lineLeft->setColor(ccColor3B{ 0, 0, 0 });

        lineRight->setPosition(CCPoint{ 115, 0 });
        lineRight->setScaleY(0.3);
        lineRight->setScaleX(11);
        lineRight->setOpacity(111);
        lineRight->setColor(ccColor3B{ 0, 0, 0 });


        menu->addChild(lineLeft);
        menu->addChild(lineRight);
        menu->addChild(label);
        menu->setPosition(width / 2, 25.0 / 2);

        this->addChild(menu);

        return true;
    }

public:
    void commit() override { this->dispatchCommitted(); }

    bool hasUncommittedChanges() override { return false; }
    bool hasNonDefaultValue() override { return false; }
    void resetToDefault() override {}

    static SectionSpacerNode* create(SectionSpacerSetting* value, float width) {
        auto ret = new SectionSpacerNode();
        if (ret && ret->init(value, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};