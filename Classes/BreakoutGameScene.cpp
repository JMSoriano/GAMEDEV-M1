#include "BreakoutGameScene.h"
#include <iostream>
#include <iomanip>
#include "SimpleAudioEngine.h" 
#include <cstdio>
#include <string>
#include <stdlib.h>
#include <MenuScene.h>

USING_NS_CC;

Scene* BreakoutGameScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::createWithPhysics();
	auto layer = BreakoutGameScene::create();

	scene->addChild(layer);

	return scene;
}

bool BreakoutGameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	//init audio
	auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
	audio->preloadEffect("blockHit1.wav");
	audio->preloadEffect("ballDie.wav");
	audio->preloadEffect("paddleHit.wav");
	audio->preloadEffect("blockHit2.wav");
	audio->preloadEffect("wallDeflect1.wav");
	audio->preloadEffect("ready.wav");
	audio->preloadEffect("streak.wav");
	audio->preloadEffect("GameOverBeep.wav");
	audio->preloadEffect("extraLife.wav");

	//init values
	xVel = 0;
	yVel = -5;
	paddleSpeed = 7;
	paddleBallAngleCF = 8;
	hitStreak = 0;
	scoreMultiplier = 1;
	score = 0;
	gameHold = true;
	lives = 2;
	holdTime = 180;
	currHoldTime = -1;
	gameOver = false;

	//create ball
	ball = Sprite::create("ball.png");
	ball->setPosition(400,150);
	addChild(ball, 100);

	//create blocks
	int i;
	int j;

	int horSpace = 38; //38 //43
	int verSpace = 22; //22  //27
	int start = 60;
	int vStart = 500;
	// 5 18
	for (j = 0; j < 5; j++) {
		for (i = 0; i < 18; i++) {
			blocks.push_back(Sprite::create("blockRed.png"));
			blocks.back()->setAnchorPoint(Vec2(0, 0));
			blocks.back()->setPosition(start + i * horSpace, vStart - verSpace * j);
			this->addChild(blocks.back(), 100);
		}
	}

	//create paddle
	paddle = Sprite::create("paddle.png");
	paddle->setPosition(400, 60);
	this->addChild(paddle, 50);

	//create BG
	bg = Sprite::create("breakoutBG.png");
	bg->setPosition(0, 0);
	bg->setAnchorPoint(Vec2(0, 0));
	this->addChild(bg, -100);

	bg2 = Sprite::create("breakoutBG2.png");
	bg2->setPosition(40, 20);
	bg2->setAnchorPoint(Vec2(0, 0));
	this->addChild(bg2, -80);

	//draw scr
	std::stringstream ss;
	ss << std::setw(7) << std::setfill('0') << score;
	labelScore = Label::createWithTTF(ss.str(), "fonts/BebasNeue.otf", 25);
	labelScore->setAnchorPoint(Vec2(0, 0));
	labelScore->setColor(Color3B(255, 255, 255));
	labelScore->setPosition(Vec2(700, 15));
	this->addChild(labelScore, 500);

	//draw multiplier
	std::stringstream ss2;
	ss2 << "x" << scoreMultiplier;
	labelMultiplier = Label::createWithTTF(ss2.str(), "fonts/Titillium-Bold.otf", 25);
	labelMultiplier->setAnchorPoint(Vec2(1, 0));
	labelMultiplier->setColor(Color3B(102, 204, 255));
	labelMultiplier->setPosition(Vec2(683, 12));
	this->addChild(labelMultiplier, 500);

	//dar scr bar
	scoreBar = Sprite::create("scoreBar.png");
	scoreBar->setPosition(690, 18);
	scoreBar->setAnchorPoint(Vec2(0, 0));
	this->addChild(scoreBar, 500);

	//draw lives
	std::stringstream ss4;
	ss4 << lives;
	labelLives = Label::createWithTTF(ss4.str(), "fonts/BebasNeue.otf", 35);
	labelLives->setAnchorPoint(Vec2(0, 0));
	labelLives->setColor(Color3B(255, 255, 255));
	labelLives->setPosition(Vec2(70, 12));
	this->addChild(labelLives, 500);

	//dar lives icon
	livesIcon = Sprite::create("livesIcon.png");
	livesIcon->setPosition(45, 25);
	livesIcon->setAnchorPoint(Vec2(0, 0));
	this->addChild(livesIcon, 500);

	//draw center message
	std::stringstream ss3;
	ss3 << "Press [Enter] To Play";
	labelCenterMessage = Label::createWithTTF(ss3.str(), "fonts/BebasNeue.otf", 35);
	labelCenterMessage->setAnchorPoint(Vec2(0.5, 0.5));
	labelCenterMessage->setColor(Color3B(255, 255, 255));
	labelCenterMessage->setPosition(Vec2(400, 300));
	this->addChild(labelCenterMessage, 500);

	//draw gameOver score
	std::stringstream ss5;
	ss5 << "";
	labelGameOverScore = Label::createWithTTF(ss5.str(), "fonts/Titillium-Bold.otf", 20);
	labelGameOverScore->setAnchorPoint(Vec2(0.5, 0.5));
	labelGameOverScore->setColor(Color3B(255, 255, 255));
	labelGameOverScore->setPosition(Vec2(400, 265));
	this->addChild(labelGameOverScore, 500);

	//draw gameOver message
	std::stringstream ss6;
	ss6 << "";
	labelGameOverMessage = Label::createWithTTF(ss6.str(), "fonts/Titillium-Bold.otf", 20);
	labelGameOverMessage->setAnchorPoint(Vec2(0.5, 0.5));
	labelGameOverMessage->setColor(Color3B(255, 255, 255));
	labelGameOverMessage->setPosition(Vec2(400, 240));
	this->addChild(labelGameOverMessage, 500);

	//keyboard listener
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(BreakoutGameScene::keyPressed, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyReleased = CC_CALLBACK_2(BreakoutGameScene::keyReleased, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);



	this->schedule(schedule_selector(BreakoutGameScene::update), 1.0f / 60);

	return true;
}

