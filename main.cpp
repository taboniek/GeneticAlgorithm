#include "pch.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <math.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <ctime>
#include <random>
#include <functional>

using namespace std;

int tab[50][2]; // table with times
vector <vector<int> > machine1; //machine1
vector <vector<int> > machine2; //machine2
vector <vector<int> >parents; //parents that later come to the mutations
vector <vector<int> >children; // order of accidental placement
vector <int> sum_time; // the sum of time to complete all operations for each placement in a given iteration (parents selected from this roulette)
vector <vector<int> > result;
int iteration = 500; //number of iterations
int k_population = 100; //number of populations
int n_tasks = 50; //number of tasks
int inaction; //value of the idle period calculated by the generator
long int long_max; // longest order
int nr_test; // to save to file
int g = 75; // when time is dislay on the screen


void write() {
	ofstream file;
	string file_name;
	string op = "op";
	cout << endl << "Enter test number: " << endl;
	cin >> nr_test;
	cout << endl << "Enter file name: \t" << endl;
	cin >> file_name;
	file_name += ".txt";
	file.open(file_name.c_str());
	if (file.good() == true) {
		file << "**************"<<nr_test<<"**************";
		file << "\n";
		file << n_tasks << "\n";
		for (int x= 0; x < n_tasks; x++) {
			file << op<<"1_"<<x+1<<", "<< tab[x][0] << ";" << op<<"2_"<<x+1<<", "<< tab[x][1] << ";1;2;\n";
		}
		file << "*** EOF ***";
		file.close();
	}
	else cout << "File error!" << endl;
}
void write_after(vector<int> &sumki) {
	ofstream file;
	string file_name;
	string op = "op";
	string id = "idle";
	string ma = "maint";
	int time = 0;
	int counter_i = 0;
	int counter_m = 0;
	cout << endl << "Enter file name: \t" << endl;
	cin >> file_name;
	file_name += ".txt";
	file.open(file_name.c_str());
	if (file.good() == true) {
		file << "**************"<<nr_test<<"**************";
		file << "\n";
		file <<result[2][0] <<", " << long_max <<"\n";
		file << "M1: ";
		for (int i = 0; i < n_tasks; i++) {
			file << op << result[0][i] <<"_1 " <<time <<", "<< tab[result[0][i]][0] <<", " <<tab[result[0][i]][0]<<"; ";
			time += tab[result[0][i]][0];
		}
		time = 0;
		file << "\n" << "M2: ";
		for (int i = 0; i < result[1].size(); i++) {
			if (result[1][i] < 0) {
				counter_i++;
				time += (result[1][i] * -1);
				file << id << counter_i << "_M2; ";
				
			}
			else if (result[1][i] == 724) {
				counter_m++;
				time += inaction;
				file << ma << counter_m << "_M2; ";
			}
			else {

				file << op << result[1][i] << "_2 " << time<<", "<< tab[result[1][i]][1] << ", " << sumki[i] << "; ";
				time += sumki[i];
			}
		}
		file << "\n";
		file << "0\n ";
		file << counter_i <<"\n";
		file << counter_m << "\n";
		file << "*** EOF ***";

		file.close();
	}
	else cout << "Error file!" << endl;

}

void generator_new() {

	const int min = 1;
	const int max = 20;
	auto random = bind(uniform_int_distribution<>(min, max),
		mt19937(time(NULL)));
	int random1; 
	int count = 0;
	for (int i = 0; i < n_tasks; i++) {
		random1 = random();
		tab[i][0] = random1;
		random1 = random();
		tab[i][1] = random1;
	}
	vector<int>temp;
	for (int i = 0; i < n_tasks; i++) { 
		temp.push_back(i);
	}
	for (int i = 0; i < k_population; i++) {
		random_shuffle(temp.begin(), temp.end());
		children.push_back(temp);
		machine1.push_back(temp);
		random_shuffle(temp.begin(), temp.end());
		machine2.push_back(temp);
	}

	for (int i = 0; i < n_tasks; i++) {
		count += tab[i][1];
	}
	inaction = ceil((count/n_tasks) *1.5);
}

void max_time() {

	long int time1=0;
	long int time2 = 0;
	long int temp = 0;
	long int temp2 = 0;
	int value_of_punishment = 0;
	float cumulation = 0.0;
	int a = 0;
	bool stop = false;
	bool *table = new bool [n_tasks];
	for (int i = 0; i < n_tasks; i++)
		table[i] = false;
	for (int i = 0; i < n_tasks; i++) {
		temp += tab[machine1[0][i]][0];
		time1+=temp;
		table[machine1[0][i]] = true;
		temp2 += tab[machine1[0][i]][0];
		time2 += temp2;
		while (table[machine2[0][a]] == true && time2>=time1) {
			value_of_punishment = ceil(tab[machine2[0][a]][1] * cumulation);
			cumulation += 0.1;
			temp2 += tab[machine2[0][a]][1]+value_of_punishment;
			time2 += temp;
			a++;
			if (a == n_tasks) break;
		}
	}
	for (a; a < n_tasks; a++) {
		temp2 += tab[machine2[0][a]][1];
		time2 += temp2;
	}
	long_max = time2;

}

