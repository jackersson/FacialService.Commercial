//#include "facial_engine.hpp"

//#include <ppl.h>
//#include <stdexcept>
//#include <queue>
//#include <time.h>

#include "pipeline/biometric_pipeline_load_balanced.hpp"


using namespace ::Concurrency;
using namespace BioFacialEngine;
using namespace BioContracts;
using namespace FRsdkTypes;
using namespace Pipeline;

#include <iostream>


int main(int argc, char** argv)
{	
	std::string cfg_path = "C:\\FVSDK_9_1_1\\etc\\frsdk.cfg";
	auto task(std::make_shared<FRsdkBiometricProcessor>(cfg_path));
	
	std::string image1 = "C:\\Users\\User\\Desktop\\face-20.jpg";
	std::string image2 = "C:\\Users\\User\\Desktop\\face-201.jpg";

	BiometricPipelineBalanced pipeline(task);
	
	unsigned int start = clock();
	try	{
	   VerificationResultPtr ptr1 = pipeline.verify_face(image1, image2);
	}
	catch ( std::exception& ex)
	{
		std::cout << ex.what();
	}


	std::cout << "pipeline = " << clock() - start << std::endl;
	std::cin.get();
	std::cout << std::endl;	


	return 0;
}




