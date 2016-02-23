#include "BreakoutGameScene.h"
#include <iostream>
#include <iomanip>

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

	//init values
	xVel = 0;
	yVel = -5;
	paddleSpeed = 7;
	paddleBallAngleCF = 8;
	hitStreak = 0;
	scoreMultiplier = 1;
	score = 0;
	gameHold = true;

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
	labelScore = Label::createWithSystemFont(ss.str(), "Bebas Neue", 25);
	labelScore->setAnchorPoint(Vec2(0, 0));
	labelScore->setColor(Color3B(255, 255, 255));
	labelScore->setPosition(Vec2(700, 15));
	this->addChild(labelScore, 500);

	//draw multiplier
	std::stringstream ss2;
	ss2 << "x" << scoreMultiplier;
	labelMultiplier = Label::createWithSystemFont(ss2.str(), "Titillium-Bold", 25);
	labelMultiplier->setAnchorPoint(Vec2(1, 0));
	labelMultiplier->setColor(Color3B(102, 204, 255));
	labelMultiplier->setPosition(Vec2(685, 15));
	this->addChild(labelMultiplier, 500);

	//dar scr bar
	scoreBar = Sprite::create("scoreBar.png");
	scoreBar->setPosition(690, 18);
	scoreBar->setAnchorPoint(Vec2(0, 0));
	this->addChild(scoreBar, 500);

	//draw center message
	std::stringstream ss3;
	ss3 << "Press Enter To Play";
	labelScore = Label::createWithSystemFont(ss3.str(), "Bebas Neue", 35);
	labelScore->setAnchorPoint(Vec2(0.5, 0.5));
	labelScore->setColor(Color3B(255, 255, 255));
	labelScore->setPosition(Vec2(400, 300));
	this->addChild(labelScore, 500);

	//keyboard listener
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(BreakoutGameScene::keyPressed, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyReleased = CC_CALLBACK_2(BreakoutGameScene::keyReleased, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);



	this->schedule(schedule_selector(BreakoutGameScene::update), 1.0f / 10);

	return true;
}

void BreakoutGameScene::update(float delta) {
	//CCRect collObjBox = wallX->getBoundingBox();
	//CCRect box2Rect = sprite2->getBoundingBox();

	bool collWithBlock = false;

	CCRect ballBox = ball->getBoundingBox();
	CCRect collObjBox;
	
	int i;

	if (gameHold) {
		return;
	}

	// Check ball collision with walls
	if(ball->getPositionX() > 790 && xVel >= 0){
		xVel = xVel * -1;
	}

	if (ball->getPositionX() < 10 && xVel < 0) {
		xVel = xVel * -1;
	}

	if (ball->getPositionY() > 590 && yVel >= 0) {
		yVel = yVel * -1;
	}

	if (ball->getPositionY() < 0 && yVel < 0) {
		yVel = yVel * -1;
	}

	// Check collision with blocks
	for (i = 0; i < blocks.size(); i++) {
		CCRect ballBox = ball->getBoundingBox();
		CCRect collObjBox = blocks[i]->getBoundingBox();

		if (collObjBox.intersectsRect(ballBox) && !collWithBlock)
		{
			//adjustTrajectory(ballBox, collObjBox);
			log("Collission");

			if (collObjBox.getMinY() <= ballBox.getMaxY() && collObjBox.getMinX() - 4 <= ballBox.getMidX() && collObjBox.getMaxX() + 4 >= ballBox.getMidX() || xVel == 0) {
					yVel = yVel * -1;
			}
			// below
			else if (collObjBox.getMaxY() >= ballBox.getMinY() && collObjBox.getMinX() - 4 <= ballBox.getMidX() && collObjBox.getMaxX() + 4 >= ballBox.getMidX() || xVel == 0) {
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

			// Delete block -- add score logic here
			
			blocks[i]->removeFromParentAndCleanup(true);
			blocks.erase(blocks.begin() + i);
			//i = i - 1;

			//check streak and score multiplier stuff
			hitStreak += 1;

			if (hitStreak == 2) {
				scoreMultiplier = 2;
			}
			else if (hitStreak == 4) {
				scoreMultiplier = 3;
			}
			else if (hitStreak == 8) {
				scoreMultiplier = 4;
			}
			else if (hitStreak == 16) {
				scoreMultiplier = 5;
			}
			else if (hitStreak == 32) {
				scoreMultiplier = 6;
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

		}

	}

	// Check collision with paddle - paddle collision has its own rules
	collObjBox = paddle->getBoundingBox();
	if (collObjBox.intersectsRect(ballBox))
	{
		yVel = yVel * -1;
		xVel = (ballBox.getMidX() - collObjBox.getMidX())/paddleBallAngleCF ;

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

	}

	// Set updated ball position based on calculated velocity
	ball->setPosition(ball->getPositionX() + xVel, ball->getPositionY() + yVel);


	// Set position of paddle
	if (leftDown) {
		paddle->setPosition(paddle->getPositionX() - paddleSpeed, paddle->getPositionY());
		bg2->setPosition(bg2->getPositionX() + 1, bg2->getPositionY());
	}
	else if (rightDown) {
		paddle->setPosition(paddle->getPositionX() + paddleSpeed, paddle->getPositionY());
		bg2->setPosition(bg2->getPositionX() - 1, bg2->getPositionY());
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
	else if (keyCode == EventKeyboard::KeyCode::KEY_Q)
	{
		this->schedule(schedule_selector(BreakoutGameScene::update), 1.0f / 60);
		//log("%lf, %lf", ball->getPositionX(), ball->getPositionY());

		slow = false;
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_W)
	{
		this->schedule(schedule_selector(BreakoutGameScene::update), 1.0f / 10);
		

		slow = true;
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_E)
	{
		this->schedule(schedule_selector(BreakoutGameScene::update), 1.0f / 0.000000000001);


		slow = true;
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_ENTER && gameHold)
	{
		gameHold = false;
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