void arranging_time2() {

	machine1.erase(machine1.begin(), machine1.end());
	machine2.erase(machine2.begin(), machine2.end());
	int time1 = 0;
	int time2 = 0;
	int number_maintenance = 0;
	int idle = 0;
	float chanse = 0;
	int temp = 0;
	int help;

	for (int i = 0; i < k_population; i++) {
		time1 = time2 = number_maintenance = idle = chanse = 0;
		vector<int> maszyna1;
		vector<int> maszyna2;
		for (int m1 = 0, m2 = 0; m1 <children[i].size(); m1++, m2++) {
			
			maszyna1.push_back(children[i][m1]);
			time1 += tab[children[i][m1]][0];

			if (m1 == 0) {				
				temp = tab[children[i][m1]][0] * -1;
				maszyna2.push_back(temp);
				time2 += (temp * -1);
				idle++;
				m2++;
				maszyna2.push_back(children[i][m1]);
				time2 += tab[children[i][m1]][1];
				chanse += 10;
			}
			
			else {
				if (rand() % 100 <= chanse) {
					maszyna2.push_back(724);
					time2 += inaction;
					chanse = 0;
					number_maintenance++; m2++;
				}
				if (time1 - tab[children[i][m1]][0] > time2) {
					time1 = time1 - tab[children[i][m1]][0];
					m1--;
					maszyna1.pop_back();
				}
				if (time1 > time2) {
					if (m1 + number_maintenance + idle - m2 <= 0) {
						temp = (time1 - time2) *-1;
						maszyna2.push_back(temp);
						time2 += (temp*-1);
						maszyna2.push_back(children[i][m1]);
						time2 += tab[children[i][m1]][1];
						chanse += 10;
						idle++;
						m2++;
					}
					else {
						help = children[i].size()-1;
						for (int a = 0; a < maszyna1.size(); a++) {
							if (find(maszyna2.begin(), maszyna2.end(), maszyna1[a]) == maszyna2.end()) {
								if (help > a)
									help = a;
									break;
							}
						}
						maszyna2.push_back(children[i][help]);
						time2 += tab[children[i][help]][1];
						chanse += 10;
					}
				}
				else if(time1<time2){
					bool dalej = true;
					while (dalej && m1 < children[i].size()-1) {
						if (time1 + tab[children[i][m1 + 1]][0] <= time2) {
							time1 += tab[children[i][m1+1]][0];
							maszyna1.push_back(children[i][m1+1]);
							m1++;
						}
						else dalej = false;
					}
					help = children[i].size()-1;
					for (int a = 0; a < maszyna1.size(); a++) {
						if (find(maszyna2.begin(), maszyna2.end(), maszyna1[a]) == maszyna2.end()) {
							if (help > a)
								help = a;
							break;
						}
					}
					maszyna2.push_back(children[i][help]);
					time2 += tab[children[i][help]][1];
					chanse += 10;
				}
				else {
					maszyna2.push_back(children[i][m1]);
					time2 += tab[children[i][m1]][1];
					chanse += 10;
				}

			}
			bool finished=true;
			if (maszyna1.size() == children[i].size()) {
				for (int a = 0; a < children[i].size(); a++) {
					if (find(maszyna2.begin(), maszyna2.end(), children[i][a]) == maszyna2.end()) {
						finished = false;
						break;
					}
					finished = true;
				}
			}
			while (finished != 1) {
				help = children[i].size()-1;
				if (rand() % 100 <= chanse) {
					maszyna2.push_back(724);
					time2 += inaction;
					chanse = 0;
					number_maintenance++; m2++;
				}
						help = children[i].size()-1;
						for (int a = 0; a < children[i].size(); a++) {
							if (find(maszyna2.begin(), maszyna2.end(), children[i][a]) == maszyna2.end()) {
								if (help > a)
									help = a;
								break;
							}
						}

				maszyna2.push_back(children[i][help]);
				time2 += tab[children[i][help]][1];
				chanse += 10;

				for (int a = 0; a < children[i].size(); a++) {
					if (find(maszyna2.begin(), maszyna2.end(), children[i][a]) == maszyna2.end()) {
						finished = false;
						break;
					}
					finished = true;
				}
				

			}
		}
		machine1.push_back(maszyna1);
		machine2.push_back(maszyna2);
		maszyna1.clear();
		maszyna2.clear();

	}
}

