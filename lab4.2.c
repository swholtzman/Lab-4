// A01308106


#include <stdio.h>
#include <stdlib.h>

#define MAX_POINTS 1000 // Maximum number of points
#define MAX_SIZE 100    // Maximum size for x and y dimensions of the array

typedef struct
{
    int x;
    int y;
} Point;

int numPoints = 0;
int rangeX = 0;
int rangeY = 0;

Point pointsArray[MAX_POINTS];

void readFile(const char *inputFileName)
{
    FILE *inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL)
    {
        perror("Error");
        exit(EXIT_FAILURE);
    }

    int x, y;
    while (fscanf(inputFile, "%d,%d", &x, &y) == 2)
    {
        pointsArray[numPoints].x = x;
        pointsArray[numPoints].y = y;
        if (++numPoints >= MAX_POINTS)
        {
            fprintf(stderr, "Too many points\n");
            exit(EXIT_FAILURE);
        }
        // Update rangeX and rangeY with the last point read
        if (x >= rangeX)
            rangeX = x + 1;
        if (y >= rangeY)
            rangeY = y + 1;
    }

    fclose(inputFile);
}

void createArray(char arr[][MAX_SIZE])
{
    // Initialize the array with spaces
    for (int y = 0; y < rangeY; y++)
    {
        for (int x = 0; x < rangeX; x++)
        {
            arr[y][x] = ' ';
        }
    }

    // Add asterisks at specified coordinates
    for (int i = 0; i < numPoints; i++)
    {
        Point currentPoint = pointsArray[i];
        Point nextPoint = pointsArray[(i + 1) % numPoints]; // Wrap around for the last point

        int x0 = currentPoint.x;
        int y0 = currentPoint.y;
        int x1 = nextPoint.x;
        int y1 = nextPoint.y;

        // Bresenham's line algorithm
        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1;
        int sy = y0 < y1 ? 1 : -1;
        int err = (dx > dy ? dx : -dy) / 2, e2;

        while (1)
        {
            arr[y0][x0] = '*';

            if (x0 == x1 && y0 == y1)
                break;

            e2 = err;
            if (e2 > -dx)
            {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dy)
            {
                err += dx;
                y0 += sy;
            }
        }
    }
}


void printArray(FILE *outputFile, char arr[][MAX_SIZE])
{
    for (int y = rangeY - 1; y >= 0; y--)
    {
        for (int x = 0; x < rangeX; x++)
        {
            fprintf(outputFile, "%c", (arr[y][x] == '*') ? '*' : ' ');
        }
        fprintf(outputFile, "\n");
    }
}



int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s input_file output_file\n", argv[0]);
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "r");
    if (inputFile == NULL)
    {
        perror("Error");
        return 1;
    }

    FILE *outputFile = fopen(argv[2], "w");
    if (outputFile == NULL)
    {
        perror("Error");
        return 1;
    }

    readFile(argv[1]);
    char asterixArray[MAX_SIZE][MAX_SIZE] = {0};
    createArray(asterixArray);
    printArray(outputFile, asterixArray);

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
