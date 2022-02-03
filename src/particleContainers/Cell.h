#pragma once

#include <vector>
#include "../Particle.h"

struct Cell {
	/**
	 * Normal default constructor
	 */
	Cell();

	void setParticles(const std::vector<Particle *> &particlesV) {
		Cell::particles = particlesV;
	}

	std::vector<Particle *> &getParticles() {
		return particles;
	}

	[[nodiscard]] const std::vector<Particle *> &getParticles() const {
		return particles;
	}

	/**
	 * @brief Provides the iterator for single particles at the start of the collection
	 * @return iterator
	 */
	std::vector<Particle *>::iterator begin();

	/**
	 * @brief Provides the iterator for single particles at the end of the collection
	 * @return iterator
	 */
	std::vector<Particle *>::iterator end();

	/**
	 * @brief Provides the _const_ iterator for single particles at the start of the collection
	 * @return const iterator
	 */
	[[nodiscard]] std::vector<Particle *>::const_iterator begin() const;

	/**
	 * @brief Provides the _const_ iterator for single particles at the end of the collection
	 * @return const iterator
	 */
	[[nodiscard]] std::vector<Particle *>::const_iterator end() const;

	/**
	 * Adds a particle to the cell
	 */
	void add_particle(Particle &);

	/**
	 * Deletes all the pointers in the particle list
	 */
	void clear();

	/**
	 * @brief Moves an object to the collection, i.e. uses one of the constructor of the Particle class
	 * and creates an object directly
	 * @tparam Args Parameter pack
	 * @param args The parameter for the constructor
	 */
	template<typename... Args>
	void emplace_back(Args &&... args);

	/*
	 * Removes particle at the position
	 */
	std::vector<Particle *>::iterator erase(std::vector<Particle *>::const_iterator position);

	/**
	 * Prints a string for debugging purposes
	 */
	void toString();

	[[nodiscard]] const std::array<int, 3> &getIndex() const;

	void setIndex(const std::array<int, 3> &indexV);

	/**
	 * Sets the neighbors of the cell along the X axis in 2D
	 */
	void setNeighbors2DInX();

	/**
	 * Sets the neighbors of the cell along the X axis in 3D
	 */
	void setNeighbors3DInX();

	/**
	 * Sets the neighbors of the cell along the Y axis in 2D
	 */
	void setNeighbors2DInY();

	/**
	 * Sets the neighbors of the cell along the Y axis in 3D
	 */
	void setNeighbors3DInY();

	/**
	 * Sets the neighbors of the cell along the Z axis in 3D
	 */
	void setNeighbors3DInZ();

	/**
	 * Sets the remaining neighbors (along the Y axis since it is the default) in 2D
	 */
	void setRemainingNeighbors2D();

	/**
	 * Sets the remaining neighbors (along the Y axis since it is the default) in 3D
	 */
	void setRemainingNeighbors3D();

	[[nodiscard]] const std::vector<std::array<int, 3>> &getNeighbors2D() const;

	void setNeighbors2D1(const std::vector<std::array<int, 3>> &neighbors_2_d);

	[[nodiscard]] const std::vector<std::array<int, 3>> &getNeighbors3D() const;

	void setNeighbors3D1(const std::vector<std::array<int, 3>> &neighbors_3_d);

	[[nodiscard]] bool isBorderCell1() const;

	void setIsBorderCell(bool is_border_cell);

	[[nodiscard]] const std::vector<std::array<int, 3>> &getRemainingNeighbors2D() const;

	[[nodiscard]] const std::vector<std::array<int, 3>> &getRemainingNeighbors3D() const;

private:

	/**
	 * The Particles in each cell
	 */
	std::vector<Particle *> particles;

	/**
	 * The index of the cell
	 */
	std::array<int, 3> index{};

	/**
	 * The 2D neighbors of this cell (default along the y-axis)
	 */
	std::vector<std::array<int, 3>> neighbors2D;

	/**
	 * The 3D neighbors of this cell (default along the y-axis)
	 */
	std::vector<std::array<int, 3>> neighbors3D;

	/**
	 * The remaining neighbors (along the y-axis) of the cell in 2D
	 */
	std::vector<std::array<int, 3>> remainingNeighbors2D;

	/**
	 * The remaining neighbors (along the y-axis) of the cell in 3D
	 */
	std::vector<std::array<int, 3>> remainingNeighbors3D;

	/**
	 * Indicates whether the cel is a border cell
	 */
	bool isBorderCell{};

};

template<typename... Args>
void Cell::emplace_back(Args &&... args) {
	particles.emplace_back(std::forward<Args>(args)...);
}

