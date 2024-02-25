// good luck reading it, i should probably split it up into multiple files
#include <Geode/Geode.hpp>
#include <Geode/modify/GJGroundLayer.hpp>
#include "../utils.hpp"

#include <random>
// setup random stuff
//std::random_device dev;
//std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> distGround(1, 22);
std::uniform_int_distribution<std::mt19937::result_type> distGroundLine(1, 3);

using namespace geode::prelude;

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