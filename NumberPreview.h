#pragma once

#include <cmath>
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
struct CountryCode;

struct CountryCode
{
    std::vector<std::uint32_t> codes;
    std::string country;
    std::string dialcode;

    CountryCode() = default;
    CountryCode(const CountryCode&) = default;
    CountryCode(CountryCode&& other) : codes(std::move(other.codes)), country(std::move(other.country)), dialcode(std::move(other.dialcode)){}

    CountryCode& operator =(const CountryCode& other){
        codes = other.codes;
        country = other.country;
        dialcode = other.dialcode;
        return *this;
    }

    CountryCode& operator =(CountryCode&& other){
        codes = (std::move(other.codes));
        country = (std::move(other.country));
        dialcode = (std::move(other.dialcode));
        return *this;
    }
};

class CountryCodeDB
{
    friend class NumberPreview;
    friend class NumberPreviewOld;
public:
    CountryCodeDB(const CountryCodeDB &) = default;
    CountryCodeDB(CountryCodeDB &&) = default;
    CountryCodeDB &operator=(const CountryCodeDB &) = default;
    CountryCodeDB &operator=(CountryCodeDB &&) = default;
    CountryCodeDB(const char *classFieldRaw);

    static const CountryCodeDB &instance();
    static std::string country(int code);
    static std::string dialcode(int code);
    static CountryCode countryCode(int code);
private:

    std::vector<CountryCode> _class;

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
    NumberPreview(std::string phoneNumber);

    std::string full(NumberFormat format = NumberFormat::Beauty);

    bool isGenericNumber() const;
    bool isEmpty() const;

    std::string country() const;
    std::string dialCode() const;
    int countryCode() const;

private:
    std::uint64_t _numerics;
    int _countryCode;
    std::string numberDouble(std::uint32_t num, std::uint32_t levels);
};

NumberPreview::NumberPreview(std::string number) {
    int x,y;
    const CountryCodeDB &cdb = CountryCodeDB::instance();
    std::string phoneNumber = number;
    x = 0;
    // remove all ignoring case [ + - ( ) ] and whitespaces
    auto new_end = std::remove_if(std::begin(phoneNumber), std::end(phoneNumber), [&x](auto & item){
        return item == '+' || item == '-' || item == '(' || item == ')' || std::isspace(item) || !std::isdigit(item) && (x = 1);
    });
    if(x)
    // End with no digital number are.
    return;
    phoneNumber.erase(new_end, phoneNumber.end());
    std::uint8_t x0=0;
    for(x = 0; x < phoneNumber.size(); ++x)
    {
        if(phoneNumber[x] != '0')
            break;
        ++x0;
    }

    // str to number and write Null value to high bits.
    _numerics = (std::atoll(phoneNumber.c_str()) & 0xFFFFFFFFFFFF);
    _numerics |= static_cast<std::uint64_t>(x0) << 48;

    _countryCode = (0);
    if(isGenericNumber())
    {
        std::vector<int> _cods;
        auto iter = std::end(cdb._class);
        x = static_cast<int>((_numerics & 0xFFFFFFFFFFFF) / 10000000000ul);
        _cods.push_back(x);
        _cods.push_back(x/10);
        _cods.push_back(x/100);
        if(x < 10)
        {
            x = (_numerics & 0xFFFFFFFFFFFF) / 100000000ul;
            _cods.push_back(x);
            _cods.push_back(x / 10);
        }
        for(y = 0; y < _cods.size() && iter == std::end(cdb._class); ++y)
        {
            x = _cods[y];
            if(x < 0)
                continue;
            iter = std::find_if(std::begin(cdb._class), std::end(cdb._class),
                                [x](auto &ref) {
                                    for (int i = 0; i < ref.codes.size(); ++i)
                                        if (ref.codes[i] == x)
                                            return true;
                                    return false;
                                });
        }
        if(iter != std::end(cdb._class))
            _countryCode = iter->codes.at(0);
    }
}


std::string NumberPreview::full(NumberFormat format) {
    std::string result;
    std::uint64_t _raw;
    int x,y,len;
    x = (_numerics >> 48) & 0xFFFF;
    result.append(x,'0');
    _raw = _numerics & 0xFFFFFFFFFFFF;
    if(isGenericNumber())
    {
        constexpr std::uint8_t vectors[] {3,3,3,2,2};
        std::array<char[8], 5> _parts;
        std::uint64_t tmp,tmp0;
        for( x = _parts.size() - 1; x > -1; --x)
        {
            int p = std::pow(10, vectors[x]);
            tmp0 = _raw / p;
            tmp = _raw - tmp0 * p;
            _raw = tmp0;
            std::snprintf(_parts[x], 15, "%s", x > 0 ? numberDouble(tmp, vectors[x]).c_str() : std::to_string(tmp).c_str());
        }
        result.resize(64);
        len = std::snprintf(result.data(), 64, (format == NumberFormat::Beauty ? "+%s (%s) %s-%s-%s" : "+%s%s%s%s%s"),
                            _parts[0], _parts[1], _parts[2],
                            _parts[3], _parts[4]);
        result.resize(result.size() - 64 + len);
    }
    else
    {
        if(result.empty() && _raw != 0)
            result += std::to_string(_raw);
    }
    return result;
}

inline bool NumberPreview::isGenericNumber() const
{
    return _numerics <= 9999999999999ul && _numerics >= 10000000000ul;
}

inline bool NumberPreview::isEmpty() const
{
    return _numerics == 0;
}

inline std::string NumberPreview::country() const
{
    return CountryCodeDB::country(_countryCode);
}

inline std::string NumberPreview::dialCode() const
{
    return CountryCodeDB::dialcode(_countryCode);
}

inline int NumberPreview::countryCode() const
{
    return _countryCode;
}

std::string NumberPreview::numberDouble(std::uint32_t num,
                                        std::uint32_t levels) {
    std::string retval;
    while (levels > 0 && num < std::pow(10, --levels))
        retval += '0';
    retval += std::to_string(num);
    return retval;
}

CountryCodeDB::CountryCodeDB(const char *classFieldRaw)
{
    const char * p = classFieldRaw, *pLeft;
    int x,y,l;
    _class.clear();
    while((pLeft = strchr(p, '\n')) != nullptr)
    {
        CountryCode cIn {};
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

inline std::string CountryCodeDB::country(int code)
{
    return countryCode(code).country;
}

inline std::string CountryCodeDB::dialcode(int code)
{
    return countryCode(code).dialcode;
}

inline CountryCode CountryCodeDB::countryCode(int code)
{
    CountryCode cc {};
    const CountryCodeDB & cdb = instance();

    auto iter = std::find_if(std::begin(cdb._class), std::end(cdb._class),
                        [code](auto &ref) {
                            for (int i = 0; i < ref.codes.size(); ++i)
                                if (ref.codes[i] == code)
                                    return true;
                            return false;
                        });
    if(iter != std::end(cdb._class))
    {
        cc = *iter;
    }
    else
    {
    cc.country = "Unknown";
    cc.dialcode = "UNKNOWN";
    cc.codes.push_back(static_cast<std::uint32_t>(0));
    }
    return cc;
}
