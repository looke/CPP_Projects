/*----------------------------------------------------------------------------

 Module: Modle1 Active Edge table.c
 Modified time:  2017/07/15
 version��1.0

 ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define subfield  80

typedef struct Xpoint{
	short x, y;
} XPoint;

typedef struct pointstr{
	struct pointstr *nextp;
	XPoint v;
} pointstr;

pointstr *points;

/*  edge structure  */

typedef struct edge{
	int ymax;
	float xmin;
	float slopeincr;
	struct edge *nextedge;
} edge;

struct edge *et[subfield]; /* edge table as pointers to edges */


int interval;
int pixelcounter = 0;

/****************************************************************************
 Input file and Output file
 *****************************************************************************/

typedef struct {
	pointstr ** polyHeaderList;
	int length;
} pointstr_linklist;

int countRowNumber(FILE* pFile) {
	char buf;
	int rowNum = 0;

	//int hasNumber = 0;

	while (1) {
		if (fread(&buf, sizeof(char), 1, pFile) != 1) {
			break;
		}
		switch (buf) {
		//���з�
		case 10: {
			//if(hasNumber)
			//{
			rowNum++;
			//}
			//hasNumber = 0;
			break;
		}
			//����
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57: {
			//hasNumber = 1;
			break;
		}
		}
	}
	return rowNum;
}

void initRowSize(FILE* pFile, char** pRowPointers, int* pRowPointerSize,
		int rowNum) {
	char buf;
	int rowCharNum = 0;
	char** pCurrent_RowPointers = pRowPointers;
	int* pCurrent_RowPointerSize = pRowPointerSize;

	//��ʼ��ÿ�е����ݳ���
	//int hasNumber = 0;
	rewind(pFile);
	while (1) {
		if (fread(&buf, sizeof(char), 1, pFile) != 1) {
			break;
		}
		switch (buf) {
		//���з�
		case 10: {
			//���������ݵ��У����������ռ�
			//if(hasNumber)
			//{
			*pCurrent_RowPointers = malloc(sizeof(char) * rowCharNum);
			*pCurrent_RowPointerSize = rowCharNum;

			pCurrent_RowPointers++;
			pCurrent_RowPointerSize++;
			//rowIndex++;
			//}
			rowCharNum = 0;
			//hasNumber = 0;
			break;
		}
			//����
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57: {
			//hasNumber = 1;
			rowCharNum++;
			break;
		}
		default: {
			rowCharNum++;
			break;
		}
		}
	}
}

void initRowContent(FILE* pFile, char** pRowPointers, int* pRowPointerSize,
		int rowNum) {
	char buf;
	//int rowCharNum = 0;

	char** pCurrent_RowPointers = pRowPointers;
	char* pCurrent_RowContentIndex = *pCurrent_RowPointers;
	int* pCurrent_RowPointerSize = pRowPointerSize;

	int rowContentIndex = 0;
	//��ʼ��ÿ�е����ݳ���
	//int hasNumber = 0;
	while (1)
	{
		if (fread(&buf, sizeof(char), 1, pFile) != 1)
		{
			break;
		}
		switch (buf)
		{
			//���з�
			case 10:
			{
				pCurrent_RowPointers++;
				pCurrent_RowContentIndex = *pCurrent_RowPointers;

				pCurrent_RowPointerSize++;

				rowContentIndex = 0;
				break;
			}

			default:
			{
				char* pChar = pCurrent_RowContentIndex + rowContentIndex;
				*pChar = buf;
				rowContentIndex++;

				break;
			}
		}
	}
}

int isDataRow(char* p_rowPointer, int charSize) {
	//��Ҫ����ǰ��n���ַ����ж��Ƿ����ڱ������ݵ���
	int charToSkip = 9;

	char temp;
	for (int j = 0; j < charSize; j++) {
		temp = *(p_rowPointer + j);

		//���ַ����ǿո��һ������������
		if (0 == j && temp != 32) {
			return 0;
		}

		//����ǰ���n���ַ�
		if (j < charToSkip) {
			continue;
		}

		//����ǰ���n+1���ַ������־�Ϊ������
		if (temp >= 48 && temp <= 57) {
			return 1;
		} else {
			return 0;
		}
	}
	return 0;
}

