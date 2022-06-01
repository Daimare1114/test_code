#include <stdio.h>
#include<stdlib.h>

typedef enum {
	RETURN_OK = 0,
	ARGUMENT_ERROR,
	FILE_OPEN_ERROR,
	MALLOC_ERROR
} RETURN_TYPE;

/*関数プロトタイプ宣言*/
int iCountRowCol(char *pszFileName, unsigned int *puiRow, unsigned int *puiCol);
int iMallocInt(unsigned int uiSize, int **ppiArray);
void freeData(void *pvData);
int iReadData(char *pszFileName, unsigned int uiRow, unsigned int uiCol, int *piArray);
int iReadCSVFile(char *pszFileName, unsigned int *puiRow, unsigned int *puiCol, int **ppiArray);
void printData(unsigned int uiRow, unsigned int uiCol, int *piArray);

int main(int iArgCnt, char **ppchArgVec) {
	char *filename = "test.csv";//読み込むファイル名
	
	int iRet;
	unsigned int uiRow, uiCol;
	int *piArray;

	
	iRet = iReadCSVFile(filename, &uiRow, &uiCol, &piArray);

	if (iRet == RETURN_OK) {
		printData(uiRow, uiCol, piArray);
		freeData((void *)piArray);
	}
	

	return 0;
}

/*
   iCountRowCol: 行数と列数をカウントする

   char *pszFileName:   読み込むファイル名
   unsigned int *piRow: 行数
   unsigned int *piCol: 列数

   return: 正常終了 -> 0
		   異常終了 -> 0 以外
 */
int iCountRowCol(char *pszFileName, unsigned int *puiRow, unsigned int *puiCol) {
	FILE *prFile;
	int iRet;
	unsigned int uiRow, uiCol;
	char chData;
	iRet = RETURN_OK;

	/* 引数のチェック */
	if ((NULL == pszFileName) || (NULL == puiRow) || (NULL == puiCol)) {
		fprintf(stderr, "argument error\n");
		iRet = ARGUMENT_ERROR;
		goto EXIT_COUNT_ROWCOL;
	}
	else {
		*puiRow = *puiCol = 0;
	}
	if (NULL == (prFile = fopen(pszFileName, "r"))) {
		fprintf(stderr, "Error: %s cannot open\n", pszFileName);
		iRet = FILE_OPEN_ERROR;
		goto EXIT_COUNT_ROWCOL;
	}
	uiRow = uiCol = 0;

	while (EOF != fscanf(prFile, "%c", &chData)) {
		/* カンマの場合 */
		if (',' == chData) {
			uiCol++;
		}
		/* 改行コードの場合 */
		if ('\n' == chData) {
			uiCol++;
			uiRow++;
		}
	}
	uiCol /= uiRow;

	*puiRow = uiRow;
	*puiCol = uiCol;
	fclose(prFile);

EXIT_COUNT_ROWCOL:
	return iRet;
}

/*
   iMallocInt:          int 型の malloc を行う

   unsigned int uiSize: 確保するサイズ
   int **ppiArray:      確保先のアドレス

   return: 正常終了 -> 0
		   異常終了 -> 0 以外
*/
int iMallocInt(unsigned int uiSize, int **ppiArray) {
	unsigned int uiCnt;
	int iRet;
	iRet = RETURN_OK;

	/* 引数チェック */
	if (NULL == ppiArray) {
		iRet = ARGUMENT_ERROR;
		goto EXIT_MALLOC_INT;
	}
	/* malloc */
	if (NULL == (*ppiArray = (int *)malloc(sizeof(int) * uiSize))) {
		fprintf(stderr, "malloc error\n");
		iRet = MALLOC_ERROR;
		goto EXIT_MALLOC_INT;
	}
	else {
		for (uiCnt = 0; uiCnt < uiSize; uiCnt++) {
			(*ppiArray)[uiCnt] = 0;
		}
	}

EXIT_MALLOC_INT:
	return iRet;
}

