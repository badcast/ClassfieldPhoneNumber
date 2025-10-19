#include <iostream>
#include <string>

#include "NumberPreview.h"
#include "NumberPreviewOld.h"

int main()
{
    std::string number = "+77021094814";
    NumberPreviewOld n(number);
    std::cout << "Raw value: " << number << std::endl;
    std::cout << "Full format (beauty): " << n.full(NumberFormat::Beauty) << std::endl;
    std::cout << "Is Generic Number: " << (n.isGenericNumber()  ? "yes" : "no") << std::endl;
    std::cout << "Is Empty: " << (n.isEmpty()  ? "yes" : "no") << std::endl;
    std::cout << "Country: " << n.country << std::endl;
    std::cout << "Dial: " << n.dialcode << std::endl;
    std::cout << "Code: " << n.codes[0] << std::endl;
    return 0;
}
