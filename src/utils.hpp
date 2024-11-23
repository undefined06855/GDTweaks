#pragma once
#include <Geode/Geode.hpp>
#include <random>

using namespace geode::prelude;

namespace gdtutils {

extern void alert(std::string, std::string, cocos2d::CCNode* _this);
extern std::random_device dev;
extern std::mt19937 rng;
extern bool loadingMainMenu;

}

