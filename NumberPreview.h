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

#include "classfieldraw.h"

#ifndef CLASSFIELDDATA
#error "Classfield data is not entry"
#endif

class NumberPreview;
class NumberPreviewOld;
class CountryCodeDB;

class CountryCodeDB
{
    friend class NumberPreview;
    friend class NumberPreviewOld;
public:
    CountryCodeDB(const char * classFieldRaw);

    static const CountryCodeDB &instance();

private:
    struct classIn
    {
        std::vector<std::uint32_t> codes;
        std::string country;
        std::string dialcode;


        classIn() = default;
        classIn(const classIn&) = default;
        classIn(classIn&& other) : codes(std::move(other.codes)), country(std::move(other.country)), dialcode(std::move(other.dialcode)){}

        classIn& operator =(const classIn& other){
            codes = other.codes;
            country = other.country;
            dialcode = other.dialcode;
            return *this;
        }

        classIn& operator =(classIn&& other){
            codes = (std::move(other.codes));
            country = (std::move(other.country));
            dialcode = (std::move(other.dialcode));
            return *this;
        }
    };

    std::vector<classIn> _class;

    static const CountryCodeDB __instance;
};

enum NumberFormat
{
    Beauty,
    Short
};

class NumberPreview
{
public:
    NumberPreview(std::string phoneNumber) ;

    std::vector<std::uint32_t> codes;
    std::string country;
    std::string dialcode;

    std::uint64_t _numerics;

    std::string full(NumberFormat format = NumberFormat::Beauty);

    bool isGenericNumber() const;
    bool isEmpty() const;

};

NumberPreview::NumberPreview(std::string number) {
    int x,y;
    const CountryCodeDB &cdb = CountryCodeDB::instance();
    std::string phoneNumber = number;
    // remove all ignoring case [ + - ( ) ] and whitespaces
    auto new_end = std::remove_if(std::begin(phoneNumber), std::end(phoneNumber), [](auto & item){
        return item == '+' || item == '-' || item == '(' || item == ')' || std::isspace(item);
    });
    phoneNumber.erase(new_end, phoneNumber.end());

    x = 0;
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

std::string NumberPreview::full(NumberFormat format) {
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

bool NumberPreview::isGenericNumber() const
{
    std::uint64_t v = _numerics & 0xFFFFFFFFFFFF;
    return v <= 9999999999999 && v >= 19999999999;
}

bool NumberPreview::isEmpty() const
{
    return _numerics == 0;
}

CountryCodeDB::CountryCodeDB(const char *classFieldRaw)
{
    const char * p = classFieldRaw, *pLeft;
    int x,y,l;
    _class.clear();
    while((pLeft = strchr(p, '\n')) != nullptr)
    {
        classIn cIn {};
        l = pLeft - p;
        for(x = 0, y = 0; x < l; ++x)
        {
            if(p[x] == ';' || x == l-1)
            {
                // Country Code
                if(p[y] == 'C')
                {
                    cIn.country = std::move(std::string(p +y+2, x-y-2));
                }
                // Number
                else if(p[y] == 'c')
                {
                    std::istringstream stream(std::move(std::string(p + y+2,x-y-2)));
                    std::string _t;
                    while(std::getline(stream, _t, '-'))
                        cIn.codes.push_back(std::atoi(_t.c_str()));
                }
                // Dial code
                else if(p[y]=='d')
                {
                    cIn.dialcode = std::move(std::string(p + y+2,x-y-1));
                }
                y = x+1;
            }
        }
        _class.push_back(std::move(cIn));
        p = pLeft + 1;
    }

    auto _sqr = [](const std::vector<std::uint32_t> & v) -> std::uint32_t {std::uint32_t retval = 0; for(std::uint32_t val : v) { retval += val * val; }  return retval;} ;
    std::sort(std::begin(_class), std::end(_class), [_sqr](auto & lhs, auto & rhs){
        return _sqr(lhs.codes) < _sqr(rhs.codes);});
}

const CountryCodeDB CountryCodeDB::__instance { CLASSFIELDDATA };

const CountryCodeDB& CountryCodeDB::instance()
{
    return CountryCodeDB::__instance;
}
