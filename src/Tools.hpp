#include <iostream>
#include <exception>

using namespace std;

double String2Float(const string& in) {
	int i = 0;
	int Int = 0;
	double Frac = 0, Pow = 1;
	bool positive = true;
	while (i < in.length() && (in[i] == '+' || in[i] == '-')) {
		positive = (in[i] == '-') ? !positive : positive;
		i++;
	}
	if (i == in.length()) throw logic_error("Not a valid float number");
	if (i == in.length() - 1 && in[i] == '.') throw logic_error("Not a valid float number");
	for (; i < in.length() && in[i] <= '9' && in[i] >= '0'; i++)
		Int = Int * 10 + in[i] - '0';
	if (i == in.length()) return (positive) ? Int : -Int;
	if (in[i] != '.') throw logic_error("Not a valid float number");
	for (i += 1; i < in.length() && in[i] <= '9' && in[i] >= '0'; i++) {
		Pow /= 10;
		Frac += (in[i] - '0') * Pow;
	}
	if (i == in.length()) return (positive) ? Int + Frac : -Int - Frac;
	throw logic_error("Not a valid float number");
}

int String2Int(const string& in) {
	int i = 0;
	int Int = 0;
	bool positive = true;
	while (i < in.length() && (in[i] == '+' || in[i] == '-')) {
		positive = (in[i] == '-') ? !positive : positive;
		i++;
	}
	if (i == in.length()) throw logic_error("Not a valid integer number");
	for (; i < in.length() && in[i] <= '9' && in[i] >= '0'; i++)
		Int = Int * 10 + in[i] - '0';
	if (i == in.length()) return (positive) ? Int : -Int;
	throw logic_error("Not a valid integer number");
}