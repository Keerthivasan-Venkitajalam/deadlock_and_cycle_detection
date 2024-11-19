#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variables
int **adjMatrix;            // Adjacency matrix
int *disc;                  // Discovery time of nodes
int *low;                   // Low-link values
int *inStack;               // Stack membership check
int *stack;                 // Stack for Tarjan's algorithm
int time = 0;               // Timer for discovery times
int stackTop = -1;          // Stack pointer
int nodeCount = 0;          // Total unique nodes in the graph

// Map names to indices
int nodeIndex = 0;
char **nodeNames;          // Store node names dynamically

// Flag to check if a real deadlock exists
int hasDeadlock = 0;

// Function to map names to unique indices
int nameToIndex(const char* name) {
    for (int i = 0; i < nodeIndex; i++) {
        if (strcmp(nodeNames[i], name) == 0)
            return i;
    }
    nodeNames[nodeIndex] = (char*)malloc(strlen(name) + 1); // Dynamically allocate memory for node name
    strcpy(nodeNames[nodeIndex], name);
    return nodeIndex++;
}

// Push to stack
void push(int node) {
    stack[++stackTop] = node;
    inStack[node] = 1;
}

// Pop from stack
int pop() {
    int node = stack[stackTop--];
    inStack[node] = 0;
    return node;
}

// Tarjan's DFS to find SCCs
void tarjanDFS(int u) {
    disc[u] = low[u] = ++time;
    push(u);

    for (int v = 0; v < nodeCount; v++) {
        if (adjMatrix[u][v]) { // Check if there is an edge u -> v
            if (disc[v] == -1) {
                tarjanDFS(v);
                low[u] = (low[u] < low[v]) ? low[u] : low[v];
            } else if (inStack[v]) {
                low[u] = (low[u] < disc[v]) ? low[u] : disc[v];
            }
        }
    }

    if (low[u] == disc[u]) {
        // Collect SCC nodes
        int w, sccSize = 0;
        printf("SCC: ");
        char *sccNodes[nodeCount];
        int idx = 0;

        do {
            w = pop();
            sccNodes[idx++] = nodeNames[w];
            printf("%s ", nodeNames[w]);
            sccSize++;
        } while (w != u);
        printf("\n");

        // Check if this SCC qualifies as a real deadlock
        if (sccSize > 1) {  // Ensure SCC has more than one node
            int isDeadlock = 0;
            for (int i = 0; i < idx; i++) {
                for (int j = 0; j < idx; j++) {
                    if (i != j) {
                        int uIndex = nameToIndex(sccNodes[i]);
                        int vIndex = nameToIndex(sccNodes[j]);
                        if (adjMatrix[uIndex][vIndex]) {
                            isDeadlock = 1;
                            break;
                        }
                    }
                }
                if (isDeadlock) break;
            }
            if (isDeadlock) {
                printf("Deadlock SSC Detected: ");
                for (int i = 0; i < idx; i++) {
                    printf("%s ", sccNodes[i]);
                }
                printf("\n");
                hasDeadlock = 1;
            }
        }
    }
}

int main() {
    int numProcesses, numResources, numEdges;
    char process[10], resource[10], source[10], destination[10];

    // Input format
    printf("Enter the number of processes, resources, and edges: ");
    scanf("%d %d %d", &numProcesses, &numResources, &numEdges);

    // Dynamically allocate memory based on the actual number of nodes
    nodeCount = numProcesses + numResources; // Total number of nodes (processes + resources)

    // Dynamically allocate memory for adjacency matrix and other arrays
    adjMatrix = (int**)malloc(nodeCount * sizeof(int*));
    for (int i = 0; i < nodeCount; i++) {
        adjMatrix[i] = (int*)calloc(nodeCount, sizeof(int));  // Initialize to 0
    }

    disc = (int*)malloc(nodeCount * sizeof(int));
    low = (int*)malloc(nodeCount * sizeof(int));
    inStack = (int*)malloc(nodeCount * sizeof(int));
    stack = (int*)malloc(nodeCount * sizeof(int));
    nodeNames = (char**)malloc(nodeCount * sizeof(char*));

    // Initialize arrays
    for (int i = 0; i < nodeCount; i++) {
        disc[i] = -1;
        low[i] = -1;
        inStack[i] = 0;
    }

    // Process and resource inputs
    printf("Enter processes: ");
    for (int i = 0; i < numProcesses; i++) {
        scanf("%s", process);
        nameToIndex(process);
    }

    printf("Enter resources: ");
    for (int i = 0; i < numResources; i++) {
        scanf("%s", resource);
        nameToIndex(resource);
    }

    printf("Enter edges (source destination):\n");
    for (int i = 0; i < numEdges; i++) {
        scanf("%s %s", source, destination);
        int u = nameToIndex(source);
        int v = nameToIndex(destination);
        adjMatrix[u][v] = 1;  // Set edge from source to destination
    }

    // Run Tarjan's algorithm to find SCCs (Deadlocks)
    for (int i = 0; i < nodeCount; i++) {
        if (disc[i] == -1) {
            tarjanDFS(i);
        }
    }

    // If no real deadlocks were detected
    if (!hasDeadlock) {
        printf("No real deadlock detected.\n");
    }

    // Free dynamically allocated memory
    for (int i = 0; i < nodeCount; i++) {
        free(adjMatrix[i]);
    }
    free(adjMatrix);
    free(disc);
    free(low);
    free(inStack);
    free(stack);
    for (int i = 0; i < nodeIndex; i++) {
        free(nodeNames[i]);
    }
    free(nodeNames);

    return 0;
}
