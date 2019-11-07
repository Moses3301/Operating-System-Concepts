#include <stdio.h>
#include <math.h>
#define _CRT_SECURE_NO_WARNINGS

#define BYTE_SIZE		8

#define FUNCTION_NUM    5
#define ADD_POLYGON		0
#define PRINT_POLYGON	1
#define PERIMETER		2
#define DIAGONALS		3
#define AREA			4

typedef enum { false, true } bool;
enum POLYGON { HEXAGON = 6, HEPTAGON = 7, OCTAGON = 8 };
struct Node* LIST_HEAD;
struct Node* LIST_TAIL;
enum POLYGON NEW_POLLY_TYPE;
bool END = false;


struct Point {
	signed char x, y;
};
struct PointNode {
	struct Point* point;
	struct PointNode* next;
};
struct polygon {
	enum POLYGON type;
	//representation for the vertices
	struct PointNode* vertices;
};
struct Node {
	struct polygon* poly;
	struct Node* next;
};

// add new polygon to the list
void add_polygon(struct polygon* iPolyPtr);
// print the type of polygon and its vertices
void print_polygon(struct polygon* iPolyPtr);
// calculate and print the perimeter
void print_perimeter(struct polygon* iPolyPtr);
// calculate and print the length of all diagonals
void print_diagonals(struct polygon* iPolyPtr);
// calculate and print the area
void print_area(struct polygon* iPolyPtr);

double  distanceBetweenTwoPoints(struct Point iPoint1, struct Point iPoint2);
double calculatePerimeter(struct polygon* iPolyPtr);
double calculateDiagonals(struct polygon* iPolyPtr);
double calculateErea(struct polygon* iPolyPtr);

void decodeAndExecute(long long opCode);

void (*func[FUNCTION_NUM])(struct polygon* ) = {
	add_polygon,
	print_polygon,
	print_perimeter,
	print_diagonals,
	print_area
};

int main() {
	while (1)
	{
		long long opCode;
		scanf_s("%lx", &opCode);
		decodeAndExecute(opCode);
		if (END) {
			break;
		}
	}
}

void print_polygon(struct polygon* iPolyPtr)  {
	enum POLYGON polyType = iPolyPtr->type;
	//Prints polygon type 
	char* name;
	switch(polyType) {
		case HEXAGON:
			name = "hexagon";
			break;
		case HEPTAGON:
			name = "heptagon";
			break;
		case OCTAGON:
			name = "octagon";
			break;
		default:
			name = "UNKNOWEN";
	}
	printf("%s ", name);
	//Prints all the vertices;
	struct PointNode* verx = iPolyPtr->vertices;
	for (int i = 0; i < polyType; i++) {
		printf("{%d, %d} ", verx->point->x,verx->point->y);
		verx = verx->next;
	}
	printf("\n");
}

void decodeAndExecute(long long opCode) {
	bool isEnd = opCode & 0b01 ? true : false;
	END = isEnd;
	bool isNewPolly = opCode & 0b10 ? true : false;
	opCode = opCode >> 2;
	NEW_POLLY_TYPE = opCode & 0b01111;
	opCode = opCode >> 4;
	bool isPrintPolygon =	opCode & 0b0001 ? true : false;
	bool isPerimeter =		opCode & 0b0010 ? true : false;
	bool isArea =			opCode & 0b0100 ? true : false;
	bool isDiagonals =		opCode & 0b1000 ? true : false;
	opCode = opCode >> 4;
	short actionPolyType = opCode & 0b01111;

	if (isNewPolly) { func[ADD_POLYGON](NULL); }
	struct Node* node = LIST_HEAD;
	while (node) {
		if (isInSelectedGroup(node->poly, actionPolyType)) {
			if (isPrintPolygon) { func[PRINT_POLYGON](node->poly); }
			if (isPerimeter) { func[PERIMETER](node->poly); }
			if (isArea) { func[AREA](node->poly); }
			if (isDiagonals) { func[DIAGONALS](node->poly); }
		}
		node = node->next;
	}
}