int countDataRowNumber(char** p_rowPointers, int* p_rowPointerSize, int rowNum) {
	int dataRowNumber = 0;
	char* p_CurrentRowPointer = *p_rowPointers;

	for (int i = 0; i < rowNum; i++) {
		p_CurrentRowPointer = *(p_rowPointers + i);
		int charSize = *(p_rowPointerSize + i);
		if (isDataRow(p_CurrentRowPointer, charSize)) {
			dataRowNumber++;
		}
	}
	return dataRowNumber;
}

int countDataNumber(char* p_rowPointer, int charSize) {
	//��Ҫ����ǰ��n���ַ���������ݸ���
	int charToSkip = 8;

	char temp;
	int dataNumber = 0;
	for (int j = charToSkip; j < charSize; j++) {
		temp = *(p_rowPointer + j);

		//���ַ����ǿո��һ������������
		if (32 == temp) {
			dataNumber++;
		}
	}
	return dataNumber;
}
;

void initDataRowSize(char** p_rowPointers, int* p_rowPointerSize, int rowNum,
		int** p_dataRowPointers, int* p_dataRowPointerSize) {
	char* p_CurrentRowPointer = *p_rowPointers;
	int** p_CurrentDataRowPointer = p_dataRowPointers;
	int* p_CurrentDataRowPointerSize = p_dataRowPointerSize;
	//int dataRowIndex = 0;
	int dataNum = 0;
	for (int i = 0; i < rowNum; i++) {
		p_CurrentRowPointer = *(p_rowPointers + i);
		int charSize = *(p_rowPointerSize + i);
		if (isDataRow(p_CurrentRowPointer, charSize)) {
			dataNum = countDataNumber(p_CurrentRowPointer, charSize);
			//Ϊ���ݷ���洢�ռ�
			*p_CurrentDataRowPointer = malloc(sizeof(int) * dataNum);

			//��¼��������
			*p_CurrentDataRowPointerSize = dataNum;

			p_CurrentDataRowPointer++;
			p_CurrentDataRowPointerSize++;
		}
	}
}
void steupDataRowContent(char* p_CurrentRowPointer, int charSize,
		int* p_CurrentDataRowPointer) {
	char temp;
	int temp_int = 0;
	int charToSkip = 9;
	int dataIndex = 0;
	int hasData = 0;
	for (int j = charToSkip; j < charSize; j++) {
		temp = *(p_CurrentRowPointer + j);

		switch (temp) {
		case 32: //�ո��
		{
			*(p_CurrentDataRowPointer + dataIndex) = temp_int;
			dataIndex++;
			temp_int = 0;
			hasData = 0;
			break;
		}
			//����
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57: {
			temp_int = 10 * temp_int + temp - 48;
			hasData = 1;
			break;
		}
		}
	}
	if (hasData) //�����ʣ�������û�г�ȡ�Ļ�����ȡһ��
	{
		*(p_CurrentDataRowPointer + dataIndex) = temp_int;
	}

}

/*
 * �ж��Ƿ���������У����ݵ�8���ַ��ǡ�1��Ϊ�������ݣ��ǡ�-��Ϊ��������
 *
 */
int isDataIndependent(char* p_CurrentRowPointer) {
	//ȡ��8���ַ�����
	if (*(p_CurrentRowPointer + 7) == 49)
		return 1;
	else
		return 0;
}

void initDataRowContent(char** p_rowPointers, int* p_rowPointerSize, int rowNum,
		int** p_dataRowPointers, int* p_dataRowPointerSize,
		int* p_dataIsIndependent) {
	char* p_CurrentRowPointer = *p_rowPointers;
	int* p_CurrentDataRowPointer = *p_dataRowPointers;
	int* p_CurrentDataIndependentPointer = p_dataIsIndependent;
	int dataRowIndex = 0;

	for (int i = 0; i < rowNum; i++) {
		p_CurrentRowPointer = *(p_rowPointers + i);
		int charSize = *(p_rowPointerSize + i);

		//����������еĻ�����Ҫ��ȡ����
		if (isDataRow(p_CurrentRowPointer, charSize)) {
			p_CurrentDataRowPointer = *(p_dataRowPointers + dataRowIndex);
			steupDataRowContent(p_CurrentRowPointer, charSize,
					p_CurrentDataRowPointer);

			p_CurrentDataIndependentPointer = p_dataIsIndependent
					+ dataRowIndex;
			*p_CurrentDataIndependentPointer = isDataIndependent(
					p_CurrentRowPointer);

			dataRowIndex++;
		}
	}
}

