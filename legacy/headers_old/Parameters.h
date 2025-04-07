#ifndef PARAMETERS_H
#define PARAMETERS_H

struct Parameters_common
{
	enum class Object_group
	{
		NONE,
		WALL
	};
};

template <typename Parameters_spec>
class Parameters: public Parameters_common
{
private:
	friend Parameters_spec;
	Parameters() = default;
public:
};
#endif
