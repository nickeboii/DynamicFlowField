#include "EntityManager.h"


EntityManager* EntityManager::instance()
{
	static EntityManager instance;
	return &instance;
}


EntityManager::EntityManager()
{
}


EntityManager::~EntityManager()
{
	for (auto it : mBuildings)
		delete it;

	for (auto it : mAgents)
		delete it;

	for (std::map<Point, Building*>::iterator it = mBuildingMap.begin(), next_it = mBuildingMap.begin(); it != mBuildingMap.end(); it = next_it)
	{
		next_it = it; ++next_it;
		mBuildingMap.erase(it);
	}

	mBuildings.clear();
	mAgents.clear();
	mBuildingMap.clear();
}

void EntityManager::createBuilding(int size, Toolbox::BuildingType type, sf::Vector2i pos)
{
	Building *building = new Building(size, type, pos);

	// Pair all coordinates in the region of the building to the building itself
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			Point p(std::make_pair(pos.x + i, pos.y + j));
			mBuildingMap.insert(std::make_pair(p, building));
		}
	}

	// Add building to vector
	mBuildings.push_back(building);

	/*BuildingRegion *region = new BuildingRegion(size, pos);

	// Map region to building
	mBuildingMap.emplace(region, building);

	// Add region and building to respective container
	mRegions.push_back(region);
	mBuildings.push_back(building);*/
}

void EntityManager::createAgent(sf::Vector2i startPos)
{
	//TODO:
}

void EntityManager::update()
{
	for (auto it : mBuildings)
		it->update();

	for (auto it : mAgents)
		it->update();
}

void EntityManager::render(sf::RenderWindow & window)
{
	for (auto it : mBuildings)
		it->render(window);

	for (auto it : mAgents)
		it->render(window);
}

// Search a point and return its building if it exists
Building * EntityManager::isBuilding(Point point)
{
	BuildingMap::iterator it = mBuildingMap.find(point);
	if (it != mBuildingMap.end())
		return it->second;
	else
		return nullptr;
}
