#ifndef ORGANISM__H
#define ORGANSIM__H

#include "peepo_network.h"
#include "sensory_input.h"
#include "generative_model.h"


std::string LEFT = "left";

std::string RIGHT = "right";

std::string VISION = "vision";

std::string MOTOR = "motor";

std::string ID_ENNEMY = "ennemy";

std::string ID_FOOD = "food";


//forward declaration (temporary)

class Food;
class Ennemies;


class Peepo
{

private:
	//members
	std::vector<Ennemies> ennemies;
	std::vector<Food> food;



public:
	//members
	std::map<std::string, bool> motor;
	std::map<std::string, bool> view;
	bool is_an_ennemy;
	bool is_food;
	GenerativeModel generative_model;
	unsigned stomach;
	unsigned bang;
	//TO COMPLETE




public:
	//functions
	Peepo();
	Peepo(const std::string&, const PeepoNetwork&, const bool&,  std::vector<float>&, std::vector<Ennemies>&, std::vector<Food>&);
	void assemble_obstacles(void);
	void update(void);
	void draw(void);
	//void make_image();//
	void calculate_obstacles(void);






};

class SensoryInputPeepo :public SensoryInput
{
private:
	Peepo peepo;

public:
	SensoryInputPeepo();
	SensoryInputPeepo(const Peepo&);
	void action(const std::string&, const std::vector<float>&);
	std::vector<float> value(const std::string&);
	static std::string get_quadrant(const std::string&);
	static std::string get_direction(const std::string&);
};




#endif