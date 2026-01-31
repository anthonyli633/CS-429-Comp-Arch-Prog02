#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_SIZE 15
#define MAX_N 5000

typedef struct vector {
    int *data;      
    int size;       
    int capacity;   
} vector;
int cmp_int(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}
void sort(vector *list) {
    qsort(list->data, list->size, sizeof(int), cmp_int);
}

vector* create() {
    vector *list = malloc(sizeof(vector));
    list->data = malloc(sizeof(int));
    list->size = 0;
    list->capacity = 1;
    return list;
}
void resize(vector *list, int new_capacity) {
    list->data = realloc(list->data, new_capacity * sizeof(int));
    list->capacity = new_capacity;
}
void add(vector *list, int value) {
    if (list->size == list->capacity) resize(list, list->capacity * 2);
    list->data[list->size++] = value;
}

typedef struct Node {
    char type[10];
    int id;
    vector *inputs;
    vector *outputs;
} Node;

int and(vector *inputs) {
    for (int i = 0; i < inputs->size; i++) {
        if (!inputs->data[i]) return 0;
    }
    return 1;
}
int or(vector *inputs) {
    for (int i = 0; i < inputs->size; i++) {
        if (inputs->data[i]) return 1;
    }
    return 0;
}
int not(int input) {
    return !input;
}
int xor(vector *inputs) {
    int result = 0;
    for (int i = 0; i < inputs->size; i++) {
        result ^= inputs->data[i];
    }
    return result;
}
int eval(Node *node, vector *bitmask) {
    if (bitmask->size == 0) { printf("Input size is 0\n"); return -1; }
    if (strcmp(node->type, "NOT") == 0) return not(bitmask->data[0]);
    if (bitmask->size == 1) return bitmask->data[0];

    if (strcmp(node->type, "AND") == 0) return and(bitmask);
    if (strcmp(node->type, "OR") == 0) return or(bitmask);
    if (strcmp(node->type, "XOR") == 0) return xor(bitmask);

    fprintf(stderr, "Invalid gate type\n");
    return -1;
}

typedef struct Pair {
    int first;
    int second;
} Pair;

Node nodes[MAX_N];
int res[1 << MAX_INPUT_SIZE][MAX_N] = {0};
void solve(vector* input_nodes, int n, int mask) {
    // topo sort
    int inDegree[MAX_N] = {0};
    for (int i = 0; i < n; i++) inDegree[i] = nodes[i].inputs->size;
    
    int bits[MAX_N] = {0};
    int queue[MAX_N];
    int front = 0, back = 0;
    int k = input_nodes->size - 1;
    for (int i = 0; i < input_nodes->size; i++) {
        bits[input_nodes->data[i]] = (mask >> k--) & 1;
        queue[back++] = input_nodes->data[i];
    }

    while (front < back) {
        int node = queue[front++];
        if (nodes[node].outputs->size == 0) {
            res[mask][node] = bits[node];
            continue;
        }

        for (int i = 0; i < nodes[node].outputs->size; i++) {
            int next = nodes[node].outputs->data[i];
            if (--inDegree[next] == 0) {
                queue[back++] = next;
                vector *input_bits = create();
                for (int j = 0; j < nodes[next].inputs->size; j++) {
                    add(input_bits, bits[nodes[next].inputs->data[j]]);
                }
                bits[next] = eval(&nodes[next], input_bits);
                free(input_bits->data);
                free(input_bits);
            }
        }
    }

    for (int i = 0; i < n; i++) {
        res[mask][i] = bits[i];
    }
}

int main(int argc, char** argv) {
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    int open = 0, stanza_index = 0, n = 0;
    int unique_ids[MAX_N] = {0};
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strchr(line, '{')) { stanza_index = 0; open++; continue; }
        if (strchr(line, '}')) { open--; continue; }

        char type[10];
        int id;
        vector *inputs;
        vector *outputs;
        if (stanza_index == 0) {
            if (strstr(line, "Type=") == NULL) { fprintf(stderr, "Invalid Stanza\n"); return 1; }
            if (sscanf(line, " Type=%s", type) != 1) { fprintf(stderr, "Invalid Type\n"); return 1; }
        }
        else if (stanza_index == 1) {
            if (strstr(line, "UniqueID=") == NULL) { fprintf(stderr, "Invalid Stanza\n"); return 1; }
            if (sscanf(line, " UniqueID=%d", &id) != 1) { fprintf(stderr, "Invalid Unique ID\n"); return 1; }
            if (unique_ids[id]) { fprintf(stderr, "Duplicate Unique ID\n"); return 1; }
            unique_ids[id] = 1;
        }
        else if (stanza_index == 2) {
            if (strstr(line, "Input=") == NULL) { fprintf(stderr, "Invalid Stanza\n"); return 1; }
            inputs = create();
            char *token = strtok(line, "=,");
            while ((token = strtok(NULL, "=,")) != NULL) {
                if (strcmp(token, "\n") == 0) break;
                add(inputs, atoi(token));
            }
        }
        else if (stanza_index == 3) {
            if (strstr(line, "Output=") == NULL) { fprintf(stderr, "Invalid Stanza\n"); return 1; }
            outputs = create();
            char *token = strtok(line, "=,");
            while ((token = strtok(NULL, "=,")) != NULL) {
                if (strcmp(token, "\n") == 0) break;
                add(outputs, atoi(token));
            }
            Node* node = &nodes[id];
            strcpy(node->type, type);
            node->id = id;
            node->inputs = inputs;
            node->outputs = outputs;
            if (strcmp(type, "INPUT") == 0) {
                
            } else if (strcmp(type, "OUTPUT") == 0) {

            } else {
                if (strcmp(type, "AND") != 0 && strcmp(type, "OR") != 0 && strcmp(type, "NOT") != 0 && strcmp(type, "XOR") != 0) {
                    fprintf(stderr, "Invalid gate type\n"); return 1;
                }
            } n++;
        }
        else {
            fprintf(stderr, "Too many lines in stanza\n"); return 1;
        }

        stanza_index++; 
    }

    // simulate
    vector *input_nodes = create(), *output_nodes = create();
    for (int i = 0; i < n; i++) {
        if (strcmp(nodes[i].type, "INPUT") == 0) {
            add(input_nodes, nodes[i].id);
        }
        if (strcmp(nodes[i].type, "OUTPUT") == 0) {
            add(output_nodes, nodes[i].id);
        }
    }
    sort(input_nodes); sort(output_nodes);

    // print header
    for (int i = 0; i < input_nodes->size; i++) {
        printf("%d ", input_nodes->data[i]);
    }
    printf("|");
    for (int i = 0; i < output_nodes->size; i++) {
        printf(" %d", output_nodes->data[i]);
    }
    printf("\n");

    for (int mask = 0; mask < (1 << input_nodes->size); mask++) {
        int k = input_nodes->size - 1;
        for (int i = 0; i < input_nodes->size; i++) {
            printf("%d ", (mask >> k--) & 1);
        }
        printf("|");
        solve(input_nodes, n, mask);
        for (int i = 0; i < output_nodes->size; i++) {
            printf(" %d", res[mask][output_nodes->data[i]]);
        }
        printf("\n");
    }
    
    fclose(file);
    return 0;
}