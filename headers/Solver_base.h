#ifndef SOLVER_BASE_H
#define SOLVER_BASE_H

template<typename Solver_spec>
class Solver_base : public Solver_spec
{
private:
	friend Solver_spec;
	Solver_base() = default;
public:
	virtual void apply_boundary_conditions() = 0;
	virtual void solve_step() = 0;
	virtual void set_initial_conditions() = 0;
	virtual void get_time_step() = 0;
	virtual void write_data() = 0;
	virtual bool start() = 0;
	virtual void check_parameters() = 0;
};

#endif
