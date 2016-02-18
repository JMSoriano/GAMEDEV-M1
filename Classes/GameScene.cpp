#include "GameScene.h"
#include "MenuScene.h"
#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <Windows.h>
#include "SimpleAudioEngine.h"  

USING_NS_CC;

Scene* GameScene::createScene()
{
	auto scene = Scene::create();
	auto layer = GameScene::create();
	scene->addChild(layer);

	return scene;
}

bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
	audio->preloadEffect("Die.wav");
	audio->preloadEffect("eat.wav");
	audio->preloadEffect("GameOverBeep.wav");
	audio->preloadEffect("snakeMove.wav");

	snakeDead = false;
	deadScreenDisplayed = false;
	snkDirection = "right";
	snkDirectionCurr = "right";
	timePassed = 0;
	snakeUpdateTime = 400; //number of mills before snake moves changes overtime

	//timeInMillis = utils::getTimeInMilliseconds();
	int i;

	//draw BG
	bg = Sprite::create("SnakeBG.png");
	bg->setAnchorPoint(Vec2(0, 0));
	bg->setPosition(0, 0);
	this->addChild(bg, -100);

	//draw scrBG
	scrBG = Sprite::create("scrBG.png");
	scrBG->setAnchorPoint(Vec2(0, 0));
	scrBG->setPosition(28*20 + 5, 25);
	this->addChild(scrBG, -50);

	//draw scr
	score = 0;
	std::stringstream ss;
	ss << "Score: " << score;
	labelTouchInfo = Label::createWithSystemFont(ss.str(), "Minecraftia", 18);
	labelTouchInfo->setAnchorPoint(Vec2(0, 0));
	labelTouchInfo->setColor(Color3B(13, 13, 13));
	labelTouchInfo->setPosition(Vec2(30*20, 33));

	this->addChild(labelTouchInfo);

	//draw walls left vertical
	for (i = 4; i <= 28; i++) {
		walls.push_back(Sprite::create("wall.png"));
		walls.back()->setAnchorPoint(Vec2(0, 0));
		walls.back()->setPosition(1*20, i * 20);
		this->addChild(walls.back(), 100);
	}

	//draw walls right vertical
	for (i = 4; i <= 28; i++) {
		walls.push_back(Sprite::create("wall.png"));
		walls.back()->setAnchorPoint(Vec2(0, 0));
		walls.back()->setPosition(38*20, i * 20);
		this->addChild(walls.back(), 100);
	}

	//draw walls top horizontal
	for (i = 2; i <= 37; i++) {
		walls.push_back(Sprite::create("wall.png"));
		walls.back()->setAnchorPoint(Vec2(0, 0));
		walls.back()->setPosition(i * 20, 4 * 20);
		this->addChild(walls.back(), 100);
	}

	//draw walls bottom horizontal
	for (i = 2; i <= 37; i++) {
		walls.push_back(Sprite::create("wall.png"));
		walls.back()->setAnchorPoint(Vec2(0, 0));
		walls.back()->setPosition(i * 20, 28 * 20);
		this->addChild(walls.back(), 100);
	}

	//initialize snake
	for (i = 7; i < 11; i++) {
		snake.push(Sprite::create("SnakeBody.png"));
		snake.back()->setAnchorPoint(Vec2(0, 0));
		snake.back()->setPosition(i * 20, 14 * 20);
		this->addChild(snake.back(), 200);
	}

	//initialize food
	food = Sprite::create("SnakeFood.png");
	food->setAnchorPoint(Vec2(0, 0));
	food->setPosition(22 * 20, 14 * 20);
	this->addChild(food, 300);


	this->schedule(schedule_selector(GameScene::update), 1.0f / 30);


	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::keyPressed, this);
	//keyboardListener->onKeyReleased = CC_CALLBACK_2(GraphicsScene::keyReleased, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	return true;
}


