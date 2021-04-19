// Sudoku puzzle verifier and solver

#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int **grid;
  int row;
  int col;
  int size;

} Param;

void *createParam(int **grid, int row, int col, int size) {
  Param *param = malloc(sizeof(Param));
  param->grid = grid;
  param->row = row;
  param->col = col;
  param->size = size;
  return (void *)param;
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
      return (void *)false;
    }
    counter[number] += 1;
  }

  // check if all counter equal 1
  for (int i = 1; i <= size; i++) {
    if (counter[i] != 1) {
      return (void *)false; // 0
    }
  }
  return (void *)true; // 1
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

  // check number is 0, otherwise increment counter
  for (int r = 1; r <= size; r++) {
    int number = grid[r][col];
    if (number == 0) {
      return (void *)false;
    }
    counter[number] += 1;
  }

  // check if all counter equal 1
  for (int i = 1; i <= size; i++) {
    if (counter[i] != 1) {
      return (void *)false; // 0
    }
  }
  return (void *)true; // 1
}
void *checkSquare(void *param) {
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

  // check number is 0, otherwise increment counter
  int interval = (int)sqrt(size);
  for (int r = row; r < row + interval; r++) {
    for (int c = col; c < col + interval; c++) {
      int number = grid[r][c];

      if (number == 0) {
        return (void *)false;
      }
      counter[number] += 1;
    }
  }

  // check if all counter equal 1
  for (int i = 1; i <= size; i++) {
    if (counter[i] != 1) {
      return (void *)false; // 0
    }
  }
  return (void *)true; // 1
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
  int interval = (int)sqrt(psize);
  for (int i = 1; i <= psize; i++) {
    void *rowData = createParam(grid, i, 1, psize);
    void *colData = createParam(grid, 1, i, psize);
    void *rowFlag = checkRow(rowData);
    void *colFlag = checkColum(colData);
    // printf("Row validation flag: %d\n", (bool) rowFlag);
    // printf("Col validation flag: %d\n", (bool) colFlag);

    free(rowData);
    free(colData);

    if (i % interval == 1) {

      for (int j = 1; j < psize; j += interval) {
        void *squareData = createParam(grid, i, j, psize);
        void *squareFlag = checkSquare(squareData);
        if (!squareFlag) {
          *complete = false;
          return;
        }

        free(squareData);
      }
    }

    if (!rowFlag || !colFlag) {
      *complete = false;
      return;
    }
  }
  *complete = true;
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
  printf("Complete puzzle? ");
  printf(complete ? "true\n" : "false\n");
  if (complete) {
    printf("Valid puzzle? ");
    printf(valid ? "true\n" : "false\n");
  }
  printSudokuPuzzle(sudokuSize, grid);
  deleteSudokuPuzzle(sudokuSize, grid);
  return EXIT_SUCCESS;
}