#include <iostream>
#include <string>
#include <fstream>

#include "NumberPreview.h"

int main()
{
    std::string number;
    std::fstream f("/media/storage/dev/FixNumber/numbers.txt");

    while(!f.eof())
    {
        std::getline(f, number);
        if(number.empty())
            continue;
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
