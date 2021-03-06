#pragma once
#include "StateBase.h"

class EditMap : public StateBase
{
public:
	static EditMap* instance();
	virtual void enter(sf::RenderWindow& window);
	virtual void update(Game* game);
	virtual void render();
	virtual void exit();
	virtual void propagateEvent(Game* game, sf::Event& event);
	virtual StateID getStateID();
private:
	EditMap();
	EditMap(const EditMap&);
	EditMap& operator=(const EditMap&);

	void changeScenario(sf::Event& event);

	sf::RenderWindow* mWindow;
	sf::Text* mStateText;
	sf::Text* mScenarioNrText;
	sf::Text* mCurrentBuildingText;
	sf::Text* mSizeText;
};

