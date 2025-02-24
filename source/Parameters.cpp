#include "Parameters.h"

template <typename Parameters_spec>
void* Parameters<Parameters_spec>::get_wall_property_impl(std::string_view name, void* ptr) const
{
	return static_cast<const Parameters_spec*>(this)->get_wall_property(name, ptr);
}

