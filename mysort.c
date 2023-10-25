#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

#define USE "./mysort <input file> <output file> <number of threads>"
#define BUFFER_SIZE 100

struct stat st;
int OFFSET;
int NUMBERS_PER_THREAD;
int NUM_THREADS;
char **data;

//Function to merge
void merging(char *arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    char **L = calloc(n1, sizeof(char *));
    char **R = calloc(n2, sizeof(char *));

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];

    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2)
    {
        if (strcmp(L[i], R[j]) <= 0)
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// Merge Sort Logic
void mergesort(char *arr[], int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        mergesort(arr, l, m);
        mergesort(arr, m + 1, r);
        merging(arr, l, m, r);
    }
}

//Merging section of array
void secarraymerging(char *arr[], int number, int aggregation, int length, int NUMBERS_PER_THREAD)
{
    for (int i = 0; i < number; i = i + 2)
    {
        int left = i * (NUMBERS_PER_THREAD * aggregation);
        int right = ((i + 2) * NUMBERS_PER_THREAD * aggregation) - 1;
        int middle = left + (NUMBERS_PER_THREAD * aggregation) - 1;
        if (right >= length)
        {
            right = length - 1;
        }
        merging(arr, left, middle, right);
    }
    if (number / 2 >= 1)
    {
        secarraymerging(arr, number / 2, aggregation * 2, length, NUMBERS_PER_THREAD);
    }
}

//Threading
void *threadmerging(void *arg)
{
    int thread_id = (long)arg;
    printf("Offset: %d, NT: %d, NPT: %d, id:%d\n", OFFSET, NUM_THREADS, NUMBERS_PER_THREAD, thread_id);

    int left = thread_id * NUMBERS_PER_THREAD;
    int right = (thread_id + 1) * NUMBERS_PER_THREAD - 1;
    if (thread_id == NUM_THREADS - 1)
    {
        right += OFFSET;
    }

    if (left < right)
    {
        mergesort(data, left, right);
    }

    return NULL;
}

//Main Logic
void mysort(char *inputFile, char *outputFile, int numThreads)
{
    char *buffer;
    FILE *fin;
    FILE *fout;
    int i = 0;

    stat(inputFile, &st);
    long int size_of_file = st.st_size / 100;
    printf("size %li\n", size_of_file);
    printf("n_threads:%d\n", numThreads);

    data = calloc(size_of_file, sizeof(char *));
    fin = fopen(inputFile, "r");

    if (fin == NULL)
    {
        fprintf(stderr, "fopen(%s) failed", inputFile);
        return;
    }

    buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);

    
    while (fread(&buffer[0], sizeof(char), BUFFER_SIZE, fin) == BUFFER_SIZE)
    {
        data[i] = strdup(buffer);
        i++;
    }
    

    free(buffer);
    fclose(fin);

    OFFSET = size_of_file % numThreads;
    NUMBERS_PER_THREAD = size_of_file / numThreads;
    NUM_THREADS = numThreads;

    pthread_t threads[numThreads];

    for (long i = 0; i < numThreads; i++)
    {
        int rc = pthread_create(&threads[i], NULL, threadmerging, (void *)i);
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    secarraymerging(data, numThreads, 1, size_of_file, NUMBERS_PER_THREAD);

    printf("\nSorted array is \n");

    fout = fopen(outputFile, "w");

    if (fout == NULL)
    {
        fprintf(stderr, "fopen(%s) failed", outputFile);
        return;
    }

    for (i = 0; i < size_of_file; i++)
    {
        fwrite(data[i], sizeof(char), BUFFER_SIZE, fout);
    }

    fclose(fout);
}



void writeToLogFile(const char*filename,const char *message, ...) {
    char newFilename[100];  // Adjust the size based on your needs

    // Find the position of the dot in the original filename
    const char *dotPosition = strrchr(filename, '.');

    if (dotPosition != NULL) {
        // Copy the characters before the dot to the new filename
        strncpy(newFilename, filename, dotPosition - filename);

        // Append the new extension
        strcpy(newFilename + (dotPosition - filename), ".log");
        
    } else {
        // The original filename doesn't have an extension
        printf("The original filename does not have an extension.\n");
    }
    FILE *logFile = fopen(newFilename, "a");  // Open or create the log file in append mode

    if (logFile == NULL) {
        printf("Failed to open the log file.\n");
        return;
    }

    va_list args;
    va_start(args, message);
    vfprintf(logFile, message, args);  // Write the message (and optional value) to the log file
    va_end(args);

    fprintf(logFile, "\n");  // Add a newline character for separation
    fclose(logFile);  // Close the log file
}

int main(int argc, char **argv)
{
    char *inputFile;
    char *outputFile;
    int numThreads;
    struct timeval start, end;
    double executionTime;

    if (argc != 4)
    {
        fprintf(stderr, USE);
        return 1;
    }

    inputFile = argv[1];
    outputFile = argv[2];
    numThreads = atoi(argv[3]);

    gettimeofday(&start, NULL);
    mysort(inputFile, outputFile, numThreads);
    gettimeofday(&end, NULL);

    executionTime = ((double)end.tv_sec - (double)start.tv_sec) + ((double)end.tv_usec - (double)start.tv_usec) / 1000000.0;

    printf("input file: %s\n", inputFile);
    printf("output file: %s\n", outputFile);
    printf("number of threads: %d\n", numThreads);
    printf("execution time: %lf\n", executionTime);
    stat(inputFile, &st);
    long double size_of_file = st.st_size / (1024.0 * 1024.0 * 1024 ) * 10;
    

    stat(outputFile, &st);
    long double outputfilesize = st.st_size / (1024.0 * 1024.0 * 1024 ) * 10;
    

    writeToLogFile(inputFile,"In Memory Sort \n Total Time: %lf Seconds \n Total Data Read: %.15Lf GB \n Total Data Write: %.15Lf GB \n \n ValSort",executionTime, size_of_file, outputfilesize);
    return 0;
}
