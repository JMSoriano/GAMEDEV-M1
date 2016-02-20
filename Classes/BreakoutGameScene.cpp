#include "BreakoutGameScene.h"

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

	//create ball
	ball = Sprite::create("ball.png");
	ball->setPosition(400,300);
	addChild(ball, 100);

	//create blocks
	int i;
	int j;

	int horSpace = 38;
	int verSpace = 22;
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
	addChild(paddle, 50);

	//create BG
	bg = Sprite::create("breakoutBG.png");
	bg->setPosition(0, 0);
	bg->setAnchorPoint(Vec2(0, 0));
	addChild(bg, -100);

	bg2 = Sprite::create("breakoutBG2.png");
	bg2->setPosition(40, 20);
	bg2->setAnchorPoint(Vec2(0, 0));
	addChild(bg2, -80);

	//Create walls
	walls.push_back(Sprite::create("wallX.png"));
	walls.back()->setAnchorPoint(Vec2(0, 0));
	walls.back()->setPosition(0, 600);
	this->addChild(walls.back(), 100);

	walls.push_back(Sprite::create("wallY.png"));
	walls.back()->setAnchorPoint(Vec2(0, 0));
	walls.back()->setPosition(0, 0);
	this->addChild(walls.back(), 100);

	walls.push_back(Sprite::create("wallY.png"));
	walls.back()->setAnchorPoint(Vec2(0, 0));
	walls.back()->setPosition(800, 0);
	this->addChild(walls.back(), 100);

	xVel = 0;
	yVel = 10;

	

	//keyboard listener
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(BreakoutGameScene::keyPressed, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyReleased = CC_CALLBACK_2(BreakoutGameScene::keyReleased, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);



	this->schedule(schedule_selector(BreakoutGameScene::update), 1.0f / 30);

	return true;
}

void BreakoutGameScene::update(float delta) {
	//CCRect collObjBox = wallX->getBoundingBox();
	//CCRect box2Rect = sprite2->getBoundingBox();

	bool collWithBlock = false;

	CCRect ballBox = ball->getBoundingBox();
	CCRect collObjBox;
	
	int i;

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

	if(ball->getPositionX() > 790 || ball->getPositionX() < 10){
		xVel = xVel * -1;
	}
	else if (ball->getPositionY() > 590) {
		yVel = yVel * -1;
	}

	// Check collision with blocks
	for (i = 0; i < blocks.size(); i++) {
		CCRect ballBox = ball->getBoundingBox();
		CCRect collObjBox = blocks[i]->getBoundingBox();

		if (collObjBox.intersectsRect(ballBox) && !collWithBlock)
		{
			//adjustTrajectory(ballBox, collObjBox);

			if (collObjBox.getMinY() <= ballBox.getMaxY()) {
					yVel = yVel * -1;
			}
			// below
			else if (collObjBox.getMaxY() >= ballBox.getMinY()) {
					yVel = yVel * -1;
			}
			// left
			else if (collObjBox.getMaxX() >= ballBox.getMinX()) {
					xVel = xVel * -1;
			}
			// right
			else if (collObjBox.getMinX() <= ballBox.getMaxX()) {
					xVel = xVel * -1;
			}

			// Delete block -- add score logic here
			blocks[i]->removeFromParentAndCleanup(true);
			blocks.erase(blocks.begin() + i);
			//i = i - 1;

			collWithBlock = true;

		}

	}

	

	// Check collision with paddle - paddle collision has its own rules
	collObjBox = paddle->getBoundingBox();
	if (collObjBox.intersectsRect(ballBox))
	{
		yVel = yVel * -1;
		xVel = (ballBox.getMidX() - collObjBox.getMidX())/5 ;

	}

	// Set updated ball position based on calculated velocity
	ball->setPosition(ball->getPositionX() + xVel, ball->getPositionY() + yVel);

	// Set position of paddle
	if (leftDown) {
		paddle->setPosition(paddle->getPositionX() - 10, paddle->getPositionY());
	}
	else if (rightDown) {
		paddle->setPosition(paddle->getPositionX() + 10, paddle->getPositionY());
	}

}

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
