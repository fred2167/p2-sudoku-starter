// Sudoku puzzle verifier and solver

//compile: gcc sudoku.c -pthread -lm
//run: ./a.out puzzle9-valid.txt
//run: ./a.out puzzle2-valid.txt
//run: ./a.out puzzle2-invalid.txt
//run: ./a.out puzzle2-fill-valid.txt

#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int **grid;
  int row, col, size;
  bool complete, valid;

  int fill;
} Param;

void *createParam(int **grid, int row, int col, int size) {
  Param *param = malloc(sizeof(Param));
  param->grid = grid;
  param->row = row;
  param->col = col;
  param->size = size;
  param->complete = false;
  param->valid = false;

  param->fill = -1;
  return (void *)param;
}

void *fillRow(void *param) {
  Param *data = (Param *)param;
  int **grid = data->grid;
  int row = data->row;
  int size = data->size;

  int x, y;

  // counter arrays for each number, also ignore idx 0 for convenience
  int counter[size + 1];
  for (int i = 1; i <= size; i++) {
    counter[i] = 0;
  }

  for (int c = 1; c <= size; c++) {
    int number = grid[row][c];
    if (number == 0) {
      x = c;
      y = row;
      continue;
    }
    counter[number] += 1;
  }

  int zeroCount = 0;
  int fillNum = 0;
  for (int i = 1; i <= size; i++) {
    if (counter[i] == 0) {
      zeroCount++;
      fillNum = i;
    }
  }

  if (zeroCount == 1) {
    data->fill = fillNum;
    grid[y][x] = fillNum;

    printf("grid[%d][%d] replaced with %d\n", y, x, grid[y][x]);
  }
  return NULL;
}
void *fillCol(void *param) {
  Param *data = (Param *)param;
  int **grid = data->grid;
  int col = data->col;
  int size = data->size;

  int x, y;

  // counter arrays for each number, also ignore idx 0 for convenience
  int counter[size + 1];
  for (int i = 1; i <= size; i++) {
    counter[i] = 0;
  }

  for (int r = 1; r <= size; r++) {
    int number = grid[r][col];
    if (number == 0) {
      x = col;
      y = r;
      continue;
    }
    counter[number] += 1;
  }

  int zeroCount = 0;
  int fillNum = 0;
  for (int i = 1; i <= size; i++) {
    if (counter[i] == 0) {
      zeroCount++;
      fillNum = i;
    }
  }

  if (zeroCount == 1) {
    data->fill = fillNum;
    grid[y][x] = fillNum;
    printf("grid[%d][%d] replaced with %d\n", y, x, grid[y][x]);
  }
  return NULL;
}
void *fillBox(void *param) {
  Param *data = (Param *)param;
  int **grid = data->grid;
  int row = data->row;
  int col = data->col;
  int size = data->size;

  int x, y;

  // counter arrays for each number, also ignore idx 0 for convenience
  int counter[size + 1];
  memset(counter, 0, size+1);
  // for (int i = 1; i <= size; i++) {
  //   counter[i] = 0;
  // }
  

  int interval = (int)sqrt(size);
  for (int r = row; r < row + interval; r++) {
    for (int c = col; c < col + interval; c++) {
      int number = grid[r][c];
      if (number == 0) {
        x = c;
        y = r;
        continue;
      }
      counter[number] += 1;
    }
  }

  int zeroCount = 0;
  int fillNum = 0;
  for (int i = 1; i <= size; i++) {
    if (counter[i] == 0) {
      zeroCount++;
      fillNum = i;
    }
  }

  if (zeroCount == 1) {
    data->fill = fillNum;
    grid[y][x] = fillNum;
    printf("grid[%d][%d] replaced with %d\n", y, x, grid[y][x]);
  }
  return NULL;
}