void constructPolyChain(pointstr **p_polyChain, int** p_dataRowPointers,
		int* p_dataRowPointerSize, int* p_isDataIndependent, int dataRowNum) {
	pointstr* p_CurrentPoly = *p_polyChain;
	pointstr* p_NextPoly;

	//int dataRowIndex = 0;
	int isCurrentDataIndependent;
	int* p_CurrentDataRowPointer;

	int polyIndex = -1;

	for (int i = 0; i < dataRowNum; i++) {
		p_CurrentDataRowPointer = *(p_dataRowPointers + i);
		//��ǰ���Ƿ������־λ
		isCurrentDataIndependent = *(p_isDataIndependent + i);
		if (isCurrentDataIndependent) {
			polyIndex++;
		}
		int dataSize = *(p_dataRowPointerSize + i);
		//����һ������
		for (int j = 0; j < dataSize; j = j + 2) {

			p_NextPoly = (pointstr*) malloc(sizeof(pointstr));
			p_NextPoly->v.x = *(p_CurrentDataRowPointer + j);
			p_NextPoly->v.y = *(p_CurrentDataRowPointer + j + 1);
			p_NextPoly->nextp = 0;

			//���������е���Ԫ�ص�ַ��Ӧ��ֵ������ͷ
			if (j == 0 && isCurrentDataIndependent) {
				*(p_polyChain + polyIndex) = p_NextPoly;

			}

			//�Ƕ��������е���Ԫ�ص�ַ����Ҫ���ӵ�ǰһ�������ڵ��nextָ��
			if (j == 0 && !isCurrentDataIndependent) {
				p_CurrentPoly->nextp = p_NextPoly;

			}

			//��������Ԫ�أ���Ҫ���ӵ�ǰһ�������ڵ��nextָ��
			if (j != 0) {
				p_CurrentPoly->nextp = p_NextPoly;
			}

			//��ǰ�ڵ��ƶ�����һ��
			p_CurrentPoly = p_NextPoly;
		}
	}

}

pointstr_linklist* parseNSFFile(char* fileDir) {
	FILE *pFile = fopen(fileDir, "r");
	if (pFile == NULL) {
		fprintf(stderr, "Cannot open file.\n");
		return 0;
	}

	//������
	int rowNum = countRowNumber(pFile);
	//������
	char* rowPointers[rowNum];
	int rowPointerSize[rowNum];

	rewind(pFile);
	//��ʼ���������ݵ�size
	initRowSize(pFile, rowPointers, rowPointerSize, rowNum);

	//��ʼ��ÿ�е�����
	rewind(pFile);
	initRowContent(pFile, rowPointers, rowPointerSize, rowNum);

	fclose(pFile);

	int dataRowNum = countDataRowNumber(rowPointers, rowPointerSize, rowNum);
	//�����е�����
	int* dataRowPointers[dataRowNum];
	int dataRowPointerSize[dataRowNum];
	int isDataIndependent[dataRowNum];

	initDataRowSize(rowPointers, rowPointerSize, rowNum, dataRowPointers,
			dataRowPointerSize);
	initDataRowContent(rowPointers, rowPointerSize, rowNum, dataRowPointers,
			dataRowPointerSize, isDataIndependent);

	//ͳ�ƶ�������εĸ���
	int independentShapeNum = 0;
	for (int i = 0; i < dataRowNum; i++) {
		if (isDataIndependent[i]) {
			independentShapeNum++;
		}
	}

	//���� ��������εĸ��� ��������������
	pointstr** polyChain = malloc(independentShapeNum * sizeof(pointstr*));

	//�����ѽ����������ݹ������ε�����
	constructPolyChain(polyChain, dataRowPointers, dataRowPointerSize,
			isDataIndependent, dataRowNum);

	pointstr_linklist* p_polylist = (pointstr_linklist*) malloc(
			sizeof(pointstr_linklist));
	p_polylist->length = independentShapeNum;
	p_polylist->polyHeaderList = polyChain;
	return p_polylist;
}

void read_file() {
	//�ļ�·�����ļ��������������·����Ҳ����д����·��
	//char fileName[50] = "PattentTest_zp.nsf";
	char fileName[50] = "PattentTest_big20170824.nsf";

	//���������������ṹ�壬��������ͷ���������ͷ����
	pointstr_linklist* p_polyList = parseNSFFile(fileName);

	//����ͷ������Ҳ���Ƕ�������ε�����
	int independentShapeNum = p_polyList->length;

	//���㷨�ӿڸ�ֵ
	if (independentShapeNum > 0) {
		points = *(p_polyList->polyHeaderList);
	}
}

