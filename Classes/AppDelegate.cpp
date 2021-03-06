#include "AppDelegate.h"
#include "BreakoutGameScene.h"
#include "MenuScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching() {
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if (!glview) {
		glview = GLViewImpl::create("Breakout");
		glview->setFrameSize(800, 600);
		director->setOpenGLView(glview);
	}

	//auto scene = GraphicsScene::createScene();
	//director->runWithScene(scene);-=

	auto scene = MenuScene::createScene();
	//auto scene = GameScene::createScene();
	director->runWithScene(scene);

	return true;
}

void AppDelegate::applicationDidEnterBackground() {
}

void AppDelegate::applicationWillEnterForeground() {
}