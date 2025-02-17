#include "elasticity_Lagrange_1D_Parameters.h"
#include <iterator>

elasticity_Lagrange_1D_Parameters::viscosity elasticity_Lagrange_1D_Parameters::interp_viscosity(std::string_view str) const
{
	using enum viscosity;
	std::unordered_map<std::string_view, viscosity> tbl
	{
		{"none", none},
		{"PIC", PIC},
	};
	auto found = tbl.find(str);
	if (found != tbl.end()) return found->second;
	else
	{
		std::cerr << "Passed incorrect value : " << str << std::endl;
		std::cerr << "Possible values : " << std::endl;
		for (auto it : tbl)
		{
			std::cerr << '\t' << it.first << std::endl;
		}
		throw std::invalid_argument("Incorrect viscosity value passed");
	}
}

elasticity_Lagrange_1D_Parameters::wall::w_type elasticity_Lagrange_1D_Parameters::interp_wall_type(std::string_view str) const
{
	using enum wall::w_type;
	std::unordered_map<std::string_view, wall::w_type> tbl
	{
		{"noslip", noslip},
		{"flux", flux},
		{"piston", piston}
	};
	auto found = tbl.find(str);
	if (found != tbl.end()) return found->second;
	else
	{
		std::cerr << "Passed incorrect value : " << str << std::endl;
		std::cerr << "Possible values : " << std::endl;
		for (auto it : tbl)
		{
			std::cerr << '\t' << it.first << std::endl;
		}
		throw std::invalid_argument("Incorrect wall type value passed");
	}
}

elasticity_Lagrange_1D_Parameters::ic_preset elasticity_Lagrange_1D_Parameters::interp_ic_preset(std::string_view str) const
{
	using enum ic_preset;
	std::unordered_map<std::string_view, ic_preset> tbl
	{
		{"test1", test1},
	};
	auto found = tbl.find(str);
	if (found != tbl.end()) return found->second;
	else
	{
		std::cerr << "Passed incorrect value : " << str << std::endl;
		std::cerr << "Possible values : " << std::endl;
		for (auto it : tbl)
		{
			std::cerr << '\t' << it.first << std::endl;
		}
		throw std::invalid_argument("Incorrect ic_preset value passed");
	}
}

void elasticity_Lagrange_1D_Parameters::assign_read_value(const std::string& value, std::string_view key)
{
	// std::stoi, stod requires for std::string, so passing value as std::string_view is meaningless i think
	auto found = var_table.find(key);
	std::string_view type(found->second.first);
	void* ptr{found->second.second};
	if (!type.compare("double"))
		*(static_cast<double*>(ptr)) = std::stod(value);
	else if (!type.compare("int"))
		*(static_cast<int*>(ptr)) = std::stoi(value);
	else if (!type.compare("bool"))
		*(static_cast<bool*>(ptr)) = static_cast<bool>(std::stoi(value));
	else if (!type.compare("string"))
		*(static_cast<std::string*>(ptr)) = value;
	else if (!type.compare("ic_preset"))
		*(static_cast<ic_preset*>(ptr)) = interp_ic_preset(value);
	else if (!type.compare("viscosity"))
		*(static_cast<viscosity*>(ptr)) = interp_viscosity(value);
	expect<Error_action::logging, custom_exceptions::multiple_read_definitions>(
		[ptr, this]() {return !initialized_variables.contains(ptr); }, 
		static_cast<std::string>("Variable ") + found->first + static_cast<std::string>(" is already defined")
	); //maybe that's too expensive to have that must static_cast's
	initialized_variables.insert(ptr);
}

void elasticity_Lagrange_1D_Parameters::assign_read_wall_value(const std::string &value, std::string_view key, const ums_w_hs<std::string, std::pair<std::string, void*>>& w_table, const int n)
{
	// std::stoi, stod requires for std::string, so passing value as std::string_view is meaningless i think
	auto found = w_table.find(key);
	std::string_view type(found->second.first);
	void* ptr{found->second.second};
	if (!type.compare("double"))
		*(static_cast<double*>(ptr)) = std::stod(value);
	else if (!type.compare("w_type"))
		*(static_cast<wall::w_type*>(ptr)) = interp_wall_type(value);
	expect<Error_action::logging, custom_exceptions::multiple_read_definitions>(
		[ptr, this]() {return !initialized_variables.contains(ptr); }, 
		static_cast<std::string>("Variable ") + found->first + static_cast<std::string>(" <- wall ") + std::to_string(n) + static_cast<std::string>(" is already defined")
	); //maybe that's too expensive to have that must static_cast's
	initialized_variables.insert(ptr);
}

bool elasticity_Lagrange_1D_Parameters::does_initialized_values_contain_all_w_vars(const int n, const ums_w_hs<std::string, std::pair<std::string, void*>>& w_table) const noexcept
{
	bool result = true;
	for (const auto &it : w_table)
	{
		if (!initialized_variables.contains(it.second.second))
		{
			if (result == true) std::cerr << "Variables are not initialized : " << std::endl;
			result = false;
			std::cerr << '\t' << "Wall " << n << " : " << it.first << std::endl;
		}
	}
	return result;
}