void clear_points() {
	pointstr *tmpp, *delp;
	for(tmpp=points;tmpp!=NULL;){
		delp = tmpp;
		tmpp = tmpp->nextp;
		free(delp);
	}
	points = NULL;
}

void writePixel(int x, int y, int value) {
	FILE *fp;
	fp = fopen("PattentTest_big.dat", "a+");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file.\n");
	}
	char str_x[10];
	char str_y[10];

	itoa(x, str_x, 10);
	itoa(y, str_y, 10);
	fputs(str_x, fp);
	fputs(" ", fp);
	fputs(str_y, fp);
	fputs(" ", fp);
	fclose(fp);

	pixelcounter++;
}

/****************************************************************************
 Find the point_ymin point_ymax where the scanline will start and end
 *****************************************************************************/

/*********************************************************************
 draws a line using the midpoint line algorithm, and draws lines
 between 0 and 1 and all others  by  using reflections. given 2 points
 **********************************************************************/
void drawLine(pointstr *p1, pointstr *p2, int interval) {

	int temp, quadrant4 = 0, quadrant1 = 0; /* variable declaration */
	int x0 = p1->v.x;
	int y0 = p1->v.y;
	int x1 = p2->v.x;
	int y1 = p2->v.y;
	int dx, dy, d, incrE, incrNE, x, y;

	double slope = (double) (y1 - y0) / (x1 - x0);

	/*  this case checks if x0 > x1, if it is the points are
	 switched.*/

	if (x0 > x1) {
		temp = x0;
		x0 = x1;
		x1 = temp;
		temp = y0;
		y0 = y1;
		y1 = temp;
		slope = (double) (y1 - y0) / (x1 - x0);
	}

	/*  test if slope is negative, then reflect about the x axis
	 and set  flag quadrant1  */

	if (slope < 0) {
		y0 = -y0;
		y1 = -y1;
		quadrant1 = 1;
	}

	slope = (double) (y1 - y0) / (x1 - x0);

	/*  test if slope > 1 if so, then reflect about x=y  */

	if (slope > 1) {
		temp = x0;
		x0 = y0;
		y0 = temp;
		temp = x1;
		x1 = y1;
		y1 = temp;
		quadrant4 = 1;
	}

	dx = x1 - x0;
	dy = y1 - y0;
	d = 2 * dy - dx;
	incrE = 2 * dy;
	incrNE = 2 * (dy - dx);
	x = x0;
	y = y0;

	writePixel(x, y, interval); /*  the start pixel  */

	while (x < x1) {
		if (d <= 0) { /* choose E */
			d += incrE;
			x++;
		} else { /*  choose NE */
			d += incrNE;
			x++;
			y++;
		}

		/*  now write the pixels by undoing the reflections the
		 appropriate reflections by checking the quadrant flags */

		if ((quadrant1 == 1) && (quadrant4 == 1))
			writePixel(y, -x, interval);
		else if (quadrant1 == 1)
			writePixel(x, -y, interval);
		else if (quadrant4 == 1)
			writePixel(y, x, interval);
		else
			writePixel(x, y, interval); /*selected pixel closest to line */
	}

}

/****************************************************************************
 remove all edges from active edge table whose ymax=y, ie those not involved
 in the next scan line
 *****************************************************************************/
void remove_from_aet(edge **aet, int y) {

	edge *edge1, *next;
	edge1 = *aet;

	/*  first case is if the head edges need to be removed  then
	 the aet pointer must be changed   */

	while ((edge1 != NULL) && (edge1->ymax == y)) {
		*aet = edge1->nextedge;
		edge1 = *aet;
	}

	if (edge1 != NULL)
		next = edge1->nextedge;

	/*  second case is  edges in the middle or end of active edge table
	 need to be removed  */

	while (next != NULL) {
		if (next->ymax == y) {
			next = next->nextedge;
			edge1->nextedge = next;
		}

		else {
			edge1 = next;
			next = edge1->nextedge;
		}
	}
}

/*****************************************************************************
 fill in the current scan line, ie fill in all the pixels between
 successive pairs in tbe active edge table
 ****************************************************************************/
