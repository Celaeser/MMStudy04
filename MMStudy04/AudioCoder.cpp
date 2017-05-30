#include "AudioCoder.h"
#include <cmath>

namespace mmAudio {
	void encode(std::istream& in, std::ostream& out,
		const std::function<uint8_t(const std::vector<uint8_t>)>& predictor, const std::function<int8_t(int8_t)>& quantizer)
	{
		uint16_t byte = 0;
		int16_t error = 0;
		uint16_t predictByte = 0;
		std::vector<uint8_t> sequence;

		if (in >> byte) {
			sequence.push_back(byte);
			out << byte << '\n';
		}

		while (in >> byte) {
			predictByte = predictor(sequence);
			error = quantizer(byte - predictByte);
			out << error << '\n';
			sequence.push_back(error + predictByte);
		}

		return;
	}

	void decode(std::istream& in, std::ostream& out,
		const std::function<uint8_t(const std::vector<uint8_t>)>& predictor)
	{
		int16_t byte = 0;
		uint16_t predictByte = 0;
		std::vector<uint8_t> sequence;

		if (in >> byte) {
			sequence.push_back(byte);
			out << byte << '\n';
		}

		while (in >> byte) {
			predictByte = predictor(sequence);
			out << byte + predictByte << '\n';
			sequence.push_back(byte + predictByte);
		}

		return;
	}
}