void *checkRow(void *param) {
  Param *data = (Param *)param;
  int **grid = data->grid;
  int row = data->row;
  int size = data->size;

  // counter arrays for each number, also ignore idx 0 for convenience
  int counter[size + 1];
  for (int i = 1; i <= size; i++) {
    counter[i] = 0;
  }

  // check number is 0, otherwise increment counter
  for (int c = 1; c <= size; c++) {
    int number = grid[row][c];
    if (number == 0) {
      data->complete = false;
      return NULL;
    }
    counter[number] += 1;
  }
  data->complete = true;

  // check if all counter equal 1
  for (int i = 1; i <= size; i++) {
    if (counter[i] != 1) {
      data->valid = false;
      return NULL;
    }
  }
  data->valid = true;
  return NULL;
}
void *checkColum(void *param) {
  Param *data = (Param *)param;
  int **grid = data->grid;
  int col = data->col;
  int size = data->size;

  // counter arrays for each number, also ignore idx 0 for convenience
  int counter[size + 1];
  for (int i = 1; i <= size; i++) {
    counter[i] = 0;
  }

  for (int r = 1; r <= size; r++) {
    int number = grid[r][col];
    if (number == 0) {
      data->complete = false;
      return NULL;
    }
    counter[number] += 1;
  }
  data->complete = true;

  // check if all counter equal 1
  for (int i = 1; i <= size; i++) {
    if (counter[i] != 1) {
      data->valid = false;
      return NULL;
    }
  }
  data->valid = true;
  return NULL;
}
void *checkBox(void *param) {
  Param *data = (Param *)param;
  int **grid = data->grid;
  int row = data->row;
  int col = data->col;
  int size = data->size;

  // counter arrays for each number, also ignore idx 0 for convenience
  int counter[size + 1];
  for (int i = 1; i <= size; i++) {
    counter[i] = 0;
  }

  int interval = (int)sqrt(size);
  for (int r = row; r < row + interval; r++) {
    for (int c = col; c < col + interval; c++) {
      int number = grid[r][c];

      if (number == 0) {
        data->complete = false;
        return NULL;
      }

      counter[number] += 1;
    }
  }

  data->complete = true;

  // check if all counter equal 1
  for (int i = 1; i <= size; i++) {
    if (counter[i] != 1) {
      data->valid = false;
      return NULL;
    }
  }
  data->valid = true;
  return NULL;
}

void fillPuzzle(int psize, int **grid) {

  pthread_attr_t attr;
  pthread_attr_init(&attr);

  int arrSize = psize * 3;
  pthread_t tids[arrSize];
  int tidCount = 0;

  void *datas[arrSize];
  int dataCount = 0;

  bool masterFlag = false;

  int interval = (int)sqrt(psize);
  do {
    tidCount = 0;
    dataCount = 0;

    for (int i = 1; i <= psize; i++) {
      void *rowData = createParam(grid, i, 1, psize);
      void *colData = createParam(grid, 1, i, psize);

      datas[dataCount++] = rowData;
      datas[dataCount++] = colData;

      pthread_create(&tids[tidCount++], &attr, fillRow, rowData);
      pthread_create(&tids[tidCount++], &attr, fillCol, colData);

      // fillRow(rowData);
      // fillCol(colData);

      if (i % interval == 1) {

        for (int j = 1; j < psize; j += interval) {
          void *boxData = createParam(grid, i, j, psize);

          datas[dataCount++] = boxData;

          pthread_create(&tids[tidCount++], &attr, fillBox, boxData);

          // fillBox(boxData);
        }
      }
    } // end outer for

    masterFlag = false;
    for (int i = 0; i < tidCount; i++) {

      pthread_join(tids[i], NULL);

      // check if fill is changed (i.e have at least one fill successfull)
      // Otherwise, terminate while loop
      Param *paramPtr = (Param *)datas[i];
      if (paramPtr->fill != -1) {
        masterFlag = true;
      }
      free(datas[i]);
    }
  } while (masterFlag);
}