void fill_scanline(edge *aet, int y, int interval) {

	edge *edge1 = NULL, *edge2 = NULL;
	float x1, x2;
	int start, end, temp;

	edge1 = aet;

	if (edge1 != NULL && edge1->nextedge != NULL)
		edge2 = edge1->nextedge;

	/*  cycle through the active edge table filling in between
	 successive pairs of edges */

	while ((edge1 != NULL) && (edge2 != NULL)) {
		x1 = floor(edge1->xmin + 0.5);
		x2 = floor(edge2->xmin + 0.5);
		start = (int) x1;
		end = (int) x2;

		/* check to make sure start>end, else switch em */
		if (start > end) {
			temp = start;
			start = end;
			end = temp;
		}

		/*  fill pixels from start value to end value  */

		while (start <= end) {
			writePixel(start, y, interval);
			start++;
		}

		/*  set edge1 and edge2 to the next pair of edges in aet */

		if (edge2->nextedge != NULL) {
			edge1 = edge2->nextedge;
			edge2 = edge1->nextedge;
		}
		else {
			edge1 = NULL;
			edge2 = NULL;
		}
	}

}

/***************************************************************************
 update the active active edge table by adding each edge's 1/slope to its
 x value
 ****************************************************************************/
void update_aet(edge *aet) {
	edge *edge1;
	edge1 = aet;

	while (edge1 != NULL) {
		edge1->xmin = edge1->xmin + edge1->slopeincr;
		edge1 = edge1->nextedge;
	}
}

/***************************************************************************
 insert the edge into the active edge table in the proper sorted position
 ****************************************************************************/
void insert_into_aet(edge *edge1, edge **aetp) {

	float newx;
	edge *next, *current, *temp;

	newx = edge1->xmin;
	current = *aetp;
	next = current->nextedge;

	/*  cycle through aet until the new edge's proper position in aet is
	 found by comparing the x values  */

	while ((next != NULL) && (newx > current->xmin) && (newx > next->xmin)) {
		current = next;
		next = current->nextedge;

	}

	/*  insert the new edge into the proper position
	 either in the front, middle or end of aet  */

	if ((current->nextedge != NULL) && (newx > current->xmin)) {
		temp = current->nextedge;
		current->nextedge = edge1;
		edge1->nextedge = temp;
	} else if (newx < current->xmin) {
		edge1->nextedge = current;
		*aetp = edge1;
	} else
		current->nextedge = edge1;
}

/***************************************************************************
 insert the edge into the edge table at given bucket, in the proper sorted
 position
 ***************************************************************************/
void insert_into_et(edge *edge1, int bucket) {
	/*  bucket means ymin  */

	float newx;
	edge *next, *current, *temp;

	newx = edge1->xmin;
	current = et[bucket];
	next = current->nextedge;

	/*  find the proper position of the edge1 in edge table by comparing x values.
	 *  ����ʹnewx��current->xmin��next->xmin֮�䡣
	 ��Ӧһ��yɨ���ߴ�������߽���������ˣ�Ҫ��xmin�����Է�ֹ�ڰ׻������⣬�����ǲ���Ҫ��
	 ����PG�������һ��ymin��ymax��et��ֻ������xmin��ͬʱ������et[ymin]�Ǽ�ϵĲ��������ģ���û����ô��yminֵ��ymin���������ڶ���ζ˵��� */

	while ((next != NULL) && (newx > current->xmin) && (newx > next->xmin)) {
		current = next;
		next = current->nextedge;
	}

	/*  insert the edge into the edge table
	 either in the front, middle or end of edge table bucket */

	if ((current->nextedge != NULL) && (newx > current->xmin)) {
		temp = current->nextedge;
		current->nextedge = edge1;
		edge1->nextedge = temp;
	} else if (newx < current->xmin) {
		edge1->nextedge = current;
		et[bucket] = edge1;
	} else {
		current->nextedge = edge1;
		edge1->nextedge = NULL;
	}
}

/*************************************************************************
 build the edge Table from the link list of points
 **************************************************************************/
