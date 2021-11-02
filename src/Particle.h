#pragma once

#include <array>
#include <string>

class Particle {

private:

    /**
     * Type of the particle. Use it for whatever you want (e.g. to separate
     * molecules belonging to different bodies, matters, and so on)
     */
    int type;

    /**
     * Force effective on this particle
     */
    std::array<double, 3> f{};

    /**
     * Force which was effective on this particle
     */
    std::array<double, 3> old_f{};

    /**
     * Position of the particle
     */
    std::array<double, 3> x{};

    /**
     * Velocity of the particle
     */
    std::array<double, 3> v{};

    /**
     * Mass of this particle
     */
    double m{};

public:

    /**
     * Normal constructor
     * @param type type of the particle
     */
    explicit Particle(int type = 0);

    /**
     * Copy constructor
     * @param other object it is copied from
     */
    Particle(const Particle &other);

    /**
     * Copy assignment operator
     * @param other object it is copied from
     * @return returning the copy
     */
    Particle &operator=(const Particle &other);


    /**
     * Overloaded constructor
     * @param x_arg position
     * @param v_arg velocity
     * @param m_arg mass
     * @param type type
     */
    Particle(
            // for visualization, we need always 3 coordinates
            // -> in case of 2d, we use only the first and the second
            std::array<double, 3> x_arg, std::array<double, 3> v_arg, double m_arg,
            int type = 0);

    virtual ~Particle();

    [[nodiscard]] const std::array<double, 3> &getX() const;

    [[nodiscard]] const std::array<double, 3> &getV() const;

    [[nodiscard]] const std::array<double, 3> &getF() const;

    [[nodiscard]] const std::array<double, 3> &getOldF() const;

    [[nodiscard]] double getM() const;

    [[nodiscard]] int getType() const;

    /**
     * @param d: dimension
     * @param v: value
     */
    void setX(int d, double v);

    /**
     * @param d: dimension
     * @param v: value
     */
    void setV(int d, double v);

    /**
     * @param d: dimension
     * @param v: value
     */
    void setF(int d, double v);

    /**
     * @param val the old force array
     */
    void setOldF(const std::array<double, 3> & val);

    void setM(double);

    void setType(int);

    /**
     * Class-specific equals operator
     * @return true if equal, false if not
     */
    bool operator==(Particle &other);

    /**
     * Class-specific not-equals operator
     * @param other object it is compared to
     * @return true if not equal, false if equal
     */
    bool operator!=(Particle &other);

    /**
     * Turns the particle into a string
     * @return the string
     */
    [[nodiscard]] std::string toString() const;
};

/**
 * Overloaded << operator for easy console prints
 * @param stream the stream the content is pushed into into
 * @param p the particle
 * @return the stream reference for further pushing into to stream
 */
std::ostream &operator<<(std::ostream &stream, Particle &p);
