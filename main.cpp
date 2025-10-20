#include <iostream>
#include <string>

#include "NumberPreview.h"

int main()
{
    std::string number;
    while(!std::cin.eof())
    {
    std::getline(std::cin, number);
    NumberPreview n(number);
    std::cout << "Raw value: " << number << std::endl;
    std::cout << "Format: " << n.full(NumberFormat::Short) << std::endl;
    std::cout << "Is Generic Number: " << (n.isGenericNumber()  ? "yes" : "no") << std::endl;
    std::cout << "Is Empty: " << (n.isEmpty()  ? "yes" : "no") << std::endl;
    std::cout << "Country: " << n.country() << std::endl;
    std::cout << "Dial: " << n.dialCode() << std::endl;
    std::cout << "Code: " << n.countryCode() << std::endl;
    std::cout << std::endl;
    }
    return 0;
}