bool isInSelectedGroup(struct polygon* iPoly, short iCon) {
	bool res = false;
	switch (iCon)
	{
	case 0x0F:
		res = true;
		break;
	case 0:
		res = (iPoly == LIST_TAIL->poly);
		break;
	default:
		res = (iPoly->type == iCon);
		break;
	}
	return res;
}

void add_polygon(struct polygon* iPolyPtr) {
	struct polygon* newPolygon = (struct polygon*)malloc(sizeof(struct polygon));
	newPolygon->type = NEW_POLLY_TYPE;
	struct PointNode* newPolygonVertex = (struct PointNode*)malloc((NEW_POLLY_TYPE) * sizeof(struct PointNode));
	newPolygon->vertices = newPolygonVertex;
	struct Point* newPolygonPoints = (struct Point*)malloc((NEW_POLLY_TYPE) * sizeof(struct Point));

	long long pointsBitArr = 0;
	int pointsBitArrSize = 4;
	for (int i = 0; i < NEW_POLLY_TYPE; i++) {
		if (i % pointsBitArrSize == 0) {
			scanf_s("%llx", &pointsBitArr);
		}
		newPolygonPoints[i].x = pointsBitArr & 0xFF;
		pointsBitArr = pointsBitArr >> BYTE_SIZE;
		newPolygonPoints[i].y = pointsBitArr & 0xFF;
		pointsBitArr = pointsBitArr >> BYTE_SIZE;
		newPolygonVertex[i].point = &newPolygonPoints[i];
		newPolygonVertex[i].next = &newPolygonVertex[(i + 1) % NEW_POLLY_TYPE];
	}

	struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
	newNode->poly = newPolygon;
	newNode->next = NULL;
	if (LIST_HEAD) { LIST_TAIL->next = newNode; LIST_TAIL = newNode;}
	else { LIST_HEAD = LIST_TAIL = newNode; }
}

void print_perimeter(struct polygon* iPolyPtr) {
	printf("perimeter = %.1f\n", calculatePerimeter(iPolyPtr));
}

void print_diagonals(struct polygon* iPolyPtr) {
	printf("diagonals = %.1f \n", calculateDiagonals(iPolyPtr));
}

void print_area(struct polygon* iPolyPtr) {
	printf("area = %.1f\n", calculateErea(iPolyPtr));
}

double calculatePerimeter(struct polygon* iPolyPtr) {
	double perimeter = 0.0f;
	struct PointNode* vertx = iPolyPtr->vertices;
	for (int i = 0; i < iPolyPtr->type; i++) {
		perimeter = perimeter + distanceBetweenTwoPoints(*(vertx->point), *(vertx->next->point));
		vertx = vertx->next;
	}
	return perimeter;
}

double calculateDiagonals(struct polygon* iPolyPtr) {
	struct PointNode* pointOneNode = iPolyPtr->vertices;
	double res = 0;
	for (int i = 0; i < iPolyPtr->type; i++) {
		struct PointNode* pointOneTwo = pointOneNode->next;
		for (int j = i + 1; j < iPolyPtr->type; j++) {
			res += distanceBetweenTwoPoints(*pointOneNode->point, *pointOneTwo->point);
			pointOneTwo = pointOneTwo->next;
		}
		pointOneNode = pointOneNode->next;
	}
	res = res - calculatePerimeter(iPolyPtr);
	return res;
}

double calculateErea(struct polygon* iPolyPtr) {
	int n = iPolyPtr->type;
	struct PointNode* pointNode = iPolyPtr->vertices;

	int first = 0;
	int second = 0;
	for (int i = 0; i < n; i++) {
		first += (pointNode->next->point->x) * (pointNode->point->y);
		second += (pointNode->point->x) * (pointNode->next->point->y);
		pointNode = pointNode->next;
	}
	return (double)abs((first - second)) / 2;
}

double  distanceBetweenTwoPoints(struct Point iPoint1, struct Point iPoint2) {
	double  x, y;
	x = pow(iPoint1.x - iPoint2.x, 2);
	y = pow(iPoint1.y - iPoint2.y, 2);
	double  res = (double)sqrt(x + y);
	return res;
}
