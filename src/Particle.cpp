#include "Particle.h"

#include "spdlog/spdlog.h"
#include "utils/ArrayUtils.h"
#include <iostream>

Particle::Particle()
	: se_index{0}, type{0}, f{std::array<double, 3>{}},
	  old_f(std::array<double, 3>{}), baseForce{0, 0, 0}, valid{true},
	  immovable{false}, membrane{false} {
	spdlog::info("Particle generated with default constructor!");
}

Particle::Particle(int type_arg)
	: se_index{0}, type{type_arg}, f{std::array<double, 3>{}},
	  old_f(std::array<double, 3>{}), baseForce{0, 0, 0}, valid{true},
	  immovable{false}, membrane{false} {
	spdlog::info("Particle generated with type!");
}

Particle::Particle(std::array<double, 3> x_arg, std::array<double, 3> v_arg,
                   double m_arg, int type_arg)
	: se_index{0}, type{type_arg}, f{std::array<double, 3>{}},
	  old_f{std::array<double, 3>{}}, x{x_arg}, v{v_arg}, m{m_arg},
	  baseForce{0, 0, 0}, valid{true}, immovable{false}, membrane{false} {
	spdlog::info("Particle generated!");
}

Particle::Particle(std::array<double, 3> x_arg, std::array<double, 3> v_arg,
                   double m_arg, int type_arg, int index)
	: se_index{index}, type{type_arg}, f{std::array<double, 3>{}},
	  old_f{std::array<double, 3>{}}, x{x_arg}, v{v_arg}, m{m_arg},
	  baseForce{0, 0, 0}, valid{true}, immovable{false}, membrane{false} {
	spdlog::info("Particle generated with se index!");
}

Particle::Particle(const Particle &other) : se_index{other.se_index},
                                            type{other.type},
                                            f{other.f},
                                            old_f{other.old_f},
                                            x{other.x},
                                            v{other.v},
                                            m{other.m},
                                            baseForce{other.baseForce},
                                            gridIndex{other.gridIndex},
                                            oldX{other.oldX},
                                            valid{other.valid},
                                            immovable{other.immovable},
                                            membrane{other.membrane} {
	spdlog::info("Particle generated by copy");
}

Particle &Particle::operator=(const Particle &other) {
	if ((*this).operator==(const_cast<Particle &>(other)))
		return (*this);
	x = other.x;
	v = other.v;
	f = other.f;
	old_f = other.old_f;
	m = other.m;
	type = other.type;
	se_index = other.se_index;
	valid = other.valid;
	baseForce = other.baseForce;
	membrane = other.membrane;
	immovable = other.immovable;
	gridIndex = other.gridIndex;
	oldX = other.oldX;
	return (*this);
}

Particle::Particle(Particle &&other) noexcept: se_index{other.se_index},
                                               type{other.type},
                                               f{other.f},
                                               old_f{other.old_f},
                                               x{other.x},
                                               v{other.v},
                                               m{other.m},
                                               baseForce{other.baseForce},
                                               gridIndex{other.gridIndex},
                                               oldX{other.oldX},
                                               valid{true},
                                               immovable{other.immovable},
                                               membrane{other.membrane} {
	other.type = 0;
	other.f = {};
	other.old_f = {};
	other.x = {};
	other.v = {};
	other.m = 0;
	other.valid = false;
	other.membrane = false;
	other.gridIndex = {};
	spdlog::info("Particle generated by moving!");
}

Particle &Particle::operator=(Particle &&other) noexcept {
	if ((*this).operator==(const_cast<Particle &>(other)))
		return (*this);
	x = other.x;
	v = other.v;
	f = other.f;
	old_f = other.old_f;
	m = other.m;
	type = other.type;
	se_index = other.se_index;
	valid = other.valid;
	baseForce = other.baseForce;
	membrane = other.membrane;
	immovable = other.immovable;
	gridIndex = other.gridIndex;
	oldX = other.oldX;
	other.type = 0;
	other.f = {};
	other.old_f = {};
	other.x = {};
	other.v = {};
	other.m = 0;
	other.valid = false;
	other.baseForce = {};
	other.membrane = false;
	other.gridIndex = {};
	return (*this);
}

Particle::~Particle() {
	spdlog::info("Particle destructed!");
}