void buildedgetable(pointstr *p1, int numpoints) {
	int x0, y0, x1, y1, ymin;
	edge *edge1;
	pointstr *p2;

	p2 = p1->nextp;

	/* initialize edge table to empty  */
	for (int i = 0; i < subfield; i++) {
		et[i] = NULL;}

	/*  for edge successive pair of points in the list of points
	 create an edge  from point1 to point2 */

	while (p1->nextp != NULL) {

		x0 = p1->v.x;
		y0 = p1->v.y;
		x1 = p2->v.x;
		y1 = p2->v.y;

		edge1 = malloc(sizeof(edge)); /* create new edge  */

		/*  set the edge's xmin,ymax and 1/slope given point1 and point2 */

		if (y0 !=y1) {   /*if y0=y1,slope=0, don't build a edgetable*/

			/*  if x1=x0, slopeinv=infinity */
			if (y0 < y1) {
				edge1->xmin = (float) x0;
				edge1->ymax = y1;
				ymin = y0;
			    }
			else {
				edge1->xmin = (float) x1;
				edge1->ymax = y0;
				ymin = y1;
				}

			edge1->slopeincr = (float)(x1-x0)/(y1-y0);
			edge1->nextedge = NULL; /* why?  */

			/* insert the new edge into the edge table  */
			if (et[ymin] == NULL)
				et[ymin] = edge1;
			else
				insert_into_et(edge1, ymin);
		}

	    /* move to next edge */
	     	p1 = p2;
	     	if(p1->nextp==NULL)   break;
	    	p2 = p1->nextp;

	}

	/* do once more for the edge made up of the point at the
	 head of the list and the point at the tail  */

	p1 = points;
	x0 = p1->v.x;
	y0 = p1->v.y;
	x1 = p2->v.x;
	y1 = p2->v.y;

	if (y0 !=y1) {   /*if y0=y1,slope=0, don't build a edgetable*/

		/*  if x1=x0, slopeinv=infinity */
		if (y0 < y1) {
			edge1->xmin = (float) x0;
			edge1->ymax = y1;
			ymin = y0;
		 }
		else {
			edge1->xmin = (float) x1;
			edge1->ymax = y0;
			ymin = y1;
		}

		edge1->slopeincr = (float)(x1-x0)/(y1-y0);
		edge1->nextedge = NULL; /* why?  */


	if (et[ymin] == NULL)
		et[ymin] = edge1;
	else
		insert_into_et(edge1, ymin);
	}

}

/**************************************************************************

 fill the Polygon, first build edge, then add entering edges to the
 aet and remove leaving edges , and sort edges in aet.
 then fill in pixels between successive pairs in aet for the current
 scan line, then go to next scan line and update value of x.
 *************************************************************************/
void fillPolygon(pointstr *p1, int numpoints, int interval) {

	int y = 0;
	edge *aet, *edge1;
	edge1 = NULL;

	buildedgetable(p1, numpoints);

	/*  go to first non empty bucket in edge table */

	while ((et[y] == NULL) && (y < subfield))
		y++;

	/* initialize aet to empty */

	aet = NULL;

	/*  repeat until reach the final scan line,
	 * ����ָ��ͬһ��ymin�У����и�xmin��scanline��ÿ��һ��ymin����ִ��һ�������whileѭ��������y++ */

	while (y < subfield) {
		edge1 = et[y];

		/*  add entering edges to aet  */

		while (et[y] != NULL) {

			if (aet == NULL) {
				aet = et[y];
				et[y] = NULL;
				edge1 = NULL;
			}
			else { /*  ���aet�Ѵ��ڣ�����et[y]ָ��ָ����һ��et[y]��ָ�룬����ͬymin�µ���һ��xin�����ҽ�����ǰymin��aet������*/
				et[y] = edge1->nextedge;
				edge1->nextedge = NULL;
				insert_into_aet(edge1, &aet); /* **aetΪָ���ָ�룬&aet��*aet��Ӧ������ */
			}
		}
		/* remove leaving edges from aet  */
		remove_from_aet(&aet, y);

		/*  fill between edges in aet */
		fill_scanline(aet, y, interval);
		y++;

		/*  add 1/slope to all edges in aet  */
		update_aet(aet);
	}

	clear_points();

}

/*************************************************************************
 callback function for fillPolygon button
 this funtion first draws the outline of the polygon from the
 link list of points,  then its calls the fillPolygon procedure
 with the list of points which then fills the polygon
 **************************************************************************/
int main() {

	FILE *fp;
	fp = fopen("PattentTest_big.dat", "w");
	fclose(fp);

	read_file();

	int pointcount = 3;
	int interval = 1;

	/* pointstr *point1,*point2;

	 int count=1;
	 point1=points;
	 point2=point1->nextp;
	 pointcount=2;

	 while (point1->nextp!=NULL){
	 drawLine(point1,point2,interval);
	 point1=point2;
	 point2=point1->nextp;
	 pointcount++;
	 count++;
	 }
	 pointcount--;
	 point2=points;

	 drawLine(point1,point2,interval);

	 point1=points;
	 */

	fillPolygon(points, pointcount, interval);

	printf("The task is done, %d pixels have been made.\n", pixelcounter);
	return (0);

}