void BreakoutGameScene::update(float delta) {
	//CCRect collObjBox = wallX->getBoundingBox();
	//CCRect box2Rect = sprite2->getBoundingBox();

	bool collWithBlock = false;

	CCRect ballBox = ball->getBoundingBox();
	CCRect collObjBox;
	
	int i;

	if (currHoldTime > 0) {
		currHoldTime -= 1;
	}
	else if (currHoldTime == 0) {

		if (blocks.size() == 0 && !gameOver) {

			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("extraLife.wav");

			gameOver = true;
			labelCenterMessage->setString("You Win");
			std::stringstream ss;
			ss << "Score: " << score;
			labelGameOverScore->setString(ss.str());
			labelGameOverMessage->setString("Play again? [Y/N]");
		}
		else if (lives < 0 && !gameOver) {

			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("GameOverBeep.wav");

			gameOver = true;
			labelCenterMessage->setString("Game Over");
			std::stringstream ss;
			ss << "Score: " << score;
			labelGameOverScore->setString(ss.str());
			labelGameOverMessage->setString("Play again? [Y/N]");
		}
		else if(!gameOver) {
			//Reset game
			labelCenterMessage->setString("Press [Enter] to Drop ball");
			paddle->setPosition(400, 60);
			ball->setPosition(400, 150);
			xVel = 0;
			yVel = -5;

			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("ready.wav");

			currHoldTime = -1;
		}
	}

	if (gameHold) {
		return;
	}

	// Check ball collision with walls
	if(ball->getPositionX() > 790 && xVel >= 0){
		xVel = xVel * -1;
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("wallDeflect1.wav");
	}

	if (ball->getPositionX() < 10 && xVel < 0) {
		xVel = xVel * -1;
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("wallDeflect1.wav");
	}

	if (ball->getPositionY() > 590 && yVel >= 0) {
		yVel = yVel * -1;
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("wallDeflect1.wav");
	}

	//if (ball->getPositionY() < 0 && yVel < 0) {
	//	yVel = yVel * -1;
	//}

	// player dies
	if (ball->getPositionY() < 0 && yVel < 0) {
		//yVel = yVel * -1;

		//player death logic here

		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("ballDie.wav");

		//update lives
		lives = lives - 1;

		//update lives label
		std::stringstream ss4;
		if (lives >= 0) {
			ss4 << lives;
		}
		else {
			ss4 << lives + 1;
		}
		labelLives->setString(ss4.str());

		//reset game
		gameHold = true;
		currHoldTime = holdTime;
	}

	// Check collision with blocks
	for (i = 0; i < blocks.size(); i++) {
		CCRect ballBox = ball->getBoundingBox();
		CCRect collObjBox = blocks[i]->getBoundingBox();

		if (collObjBox.intersectsRect(ballBox) && !collWithBlock)
		{

			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("blockHit2.wav");

			//adjustTrajectory(ballBox, collObjBox);
			log("Collission");

			if (collObjBox.getMinY() <= ballBox.getMaxY() && collObjBox.getMinX() - 4 <= ballBox.getMidX() && collObjBox.getMaxX() + 4 >= ballBox.getMidX() && yVel > 0 || xVel == 0) {
					yVel = yVel * -1;
			}
			// below
			else if (collObjBox.getMaxY() >= ballBox.getMinY() && collObjBox.getMinX() - 4 <= ballBox.getMidX() && collObjBox.getMaxX() + 4 >= ballBox.getMidX() && yVel < 0 || xVel == 0) {
					yVel = yVel * -1;
			}
			// left
			else if (collObjBox.getMaxX() >= ballBox.getMinX() && xVel < 0) {
					xVel = xVel * -1;
			}
			// right
			else if (collObjBox.getMinX() <= ballBox.getMaxX() && xVel >= 0) {
					xVel = xVel * -1;
			}
			else {
				log("unknown collision");
			}

			// Delete block -- add score logic here
			
			blocks[i]->removeFromParentAndCleanup(true);
			blocks.erase(blocks.begin() + i);
			//i = i - 1;

			//check streak and score multiplier stuff
			hitStreak += 1;

			if (hitStreak == 2) {
				scoreMultiplier = 2;
				CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("streak.wav");
			}
			else if (hitStreak == 4) {
				scoreMultiplier = 3;
				CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("streak.wav");
			}
			else if (hitStreak == 8) {
				scoreMultiplier = 4;
				CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("streak.wav");
			}
			else if (hitStreak == 16) {
				scoreMultiplier = 5;
				CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("streak.wav");
			}
			else if (hitStreak == 32) {
				scoreMultiplier = 6;
				CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("streak.wav");
			}

			score = score + 100 * scoreMultiplier;

			//update score label
			std::stringstream ss;
			ss << std::setw(7) << std::setfill('0') << score;
			labelScore->setString(ss.str());

			//update multiplier label
			std::stringstream ss2;
			ss2 << "x" << scoreMultiplier;
			labelMultiplier->setString(ss2.str());

			collWithBlock = true;

			// no blocks left so player won
			if (blocks.size() == 0){
				gameHold = true;
				currHoldTime = holdTime;
			}

		}

	}

	// Check collision with paddle - paddle collision has its own rules
	collObjBox = paddle->getBoundingBox();
	if (collObjBox.intersectsRect(ballBox))
	{

		

		if (collObjBox.getMidY() > ballBox.getMinY() && yVel < 0 && !(ballBox.getMaxX() < collObjBox.getMinX() || ballBox.getMinX() > collObjBox.getMaxX())) {
			yVel = yVel * -1;
			xVel = (ballBox.getMidX() - collObjBox.getMidX()) / paddleBallAngleCF;

			//Reset hit streak and multiplier
			hitStreak = 0;
			scoreMultiplier = 1;

			//update score label
			std::stringstream ss;
			ss << std::setw(7) << std::setfill('0') << score;
			labelScore->setString(ss.str());

			//update multiplier label
			std::stringstream ss2;
			ss2 << "x" << scoreMultiplier;
			labelMultiplier->setString(ss2.str());

			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("paddleHit.wav");
		}
		//otherwise the ball fell below the paddle and is beyond hope

	}

	// Set updated ball position based on calculated velocity
	ball->setPosition(ball->getPositionX() + xVel, ball->getPositionY() + yVel);


	// Set position of paddle
	if (leftDown) {
		if (paddle->getPositionX() > 50) {
			paddle->setPosition(paddle->getPositionX() - paddleSpeed, paddle->getPositionY());
			bg2->setPosition(bg2->getPositionX() + 1, bg2->getPositionY());
		}
		
	}
	else if (rightDown) {
		if (paddle->getPositionX() < 750) {
			paddle->setPosition(paddle->getPositionX() + paddleSpeed, paddle->getPositionY());
			bg2->setPosition(bg2->getPositionX() - 1, bg2->getPositionY());
		}
	}
	//else {
	//	paddle->setPosition(ball->getPositionX() + 10, paddle->getPositionY());
	//}

	if (slow) {
		log("%lf, %lf", ball->getPositionX(), ball->getPositionY());
	}

}

