#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INF 99999 // Define a large number as infinity

typedef struct EdgeInfo {
    char from;
    char to;
    int weight;
    struct EdgeInfo* next;  // To point to the next edge in the adjacency list
} EdgeInfo;

typedef struct {
    EdgeInfo* head; // Head of the adjacency list for each vertex
} Vertex;

typedef struct {
    EdgeInfo* edges;
    int edgeCount;
} Cycle;

int findIndex(char* labels, int size, char vertex) {
    for (int i = 0; i < size; i++) {
        if (labels[i] == vertex) {
            return i;
        }
    }
    return -1;
}

void dfs(int v, bool* visited, bool* recStack, Vertex* adjList, char* labels, Cycle* cycles, int* cycleCount, char* currentCycle, int* currentIndex, int size) {
    visited[v] = true;
    recStack[v] = true;
    currentCycle[*currentIndex] = labels[v];
    (*currentIndex)++;

    if (*currentIndex >= size) {
        printf("Cycle buffer overflow detected!\n");
        return;
    }

    EdgeInfo* edge = adjList[v].head;
    while (edge != NULL) {
        int i = findIndex(labels, size, edge->to);
        if (i != -1) {
            if (!visited[i]) {
                dfs(i, visited, recStack, adjList, labels, cycles, cycleCount, currentCycle, currentIndex, size);
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
                    int edgeWeight = INF;  // Initialize edge weight as INF

                    // Find edge weight
                    EdgeInfo* tempEdge = adjList[findIndex(labels, size, from)].head;
                    while (tempEdge != NULL) {
                        if (tempEdge->to == to) {
                            edgeWeight = tempEdge->weight;
                            break;
                        }
                        tempEdge = tempEdge->next;
                    }

                    if (edgeWeight != INF) {
                        currentCycleInfo->edges[currentCycleInfo->edgeCount++] = (EdgeInfo){from, to, edgeWeight};
                    }
                }

                printf("Detected cycle: ");
                for (int j = cycleStartIndex; j < *currentIndex; j++) {
                    printf("%c ", currentCycle[j]);
                }
                printf("%c\n", currentCycle[cycleStartIndex]);

                (*cycleCount)++;
            }
        }
        edge = edge->next;
    }

    recStack[v] = false;
    (*currentIndex)--;
}

void removeEdge(Vertex* adjList, char from, char to, char* labels, int size) {
    int fromIndex = findIndex(labels, size, from);
    int toIndex = findIndex(labels, size, to);
    if (fromIndex != -1 && toIndex != -1) {
        EdgeInfo* prev = NULL;
        EdgeInfo* curr = adjList[fromIndex].head;
        while (curr != NULL) {
            if (curr->to == to) {
                if (prev == NULL) {
                    adjList[fromIndex].head = curr->next;
                } else {
                    prev->next = curr->next;
                }
                free(curr);
                break;
            }
            prev = curr;
            curr = curr->next;
        }
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

    // Dynamically allocate memory for labels, adjacency list, visited, and recStack
    char* labels = (char*)malloc(totalVertices * sizeof(char));
    Vertex* adjList = (Vertex*)malloc(totalVertices * sizeof(Vertex));
    bool* visited = (bool*)malloc(totalVertices * sizeof(bool));
    bool* recStack = (bool*)malloc(totalVertices * sizeof(bool));
    char* currentCycle = (char*)malloc(totalVertices * sizeof(char));

    // Check if memory allocation was successful
    if (!labels || !adjList || !visited || !recStack || !currentCycle) {
        printf("Memory allocation failed!\n");
        return -1;
    }

    // Initialize adjacency list
    for (int i = 0; i < totalVertices; i++) {
        adjList[i].head = NULL;
    }

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
            // Add edge to adjacency list
            EdgeInfo* newEdge = (EdgeInfo*)malloc(sizeof(EdgeInfo));
            newEdge->from = src;
            newEdge->to = dest;
            newEdge->weight = weight;
            newEdge->next = adjList[srcIndex].head;
            adjList[srcIndex].head = newEdge;
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
            dfs(i, visited, recStack, adjList, labels, cycles, &cycleCount, currentCycle, &currentIndex, totalVertices);
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
            removeEdge(adjList, cycles[i].edges[minIndex].from, cycles[i].edges[minIndex].to, labels, totalVertices);
        }
    }

    // Output updated graph
    printf("\nUpdated graph after removing edges:\n");
    for (int i = 0; i < totalVertices; i++) {
        printf("%c -> ", labels[i]);
        EdgeInfo* edge = adjList[i].head;
        while (edge != NULL) {
            printf("%c(%d) -> ", edge->to, edge->weight);
            edge = edge->next;
        }
        printf("NULL\n");
    }

    // Free dynamically allocated memory
    free(labels);
    free(visited);
    free(recStack);
    free(currentCycle);
    for (int i = 0; i < totalVertices; i++) {
        EdgeInfo* edge = adjList[i].head;
        while (edge != NULL) {
            EdgeInfo* temp = edge;
            edge = edge->next;
            free(temp);
        }
    }
    free(adjList);
    free(cycles);

    return 0;
}