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

	long acquire = FaceFind | PortraitAnalysis | FaceImageExtraction | IsoComplianceTest;// | Enrollment;

	std::string shwarc   = "C:\\Users\\User\\Desktop\\2.jpg";
	std::string klichko2 = "C:\\Users\\User\\Desktop\\33.jpg";
	//std::string klichko  = "C:\\Users\\User\\Desktop\\3.jpg";
  //std::string gates    = "C:\\Users\\User\\Desktop\\fd.jpg";

	BiometricPipelineBalanced pipeline(task);
	
	unsigned int start = clock();
	//FRsdkEntities::ImageInfoPtr ptr1 = nullptr;
	//FRsdkEntities::ImageInfoPtr ptr2 = nullptr;
	//FRsdkEntities::ImageInfoPtr ptr3 = nullptr;

	//std::list<std::string> images = { klichko2, klichko2 };
	//IdentificationResultPtr ptr = pipeline.identify_face(klichko2, images);

	//IImageInfoPtr ptr = pipeline.acquire(shwarc, acquire);
	//if (ptr != nullptr)
	//  (*ptr)[0].characteristics().faceBox();
	//IImageInfoPtr ptr1 = pipeline.acquire(klichko2);
	//IImageInfoPtr ptr2 = pipeline.acquire(klichko);
	//IImageInfoPtr ptr3 = pipeline.acquire(gates);
	//std::cin.get();
	/*
	parallel_invoke(
		[&]() {
		
		ptr1 = pipeline.acquire(shwarc, acquire);
		std::cout << "first = " << clock() - start << std::endl;

	},
		[&]() { 

		
		ptr2 = pipeline.acquire(klichko, acquire);
		std::cout << "second = " << clock() - start << std::endl;
	},

		[&]() {

		ptr3 = pipeline.acquire(gates, acquire);
		std::cout << "third = " << clock() - start << std::endl;
	}
		
	);
	*/
	/*
	for (auto it = ptr1->cbegin(); it != ptr1->cend(); ++it)
	{
		std::cout << "has fir = " << (*it)->id() << " " << (*it)->hasFir() << std::endl;
	}

	for (auto it = ptr2->cbegin(); it != ptr2->cend(); ++it)
	{
		std::cout << "has fir = " <<  (*it)->id() << " " << (*it)->hasFir() << std::endl;
	}
	*/
	std::cout << "pipeline = " << clock() - start << std::endl;
	std::cin.get();
	std::cout << std::endl;	


	return 0;
}




