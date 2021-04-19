#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
//#include <unistd.h>

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
	char affiliation;
	int activationCount[3];
};

struct BMU {
	double distEucl;
	struct neurone* neurone;
	struct BMU* nextBmu;
	int col, row;
};

struct dataRow* vectors;

int dataSetSize;

int* fauxPointeurs;

//CONST DEFINITION
static const int NB_NEURONE_COL = 6;
static const int NB_NEURONE_ROW = 10;
struct neurone neurones[6][10];

int bmuListSize = 1;
char ID_SETOSA = 'S';
char ID_VERSICOLOR = 'E';
char ID_VIRGINICA = 'I';

double ORDONNACE_COEFF = 0.6;
double LEARNING_COEFF = 0.0088;

int neightborhoodRange = 3;
int neightborhoodRangeEvolutionPeriods = 3;
int dataDimension = 4;

double bornesData[4][2];

/*
* Test function to display vectors array
*/
void displayVectors() {
	for (int i = 0; i < dataSetSize; i++) {
		printf("%f  %f  %f  %f  %s\n ", vectors[i].c1, vectors[i].c2, vectors[i].c3, vectors[i].c4, vectors[i].id);
	}
}

/*
* Create a vector parsing line and insert it in results
*/
void creatVector(char* line, struct dataRow* results, int resultSize) {
	char* value;
	char* ptr;
	struct dataRow* tempData;
	tempData = (struct dataRow*)malloc(sizeof(struct dataRow));
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
	vectors = (struct dataRow*)realloc(vectors, (resultSize + 1) * sizeof * vectors);
	vectors[resultSize] = *tempData;
}

void updateBornes(struct dataRow* result) {
	if (result->c1 < bornesData[0][1] || bornesData[0][1] == 0) {
		bornesData[0][1] = result->c1;
	}

	if (result->c2 < bornesData[1][1] || bornesData[1][1] == 0) {
		bornesData[1][1] = result->c2;
	}

	if (result->c3 < bornesData[2][1] || bornesData[2][1] == 0) {
		bornesData[2][1] = result->c3;
	}

	if (result->c4 < bornesData[3][1] || bornesData[3][1] == 0) {
		bornesData[3][1] = result->c4;
	}


	if (result->c1 > bornesData[0][0]) {
		bornesData[0][0] = result->c1;
	}

	if (result->c2 > bornesData[1][0]) {
		bornesData[1][0] = result->c2;
	}

	if (result->c3 > bornesData[2][0]) {
		bornesData[2][0] = result->c3;
	}

	if (result->c4 > bornesData[3][0]) {
		bornesData[3][0] = result->c4;
	}

}

/*
* Normalize vectors array
* return average vector of the array
*/
struct dataRow* normalizeAndGetAverageVector() {
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
		updateBornes(&vectors[i]);
	}

	//Return Avg
	struct dataRow* result = (struct dataRow*)malloc(sizeof(struct dataRow));
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
}

void shuffleFauxPointeurs() {
	int seed = getpid();
	srand(seed += getpid());
	//Shuffle
	for (int i = dataSetSize - 1; i > 0; i--) {
		int random = rand() % (i + 1);
		swap(fauxPointeurs, i, random);
	}
}

void fillPreferencesArrays(struct neurone* neurone) {
	for (int i = 0; i < 3; i++) {
		neurone->activationCount[i] = 0;
	}
}

/*
* Init neurones with avgVector value +-randomRange
*/
void initNeurones(struct dataRow* avgVector) {
	const double randomRange = 0.2;
	//Use to change seeding for each neurone
	int seed = getpid();

	for (int i = 0; i < NB_NEURONE_COL; i++) {
		for (int j = 0; j < NB_NEURONE_ROW; j++) {
			srand(seed += getpid());
			struct neurone newNeurone;

			double randOffset = ((double)rand() / (double)(RAND_MAX)) * (bornesData[0][0] - bornesData[0][1]);
			newNeurone.c1 = randOffset + bornesData[0][1];

			randOffset = ((double)rand() / (double)(RAND_MAX)) * (bornesData[1][0] - bornesData[1][1]);
			newNeurone.c2 = randOffset + bornesData[1][1];

			randOffset = ((double)rand() / (double)(RAND_MAX)) * (bornesData[2][0] - bornesData[2][1]);
			newNeurone.c3 = randOffset + bornesData[2][1];

			randOffset = ((double)rand() / (double)(RAND_MAX)) * (bornesData[3][0] - bornesData[3][1]);
			newNeurone.c4 = randOffset + bornesData[3][1];

			newNeurone.affiliation = '0';
			fillPreferencesArrays(&newNeurone);
			neurones[i][j] = newNeurone;
		}
	}
}

