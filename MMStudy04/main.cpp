#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>

#include "AudioCoder.h"
#include "getopt.h"

#define USAGE "=== Help Info ===\n"\
			  "-h : Show help info\n"\
			  "-i path : Set wave data file path\n"\
			  "-e path : Set path of output encode text file( default for xxx_encode.txt )\n"\
			  "-d path : Set path of output decode text file( default for xxx_decode.txt )\n"\
			  "-m encode method(Loseless \\ DPCM \\ DM \\ ADPCM) : Set encode method( default for Loseless )\n"

int main(int argc, char* argv[])
{
	::opterr = 0;
	char optch = 0;
	bool over = false;

	mmAudio::CodeType type = mmAudio::Loseless;
	const char* typeName[] = {
		"_loseless_", "_DPCM_", "_DM_"
	};
	std::ifstream fin;
	std::ofstream fout;
	std::string inputFile;
	std::string outputFile[2];
	bool defaultName[2] = { true, true };

	if (argc < 2) {
		std::cout << USAGE << std::endl;
		return 0;
	}

	while ((optch = getopt(argc, argv, "hi:e:d:m:")) != -1 && !over) {
		switch (optch) {
		case 'i':
			inputFile = ::optarg;
			fin.open(inputFile);
			if (fin.fail()) {
				std::cerr << "Can't open input wave file " << ::optarg << std::endl;
				over = true;
			}
			break;

		case 'e':
			outputFile[0] = ::optarg;
			fout.open(outputFile[0]);
			if (fout.fail()) {
				std::cerr << "Can't create output encode file " << ::optarg << std::endl;
				over = true;
			}
			defaultName[0] = false;
			break;

		case 'd':
			outputFile[1] = ::optarg;
			defaultName[1] = false;
			break;

		case 'm':
			if (std::strcmp(::optarg, "Loseless") == 0) {
				type = mmAudio::Loseless;
			}
			else if (std::strcmp(::optarg, "DPCM") == 0) {
				type = mmAudio::DPCM;
			}
			else if (std::strcmp(::optarg, "DM") == 0) {
				type = mmAudio::DM;
			}
			else {
				std::cerr << "Can't do audio coding for type " << ::optarg << std::endl;
				over = true;
			}
			break;
		}
	}

	if (defaultName[0]) {
		outputFile[0] = inputFile.substr(0, inputFile.length() - 4) + typeName[type] + "encode.txt";
		fout.open(outputFile[0]);
		if (fout.fail()) {
			std::cerr << "Can't create output encode file " << outputFile[0] << std::endl;
			over = true;
		}
	}
	if (defaultName[1]) {
		outputFile[1] = inputFile.substr(0, inputFile.length() - 4) + typeName[type] + "decode.txt";
	}

	if (!over) {
		std::cout << "encoding..." << std::endl;

		auto predictor = [](const std::vector<uint8_t>& seq) {
			return seq.size() > 1 ? (seq.back() + seq[seq.size() - 2]) / 2 : seq[0];
		}; 

		switch (type)
		{
		case mmAudio::Loseless:
			mmAudio::encode(fin, fout, predictor, [](int8_t error) { return error; });
			break;

		case mmAudio::DPCM:
			mmAudio::encode(fin, fout, predictor, [](int8_t error)
			{ return 16 * ((error + 127) / 16) - 128 + 8; });
			break;

		case mmAudio::DM:
			mmAudio::encode(fin, fout, predictor, [](int8_t error)
			{ return error > 0 ? 3 : -3; });
			break;
		}

		fin.close();
		fout.close();
		fin.open(outputFile[0]);
		fout.open(outputFile[1]);
		if (fout.fail()) {
			std::cerr << "Can't create output encode file " << outputFile[1] << std::endl;
			return -1;
		}

		std::cout << "decoding..." << std::endl;
		switch (type)
		{
		case mmAudio::Loseless:
			mmAudio::decode(fin, fout, predictor);
			break;

		case mmAudio::DPCM:
			mmAudio::decode(fin, fout, predictor);
			break;

		case mmAudio::DM:
			mmAudio::decode(fin, fout, predictor);
			break;
		}

		fin.close();
		fout.close();
	}

	return 0;
}