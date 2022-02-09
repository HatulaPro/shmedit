#include "Display.h"
#include "helpers/Helper.h"
#include "Config.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

int main() {

	std::string fileName = "readme.md";
	Display d = Display(fileName);
	Config::parse("shmedit.conf");
	//Display d = Display("mytest.txt");
	while (true) {
		d.show();
		d.callAction(Helper::getNextKey());
		//std::this_thread::sleep_for(std::chrono::milliseconds(200));
		//std::cout << Helper::getNextKey() << std::endl;
	}
	return 0;
}