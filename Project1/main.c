#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

struct dataRow {
	double c1;
	double c2;
	double c3;
	double c4;
	char* id;
};

struct neurone {
	double c1;
	double c2;
	double c3;
	double c4;
};

struct dataRow* vectors;

int dataSetSize;

int* fauxPointeurs;

static const int NB_NEURONE_COL = 6;
static const int NB_NEURONE_ROW = 10;
struct neurone neurones[6][10];

/*
* Create a vector parsing line and insert it in results
*/
void creatVector(char *line, struct dataRow* results, int resultSize) {
	char* value;
	char *ptr;
	struct dataRow* tempData;
	tempData = (struct dataRow*) malloc (sizeof(struct dataRow));
	if (tempData != NULL) {
		// first field
		value = strtok(line, ",");
		tempData->c1 = strtod(value, &ptr);

		// second field
		value = strtok(NULL, ",");
		tempData->c2 = strtod(value, &ptr);

		// triple field
		value = strtok(NULL, ",");
		tempData->c3 = strtod(value, &ptr);

		// fourth field
		value = strtok(NULL, ",");
		tempData->c4 = strtod(value, &ptr);

		// id
		value = strtok(NULL, ",");
		tempData->id = (char*)malloc(sizeof(value));
		tempData->id = _strdup(value);

	}
	vectors = (struct dataRow*) realloc (vectors, (resultSize + 1) * sizeof(struct dataRow));
	vectors[resultSize] = *tempData;
}

/*
* Normalize vectors array
* return average vector of the array
*/
struct dataRow* normalizeAndAverageVector() {
	double sumC1 = 0, sumC2 = 0, sumC3 = 0, sumPowC1 = 0, sumPowC2 = 0, sumPowC3 = 0;

	//Normalization
	for (int i = 0; i < dataSetSize; i++) {
		sumPowC1 += pow(vectors[i].c1, 2);
		sumPowC2 += pow(vectors[i].c2, 2);
		sumPowC3 += pow(vectors[i].c3, 2);
	}

	sumPowC1 = sqrt(sumPowC1);
	sumPowC2 = sqrt(sumPowC2);
	sumPowC3 = sqrt(sumPowC3);

	for (int i = 0; i < dataSetSize; i++) {
		vectors[i].c1 = vectors[i].c1 / sumPowC1;
		vectors[i].c2 = vectors[i].c2 / sumPowC2;
		vectors[i].c3 = vectors[i].c3 / sumPowC3;

		//averageVector
		sumC1 += vectors[i].c1;
		sumC2 += vectors[i].c2;
		sumC3 += vectors[i].c3;
	}

	//Return Avg
	struct dataRow* result = (struct dataRow*) malloc (sizeof(struct dataRow));
	result->c1 = sumC1 / dataSetSize;
	result->c2 = sumC2 / dataSetSize;
	result->c3 = sumC3 / dataSetSize;
	return result;
}

void swap(int a[], int i, int j) {
	int temp = a[i];
	a[i] = a[j];
	a[j] = temp;
}

void initFauxPointeurs() {
	// Initialisation
	for (int i = 0; i < dataSetSize; i++) {
		fauxPointeurs[i] = i;
	}

	//Shuffle
	for (int i = dataSetSize - 1; i > 0; i--) {
		int random = rand() % (i + 1);
		swap(fauxPointeurs, i, random);
	}
}

/*
* Init neurones with avgVector value +-randomRange
*/
void initNeurones(struct dataRow* avgVector) {
	const float randomRange = 0.04;
	for (int i = 0; i < NB_NEURONE_COL; i++) {
		for (int j = 0; j < NB_NEURONE_ROW; j++) {
			float randOffset = (float)rand() / (float)(RAND_MAX / randomRange);

			// Map from [0..randomRange] to [-randomRange/2..randomRange/2]
			randOffset -= randomRange / 2;

			struct neurone newNeurone;
			newNeurone.c1 = avgVector->c1 + randOffset;
			newNeurone.c2 = avgVector->c2 + randOffset;
			newNeurone.c3 = avgVector->c3 + randOffset;

			neurones[i][j] = newNeurone;
		}
	}
}

/*
* Test function to display neurones array
*/
void displayNeurones() {
	for (int i = 0; i < NB_NEURONE_COL; i++) {
		for (int j = 0; j < NB_NEURONE_ROW; j++) {
			struct neurone* pickedNeurone = &neurones[i][j];
			printf("{ c1: %f\n ", pickedNeurone->c1);
			printf("c2: %f\n ", pickedNeurone->c2);
			printf("c3: %f\n }", pickedNeurone->c3);
		}
	}
}

int main() {
	char line[128];

	FILE* data = fopen("iris3.data", "r");
	if (data == NULL) {
		perror("Error opening file");
		return(-1);
	}
	int i = 0;
	while (fgets(line, sizeof(line), data) && isdigit(line[0])) {
		creatVector(line, &vectors, i);
		printf(vectors[i].id);
		i++;
	}
	  
	dataSetSize = i;
	struct dataRow* avgVector = normalizeAndAverageVector();
	fauxPointeurs = (int*) malloc (sizeof(int) * dataSetSize);
	initFauxPointeurs();
	initNeurones(avgVector);
	displayNeurones();
	return 0;
}