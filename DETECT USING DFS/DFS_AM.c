#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 52  // Maximum vertices (26 lowercase + 26 uppercase)

// Function to find the index of a vertex in labels array
int findIndex(char* labels, int size, char vertex) {
    for (int i = 0; i < size; i++) {
        if (labels[i] == vertex) {
            return i;
        }
    }
    return -1;
}

// Cycle detection function
int detectCycle(int** adjMatrix, int vertex, int* visited, int* recStack, char* labels, int* path, int pathIdx, int totalVertices) {
    visited[vertex] = 1;       // Mark the vertex as visited
    recStack[vertex] = 1;      // Add the vertex to the recursion stack
    path[pathIdx++] = vertex;  // Add the vertex to the path

    for (int adjVertex = 0; adjVertex < totalVertices; adjVertex++) {
        if (adjMatrix[vertex][adjVertex]) {  // Check if there is an edge
            if (!visited[adjVertex]) {  // If adjVertex is not visited
                if (detectCycle(adjMatrix, adjVertex, visited, recStack, labels, path, pathIdx, totalVertices)) {
                    return 1;  // Cycle detected
                }
            } else if (recStack[adjVertex]) {  // Cycle detected
                // We found a cycle, print it
                printf("Cycle detected: ");
                int cycleStartIndex = -1;

                // Find the starting point of the cycle in the path
                for (int i = 0; i < pathIdx; i++) {
                    if (path[i] == adjVertex) {
                        cycleStartIndex = i;  // Start from the first occurrence of adjVertex
                        break;
                    }
                }
                
                // Print the cycle path
                for (int j = cycleStartIndex; j < pathIdx; j++) {
                    printf("%c -> ", labels[path[j]]);
                }
                printf("%c\n", labels[adjVertex]); // Complete the cycle output
                return 1;  // Cycle detected
            }
        }
    }

    recStack[vertex] = 0;  // Remove the vertex from the recursion stack
    return 0;              // No cycle detected
}

int main() {
    int numProcesses, numResources, edges;

    printf("Enter the number of processes, resources, and edges:\n");
    if (scanf("%d %d %d", &numProcesses, &numResources, &edges) != 3) {
        printf("Error reading inputs.\n");
        return 1;
    }

    int totalVertices = numProcesses + numResources;
    char* labels = (char*)malloc(MAX_VERTICES * sizeof(char));  // Dynamic allocation for labels array

    // Input process and resource labels
    printf("Enter process labels (lowercase a-z):\n");
    for (int i = 0; i < numProcesses; i++) {
        scanf(" %c", &labels[i]);  // Space before %c to clear buffer
    }
    printf("Enter resource labels (uppercase A-Z):\n");
    for (int i = 0; i < numResources; i++) {
        scanf(" %c", &labels[numProcesses + i]);
    }

    // Initialize adjacency matrix
    int** adjMatrix = (int**)malloc(totalVertices * sizeof(int*));
    for (int i = 0; i < totalVertices; i++) {
        adjMatrix[i] = (int*)calloc(totalVertices, sizeof(int));
    }

    // Input edges
    for (int i = 0; i < edges; i++) {
        char src, dest;
        printf("Enter edge (source and destination): ");
        scanf(" %c %c", &src, &dest);

        int srcIndex = findIndex(labels, totalVertices, src);
        int destIndex = findIndex(labels, totalVertices, dest);

        if (srcIndex != -1 && destIndex != -1) {
            adjMatrix[srcIndex][destIndex] = 1;  // Add directed edge
        }
    }

    // Arrays for visited, recursion stack, and path tracking
    int* visited = (int*)calloc(totalVertices, sizeof(int));
    int* recStack = (int*)calloc(totalVertices, sizeof(int));
    int* path = (int*)malloc(totalVertices * sizeof(int));

    // Check each vertex for cycle detection
    int cycleDetected = 0;
    for (int i = 0; i < totalVertices; i++) {
        if (!visited[i]) {
            if (detectCycle(adjMatrix, i, visited, recStack, labels, path, 0, totalVertices)) {
                cycleDetected = 1;
                break;
            }
        }
    }

    if (!cycleDetected) {
        printf("No deadlock detected.\n");
    } else {
        printf("Deadlock detected (cycle exists).\n");
    }

    // Free allocated memory
    for (int i = 0; i < totalVertices; i++) {
        free(adjMatrix[i]);
    }
    free(adjMatrix);
    free(visited);
    free(recStack);
    free(path);
    free(labels);

    return 0;
}
