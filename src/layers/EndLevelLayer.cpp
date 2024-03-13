#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include "../utils.hpp"

using namespace geode::prelude;

// "remove-chains"
class $modify(EndLevelLayer)
{
	void customSetup()
	{
		EndLevelLayer::customSetup();

		if (Mod::get()->getSettingValue<bool>("remove-chains"))
		{
			if (auto chain = this->getChildByIDRecursive("chain-left"))
				chain->setVisible(false);
            else
            {
                alert("chain-left", "remove the chains from the level end screen", this);
                goto endOfRemoveChains;
            }

			if (auto chain = this->getChildByIDRecursive("chain-right"))
				chain->setVisible(false);
			else
			{
				alert("chain-right", "remove the chains from the level end screen", this);
				goto endOfRemoveChains;
			}
		}

	endOfRemoveChains:

		return;
	}
};
