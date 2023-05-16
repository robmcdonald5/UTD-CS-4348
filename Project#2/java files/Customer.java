/*
+------------------------------------------------+
|              Customer                          |
+------------------------------------------------+
| - id: int                                      |
| - postalWorkerSemaphore: Semaphore             |
| - customerSemaphore: Semaphore                 |
| - taskAvailableSemaphore: Semaphore            |
| - taskQueue: BlockingQueue<Integer>            |
+------------------------------------------------+
| + Customer(id: int,                            |
|            postalWorkerSemaphore: Semaphore,   |
|            customerSemaphore: Semaphore,       |
|            taskAvailableSemaphore: Semaphore,  |
|            taskQueue: BlockingQueue<Integer>)  |
|                                                |
| + run(): void                                  |
| - performTask(task: int): void                 |
+------------------------------------------------+
|                                                |
|   Implements:                                  |
|   - Runnable                                   |
|                                                |
|   Uses:                                        |
|   - Semaphore                                  |
|   - BlockingQueue                              |
|                                                |
+------------------------------------------------+
*/
import java.util.concurrent.*;

public class Customer implements Runnable {
	/* class constructor variables */
    private int id;
    private Semaphore postalWorkerSemaphore;
    private Semaphore customerSemaphore;
    private Semaphore taskAvailableSemaphore;
    private BlockingQueue<Integer> taskQueue;
    /* class constructor variables */

    public Customer(int id, Semaphore postalWorkerSemaphore, Semaphore customerSemaphore, Semaphore taskAvailableSemaphore, BlockingQueue<Integer> taskQueue) {
    	/* constructor initialization variables for queue and semaphores */
        this.id = id;
        this.postalWorkerSemaphore = postalWorkerSemaphore;
        this.customerSemaphore = customerSemaphore;
        this.taskAvailableSemaphore = taskAvailableSemaphore;
        this.taskQueue = taskQueue;
        /* constructor initialization variables for queue and semaphores */
    }

    @Override
    public void run() {
        System.out.println("Customer " + id + " created");

        try {
            customerSemaphore.acquire(); // Acquire customer semaphore to enter post office
            System.out.println("Customer " + id + " enters post office"); // Update console

            /* Generate a random task for customer and add it to task queue */
            int task = (int) (Math.random() * 3) + 1;
            taskQueue.put(task);
            taskAvailableSemaphore.release(); // Signal that a new task is available for postal workers

            postalWorkerSemaphore.acquire(); performTask(task); // Acquire postal worker semaphore to perform task

            postalWorkerSemaphore.release(); customerSemaphore.release(); // Release postal worker semaphore and customer semaphore after completing task
            System.out.println("Customer " + id + " leaves post office"); // Update console

        } catch (InterruptedException e) { // Default thread interrupt
            Thread.currentThread().interrupt();
        }
    }

    private void performTask(int task) throws InterruptedException { // Method to perform a task based on task number
        int sleepTime;

        switch (task) { // Perform task based on current task
            case 1:
                System.out.println("Customer " + id + " asks postal worker to buy stamps");
                sleepTime = 60;
                break;
            case 2:
                System.out.println("Customer " + id + " asks postal worker to mail a letter");
                sleepTime = 90;
                break;
            case 3:
                System.out.println("Customer " + id + " asks postal worker to mail a package");
                sleepTime = 120;
                break;
            default:
                throw new IllegalStateException("Invalid task number");
        }

        Thread.sleep(sleepTime * 1000 / 60); // Sleep to simulate the time it takes to perform the task
    }
} // Customer
