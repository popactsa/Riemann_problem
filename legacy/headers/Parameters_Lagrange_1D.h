#ifndef PARAMETERS_LAGRANGE_1D_H
#define PARAMETERS_LAGRANGE_1D_H

#include <array>
#include <cstring>
#include <set>
#include <string>

#include "Parameters.h"
#include "Parser.h"
#include "custom.h"
#include "io_auxiliary.h"

class Parameters_Lagrange_1D : public Parameters<Parameters_Lagrange_1D> {
private:
    friend class Lagrange_1D;

    int nx_all, nx, nt;
    double CFL, dx, x_start, x_end;
    double gamma, mu0 = 2;
    bool is_conservative;

    int nt_write;
    std::string write_file;

    enum class viscosity { none, Neuman, Latter, linear, sum } visc;

    enum class ic_preset { test1, test2, test3, test4 } ic;

    struct wall {
        enum class w_type { noslip, flux } type;
        int n_fict = 1; // set implicitly depending on type(will make it later,
                        // depending on solver)
        double P, v;
    };
    static inline unordered_map_w_shs<std::string_view, std::string_view>
        wall_properties_tbl{{"type", "w_type"},
                            {"n_fict", "int"},
                            {"P", "double"},
                            {"v", "double"}};
    enum { number_of_walls = 2 };
    std::array<wall, number_of_walls> walls;

    using enum OBJECT_GROUP;
    unordered_map_w_shs<std::string_view,
                        std::tuple<std::string_view, OBJECT_GROUP, void*>>
        parsing_tbl{{"x_start", {"double", NONE, &x_start}},
                    {"x_end", {"double", NONE, &x_end}},
                    {"nx", {"int", NONE, &nx}},
                    {"CFL", {"double", NONE, &CFL}},
                    {"nt", {"int", NONE, &nt}},
                    {"nt_write", {"int", NONE, &nt_write}},
                    {"write_file", {"string", NONE, &write_file}},
                    {"gamma", {"double", NONE, &gamma}},
                    {"mu0", {"double", NONE, &mu0}},
                    {"viscosity", {"viscosity", NONE, &visc}},
                    {"initial_conditions", {"ic_preset", NONE, &ic}},
                    {"is_conservative", {"bool", NONE, &is_conservative}},
                    {"wall", {"wall", WALL, walls.data()}}};
    std::set<void*> initialized_variables{
        &mu0 // adding default-initialized variables
    };

    static std::tuple<std::string_view, OBJECT_GROUP, void*>
    get_WALL_object_variable_info(std::string_view, void*);
    static viscosity interp_viscosity(std::string_view);
    static ic_preset interp_ic_preset(std::string_view);
    static wall::w_type interp_w_type(std::string_view);
public:
    Parameters_Lagrange_1D(const std::filesystem::path& path)
    {
        start_Parser(*this, path);
    };
    static std::tuple<std::string_view, OBJECT_GROUP, void*>
    get_object_variable_info_impl(std::string_view,
                                  const std::pair<OBJECT_GROUP, void*>&);
    static void
    assign_enum_variable_impl(std::string_view, std::string_view, void*);
    void initialize_dependent_variables_impl();
    inline std::set<void*>* get_initialized_variables_impl()
    {
        return &initialized_variables;
    }
    inline unordered_map_w_shs<
        std::string_view,
        std::tuple<std::string_view, OBJECT_GROUP, void*>>*
    get_parsing_tbl_impl()
    {
        return &parsing_tbl;
    }
};

#endif
