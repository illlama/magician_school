﻿#include "MaraudersMap.h"
#include "main.h"

#include <bangtal>
#include <iostream>
#include <ctime>

using namespace bangtal;
using namespace std;

extern ScenePtr home;

ScenePtr mapScene; 
MapObjectPtr player;
TrapPtr trap[trapMAX];
//ObjectPtr target;
int target_xMax = 0, target_xMin = 0, target_yMax = 0, target_yMin = 0;
SoundPtr mapSound;
TimerPtr mainTimer, checkTimer;

void MaraudersMap_main(Dorm dorm) {

	srand(unsigned(time(NULL)));

	mapScene = Scene::create(" ", "maze/background.png");

	mapSound = Sound::create("music/map.mp3");
	mapSound->play();

	player = MapObject::create(17, 48, mapScene, "maze/player.png");
	player->setScale(0.1f);


	// player
	mapScene->setOnKeyboardCallback([&](ScenePtr scene, int key, bool pressed)->bool {

		Direction d = Direction::STAY;
		if (key == 82) d = Direction::LEFT;
		else if (key == 83) d = Direction::RIGHT;
		else if (key == 84) d = Direction::UP;
		else if (key == 85) d = Direction::DOWN;


		if (pressed && d != Direction::STAY) {
			if (player->move(d)) {
				player->locate(mapScene, player->setX(player->X()), player->setY(player->Y()));
			}
		}

		return true;

		});


	// trap
	int timeArr[trapMAX] = { 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 4 };
	for (int i = 0; i < trapMAX; i++) {
		int j = rand() % trapMAX;

		if (i != j) {
			swap(timeArr[i], timeArr[j]);
		}
	}

	int speedArr[trapMAX];
	for (int i = 0; i < trapMAX; i++) {
		timeArr[i] *= 10;
		speedArr[i] = ((rand() % 3) + 1);
	}

	int xyArr[trapMAX][2] = { { 37, 34 },
							  { 45, 37 },
							  { 76, 40 },
							  { 21, 9 },
							  { 58, 8 },
							  { 40, 17 },
							  { 35, 47 },
							  { 50, 14 },
							  { 20, 19 },
							  { 15, 39 },
							  { 19, 3 },
							  { 17, 35 },
							  { 71, 22 } };



	for (int i = 0; i < trapMAX; i++) {


		if (i < upMAX) {
			trap[i] = Trap::create(Direction::UP, xyArr[i][0], xyArr[i][1], mapScene);
		}
		else if (i < upMAX + downMAX) {
			trap[i] = Trap::create(Direction::DOWN, xyArr[i][0], xyArr[i][1], mapScene);
		}
		else if (i < upMAX + downMAX + leftMAX) {
			trap[i] = Trap::create(Direction::LEFT, xyArr[i][0], xyArr[i][1], mapScene);

		}
		else {
			trap[i] = Trap::create(Direction::RIGHT, xyArr[i][0], xyArr[i][1], mapScene);
		}


		trap[i]->foot1->setScale(0.25f);
		trap[i]->foot2->setScale(0.25f);


		trap[i]->timer = Timer::create(0.5f * timeArr[i]);
		trap[i]->timer->setOnTimerCallback([&, i, speedArr](TimerPtr t)->bool {

			trap[i]->foot1->show();
			trap[i]->foot2->show();

			(trap[i]->direction == Direction::UP || trap[i]->direction == Direction::DOWN)
				? trap[i]->foot2->changeY(trap[i]->foot1->Y() + trap[i]->setFootXY())
				: trap[i]->foot2->changeX(trap[i]->foot1->X() + trap[i]->setFootXY());
			if (trap[i]->foot2->checkXY(trap[i]->foot2->X(), trap[i]->foot2->Y()))
				trap[i]->locateFootprint(mapScene, 2);
			else trap[i]->turn_trap();


			auto timer2 = Timer::create(0.3f * speedArr[i]);
			timer2->setOnTimerCallback([&, i](TimerPtr t2) -> bool {
				(trap[i]->direction == Direction::UP || trap[i]->direction == Direction::DOWN)
					? trap[i]->foot1->changeY(trap[i]->foot2->Y())
					: trap[i]->foot1->changeX(trap[i]->foot2->X());

				if (trap[i]->foot1->move(trap[i]->direction))
					trap[i]->locateFootprint(mapScene, 1);
				else trap[i]->turn_trap();

				return true;
				});
			timer2->start();

			cout << i << "-" << speedArr[i] << endl;
			t->set(0.6f * speedArr[i]);
			t->start();

			return true;
			});

		trap[i]->timer->start();


	}


	// target
	auto gryffindor = Object::create("maze/g_logo.png", mapScene, 1155, 380);
	gryffindor->setScale(0.2f);
	auto hufflepuff = Object::create("maze/h_logo.png", mapScene, 350, 580);
	hufflepuff->setScale(0.2f);
	auto ravenclaw  = Object::create("maze/r_logo.png", mapScene, 640, 535);
	ravenclaw->setScale(0.2f);
	auto slytherin = Object::create("maze/s_logo.png", mapScene, 870, 620);
	slytherin->setScale(0.2f);

	
	switch (dorm) {
	case Dorm::G: 
		//target = gryffindor; 
		target_xMax = 87, target_xMin = 84, target_yMax = 22, target_yMin = 20;
		break;
	case Dorm::H: 
		//target = hufflepuff; 
		target_xMax = 25, target_xMin = 23, target_yMax = 9, target_yMin = 4; 
		break;
	case Dorm::R: 
		//target = ravenclaw; 
		target_xMax = 49, target_xMin = 45, target_yMax = 13, target_yMin = 10; 
		break;
	case Dorm::S: 
		//target = slytherin; 
		target_xMax = 65, target_xMin = 63, target_yMax = 5, target_yMin = 3; 
		break;
	}

	
	// timer
	mainTimer = Timer::create(60.0f);
	showTimer(mainTimer);
	mainTimer->setOnTimerCallback([](TimerPtr)->bool {
		showMessage("time over");

		return false;
		});

	mainTimer->start();

	// check trap & target
	checkTimer = Timer::create(0.1f);
	checkTimer->setOnTimerCallback([&](TimerPtr t)->bool {

		for (int i = 0; i < trapMAX; i++) {
			if ((player->X() == trap[i]->foot1->X() && player->Y() == trap[i]->foot1->Y())
				|| (player->X() == trap[i]->foot2->X() && player->Y() == trap[i]->foot2->Y())) {
				showMessage("fail");
				mainTimer->stop();
				checkTimer->stop();
				hideTimer();
				mapSound->stop();
				//home->enter();
				enterScene(home->ID());
			}
			if ((player->X() >= target_xMin && player->X() <= target_xMax
				&& player->Y() >= target_yMin && player->Y() <= target_yMax)) {
				showMessage("Game clear");
				mapSound->stop();
				enterScene(home->ID());
				mainTimer->stop();
				checkTimer->stop();
			}
		}

		t->set(0.1f);
		t->start();

		return true;
		});
	checkTimer->start();

	enterScene(mapScene->ID());

}