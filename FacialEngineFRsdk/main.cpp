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
	std::cout << "Here" << std::endl;
	std::string cfg_path = "C:\\FVSDK_9_1_1\\etc\\frsdk.cfg";
	auto task(std::make_shared<FRsdkBiometricProcessor>(cfg_path));
	
	std::string image1 = "C:\\Users\\User\\Desktop\\face-20.jpg";
	std::string image2 = "C:\\Users\\User\\Desktop\\face-201.jpg";

	std::string rivs       = "C:\\Users\\User\\Desktop\\men3.jpg";

	std::string rivs_kerry = "C:\\Users\\User\\Desktop\\rivz.jpg";
	std::string men2       = "C:\\Users\\User\\Desktop\\men2.jpg";

	std::list<std::string> population = { rivs_kerry , men2 };

	BiometricPipelineBalanced pipeline(task);

	long population_id = -1;
	try
	{
//		population_id = pipeline.load_identification_population(population);
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what();
	}
	
	
	try	{		
		 unsigned int start = clock();
	   IdentificationResultPtr ptr1 = pipeline.identify_face(rivs, population_id);
		 std::cout << "match = " << clock() - start << std::endl;
	}
	catch ( std::exception& ex)
	{
		std::cout << ex.what();
	}


	
	std::cin.get();
	std::cout << std::endl;	


	return 0;
}




