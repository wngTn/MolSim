/*
 * Particle.cpp
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#include "Particle.h"

#include <iostream>
#include "utils/ArrayUtils.h"

log4cxx::LoggerPtr Particle::particleLogger(log4cxx::Logger::getLogger("MolSim.Particle"));

Particle::Particle(int type_arg) : type{type_arg}, f{std::array<double, 3>{}}, old_f(std::array<double, 3>{}) {
    LOG4CXX_DEBUG(particleLogger, "Particle generated with type!");
}

Particle::Particle(const Particle &other) : x{other.x}, v{other.v}, f{other.f}, old_f{other.old_f},
m{other.m}, type{other.type} {
    LOG4CXX_DEBUG(particleLogger, "Particle generated by copy!");
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
    return (*this);
}

Particle::Particle(std::array<double, 3> x_arg, std::array<double, 3> v_arg,
                   double m_arg, int type_arg) : x{x_arg}, v{v_arg}, m{m_arg}, type{type_arg},
                   f{std::array<double, 3>{}}, old_f{std::array<double, 3>{}}{
    LOG4CXX_DEBUG(particleLogger, "Particle generated!");
}

Particle::~Particle() { LOG4CXX_DEBUG(particleLogger, "Particle destructed!"); }

const std::array<double, 3> &Particle::getX() const { return x; }

const std::array<double, 3> &Particle::getV() const { return v; }

const std::array<double, 3> &Particle::getF() const { return f; }

const std::array<double, 3> &Particle::getOldF() const { return old_f; }

double Particle::getM() const { return m; }

int Particle::getType() const { return type; }

void Particle::setX(int d, double val) {
    x.at(d) = val;
}

void Particle::setV(int d, double val) {
    v.at(d) = val;
}

void Particle::setF(int d, double val) {
    f.at(d) = val;
}

void Particle::setOldF(const std::array<double, 3> & val) {
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
  stream << "Particle: X:" << x << " v: " << v << " f: " << f
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
