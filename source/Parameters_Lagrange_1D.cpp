#include "Parameters_Lagrange_1D.h"

void* Parameters_Lagrange_1D::get_wall_property_impl(std::string_view variable_name, void* object) const
{
	if (!variable_name.compare("P"))
		return &(static_cast<wall*>(object)->P);
	else if (!variable_name.compare("type"))
		return &(static_cast<wall*>(object)->type);
	else if (!variable_name.compare("n_fict"))
		return &(static_cast<wall*>(object)->n_fict);
	else if (!variable_name.compare("v"))
		return &(static_cast<wall*>(object)->v);
}
