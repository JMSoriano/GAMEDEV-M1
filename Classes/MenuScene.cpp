#include "MenuScene.h"
#include "BreakoutGameScene.h"
#include "SimpleAudioEngine.h" 


USING_NS_CC;

Scene* MenuScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();
	auto layer = MenuScene::create();

	scene->addChild(layer);

	return scene;
}

bool MenuScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
	audio->preloadEffect("menuMusic.wav");

	/*auto label = Label::createWithSystemFont("Hello World", "Arial", 96);
	label->setAnchorPoint(cocos2d::Vec2(0.0, 0.0));
	this->addChild(label, 1);*/

	//draw BG
	menu = Sprite::create("Title.png");
	menu->setAnchorPoint(Vec2(0, 0));
	menu->setPosition(0, 0);
	this->addChild(menu, -100);

	//keyboard listener
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(MenuScene::keyPressed, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("menuMusic.mp3");

	return true;
}

void MenuScene::keyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ENTER)
	{
		auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
		audio->stopAllEffects();
		auto newScene = BreakoutGameScene::createScene();
		Director::getInstance()->replaceScene(newScene);
	}
}