void counting_time(int h) {

	int index = 0; 
	int time = 0;
	int temp = 0;
	float cumulation = 0;
	int value_punishment = 0;
	vector<int> temporary;
	sum_time.clear();
	temporary.clear();
	for (int i = 0; i < machine2.size(); i++) {
		for (int j = 0; j < machine2[i].size(); j++) {
			if (machine2[i][j] == 724) {
				cumulation = 0;
				temp += inaction;
				time += temp;
			}
			else if (machine2[i][j] < 0) {
				temp += (machine2[i][j] * -1);
				time += temp;
			}
			else {
				value_punishment = ceil(tab[machine2[i][j]][1] * cumulation);
				temp += tab[machine2[i][j]][1] + value_punishment;
				time += temp;
				cumulation += 0.1;
			}
		}
		sum_time.push_back(time);
		if (sum_time[index] > sum_time[sum_time.size()-1]){
			index = sum_time.size() - 1;
		}
		
	}
	if (h % g == 0)
		cout << h << " ~~ " << sum_time[index] << endl;
	if (result.size() == 0) {
		result.push_back(machine1[index]);
		result.push_back(machine2[index]);
		temporary.push_back(sum_time[index]);
		result.push_back(temporary);
	}
	else {
		if (result[2][0] > sum_time[index]) {
			result.erase(result.begin(), result.end());
			result.push_back(machine1[index]);
			result.push_back(machine2[index]);
			temporary.push_back(sum_time[index]);
			result.push_back(temporary);
		}
	}
	
}

void choose_parent() {
	parents.clear();
	vector<int> j;
	int help[9];
	bool was = false;
	int luck;
	int luck2;
	const int min = 0;
	const int max = n_tasks-1;
	auto random = bind(uniform_int_distribution<>(min, max),
		mt19937(time(NULL)));
	for (int i = 0; i < 9; i++) {
		help[i] = -1;
	}
	for (int i = 0; i < 9; i++) {
		luck = random();
		luck2 = random();
		if (sum_time[luck] < sum_time[luck2]) {
			was = true;
			if (find(begin(help), end(help), luck) != end(help)) {
				i--;
				was = true;
			}
			if (was != false) {
				j = machine1[luck];
				help[i] = luck;
				parents.push_back(j);
			}
		}
		else i--;
	}
	j = result[0];
	parents.push_back(j);	
}

void cross() {

	const int min = 0;
	const int max = 9;
	auto random = bind(uniform_int_distribution<>(min, max),
		mt19937(time(NULL)));
	machine1.clear();
	machine2.clear();
	children.clear();
	int losik;
	for (int i = 0; i < k_population; i++) {
		vector<int> temp;
		losik = random();
		for (int j = 0; j < n_tasks/2+15; j++) {
			temp.push_back(parents[losik][j]);
		}
		losik=random();
		for (int j = 0; j < parents[losik].size(); j++) {
			if (find(temp.begin(), temp.end(), parents[losik][j]) == temp.end()) {
				temp.push_back(parents[losik][j]);
			}
		}
		children.push_back(temp);
	}
}

void mutation() {

	const int min = 0;
	const int max = 49;
	auto random = bind(uniform_int_distribution<>(min, max),
		mt19937(time(NULL)));
	int los = 0;
	int nr_zadania = 0;
	int nr_zadania2 = 0;
	int temp = 0;
	for (int i = 0; i < k_population; i++) {
		los= int((2 - 0 + 1) * rand() / (RAND_MAX + 1.0));
		while (los > 0) {
			nr_zadania = random();
			nr_zadania2 = random();
			if (nr_zadania != nr_zadania2) {
				temp = children[i][nr_zadania];
				children[i][nr_zadania] = children[i][nr_zadania2];
				children[i][nr_zadania2] = temp;
				los--;
			}
		}
	}
}

int main()
{
	
	int h = 0;
    cout << "Hello World!\n"; 
	generator_new();
	max_time();
	cout << "Placing tasks on machines has time " << long_max<<endl;
	write();
	arranging_time2();
	cout << "2" << endl;
	
	counting_time(h);
	while (h<iteration) {
		choose_parent();
		cross();
		mutation();
		arranging_time2();
		counting_time(h);
		h++;
	}
	vector<int> times_punishment;
	int value_punishment;
	float cumulation=0;
	for (int j = 0; j < result[1].size(); j++) {
		if (result[1][j] == 724) {
			cumulation = 0;
			times_punishment.push_back(0);
		}
		else if (result[1][j] < 0) {
			times_punishment.push_back(result[1][j]);
		}
		else {
			value_punishment = ceil(tab[result[1][j]][1] * cumulation);
			times_punishment.push_back(tab[result[1][j]][1] + value_punishment);
			cumulation += 0.1;
		}
	}
	if (iteration == h) {
		cout << h << " ~~ " << result[2][0] << endl;
	}
	write_after(times_punishment);
	
	system("pause");
}
