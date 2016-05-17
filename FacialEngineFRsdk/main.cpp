#include "facial_engine.hpp"

using namespace BioContracts;
using namespace BioFacialEngine;
using namespace std;


int main(int argc, char** argv)
{
	std::string cfg_path = "C:\\FVSDK_8_9_5\\etc\\frsdk.cfg";
	FacialEngine engine(cfg_path);

	std::string filename  = "C:\\Users\\jacke\\Desktop\\3423.jpg";
	std::string filename2 = "C:\\Users\\jacke\\Desktop\\shwarc.jpg";

	//std::string file1( readFileBytes(filename));
	//std::string file2( readFileBytes(filename2));

	//RawImage raw1 = readFileBytes(filename);//, file1.size());
	//RawImage raw2 = readFileBytes(filename2);

	engine.verify(filename, filename);



	return 0;
}
