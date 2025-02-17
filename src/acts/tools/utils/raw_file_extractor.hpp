#pragma once

namespace tool::utils::raw_file_extractor {

	std::string ExtractRawFile(std::vector<byte>& data);

	void WriteRawFileInto(std::vector<byte>& data, std::ostream& out);
}