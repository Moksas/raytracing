#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "primitives.h"
#include "raytracing.h"
#include <pthread.h>
#define OUT_FILENAME "out.ppm"

#define ROWS 512
#define COLS 512

static void write_to_ppm(FILE *outfile, uint8_t *pixels,
                         int width, int height)
{
    fprintf(outfile, "P6\n%d %d\n%d\n", width, height, 255);
    fwrite(pixels, 1, height * width * 3, outfile);
}

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}
struct thread_args {
    int begin_width,begin_height;
    int end_width,end_height;
    uint8_t *pixels;
    sphere_node spheres;
    color background ;
};
void thread(void *ptr)
{
    light_node lights = NULL;
    rectangular_node rectangulars = NULL;
    sphere_node spheres = NULL;
    color background = { 0.0, 0.1, 0.1 };
#include "use-models.h"
    struct thread_args *args =(struct thread_args*) ptr;
//args->pixels[0]=2;
//printf("args:%d\n",args->pixels[0]);

    raytracing(args->pixels, background,
               rectangulars, spheres, lights, &view, ROWS, COLS,args->begin_width,args->begin_height,args->end_width,args->end_height);
    delete_rectangular_list(&rectangulars);
    delete_sphere_list(&spheres);
    delete_light_list(&lights);
}
int main()
{
    uint8_t *pixels;

    struct timespec start, end;
    pthread_t id[4];
    int ret;
    struct thread_args *args[4];


    /* allocate by the given resolution */

    pixels = malloc(sizeof(unsigned char) * ROWS * COLS * 3);
    if (!pixels) exit(-1);

    for (int i=0; i<4 ; i++) {
        args[i]=malloc(sizeof (struct thread_args));
        if(i==0) {
            args[i]->begin_height = 0;
            args[i]->begin_width = 0;
            args[i]->end_height = COLS/2;
            args[i]->end_width = ROWS/2;
        } else     if(i==1) {
            args[i]->begin_height = COLS/2;
            args[i]->begin_width = 0;
            args[i]->end_height = COLS;
            args[i]->end_width = ROWS/2;
        } else     if(i==2) {
            args[i]->begin_height = 0;
            args[i]->begin_width = ROWS/2;
            args[i]->end_height = COLS/2;
            args[i]->end_width = ROWS;
        } else     if(i==3) {
            args[i]->begin_height = COLS/2;
            args[i]->begin_width = ROWS/2;
            args[i]->end_height = COLS;
            args[i]->end_width = ROWS;
        }
        args[i]->pixels = pixels;
    }


    printf("# Rendering scene\n");
    /* do the ray tracing with the given geometry */
    clock_gettime(CLOCK_REALTIME, &start);

    ret=pthread_create(&id[0],NULL, (void*)thread,(void*) args[0] );
    ret=pthread_create(&id[0]+1,NULL, (void*)thread,(void*) args[1] );
    ret=pthread_create(&id[2],NULL, (void*)thread,(void*) args[2] );
    ret=pthread_create(&id[3],NULL, (void*)thread,(void*) args[3] );
    if(ret!=0) {
        printf ("Create pthread error!\n");
        exit (1);
    }
    pthread_join(id[0],NULL);
    pthread_join(id[1],NULL);
    pthread_join(id[2],NULL);
    pthread_join(id[3],NULL);




    clock_gettime(CLOCK_REALTIME, &end);
    {
        FILE *outfile = fopen(OUT_FILENAME, "wb");
        write_to_ppm(outfile, pixels, ROWS, COLS);
        fclose(outfile);

    }

    //delete_rectangular_list(&rectangulars);
    //delete_sphere_list(&spheres);
    //delete_light_list(&lights);

    free(pixels);
    printf("Done!\n");
    printf("Execution time of raytracing() : %lf sec\n", diff_in_second(start, end));
    return 0;
}
