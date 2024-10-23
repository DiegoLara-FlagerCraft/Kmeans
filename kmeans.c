#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_ITER 100  // Máximo número de iteraciones
#define DIMENSIONS 2  // Dimensión de los puntos (2D en este caso)

// Estructura para representar un punto
typedef struct {
    double coords[DIMENSIONS];  // Coordenadas
    int cluster;  // Índice del cluster al que pertenece
} Point;

// Estructura para representar un centroide
typedef struct {
    double coords[DIMENSIONS];  // Coordenadas del centroide
} Centroid;

// Función para calcular la distancia euclidiana entre dos puntos
double distance(Point p, Centroid c) {
    double sum = 0.0;
    for (int i = 0; i < DIMENSIONS; i++) {
        sum += pow(p.coords[i] - c.coords[i], 2);
    }
    return sqrt(sum);
}

// Función para inicializar los centroides aleatoriamente
void initialize_centroids(Point* points, Centroid* centroids, int n_points, int k) {
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < DIMENSIONS; j++) {
            centroids[i].coords[j] = points[rand() % n_points].coords[j];  // Elegir un punto aleatorio
        }
    }
}

// Función para asignar puntos a los clusters más cercanos
void assign_clusters(Point* points, Centroid* centroids, int n_points, int k) {
    for (int i = 0; i < n_points; i++) {
        double min_dist = distance(points[i], centroids[0]);
        int cluster = 0;
        for (int j = 1; j < k; j++) {
            double dist = distance(points[i], centroids[j]);
            if (dist < min_dist) {
                min_dist = dist;
                cluster = j;
            }
        }
        points[i].cluster = cluster;
    }
}

// Función para recalcular los centroides
void update_centroids(Point* points, Centroid* centroids, int n_points, int k) {
    int* counts = (int*)calloc(k, sizeof(int));  // Contador de puntos por cluster
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < DIMENSIONS; j++) {
            centroids[i].coords[j] = 0.0;
        }
    }

    for (int i = 0; i < n_points; i++) {
        int cluster = points[i].cluster;
        counts[cluster]++;
        for (int j = 0; j < DIMENSIONS; j++) {
            centroids[cluster].coords[j] += points[i].coords[j];
        }
    }

    for (int i = 0; i < k; i++) {
        if (counts[i] != 0) {
            for (int j = 0; j < DIMENSIONS; j++) {
                centroids[i].coords[j] /= counts[i];
            }
        }
    }
    free(counts);
}

int main() {
    int n_points = 10;  // Número de puntos
    int k = 2;  // Número de clusters
    Point points[10] = {  // Datos de ejemplo (2D)
        {{1.0, 1.0}}, {{2.0, 1.0}}, {{4.0, 3.0}}, {{5.0, 4.0}}, {{3.0, 2.5}},
        {{8.0, 8.0}}, {{9.0, 8.0}}, {{8.0, 9.0}}, {{10.0, 8.0}}, {{9.0, 9.0}}
    };
    Centroid centroids[k];
    
    // Inicializar los centroides aleatoriamente
    initialize_centroids(points, centroids, n_points, k);

    // Iterar hasta que los centroides converjan o se alcance el máximo de iteraciones
    for (int iter = 0; iter < MAX_ITER; iter++) {
        assign_clusters(points, centroids, n_points, k);  // Asignar puntos a los clusters más cercanos
        update_centroids(points, centroids, n_points, k);  // Actualizar centroides

        // Mostrar los centroides y los clusters
        printf("Iteración %d:\n", iter);
        for (int i = 0; i < k; i++) {
            printf("Centroide %d: (%.2f, %.2f)\n", i, centroids[i].coords[0], centroids[i].coords[1]);
        }
        printf("\n");
    }

    // Mostrar el cluster de cada punto
    for (int i = 0; i < n_points; i++) {
        printf("Punto (%.2f, %.2f) está en el cluster %d\n", points[i].coords[0], points[i].coords[1], points[i].cluster);
    }

    return 0;
}
