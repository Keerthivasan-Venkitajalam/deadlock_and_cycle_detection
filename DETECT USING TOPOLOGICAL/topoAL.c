#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 52  // For lowercase a-z (processes) and uppercase A-Z (resources)

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

// Function to create a new node
Node* createNode(int v) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}

// Function to create an adjacency list for the graph
Node** createAdjList(int size) {
    Node** adjList = (Node**)malloc(size * sizeof(Node));
    for (int i = 0; i < size; i++) {
        adjList[i] = NULL;
    }
    return adjList;
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

// Function to initialize and return the adjacency list for the graph
Node** createGraph(int numProcesses, int numResources, int edges, char* labels) {
    int totalVertices = numProcesses + numResources;
    Node** adjList = createAdjList(totalVertices);

    // Input edges
    for (int i = 0; i < edges; i++) {
        char src, dest;
        printf("Enter edge (src dest): ");
        scanf(" %c %c", &src, &dest);

        int srcIndex = findIndex(labels, totalVertices, src);
        int destIndex = findIndex(labels, totalVertices, dest);

        if (srcIndex != -1 && destIndex != -1) {
            addEdge(adjList, srcIndex, destIndex);
        }
    }

    return adjList;
}

// Kahn's Algorithm for topological sorting and cycle detection
int kahn_topological_sort(Node** adjList, char* labels, int totalVertices) {
    int in_degree[MAX_VERTICES] = {0};
    int queue[MAX_VERTICES];
    int front = 0, rear = 0;
    int topological_order[MAX_VERTICES];
    int top_order_index = 0;

    // Calculate in-degrees for each vertex
    for (int i = 0; i < totalVertices; i++) {
        Node* temp = adjList[i];
        while (temp) {
            in_degree[temp->vertex]++;
            temp = temp->next;
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

        Node* temp = adjList[current];
        while (temp) {
            int adjVertex = temp->vertex;
            in_degree[adjVertex]--;
            if (in_degree[adjVertex] == 0) {
                queue[rear++] = adjVertex;
            }
            temp = temp->next;
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

    // Create the adjacency list
    printf("Enter edges as pairs of characters (src dest):\n");
    Node** graph = createGraph(numProcesses, numResources, edges, labels);

    // Perform Kahn's algorithm on the created adjacency list
    if (kahn_topological_sort(graph, labels, totalVertices) == 0) {
        printf("No deadlock detected.\n");
    } else {
        printf("System is in a deadlock state.\n");
    }

    // Free allocated memory
    for (int i = 0; i < totalVertices; i++) {
        Node* temp = graph[i];
        while (temp) {
            Node* toFree = temp;
            temp = temp->next;
            free(toFree);
        }
    }
    free(graph);

    return 0;
}