void BreakoutGameScene::keyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
	{
		//paddle->setPosition(paddle->getPositionX() + 10, paddle->getPositionY());
		rightDown = true;
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
	{
		//paddle->setPosition(paddle->getPositionX() - 10, paddle->getPositionY());
		leftDown = true;
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_ENTER && gameHold && currHoldTime == -1 && !gameOver)
	{
		gameHold = false;
		labelCenterMessage->setString("");
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_Y && gameOver)
	{
		auto newScene = BreakoutGameScene::createScene();
		Director::getInstance()->replaceScene(newScene);
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_N && gameOver)
	{
		auto newScene = MenuScene::createScene();
		Director::getInstance()->replaceScene(newScene);
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
	{
		auto newScene = MenuScene::createScene();
		Director::getInstance()->replaceScene(newScene);
	}
	else {
		
	}
}

void BreakoutGameScene::keyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
	{
		//paddle->setPosition(paddle->getPositionX() + 10, paddle->getPositionY());
		rightDown = false;
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
	{
		//paddle->setPosition(paddle->getPositionX() - 10, paddle->getPositionY());
		leftDown = false;
	}
	else {

	}
}

/*auto newScene = GameScene::createScene();
Director::getInstance()->replaceScene(newScene);*/

// Check collision with walls
//for (i = 0; i < walls.size(); i++) {
//	CCRect ballBox = ball->getBoundingBox();
//	CCRect collObjBox = walls[i]->getBoundingBox();

//	if (collObjBox.intersectsRect(ballBox))
//	{
//		if (collObjBox.intersectsRect(ballBox))
//		{
//			//adjustTrajectory(ballBox, collObjBox);

//			if (collObjBox.getMinY() <= ballBox.getMaxY()) {
//				yVel = yVel * -1;
//			}
//			// below
//			else if (collObjBox.getMaxY() >= ballBox.getMinY()) {
//				yVel = yVel * -1;
//			}
//			// left
//			else if (collObjBox.getMinX() <= ballBox.getMaxX()) {
//				xVel = xVel * -1;
//			}
//			// right
//			else if (collObjBox.getMaxX() >= ballBox.getMinX()) {
//				xVel = xVel * -1;
//			}


//		}
//		
//	}

//Create walls
//walls.push_back(Sprite::create("wallX.png"));
//walls.back()->setAnchorPoint(Vec2(0, 0));
//walls.back()->setPosition(0, 600);
//this->addChild(walls.back(), 100);

//walls.push_back(Sprite::create("wallY.png"));
//walls.back()->setAnchorPoint(Vec2(0, 0));
//walls.back()->setPosition(0, 0);
//this->addChild(walls.back(), 100);

//walls.push_back(Sprite::create("wallY.png"));
//walls.back()->setAnchorPoint(Vec2(0, 0));
//walls.back()->setPosition(800, 0);
//this->addChild(walls.back(), 100);

// Adjusts ball trajectory based on collision with blocks and walls
//void BreakoutGameScene::adjustTrajectory(CCRect ballBox, CCRect collObjBox) {
//	//If collided object is above
//	if (collObjBox.getMinY() <= ballBox.getMaxY()) {
//		yVel = yVel * -1;
//	}
//	// below
//	else if (collObjBox.getMaxY() >= ballBox.getMinY()) {
//		yVel = yVel * -1;
//	}
//	// left
//	else if (collObjBox.getMinX() <= ballBox.getMaxX()) {
//		xVel = xVel * -1;
//	}
//	// right
//	else if (collObjBox.getMaxX() >= ballBox.getMinX()) {
//		xVel = xVel * -1;
//	}
//}

//else if (keyCode == EventKeyboard::KeyCode::KEY_Q)
//{
//	this->schedule(schedule_selector(BreakoutGameScene::update), 1.0f / 60);
//	//log("%lf, %lf", ball->getPositionX(), ball->getPositionY());

//	slow = false;
//}
//else if (keyCode == EventKeyboard::KeyCode::KEY_W)
//{
//	this->schedule(schedule_selector(BreakoutGameScene::update), 1.0f / 10);
//	

//	slow = true;
//}
//else if (keyCode == EventKeyboard::KeyCode::KEY_E)
//{
//	this->schedule(schedule_selector(BreakoutGameScene::update), 1.0f / 0.000000000001);


//	slow = true;
//}