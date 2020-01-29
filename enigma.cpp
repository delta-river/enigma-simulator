#include <cstdlib>
#include <iostream>
#include <string.h>
#include <vector>
using namespace std;

#define ALPH "abcdefghijklmnopqrstuvwxyz"

template <class T> class Ring{
	private:
		T *buf;
		int size;
		int head;
	public:
		Ring(unsigned int size){
			this->size = size;
			buf = new T[size];
			head = 0;
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

class Rotor{
		int count;
		Ring <char> alph;
	public:
		Rotor(): alph(26){
			count = 0;
			char s[] = ALPH;
			alph.set(s);
		}

		int get_count(){ return count;}

		void count_reset(){
			count = 0;
			alph.reset();
		}

		void set(int seed, int n){
			//変換用配列
			char s[] = ALPH;
			perm(seed, s, 26);
			alph.set(s);

			count = 0;
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

class Reflector{
		char alph[26];
	public:
		Reflector(){
			strncpy(alph, DISALPH, 26);
		}
		void set(int seed){
			char s[] = ALPH;
			perm(seed, s, 26);
			//ランダム配列からペアを作る
			for(int i = 0; i < 26/2; i++){
				alph[s[i] - 'a'] = s[i + 13];
				alph[s[i + 13] - 'a'] = s[i];
			}
		}
		char reflect(char c){
			unsigned int index = (c - 'a') % 26;
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
		void set(vector<pair<char, char>> l){
			char s[] = ALPH;
			strncpy(alph, s, 26);
			int swaped = 0;
			//swapするものに重複は許さない
			for(auto itr = l.begin(); itr != l.end(); ++itr){
				char c1 = (*itr).first;
				char c2 = (*itr).second;
				int check = 1 << c1 | 1 << c2;
				if(!(swaped & check)){
					swap(alph, c1, c2);
					swaped = swaped | check;
				}
				else{
					cout << "invalid plug" << ": (" << c1 << ", " << c2 << ")\n";
					cout << "pair ignored\n";
				}
			}
		}
		char scramble(char c){
			unsigned int index = (c - 'a') % 26;
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

		//Enigma(){}
		void set(pair<int, int> r, pair<int, int> m, pair<int, int> l, int sdref, vector<pair<char, char>> v){
			right_rotor.set(r.first, r.second);
			middle_rotor.set(m.first, m.second);
			left_rotor.set(l.first, l.second);
			reflector.set(sdref);
			plugboard.set(v);
		}

		void count_reset(){
			right_rotor.count_reset();
			middle_rotor.count_reset();
			left_rotor.count_reset();
		}

		char encode(char c){
			char c1 = plugboard.scramble(c);
			char c2 = right_rotor.scramble(c1);
			char c3 = middle_rotor.scramble(c2);
			char c4 = left_rotor.scramble(c3);
			char c5 = reflector.reflect(c4);
			char c6 = left_rotor.disscramble(c5);
			char c7 = middle_rotor.disscramble(c6);
			char c8 = right_rotor.disscramble(c7);
			char c9 = plugboard.scramble(c8);
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
		if( !('a' <= p.first && p.first <= 'z' && 'a' <= p.second && p.second <= 'z')){
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
			enigma.count_reset();
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
	cout << "Type SET for initialize setting\n";
	cout << "Type RESET for reset count\n";
	cout << "Type QUIT to quit\n";
	cout << "small alphabets will be encoded\n\n";

	cout << "enigma simulator> "; 
	cin >> s;
	while(true){
		if(s == "QUIT")break;
		else if(s == "SET"){set(enigma);}
		else if(s == "RESET"){enigma.count_reset(); cout << "count reset\n\n";}
		else encode(enigma, s);
		cout << "enigma simulator> "; 
		cin >> s;
	}

	return 0;
}
