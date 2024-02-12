// A01308106

#include <stdio.h>
#include <stdlib.h>

#define MAX_POINTS 1000
#define MAX_SIZE 100

typedef struct
{
    int x;
    int y;
} Point;

int numPoints = 0;
int maxX = 0;
int maxY = 0;

Point pointsArray[MAX_POINTS];

/**
 * Function to read points from the input file
 * @param inputFileName The name of the input file containing points
 */
void readPoints(const char *inputFileName)
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
        // Update maxX and maxY with the last point read
        if (x >= maxX)
            maxX = x + 1;
        if (y >= maxY)
            maxY = y + 1;
    }

    fclose(inputFile);
}

/**
 * Function to calculate the maximum bounds of the points
 */
void calculateMaxBounds()
{
    maxX = pointsArray[0].x;
    maxY = pointsArray[0].y;
    for (int i = 1; i < numPoints; i++)
    {
        if (maxX < pointsArray[i].x)
        {
            maxX = pointsArray[i].x;
        }
        if (maxY < pointsArray[i].y)
        {
            maxY = pointsArray[i].y;
        }
    }
    maxX++; // Increment by 1 to account for zero-based indexing
    maxY++; // Increment by 1 to account for zero-based indexing
}

/**
 * Function to create array and draw outline based on points
 * @param matrix The character matrix representing the drawing
 */
void createArrayAndDrawOutline(char *matrix)
{
    // Initialize the array with spaces
    for (int y = 0; y < maxY; y++)
    {
        for (int x = 0; x < maxX; x++)
        {
            matrix[y * maxX + x] = ' ';
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
            matrix[y0 * maxX + x0] = '*';

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

/**
 * Function to find centroid of the points
 * @return The centroid point
 */
Point findCentroid()
{
    int sumX = 0, sumY = 0;
    for (int i = 0; i < numPoints; i++)
    {
        sumX += pointsArray[i].x;
        sumY += pointsArray[i].y;
    }
    Point centroid;
    centroid.x = sumX / numPoints;
    centroid.y = sumY / numPoints;
    return centroid;
}

/**
 * Function to fill the shape using flood fill algorithm
 * @param arr The character matrix representing the shape
 * @param x The x-coordinate to start filling from
 * @param y The y-coordinate to start filling from
 */
void fillShape(char *arr, int x, int y)
{
    // Check if the current spot is empty and within bounds
    if (x >= 0 && x < maxX && y >= 0 && y < maxY && arr[y * maxX + x] == ' ')
    {
        // Fill the current spot with an asterisk
        arr[y * maxX + x] = '*';

        // Recursively fill neighboring spots
        fillShape(arr, x + 1, y);
        fillShape(arr, x - 1, y);
        fillShape(arr, x, y + 1);
        fillShape(arr, x, y - 1);
    }
}

/**
 * Function to print the array
 * @param outputFile The output file to print to
 * @param arr The character matrix representing the shape
 */
void printArray(FILE *outputFile, char *arr)
{
    int startRow = maxY - 1;
    int endRow = 0;
    int startCol = maxX - 1;
    int endCol = 0;

    // Find the boundaries of the shape
    for (int y = 0; y < maxY; y++)
    {
        for (int x = 0; x < maxX; x++)
        {
            if (arr[y * maxX + x] == '*')
            {
                startRow = y < startRow ? y : startRow;
                endRow = y > endRow ? y : endRow;
                startCol = x < startCol ? x : startCol;
                endCol = x > endCol ? x : endCol;
            }
        }
    }

    // Print the array within the boundaries of the shape
    for (int y = endRow; y >= startRow; y--)
    {
        for (int x = startCol; x <= endCol; x++)
        {
            if (arr[y * maxX + x] == '*')
            {
                fprintf(outputFile, "*");
            }
            else 
            {
                fprintf(outputFile, " ");
            }
        }
        // Check if this is not the last row
        if (y != startRow)
            fprintf(outputFile, "\n");
    }
}

/**
 * Main function
 * @param argc Number of command-line arguments
 * @param argv Array of command-line arguments
 * @return Exit status
 */
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

    readPoints(argv[1]);
    calculateMaxBounds();

    char *matrix = malloc(maxX * maxY * sizeof(char));
    if (matrix == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    createArrayAndDrawOutline(matrix);

    // Find centroid and fill the shape starting from there
    Point centroid = findCentroid();
    fillShape(matrix, centroid.x, centroid.y);

    // for (int y = maxY - 1; y >= 0; y--)
    // {
    //     int pastStar = 0;
    //     for (int x = 0; x < maxX; x++)
    //     {
    //         printf("%c ", matrix[y * maxX + x]);
    //         if (matrix[y * maxX + x] == ' ' && pastStar == 0)
    //         {
    //             printf(" ");
    //         }
    //         if (matrix[y * maxX + x] == ' ')
    //         {
    //             printf("");
    //             pastStar = 1;
    //         }
    //     }
    //     pastStar = 0;
    //     if (y != 0)
    //     {
    //         printf("\n");
    //     }
    // }

    printArray(outputFile, matrix);

    fclose(inputFile);
    fclose(outputFile);

    free(matrix);

    return 0;
}
