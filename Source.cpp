#include <iostream>
#include <climits>
#include <cstring>
#include <algorithm>

using namespace std;

// Structure to represent an Edge between nodes
struct Edge {
    int src, dest, weight;
};

// Node class for the linked list implementation
class Node {
public:
    string data;
    Node* next;

    Node() {
        data = "";
        next = nullptr;
    }

    Node(const string& value) : data(value), next(nullptr) {}
};

// Relationship class to represent connections between nodes
class Relationship {
public:
    string type;
    int weight;
    Relationship* next;

    Relationship() {
        type = "";
        weight = INT_MAX;
        next = nullptr;
    }

    Relationship(const string& relationshipType, int edgeWeight) : type(relationshipType), weight(edgeWeight), next(nullptr) {}
};

// Event class to track events and their attendance
class Event {
public:
    string eventName;
    int attendance;

    Event() {
        eventName = "";
        attendance = 0;
    }

    Event(const string& name) : eventName(name), attendance(0) {}
};

// Graph class to manage the university graph operations
class Graph {
private:
    Node* nodes; // Array of nodes
    Relationship*** relationships; // 2D array representing relationships between nodes
    int size; // Number of nodes
    Event* events; // Array to store events
    Edge* allEdges; // Array to store all edges for Kruskal's algorithm
    int* parent; // Array to track parent for Union-Find algorithm
    int numEdges = 0; // Counter to track the number of edges

    // Helper function for Union-Find algorithm to perform union operation
    void unionSets(int x, int y) {
        int xRoot = findSet(x);
        int yRoot = findSet(y);
        parent[xRoot] = yRoot;
    }

    // Helper function for Union-Find algorithm to find the set of an element
    int findSet(int i) {
        if (parent[i] == -1)
            return i;
        return findSet(parent[i]);
    }

public:
    Graph(int numNodes) : size(numNodes) {
        nodes = new Node[numNodes];
        relationships = new Relationship * *[numNodes];
        events = new Event[numNodes];
        allEdges = new Edge[numNodes * (numNodes - 1) / 2];

        // Initialize arrays and structures
        for (int i = 0; i < numNodes; ++i) {
            nodes[i].data = "";
            nodes[i].next = nullptr;
            relationships[i] = new Relationship * [numNodes];
            for (int j = 0; j < numNodes; ++j) {
                relationships[i][j] = nullptr;
            }
        }
    }

    ~Graph() {
        delete[] nodes;
        delete[] events;

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                Relationship* current = relationships[i][j];
                while (current != nullptr) {
                    Relationship* temp = current;
                    current = current->next;
                    delete temp;
                }
            }
            delete[] relationships[i];
        }

        delete[] relationships;
        delete[] allEdges;
    }

    void addNode(int index, const string& nodeName) {
        if (index >= 0 && index < size) {
            nodes[index].data = nodeName;
        }
    }

    void addRelationship(int index1, int index2, const string& relationshipType, int edgeWeight) {
        if (index1 >= 0 && index1 < size && index2 >= 0 && index2 < size) {
            Relationship* newRelationship1 = new Relationship(relationshipType, edgeWeight);
            newRelationship1->next = relationships[index1][index2];
            relationships[index1][index2] = newRelationship1;

            Relationship* newRelationship2 = new Relationship(relationshipType, edgeWeight);
            newRelationship2->next = relationships[index2][index1];
            relationships[index2][index1] = newRelationship2;

            // Adding edges to the list for Kruskal's algorithm
            addEdgeToList(index1, index2, edgeWeight);
            addEdgeToList(index2, index1, edgeWeight);
        }
    }

    void addEvent(int index, const string& eventName) {
        if (index >= 0 && index < size) {
            events[index] = Event(eventName);
        }
    }

    void attendEvent(int index) {
        if (index >= 0 && index < size) {
            events[index].attendance++;
        }
    }

    void displayEventAttendance(int index) {
        if (index >= 0 && index < size) {
            cout << "Event Attendance for " << events[index].eventName << ": " << events[index].attendance << " participants.\n";
        }
    }

    void displayRelationships(int index) {
        if (index >= 0 && index < size) {
            cout << "Relationships of " << nodes[index].data << ": ";
            for (int i = 0; i < size; ++i) {
                Relationship* current = relationships[index][i];
                while (current != nullptr) {
                    cout << current->type << "(" << current->weight << ") ";
                    current = current->next;
                }
            }
            cout << endl;
        }
    }

    int minDistance(int dist[], bool sptSet[]) {
        int min = INT_MAX, min_index = -1;

        for (int v = 0; v < size; v++)
            if (!sptSet[v] && dist[v] <= min)
                min = dist[v], min_index = v;

        return min_index;
    }

    int minKey(int key[], bool mstSet[]) {
        int min = INT_MAX, min_index;

        for (int v = 0; v < size; v++)
            if (mstSet[v] == false && key[v] < min)
                min = key[v], min_index = v;

        return min_index;
    }

    void prim() {
        int* parent = new int[size];
        int* key = new int[size];
        bool* mstSet = new bool[size];

        for (int i = 0; i < size; i++)
            key[i] = INT_MAX, mstSet[i] = false;

        key[0] = 0;
        parent[0] = -1;

        for (int count = 0; count < size - 1; count++) {
            int u = minKey(key, mstSet);
            mstSet[u] = true;

            for (int v = 0; v < size; v++) {
                if (relationships[u][v] != nullptr && mstSet[v] == false && relationships[u][v]->weight < key[v]) {
                    parent[v] = u;
                    key[v] = relationships[u][v]->weight;
                }
            }
        }

        cout << "Minimum Spanning Tree (MST) using Prim's Algorithm:\n";
        for (int i = 1; i < size; i++)
            cout << "Edge: " << nodes[parent[i]].data << " - " << nodes[i].data << " (" << key[i] << ")\n";

        delete[] parent;
        delete[] key;
        delete[] mstSet;
    }

    void dijkstra(int src) {
        int* dist = new int[size];
        bool* sptSet = new bool[size];

        for (int i = 0; i < size; i++)
            dist[i] = INT_MAX, sptSet[i] = false;

        dist[src] = 0;

        for (int count = 0; count < size - 1; count++) {
            int u = minDistance(dist, sptSet);

            sptSet[u] = true;

            for (int v = 0; v < size; v++) {
                if (relationships[u][v] != nullptr && !sptSet[v] && dist[u] != INT_MAX &&
                    dist[u] + relationships[u][v]->weight < dist[v]) {
                    dist[v] = dist[u] + relationships[u][v]->weight;
                }
            }
        }

        cout << "Shortest paths from source " << nodes[src].data << ":\n";
        for (int i = 0; i < size; i++) {
            cout << "To " << nodes[i].data << ": ";
            if (dist[i] == INT_MAX)
                cout << "No path exists\n";
            else
                cout << dist[i] << " units\n";
        }

        delete[] dist;
        delete[] sptSet;
    }

    void DFS(int startNode, bool visited[]) {
        visited[startNode] = true;
        cout << nodes[startNode].data << " ";

        for (int i = 0; i < size; ++i) {
            if (relationships[startNode][i] != nullptr && !visited[i]) {
                DFS(i, visited);
            }
        }
    }

    void detectCommunities() {
        bool* visited = new bool[size];
        for (int i = 0; i < size; ++i) {
            visited[i] = false;
        }

        cout << "Communities within the university:\n";
        for (int i = 0; i < size; ++i) {
            if (!visited[i]) {
                cout << "Community: ";
                DFS(i, visited);
                cout << endl;
            }
        }

        delete[] visited;
    }

    void addEdgeToList(int src, int dest, int weight) {
        allEdges[numEdges].src = src;
        allEdges[numEdges].dest = dest;
        allEdges[numEdges].weight = weight;
        ++numEdges;
    }

    void kruskal() {
        Edge* result = new Edge[size - 1];
        int e = 0;
        int i = 0;

        sort(allEdges, allEdges + numEdges, [](const Edge& a, const Edge& b) {
            return a.weight < b.weight;
            });

        parent = new int[size];
        fill(parent, parent + size, -1);

        while (e < size - 1 && i < numEdges) {
            Edge nextEdge = allEdges[i++];

            int x = findSet(nextEdge.src);
            int y = findSet(nextEdge.dest);

            if (x != y) {
                result[e++] = nextEdge;
                unionSets(x, y);
            }
        }

        delete[] parent;

        cout << "Minimum Spanning Tree (MST) using Kruskal's Algorithm:\n";
        for (i = 0; i < e; ++i) {
            cout << "Edge: " << nodes[result[i].src].data << " - " << nodes[result[i].dest].data << " (" << result[i].weight << ")\n";
        }

        delete[] result;
    }
};

