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
			// we DONT want to alert when it doesnt exist since it wont
			// exist for levels that havent been touched yet

			if (auto orbIcon = this->getChildByIDRecursive("orbs-icon"))
				orbIcon->setVisible(false);
			//else
			//{
			//	gdtutils::alert("orbs-icon", "remove the orbs label", this);
			//	goto endOfRemoveOrbs;
			//}

			if (auto orbLabel = this->getChildByIDRecursive("orbs-label"))
				orbLabel->setVisible(false);
			//else
			//{
			//	gdtutils::alert("orbs-label", "remove the orbs label", this);
			//	goto endOfRemoveOrbs;
			//}
		}

	endOfRemoveOrbs:

		return;
	}
};