// takes puzzle size and grid[][] representing sudoku puzzle
// and two booleans to be assigned: complete and valid.
// row-0 and column-0 is ignored for convenience, so a 9x9 puzzle
// has grid[1][1] as the top-left element and grid[9]9] as bottom right
// A puzzle is complete if it can be completed with no 0s in it
// If complete, a puzzle is valid if all rows/columns/boxes have numbers from 1
// to psize For incomplete puzzles, we cannot say anything about validity
void checkPuzzle(int psize, int **grid, bool *valid, bool *complete) {
  // YOUR CODE GOES HERE and in HELPER FUNCTIONS

  pthread_attr_t attr;
  pthread_attr_init(&attr);

  int arrSize = psize * 3;
  pthread_t tids[arrSize];
  void *datas[arrSize];

  int tidCount = 0;
  int dataCount = 0;

  int interval = (int)sqrt(psize);

  for (int i = 1; i <= psize; i++) {
    void *rowData = createParam(grid, i, 1, psize);
    void *colData = createParam(grid, 1, i, psize);

    datas[dataCount++] = rowData;
    datas[dataCount++] = colData;

    pthread_create(&tids[tidCount++], &attr, checkRow, rowData);
    pthread_create(&tids[tidCount++], &attr, checkColum, colData);

    // checkRow(rowData);
    // checkColum(colData);

    if (i % interval == 1) {

      for (int j = 1; j < psize; j += interval) {
        void *boxData = createParam(grid, i, j, psize);

        datas[dataCount++] = boxData;

        pthread_create(&tids[tidCount++], &attr, checkBox, boxData);

        // checkBox(boxData);
      }
    }
  }

  bool completes[arrSize];
  bool valids[arrSize];
  for (int i = 0; i < tidCount; i++) {
    pthread_join(tids[i], NULL);

    Param *paramPtr = (Param *)datas[i];

    completes[i] = paramPtr->complete;
    valids[i] = paramPtr->valid;

    free(datas[i]);
  }

  for (int i = 0; i < tidCount; i++) {
    *complete = completes[i];
    *valid = valids[i];

    if (!*complete || !*valid) {
      return;
    }
  }
}

// takes filename and pointer to grid[][]
// returns size of Sudoku puzzle and fills grid
int readSudokuPuzzle(char *filename, int ***grid) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Could not open file %s\n", filename);
    exit(EXIT_FAILURE);
  }
  int psize;
  fscanf(fp, "%d", &psize);
  int **agrid = (int **)malloc((psize + 1) * sizeof(int *));
  for (int row = 1; row <= psize; row++) {
    agrid[row] = (int *)malloc((psize + 1) * sizeof(int));
    for (int col = 1; col <= psize; col++) {
      fscanf(fp, "%d", &agrid[row][col]);
    }
  }
  fclose(fp);
  *grid = agrid;
  return psize;
}

// takes puzzle size and grid[][]
// prints the puzzle
void printSudokuPuzzle(int psize, int **grid) {
  printf("%d\n", psize);
  for (int row = 1; row <= psize; row++) {
    for (int col = 1; col <= psize; col++) {
      printf("%d ", grid[row][col]);
    }
    printf("\n");
  }
  printf("\n");
}

// takes puzzle size and grid[][]
// frees the memory allocated
void deleteSudokuPuzzle(int psize, int **grid) {
  for (int row = 1; row <= psize; row++) {
    free(grid[row]);
  }
  free(grid);
}

// expects file name of the puzzle as argument in command line
int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: ./sudoku puzzle.txt\n");
    return EXIT_FAILURE;
  }
  // grid is a 2D array
  int **grid = NULL;
  // find grid size and fill grid
  int sudokuSize = readSudokuPuzzle(argv[1], &grid);
  bool valid = false;
  bool complete = false;
  checkPuzzle(sudokuSize, grid, &valid, &complete);
  printf("%s\n",argv[1]);
  printf("Complete puzzle? ");
  printf(complete ? "true\n" : "false\n");
  if (complete) {
    printf("Valid puzzle? ");
    printf(valid ? "true\n" : "false\n");
  } else {
    fillPuzzle(sudokuSize, grid);
  }
  printSudokuPuzzle(sudokuSize, grid);
  deleteSudokuPuzzle(sudokuSize, grid);
  return EXIT_SUCCESS;
}
