#include "Toolbox.h"

static sf::Vector2i mMapDimensions;
static std::string mLevelDebug = "resources/Debug.dat";
static std::string mLevel1     = "resources/Level1.dat";
static std::string mLevel2     = "resources/Level2.dat";
static std::string mLevel3     = "resources/Level3.dat";
static std::string mLevel4     = "resources/Level4.dat";
static std::string mLevel5     = "resources/Level5.dat";
static std::string mLevel6     = "resources/Level6.dat";
static std::string mLevel7     = "resources/Level7.dat";
static std::string mLevel8     = "resources/Level8.dat";
static std::string mLevel9     = "resources/Level9.dat";
static sf::RenderWindow* mWindow;
static sf::Time mTime;
static sf::Vector2f mBlockSize;

static std::string mFontFile = "resources/Freeroad Regular.ttf";
static sf::Font mFont;

static sf::Texture mTextureBuilding;
static sf::Texture mTextureWall;
static sf::Texture mTextureAgent;
static sf::Texture mTextureConfirmed;
static std::string mTextureFileBuilding = "resources/hus.png";
static std::string mTextureFileWall = "resources/vagg.png";
static std::string mTextureFileAgent = "resources/gubbe.png";
static std::string mTextureFileConfirmed = "resources/confirmed.png";

static std::atomic<bool> mTerminateSimulation = false;
static std::atomic<bool> mDynamicOrStatic = true; // True = dynamic, false = static
static std::atomic<bool> mRenderWeights = false;
static std::atomic<bool> mRenderClosestPoints = false;
static std::atomic<bool> mRenderFlow = false;
static std::atomic<bool> mRenderConfirmed = false;
static std::atomic<bool> mRenderTexts = true;

static std::atomic<bool> mIsFinished = false;
static std::atomic<bool> mIsFrameRateLocked = true;
static std::atomic<bool> mIsBuildingKilled = false;

static float const PI = 3.1415926535897932384f;

Toolbox* Toolbox::instance()
{
	static Toolbox instance;
	return &instance;
}

std::string Toolbox::getLevel(LevelCode code)
{
	switch (code)
	{
	case Toolbox::LevelDebug:
		return mLevelDebug;
		break;
	case Toolbox::Level1:
		return mLevel1;
		break;
	case Toolbox::Level2:
		return mLevel2;
		break;
	case Toolbox::Level3:
		return mLevel3;
		break;
	case Toolbox::Level4:
		return mLevel4;
		break;
	case Toolbox::Level5:
		return mLevel5;
		break;
	case Toolbox::Level6:
		return mLevel6;
		break;
	case Toolbox::Level7:
		return mLevel7;
		break;
	case Toolbox::Level8:
		return mLevel8;
		break;
	case Toolbox::Level9:
		return mLevel9;
		break;
	default:
		return mLevelDebug;
		break;
	}
}

Toolbox::Toolbox()
{
	// Load textures
	mFont.loadFromFile(mFontFile);
	mTextureBuilding.loadFromFile(mTextureFileBuilding);
	mTextureWall.loadFromFile(mTextureFileWall);
	mTextureAgent.loadFromFile(mTextureFileAgent);
	mTextureConfirmed.loadFromFile(mTextureFileConfirmed);
}


void Toolbox::setMapDimensions(sf::Vector2i dim)
{
	mMapDimensions = dim;
}

sf::Vector2i& Toolbox::getMapDimensions()
{
	return mMapDimensions;
}

sf::Vector2f& Toolbox::getMapBlockSize()
{
	mBlockSize = sf::Vector2f((float)(mWindow->getSize().x / mMapDimensions.x), (float)(mWindow->getSize().y / mMapDimensions.y));
	return mBlockSize;
}

sf::RenderWindow& Toolbox::getWindow()
{
	return *mWindow;
}

void Toolbox::setWindow(sf::RenderWindow* window)
{
	mWindow = window;
}

void Toolbox::setDeltaTime(sf::Time time)
{
	mTime = time;
}

sf::Time& Toolbox::getDeltaTime()
{
	return mTime;
}

bool Toolbox::getTerminateSimulation()
{
	return mTerminateSimulation;
}

void Toolbox::setTerminateSimulation(bool value)
{
	mTerminateSimulation = value;
}

sf::Font& Toolbox::getFont()
{
	return mFont;
}

sf::Texture& Toolbox::getTexture(TextureCode code)
{
	switch (code)
	{
	case Toolbox::BUILDING:
		return mTextureBuilding;
		break;
	case Toolbox::WALL:
		return mTextureWall;
		break;
	case Toolbox::AGENT:
		return mTextureAgent;
		break;
	case Toolbox::POLY:
		return mTextureConfirmed;
		break;
	default:
		return mTextureBuilding;
		break;
	}
}

std::string Toolbox::floatToString(float f)
{
	std::ostringstream stringTemp;
	stringTemp << f;
	return stringTemp.str();
}

