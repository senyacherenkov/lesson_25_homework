#include "utility.h"

std::vector<std::string> parseData(std::string data)
{
    size_t pos = 0;
    size_t newStart = 0;
    std::string token;
    std::vector<std::string> result;
    while((pos = data.find(escChar, newStart)) != std::string::npos) {
        token = std::string(std::next(data.begin(), static_cast<long>(newStart)), std::next(data.begin(), static_cast<long>(pos)));
        result.push_back(token);
        newStart = pos + 1;
    }
    return result;
}

std::ostream& operator << (std::ostream& out, ThreadData const& data){
    if(data.m_nStrings != 0)
        out << data.m_nStrings << " strings, ";
    out << data.m_nCommands << " commands, "
        << data.m_nBlocks << " blocks" << std::endl;
    return out;
}
