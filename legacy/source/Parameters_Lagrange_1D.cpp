#include "Parameters.h"
#include "Parameters_Lagrange_1D.h"

std::tuple<std::string_view, Parameters_common::OBJECT_GROUP, void*>
Parameters_Lagrange_1D::get_object_variable_info_impl(
    std::string_view object_variable_name,
    const std::pair<OBJECT_GROUP, void*>& object_info)
{
    using enum OBJECT_GROUP;
    auto& [object_group, object_ptr] = object_info;
    switch (object_group) {
    case NONE:
        std::cout << "Something is wrong" << std::endl;
        return {"none", NONE, nullptr};
    case WALL:
        return get_WALL_object_variable_info(object_variable_name, object_ptr);
    }
}

std::tuple<std::string_view, Parameters_common::OBJECT_GROUP, void*>
Parameters_Lagrange_1D::get_WALL_object_variable_info(
    std::string_view object_variable_name,
    void* object_ptr)
{
    void* result_ptr = nullptr;
    if (!object_variable_name.compare("P"))
        result_ptr = &(static_cast<wall*>(object_ptr)->P);
    else if (!object_variable_name.compare("type"))
        result_ptr = &(static_cast<wall*>(object_ptr)->type);
    else if (!object_variable_name.compare("n_fict"))
        result_ptr = &(static_cast<wall*>(object_ptr)->n_fict);
    else if (!object_variable_name.compare("v"))
        result_ptr = &(static_cast<wall*>(object_ptr)->v);
    std::string_view object_variable_type =
        wall_properties_tbl.find(object_variable_name)->second;
    return {object_variable_type, WALL, result_ptr};
}

void Parameters_Lagrange_1D::initialize_dependent_variables_impl()
{
    nx_all = nx;
    for (auto it : walls) {
        nx_all += it.n_fict;
    }
    dx = (x_end - x_start) / static_cast<double>(nx);
}

Parameters_Lagrange_1D::viscosity
Parameters_Lagrange_1D::interp_viscosity(std::string_view variable_value)
{
    using enum Parameters_Lagrange_1D::viscosity;
    unordered_map_w_shs<std::string_view, Parameters_Lagrange_1D::viscosity>
        tbl{{"none", none},
            {"Neuman", Neuman},
            {"Latter", Latter},
            {"linear", linear},
            {"sum", sum}};
    auto found = tbl.find(variable_value);
    if (found != tbl.end())
        return found->second;
    else {
        std::cerr << "Passed incorrect value : " << variable_value << std::endl;
        std::cerr << "Possible values : " << std::endl;
        for (auto it : tbl) {
            std::cerr << '\t' << it.first << std::endl;
        }
        throw std::invalid_argument("Incorrect viscosity value passed");
    }
}

Parameters_Lagrange_1D::ic_preset
Parameters_Lagrange_1D::interp_ic_preset(std::string_view read_value)
{
    using enum parameters_lagrange_1d::ic_preset;
    unordered_map_w_shs<std::string_view, parameters_lagrange_1d::ic_preset>
        tbl{{"test1", test1},
            {"test2", test2},
            {"test3", test3},
            {"test4", test4}};
    auto found = tbl.find(read_value);
    if (found != tbl.end())
        return found->second;
    else {
        std::cerr << "passed incorrect value : " << read_value << std::endl;
        std::cerr << "possible values : " << std::endl;
        for (auto it : tbl) {
            std::cerr << '\t' << it.first << std::endl;
        }
        throw std::invalid_argument("incorrect ic_preset value passed");
    }
}

Parameters_Lagrange_1D::wall::w_type
Parameters_Lagrange_1D::interp_w_type(std::string_view read_value)
{
    using enum Parameters_Lagrange_1D::wall::w_type;
    unordered_map_w_shs<std::string, Parameters_Lagrange_1D::wall::w_type> tbl{
        {"noslip", noslip}, {"flux", flux}};
    auto found = tbl.find(read_value);
    if (found != tbl.end())
        return found->second;
    else {
        std::cerr << "Passed incorrect value : " << read_value << std::endl;
        std::cerr << "Possible values : " << std::endl;
        for (auto it : tbl) {
            std::cerr << '\t' << it.first << std::endl;
        }
        throw std::invalid_argument("Incorrect wall type value passed");
    }
}

void Parameters_Lagrange_1D::assign_enum_variable_impl(
    std::string_view variable_type,
    std::string_view read_value,
    void* variable_ptr)
{
    if (!variable_type.compare("ic_preset"))
        *(static_cast<Parameters_Lagrange_1D::ic_preset*>(variable_ptr)) =
            interp_ic_preset(read_value);
    else if (!variable_type.compare("w_type"))
        *(static_cast<Parameters_Lagrange_1D::wall::w_type*>(variable_ptr)) =
            interp_w_type(read_value);
    else if (!variable_type.compare("viscosity"))
        *(static_cast<Parameters_Lagrange_1D::viscosity*>(variable_ptr)) =
            interp_viscosity(read_value);
}
