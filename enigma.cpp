#include <iostream>
using namespace std;

class Rotor{
		int count;
		string alph;
	public:
		Rotor();
		char scramble(char);
};

class Reflector{
		string alph;
	public:
		Reflector();
		char reflect(char);
};

class Plugboard{
		string alph;
	public:
		Plugboard();
		char scramble(char);
};

class Enigma{
		Plugboard plugboard;
		Rotor right_rotor;
		Rotor middle_rotor;
		Rotor left_rotor;
		Reflector reflector;
	public:
		Enigma();
		void set();
		char encode(char);
};

int main(){
		return 0;
}
