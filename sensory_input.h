#ifndef SENSORY_INPUT__H
#define SENSORY_INPUT__H


class SensoryInput {
public:
	virtual void action(const std::string&, const std::vector<float>&);
	virtual std::vector<float> value(const std::string&);
};

#endif