/*
   freeData: 確保した領域を開放する

   void *pvData: 確保した領域の先頭アドレス
*/
void freeData(void *pvData) {
	if (NULL != pvData) {
		free(pvData);
	}

	return;
}

/*
   iReadData: データの読み込み

   char *pszFileName:  読み込むファイル名
   unsigned int uiRow: ファイルの行数
   unsigned int uiCol: ファイルの列数
   int *piArray:       データの格納先

   return: 正常終了 -> 0
		   異常終了 -> 0 以外
*/
int iReadData(char *pszFileName, unsigned int uiRow, unsigned int uiCol, int *piArray) {
	FILE *prFile;
	char chData;
	int iRet;
	unsigned int uiCntRow, uiCntCol, uiCharCnt;
	char achInputDataList[11];
	iRet = RETURN_OK;

	/* 引数のチェック */
	if (NULL == pszFileName) {
		iRet = ARGUMENT_ERROR;
		goto EXIT_READ_DATA;
	}

	/* ファイルオープン */
	if (NULL == (prFile = fopen(pszFileName, "r"))) {
		fprintf(stderr, "Error: %s cannot open\n", pszFileName);
		iRet = FILE_OPEN_ERROR;
		goto EXIT_READ_DATA;
	}

	/* ファイルの読み込み */
	for (uiCntRow = 0; uiCntRow < uiRow; uiCntRow++) {
		for (uiCntCol = 0; uiCntCol < uiCol; uiCntCol++) {
			uiCharCnt = 0;
			achInputDataList[0] = '\0';

			/* 1 文字ずつ読み込む  */
			while (EOF != fscanf(prFile, "%c", &chData)) {
				/* 「,」または「\n」が来た場合，ループを抜ける */
				if ((',' == chData) || ('\n' == chData)) {
					break;
				}

				/* 今回は，数値の部分だけ保存 */
				if (((int)'0' <= (int)chData) && ((int)chData <= (int)'9')) {
					achInputDataList[uiCharCnt++] = chData;
				}
			}
			achInputDataList[uiCharCnt] = '\0';
			piArray[uiCntRow * uiCol + uiCntCol] = atoi(achInputDataList);
		}
	}
	fclose(prFile);

EXIT_READ_DATA:
	return iRet;
}

/*
   iReadCSVFile: CSV ファイルからデータを読み込む

   char *pszFileName:   読み込むファイル名
   unsigned int *piRow: 行数
   unsigned int *piCol: 列数
   int **ppiArray:      確保先のアドレス

   return: 正常終了 -> 0
		   異常終了 -> 0 以外
*/
int iReadCSVFile(char *pszFileName, unsigned int *puiRow, unsigned int *puiCol, int **ppiArray) {
	int iRet;
	unsigned int uiSize = 0;

	iRet = iCountRowCol(pszFileName, puiRow, puiCol);
	if (RETURN_OK != iRet) {
		fprintf(stderr, "Error: iCountRowCol\n");
		goto EXIT_READ_CSV_FILE;
	}

	uiSize = (*puiRow) * (*puiCol);
	iRet = iMallocInt(uiSize, ppiArray);
	if (RETURN_OK != iRet) {
		fprintf(stderr, "Error: iMallocInt\n");
		goto EXIT_READ_CSV_FILE;
	}

	iRet = iReadData(pszFileName, *puiRow, *puiCol, *ppiArray);
	if (RETURN_OK != iRet) {
		fprintf(stderr, "Error: iReadData\n");
		goto EXIT_READ_CSV_FILE;
	}

EXIT_READ_CSV_FILE:
	return iRet;
}

void printData(unsigned int uiRow, unsigned int uiCol, int *piArray) {
	unsigned int uiCntRow, uiCntCol;

	for (uiCntRow = 0; uiCntRow < uiRow; uiCntRow++) {
		for (uiCntCol = 0; uiCntCol < uiCol; uiCntCol++) {
			printf("%d ", piArray[uiCntRow * uiCol + uiCntCol]);
		}
		printf("\n");
	}
}
