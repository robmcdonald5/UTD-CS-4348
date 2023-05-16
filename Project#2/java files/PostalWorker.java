/*
+----------------------------------------------------+
|           PostalWorker                             |
+----------------------------------------------------+
| - id: int                                          |
| - postalWorkerSemaphore: Semaphore                 |
| - customerSemaphore: Semaphore                     |
| - scales: Scales                                   |
| - taskAvailableSemaphore: Semaphore                |
| - taskQueue: BlockingQueue<Integer>                |
| - scalesAccessSemaphore: Semaphore                 |
+----------------------------------------------------+
| + PostalWorker(id: int,                            |
|                postalWorkerSemaphore: Semaphore,   |
|                customerSemaphore: Semaphore,       |
|                scales: Scales,                     |
|                taskAvailableSemaphore: Semaphore,  |
|                taskQueue: BlockingQueue<Integer>,  |
|                scalesAccessSemaphore: Semaphore)   |
|                                                    |
| + run(): void                                      |
+----------------------------------------------------+
|                                                    |
|   Implements:                                      |
|   - Runnable                                       |
|                                                    |
|   Uses:                                            |
|   - Semaphore                                      |
|   - Scales                                         |
|   - BlockingQueue                                  |
|                                                    |
+----------------------------------------------------+
*/

import java.util.concurrent.*;

public class PostalWorker implements Runnable {
	/* class constructor variables */
    private int id;
    private Semaphore postalWorkerSemaphore;
    private Semaphore customerSemaphore;
    private Scales scales;
    private Semaphore taskAvailableSemaphore;
    private BlockingQueue<Integer> taskQueue;
    private Semaphore scalesAccessSemaphore;
    /* class constructor variables */

    public PostalWorker(int id, Semaphore postalWorkerSemaphore, Semaphore customerSemaphore, Scales scales, Semaphore taskAvailableSemaphore, BlockingQueue<Integer> taskQueue, Semaphore scalesAccessSemaphore) {
    	/* constructor initialization variables for queue and semaphores */
    	this.id = id;
        this.postalWorkerSemaphore = postalWorkerSemaphore;
        this.customerSemaphore = customerSemaphore;
        this.scales = scales;
        this.taskAvailableSemaphore = taskAvailableSemaphore;
        this.taskQueue = taskQueue;
        this.scalesAccessSemaphore = scalesAccessSemaphore;
        /* constructor initialization variables for queue and semaphores */
    }

    @Override
    public void run() {
        System.out.println("Postal worker " + id + " created");

        while (!Thread.currentThread().isInterrupted()) { // Keep executing tasks until thread is interrupted
            try {
                taskAvailableSemaphore.acquire(); int task = taskQueue.take(); // Acquire task available semaphore to start serving a customer

                System.out.println("Postal worker " + id + " serving customer"); // Update console
                int sleepTime; // Sleep variable for mutation

                switch (task) { // Perform task based on current task
                    case 1:
                        System.out.println("Postal worker " + id + " helps customer buy stamps"); // Update console
                        sleepTime = 60;
                        break;
                    case 2:
                        System.out.println("Postal worker " + id + " helps customer mail a letter"); // Update console
                        sleepTime = 90;
                        break;
                    case 3:
                        System.out.println("Postal worker " + id + " helps customer mail a package"); // Update console
                        scalesAccessSemaphore.acquire(); // Acquire scales access semaphore to scales
                        scales.useScales(id);
                        sleepTime = 120;
                        scales.releaseScales(id); // Release scales and scales access semaphore after using them
                        scalesAccessSemaphore.release();
                        break;
                    default:
                        throw new IllegalStateException("Invalid task number");
                }

                Thread.sleep(sleepTime * 1000 / 60); // Sleep to simulate time it takes to perform task
                postalWorkerSemaphore.release(); // Release postal worker semaphore after completing task

            } catch (InterruptedException e) { // Default thread interrupt
                Thread.currentThread().interrupt();
            }
        }
    } // run
} // PostalWorker
