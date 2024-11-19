#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 52  // Maximum vertices (26 lowercase + 26 uppercase)

// Node structure for the adjacency list
typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

// Function to create a new node
Node* createNode(int vertex) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = vertex;
    newNode->next = NULL;
    return newNode;
}

// Function to add an edge to the adjacency list
void addEdge(Node** adjList, int src, int dest) {
    Node* newNode = createNode(dest);
    newNode->next = adjList[src];
    adjList[src] = newNode;
}

// Function to find the index of a vertex in labels array
int findIndex(char* labels, int size, char vertex) {
    for (int i = 0; i < size; i++) {
        if (labels[i] == vertex) {
            return i;
        }
    }
    return -1;
}

// Cycle detection and printing function
int detectCycle(Node** adjList, int vertex, int* visited, int* recStack, char* labels, int* path, int pathIdx) {
    visited[vertex] = 1;
    recStack[vertex] = 1;
    path[pathIdx++] = vertex;

    Node* temp = adjList[vertex];
    while (temp) {
        int adjVertex = temp->vertex;

        if (!visited[adjVertex]) {  // If adjVertex is not visited
            if (detectCycle(adjList, adjVertex, visited, recStack, labels, path, pathIdx)) {
                return 1;
            }
        } else if (recStack[adjVertex]) {  // Cycle detected
            printf("Cycle detected: ");
            for (int i = 0; i < pathIdx; i++) {
                if (path[i] == adjVertex) {  // Start of the cycle
                    for (int j = i; j < pathIdx; j++) {
                        printf("%c -> ", labels[path[j]]);
                    }
                    printf("%c\n", labels[adjVertex]);
                    return 1;
                }
            }
        }

        temp = temp->next;
    }

    recStack[vertex] = 0;
    return 0;
}

int main() {
    int numProcesses, numResources, edges;

    printf("Enter the number of processes, resources, and edges:\n");
    scanf("%d %d %d", &numProcesses, &numResources, &edges);

    int totalVertices = numProcesses + numResources;
    char labels[MAX_VERTICES];

    // Input process and resource labels
    printf("Enter process labels (lowercase a-z):\n");
    for (int i = 0; i < numProcesses; i++) {
        scanf(" %c", &labels[i]);
    }
    printf("Enter resource labels (uppercase A-Z):\n");
    for (int i = 0; i < numResources; i++) {
        scanf(" %c", &labels[numProcesses + i]);
    }

    // Initialize adjacency list
    Node** adjList = (Node**)malloc(totalVertices * sizeof(Node*));
    for (int i = 0; i < totalVertices; i++) {
        adjList[i] = NULL;
    }

    // Input edges
    for (int i = 0; i < edges; i++) {
        char src, dest;
        printf("Enter edge (source and destination): ");
        scanf(" %c %c", &src, &dest);

        int srcIndex = findIndex(labels, totalVertices, src);
        int destIndex = findIndex(labels, totalVertices, dest);

        if (srcIndex != -1 && destIndex != -1) {
            addEdge(adjList, srcIndex, destIndex);
        }
    }

    // Arrays for visited, recursion stack, and path tracking
    int* visited = (int*)malloc(totalVertices * sizeof(int));
    int* recStack = (int*)malloc(totalVertices * sizeof(int));
    int* path = (int*)malloc(totalVertices * sizeof(int));

    for (int i = 0; i < totalVertices; i++) {
        visited[i] = 0;
        recStack[i] = 0;
    }

    // Check each vertex for cycle detection
    int cycleDetected = 0;
    for (int i = 0; i < totalVertices; i++) {
        if (!visited[i]) {
            if (detectCycle(adjList, i, visited, recStack, labels, path, 0)) {
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
        Node* temp;
        while (adjList[i]) {
            temp = adjList[i];
            adjList[i] = adjList[i]->next;
            free(temp);
        }
    }
    free(adjList);
    free(visited);
    free(recStack);
    free(path);

    return 0;
}
