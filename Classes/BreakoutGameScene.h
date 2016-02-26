#pragma once

#include "cocos2d.h"
#include <iostream>



class BreakoutGameScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();

	void keyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event * event);
	void keyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event * event);

	//void adjustTrajectory(CCRect a, CCRect b);

	void update(float) override;
	CREATE_FUNC(BreakoutGameScene);

private:
	cocos2d::Sprite* menu;
	cocos2d::CCRect box1Rect;

	//cocos2d::Sprite* sprite2;

	cocos2d::Sprite* ball;
	//cocos2d::Sprite* paddle;

	cocos2d::Sprite* paddle;

	cocos2d::Sprite* bg;
	cocos2d::Sprite* bg2;
	cocos2d::Sprite* scoreBar;
	cocos2d::Sprite* livesIcon;

	cocos2d::CCRect collObjBox;

	cocos2d::Label* labelScore;
	cocos2d::Label* labelMultiplier;

	cocos2d::Label* labelCenterMessage;

	cocos2d::Label* labelLives;
	cocos2d::Label* labelGameOverScore;
	cocos2d::Label* labelGameOverMessage;

	//cocos2d::Sprite* wallX;
	//cocos2d::Sprite* wallY1;
	//cocos2d::Sprite* wallY2;

	std::vector<cocos2d::Sprite*> walls;

	std::vector<cocos2d::Sprite*> blocks;

	int xVel;
	int yVel;
	int paddleSpeed;
	int paddleBallAngleCF;
	int score;
	int scoreMultiplier;
	int hitStreak;
	int lives;

	int holdTime;
	int currHoldTime;

	bool leftDown;
	bool rightDown;

	bool gameHold;

	bool slow;
	bool gameOver;
};