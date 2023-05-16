/*
 * CS 4348 Project#3
 * Code Written By Joshua McDonald
 * Written in Java
 * 
 * DESCRIPTION:
 * 	This program is a simulation of a buddy system performing memory allocations.
 * 	It accomplishes this by taking in an input text file with requests executes
 * 	those requests in blocks and outputs the result visually.
 */

/*
+-----------------------+
|   Main                |
+-----------------------+
| +main(String[]): void |
+-----------------------+
*/

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Scanner;

public class Main {

    public static void main(String[] args) { // Datastructure structure definition
        Scanner scanner = new Scanner(System.in);
        System.out.print("Enter the name of the input file: ");
        String inputFile = scanner.nextLine();
        scanner.close();

        MemoryAllocator allocator = new MemoryAllocator(); // Datastructure structure definition
        try (BufferedReader br = new BufferedReader(new FileReader(inputFile))) {
            String line;
            char currentRequestName = 'A';
            /* Read and process each line in the input file */
            while ((line = br.readLine()) != null) {
                String[] parts = line.split(" ");
                if (parts[0].equals("Request")) {
                    int size = Integer.parseInt(parts[1].substring(0, parts[1].length() - 1));
                    System.out.printf("Requesting %dK memory for %s:\n", size, currentRequestName);
                    allocator.allocateMemory(String.valueOf(currentRequestName), size);
                    currentRequestName++;
                } else if (parts[0].equals("Release")) {
                    System.out.printf("Releasing memory for %s:\n", parts[1]);
                    allocator.releaseMemory(parts[1]);
                } else {
                    throw new IllegalArgumentException("Invalid input format.");
                }
                
                /* Print the current memory state after each operation */
                allocator.printMemory();
                System.out.println();
            }
        } catch (IOException e) { // Default file read error
            System.err.println("Error reading input file.");
            e.printStackTrace();
        } catch (IllegalArgumentException e) { // Default argument error
            System.err.println(e.getMessage());
        }
    } // main
} // Main
