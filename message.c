#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"

#define K 1024            /* One Kilobyte */
#define M K*K             /* One Megabyte */
#define MAXSIZE K*M       /* One Gigabyte */

int main(int argc, char* argv[]) {

  // Initialize MPI
  int np, id;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  /* Check that we run on more than one process */
  if (np < 2) {
    printf("You have to use at least 2 processes to run this program\n");
    MPI_Finalize();            // Quit if there is only one process
    exit(0);
  }

  // Process 0
  if (id == 0) {
    /* Allocate a sufficiently large message buffer */
    char *buffer = (char*) malloc(MAXSIZE*sizeof(char));

    /* Check that allocation succeeded */
    if (buffer == NULL) {
      printf("Could not allocate memory, exiting\n");
      MPI_Finalize();
      exit(0);
    }

    // Initialize the whole message buffer to some values
    for (int i=0; i<MAXSIZE; i++) {
      buffer[i] = (char) 75;
    }

    printf("[0] Memory allocated.\n");

    int inputSize = 0;

    // Repeat until an inputSize <= 0 is entered
    while(1) {
        // Input size
        printf("[0] Please give an input size in bytes:\n");fflush(stdout);
        scanf("%d", &inputSize);
        if (inputSize > MAXSIZE) {
          printf("[0] Input size is too large, maximum value is %d\n", MAXSIZE);
          inputSize = 0;
        }

        printf("Input size: %d\n", inputSize);

        // Send size to other processes
        for (int i = 1; i < np; i++) {
          printf("[0] Sending size to process [%d]...\n", i);
          MPI_Send(&inputSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
          printf("[0] Size sent to process [%d]\n", i);
        }

        if(inputSize <= 0) {
          printf("[0] Process terminated.\n");
          break;   /* Terminate if size = 0 */
        }

        // Start timer
        double start = MPI_Wtime();

        // Send message to process 1
        MPI_Send(buffer, inputSize, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        printf("[0] Message sent to process 1\n");

        // Wait for response to process N-1
        MPI_Recv(buffer, inputSize, MPI_CHAR, np - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // End timer
        printf("[0] Total time: %f\n", MPI_Wtime() - start);
      }

    free(buffer);     // Deallocate the buffer
    MPI_Finalize();   // Terminate MPI
    exit(0);          // Quit
  } else {
    int inputSize = 0;
    // Processes 1 to N-1
    while(1) {
      // Wait for size from process 0
      printf("[%d] Waiting for size.\n", id);
      MPI_Recv(&inputSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("[%d] Size received: %d\n", id, inputSize);

      // Terminate process if size = 0
      if (inputSize == 0) {
        printf("[%d] Process terminated.\n", id);
        break;
      }

      // Allocate buffer with size inputSize
      char *buffer = (char*) malloc(inputSize * sizeof(char));
      if (buffer == NULL) {
        printf("[%d] Cannot allocate buffer.\n", id);
        MPI_Finalize();
        exit(0);
      }

      // Wait for message from process id-1
      printf("[%d] Waiting message from process [%d].\n", id, id - 1);
      MPI_Recv(buffer, inputSize, MPI_CHAR, id - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      // Send message to process id+1
      printf("[%d] Sending message to process [%d]\n", id, (id + 1) % np);
      MPI_Send(buffer, inputSize, MPI_CHAR, (id + 1) % np, 0, MPI_COMM_WORLD);

      free(buffer);     // Deallocate the buffer
    }

    MPI_Finalize();   // Terminate MPI
    exit(0);          // Quit
  }

}