double euclDistance(struct neurone* neur, struct dataRow* data) {
	double c1 = pow((data->c1 - neur->c1), 2);
	double c2 = pow((data->c2 - neur->c2), 2);
	double c3 = pow((data->c3 - neur->c3), 2);
	double c4 = pow((data->c4 - neur->c4), 2);

	return (double)sqrt(c1 + c2 + c3 + c4);
}

void insertNewBmu(struct BMU* bmu, struct BMU* newBmu) {
	if (bmu->nextBmu == NULL) {
		bmu->nextBmu = newBmu;
	}
	else {
		insertNewBmu(bmu->nextBmu, newBmu);
	}
}

void freeBMUMemory(struct BMU* bmu) {
	struct BMU* tmp;

	while (bmu != NULL)
	{
		tmp = bmu;
		bmu = bmu->nextBmu;
		free(tmp);
	}
}

struct BMU* findBestMatchUnit(struct dataRow* data) {
	// Initialisation de la chaine de BMU au premier neurone 
	struct BMU* bmu = (struct BMU*)malloc(sizeof(struct BMU));
	bmu->neurone = &neurones[0][0];
	bmu->distEucl = -1;
	bmu->nextBmu = NULL;

	for (int i = 0; i < NB_NEURONE_COL; i++) {
		for (int j = 0; j < NB_NEURONE_ROW; j++) {
			double distEucl = euclDistance(&neurones[i][j], data);
			if (bmu->distEucl > distEucl || bmu->distEucl == -1) {
				bmu->neurone = &neurones[i][j];
				bmu->distEucl = distEucl;
				bmu->col = i;
				bmu->row = j;

				if (bmu->nextBmu != NULL) {
					freeBMUMemory(bmu->nextBmu);
					bmu->nextBmu = NULL;
				}
				bmuListSize = 1;
			}
			else if (bmu->distEucl == distEucl) {
				struct BMU* newBmu = (struct BMU*)malloc(sizeof(struct BMU));
				newBmu->distEucl = distEucl;
				newBmu->neurone = &neurones[i][j];
				newBmu->col = i;
				newBmu->row = j;
				newBmu->nextBmu = bmu;
				bmu = newBmu;
				bmuListSize++;
			}
		}
	}
	return bmu;
}

/*
* Propagate learning to neighborhoors on 2 column on each side from BMU
*/
void adjustNeurone(struct neurone* neurone, struct dataRow* data, double coefficient) {
	double deltaC1 = data->c1 - neurone->c1;
	//Commenter la ligne suivante pour éviter le "segmentation fault(core dumped)"
	neurone->c1 += coefficient * deltaC1;

	double deltaC2 = data->c2 - neurone->c2;
	//Commenter la ligne suivante pour éviter le "segmentation fault(core dumped)"
	neurone->c2 += coefficient * deltaC2;

	double deltaC3 = data->c3 - neurone->c3;
	//Commenter la ligne suivante pour éviter le "segmentation fault(core dumped)"
	neurone->c3 += coefficient * deltaC3;

	double deltaC4 = data->c4 - neurone->c4;
	//Commenter la ligne suivante pour éviter le "segmentation fault(core dumped)"
	neurone->c4 += coefficient * deltaC4;
}

void assignAffiliation(struct neurone* neurone, char* dataId) {
	if (strcmp(dataId, "Iris-setosa\n") == 0) {
		neurone->activationCount[0]++;
	}
	else if (strcmp(dataId, "Iris-versicolor\n") == 0) {
		neurone->activationCount[1]++;
	}
	else if (strcmp(dataId, "Iris-virginica\n") == 0) {
		neurone->activationCount[2]++;
	}

}

void adjustTowardData(struct BMU* bmu, struct dataRow* data, double coeff) {
	if (bmuListSize > 1) {
		srand(getpid());
		int randomIndex = rand() % (bmuListSize - 1);

		while (randomIndex) {
			bmu = bmu->nextBmu;
			randomIndex--;
		}
	}

	int xNeighbStart = bmu->col - neightborhoodRange < 0 ? 0 : bmu->col - neightborhoodRange;
	int xNeighbEnd = bmu->col + neightborhoodRange < NB_NEURONE_COL ? bmu->col + neightborhoodRange : NB_NEURONE_COL;
	int yNeighbStart = bmu->row - neightborhoodRange < 0 ? 0 : bmu->row - neightborhoodRange;
	int yNeighbEnd = bmu->row + neightborhoodRange < NB_NEURONE_ROW ? bmu->row + neightborhoodRange : NB_NEURONE_ROW;

	for (int i = xNeighbStart; i <= xNeighbEnd; i++) {
		for (int j = yNeighbStart; j <= yNeighbEnd; j++) {
			adjustNeurone(&neurones[i][j], data, coeff);
			assignAffiliation(&neurones[i][j], data->id);
		}
	}
	freeBMUMemory(bmu);
}

