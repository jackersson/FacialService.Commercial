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

	std::string cfg_path = "C:\\FVSDK_8_9_5\\etc\\frsdk.cfg";
	std::shared_ptr<FRsdkBiometricProcessor> task(new FRsdkBiometricProcessor(cfg_path));

	long acquire = FaceFind | PortraitAnalysis | FaceImageExtraction | IsoComplianceTest | Enrollment;

	std::string shwarc  = "C:\\Users\\jacke\\Desktop\\3423.jpg";
	std::string klichko = "C:\\Users\\jacke\\Desktop\\shwarc.jpg";
	std::string gates  = "C:\\Users\\jacke\\Desktop\\images1ARMIOC1.jpg";

	BiometricPipelineBalanced pipeline(task);
	
	unsigned int start = clock();
	FRsdkEntities::ImageInfoPtr ptr1 = nullptr;
	FRsdkEntities::ImageInfoPtr ptr2 = nullptr;
	FRsdkEntities::ImageInfoPtr ptr3 = nullptr;

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




