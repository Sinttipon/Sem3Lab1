#pragma once
#include <cmath>
#include <istream>
#include <ostream>
#include <string>

struct Complex
{
    double re;
    double im;

    Complex(double r = 0, double i = 0) : re(r), im(i) {}

    double Abs() const { return std::sqrt(re * re + im * im); }

    Complex operator+(const Complex &other) const { return Complex(re + other.re, im + other.im); }
    Complex operator*(const Complex &other) const
    {
        return Complex(re * other.re - im * other.im, re * other.im + im * other.re);
    }

    bool operator<(const Complex &other) const
    {
        if (re != other.re)
            return re < other.re;
        return im < other.im;
    }
};

inline std::ostream &operator<<(std::ostream &out, const Complex &c)
{
    return out << c.re << ',' << c.im;
}

inline std::istream &operator>>(std::istream &in, Complex &c)
{
    char comma;
    if (in >> c.re >> comma >> c.im && comma != ',')
    {
        in.setstate(std::ios::failbit);
    }
    return in;
}

struct Student
{
    int id;
    std::string lastName;
    std::string firstName;
    int birthYear;

    Student(int i = 0, const std::string &last = "", const std::string &first = "", int year = 2000)
        : id(i), lastName(last), firstName(first), birthYear(year) {}

    std::string GetFullName() const { return lastName + " " + firstName; }

    bool operator<(const Student &other) const { return id < other.id; }
};

inline std::ostream &operator<<(std::ostream &out, const Student &s)
{
    return out << s.id << ';' << s.lastName << ';' << s.firstName << ';' << s.birthYear;
}

inline std::istream &operator>>(std::istream &in, Student &s)
{
    std::string token;
    if (!(in >> token))
        return in;
    size_t p1 = token.find(';');
    size_t p2 = token.find(';', p1 + 1);
    size_t p3 = token.find(';', p2 + 1);
    if (p1 == std::string::npos || p2 == std::string::npos || p3 == std::string::npos)
    {
        in.setstate(std::ios::failbit);
        return in;
    }
    try
    {
        s.id = std::stoi(token.substr(0, p1));
        s.lastName = token.substr(p1 + 1, p2 - p1 - 1);
        s.firstName = token.substr(p2 + 1, p3 - p2 - 1);
        s.birthYear = std::stoi(token.substr(p3 + 1));
    }
    catch (const std::exception &)
    {
        in.setstate(std::ios::failbit);
    }
    return in;
}