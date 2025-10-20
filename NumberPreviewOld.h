#pragma once

#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <array>
#include <algorithm>

#include <string.h>

#include "NumberPreview.h"

class NumberPreviewOld;
class CountryCodeDB;

class NumberPreviewOld
{
public:
    NumberPreviewOld(std::string phoneNumber) ;

    std::vector<std::uint32_t> codes;
    std::string country;
    std::string dialcode;

    std::array<std::uint32_t,5> numerics;

    std::string full(NumberFormat format = NumberFormat::Beauty);

    bool isGenericNumber() const;
    bool isEmpty() const;

};

NumberPreviewOld::NumberPreviewOld(std::string number) : numerics() {
    int x,y;
    const CountryCodeDB &cdb = CountryCodeDB::instance();
    std::string phoneNumber = number;
    // remove all ignoring case [ + - ( ) ] and whitespaces
    auto new_end = std::remove_if(std::begin(phoneNumber), std::end(phoneNumber), [](auto & item){
        return item == '+' || item == '-' || item == '(' || item == ')' || std::isspace(item);
    });
    phoneNumber.erase(new_end, phoneNumber.end());
    numerics.fill(-1);
    x = std::max<int>(0,static_cast<int>(phoneNumber.size())-2);
    y = 4 - (x==0);
    for(; y > 2; --y, x = std::max<int>(0, x-2))
    {
        numerics[y]=std::strtoul(phoneNumber.c_str() + x, nullptr, 10) | (phoneNumber[x]=='0') << 31;
        phoneNumber[x] = '\0';
    }
    if(x >= 0 && y >= 0)
    {
        x = std::max(0,x - 1);
        numerics[y] = std::strtoul(phoneNumber.c_str() + x, nullptr, 10);
        numerics[y] |= (numerics[y] < 100 && numerics[y] > 10) << 30 | (numerics[y] < 10) << 31;
        --y;
        phoneNumber[x] = '\0';
    }
    if(x > 0 && y >= 0)
    {
        x = std::max(0,x - 3);
        numerics[y--] = std::strtoul(phoneNumber.c_str() + x, nullptr, 10) | (phoneNumber[x]=='0') << 31;
        phoneNumber[x] = '\0';
    }
    if(x > 0 && y >= 0)
    {
        x = 0;
        numerics[y] = std::strtoul(phoneNumber.c_str() + x, nullptr, 10) | (phoneNumber[x]=='0') << 31;
    }

    x = numerics.front();
    auto iter = (x == -1) ? std::end(cdb._class) : std::find_if(std::begin(cdb._class), std::end(cdb._class),
                             [x](auto &ref) {
                                 for (int i = 0; i < ref.codes.size(); ++i)
                                     if (ref.codes[i] == x)
                                         return true;
                                 return false;
                             });
    if (iter != std::end(cdb._class)) {
        country = iter->country;
        dialcode = iter->dialcode;
        codes = iter->codes;
    } else {
        country = "Invalid";
        dialcode = "invalid";
        codes.push_back(static_cast<std::uint32_t>(0));
    }
}

inline std::string numberDouble(std::uint32_t num, std::uint32_t levels)
{
    std::string retval;
    while(levels > 0 && num < std::pow(10,levels--))
        retval += '0';
    retval += std::to_string(num);
    return retval;
}

std::string NumberPreviewOld::full(NumberFormat format) {
    constexpr auto _formatBeauty = "+%s (%s) %s-%s-%s";
    constexpr auto _formatShort = "+%s%s%s%s%s";
    int y,len;
    char buffer[32];
    std::vector<std::string> _converted;

    for(y = 0; y < numerics.size(); ++y)
    {
        if(numerics[y] != static_cast<std::uint32_t>(-1))
        {
            len = (numerics[y] >> 31) + (numerics[y] >> 30 & 0x1);
            _converted.push_back(std::move(numberDouble(numerics[y] & 0x3FFFFFFF, len)));
        }
    }

    if(isGenericNumber())
    {
        len = std::snprintf(buffer, 32, (format == NumberFormat::Beauty ? _formatBeauty : _formatShort), _converted[0].c_str(), _converted[1].c_str(), _converted[2].c_str(), _converted[3].c_str(), _converted[4].c_str());
    }
    else
    {
        int y;
        for(y = 0, len = 0; y < _converted.size(); ++y)
        {
            len += std::snprintf(buffer + len, 32-len, "%s", _converted[y].c_str());
        }
    }
    return std::string(buffer, len);
}

bool NumberPreviewOld::isGenericNumber() const
{
    return numerics.front() != static_cast<std::uint32_t>(-1);
}

bool NumberPreviewOld::isEmpty() const
{
    return std::all_of(std::begin(numerics), std::end(numerics), std::bind(std::equal_to<std::uint32_t>(), std::placeholders::_1, static_cast<std::uint32_t>(-1)));
}
