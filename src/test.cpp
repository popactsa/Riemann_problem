#include <iostream>

enum class SpecChar : char { qNotSet = 0, qSpec1 = ':', qSpec2 = '!' };

int main()
{
    char c{'.'};
    switch (static_cast<SpecChar>(c)) {
        using enum SpecChar;
    case qNotSet:
        std::cout << "notset" << std::endl;
        break;
    case qSpec1:
        std::cout << "Spec1" << std::endl;
        break;
    case qSpec2:
        std::cout << "Spec2" << std::endl;
        break;
    }
    return 0;
}
