#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <bits/stdc++.h>

using namespace std;

pthread_barrier_t barrier;
pthread_mutex_t mutexx, result_mutex;

// structura cu date utilizate de functiile thread-urilor
struct arg_struct {
	int k;								// exponentul de care se ocupa fiecare thread reduce
	int R;								// numarul de thread-uri reduce
    queue<string> file_names;			// coada cu fisierele ce trebuie procesate de map_func()
	vector<vector<set<int>>> results;	// rezultatele produse de fiecare thread
};

// algoritm de binary search pentru a afla daca numarul este o putere perfecta
bool perfect_power(long l, long r, int power, long x)
{
    if (!(abs(l - r) <= 1)) {
        long mid = (l + r) / 2;
		long check = 1;
		for (int i = power; i > 0; check *= mid, i--);

        if (check == x)
            return true;
        if (check > x)
            return perfect_power(l, mid, power, x);
        return perfect_power(mid, r, power, x);
    }
    return false;
}

// citeste numere din fisierul file_name si stocheaza puterile perfecte in result
void read_from_file(string file_name, vector<set<int>> &result, int E)
{
    ifstream file(file_name);
    long number;
	int amount_of_numbers;
	file >> amount_of_numbers;

    for (int k = 0; k < amount_of_numbers; k++) {
		file >> number;
		if (number == 0)
			continue;
		else if (number == 1)
			for (int j = 2; j < E + 2; j++)
				result[j].insert(1);
		else {
			int sq = sqrt(number);
			for (int p = 2; p < E + 2; p++) {
				if (perfect_power(1, sq + 1, p, number)) {
					result[p].insert(number);
				}
			}
		}
	}
}

// functie de map folosita de thread-uri
void *map_func(void *arg)
{
	struct arg_struct *args = (struct arg_struct *) arg;
	vector<set<int>> result(args->R + 2);

	// fiecare thread dispobil ia un fisier din coada si il proceseaza
	while (true) {
		pthread_mutex_lock(&mutexx);
		if (!args->file_names.empty()) {
			string file_name = args->file_names.front();
			args->file_names.pop();
			pthread_mutex_unlock(&mutexx);
			read_from_file(file_name, result, args->R);
		}
		else {
			pthread_mutex_unlock(&mutexx);
			break;
		}
	}

	// stocam rezultatul in structura
	pthread_mutex_lock(&result_mutex);
	args->results.push_back(result);
	pthread_mutex_unlock(&result_mutex);

	pthread_barrier_wait(&barrier);
	pthread_exit(NULL);
}

// functie de reduce folosita de thread-uri
void *reduce_func(void *arg)
{
	pthread_barrier_wait(&barrier);
	struct arg_struct *args = (struct arg_struct *) arg;
	int size = args->results.size();
	set<int> reduce_result;

	// fiecare thread reduce ia un exponent pentru care proceseaza numarul de puteri perfecte
	pthread_mutex_lock(&mutexx);
	int kk = (args->k)++;
	pthread_mutex_unlock(&mutexx);
	for (int j = 0; j < size; j++) {
		set_union(reduce_result.begin(), reduce_result.end(),
					args->results[j][kk].begin(), args->results[j][kk].end(), 
					inserter(reduce_result, reduce_result.begin()));
	}
	ofstream out("out" + to_string(kk) + ".txt");
	out << reduce_result.size();
	out.close();

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	int M, R;
	int number_of_files;

	M = atoi(argv[1]);	//numar de thread-uri map
	R = atoi(argv[2]);	//numar de thread-uri reduce

	struct arg_struct args;
	args.R = R;
	args.k = 2;

	// citim numele fisierelor ce trebuie procesate
	ifstream main_file(argv[3]);
	string s;
	getline(main_file, s);
	istringstream iss(s);
	iss >> number_of_files;
	for (int i = 0; i < number_of_files; i++) {
		string line;
		getline(main_file, line);
		args.file_names.push(line);
	}
	main_file.close();

	pthread_barrier_init(&barrier, NULL, M + R);
	pthread_mutex_init(&mutexx, NULL);
	pthread_mutex_init(&result_mutex, NULL);

	pthread_t tid[M + R];

	// thread-urile map
	for (int i = 0; i < M; i++) {
		pthread_create(&tid[i], NULL, map_func, (void *) &args);
	}

	// thread-urile reduce
	for (int i = M; i < M + R; i++) {
		pthread_create(&tid[i], NULL, reduce_func, (void *) &args);
	}

	// join la thread-uri
	for (int i = 0; i < M + R; i++) {
		pthread_join(tid[i], NULL);
	}

	pthread_barrier_destroy(&barrier);
	pthread_mutex_destroy(&mutexx);
	pthread_mutex_destroy(&result_mutex);
	return 0;
}
