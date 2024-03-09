#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include "../utils.hpp"

using namespace geode::prelude;

class $modify(LevelCell)
{
	void loadCustomLevelCell() {
		LevelCell::loadCustomLevelCell();

		if (Mod::get()->getSettingValue<bool>("remove-orbs"))
		{
			if (auto orbIcon = this->getChildByIDRecursive("orbs-icon"))
				orbIcon->setVisible(false);
			else
			{
				alert("orbs-icon", "remove the orbs label", this);
				goto endOfRemoveOrbs;
			}

			if (auto orbLabel = this->getChildByIDRecursive("orbs-label"))
				orbLabel->setVisible(false);
			else
			{
				alert("orbs-label", "remove the orbs label", this);
				goto endOfRemoveOrbs;
			}
		}

	endOfRemoveOrbs:

		return;
	}
};
