#include <cstdlib>
#include <iostream>
#include <string.h>
#include <vector>
using namespace std;

#define ALPH "abcdefghijklmnopqrstuvwxyz"
#define DISALPH "zyxwvutsrqponmlkjihgfedcba"

template <class T> class Ring{
	private:
		T *buf;
		const int size;
		int head;
	public:
		Ring(int n): size(n){
			buf = new T[n];
			head = 0;
			//printf("n:%d\n", n);
		}
		~Ring(){
			delete [] buf;
		}

		void set(T *t){
			for(int i = 0; i < size; i++){
				buf[i] = t[i];
			}
		}

		void rotate(int n){
			head = (head + n) % size;
		}

		void reset(){ head = 0;}

		T access(int index){
			int i = (head + index) % size;
			if(i < 0)i += size;
			//printf("i: %d, head: %d, index: %d, size: %d\n", i, head, index, size);
			return buf[i];
		}

		int find(T t){
			for(int i = head; i < size + head; i++){
				int index = i % size;
				if(buf[index] == t)return i - head;
			}
			return -1;
		}

		T *get_buf(){ return buf;}
		int get_head(){ return head;}
};

template <class T> void del(T *t, int size, int index){
	for(int i = index; i < size -1; i++)t[i] = t[i+1];
}

int find(char *s, int size, char c){
	for(int i = 0; i < size; i++)
		if(s[i] == c)return i;
	return -1;
}

void perm(int n, char *s, int size){
	//シードを固定
	srand(n);

	char tmp[size];
	for(int i = 0; i < size; i++)tmp[i] = s[i];

	for(int i = 0; i < size; i++){
		int index = rand() % (size - i);
		s[i] = tmp[index];
		del(tmp, size - i, index);
	}
}

inline void print_alph(char *alph){
	cout << "out: ";
	for(int i = 0; i < 26; i++){
		putchar(alph[i]);
	}
	cout << endl;
}

class Rotor{
		int count;
		Ring <char> alph;
		int seed;
		int offset;
	public:
		Rotor(): alph(26), seed(0), offset(0){
			count = 0;
			char s[] = ALPH;
			perm(seed, s, 26);
			alph.set(s);
		}

		int get_count(){ return count;}

		void print_info(){
			char s[] = ALPH;
			int head = alph.get_head();
			cout << "in:  ";
			for(int i = head; i < 26; i++)putchar(s[i]);
			for(int i = 0; i < head; i++)putchar(s[i]);
			cout << endl;
			print_alph(alph.get_buf());
			cout << "count: " << count << endl;
			cout << "initial offset: " << offset << endl;
			cout << "initial seed: " << seed << endl;
		}

		void reset(){
			count = 0;
			alph.reset();
			alph.rotate(offset);
		}

		void set(int seed, int n){
			//変換用配列
			char s[] = ALPH;
			perm(seed, s, 26);
			alph.set(s);

			count = 0;
			offset = n;
			this->seed = seed;
			alph.reset();
			alph.rotate(n);
		}
		void rotate(int n){
			alph.rotate(n);
			count = (count + n) % 26;
		}
		char scramble(char c){
			return alph.access(c - 'a');
		}
		char disscramble(char c){
			int index = alph.find(c);
			if(index < 0)exit(EXIT_FAILURE);
			return ('a' + index);
		}
};

void alph_pair(int seed, char *alph, int size){
		char s[] = ALPH;
		perm(seed, s, size);
		//ランダム配列からペアを作る
		int half = size/2;
		for(int i = 0; i < half; i++){
			alph[s[i] - 'a'] = s[i + half];
			alph[s[i + half] - 'a'] = s[i];
		}
}

class Reflector{
		char alph[26];
		int seed;
	public:
		Reflector(): seed(0){
			alph_pair(seed, alph, 26);
		}
		void print_info(){
			cout << "in:  " << ALPH << endl;
			print_alph(alph);
			cout << "initial seed: " << seed << endl;
		}
		void set(int seed){
			alph_pair(seed, alph, 26);
			this->seed = seed;
		}
		char reflect(char c){
			int index = (c - 'a') % 26;
			return alph[index];
		}
};

void swap(char *s, int i, int j){
	char tmp = s[i];
	s[i] = s[j];
	s[j] = tmp;
}

class Plugboard{
		char alph[26];
	public:
		Plugboard(){
			char a[] = ALPH;
			strncpy(alph, a, 26);
		}
		void print_info(){ cout << "in:  " << ALPH << endl; print_alph(alph);}
		void set(vector<pair<char, char>> l){
			char s[] = ALPH;
			strncpy(alph, s, 26);
			int swaped = 0;
			//swapするものに重複は許さない
			for(auto itr = l.begin(); itr != l.end(); ++itr){
				char c1 = (*itr).first;
				char c2 = (*itr).second;
				int check = 1 << (c1 - 'a') | 1 << (c2 - 'a');
				if(!(swaped & check)){
					swap(alph, c1 - 'a', c2 - 'a');
					swaped = swaped | check;
				}
				else{
					cout << "invalid plug" << ": (" << c1 << ", " << c2 << ")\n";
					cout << "pair ignored\n";
				}
			}
		}
		char scramble(char c){
			int index = (c - 'a') % 26;
			return alph[index];
		}
};

