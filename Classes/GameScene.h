#pragma once
#include "cocos2d.h"
#include <queue>
#include <vector>
#include <string>
#include <ctime>

class GameScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(GameScene);

	void update(float) override;

private:
	cocos2d::Sprite* sprite;

	//std::queue<cocos2d::Sprite*> walls;

	//long timeInMillis = utils::getTimeInMilliseconds();

	std::vector<cocos2d::Sprite*> walls;
	std::queue<cocos2d::Sprite*> snake;
	cocos2d::Sprite* food;
	cocos2d::Sprite* bg;
	cocos2d::Sprite* scrBG;
	cocos2d::Sprite* deadSign;
	bool snakeDead;
	std::string snkDirection;
	std::string snkDirectionCurr;
	int score;
	float timePassed;
	float snakeUpdateTime;

	cocos2d::Label* labelTouchInfo;

	bool deadScreenDisplayed;
	time_t tod;

	void keyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);

};