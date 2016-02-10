#pragma once

#include "cocos2d.h"


class MenuScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();

	void keyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event * event);

	CREATE_FUNC(MenuScene);

private:
	cocos2d::Sprite* menu;
};