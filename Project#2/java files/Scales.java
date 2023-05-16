/*
+----------------------------------------+
|         Scales                         |
+----------------------------------------+
| - inUse: boolean                       |
+----------------------------------------+
| + useScales(workerId: int): void       |
| + releaseScales(workerId: int): void   |
+----------------------------------------+
*/

public class Scales {
    private boolean inUse = false; // Variable to track if scales are currently in use

    public synchronized void useScales(int workerId) { // Synchronized method for a postal worker to scales
        if (inUse) { // If scales are already in use, throw an exception
            throw new IllegalStateException("Scales are already in use");
        }
        
        inUse = true; // Mark scales as in use
        System.out.println("Scales in use by postal worker " + workerId); // Update console
    }

    public synchronized void releaseScales(int workerId) { // Synchronized method for a postal worker to release scales after use
        if (!inUse) { // If scales are not in use, throw an exception
            throw new IllegalStateException("Scales are not in use");
        }
        
        inUse = false; // Mark scales as not in use
        System.out.println("Scales released by postal worker " + workerId); // Update console
    }
} // Scales
