#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INF 99999 // Define a large number as infinity

typedef struct {
    char from;
    char to;
    int weight;
} EdgeInfo;

typedef struct {
    EdgeInfo* edges;
    int edgeCount;
} Cycle;

void initAdjMatrix(int** matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = (i == j) ? 0 : INF; // 0 for self-loop, INF for no edge
        }
    }
}

int findIndex(char* labels, int size, char vertex) {
    for (int i = 0; i < size; i++) {
        if (labels[i] == vertex) {
            return i;
        }
    }
    return -1;
}

void dfs(int v, bool* visited, bool* recStack, int** matrix, char* labels, Cycle* cycles, int* cycleCount, char* currentCycle, int* currentIndex, int size) {
    visited[v] = true;
    recStack[v] = true;
    currentCycle[*currentIndex] = labels[v];
    (*currentIndex)++;

    if (*currentIndex >= size) {
        printf("Cycle buffer overflow detected!\n");
        return;
    }

    for (int i = 0; i < size; i++) {
        if (matrix[v][i] != INF && matrix[v][i] != 0) { // There's an edge
            if (!visited[i]) {
                dfs(i, visited, recStack, matrix, labels, cycles, cycleCount, currentCycle, currentIndex, size);
            } else if (recStack[i]) {
                // Cycle detected
                Cycle* currentCycleInfo = &cycles[*cycleCount];
                int cycleStartIndex = 0;
                while (currentCycle[cycleStartIndex] != labels[i]) {
                    cycleStartIndex++;
                }

                for (int j = cycleStartIndex; j < *currentIndex; j++) {
                    char from = currentCycle[j];
                    char to = (j == *currentIndex - 1) ? currentCycle[cycleStartIndex] : currentCycle[j + 1];
                    int edgeWeight = matrix[findIndex(labels, size, from)][findIndex(labels, size, to)];

                    currentCycleInfo->edges[currentCycleInfo->edgeCount++] = (EdgeInfo){from, to, edgeWeight};
                }

                printf("Detected cycle: ");
                for (int j = cycleStartIndex; j < *currentIndex; j++) {
                    printf("%c ", currentCycle[j]);
                }
                printf("%c\n", currentCycle[cycleStartIndex]);

                (*cycleCount)++;
            }
        }
    }

    recStack[v] = false;
    (*currentIndex)--;
}

void removeEdge(int** matrix, char from, char to, char* labels, int size) {
    int fromIndex = findIndex(labels, size, from);
    int toIndex = findIndex(labels, size, to);
    if (fromIndex != -1 && toIndex != -1) {
        matrix[fromIndex][toIndex] = INF; // Remove the edge by setting it to infinity
    }
}

int main() {
    int numProcesses, numResources, edges;

    // User input for the number of processes, resources, and edges
    printf("Enter number of processes: ");
    scanf("%d", &numProcesses);
    printf("Enter number of resources: ");
    scanf("%d", &numResources);
    printf("Enter number of edges: ");
    scanf("%d", &edges);

    int totalVertices = numProcesses + numResources;

    // Dynamically allocate memory for labels, adjacency matrix, visited, and recStack
    char* labels = (char*)malloc(totalVertices * sizeof(char));
    int** adjMatrix = (int**)malloc(totalVertices * sizeof(int*));
    bool* visited = (bool*)malloc(totalVertices * sizeof(bool));
    bool* recStack = (bool*)malloc(totalVertices * sizeof(bool));
    char* currentCycle = (char*)malloc(totalVertices * sizeof(char));

    // Check if memory allocation was successful
    if (!labels || !adjMatrix || !visited || !recStack || !currentCycle) {
        printf("Memory allocation failed!\n");
        return -1;
    }

    // Initialize adjacency matrix
    for (int i = 0; i < totalVertices; i++) {
        adjMatrix[i] = (int*)malloc(totalVertices * sizeof(int));
    }

    initAdjMatrix(adjMatrix, totalVertices);

    // Input labels
    printf("Enter process labels (%d characters): ", numProcesses);
    for (int i = 0; i < numProcesses; i++) {
        scanf(" %c", &labels[i]);
    }

    printf("Enter resource labels (%d characters): ", numResources);
    for (int i = 0; i < numResources; i++) {
        scanf(" %c", &labels[numProcesses + i]);
    }

    // Input edges
    printf("Enter %d edges (source destination weight):\n", edges);
    for (int i = 0; i < edges; i++) {
        char src, dest;
        int weight;
        scanf(" %c %c %d", &src, &dest, &weight);

        int srcIndex = findIndex(labels, totalVertices, src);
        int destIndex = findIndex(labels, totalVertices, dest);

        if (srcIndex != -1 && destIndex != -1) {
            adjMatrix[srcIndex][destIndex] = weight; // Add edge
        } else {
            printf("Invalid edge: %c -> %c\n", src, dest);
        }
    }

    // Initialize visited and recStack arrays
    for (int i = 0; i < totalVertices; i++) {
        visited[i] = false;
        recStack[i] = false;
    }

    // Initialize cycles
    Cycle* cycles = (Cycle*)malloc(totalVertices * sizeof(Cycle));
    int cycleCount = 0;
    for (int i = 0; i < totalVertices; i++) {
        cycles[i].edges = (EdgeInfo*)malloc(edges * sizeof(EdgeInfo)); // Max edges per cycle
        cycles[i].edgeCount = 0;
    }

    int currentIndex = 0;

    // Perform DFS to detect cycles
    for (int i = 0; i < totalVertices; i++) {
        if (!visited[i]) {
            dfs(i, visited, recStack, adjMatrix, labels, cycles, &cycleCount, currentCycle, &currentIndex, totalVertices);
        }
    }

    // Remove the minimum weight edge from each cycle
    for (int i = 0; i < cycleCount; i++) {
        if (cycles[i].edgeCount > 0) {
            int minIndex = 0;
            for (int j = 1; j < cycles[i].edgeCount; j++) {
                if (cycles[i].edges[j].weight < cycles[i].edges[minIndex].weight) {
                    minIndex = j;
                }
            }

            printf("Removing edge: %c -> %c (Weight: %d)\n", cycles[i].edges[minIndex].from, cycles[i].edges[minIndex].to, cycles[i].edges[minIndex].weight);
            removeEdge(adjMatrix, cycles[i].edges[minIndex].from, cycles[i].edges[minIndex].to, labels, totalVertices);
        }
    }

    // Output updated graph
    printf("\nUpdated graph after removing edges:\n");
    for (int i = 0; i < totalVertices; i++) {
        printf("%c -> ", labels[i]);
        for (int j = 0; j < totalVertices; j++) {
            if (adjMatrix[i][j] != INF && adjMatrix[i][j] != 0) {
                printf("%c(%d) -> ", labels[j], adjMatrix[i][j]);
            }
        }
        printf("NULL\n");
    }

    // Free dynamically allocated memory
    free(labels);
    free(visited);
    free(recStack);
    free(currentCycle);
    for (int i = 0; i < totalVertices; i++) {
        free(adjMatrix[i]);
    }
    free(adjMatrix);
    for (int i = 0; i < totalVertices; i++) {
        free(cycles[i].edges);
    }
    free(cycles);

    return 0;
}
