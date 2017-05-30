#include <functional>
#include <string>
#include <iostream>
#include <vector>

namespace mmAudio {
	enum CodeType {
		Loseless, DPCM, DM
	};

	//Stupid Rat Style Commit, keep it stupid simple.
	/**
	* encode function of audio
	*
	* @param in					The input stream
	* @param out				The output stream
	* @param predictor			The predict function object
	* @param quantizer			The quantize function object
	* @return					Nothing
	*/
	void encode(std::istream& in, std::ostream& out,
		const std::function<uint8_t(const std::vector<uint8_t>)>& predictor, const std::function<int8_t(int8_t)>& quantizer);

	/**
	* decode function of audio
	*
	* @param in					The input stream
	* @param out				The output stream
	* @param predictor			The predict function object
	* @return					Nothing
	*/
	void decode(std::istream& in, std::ostream& out,
		const std::function<uint8_t(const std::vector<uint8_t>)>& predictor);
}
