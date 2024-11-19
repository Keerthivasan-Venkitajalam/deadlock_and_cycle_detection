#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    int dest;
    struct Node* next;
} Node;

Node** adjList;  // Adjacency list
int* disc;       // Discovery time of nodes
int* low;        // Low-link values
int* inStack;    // Stack membership check
int* stack;      // Stack for Tarjan's algorithm
int stackTop = -1; // Top of stack
int time = 0;    // Global timer for DFS discovery times
char** nodeNames;  // Dynamically allocated array of strings for node names
int nodeIndex = 0; // Counter for node indexes
int hasDeadlock = 0; // Flag to check if a real deadlock exists

// Dynamically allocate memory for name strings
char* allocateName(const char* name) {
    char* newName = (char*)malloc(strlen(name) + 1);
    if (newName == NULL) {
        printf("Memory allocation failed for name: %s\n", name);
        exit(1);
    }
    strcpy(newName, name);
    return newName;
}

// Map a name to a unique index, dynamically adding names
int nameToIndex(const char* name) {
    for (int i = 0; i < nodeIndex; i++) {
        if (strcmp(nodeNames[i], name) == 0)
            return i;
    }
    nodeNames[nodeIndex] = allocateName(name);
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

// Add edge to adjacency list
void addEdge(int u, int v) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed for edge\n");
        exit(1);
    }
    newNode->dest = v;
    newNode->next = adjList[u];
    adjList[u] = newNode;
}

// Tarjan's DFS to find SCCs
void tarjanDFS(int u, int* isDeadlock) {
    disc[u] = low[u] = ++time;
    push(u);

    Node* temp = adjList[u];
    while (temp != NULL) {
        int v = temp->dest;
        if (disc[v] == -1) {
            tarjanDFS(v, isDeadlock);
            low[u] = (low[u] < low[v]) ? low[u] : low[v];
        } else if (inStack[v]) {
            low[u] = (low[u] < disc[v]) ? low[u] : disc[v];
        }
        temp = temp->next;
    }

if (low[u] == disc[u]) {
    int w, sccSize = 0;
    char* sccNodes[nodeIndex];  // Temporary storage for SCC nodes
    int idx = 0;

    printf("SCC: ");
    do {
        w = pop();
        sccNodes[idx++] = nodeNames[w];
        printf("%s ", nodeNames[w]);
        sccSize++;
    } while (w != u);
    printf("\n");

    // Check for deadlock: SCC must have more than one node
    if (sccSize > 1) {
        for (int i = 0; i < idx; i++) {
            for (Node* temp = adjList[nameToIndex(sccNodes[i])]; temp != NULL; temp = temp->next) {
                for (int j = 0; j < idx; j++) {
                    if (strcmp(nodeNames[temp->dest], sccNodes[j]) == 0) {
                        *isDeadlock = 1;  // Update the global flag
                        break;
                    }
                }
                if (*isDeadlock) break;
            }
            if (*isDeadlock) break;
        }
        if (*isDeadlock) {
            printf("Deadlock SCC Detected: ");
            for (int i = 0; i < idx; i++) {
                printf("%s ", sccNodes[i]);
            }
            printf("\n");
        }
    }
}


}

int main() {
    int numProcesses, numResources, numEdges;
    char process[10], resource[10], source[10], destination[10];

    printf("Enter number of processes, resources, and edges: ");
    scanf("%d %d %d", &numProcesses, &numResources, &numEdges);

    int totalNodes = numProcesses + numResources;

    // Dynamically allocate memory for arrays
    adjList = (Node**)malloc(totalNodes * sizeof(Node*));
    disc = (int*)malloc(totalNodes * sizeof(int));
    low = (int*)malloc(totalNodes * sizeof(int));
    inStack = (int*)malloc(totalNodes * sizeof(int));
    stack = (int*)malloc(totalNodes * sizeof(int));
    nodeNames = (char**)malloc(totalNodes * sizeof(char*));

    if (!adjList || !disc || !low || !inStack || !stack || !nodeNames) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    // Initialize adjacency list and other arrays
    for (int i = 0; i < totalNodes; i++) {
        adjList[i] = NULL;
        disc[i] = -1;
        low[i] = -1;
        inStack[i] = 0;
    }

    // Processes and resources input
    printf("Enter process names: ");
    for (int i = 0; i < numProcesses; i++) {
        scanf("%s", process);
        nameToIndex(process);
    }

    printf("Enter resource names: ");
    for (int i = 0; i < numResources; i++) {
        scanf("%s", resource);
        nameToIndex(resource);
    }

    // Edges input
    printf("Enter edges (source destination):\n");
    for (int i = 0; i < numEdges; i++) {
        scanf("%s %s", source, destination);
        int u = nameToIndex(source);
        int v = nameToIndex(destination);
        addEdge(u, v);
    }

    int isDeadlock = 0;
    // Run Tarjan's algorithm to find SCCs
    for (int i = 0; i < nodeIndex; i++) {
        if (disc[i] == -1) {
            tarjanDFS(i, &isDeadlock);
        }
    }

    if (isDeadlock==0) {
        printf("No deadlock detected in the graph\n");
    }

    // Free dynamically allocated memory
    for (int i = 0; i < nodeIndex; i++) {
        Node* temp = adjList[i];
        while (temp) {
            Node* toFree = temp;
            temp = temp->next;
            free(toFree);
        }
        free(nodeNames[i]);
    }
    free(adjList);
    free(disc);
    free(low);
    free(inStack);
    free(stack);
    free(nodeNames);

    return 0;
}
