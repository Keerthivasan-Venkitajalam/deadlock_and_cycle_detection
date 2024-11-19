#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 52  // For lowercase a-z (processes) and uppercase A-Z (resources)

// Function to initialize and return an adjacency matrix for the graph
int** createAdjMatrix(int size) {
    int** adjMatrix = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++) {
        adjMatrix[i] = (int*)malloc(size * sizeof(int));
        for (int j = 0; j < size; j++) {
            adjMatrix[i][j] = 0;  // Initialize all entries to 0
        }
    }
    return adjMatrix;
}

// Function to find the index of a vertex in the labels array
int findIndex(char* labels, int size, char vertex) {
    for (int i = 0; i < size; i++) {
        if (labels[i] == vertex) {
            return i;
        }
    }
    return -1;
}

// Function to initialize and populate the adjacency matrix for the graph
int** createGraph(int numProcesses, int numResources, int edges, char* labels) {
    int totalVertices = numProcesses + numResources;
    int** adjMatrix = createAdjMatrix(totalVertices);

    // Input edges
    for (int i = 0; i < edges; i++) {
        char src, dest;
        printf("Enter edge (src dest): ");
        scanf(" %c %c", &src, &dest);

        int srcIndex = findIndex(labels, totalVertices, src);
        int destIndex = findIndex(labels, totalVertices, dest);

        if (srcIndex != -1 && destIndex != -1) {
            adjMatrix[srcIndex][destIndex] = 1;  // Add edge
        }
    }

    return adjMatrix;
}

// Kahn's Algorithm for topological sorting and cycle detection
int kahn_topological_sort(int** adjMatrix, char* labels, int totalVertices) {
    int in_degree[MAX_VERTICES] = {0};
    int queue[MAX_VERTICES];
    int front = 0, rear = 0;
    int topological_order[MAX_VERTICES];
    int top_order_index = 0;

    // Calculate in-degrees for each vertex
    for (int i = 0; i < totalVertices; i++) {
        for (int j = 0; j < totalVertices; j++) {
            if (adjMatrix[i][j] == 1) {
                in_degree[j]++;
            }
        }
    }

    // Enqueue vertices with zero in-degree
    for (int i = 0; i < totalVertices; i++) {
        if (in_degree[i] == 0) {
            queue[rear++] = i;
        }
    }

    // Process each vertex in the queue
    while (front < rear) {
        int current = queue[front++];
        topological_order[top_order_index++] = current;

        for (int i = 0; i < totalVertices; i++) {
            if (adjMatrix[current][i] == 1) {
                in_degree[i]--;
                if (in_degree[i] == 0) {
                    queue[rear++] = i;
                }
            }
        }
    }

    // If top_order_index is not equal to totalVertices, there is a cycle
    if (top_order_index != totalVertices) {
        printf("Deadlock detected (cycle exists).\n");
        return -1;
    }

    printf("Topological Order: ");
    for (int i = 0; i < top_order_index; i++) {
        printf("%c ", labels[topological_order[i]]);
    }
    printf("\n");

    return 0;
}

int main() {
    int numProcesses, numResources, edges;

    // Input for the number of processes, resources, and edges
    printf("Enter number of processes, resources, and edges:\n");
    scanf("%d %d %d", &numProcesses, &numResources, &edges);

    int totalVertices = numProcesses + numResources;

    // Initialize labels for vertices
    char labels[MAX_VERTICES];
    printf("Enter process labels (lowercase a-z):\n");
    for (int i = 0; i < numProcesses; i++) {
        scanf(" %c", &labels[i]);
    }
    printf("Enter resource labels (uppercase A-Z):\n");
    for (int i = 0; i < numResources; i++) {
        scanf(" %c", &labels[numProcesses + i]);
    }

    // Create the adjacency matrix
    printf("Enter edges as pairs of characters (src dest):\n");
    int** graph = createGraph(numProcesses, numResources, edges, labels);

    // Perform Kahn's algorithm on the created adjacency matrix
    if (kahn_topological_sort(graph, labels, totalVertices) == 0) {
        printf("No deadlock detected.\n");
    } else {
        printf("System is in a deadlock state.\n");
    }

    // Free allocated memory
    for (int i = 0; i < totalVertices; i++) {
        free(graph[i]);
    }
    free(graph);

    return 0;
}