int main() {
    const int numNodes = 4;
    Graph universityGraph(numNodes);

    universityGraph.addNode(0, "Mobin");
    universityGraph.addNode(1, "Shahnawaz");
    universityGraph.addNode(2, "Minhaj");
    universityGraph.addNode(3, "Behzad");

    universityGraph.addRelationship(0, 1, "Friendship", 2);
    universityGraph.addRelationship(0, 2, "AcademicCollaboration", 1);
    universityGraph.addRelationship(2, 3, "Mentorship", 3);
    universityGraph.addRelationship(1, 3, "Extracurricular", 4);

    universityGraph.addEvent(0, "SeminarA");
    universityGraph.addEvent(1, "WorkshopB");
    universityGraph.addEvent(2, "ConferenceX");
    universityGraph.addEvent(3, "MeetingY");

    universityGraph.attendEvent(0);
    universityGraph.attendEvent(0);
    universityGraph.attendEvent(1);
    universityGraph.attendEvent(2);

    universityGraph.displayRelationships(2);

    universityGraph.displayEventAttendance(0);
    universityGraph.displayEventAttendance(1);
    universityGraph.displayEventAttendance(2);
    universityGraph.displayEventAttendance(3);

    int choice = 0;
    bool exitFlag = false;

    while (!exitFlag) {

        cout << "\nEnter \n0 for Communication Pattern analysis\n1 for Influence Analysis\n2 for Collaboration Network\n3 for Community Detection using DFS\n4 to exit: ";
        cin >> choice;

        bool* visited = new bool[numNodes];
        for (int i = 0; i < numNodes; ++i) {
            visited[i] = false;
        }

        switch (choice) {
        case 0:
            cout << "Communication Pattern Analysis\n";
            universityGraph.prim();
            break;

        case 1:
            cout << "Influence Analysis\n";
            int sourceVertex;
            cout << "Enter the source vertex for Dijkstra's Algorithm: ";
            cin >> sourceVertex;
            universityGraph.dijkstra(sourceVertex);
            break;

        case 2:
            cout << "Collaboration Network\n";
            universityGraph.kruskal();
            break;

        case 3:
            cout << "Community Detection using DFS\n";
            universityGraph.detectCommunities();
            break;

        case 4:
            exitFlag = true;
            cout << "Exiting the program.\n";
            break;

        default:
            cout << "Invalid choice\n";
        }

        delete[] visited;
    }

    return 0;
}