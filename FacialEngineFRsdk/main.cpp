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
	std::shared_ptr<FRsdkBiometricProcessor> task(new FRsdkBiometricProcessor(cfg_path));

	//long acquire = FaceFind | PortraitAnalysis | FaceImageExtraction | IsoComplianceTest;// | Enrollment;

	std::string shwarc   = "C:\\Users\\User\\Desktop\\face-201.jpg";


	BiometricPipelineBalanced pipeline(task);
	
	unsigned int start = clock();
	try
	{
		FRsdkEntities::ImageInfoPtr ptr1 = pipeline.acquire(shwarc);
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




