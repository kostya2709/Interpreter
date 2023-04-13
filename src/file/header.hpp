#pragma once

#include <inttypes.h>

struct Header {
    uint32_t entryPointOffset;
    uint32_t sectionDataEntryOffset;

    friend std::ostream& operator<< (std::ostream& stream, const Header& header) {
        return stream << header.entryPointOffset << header.sectionDataEntryOffset;
    }
};