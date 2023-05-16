/*
 * CS 4348 Project#2
 * Code Written By Joshua McDonald
 * Written in Java
 * 
 * DESCRIPTION:
 * 	This program is a simulation of a post office and its day to day actions.
 * 	Semaphores are employed to simulate different post office workers and the
 * 	customers they are serving. Workers use  a special access semaphore to
 * 	use the scales 'resource' without getting in the way of each other.
 * 
 * Notes:
 * 	The CS1 JVM parameters are somewhat limiting when it comes to memory usage.
 * 	It is possible that running this program in the Linux environment can cause
 * 	it to run out of access memory and loop over specific actions. Within
 * 	eclipse this is not a problem. If the program seems to be looping, stop the
 * 	program and refresh memory before executing it again.
 */

/*
+------------------------------------+
|                Main                |
+------------------------------------+
|                                    |
+------------------------------------+
| + main(args: String[]): void       |
+------------------------------------+
|                                    |
|   Uses:                            |
|   - Semaphore                      |
|   - BlockingQueue                  |
|   - ExecutorService                |
|   - Scales                         |
|   - Customer                       |
|   - PostalWorker                   |
|                                    |
|   Creates:                         |
|   - customerSemaphore              |
|   - postalWorkerSemaphore          |
|   - taskAvailableSemaphore         |
|   - taskQueue                      |
|   - scalesAccessSemaphore          |
|   - customerExecutor               |
|   - postalWorkerExecutor           |
|   - scales                         |
|                                    |
+------------------------------------+
*/

import java.util.concurrent.*;

public class Main {
    public static void main(String[] args) throws InterruptedException {
        int numCustomers = 50; int numPostalWorkers = 3; // Predefined MAX customers and postal workers

        Semaphore customerSemaphore = new Semaphore(10); // Semaphore to limit the number of customers inside post office
        Semaphore postalWorkerSemaphore = new Semaphore(numPostalWorkers); // Semaphore to control availability of postal workers
        Semaphore taskAvailableSemaphore = new Semaphore(0); // Semaphore to signal availability of tasks for postal workers
        BlockingQueue<Integer> taskQueue = new LinkedBlockingQueue<>(); // Concurrent queue to store customer tasks
        Semaphore scalesAccessSemaphore = new Semaphore(1); // Semaphore to control access to scales resource

        /* Executor service calls */
        ExecutorService customerExecutor = Executors.newCachedThreadPool();
        ExecutorService postalWorkerExecutor = Executors.newCachedThreadPool();
        /* Executor service calls */

        Scales scales = new Scales(); // Create scales resource

        for (int i = 0; i < numPostalWorkers; i++) { // Create and submit postal worker threads
            postalWorkerExecutor.submit(new PostalWorker(i, postalWorkerSemaphore, customerSemaphore, scales, taskAvailableSemaphore, taskQueue, scalesAccessSemaphore));
        }

        for (int i = 0; i < numCustomers; i++) { // Create and submit customer threads
            customerExecutor.submit(new Customer(i, postalWorkerSemaphore, customerSemaphore, taskAvailableSemaphore, taskQueue));
            Thread.sleep(1000 / 60); // Sleep to simulate passage of time (1 second in the program = 60 seconds in real life)
        }

        customerExecutor.shutdown(); customerExecutor.awaitTermination(1, TimeUnit.DAYS); // Wait for all customer threads to finish

        postalWorkerExecutor.shutdownNow(); // Terminate all postal worker threads
        
        System.out.println("╔════════════════════════════════════════════════════════════╗");
        System.out.println("║ Finished serving customers! Ending simulation..            ║");
        System.out.println("╚════════════════════════════════════════════════════════════╝");
    } // main
} // Main