char* evaluateActivationTedencies(struct neurone* neurone) {
	int indexMaxTendencie = -1;
	int maxActivation = -1;
	int borderNeuron = 0;

	for (int i = 0; i < 3; i++) {
		if (neurone->activationCount[i] != 0) {
			if (neurone->activationCount[i] > maxActivation) {
				maxActivation = neurone->activationCount[i];
				indexMaxTendencie = i;
				borderNeuron = 0;
			}
			else if (abs(neurone->activationCount[i] - maxActivation) <= 3 && neurone->activationCount[i] != 0) {
				borderNeuron = 1;
			}
		}
	}

	if (indexMaxTendencie == 0) {
		neurone->affiliation = ID_SETOSA;
	}
	else if (indexMaxTendencie == 1) {
		neurone->affiliation = ID_VERSICOLOR;
	}
	else if (indexMaxTendencie == 2) {
		neurone->affiliation = ID_VIRGINICA;
	}
	else {
		neurone->affiliation = 'O';
	}
	if (borderNeuron) {
		printf("\x1b[31m %c \x1b[0m", neurone->affiliation);
	}
	else {
		printf(" %c ", neurone->affiliation);
	}
}

void evaluateNeuronesAffiliation(struct BMU* bmu) {
	for (int i = 0; i < NB_NEURONE_COL; i++) {
		for (int j = 0; j < NB_NEURONE_ROW; j++) {
			evaluateActivationTedencies(&neurones[i][j]);
		}
		printf("\n");
	}
}

// return true if the prediction was succesful false if not
int predictRace(struct dataRow* vector) {
	struct BMU* bmu = findBestMatchUnit(vector);
	int result = 0;
	if (strcmp(vector->id, "Iris-setosa\n") == 0 && bmu->neurone->affiliation == ID_SETOSA ||
		strcmp(vector->id, "Iris-versicolor\n") == 0 && bmu->neurone->affiliation == ID_VERSICOLOR ||
		strcmp(vector->id, "Iris-virginica\n") == 0 && bmu->neurone->affiliation == ID_VIRGINICA) {
		result = 1;
	}
	freeBMUMemory(bmu);
	return result;
}
double evaluateAccruacy() {
	int accruacy = 0;
	for (int k = 0; k < dataSetSize; k++) {
		accruacy += predictRace(&vectors[fauxPointeurs[k]]);
	}
	return ((double)accruacy / (double)dataSetSize) * 100;
}

int main() {
	//File parsing
	char line[128];

	FILE* data = fopen("iris3.data", "r");
	if (data == NULL) {
		perror("Error opening file");
		return(-1);
	}
	int i = 0;
	while (fgets(line, sizeof(line), data) && isdigit(line[0])) {
		creatVector(line, vectors, i);
		i++;
	}
	dataSetSize = i;

	//Data process
	struct dataRow* avgVector = normalizeAndGetAverageVector();
	fauxPointeurs = (int*)malloc(sizeof(int) * dataSetSize);

	//Learning
	initFauxPointeurs();
	int nbIteration = dataDimension * 500;
	int firstPhase = nbIteration / 4;
	double coeffLearning;

	double besti = 0;
	double bestj = 0;
	double bestScore = 0.0;
	double accruacy;

	initNeurones(avgVector);

	for (int l = 0; l < nbIteration; l++) {
		shuffleFauxPointeurs();
		struct BMU* bmu = NULL;
		if (l < firstPhase) {
			if (l % (int)(firstPhase / neightborhoodRangeEvolutionPeriods) - 1 == 0 && l != 0) {
				neightborhoodRange--;
			}
			coeffLearning = ORDONNACE_COEFF;
		}
		else {
			neightborhoodRange = 2;
			coeffLearning = LEARNING_COEFF;
		}
		for (int j = 0; j < dataSetSize; j++) {
			bmu = findBestMatchUnit(&vectors[fauxPointeurs[j]]);
			adjustTowardData(bmu, &vectors[fauxPointeurs[j]], coeffLearning);
		}
		evaluateNeuronesAffiliation(bmu);
		printf("\n////////////////////\n");
		shuffleFauxPointeurs();
		accruacy = evaluateAccruacy();
		printf("generation: %d SOM has %f percent accruacy\n", l, accruacy);
	}
	return 0;
}