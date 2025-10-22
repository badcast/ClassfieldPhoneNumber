#include <iostream>
#include <string>

#include "NumberPreview.h"

std::string to_json(const NumberPreview & param)
{
    constexpr auto _format = "{\n"
                             "\"status\":   \"%d\",\n"
                             "\"format\":   \"%s\",\n"
                             "\"country\":  \"%s\",\n"
                             "\"dialcode\": \"%s\",\n"
                             "\"generic\": %s,\n"
                             "\"code\":     \"%d\"\n"
                             "}";
    std::string jresult;
    std::string fullformat = param.full();
    std::string generic = param.isGenericNumber() ? "true" : "false";
    int status = param.isEmpty() ? 0 : 1;
    CountryCode ct = CountryCodeDB::countryCode(param.countryCode());
    int l = std::snprintf(nullptr,0,_format,status,fullformat.c_str(),ct.country.c_str(),ct.dialcode.c_str(),generic.c_str(),param.countryCode());
    jresult.resize(l);
    std::snprintf(jresult.data(),l+1,_format,status,fullformat.c_str(),ct.country.c_str(),ct.dialcode.c_str(),generic.c_str(),param.countryCode());
    return jresult;
}

int main(int argn, char** argv)
{
    if(argn != 2)
        return 0;
    NumberPreview n(argv[1]);
    // std::cout << "Raw value: " << number << std::endl;
    // std::cout << "Format: " << n.full(NumberFormat::Short) << std::endl;
    // std::cout << "Is Generic Number: " << (n.isGenericNumber()  ? "yes" : "no") << std::endl;
    // std::cout << "Is Empty: " << (n.isEmpty()  ? "yes" : "no") << std::endl;
    // std::cout << "Country: " << n.country() << std::endl;
    // std::cout << "Dial: " << n.dialCode() << std::endl;
    // std::cout << "Code: " << n.countryCode() << std::endl;
    //std::cout << std::endl;
    std::cout << to_json(n) << std::endl;
    return 0;
}