void GameScene::update(float delta)
{
	timePassed = timePassed + delta*1000;

	if (!snakeDead && snakeUpdateTime - timePassed <= 0) {

		//reset timer
		timePassed = 0;

		//--------------------
		//Snake movement
		//--------------------
		auto position = snake.back()->getPosition();

		//CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
		//	"snakeMove.wav");

		snkDirectionCurr = snkDirection;

		if (snkDirection == "up") {
			snake.push(Sprite::create("SnakeBody.png"));
			snake.back()->setPosition(position.x, position.y + 20);
		}
		else if (snkDirection == "down") {
			snake.push(Sprite::create("SnakeBody.png"));
			snake.back()->setPosition(position.x, position.y - 20);
		}
		else if (snkDirection == "left") {
			snake.push(Sprite::create("SnakeBody.png"));
			snake.back()->setPosition(position.x - 20, position.y);
		}
		else if (snkDirection == "right") {
			snake.push(Sprite::create("SnakeBody.png"));
			snake.back()->setPosition(position.x + 20, position.y);
		}

		snake.back()->setAnchorPoint(Vec2(0, 0));
		this->addChild(snake.back(), 200);

		position = snake.back()->getPosition();

		//--------------------
		//Food mechanics/snake growth
		//--------------------
		auto positionFood = food->getPosition();

		int newX;
		int newY;
		int i;
		bool badCoords;
		int debugT1, debugT2;
		std::queue<cocos2d::Sprite*> tempSnake;

		if (position == positionFood) {

			//play sound
			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"eat.wav");

			//update score
			score = score + 2 * snake.size();
			std::stringstream ss;
			ss << "Score: " << score;
			labelTouchInfo->setString(ss.str());

			srand(time(NULL));

			//food grid positions
			//x 3-36
			//y 5-27

			badCoords = true;
			
			while (badCoords) {
				badCoords = false;

				newX = rand() % 33 + 3;
				newY = rand() % 23 + 5;

				//turns out queue cannot iterate through elements -- hassle to change datastructure so heres a workaround
				tempSnake = snake;

				//run check to see if food will spawn over snake - generate new coords if they're bad
				for (i = 0; i < snake.size(); i++) {

					debugT1 = tempSnake.front()->getPosition().x / 20;
					debugT2 = tempSnake.front()->getPosition().y / 20;
					//	OutputDebugString("hello \r\n");

					if (tempSnake.front()->getPosition().x / 20 == newX && tempSnake.front()->getPosition().y / 20 == newY) {
						badCoords = true;
					}
					tempSnake.pop();
				}
			}

			food->setPosition(newX * 20, newY * 20);

			//speed up snake upon eating
			//this->schedule(schedule_selector(GraphicsScene::update), 1.0f / (7 + snake.size() - 3));
			snakeUpdateTime -= 25;
			if (snakeUpdateTime < 25) {
				snakeUpdateTime = 25;
			}
		}
		else {
			snake.front()->removeFromParentAndCleanup(true);
			snake.pop();
		}

		//--------------------
		//Death detection
		//--------------------

		cocos2d::Sprite* tempSpr;
		//check if head colided with walls
		for (i = 0; i < walls.size(); i++) {

			if (walls[i]->getPosition().x == snake.back()->getPosition().x && walls[i]->getPosition().y == snake.back()->getPosition().y) {
				//Snake colided with wall
				CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
					"Die.wav");
				snakeDead = true;
			}
		}

		tempSnake = snake;

		//check if snake collided with self
		for (i = 0; i < snake.size() - 1; i++) {
			if (tempSnake.front()->getPosition().x == snake.back()->getPositionX() && tempSnake.front()->getPosition().y == snake.back()->getPositionY()) {
				CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
					"Die.wav");
				snakeDead = true;
			}
			tempSnake.pop();
		}

		if (snakeDead) {
			tod = time(0);
			snake.back()->setColor(Color3B(145, 13, 13));
		}
	}
	else if(snakeDead) {
		//gameOver sequence
		//auto newScene =  GraphicsScene::createScene();
		//Director::getInstance()->replaceScene(newScene);

		time_t t = time(0);

		if (tod + 1 < t && !deadScreenDisplayed) {
			//display game over screen
			//draw BG
			deadSign = Sprite::create("GameOver.png");
			deadSign->setAnchorPoint(Vec2(0.5, 0.5));
			deadSign->setPosition(400, 300);
			this->addChild(deadSign, 400);
			//play sound
			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(
				"GameOverBeep.wav");
			deadScreenDisplayed = true;
		}

	}
}

void GameScene::keyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
	{
		if (snkDirectionCurr != "down") {
			snkDirection = "up";
		}
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
		if (snkDirectionCurr != "right") {
			snkDirection = "left";
		}
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
		if (snkDirectionCurr != "up") {
			snkDirection = "down";
		}
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
		if (snkDirectionCurr != "left") {
			snkDirection = "right";
		}
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_Y) {
		if (snakeDead) {
			auto newScene =  GameScene::createScene();
			Director::getInstance()->replaceScene(newScene);
		}
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_N) {
		if (snakeDead) {
			auto newScene = MenuScene::createScene();
			Director::getInstance()->replaceScene(newScene);
		}
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
		auto newScene = MenuScene::createScene();
		Director::getInstance()->replaceScene(newScene);
	}
}