std::string Toolbox::boolToString(bool value)
{
	return value ? "true" : "false";
}

// Convert mouse position to a coordinate on the 2D grid
sf::Vector2i Toolbox::globalToIndexCoords(const sf::Vector2f& pos)
{
	float xTileSize = getMapBlockSize().x;
	float yTileSize = getMapBlockSize().y;

	int localX = (int)(pos.x / xTileSize);
	int localY = (int)(pos.y / yTileSize);

	sf::Vector2i mouseIndex(localX, localY);
	return mouseIndex;
}

sf::Vector2f Toolbox::localToGlobalCoords(const sf::Vector2i& localPos)
{
	float globalX = localPos.x * getMapBlockSize().x;
	float globalY = localPos.y * getMapBlockSize().y;

	sf::Vector2f global(globalX, globalY);
	return global;
}

sf::Vector2f Toolbox::getMiddleOfBlock(const sf::Vector2f & globalPos)
{
	sf::Vector2f middle(globalPos.x + getMapBlockSize().x / 2, globalPos.y + getMapBlockSize().y / 2);
	return middle;
}

// External algorithm from https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
int Toolbox::pointInPoly(int nrOfVerts, std::vector<float>& vertX, std::vector<float>& vertY, float testX, float testY)
{
	int i, j, c = 0;
	for (i = 0, j = nrOfVerts - 1; i < nrOfVerts; j = i++) {
		if (((vertY[i]>testY) != (vertY[j]>testY)) &&
			(testX < (vertX[j] - vertX[i]) * (testY - vertY[i]) / (vertY[j] - vertY[i]) + vertX[i]))
			c = !c;
	}
	return c;
}

bool Toolbox::getGenerateDynamicFlow()
{
	return mDynamicOrStatic;
}

void Toolbox::toggleGenerateDynamicFlow()
{
	mDynamicOrStatic = !mDynamicOrStatic;
}

float Toolbox::getMagnitude(sf::Vector2f& vec)
{
	return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

float Toolbox::getDistance(sf::Vector2f& a, sf::Vector2f& b)
{
	sf::Vector2f between(a - b);
	return getMagnitude(between);
}

sf::Vector2f Toolbox::normalize(sf::Vector2f & vec)
{
	sf::Vector2f normalized(vec);
	float magnitude = getMagnitude(vec);
	if (magnitude > 0.0f)
	{
		normalized.x = vec.x / magnitude;
		normalized.y = vec.y / magnitude;
	}
	return normalized;
}

float Toolbox::getAngleBetween(sf::Vector2f & vecA, sf::Vector2f & vecB)
{
	float angle = std::atan2(getDet(vecA, vecB), getDot(vecA, vecB));
	return angle;
}

float Toolbox::getDot(sf::Vector2f & vecA, sf::Vector2f & vecB)
{
	return vecA.x * vecB.x + vecA.y * vecB.y;
}

float Toolbox::getDet(sf::Vector2f & vecA, sf::Vector2f & vecB)
{
	return vecA.x * vecB.y - vecA.y * vecB.x;
}

float Toolbox::cross(sf::Vector2f & a, sf::Vector2f & b, sf::Vector2f & point)
{
	return ((point.x - a.x) * (b.y - a.y)) - ((point.y - a.y) * (b.x - a.x));
}

bool Toolbox::getRenderWeights()
{
	return mRenderWeights;
}

void Toolbox::toggleRenderWeights()
{
	mRenderWeights = !mRenderWeights;
}

bool Toolbox::getRenderClosestPoints()
{
	return mRenderClosestPoints;
}

void Toolbox::toggleRenderClosestPoints()
{
	mRenderClosestPoints = !mRenderClosestPoints;
}

bool Toolbox::getRenderFlow()
{
	return mRenderFlow;
}

void Toolbox::toggleRenderFlow()
{
	mRenderFlow = !mRenderFlow;
}

bool Toolbox::getRenderConfirmed()
{
	return mRenderConfirmed;
}

void Toolbox::toggleRenderConfirmed()
{
	mRenderConfirmed = !mRenderConfirmed;
}

bool Toolbox::getRenderTexts()
{
	return mRenderTexts;
}

void Toolbox::toggleRenderTexts()
{
	mRenderTexts = !mRenderTexts;
}

bool Toolbox::isFinished()
{
	return mIsFinished;
}

void Toolbox::setIsFinished(bool value)
{
	mIsFinished = value;
}

bool Toolbox::isFrameRateLocked()
{
	return mIsFrameRateLocked;
}

void Toolbox::toggleFrameRateLocked()
{
	mIsFrameRateLocked = !mIsFrameRateLocked;
}

void Toolbox::setFrameRateLocked()
{
	mIsFrameRateLocked = true;
}

bool Toolbox::isBuildingKilled()
{
	return mIsBuildingKilled;
}

void Toolbox::setBuildingKilled(bool value)
{
	mIsBuildingKilled = value;
}

