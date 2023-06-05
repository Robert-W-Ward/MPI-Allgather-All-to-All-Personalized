#include "utilities.h"
#include <mpi.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <set>
#include <math.h>
using std::cerr;
using std::cout;
using std::endl;

/******************************************************************************
 * numprocs:  number of processors                                             *
 * myid:      my processor number (numbered 0 - numprocs-1)                    *
 ******************************************************************************/
int numprocs, myid;

/******************************************************************************
 * evaluate 2^i                                                                *
 ******************************************************************************/
inline unsigned int pow2(unsigned int i) { return 1 << i; }

/******************************************************************************
 * evaluate ceil(log2(i))                                                      *
 ******************************************************************************/
inline unsigned int log2(unsigned int i)
{
  i--;
  unsigned int log = 1;
  for (i >>= 1; i != 0; i >>= 1)
    log++;
  return log;
}


/******************************************************************************
 * This function should implement the All to All Broadcast.  The value for each*
 * processor is given by the argument send_value.  The recv_buffer argument is *5
 * an array of size p that will store the values that each processor transmits.*
 * See Program 4.7 page 162 of the text.                                       *
 ******************************************************************************/
// All-to-all on a d-dimensional hypercube
void AllToAll(int send_value[], int recv_buffer[], int size, MPI_Comm comm)
{
  int d = log2(numprocs);
  int partner;
  std::vector<int> result;
  // result := my_msg
  result.insert(result.end(), send_value, send_value + size);
  MPI_Request req;
  // for i := 0 to d-1 do
  for (int i = 1; i <= d; ++i)
  {
    partner = myid ^ pow2(i - 1);
    MPI_Isend(send_value, size, MPI_INT, partner, 0, comm, &req);
    MPI_Irecv(recv_buffer, (numprocs * size), MPI_INT, partner, 0, comm, &req);
    MPI_Wait(&req, MPI_STATUS_IGNORE);
    if (myid < partner)
      result.insert(result.end(), recv_buffer, recv_buffer + size);
    else
      result.insert(result.begin(), recv_buffer, recv_buffer + size);
    size = size * 2;
    std::copy(result.begin(), result.end(), send_value);
  }
  std::copy(result.begin(), result.end(), recv_buffer);
}

/******************************************************************************
 * This function should implement the All to All Personalized Broadcast.       *
 * A value destined for each processor is given by the argument array          *
 * send_buffer of size p.  The recv_buffer argument is an array of size p      *
 * that will store the values that each processor transmits.                   *
 * See pages 175-179 in the text.       5                                       *
 ******************************************************************************/
void AllToAllPersonalized(int send_buffer[], int recv_buffer[], int size, MPI_Comm comm)
{
  int d = log2(numprocs);
  int partner;
  MPI_Request request;
  for (int i = 1; i <= pow2(d)-1; ++i)
  {
    partner = myid ^ i;
    MPI_Isend((&send_buffer[partner*size]),size, MPI_INT, partner, 0, comm, &request);
    MPI_Irecv((&send_buffer[partner*size]), (numprocs*size), MPI_INT, partner, 0, comm, &request);
    MPI_Wait(&request, MPI_STATUS_IGNORE);

  }
  std::copy(send_buffer, send_buffer + (numprocs*size) , recv_buffer);

}
int main(int argc, char **argv)
{
  chopsigs_();

  double time_passes, max_time;
  /* Initialize MPI */
  MPI_Init(&argc, &argv);

  /* Get the number of processors and my processor identification */
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);

  int test_runs = 800000 / numprocs;
  if (argc == 2)
    test_runs = atoi(argv[1]);
  const int max_size = pow2(16);
  int *recv_buffer = new int[numprocs * max_size];
  int *send_buffer = new int[numprocs * max_size];

  if (0 == myid)
  {
    cout << "Starting " << numprocs << " processors." << endl;
  }

  /***************************************************************************/
  /* Check Timing for Single Node Broadcast emulating an alltoall broadcast  */
  /***************************************************************************/

  MPI_Barrier(MPI_COMM_WORLD);

  /* We can't accurately measure short times so we must execute this operation
     many times to get accurate timing measurements.*/
  for (int l = 0; l <= 16; l += 4)
  {
    int msize = pow2(l);
    /* Every call to get_timer resets the stopwatch.  The next call to
       get_timer will return the amount of time since now */
    get_timer();
    for (int i = 0; i < test_runs; ++i)
    {
      /* Slow alltoall broadcast using p single node broadcasts */
      for (int p = 0; p < numprocs; ++p)
        recv_buffer[p] = 0;
      int send_info = myid + i * numprocs;
      for (int k = 0; k < msize; ++k)
        send_buffer[k] = send_info;
      AllToAll(send_buffer, recv_buffer, msize, MPI_COMM_WORLD);

      for (int p = 0; p < numprocs; ++p)
        if (recv_buffer[p * msize] != (p + i * numprocs))
        {

          cerr << "recv failed on processor " << myid << " recv_buffer["
               << p << "] = "
               << recv_buffer[p * msize] << " should  be " << p + i * numprocs << endl;
        }
    }
    time_passes = get_timer();

    MPI_Reduce(&time_passes, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (0 == myid)
      cout << "all to all broadcast for m=" << msize << " required " << max_time / double(test_runs)
           << " seconds." << endl;
  }
  /***************************************************************************/
  /* Check Timing for All to All personalized Broadcast Algorithm            */
  /***************************************************************************/

  MPI_Barrier(MPI_COMM_WORLD);

  for (int l = 0; l <= 16; l += 4)
  {
    int msize = pow2(l);
    /* Every call to get_timer resets the stopwatch.  The next call to
       get_timer will return the amount of time since now */
    get_timer();

    for (int i = 0; i < test_runs; ++i)
    {
      for (int p = 0; p < numprocs; ++p)
        for (int k = 0; k < msize; ++k)
          recv_buffer[p * msize + k] = 0;
      int factor = (myid & 1 == 1) ? -1 : 1;
      for (int p = 0; p < numprocs; ++p)
        for (int k = 0; k < msize; ++k)
          send_buffer[p * msize + k] = myid * numprocs + p + i * myid * myid * factor;
      int send_info = myid + i * numprocs;

      AllToAllPersonalized(send_buffer, recv_buffer, msize, MPI_COMM_WORLD);
    
      for (int p = 0; p < numprocs; ++p)
      {
        int factor = (p & 1 == 1) ? -1 : 1;
        if (recv_buffer[p * msize] != (p * numprocs + myid + i * p * p * factor))
          cerr << "recv failed on processor " << myid << " recv_buffer["
               << p << "] = "
               << recv_buffer[p * msize] << " should  be " << p * numprocs + myid + i * p * p * factor << endl;
      }
    }

    time_passes = get_timer();

    MPI_Reduce(&time_passes, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (0 == myid)
      cout << "all-to-all-personalized broadcast, m=" << msize
           << " required " << max_time / double(test_runs)
           << " seconds." << endl;
  }

  /* We're finished, so call MPI_Finalize() to clean things up */
  MPI_Finalize();
  return 0;
}
