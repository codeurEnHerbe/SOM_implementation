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

struct BMU {
	float distEucl;
	struct neurone* neurone;
	struct BMU* nextNeurone;
	int col, row;
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
	double sumC1 = 0, sumC2 = 0, sumC3 = 0, sumC4 = 0, sumPowC1 = 0, sumPowC2 = 0, sumPowC3 = 0, sumPowC4 = 0;

	//Normalization
	for (int i = 0; i < dataSetSize; i++) {
		sumPowC1 += pow(vectors[i].c1, 2);
		sumPowC2 += pow(vectors[i].c2, 2);
		sumPowC3 += pow(vectors[i].c3, 2);
		sumPowC4 += pow(vectors[i].c4, 2);
	}

	sumPowC1 = sqrt(sumPowC1);
	sumPowC2 = sqrt(sumPowC2);
	sumPowC3 = sqrt(sumPowC3);
	sumPowC4 = sqrt(sumPowC4);

	for (int i = 0; i < dataSetSize; i++) {
		vectors[i].c1 = vectors[i].c1 / sumPowC1;
		vectors[i].c2 = vectors[i].c2 / sumPowC2;
		vectors[i].c3 = vectors[i].c3 / sumPowC3;
		vectors[i].c4 = vectors[i].c4 / sumPowC4;

		//averageVector
		sumC1 += vectors[i].c1;
		sumC2 += vectors[i].c2;
		sumC3 += vectors[i].c3;
		sumC4 += vectors[i].c4;
	}

	//Return Avg
	struct dataRow* result = (struct dataRow*) malloc (sizeof(struct dataRow));
	result->c1 = sumC1 / dataSetSize;
	result->c2 = sumC2 / dataSetSize;
	result->c3 = sumC3 / dataSetSize;
	result->c4 = sumC4 / dataSetSize;
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
			float randOffset = ((float)rand() / (float)(RAND_MAX)) * randomRange;
			// Map from [0..randomRange] to [-randomRange/2..randomRange/2]
			randOffset -= randomRange / 2;

			struct neurone newNeurone;
			newNeurone.c1 = avgVector->c1 + randOffset;

			randOffset = (float)rand() / (float)(RAND_MAX) * randomRange;
			randOffset -= randomRange / 2;
			newNeurone.c2 = avgVector->c2 + randOffset;

			randOffset = (float)rand() / (float)(RAND_MAX) * randomRange;
			randOffset -= randomRange / 2;
			newNeurone.c3 = avgVector->c3 + randOffset;

			randOffset = (float)rand() / (float)(RAND_MAX) * randomRange;
			randOffset -= randomRange / 2;
			newNeurone.c4 = avgVector->c4 + randOffset;

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
			printf("%f\n", pickedNeurone->c1);
		}
	}
}

float euclDistance(struct neurone* neur, struct dataRow* data) {
	float c1 = pow((data->c1 - neur->c1), 2);
	float c2 = pow((data->c2 - neur->c2), 2);
	float c3 = pow((data->c3 - neur->c3), 2);
	float c4 = pow((data->c4 - neur->c4), 2);

	return (float)sqrt(c1 + c2 + c3 + c4);
}

struct BMU* findBestMatchUnit(struct dataRow* data) {
	// Initialisation de la chaine de BMU au premier neurone 
	struct BMU* bmu = (struct BMU*)malloc(sizeof(struct BMU));
	bmu->neurone = &neurones[0][0];
	bmu->distEucl = -1;

	for (int i = 0; i < NB_NEURONE_COL; i++) {
		for (int j = 0; j < NB_NEURONE_ROW; j++) {
			float distEucl = euclDistance(&neurones[i][j], data);

			if (bmu->distEucl > distEucl || bmu->distEucl == -1) {
				bmu->neurone = &neurones[i][j];
				bmu->distEucl = distEucl;
				bmu->col = i;
				bmu->row = j;

			}
			else if (bmu->distEucl == distEucl) {
				bmu->nextNeurone = (struct BMU*)malloc(sizeof(struct BMU));
				bmu->nextNeurone->distEucl = distEucl;
				bmu->nextNeurone->neurone = &neurones[i][j];
			}
		}
	}
	return bmu;
}

void propagateNeigh(struct BMU* bmu, float delta) {
	for (int i = 0; i < 4; i++) {
		int rangeVoisin;
		struct neurone* targetNeurone;
		switch (i) {
		case 1:
			rangeVoisin = NB_NEURONE_COL - bmu->col;
			targetNeurone = &neurones[bmu->col][bmu->row + j];
			break;

		case 2:
			rangeVoisin = NB_NEURONE_ROW - bmu->row;
			targetNeurone = &neurones[bmu->col + j][bmu->row];
			break;

		case 3:
			rangeVoisin = bmu->col;
			targetNeurone = &neurones[bmu->col][bmu->row - j];
			break;

		case 4:
			rangeVoisin = bmu->row;
			targetNeurone = &neurones[bmu->col - j][bmu->row];
			break;
		}

		for (int j = 1; j <= rangeVoisin; j++) {
			switch (i) {
				case 1: 
					targetNeurone = &neurones[bmu->col][bmu->row + j];
					break;

				case 2:
					targetNeurone = &neurones[bmu->col + j][bmu->row];
					break;

				case 3:
					targetNeurone = &neurones[bmu->col][bmu->row - j];
					break;

				case 4:
					targetNeurone = &neurones[bmu->col - j][bmu->row];
					break;
			} 
		}
	}
}

void adjustTowardData(struct BMU* bmu, struct dataRow* data) {
	//TODO IMPLEMENTS FOR MULTIPLES BMUS + ALPHA QUI EVOLUE AVEC L'APRENTISSAGE
	float alpha = 0.8;
	float coeffVoisinage = 1;
	float delta = data->c1 - bmu->neurone->c1;
	bmu->neurone->c1 += coeffVoisinage * alpha * delta;

	propagateNeigh(bmu, delta);
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
		i++;
	}
	  
	dataSetSize = i;
	struct dataRow* avgVector = normalizeAndAverageVector();
	fauxPointeurs = (int*) malloc (sizeof(int) * dataSetSize);
	initFauxPointeurs();
	initNeurones(avgVector);
	displayNeurones();
	struct BMU* bmu = findBestMatchUnit(&vectors[0]);
	printf("%f \n ", bmu->distEucl);
	adjustTowardData(bmu, &vectors[0]);
	return 0;
}