class Enigma{
	public:
		Plugboard plugboard;
		Rotor right_rotor;
		Rotor middle_rotor;
		Rotor left_rotor;
		Reflector reflector;

		Enigma(): plugboard(), right_rotor(), middle_rotor(), left_rotor(), reflector(){}
		void set(pair<int, int> r, pair<int, int> m, pair<int, int> l, int sdref, vector<pair<char, char>> v){
			right_rotor.set(r.first, r.second);
			middle_rotor.set(m.first, m.second);
			left_rotor.set(l.first, l.second);
			reflector.set(sdref);
			plugboard.set(v);
		}

		void reset(){
			right_rotor.reset();
			middle_rotor.reset();
			left_rotor.reset();
		}

		void print_info(){
			cout << "Plugboard:\n";
			plugboard.print_info();
			cout << endl << "Right Rotor:\n";
			right_rotor.print_info();
			cout << endl << "Middle Rotor:\n";
			middle_rotor.print_info();
			cout << endl << "Left Rotor:\n";
			left_rotor.print_info();
			cout << endl << "Reflector:\n";
			reflector.print_info();
			cout << endl;
		}

		char encode(char c){
			//printf("c:%c\n", c);
			char c1 = plugboard.scramble(c);
			//printf("c1:%c\n", c1);
			char c2 = right_rotor.scramble(c1);
			//printf("c2:%c\n", c2);
			char c3 = middle_rotor.scramble(c2);
			//printf("c3:%c\n", c3);
			char c4 = left_rotor.scramble(c3);
			//printf("c4:%c\n", c4);
			char c5 = reflector.reflect(c4);
			//printf("c5:%c\n", c5);
			char c6 = left_rotor.disscramble(c5);
			//printf("c6:%c\n", c6);
			char c7 = middle_rotor.disscramble(c6);
			//printf("c7:%c\n", c7);
			char c8 = right_rotor.disscramble(c7);
			//printf("c8:%c\n", c8);
			char c9 = plugboard.scramble(c8);
			//printf("c9:%c\n", c9);
			//printf("c1:%c, c2:%c, c3:%c, c4:%c, c5:%c, c6:%c, c7:%c, c8:%c\n", c1,c2,c3,c4,c5,c6,c7,c8);

			right_rotor.rotate(1);
			if(right_rotor.get_count() == 0){
				middle_rotor.rotate(1);
				if(middle_rotor.get_count() == 0)left_rotor.rotate(1);
			}

			return c9;
		}
};


void set(Enigma &enigma){
	pair <int, int> r;
	pair <int, int> m;
	pair <int, int> l;
	int s;
	vector<pair<char, char>> v;

	cout << "right rotor: seed, position\n";
	cin >> r.first >> r.second;
	cout << "middle rotor: seed, position\n";
	cin >> m.first >> m.second;
	cout << "left rotor: seed, position\n";
	cin >> l.first >> l.second;
	cout << "reflector: seeed\n";
	cin >> s;
	cout << "plugboard: #pairs, pairs of small alphabet\n";
	int n;
	cin >> n;
	if(n > 13)n = 13;
	while(n--){
		pair <char, char> p;
		cin >> p.first >> p.second;
		if( !(('a' <= p.first) && (p.first <= 'z') && ('a' <= p.second) && (p.second <= 'z'))){
			cout << "error: invalid character: only for small alphabet\n";
			cout << "pair ignored\n";
			continue;
		}
		v.push_back(p);
	}

	enigma.set(r, m, l, s, v);
	cout << "setting done\n";
}

void encode(Enigma &enigma, string s){
	cout << "encoded characters\n\n";
	for(auto itr = s.begin(); itr != s.end(); ++itr){
		if(!('a' <= *itr && *itr <= 'z')){
			cout << "error: invalid character: only for small alphabet\n";
			cout << "counter reset\n";
			enigma.reset();
			break;
		}
		cout << enigma.encode(*itr);
	}
	cout << "\n\n";
}

int main(){
	Enigma enigma;

	string s;

	cout << "Enigma Simulator (only for small alphabet)\n" << endl;
	cout << "Type SET for setting\n";
	cout << "Type RESET for reset count\n";
	cout << "Type INFO for checking status and setting\n";
	cout << "Type QUIT to quit\n";
	cout << "small alphabets will be encoded\n\n";

	cout << "enigma simulator> "; 
	cin >> s;
	while(true){
		if(s == "QUIT")break;
		else if(s == "SET")set(enigma);
		else if(s == "RESET"){ enigma.reset(); cout << "reset done\n";}
		else if(s == "INFO")enigma.print_info();
		else encode(enigma, s);
		cout << "enigma simulator> "; 
		cin >> s;
	}

	return 0;
}