std::string elasticity_Lagrange_1D_Parameters::set_wall_properties(std::ifstream& fin, const int n)
{
	ums_w_hs<std::string, std::pair<std::string, void*>> w_table
	{
		{"type", {"w_type", &(walls[n].type)}}
	};
	for (std::string read; std::getline(fin, read); )
	{
		if (read[0] == '#') continue;
		else if (read[0] == '\t')
		{
			std::cout << read[1] << std::endl;
			const int number_of_properties = 2;
			std::array<std::string, number_of_properties> var_read_properties{}; // name value
			split_string(read, var_read_properties);
			std::cout << var_read_properties[0] << var_read_properties[1] << std::endl;
			var_read_properties[0].erase(0, 1); // pop aligning character

			auto found_name = w_table.find(var_read_properties[0]);
			expect<Error_action::throwing, std::invalid_argument>(
				[&]() {return found_name != w_table.end(); }, 
				"Can't find read variable name in w_table"
			);
			std::string value = var_read_properties[1];
			assign_read_wall_value(value, found_name->first, w_table, n);
		}
		else
		{
			expect<Error_action::terminating, custom_exceptions::parameters_exception>(
				[this, n, &w_table]() {return does_initialized_values_contain_all_w_vars(n, w_table); }, 
				"Some wall-related variables are not initialized"
			);
			return read; // this string contains information about non-wall variables
		}
	}
	expect<Error_action::terminating, custom_exceptions::parameters_exception>(
		[this, n, &w_table]() {return does_initialized_values_contain_all_w_vars(n, w_table); }, 
		"Some wall-related variables are not initialized"
	);
	return std::string("-end-");
}

bool elasticity_Lagrange_1D_Parameters::are_all_wall_variables_initialized(const std::vector<int>& initialized) const noexcept
{
	bool result = true;
	for (int i = 0; i < number_of_walls; ++i)
	{
		auto ptr = find(initialized.begin(), initialized.end(), i);
		if (ptr == initialized.end())
		{
			if (result == true) std::cerr << "Variables are not initialized : " << std::endl;
			result = false;
			std::cerr << '\t' << "Wall " << i << std::endl;
		}
	}
	return result;
}

bool elasticity_Lagrange_1D_Parameters::are_all_general_variables_initialized() const noexcept
{
	bool result = true;
	for (auto it : var_table) // non-walls variables
	{
		auto ptr = initialized_variables.find(it.second.second);
		if (ptr == initialized_variables.end())
		{
			if (result == true) std::cerr << "Variables are not initialized : " << std::endl;
			result = false;
			std::cerr << '\t' << it.first << std::endl;
		}
	}
	return result;
}

elasticity_Lagrange_1D_Parameters::elasticity_Lagrange_1D_Parameters(std::ifstream fin)
{
	using t_format = std::chrono::milliseconds;
	const auto init_start_tick_time = std::chrono::system_clock::now();
	auto print_solve_time = print_time_between_on_exit([&]()
		{
			std::cout << "Reading from file : done!" << std::endl;
			std::cout << "Processing time : " << count_time_between_const<t_format>(init_start_tick_time, std::chrono::system_clock::now()) << " ms" << std::endl << std::endl;
			std::cout << "===================" << std::endl;
		}
	);
	if (!fin.is_open()) std::cout << "failed to open" << std::endl;
	else
	{
		const int number_of_properties = 2;
		std::vector<int> initialized_walls;
		std::string read;
		bool is_solver_name_read = false;
		while(std::getline(fin, read))
		{
			if (read[0] == '#') continue;
			if (!is_solver_name_read)
			{
				is_solver_name_read = true;
				continue;
			}
			std::array<std::string, number_of_properties> var_read_properties{}; // name value
			try
			{
				for (bool is_a_wall = true; is_a_wall;)
				{
					split_string(read, var_read_properties);
					if (var_read_properties[0] == "wall") 
					{
						expect<Error_action::throwing, std::invalid_argument>( // std::invalid_argument just to have it worked out equally to std::stoi errors and others
							[&var_read_properties]() {return std::stoi(var_read_properties[1]) <= number_of_walls; }, 
							"Wall number doesn't fit in range"
						);
						int wall_number = std::stoi(var_read_properties[1]);
						read = set_wall_properties(fin, wall_number);
						initialized_walls.push_back(wall_number);
						if (read == std::string("-end-")) break; // "-end-" is returned when wall is initialized last in file
					}
					else is_a_wall = false;
				}
				if (read == std::string("-end-")) break;
				auto found_name = var_table.find(var_read_properties[0]);
				expect<Error_action::throwing, std::invalid_argument>( // std::invalid_argument just to have it worked out equally to std::stoi errors and others
					[found_name, this]() {return found_name != var_table.end(); }, 
					"Can't find read variable name in var_table"
				);
				std::string value = var_read_properties[1];
				assign_read_value(value, found_name->first);
			}
			catch (const std::exception& err)
			{
				std::cerr << std::endl << "failure : " << err.what() << std::endl;
				std::cerr << "Read : " << read << std::endl << std::endl;
				std::terminate();
			}
		}
		expect<Error_action::terminating, custom_exceptions::parameters_exception>(
			[this]() {return are_all_general_variables_initialized(); }, 
			"Some general variables are not initialized"
		);
		expect<Error_action::terminating, custom_exceptions::parameters_exception>(
			[this, &initialized_walls]() {return are_all_wall_variables_initialized(initialized_walls); }, 
			"Some wall-related variables are not initialized"
		);
		nx_all = nx;
		for (auto it : walls)
			nx_all += it.n_fict;
		dx = (x_end - x_start) / nx;
		fin.close();
	}	
}