const std::array<double, 3> &Particle::getX() const {
	return x;
}

const std::array<double, 3> &Particle::getV() const {
	return v;
}

const std::array<double, 3> &Particle::getF() const {
	return f;
}

const std::array<double, 3> &Particle::getOldF() const {
	return old_f;
}

double Particle::getM() const {
	return m;
}

int Particle::getType() const {
	return type;
}

int Particle::getSEIndex() const {
	return se_index;
}

const std::array<double, 3> &Particle::getOldX() const {
	return oldX;
}

void Particle::setOldX(std::array<double, 3> pos) {
	oldX = pos;
}

void Particle::setX(int d, double val) {
	x[d] = val;
}

void Particle::setX(std::array<double, 3> pos) {
	x = pos;
}

void Particle::setV(int d, double val) {
	v[d] = val;
}

void Particle::setV(std::array<double, 3> vel) {
	v = vel;
}

void Particle::setF(int d, double val) {
	f[d] = val;
}

void Particle::setF(std::array<double, 3> force) {
	f = !immovable * force;
}

void Particle::setOldF(const std::array<double, 3> &val) {
	old_f = val;
}

void Particle::setM(double val) {
	m = val;
}

void Particle::setType(int val) {
	type = val;
}

std::string Particle::toString() const {
	std::stringstream stream;
	stream << "Particle: X:" << x << " v: " << v << " f: " << f << " m: " << m
	       << " old_f: " << old_f << " type: " << type;
	return stream.str();
}

bool Particle::operator==(Particle &other) {
	return (x == other.x) and (v == other.v) and (f == other.f) and
		(type == other.type) and (m == other.m) and (old_f == other.old_f);
}

bool Particle::operator!=(Particle &other) {
	return !((*this) == other);
}

std::ostream &operator<<(std::ostream &stream, Particle &p) {
	stream << p.toString();
	return stream;
}

void to_json(nlohmann::json &j, const Particle &p) {
	j = nlohmann::json{{"type", p.getType()}, {"se_index", p.getSEIndex()},
	                   {"force", p.getF()}, {"oldForce", p.getOldF()},
	                   {"pos", p.getX()}, {"vel", p.getV()},
	                   {"mass", p.getM()}};
}

void from_json(const nlohmann::json &j, Particle &p) {
	j.at("type").get_to(p.type);
	j.at("se_index").get_to(p.se_index);
	j.at("force").get_to(p.f);
	j.at("oldForce").get_to(p.old_f);
	j.at("pos").get_to(p.x);
	j.at("vel").get_to(p.v);
	j.at("mass").get_to(p.m);
}

void Particle::setBaseForce(std::array<double, 3> &force) {
	baseForce = force;
}

void Particle::applyBaseForceAndGrav(std::array<double, 3> grav) {
	this->setF(this->baseForce + (m * grav));
	//f[1] = f[1] + grav * m;
}

const std::array<int, 3> &Particle::getGridIndex() const {
	return gridIndex;
}

void Particle::setGridIndex(std::array<int, 3> &index) {
	gridIndex = index;
}

void Particle::setMembrane(bool mem) {
	membrane = mem;
}

void Particle::setImmovable(bool imov) {
	immovable = imov;
}

void Particle::setPassedPeriodic(int d) {
	switch (d) {
		case 0: passedPeriodicX = !passedPeriodicX;
			break;
		case 1: passedPeriodicY = !passedPeriodicY;
			break;
		case 2: passedPeriodicZ = !passedPeriodicZ;
			break;
		default:;
	}
}

bool Particle::isPassedPeriodicY() const {
	return passedPeriodicY;
}

bool Particle::isPassedPeriodicX() const {
	return passedPeriodicX;
}

bool Particle::isPassedPeriodicZ() const {
	return passedPeriodicZ;
}

bool Particle::operator<(const Particle &other) noexcept {
	if (this->x[2] < other.x[2]) {
		return true;
	} else if (this->x[2] > other.x[2]) {
		return false;
	}
	if (this->x[1] < other.x[1]) {
		return true;
	} else if (this->x[1] > other.x[1]) {
		return false;
	}
	if (this->x[0] < other.x[0]) {
		return true;
	} else {
		return false;
	}
}

const std::array<double, 3> &Particle::getBaseForce() const {
	return baseForce;
}
