#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>  

int num_clusters, num_points, num_dimensions;
double **points;
double **clusters;
int *labels;
double total_time = 0.0;
int max_iterations = 1000;

double squared_distance(double *point1, double *point2) {
    double sum = 0.0;
    for (int i = 0; i < num_dimensions; i++) {
        sum += pow(point1[i] - point2[i], 2);
    }
    return sum;
}

void assign_clusters() {
    for (int i = 0; i < num_points; i++) {
        double min_dist = squared_distance(points[i], clusters[0]);
        labels[i] = 0;
        for (int j = 1; j < num_clusters; j++) {
            double dist = squared_distance(points[i], clusters[j]);
            if (dist < min_dist) {
                min_dist = dist;
                labels[i] = j;
            }
        }
    }
}

void update_clusters() {
    double **sum = (double **)malloc(num_clusters * sizeof(double *));
    int *count = (int *)calloc(num_clusters, sizeof(int));

    for (int i = 0; i < num_clusters; i++) {
        sum[i] = (double *)calloc(num_dimensions, sizeof(double));
    }

    for (int i = 0; i < num_points; i++) {
        int cluster_id = labels[i];
        for (int j = 0; j < num_dimensions; j++) {
            sum[cluster_id][j] += points[i][j];
        }
        count[cluster_id]++;
    }

    for (int i = 0; i < num_clusters; i++) {
        for (int j = 0; j < num_dimensions; j++) {
            if (count[i] > 0) {
                clusters[i][j] = sum[i][j] / count[i];
            }
        }
        free(sum[i]);
    }

    free(sum);
    free(count);
}

void initialize_clusters_kmeanspp() {
    int first_center = rand() % num_points;
    for (int j = 0; j < num_dimensions; j++) {
        clusters[0][j] = points[first_center][j];
    }

    for (int i = 1; i < num_clusters; i++) {
        double *distances = (double *)malloc(num_points * sizeof(double));
        double total_dist = 0.0;

        for (int j = 0; j < num_points; j++) {
            double min_dist = squared_distance(points[j], clusters[0]);
            for (int k = 1; k < i; k++) {
                double dist = squared_distance(points[j], clusters[k]);
                if (dist < min_dist) {
                    min_dist = dist;
                }
            }
            distances[j] = min_dist;
            total_dist += min_dist;
        }

        double r = (double)rand() / RAND_MAX * total_dist;
        double cumulative_dist = 0.0;
        int next_center = 0;
        for (int j = 0; j < num_points; j++) {
            cumulative_dist += distances[j];
            if (cumulative_dist >= r) {
                next_center = j;
                break;
            }
        }

        for (int j = 0; j < num_dimensions; j++) {
            clusters[i][j] = points[next_center][j];
        }

        free(distances);
    }
}

void generate_random_points() {
    for (int i = 0; i < num_points; i++) {
        for (int j = 0; j < num_dimensions; j++) {
            points[i][j] = 1 + rand() % 100;
        }
    }
}

void save_results() {
    FILE *points_file = fopen("points.dat", "w");
    FILE *clusters_file = fopen("clusters.dat", "w");

    if (points_file == NULL || clusters_file == NULL) {
        printf("Error al abrir los archivos.\n");
        return;
    }

    for (int i = 0; i < num_points; i++) {
        for (int j = 0; j < num_dimensions; j++) {
            fprintf(points_file, "%lf ", points[i][j]);
        }
        fprintf(points_file, "%d\n", labels[i]);
    }

    for (int i = 0; i < num_clusters; i++) {
        for (int j = 0; j < num_dimensions; j++) {
            fprintf(clusters_file, "%lf ", clusters[i][j]);
        }
        fprintf(clusters_file, "\n");
    }

    fclose(points_file);
    fclose(clusters_file);
}

void calculate_memory_usage() {
    size_t points_mem = num_points * num_dimensions * sizeof(double);
    size_t clusters_mem = num_clusters * num_dimensions * sizeof(double);
    size_t labels_mem = num_points * sizeof(int);

    printf("Memoria usada para los puntos: %zu bytes\n", points_mem);
    printf("Memoria usada para los clusters: %zu bytes\n", clusters_mem);
    printf("Memoria usada para las etiquetas: %zu bytes\n", labels_mem);
    printf("Memoria total utilizada: %zu bytes\n", points_mem + clusters_mem + labels_mem);
}

int main() {
    srand(time(NULL));

    printf("Ingrese la cantidad de clusters: ");
    scanf("%d", &num_clusters);
    printf("Ingrese la cantidad de puntos: ");
    scanf("%d", &num_points);
    printf("Ingrese el numero de dimensiones: ");
    scanf("%d", &num_dimensions);

    if (num_clusters <= 0 || num_points <= 0 || num_dimensions <= 0) {
        printf("Error: El número de clusters, puntos y dimensiones debe ser mayor que cero.\n");
        return -1;
    }

    points = (double **)malloc(num_points * sizeof(double *));
    clusters = (double **)malloc(num_clusters * sizeof(double *));
    labels = (int *)malloc(num_points * sizeof(int));

    for (int i = 0; i < num_points; i++) {
        points[i] = (double *)malloc(num_dimensions * sizeof(double));
    }

    for (int i = 0; i < num_clusters; i++) {
        clusters[i] = (double *)malloc(num_dimensions * sizeof(double));
    }

    generate_random_points();
    initialize_clusters_kmeanspp();

    clock_t start_time = clock();
    for (int i = 0; i < max_iterations; i++) {
        assign_clusters();
        update_clusters();
    }
    clock_t end_time = clock();
    total_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    save_results();

    printf("Clusters finales (centroides):\n");
    for (int i = 0; i < num_clusters; i++) {
        printf("Cluster %d: ", i);
        for (int j = 0; j < num_dimensions; j++) {
            printf("%.2f ", clusters[i][j]);
        }
        printf("\n");
    }

    printf("Asignaciones de puntos a clusters:\n");
    for (int i = 0; i < num_points; i++) {
        printf("Punto %d (", i);
        for (int j = 0; j < num_dimensions; j++) {
            printf("%.2f ", points[i][j]);
        }
        printf(") pertenece al cluster %d\n", labels[i]);
    }

    printf("Total tiempo: %.6f seconds\n", total_time);
    calculate_memory_usage();

    for (int i = 0; i < num_points; i++) {
        free(points[i]);
    }
    for (int i = 0; i < num_clusters; i++) {
        free(clusters[i]);
    }
    free(points);
    free(clusters);
    free(labels);

    return 0;
}

//https://github.com/TheAlgorithms/C/blob/master/machine_learning/k_means_clustering.c
