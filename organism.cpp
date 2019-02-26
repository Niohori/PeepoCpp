#include "organism.h"



SensoryInputPeepo::SensoryInputPeepo(void) { ; }


SensoryInputPeepo::SensoryInputPeepo(const Peepo& peep) :
	peepo(peep)
{
	;
}


void SensoryInputPeepo::action(const std::string& node, const  std::vector<float>& prediction)
{
	std::vector<float> cpd;
	if (std::max_element(prediction.begin(), prediction.end()) - prediction.begin() == 0)
	{
		peepo.motor(get_direction(node)) = false;
	}
	else {
		peepo.motor(get_direction(node)) = true;
	}
}


std::vector<float> SensoryInputPeepo::value(const std::string& name)
{
	if (name.find(VISION) != std::string::npos){
		if (peepo.view.at(get_quadrant(name))) {
			return { 0.1,0.9 };
		}
		else {
			return {0.9, 0.1};
		}
	}
	if (name.find(MOTOR) != std::string::npos) {
		if (peepo.motor.at(get_quadrant(name))) {
			return { 0.1,0.9 };
		}
		else {
			return {0.9, 0.1};
		}
	}
	if (name.find(ID_ENNEMY) != std::string::npos) {
		if (peepo.is_an_ennemy.at(get_quadrant(name))) {
			return { 0.1,0.9 };
		}
		else {
			return {0.9, 0.1};
		}
	}
	if (name.find(ID_FOOD) != std::string::npos) {
		if (peepo.is_food[get_quadrant(name)]) {
			return { 0.1,0.9 };
		}
		else {
			return {0.9, 0.1};
		}
	}
	return { 0.5,0.5 };
}


std::string SensoryInputPeepo::get_quadrant(const std::string& name)
{
	std::string quad;
	std::vector<std::string> quadrants = { "1","2","3","4","5","6" };
	for (auto qd : quadrants)
	{
		if (name.find(qd)) { quad = qd; break; }
	}
	return quad;
}



std::string SensoryInputPeepo::get_direction(const std::string& name)
{
	std::string direction;
	for (auto dir : { LEFT, RIGHT })
	{
		if (name.find(dir)) { direction  = dir; break; }
	}
	return direction;
}

