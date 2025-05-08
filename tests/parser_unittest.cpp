#include <gtest/gtest.h>

#include "Parser.h"
#include "Solver_Lagrange_1D.h"
#include "parser_test.h"
#include "parsing_line.h"
#include <array>

#include <sys/ioctl.h>
#include <unistd.h>

struct winsize w;
namespace {
TEST(ParserTest, DifferentTypeCombinations)
{
    ScenParsingLine::SetSep(' ');
    std::string str;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    str = "name 0.0";
    EXPECT_TRUE(IsCommonType(str) && !IsArrayType(str) && !IsNamedType(str));

    str = "name : 0.0 1.0 3.0";
    EXPECT_TRUE(IsCommonType(str) && IsArrayType(str) && !IsNamedType(str));

    str = "name 1 : a 1.0 b 2.0";
    EXPECT_TRUE(IsNamedType(str) && IsArrayType(str) && !IsCommonType(str));

    str = "name : a 1.0 b 2.0";
    EXPECT_TRUE(IsNamedType(str) && !IsArrayType(str) && !IsCommonType(str));
}

TEST(ParserTest, AssigningValues)
{
    ScenParsingLine::SetSep(' ');
    ScenParsingLine line;
    std::string str;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    {
        double name;
        str = "name 0.0";
        line.Load(str);
        Parse(&name, line);
        EXPECT_TRUE(name == 0.0);
    }
    {
        Viscosity<Solver_Lagrange_1D> name;
        str = "name Linear";
        line.Load(str);
        Parse(&name, line);
        EXPECT_TRUE(name.value_ == Viscosity<Solver_Lagrange_1D>::E::qLinear);
    }

    {
        Wall<Solver_Lagrange_1D> name;
        str = "name : P 1.0 v 2.0 type FreeFlux";
        line.Load(str);
        Parse(&name, line);
        EXPECT_TRUE(name.P
                    == 1.0
                    && name.v
                    == 2.0
                    && name.type.value_
                    == WallType<Solver_Lagrange_1D>::E::qFreeFlux);
    }

    {
        std::array<Wall<Solver_Lagrange_1D>, 2> name;
        str = "name 1 : P 1.0 v 2.0 type FreeFlux";
        line.Load(str);
        Parse(&name, line);
        EXPECT_TRUE(name[1].P
                    == 1.0
                    && name[1].v
                    == 2.0
                    && name[1].type.value_
                    == WallType<Solver_Lagrange_1D>::E::qFreeFlux);
    }
    {
        std::array<int, 3> name;
        str = "name : 1 2 3";
        line.Load(str);
        Parse(&name, line);
        EXPECT_TRUE(name[0] == 1 && name[1] == 2 && name[2] == 3);
    }
}
} // namespace
