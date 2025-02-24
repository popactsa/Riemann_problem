#ifndef SOLVER_BASE_H
#define SOLVER_BASE_H

template<typename Solver_spec>
class Solver_base
{
private:
	friend Solver_spec;
	Solver_base() = default;
protected:
	void apply_boundary_conditions()
	{
		return static_cast<Solver_spec*>(this)->apply_boundary_conditions_impl();
	}
	void solve_step()
	{
		return static_cast<Solver_spec*>(this)->solve_step_impl();
	}
	void set_initial_conditions()
	{
		return static_cast<Solver_spec*>(this)->set_initial_conditions_impl();
	}
	void get_time_step()
	{
		return static_cast<Solver_spec*>(this)->get_time_step_impl();
	}
	void write_data() const
	{
		return static_cast<const Solver_spec*>(this)->write_data_impl();
	}
	void check_parameters() const
	{
		return static_cast<const Solver_spec*>(this)->check_parameters_impl();
	}
public:
	bool start()
	{
		return static_cast<Solver_spec*>(this)->start_impl();
	}
};

#endif
