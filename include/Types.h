#pragma once
#include <cmath>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>

struct Complex {
    double re;
    double im;

    Complex(double r = 0, double i = 0) : re(r), im(i) {}

    double Abs() const { return std::sqrt(re * re + im * im); }
};

inline Complex operator+(const Complex& a, const Complex& b) { return Complex(a.re + b.re, a.im + b.im); }

inline Complex operator*(const Complex& a, const Complex& b) {
    return Complex(a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re);
}

inline bool operator<(const Complex& a, const Complex& b) {
    if (a.re != b.re) return a.re < b.re;
    return a.im < b.im;
}

inline std::ostream& operator<<(std::ostream& out, const Complex& c) { return out << c.re << ',' << c.im; }

inline std::istream& operator>>(std::istream& in, Complex& c) {
    double re = 0;
    double im = 0;
    char comma = 0;
    if (in >> re >> comma >> im && comma == ',') c = Complex(re, im);
    else in.setstate(std::ios::failbit);
    return in;
}

struct Student {
    int id;
    std::string lastName;
    std::string firstName;
    int birthYear;

    Student(int i = 0, const std::string& last = "", const std::string& first = "", int year = 2000)
        : id(i), lastName(last), firstName(first), birthYear(year) {}

    std::string GetFullName() const { return lastName + " " + firstName; }
};

inline bool operator<(const Student& a, const Student& b) { return a.id < b.id; }

inline std::ostream& operator<<(std::ostream& out, const Student& s) {
    return out << s.id << ';' << s.lastName << ';' << s.firstName << ';' << s.birthYear;
}

inline std::istream& operator>>(std::istream& in, Student& s) {
    std::string token;
    if (!(in >> token)) return in;
    for (char& c : token) {
        if (c == ';') c = ' ';
    }
    std::istringstream fields(token);
    Student result;
    if (fields >> result.id >> result.lastName >> result.firstName >> result.birthYear && (fields >> std::ws).eof())
        s = result;
    else
        in.setstate(std::ios::failbit);
    return in;
}
