// Example that shows simple usage of the INIReader class

#include <iostream>
#include <sstream>
#include "INIReader.h"

std::string sections(INIReader &reader)
{
    std::stringstream ss;
    std::set<std::string> sections = reader.Sections();
    for (std::set<std::string>::iterator it = sections.begin(); it != sections.end(); ++it)
        ss << *it << ",";
    return ss.str();
}

int main()
{
    INIReader reader("streamer.ini");

    if (reader.ParseError() < 0) {
        std::cout << "Can't load 'streamer.ini'\n";
        return 1;
    }
	std::cout << "Config loaded from 'streamer.ini': found sections=" << sections(reader)
		<< " roiheight=" << reader.GetInteger("Calibration", "ROIHeight", -1) 
		<< ", roiwidth="<< reader.GetInteger("Calibration", "ROIWidth", -1) 
		<< ", offsetheight=" << reader.GetInteger("Calibration", "OffsetHeight", -1)
		<< ", offsetwidth=" << reader.GetInteger("Calibration", "OffsetWidth", -1)
		<< ", offsetwidthLR=" << reader.GetInteger("Calibration", "OffsetWidthLR", -1) << "\n";

	int rw = 0, rh = 0, ow = 0, oh = 0, olr = 0;
	rh = reader.GetInteger("Calibration", "ROIHeight", -1);
	rw = reader.GetInteger("Calibration", "ROIWidth", -1);
	oh = reader.GetInteger("Calibration", "OffsetHeight", -1);
	ow = reader.GetInteger("Calibration", "OffsetWidth", -1);
	olr = reader.GetInteger("Calibration", "OffsetWidthLR", -1);
//     std::cout << "Config loaded from 'streamer.ini': found sections=" << sections(reader)
//               << " version="
//               << reader.GetInteger("protocol", "version", -1) << ", name="
//               << reader.Get("user", "name", "UNKNOWN") << ", email="
//               << reader.Get("user", "email", "UNKNOWN") << ", multi="
//               << reader.Get("user", "multi", "UNKNOWN") << ", pi="
//               << reader.GetReal("user", "pi", -1) << ", active="
//               << reader.GetBoolean("user", "active", true) << "\n";
    return 0;
}
