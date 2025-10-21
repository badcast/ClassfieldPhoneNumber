#include <iostream>
#include <string>
#include <cstring>

#include "NumberPreview.h"

std::string to_json(const NumberPreview & param, bool localCode, bool beautify)
{
    constexpr auto _format = "{\n"
                             "\"status\":   \"%d\",\n"
                             "\"format\":   \"%s\",\n"
                             "\"country\":  \"%s\",\n"
                             "\"dialcode\": \"%s\",\n"
                             "\"generic\": %s,\n"
                             "\"localcode\": \"%s\",\n"
                             "\"code\":     \"%d\"\n"
                             "}";
    CountryCode ct = CountryCodeDB::countryCode(param.countryCode());
    int formatFlag = beautify ? NumberFormat::Beauty : NumberFormat::Compact;
    if(localCode)
        formatFlag |= NumberFormat::Local;
    std::string jresult;
    std::string fullformat = param.format(formatFlag);
    std::string generic = param.isGenericNumber() ? "true" : "false";
    std::string localcode = ct.localCode;
    int status = param.isEmpty() ? 0 : 1;
    int l = std::snprintf(nullptr,0,_format,status,fullformat.c_str(),ct.country.c_str(),ct.dialcode.c_str(),generic.c_str(),localcode.c_str(),param.countryCode());
    jresult.resize(l);
    std::snprintf(jresult.data(),l+1,_format,status,fullformat.c_str(),ct.country.c_str(),ct.dialcode.c_str(),generic.c_str(),localcode.c_str(),param.countryCode());
    return jresult;
}

int main(int argn, char** argv)
{
    if(argn < 2)
        return 0;
    bool beautify = false;
    bool localCode = false;
    for(int x = 2; x < argn; ++x)
    {
        if(!std::strcmp("local",argv[x]))
            localCode = true;
        if(!std::strcmp("beautify",argv[x]))
            beautify = true;
    }
    NumberPreview n(argv[1]);
    // std::cout << "Raw value: " << number << std::endl;
    // std::cout << "Format: " << n.full(NumberFormat::Short) << std::endl;
    // std::cout << "Is Generic Number: " << (n.isGenericNumber()  ? "yes" : "no") << std::endl;
    // std::cout << "Is Empty: " << (n.isEmpty()  ? "yes" : "no") << std::endl;
    // std::cout << "Country: " << n.country() << std::endl;
    // std::cout << "Dial: " << n.dialCode() << std::endl;
    // std::cout << "Code: " << n.countryCode() << std::endl;
    //std::cout << std::endl;
    std::cout << to_json(n,localCode,beautify) << std::endl;
    return 0;
}
