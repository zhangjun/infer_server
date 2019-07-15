#include <iostream>
#include <string>

#include "config_reader.h"

int main() {
	ConfigReader reader("sample.ini");
	std::string ip = reader.Get("main", "ip", "127.0.0.1");
	std::cout << ip << std::endl;
	return 0;
}
