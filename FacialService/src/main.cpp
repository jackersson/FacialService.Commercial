#include "facial_service_starter.hpp"

int main(int argc, char** argv)
{

	BioFacialService::FacialServiceStarter starter;

	starter.start();

	std::cin.get(); 

	starter.stop();

	return 0;
}